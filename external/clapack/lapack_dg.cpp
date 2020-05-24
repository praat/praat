#include "clapack.h"
#include "f2cP.h"

/* Subroutine */ int dgbbrd_(const char *vect, integer *m, integer *n, integer *ncc,
	integer *kl, integer *ku, double *ab, integer *ldab, double *
	d__, double *e, double *q, integer *ldq, double *pt,
	integer *ldpt, double *c__, integer *ldc, double *work,
	integer *info)
{
	/* Table of constant values */
	static double c_b8 = 0.;
	static double c_b9 = 1.;
	static integer c__1 = 1;

    /* System generated locals */
    integer ab_dim1, ab_offset, c_dim1, c_offset, pt_dim1, pt_offset, q_dim1,
	    q_offset, i__1, i__2, i__3, i__4, i__5, i__6, i__7;

    /* Local variables */
    integer i__, j, l, j1, j2, kb;
    double ra, rb, rc;
    integer kk, ml, mn, nr, mu;
    double rs;
    integer kb1, ml0, mu0, klm, kun, nrt, klu1, inca;
    bool wantb, wantc;
    integer minmn;
    bool wantq;
    bool wantpt;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGBBRD reduces a real general m-by-n band matrix A to upper */
/*  bidiagonal form B by an orthogonal transformation: Q' * A * P = B. */

/*  The routine computes B, and optionally forms Q or P', or computes */
/*  Q'*C for a given matrix C. */

/*  Arguments */
/*  ========= */

/*  VECT    (input) CHARACTER*1 */
/*          Specifies whether or not the matrices Q and P' are to be */
/*          formed. */
/*          = 'N': do not form Q or P'; */
/*          = 'Q': form Q only; */
/*          = 'P': form P' only; */
/*          = 'B': form both. */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  NCC     (input) INTEGER */
/*          The number of columns of the matrix C.  NCC >= 0. */

/*  KL      (input) INTEGER */
/*          The number of subdiagonals of the matrix A. KL >= 0. */

/*  KU      (input) INTEGER */
/*          The number of superdiagonals of the matrix A. KU >= 0. */

/*  AB      (input/output) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          On entry, the m-by-n band matrix A, stored in rows 1 to */
/*          KL+KU+1. The j-th column of A is stored in the j-th column of */
/*          the array AB as follows: */
/*          AB(ku+1+i-j,j) = A(i,j) for max(1,j-ku)<=i<=min(m,j+kl). */
/*          On exit, A is overwritten by values generated during the */
/*          reduction. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array A. LDAB >= KL+KU+1. */

/*  D       (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The diagonal elements of the bidiagonal matrix B. */

/*  E       (output) DOUBLE PRECISION array, dimension (min(M,N)-1) */
/*          The superdiagonal elements of the bidiagonal matrix B. */

/*  Q       (output) DOUBLE PRECISION array, dimension (LDQ,M) */
/*          If VECT = 'Q' or 'B', the m-by-m orthogonal matrix Q. */
/*          If VECT = 'N' or 'P', the array Q is not referenced. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q. */
/*          LDQ >= max(1,M) if VECT = 'Q' or 'B'; LDQ >= 1 otherwise. */

/*  PT      (output) DOUBLE PRECISION array, dimension (LDPT,N) */
/*          If VECT = 'P' or 'B', the n-by-n orthogonal matrix P'. */
/*          If VECT = 'N' or 'Q', the array PT is not referenced. */

/*  LDPT    (input) INTEGER */
/*          The leading dimension of the array PT. */
/*          LDPT >= max(1,N) if VECT = 'P' or 'B'; LDPT >= 1 otherwise. */

/*  C       (input/output) DOUBLE PRECISION array, dimension (LDC,NCC) */
/*          On entry, an m-by-ncc matrix C. */
/*          On exit, C is overwritten by Q'*C. */
/*          C is not referenced if NCC = 0. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C. */
/*          LDC >= max(1,M) if NCC > 0; LDC >= 1 if NCC = 0. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (2*max(M,N)) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

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

/*     Test the input parameters */

    /* Parameter adjustments */
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    --d__;
    --e;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    pt_dim1 = *ldpt;
    pt_offset = 1 + pt_dim1;
    pt -= pt_offset;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    wantb = lsame_(vect, "B");
    wantq = lsame_(vect, "Q") || wantb;
    wantpt = lsame_(vect, "P") || wantb;
    wantc = *ncc > 0;
    klu1 = *kl + *ku + 1;
    *info = 0;
    if (! wantq && ! wantpt && ! lsame_(vect, "N")) {
	*info = -1;
    } else if (*m < 0) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ncc < 0) {
	*info = -4;
    } else if (*kl < 0) {
	*info = -5;
    } else if (*ku < 0) {
	*info = -6;
    } else if (*ldab < klu1) {
	*info = -8;
    } else if (*ldq < 1 || wantq && *ldq < std::max(1_integer,*m)) {
	*info = -12;
    } else if (*ldpt < 1 || wantpt && *ldpt < std::max(1_integer,*n)) {
	*info = -14;
    } else if (*ldc < 1 || wantc && *ldc < std::max(1_integer,*m)) {
	*info = -16;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGBBRD", &i__1);
	return 0;
    }

/*     Initialize Q and P' to the unit matrix, if needed */

    if (wantq) {
	dlaset_("Full", m, m, &c_b8, &c_b9, &q[q_offset], ldq);
    }
    if (wantpt) {
	dlaset_("Full", n, n, &c_b8, &c_b9, &pt[pt_offset], ldpt);
    }

/*     Quick return if possible. */

    if (*m == 0 || *n == 0) {
	return 0;
    }

    minmn = std::min(*m,*n);

    if (*kl + *ku > 1) {

/*        Reduce to upper bidiagonal form if KU > 0; if KU = 0, reduce */
/*        first to lower bidiagonal form and then transform to upper */
/*        bidiagonal */

	if (*ku > 0) {
	    ml0 = 1;
	    mu0 = 2;
	} else {
	    ml0 = 2;
	    mu0 = 1;
	}

/*        Wherever possible, plane rotations are generated and applied in */
/*        vector operations of length NR over the index set J1:J2:KLU1. */

/*        The sines of the plane rotations are stored in WORK(1:max(m,n)) */
/*        and the cosines in WORK(max(m,n)+1:2*max(m,n)). */

	mn = std::max(*m,*n);
/* Computing MIN */
	i__1 = *m - 1;
	klm = std::min(i__1,*kl);
/* Computing MIN */
	i__1 = *n - 1;
	kun = std::min(i__1,*ku);
	kb = klm + kun;
	kb1 = kb + 1;
	inca = kb1 * *ldab;
	nr = 0;
	j1 = klm + 2;
	j2 = 1 - kun;

	i__1 = minmn;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Reduce i-th column and i-th row of matrix to bidiagonal form */

	    ml = klm + 1;
	    mu = kun + 1;
	    i__2 = kb;
	    for (kk = 1; kk <= i__2; ++kk) {
		j1 += kb;
		j2 += kb;

/*              generate plane rotations to annihilate nonzero elements */
/*              which have been created below the band */

		if (nr > 0) {
		    dlargv_(&nr, &ab[klu1 + (j1 - klm - 1) * ab_dim1], &inca,
			    &work[j1], &kb1, &work[mn + j1], &kb1);
		}

/*              apply plane rotations from the left */

		i__3 = kb;
		for (l = 1; l <= i__3; ++l) {
		    if (j2 - klm + l - 1 > *n) {
			nrt = nr - 1;
		    } else {
			nrt = nr;
		    }
		    if (nrt > 0) {
			dlartv_(&nrt, &ab[klu1 - l + (j1 - klm + l - 1) *
				ab_dim1], &inca, &ab[klu1 - l + 1 + (j1 - klm
				+ l - 1) * ab_dim1], &inca, &work[mn + j1], &
				work[j1], &kb1);
		    }
/* L10: */
		}

		if (ml > ml0) {
		    if (ml <= *m - i__ + 1) {

/*                    generate plane rotation to annihilate a(i+ml-1,i) */
/*                    within the band, and apply rotation from the left */

			dlartg_(&ab[*ku + ml - 1 + i__ * ab_dim1], &ab[*ku +
				ml + i__ * ab_dim1], &work[mn + i__ + ml - 1],
				 &work[i__ + ml - 1], &ra);
			ab[*ku + ml - 1 + i__ * ab_dim1] = ra;
			if (i__ < *n) {
/* Computing MIN */
			    i__4 = *ku + ml - 2, i__5 = *n - i__;
			    i__3 = std::min(i__4,i__5);
			    i__6 = *ldab - 1;
			    i__7 = *ldab - 1;
			    drot_(&i__3, &ab[*ku + ml - 2 + (i__ + 1) *
				    ab_dim1], &i__6, &ab[*ku + ml - 1 + (i__
				    + 1) * ab_dim1], &i__7, &work[mn + i__ +
				    ml - 1], &work[i__ + ml - 1]);
			}
		    }
		    ++nr;
		    j1 -= kb1;
		}

		if (wantq) {

/*                 accumulate product of plane rotations in Q */

		    i__3 = j2;
		    i__4 = kb1;
		    for (j = j1; i__4 < 0 ? j >= i__3 : j <= i__3; j += i__4)
			    {
			drot_(m, &q[(j - 1) * q_dim1 + 1], &c__1, &q[j *
				q_dim1 + 1], &c__1, &work[mn + j], &work[j]);
/* L20: */
		    }
		}

		if (wantc) {

/*                 apply plane rotations to C */

		    i__4 = j2;
		    i__3 = kb1;
		    for (j = j1; i__3 < 0 ? j >= i__4 : j <= i__4; j += i__3)
			    {
			drot_(ncc, &c__[j - 1 + c_dim1], ldc, &c__[j + c_dim1]
, ldc, &work[mn + j], &work[j]);
/* L30: */
		    }
		}

		if (j2 + kun > *n) {

/*                 adjust J2 to keep within the bounds of the matrix */

		    --nr;
		    j2 -= kb1;
		}

		i__3 = j2;
		i__4 = kb1;
		for (j = j1; i__4 < 0 ? j >= i__3 : j <= i__3; j += i__4) {

/*                 create nonzero element a(j-1,j+ku) above the band */
/*                 and store it in WORK(n+1:2*n) */

		    work[j + kun] = work[j] * ab[(j + kun) * ab_dim1 + 1];
		    ab[(j + kun) * ab_dim1 + 1] = work[mn + j] * ab[(j + kun)
			    * ab_dim1 + 1];
/* L40: */
		}

/*              generate plane rotations to annihilate nonzero elements */
/*              which have been generated above the band */

		if (nr > 0) {
		    dlargv_(&nr, &ab[(j1 + kun - 1) * ab_dim1 + 1], &inca, &
			    work[j1 + kun], &kb1, &work[mn + j1 + kun], &kb1);
		}

/*              apply plane rotations from the right */

		i__4 = kb;
		for (l = 1; l <= i__4; ++l) {
		    if (j2 + l - 1 > *m) {
			nrt = nr - 1;
		    } else {
			nrt = nr;
		    }
		    if (nrt > 0) {
			dlartv_(&nrt, &ab[l + 1 + (j1 + kun - 1) * ab_dim1], &
				inca, &ab[l + (j1 + kun) * ab_dim1], &inca, &
				work[mn + j1 + kun], &work[j1 + kun], &kb1);
		    }
/* L50: */
		}

		if (ml == ml0 && mu > mu0) {
		    if (mu <= *n - i__ + 1) {

/*                    generate plane rotation to annihilate a(i,i+mu-1) */
/*                    within the band, and apply rotation from the right */

			dlartg_(&ab[*ku - mu + 3 + (i__ + mu - 2) * ab_dim1],
				&ab[*ku - mu + 2 + (i__ + mu - 1) * ab_dim1],
				&work[mn + i__ + mu - 1], &work[i__ + mu - 1],
				 &ra);
			ab[*ku - mu + 3 + (i__ + mu - 2) * ab_dim1] = ra;
/* Computing MIN */
			i__3 = *kl + mu - 2, i__5 = *m - i__;
			i__4 = std::min(i__3,i__5);
			drot_(&i__4, &ab[*ku - mu + 4 + (i__ + mu - 2) *
				ab_dim1], &c__1, &ab[*ku - mu + 3 + (i__ + mu
				- 1) * ab_dim1], &c__1, &work[mn + i__ + mu -
				1], &work[i__ + mu - 1]);
		    }
		    ++nr;
		    j1 -= kb1;
		}

		if (wantpt) {

/*                 accumulate product of plane rotations in P' */

		    i__4 = j2;
		    i__3 = kb1;
		    for (j = j1; i__3 < 0 ? j >= i__4 : j <= i__4; j += i__3)
			    {
			drot_(n, &pt[j + kun - 1 + pt_dim1], ldpt, &pt[j +
				kun + pt_dim1], ldpt, &work[mn + j + kun], &
				work[j + kun]);
/* L60: */
		    }
		}

		if (j2 + kb > *m) {

/*                 adjust J2 to keep within the bounds of the matrix */

		    --nr;
		    j2 -= kb1;
		}

		i__3 = j2;
		i__4 = kb1;
		for (j = j1; i__4 < 0 ? j >= i__3 : j <= i__3; j += i__4) {

/*                 create nonzero element a(j+kl+ku,j+ku-1) below the */
/*                 band and store it in WORK(1:n) */

		    work[j + kb] = work[j + kun] * ab[klu1 + (j + kun) *
			    ab_dim1];
		    ab[klu1 + (j + kun) * ab_dim1] = work[mn + j + kun] * ab[
			    klu1 + (j + kun) * ab_dim1];
/* L70: */
		}

		if (ml > ml0) {
		    --ml;
		} else {
		    --mu;
		}
/* L80: */
	    }
/* L90: */
	}
    }

    if (*ku == 0 && *kl > 0) {

/*        A has been reduced to lower bidiagonal form */

/*        Transform lower bidiagonal form to upper bidiagonal by applying */
/*        plane rotations from the left, storing diagonal elements in D */
/*        and off-diagonal elements in E */

/* Computing MIN */
	i__2 = *m - 1;
	i__1 = std::min(i__2,*n);
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dlartg_(&ab[i__ * ab_dim1 + 1], &ab[i__ * ab_dim1 + 2], &rc, &rs,
		    &ra);
	    d__[i__] = ra;
	    if (i__ < *n) {
		e[i__] = rs * ab[(i__ + 1) * ab_dim1 + 1];
		ab[(i__ + 1) * ab_dim1 + 1] = rc * ab[(i__ + 1) * ab_dim1 + 1]
			;
	    }
	    if (wantq) {
		drot_(m, &q[i__ * q_dim1 + 1], &c__1, &q[(i__ + 1) * q_dim1 +
			1], &c__1, &rc, &rs);
	    }
	    if (wantc) {
		drot_(ncc, &c__[i__ + c_dim1], ldc, &c__[i__ + 1 + c_dim1],
			ldc, &rc, &rs);
	    }
/* L100: */
	}
	if (*m <= *n) {
	    d__[*m] = ab[*m * ab_dim1 + 1];
	}
    } else if (*ku > 0) {

/*        A has been reduced to upper bidiagonal form */

	if (*m < *n) {

/*           Annihilate a(m,m+1) by applying plane rotations from the */
/*           right, storing diagonal elements in D and off-diagonal */
/*           elements in E */

	    rb = ab[*ku + (*m + 1) * ab_dim1];
	    for (i__ = *m; i__ >= 1; --i__) {
		dlartg_(&ab[*ku + 1 + i__ * ab_dim1], &rb, &rc, &rs, &ra);
		d__[i__] = ra;
		if (i__ > 1) {
		    rb = -rs * ab[*ku + i__ * ab_dim1];
		    e[i__ - 1] = rc * ab[*ku + i__ * ab_dim1];
		}
		if (wantpt) {
		    drot_(n, &pt[i__ + pt_dim1], ldpt, &pt[*m + 1 + pt_dim1],
			    ldpt, &rc, &rs);
		}
/* L110: */
	    }
	} else {

/*           Copy off-diagonal elements to E and diagonal elements to D */

	    i__1 = minmn - 1;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		e[i__] = ab[*ku + (i__ + 1) * ab_dim1];
/* L120: */
	    }
	    i__1 = minmn;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		d__[i__] = ab[*ku + 1 + i__ * ab_dim1];
/* L130: */
	    }
	}
    } else {

/*        A is diagonal. Set elements of E to zero and copy diagonal */
/*        elements to D. */

	i__1 = minmn - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    e[i__] = 0.;
/* L140: */
	}
	i__1 = minmn;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    d__[i__] = ab[i__ * ab_dim1 + 1];
/* L150: */
	}
    }
    return 0;

/*     End of DGBBRD */

} /* dgbbrd_ */

/* Subroutine */ int dgbcon_(const char *norm, integer *n, integer *kl, integer *ku,
	double *ab, integer *ldab, integer *ipiv, double *anorm,
	double *rcond, double *work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer ab_dim1, ab_offset, i__1, i__2, i__3;
    double d__1;

    /* Local variables */
    integer j;
    double t;
    integer kd, lm, jp, ix, kase;
    integer kase1;
    double scale;
    integer isave[3];
    bool lnoti;
    double ainvnm;
    bool onenrm;
    char normin[1];
    double smlnum;

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

/*  DGBCON estimates the reciprocal of the condition number of a real */
/*  general band matrix A, in either the 1-norm or the infinity-norm, */
/*  using the LU factorization computed by DGBTRF. */

/*  An estimate is obtained for norm(inv(A)), and the reciprocal of the */
/*  condition number is computed as */
/*     RCOND = 1 / ( norm(A) * norm(inv(A)) ). */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies whether the 1-norm condition number or the */
/*          infinity-norm condition number is required: */
/*          = '1' or 'O':  1-norm; */
/*          = 'I':         Infinity-norm. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  KL      (input) INTEGER */
/*          The number of subdiagonals within the band of A.  KL >= 0. */

/*  KU      (input) INTEGER */
/*          The number of superdiagonals within the band of A.  KU >= 0. */

/*  AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          Details of the LU factorization of the band matrix A, as */
/*          computed by DGBTRF.  U is stored as an upper triangular band */
/*          matrix with KL+KU superdiagonals in rows 1 to KL+KU+1, and */
/*          the multipliers used during the factorization are stored in */
/*          rows KL+KU+2 to 2*KL+KU+1. */
/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= 2*KL+KU+1. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices; for 1 <= i <= N, row i of the matrix was */
/*          interchanged with row IPIV(i). */

/*  ANORM   (input) DOUBLE PRECISION */
/*          If NORM = '1' or 'O', the 1-norm of the original matrix A. */
/*          If NORM = 'I', the infinity-norm of the original matrix A. */

/*  RCOND   (output) DOUBLE PRECISION */
/*          The reciprocal of the condition number of the matrix A, */
/*          computed as RCOND = 1/(norm(A) * norm(inv(A))). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (3*N) */

/*  IWORK   (workspace) INTEGER array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */

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
    --ipiv;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    onenrm = *(unsigned char *)norm == '1' || lsame_(norm, "O");
    if (! onenrm && ! lsame_(norm, "I")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*kl < 0) {
	*info = -3;
    } else if (*ku < 0) {
	*info = -4;
    } else if (*ldab < (*kl << 1) + *ku + 1) {
	*info = -6;
    } else if (*anorm < 0.) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGBCON", &i__1);
	return 0;
    }

/*     Quick return if possible */

    *rcond = 0.;
    if (*n == 0) {
	*rcond = 1.;
	return 0;
    } else if (*anorm == 0.) {
	return 0;
    }

    smlnum = dlamch_("Safe minimum");

/*     Estimate the norm of inv(A). */

    ainvnm = 0.;
    *(unsigned char *)normin = 'N';
    if (onenrm) {
	kase1 = 1;
    } else {
	kase1 = 2;
    }
    kd = *kl + *ku + 1;
    lnoti = *kl > 0;
    kase = 0;
L10:
    dlacn2_(n, &work[*n + 1], &work[1], &iwork[1], &ainvnm, &kase, isave);
    if (kase != 0) {
	if (kase == kase1) {

/*           Multiply by inv(L). */

	    if (lnoti) {
		i__1 = *n - 1;
		for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		    i__2 = *kl, i__3 = *n - j;
		    lm = std::min(i__2,i__3);
		    jp = ipiv[j];
		    t = work[jp];
		    if (jp != j) {
			work[jp] = work[j];
			work[j] = t;
		    }
		    d__1 = -t;
		    daxpy_(&lm, &d__1, &ab[kd + 1 + j * ab_dim1], &c__1, &
			    work[j + 1], &c__1);
/* L20: */
		}
	    }

/*           Multiply by inv(U). */

	    i__1 = *kl + *ku;
	    dlatbs_("Upper", "No transpose", "Non-unit", normin, n, &i__1, &
		    ab[ab_offset], ldab, &work[1], &scale, &work[(*n << 1) +
		    1], info);
	} else {

/*           Multiply by inv(U'). */

	    i__1 = *kl + *ku;
	    dlatbs_("Upper", "Transpose", "Non-unit", normin, n, &i__1, &ab[
		    ab_offset], ldab, &work[1], &scale, &work[(*n << 1) + 1],
		    info);

/*           Multiply by inv(L'). */

	    if (lnoti) {
		for (j = *n - 1; j >= 1; --j) {
/* Computing MIN */
		    i__1 = *kl, i__2 = *n - j;
		    lm = std::min(i__1,i__2);
		    work[j] -= ddot_(&lm, &ab[kd + 1 + j * ab_dim1], &c__1, &
			    work[j + 1], &c__1);
		    jp = ipiv[j];
		    if (jp != j) {
			t = work[jp];
			work[jp] = work[j];
			work[j] = t;
		    }
/* L30: */
		}
	    }
	}

/*        Divide X by 1/SCALE if doing so will not cause overflow. */

	*(unsigned char *)normin = 'Y';
	if (scale != 1.) {
	    ix = idamax_(n, &work[1], &c__1);
	    if (scale < (d__1 = work[ix], abs(d__1)) * smlnum || scale == 0.)
		    {
		goto L40;
	    }
	    drscl_(n, &scale, &work[1], &c__1);
	}
	goto L10;
    }

/*     Compute the estimate of the reciprocal condition number. */

    if (ainvnm != 0.) {
	*rcond = 1. / ainvnm / *anorm;
    }

L40:
    return 0;

/*     End of DGBCON */

} /* dgbcon_ */

/* Subroutine */ int dgbequ_(integer *m, integer *n, integer *kl, integer *ku,
	double *ab, integer *ldab, double *r__, double *c__,
	double *rowcnd, double *colcnd, double *amax, integer *
	info)
{
    /* System generated locals */
    integer ab_dim1, ab_offset, i__1, i__2, i__3, i__4;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, kd;
    double rcmin, rcmax;
    double bignum, smlnum;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGBEQU computes row and column scalings intended to equilibrate an */
/*  M-by-N band matrix A and reduce its condition number.  R returns the */
/*  row scale factors and C the column scale factors, chosen to try to */
/*  make the largest element in each row and column of the matrix B with */
/*  elements B(i,j)=R(i)*A(i,j)*C(j) have absolute value 1. */

/*  R(i) and C(j) are restricted to be between SMLNUM = smallest safe */
/*  number and BIGNUM = largest safe number.  Use of these scaling */
/*  factors is not guaranteed to reduce the condition number of A but */
/*  works well in practice. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  KL      (input) INTEGER */
/*          The number of subdiagonals within the band of A.  KL >= 0. */

/*  KU      (input) INTEGER */
/*          The number of superdiagonals within the band of A.  KU >= 0. */

/*  AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          The band matrix A, stored in rows 1 to KL+KU+1.  The j-th */
/*          column of A is stored in the j-th column of the array AB as */
/*          follows: */
/*          AB(ku+1+i-j,j) = A(i,j) for max(1,j-ku)<=i<=min(m,j+kl). */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KL+KU+1. */

/*  R       (output) DOUBLE PRECISION array, dimension (M) */
/*          If INFO = 0, or INFO > M, R contains the row scale factors */
/*          for A. */

/*  C       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, C contains the column scale factors for A. */

/*  ROWCND  (output) DOUBLE PRECISION */
/*          If INFO = 0 or INFO > M, ROWCND contains the ratio of the */
/*          smallest R(i) to the largest R(i).  If ROWCND >= 0.1 and */
/*          AMAX is neither too large nor too small, it is not worth */
/*          scaling by R. */

/*  COLCND  (output) DOUBLE PRECISION */
/*          If INFO = 0, COLCND contains the ratio of the smallest */
/*          C(i) to the largest C(i).  If COLCND >= 0.1, it is not */
/*          worth scaling by C. */

/*  AMAX    (output) DOUBLE PRECISION */
/*          Absolute value of largest matrix element.  If AMAX is very */
/*          close to overflow or very close to underflow, the matrix */
/*          should be scaled. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, and i is */
/*                <= M:  the i-th row of A is exactly zero */
/*                >  M:  the (i-M)-th column of A is exactly zero */

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
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    --r__;
    --c__;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*kl < 0) {
	*info = -3;
    } else if (*ku < 0) {
	*info = -4;
    } else if (*ldab < *kl + *ku + 1) {
	*info = -6;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGBEQU", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	*rowcnd = 1.;
	*colcnd = 1.;
	*amax = 0.;
	return 0;
    }

/*     Get machine constants. */

    smlnum = dlamch_("S");
    bignum = 1. / smlnum;

/*     Compute row scale factors. */

    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	r__[i__] = 0.;
/* L10: */
    }

/*     Find the maximum element in each row. */

    kd = *ku + 1;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	i__2 = j - *ku;
/* Computing MIN */
	i__4 = j + *kl;
	i__3 = std::min(i__4,*m);
	for (i__ = std::max(i__2,1_integer); i__ <= i__3; ++i__) {
/* Computing MAX */
	    d__2 = r__[i__], d__3 = (d__1 = ab[kd + i__ - j + j * ab_dim1],
		    abs(d__1));
	    r__[i__] = std::max(d__2,d__3);
/* L20: */
	}
/* L30: */
    }

/*     Find the maximum and minimum scale factors. */

    rcmin = bignum;
    rcmax = 0.;
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	d__1 = rcmax, d__2 = r__[i__];
	rcmax = std::max(d__1,d__2);
/* Computing MIN */
	d__1 = rcmin, d__2 = r__[i__];
	rcmin = std::min(d__1,d__2);
/* L40: */
    }
    *amax = rcmax;

    if (rcmin == 0.) {

/*        Find the first zero scale factor and return an error code. */

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (r__[i__] == 0.) {
		*info = i__;
		return 0;
	    }
/* L50: */
	}
    } else {

/*        Invert the scale factors. */

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MIN */
/* Computing MAX */
	    d__2 = r__[i__];
	    d__1 = std::max(d__2,smlnum);
	    r__[i__] = 1. / std::min(d__1,bignum);
/* L60: */
	}

/*        Compute ROWCND = min(R(I)) / max(R(I)) */

	*rowcnd = std::max(rcmin,smlnum) / std::min(rcmax,bignum);
    }

/*     Compute column scale factors */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	c__[j] = 0.;
/* L70: */
    }

/*     Find the maximum element in each column, */
/*     assuming the row scaling computed above. */

    kd = *ku + 1;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	i__3 = j - *ku;
/* Computing MIN */
	i__4 = j + *kl;
	i__2 = std::min(i__4,*m);
	for (i__ = std::max(i__3,1_integer); i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = c__[j], d__3 = (d__1 = ab[kd + i__ - j + j * ab_dim1], abs(
		    d__1)) * r__[i__];
	    c__[j] = std::max(d__2,d__3);
/* L80: */
	}
/* L90: */
    }

/*     Find the maximum and minimum scale factors. */

    rcmin = bignum;
    rcmax = 0.;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	d__1 = rcmin, d__2 = c__[j];
	rcmin = std::min(d__1,d__2);
/* Computing MAX */
	d__1 = rcmax, d__2 = c__[j];
	rcmax = std::max(d__1,d__2);
/* L100: */
    }

    if (rcmin == 0.) {

/*        Find the first zero scale factor and return an error code. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    if (c__[j] == 0.) {
		*info = *m + j;
		return 0;
	    }
/* L110: */
	}
    } else {

/*        Invert the scale factors. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
/* Computing MAX */
	    d__2 = c__[j];
	    d__1 = std::max(d__2,smlnum);
	    c__[j] = 1. / std::min(d__1,bignum);
/* L120: */
	}

/*        Compute COLCND = min(C(J)) / max(C(J)) */

	*colcnd = std::max(rcmin,smlnum) / std::min(rcmax,bignum);
    }

    return 0;

/*     End of DGBEQU */

} /* dgbequ_ */

/* Subroutine */ int dgbequb_(integer *m, integer *n, integer *kl, integer *
	ku, double *ab, integer *ldab, double *r__, double *c__,
	double *rowcnd, double *colcnd, double *amax, integer *
	info)
{
    /* System generated locals */
    integer ab_dim1, ab_offset, i__1, i__2, i__3, i__4;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, kd;
    double radix, rcmin, rcmax;
	double bignum, logrdx, smlnum;


/*     -- LAPACK routine (version 3.2)                                 -- */
/*     -- Contributed by James Demmel, Deaglan Halligan, Yozo Hida and -- */
/*     -- Jason Riedy of Univ. of California Berkeley.                 -- */
/*     -- November 2008                                                -- */

/*     -- LAPACK is a software package provided by Univ. of Tennessee, -- */
/*     -- Univ. of California Berkeley and NAG Ltd.                    -- */

/*     .. */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGBEQUB computes row and column scalings intended to equilibrate an */
/*  M-by-N matrix A and reduce its condition number.  R returns the row */
/*  scale factors and C the column scale factors, chosen to try to make */
/*  the largest element in each row and column of the matrix B with */
/*  elements B(i,j)=R(i)*A(i,j)*C(j) have an absolute value of at most */
/*  the radix. */

/*  R(i) and C(j) are restricted to be a power of the radix between */
/*  SMLNUM = smallest safe number and BIGNUM = largest safe number.  Use */
/*  of these scaling factors is not guaranteed to reduce the condition */
/*  number of A but works well in practice. */

/*  This routine differs from DGEEQU by restricting the scaling factors */
/*  to a power of the radix.  Baring over- and underflow, scaling by */
/*  these factors introduces no additional rounding errors.  However, the */
/*  scaled entries' magnitured are no longer approximately 1 but lie */
/*  between sqrt(radix) and 1/sqrt(radix). */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  KL      (input) INTEGER */
/*          The number of subdiagonals within the band of A.  KL >= 0. */

/*  KU      (input) INTEGER */
/*          The number of superdiagonals within the band of A.  KU >= 0. */

/*  AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          On entry, the matrix A in band storage, in rows 1 to KL+KU+1. */
/*          The j-th column of A is stored in the j-th column of the */
/*          array AB as follows: */
/*          AB(KU+1+i-j,j) = A(i,j) for max(1,j-KU)<=i<=min(N,j+kl) */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array A.  LDAB >= max(1,M). */

/*  R       (output) DOUBLE PRECISION array, dimension (M) */
/*          If INFO = 0 or INFO > M, R contains the row scale factors */
/*          for A. */

/*  C       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0,  C contains the column scale factors for A. */

/*  ROWCND  (output) DOUBLE PRECISION */
/*          If INFO = 0 or INFO > M, ROWCND contains the ratio of the */
/*          smallest R(i) to the largest R(i).  If ROWCND >= 0.1 and */
/*          AMAX is neither too large nor too small, it is not worth */
/*          scaling by R. */

/*  COLCND  (output) DOUBLE PRECISION */
/*          If INFO = 0, COLCND contains the ratio of the smallest */
/*          C(i) to the largest C(i).  If COLCND >= 0.1, it is not */
/*          worth scaling by C. */

/*  AMAX    (output) DOUBLE PRECISION */
/*          Absolute value of largest matrix element.  If AMAX is very */
/*          close to overflow or very close to underflow, the matrix */
/*          should be scaled. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i,  and i is */
/*                <= M:  the i-th row of A is exactly zero */
/*                >  M:  the (i-M)-th column of A is exactly zero */

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
    --r__;
    --c__;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*kl < 0) {
	*info = -3;
    } else if (*ku < 0) {
	*info = -4;
    } else if (*ldab < *kl + *ku + 1) {
	*info = -6;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGBEQUB", &i__1);
	return 0;
    }

/*     Quick return if possible. */

    if (*m == 0 || *n == 0) {
	*rowcnd = 1.;
	*colcnd = 1.;
	*amax = 0.;
	return 0;
    }

/*     Get machine constants.  Assume SMLNUM is a power of the radix. */

    smlnum = dlamch_("S");
    bignum = 1. / smlnum;
    radix = dlamch_("B");
    logrdx = log(radix);

/*     Compute row scale factors. */

    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	r__[i__] = 0.;
/* L10: */
    }

/*     Find the maximum element in each row. */

    kd = *ku + 1;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	i__2 = j - *ku;
/* Computing MIN */
	i__4 = j + *kl;
	i__3 = std::min(i__4,*m);
	for (i__ = std::max(i__2,1_integer); i__ <= i__3; ++i__) {
/* Computing MAX */
	    d__2 = r__[i__], d__3 = (d__1 = ab[kd + i__ - j + j * ab_dim1],
		    abs(d__1));
	    r__[i__] = std::max(d__2,d__3);
/* L20: */
	}
/* L30: */
    }
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (r__[i__] > 0.) {
	    i__3 = (integer) (log(r__[i__]) / logrdx);
	    r__[i__] = pow_di(&radix, &i__3);
	}
    }

/*     Find the maximum and minimum scale factors. */

    rcmin = bignum;
    rcmax = 0.;
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	d__1 = rcmax, d__2 = r__[i__];
	rcmax = std::max(d__1,d__2);
/* Computing MIN */
	d__1 = rcmin, d__2 = r__[i__];
	rcmin = std::min(d__1,d__2);
/* L40: */
    }
    *amax = rcmax;

    if (rcmin == 0.) {

/*        Find the first zero scale factor and return an error code. */

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (r__[i__] == 0.) {
		*info = i__;
		return 0;
	    }
/* L50: */
	}
    } else {

/*        Invert the scale factors. */

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MIN */
/* Computing MAX */
	    d__2 = r__[i__];
	    d__1 = std::max(d__2,smlnum);
	    r__[i__] = 1. / std::min(d__1,bignum);
/* L60: */
	}

/*        Compute ROWCND = min(R(I)) / max(R(I)). */

	*rowcnd = std::max(rcmin,smlnum) / std::min(rcmax,bignum);
    }

/*     Compute column scale factors. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	c__[j] = 0.;
/* L70: */
    }

/*     Find the maximum element in each column, */
/*     assuming the row scaling computed above. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	i__3 = j - *ku;
/* Computing MIN */
	i__4 = j + *kl;
	i__2 = std::min(i__4,*m);
	for (i__ = std::max(i__3,1_integer); i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = c__[j], d__3 = (d__1 = ab[kd + i__ - j + j * ab_dim1], abs(
		    d__1)) * r__[i__];
	    c__[j] = std::max(d__2,d__3);
/* L80: */
	}
	if (c__[j] > 0.) {
	    i__2 = (integer) (log(c__[j]) / logrdx);
	    c__[j] = pow_di(&radix, &i__2);
	}
/* L90: */
    }

/*     Find the maximum and minimum scale factors. */

    rcmin = bignum;
    rcmax = 0.;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	d__1 = rcmin, d__2 = c__[j];
	rcmin = std::min(d__1,d__2);
/* Computing MAX */
	d__1 = rcmax, d__2 = c__[j];
	rcmax = std::max(d__1,d__2);
/* L100: */
    }

    if (rcmin == 0.) {

/*        Find the first zero scale factor and return an error code. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    if (c__[j] == 0.) {
		*info = *m + j;
		return 0;
	    }
/* L110: */
	}
    } else {

/*        Invert the scale factors. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
/* Computing MAX */
	    d__2 = c__[j];
	    d__1 = std::max(d__2,smlnum);
	    c__[j] = 1. / std::min(d__1,bignum);
/* L120: */
	}

/*        Compute COLCND = min(C(J)) / max(C(J)). */

	*colcnd = std::max(rcmin,smlnum) / std::min(rcmax,bignum);
    }

    return 0;

/*     End of DGBEQUB */

} /* dgbequb_ */

/* Subroutine */ int dgbrfs_(const char *trans, integer *n, integer *kl, integer *
	ku, integer *nrhs, double *ab, integer *ldab, double *afb,
	integer *ldafb, integer *ipiv, double *b, integer *ldb,
	double *x, integer *ldx, double *ferr, double *berr,
	double *work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b15 = -1.;
	static double c_b17 = 1.;

    /* System generated locals */
    integer ab_dim1, ab_offset, afb_dim1, afb_offset, b_dim1, b_offset,
	    x_dim1, x_offset, i__1, i__2, i__3, i__4, i__5, i__6, i__7;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, k;
    double s;
    integer kk;
    double xk;
    integer nz;
    double eps;
    integer kase;
    double safe1, safe2;
    integer isave[3];
    integer count;
    double safmin;
    bool notran;
    char transt[1];
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

/*  DGBRFS improves the computed solution to a system of linear */
/*  equations when the coefficient matrix is banded, and provides */
/*  error bounds and backward error estimates for the solution. */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations: */
/*          = 'N':  A * X = B     (No transpose) */
/*          = 'T':  A**T * X = B  (Transpose) */
/*          = 'C':  A**H * X = B  (Conjugate transpose = Transpose) */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  KL      (input) INTEGER */
/*          The number of subdiagonals within the band of A.  KL >= 0. */

/*  KU      (input) INTEGER */
/*          The number of superdiagonals within the band of A.  KU >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X.  NRHS >= 0. */

/*  AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          The original band matrix A, stored in rows 1 to KL+KU+1. */
/*          The j-th column of A is stored in the j-th column of the */
/*          array AB as follows: */
/*          AB(ku+1+i-j,j) = A(i,j) for max(1,j-ku)<=i<=min(n,j+kl). */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KL+KU+1. */

/*  AFB     (input) DOUBLE PRECISION array, dimension (LDAFB,N) */
/*          Details of the LU factorization of the band matrix A, as */
/*          computed by DGBTRF.  U is stored as an upper triangular band */
/*          matrix with KL+KU superdiagonals in rows 1 to KL+KU+1, and */
/*          the multipliers used during the factorization are stored in */
/*          rows KL+KU+2 to 2*KL+KU+1. */

/*  LDAFB   (input) INTEGER */
/*          The leading dimension of the array AFB.  LDAFB >= 2*KL*KU+1. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices from DGBTRF; for 1<=i<=N, row i of the */
/*          matrix was interchanged with row IPIV(i). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          The right hand side matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (input/output) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          On entry, the solution matrix X, as computed by DGBTRS. */
/*          On exit, the improved solution matrix X. */

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

/*  Internal Parameters */
/*  =================== */

/*  ITMAX is the maximum number of steps of iterative refinement. */

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
    afb_dim1 = *ldafb;
    afb_offset = 1 + afb_dim1;
    afb -= afb_offset;
    --ipiv;
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
    notran = lsame_(trans, "N");
    if (! notran && ! lsame_(trans, "T") && ! lsame_(
	    trans, "C")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*kl < 0) {
	*info = -3;
    } else if (*ku < 0) {
	*info = -4;
    } else if (*nrhs < 0) {
	*info = -5;
    } else if (*ldab < *kl + *ku + 1) {
	*info = -7;
    } else if (*ldafb < (*kl << 1) + *ku + 1) {
	*info = -9;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -12;
    } else if (*ldx < std::max(1_integer,*n)) {
	*info = -14;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGBRFS", &i__1);
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

/* Computing MIN */
    i__1 = *kl + *ku + 2, i__2 = *n + 1;
    nz = std::min(i__1,i__2);
    eps = dlamch_("Epsilon");
    safmin = dlamch_("Safe minimum");
    safe1 = nz * safmin;
    safe2 = safe1 / eps;

/*     Do for each right hand side */

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {

	count = 1;
	lstres = 3.;
L20:

/*        Loop until stopping criterion is satisfied. */

/*        Compute residual R = B - op(A) * X, */
/*        where op(A) = A, A**T, or A**H, depending on TRANS. */

	dcopy_(n, &b[j * b_dim1 + 1], &c__1, &work[*n + 1], &c__1);
	dgbmv_(trans, n, n, kl, ku, &c_b15, &ab[ab_offset], ldab, &x[j *
		x_dim1 + 1], &c__1, &c_b17, &work[*n + 1], &c__1);

/*        Compute componentwise relative backward error from formula */

/*        max(i) ( abs(R(i)) / ( abs(op(A))*abs(X) + abs(B) )(i) ) */

/*        where abs(Z) is the componentwise absolute value of the matrix */
/*        or vector Z.  If the i-th component of the denominator is less */
/*        than SAFE2, then SAFE1 is added to the i-th components of the */
/*        numerator and denominator before dividing. */

	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[i__] = (d__1 = b[i__ + j * b_dim1], abs(d__1));
/* L30: */
	}

/*        Compute abs(op(A))*abs(X) + abs(B). */

	if (notran) {
	    i__2 = *n;
	    for (k = 1; k <= i__2; ++k) {
		kk = *ku + 1 - k;
		xk = (d__1 = x[k + j * x_dim1], abs(d__1));
/* Computing MAX */
		i__3 = 1, i__4 = k - *ku;
/* Computing MIN */
		i__6 = *n, i__7 = k + *kl;
		i__5 = std::min(i__6,i__7);
		for (i__ = std::max(i__3,i__4); i__ <= i__5; ++i__) {
		    work[i__] += (d__1 = ab[kk + i__ + k * ab_dim1], abs(d__1)
			    ) * xk;
/* L40: */
		}
/* L50: */
	    }
	} else {
	    i__2 = *n;
	    for (k = 1; k <= i__2; ++k) {
		s = 0.;
		kk = *ku + 1 - k;
/* Computing MAX */
		i__5 = 1, i__3 = k - *ku;
/* Computing MIN */
		i__6 = *n, i__7 = k + *kl;
		i__4 = std::min(i__6,i__7);
		for (i__ = std::max(i__5,i__3); i__ <= i__4; ++i__) {
		    s += (d__1 = ab[kk + i__ + k * ab_dim1], abs(d__1)) * (
			    d__2 = x[i__ + j * x_dim1], abs(d__2));
/* L60: */
		}
		work[k] += s;
/* L70: */
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
/* L80: */
	}
	berr[j] = s;

/*        Test stopping criterion. Continue iterating if */
/*           1) The residual BERR(J) is larger than machine epsilon, and */
/*           2) BERR(J) decreased by at least a factor of 2 during the */
/*              last iteration, and */
/*           3) At most ITMAX iterations tried. */

	if (berr[j] > eps && berr[j] * 2. <= lstres && count <= 5) {

/*           Update solution and try again. */

	    dgbtrs_(trans, n, kl, ku, &c__1, &afb[afb_offset], ldafb, &ipiv[1]
, &work[*n + 1], n, info);
	    daxpy_(n, &c_b17, &work[*n + 1], &c__1, &x[j * x_dim1 + 1], &c__1)
		    ;
	    lstres = berr[j];
	    ++count;
	    goto L20;
	}

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
/* L90: */
	}

	kase = 0;
L100:
	dlacn2_(n, &work[(*n << 1) + 1], &work[*n + 1], &iwork[1], &ferr[j], &
		kase, isave);
	if (kase != 0) {
	    if (kase == 1) {

/*              Multiply by diag(W)*inv(op(A)**T). */

		dgbtrs_(transt, n, kl, ku, &c__1, &afb[afb_offset], ldafb, &
			ipiv[1], &work[*n + 1], n, info);
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] *= work[i__];
/* L110: */
		}
	    } else {

/*              Multiply by inv(op(A))*diag(W). */

		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] *= work[i__];
/* L120: */
		}
		dgbtrs_(trans, n, kl, ku, &c__1, &afb[afb_offset], ldafb, &
			ipiv[1], &work[*n + 1], n, info);
	    }
	    goto L100;
	}

/*        Normalize error. */

	lstres = 0.;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = lstres, d__3 = (d__1 = x[i__ + j * x_dim1], abs(d__1));
	    lstres = std::max(d__2,d__3);
/* L130: */
	}
	if (lstres != 0.) {
	    ferr[j] /= lstres;
	}

/* L140: */
    }

    return 0;

/*     End of DGBRFS */

} /* dgbrfs_ */

#if 0
/* Subroutine */ int dgbrfsx_(const char *trans, const char *equed, integer *n, integer *
	kl, integer *ku, integer *nrhs, double *ab, integer *ldab,
	double *afb, integer *ldafb, integer *ipiv, double *r__,
	double *c__, double *b, integer *ldb, double *x, integer *
	ldx, double *rcond, double *berr, integer *n_err_bnds__,
	double *err_bnds_norm__, double *err_bnds_comp__, integer *
	nparams, double *params, double *work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c_n1 = -1;
	static integer c__0 = 0;
	static integer c__1 = 1;

    /* System generated locals */
    integer ab_dim1, ab_offset, afb_dim1, afb_offset, b_dim1, b_offset,
	    x_dim1, x_offset, err_bnds_norm_dim1, err_bnds_norm_offset,
	    err_bnds_comp_dim1, err_bnds_comp_offset, i__1;
    double d__1, d__2;

    /* Local variables */
    double illrcond_thresh__, unstable_thresh__, err_lbnd__;
    integer ref_type__;
    integer j;
    double rcond_tmp__;
    integer prec_type__, trans_type__;
    double cwise_wrong__;
    char norm[1];
    bool ignore_cwise__;
    double anorm;
    bool colequ, notran, rowequ;
    integer ithresh, n_norms__;
    double rthresh;


/*     -- LAPACK routine (version 3.2.1)                                 -- */
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

/*     Purpose */
/*     ======= */

/*     DGBRFSX improves the computed solution to a system of linear */
/*     equations and provides error bounds and backward error estimates */
/*     for the solution.  In addition to normwise error bound, the code */
/*     provides maximum componentwise error bound if possible.  See */
/*     comments for ERR_BNDS_NORM and ERR_BNDS_COMP for details of the */
/*     error bounds. */

/*     The original system of linear equations may have been equilibrated */
/*     before calling this routine, as described by arguments EQUED, R */
/*     and C below. In this case, the solution and error bounds returned */
/*     are for the original unequilibrated system. */

/*     Arguments */
/*     ========= */

/*     Some optional parameters are bundled in the PARAMS array.  These */
/*     settings determine how refinement is performed, but often the */
/*     defaults are acceptable.  If the defaults are acceptable, users */
/*     can pass NPARAMS = 0 which prevents the source code from accessing */
/*     the PARAMS argument. */

/*     TRANS   (input) CHARACTER*1 */
/*     Specifies the form of the system of equations: */
/*       = 'N':  A * X = B     (No transpose) */
/*       = 'T':  A**T * X = B  (Transpose) */
/*       = 'C':  A**H * X = B  (Conjugate transpose = Transpose) */

/*     EQUED   (input) CHARACTER*1 */
/*     Specifies the form of equilibration that was done to A */
/*     before calling this routine. This is needed to compute */
/*     the solution and error bounds correctly. */
/*       = 'N':  No equilibration */
/*       = 'R':  Row equilibration, i.e., A has been premultiplied by */
/*               diag(R). */
/*       = 'C':  Column equilibration, i.e., A has been postmultiplied */
/*               by diag(C). */
/*       = 'B':  Both row and column equilibration, i.e., A has been */
/*               replaced by diag(R) * A * diag(C). */
/*               The right hand side B has been changed accordingly. */

/*     N       (input) INTEGER */
/*     The order of the matrix A.  N >= 0. */

/*     KL      (input) INTEGER */
/*     The number of subdiagonals within the band of A.  KL >= 0. */

/*     KU      (input) INTEGER */
/*     The number of superdiagonals within the band of A.  KU >= 0. */

/*     NRHS    (input) INTEGER */
/*     The number of right hand sides, i.e., the number of columns */
/*     of the matrices B and X.  NRHS >= 0. */

/*     AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*     The original band matrix A, stored in rows 1 to KL+KU+1. */
/*     The j-th column of A is stored in the j-th column of the */
/*     array AB as follows: */
/*     AB(ku+1+i-j,j) = A(i,j) for max(1,j-ku)<=i<=min(n,j+kl). */

/*     LDAB    (input) INTEGER */
/*     The leading dimension of the array AB.  LDAB >= KL+KU+1. */

/*     AFB     (input) DOUBLE PRECISION array, dimension (LDAFB,N) */
/*     Details of the LU factorization of the band matrix A, as */
/*     computed by DGBTRF.  U is stored as an upper triangular band */
/*     matrix with KL+KU superdiagonals in rows 1 to KL+KU+1, and */
/*     the multipliers used during the factorization are stored in */
/*     rows KL+KU+2 to 2*KL+KU+1. */

/*     LDAFB   (input) INTEGER */
/*     The leading dimension of the array AFB.  LDAFB >= 2*KL*KU+1. */

/*     IPIV    (input) INTEGER array, dimension (N) */
/*     The pivot indices from DGETRF; for 1<=i<=N, row i of the */
/*     matrix was interchanged with row IPIV(i). */

/*     R       (input or output) DOUBLE PRECISION array, dimension (N) */
/*     The row scale factors for A.  If EQUED = 'R' or 'B', A is */
/*     multiplied on the left by diag(R); if EQUED = 'N' or 'C', R */
/*     is not accessed.  R is an input argument if FACT = 'F'; */
/*     otherwise, R is an output argument.  If FACT = 'F' and */
/*     EQUED = 'R' or 'B', each element of R must be positive. */
/*     If R is output, each element of R is a power of the radix. */
/*     If R is input, each element of R should be a power of the radix */
/*     to ensure a reliable solution and error estimates. Scaling by */
/*     powers of the radix does not cause rounding errors unless the */
/*     result underflows or overflows. Rounding errors during scaling */
/*     lead to refining with a matrix that is not equivalent to the */
/*     input matrix, producing error estimates that may not be */
/*     reliable. */

/*     C       (input or output) DOUBLE PRECISION array, dimension (N) */
/*     The column scale factors for A.  If EQUED = 'C' or 'B', A is */
/*     multiplied on the right by diag(C); if EQUED = 'N' or 'R', C */
/*     is not accessed.  C is an input argument if FACT = 'F'; */
/*     otherwise, C is an output argument.  If FACT = 'F' and */
/*     EQUED = 'C' or 'B', each element of C must be positive. */
/*     If C is output, each element of C is a power of the radix. */
/*     If C is input, each element of C should be a power of the radix */
/*     to ensure a reliable solution and error estimates. Scaling by */
/*     powers of the radix does not cause rounding errors unless the */
/*     result underflows or overflows. Rounding errors during scaling */
/*     lead to refining with a matrix that is not equivalent to the */
/*     input matrix, producing error estimates that may not be */
/*     reliable. */

/*     B       (input) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*     The right hand side matrix B. */

/*     LDB     (input) INTEGER */
/*     The leading dimension of the array B.  LDB >= max(1,N). */

/*     X       (input/output) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*     On entry, the solution matrix X, as computed by DGETRS. */
/*     On exit, the improved solution matrix X. */

/*     LDX     (input) INTEGER */
/*     The leading dimension of the array X.  LDX >= max(1,N). */

/*     RCOND   (output) DOUBLE PRECISION */
/*     Reciprocal scaled condition number.  This is an estimate of the */
/*     reciprocal Skeel condition number of the matrix A after */
/*     equilibration (if done).  If this is less than the machine */
/*     precision (in particular, if it is zero), the matrix is singular */
/*     to working precision.  Note that the error may still be small even */
/*     if this number is very small and the matrix appears ill- */
/*     conditioned. */

/*     BERR    (output) DOUBLE PRECISION array, dimension (NRHS) */
/*     Componentwise relative backward error.  This is the */
/*     componentwise relative backward error of each solution vector X(j) */
/*     (i.e., the smallest relative change in any element of A or B that */
/*     makes X(j) an exact solution). */

/*     N_ERR_BNDS (input) INTEGER */
/*     Number of error bounds to return for each right hand side */
/*     and each type (normwise or componentwise).  See ERR_BNDS_NORM and */
/*     ERR_BNDS_COMP below. */

/*     ERR_BNDS_NORM  (output) DOUBLE PRECISION array, dimension (NRHS, N_ERR_BNDS) */
/*     For each right-hand side, this array contains information about */
/*     various error bounds and condition numbers corresponding to the */
/*     normwise relative error, which is defined as follows: */

/*     Normwise relative error in the ith solution vector: */
/*             max_j (abs(XTRUE(j,i) - X(j,i))) */
/*            ------------------------------ */
/*                  max_j abs(X(j,i)) */

/*     The array is indexed by the type of error information as described */
/*     below. There currently are up to three pieces of information */
/*     returned. */

/*     The first index in ERR_BNDS_NORM(i,:) corresponds to the ith */
/*     right-hand side. */

/*     The second index in ERR_BNDS_NORM(:,err) contains the following */
/*     three fields: */
/*     err = 1 "Trust/don't trust" boolean. Trust the answer if the */
/*              reciprocal condition number is less than the threshold */
/*              sqrt(n) * dlamch('Epsilon'). */

/*     err = 2 "Guaranteed" error bound: The estimated forward error, */
/*              almost certainly within a factor of 10 of the true error */
/*              so long as the next entry is greater than the threshold */
/*              sqrt(n) * dlamch('Epsilon'). This error bound should only */
/*              be trusted if the previous boolean is true. */

/*     err = 3  Reciprocal condition number: Estimated normwise */
/*              reciprocal condition number.  Compared with the threshold */
/*              sqrt(n) * dlamch('Epsilon') to determine if the error */
/*              estimate is "guaranteed". These reciprocal condition */
/*              numbers are 1 / (norm(Z^{-1},inf) * norm(Z,inf)) for some */
/*              appropriately scaled matrix Z. */
/*              Let Z = S*A, where S scales each row by a power of the */
/*              radix so all absolute row sums of Z are approximately 1. */

/*     See Lapack Working Note 165 for further details and extra */
/*     cautions. */

/*     ERR_BNDS_COMP  (output) DOUBLE PRECISION array, dimension (NRHS, N_ERR_BNDS) */
/*     For each right-hand side, this array contains information about */
/*     various error bounds and condition numbers corresponding to the */
/*     componentwise relative error, which is defined as follows: */

/*     Componentwise relative error in the ith solution vector: */
/*                    abs(XTRUE(j,i) - X(j,i)) */
/*             max_j ---------------------- */
/*                         abs(X(j,i)) */

/*     The array is indexed by the right-hand side i (on which the */
/*     componentwise relative error depends), and the type of error */
/*     information as described below. There currently are up to three */
/*     pieces of information returned for each right-hand side. If */
/*     componentwise accuracy is not requested (PARAMS(3) = 0.0), then */
/*     ERR_BNDS_COMP is not accessed.  If N_ERR_BNDS .LT. 3, then at most */
/*     the first (:,N_ERR_BNDS) entries are returned. */

/*     The first index in ERR_BNDS_COMP(i,:) corresponds to the ith */
/*     right-hand side. */

/*     The second index in ERR_BNDS_COMP(:,err) contains the following */
/*     three fields: */
/*     err = 1 "Trust/don't trust" boolean. Trust the answer if the */
/*              reciprocal condition number is less than the threshold */
/*              sqrt(n) * dlamch('Epsilon'). */

/*     err = 2 "Guaranteed" error bound: The estimated forward error, */
/*              almost certainly within a factor of 10 of the true error */
/*              so long as the next entry is greater than the threshold */
/*              sqrt(n) * dlamch('Epsilon'). This error bound should only */
/*              be trusted if the previous boolean is true. */

/*     err = 3  Reciprocal condition number: Estimated componentwise */
/*              reciprocal condition number.  Compared with the threshold */
/*              sqrt(n) * dlamch('Epsilon') to determine if the error */
/*              estimate is "guaranteed". These reciprocal condition */
/*              numbers are 1 / (norm(Z^{-1},inf) * norm(Z,inf)) for some */
/*              appropriately scaled matrix Z. */
/*              Let Z = S*(A*diag(x)), where x is the solution for the */
/*              current right-hand side and S scales each row of */
/*              A*diag(x) by a power of the radix so all absolute row */
/*              sums of Z are approximately 1. */

/*     See Lapack Working Note 165 for further details and extra */
/*     cautions. */

/*     NPARAMS (input) INTEGER */
/*     Specifies the number of parameters set in PARAMS.  If .LE. 0, the */
/*     PARAMS array is never referenced and default values are used. */

/*     PARAMS  (input / output) DOUBLE PRECISION array, dimension NPARAMS */
/*     Specifies algorithm parameters.  If an entry is .LT. 0.0, then */
/*     that entry will be filled with default value used for that */
/*     parameter.  Only positions up to NPARAMS are accessed; defaults */
/*     are used for higher-numbered parameters. */

/*       PARAMS(LA_LINRX_ITREF_I = 1) : Whether to perform iterative */
/*            refinement or not. */
/*         Default: 1.0D+0 */
/*            = 0.0 : No refinement is performed, and no error bounds are */
/*                    computed. */
/*            = 1.0 : Use the double-precision refinement algorithm, */
/*                    possibly with doubled-single computations if the */
/*                    compilation environment does not support DOUBLE */
/*                    PRECISION. */
/*              (other values are reserved for future use) */

/*       PARAMS(LA_LINRX_ITHRESH_I = 2) : Maximum number of residual */
/*            computations allowed for refinement. */
/*         Default: 10 */
/*         Aggressive: Set to 100 to permit convergence using approximate */
/*                     factorizations or factorizations other than LU. If */
/*                     the factorization uses a technique other than */
/*                     Gaussian elimination, the guarantees in */
/*                     err_bnds_norm and err_bnds_comp may no longer be */
/*                     trustworthy. */

/*       PARAMS(LA_LINRX_CWISE_I = 3) : Flag determining if the code */
/*            will attempt to find a solution with small componentwise */
/*            relative error in the double-precision algorithm.  Positive */
/*            is true, 0.0 is false. */
/*         Default: 1.0 (attempt componentwise convergence) */

/*     WORK    (workspace) DOUBLE PRECISION array, dimension (4*N) */

/*     IWORK   (workspace) INTEGER array, dimension (N) */

/*     INFO    (output) INTEGER */
/*       = 0:  Successful exit. The solution to every right-hand side is */
/*         guaranteed. */
/*       < 0:  If INFO = -i, the i-th argument had an illegal value */
/*       > 0 and <= N:  U(INFO,INFO) is exactly zero.  The factorization */
/*         has been completed, but the factor U is exactly singular, so */
/*         the solution and error bounds could not be computed. RCOND = 0 */
/*         is returned. */
/*       = N+J: The solution corresponding to the Jth right-hand side is */
/*         not guaranteed. The solutions corresponding to other right- */
/*         hand sides K with K > J may not be guaranteed as well, but */
/*         only the first such right-hand side is reported. If a small */
/*         componentwise error is not requested (PARAMS(3) = 0.0) then */
/*         the Jth right-hand side is the first with a normwise error */
/*         bound that is not guaranteed (the smallest J such */
/*         that ERR_BNDS_NORM(J,1) = 0.0). By default (PARAMS(3) = 1.0) */
/*         the Jth right-hand side is the first with either a normwise or */
/*         componentwise error bound that is not guaranteed (the smallest */
/*         J such that either ERR_BNDS_NORM(J,1) = 0.0 or */
/*         ERR_BNDS_COMP(J,1) = 0.0). See the definition of */
/*         ERR_BNDS_NORM(:,1) and ERR_BNDS_COMP(:,1). To get information */
/*         about all of the right-hand sides check ERR_BNDS_NORM or */
/*         ERR_BNDS_COMP. */

/*     ================================================================== */

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

/*     Check the input parameters. */

    /* Parameter adjustments */
    err_bnds_comp_dim1 = *nrhs;
    err_bnds_comp_offset = 1 + err_bnds_comp_dim1;
    err_bnds_comp__ -= err_bnds_comp_offset;
    err_bnds_norm_dim1 = *nrhs;
    err_bnds_norm_offset = 1 + err_bnds_norm_dim1;
    err_bnds_norm__ -= err_bnds_norm_offset;
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    afb_dim1 = *ldafb;
    afb_offset = 1 + afb_dim1;
    afb -= afb_offset;
    --ipiv;
    --r__;
    --c__;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    --berr;
    --params;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    trans_type__ = ilatrans_(trans);
    ref_type__ = 1;
    if (*nparams >= 1) {
	if (params[1] < 0.) {
	    params[1] = 1.;
	} else {
	    ref_type__ = (integer) params[1];
	}
    }

/*     Set default parameters. */

    illrcond_thresh__ = (double) (*n) * dlamch_("Epsilon");
    ithresh = 10;
    rthresh = .5;
    unstable_thresh__ = .25;
    ignore_cwise__ = false;

    if (*nparams >= 2) {
	if (params[2] < 0.) {
	    params[2] = (double) ithresh;
	} else {
	    ithresh = (integer) params[2];
	}
    }
    if (*nparams >= 3) {
	if (params[3] < 0.) {
	    if (ignore_cwise__) {
		params[3] = 0.;
	    } else {
		params[3] = 1.;
	    }
	} else {
	    ignore_cwise__ = params[3] == 0.;
	}
    }
    if (ref_type__ == 0 || *n_err_bnds__ == 0) {
	n_norms__ = 0;
    } else if (ignore_cwise__) {
	n_norms__ = 1;
    } else {
	n_norms__ = 2;
    }

    notran = lsame_(trans, "N");
    rowequ = lsame_(equed, "R") || lsame_(equed, "B");
    colequ = lsame_(equed, "C") || lsame_(equed, "B");

/*     Test input parameters. */

    if (trans_type__ == -1) {
	*info = -1;
    } else if (! rowequ && ! colequ && ! lsame_(equed, "N")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*kl < 0) {
	*info = -4;
    } else if (*ku < 0) {
	*info = -5;
    } else if (*nrhs < 0) {
	*info = -6;
    } else if (*ldab < *kl + *ku + 1) {
	*info = -8;
    } else if (*ldafb < (*kl << 1) + *ku + 1) {
	*info = -10;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -13;
    } else if (*ldx < std::max(1_integer,*n)) {
	*info = -15;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGBRFSX", &i__1);
	return 0;
    }

/*     Quick return if possible. */

    if (*n == 0 || *nrhs == 0) {
	*rcond = 1.;
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    berr[j] = 0.;
	    if (*n_err_bnds__ >= 1) {
		err_bnds_norm__[j + err_bnds_norm_dim1] = 1.;
		err_bnds_comp__[j + err_bnds_comp_dim1] = 1.;
	    } else if (*n_err_bnds__ >= 2) {
		err_bnds_norm__[j + (err_bnds_norm_dim1 << 1)] = 0.;
		err_bnds_comp__[j + (err_bnds_comp_dim1 << 1)] = 0.;
	    } else if (*n_err_bnds__ >= 3) {
		err_bnds_norm__[j + err_bnds_norm_dim1 * 3] = 1.;
		err_bnds_comp__[j + err_bnds_comp_dim1 * 3] = 1.;
	    }
	}
	return 0;
    }

/*     Default to failure. */

    *rcond = 0.;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	berr[j] = 1.;
	if (*n_err_bnds__ >= 1) {
	    err_bnds_norm__[j + err_bnds_norm_dim1] = 1.;
	    err_bnds_comp__[j + err_bnds_comp_dim1] = 1.;
	} else if (*n_err_bnds__ >= 2) {
	    err_bnds_norm__[j + (err_bnds_norm_dim1 << 1)] = 1.;
	    err_bnds_comp__[j + (err_bnds_comp_dim1 << 1)] = 1.;
	} else if (*n_err_bnds__ >= 3) {
	    err_bnds_norm__[j + err_bnds_norm_dim1 * 3] = 0.;
	    err_bnds_comp__[j + err_bnds_comp_dim1 * 3] = 0.;
	}
    }

/*     Compute the norm of A and the reciprocal of the condition */
/*     number of A. */

    if (notran) {
	*(unsigned char *)norm = 'I';
    } else {
	*(unsigned char *)norm = '1';
    }
    anorm = dlangb_(norm, n, kl, ku, &ab[ab_offset], ldab, &work[1]);
    dgbcon_(norm, n, kl, ku, &afb[afb_offset], ldafb, &ipiv[1], &anorm, rcond,
	     &work[1], &iwork[1], info);

/*     Perform refinement on each right-hand side */

    if (ref_type__ != 0) {
	prec_type__ = ilaprec_("E");
	if (notran) {
	    dla_gbrfsx_extended__(&prec_type__, &trans_type__, n, kl, ku,
		    nrhs, &ab[ab_offset], ldab, &afb[afb_offset], ldafb, &
		    ipiv[1], &colequ, &c__[1], &b[b_offset], ldb, &x[x_offset]
		    , ldx, &berr[1], &n_norms__, &err_bnds_norm__[
		    err_bnds_norm_offset], &err_bnds_comp__[
		    err_bnds_comp_offset], &work[*n + 1], &work[1], &work[(*n
		    << 1) + 1], &work[1], rcond, &ithresh, &rthresh, &
		    unstable_thresh__, &ignore_cwise__, info);
	} else {
	    dla_gbrfsx_extended__(&prec_type__, &trans_type__, n, kl, ku,
		    nrhs, &ab[ab_offset], ldab, &afb[afb_offset], ldafb, &
		    ipiv[1], &rowequ, &r__[1], &b[b_offset], ldb, &x[x_offset]
		    , ldx, &berr[1], &n_norms__, &err_bnds_norm__[
		    err_bnds_norm_offset], &err_bnds_comp__[
		    err_bnds_comp_offset], &work[*n + 1], &work[1], &work[(*n
		    << 1) + 1], &work[1], rcond, &ithresh, &rthresh, &
		    unstable_thresh__, &ignore_cwise__, info);
	}
    }
/* Computing MAX */
    d__1 = 10., d__2 = sqrt((double) (*n));
    err_lbnd__ = std::max(d__1,d__2) * dlamch_("Epsilon");
    if (*n_err_bnds__ >= 1 && n_norms__ >= 1) {

/*     Compute scaled normwise condition number cond(A*C). */

	if (colequ && notran) {
	    rcond_tmp__ = dla_gbrcond__(trans, n, kl, ku, &ab[ab_offset],
		    ldab, &afb[afb_offset], ldafb, &ipiv[1], &c_n1, &c__[1],
		    info, &work[1], &iwork[1], 1_integer);
	} else if (rowequ && ! notran) {
	    rcond_tmp__ = dla_gbrcond__(trans, n, kl, ku, &ab[ab_offset],
		    ldab, &afb[afb_offset], ldafb, &ipiv[1], &c_n1, &r__[1],
		    info, &work[1], &iwork[1], 1_integer);
	} else {
	    rcond_tmp__ = dla_gbrcond__(trans, n, kl, ku, &ab[ab_offset],
		    ldab, &afb[afb_offset], ldafb, &ipiv[1], &c__0, &r__[1],
		    info, &work[1], &iwork[1], 1_integer);
	}
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {

/*     Cap the error at 1.0. */

	    if (*n_err_bnds__ >= 2 && err_bnds_norm__[j + (err_bnds_norm_dim1
		    << 1)] > 1.) {
		err_bnds_norm__[j + (err_bnds_norm_dim1 << 1)] = 1.;
	    }

/*     Threshold the error (see LAWN). */

	    if (rcond_tmp__ < illrcond_thresh__) {
		err_bnds_norm__[j + (err_bnds_norm_dim1 << 1)] = 1.;
		err_bnds_norm__[j + err_bnds_norm_dim1] = 0.;
		if (*info <= *n) {
		    *info = *n + j;
		}
	    } else if (err_bnds_norm__[j + (err_bnds_norm_dim1 << 1)] <
		    err_lbnd__) {
		err_bnds_norm__[j + (err_bnds_norm_dim1 << 1)] = err_lbnd__;
		err_bnds_norm__[j + err_bnds_norm_dim1] = 1.;
	    }

/*     Save the condition number. */

	    if (*n_err_bnds__ >= 3) {
		err_bnds_norm__[j + err_bnds_norm_dim1 * 3] = rcond_tmp__;
	    }
	}
    }
    if (*n_err_bnds__ >= 1 && n_norms__ >= 2) {

/*     Compute componentwise condition number cond(A*diag(Y(:,J))) for */
/*     each right-hand side using the current solution as an estimate of */
/*     the true solution.  If the componentwise error estimate is too */
/*     large, then the solution is a lousy estimate of truth and the */
/*     estimated RCOND may be too optimistic.  To avoid misleading users, */
/*     the inverse condition number is set to 0.0 when the estimated */
/*     cwise error is at least CWISE_WRONG. */

	cwise_wrong__ = sqrt(dlamch_("Epsilon"));
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    if (err_bnds_comp__[j + (err_bnds_comp_dim1 << 1)] <
		    cwise_wrong__) {
		rcond_tmp__ = dla_gbrcond__(trans, n, kl, ku, &ab[ab_offset],
			ldab, &afb[afb_offset], ldafb, &ipiv[1], &c__1, &x[j *
			 x_dim1 + 1], info, &work[1], &iwork[1], 1_integer);
	    } else {
		rcond_tmp__ = 0.;
	    }

/*     Cap the error at 1.0. */

	    if (*n_err_bnds__ >= 2 && err_bnds_comp__[j + (err_bnds_comp_dim1
		    << 1)] > 1.) {
		err_bnds_comp__[j + (err_bnds_comp_dim1 << 1)] = 1.;
	    }

/*     Threshold the error (see LAWN). */

	    if (rcond_tmp__ < illrcond_thresh__) {
		err_bnds_comp__[j + (err_bnds_comp_dim1 << 1)] = 1.;
		err_bnds_comp__[j + err_bnds_comp_dim1] = 0.;
		if (params[3] == 1. && *info < *n + j) {
		    *info = *n + j;
		}
	    } else if (err_bnds_comp__[j + (err_bnds_comp_dim1 << 1)] <
		    err_lbnd__) {
		err_bnds_comp__[j + (err_bnds_comp_dim1 << 1)] = err_lbnd__;
		err_bnds_comp__[j + err_bnds_comp_dim1] = 1.;
	    }

/*     Save the condition number. */

	    if (*n_err_bnds__ >= 3) {
		err_bnds_comp__[j + err_bnds_comp_dim1 * 3] = rcond_tmp__;
	    }
	}
    }

    return 0;

/*     End of DGBRFSX */

} /* dgbrfsx_ */
#endif

/* Subroutine */ int dgbsv_(integer *n, integer *kl, integer *ku, integer *
	nrhs, double *ab, integer *ldab, integer *ipiv, double *b,
	integer *ldb, integer *info)
{
    /* System generated locals */
    integer ab_dim1, ab_offset, b_dim1, b_offset, i__1;

/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGBSV computes the solution to a real system of linear equations */
/*  A * X = B, where A is a band matrix of order N with KL subdiagonals */
/*  and KU superdiagonals, and X and B are N-by-NRHS matrices. */

/*  The LU decomposition with partial pivoting and row interchanges is */
/*  used to factor A as A = L * U, where L is a product of permutation */
/*  and unit lower triangular matrices with KL subdiagonals, and U is */
/*  upper triangular with KL+KU superdiagonals.  The factored form of A */
/*  is then used to solve the system of equations A * X = B. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of linear equations, i.e., the order of the */
/*          matrix A.  N >= 0. */

/*  KL      (input) INTEGER */
/*          The number of subdiagonals within the band of A.  KL >= 0. */

/*  KU      (input) INTEGER */
/*          The number of superdiagonals within the band of A.  KU >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  AB      (input/output) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          On entry, the matrix A in band storage, in rows KL+1 to */
/*          2*KL+KU+1; rows 1 to KL of the array need not be set. */
/*          The j-th column of A is stored in the j-th column of the */
/*          array AB as follows: */
/*          AB(KL+KU+1+i-j,j) = A(i,j) for max(1,j-KU)<=i<=min(N,j+KL) */
/*          On exit, details of the factorization: U is stored as an */
/*          upper triangular band matrix with KL+KU superdiagonals in */
/*          rows 1 to KL+KU+1, and the multipliers used during the */
/*          factorization are stored in rows KL+KU+2 to 2*KL+KU+1. */
/*          See below for further details. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= 2*KL+KU+1. */

/*  IPIV    (output) INTEGER array, dimension (N) */
/*          The pivot indices that define the permutation matrix P; */
/*          row i of the matrix was interchanged with row IPIV(i). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the N-by-NRHS right hand side matrix B. */
/*          On exit, if INFO = 0, the N-by-NRHS solution matrix X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, U(i,i) is exactly zero.  The factorization */
/*                has been completed, but the factor U is exactly */
/*                singular, and the solution has not been computed. */

/*  Further Details */
/*  =============== */

/*  The band storage scheme is illustrated by the following example, when */
/*  M = N = 6, KL = 2, KU = 1: */

/*  On entry:                       On exit: */

/*      *    *    *    +    +    +       *    *    *   u14  u25  u36 */
/*      *    *    +    +    +    +       *    *   u13  u24  u35  u46 */
/*      *   a12  a23  a34  a45  a56      *   u12  u23  u34  u45  u56 */
/*     a11  a22  a33  a44  a55  a66     u11  u22  u33  u44  u55  u66 */
/*     a21  a32  a43  a54  a65   *      m21  m32  m43  m54  m65   * */
/*     a31  a42  a53  a64   *    *      m31  m42  m53  m64   *    * */

/*  Array elements marked * are not used by the routine; elements marked */
/*  + need not be set on entry, but are required by the routine to store */
/*  elements of U because of fill-in resulting from the row interchanges. */

/*  ===================================================================== */

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
    --ipiv;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    *info = 0;
    if (*n < 0) {
	*info = -1;
    } else if (*kl < 0) {
	*info = -2;
    } else if (*ku < 0) {
	*info = -3;
    } else if (*nrhs < 0) {
	*info = -4;
    } else if (*ldab < (*kl << 1) + *ku + 1) {
	*info = -6;
    } else if (*ldb < std::max(*n,1_integer)) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGBSV ", &i__1);
	return 0;
    }

/*     Compute the LU factorization of the band matrix A. */

    dgbtrf_(n, n, kl, ku, &ab[ab_offset], ldab, &ipiv[1], info);
    if (*info == 0) {

/*        Solve the system A*X = B, overwriting B with X. */

	dgbtrs_("No transpose", n, kl, ku, nrhs, &ab[ab_offset], ldab, &ipiv[
		1], &b[b_offset], ldb, info);
    }
    return 0;

/*     End of DGBSV */

} /* dgbsv_ */

/* Subroutine */ int dgbsvx_(const char *fact, const char *trans, integer *n, integer *kl,
	integer *ku, integer *nrhs, double *ab, integer *ldab,
	double *afb, integer *ldafb, integer *ipiv, char *equed,
	double *r__, double *c__, double *b, integer *ldb,
	double *x, integer *ldx, double *rcond, double *ferr,
	double *berr, double *work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer ab_dim1, ab_offset, afb_dim1, afb_offset, b_dim1, b_offset,
	    x_dim1, x_offset, i__1, i__2, i__3, i__4, i__5;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, j1, j2;
    double amax;
    char norm[1];
    double rcmin, rcmax, anorm;
    bool equil;
    double colcnd;
    bool nofact;
    double bignum;
    integer infequ;
    bool colequ;
    double rowcnd;
    bool notran;
    double smlnum;
    bool rowequ;
    double rpvgrw;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGBSVX uses the LU factorization to compute the solution to a real */
/*  system of linear equations A * X = B, A**T * X = B, or A**H * X = B, */
/*  where A is a band matrix of order N with KL subdiagonals and KU */
/*  superdiagonals, and X and B are N-by-NRHS matrices. */

/*  Error bounds on the solution and a condition estimate are also */
/*  provided. */

/*  Description */
/*  =========== */

/*  The following steps are performed by this subroutine: */

/*  1. If FACT = 'E', real scaling factors are computed to equilibrate */
/*     the system: */
/*        TRANS = 'N':  diag(R)*A*diag(C)     *inv(diag(C))*X = diag(R)*B */
/*        TRANS = 'T': (diag(R)*A*diag(C))**T *inv(diag(R))*X = diag(C)*B */
/*        TRANS = 'C': (diag(R)*A*diag(C))**H *inv(diag(R))*X = diag(C)*B */
/*     Whether or not the system will be equilibrated depends on the */
/*     scaling of the matrix A, but if equilibration is used, A is */
/*     overwritten by diag(R)*A*diag(C) and B by diag(R)*B (if TRANS='N') */
/*     or diag(C)*B (if TRANS = 'T' or 'C'). */

/*  2. If FACT = 'N' or 'E', the LU decomposition is used to factor the */
/*     matrix A (after equilibration if FACT = 'E') as */
/*        A = L * U, */
/*     where L is a product of permutation and unit lower triangular */
/*     matrices with KL subdiagonals, and U is upper triangular with */
/*     KL+KU superdiagonals. */

/*  3. If some U(i,i)=0, so that U is exactly singular, then the routine */
/*     returns with INFO = i. Otherwise, the factored form of A is used */
/*     to estimate the condition number of the matrix A.  If the */
/*     reciprocal of the condition number is less than machine precision, */
/*     INFO = N+1 is returned as a warning, but the routine still goes on */
/*     to solve for X and compute error bounds as described below. */

/*  4. The system of equations is solved for X using the factored form */
/*     of A. */

/*  5. Iterative refinement is applied to improve the computed solution */
/*     matrix and calculate error bounds and backward error estimates */
/*     for it. */

/*  6. If equilibration was used, the matrix X is premultiplied by */
/*     diag(C) (if TRANS = 'N') or diag(R) (if TRANS = 'T' or 'C') so */
/*     that it solves the original system before equilibration. */

/*  Arguments */
/*  ========= */

/*  FACT    (input) CHARACTER*1 */
/*          Specifies whether or not the factored form of the matrix A is */
/*          supplied on entry, and if not, whether the matrix A should be */
/*          equilibrated before it is factored. */
/*          = 'F':  On entry, AFB and IPIV contain the factored form of */
/*                  A.  If EQUED is not 'N', the matrix A has been */
/*                  equilibrated with scaling factors given by R and C. */
/*                  AB, AFB, and IPIV are not modified. */
/*          = 'N':  The matrix A will be copied to AFB and factored. */
/*          = 'E':  The matrix A will be equilibrated if necessary, then */
/*                  copied to AFB and factored. */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations. */
/*          = 'N':  A * X = B     (No transpose) */
/*          = 'T':  A**T * X = B  (Transpose) */
/*          = 'C':  A**H * X = B  (Transpose) */

/*  N       (input) INTEGER */
/*          The number of linear equations, i.e., the order of the */
/*          matrix A.  N >= 0. */

/*  KL      (input) INTEGER */
/*          The number of subdiagonals within the band of A.  KL >= 0. */

/*  KU      (input) INTEGER */
/*          The number of superdiagonals within the band of A.  KU >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X.  NRHS >= 0. */

/*  AB      (input/output) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          On entry, the matrix A in band storage, in rows 1 to KL+KU+1. */
/*          The j-th column of A is stored in the j-th column of the */
/*          array AB as follows: */
/*          AB(KU+1+i-j,j) = A(i,j) for max(1,j-KU)<=i<=min(N,j+kl) */

/*          If FACT = 'F' and EQUED is not 'N', then A must have been */
/*          equilibrated by the scaling factors in R and/or C.  AB is not */
/*          modified if FACT = 'F' or 'N', or if FACT = 'E' and */
/*          EQUED = 'N' on exit. */

/*          On exit, if EQUED .ne. 'N', A is scaled as follows: */
/*          EQUED = 'R':  A := diag(R) * A */
/*          EQUED = 'C':  A := A * diag(C) */
/*          EQUED = 'B':  A := diag(R) * A * diag(C). */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KL+KU+1. */

/*  AFB     (input or output) DOUBLE PRECISION array, dimension (LDAFB,N) */
/*          If FACT = 'F', then AFB is an input argument and on entry */
/*          contains details of the LU factorization of the band matrix */
/*          A, as computed by DGBTRF.  U is stored as an upper triangular */
/*          band matrix with KL+KU superdiagonals in rows 1 to KL+KU+1, */
/*          and the multipliers used during the factorization are stored */
/*          in rows KL+KU+2 to 2*KL+KU+1.  If EQUED .ne. 'N', then AFB is */
/*          the factored form of the equilibrated matrix A. */

/*          If FACT = 'N', then AFB is an output argument and on exit */
/*          returns details of the LU factorization of A. */

/*          If FACT = 'E', then AFB is an output argument and on exit */
/*          returns details of the LU factorization of the equilibrated */
/*          matrix A (see the description of AB for the form of the */
/*          equilibrated matrix). */

/*  LDAFB   (input) INTEGER */
/*          The leading dimension of the array AFB.  LDAFB >= 2*KL+KU+1. */

/*  IPIV    (input or output) INTEGER array, dimension (N) */
/*          If FACT = 'F', then IPIV is an input argument and on entry */
/*          contains the pivot indices from the factorization A = L*U */
/*          as computed by DGBTRF; row i of the matrix was interchanged */
/*          with row IPIV(i). */

/*          If FACT = 'N', then IPIV is an output argument and on exit */
/*          contains the pivot indices from the factorization A = L*U */
/*          of the original matrix A. */

/*          If FACT = 'E', then IPIV is an output argument and on exit */
/*          contains the pivot indices from the factorization A = L*U */
/*          of the equilibrated matrix A. */

/*  EQUED   (input or output) CHARACTER*1 */
/*          Specifies the form of equilibration that was done. */
/*          = 'N':  No equilibration (always true if FACT = 'N'). */
/*          = 'R':  Row equilibration, i.e., A has been premultiplied by */
/*                  diag(R). */
/*          = 'C':  Column equilibration, i.e., A has been postmultiplied */
/*                  by diag(C). */
/*          = 'B':  Both row and column equilibration, i.e., A has been */
/*                  replaced by diag(R) * A * diag(C). */
/*          EQUED is an input argument if FACT = 'F'; otherwise, it is an */
/*          output argument. */

/*  R       (input or output) DOUBLE PRECISION array, dimension (N) */
/*          The row scale factors for A.  If EQUED = 'R' or 'B', A is */
/*          multiplied on the left by diag(R); if EQUED = 'N' or 'C', R */
/*          is not accessed.  R is an input argument if FACT = 'F'; */
/*          otherwise, R is an output argument.  If FACT = 'F' and */
/*          EQUED = 'R' or 'B', each element of R must be positive. */

/*  C       (input or output) DOUBLE PRECISION array, dimension (N) */
/*          The column scale factors for A.  If EQUED = 'C' or 'B', A is */
/*          multiplied on the right by diag(C); if EQUED = 'N' or 'R', C */
/*          is not accessed.  C is an input argument if FACT = 'F'; */
/*          otherwise, C is an output argument.  If FACT = 'F' and */
/*          EQUED = 'C' or 'B', each element of C must be positive. */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the right hand side matrix B. */
/*          On exit, */
/*          if EQUED = 'N', B is not modified; */
/*          if TRANS = 'N' and EQUED = 'R' or 'B', B is overwritten by */
/*          diag(R)*B; */
/*          if TRANS = 'T' or 'C' and EQUED = 'C' or 'B', B is */
/*          overwritten by diag(C)*B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (output) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          If INFO = 0 or INFO = N+1, the N-by-NRHS solution matrix X */
/*          to the original system of equations.  Note that A and B are */
/*          modified on exit if EQUED .ne. 'N', and the solution to the */
/*          equilibrated system is inv(diag(C))*X if TRANS = 'N' and */
/*          EQUED = 'C' or 'B', or inv(diag(R))*X if TRANS = 'T' or 'C' */
/*          and EQUED = 'R' or 'B'. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  RCOND   (output) DOUBLE PRECISION */
/*          The estimate of the reciprocal condition number of the matrix */
/*          A after equilibration (if done).  If RCOND is less than the */
/*          machine precision (in particular, if RCOND = 0), the matrix */
/*          is singular to working precision.  This condition is */
/*          indicated by a return code of INFO > 0. */

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

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (3*N) */
/*          On exit, WORK(1) contains the reciprocal pivot growth */
/*          factor norm(A)/norm(U). The "max absolute element" norm is */
/*          used. If WORK(1) is much less than 1, then the stability */
/*          of the LU factorization of the (equilibrated) matrix A */
/*          could be poor. This also means that the solution X, condition */
/*          estimator RCOND, and forward error bound FERR could be */
/*          unreliable. If factorization fails with 0<INFO<=N, then */
/*          WORK(1) contains the reciprocal pivot growth factor for the */
/*          leading INFO columns of A. */

/*  IWORK   (workspace) INTEGER array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, and i is */
/*                <= N:  U(i,i) is exactly zero.  The factorization */
/*                       has been completed, but the factor U is exactly */
/*                       singular, so the solution and error bounds */
/*                       could not be computed. RCOND = 0 is returned. */
/*                = N+1: U is nonsingular, but RCOND is less than machine */
/*                       precision, meaning that the matrix is singular */
/*                       to working precision.  Nevertheless, the */
/*                       solution and error bounds are computed because */
/*                       there are a number of situations where the */
/*                       computed solution can be more accurate than the */
/*                       value of RCOND would suggest. */

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
    afb_dim1 = *ldafb;
    afb_offset = 1 + afb_dim1;
    afb -= afb_offset;
    --ipiv;
    --r__;
    --c__;
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
    nofact = lsame_(fact, "N");
    equil = lsame_(fact, "E");
    notran = lsame_(trans, "N");
    if (nofact || equil) {
	*(unsigned char *)equed = 'N';
	rowequ = false;
	colequ = false;
    } else {
	rowequ = lsame_(equed, "R") || lsame_(equed,
		"B");
	colequ = lsame_(equed, "C") || lsame_(equed,
		"B");
	smlnum = dlamch_("Safe minimum");
	bignum = 1. / smlnum;
    }

/*     Test the input parameters. */

    if (! nofact && ! equil && ! lsame_(fact, "F")) {
	*info = -1;
    } else if (! notran && ! lsame_(trans, "T") && !
	    lsame_(trans, "C")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*kl < 0) {
	*info = -4;
    } else if (*ku < 0) {
	*info = -5;
    } else if (*nrhs < 0) {
	*info = -6;
    } else if (*ldab < *kl + *ku + 1) {
	*info = -8;
    } else if (*ldafb < (*kl << 1) + *ku + 1) {
	*info = -10;
    } else if (lsame_(fact, "F") && ! (rowequ || colequ
	    || lsame_(equed, "N"))) {
	*info = -12;
    } else {
	if (rowequ) {
	    rcmin = bignum;
	    rcmax = 0.;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		d__1 = rcmin, d__2 = r__[j];
		rcmin = std::min(d__1,d__2);
/* Computing MAX */
		d__1 = rcmax, d__2 = r__[j];
		rcmax = std::max(d__1,d__2);
/* L10: */
	    }
	    if (rcmin <= 0.) {
		*info = -13;
	    } else if (*n > 0) {
		rowcnd = std::max(rcmin,smlnum) / std::min(rcmax,bignum);
	    } else {
		rowcnd = 1.;
	    }
	}
	if (colequ && *info == 0) {
	    rcmin = bignum;
	    rcmax = 0.;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		d__1 = rcmin, d__2 = c__[j];
		rcmin = std::min(d__1,d__2);
/* Computing MAX */
		d__1 = rcmax, d__2 = c__[j];
		rcmax = std::max(d__1,d__2);
/* L20: */
	    }
	    if (rcmin <= 0.) {
		*info = -14;
	    } else if (*n > 0) {
		colcnd = std::max(rcmin,smlnum) / std::min(rcmax,bignum);
	    } else {
		colcnd = 1.;
	    }
	}
	if (*info == 0) {
	    if (*ldb < std::max(1_integer,*n)) {
		*info = -16;
	    } else if (*ldx < std::max(1_integer,*n)) {
		*info = -18;
	    }
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGBSVX", &i__1);
	return 0;
    }

    if (equil) {

/*        Compute row and column scalings to equilibrate the matrix A. */

	dgbequ_(n, n, kl, ku, &ab[ab_offset], ldab, &r__[1], &c__[1], &rowcnd,
		 &colcnd, &amax, &infequ);
	if (infequ == 0) {

/*           Equilibrate the matrix. */

	    dlaqgb_(n, n, kl, ku, &ab[ab_offset], ldab, &r__[1], &c__[1], &
		    rowcnd, &colcnd, &amax, equed);
	    rowequ = lsame_(equed, "R") || lsame_(equed,
		     "B");
	    colequ = lsame_(equed, "C") || lsame_(equed,
		     "B");
	}
    }

/*     Scale the right hand side. */

    if (notran) {
	if (rowequ) {
	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    b[i__ + j * b_dim1] = r__[i__] * b[i__ + j * b_dim1];
/* L30: */
		}
/* L40: */
	    }
	}
    } else if (colequ) {
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		b[i__ + j * b_dim1] = c__[i__] * b[i__ + j * b_dim1];
/* L50: */
	    }
/* L60: */
	}
    }

    if (nofact || equil) {

/*        Compute the LU factorization of the band matrix A. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	    i__2 = j - *ku;
	    j1 = std::max(i__2,1_integer);
/* Computing MIN */
	    i__2 = j + *kl;
	    j2 = std::min(i__2,*n);
	    i__2 = j2 - j1 + 1;
	    dcopy_(&i__2, &ab[*ku + 1 - j + j1 + j * ab_dim1], &c__1, &afb[*
		    kl + *ku + 1 - j + j1 + j * afb_dim1], &c__1);
/* L70: */
	}

	dgbtrf_(n, n, kl, ku, &afb[afb_offset], ldafb, &ipiv[1], info);

/*        Return if INFO is non-zero. */

	if (*info > 0) {

/*           Compute the reciprocal pivot growth factor of the */
/*           leading rank-deficient INFO columns of A. */

	    anorm = 0.;
	    i__1 = *info;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
		i__2 = *ku + 2 - j;
/* Computing MIN */
		i__4 = *n + *ku + 1 - j, i__5 = *kl + *ku + 1;
		i__3 = std::min(i__4,i__5);
		for (i__ = std::max(i__2,1_integer); i__ <= i__3; ++i__) {
/* Computing MAX */
		    d__2 = anorm, d__3 = (d__1 = ab[i__ + j * ab_dim1], abs(
			    d__1));
		    anorm = std::max(d__2,d__3);
/* L80: */
		}
/* L90: */
	    }
/* Computing MIN */
	    i__3 = *info - 1, i__2 = *kl + *ku;
	    i__1 = std::min(i__3,i__2);
/* Computing MAX */
	    i__4 = 1, i__5 = *kl + *ku + 2 - *info;
	    rpvgrw = dlantb_("M", "U", "N", info, &i__1, &afb[std::max(i__4, i__5)
		    + afb_dim1], ldafb, &work[1]);
	    if (rpvgrw == 0.) {
		rpvgrw = 1.;
	    } else {
		rpvgrw = anorm / rpvgrw;
	    }
	    work[1] = rpvgrw;
	    *rcond = 0.;
	    return 0;
	}
    }

/*     Compute the norm of the matrix A and the */
/*     reciprocal pivot growth factor RPVGRW. */

    if (notran) {
	*(unsigned char *)norm = '1';
    } else {
	*(unsigned char *)norm = 'I';
    }
    anorm = dlangb_(norm, n, kl, ku, &ab[ab_offset], ldab, &work[1]);
    i__1 = *kl + *ku;
    rpvgrw = dlantb_("M", "U", "N", n, &i__1, &afb[afb_offset], ldafb, &work[
	    1]);
    if (rpvgrw == 0.) {
	rpvgrw = 1.;
    } else {
	rpvgrw = dlangb_("M", n, kl, ku, &ab[ab_offset], ldab, &work[1]) / rpvgrw;
    }

/*     Compute the reciprocal of the condition number of A. */

    dgbcon_(norm, n, kl, ku, &afb[afb_offset], ldafb, &ipiv[1], &anorm, rcond,
	     &work[1], &iwork[1], info);

/*     Compute the solution matrix X. */

    dlacpy_("Full", n, nrhs, &b[b_offset], ldb, &x[x_offset], ldx);
    dgbtrs_(trans, n, kl, ku, nrhs, &afb[afb_offset], ldafb, &ipiv[1], &x[
	    x_offset], ldx, info);

/*     Use iterative refinement to improve the computed solution and */
/*     compute error bounds and backward error estimates for it. */

    dgbrfs_(trans, n, kl, ku, nrhs, &ab[ab_offset], ldab, &afb[afb_offset],
	    ldafb, &ipiv[1], &b[b_offset], ldb, &x[x_offset], ldx, &ferr[1], &
	    berr[1], &work[1], &iwork[1], info);

/*     Transform the solution matrix X to a solution of the original */
/*     system. */

    if (notran) {
	if (colequ) {
	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		i__3 = *n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    x[i__ + j * x_dim1] = c__[i__] * x[i__ + j * x_dim1];
/* L100: */
		}
/* L110: */
	    }
	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		ferr[j] /= colcnd;
/* L120: */
	    }
	}
    } else if (rowequ) {
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    i__3 = *n;
	    for (i__ = 1; i__ <= i__3; ++i__) {
		x[i__ + j * x_dim1] = r__[i__] * x[i__ + j * x_dim1];
/* L130: */
	    }
/* L140: */
	}
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    ferr[j] /= rowcnd;
/* L150: */
	}
    }

/*     Set INFO = N+1 if the matrix is singular to working precision. */

    if (*rcond < dlamch_("Epsilon")) {
	*info = *n + 1;
    }

    work[1] = rpvgrw;
    return 0;

/*     End of DGBSVX */

} /* dgbsvx_ */

#if 0
/* Subroutine */ int dgbsvxx_(const char *fact, const char *trans, integer *n, integer *
	kl, integer *ku, integer *nrhs, double *ab, integer *ldab,
	double *afb, integer *ldafb, integer *ipiv, char *equed,
	double *r__, double *c__, double *b, integer *ldb,
	double *x, integer *ldx, double *rcond, double *rpvgrw,
	double *berr, integer *n_err_bnds__, double *err_bnds_norm__,
	double *err_bnds_comp__, integer *nparams, double *params,
	double *work, integer *iwork, integer *info)
{
    /* System generated locals */
    integer ab_dim1, ab_offset, afb_dim1, afb_offset, b_dim1, b_offset,
	    x_dim1, x_offset, err_bnds_norm_dim1, err_bnds_norm_offset,
	    err_bnds_comp_dim1, err_bnds_comp_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j;
    double amax;
    double rcmin, rcmax;
    bool equil;
    double colcnd;
    bool nofact;
    double bignum;
     integer infequ;
    bool colequ;
    double rowcnd;
    bool notran;
    double smlnum;
    bool rowequ;

/*     -- LAPACK driver routine (version 3.2)                          -- */
/*     -- Contributed by James Demmel, Deaglan Halligan, Yozo Hida and -- */
/*     -- Jason Riedy of Univ. of California Berkeley.                 -- */
/*     -- November 2008                                                -- */

/*     -- LAPACK is a software package provided by Univ. of Tennessee, -- */
/*     -- Univ. of California Berkeley and NAG Ltd.                    -- */

/*     .. */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*     Purpose */
/*     ======= */

/*     DGBSVXX uses the LU factorization to compute the solution to a */
/*     double precision system of linear equations  A * X = B,  where A is an */
/*     N-by-N matrix and X and B are N-by-NRHS matrices. */

/*     If requested, both normwise and maximum componentwise error bounds */
/*     are returned. DGBSVXX will return a solution with a tiny */
/*     guaranteed error (O(eps) where eps is the working machine */
/*     precision) unless the matrix is very ill-conditioned, in which */
/*     case a warning is returned. Relevant condition numbers also are */
/*     calculated and returned. */

/*     DGBSVXX accepts user-provided factorizations and equilibration */
/*     factors; see the definitions of the FACT and EQUED options. */
/*     Solving with refinement and using a factorization from a previous */
/*     DGBSVXX call will also produce a solution with either O(eps) */
/*     errors or warnings, but we cannot make that claim for general */
/*     user-provided factorizations and equilibration factors if they */
/*     differ from what DGBSVXX would itself produce. */

/*     Description */
/*     =========== */

/*     The following steps are performed: */

/*     1. If FACT = 'E', double precision scaling factors are computed to equilibrate */
/*     the system: */

/*       TRANS = 'N':  diag(R)*A*diag(C)     *inv(diag(C))*X = diag(R)*B */
/*       TRANS = 'T': (diag(R)*A*diag(C))**T *inv(diag(R))*X = diag(C)*B */
/*       TRANS = 'C': (diag(R)*A*diag(C))**H *inv(diag(R))*X = diag(C)*B */

/*     Whether or not the system will be equilibrated depends on the */
/*     scaling of the matrix A, but if equilibration is used, A is */
/*     overwritten by diag(R)*A*diag(C) and B by diag(R)*B (if TRANS='N') */
/*     or diag(C)*B (if TRANS = 'T' or 'C'). */

/*     2. If FACT = 'N' or 'E', the LU decomposition is used to factor */
/*     the matrix A (after equilibration if FACT = 'E') as */

/*       A = P * L * U, */

/*     where P is a permutation matrix, L is a unit lower triangular */
/*     matrix, and U is upper triangular. */

/*     3. If some U(i,i)=0, so that U is exactly singular, then the */
/*     routine returns with INFO = i. Otherwise, the factored form of A */
/*     is used to estimate the condition number of the matrix A (see */
/*     argument RCOND). If the reciprocal of the condition number is less */
/*     than machine precision, the routine still goes on to solve for X */
/*     and compute error bounds as described below. */

/*     4. The system of equations is solved for X using the factored form */
/*     of A. */

/*     5. By default (unless PARAMS(LA_LINRX_ITREF_I) is set to zero), */
/*     the routine will use iterative refinement to try to get a small */
/*     error and error bounds.  Refinement calculates the residual to at */
/*     least twice the working precision. */

/*     6. If equilibration was used, the matrix X is premultiplied by */
/*     diag(C) (if TRANS = 'N') or diag(R) (if TRANS = 'T' or 'C') so */
/*     that it solves the original system before equilibration. */

/*     Arguments */
/*     ========= */

/*     Some optional parameters are bundled in the PARAMS array.  These */
/*     settings determine how refinement is performed, but often the */
/*     defaults are acceptable.  If the defaults are acceptable, users */
/*     can pass NPARAMS = 0 which prevents the source code from accessing */
/*     the PARAMS argument. */

/*     FACT    (input) CHARACTER*1 */
/*     Specifies whether or not the factored form of the matrix A is */
/*     supplied on entry, and if not, whether the matrix A should be */
/*     equilibrated before it is factored. */
/*       = 'F':  On entry, AF and IPIV contain the factored form of A. */
/*               If EQUED is not 'N', the matrix A has been */
/*               equilibrated with scaling factors given by R and C. */
/*               A, AF, and IPIV are not modified. */
/*       = 'N':  The matrix A will be copied to AF and factored. */
/*       = 'E':  The matrix A will be equilibrated if necessary, then */
/*               copied to AF and factored. */

/*     TRANS   (input) CHARACTER*1 */
/*     Specifies the form of the system of equations: */
/*       = 'N':  A * X = B     (No transpose) */
/*       = 'T':  A**T * X = B  (Transpose) */
/*       = 'C':  A**H * X = B  (Conjugate Transpose = Transpose) */

/*     N       (input) INTEGER */
/*     The number of linear equations, i.e., the order of the */
/*     matrix A.  N >= 0. */

/*     KL      (input) INTEGER */
/*     The number of subdiagonals within the band of A.  KL >= 0. */

/*     KU      (input) INTEGER */
/*     The number of superdiagonals within the band of A.  KU >= 0. */

/*     NRHS    (input) INTEGER */
/*     The number of right hand sides, i.e., the number of columns */
/*     of the matrices B and X.  NRHS >= 0. */

/*     AB      (input/output) DOUBLE PRECISION array, dimension (LDAB,N) */
/*     On entry, the matrix A in band storage, in rows 1 to KL+KU+1. */
/*     The j-th column of A is stored in the j-th column of the */
/*     array AB as follows: */
/*     AB(KU+1+i-j,j) = A(i,j) for max(1,j-KU)<=i<=min(N,j+kl) */

/*     If FACT = 'F' and EQUED is not 'N', then AB must have been */
/*     equilibrated by the scaling factors in R and/or C.  AB is not */
/*     modified if FACT = 'F' or 'N', or if FACT = 'E' and */
/*     EQUED = 'N' on exit. */

/*     On exit, if EQUED .ne. 'N', A is scaled as follows: */
/*     EQUED = 'R':  A := diag(R) * A */
/*     EQUED = 'C':  A := A * diag(C) */
/*     EQUED = 'B':  A := diag(R) * A * diag(C). */

/*     LDAB    (input) INTEGER */
/*     The leading dimension of the array AB.  LDAB >= KL+KU+1. */

/*     AFB     (input or output) DOUBLE PRECISION array, dimension (LDAFB,N) */
/*     If FACT = 'F', then AFB is an input argument and on entry */
/*     contains details of the LU factorization of the band matrix */
/*     A, as computed by DGBTRF.  U is stored as an upper triangular */
/*     band matrix with KL+KU superdiagonals in rows 1 to KL+KU+1, */
/*     and the multipliers used during the factorization are stored */
/*     in rows KL+KU+2 to 2*KL+KU+1.  If EQUED .ne. 'N', then AFB is */
/*     the factored form of the equilibrated matrix A. */

/*     If FACT = 'N', then AF is an output argument and on exit */
/*     returns the factors L and U from the factorization A = P*L*U */
/*     of the original matrix A. */

/*     If FACT = 'E', then AF is an output argument and on exit */
/*     returns the factors L and U from the factorization A = P*L*U */
/*     of the equilibrated matrix A (see the description of A for */
/*     the form of the equilibrated matrix). */

/*     LDAFB   (input) INTEGER */
/*     The leading dimension of the array AFB.  LDAFB >= 2*KL+KU+1. */

/*     IPIV    (input or output) INTEGER array, dimension (N) */
/*     If FACT = 'F', then IPIV is an input argument and on entry */
/*     contains the pivot indices from the factorization A = P*L*U */
/*     as computed by DGETRF; row i of the matrix was interchanged */
/*     with row IPIV(i). */

/*     If FACT = 'N', then IPIV is an output argument and on exit */
/*     contains the pivot indices from the factorization A = P*L*U */
/*     of the original matrix A. */

/*     If FACT = 'E', then IPIV is an output argument and on exit */
/*     contains the pivot indices from the factorization A = P*L*U */
/*     of the equilibrated matrix A. */

/*     EQUED   (input or output) CHARACTER*1 */
/*     Specifies the form of equilibration that was done. */
/*       = 'N':  No equilibration (always true if FACT = 'N'). */
/*       = 'R':  Row equilibration, i.e., A has been premultiplied by */
/*               diag(R). */
/*       = 'C':  Column equilibration, i.e., A has been postmultiplied */
/*               by diag(C). */
/*       = 'B':  Both row and column equilibration, i.e., A has been */
/*               replaced by diag(R) * A * diag(C). */
/*     EQUED is an input argument if FACT = 'F'; otherwise, it is an */
/*     output argument. */

/*     R       (input or output) DOUBLE PRECISION array, dimension (N) */
/*     The row scale factors for A.  If EQUED = 'R' or 'B', A is */
/*     multiplied on the left by diag(R); if EQUED = 'N' or 'C', R */
/*     is not accessed.  R is an input argument if FACT = 'F'; */
/*     otherwise, R is an output argument.  If FACT = 'F' and */
/*     EQUED = 'R' or 'B', each element of R must be positive. */
/*     If R is output, each element of R is a power of the radix. */
/*     If R is input, each element of R should be a power of the radix */
/*     to ensure a reliable solution and error estimates. Scaling by */
/*     powers of the radix does not cause rounding errors unless the */
/*     result underflows or overflows. Rounding errors during scaling */
/*     lead to refining with a matrix that is not equivalent to the */
/*     input matrix, producing error estimates that may not be */
/*     reliable. */

/*     C       (input or output) DOUBLE PRECISION array, dimension (N) */
/*     The column scale factors for A.  If EQUED = 'C' or 'B', A is */
/*     multiplied on the right by diag(C); if EQUED = 'N' or 'R', C */
/*     is not accessed.  C is an input argument if FACT = 'F'; */
/*     otherwise, C is an output argument.  If FACT = 'F' and */
/*     EQUED = 'C' or 'B', each element of C must be positive. */
/*     If C is output, each element of C is a power of the radix. */
/*     If C is input, each element of C should be a power of the radix */
/*     to ensure a reliable solution and error estimates. Scaling by */
/*     powers of the radix does not cause rounding errors unless the */
/*     result underflows or overflows. Rounding errors during scaling */
/*     lead to refining with a matrix that is not equivalent to the */
/*     input matrix, producing error estimates that may not be */
/*     reliable. */

/*     B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*     On entry, the N-by-NRHS right hand side matrix B. */
/*     On exit, */
/*     if EQUED = 'N', B is not modified; */
/*     if TRANS = 'N' and EQUED = 'R' or 'B', B is overwritten by */
/*        diag(R)*B; */
/*     if TRANS = 'T' or 'C' and EQUED = 'C' or 'B', B is */
/*        overwritten by diag(C)*B. */

/*     LDB     (input) INTEGER */
/*     The leading dimension of the array B.  LDB >= max(1,N). */

/*     X       (output) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*     If INFO = 0, the N-by-NRHS solution matrix X to the original */
/*     system of equations.  Note that A and B are modified on exit */
/*     if EQUED .ne. 'N', and the solution to the equilibrated system is */
/*     inv(diag(C))*X if TRANS = 'N' and EQUED = 'C' or 'B', or */
/*     inv(diag(R))*X if TRANS = 'T' or 'C' and EQUED = 'R' or 'B'. */

/*     LDX     (input) INTEGER */
/*     The leading dimension of the array X.  LDX >= max(1,N). */

/*     RCOND   (output) DOUBLE PRECISION */
/*     Reciprocal scaled condition number.  This is an estimate of the */
/*     reciprocal Skeel condition number of the matrix A after */
/*     equilibration (if done).  If this is less than the machine */
/*     precision (in particular, if it is zero), the matrix is singular */
/*     to working precision.  Note that the error may still be small even */
/*     if this number is very small and the matrix appears ill- */
/*     conditioned. */

/*     RPVGRW  (output) DOUBLE PRECISION */
/*     Reciprocal pivot growth.  On exit, this contains the reciprocal */
/*     pivot growth factor norm(A)/norm(U). The "max absolute element" */
/*     norm is used.  If this is much less than 1, then the stability of */
/*     the LU factorization of the (equilibrated) matrix A could be poor. */
/*     This also means that the solution X, estimated condition numbers, */
/*     and error bounds could be unreliable. If factorization fails with */
/*     0<INFO<=N, then this contains the reciprocal pivot growth factor */
/*     for the leading INFO columns of A.  In DGESVX, this quantity is */
/*     returned in WORK(1). */

/*     BERR    (output) DOUBLE PRECISION array, dimension (NRHS) */
/*     Componentwise relative backward error.  This is the */
/*     componentwise relative backward error of each solution vector X(j) */
/*     (i.e., the smallest relative change in any element of A or B that */
/*     makes X(j) an exact solution). */

/*     N_ERR_BNDS (input) INTEGER */
/*     Number of error bounds to return for each right hand side */
/*     and each type (normwise or componentwise).  See ERR_BNDS_NORM and */
/*     ERR_BNDS_COMP below. */

/*     ERR_BNDS_NORM  (output) DOUBLE PRECISION array, dimension (NRHS, N_ERR_BNDS) */
/*     For each right-hand side, this array contains information about */
/*     various error bounds and condition numbers corresponding to the */
/*     normwise relative error, which is defined as follows: */

/*     Normwise relative error in the ith solution vector: */
/*             max_j (abs(XTRUE(j,i) - X(j,i))) */
/*            ------------------------------ */
/*                  max_j abs(X(j,i)) */

/*     The array is indexed by the type of error information as described */
/*     below. There currently are up to three pieces of information */
/*     returned. */

/*     The first index in ERR_BNDS_NORM(i,:) corresponds to the ith */
/*     right-hand side. */

/*     The second index in ERR_BNDS_NORM(:,err) contains the following */
/*     three fields: */
/*     err = 1 "Trust/don't trust" boolean. Trust the answer if the */
/*              reciprocal condition number is less than the threshold */
/*              sqrt(n) * dlamch('Epsilon'). */

/*     err = 2 "Guaranteed" error bound: The estimated forward error, */
/*              almost certainly within a factor of 10 of the true error */
/*              so long as the next entry is greater than the threshold */
/*              sqrt(n) * dlamch('Epsilon'). This error bound should only */
/*              be trusted if the previous boolean is true. */

/*     err = 3  Reciprocal condition number: Estimated normwise */
/*              reciprocal condition number.  Compared with the threshold */
/*              sqrt(n) * dlamch('Epsilon') to determine if the error */
/*              estimate is "guaranteed". These reciprocal condition */
/*              numbers are 1 / (norm(Z^{-1},inf) * norm(Z,inf)) for some */
/*              appropriately scaled matrix Z. */
/*              Let Z = S*A, where S scales each row by a power of the */
/*              radix so all absolute row sums of Z are approximately 1. */

/*     See Lapack Working Note 165 for further details and extra */
/*     cautions. */

/*     ERR_BNDS_COMP  (output) DOUBLE PRECISION array, dimension (NRHS, N_ERR_BNDS) */
/*     For each right-hand side, this array contains information about */
/*     various error bounds and condition numbers corresponding to the */
/*     componentwise relative error, which is defined as follows: */

/*     Componentwise relative error in the ith solution vector: */
/*                    abs(XTRUE(j,i) - X(j,i)) */
/*             max_j ---------------------- */
/*                         abs(X(j,i)) */

/*     The array is indexed by the right-hand side i (on which the */
/*     componentwise relative error depends), and the type of error */
/*     information as described below. There currently are up to three */
/*     pieces of information returned for each right-hand side. If */
/*     componentwise accuracy is not requested (PARAMS(3) = 0.0), then */
/*     ERR_BNDS_COMP is not accessed.  If N_ERR_BNDS .LT. 3, then at most */
/*     the first (:,N_ERR_BNDS) entries are returned. */

/*     The first index in ERR_BNDS_COMP(i,:) corresponds to the ith */
/*     right-hand side. */

/*     The second index in ERR_BNDS_COMP(:,err) contains the following */
/*     three fields: */
/*     err = 1 "Trust/don't trust" boolean. Trust the answer if the */
/*              reciprocal condition number is less than the threshold */
/*              sqrt(n) * dlamch('Epsilon'). */

/*     err = 2 "Guaranteed" error bound: The estimated forward error, */
/*              almost certainly within a factor of 10 of the true error */
/*              so long as the next entry is greater than the threshold */
/*              sqrt(n) * dlamch('Epsilon'). This error bound should only */
/*              be trusted if the previous boolean is true. */

/*     err = 3  Reciprocal condition number: Estimated componentwise */
/*              reciprocal condition number.  Compared with the threshold */
/*              sqrt(n) * dlamch('Epsilon') to determine if the error */
/*              estimate is "guaranteed". These reciprocal condition */
/*              numbers are 1 / (norm(Z^{-1},inf) * norm(Z,inf)) for some */
/*              appropriately scaled matrix Z. */
/*              Let Z = S*(A*diag(x)), where x is the solution for the */
/*              current right-hand side and S scales each row of */
/*              A*diag(x) by a power of the radix so all absolute row */
/*              sums of Z are approximately 1. */

/*     See Lapack Working Note 165 for further details and extra */
/*     cautions. */

/*     NPARAMS (input) INTEGER */
/*     Specifies the number of parameters set in PARAMS.  If .LE. 0, the */
/*     PARAMS array is never referenced and default values are used. */

/*     PARAMS  (input / output) DOUBLE PRECISION array, dimension NPARAMS */
/*     Specifies algorithm parameters.  If an entry is .LT. 0.0, then */
/*     that entry will be filled with default value used for that */
/*     parameter.  Only positions up to NPARAMS are accessed; defaults */
/*     are used for higher-numbered parameters. */

/*       PARAMS(LA_LINRX_ITREF_I = 1) : Whether to perform iterative */
/*            refinement or not. */
/*         Default: 1.0D+0 */
/*            = 0.0 : No refinement is performed, and no error bounds are */
/*                    computed. */
/*            = 1.0 : Use the extra-precise refinement algorithm. */
/*              (other values are reserved for future use) */

/*       PARAMS(LA_LINRX_ITHRESH_I = 2) : Maximum number of residual */
/*            computations allowed for refinement. */
/*         Default: 10 */
/*         Aggressive: Set to 100 to permit convergence using approximate */
/*                     factorizations or factorizations other than LU. If */
/*                     the factorization uses a technique other than */
/*                     Gaussian elimination, the guarantees in */
/*                     err_bnds_norm and err_bnds_comp may no longer be */
/*                     trustworthy. */

/*       PARAMS(LA_LINRX_CWISE_I = 3) : Flag determining if the code */
/*            will attempt to find a solution with small componentwise */
/*            relative error in the double-precision algorithm.  Positive */
/*            is true, 0.0 is false. */
/*         Default: 1.0 (attempt componentwise convergence) */

/*     WORK    (workspace) DOUBLE PRECISION array, dimension (4*N) */

/*     IWORK   (workspace) INTEGER array, dimension (N) */

/*     INFO    (output) INTEGER */
/*       = 0:  Successful exit. The solution to every right-hand side is */
/*         guaranteed. */
/*       < 0:  If INFO = -i, the i-th argument had an illegal value */
/*       > 0 and <= N:  U(INFO,INFO) is exactly zero.  The factorization */
/*         has been completed, but the factor U is exactly singular, so */
/*         the solution and error bounds could not be computed. RCOND = 0 */
/*         is returned. */
/*       = N+J: The solution corresponding to the Jth right-hand side is */
/*         not guaranteed. The solutions corresponding to other right- */
/*         hand sides K with K > J may not be guaranteed as well, but */
/*         only the first such right-hand side is reported. If a small */
/*         componentwise error is not requested (PARAMS(3) = 0.0) then */
/*         the Jth right-hand side is the first with a normwise error */
/*         bound that is not guaranteed (the smallest J such */
/*         that ERR_BNDS_NORM(J,1) = 0.0). By default (PARAMS(3) = 1.0) */
/*         the Jth right-hand side is the first with either a normwise or */
/*         componentwise error bound that is not guaranteed (the smallest */
/*         J such that either ERR_BNDS_NORM(J,1) = 0.0 or */
/*         ERR_BNDS_COMP(J,1) = 0.0). See the definition of */
/*         ERR_BNDS_NORM(:,1) and ERR_BNDS_COMP(:,1). To get information */
/*         about all of the right-hand sides check ERR_BNDS_NORM or */
/*         ERR_BNDS_COMP. */

/*     ================================================================== */

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
    err_bnds_comp_dim1 = *nrhs;
    err_bnds_comp_offset = 1 + err_bnds_comp_dim1;
    err_bnds_comp__ -= err_bnds_comp_offset;
    err_bnds_norm_dim1 = *nrhs;
    err_bnds_norm_offset = 1 + err_bnds_norm_dim1;
    err_bnds_norm__ -= err_bnds_norm_offset;
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    afb_dim1 = *ldafb;
    afb_offset = 1 + afb_dim1;
    afb -= afb_offset;
    --ipiv;
    --r__;
    --c__;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    --berr;
    --params;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    nofact = lsame_(fact, "N");
    equil = lsame_(fact, "E");
    notran = lsame_(trans, "N");
    smlnum = dlamch_("Safe minimum");
    bignum = 1. / smlnum;
    if (nofact || equil) {
	*(unsigned char *)equed = 'N';
	rowequ = false;
	colequ = false;
    } else {
	rowequ = lsame_(equed, "R") || lsame_(equed, "B");
	colequ = lsame_(equed, "C") || lsame_(equed, "B");
    }

/*     Default is failure.  If an input parameter is wrong or */
/*     factorization fails, make everything look horrible.  Only the */
/*     pivot growth is set here, the rest is initialized in DGBRFSX. */

    *rpvgrw = 0.;

/*     Test the input parameters.  PARAMS is not tested until DGBRFSX. */

    if (! nofact && ! equil && ! lsame_(fact, "F")) {
	*info = -1;
    } else if (! notran && ! lsame_(trans, "T") && !
	    lsame_(trans, "C")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*kl < 0) {
	*info = -4;
    } else if (*ku < 0) {
	*info = -5;
    } else if (*nrhs < 0) {
	*info = -6;
    } else if (*ldab < *kl + *ku + 1) {
	*info = -8;
    } else if (*ldafb < (*kl << 1) + *ku + 1) {
	*info = -10;
    } else if (lsame_(fact, "F") && ! (rowequ || colequ
	    || lsame_(equed, "N"))) {
	*info = -12;
    } else {
	if (rowequ) {
	    rcmin = bignum;
	    rcmax = 0.;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		d__1 = rcmin, d__2 = r__[j];
		rcmin = std::min(d__1,d__2);
/* Computing MAX */
		d__1 = rcmax, d__2 = r__[j];
		rcmax = std::max(d__1,d__2);
/* L10: */
	    }
	    if (rcmin <= 0.) {
		*info = -13;
	    } else if (*n > 0) {
		rowcnd = std::max(rcmin,smlnum) / std::min(rcmax,bignum);
	    } else {
		rowcnd = 1.;
	    }
	}
	if (colequ && *info == 0) {
	    rcmin = bignum;
	    rcmax = 0.;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		d__1 = rcmin, d__2 = c__[j];
		rcmin = std::min(d__1,d__2);
/* Computing MAX */
		d__1 = rcmax, d__2 = c__[j];
		rcmax = std::max(d__1,d__2);
/* L20: */
	    }
	    if (rcmin <= 0.) {
		*info = -14;
	    } else if (*n > 0) {
		colcnd = std::max(rcmin,smlnum) / std::min(rcmax,bignum);
	    } else {
		colcnd = 1.;
	    }
	}
	if (*info == 0) {
	    if (*ldb < std::max(1_integer,*n)) {
		*info = -15;
	    } else if (*ldx < std::max(1_integer,*n)) {
		*info = -16;
	    }
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGBSVXX", &i__1);
	return 0;
    }

    if (equil) {

/*     Compute row and column scalings to equilibrate the matrix A. */

	dgbequb_(n, n, kl, ku, &ab[ab_offset], ldab, &r__[1], &c__[1], &
		rowcnd, &colcnd, &amax, &infequ);
	if (infequ == 0) {

/*     Equilibrate the matrix. */

	    dlaqgb_(n, n, kl, ku, &ab[ab_offset], ldab, &r__[1], &c__[1], &
		    rowcnd, &colcnd, &amax, equed);
	    rowequ = lsame_(equed, "R") || lsame_(equed,
		     "B");
	    colequ = lsame_(equed, "C") || lsame_(equed,
		     "B");
	}

/*     If the scaling factors are not applied, set them to 1.0. */

	if (! rowequ) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		r__[j] = 1.;
	    }
	}
	if (! colequ) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		c__[j] = 1.;
	    }
	}
    }

/*     Scale the right hand side. */

    if (notran) {
	if (rowequ) {
	    dlascl2_(n, nrhs, &r__[1], &b[b_offset], ldb);
	}
    } else {
	if (colequ) {
	    dlascl2_(n, nrhs, &c__[1], &b[b_offset], ldb);
	}
    }

    if (nofact || equil) {

/*        Compute the LU factorization of A. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = (*kl << 1) + *ku + 1;
	    for (i__ = *kl + 1; i__ <= i__2; ++i__) {
		afb[i__ + j * afb_dim1] = ab[i__ - *kl + j * ab_dim1];
/* L30: */
	    }
/* L40: */
	}
	dgbtrf_(n, n, kl, ku, &afb[afb_offset], ldafb, &ipiv[1], info);

/*        Return if INFO is non-zero. */

	if (*info > 0) {

/*           Pivot in column INFO is exactly 0 */
/*           Compute the reciprocal pivot growth factor of the */
/*           leading rank-deficient INFO columns of A. */

	    *rpvgrw = dla_gbrpvgrw__(n, kl, ku, info, &ab[ab_offset], ldab, &
		    afb[afb_offset], ldafb);
	    return 0;
	}
    }

/*     Compute the reciprocal pivot growth factor RPVGRW. */

    *rpvgrw = dla_gbrpvgrw__(n, kl, ku, n, &ab[ab_offset], ldab, &afb[
	    afb_offset], ldafb);

/*     Compute the solution matrix X. */

    dlacpy_("Full", n, nrhs, &b[b_offset], ldb, &x[x_offset], ldx);
    dgbtrs_(trans, n, kl, ku, nrhs, &afb[afb_offset], ldafb, &ipiv[1], &x[
	    x_offset], ldx, info);

/*     Use iterative refinement to improve the computed solution and */
/*     compute error bounds and backward error estimates for it. */

    dgbrfsx_(trans, equed, n, kl, ku, nrhs, &ab[ab_offset], ldab, &afb[
	    afb_offset], ldafb, &ipiv[1], &r__[1], &c__[1], &b[b_offset], ldb,
	     &x[x_offset], ldx, rcond, &berr[1], n_err_bnds__, &
	    err_bnds_norm__[err_bnds_norm_offset], &err_bnds_comp__[
	    err_bnds_comp_offset], nparams, &params[1], &work[1], &iwork[1],
	    info);

/*     Scale solutions. */

    if (colequ && notran) {
	dlascl2_(n, nrhs, &c__[1], &x[x_offset], ldx);
    } else if (rowequ && ! notran) {
	dlascl2_(n, nrhs, &r__[1], &x[x_offset], ldx);
    }

    return 0;

/*     End of DGBSVXX */

} /* dgbsvxx_ */
#endif

/* Subroutine */ int dgbtf2_(integer *m, integer *n, integer *kl, integer *ku,
	double *ab, integer *ldab, integer *ipiv, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b9 = -1.;

    /* System generated locals */
    integer ab_dim1, ab_offset, i__1, i__2, i__3, i__4;
    double d__1;

    /* Local variables */
    integer i__, j, km, jp, ju, kv;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGBTF2 computes an LU factorization of a real m-by-n band matrix A */
/*  using partial pivoting with row interchanges. */

/*  This is the unblocked version of the algorithm, calling Level 2 BLAS. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  KL      (input) INTEGER */
/*          The number of subdiagonals within the band of A.  KL >= 0. */

/*  KU      (input) INTEGER */
/*          The number of superdiagonals within the band of A.  KU >= 0. */

/*  AB      (input/output) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          On entry, the matrix A in band storage, in rows KL+1 to */
/*          2*KL+KU+1; rows 1 to KL of the array need not be set. */
/*          The j-th column of A is stored in the j-th column of the */
/*          array AB as follows: */
/*          AB(kl+ku+1+i-j,j) = A(i,j) for max(1,j-ku)<=i<=min(m,j+kl) */

/*          On exit, details of the factorization: U is stored as an */
/*          upper triangular band matrix with KL+KU superdiagonals in */
/*          rows 1 to KL+KU+1, and the multipliers used during the */
/*          factorization are stored in rows KL+KU+2 to 2*KL+KU+1. */
/*          See below for further details. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= 2*KL+KU+1. */

/*  IPIV    (output) INTEGER array, dimension (min(M,N)) */
/*          The pivot indices; for 1 <= i <= min(M,N), row i of the */
/*          matrix was interchanged with row IPIV(i). */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          > 0: if INFO = +i, U(i,i) is exactly zero. The factorization */
/*               has been completed, but the factor U is exactly */
/*               singular, and division by zero will occur if it is used */
/*               to solve a system of equations. */

/*  Further Details */
/*  =============== */

/*  The band storage scheme is illustrated by the following example, when */
/*  M = N = 6, KL = 2, KU = 1: */

/*  On entry:                       On exit: */

/*      *    *    *    +    +    +       *    *    *   u14  u25  u36 */
/*      *    *    +    +    +    +       *    *   u13  u24  u35  u46 */
/*      *   a12  a23  a34  a45  a56      *   u12  u23  u34  u45  u56 */
/*     a11  a22  a33  a44  a55  a66     u11  u22  u33  u44  u55  u66 */
/*     a21  a32  a43  a54  a65   *      m21  m32  m43  m54  m65   * */
/*     a31  a42  a53  a64   *    *      m31  m42  m53  m64   *    * */

/*  Array elements marked * are not used by the routine; elements marked */
/*  + need not be set on entry, but are required by the routine to store */
/*  elements of U, because of fill-in resulting from the row */
/*  interchanges. */

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

/*     KV is the number of superdiagonals in the factor U, allowing for */
/*     fill-in. */

    /* Parameter adjustments */
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    --ipiv;

    /* Function Body */
    kv = *ku + *kl;

/*     Test the input parameters. */

    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*kl < 0) {
	*info = -3;
    } else if (*ku < 0) {
	*info = -4;
    } else if (*ldab < *kl + kv + 1) {
	*info = -6;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGBTF2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	return 0;
    }

/*     Gaussian elimination with partial pivoting */

/*     Set fill-in elements in columns KU+2 to KV to zero. */

    i__1 = std::min(kv,*n);
    for (j = *ku + 2; j <= i__1; ++j) {
	i__2 = *kl;
	for (i__ = kv - j + 2; i__ <= i__2; ++i__) {
	    ab[i__ + j * ab_dim1] = 0.;
/* L10: */
	}
/* L20: */
    }

/*     JU is the index of the last column affected by the current stage */
/*     of the factorization. */

    ju = 1;

    i__1 = std::min(*m,*n);
    for (j = 1; j <= i__1; ++j) {

/*        Set fill-in elements in column J+KV to zero. */

	if (j + kv <= *n) {
	    i__2 = *kl;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		ab[i__ + (j + kv) * ab_dim1] = 0.;
/* L30: */
	    }
	}

/*        Find pivot and test for singularity. KM is the number of */
/*        subdiagonal elements in the current column. */

/* Computing MIN */
	i__2 = *kl, i__3 = *m - j;
	km = std::min(i__2,i__3);
	i__2 = km + 1;
	jp = idamax_(&i__2, &ab[kv + 1 + j * ab_dim1], &c__1);
	ipiv[j] = jp + j - 1;
	if (ab[kv + jp + j * ab_dim1] != 0.) {
/* Computing MAX */
/* Computing MIN */
	    i__4 = j + *ku + jp - 1;
	    i__2 = ju, i__3 = std::min(i__4,*n);
	    ju = std::max(i__2,i__3);

/*           Apply interchange to columns J to JU. */

	    if (jp != 1) {
		i__2 = ju - j + 1;
		i__3 = *ldab - 1;
		i__4 = *ldab - 1;
		dswap_(&i__2, &ab[kv + jp + j * ab_dim1], &i__3, &ab[kv + 1 +
			j * ab_dim1], &i__4);
	    }

	    if (km > 0) {

/*              Compute multipliers. */

		d__1 = 1. / ab[kv + 1 + j * ab_dim1];
		dscal_(&km, &d__1, &ab[kv + 2 + j * ab_dim1], &c__1);

/*              Update trailing submatrix within the band. */

		if (ju > j) {
		    i__2 = ju - j;
		    i__3 = *ldab - 1;
		    i__4 = *ldab - 1;
		    dger_(&km, &i__2, &c_b9, &ab[kv + 2 + j * ab_dim1], &c__1,
			     &ab[kv + (j + 1) * ab_dim1], &i__3, &ab[kv + 1 +
			    (j + 1) * ab_dim1], &i__4);
		}
	    }
	} else {

/*           If pivot is zero, set INFO to the index of the pivot */
/*           unless a zero pivot has already been found. */

	    if (*info == 0) {
		*info = j;
	    }
	}
/* L40: */
    }
    return 0;

/*     End of DGBTF2 */

} /* dgbtf2_ */

/* Subroutine */ int dgbtrf_(integer *m, integer *n, integer *kl, integer *ku,
	double *ab, integer *ldab, integer *ipiv, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__65 = 65;
	static double c_b18 = -1.;
	static double c_b31 = 1.;

    /* System generated locals */
    integer ab_dim1, ab_offset, i__1, i__2, i__3, i__4, i__5, i__6;
    double d__1;

    /* Local variables */
    integer i__, j, i2, i3, j2, j3, k2, jb, nb, ii, jj, jm, ip, jp, km, ju,
	    kv, nw;
    double temp;
    double work13[4160]	/* was [65][64] */, work31[4160]	/*
	    was [65][64] */;

/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGBTRF computes an LU factorization of a real m-by-n band matrix A */
/*  using partial pivoting with row interchanges. */

/*  This is the blocked version of the algorithm, calling Level 3 BLAS. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  KL      (input) INTEGER */
/*          The number of subdiagonals within the band of A.  KL >= 0. */

/*  KU      (input) INTEGER */
/*          The number of superdiagonals within the band of A.  KU >= 0. */

/*  AB      (input/output) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          On entry, the matrix A in band storage, in rows KL+1 to */
/*          2*KL+KU+1; rows 1 to KL of the array need not be set. */
/*          The j-th column of A is stored in the j-th column of the */
/*          array AB as follows: */
/*          AB(kl+ku+1+i-j,j) = A(i,j) for max(1,j-ku)<=i<=min(m,j+kl) */

/*          On exit, details of the factorization: U is stored as an */
/*          upper triangular band matrix with KL+KU superdiagonals in */
/*          rows 1 to KL+KU+1, and the multipliers used during the */
/*          factorization are stored in rows KL+KU+2 to 2*KL+KU+1. */
/*          See below for further details. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= 2*KL+KU+1. */

/*  IPIV    (output) INTEGER array, dimension (min(M,N)) */
/*          The pivot indices; for 1 <= i <= min(M,N), row i of the */
/*          matrix was interchanged with row IPIV(i). */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          > 0: if INFO = +i, U(i,i) is exactly zero. The factorization */
/*               has been completed, but the factor U is exactly */
/*               singular, and division by zero will occur if it is used */
/*               to solve a system of equations. */

/*  Further Details */
/*  =============== */

/*  The band storage scheme is illustrated by the following example, when */
/*  M = N = 6, KL = 2, KU = 1: */

/*  On entry:                       On exit: */

/*      *    *    *    +    +    +       *    *    *   u14  u25  u36 */
/*      *    *    +    +    +    +       *    *   u13  u24  u35  u46 */
/*      *   a12  a23  a34  a45  a56      *   u12  u23  u34  u45  u56 */
/*     a11  a22  a33  a44  a55  a66     u11  u22  u33  u44  u55  u66 */
/*     a21  a32  a43  a54  a65   *      m21  m32  m43  m54  m65   * */
/*     a31  a42  a53  a64   *    *      m31  m42  m53  m64   *    * */

/*  Array elements marked * are not used by the routine; elements marked */
/*  + need not be set on entry, but are required by the routine to store */
/*  elements of U because of fill-in resulting from the row interchanges. */

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

/*     KV is the number of superdiagonals in the factor U, allowing for */
/*     fill-in */

    /* Parameter adjustments */
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    --ipiv;

    /* Function Body */
    kv = *ku + *kl;

/*     Test the input parameters. */

    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*kl < 0) {
	*info = -3;
    } else if (*ku < 0) {
	*info = -4;
    } else if (*ldab < *kl + kv + 1) {
	*info = -6;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGBTRF", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	return 0;
    }

/*     Determine the block size for this environment */

    nb = ilaenv_(&c__1, "DGBTRF", " ", m, n, kl, ku);

/*     The block size must not exceed the limit set by the size of the */
/*     local arrays WORK13 and WORK31. */

    nb = std::min(nb,64_integer);

    if (nb <= 1 || nb > *kl) {

/*        Use unblocked code */

	dgbtf2_(m, n, kl, ku, &ab[ab_offset], ldab, &ipiv[1], info);
    } else {

/*        Use blocked code */

/*        Zero the superdiagonal elements of the work array WORK13 */

	i__1 = nb;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j - 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		work13[i__ + j * 65 - 66] = 0.;
/* L10: */
	    }
/* L20: */
	}

/*        Zero the subdiagonal elements of the work array WORK31 */

	i__1 = nb;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = nb;
	    for (i__ = j + 1; i__ <= i__2; ++i__) {
		work31[i__ + j * 65 - 66] = 0.;
/* L30: */
	    }
/* L40: */
	}

/*        Gaussian elimination with partial pivoting */

/*        Set fill-in elements in columns KU+2 to KV to zero */

	i__1 = std::min(kv,*n);
	for (j = *ku + 2; j <= i__1; ++j) {
	    i__2 = *kl;
	    for (i__ = kv - j + 2; i__ <= i__2; ++i__) {
		ab[i__ + j * ab_dim1] = 0.;
/* L50: */
	    }
/* L60: */
	}

/*        JU is the index of the last column affected by the current */
/*        stage of the factorization */

	ju = 1;

	i__1 = std::min(*m,*n);
	i__2 = nb;
	for (j = 1; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {
/* Computing MIN */
	    i__3 = nb, i__4 = std::min(*m,*n) - j + 1;
	    jb = std::min(i__3,i__4);

/*           The active part of the matrix is partitioned */

/*              A11   A12   A13 */
/*              A21   A22   A23 */
/*              A31   A32   A33 */

/*           Here A11, A21 and A31 denote the current block of JB columns */
/*           which is about to be factorized. The number of rows in the */
/*           partitioning are JB, I2, I3 respectively, and the numbers */
/*           of columns are JB, J2, J3. The superdiagonal elements of A13 */
/*           and the subdiagonal elements of A31 lie outside the band. */

/* Computing MIN */
	    i__3 = *kl - jb, i__4 = *m - j - jb + 1;
	    i2 = std::min(i__3,i__4);
/* Computing MIN */
	    i__3 = jb, i__4 = *m - j - *kl + 1;
	    i3 = std::min(i__3,i__4);

/*           J2 and J3 are computed after JU has been updated. */

/*           Factorize the current block of JB columns */

	    i__3 = j + jb - 1;
	    for (jj = j; jj <= i__3; ++jj) {

/*              Set fill-in elements in column JJ+KV to zero */

		if (jj + kv <= *n) {
		    i__4 = *kl;
		    for (i__ = 1; i__ <= i__4; ++i__) {
			ab[i__ + (jj + kv) * ab_dim1] = 0.;
/* L70: */
		    }
		}

/*              Find pivot and test for singularity. KM is the number of */
/*              subdiagonal elements in the current column. */

/* Computing MIN */
		i__4 = *kl, i__5 = *m - jj;
		km = std::min(i__4,i__5);
		i__4 = km + 1;
		jp = idamax_(&i__4, &ab[kv + 1 + jj * ab_dim1], &c__1);
		ipiv[jj] = jp + jj - j;
		if (ab[kv + jp + jj * ab_dim1] != 0.) {
/* Computing MAX */
/* Computing MIN */
		    i__6 = jj + *ku + jp - 1;
		    i__4 = ju, i__5 = std::min(i__6,*n);
		    ju = std::max(i__4,i__5);
		    if (jp != 1) {

/*                    Apply interchange to columns J to J+JB-1 */

			if (jp + jj - 1 < j + *kl) {

			    i__4 = *ldab - 1;
			    i__5 = *ldab - 1;
			    dswap_(&jb, &ab[kv + 1 + jj - j + j * ab_dim1], &
				    i__4, &ab[kv + jp + jj - j + j * ab_dim1],
				     &i__5);
			} else {

/*                       The interchange affects columns J to JJ-1 of A31 */
/*                       which are stored in the work array WORK31 */

			    i__4 = jj - j;
			    i__5 = *ldab - 1;
			    dswap_(&i__4, &ab[kv + 1 + jj - j + j * ab_dim1],
				    &i__5, &work31[jp + jj - j - *kl - 1], &
				    c__65);
			    i__4 = j + jb - jj;
			    i__5 = *ldab - 1;
			    i__6 = *ldab - 1;
			    dswap_(&i__4, &ab[kv + 1 + jj * ab_dim1], &i__5, &
				    ab[kv + jp + jj * ab_dim1], &i__6);
			}
		    }

/*                 Compute multipliers */

		    d__1 = 1. / ab[kv + 1 + jj * ab_dim1];
		    dscal_(&km, &d__1, &ab[kv + 2 + jj * ab_dim1], &c__1);

/*                 Update trailing submatrix within the band and within */
/*                 the current block. JM is the index of the last column */
/*                 which needs to be updated. */

/* Computing MIN */
		    i__4 = ju, i__5 = j + jb - 1;
		    jm = std::min(i__4,i__5);
		    if (jm > jj) {
			i__4 = jm - jj;
			i__5 = *ldab - 1;
			i__6 = *ldab - 1;
			dger_(&km, &i__4, &c_b18, &ab[kv + 2 + jj * ab_dim1],
				&c__1, &ab[kv + (jj + 1) * ab_dim1], &i__5, &
				ab[kv + 1 + (jj + 1) * ab_dim1], &i__6);
		    }
		} else {

/*                 If pivot is zero, set INFO to the index of the pivot */
/*                 unless a zero pivot has already been found. */

		    if (*info == 0) {
			*info = jj;
		    }
		}

/*              Copy current column of A31 into the work array WORK31 */

/* Computing MIN */
		i__4 = jj - j + 1;
		nw = std::min(i__4,i3);
		if (nw > 0) {
		    dcopy_(&nw, &ab[kv + *kl + 1 - jj + j + jj * ab_dim1], &
			    c__1, &work31[(jj - j + 1) * 65 - 65], &c__1);
		}
/* L80: */
	    }
	    if (j + jb <= *n) {

/*              Apply the row interchanges to the other blocks. */

/* Computing MIN */
		i__3 = ju - j + 1;
		j2 = std::min(i__3,kv) - jb;
/* Computing MAX */
		i__3 = 0, i__4 = ju - j - kv + 1;
		j3 = std::max(i__3,i__4);

/*              Use DLASWP to apply the row interchanges to A12, A22, and */
/*              A32. */

		i__3 = *ldab - 1;
		dlaswp_(&j2, &ab[kv + 1 - jb + (j + jb) * ab_dim1], &i__3, &
			c__1, &jb, &ipiv[j], &c__1);

/*              Adjust the pivot indices. */

		i__3 = j + jb - 1;
		for (i__ = j; i__ <= i__3; ++i__) {
		    ipiv[i__] = ipiv[i__] + j - 1;
/* L90: */
		}

/*              Apply the row interchanges to A13, A23, and A33 */
/*              columnwise. */

		k2 = j - 1 + jb + j2;
		i__3 = j3;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    jj = k2 + i__;
		    i__4 = j + jb - 1;
		    for (ii = j + i__ - 1; ii <= i__4; ++ii) {
			ip = ipiv[ii];
			if (ip != ii) {
			    temp = ab[kv + 1 + ii - jj + jj * ab_dim1];
			    ab[kv + 1 + ii - jj + jj * ab_dim1] = ab[kv + 1 +
				    ip - jj + jj * ab_dim1];
			    ab[kv + 1 + ip - jj + jj * ab_dim1] = temp;
			}
/* L100: */
		    }
/* L110: */
		}

/*              Update the relevant part of the trailing submatrix */

		if (j2 > 0) {

/*                 Update A12 */

		    i__3 = *ldab - 1;
		    i__4 = *ldab - 1;
		    dtrsm_("Left", "Lower", "No transpose", "Unit", &jb, &j2,
			    &c_b31, &ab[kv + 1 + j * ab_dim1], &i__3, &ab[kv
			    + 1 - jb + (j + jb) * ab_dim1], &i__4);

		    if (i2 > 0) {

/*                    Update A22 */

			i__3 = *ldab - 1;
			i__4 = *ldab - 1;
			i__5 = *ldab - 1;
			dgemm_("No transpose", "No transpose", &i2, &j2, &jb,
				&c_b18, &ab[kv + 1 + jb + j * ab_dim1], &i__3,
				 &ab[kv + 1 - jb + (j + jb) * ab_dim1], &i__4,
				 &c_b31, &ab[kv + 1 + (j + jb) * ab_dim1], &
				i__5);
		    }

		    if (i3 > 0) {

/*                    Update A32 */

			i__3 = *ldab - 1;
			i__4 = *ldab - 1;
			dgemm_("No transpose", "No transpose", &i3, &j2, &jb,
				&c_b18, work31, &c__65, &ab[kv + 1 - jb + (j
				+ jb) * ab_dim1], &i__3, &c_b31, &ab[kv + *kl
				+ 1 - jb + (j + jb) * ab_dim1], &i__4);
		    }
		}

		if (j3 > 0) {

/*                 Copy the lower triangle of A13 into the work array */
/*                 WORK13 */

		    i__3 = j3;
		    for (jj = 1; jj <= i__3; ++jj) {
			i__4 = jb;
			for (ii = jj; ii <= i__4; ++ii) {
			    work13[ii + jj * 65 - 66] = ab[ii - jj + 1 + (jj
				    + j + kv - 1) * ab_dim1];
/* L120: */
			}
/* L130: */
		    }

/*                 Update A13 in the work array */

		    i__3 = *ldab - 1;
		    dtrsm_("Left", "Lower", "No transpose", "Unit", &jb, &j3,
			    &c_b31, &ab[kv + 1 + j * ab_dim1], &i__3, work13,
			    &c__65);

		    if (i2 > 0) {

/*                    Update A23 */

			i__3 = *ldab - 1;
			i__4 = *ldab - 1;
			dgemm_("No transpose", "No transpose", &i2, &j3, &jb,
				&c_b18, &ab[kv + 1 + jb + j * ab_dim1], &i__3,
				 work13, &c__65, &c_b31, &ab[jb + 1 + (j + kv)
				 * ab_dim1], &i__4);
		    }

		    if (i3 > 0) {

/*                    Update A33 */

			i__3 = *ldab - 1;
			dgemm_("No transpose", "No transpose", &i3, &j3, &jb,
				&c_b18, work31, &c__65, work13, &c__65, &
				c_b31, &ab[*kl + 1 + (j + kv) * ab_dim1], &
				i__3);
		    }

/*                 Copy the lower triangle of A13 back into place */

		    i__3 = j3;
		    for (jj = 1; jj <= i__3; ++jj) {
			i__4 = jb;
			for (ii = jj; ii <= i__4; ++ii) {
			    ab[ii - jj + 1 + (jj + j + kv - 1) * ab_dim1] =
				    work13[ii + jj * 65 - 66];
/* L140: */
			}
/* L150: */
		    }
		}
	    } else {

/*              Adjust the pivot indices. */

		i__3 = j + jb - 1;
		for (i__ = j; i__ <= i__3; ++i__) {
		    ipiv[i__] = ipiv[i__] + j - 1;
/* L160: */
		}
	    }

/*           Partially undo the interchanges in the current block to */
/*           restore the upper triangular form of A31 and copy the upper */
/*           triangle of A31 back into place */

	    i__3 = j;
	    for (jj = j + jb - 1; jj >= i__3; --jj) {
		jp = ipiv[jj] - jj + 1;
		if (jp != 1) {

/*                 Apply interchange to columns J to JJ-1 */

		    if (jp + jj - 1 < j + *kl) {

/*                    The interchange does not affect A31 */

			i__4 = jj - j;
			i__5 = *ldab - 1;
			i__6 = *ldab - 1;
			dswap_(&i__4, &ab[kv + 1 + jj - j + j * ab_dim1], &
				i__5, &ab[kv + jp + jj - j + j * ab_dim1], &
				i__6);
		    } else {

/*                    The interchange does affect A31 */

			i__4 = jj - j;
			i__5 = *ldab - 1;
			dswap_(&i__4, &ab[kv + 1 + jj - j + j * ab_dim1], &
				i__5, &work31[jp + jj - j - *kl - 1], &c__65);
		    }
		}

/*              Copy the current column of A31 back into place */

/* Computing MIN */
		i__4 = i3, i__5 = jj - j + 1;
		nw = std::min(i__4,i__5);
		if (nw > 0) {
		    dcopy_(&nw, &work31[(jj - j + 1) * 65 - 65], &c__1, &ab[
			    kv + *kl + 1 - jj + j + jj * ab_dim1], &c__1);
		}
/* L170: */
	    }
/* L180: */
	}
    }

    return 0;

/*     End of DGBTRF */

} /* dgbtrf_ */

/* Subroutine */ int dgbtrs_(const char *trans, integer *n, integer *kl, integer *
	ku, integer *nrhs, double *ab, integer *ldab, integer *ipiv,
	double *b, integer *ldb, integer *info)
{
	/* Table of constant values */
	static double c_b7 = -1.;
	static integer c__1 = 1;
	static double c_b23 = 1.;

    /* System generated locals */
    integer ab_dim1, ab_offset, b_dim1, b_offset, i__1, i__2, i__3;

    /* Local variables */
    integer i__, j, l, kd, lm;
    bool lnoti;
    bool notran;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGBTRS solves a system of linear equations */
/*     A * X = B  or  A' * X = B */
/*  with a general band matrix A using the LU factorization computed */
/*  by DGBTRF. */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations. */
/*          = 'N':  A * X = B  (No transpose) */
/*          = 'T':  A'* X = B  (Transpose) */
/*          = 'C':  A'* X = B  (Conjugate transpose = Transpose) */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  KL      (input) INTEGER */
/*          The number of subdiagonals within the band of A.  KL >= 0. */

/*  KU      (input) INTEGER */
/*          The number of superdiagonals within the band of A.  KU >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          Details of the LU factorization of the band matrix A, as */
/*          computed by DGBTRF.  U is stored as an upper triangular band */
/*          matrix with KL+KU superdiagonals in rows 1 to KL+KU+1, and */
/*          the multipliers used during the factorization are stored in */
/*          rows KL+KU+2 to 2*KL+KU+1. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= 2*KL+KU+1. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices; for 1 <= i <= N, row i of the matrix was */
/*          interchanged with row IPIV(i). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the right hand side matrix B. */
/*          On exit, the solution matrix X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */

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
    --ipiv;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    *info = 0;
    notran = lsame_(trans, "N");
    if (! notran && ! lsame_(trans, "T") && ! lsame_(
	    trans, "C")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*kl < 0) {
	*info = -3;
    } else if (*ku < 0) {
	*info = -4;
    } else if (*nrhs < 0) {
	*info = -5;
    } else if (*ldab < (*kl << 1) + *ku + 1) {
	*info = -7;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -10;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGBTRS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *nrhs == 0) {
	return 0;
    }

    kd = *ku + *kl + 1;
    lnoti = *kl > 0;

    if (notran) {

/*        Solve  A*X = B. */

/*        Solve L*X = B, overwriting B with X. */

/*        L is represented as a product of permutations and unit lower */
/*        triangular matrices L = P(1) * L(1) * ... * P(n-1) * L(n-1), */
/*        where each transformation L(i) is a rank-one modification of */
/*        the identity matrix. */

	if (lnoti) {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__2 = *kl, i__3 = *n - j;
		lm = std::min(i__2,i__3);
		l = ipiv[j];
		if (l != j) {
		    dswap_(nrhs, &b[l + b_dim1], ldb, &b[j + b_dim1], ldb);
		}
		dger_(&lm, nrhs, &c_b7, &ab[kd + 1 + j * ab_dim1], &c__1, &b[
			j + b_dim1], ldb, &b[j + 1 + b_dim1], ldb);
/* L10: */
	    }
	}

	i__1 = *nrhs;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Solve U*X = B, overwriting B with X. */

	    i__2 = *kl + *ku;
	    dtbsv_("Upper", "No transpose", "Non-unit", n, &i__2, &ab[
		    ab_offset], ldab, &b[i__ * b_dim1 + 1], &c__1);
/* L20: */
	}

    } else {

/*        Solve A'*X = B. */

	i__1 = *nrhs;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Solve U'*X = B, overwriting B with X. */

	    i__2 = *kl + *ku;
	    dtbsv_("Upper", "Transpose", "Non-unit", n, &i__2, &ab[ab_offset],
		     ldab, &b[i__ * b_dim1 + 1], &c__1);
/* L30: */
	}

/*        Solve L'*X = B, overwriting B with X. */

	if (lnoti) {
	    for (j = *n - 1; j >= 1; --j) {
/* Computing MIN */
		i__1 = *kl, i__2 = *n - j;
		lm = std::min(i__1,i__2);
		dgemv_("Transpose", &lm, nrhs, &c_b7, &b[j + 1 + b_dim1], ldb,
			 &ab[kd + 1 + j * ab_dim1], &c__1, &c_b23, &b[j +
			b_dim1], ldb);
		l = ipiv[j];
		if (l != j) {
		    dswap_(nrhs, &b[l + b_dim1], ldb, &b[j + b_dim1], ldb);
		}
/* L40: */
	    }
	}
    }
    return 0;

/*     End of DGBTRS */

} /* dgbtrs_ */

/* Subroutine */ int dgebak_(const char *job, const char *side, integer *n, integer *ilo,
	integer *ihi, double *scale, integer *m, double *v, integer *
	ldv, integer *info)
{
    /* System generated locals */
    integer v_dim1, v_offset, i__1;

    /* Local variables */
    integer i__, k;
    double s;
    integer ii;
    bool leftv;
    bool rightv;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGEBAK forms the right or left eigenvectors of a real general matrix */
/*  by backward transformation on the computed eigenvectors of the */
/*  balanced matrix output by DGEBAL. */

/*  Arguments */
/*  ========= */

/*  JOB     (input) CHARACTER*1 */
/*          Specifies the type of backward transformation required: */
/*          = 'N', do nothing, return immediately; */
/*          = 'P', do backward transformation for permutation only; */
/*          = 'S', do backward transformation for scaling only; */
/*          = 'B', do backward transformations for both permutation and */
/*                 scaling. */
/*          JOB must be the same as the argument JOB supplied to DGEBAL. */

/*  SIDE    (input) CHARACTER*1 */
/*          = 'R':  V contains right eigenvectors; */
/*          = 'L':  V contains left eigenvectors. */

/*  N       (input) INTEGER */
/*          The number of rows of the matrix V.  N >= 0. */

/*  ILO     (input) INTEGER */
/*  IHI     (input) INTEGER */
/*          The integers ILO and IHI determined by DGEBAL. */
/*          1 <= ILO <= IHI <= N, if N > 0; ILO=1 and IHI=0, if N=0. */

/*  SCALE   (input) DOUBLE PRECISION array, dimension (N) */
/*          Details of the permutation and scaling factors, as returned */
/*          by DGEBAL. */

/*  M       (input) INTEGER */
/*          The number of columns of the matrix V.  M >= 0. */

/*  V       (input/output) DOUBLE PRECISION array, dimension (LDV,M) */
/*          On entry, the matrix of right or left eigenvectors to be */
/*          transformed, as returned by DHSEIN or DTREVC. */
/*          On exit, V is overwritten by the transformed eigenvectors. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of the array V. LDV >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

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

/*     Decode and Test the input parameters */

    /* Parameter adjustments */
    --scale;
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;

    /* Function Body */
    rightv = lsame_(side, "R");
    leftv = lsame_(side, "L");

    *info = 0;
    if (! lsame_(job, "N") && ! lsame_(job, "P") && ! lsame_(job, "S")
	    && ! lsame_(job, "B")) {
	*info = -1;
    } else if (! rightv && ! leftv) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ilo < 1 || *ilo > std::max(1_integer,*n)) {
	*info = -4;
    } else if (*ihi < std::min(*ilo,*n) || *ihi > *n) {
	*info = -5;
    } else if (*m < 0) {
	*info = -7;
    } else if (*ldv < std::max(1_integer,*n)) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEBAK", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }
    if (*m == 0) {
	return 0;
    }
    if (lsame_(job, "N")) {
	return 0;
    }

    if (*ilo == *ihi) {
	goto L30;
    }

/*     Backward balance */

    if (lsame_(job, "S") || lsame_(job, "B")) {

	if (rightv) {
	    i__1 = *ihi;
	    for (i__ = *ilo; i__ <= i__1; ++i__) {
		s = scale[i__];
		dscal_(m, &s, &v[i__ + v_dim1], ldv);
/* L10: */
	    }
	}

	if (leftv) {
	    i__1 = *ihi;
	    for (i__ = *ilo; i__ <= i__1; ++i__) {
		s = 1. / scale[i__];
		dscal_(m, &s, &v[i__ + v_dim1], ldv);
/* L20: */
	    }
	}

    }

/*     Backward permutation */

/*     For  I = ILO-1 step -1 until 1, */
/*              IHI+1 step 1 until N do -- */

L30:
    if (lsame_(job, "P") || lsame_(job, "B")) {
	if (rightv) {
	    i__1 = *n;
	    for (ii = 1; ii <= i__1; ++ii) {
		i__ = ii;
		if (i__ >= *ilo && i__ <= *ihi) {
		    goto L40;
		}
		if (i__ < *ilo) {
		    i__ = *ilo - ii;
		}
		k = (integer) scale[i__];
		if (k == i__) {
		    goto L40;
		}
		dswap_(m, &v[i__ + v_dim1], ldv, &v[k + v_dim1], ldv);
L40:
		;
	    }
	}

	if (leftv) {
	    i__1 = *n;
	    for (ii = 1; ii <= i__1; ++ii) {
		i__ = ii;
		if (i__ >= *ilo && i__ <= *ihi) {
		    goto L50;
		}
		if (i__ < *ilo) {
		    i__ = *ilo - ii;
		}
		k = (integer) scale[i__];
		if (k == i__) {
		    goto L50;
		}
		dswap_(m, &v[i__ + v_dim1], ldv, &v[k + v_dim1], ldv);
L50:
		;
	    }
	}
    }

    return 0;

/*     End of DGEBAK */

} /* dgebak_ */

/* Subroutine */ int dgebal_(const char *job, integer *n, double *a, integer *
	lda, integer *ilo, integer *ihi, double *scale, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    double c__, f, g;
    integer i__, j, k, l, m;
    double r__, s, ca, ra;
    integer ica, ira, iexc;
	double sfmin1, sfmin2, sfmax1, sfmax2;
    bool noconv;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGEBAL balances a general real matrix A.  This involves, first, */
/*  permuting A by a similarity transformation to isolate eigenvalues */
/*  in the first 1 to ILO-1 and last IHI+1 to N elements on the */
/*  diagonal; and second, applying a diagonal similarity transformation */
/*  to rows and columns ILO to IHI to make the rows and columns as */
/*  close in norm as possible.  Both steps are optional. */

/*  Balancing may reduce the 1-norm of the matrix, and improve the */
/*  accuracy of the computed eigenvalues and/or eigenvectors. */

/*  Arguments */
/*  ========= */

/*  JOB     (input) CHARACTER*1 */
/*          Specifies the operations to be performed on A: */
/*          = 'N':  none:  simply set ILO = 1, IHI = N, SCALE(I) = 1.0 */
/*                  for i = 1,...,N; */
/*          = 'P':  permute only; */
/*          = 'S':  scale only; */
/*          = 'B':  both permute and scale. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the input matrix A. */
/*          On exit,  A is overwritten by the balanced matrix. */
/*          If JOB = 'N', A is not referenced. */
/*          See Further Details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  ILO     (output) INTEGER */
/*  IHI     (output) INTEGER */
/*          ILO and IHI are set to integers such that on exit */
/*          A(i,j) = 0 if i > j and j = 1,...,ILO-1 or I = IHI+1,...,N. */
/*          If JOB = 'N' or 'S', ILO = 1 and IHI = N. */

/*  SCALE   (output) DOUBLE PRECISION array, dimension (N) */
/*          Details of the permutations and scaling factors applied to */
/*          A.  If P(j) is the index of the row and column interchanged */
/*          with row and column j and D(j) is the scaling factor */
/*          applied to row and column j, then */
/*          SCALE(j) = P(j)    for j = 1,...,ILO-1 */
/*                   = D(j)    for j = ILO,...,IHI */
/*                   = P(j)    for j = IHI+1,...,N. */
/*          The order in which the interchanges are made is N to IHI+1, */
/*          then 1 to ILO-1. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  The permutations consist of row and column interchanges which put */
/*  the matrix in the form */

/*             ( T1   X   Y  ) */
/*     P A P = (  0   B   Z  ) */
/*             (  0   0   T2 ) */

/*  where T1 and T2 are upper triangular matrices whose eigenvalues lie */
/*  along the diagonal.  The column indices ILO and IHI mark the starting */
/*  and ending columns of the submatrix B. Balancing consists of applying */
/*  a diagonal similarity transformation inv(D) * B * D to make the */
/*  1-norms of each row of B and its corresponding column nearly equal. */
/*  The output matrix is */

/*     ( T1     X*D          Y    ) */
/*     (  0  inv(D)*B*D  inv(D)*Z ). */
/*     (  0      0           T2   ) */

/*  Information about the permutations P and the diagonal matrix D is */
/*  returned in the vector SCALE. */

/*  This subroutine is based on the EISPACK routine BALANC. */

/*  Modified by Tzu-Yi Chen, Computer Science Division, University of */
/*    California at Berkeley, USA */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --scale;

    /* Function Body */
    *info = 0;
    if (! lsame_(job, "N") && ! lsame_(job, "P") && ! lsame_(job, "S")
	    && ! lsame_(job, "B")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEBAL", &i__1);
	return 0;
    }

    k = 1;
    l = *n;

    if (*n == 0) {
	goto L210;
    }

    if (lsame_(job, "N")) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    scale[i__] = 1.;
/* L10: */
	}
	goto L210;
    }

    if (lsame_(job, "S")) {
	goto L120;
    }

/*     Permutation to isolate eigenvalues if possible */

    goto L50;

/*     Row and column exchange. */

L20:
    scale[m] = (double) j;
    if (j == m) {
	goto L30;
    }

    dswap_(&l, &a[j * a_dim1 + 1], &c__1, &a[m * a_dim1 + 1], &c__1);
    i__1 = *n - k + 1;
    dswap_(&i__1, &a[j + k * a_dim1], lda, &a[m + k * a_dim1], lda);

L30:
    switch (iexc) {
	case 1:  goto L40;
	case 2:  goto L80;
    }

/*     Search for rows isolating an eigenvalue and push them down. */

L40:
    if (l == 1) {
	goto L210;
    }
    --l;

L50:
    for (j = l; j >= 1; --j) {

	i__1 = l;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (i__ == j) {
		goto L60;
	    }
	    if (a[j + i__ * a_dim1] != 0.) {
		goto L70;
	    }
L60:
	    ;
	}

	m = l;
	iexc = 1;
	goto L20;
L70:
	;
    }

    goto L90;

/*     Search for columns isolating an eigenvalue and push them left. */

L80:
    ++k;

L90:
    i__1 = l;
    for (j = k; j <= i__1; ++j) {

	i__2 = l;
	for (i__ = k; i__ <= i__2; ++i__) {
	    if (i__ == j) {
		goto L100;
	    }
	    if (a[i__ + j * a_dim1] != 0.) {
		goto L110;
	    }
L100:
	    ;
	}

	m = k;
	iexc = 2;
	goto L20;
L110:
	;
    }

L120:
    i__1 = l;
    for (i__ = k; i__ <= i__1; ++i__) {
	scale[i__] = 1.;
/* L130: */
    }

    if (lsame_(job, "P")) {
	goto L210;
    }

/*     Balance the submatrix in rows K to L. */

/*     Iterative loop for norm reduction */

    sfmin1 = dlamch_("S") / dlamch_("P");
    sfmax1 = 1. / sfmin1;
    sfmin2 = sfmin1 * 2.;
    sfmax2 = 1. / sfmin2;
L140:
    noconv = false;

    i__1 = l;
    for (i__ = k; i__ <= i__1; ++i__) {
	c__ = 0.;
	r__ = 0.;

	i__2 = l;
	for (j = k; j <= i__2; ++j) {
	    if (j == i__) {
		goto L150;
	    }
	    c__ += (d__1 = a[j + i__ * a_dim1], abs(d__1));
	    r__ += (d__1 = a[i__ + j * a_dim1], abs(d__1));
L150:
	    ;
	}
	ica = idamax_(&l, &a[i__ * a_dim1 + 1], &c__1);
	ca = (d__1 = a[ica + i__ * a_dim1], abs(d__1));
	i__2 = *n - k + 1;
	ira = idamax_(&i__2, &a[i__ + k * a_dim1], lda);
	ra = (d__1 = a[i__ + (ira + k - 1) * a_dim1], abs(d__1));

/*        Guard against zero C or R due to underflow. */

	if (c__ == 0. || r__ == 0.) {
	    goto L200;
	}
	g = r__ / 2.;
	f = 1.;
	s = c__ + r__;
L160:
/* Computing MAX */
	d__1 = std::max(f,c__);
/* Computing MIN */
	d__2 = std::min(r__,g);
	if (c__ >= g || std::max(d__1,ca) >= sfmax2 || std::min(d__2,ra) <= sfmin2) {
	    goto L170;
	}
	f *= 2.;
	c__ *= 2.;
	ca *= 2.;
	r__ /= 2.;
	g /= 2.;
	ra /= 2.;
	goto L160;

L170:
	g = c__ / 2.;
L180:
/* Computing MIN */
	d__1 = std::min(f,c__), d__1 = std::min(d__1,g);
	if (g < r__ || std::max(r__,ra) >= sfmax2 || std::min(d__1,ca) <= sfmin2) {
	    goto L190;
	}
	f /= 2.;
	c__ /= 2.;
	g /= 2.;
	ca /= 2.;
	r__ *= 2.;
	ra *= 2.;
	goto L180;

/*        Now balance. */

L190:
	if (c__ + r__ >= s * .95) {
	    goto L200;
	}
	if (f < 1. && scale[i__] < 1.) {
	    if (f * scale[i__] <= sfmin1) {
		goto L200;
	    }
	}
	if (f > 1. && scale[i__] > 1.) {
	    if (scale[i__] >= sfmax1 / f) {
		goto L200;
	    }
	}
	g = 1. / f;
	scale[i__] *= f;
	noconv = true;

	i__2 = *n - k + 1;
	dscal_(&i__2, &g, &a[i__ + k * a_dim1], lda);
	dscal_(&l, &f, &a[i__ * a_dim1 + 1], &c__1);

L200:
	;
    }

    if (noconv) {
	goto L140;
    }

L210:
    *ilo = k;
    *ihi = l;

    return 0;

/*     End of DGEBAL */

} /* dgebal_ */

/* Subroutine */ int dgebd2_(integer *m, integer *n, double *a, integer *
	lda, double *d__, double *e, double *tauq, double *
	taup, double *work, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    integer i__;

/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGEBD2 reduces a real general m by n matrix A to upper or lower */
/*  bidiagonal form B by an orthogonal transformation: Q' * A * P = B. */

/*  If m >= n, B is upper bidiagonal; if m < n, B is lower bidiagonal. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows in the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns in the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the m by n general matrix to be reduced. */
/*          On exit, */
/*          if m >= n, the diagonal and the first superdiagonal are */
/*            overwritten with the upper bidiagonal matrix B; the */
/*            elements below the diagonal, with the array TAUQ, represent */
/*            the orthogonal matrix Q as a product of elementary */
/*            reflectors, and the elements above the first superdiagonal, */
/*            with the array TAUP, represent the orthogonal matrix P as */
/*            a product of elementary reflectors; */
/*          if m < n, the diagonal and the first subdiagonal are */
/*            overwritten with the lower bidiagonal matrix B; the */
/*            elements below the first subdiagonal, with the array TAUQ, */
/*            represent the orthogonal matrix Q as a product of */
/*            elementary reflectors, and the elements above the diagonal, */
/*            with the array TAUP, represent the orthogonal matrix P as */
/*            a product of elementary reflectors. */
/*          See Further Details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  D       (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The diagonal elements of the bidiagonal matrix B: */
/*          D(i) = A(i,i). */

/*  E       (output) DOUBLE PRECISION array, dimension (min(M,N)-1) */
/*          The off-diagonal elements of the bidiagonal matrix B: */
/*          if m >= n, E(i) = A(i,i+1) for i = 1,2,...,n-1; */
/*          if m < n, E(i) = A(i+1,i) for i = 1,2,...,m-1. */

/*  TAUQ    (output) DOUBLE PRECISION array dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors which */
/*          represent the orthogonal matrix Q. See Further Details. */

/*  TAUP    (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors which */
/*          represent the orthogonal matrix P. See Further Details. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (max(M,N)) */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit. */
/*          < 0: if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  The matrices Q and P are represented as products of elementary */
/*  reflectors: */

/*  If m >= n, */

/*     Q = H(1) H(2) . . . H(n)  and  P = G(1) G(2) . . . G(n-1) */

/*  Each H(i) and G(i) has the form: */

/*     H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u' */

/*  where tauq and taup are real scalars, and v and u are real vectors; */
/*  v(1:i-1) = 0, v(i) = 1, and v(i+1:m) is stored on exit in A(i+1:m,i); */
/*  u(1:i) = 0, u(i+1) = 1, and u(i+2:n) is stored on exit in A(i,i+2:n); */
/*  tauq is stored in TAUQ(i) and taup in TAUP(i). */

/*  If m < n, */

/*     Q = H(1) H(2) . . . H(m-1)  and  P = G(1) G(2) . . . G(m) */

/*  Each H(i) and G(i) has the form: */

/*     H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u' */

/*  where tauq and taup are real scalars, and v and u are real vectors; */
/*  v(1:i) = 0, v(i+1) = 1, and v(i+2:m) is stored on exit in A(i+2:m,i); */
/*  u(1:i-1) = 0, u(i) = 1, and u(i+1:n) is stored on exit in A(i,i+1:n); */
/*  tauq is stored in TAUQ(i) and taup in TAUP(i). */

/*  The contents of A on exit are illustrated by the following examples: */

/*  m = 6 and n = 5 (m > n):          m = 5 and n = 6 (m < n): */

/*    (  d   e   u1  u1  u1 )           (  d   u1  u1  u1  u1  u1 ) */
/*    (  v1  d   e   u2  u2 )           (  e   d   u2  u2  u2  u2 ) */
/*    (  v1  v2  d   e   u3 )           (  v1  e   d   u3  u3  u3 ) */
/*    (  v1  v2  v3  d   e  )           (  v1  v2  e   d   u4  u4 ) */
/*    (  v1  v2  v3  v4  d  )           (  v1  v2  v3  e   d   u5 ) */
/*    (  v1  v2  v3  v4  v5 ) */

/*  where d and e denote diagonal and off-diagonal elements of B, vi */
/*  denotes an element of the vector defining H(i), and ui an element of */
/*  the vector defining G(i). */

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

/*     Test the input parameters */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --d__;
    --e;
    --tauq;
    --taup;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    }
    if (*info < 0) {
	i__1 = -(*info);
	xerbla_("DGEBD2", &i__1);
	return 0;
    }

    if (*m >= *n) {

/*        Reduce to upper bidiagonal form */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Generate elementary reflector H(i) to annihilate A(i+1:m,i) */

	    i__2 = *m - i__ + 1;
/* Computing MIN */
	    i__3 = i__ + 1;
	    dlarfg_(&i__2, &a[i__ + i__ * a_dim1], &a[std::min(i__3, *m)+ i__ *
		    a_dim1], &c__1, &tauq[i__]);
	    d__[i__] = a[i__ + i__ * a_dim1];
	    a[i__ + i__ * a_dim1] = 1.;

/*           Apply H(i) to A(i:m,i+1:n) from the left */

	    if (i__ < *n) {
		i__2 = *m - i__ + 1;
		i__3 = *n - i__;
		dlarf_("Left", &i__2, &i__3, &a[i__ + i__ * a_dim1], &c__1, &
			tauq[i__], &a[i__ + (i__ + 1) * a_dim1], lda, &work[1]
);
	    }
	    a[i__ + i__ * a_dim1] = d__[i__];

	    if (i__ < *n) {

/*              Generate elementary reflector G(i) to annihilate */
/*              A(i,i+2:n) */

		i__2 = *n - i__;
/* Computing MIN */
		i__3 = i__ + 2;
		dlarfg_(&i__2, &a[i__ + (i__ + 1) * a_dim1], &a[i__ + std::min(
			i__3, *n)* a_dim1], lda, &taup[i__]);
		e[i__] = a[i__ + (i__ + 1) * a_dim1];
		a[i__ + (i__ + 1) * a_dim1] = 1.;

/*              Apply G(i) to A(i+1:m,i+1:n) from the right */

		i__2 = *m - i__;
		i__3 = *n - i__;
		dlarf_("Right", &i__2, &i__3, &a[i__ + (i__ + 1) * a_dim1],
			lda, &taup[i__], &a[i__ + 1 + (i__ + 1) * a_dim1],
			lda, &work[1]);
		a[i__ + (i__ + 1) * a_dim1] = e[i__];
	    } else {
		taup[i__] = 0.;
	    }
/* L10: */
	}
    } else {

/*        Reduce to lower bidiagonal form */

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Generate elementary reflector G(i) to annihilate A(i,i+1:n) */

	    i__2 = *n - i__ + 1;
/* Computing MIN */
	    i__3 = i__ + 1;
	    dlarfg_(&i__2, &a[i__ + i__ * a_dim1], &a[i__ + std::min(i__3, *n)*
		    a_dim1], lda, &taup[i__]);
	    d__[i__] = a[i__ + i__ * a_dim1];
	    a[i__ + i__ * a_dim1] = 1.;

/*           Apply G(i) to A(i+1:m,i:n) from the right */

	    if (i__ < *m) {
		i__2 = *m - i__;
		i__3 = *n - i__ + 1;
		dlarf_("Right", &i__2, &i__3, &a[i__ + i__ * a_dim1], lda, &
			taup[i__], &a[i__ + 1 + i__ * a_dim1], lda, &work[1]);
	    }
	    a[i__ + i__ * a_dim1] = d__[i__];

	    if (i__ < *m) {

/*              Generate elementary reflector H(i) to annihilate */
/*              A(i+2:m,i) */

		i__2 = *m - i__;
/* Computing MIN */
		i__3 = i__ + 2;
		dlarfg_(&i__2, &a[i__ + 1 + i__ * a_dim1], &a[std::min(i__3, *m)+
			i__ * a_dim1], &c__1, &tauq[i__]);
		e[i__] = a[i__ + 1 + i__ * a_dim1];
		a[i__ + 1 + i__ * a_dim1] = 1.;

/*              Apply H(i) to A(i+1:m,i+1:n) from the left */

		i__2 = *m - i__;
		i__3 = *n - i__;
		dlarf_("Left", &i__2, &i__3, &a[i__ + 1 + i__ * a_dim1], &
			c__1, &tauq[i__], &a[i__ + 1 + (i__ + 1) * a_dim1],
			lda, &work[1]);
		a[i__ + 1 + i__ * a_dim1] = e[i__];
	    } else {
		tauq[i__] = 0.;
	    }
/* L20: */
	}
    }
    return 0;

/*     End of DGEBD2 */

} /* dgebd2_ */

/* Subroutine */ int dgebrd_(integer *m, integer *n, double *a, integer *
	lda, double *d__, double *e, double *tauq, double *
	taup, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__3 = 3;
	static integer c__2 = 2;
	static double c_b21 = -1.;
	static double c_b22 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    integer i__, j, nb, nx;
    double ws;
    integer nbmin, iinfo, minmn;
    integer ldwrkx, ldwrky, lwkopt;
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

/*  DGEBRD reduces a general real M-by-N matrix A to upper or lower */
/*  bidiagonal form B by an orthogonal transformation: Q**T * A * P = B. */

/*  If m >= n, B is upper bidiagonal; if m < n, B is lower bidiagonal. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows in the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns in the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N general matrix to be reduced. */
/*          On exit, */
/*          if m >= n, the diagonal and the first superdiagonal are */
/*            overwritten with the upper bidiagonal matrix B; the */
/*            elements below the diagonal, with the array TAUQ, represent */
/*            the orthogonal matrix Q as a product of elementary */
/*            reflectors, and the elements above the first superdiagonal, */
/*            with the array TAUP, represent the orthogonal matrix P as */
/*            a product of elementary reflectors; */
/*          if m < n, the diagonal and the first subdiagonal are */
/*            overwritten with the lower bidiagonal matrix B; the */
/*            elements below the first subdiagonal, with the array TAUQ, */
/*            represent the orthogonal matrix Q as a product of */
/*            elementary reflectors, and the elements above the diagonal, */
/*            with the array TAUP, represent the orthogonal matrix P as */
/*            a product of elementary reflectors. */
/*          See Further Details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  D       (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The diagonal elements of the bidiagonal matrix B: */
/*          D(i) = A(i,i). */

/*  E       (output) DOUBLE PRECISION array, dimension (min(M,N)-1) */
/*          The off-diagonal elements of the bidiagonal matrix B: */
/*          if m >= n, E(i) = A(i,i+1) for i = 1,2,...,n-1; */
/*          if m < n, E(i) = A(i+1,i) for i = 1,2,...,m-1. */

/*  TAUQ    (output) DOUBLE PRECISION array dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors which */
/*          represent the orthogonal matrix Q. See Further Details. */

/*  TAUP    (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors which */
/*          represent the orthogonal matrix P. See Further Details. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= max(1,M,N). */
/*          For optimum performance LWORK >= (M+N)*NB, where NB */
/*          is the optimal blocksize. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  The matrices Q and P are represented as products of elementary */
/*  reflectors: */

/*  If m >= n, */

/*     Q = H(1) H(2) . . . H(n)  and  P = G(1) G(2) . . . G(n-1) */

/*  Each H(i) and G(i) has the form: */

/*     H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u' */

/*  where tauq and taup are real scalars, and v and u are real vectors; */
/*  v(1:i-1) = 0, v(i) = 1, and v(i+1:m) is stored on exit in A(i+1:m,i); */
/*  u(1:i) = 0, u(i+1) = 1, and u(i+2:n) is stored on exit in A(i,i+2:n); */
/*  tauq is stored in TAUQ(i) and taup in TAUP(i). */

/*  If m < n, */

/*     Q = H(1) H(2) . . . H(m-1)  and  P = G(1) G(2) . . . G(m) */

/*  Each H(i) and G(i) has the form: */

/*     H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u' */

/*  where tauq and taup are real scalars, and v and u are real vectors; */
/*  v(1:i) = 0, v(i+1) = 1, and v(i+2:m) is stored on exit in A(i+2:m,i); */
/*  u(1:i-1) = 0, u(i) = 1, and u(i+1:n) is stored on exit in A(i,i+1:n); */
/*  tauq is stored in TAUQ(i) and taup in TAUP(i). */

/*  The contents of A on exit are illustrated by the following examples: */

/*  m = 6 and n = 5 (m > n):          m = 5 and n = 6 (m < n): */

/*    (  d   e   u1  u1  u1 )           (  d   u1  u1  u1  u1  u1 ) */
/*    (  v1  d   e   u2  u2 )           (  e   d   u2  u2  u2  u2 ) */
/*    (  v1  v2  d   e   u3 )           (  v1  e   d   u3  u3  u3 ) */
/*    (  v1  v2  v3  d   e  )           (  v1  v2  e   d   u4  u4 ) */
/*    (  v1  v2  v3  v4  d  )           (  v1  v2  v3  e   d   u5 ) */
/*    (  v1  v2  v3  v4  v5 ) */

/*  where d and e denote diagonal and off-diagonal elements of B, vi */
/*  denotes an element of the vector defining H(i), and ui an element of */
/*  the vector defining G(i). */

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

/*     Test the input parameters */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --d__;
    --e;
    --tauq;
    --taup;
    --work;

    /* Function Body */
    *info = 0;
/* Computing MAX */
    i__1 = 1, i__2 = ilaenv_(&c__1, "DGEBRD", " ", m, n, &c_n1, &c_n1);
    nb = std::max(i__1,i__2);
    lwkopt = (*m + *n) * nb;
    work[1] = (double) lwkopt;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = std::max(1_integer,*m);
	if (*lwork < std::max(i__1,*n) && ! lquery) {
	    *info = -10;
	}
    }
    if (*info < 0) {
	i__1 = -(*info);
	xerbla_("DGEBRD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    minmn = std::min(*m,*n);
    if (minmn == 0) {
	work[1] = 1.;
	return 0;
    }

    ws = (double) std::max(*m,*n);
    ldwrkx = *m;
    ldwrky = *n;

    if (nb > 1 && nb < minmn) {

/*        Set the crossover point NX. */

/* Computing MAX */
	i__1 = nb, i__2 = ilaenv_(&c__3, "DGEBRD", " ", m, n, &c_n1, &c_n1);
	nx = std::max(i__1,i__2);

/*        Determine when to switch from blocked to unblocked code. */

	if (nx < minmn) {
	    ws = (double) ((*m + *n) * nb);
	    if ((double) (*lwork) < ws) {

/*              Not enough work space for the optimal NB, consider using */
/*              a smaller block size. */

		nbmin = ilaenv_(&c__2, "DGEBRD", " ", m, n, &c_n1, &c_n1);
		if (*lwork >= (*m + *n) * nbmin) {
		    nb = *lwork / (*m + *n);
		} else {
		    nb = 1;
		    nx = minmn;
		}
	    }
	}
    } else {
	nx = minmn;
    }

    i__1 = minmn - nx;
    i__2 = nb;
    for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {

/*        Reduce rows and columns i:i+nb-1 to bidiagonal form and return */
/*        the matrices X and Y which are needed to update the unreduced */
/*        part of the matrix */

	i__3 = *m - i__ + 1;
	i__4 = *n - i__ + 1;
	dlabrd_(&i__3, &i__4, &nb, &a[i__ + i__ * a_dim1], lda, &d__[i__], &e[
		i__], &tauq[i__], &taup[i__], &work[1], &ldwrkx, &work[ldwrkx
		* nb + 1], &ldwrky);

/*        Update the trailing submatrix A(i+nb:m,i+nb:n), using an update */
/*        of the form  A := A - V*Y' - X*U' */

	i__3 = *m - i__ - nb + 1;
	i__4 = *n - i__ - nb + 1;
	dgemm_("No transpose", "Transpose", &i__3, &i__4, &nb, &c_b21, &a[i__
		+ nb + i__ * a_dim1], lda, &work[ldwrkx * nb + nb + 1], &
		ldwrky, &c_b22, &a[i__ + nb + (i__ + nb) * a_dim1], lda);
	i__3 = *m - i__ - nb + 1;
	i__4 = *n - i__ - nb + 1;
	dgemm_("No transpose", "No transpose", &i__3, &i__4, &nb, &c_b21, &
		work[nb + 1], &ldwrkx, &a[i__ + (i__ + nb) * a_dim1], lda, &
		c_b22, &a[i__ + nb + (i__ + nb) * a_dim1], lda);

/*        Copy diagonal and off-diagonal elements of B back into A */

	if (*m >= *n) {
	    i__3 = i__ + nb - 1;
	    for (j = i__; j <= i__3; ++j) {
		a[j + j * a_dim1] = d__[j];
		a[j + (j + 1) * a_dim1] = e[j];
/* L10: */
	    }
	} else {
	    i__3 = i__ + nb - 1;
	    for (j = i__; j <= i__3; ++j) {
		a[j + j * a_dim1] = d__[j];
		a[j + 1 + j * a_dim1] = e[j];
/* L20: */
	    }
	}
/* L30: */
    }

/*     Use unblocked code to reduce the remainder of the matrix */

    i__2 = *m - i__ + 1;
    i__1 = *n - i__ + 1;
    dgebd2_(&i__2, &i__1, &a[i__ + i__ * a_dim1], lda, &d__[i__], &e[i__], &
	    tauq[i__], &taup[i__], &work[1], &iinfo);
    work[1] = ws;
    return 0;

/*     End of DGEBRD */

} /* dgebrd_ */

/* Subroutine */ int dgecon_(const char *norm, integer *n, double *a, integer *
	lda, double *anorm, double *rcond, double *work, integer *
	iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1;
    double d__1;

    /* Local variables */
    double sl;
    integer ix;
    double su;
    integer kase, kase1;
    double scale;
    integer isave[3];
    double ainvnm;
    bool onenrm;
    char normin[1];
    double smlnum;


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

/*  DGECON estimates the reciprocal of the condition number of a general */
/*  real matrix A, in either the 1-norm or the infinity-norm, using */
/*  the LU factorization computed by DGETRF. */

/*  An estimate is obtained for norm(inv(A)), and the reciprocal of the */
/*  condition number is computed as */
/*     RCOND = 1 / ( norm(A) * norm(inv(A)) ). */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies whether the 1-norm condition number or the */
/*          infinity-norm condition number is required: */
/*          = '1' or 'O':  1-norm; */
/*          = 'I':         Infinity-norm. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The factors L and U from the factorization A = P*L*U */
/*          as computed by DGETRF. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  ANORM   (input) DOUBLE PRECISION */
/*          If NORM = '1' or 'O', the 1-norm of the original matrix A. */
/*          If NORM = 'I', the infinity-norm of the original matrix A. */

/*  RCOND   (output) DOUBLE PRECISION */
/*          The reciprocal of the condition number of the matrix A, */
/*          computed as RCOND = 1/(norm(A) * norm(inv(A))). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (4*N) */

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
    onenrm = *(unsigned char *)norm == '1' || lsame_(norm, "O");
    if (! onenrm && ! lsame_(norm, "I")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -4;
    } else if (*anorm < 0.) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGECON", &i__1);
	return 0;
    }

/*     Quick return if possible */

    *rcond = 0.;
    if (*n == 0) {
	*rcond = 1.;
	return 0;
    } else if (*anorm == 0.) {
	return 0;
    }

    smlnum = dlamch_("Safe minimum");

/*     Estimate the norm of inv(A). */

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

/*           Multiply by inv(L). */

	    dlatrs_("Lower", "No transpose", "Unit", normin, n, &a[a_offset],
		    lda, &work[1], &sl, &work[(*n << 1) + 1], info);

/*           Multiply by inv(U). */

	    dlatrs_("Upper", "No transpose", "Non-unit", normin, n, &a[
		    a_offset], lda, &work[1], &su, &work[*n * 3 + 1], info);
	} else {

/*           Multiply by inv(U'). */

	    dlatrs_("Upper", "Transpose", "Non-unit", normin, n, &a[a_offset],
		     lda, &work[1], &su, &work[*n * 3 + 1], info);

/*           Multiply by inv(L'). */

	    dlatrs_("Lower", "Transpose", "Unit", normin, n, &a[a_offset],
		    lda, &work[1], &sl, &work[(*n << 1) + 1], info);
	}

/*        Divide X by 1/(SL*SU) if doing so will not cause overflow. */

	scale = sl * su;
	*(unsigned char *)normin = 'Y';
	if (scale != 1.) {
	    ix = idamax_(n, &work[1], &c__1);
	    if (scale < (d__1 = work[ix], abs(d__1)) * smlnum || scale == 0.)
		    {
		goto L20;
	    }
	    drscl_(n, &scale, &work[1], &c__1);
	}
	goto L10;
    }

/*     Compute the estimate of the reciprocal condition number. */

    if (ainvnm != 0.) {
	*rcond = 1. / ainvnm / *anorm;
    }

L20:
    return 0;

/*     End of DGECON */

} /* dgecon_ */

/* Subroutine */ int dgeequ_(integer *m, integer *n, double *a, integer *
	lda, double *r__, double *c__, double *rowcnd, double
	*colcnd, double *amax, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j;
    double rcmin, rcmax;
    double bignum, smlnum;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGEEQU computes row and column scalings intended to equilibrate an */
/*  M-by-N matrix A and reduce its condition number.  R returns the row */
/*  scale factors and C the column scale factors, chosen to try to make */
/*  the largest element in each row and column of the matrix B with */
/*  elements B(i,j)=R(i)*A(i,j)*C(j) have absolute value 1. */

/*  R(i) and C(j) are restricted to be between SMLNUM = smallest safe */
/*  number and BIGNUM = largest safe number.  Use of these scaling */
/*  factors is not guaranteed to reduce the condition number of A but */
/*  works well in practice. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The M-by-N matrix whose equilibration factors are */
/*          to be computed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  R       (output) DOUBLE PRECISION array, dimension (M) */
/*          If INFO = 0 or INFO > M, R contains the row scale factors */
/*          for A. */

/*  C       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0,  C contains the column scale factors for A. */

/*  ROWCND  (output) DOUBLE PRECISION */
/*          If INFO = 0 or INFO > M, ROWCND contains the ratio of the */
/*          smallest R(i) to the largest R(i).  If ROWCND >= 0.1 and */
/*          AMAX is neither too large nor too small, it is not worth */
/*          scaling by R. */

/*  COLCND  (output) DOUBLE PRECISION */
/*          If INFO = 0, COLCND contains the ratio of the smallest */
/*          C(i) to the largest C(i).  If COLCND >= 0.1, it is not */
/*          worth scaling by C. */

/*  AMAX    (output) DOUBLE PRECISION */
/*          Absolute value of largest matrix element.  If AMAX is very */
/*          close to overflow or very close to underflow, the matrix */
/*          should be scaled. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i,  and i is */
/*                <= M:  the i-th row of A is exactly zero */
/*                >  M:  the (i-M)-th column of A is exactly zero */

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
    --r__;
    --c__;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEEQU", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	*rowcnd = 1.;
	*colcnd = 1.;
	*amax = 0.;
	return 0;
    }

/*     Get machine constants. */

    smlnum = dlamch_("S");
    bignum = 1. / smlnum;

/*     Compute row scale factors. */

    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	r__[i__] = 0.;
/* L10: */
    }

/*     Find the maximum element in each row. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = r__[i__], d__3 = (d__1 = a[i__ + j * a_dim1], abs(d__1));
	    r__[i__] = std::max(d__2,d__3);
/* L20: */
	}
/* L30: */
    }

/*     Find the maximum and minimum scale factors. */

    rcmin = bignum;
    rcmax = 0.;
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	d__1 = rcmax, d__2 = r__[i__];
	rcmax = std::max(d__1,d__2);
/* Computing MIN */
	d__1 = rcmin, d__2 = r__[i__];
	rcmin = std::min(d__1,d__2);
/* L40: */
    }
    *amax = rcmax;

    if (rcmin == 0.) {

/*        Find the first zero scale factor and return an error code. */

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (r__[i__] == 0.) {
		*info = i__;
		return 0;
	    }
/* L50: */
	}
    } else {

/*        Invert the scale factors. */

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MIN */
/* Computing MAX */
	    d__2 = r__[i__];
	    d__1 = std::max(d__2,smlnum);
	    r__[i__] = 1. / std::min(d__1,bignum);
/* L60: */
	}

/*        Compute ROWCND = min(R(I)) / max(R(I)) */

	*rowcnd = std::max(rcmin,smlnum) / std::min(rcmax,bignum);
    }

/*     Compute column scale factors */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	c__[j] = 0.;
/* L70: */
    }

/*     Find the maximum element in each column, */
/*     assuming the row scaling computed above. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = c__[j], d__3 = (d__1 = a[i__ + j * a_dim1], abs(d__1)) *
		    r__[i__];
	    c__[j] = std::max(d__2,d__3);
/* L80: */
	}
/* L90: */
    }

/*     Find the maximum and minimum scale factors. */

    rcmin = bignum;
    rcmax = 0.;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	d__1 = rcmin, d__2 = c__[j];
	rcmin = std::min(d__1,d__2);
/* Computing MAX */
	d__1 = rcmax, d__2 = c__[j];
	rcmax = std::max(d__1,d__2);
/* L100: */
    }

    if (rcmin == 0.) {

/*        Find the first zero scale factor and return an error code. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    if (c__[j] == 0.) {
		*info = *m + j;
		return 0;
	    }
/* L110: */
	}
    } else {

/*        Invert the scale factors. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
/* Computing MAX */
	    d__2 = c__[j];
	    d__1 = std::max(d__2,smlnum);
	    c__[j] = 1. / std::min(d__1,bignum);
/* L120: */
	}

/*        Compute COLCND = min(C(J)) / max(C(J)) */

	*colcnd = std::max(rcmin,smlnum) / std::min(rcmax,bignum);
    }

    return 0;

/*     End of DGEEQU */

} /* dgeequ_ */

/* Subroutine */ int dgeequb_(integer *m, integer *n, double *a, integer *
	lda, double *r__, double *c__, double *rowcnd, double
	*colcnd, double *amax, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j;
    double radix, rcmin, rcmax;
    double bignum, logrdx, smlnum;


/*     -- LAPACK routine (version 3.2)                                 -- */
/*     -- Contributed by James Demmel, Deaglan Halligan, Yozo Hida and -- */
/*     -- Jason Riedy of Univ. of California Berkeley.                 -- */
/*     -- November 2008                                                -- */

/*     -- LAPACK is a software package provided by Univ. of Tennessee, -- */
/*     -- Univ. of California Berkeley and NAG Ltd.                    -- */

/*     .. */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGEEQUB computes row and column scalings intended to equilibrate an */
/*  M-by-N matrix A and reduce its condition number.  R returns the row */
/*  scale factors and C the column scale factors, chosen to try to make */
/*  the largest element in each row and column of the matrix B with */
/*  elements B(i,j)=R(i)*A(i,j)*C(j) have an absolute value of at most */
/*  the radix. */

/*  R(i) and C(j) are restricted to be a power of the radix between */
/*  SMLNUM = smallest safe number and BIGNUM = largest safe number.  Use */
/*  of these scaling factors is not guaranteed to reduce the condition */
/*  number of A but works well in practice. */

/*  This routine differs from DGEEQU by restricting the scaling factors */
/*  to a power of the radix.  Baring over- and underflow, scaling by */
/*  these factors introduces no additional rounding errors.  However, the */
/*  scaled entries' magnitured are no longer approximately 1 but lie */
/*  between sqrt(radix) and 1/sqrt(radix). */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The M-by-N matrix whose equilibration factors are */
/*          to be computed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  R       (output) DOUBLE PRECISION array, dimension (M) */
/*          If INFO = 0 or INFO > M, R contains the row scale factors */
/*          for A. */

/*  C       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0,  C contains the column scale factors for A. */

/*  ROWCND  (output) DOUBLE PRECISION */
/*          If INFO = 0 or INFO > M, ROWCND contains the ratio of the */
/*          smallest R(i) to the largest R(i).  If ROWCND >= 0.1 and */
/*          AMAX is neither too large nor too small, it is not worth */
/*          scaling by R. */

/*  COLCND  (output) DOUBLE PRECISION */
/*          If INFO = 0, COLCND contains the ratio of the smallest */
/*          C(i) to the largest C(i).  If COLCND >= 0.1, it is not */
/*          worth scaling by C. */

/*  AMAX    (output) DOUBLE PRECISION */
/*          Absolute value of largest matrix element.  If AMAX is very */
/*          close to overflow or very close to underflow, the matrix */
/*          should be scaled. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i,  and i is */
/*                <= M:  the i-th row of A is exactly zero */
/*                >  M:  the (i-M)-th column of A is exactly zero */

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
    --r__;
    --c__;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEEQUB", &i__1);
	return 0;
    }

/*     Quick return if possible. */

    if (*m == 0 || *n == 0) {
	*rowcnd = 1.;
	*colcnd = 1.;
	*amax = 0.;
	return 0;
    }

/*     Get machine constants.  Assume SMLNUM is a power of the radix. */

    smlnum = dlamch_("S");
    bignum = 1. / smlnum;
    radix = dlamch_("B");
    logrdx = log(radix);

/*     Compute row scale factors. */

    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	r__[i__] = 0.;
/* L10: */
    }

/*     Find the maximum element in each row. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = r__[i__], d__3 = (d__1 = a[i__ + j * a_dim1], abs(d__1));
	    r__[i__] = std::max(d__2,d__3);
/* L20: */
	}
/* L30: */
    }
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (r__[i__] > 0.) {
	    i__2 = (integer) (log(r__[i__]) / logrdx);
	    r__[i__] = pow_di(&radix, &i__2);
	}
    }

/*     Find the maximum and minimum scale factors. */

    rcmin = bignum;
    rcmax = 0.;
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	d__1 = rcmax, d__2 = r__[i__];
	rcmax = std::max(d__1,d__2);
/* Computing MIN */
	d__1 = rcmin, d__2 = r__[i__];
	rcmin = std::min(d__1,d__2);
/* L40: */
    }
    *amax = rcmax;

    if (rcmin == 0.) {

/*        Find the first zero scale factor and return an error code. */

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (r__[i__] == 0.) {
		*info = i__;
		return 0;
	    }
/* L50: */
	}
    } else {

/*        Invert the scale factors. */

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MIN */
/* Computing MAX */
	    d__2 = r__[i__];
	    d__1 = std::max(d__2,smlnum);
	    r__[i__] = 1. / std::min(d__1,bignum);
/* L60: */
	}

/*        Compute ROWCND = min(R(I)) / max(R(I)). */

	*rowcnd = std::max(rcmin,smlnum) / std::min(rcmax,bignum);
    }

/*     Compute column scale factors */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	c__[j] = 0.;
/* L70: */
    }

/*     Find the maximum element in each column, */
/*     assuming the row scaling computed above. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = c__[j], d__3 = (d__1 = a[i__ + j * a_dim1], abs(d__1)) *
		    r__[i__];
	    c__[j] = std::max(d__2,d__3);
/* L80: */
	}
	if (c__[j] > 0.) {
	    i__2 = (integer) (log(c__[j]) / logrdx);
	    c__[j] = pow_di(&radix, &i__2);
	}
/* L90: */
    }

/*     Find the maximum and minimum scale factors. */

    rcmin = bignum;
    rcmax = 0.;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	d__1 = rcmin, d__2 = c__[j];
	rcmin = std::min(d__1,d__2);
/* Computing MAX */
	d__1 = rcmax, d__2 = c__[j];
	rcmax = std::max(d__1,d__2);
/* L100: */
    }

    if (rcmin == 0.) {

/*        Find the first zero scale factor and return an error code. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    if (c__[j] == 0.) {
		*info = *m + j;
		return 0;
	    }
/* L110: */
	}
    } else {

/*        Invert the scale factors. */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
/* Computing MAX */
	    d__2 = c__[j];
	    d__1 = std::max(d__2,smlnum);
	    c__[j] = 1. / std::min(d__1,bignum);
/* L120: */
	}

/*        Compute COLCND = min(C(J)) / max(C(J)). */

	*colcnd = std::max(rcmin,smlnum) / std::min(rcmax,bignum);
    }

    return 0;

/*     End of DGEEQUB */

} /* dgeequb_ */

/* Subroutine */ int dgees_(const char *jobvs, const char *sort, bool (*select)(const double *, const double *),
	integer *n, double *a, integer *lda, integer *sdim, double *wr,
	double *wi, double *vs, integer *ldvs, double *work,
	integer *lwork, bool *bwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__0 = 0;
	static integer c_n1 = -1;

    /* System generated locals */
    integer a_dim1, a_offset, vs_dim1, vs_offset, i__1, i__2, i__3;

    /* Builtin functions
    double sqrt(double);*/

    /* Local variables */
    integer i__;
    double s;
    integer i1, i2, ip, ihi, ilo;
    double dum[1], eps, sep;
    integer ibal;
    double anrm;
    integer idum[1], ierr, itau, iwrk, inxt, icond, ieval;
    bool cursl;
    bool lst2sl, scalea;
    double cscale;
    double bignum;
    bool lastsl;
    integer minwrk, maxwrk;
    double smlnum;
    integer hswork;
    bool wantst, lquery, wantvs;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */
/*     .. Function Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGEES computes for an N-by-N real nonsymmetric matrix A, the */
/*  eigenvalues, the real Schur form T, and, optionally, the matrix of */
/*  Schur vectors Z.  This gives the Schur factorization A = Z*T*(Z**T). */

/*  Optionally, it also orders the eigenvalues on the diagonal of the */
/*  real Schur form so that selected eigenvalues are at the top left. */
/*  The leading columns of Z then form an orthonormal basis for the */
/*  invariant subspace corresponding to the selected eigenvalues. */

/*  A matrix is in real Schur form if it is upper quasi-triangular with */
/*  1-by-1 and 2-by-2 blocks. 2-by-2 blocks will be standardized in the */
/*  form */
/*          [  a  b  ] */
/*          [  c  a  ] */

/*  where b*c < 0. The eigenvalues of such a block are a +- sqrt(bc). */

/*  Arguments */
/*  ========= */

/*  JOBVS   (input) CHARACTER*1 */
/*          = 'N': Schur vectors are not computed; */
/*          = 'V': Schur vectors are computed. */

/*  SORT    (input) CHARACTER*1 */
/*          Specifies whether or not to order the eigenvalues on the */
/*          diagonal of the Schur form. */
/*          = 'N': Eigenvalues are not ordered; */
/*          = 'S': Eigenvalues are ordered (see SELECT). */

/*  SELECT  (external procedure) LOGICAL FUNCTION of two DOUBLE PRECISION arguments */
/*          SELECT must be declared EXTERNAL in the calling subroutine. */
/*          If SORT = 'S', SELECT is used to select eigenvalues to sort */
/*          to the top left of the Schur form. */
/*          If SORT = 'N', SELECT is not referenced. */
/*          An eigenvalue WR(j)+sqrt(-1)*WI(j) is selected if */
/*          SELECT(WR(j),WI(j)) is true; i.e., if either one of a complex */
/*          conjugate pair of eigenvalues is selected, then both complex */
/*          eigenvalues are selected. */
/*          Note that a selected complex eigenvalue may no longer */
/*          satisfy SELECT(WR(j),WI(j)) = .TRUE. after ordering, since */
/*          ordering may change the value of complex eigenvalues */
/*          (especially if the eigenvalue is ill-conditioned); in this */
/*          case INFO is set to N+2 (see INFO below). */

/*  N       (input) INTEGER */
/*          The order of the matrix A. N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the N-by-N matrix A. */
/*          On exit, A has been overwritten by its real Schur form T. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  SDIM    (output) INTEGER */
/*          If SORT = 'N', SDIM = 0. */
/*          If SORT = 'S', SDIM = number of eigenvalues (after sorting) */
/*                         for which SELECT is true. (Complex conjugate */
/*                         pairs for which SELECT is true for either */
/*                         eigenvalue count as 2.) */

/*  WR      (output) DOUBLE PRECISION array, dimension (N) */
/*  WI      (output) DOUBLE PRECISION array, dimension (N) */
/*          WR and WI contain the real and imaginary parts, */
/*          respectively, of the computed eigenvalues in the same order */
/*          that they appear on the diagonal of the output Schur form T. */
/*          Complex conjugate pairs of eigenvalues will appear */
/*          consecutively with the eigenvalue having the positive */
/*          imaginary part first. */

/*  VS      (output) DOUBLE PRECISION array, dimension (LDVS,N) */
/*          If JOBVS = 'V', VS contains the orthogonal matrix Z of Schur */
/*          vectors. */
/*          If JOBVS = 'N', VS is not referenced. */

/*  LDVS    (input) INTEGER */
/*          The leading dimension of the array VS.  LDVS >= 1; if */
/*          JOBVS = 'V', LDVS >= N. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) contains the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= max(1,3*N). */
/*          For good performance, LWORK must generally be larger. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  BWORK   (workspace) LOGICAL array, dimension (N) */
/*          Not referenced if SORT = 'N'. */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value. */
/*          > 0: if INFO = i, and i is */
/*             <= N: the QR algorithm failed to compute all the */
/*                   eigenvalues; elements 1:ILO-1 and i+1:N of WR and WI */
/*                   contain those eigenvalues which have converged; if */
/*                   JOBVS = 'V', VS contains the matrix which reduces A */
/*                   to its partially converged Schur form. */
/*             = N+1: the eigenvalues could not be reordered because some */
/*                   eigenvalues were too close to separate (the problem */
/*                   is very ill-conditioned); */
/*             = N+2: after reordering, roundoff changed values of some */
/*                   complex eigenvalues so that leading eigenvalues in */
/*                   the Schur form no longer satisfy SELECT=.TRUE.  This */
/*                   could also be caused by underflow due to scaling. */

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

/*     Test the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --wr;
    --wi;
    vs_dim1 = *ldvs;
    vs_offset = 1 + vs_dim1;
    vs -= vs_offset;
    --work;
    --bwork;

    /* Function Body */
    *info = 0;
    lquery = *lwork == -1;
    wantvs = lsame_(jobvs, "V");
    wantst = lsame_(sort, "S");
    if (! wantvs && ! lsame_(jobvs, "N")) {
	*info = -1;
    } else if (! wantst && ! lsame_(sort, "N")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -4;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -6;
    } else if (*ldvs < 1 || wantvs && *ldvs < *n) {
	*info = -11;
    }

/*     Compute workspace */
/*      (Note: Comments in the code beginning "Workspace:" describe the */
/*       minimal amount of workspace needed at that point in the code, */
/*       as well as the preferred amount for good performance. */
/*       NB refers to the optimal block size for the immediately */
/*       following subroutine, as returned by ILAENV. */
/*       HSWORK refers to the workspace preferred by DHSEQR, as */
/*       calculated below. HSWORK is computed assuming ILO=1 and IHI=N, */
/*       the worst case.) */

    if (*info == 0) {
	if (*n == 0) {
	    minwrk = 1;
	    maxwrk = 1;
	} else {
	    maxwrk = (*n << 1) + *n * ilaenv_(&c__1, "DGEHRD", " ", n, &c__1,
		    n, &c__0);
	    minwrk = *n * 3;

	    dhseqr_("S", jobvs, n, &c__1, n, &a[a_offset], lda, &wr[1], &wi[1]
, &vs[vs_offset], ldvs, &work[1], &c_n1, &ieval);
	    hswork = (integer) work[1];

	    if (! wantvs) {
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n + hswork;
		maxwrk = std::max(i__1,i__2);
	    } else {
/* Computing MAX */
		i__1 = maxwrk, i__2 = (*n << 1) + (*n - 1) * ilaenv_(&c__1,
			"DORGHR", " ", n, &c__1, n, &c_n1);
		maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n + hswork;
		maxwrk = std::max(i__1,i__2);
	    }
	}
	work[1] = (double) maxwrk;

	if (*lwork < minwrk && ! lquery) {
	    *info = -13;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEES ", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	*sdim = 0;
	return 0;
    }

/*     Get machine constants */

    eps = dlamch_("P");
    smlnum = dlamch_("S");
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);
    smlnum = sqrt(smlnum) / eps;
    bignum = 1. / smlnum;

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", n, n, &a[a_offset], lda, dum);
    scalea = false;
    if (anrm > 0. && anrm < smlnum) {
	scalea = true;
	cscale = smlnum;
    } else if (anrm > bignum) {
	scalea = true;
	cscale = bignum;
    }
    if (scalea) {
	dlascl_("G", &c__0, &c__0, &anrm, &cscale, n, n, &a[a_offset], lda, &
		ierr);
    }

/*     Permute the matrix to make it more nearly triangular */
/*     (Workspace: need N) */

    ibal = 1;
    dgebal_("P", n, &a[a_offset], lda, &ilo, &ihi, &work[ibal], &ierr);

/*     Reduce to upper Hessenberg form */
/*     (Workspace: need 3*N, prefer 2*N+N*NB) */

    itau = *n + ibal;
    iwrk = *n + itau;
    i__1 = *lwork - iwrk + 1;
    dgehrd_(n, &ilo, &ihi, &a[a_offset], lda, &work[itau], &work[iwrk], &i__1,
	     &ierr);

    if (wantvs) {

/*        Copy Householder vectors to VS */

	dlacpy_("L", n, n, &a[a_offset], lda, &vs[vs_offset], ldvs)
		;

/*        Generate orthogonal matrix in VS */
/*        (Workspace: need 3*N-1, prefer 2*N+(N-1)*NB) */

	i__1 = *lwork - iwrk + 1;
	dorghr_(n, &ilo, &ihi, &vs[vs_offset], ldvs, &work[itau], &work[iwrk],
		 &i__1, &ierr);
    }

    *sdim = 0;

/*     Perform QR iteration, accumulating Schur vectors in VS if desired */
/*     (Workspace: need N+1, prefer N+HSWORK (see comments) ) */

    iwrk = itau;
    i__1 = *lwork - iwrk + 1;
    dhseqr_("S", jobvs, n, &ilo, &ihi, &a[a_offset], lda, &wr[1], &wi[1], &vs[
	    vs_offset], ldvs, &work[iwrk], &i__1, &ieval);
    if (ieval > 0) {
	*info = ieval;
    }

/*     Sort eigenvalues if desired */

    if (wantst && *info == 0) {
	if (scalea) {
	    dlascl_("G", &c__0, &c__0, &cscale, &anrm, n, &c__1, &wr[1], n, &
		    ierr);
	    dlascl_("G", &c__0, &c__0, &cscale, &anrm, n, &c__1, &wi[1], n, &
		    ierr);
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    bwork[i__] = (*select)(&wr[i__], &wi[i__]);
/* L10: */
	}

/*        Reorder eigenvalues and transform Schur vectors */
/*        (Workspace: none needed) */

	i__1 = *lwork - iwrk + 1;
	dtrsen_("N", jobvs, &bwork[1], n, &a[a_offset], lda, &vs[vs_offset],
		ldvs, &wr[1], &wi[1], sdim, &s, &sep, &work[iwrk], &i__1,
		idum, &c__1, &icond);
	if (icond > 0) {
	    *info = *n + icond;
	}
    }

    if (wantvs) {

/*        Undo balancing */
/*        (Workspace: need N) */

	dgebak_("P", "R", n, &ilo, &ihi, &work[ibal], n, &vs[vs_offset], ldvs,
		 &ierr);
    }

    if (scalea) {

/*        Undo scaling for the Schur form of A */

	dlascl_("H", &c__0, &c__0, &cscale, &anrm, n, n, &a[a_offset], lda, &
		ierr);
	i__1 = *lda + 1;
	dcopy_(n, &a[a_offset], &i__1, &wr[1], &c__1);
	if (cscale == smlnum) {

/*           If scaling back towards underflow, adjust WI if an */
/*           offdiagonal element of a 2-by-2 block in the Schur form */
/*           underflows. */

	    if (ieval > 0) {
		i1 = ieval + 1;
		i2 = ihi - 1;
		i__1 = ilo - 1;
/* Computing MAX */
		i__3 = ilo - 1;
		i__2 = std::max(i__3,1_integer);
		dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wi[
			1], &i__2, &ierr);
	    } else if (wantst) {
		i1 = 1;
		i2 = *n - 1;
	    } else {
		i1 = ilo;
		i2 = ihi - 1;
	    }
	    inxt = i1 - 1;
	    i__1 = i2;
	    for (i__ = i1; i__ <= i__1; ++i__) {
		if (i__ < inxt) {
		    goto L20;
		}
		if (wi[i__] == 0.) {
		    inxt = i__ + 1;
		} else {
		    if (a[i__ + 1 + i__ * a_dim1] == 0.) {
			wi[i__] = 0.;
			wi[i__ + 1] = 0.;
		    } else if (a[i__ + 1 + i__ * a_dim1] != 0. && a[i__ + (
			    i__ + 1) * a_dim1] == 0.) {
			wi[i__] = 0.;
			wi[i__ + 1] = 0.;
			if (i__ > 1) {
			    i__2 = i__ - 1;
			    dswap_(&i__2, &a[i__ * a_dim1 + 1], &c__1, &a[(
				    i__ + 1) * a_dim1 + 1], &c__1);
			}
			if (*n > i__ + 1) {
			    i__2 = *n - i__ - 1;
			    dswap_(&i__2, &a[i__ + (i__ + 2) * a_dim1], lda, &
				    a[i__ + 1 + (i__ + 2) * a_dim1], lda);
			}
			if (wantvs) {
			    dswap_(n, &vs[i__ * vs_dim1 + 1], &c__1, &vs[(i__
				    + 1) * vs_dim1 + 1], &c__1);
			}
			a[i__ + (i__ + 1) * a_dim1] = a[i__ + 1 + i__ *
				a_dim1];
			a[i__ + 1 + i__ * a_dim1] = 0.;
		    }
		    inxt = i__ + 2;
		}
L20:
		;
	    }
	}

/*        Undo scaling for the imaginary part of the eigenvalues */

	i__1 = *n - ieval;
/* Computing MAX */
	i__3 = *n - ieval;
	i__2 = std::max(i__3,1_integer);
	dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wi[ieval +
		1], &i__2, &ierr);
    }

    if (wantst && *info == 0) {

/*        Check if reordering successful */

	lastsl = true;
	lst2sl = true;
	*sdim = 0;
	ip = 0;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    cursl = (*select)(&wr[i__], &wi[i__]);
	    if (wi[i__] == 0.) {
		if (cursl) {
		    ++(*sdim);
		}
		ip = 0;
		if (cursl && ! lastsl) {
		    *info = *n + 2;
		}
	    } else {
		if (ip == 1) {

/*                 Last eigenvalue of conjugate pair */

		    cursl = cursl || lastsl;
		    lastsl = cursl;
		    if (cursl) {
			*sdim += 2;
		    }
		    ip = -1;
		    if (cursl && ! lst2sl) {
			*info = *n + 2;
		    }
		} else {

/*                 First eigenvalue of conjugate pair */

		    ip = 1;
		}
	    }
	    lst2sl = lastsl;
	    lastsl = cursl;
/* L30: */
	}
    }

    work[1] = (double) maxwrk;
    return 0;

/*     End of DGEES */

} /* dgees_ */

/* Subroutine */ int dgeesx_(const char *jobvs, const char *sort, bool (*select)(const double *, const double *),
	const char *sense, integer *n, double *a, integer *lda, integer *sdim,
	double *wr, double *wi, double *vs, integer *ldvs,
	double *rconde, double *rcondv, double *work, integer *
	lwork, integer *iwork, integer *liwork, bool *bwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__0 = 0;
	static integer c_n1 = -1;

    /* System generated locals */
    integer a_dim1, a_offset, vs_dim1, vs_offset, i__1, i__2, i__3;

    /* Local variables */
    integer i__, i1, i2, ip, ihi, ilo;
    double dum[1], eps;
    integer ibal;
    double anrm;
    integer ierr, itau, iwrk, lwrk, inxt, icond, ieval;
    bool cursl;
    integer liwrk;
    bool lst2sl, scalea;
    double cscale;
    double bignum;
    bool wantsb;
    bool wantse, lastsl;
    integer minwrk, maxwrk;
    bool wantsn;
    double smlnum;
    integer hswork;
    bool wantst, lquery, wantsv, wantvs;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */
/*     .. Function Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGEESX computes for an N-by-N real nonsymmetric matrix A, the */
/*  eigenvalues, the real Schur form T, and, optionally, the matrix of */
/*  Schur vectors Z.  This gives the Schur factorization A = Z*T*(Z**T). */

/*  Optionally, it also orders the eigenvalues on the diagonal of the */
/*  real Schur form so that selected eigenvalues are at the top left; */
/*  computes a reciprocal condition number for the average of the */
/*  selected eigenvalues (RCONDE); and computes a reciprocal condition */
/*  number for the right invariant subspace corresponding to the */
/*  selected eigenvalues (RCONDV).  The leading columns of Z form an */
/*  orthonormal basis for this invariant subspace. */

/*  For further explanation of the reciprocal condition numbers RCONDE */
/*  and RCONDV, see Section 4.10 of the LAPACK Users' Guide (where */
/*  these quantities are called s and sep respectively). */

/*  A real matrix is in real Schur form if it is upper quasi-triangular */
/*  with 1-by-1 and 2-by-2 blocks. 2-by-2 blocks will be standardized in */
/*  the form */
/*            [  a  b  ] */
/*            [  c  a  ] */

/*  where b*c < 0. The eigenvalues of such a block are a +- sqrt(bc). */

/*  Arguments */
/*  ========= */

/*  JOBVS   (input) CHARACTER*1 */
/*          = 'N': Schur vectors are not computed; */
/*          = 'V': Schur vectors are computed. */

/*  SORT    (input) CHARACTER*1 */
/*          Specifies whether or not to order the eigenvalues on the */
/*          diagonal of the Schur form. */
/*          = 'N': Eigenvalues are not ordered; */
/*          = 'S': Eigenvalues are ordered (see SELECT). */

/*  SELECT  (external procedure) LOGICAL FUNCTION of two DOUBLE PRECISION arguments */
/*          SELECT must be declared EXTERNAL in the calling subroutine. */
/*          If SORT = 'S', SELECT is used to select eigenvalues to sort */
/*          to the top left of the Schur form. */
/*          If SORT = 'N', SELECT is not referenced. */
/*          An eigenvalue WR(j)+sqrt(-1)*WI(j) is selected if */
/*          SELECT(WR(j),WI(j)) is true; i.e., if either one of a */
/*          complex conjugate pair of eigenvalues is selected, then both */
/*          are.  Note that a selected complex eigenvalue may no longer */
/*          satisfy SELECT(WR(j),WI(j)) = .TRUE. after ordering, since */
/*          ordering may change the value of complex eigenvalues */
/*          (especially if the eigenvalue is ill-conditioned); in this */
/*          case INFO may be set to N+3 (see INFO below). */

/*  SENSE   (input) CHARACTER*1 */
/*          Determines which reciprocal condition numbers are computed. */
/*          = 'N': None are computed; */
/*          = 'E': Computed for average of selected eigenvalues only; */
/*          = 'V': Computed for selected right invariant subspace only; */
/*          = 'B': Computed for both. */
/*          If SENSE = 'E', 'V' or 'B', SORT must equal 'S'. */

/*  N       (input) INTEGER */
/*          The order of the matrix A. N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the N-by-N matrix A. */
/*          On exit, A is overwritten by its real Schur form T. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  SDIM    (output) INTEGER */
/*          If SORT = 'N', SDIM = 0. */
/*          If SORT = 'S', SDIM = number of eigenvalues (after sorting) */
/*                         for which SELECT is true. (Complex conjugate */
/*                         pairs for which SELECT is true for either */
/*                         eigenvalue count as 2.) */

/*  WR      (output) DOUBLE PRECISION array, dimension (N) */
/*  WI      (output) DOUBLE PRECISION array, dimension (N) */
/*          WR and WI contain the real and imaginary parts, respectively, */
/*          of the computed eigenvalues, in the same order that they */
/*          appear on the diagonal of the output Schur form T.  Complex */
/*          conjugate pairs of eigenvalues appear consecutively with the */
/*          eigenvalue having the positive imaginary part first. */

/*  VS      (output) DOUBLE PRECISION array, dimension (LDVS,N) */
/*          If JOBVS = 'V', VS contains the orthogonal matrix Z of Schur */
/*          vectors. */
/*          If JOBVS = 'N', VS is not referenced. */

/*  LDVS    (input) INTEGER */
/*          The leading dimension of the array VS.  LDVS >= 1, and if */
/*          JOBVS = 'V', LDVS >= N. */

/*  RCONDE  (output) DOUBLE PRECISION */
/*          If SENSE = 'E' or 'B', RCONDE contains the reciprocal */
/*          condition number for the average of the selected eigenvalues. */
/*          Not referenced if SENSE = 'N' or 'V'. */

/*  RCONDV  (output) DOUBLE PRECISION */
/*          If SENSE = 'V' or 'B', RCONDV contains the reciprocal */
/*          condition number for the selected right invariant subspace. */
/*          Not referenced if SENSE = 'N' or 'E'. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= max(1,3*N). */
/*          Also, if SENSE = 'E' or 'V' or 'B', */
/*          LWORK >= N+2*SDIM*(N-SDIM), where SDIM is the number of */
/*          selected eigenvalues computed by this routine.  Note that */
/*          N+2*SDIM*(N-SDIM) <= N+N*N/2. Note also that an error is only */
/*          returned if LWORK < max(1,3*N), but if SENSE = 'E' or 'V' or */
/*          'B' this may not be large enough. */
/*          For good performance, LWORK must generally be larger. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates upper bounds on the optimal sizes of the */
/*          arrays WORK and IWORK, returns these values as the first */
/*          entries of the WORK and IWORK arrays, and no error messages */
/*          related to LWORK or LIWORK are issued by XERBLA. */

/*  IWORK   (workspace/output) INTEGER array, dimension (MAX(1,LIWORK)) */
/*          On exit, if INFO = 0, IWORK(1) returns the optimal LIWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK. */
/*          LIWORK >= 1; if SENSE = 'V' or 'B', LIWORK >= SDIM*(N-SDIM). */
/*          Note that SDIM*(N-SDIM) <= N*N/4. Note also that an error is */
/*          only returned if LIWORK < 1, but if SENSE = 'V' or 'B' this */
/*          may not be large enough. */

/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates upper bounds on the optimal sizes of */
/*          the arrays WORK and IWORK, returns these values as the first */
/*          entries of the WORK and IWORK arrays, and no error messages */
/*          related to LWORK or LIWORK are issued by XERBLA. */

/*  BWORK   (workspace) LOGICAL array, dimension (N) */
/*          Not referenced if SORT = 'N'. */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value. */
/*          > 0: if INFO = i, and i is */
/*             <= N: the QR algorithm failed to compute all the */
/*                   eigenvalues; elements 1:ILO-1 and i+1:N of WR and WI */
/*                   contain those eigenvalues which have converged; if */
/*                   JOBVS = 'V', VS contains the transformation which */
/*                   reduces A to its partially converged Schur form. */
/*             = N+1: the eigenvalues could not be reordered because some */
/*                   eigenvalues were too close to separate (the problem */
/*                   is very ill-conditioned); */
/*             = N+2: after reordering, roundoff changed values of some */
/*                   complex eigenvalues so that leading eigenvalues in */
/*                   the Schur form no longer satisfy SELECT=.TRUE.  This */
/*                   could also be caused by underflow due to scaling. */

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

/*     Test the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --wr;
    --wi;
    vs_dim1 = *ldvs;
    vs_offset = 1 + vs_dim1;
    vs -= vs_offset;
    --work;
    --iwork;
    --bwork;

    /* Function Body */
    *info = 0;
    wantvs = lsame_(jobvs, "V");
    wantst = lsame_(sort, "S");
    wantsn = lsame_(sense, "N");
    wantse = lsame_(sense, "E");
    wantsv = lsame_(sense, "V");
    wantsb = lsame_(sense, "B");
    lquery = *lwork == -1 || *liwork == -1;
    if (! wantvs && ! lsame_(jobvs, "N")) {
	*info = -1;
    } else if (! wantst && ! lsame_(sort, "N")) {
	*info = -2;
    } else if (! (wantsn || wantse || wantsv || wantsb) || ! wantst && !
	    wantsn) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldvs < 1 || wantvs && *ldvs < *n) {
	*info = -12;
    }

/*     Compute workspace */
/*      (Note: Comments in the code beginning "RWorkspace:" describe the */
/*       minimal amount of real workspace needed at that point in the */
/*       code, as well as the preferred amount for good performance. */
/*       IWorkspace refers to integer workspace. */
/*       NB refers to the optimal block size for the immediately */
/*       following subroutine, as returned by ILAENV. */
/*       HSWORK refers to the workspace preferred by DHSEQR, as */
/*       calculated below. HSWORK is computed assuming ILO=1 and IHI=N, */
/*       the worst case. */
/*       If SENSE = 'E', 'V' or 'B', then the amount of workspace needed */
/*       depends on SDIM, which is computed by the routine DTRSEN later */
/*       in the code.) */

    if (*info == 0) {
	liwrk = 1;
	if (*n == 0) {
	    minwrk = 1;
	    lwrk = 1;
	} else {
	    maxwrk = (*n << 1) + *n * ilaenv_(&c__1, "DGEHRD", " ", n, &c__1,
		    n, &c__0);
	    minwrk = *n * 3;

	    dhseqr_("S", jobvs, n, &c__1, n, &a[a_offset], lda, &wr[1], &wi[1]
, &vs[vs_offset], ldvs, &work[1], &c_n1, &ieval);
	    hswork = (integer) work[1];

	    if (! wantvs) {
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n + hswork;
		maxwrk = std::max(i__1,i__2);
	    } else {
/* Computing MAX */
		i__1 = maxwrk, i__2 = (*n << 1) + (*n - 1) * ilaenv_(&c__1,
			"DORGHR", " ", n, &c__1, n, &c_n1);
		maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n + hswork;
		maxwrk = std::max(i__1,i__2);
	    }
	    lwrk = maxwrk;
	    if (! wantsn) {
/* Computing MAX */
		i__1 = lwrk, i__2 = *n + *n * *n / 2;
		lwrk = std::max(i__1,i__2);
	    }
	    if (wantsv || wantsb) {
		liwrk = *n * *n / 4;
	    }
	}
	iwork[1] = liwrk;
	work[1] = (double) lwrk;

	if (*lwork < minwrk && ! lquery) {
	    *info = -16;
	} else if (*liwork < 1 && ! lquery) {
	    *info = -18;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEESX", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	*sdim = 0;
	return 0;
    }

/*     Get machine constants */

    eps = dlamch_("P");
    smlnum = dlamch_("S");
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);
    smlnum = sqrt(smlnum) / eps;
    bignum = 1. / smlnum;

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", n, n, &a[a_offset], lda, dum);
    scalea = false;
    if (anrm > 0. && anrm < smlnum) {
	scalea = true;
	cscale = smlnum;
    } else if (anrm > bignum) {
	scalea = true;
	cscale = bignum;
    }
    if (scalea) {
	dlascl_("G", &c__0, &c__0, &anrm, &cscale, n, n, &a[a_offset], lda, &
		ierr);
    }

/*     Permute the matrix to make it more nearly triangular */
/*     (RWorkspace: need N) */

    ibal = 1;
    dgebal_("P", n, &a[a_offset], lda, &ilo, &ihi, &work[ibal], &ierr);

/*     Reduce to upper Hessenberg form */
/*     (RWorkspace: need 3*N, prefer 2*N+N*NB) */

    itau = *n + ibal;
    iwrk = *n + itau;
    i__1 = *lwork - iwrk + 1;
    dgehrd_(n, &ilo, &ihi, &a[a_offset], lda, &work[itau], &work[iwrk], &i__1,
	     &ierr);

    if (wantvs) {

/*        Copy Householder vectors to VS */

	dlacpy_("L", n, n, &a[a_offset], lda, &vs[vs_offset], ldvs)
		;

/*        Generate orthogonal matrix in VS */
/*        (RWorkspace: need 3*N-1, prefer 2*N+(N-1)*NB) */

	i__1 = *lwork - iwrk + 1;
	dorghr_(n, &ilo, &ihi, &vs[vs_offset], ldvs, &work[itau], &work[iwrk],
		 &i__1, &ierr);
    }

    *sdim = 0;

/*     Perform QR iteration, accumulating Schur vectors in VS if desired */
/*     (RWorkspace: need N+1, prefer N+HSWORK (see comments) ) */

    iwrk = itau;
    i__1 = *lwork - iwrk + 1;
    dhseqr_("S", jobvs, n, &ilo, &ihi, &a[a_offset], lda, &wr[1], &wi[1], &vs[
	    vs_offset], ldvs, &work[iwrk], &i__1, &ieval);
    if (ieval > 0) {
	*info = ieval;
    }

/*     Sort eigenvalues if desired */

    if (wantst && *info == 0) {
	if (scalea) {
	    dlascl_("G", &c__0, &c__0, &cscale, &anrm, n, &c__1, &wr[1], n, &
		    ierr);
	    dlascl_("G", &c__0, &c__0, &cscale, &anrm, n, &c__1, &wi[1], n, &
		    ierr);
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    bwork[i__] = (*select)(&wr[i__], &wi[i__]);
/* L10: */
	}

/*        Reorder eigenvalues, transform Schur vectors, and compute */
/*        reciprocal condition numbers */
/*        (RWorkspace: if SENSE is not 'N', need N+2*SDIM*(N-SDIM) */
/*                     otherwise, need N ) */
/*        (IWorkspace: if SENSE is 'V' or 'B', need SDIM*(N-SDIM) */
/*                     otherwise, need 0 ) */

	i__1 = *lwork - iwrk + 1;
	dtrsen_(sense, jobvs, &bwork[1], n, &a[a_offset], lda, &vs[vs_offset],
		 ldvs, &wr[1], &wi[1], sdim, rconde, rcondv, &work[iwrk], &
		i__1, &iwork[1], liwork, &icond);
	if (! wantsn) {
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n + (*sdim << 1) * (*n - *sdim);
	    maxwrk = std::max(i__1,i__2);
	}
	if (icond == -15) {

/*           Not enough real workspace */

	    *info = -16;
	} else if (icond == -17) {

/*           Not enough integer workspace */

	    *info = -18;
	} else if (icond > 0) {

/*           DTRSEN failed to reorder or to restore standard Schur form */

	    *info = icond + *n;
	}
    }

    if (wantvs) {

/*        Undo balancing */
/*        (RWorkspace: need N) */

	dgebak_("P", "R", n, &ilo, &ihi, &work[ibal], n, &vs[vs_offset], ldvs,
		 &ierr);
    }

    if (scalea) {

/*        Undo scaling for the Schur form of A */

	dlascl_("H", &c__0, &c__0, &cscale, &anrm, n, n, &a[a_offset], lda, &
		ierr);
	i__1 = *lda + 1;
	dcopy_(n, &a[a_offset], &i__1, &wr[1], &c__1);
	if ((wantsv || wantsb) && *info == 0) {
	    dum[0] = *rcondv;
	    dlascl_("G", &c__0, &c__0, &cscale, &anrm, &c__1, &c__1, dum, &
		    c__1, &ierr);
	    *rcondv = dum[0];
	}
	if (cscale == smlnum) {

/*           If scaling back towards underflow, adjust WI if an */
/*           offdiagonal element of a 2-by-2 block in the Schur form */
/*           underflows. */

	    if (ieval > 0) {
		i1 = ieval + 1;
		i2 = ihi - 1;
		i__1 = ilo - 1;
		dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wi[
			1], n, &ierr);
	    } else if (wantst) {
		i1 = 1;
		i2 = *n - 1;
	    } else {
		i1 = ilo;
		i2 = ihi - 1;
	    }
	    inxt = i1 - 1;
	    i__1 = i2;
	    for (i__ = i1; i__ <= i__1; ++i__) {
		if (i__ < inxt) {
		    goto L20;
		}
		if (wi[i__] == 0.) {
		    inxt = i__ + 1;
		} else {
		    if (a[i__ + 1 + i__ * a_dim1] == 0.) {
			wi[i__] = 0.;
			wi[i__ + 1] = 0.;
		    } else if (a[i__ + 1 + i__ * a_dim1] != 0. && a[i__ + (
			    i__ + 1) * a_dim1] == 0.) {
			wi[i__] = 0.;
			wi[i__ + 1] = 0.;
			if (i__ > 1) {
			    i__2 = i__ - 1;
			    dswap_(&i__2, &a[i__ * a_dim1 + 1], &c__1, &a[(
				    i__ + 1) * a_dim1 + 1], &c__1);
			}
			if (*n > i__ + 1) {
			    i__2 = *n - i__ - 1;
			    dswap_(&i__2, &a[i__ + (i__ + 2) * a_dim1], lda, &
				    a[i__ + 1 + (i__ + 2) * a_dim1], lda);
			}
			dswap_(n, &vs[i__ * vs_dim1 + 1], &c__1, &vs[(i__ + 1)
				 * vs_dim1 + 1], &c__1);
			a[i__ + (i__ + 1) * a_dim1] = a[i__ + 1 + i__ *
				a_dim1];
			a[i__ + 1 + i__ * a_dim1] = 0.;
		    }
		    inxt = i__ + 2;
		}
L20:
		;
	    }
	}
	i__1 = *n - ieval;
/* Computing MAX */
	i__3 = *n - ieval;
	i__2 = std::max(i__3,1_integer);
	dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wi[ieval +
		1], &i__2, &ierr);
    }

    if (wantst && *info == 0) {

/*        Check if reordering successful */

	lastsl = true;
	lst2sl = true;
	*sdim = 0;
	ip = 0;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    cursl = (*select)(&wr[i__], &wi[i__]);
	    if (wi[i__] == 0.) {
		if (cursl) {
		    ++(*sdim);
		}
		ip = 0;
		if (cursl && ! lastsl) {
		    *info = *n + 2;
		}
	    } else {
		if (ip == 1) {

/*                 Last eigenvalue of conjugate pair */

		    cursl = cursl || lastsl;
		    lastsl = cursl;
		    if (cursl) {
			*sdim += 2;
		    }
		    ip = -1;
		    if (cursl && ! lst2sl) {
			*info = *n + 2;
		    }
		} else {

/*                 First eigenvalue of conjugate pair */

		    ip = 1;
		}
	    }
	    lst2sl = lastsl;
	    lastsl = cursl;
/* L30: */
	}
    }

    work[1] = (double) maxwrk;
    if (wantsv || wantsb) {
/* Computing MAX */
	i__1 = 1, i__2 = *sdim * (*n - *sdim);
	iwork[1] = std::max(i__1,i__2);
    } else {
	iwork[1] = 1;
    }

    return 0;

/*     End of DGEESX */

} /* dgeesx_ */

/* Subroutine */ int dgeev_(const char *jobvl, const char *jobvr, integer *n, double *a, integer *lda, double *wr, double *wi,
	double *vl, integer *ldvl, double *vr, integer *ldvr, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__0 = 0;
	static integer c_n1 = -1;

    /* System generated locals */
    integer a_dim1, a_offset, vl_dim1, vl_offset, vr_dim1, vr_offset, i__1,
	    i__2, i__3;
    double d__1, d__2;

    /* Local variables */
    integer i__, k;
    double r__, cs, sn;
    integer ihi;
    double scl;
    integer ilo;
    double dum[1], eps;
    integer ibal;
    char side[1];
    double anrm;
    integer ierr, itau;
    integer iwrk, nout;
    bool scalea;
    double cscale;
    bool select[1];
    double bignum;
    integer minwrk, maxwrk;
    bool wantvl;
    double smlnum;
    integer hswork;
    bool lquery, wantvr;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGEEV computes for an N-by-N real nonsymmetric matrix A, the */
/*  eigenvalues and, optionally, the left and/or right eigenvectors. */

/*  The right eigenvector v(j) of A satisfies */
/*                   A * v(j) = lambda(j) * v(j) */
/*  where lambda(j) is its eigenvalue. */
/*  The left eigenvector u(j) of A satisfies */
/*                u(j)**H * A = lambda(j) * u(j)**H */
/*  where u(j)**H denotes the conjugate transpose of u(j). */

/*  The computed eigenvectors are normalized to have Euclidean norm */
/*  equal to 1 and largest component real. */

/*  Arguments */
/*  ========= */

/*  JOBVL   (input) CHARACTER*1 */
/*          = 'N': left eigenvectors of A are not computed; */
/*          = 'V': left eigenvectors of A are computed. */

/*  JOBVR   (input) CHARACTER*1 */
/*          = 'N': right eigenvectors of A are not computed; */
/*          = 'V': right eigenvectors of A are computed. */

/*  N       (input) INTEGER */
/*          The order of the matrix A. N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the N-by-N matrix A. */
/*          On exit, A has been overwritten. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  WR      (output) DOUBLE PRECISION array, dimension (N) */
/*  WI      (output) DOUBLE PRECISION array, dimension (N) */
/*          WR and WI contain the real and imaginary parts, */
/*          respectively, of the computed eigenvalues.  Complex */
/*          conjugate pairs of eigenvalues appear consecutively */
/*          with the eigenvalue having the positive imaginary part */
/*          first. */

/*  VL      (output) DOUBLE PRECISION array, dimension (LDVL,N) */
/*          If JOBVL = 'V', the left eigenvectors u(j) are stored one */
/*          after another in the columns of VL, in the same order */
/*          as their eigenvalues. */
/*          If JOBVL = 'N', VL is not referenced. */
/*          If the j-th eigenvalue is real, then u(j) = VL(:,j), */
/*          the j-th column of VL. */
/*          If the j-th and (j+1)-st eigenvalues form a complex */
/*          conjugate pair, then u(j) = VL(:,j) + i*VL(:,j+1) and */
/*          u(j+1) = VL(:,j) - i*VL(:,j+1). */

/*  LDVL    (input) INTEGER */
/*          The leading dimension of the array VL.  LDVL >= 1; if */
/*          JOBVL = 'V', LDVL >= N. */

/*  VR      (output) DOUBLE PRECISION array, dimension (LDVR,N) */
/*          If JOBVR = 'V', the right eigenvectors v(j) are stored one */
/*          after another in the columns of VR, in the same order */
/*          as their eigenvalues. */
/*          If JOBVR = 'N', VR is not referenced. */
/*          If the j-th eigenvalue is real, then v(j) = VR(:,j), */
/*          the j-th column of VR. */
/*          If the j-th and (j+1)-st eigenvalues form a complex */
/*          conjugate pair, then v(j) = VR(:,j) + i*VR(:,j+1) and */
/*          v(j+1) = VR(:,j) - i*VR(:,j+1). */

/*  LDVR    (input) INTEGER */
/*          The leading dimension of the array VR.  LDVR >= 1; if */
/*          JOBVR = 'V', LDVR >= N. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= max(1,3*N), and */
/*          if JOBVL = 'V' or JOBVR = 'V', LWORK >= 4*N.  For good */
/*          performance, LWORK must generally be larger. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = i, the QR algorithm failed to compute all the */
/*                eigenvalues, and no eigenvectors have been computed; */
/*                elements i+1:N of WR and WI contain eigenvalues which */
/*                have converged. */

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

/*     Test the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --wr;
    --wi;
    vl_dim1 = *ldvl;
    vl_offset = 1 + vl_dim1;
    vl -= vl_offset;
    vr_dim1 = *ldvr;
    vr_offset = 1 + vr_dim1;
    vr -= vr_offset;
    --work;

    /* Function Body */
    *info = 0;
    lquery = *lwork == -1;
    wantvl = lsame_(jobvl, "V");
    wantvr = lsame_(jobvr, "V");
    if (! wantvl && ! lsame_(jobvl, "N")) {
	*info = -1;
    } else if (! wantvr && ! lsame_(jobvr, "N")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    } else if (*ldvl < 1 || wantvl && *ldvl < *n) {
	*info = -9;
    } else if (*ldvr < 1 || wantvr && *ldvr < *n) {
	*info = -11;
    }

/*     Compute workspace */
/*      (Note: Comments in the code beginning "Workspace:" describe the */
/*       minimal amount of workspace needed at that point in the code, */
/*       as well as the preferred amount for good performance. */
/*       NB refers to the optimal block size for the immediately */
/*       following subroutine, as returned by ILAENV. */
/*       HSWORK refers to the workspace preferred by DHSEQR, as */
/*       calculated below. HSWORK is computed assuming ILO=1 and IHI=N, */
/*       the worst case.) */

    if (*info == 0) {
	if (*n == 0) {
	    minwrk = 1;
	    maxwrk = 1;
	} else {
	    maxwrk = (*n << 1) + *n * ilaenv_(&c__1, "DGEHRD", " ", n, &c__1,
		    n, &c__0);
	    if (wantvl) {
		minwrk = *n << 2;
/* Computing MAX */
		i__1 = maxwrk, i__2 = (*n << 1) + (*n - 1) * ilaenv_(&c__1,
			"DORGHR", " ", n, &c__1, n, &c_n1);
		maxwrk = std::max(i__1,i__2);
		dhseqr_("S", "V", n, &c__1, n, &a[a_offset], lda, &wr[1], &wi[
			1], &vl[vl_offset], ldvl, &work[1], &c_n1, info);
		hswork = (integer) work[1];
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n + 1, i__1 = std::max(i__1,i__2), i__2 = *
			n + hswork;
		maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n << 2;
		maxwrk = std::max(i__1,i__2);
	    } else if (wantvr) {
		minwrk = *n << 2;
/* Computing MAX */
		i__1 = maxwrk, i__2 = (*n << 1) + (*n - 1) * ilaenv_(&c__1,
			"DORGHR", " ", n, &c__1, n, &c_n1);
		maxwrk = std::max(i__1,i__2);
		dhseqr_("S", "V", n, &c__1, n, &a[a_offset], lda, &wr[1], &wi[
			1], &vr[vr_offset], ldvr, &work[1], &c_n1, info);
		hswork = (integer) work[1];
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n + 1, i__1 = std::max(i__1,i__2), i__2 = *
			n + hswork;
		maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n << 2;
		maxwrk = std::max(i__1,i__2);
	    } else {
		minwrk = *n * 3;
		dhseqr_("E", "N", n, &c__1, n, &a[a_offset], lda, &wr[1], &wi[
			1], &vr[vr_offset], ldvr, &work[1], &c_n1, info);
		hswork = (integer) work[1];
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n + 1, i__1 = std::max(i__1,i__2), i__2 = *
			n + hswork;
		maxwrk = std::max(i__1,i__2);
	    }
	    maxwrk = std::max(maxwrk,minwrk);
	}
	work[1] = (double) maxwrk;

	if (*lwork < minwrk && ! lquery) {
	    *info = -13;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEEV ", &i__1);
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
    smlnum = dlamch_("S");
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);
    smlnum = sqrt(smlnum) / eps;
    bignum = 1. / smlnum;

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", n, n, &a[a_offset], lda, dum);
    scalea = false;
    if (anrm > 0. && anrm < smlnum) {
	scalea = true;
	cscale = smlnum;
    } else if (anrm > bignum) {
	scalea = true;
	cscale = bignum;
    }
    if (scalea) {
	dlascl_("G", &c__0, &c__0, &anrm, &cscale, n, n, &a[a_offset], lda, &
		ierr);
    }

/*     Balance the matrix */
/*     (Workspace: need N) */

    ibal = 1;
    dgebal_("B", n, &a[a_offset], lda, &ilo, &ihi, &work[ibal], &ierr);

/*     Reduce to upper Hessenberg form */
/*     (Workspace: need 3*N, prefer 2*N+N*NB) */

    itau = ibal + *n;
    iwrk = itau + *n;
    i__1 = *lwork - iwrk + 1;
    dgehrd_(n, &ilo, &ihi, &a[a_offset], lda, &work[itau], &work[iwrk], &i__1,
	     &ierr);

    if (wantvl) {

/*        Want left eigenvectors */
/*        Copy Householder vectors to VL */

	*(unsigned char *)side = 'L';
	dlacpy_("L", n, n, &a[a_offset], lda, &vl[vl_offset], ldvl)
		;

/*        Generate orthogonal matrix in VL */
/*        (Workspace: need 3*N-1, prefer 2*N+(N-1)*NB) */

	i__1 = *lwork - iwrk + 1;
	dorghr_(n, &ilo, &ihi, &vl[vl_offset], ldvl, &work[itau], &work[iwrk],
		 &i__1, &ierr);

/*        Perform QR iteration, accumulating Schur vectors in VL */
/*        (Workspace: need N+1, prefer N+HSWORK (see comments) ) */

	iwrk = itau;
	i__1 = *lwork - iwrk + 1;
	dhseqr_("S", "V", n, &ilo, &ihi, &a[a_offset], lda, &wr[1], &wi[1], &
		vl[vl_offset], ldvl, &work[iwrk], &i__1, info);

	if (wantvr) {

/*           Want left and right eigenvectors */
/*           Copy Schur vectors to VR */

	    *(unsigned char *)side = 'B';
	    dlacpy_("F", n, n, &vl[vl_offset], ldvl, &vr[vr_offset], ldvr);
	}

    } else if (wantvr) {

/*        Want right eigenvectors */
/*        Copy Householder vectors to VR */

	*(unsigned char *)side = 'R';
	dlacpy_("L", n, n, &a[a_offset], lda, &vr[vr_offset], ldvr)
		;

/*        Generate orthogonal matrix in VR */
/*        (Workspace: need 3*N-1, prefer 2*N+(N-1)*NB) */

	i__1 = *lwork - iwrk + 1;
	dorghr_(n, &ilo, &ihi, &vr[vr_offset], ldvr, &work[itau], &work[iwrk],
		 &i__1, &ierr);

/*        Perform QR iteration, accumulating Schur vectors in VR */
/*        (Workspace: need N+1, prefer N+HSWORK (see comments) ) */

	iwrk = itau;
	i__1 = *lwork - iwrk + 1;
	dhseqr_("S", "V", n, &ilo, &ihi, &a[a_offset], lda, &wr[1], &wi[1], &
		vr[vr_offset], ldvr, &work[iwrk], &i__1, info);

    } else {

/*        Compute eigenvalues only */
/*        (Workspace: need N+1, prefer N+HSWORK (see comments) ) */

	iwrk = itau;
	i__1 = *lwork - iwrk + 1;
	dhseqr_("E", "N", n, &ilo, &ihi, &a[a_offset], lda, &wr[1], &wi[1], &
		vr[vr_offset], ldvr, &work[iwrk], &i__1, info);
    }

/*     If INFO > 0 from DHSEQR, then quit */

    if (*info > 0) {
	goto L50;
    }

    if (wantvl || wantvr) {

/*        Compute left and/or right eigenvectors */
/*        (Workspace: need 4*N) */

	dtrevc_(side, "B", select, n, &a[a_offset], lda, &vl[vl_offset], ldvl,
		 &vr[vr_offset], ldvr, n, &nout, &work[iwrk], &ierr);
    }

    if (wantvl) {

/*        Undo balancing of left eigenvectors */
/*        (Workspace: need N) */

	dgebak_("B", "L", n, &ilo, &ihi, &work[ibal], n, &vl[vl_offset], ldvl,
		 &ierr);

/*        Normalize left eigenvectors and make largest component real */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (wi[i__] == 0.) {
		scl = 1. / dnrm2_(n, &vl[i__ * vl_dim1 + 1], &c__1);
		dscal_(n, &scl, &vl[i__ * vl_dim1 + 1], &c__1);
	    } else if (wi[i__] > 0.) {
		d__1 = dnrm2_(n, &vl[i__ * vl_dim1 + 1], &c__1);
		d__2 = dnrm2_(n, &vl[(i__ + 1) * vl_dim1 + 1], &c__1);
		scl = 1. / dlapy2_(&d__1, &d__2);
		dscal_(n, &scl, &vl[i__ * vl_dim1 + 1], &c__1);
		dscal_(n, &scl, &vl[(i__ + 1) * vl_dim1 + 1], &c__1);
		i__2 = *n;
		for (k = 1; k <= i__2; ++k) {
/* Computing 2nd power */
		    d__1 = vl[k + i__ * vl_dim1];
/* Computing 2nd power */
		    d__2 = vl[k + (i__ + 1) * vl_dim1];
		    work[iwrk + k - 1] = d__1 * d__1 + d__2 * d__2;
/* L10: */
		}
		k = idamax_(n, &work[iwrk], &c__1);
		dlartg_(&vl[k + i__ * vl_dim1], &vl[k + (i__ + 1) * vl_dim1],
			&cs, &sn, &r__);
		drot_(n, &vl[i__ * vl_dim1 + 1], &c__1, &vl[(i__ + 1) *
			vl_dim1 + 1], &c__1, &cs, &sn);
		vl[k + (i__ + 1) * vl_dim1] = 0.;
	    }
/* L20: */
	}
    }

    if (wantvr) {

/*        Undo balancing of right eigenvectors */
/*        (Workspace: need N) */

	dgebak_("B", "R", n, &ilo, &ihi, &work[ibal], n, &vr[vr_offset], ldvr,
		 &ierr);

/*        Normalize right eigenvectors and make largest component real */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (wi[i__] == 0.) {
		scl = 1. / dnrm2_(n, &vr[i__ * vr_dim1 + 1], &c__1);
		dscal_(n, &scl, &vr[i__ * vr_dim1 + 1], &c__1);
	    } else if (wi[i__] > 0.) {
		d__1 = dnrm2_(n, &vr[i__ * vr_dim1 + 1], &c__1);
		d__2 = dnrm2_(n, &vr[(i__ + 1) * vr_dim1 + 1], &c__1);
		scl = 1. / dlapy2_(&d__1, &d__2);
		dscal_(n, &scl, &vr[i__ * vr_dim1 + 1], &c__1);
		dscal_(n, &scl, &vr[(i__ + 1) * vr_dim1 + 1], &c__1);
		i__2 = *n;
		for (k = 1; k <= i__2; ++k) {
/* Computing 2nd power */
		    d__1 = vr[k + i__ * vr_dim1];
/* Computing 2nd power */
		    d__2 = vr[k + (i__ + 1) * vr_dim1];
		    work[iwrk + k - 1] = d__1 * d__1 + d__2 * d__2;
/* L30: */
		}
		k = idamax_(n, &work[iwrk], &c__1);
		dlartg_(&vr[k + i__ * vr_dim1], &vr[k + (i__ + 1) * vr_dim1],
			&cs, &sn, &r__);
		drot_(n, &vr[i__ * vr_dim1 + 1], &c__1, &vr[(i__ + 1) *
			vr_dim1 + 1], &c__1, &cs, &sn);
		vr[k + (i__ + 1) * vr_dim1] = 0.;
	    }
/* L40: */
	}
    }

/*     Undo scaling if necessary */

L50:
    if (scalea) {
	i__1 = *n - *info;
/* Computing MAX */
	i__3 = *n - *info;
	i__2 = std::max(i__3,1_integer);
	dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wr[*info +
		1], &i__2, &ierr);
	i__1 = *n - *info;
/* Computing MAX */
	i__3 = *n - *info;
	i__2 = std::max(i__3,1_integer);
	dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wi[*info +
		1], &i__2, &ierr);
	if (*info > 0) {
	    i__1 = ilo - 1;
	    dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wr[1],
		    n, &ierr);
	    i__1 = ilo - 1;
	    dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wi[1],
		    n, &ierr);
	}
    }

    work[1] = (double) maxwrk;
    return 0;

/*     End of DGEEV */

} /* dgeev_ */

/* Subroutine */ int dgeevx_(const char *balanc, const char *jobvl, const char *jobvr, const char *
	sense, integer *n, double *a, integer *lda, double *wr,
	double *wi, double *vl, integer *ldvl, double *vr,
	integer *ldvr, integer *ilo, integer *ihi, double *scale,
	double *abnrm, double *rconde, double *rcondv, double
	*work, integer *lwork, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__0 = 0;
	static integer c_n1 = -1;

    /* System generated locals */
    integer a_dim1, a_offset, vl_dim1, vl_offset, vr_dim1, vr_offset, i__1,
	    i__2, i__3;
    double d__1, d__2;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    integer i__, k;
    double r__, cs, sn;
    char job[1];
    double scl, dum[1], eps;
    char side[1];
    double anrm;
    integer ierr, itau;
    integer iwrk, nout;
	integer icond;
    bool scalea;
    double cscale;
    bool select[1];
    double bignum;
	integer minwrk, maxwrk;
    bool wantvl, wntsnb;
    integer hswork;
    bool wntsne;
    double smlnum;
    bool lquery, wantvr, wntsnn, wntsnv;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGEEVX computes for an N-by-N real nonsymmetric matrix A, the */
/*  eigenvalues and, optionally, the left and/or right eigenvectors. */

/*  Optionally also, it computes a balancing transformation to improve */
/*  the conditioning of the eigenvalues and eigenvectors (ILO, IHI, */
/*  SCALE, and ABNRM), reciprocal condition numbers for the eigenvalues */
/*  (RCONDE), and reciprocal condition numbers for the right */
/*  eigenvectors (RCONDV). */

/*  The right eigenvector v(j) of A satisfies */
/*                   A * v(j) = lambda(j) * v(j) */
/*  where lambda(j) is its eigenvalue. */
/*  The left eigenvector u(j) of A satisfies */
/*                u(j)**H * A = lambda(j) * u(j)**H */
/*  where u(j)**H denotes the conjugate transpose of u(j). */

/*  The computed eigenvectors are normalized to have Euclidean norm */
/*  equal to 1 and largest component real. */

/*  Balancing a matrix means permuting the rows and columns to make it */
/*  more nearly upper triangular, and applying a diagonal similarity */
/*  transformation D * A * D**(-1), where D is a diagonal matrix, to */
/*  make its rows and columns closer in norm and the condition numbers */
/*  of its eigenvalues and eigenvectors smaller.  The computed */
/*  reciprocal condition numbers correspond to the balanced matrix. */
/*  Permuting rows and columns will not change the condition numbers */
/*  (in exact arithmetic) but diagonal scaling will.  For further */
/*  explanation of balancing, see section 4.10.2 of the LAPACK */
/*  Users' Guide. */

/*  Arguments */
/*  ========= */

/*  BALANC  (input) CHARACTER*1 */
/*          Indicates how the input matrix should be diagonally scaled */
/*          and/or permuted to improve the conditioning of its */
/*          eigenvalues. */
/*          = 'N': Do not diagonally scale or permute; */
/*          = 'P': Perform permutations to make the matrix more nearly */
/*                 upper triangular. Do not diagonally scale; */
/*          = 'S': Diagonally scale the matrix, i.e. replace A by */
/*                 D*A*D**(-1), where D is a diagonal matrix chosen */
/*                 to make the rows and columns of A more equal in */
/*                 norm. Do not permute; */
/*          = 'B': Both diagonally scale and permute A. */

/*          Computed reciprocal condition numbers will be for the matrix */
/*          after balancing and/or permuting. Permuting does not change */
/*          condition numbers (in exact arithmetic), but balancing does. */

/*  JOBVL   (input) CHARACTER*1 */
/*          = 'N': left eigenvectors of A are not computed; */
/*          = 'V': left eigenvectors of A are computed. */
/*          If SENSE = 'E' or 'B', JOBVL must = 'V'. */

/*  JOBVR   (input) CHARACTER*1 */
/*          = 'N': right eigenvectors of A are not computed; */
/*          = 'V': right eigenvectors of A are computed. */
/*          If SENSE = 'E' or 'B', JOBVR must = 'V'. */

/*  SENSE   (input) CHARACTER*1 */
/*          Determines which reciprocal condition numbers are computed. */
/*          = 'N': None are computed; */
/*          = 'E': Computed for eigenvalues only; */
/*          = 'V': Computed for right eigenvectors only; */
/*          = 'B': Computed for eigenvalues and right eigenvectors. */

/*          If SENSE = 'E' or 'B', both left and right eigenvectors */
/*          must also be computed (JOBVL = 'V' and JOBVR = 'V'). */

/*  N       (input) INTEGER */
/*          The order of the matrix A. N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the N-by-N matrix A. */
/*          On exit, A has been overwritten.  If JOBVL = 'V' or */
/*          JOBVR = 'V', A contains the real Schur form of the balanced */
/*          version of the input matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  WR      (output) DOUBLE PRECISION array, dimension (N) */
/*  WI      (output) DOUBLE PRECISION array, dimension (N) */
/*          WR and WI contain the real and imaginary parts, */
/*          respectively, of the computed eigenvalues.  Complex */
/*          conjugate pairs of eigenvalues will appear consecutively */
/*          with the eigenvalue having the positive imaginary part */
/*          first. */

/*  VL      (output) DOUBLE PRECISION array, dimension (LDVL,N) */
/*          If JOBVL = 'V', the left eigenvectors u(j) are stored one */
/*          after another in the columns of VL, in the same order */
/*          as their eigenvalues. */
/*          If JOBVL = 'N', VL is not referenced. */
/*          If the j-th eigenvalue is real, then u(j) = VL(:,j), */
/*          the j-th column of VL. */
/*          If the j-th and (j+1)-st eigenvalues form a complex */
/*          conjugate pair, then u(j) = VL(:,j) + i*VL(:,j+1) and */
/*          u(j+1) = VL(:,j) - i*VL(:,j+1). */

/*  LDVL    (input) INTEGER */
/*          The leading dimension of the array VL.  LDVL >= 1; if */
/*          JOBVL = 'V', LDVL >= N. */

/*  VR      (output) DOUBLE PRECISION array, dimension (LDVR,N) */
/*          If JOBVR = 'V', the right eigenvectors v(j) are stored one */
/*          after another in the columns of VR, in the same order */
/*          as their eigenvalues. */
/*          If JOBVR = 'N', VR is not referenced. */
/*          If the j-th eigenvalue is real, then v(j) = VR(:,j), */
/*          the j-th column of VR. */
/*          If the j-th and (j+1)-st eigenvalues form a complex */
/*          conjugate pair, then v(j) = VR(:,j) + i*VR(:,j+1) and */
/*          v(j+1) = VR(:,j) - i*VR(:,j+1). */

/*  LDVR    (input) INTEGER */
/*          The leading dimension of the array VR.  LDVR >= 1, and if */
/*          JOBVR = 'V', LDVR >= N. */

/*  ILO     (output) INTEGER */
/*  IHI     (output) INTEGER */
/*          ILO and IHI are integer values determined when A was */
/*          balanced.  The balanced A(i,j) = 0 if I > J and */
/*          J = 1,...,ILO-1 or I = IHI+1,...,N. */

/*  SCALE   (output) DOUBLE PRECISION array, dimension (N) */
/*          Details of the permutations and scaling factors applied */
/*          when balancing A.  If P(j) is the index of the row and column */
/*          interchanged with row and column j, and D(j) is the scaling */
/*          factor applied to row and column j, then */
/*          SCALE(J) = P(J),    for J = 1,...,ILO-1 */
/*                   = D(J),    for J = ILO,...,IHI */
/*                   = P(J)     for J = IHI+1,...,N. */
/*          The order in which the interchanges are made is N to IHI+1, */
/*          then 1 to ILO-1. */

/*  ABNRM   (output) DOUBLE PRECISION */
/*          The one-norm of the balanced matrix (the maximum */
/*          of the sum of absolute values of elements of any column). */

/*  RCONDE  (output) DOUBLE PRECISION array, dimension (N) */
/*          RCONDE(j) is the reciprocal condition number of the j-th */
/*          eigenvalue. */

/*  RCONDV  (output) DOUBLE PRECISION array, dimension (N) */
/*          RCONDV(j) is the reciprocal condition number of the j-th */
/*          right eigenvector. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.   If SENSE = 'N' or 'E', */
/*          LWORK >= max(1,2*N), and if JOBVL = 'V' or JOBVR = 'V', */
/*          LWORK >= 3*N.  If SENSE = 'V' or 'B', LWORK >= N*(N+6). */
/*          For good performance, LWORK must generally be larger. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  IWORK   (workspace) INTEGER array, dimension (2*N-2) */
/*          If SENSE = 'N' or 'E', not referenced. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = i, the QR algorithm failed to compute all the */
/*                eigenvalues, and no eigenvectors or condition numbers */
/*                have been computed; elements 1:ILO-1 and i+1:N of WR */
/*                and WI contain eigenvalues which have converged. */

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

/*     Test the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --wr;
    --wi;
    vl_dim1 = *ldvl;
    vl_offset = 1 + vl_dim1;
    vl -= vl_offset;
    vr_dim1 = *ldvr;
    vr_offset = 1 + vr_dim1;
    vr -= vr_offset;
    --scale;
    --rconde;
    --rcondv;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    lquery = *lwork == -1;
    wantvl = lsame_(jobvl, "V");
    wantvr = lsame_(jobvr, "V");
    wntsnn = lsame_(sense, "N");
    wntsne = lsame_(sense, "E");
    wntsnv = lsame_(sense, "V");
    wntsnb = lsame_(sense, "B");
    if (! (lsame_(balanc, "N") || lsame_(balanc, "S") || lsame_(balanc, "P")
	    || lsame_(balanc, "B"))) {
	*info = -1;
    } else if (! wantvl && ! lsame_(jobvl, "N")) {
	*info = -2;
    } else if (! wantvr && ! lsame_(jobvr, "N")) {
	*info = -3;
    } else if (! (wntsnn || wntsne || wntsnb || wntsnv) || (wntsne || wntsnb)
	    && ! (wantvl && wantvr)) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldvl < 1 || wantvl && *ldvl < *n) {
	*info = -11;
    } else if (*ldvr < 1 || wantvr && *ldvr < *n) {
	*info = -13;
    }

/*     Compute workspace */
/*      (Note: Comments in the code beginning "Workspace:" describe the */
/*       minimal amount of workspace needed at that point in the code, */
/*       as well as the preferred amount for good performance. */
/*       NB refers to the optimal block size for the immediately */
/*       following subroutine, as returned by ILAENV. */
/*       HSWORK refers to the workspace preferred by DHSEQR, as */
/*       calculated below. HSWORK is computed assuming ILO=1 and IHI=N, */
/*       the worst case.) */

    if (*info == 0) {
	if (*n == 0) {
	    minwrk = 1;
	    maxwrk = 1;
	} else {
	    maxwrk = *n + *n * ilaenv_(&c__1, "DGEHRD", " ", n, &c__1, n, &
		    c__0);

	    if (wantvl) {
		dhseqr_("S", "V", n, &c__1, n, &a[a_offset], lda, &wr[1], &wi[
			1], &vl[vl_offset], ldvl, &work[1], &c_n1, info);
	    } else if (wantvr) {
		dhseqr_("S", "V", n, &c__1, n, &a[a_offset], lda, &wr[1], &wi[
			1], &vr[vr_offset], ldvr, &work[1], &c_n1, info);
	    } else {
		if (wntsnn) {
		    dhseqr_("E", "N", n, &c__1, n, &a[a_offset], lda, &wr[1],
			    &wi[1], &vr[vr_offset], ldvr, &work[1], &c_n1,
			    info);
		} else {
		    dhseqr_("S", "N", n, &c__1, n, &a[a_offset], lda, &wr[1],
			    &wi[1], &vr[vr_offset], ldvr, &work[1], &c_n1,
			    info);
		}
	    }
	    hswork = (integer) work[1];

	    if (! wantvl && ! wantvr) {
		minwrk = *n << 1;
		if (! wntsnn) {
/* Computing MAX */
		    i__1 = minwrk, i__2 = *n * *n + *n * 6;
		    minwrk = std::max(i__1,i__2);
		}
		maxwrk = std::max(maxwrk,hswork);
		if (! wntsnn) {
/* Computing MAX */
		    i__1 = maxwrk, i__2 = *n * *n + *n * 6;
		    maxwrk = std::max(i__1,i__2);
		}
	    } else {
		minwrk = *n * 3;
		if (! wntsnn && ! wntsne) {
/* Computing MAX */
		    i__1 = minwrk, i__2 = *n * *n + *n * 6;
		    minwrk = std::max(i__1,i__2);
		}
		maxwrk = std::max(maxwrk,hswork);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n + (*n - 1) * ilaenv_(&c__1, "DORGHR",
			 " ", n, &c__1, n, &c_n1);
		maxwrk = std::max(i__1,i__2);
		if (! wntsnn && ! wntsne) {
/* Computing MAX */
		    i__1 = maxwrk, i__2 = *n * *n + *n * 6;
		    maxwrk = std::max(i__1,i__2);
		}
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n * 3;
		maxwrk = std::max(i__1,i__2);
	    }
	    maxwrk = std::max(maxwrk,minwrk);
	}
	work[1] = (double) maxwrk;

	if (*lwork < minwrk && ! lquery) {
	    *info = -21;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEEVX", &i__1);
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
    smlnum = dlamch_("S");
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);
    smlnum = sqrt(smlnum) / eps;
    bignum = 1. / smlnum;

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    icond = 0;
    anrm = dlange_("M", n, n, &a[a_offset], lda, dum);
    scalea = false;
    if (anrm > 0. && anrm < smlnum) {
	scalea = true;
	cscale = smlnum;
    } else if (anrm > bignum) {
	scalea = true;
	cscale = bignum;
    }
    if (scalea) {
	dlascl_("G", &c__0, &c__0, &anrm, &cscale, n, n, &a[a_offset], lda, &
		ierr);
    }

/*     Balance the matrix and compute ABNRM */

    dgebal_(balanc, n, &a[a_offset], lda, ilo, ihi, &scale[1], &ierr);
    *abnrm = dlange_("1", n, n, &a[a_offset], lda, dum);
    if (scalea) {
	dum[0] = *abnrm;
	dlascl_("G", &c__0, &c__0, &cscale, &anrm, &c__1, &c__1, dum, &c__1, &
		ierr);
	*abnrm = dum[0];
    }

/*     Reduce to upper Hessenberg form */
/*     (Workspace: need 2*N, prefer N+N*NB) */

    itau = 1;
    iwrk = itau + *n;
    i__1 = *lwork - iwrk + 1;
    dgehrd_(n, ilo, ihi, &a[a_offset], lda, &work[itau], &work[iwrk], &i__1, &
	    ierr);

    if (wantvl) {

/*        Want left eigenvectors */
/*        Copy Householder vectors to VL */

	*(unsigned char *)side = 'L';
	dlacpy_("L", n, n, &a[a_offset], lda, &vl[vl_offset], ldvl)
		;

/*        Generate orthogonal matrix in VL */
/*        (Workspace: need 2*N-1, prefer N+(N-1)*NB) */

	i__1 = *lwork - iwrk + 1;
	dorghr_(n, ilo, ihi, &vl[vl_offset], ldvl, &work[itau], &work[iwrk], &
		i__1, &ierr);

/*        Perform QR iteration, accumulating Schur vectors in VL */
/*        (Workspace: need 1, prefer HSWORK (see comments) ) */

	iwrk = itau;
	i__1 = *lwork - iwrk + 1;
	dhseqr_("S", "V", n, ilo, ihi, &a[a_offset], lda, &wr[1], &wi[1], &vl[
		vl_offset], ldvl, &work[iwrk], &i__1, info);

	if (wantvr) {

/*           Want left and right eigenvectors */
/*           Copy Schur vectors to VR */

	    *(unsigned char *)side = 'B';
	    dlacpy_("F", n, n, &vl[vl_offset], ldvl, &vr[vr_offset], ldvr);
	}

    } else if (wantvr) {

/*        Want right eigenvectors */
/*        Copy Householder vectors to VR */

	*(unsigned char *)side = 'R';
	dlacpy_("L", n, n, &a[a_offset], lda, &vr[vr_offset], ldvr)
		;

/*        Generate orthogonal matrix in VR */
/*        (Workspace: need 2*N-1, prefer N+(N-1)*NB) */

	i__1 = *lwork - iwrk + 1;
	dorghr_(n, ilo, ihi, &vr[vr_offset], ldvr, &work[itau], &work[iwrk], &
		i__1, &ierr);

/*        Perform QR iteration, accumulating Schur vectors in VR */
/*        (Workspace: need 1, prefer HSWORK (see comments) ) */

	iwrk = itau;
	i__1 = *lwork - iwrk + 1;
	dhseqr_("S", "V", n, ilo, ihi, &a[a_offset], lda, &wr[1], &wi[1], &vr[
		vr_offset], ldvr, &work[iwrk], &i__1, info);

    } else {

/*        Compute eigenvalues only */
/*        If condition numbers desired, compute Schur form */

	if (wntsnn) {
	    *(unsigned char *)job = 'E';
	} else {
	    *(unsigned char *)job = 'S';
	}

/*        (Workspace: need 1, prefer HSWORK (see comments) ) */

	iwrk = itau;
	i__1 = *lwork - iwrk + 1;
	dhseqr_(job, "N", n, ilo, ihi, &a[a_offset], lda, &wr[1], &wi[1], &vr[
		vr_offset], ldvr, &work[iwrk], &i__1, info);
    }

/*     If INFO > 0 from DHSEQR, then quit */

    if (*info > 0) {
	goto L50;
    }

    if (wantvl || wantvr) {

/*        Compute left and/or right eigenvectors */
/*        (Workspace: need 3*N) */

	dtrevc_(side, "B", select, n, &a[a_offset], lda, &vl[vl_offset], ldvl,
		 &vr[vr_offset], ldvr, n, &nout, &work[iwrk], &ierr);
    }

/*     Compute condition numbers if desired */
/*     (Workspace: need N*N+6*N unless SENSE = 'E') */

    if (! wntsnn) {
	dtrsna_(sense, "A", select, n, &a[a_offset], lda, &vl[vl_offset],
		ldvl, &vr[vr_offset], ldvr, &rconde[1], &rcondv[1], n, &nout,
		&work[iwrk], n, &iwork[1], &icond);
    }

    if (wantvl) {

/*        Undo balancing of left eigenvectors */

	dgebak_(balanc, "L", n, ilo, ihi, &scale[1], n, &vl[vl_offset], ldvl,
		&ierr);

/*        Normalize left eigenvectors and make largest component real */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (wi[i__] == 0.) {
		scl = 1. / dnrm2_(n, &vl[i__ * vl_dim1 + 1], &c__1);
		dscal_(n, &scl, &vl[i__ * vl_dim1 + 1], &c__1);
	    } else if (wi[i__] > 0.) {
		d__1 = dnrm2_(n, &vl[i__ * vl_dim1 + 1], &c__1);
		d__2 = dnrm2_(n, &vl[(i__ + 1) * vl_dim1 + 1], &c__1);
		scl = 1. / dlapy2_(&d__1, &d__2);
		dscal_(n, &scl, &vl[i__ * vl_dim1 + 1], &c__1);
		dscal_(n, &scl, &vl[(i__ + 1) * vl_dim1 + 1], &c__1);
		i__2 = *n;
		for (k = 1; k <= i__2; ++k) {
/* Computing 2nd power */
		    d__1 = vl[k + i__ * vl_dim1];
/* Computing 2nd power */
		    d__2 = vl[k + (i__ + 1) * vl_dim1];
		    work[k] = d__1 * d__1 + d__2 * d__2;
/* L10: */
		}
		k = idamax_(n, &work[1], &c__1);
		dlartg_(&vl[k + i__ * vl_dim1], &vl[k + (i__ + 1) * vl_dim1],
			&cs, &sn, &r__);
		drot_(n, &vl[i__ * vl_dim1 + 1], &c__1, &vl[(i__ + 1) *
			vl_dim1 + 1], &c__1, &cs, &sn);
		vl[k + (i__ + 1) * vl_dim1] = 0.;
	    }
/* L20: */
	}
    }

    if (wantvr) {

/*        Undo balancing of right eigenvectors */

	dgebak_(balanc, "R", n, ilo, ihi, &scale[1], n, &vr[vr_offset], ldvr,
		&ierr);

/*        Normalize right eigenvectors and make largest component real */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (wi[i__] == 0.) {
		scl = 1. / dnrm2_(n, &vr[i__ * vr_dim1 + 1], &c__1);
		dscal_(n, &scl, &vr[i__ * vr_dim1 + 1], &c__1);
	    } else if (wi[i__] > 0.) {
		d__1 = dnrm2_(n, &vr[i__ * vr_dim1 + 1], &c__1);
		d__2 = dnrm2_(n, &vr[(i__ + 1) * vr_dim1 + 1], &c__1);
		scl = 1. / dlapy2_(&d__1, &d__2);
		dscal_(n, &scl, &vr[i__ * vr_dim1 + 1], &c__1);
		dscal_(n, &scl, &vr[(i__ + 1) * vr_dim1 + 1], &c__1);
		i__2 = *n;
		for (k = 1; k <= i__2; ++k) {
/* Computing 2nd power */
		    d__1 = vr[k + i__ * vr_dim1];
/* Computing 2nd power */
		    d__2 = vr[k + (i__ + 1) * vr_dim1];
		    work[k] = d__1 * d__1 + d__2 * d__2;
/* L30: */
		}
		k = idamax_(n, &work[1], &c__1);
		dlartg_(&vr[k + i__ * vr_dim1], &vr[k + (i__ + 1) * vr_dim1],
			&cs, &sn, &r__);
		drot_(n, &vr[i__ * vr_dim1 + 1], &c__1, &vr[(i__ + 1) *
			vr_dim1 + 1], &c__1, &cs, &sn);
		vr[k + (i__ + 1) * vr_dim1] = 0.;
	    }
/* L40: */
	}
    }

/*     Undo scaling if necessary */

L50:
    if (scalea) {
	i__1 = *n - *info;
/* Computing MAX */
	i__3 = *n - *info;
	i__2 = std::max(i__3,1_integer);
	dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wr[*info +
		1], &i__2, &ierr);
	i__1 = *n - *info;
/* Computing MAX */
	i__3 = *n - *info;
	i__2 = std::max(i__3,1_integer);
	dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wi[*info +
		1], &i__2, &ierr);
	if (*info == 0) {
	    if ((wntsnv || wntsnb) && icond == 0) {
		dlascl_("G", &c__0, &c__0, &cscale, &anrm, n, &c__1, &rcondv[
			1], n, &ierr);
	    }
	} else {
	    i__1 = *ilo - 1;
	    dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wr[1],
		    n, &ierr);
	    i__1 = *ilo - 1;
	    dlascl_("G", &c__0, &c__0, &cscale, &anrm, &i__1, &c__1, &wi[1],
		    n, &ierr);
	}
    }

    work[1] = (double) maxwrk;
    return 0;

/*     End of DGEEVX */

} /* dgeevx_ */

/* Subroutine */ int dgegs_(const char *jobvsl, const char *jobvsr, integer *n,
	double *a, integer *lda, double *b, integer *ldb, double *
	alphar, double *alphai, double *beta, double *vsl,
	integer *ldvsl, double *vsr, integer *ldvsr, double *work,
	integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static double c_b36 = 0.;
	static double c_b37 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, vsl_dim1, vsl_offset,
	    vsr_dim1, vsr_offset, i__1, i__2;

    /* Local variables */
    integer nb, nb1, nb2, nb3, ihi, ilo;
    double eps, anrm, bnrm;
    integer itau, lopt;
    integer ileft, iinfo, icols;
    bool ilvsl;
    integer iwork;
    bool ilvsr;
    integer irows;
    bool ilascl, ilbscl;
    double safmin;
    double bignum;
    integer ijobvl, iright, ijobvr;
    double anrmto;
    integer lwkmin;
    double bnrmto;
    double smlnum;
    integer lwkopt;
    bool lquery;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  This routine is deprecated and has been replaced by routine DGGES. */

/*  DGEGS computes the eigenvalues, real Schur form, and, optionally, */
/*  left and or/right Schur vectors of a real matrix pair (A,B). */
/*  Given two square matrices A and B, the generalized real Schur */
/*  factorization has the form */

/*    A = Q*S*Z**T,  B = Q*T*Z**T */

/*  where Q and Z are orthogonal matrices, T is upper triangular, and S */
/*  is an upper quasi-triangular matrix with 1-by-1 and 2-by-2 diagonal */
/*  blocks, the 2-by-2 blocks corresponding to complex conjugate pairs */
/*  of eigenvalues of (A,B).  The columns of Q are the left Schur vectors */
/*  and the columns of Z are the right Schur vectors. */

/*  If only the eigenvalues of (A,B) are needed, the driver routine */
/*  DGEGV should be used instead.  See DGEGV for a description of the */
/*  eigenvalues of the generalized nonsymmetric eigenvalue problem */
/*  (GNEP). */

/*  Arguments */
/*  ========= */

/*  JOBVSL  (input) CHARACTER*1 */
/*          = 'N':  do not compute the left Schur vectors; */
/*          = 'V':  compute the left Schur vectors (returned in VSL). */

/*  JOBVSR  (input) CHARACTER*1 */
/*          = 'N':  do not compute the right Schur vectors; */
/*          = 'V':  compute the right Schur vectors (returned in VSR). */

/*  N       (input) INTEGER */
/*          The order of the matrices A, B, VSL, and VSR.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the matrix A. */
/*          On exit, the upper quasi-triangular matrix S from the */
/*          generalized real Schur factorization. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  LDA >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB, N) */
/*          On entry, the matrix B. */
/*          On exit, the upper triangular matrix T from the generalized */
/*          real Schur factorization. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  LDB >= max(1,N). */

/*  ALPHAR  (output) DOUBLE PRECISION array, dimension (N) */
/*          The real parts of each scalar alpha defining an eigenvalue */
/*          of GNEP. */

/*  ALPHAI  (output) DOUBLE PRECISION array, dimension (N) */
/*          The imaginary parts of each scalar alpha defining an */
/*          eigenvalue of GNEP.  If ALPHAI(j) is zero, then the j-th */
/*          eigenvalue is real; if positive, then the j-th and (j+1)-st */
/*          eigenvalues are a complex conjugate pair, with */
/*          ALPHAI(j+1) = -ALPHAI(j). */

/*  BETA    (output) DOUBLE PRECISION array, dimension (N) */
/*          The scalars beta that define the eigenvalues of GNEP. */
/*          Together, the quantities alpha = (ALPHAR(j),ALPHAI(j)) and */
/*          beta = BETA(j) represent the j-th eigenvalue of the matrix */
/*          pair (A,B), in one of the forms lambda = alpha/beta or */
/*          mu = beta/alpha.  Since either lambda or mu may overflow, */
/*          they should not, in general, be computed. */

/*  VSL     (output) DOUBLE PRECISION array, dimension (LDVSL,N) */
/*          If JOBVSL = 'V', the matrix of left Schur vectors Q. */
/*          Not referenced if JOBVSL = 'N'. */

/*  LDVSL   (input) INTEGER */
/*          The leading dimension of the matrix VSL. LDVSL >=1, and */
/*          if JOBVSL = 'V', LDVSL >= N. */

/*  VSR     (output) DOUBLE PRECISION array, dimension (LDVSR,N) */
/*          If JOBVSR = 'V', the matrix of right Schur vectors Z. */
/*          Not referenced if JOBVSR = 'N'. */

/*  LDVSR   (input) INTEGER */
/*          The leading dimension of the matrix VSR. LDVSR >= 1, and */
/*          if JOBVSR = 'V', LDVSR >= N. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= max(1,4*N). */
/*          For good performance, LWORK must generally be larger. */
/*          To compute the optimal value of LWORK, call ILAENV to get */
/*          blocksizes (for DGEQRF, DORMQR, and DORGQR.)  Then compute: */
/*          NB  -- MAX of the blocksizes for DGEQRF, DORMQR, and DORGQR */
/*          The optimal LWORK is  2*N + N*(NB+1). */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          = 1,...,N: */
/*                The QZ iteration failed.  (A,B) are not in Schur */
/*                form, but ALPHAR(j), ALPHAI(j), and BETA(j) should */
/*                be correct for j=INFO+1,...,N. */
/*          > N:  errors that usually indicate LAPACK problems: */
/*                =N+1: error return from DGGBAL */
/*                =N+2: error return from DGEQRF */
/*                =N+3: error return from DORMQR */
/*                =N+4: error return from DORGQR */
/*                =N+5: error return from DGGHRD */
/*                =N+6: error return from DHGEQZ (other than failed */
/*                                                iteration) */
/*                =N+7: error return from DGGBAK (computing VSL) */
/*                =N+8: error return from DGGBAK (computing VSR) */
/*                =N+9: error return from DLASCL (various places) */

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

/*     Decode the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --alphar;
    --alphai;
    --beta;
    vsl_dim1 = *ldvsl;
    vsl_offset = 1 + vsl_dim1;
    vsl -= vsl_offset;
    vsr_dim1 = *ldvsr;
    vsr_offset = 1 + vsr_dim1;
    vsr -= vsr_offset;
    --work;

    /* Function Body */
    if (lsame_(jobvsl, "N")) {
	ijobvl = 1;
	ilvsl = false;
    } else if (lsame_(jobvsl, "V")) {
	ijobvl = 2;
	ilvsl = true;
    } else {
	ijobvl = -1;
	ilvsl = false;
    }

    if (lsame_(jobvsr, "N")) {
	ijobvr = 1;
	ilvsr = false;
    } else if (lsame_(jobvsr, "V")) {
	ijobvr = 2;
	ilvsr = true;
    } else {
	ijobvr = -1;
	ilvsr = false;
    }

/*     Test the input arguments */

/* Computing MAX */
    i__1 = *n << 2;
    lwkmin = std::max(i__1,1_integer);
    lwkopt = lwkmin;
    work[1] = (double) lwkopt;
    lquery = *lwork == -1;
    *info = 0;
    if (ijobvl <= 0) {
	*info = -1;
    } else if (ijobvr <= 0) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldvsl < 1 || ilvsl && *ldvsl < *n) {
	*info = -12;
    } else if (*ldvsr < 1 || ilvsr && *ldvsr < *n) {
	*info = -14;
    } else if (*lwork < lwkmin && ! lquery) {
	*info = -16;
    }

    if (*info == 0) {
	nb1 = ilaenv_(&c__1, "DGEQRF", " ", n, n, &c_n1, &c_n1);
	nb2 = ilaenv_(&c__1, "DORMQR", " ", n, n, n, &c_n1);
	nb3 = ilaenv_(&c__1, "DORGQR", " ", n, n, n, &c_n1);
/* Computing MAX */
	i__1 = std::max(nb1,nb2);
	nb = std::max(i__1,nb3);
	lopt = (*n << 1) + *n * (nb + 1);
	work[1] = (double) lopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEGS ", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Get machine constants */

    eps = dlamch_("E") * dlamch_("B");
    safmin = dlamch_("S");
    smlnum = *n * safmin / eps;
    bignum = 1. / smlnum;

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", n, n, &a[a_offset], lda, &work[1]);
    ilascl = false;
    if (anrm > 0. && anrm < smlnum) {
	anrmto = smlnum;
	ilascl = true;
    } else if (anrm > bignum) {
	anrmto = bignum;
	ilascl = true;
    }

    if (ilascl) {
	dlascl_("G", &c_n1, &c_n1, &anrm, &anrmto, n, n, &a[a_offset], lda, &
		iinfo);
	if (iinfo != 0) {
	    *info = *n + 9;
	    return 0;
	}
    }

/*     Scale B if max element outside range [SMLNUM,BIGNUM] */

    bnrm = dlange_("M", n, n, &b[b_offset], ldb, &work[1]);
    ilbscl = false;
    if (bnrm > 0. && bnrm < smlnum) {
	bnrmto = smlnum;
	ilbscl = true;
    } else if (bnrm > bignum) {
	bnrmto = bignum;
	ilbscl = true;
    }

    if (ilbscl) {
	dlascl_("G", &c_n1, &c_n1, &bnrm, &bnrmto, n, n, &b[b_offset], ldb, &
		iinfo);
	if (iinfo != 0) {
	    *info = *n + 9;
	    return 0;
	}
    }

/*     Permute the matrix to make it more nearly triangular */
/*     Workspace layout:  (2*N words -- "work..." not actually used) */
/*        left_permutation, right_permutation, work... */

    ileft = 1;
    iright = *n + 1;
    iwork = iright + *n;
    dggbal_("P", n, &a[a_offset], lda, &b[b_offset], ldb, &ilo, &ihi, &work[
	    ileft], &work[iright], &work[iwork], &iinfo);
    if (iinfo != 0) {
	*info = *n + 1;
	goto L10;
    }

/*     Reduce B to triangular form, and initialize VSL and/or VSR */
/*     Workspace layout:  ("work..." must have at least N words) */
/*        left_permutation, right_permutation, tau, work... */

    irows = ihi + 1 - ilo;
    icols = *n + 1 - ilo;
    itau = iwork;
    iwork = itau + irows;
    i__1 = *lwork + 1 - iwork;
    dgeqrf_(&irows, &icols, &b[ilo + ilo * b_dim1], ldb, &work[itau], &work[
	    iwork], &i__1, &iinfo);
    if (iinfo >= 0) {
/* Computing MAX */
	i__1 = lwkopt, i__2 = (integer) work[iwork] + iwork - 1;
	lwkopt = std::max(i__1,i__2);
    }
    if (iinfo != 0) {
	*info = *n + 2;
	goto L10;
    }

    i__1 = *lwork + 1 - iwork;
    dormqr_("L", "T", &irows, &icols, &irows, &b[ilo + ilo * b_dim1], ldb, &
	    work[itau], &a[ilo + ilo * a_dim1], lda, &work[iwork], &i__1, &
	    iinfo);
    if (iinfo >= 0) {
/* Computing MAX */
	i__1 = lwkopt, i__2 = (integer) work[iwork] + iwork - 1;
	lwkopt = std::max(i__1,i__2);
    }
    if (iinfo != 0) {
	*info = *n + 3;
	goto L10;
    }

    if (ilvsl) {
	dlaset_("Full", n, n, &c_b36, &c_b37, &vsl[vsl_offset], ldvsl);
	i__1 = irows - 1;
	i__2 = irows - 1;
	dlacpy_("L", &i__1, &i__2, &b[ilo + 1 + ilo * b_dim1], ldb, &vsl[ilo
		+ 1 + ilo * vsl_dim1], ldvsl);
	i__1 = *lwork + 1 - iwork;
	dorgqr_(&irows, &irows, &irows, &vsl[ilo + ilo * vsl_dim1], ldvsl, &
		work[itau], &work[iwork], &i__1, &iinfo);
	if (iinfo >= 0) {
/* Computing MAX */
	    i__1 = lwkopt, i__2 = (integer) work[iwork] + iwork - 1;
	    lwkopt = std::max(i__1,i__2);
	}
	if (iinfo != 0) {
	    *info = *n + 4;
	    goto L10;
	}
    }

    if (ilvsr) {
	dlaset_("Full", n, n, &c_b36, &c_b37, &vsr[vsr_offset], ldvsr);
    }

/*     Reduce to generalized Hessenberg form */

    dgghrd_(jobvsl, jobvsr, n, &ilo, &ihi, &a[a_offset], lda, &b[b_offset],
	    ldb, &vsl[vsl_offset], ldvsl, &vsr[vsr_offset], ldvsr, &iinfo);
    if (iinfo != 0) {
	*info = *n + 5;
	goto L10;
    }

/*     Perform QZ algorithm, computing Schur vectors if desired */
/*     Workspace layout:  ("work..." must have at least 1 word) */
/*        left_permutation, right_permutation, work... */

    iwork = itau;
    i__1 = *lwork + 1 - iwork;
    dhgeqz_("S", jobvsl, jobvsr, n, &ilo, &ihi, &a[a_offset], lda, &b[
	    b_offset], ldb, &alphar[1], &alphai[1], &beta[1], &vsl[vsl_offset]
, ldvsl, &vsr[vsr_offset], ldvsr, &work[iwork], &i__1, &iinfo);
    if (iinfo >= 0) {
/* Computing MAX */
	i__1 = lwkopt, i__2 = (integer) work[iwork] + iwork - 1;
	lwkopt = std::max(i__1,i__2);
    }
    if (iinfo != 0) {
	if (iinfo > 0 && iinfo <= *n) {
	    *info = iinfo;
	} else if (iinfo > *n && iinfo <= *n << 1) {
	    *info = iinfo - *n;
	} else {
	    *info = *n + 6;
	}
	goto L10;
    }

/*     Apply permutation to VSL and VSR */

    if (ilvsl) {
	dggbak_("P", "L", n, &ilo, &ihi, &work[ileft], &work[iright], n, &vsl[
		vsl_offset], ldvsl, &iinfo);
	if (iinfo != 0) {
	    *info = *n + 7;
	    goto L10;
	}
    }
    if (ilvsr) {
	dggbak_("P", "R", n, &ilo, &ihi, &work[ileft], &work[iright], n, &vsr[
		vsr_offset], ldvsr, &iinfo);
	if (iinfo != 0) {
	    *info = *n + 8;
	    goto L10;
	}
    }

/*     Undo scaling */

    if (ilascl) {
	dlascl_("H", &c_n1, &c_n1, &anrmto, &anrm, n, n, &a[a_offset], lda, &
		iinfo);
	if (iinfo != 0) {
	    *info = *n + 9;
	    return 0;
	}
	dlascl_("G", &c_n1, &c_n1, &anrmto, &anrm, n, &c__1, &alphar[1], n, &
		iinfo);
	if (iinfo != 0) {
	    *info = *n + 9;
	    return 0;
	}
	dlascl_("G", &c_n1, &c_n1, &anrmto, &anrm, n, &c__1, &alphai[1], n, &
		iinfo);
	if (iinfo != 0) {
	    *info = *n + 9;
	    return 0;
	}
    }

    if (ilbscl) {
	dlascl_("U", &c_n1, &c_n1, &bnrmto, &bnrm, n, n, &b[b_offset], ldb, &
		iinfo);
	if (iinfo != 0) {
	    *info = *n + 9;
	    return 0;
	}
	dlascl_("G", &c_n1, &c_n1, &bnrmto, &bnrm, n, &c__1, &beta[1], n, &
		iinfo);
	if (iinfo != 0) {
	    *info = *n + 9;
	    return 0;
	}
    }

L10:
    work[1] = (double) lwkopt;

    return 0;

/*     End of DGEGS */

} /* dgegs_ */

/* Subroutine */ int dgegv_(const char *jobvl, const char *jobvr, integer *n, double *
	a, integer *lda, double *b, integer *ldb, double *alphar,
	double *alphai, double *beta, double *vl, integer *ldvl,
	double *vr, integer *ldvr, double *work, integer *lwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static double c_b27 = 1.;
	static double c_b38 = 0.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, vl_dim1, vl_offset, vr_dim1,
	    vr_offset, i__1, i__2;
    double d__1, d__2, d__3, d__4;

    /* Local variables */
    integer jc, nb, in, jr, nb1, nb2, nb3, ihi, ilo;
    double eps;
    bool ilv;
    double absb, anrm, bnrm;
    integer itau;
    double temp;
    bool ilvl, ilvr;
    integer lopt;
    double anrm1, anrm2, bnrm1, bnrm2, absai, scale, absar, sbeta;
    integer ileft, iinfo, icols, iwork, irows;
    double salfai;
    double salfar;
    double safmin;
    double safmax;
    char chtemp[1];
    bool ldumma[1];
    integer ijobvl, iright;
    bool ilimit;
    integer ijobvr;
    double onepls;
    integer lwkmin;
    integer lwkopt;
    bool lquery;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  This routine is deprecated and has been replaced by routine DGGEV. */

/*  DGEGV computes the eigenvalues and, optionally, the left and/or right */
/*  eigenvectors of a real matrix pair (A,B). */
/*  Given two square matrices A and B, */
/*  the generalized nonsymmetric eigenvalue problem (GNEP) is to find the */
/*  eigenvalues lambda and corresponding (non-zero) eigenvectors x such */
/*  that */

/*     A*x = lambda*B*x. */

/*  An alternate form is to find the eigenvalues mu and corresponding */
/*  eigenvectors y such that */

/*     mu*A*y = B*y. */

/*  These two forms are equivalent with mu = 1/lambda and x = y if */
/*  neither lambda nor mu is zero.  In order to deal with the case that */
/*  lambda or mu is zero or small, two values alpha and beta are returned */
/*  for each eigenvalue, such that lambda = alpha/beta and */
/*  mu = beta/alpha. */

/*  The vectors x and y in the above equations are right eigenvectors of */
/*  the matrix pair (A,B).  Vectors u and v satisfying */

/*     u**H*A = lambda*u**H*B  or  mu*v**H*A = v**H*B */

/*  are left eigenvectors of (A,B). */

/*  Note: this routine performs "full balancing" on A and B -- see */
/*  "Further Details", below. */

/*  Arguments */
/*  ========= */

/*  JOBVL   (input) CHARACTER*1 */
/*          = 'N':  do not compute the left generalized eigenvectors; */
/*          = 'V':  compute the left generalized eigenvectors (returned */
/*                  in VL). */

/*  JOBVR   (input) CHARACTER*1 */
/*          = 'N':  do not compute the right generalized eigenvectors; */
/*          = 'V':  compute the right generalized eigenvectors (returned */
/*                  in VR). */

/*  N       (input) INTEGER */
/*          The order of the matrices A, B, VL, and VR.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the matrix A. */
/*          If JOBVL = 'V' or JOBVR = 'V', then on exit A */
/*          contains the real Schur form of A from the generalized Schur */
/*          factorization of the pair (A,B) after balancing. */
/*          If no eigenvectors were computed, then only the diagonal */
/*          blocks from the Schur form will be correct.  See DGGHRD and */
/*          DHGEQZ for details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  LDA >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB, N) */
/*          On entry, the matrix B. */
/*          If JOBVL = 'V' or JOBVR = 'V', then on exit B contains the */
/*          upper triangular matrix obtained from B in the generalized */
/*          Schur factorization of the pair (A,B) after balancing. */
/*          If no eigenvectors were computed, then only those elements of */
/*          B corresponding to the diagonal blocks from the Schur form of */
/*          A will be correct.  See DGGHRD and DHGEQZ for details. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  LDB >= max(1,N). */

/*  ALPHAR  (output) DOUBLE PRECISION array, dimension (N) */
/*          The real parts of each scalar alpha defining an eigenvalue of */
/*          GNEP. */

/*  ALPHAI  (output) DOUBLE PRECISION array, dimension (N) */
/*          The imaginary parts of each scalar alpha defining an */
/*          eigenvalue of GNEP.  If ALPHAI(j) is zero, then the j-th */
/*          eigenvalue is real; if positive, then the j-th and */
/*          (j+1)-st eigenvalues are a complex conjugate pair, with */
/*          ALPHAI(j+1) = -ALPHAI(j). */

/*  BETA    (output) DOUBLE PRECISION array, dimension (N) */
/*          The scalars beta that define the eigenvalues of GNEP. */

/*          Together, the quantities alpha = (ALPHAR(j),ALPHAI(j)) and */
/*          beta = BETA(j) represent the j-th eigenvalue of the matrix */
/*          pair (A,B), in one of the forms lambda = alpha/beta or */
/*          mu = beta/alpha.  Since either lambda or mu may overflow, */
/*          they should not, in general, be computed. */

/*  VL      (output) DOUBLE PRECISION array, dimension (LDVL,N) */
/*          If JOBVL = 'V', the left eigenvectors u(j) are stored */
/*          in the columns of VL, in the same order as their eigenvalues. */
/*          If the j-th eigenvalue is real, then u(j) = VL(:,j). */
/*          If the j-th and (j+1)-st eigenvalues form a complex conjugate */
/*          pair, then */
/*             u(j) = VL(:,j) + i*VL(:,j+1) */
/*          and */
/*            u(j+1) = VL(:,j) - i*VL(:,j+1). */

/*          Each eigenvector is scaled so that its largest component has */
/*          abs(real part) + abs(imag. part) = 1, except for eigenvectors */
/*          corresponding to an eigenvalue with alpha = beta = 0, which */
/*          are set to zero. */
/*          Not referenced if JOBVL = 'N'. */

/*  LDVL    (input) INTEGER */
/*          The leading dimension of the matrix VL. LDVL >= 1, and */
/*          if JOBVL = 'V', LDVL >= N. */

/*  VR      (output) DOUBLE PRECISION array, dimension (LDVR,N) */
/*          If JOBVR = 'V', the right eigenvectors x(j) are stored */
/*          in the columns of VR, in the same order as their eigenvalues. */
/*          If the j-th eigenvalue is real, then x(j) = VR(:,j). */
/*          If the j-th and (j+1)-st eigenvalues form a complex conjugate */
/*          pair, then */
/*            x(j) = VR(:,j) + i*VR(:,j+1) */
/*          and */
/*            x(j+1) = VR(:,j) - i*VR(:,j+1). */

/*          Each eigenvector is scaled so that its largest component has */
/*          abs(real part) + abs(imag. part) = 1, except for eigenvalues */
/*          corresponding to an eigenvalue with alpha = beta = 0, which */
/*          are set to zero. */
/*          Not referenced if JOBVR = 'N'. */

/*  LDVR    (input) INTEGER */
/*          The leading dimension of the matrix VR. LDVR >= 1, and */
/*          if JOBVR = 'V', LDVR >= N. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= max(1,8*N). */
/*          For good performance, LWORK must generally be larger. */
/*          To compute the optimal value of LWORK, call ILAENV to get */
/*          blocksizes (for DGEQRF, DORMQR, and DORGQR.)  Then compute: */
/*          NB  -- MAX of the blocksizes for DGEQRF, DORMQR, and DORGQR; */
/*          The optimal LWORK is: */
/*              2*N + MAX( 6*N, N*(NB+1) ). */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          = 1,...,N: */
/*                The QZ iteration failed.  No eigenvectors have been */
/*                calculated, but ALPHAR(j), ALPHAI(j), and BETA(j) */
/*                should be correct for j=INFO+1,...,N. */
/*          > N:  errors that usually indicate LAPACK problems: */
/*                =N+1: error return from DGGBAL */
/*                =N+2: error return from DGEQRF */
/*                =N+3: error return from DORMQR */
/*                =N+4: error return from DORGQR */
/*                =N+5: error return from DGGHRD */
/*                =N+6: error return from DHGEQZ (other than failed */
/*                                                iteration) */
/*                =N+7: error return from DTGEVC */
/*                =N+8: error return from DGGBAK (computing VL) */
/*                =N+9: error return from DGGBAK (computing VR) */
/*                =N+10: error return from DLASCL (various calls) */

/*  Further Details */
/*  =============== */

/*  Balancing */
/*  --------- */

/*  This driver calls DGGBAL to both permute and scale rows and columns */
/*  of A and B.  The permutations PL and PR are chosen so that PL*A*PR */
/*  and PL*B*R will be upper triangular except for the diagonal blocks */
/*  A(i:j,i:j) and B(i:j,i:j), with i and j as close together as */
/*  possible.  The diagonal scaling matrices DL and DR are chosen so */
/*  that the pair  DL*PL*A*PR*DR, DL*PL*B*PR*DR have elements close to */
/*  one (except for the elements that start out zero.) */

/*  After the eigenvalues and eigenvectors of the balanced matrices */
/*  have been computed, DGGBAK transforms the eigenvectors back to what */
/*  they would have been (in perfect arithmetic) if they had not been */
/*  balanced. */

/*  Contents of A and B on Exit */
/*  -------- -- - --- - -- ---- */

/*  If any eigenvectors are computed (either JOBVL='V' or JOBVR='V' or */
/*  both), then on exit the arrays A and B will contain the real Schur */
/*  form[*] of the "balanced" versions of A and B.  If no eigenvectors */
/*  are computed, then only the diagonal blocks will be correct. */

/*  [*] See DHGEQZ, DGEGS, or read the book "Matrix Computations", */
/*      by Golub & van Loan, pub. by Johns Hopkins U. Press. */

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

/*     Decode the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --alphar;
    --alphai;
    --beta;
    vl_dim1 = *ldvl;
    vl_offset = 1 + vl_dim1;
    vl -= vl_offset;
    vr_dim1 = *ldvr;
    vr_offset = 1 + vr_dim1;
    vr -= vr_offset;
    --work;

    /* Function Body */
    if (lsame_(jobvl, "N")) {
	ijobvl = 1;
	ilvl = false;
    } else if (lsame_(jobvl, "V")) {
	ijobvl = 2;
	ilvl = true;
    } else {
	ijobvl = -1;
	ilvl = false;
    }

    if (lsame_(jobvr, "N")) {
	ijobvr = 1;
	ilvr = false;
    } else if (lsame_(jobvr, "V")) {
	ijobvr = 2;
	ilvr = true;
    } else {
	ijobvr = -1;
	ilvr = false;
    }
    ilv = ilvl || ilvr;

/*     Test the input arguments */

/* Computing MAX */
    i__1 = *n << 3;
    lwkmin = std::max(i__1,1_integer);
    lwkopt = lwkmin;
    work[1] = (double) lwkopt;
    lquery = *lwork == -1;
    *info = 0;
    if (ijobvl <= 0) {
	*info = -1;
    } else if (ijobvr <= 0) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldvl < 1 || ilvl && *ldvl < *n) {
	*info = -12;
    } else if (*ldvr < 1 || ilvr && *ldvr < *n) {
	*info = -14;
    } else if (*lwork < lwkmin && ! lquery) {
	*info = -16;
    }

    if (*info == 0) {
	nb1 = ilaenv_(&c__1, "DGEQRF", " ", n, n, &c_n1, &c_n1);
	nb2 = ilaenv_(&c__1, "DORMQR", " ", n, n, n, &c_n1);
	nb3 = ilaenv_(&c__1, "DORGQR", " ", n, n, n, &c_n1);
/* Computing MAX */
	i__1 = std::max(nb1,nb2);
	nb = std::max(i__1,nb3);
/* Computing MAX */
	i__1 = *n * 6, i__2 = *n * (nb + 1);
	lopt = (*n << 1) + std::max(i__1,i__2);
	work[1] = (double) lopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEGV ", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Get machine constants */

    eps = dlamch_("E") * dlamch_("B");
    safmin = dlamch_("S");
    safmin += safmin;
    safmax = 1. / safmin;
    onepls = eps * 4 + 1.;

/*     Scale A */

    anrm = dlange_("M", n, n, &a[a_offset], lda, &work[1]);
    anrm1 = anrm;
    anrm2 = 1.;
    if (anrm < 1.) {
	if (safmax * anrm < 1.) {
	    anrm1 = safmin;
	    anrm2 = safmax * anrm;
	}
    }

    if (anrm > 0.) {
	dlascl_("G", &c_n1, &c_n1, &anrm, &c_b27, n, n, &a[a_offset], lda, &
		iinfo);
	if (iinfo != 0) {
	    *info = *n + 10;
	    return 0;
	}
    }

/*     Scale B */

    bnrm = dlange_("M", n, n, &b[b_offset], ldb, &work[1]);
    bnrm1 = bnrm;
    bnrm2 = 1.;
    if (bnrm < 1.) {
	if (safmax * bnrm < 1.) {
	    bnrm1 = safmin;
	    bnrm2 = safmax * bnrm;
	}
    }

    if (bnrm > 0.) {
	dlascl_("G", &c_n1, &c_n1, &bnrm, &c_b27, n, n, &b[b_offset], ldb, &
		iinfo);
	if (iinfo != 0) {
	    *info = *n + 10;
	    return 0;
	}
    }

/*     Permute the matrix to make it more nearly triangular */
/*     Workspace layout:  (8*N words -- "work" requires 6*N words) */
/*        left_permutation, right_permutation, work... */

    ileft = 1;
    iright = *n + 1;
    iwork = iright + *n;
    dggbal_("P", n, &a[a_offset], lda, &b[b_offset], ldb, &ilo, &ihi, &work[
	    ileft], &work[iright], &work[iwork], &iinfo);
    if (iinfo != 0) {
	*info = *n + 1;
	goto L120;
    }

/*     Reduce B to triangular form, and initialize VL and/or VR */
/*     Workspace layout:  ("work..." must have at least N words) */
/*        left_permutation, right_permutation, tau, work... */

    irows = ihi + 1 - ilo;
    if (ilv) {
	icols = *n + 1 - ilo;
    } else {
	icols = irows;
    }
    itau = iwork;
    iwork = itau + irows;
    i__1 = *lwork + 1 - iwork;
    dgeqrf_(&irows, &icols, &b[ilo + ilo * b_dim1], ldb, &work[itau], &work[
	    iwork], &i__1, &iinfo);
    if (iinfo >= 0) {
/* Computing MAX */
	i__1 = lwkopt, i__2 = (integer) work[iwork] + iwork - 1;
	lwkopt = std::max(i__1,i__2);
    }
    if (iinfo != 0) {
	*info = *n + 2;
	goto L120;
    }

    i__1 = *lwork + 1 - iwork;
    dormqr_("L", "T", &irows, &icols, &irows, &b[ilo + ilo * b_dim1], ldb, &
	    work[itau], &a[ilo + ilo * a_dim1], lda, &work[iwork], &i__1, &
	    iinfo);
    if (iinfo >= 0) {
/* Computing MAX */
	i__1 = lwkopt, i__2 = (integer) work[iwork] + iwork - 1;
	lwkopt = std::max(i__1,i__2);
    }
    if (iinfo != 0) {
	*info = *n + 3;
	goto L120;
    }

    if (ilvl) {
	dlaset_("Full", n, n, &c_b38, &c_b27, &vl[vl_offset], ldvl)
		;
	i__1 = irows - 1;
	i__2 = irows - 1;
	dlacpy_("L", &i__1, &i__2, &b[ilo + 1 + ilo * b_dim1], ldb, &vl[ilo +
		1 + ilo * vl_dim1], ldvl);
	i__1 = *lwork + 1 - iwork;
	dorgqr_(&irows, &irows, &irows, &vl[ilo + ilo * vl_dim1], ldvl, &work[
		itau], &work[iwork], &i__1, &iinfo);
	if (iinfo >= 0) {
/* Computing MAX */
	    i__1 = lwkopt, i__2 = (integer) work[iwork] + iwork - 1;
	    lwkopt = std::max(i__1,i__2);
	}
	if (iinfo != 0) {
	    *info = *n + 4;
	    goto L120;
	}
    }

    if (ilvr) {
	dlaset_("Full", n, n, &c_b38, &c_b27, &vr[vr_offset], ldvr)
		;
    }

/*     Reduce to generalized Hessenberg form */

    if (ilv) {

/*        Eigenvectors requested -- work on whole matrix. */

	dgghrd_(jobvl, jobvr, n, &ilo, &ihi, &a[a_offset], lda, &b[b_offset],
		ldb, &vl[vl_offset], ldvl, &vr[vr_offset], ldvr, &iinfo);
    } else {
	dgghrd_("N", "N", &irows, &c__1, &irows, &a[ilo + ilo * a_dim1], lda,
		&b[ilo + ilo * b_dim1], ldb, &vl[vl_offset], ldvl, &vr[
		vr_offset], ldvr, &iinfo);
    }
    if (iinfo != 0) {
	*info = *n + 5;
	goto L120;
    }

/*     Perform QZ algorithm */
/*     Workspace layout:  ("work..." must have at least 1 word) */
/*        left_permutation, right_permutation, work... */

    iwork = itau;
    if (ilv) {
	*(unsigned char *)chtemp = 'S';
    } else {
	*(unsigned char *)chtemp = 'E';
    }
    i__1 = *lwork + 1 - iwork;
    dhgeqz_(chtemp, jobvl, jobvr, n, &ilo, &ihi, &a[a_offset], lda, &b[
	    b_offset], ldb, &alphar[1], &alphai[1], &beta[1], &vl[vl_offset],
	    ldvl, &vr[vr_offset], ldvr, &work[iwork], &i__1, &iinfo);
    if (iinfo >= 0) {
/* Computing MAX */
	i__1 = lwkopt, i__2 = (integer) work[iwork] + iwork - 1;
	lwkopt = std::max(i__1,i__2);
    }
    if (iinfo != 0) {
	if (iinfo > 0 && iinfo <= *n) {
	    *info = iinfo;
	} else if (iinfo > *n && iinfo <= *n << 1) {
	    *info = iinfo - *n;
	} else {
	    *info = *n + 6;
	}
	goto L120;
    }

    if (ilv) {

/*        Compute Eigenvectors  (DTGEVC requires 6*N words of workspace) */

	if (ilvl) {
	    if (ilvr) {
		*(unsigned char *)chtemp = 'B';
	    } else {
		*(unsigned char *)chtemp = 'L';
	    }
	} else {
	    *(unsigned char *)chtemp = 'R';
	}

	dtgevc_(chtemp, "B", ldumma, n, &a[a_offset], lda, &b[b_offset], ldb,
		&vl[vl_offset], ldvl, &vr[vr_offset], ldvr, n, &in, &work[
		iwork], &iinfo);
	if (iinfo != 0) {
	    *info = *n + 7;
	    goto L120;
	}

/*        Undo balancing on VL and VR, rescale */

	if (ilvl) {
	    dggbak_("P", "L", n, &ilo, &ihi, &work[ileft], &work[iright], n, &
		    vl[vl_offset], ldvl, &iinfo);
	    if (iinfo != 0) {
		*info = *n + 8;
		goto L120;
	    }
	    i__1 = *n;
	    for (jc = 1; jc <= i__1; ++jc) {
		if (alphai[jc] < 0.) {
		    goto L50;
		}
		temp = 0.;
		if (alphai[jc] == 0.) {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
/* Computing MAX */
			d__2 = temp, d__3 = (d__1 = vl[jr + jc * vl_dim1],
				abs(d__1));
			temp = std::max(d__2,d__3);
/* L10: */
		    }
		} else {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
/* Computing MAX */
			d__3 = temp, d__4 = (d__1 = vl[jr + jc * vl_dim1],
				abs(d__1)) + (d__2 = vl[jr + (jc + 1) *
				vl_dim1], abs(d__2));
			temp = std::max(d__3,d__4);
/* L20: */
		    }
		}
		if (temp < safmin) {
		    goto L50;
		}
		temp = 1. / temp;
		if (alphai[jc] == 0.) {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
			vl[jr + jc * vl_dim1] *= temp;
/* L30: */
		    }
		} else {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
			vl[jr + jc * vl_dim1] *= temp;
			vl[jr + (jc + 1) * vl_dim1] *= temp;
/* L40: */
		    }
		}
L50:
		;
	    }
	}
	if (ilvr) {
	    dggbak_("P", "R", n, &ilo, &ihi, &work[ileft], &work[iright], n, &
		    vr[vr_offset], ldvr, &iinfo);
	    if (iinfo != 0) {
		*info = *n + 9;
		goto L120;
	    }
	    i__1 = *n;
	    for (jc = 1; jc <= i__1; ++jc) {
		if (alphai[jc] < 0.) {
		    goto L100;
		}
		temp = 0.;
		if (alphai[jc] == 0.) {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
/* Computing MAX */
			d__2 = temp, d__3 = (d__1 = vr[jr + jc * vr_dim1],
				abs(d__1));
			temp = std::max(d__2,d__3);
/* L60: */
		    }
		} else {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
/* Computing MAX */
			d__3 = temp, d__4 = (d__1 = vr[jr + jc * vr_dim1],
				abs(d__1)) + (d__2 = vr[jr + (jc + 1) *
				vr_dim1], abs(d__2));
			temp = std::max(d__3,d__4);
/* L70: */
		    }
		}
		if (temp < safmin) {
		    goto L100;
		}
		temp = 1. / temp;
		if (alphai[jc] == 0.) {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
			vr[jr + jc * vr_dim1] *= temp;
/* L80: */
		    }
		} else {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
			vr[jr + jc * vr_dim1] *= temp;
			vr[jr + (jc + 1) * vr_dim1] *= temp;
/* L90: */
		    }
		}
L100:
		;
	    }
	}

/*        End of eigenvector calculation */

    }

/*     Undo scaling in alpha, beta */

/*     Note: this does not give the alpha and beta for the unscaled */
/*     problem. */

/*     Un-scaling is limited to avoid underflow in alpha and beta */
/*     if they are significant. */

    i__1 = *n;
    for (jc = 1; jc <= i__1; ++jc) {
	absar = (d__1 = alphar[jc], abs(d__1));
	absai = (d__1 = alphai[jc], abs(d__1));
	absb = (d__1 = beta[jc], abs(d__1));
	salfar = anrm * alphar[jc];
	salfai = anrm * alphai[jc];
	sbeta = bnrm * beta[jc];
	ilimit = false;
	scale = 1.;

/*        Check for significant underflow in ALPHAI */

/* Computing MAX */
	d__1 = safmin, d__2 = eps * absar, d__1 = std::max(d__1,d__2), d__2 = eps *
		 absb;
	if (abs(salfai) < safmin && absai >= std::max(d__1,d__2)) {
	    ilimit = true;
/* Computing MAX */
	    d__1 = onepls * safmin, d__2 = anrm2 * absai;
	    scale = onepls * safmin / anrm1 / std::max(d__1,d__2);

	} else if (salfai == 0.) {

/*           If insignificant underflow in ALPHAI, then make the */
/*           conjugate eigenvalue real. */

	    if (alphai[jc] < 0. && jc > 1) {
		alphai[jc - 1] = 0.;
	    } else if (alphai[jc] > 0. && jc < *n) {
		alphai[jc + 1] = 0.;
	    }
	}

/*        Check for significant underflow in ALPHAR */

/* Computing MAX */
	d__1 = safmin, d__2 = eps * absai, d__1 = std::max(d__1,d__2), d__2 = eps *
		 absb;
	if (abs(salfar) < safmin && absar >= std::max(d__1,d__2)) {
	    ilimit = true;
/* Computing MAX */
/* Computing MAX */
	    d__3 = onepls * safmin, d__4 = anrm2 * absar;
	    d__1 = scale, d__2 = onepls * safmin / anrm1 / std::max(d__3,d__4);
	    scale = std::max(d__1,d__2);
	}

/*        Check for significant underflow in BETA */

/* Computing MAX */
	d__1 = safmin, d__2 = eps * absar, d__1 = std::max(d__1,d__2), d__2 = eps *
		 absai;
	if (abs(sbeta) < safmin && absb >= std::max(d__1,d__2)) {
	    ilimit = true;
/* Computing MAX */
/* Computing MAX */
	    d__3 = onepls * safmin, d__4 = bnrm2 * absb;
	    d__1 = scale, d__2 = onepls * safmin / bnrm1 / std::max(d__3,d__4);
	    scale = std::max(d__1,d__2);
	}

/*        Check for possible overflow when limiting scaling */

	if (ilimit) {
/* Computing MAX */
	    d__1 = abs(salfar), d__2 = abs(salfai), d__1 = std::max(d__1,d__2),
		    d__2 = abs(sbeta);
	    temp = scale * safmin * std::max(d__1,d__2);
	    if (temp > 1.) {
		scale /= temp;
	    }
	    if (scale < 1.) {
		ilimit = false;
	    }
	}

/*        Recompute un-scaled ALPHAR, ALPHAI, BETA if necessary. */

	if (ilimit) {
	    salfar = scale * alphar[jc] * anrm;
	    salfai = scale * alphai[jc] * anrm;
	    sbeta = scale * beta[jc] * bnrm;
	}
	alphar[jc] = salfar;
	alphai[jc] = salfai;
	beta[jc] = sbeta;
/* L110: */
    }

L120:
    work[1] = (double) lwkopt;

    return 0;

/*     End of DGEGV */

} /* dgegv_ */

/* Subroutine */ int dgehd2_(integer *n, integer *ilo, integer *ihi,
	double *a, integer *lda, double *tau, double *work,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    integer i__;
    double aii;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGEHD2 reduces a real general matrix A to upper Hessenberg form H by */
/*  an orthogonal similarity transformation:  Q' * A * Q = H . */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  ILO     (input) INTEGER */
/*  IHI     (input) INTEGER */
/*          It is assumed that A is already upper triangular in rows */
/*          and columns 1:ILO-1 and IHI+1:N. ILO and IHI are normally */
/*          set by a previous call to DGEBAL; otherwise they should be */
/*          set to 1 and N respectively. See Further Details. */
/*          1 <= ILO <= IHI <= max(1,N). */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the n by n general matrix to be reduced. */
/*          On exit, the upper triangle and the first subdiagonal of A */
/*          are overwritten with the upper Hessenberg matrix H, and the */
/*          elements below the first subdiagonal, with the array TAU, */
/*          represent the orthogonal matrix Q as a product of elementary */
/*          reflectors. See Further Details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (N-1) */
/*          The scalar factors of the elementary reflectors (see Further */
/*          Details). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of (ihi-ilo) elementary */
/*  reflectors */

/*     Q = H(ilo) H(ilo+1) . . . H(ihi-1). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(1:i) = 0, v(i+1) = 1 and v(ihi+1:n) = 0; v(i+2:ihi) is stored on */
/*  exit in A(i+2:ihi,i), and tau in TAU(i). */

/*  The contents of A are illustrated by the following example, with */
/*  n = 7, ilo = 2 and ihi = 6: */

/*  on entry,                        on exit, */

/*  ( a   a   a   a   a   a   a )    (  a   a   h   h   h   h   a ) */
/*  (     a   a   a   a   a   a )    (      a   h   h   h   h   a ) */
/*  (     a   a   a   a   a   a )    (      h   h   h   h   h   h ) */
/*  (     a   a   a   a   a   a )    (      v2  h   h   h   h   h ) */
/*  (     a   a   a   a   a   a )    (      v2  v3  h   h   h   h ) */
/*  (     a   a   a   a   a   a )    (      v2  v3  v4  h   h   h ) */
/*  (                         a )    (                          a ) */

/*  where a denotes an element of the original matrix A, h denotes a */
/*  modified element of the upper Hessenberg matrix H, and vi denotes an */
/*  element of the vector defining H(i). */

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

/*     Test the input parameters */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*n < 0) {
	*info = -1;
    } else if (*ilo < 1 || *ilo > std::max(1_integer,*n)) {
	*info = -2;
    } else if (*ihi < std::min(*ilo,*n) || *ihi > *n) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEHD2", &i__1);
	return 0;
    }

    i__1 = *ihi - 1;
    for (i__ = *ilo; i__ <= i__1; ++i__) {

/*        Compute elementary reflector H(i) to annihilate A(i+2:ihi,i) */

	i__2 = *ihi - i__;
/* Computing MIN */
	i__3 = i__ + 2;
	dlarfg_(&i__2, &a[i__ + 1 + i__ * a_dim1], &a[std::min(i__3, *n)+ i__ *
		a_dim1], &c__1, &tau[i__]);
	aii = a[i__ + 1 + i__ * a_dim1];
	a[i__ + 1 + i__ * a_dim1] = 1.;

/*        Apply H(i) to A(1:ihi,i+1:ihi) from the right */

	i__2 = *ihi - i__;
	dlarf_("Right", ihi, &i__2, &a[i__ + 1 + i__ * a_dim1], &c__1, &tau[
		i__], &a[(i__ + 1) * a_dim1 + 1], lda, &work[1]);

/*        Apply H(i) to A(i+1:ihi,i+1:n) from the left */

	i__2 = *ihi - i__;
	i__3 = *n - i__;
	dlarf_("Left", &i__2, &i__3, &a[i__ + 1 + i__ * a_dim1], &c__1, &tau[
		i__], &a[i__ + 1 + (i__ + 1) * a_dim1], lda, &work[1]);

	a[i__ + 1 + i__ * a_dim1] = aii;
/* L10: */
    }

    return 0;

/*     End of DGEHD2 */

} /* dgehd2_ */

/* Subroutine */ int dgehrd_(integer *n, integer *ilo, integer *ihi,
	double *a, integer *lda, double *tau, double *work,
	integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__3 = 3;
	static integer c__2 = 2;
	static integer c__65 = 65;
	static double c_b25 = -1.;
	static double c_b26 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    integer i__, j;
    double t[4160]	/* was [65][64] */;
    integer ib;
    double ei;
    integer nb, nh, nx, iws;
    integer nbmin, iinfo;
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

/*  DGEHRD reduces a real general matrix A to upper Hessenberg form H by */
/*  an orthogonal similarity transformation:  Q' * A * Q = H . */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  ILO     (input) INTEGER */
/*  IHI     (input) INTEGER */
/*          It is assumed that A is already upper triangular in rows */
/*          and columns 1:ILO-1 and IHI+1:N. ILO and IHI are normally */
/*          set by a previous call to DGEBAL; otherwise they should be */
/*          set to 1 and N respectively. See Further Details. */
/*          1 <= ILO <= IHI <= N, if N > 0; ILO=1 and IHI=0, if N=0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the N-by-N general matrix to be reduced. */
/*          On exit, the upper triangle and the first subdiagonal of A */
/*          are overwritten with the upper Hessenberg matrix H, and the */
/*          elements below the first subdiagonal, with the array TAU, */
/*          represent the orthogonal matrix Q as a product of elementary */
/*          reflectors. See Further Details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (N-1) */
/*          The scalar factors of the elementary reflectors (see Further */
/*          Details). Elements 1:ILO-1 and IHI:N-1 of TAU are set to */
/*          zero. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= max(1,N). */
/*          For optimum performance LWORK >= N*NB, where NB is the */
/*          optimal blocksize. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of (ihi-ilo) elementary */
/*  reflectors */

/*     Q = H(ilo) H(ilo+1) . . . H(ihi-1). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(1:i) = 0, v(i+1) = 1 and v(ihi+1:n) = 0; v(i+2:ihi) is stored on */
/*  exit in A(i+2:ihi,i), and tau in TAU(i). */

/*  The contents of A are illustrated by the following example, with */
/*  n = 7, ilo = 2 and ihi = 6: */

/*  on entry,                        on exit, */

/*  ( a   a   a   a   a   a   a )    (  a   a   h   h   h   h   a ) */
/*  (     a   a   a   a   a   a )    (      a   h   h   h   h   a ) */
/*  (     a   a   a   a   a   a )    (      h   h   h   h   h   h ) */
/*  (     a   a   a   a   a   a )    (      v2  h   h   h   h   h ) */
/*  (     a   a   a   a   a   a )    (      v2  v3  h   h   h   h ) */
/*  (     a   a   a   a   a   a )    (      v2  v3  v4  h   h   h ) */
/*  (                         a )    (                          a ) */

/*  where a denotes an element of the original matrix A, h denotes a */
/*  modified element of the upper Hessenberg matrix H, and vi denotes an */
/*  element of the vector defining H(i). */

/*  This file is a slight modification of LAPACK-3.0's DGEHRD */
/*  subroutine incorporating improvements proposed by Quintana-Orti and */
/*  Van de Geijn (2005). */

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

/*     Test the input parameters */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
/* Computing MIN */
    i__1 = 64, i__2 = ilaenv_(&c__1, "DGEHRD", " ", n, ilo, ihi, &c_n1);
    nb = std::min(i__1,i__2);
    lwkopt = *n * nb;
    work[1] = (double) lwkopt;
    lquery = *lwork == -1;
    if (*n < 0) {
	*info = -1;
    } else if (*ilo < 1 || *ilo > std::max(1_integer,*n)) {
	*info = -2;
    } else if (*ihi < std::min(*ilo,*n) || *ihi > *n) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    } else if (*lwork < std::max(1_integer,*n) && ! lquery) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEHRD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Set elements 1:ILO-1 and IHI:N-1 of TAU to zero */

    i__1 = *ilo - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	tau[i__] = 0.;
/* L10: */
    }
    i__1 = *n - 1;
    for (i__ = std::max(1_integer,*ihi); i__ <= i__1; ++i__) {
	tau[i__] = 0.;
/* L20: */
    }

/*     Quick return if possible */

    nh = *ihi - *ilo + 1;
    if (nh <= 1) {
	work[1] = 1.;
	return 0;
    }

/*     Determine the block size */

/* Computing MIN */
    i__1 = 64, i__2 = ilaenv_(&c__1, "DGEHRD", " ", n, ilo, ihi, &c_n1);
    nb = std::min(i__1,i__2);
    nbmin = 2;
    iws = 1;
    if (nb > 1 && nb < nh) {

/*        Determine when to cross over from blocked to unblocked code */
/*        (last block is always handled by unblocked code) */

/* Computing MAX */
	i__1 = nb, i__2 = ilaenv_(&c__3, "DGEHRD", " ", n, ilo, ihi, &c_n1);
	nx = std::max(i__1,i__2);
	if (nx < nh) {

/*           Determine if workspace is large enough for blocked code */

	    iws = *n * nb;
	    if (*lwork < iws) {

/*              Not enough workspace to use optimal NB:  determine the */
/*              minimum value of NB, and reduce NB or force use of */
/*              unblocked code */

/* Computing MAX */
		i__1 = 2, i__2 = ilaenv_(&c__2, "DGEHRD", " ", n, ilo, ihi, &
			c_n1);
		nbmin = std::max(i__1,i__2);
		if (*lwork >= *n * nbmin) {
		    nb = *lwork / *n;
		} else {
		    nb = 1;
		}
	    }
	}
    }
    ldwork = *n;

    if (nb < nbmin || nb >= nh) {

/*        Use unblocked code below */

	i__ = *ilo;

    } else {

/*        Use blocked code */

	i__1 = *ihi - 1 - nx;
	i__2 = nb;
	for (i__ = *ilo; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
	    i__3 = nb, i__4 = *ihi - i__;
	    ib = std::min(i__3,i__4);

/*           Reduce columns i:i+ib-1 to Hessenberg form, returning the */
/*           matrices V and T of the block reflector H = I - V*T*V' */
/*           which performs the reduction, and also the matrix Y = A*V*T */

	    dlahr2_(ihi, &i__, &ib, &a[i__ * a_dim1 + 1], lda, &tau[i__], t, &
		    c__65, &work[1], &ldwork);

/*           Apply the block reflector H to A(1:ihi,i+ib:ihi) from the */
/*           right, computing  A := A - Y * V'. V(i+ib,ib-1) must be set */
/*           to 1 */

	    ei = a[i__ + ib + (i__ + ib - 1) * a_dim1];
	    a[i__ + ib + (i__ + ib - 1) * a_dim1] = 1.;
	    i__3 = *ihi - i__ - ib + 1;
	    dgemm_("No transpose", "Transpose", ihi, &i__3, &ib, &c_b25, &
		    work[1], &ldwork, &a[i__ + ib + i__ * a_dim1], lda, &
		    c_b26, &a[(i__ + ib) * a_dim1 + 1], lda);
	    a[i__ + ib + (i__ + ib - 1) * a_dim1] = ei;

/*           Apply the block reflector H to A(1:i,i+1:i+ib-1) from the */
/*           right */

	    i__3 = ib - 1;
	    dtrmm_("Right", "Lower", "Transpose", "Unit", &i__, &i__3, &c_b26,
		     &a[i__ + 1 + i__ * a_dim1], lda, &work[1], &ldwork);
	    i__3 = ib - 2;
	    for (j = 0; j <= i__3; ++j) {
		daxpy_(&i__, &c_b25, &work[ldwork * j + 1], &c__1, &a[(i__ +
			j + 1) * a_dim1 + 1], &c__1);
/* L30: */
	    }

/*           Apply the block reflector H to A(i+1:ihi,i+ib:n) from the */
/*           left */

	    i__3 = *ihi - i__;
	    i__4 = *n - i__ - ib + 1;
	    dlarfb_("Left", "Transpose", "Forward", "Columnwise", &i__3, &
		    i__4, &ib, &a[i__ + 1 + i__ * a_dim1], lda, t, &c__65, &a[
		    i__ + 1 + (i__ + ib) * a_dim1], lda, &work[1], &ldwork);
/* L40: */
	}
    }

/*     Use unblocked code to reduce the rest of the matrix */

    dgehd2_(n, &i__, ihi, &a[a_offset], lda, &tau[1], &work[1], &iinfo);
    work[1] = (double) iws;

    return 0;

/*     End of DGEHRD */

} /* dgehrd_ */

/* Subroutine */ int dgelq2_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    integer i__, k;
    double aii;

/*  -- LAPACK routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGELQ2 computes an LQ factorization of a real m by n matrix A: */
/*  A = L * Q. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the m by n matrix A. */
/*          On exit, the elements on and below the diagonal of the array */
/*          contain the m by min(m,n) lower trapezoidal matrix L (L is */
/*          lower triangular if m <= n); the elements above the diagonal, */
/*          with the array TAU, represent the orthogonal matrix Q as a */
/*          product of elementary reflectors (see Further Details). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors (see Further */
/*          Details). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (M) */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of elementary reflectors */

/*     Q = H(k) . . . H(2) H(1), where k = min(m,n). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(1:i-1) = 0 and v(i) = 1; v(i+1:n) is stored on exit in A(i,i+1:n), */
/*  and tau in TAU(i). */

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

/*     Test the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGELQ2", &i__1);
	return 0;
    }

    k = std::min(*m,*n);

    i__1 = k;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*        Generate elementary reflector H(i) to annihilate A(i,i+1:n) */

	i__2 = *n - i__ + 1;
/* Computing MIN */
	i__3 = i__ + 1;
	dlarfp_(&i__2, &a[i__ + i__ * a_dim1], &a[i__ + std::min(i__3, *n)* a_dim1]
, lda, &tau[i__]);
	if (i__ < *m) {

/*           Apply H(i) to A(i+1:m,i:n) from the right */

	    aii = a[i__ + i__ * a_dim1];
	    a[i__ + i__ * a_dim1] = 1.;
	    i__2 = *m - i__;
	    i__3 = *n - i__ + 1;
	    dlarf_("Right", &i__2, &i__3, &a[i__ + i__ * a_dim1], lda, &tau[
		    i__], &a[i__ + 1 + i__ * a_dim1], lda, &work[1]);
	    a[i__ + i__ * a_dim1] = aii;
	}
/* L10: */
    }
    return 0;

/*     End of DGELQ2 */

} /* dgelq2_ */

/* Subroutine */ int dgelqf_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__3 = 3;
	static integer c__2 = 2;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    integer i__, k, ib, nb, nx, iws, nbmin, iinfo;
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

/*  DGELQF computes an LQ factorization of a real M-by-N matrix A: */
/*  A = L * Q. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, the elements on and below the diagonal of the array */
/*          contain the m-by-min(m,n) lower trapezoidal matrix L (L is */
/*          lower triangular if m <= n); the elements above the diagonal, */
/*          with the array TAU, represent the orthogonal matrix Q as a */
/*          product of elementary reflectors (see Further Details). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors (see Further */
/*          Details). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= max(1,M). */
/*          For optimum performance LWORK >= M*NB, where NB is the */
/*          optimal blocksize. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of elementary reflectors */

/*     Q = H(k) . . . H(2) H(1), where k = min(m,n). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(1:i-1) = 0 and v(i) = 1; v(i+1:n) is stored on exit in A(i,i+1:n), */
/*  and tau in TAU(i). */

/*  ===================================================================== */

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
    nb = ilaenv_(&c__1, "DGELQF", " ", m, n, &c_n1, &c_n1);
    lwkopt = *m * nb;
    work[1] = (double) lwkopt;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    } else if (*lwork < std::max(1_integer,*m) && ! lquery) {
	*info = -7;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGELQF", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    k = std::min(*m,*n);
    if (k == 0) {
	work[1] = 1.;
	return 0;
    }

    nbmin = 2;
    nx = 0;
    iws = *m;
    if (nb > 1 && nb < k) {

/*        Determine when to cross over from blocked to unblocked code. */

/* Computing MAX */
	i__1 = 0, i__2 = ilaenv_(&c__3, "DGELQF", " ", m, n, &c_n1, &c_n1);
	nx = std::max(i__1,i__2);
	if (nx < k) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *m;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*              Not enough workspace to use optimal NB:  reduce NB and */
/*              determine the minimum value of NB. */

		nb = *lwork / ldwork;
/* Computing MAX */
		i__1 = 2, i__2 = ilaenv_(&c__2, "DGELQF", " ", m, n, &c_n1, &
			c_n1);
		nbmin = std::max(i__1,i__2);
	    }
	}
    }

    if (nb >= nbmin && nb < k && nx < k) {

/*        Use blocked code initially */

	i__1 = k - nx;
	i__2 = nb;
	for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
	    i__3 = k - i__ + 1;
	    ib = std::min(i__3,nb);

/*           Compute the LQ factorization of the current block */
/*           A(i:i+ib-1,i:n) */

	    i__3 = *n - i__ + 1;
	    dgelq2_(&ib, &i__3, &a[i__ + i__ * a_dim1], lda, &tau[i__], &work[
		    1], &iinfo);
	    if (i__ + ib <= *m) {

/*              Form the triangular factor of the block reflector */
/*              H = H(i) H(i+1) . . . H(i+ib-1) */

		i__3 = *n - i__ + 1;
		dlarft_("Forward", "Rowwise", &i__3, &ib, &a[i__ + i__ *
			a_dim1], lda, &tau[i__], &work[1], &ldwork);

/*              Apply H to A(i+ib:m,i:n) from the right */

		i__3 = *m - i__ - ib + 1;
		i__4 = *n - i__ + 1;
		dlarfb_("Right", "No transpose", "Forward", "Rowwise", &i__3,
			&i__4, &ib, &a[i__ + i__ * a_dim1], lda, &work[1], &
			ldwork, &a[i__ + ib + i__ * a_dim1], lda, &work[ib +
			1], &ldwork);
	    }
/* L10: */
	}
    } else {
	i__ = 1;
    }

/*     Use unblocked code to factor the last or only block. */

    if (i__ <= k) {
	i__2 = *m - i__ + 1;
	i__1 = *n - i__ + 1;
	dgelq2_(&i__2, &i__1, &a[i__ + i__ * a_dim1], lda, &tau[i__], &work[1]
, &iinfo);
    }

    work[1] = (double) iws;
    return 0;

/*     End of DGELQF */

} /* dgelqf_ */

/* Subroutine */ int dgels_(const char *trans, integer *m, integer *n, integer *
	nrhs, double *a, integer *lda, double *b, integer *ldb,
	double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static double c_b33 = 0.;
	static integer c__0 = 0;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, nb, mn;
    double anrm, bnrm;
    integer brow;
    bool tpsd;
    integer iascl, ibscl;
    integer wsize;
    double rwork[1];
    integer scllen;
    double bignum;
    double smlnum;
    bool lquery;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGELS solves overdetermined or underdetermined real linear systems */
/*  involving an M-by-N matrix A, or its transpose, using a QR or LQ */
/*  factorization of A.  It is assumed that A has full rank. */

/*  The following options are provided: */

/*  1. If TRANS = 'N' and m >= n:  find the least squares solution of */
/*     an overdetermined system, i.e., solve the least squares problem */
/*                  minimize || B - A*X ||. */

/*  2. If TRANS = 'N' and m < n:  find the minimum norm solution of */
/*     an underdetermined system A * X = B. */

/*  3. If TRANS = 'T' and m >= n:  find the minimum norm solution of */
/*     an undetermined system A**T * X = B. */

/*  4. If TRANS = 'T' and m < n:  find the least squares solution of */
/*     an overdetermined system, i.e., solve the least squares problem */
/*                  minimize || B - A**T * X ||. */

/*  Several right hand side vectors b and solution vectors x can be */
/*  handled in a single call; they are stored as the columns of the */
/*  M-by-NRHS right hand side matrix B and the N-by-NRHS solution */
/*  matrix X. */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER*1 */
/*          = 'N': the linear system involves A; */
/*          = 'T': the linear system involves A**T. */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of */
/*          columns of the matrices B and X. NRHS >=0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, */
/*            if M >= N, A is overwritten by details of its QR */
/*                       factorization as returned by DGEQRF; */
/*            if M <  N, A is overwritten by details of its LQ */
/*                       factorization as returned by DGELQF. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the matrix B of right hand side vectors, stored */
/*          columnwise; B is M-by-NRHS if TRANS = 'N', or N-by-NRHS */
/*          if TRANS = 'T'. */
/*          On exit, if INFO = 0, B is overwritten by the solution */
/*          vectors, stored columnwise: */
/*          if TRANS = 'N' and m >= n, rows 1 to n of B contain the least */
/*          squares solution vectors; the residual sum of squares for the */
/*          solution in each column is given by the sum of squares of */
/*          elements N+1 to M in that column; */
/*          if TRANS = 'N' and m < n, rows 1 to N of B contain the */
/*          minimum norm solution vectors; */
/*          if TRANS = 'T' and m >= n, rows 1 to M of B contain the */
/*          minimum norm solution vectors; */
/*          if TRANS = 'T' and m < n, rows 1 to M of B contain the */
/*          least squares solution vectors; the residual sum of squares */
/*          for the solution in each column is given by the sum of */
/*          squares of elements M+1 to N in that column. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= MAX(1,M,N). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          LWORK >= max( 1, MN + max( MN, NRHS ) ). */
/*          For optimal performance, */
/*          LWORK >= max( 1, MN + max( MN, NRHS )*NB ). */
/*          where MN = min(M,N) and NB is the optimum block size. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO =  i, the i-th diagonal element of the */
/*                triangular factor of A is zero, so that A does not have */
/*                full rank; the least squares solution could not be */
/*                computed. */

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

/*     Test the input arguments. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --work;

    /* Function Body */
    *info = 0;
    mn = std::min(*m,*n);
    lquery = *lwork == -1;
    if (! (lsame_(trans, "N") || lsame_(trans, "T"))) {
	*info = -1;
    } else if (*m < 0) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*nrhs < 0) {
	*info = -4;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -6;
    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = std::max(1_integer,*m);
	if (*ldb < std::max(i__1,*n)) {
	    *info = -8;
	} else /* if(complicated condition) */ {
/* Computing MAX */
	    i__1 = 1, i__2 = mn + std::max(mn,*nrhs);
	    if (*lwork < std::max(i__1,i__2) && ! lquery) {
		*info = -10;
	    }
	}
    }

/*     Figure out optimal block size */

    if (*info == 0 || *info == -10) {

	tpsd = true;
	if (lsame_(trans, "N")) {
	    tpsd = false;
	}

	if (*m >= *n) {
	    nb = ilaenv_(&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1);
	    if (tpsd) {
/* Computing MAX */
		i__1 = nb, i__2 = ilaenv_(&c__1, "DORMQR", "LN", m, nrhs, n, &
			c_n1);
		nb = std::max(i__1,i__2);
	    } else {
/* Computing MAX */
		i__1 = nb, i__2 = ilaenv_(&c__1, "DORMQR", "LT", m, nrhs, n, &
			c_n1);
		nb = std::max(i__1,i__2);
	    }
	} else {
	    nb = ilaenv_(&c__1, "DGELQF", " ", m, n, &c_n1, &c_n1);
	    if (tpsd) {
/* Computing MAX */
		i__1 = nb, i__2 = ilaenv_(&c__1, "DORMLQ", "LT", n, nrhs, m, &
			c_n1);
		nb = std::max(i__1,i__2);
	    } else {
/* Computing MAX */
		i__1 = nb, i__2 = ilaenv_(&c__1, "DORMLQ", "LN", n, nrhs, m, &
			c_n1);
		nb = std::max(i__1,i__2);
	    }
	}

/* Computing MAX */
	i__1 = 1, i__2 = mn + std::max(mn,*nrhs) * nb;
	wsize = std::max(i__1,i__2);
	work[1] = (double) wsize;

    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGELS ", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

/* Computing MIN */
    i__1 = std::min(*m,*n);
    if (std::min(i__1,*nrhs) == 0) {
	i__1 = std::max(*m,*n);
	dlaset_("Full", &i__1, nrhs, &c_b33, &c_b33, &b[b_offset], ldb);
	return 0;
    }

/*     Get machine parameters */

    smlnum = dlamch_("S") / dlamch_("P");
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);

/*     Scale A, B if max element outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", m, n, &a[a_offset], lda, rwork);
    iascl = 0;
    if (anrm > 0. && anrm < smlnum) {

/*        Scale matrix norm up to SMLNUM */

	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, m, n, &a[a_offset], lda,
		info);
	iascl = 1;
    } else if (anrm > bignum) {

/*        Scale matrix norm down to BIGNUM */

	dlascl_("G", &c__0, &c__0, &anrm, &bignum, m, n, &a[a_offset], lda,
		info);
	iascl = 2;
    } else if (anrm == 0.) {

/*        Matrix all zero. Return zero solution. */

	i__1 = std::max(*m,*n);
	dlaset_("F", &i__1, nrhs, &c_b33, &c_b33, &b[b_offset], ldb);
	goto L50;
    }

    brow = *m;
    if (tpsd) {
	brow = *n;
    }
    bnrm = dlange_("M", &brow, nrhs, &b[b_offset], ldb, rwork);
    ibscl = 0;
    if (bnrm > 0. && bnrm < smlnum) {

/*        Scale matrix norm up to SMLNUM */

	dlascl_("G", &c__0, &c__0, &bnrm, &smlnum, &brow, nrhs, &b[b_offset],
		ldb, info);
	ibscl = 1;
    } else if (bnrm > bignum) {

/*        Scale matrix norm down to BIGNUM */

	dlascl_("G", &c__0, &c__0, &bnrm, &bignum, &brow, nrhs, &b[b_offset],
		ldb, info);
	ibscl = 2;
    }

    if (*m >= *n) {

/*        compute QR factorization of A */

	i__1 = *lwork - mn;
	dgeqrf_(m, n, &a[a_offset], lda, &work[1], &work[mn + 1], &i__1, info)
		;

/*        workspace at least N, optimally N*NB */

	if (! tpsd) {

/*           Least-Squares Problem min || A * X - B || */

/*           B(1:M,1:NRHS) := Q' * B(1:M,1:NRHS) */

	    i__1 = *lwork - mn;
	    dormqr_("Left", "Transpose", m, nrhs, n, &a[a_offset], lda, &work[
		    1], &b[b_offset], ldb, &work[mn + 1], &i__1, info);

/*           workspace at least NRHS, optimally NRHS*NB */

/*           B(1:N,1:NRHS) := inv(R) * B(1:N,1:NRHS) */

	    dtrtrs_("Upper", "No transpose", "Non-unit", n, nrhs, &a[a_offset]
, lda, &b[b_offset], ldb, info);

	    if (*info > 0) {
		return 0;
	    }

	    scllen = *n;

	} else {

/*           Overdetermined system of equations A' * X = B */

/*           B(1:N,1:NRHS) := inv(R') * B(1:N,1:NRHS) */

	    dtrtrs_("Upper", "Transpose", "Non-unit", n, nrhs, &a[a_offset],
		    lda, &b[b_offset], ldb, info);

	    if (*info > 0) {
		return 0;
	    }

/*           B(N+1:M,1:NRHS) = ZERO */

	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *m;
		for (i__ = *n + 1; i__ <= i__2; ++i__) {
		    b[i__ + j * b_dim1] = 0.;
/* L10: */
		}
/* L20: */
	    }

/*           B(1:M,1:NRHS) := Q(1:N,:) * B(1:N,1:NRHS) */

	    i__1 = *lwork - mn;
	    dormqr_("Left", "No transpose", m, nrhs, n, &a[a_offset], lda, &
		    work[1], &b[b_offset], ldb, &work[mn + 1], &i__1, info);

/*           workspace at least NRHS, optimally NRHS*NB */

	    scllen = *m;

	}

    } else {

/*        Compute LQ factorization of A */

	i__1 = *lwork - mn;
	dgelqf_(m, n, &a[a_offset], lda, &work[1], &work[mn + 1], &i__1, info)
		;

/*        workspace at least M, optimally M*NB. */

	if (! tpsd) {

/*           underdetermined system of equations A * X = B */

/*           B(1:M,1:NRHS) := inv(L) * B(1:M,1:NRHS) */

	    dtrtrs_("Lower", "No transpose", "Non-unit", m, nrhs, &a[a_offset]
, lda, &b[b_offset], ldb, info);

	    if (*info > 0) {
		return 0;
	    }

/*           B(M+1:N,1:NRHS) = 0 */

	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = *m + 1; i__ <= i__2; ++i__) {
		    b[i__ + j * b_dim1] = 0.;
/* L30: */
		}
/* L40: */
	    }

/*           B(1:N,1:NRHS) := Q(1:N,:)' * B(1:M,1:NRHS) */

	    i__1 = *lwork - mn;
	    dormlq_("Left", "Transpose", n, nrhs, m, &a[a_offset], lda, &work[
		    1], &b[b_offset], ldb, &work[mn + 1], &i__1, info);

/*           workspace at least NRHS, optimally NRHS*NB */

	    scllen = *n;

	} else {

/*           overdetermined system min || A' * X - B || */

/*           B(1:N,1:NRHS) := Q * B(1:N,1:NRHS) */

	    i__1 = *lwork - mn;
	    dormlq_("Left", "No transpose", n, nrhs, m, &a[a_offset], lda, &
		    work[1], &b[b_offset], ldb, &work[mn + 1], &i__1, info);

/*           workspace at least NRHS, optimally NRHS*NB */

/*           B(1:M,1:NRHS) := inv(L') * B(1:M,1:NRHS) */

	    dtrtrs_("Lower", "Transpose", "Non-unit", m, nrhs, &a[a_offset],
		    lda, &b[b_offset], ldb, info);

	    if (*info > 0) {
		return 0;
	    }

	    scllen = *m;

	}

    }

/*     Undo scaling */

    if (iascl == 1) {
	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, &scllen, nrhs, &b[b_offset]
, ldb, info);
    } else if (iascl == 2) {
	dlascl_("G", &c__0, &c__0, &anrm, &bignum, &scllen, nrhs, &b[b_offset]
, ldb, info);
    }
    if (ibscl == 1) {
	dlascl_("G", &c__0, &c__0, &smlnum, &bnrm, &scllen, nrhs, &b[b_offset]
, ldb, info);
    } else if (ibscl == 2) {
	dlascl_("G", &c__0, &c__0, &bignum, &bnrm, &scllen, nrhs, &b[b_offset]
, ldb, info);
    }

L50:
    work[1] = (double) wsize;

    return 0;

/*     End of DGELS */

} /* dgels_ */

/* Subroutine */ int dgelsd_(integer *m, integer *n, integer *nrhs,
	double *a, integer *lda, double *b, integer *ldb, double *
	s, double *rcond, integer *rank, double *work, integer *lwork,
	 integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__6 = 6;
	static integer c_n1 = -1;
	static integer c__9 = 9;
	static integer c__0 = 0;
	static integer c__1 = 1;
	static double c_b82 = 0.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2, i__3, i__4;

    /* Builtin functions
    double log(double); */

    /* Local variables */
    integer ie, il, mm;
    double eps, anrm, bnrm;
    integer itau, nlvl, iascl, ibscl;
    double sfmin;
    integer minmn, maxmn, itaup, itauq, mnthr, nwork;
    double bignum;
    integer wlalsd;
    integer ldwork;
    integer minwrk, maxwrk;
    double smlnum;
    bool lquery;
    integer smlsiz;


/*  -- LAPACK driver routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGELSD computes the minimum-norm solution to a real linear least */
/*  squares problem: */
/*      minimize 2-norm(| b - A*x |) */
/*  using the singular value decomposition (SVD) of A. A is an M-by-N */
/*  matrix which may be rank-deficient. */

/*  Several right hand side vectors b and solution vectors x can be */
/*  handled in a single call; they are stored as the columns of the */
/*  M-by-NRHS right hand side matrix B and the N-by-NRHS solution */
/*  matrix X. */

/*  The problem is solved in three steps: */
/*  (1) Reduce the coefficient matrix A to bidiagonal form with */
/*      Householder transformations, reducing the original problem */
/*      into a "bidiagonal least squares problem" (BLS) */
/*  (2) Solve the BLS using a divide and conquer approach. */
/*  (3) Apply back all the Householder tranformations to solve */
/*      the original least squares problem. */

/*  The effective rank of A is determined by treating as zero those */
/*  singular values which are less than RCOND times the largest singular */
/*  value. */

/*  The divide and conquer algorithm makes very mild assumptions about */
/*  floating point arithmetic. It will work on machines with a guard */
/*  digit in add/subtract, or on those binary machines without guard */
/*  digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or */
/*  Cray-2. It could conceivably fail on hexadecimal or decimal machines */
/*  without guard digits, but we know of none. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of A. M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of A. N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X. NRHS >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, A has been destroyed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the M-by-NRHS right hand side matrix B. */
/*          On exit, B is overwritten by the N-by-NRHS solution */
/*          matrix X.  If m >= n and RANK = n, the residual */
/*          sum-of-squares for the solution in the i-th column is given */
/*          by the sum of squares of elements n+1:m in that column. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,max(M,N)). */

/*  S       (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The singular values of A in decreasing order. */
/*          The condition number of A in the 2-norm = S(1)/S(min(m,n)). */

/*  RCOND   (input) DOUBLE PRECISION */
/*          RCOND is used to determine the effective rank of A. */
/*          Singular values S(i) <= RCOND*S(1) are treated as zero. */
/*          If RCOND < 0, machine precision is used instead. */

/*  RANK    (output) INTEGER */
/*          The effective rank of A, i.e., the number of singular values */
/*          which are greater than RCOND*S(1). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. LWORK must be at least 1. */
/*          The exact minimum amount of workspace needed depends on M, */
/*          N and NRHS. As long as LWORK is at least */
/*              12*N + 2*N*SMLSIZ + 8*N*NLVL + N*NRHS + (SMLSIZ+1)**2, */
/*          if M is greater than or equal to N or */
/*              12*M + 2*M*SMLSIZ + 8*M*NLVL + M*NRHS + (SMLSIZ+1)**2, */
/*          if M is less than N, the code will execute correctly. */
/*          SMLSIZ is returned by ILAENV and is equal to the maximum */
/*          size of the subproblems at the bottom of the computation */
/*          tree (usually about 25), and */
/*             NLVL = MAX( 0, INT( LOG_2( MIN( M,N )/(SMLSIZ+1) ) ) + 1 ) */
/*          For good performance, LWORK should generally be larger. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  IWORK   (workspace) INTEGER array, dimension (MAX(1,LIWORK)) */
/*          LIWORK >= 3 * MINMN * NLVL + 11 * MINMN, */
/*          where MINMN = MIN( M,N ). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  the algorithm for computing the SVD failed to converge; */
/*                if INFO = i, i off-diagonal elements of an intermediate */
/*                bidiagonal form did not converge to zero. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Ming Gu and Ren-Cang Li, Computer Science Division, University of */
/*       California at Berkeley, USA */
/*     Osni Marques, LBNL/NERSC, USA */

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

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --s;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    minmn = std::min(*m,*n);
    maxmn = std::max(*m,*n);
    mnthr = ilaenv_(&c__6, "DGELSD", " ", m, n, nrhs, &c_n1);
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,maxmn)) {
	*info = -7;
    }

    smlsiz = ilaenv_(&c__9, "DGELSD", " ", &c__0, &c__0, &c__0, &c__0);

/*     Compute workspace. */
/*     (Note: Comments in the code beginning "Workspace:" describe the */
/*     minimal amount of workspace needed at that point in the code, */
/*     as well as the preferred amount for good performance. */
/*     NB refers to the optimal block size for the immediately */
/*     following subroutine, as returned by ILAENV.) */

    minwrk = 1;
    minmn = std::max(1_integer,minmn);
/* Computing MAX */
    i__1 = (integer) (log((double) minmn / (double) (smlsiz + 1)) /
	    log(2.)) + 1;
    nlvl = std::max(i__1,0_integer);

    if (*info == 0) {
	maxwrk = 0;
	mm = *m;
	if (*m >= *n && *m >= mnthr) {

/*           Path 1a - overdetermined, with many more rows than columns. */

	    mm = *n;
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m,
		    n, &c_n1, &c_n1);
	    maxwrk = std::max(i__1,i__2);
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n + *nrhs * ilaenv_(&c__1, "DORMQR", "LT",
		    m, nrhs, n, &c_n1);
	    maxwrk = std::max(i__1,i__2);
	}
	if (*m >= *n) {

/*           Path 1 - overdetermined or exactly determined. */

/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n * 3 + (mm + *n) * ilaenv_(&c__1, "DGEBRD"
, " ", &mm, n, &c_n1, &c_n1);
	    maxwrk = std::max(i__1,i__2);
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n * 3 + *nrhs * ilaenv_(&c__1, "DORMBR",
		    "QLT", &mm, nrhs, n, &c_n1);
	    maxwrk = std::max(i__1,i__2);
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n * 3 + (*n - 1) * ilaenv_(&c__1, "DORMBR",
		     "PLN", n, nrhs, n, &c_n1);
	    maxwrk = std::max(i__1,i__2);
/* Computing 2nd power */
	    i__1 = smlsiz + 1;
	    wlalsd = *n * 9 + (*n << 1) * smlsiz + (*n << 3) * nlvl + *n * *
		    nrhs + i__1 * i__1;
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n * 3 + wlalsd;
	    maxwrk = std::max(i__1,i__2);
/* Computing MAX */
	    i__1 = *n * 3 + mm, i__2 = *n * 3 + *nrhs, i__1 = std::max(i__1,i__2),
		    i__2 = *n * 3 + wlalsd;
	    minwrk = std::max(i__1,i__2);
	}
	if (*n > *m) {
/* Computing 2nd power */
	    i__1 = smlsiz + 1;
	    wlalsd = *m * 9 + (*m << 1) * smlsiz + (*m << 3) * nlvl + *m * *
		    nrhs + i__1 * i__1;
	    if (*n >= mnthr) {

/*              Path 2a - underdetermined, with many more columns */
/*              than rows. */

		maxwrk = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &c_n1,
			&c_n1);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m * *m + (*m << 2) + (*m << 1) *
			ilaenv_(&c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1);
		maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m * *m + (*m << 2) + *nrhs * ilaenv_(&
			c__1, "DORMBR", "QLT", m, nrhs, m, &c_n1);
		maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m * *m + (*m << 2) + (*m - 1) *
			ilaenv_(&c__1, "DORMBR", "PLN", m, nrhs, m, &c_n1);
		maxwrk = std::max(i__1,i__2);
		if (*nrhs > 1) {
/* Computing MAX */
		    i__1 = maxwrk, i__2 = *m * *m + *m + *m * *nrhs;
		    maxwrk = std::max(i__1,i__2);
		} else {
/* Computing MAX */
		    i__1 = maxwrk, i__2 = *m * *m + (*m << 1);
		    maxwrk = std::max(i__1,i__2);
		}
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m + *nrhs * ilaenv_(&c__1, "DORMLQ",
			"LT", n, nrhs, m, &c_n1);
		maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m * *m + (*m << 2) + wlalsd;
		maxwrk = std::max(i__1,i__2);
/*     XXX: Ensure the Path 2a case below is triggered.  The workspace */
/*     calculation should use queries for all routines eventually. */
/* Computing MAX */
/* Computing MAX */
		i__3 = *m, i__4 = (*m << 1) - 4, i__3 = std::max(i__3,i__4), i__3 =
			 std::max(i__3,*nrhs), i__4 = *n - *m * 3;
		i__1 = maxwrk, i__2 = (*m << 2) + *m * *m + std::max(i__3,i__4);
		maxwrk = std::max(i__1,i__2);
	    } else {

/*              Path 2 - remaining underdetermined cases. */

		maxwrk = *m * 3 + (*n + *m) * ilaenv_(&c__1, "DGEBRD", " ", m,
			 n, &c_n1, &c_n1);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m * 3 + *nrhs * ilaenv_(&c__1, "DORMBR"
, "QLT", m, nrhs, n, &c_n1);
		maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR",
			"PLN", n, nrhs, m, &c_n1);
		maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *m * 3 + wlalsd;
		maxwrk = std::max(i__1,i__2);
	    }
/* Computing MAX */
	    i__1 = *m * 3 + *nrhs, i__2 = *m * 3 + *m, i__1 = std::max(i__1,i__2),
		    i__2 = *m * 3 + wlalsd;
	    minwrk = std::max(i__1,i__2);
	}
	minwrk = std::min(minwrk,maxwrk);
	work[1] = (double) maxwrk;
	if (*lwork < minwrk && ! lquery) {
	    *info = -12;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGELSD", &i__1);
	return 0;
    } else if (lquery) {
	goto L10;
    }

/*     Quick return if possible. */

    if (*m == 0 || *n == 0) {
	*rank = 0;
	return 0;
    }

/*     Get machine parameters. */

    eps = dlamch_("P");
    sfmin = dlamch_("S");
    smlnum = sfmin / eps;
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);

/*     Scale A if max entry outside range [SMLNUM,BIGNUM]. */

    anrm = dlange_("M", m, n, &a[a_offset], lda, &work[1]);
    iascl = 0;
    if (anrm > 0. && anrm < smlnum) {

/*        Scale matrix norm up to SMLNUM. */

	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, m, n, &a[a_offset], lda,
		info);
	iascl = 1;
    } else if (anrm > bignum) {

/*        Scale matrix norm down to BIGNUM. */

	dlascl_("G", &c__0, &c__0, &anrm, &bignum, m, n, &a[a_offset], lda,
		info);
	iascl = 2;
    } else if (anrm == 0.) {

/*        Matrix all zero. Return zero solution. */

	i__1 = std::max(*m,*n);
	dlaset_("F", &i__1, nrhs, &c_b82, &c_b82, &b[b_offset], ldb);
	dlaset_("F", &minmn, &c__1, &c_b82, &c_b82, &s[1], &c__1);
	*rank = 0;
	goto L10;
    }

/*     Scale B if max entry outside range [SMLNUM,BIGNUM]. */

    bnrm = dlange_("M", m, nrhs, &b[b_offset], ldb, &work[1]);
    ibscl = 0;
    if (bnrm > 0. && bnrm < smlnum) {

/*        Scale matrix norm up to SMLNUM. */

	dlascl_("G", &c__0, &c__0, &bnrm, &smlnum, m, nrhs, &b[b_offset], ldb,
		 info);
	ibscl = 1;
    } else if (bnrm > bignum) {

/*        Scale matrix norm down to BIGNUM. */

	dlascl_("G", &c__0, &c__0, &bnrm, &bignum, m, nrhs, &b[b_offset], ldb,
		 info);
	ibscl = 2;
    }

/*     If M < N make sure certain entries of B are zero. */

    if (*m < *n) {
	i__1 = *n - *m;
	dlaset_("F", &i__1, nrhs, &c_b82, &c_b82, &b[*m + 1 + b_dim1], ldb);
    }

/*     Overdetermined case. */

    if (*m >= *n) {

/*        Path 1 - overdetermined or exactly determined. */

	mm = *m;
	if (*m >= mnthr) {

/*           Path 1a - overdetermined, with many more rows than columns. */

	    mm = *n;
	    itau = 1;
	    nwork = itau + *n;

/*           Compute A=Q*R. */
/*           (Workspace: need 2*N, prefer N+N*NB) */

	    i__1 = *lwork - nwork + 1;
	    dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &i__1,
		     info);

/*           Multiply B by transpose(Q). */
/*           (Workspace: need N+NRHS, prefer N+NRHS*NB) */

	    i__1 = *lwork - nwork + 1;
	    dormqr_("L", "T", m, nrhs, n, &a[a_offset], lda, &work[itau], &b[
		    b_offset], ldb, &work[nwork], &i__1, info);

/*           Zero out below R. */

	    if (*n > 1) {
		i__1 = *n - 1;
		i__2 = *n - 1;
		dlaset_("L", &i__1, &i__2, &c_b82, &c_b82, &a[a_dim1 + 2],
			lda);
	    }
	}

	ie = 1;
	itauq = ie + *n;
	itaup = itauq + *n;
	nwork = itaup + *n;

/*        Bidiagonalize R in A. */
/*        (Workspace: need 3*N+MM, prefer 3*N+(MM+N)*NB) */

	i__1 = *lwork - nwork + 1;
	dgebrd_(&mm, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &
		work[itaup], &work[nwork], &i__1, info);

/*        Multiply B by transpose of left bidiagonalizing vectors of R. */
/*        (Workspace: need 3*N+NRHS, prefer 3*N+NRHS*NB) */

	i__1 = *lwork - nwork + 1;
	dormbr_("Q", "L", "T", &mm, nrhs, n, &a[a_offset], lda, &work[itauq],
		&b[b_offset], ldb, &work[nwork], &i__1, info);

/*        Solve the bidiagonal least squares problem. */

	dlalsd_("U", &smlsiz, n, nrhs, &s[1], &work[ie], &b[b_offset], ldb,
		rcond, rank, &work[nwork], &iwork[1], info);
	if (*info != 0) {
	    goto L10;
	}

/*        Multiply B by right bidiagonalizing vectors of R. */

	i__1 = *lwork - nwork + 1;
	dormbr_("P", "L", "N", n, nrhs, n, &a[a_offset], lda, &work[itaup], &
		b[b_offset], ldb, &work[nwork], &i__1, info);

    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = *m, i__2 = (*m << 1) - 4, i__1 = std::max(i__1,i__2), i__1 = std::max(
		i__1,*nrhs), i__2 = *n - *m * 3, i__1 = std::max(i__1,i__2);
	if (*n >= mnthr && *lwork >= (*m << 2) + *m * *m + std::max(i__1,wlalsd)) {

/*        Path 2a - underdetermined, with many more columns than rows */
/*        and sufficient workspace for an efficient algorithm. */

	    ldwork = *m;
/* Computing MAX */
/* Computing MAX */
	    i__3 = *m, i__4 = (*m << 1) - 4, i__3 = std::max(i__3,i__4), i__3 =
		    std::max(i__3,*nrhs), i__4 = *n - *m * 3;
	    i__1 = (*m << 2) + *m * *lda + std::max(i__3,i__4), i__2 = *m * *lda +
		    *m + *m * *nrhs, i__1 = std::max(i__1,i__2), i__2 = (*m << 2)
		    + *m * *lda + wlalsd;
	    if (*lwork >= std::max(i__1,i__2)) {
		ldwork = *lda;
	    }
	    itau = 1;
	    nwork = *m + 1;

/*        Compute A=L*Q. */
/*        (Workspace: need 2*M, prefer M+M*NB) */

	    i__1 = *lwork - nwork + 1;
	    dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &i__1,
		     info);
	    il = nwork;

/*        Copy L to WORK(IL), zeroing out above its diagonal. */

	    dlacpy_("L", m, m, &a[a_offset], lda, &work[il], &ldwork);
	    i__1 = *m - 1;
	    i__2 = *m - 1;
	    dlaset_("U", &i__1, &i__2, &c_b82, &c_b82, &work[il + ldwork], &
		    ldwork);
	    ie = il + ldwork * *m;
	    itauq = ie + *m;
	    itaup = itauq + *m;
	    nwork = itaup + *m;

/*        Bidiagonalize L in WORK(IL). */
/*        (Workspace: need M*M+5*M, prefer M*M+4*M+2*M*NB) */

	    i__1 = *lwork - nwork + 1;
	    dgebrd_(m, m, &work[il], &ldwork, &s[1], &work[ie], &work[itauq],
		    &work[itaup], &work[nwork], &i__1, info);

/*        Multiply B by transpose of left bidiagonalizing vectors of L. */
/*        (Workspace: need M*M+4*M+NRHS, prefer M*M+4*M+NRHS*NB) */

	    i__1 = *lwork - nwork + 1;
	    dormbr_("Q", "L", "T", m, nrhs, m, &work[il], &ldwork, &work[
		    itauq], &b[b_offset], ldb, &work[nwork], &i__1, info);

/*        Solve the bidiagonal least squares problem. */

	    dlalsd_("U", &smlsiz, m, nrhs, &s[1], &work[ie], &b[b_offset],
		    ldb, rcond, rank, &work[nwork], &iwork[1], info);
	    if (*info != 0) {
		goto L10;
	    }

/*        Multiply B by right bidiagonalizing vectors of L. */

	    i__1 = *lwork - nwork + 1;
	    dormbr_("P", "L", "N", m, nrhs, m, &work[il], &ldwork, &work[
		    itaup], &b[b_offset], ldb, &work[nwork], &i__1, info);

/*        Zero out below first M rows of B. */

	    i__1 = *n - *m;
	    dlaset_("F", &i__1, nrhs, &c_b82, &c_b82, &b[*m + 1 + b_dim1],
		    ldb);
	    nwork = itau + *m;

/*        Multiply transpose(Q) by B. */
/*        (Workspace: need M+NRHS, prefer M+NRHS*NB) */

	    i__1 = *lwork - nwork + 1;
	    dormlq_("L", "T", n, nrhs, m, &a[a_offset], lda, &work[itau], &b[
		    b_offset], ldb, &work[nwork], &i__1, info);

	} else {

/*        Path 2 - remaining underdetermined cases. */

	    ie = 1;
	    itauq = ie + *m;
	    itaup = itauq + *m;
	    nwork = itaup + *m;

/*        Bidiagonalize A. */
/*        (Workspace: need 3*M+N, prefer 3*M+(M+N)*NB) */

	    i__1 = *lwork - nwork + 1;
	    dgebrd_(m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &
		    work[itaup], &work[nwork], &i__1, info);

/*        Multiply B by transpose of left bidiagonalizing vectors. */
/*        (Workspace: need 3*M+NRHS, prefer 3*M+NRHS*NB) */

	    i__1 = *lwork - nwork + 1;
	    dormbr_("Q", "L", "T", m, nrhs, n, &a[a_offset], lda, &work[itauq]
, &b[b_offset], ldb, &work[nwork], &i__1, info);

/*        Solve the bidiagonal least squares problem. */

	    dlalsd_("L", &smlsiz, m, nrhs, &s[1], &work[ie], &b[b_offset],
		    ldb, rcond, rank, &work[nwork], &iwork[1], info);
	    if (*info != 0) {
		goto L10;
	    }

/*        Multiply B by right bidiagonalizing vectors of A. */

	    i__1 = *lwork - nwork + 1;
	    dormbr_("P", "L", "N", n, nrhs, m, &a[a_offset], lda, &work[itaup]
, &b[b_offset], ldb, &work[nwork], &i__1, info);

	}
    }

/*     Undo scaling. */

    if (iascl == 1) {
	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, n, nrhs, &b[b_offset], ldb,
		 info);
	dlascl_("G", &c__0, &c__0, &smlnum, &anrm, &minmn, &c__1, &s[1], &
		minmn, info);
    } else if (iascl == 2) {
	dlascl_("G", &c__0, &c__0, &anrm, &bignum, n, nrhs, &b[b_offset], ldb,
		 info);
	dlascl_("G", &c__0, &c__0, &bignum, &anrm, &minmn, &c__1, &s[1], &
		minmn, info);
    }
    if (ibscl == 1) {
	dlascl_("G", &c__0, &c__0, &smlnum, &bnrm, n, nrhs, &b[b_offset], ldb,
		 info);
    } else if (ibscl == 2) {
	dlascl_("G", &c__0, &c__0, &bignum, &bnrm, n, nrhs, &b[b_offset], ldb,
		 info);
    }

L10:
    work[1] = (double) maxwrk;
    return 0;

/*     End of DGELSD */

} /* dgelsd_ */

/* Subroutine */ int dgelss_(integer *m, integer *n, integer *nrhs,
	double *a, integer *lda, double *b, integer *ldb, double *
	s, double *rcond, integer *rank, double *work, integer *lwork,
	 integer *info)
{
	/* Table of constant values */
	static integer c__6 = 6;
	static integer c_n1 = -1;
	static integer c__1 = 1;
	static integer c__0 = 0;
	static double c_b74 = 0.;
	static double c_b108 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2, i__3, i__4;
    double d__1;

    /* Local variables */
    integer i__, bl, ie, il, mm;
    double eps, thr, anrm, bnrm;
    integer itau;
    double vdum[1];
    integer iascl, ibscl;
    integer chunk;
    double sfmin;
    integer minmn;
    integer maxmn, itaup, itauq, mnthr, iwork;
    integer bdspac;
    double bignum;
    integer ldwork;
    integer minwrk, maxwrk;
    double smlnum;
    bool lquery;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGELSS computes the minimum norm solution to a real linear least */
/*  squares problem: */

/*  Minimize 2-norm(| b - A*x |). */

/*  using the singular value decomposition (SVD) of A. A is an M-by-N */
/*  matrix which may be rank-deficient. */

/*  Several right hand side vectors b and solution vectors x can be */
/*  handled in a single call; they are stored as the columns of the */
/*  M-by-NRHS right hand side matrix B and the N-by-NRHS solution matrix */
/*  X. */

/*  The effective rank of A is determined by treating as zero those */
/*  singular values which are less than RCOND times the largest singular */
/*  value. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A. M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A. N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X. NRHS >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, the first min(m,n) rows of A are overwritten with */
/*          its right singular vectors, stored rowwise. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the M-by-NRHS right hand side matrix B. */
/*          On exit, B is overwritten by the N-by-NRHS solution */
/*          matrix X.  If m >= n and RANK = n, the residual */
/*          sum-of-squares for the solution in the i-th column is given */
/*          by the sum of squares of elements n+1:m in that column. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,max(M,N)). */

/*  S       (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The singular values of A in decreasing order. */
/*          The condition number of A in the 2-norm = S(1)/S(min(m,n)). */

/*  RCOND   (input) DOUBLE PRECISION */
/*          RCOND is used to determine the effective rank of A. */
/*          Singular values S(i) <= RCOND*S(1) are treated as zero. */
/*          If RCOND < 0, machine precision is used instead. */

/*  RANK    (output) INTEGER */
/*          The effective rank of A, i.e., the number of singular values */
/*          which are greater than RCOND*S(1). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. LWORK >= 1, and also: */
/*          LWORK >= 3*min(M,N) + max( 2*min(M,N), max(M,N), NRHS ) */
/*          For good performance, LWORK should generally be larger. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  the algorithm for computing the SVD failed to converge; */
/*                if INFO = i, i off-diagonal elements of an intermediate */
/*                bidiagonal form did not converge to zero. */

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

/*     Test the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --s;
    --work;

    /* Function Body */
    *info = 0;
    minmn = std::min(*m,*n);
    maxmn = std::max(*m,*n);
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,maxmn)) {
	*info = -7;
    }

/*     Compute workspace */
/*      (Note: Comments in the code beginning "Workspace:" describe the */
/*       minimal amount of workspace needed at that point in the code, */
/*       as well as the preferred amount for good performance. */
/*       NB refers to the optimal block size for the immediately */
/*       following subroutine, as returned by ILAENV.) */

    if (*info == 0) {
	minwrk = 1;
	maxwrk = 1;
	if (minmn > 0) {
	    mm = *m;
	    mnthr = ilaenv_(&c__6, "DGELSS", " ", m, n, nrhs, &c_n1);
	    if (*m >= *n && *m >= mnthr) {

/*              Path 1a - overdetermined, with many more rows than */
/*                        columns */

		mm = *n;
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n + *n * ilaenv_(&c__1, "DGEQRF",
			" ", m, n, &c_n1, &c_n1);
		maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n + *nrhs * ilaenv_(&c__1, "DORMQR",
			"LT", m, nrhs, n, &c_n1);
		maxwrk = std::max(i__1,i__2);
	    }
	    if (*m >= *n) {

/*              Path 1 - overdetermined or exactly determined */

/*              Compute workspace needed for DBDSQR */

/* Computing MAX */
		i__1 = 1, i__2 = *n * 5;
		bdspac = std::max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n * 3 + (mm + *n) * ilaenv_(&c__1,
			"DGEBRD", " ", &mm, n, &c_n1, &c_n1);
		maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n * 3 + *nrhs * ilaenv_(&c__1, "DORMBR"
, "QLT", &mm, nrhs, n, &c_n1);
		maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n * 3 + (*n - 1) * ilaenv_(&c__1,
			"DORGBR", "P", n, n, n, &c_n1);
		maxwrk = std::max(i__1,i__2);
		maxwrk = std::max(maxwrk,bdspac);
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n * *nrhs;
		maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		i__1 = *n * 3 + mm, i__2 = *n * 3 + *nrhs, i__1 = std::max(i__1,
			i__2);
		minwrk = std::max(i__1,bdspac);
		maxwrk = std::max(minwrk,maxwrk);
	    }
	    if (*n > *m) {

/*              Compute workspace needed for DBDSQR */

/* Computing MAX */
		i__1 = 1, i__2 = *m * 5;
		bdspac = std::max(i__1,i__2);
/* Computing MAX */
		i__1 = *m * 3 + *nrhs, i__2 = *m * 3 + *n, i__1 = std::max(i__1,
			i__2);
		minwrk = std::max(i__1,bdspac);
		if (*n >= mnthr) {

/*                 Path 2a - underdetermined, with many more columns */
/*                 than rows */

		    maxwrk = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__1 = maxwrk, i__2 = *m * *m + (*m << 2) + (*m << 1) *
			    ilaenv_(&c__1, "DGEBRD", " ", m, m, &c_n1, &c_n1);
		    maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = maxwrk, i__2 = *m * *m + (*m << 2) + *nrhs *
			    ilaenv_(&c__1, "DORMBR", "QLT", m, nrhs, m, &c_n1);
		    maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = maxwrk, i__2 = *m * *m + (*m << 2) + (*m - 1) *
			    ilaenv_(&c__1, "DORGBR", "P", m, m, m, &c_n1);
		    maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = maxwrk, i__2 = *m * *m + *m + bdspac;
		    maxwrk = std::max(i__1,i__2);
		    if (*nrhs > 1) {
/* Computing MAX */
			i__1 = maxwrk, i__2 = *m * *m + *m + *m * *nrhs;
			maxwrk = std::max(i__1,i__2);
		    } else {
/* Computing MAX */
			i__1 = maxwrk, i__2 = *m * *m + (*m << 1);
			maxwrk = std::max(i__1,i__2);
		    }
/* Computing MAX */
		    i__1 = maxwrk, i__2 = *m + *nrhs * ilaenv_(&c__1, "DORMLQ"
, "LT", n, nrhs, m, &c_n1);
		    maxwrk = std::max(i__1,i__2);
		} else {

/*                 Path 2 - underdetermined */

		    maxwrk = *m * 3 + (*n + *m) * ilaenv_(&c__1, "DGEBRD",
			    " ", m, n, &c_n1, &c_n1);
/* Computing MAX */
		    i__1 = maxwrk, i__2 = *m * 3 + *nrhs * ilaenv_(&c__1,
			    "DORMBR", "QLT", m, nrhs, m, &c_n1);
		    maxwrk = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = maxwrk, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORG"
			    "BR", "P", m, n, m, &c_n1);
		    maxwrk = std::max(i__1,i__2);
		    maxwrk = std::max(maxwrk,bdspac);
/* Computing MAX */
		    i__1 = maxwrk, i__2 = *n * *nrhs;
		    maxwrk = std::max(i__1,i__2);
		}
	    }
	    maxwrk = std::max(minwrk,maxwrk);
	}
	work[1] = (double) maxwrk;

	if (*lwork < minwrk && ! lquery) {
	    *info = -12;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGELSS", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	*rank = 0;
	return 0;
    }

/*     Get machine parameters */

    eps = dlamch_("P");
    sfmin = dlamch_("S");
    smlnum = sfmin / eps;
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", m, n, &a[a_offset], lda, &work[1]);
    iascl = 0;
    if (anrm > 0. && anrm < smlnum) {

/*        Scale matrix norm up to SMLNUM */

	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, m, n, &a[a_offset], lda,
		info);
	iascl = 1;
    } else if (anrm > bignum) {

/*        Scale matrix norm down to BIGNUM */

	dlascl_("G", &c__0, &c__0, &anrm, &bignum, m, n, &a[a_offset], lda,
		info);
	iascl = 2;
    } else if (anrm == 0.) {

/*        Matrix all zero. Return zero solution. */

	i__1 = std::max(*m,*n);
	dlaset_("F", &i__1, nrhs, &c_b74, &c_b74, &b[b_offset], ldb);
	dlaset_("F", &minmn, &c__1, &c_b74, &c_b74, &s[1], &c__1);
	*rank = 0;
	goto L70;
    }

/*     Scale B if max element outside range [SMLNUM,BIGNUM] */

    bnrm = dlange_("M", m, nrhs, &b[b_offset], ldb, &work[1]);
    ibscl = 0;
    if (bnrm > 0. && bnrm < smlnum) {

/*        Scale matrix norm up to SMLNUM */

	dlascl_("G", &c__0, &c__0, &bnrm, &smlnum, m, nrhs, &b[b_offset], ldb,
		 info);
	ibscl = 1;
    } else if (bnrm > bignum) {

/*        Scale matrix norm down to BIGNUM */

	dlascl_("G", &c__0, &c__0, &bnrm, &bignum, m, nrhs, &b[b_offset], ldb,
		 info);
	ibscl = 2;
    }

/*     Overdetermined case */

    if (*m >= *n) {

/*        Path 1 - overdetermined or exactly determined */

	mm = *m;
	if (*m >= mnthr) {

/*           Path 1a - overdetermined, with many more rows than columns */

	    mm = *n;
	    itau = 1;
	    iwork = itau + *n;

/*           Compute A=Q*R */
/*           (Workspace: need 2*N, prefer N+N*NB) */

	    i__1 = *lwork - iwork + 1;
	    dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__1,
		     info);

/*           Multiply B by transpose(Q) */
/*           (Workspace: need N+NRHS, prefer N+NRHS*NB) */

	    i__1 = *lwork - iwork + 1;
	    dormqr_("L", "T", m, nrhs, n, &a[a_offset], lda, &work[itau], &b[
		    b_offset], ldb, &work[iwork], &i__1, info);

/*           Zero out below R */

	    if (*n > 1) {
		i__1 = *n - 1;
		i__2 = *n - 1;
		dlaset_("L", &i__1, &i__2, &c_b74, &c_b74, &a[a_dim1 + 2],
			lda);
	    }
	}

	ie = 1;
	itauq = ie + *n;
	itaup = itauq + *n;
	iwork = itaup + *n;

/*        Bidiagonalize R in A */
/*        (Workspace: need 3*N+MM, prefer 3*N+(MM+N)*NB) */

	i__1 = *lwork - iwork + 1;
	dgebrd_(&mm, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &
		work[itaup], &work[iwork], &i__1, info);

/*        Multiply B by transpose of left bidiagonalizing vectors of R */
/*        (Workspace: need 3*N+NRHS, prefer 3*N+NRHS*NB) */

	i__1 = *lwork - iwork + 1;
	dormbr_("Q", "L", "T", &mm, nrhs, n, &a[a_offset], lda, &work[itauq],
		&b[b_offset], ldb, &work[iwork], &i__1, info);

/*        Generate right bidiagonalizing vectors of R in A */
/*        (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

	i__1 = *lwork - iwork + 1;
	dorgbr_("P", n, n, n, &a[a_offset], lda, &work[itaup], &work[iwork], &
		i__1, info);
	iwork = ie + *n;

/*        Perform bidiagonal QR iteration */
/*          multiply B by transpose of left singular vectors */
/*          compute right singular vectors in A */
/*        (Workspace: need BDSPAC) */

	dbdsqr_("U", n, n, &c__0, nrhs, &s[1], &work[ie], &a[a_offset], lda,
		vdum, &c__1, &b[b_offset], ldb, &work[iwork], info)
		;
	if (*info != 0) {
	    goto L70;
	}

/*        Multiply B by reciprocals of singular values */

/* Computing MAX */
	d__1 = *rcond * s[1];
	thr = std::max(d__1,sfmin);
	if (*rcond < 0.) {
/* Computing MAX */
	    d__1 = eps * s[1];
	    thr = std::max(d__1,sfmin);
	}
	*rank = 0;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (s[i__] > thr) {
		drscl_(nrhs, &s[i__], &b[i__ + b_dim1], ldb);
		++(*rank);
	    } else {
		dlaset_("F", &c__1, nrhs, &c_b74, &c_b74, &b[i__ + b_dim1],
			ldb);
	    }
/* L10: */
	}

/*        Multiply B by right singular vectors */
/*        (Workspace: need N, prefer N*NRHS) */

	if (*lwork >= *ldb * *nrhs && *nrhs > 1) {
	    dgemm_("T", "N", n, nrhs, n, &c_b108, &a[a_offset], lda, &b[
		    b_offset], ldb, &c_b74, &work[1], ldb);
	    dlacpy_("G", n, nrhs, &work[1], ldb, &b[b_offset], ldb)
		    ;
	} else if (*nrhs > 1) {
	    chunk = *lwork / *n;
	    i__1 = *nrhs;
	    i__2 = chunk;
	    for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
		i__3 = *nrhs - i__ + 1;
		bl = std::min(i__3,chunk);
		dgemm_("T", "N", n, &bl, n, &c_b108, &a[a_offset], lda, &b[
			i__ * b_dim1 + 1], ldb, &c_b74, &work[1], n);
		dlacpy_("G", n, &bl, &work[1], n, &b[i__ * b_dim1 + 1], ldb);
/* L20: */
	    }
	} else {
	    dgemv_("T", n, n, &c_b108, &a[a_offset], lda, &b[b_offset], &c__1,
		     &c_b74, &work[1], &c__1);
	    dcopy_(n, &work[1], &c__1, &b[b_offset], &c__1);
	}

    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__2 = *m, i__1 = (*m << 1) - 4, i__2 = std::max(i__2,i__1), i__2 = std::max(
		i__2,*nrhs), i__1 = *n - *m * 3;
	if (*n >= mnthr && *lwork >= (*m << 2) + *m * *m + std::max(i__2,i__1)) {

/*        Path 2a - underdetermined, with many more columns than rows */
/*        and sufficient workspace for an efficient algorithm */

	    ldwork = *m;
/* Computing MAX */
/* Computing MAX */
	    i__3 = *m, i__4 = (*m << 1) - 4, i__3 = std::max(i__3,i__4), i__3 =
		    std::max(i__3,*nrhs), i__4 = *n - *m * 3;
	    i__2 = (*m << 2) + *m * *lda + std::max(i__3,i__4), i__1 = *m * *lda +
		    *m + *m * *nrhs;
	    if (*lwork >= std::max(i__2,i__1)) {
		ldwork = *lda;
	    }
	    itau = 1;
	    iwork = *m + 1;

/*        Compute A=L*Q */
/*        (Workspace: need 2*M, prefer M+M*NB) */

	    i__2 = *lwork - iwork + 1;
	    dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork], &i__2,
		     info);
	    il = iwork;

/*        Copy L to WORK(IL), zeroing out above it */

	    dlacpy_("L", m, m, &a[a_offset], lda, &work[il], &ldwork);
	    i__2 = *m - 1;
	    i__1 = *m - 1;
	    dlaset_("U", &i__2, &i__1, &c_b74, &c_b74, &work[il + ldwork], &
		    ldwork);
	    ie = il + ldwork * *m;
	    itauq = ie + *m;
	    itaup = itauq + *m;
	    iwork = itaup + *m;

/*        Bidiagonalize L in WORK(IL) */
/*        (Workspace: need M*M+5*M, prefer M*M+4*M+2*M*NB) */

	    i__2 = *lwork - iwork + 1;
	    dgebrd_(m, m, &work[il], &ldwork, &s[1], &work[ie], &work[itauq],
		    &work[itaup], &work[iwork], &i__2, info);

/*        Multiply B by transpose of left bidiagonalizing vectors of L */
/*        (Workspace: need M*M+4*M+NRHS, prefer M*M+4*M+NRHS*NB) */

	    i__2 = *lwork - iwork + 1;
	    dormbr_("Q", "L", "T", m, nrhs, m, &work[il], &ldwork, &work[
		    itauq], &b[b_offset], ldb, &work[iwork], &i__2, info);

/*        Generate right bidiagonalizing vectors of R in WORK(IL) */
/*        (Workspace: need M*M+5*M-1, prefer M*M+4*M+(M-1)*NB) */

	    i__2 = *lwork - iwork + 1;
	    dorgbr_("P", m, m, m, &work[il], &ldwork, &work[itaup], &work[
		    iwork], &i__2, info);
	    iwork = ie + *m;

/*        Perform bidiagonal QR iteration, */
/*           computing right singular vectors of L in WORK(IL) and */
/*           multiplying B by transpose of left singular vectors */
/*        (Workspace: need M*M+M+BDSPAC) */

	    dbdsqr_("U", m, m, &c__0, nrhs, &s[1], &work[ie], &work[il], &
		    ldwork, &a[a_offset], lda, &b[b_offset], ldb, &work[iwork]
, info);
	    if (*info != 0) {
		goto L70;
	    }

/*        Multiply B by reciprocals of singular values */

/* Computing MAX */
	    d__1 = *rcond * s[1];
	    thr = std::max(d__1,sfmin);
	    if (*rcond < 0.) {
/* Computing MAX */
		d__1 = eps * s[1];
		thr = std::max(d__1,sfmin);
	    }
	    *rank = 0;
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		if (s[i__] > thr) {
		    drscl_(nrhs, &s[i__], &b[i__ + b_dim1], ldb);
		    ++(*rank);
		} else {
		    dlaset_("F", &c__1, nrhs, &c_b74, &c_b74, &b[i__ + b_dim1]
, ldb);
		}
/* L30: */
	    }
	    iwork = ie;

/*        Multiply B by right singular vectors of L in WORK(IL) */
/*        (Workspace: need M*M+2*M, prefer M*M+M+M*NRHS) */

	    if (*lwork >= *ldb * *nrhs + iwork - 1 && *nrhs > 1) {
		dgemm_("T", "N", m, nrhs, m, &c_b108, &work[il], &ldwork, &b[
			b_offset], ldb, &c_b74, &work[iwork], ldb);
		dlacpy_("G", m, nrhs, &work[iwork], ldb, &b[b_offset], ldb);
	    } else if (*nrhs > 1) {
		chunk = (*lwork - iwork + 1) / *m;
		i__2 = *nrhs;
		i__1 = chunk;
		for (i__ = 1; i__1 < 0 ? i__ >= i__2 : i__ <= i__2; i__ +=
			i__1) {
/* Computing MIN */
		    i__3 = *nrhs - i__ + 1;
		    bl = std::min(i__3,chunk);
		    dgemm_("T", "N", m, &bl, m, &c_b108, &work[il], &ldwork, &
			    b[i__ * b_dim1 + 1], ldb, &c_b74, &work[iwork], m);
		    dlacpy_("G", m, &bl, &work[iwork], m, &b[i__ * b_dim1 + 1]
, ldb);
/* L40: */
		}
	    } else {
		dgemv_("T", m, m, &c_b108, &work[il], &ldwork, &b[b_dim1 + 1],
			 &c__1, &c_b74, &work[iwork], &c__1);
		dcopy_(m, &work[iwork], &c__1, &b[b_dim1 + 1], &c__1);
	    }

/*        Zero out below first M rows of B */

	    i__1 = *n - *m;
	    dlaset_("F", &i__1, nrhs, &c_b74, &c_b74, &b[*m + 1 + b_dim1],
		    ldb);
	    iwork = itau + *m;

/*        Multiply transpose(Q) by B */
/*        (Workspace: need M+NRHS, prefer M+NRHS*NB) */

	    i__1 = *lwork - iwork + 1;
	    dormlq_("L", "T", n, nrhs, m, &a[a_offset], lda, &work[itau], &b[
		    b_offset], ldb, &work[iwork], &i__1, info);

	} else {

/*        Path 2 - remaining underdetermined cases */

	    ie = 1;
	    itauq = ie + *m;
	    itaup = itauq + *m;
	    iwork = itaup + *m;

/*        Bidiagonalize A */
/*        (Workspace: need 3*M+N, prefer 3*M+(M+N)*NB) */

	    i__1 = *lwork - iwork + 1;
	    dgebrd_(m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &
		    work[itaup], &work[iwork], &i__1, info);

/*        Multiply B by transpose of left bidiagonalizing vectors */
/*        (Workspace: need 3*M+NRHS, prefer 3*M+NRHS*NB) */

	    i__1 = *lwork - iwork + 1;
	    dormbr_("Q", "L", "T", m, nrhs, n, &a[a_offset], lda, &work[itauq]
, &b[b_offset], ldb, &work[iwork], &i__1, info);

/*        Generate right bidiagonalizing vectors in A */
/*        (Workspace: need 4*M, prefer 3*M+M*NB) */

	    i__1 = *lwork - iwork + 1;
	    dorgbr_("P", m, n, m, &a[a_offset], lda, &work[itaup], &work[
		    iwork], &i__1, info);
	    iwork = ie + *m;

/*        Perform bidiagonal QR iteration, */
/*           computing right singular vectors of A in A and */
/*           multiplying B by transpose of left singular vectors */
/*        (Workspace: need BDSPAC) */

	    dbdsqr_("L", m, n, &c__0, nrhs, &s[1], &work[ie], &a[a_offset],
		    lda, vdum, &c__1, &b[b_offset], ldb, &work[iwork], info);
	    if (*info != 0) {
		goto L70;
	    }

/*        Multiply B by reciprocals of singular values */

/* Computing MAX */
	    d__1 = *rcond * s[1];
	    thr = std::max(d__1,sfmin);
	    if (*rcond < 0.) {
/* Computing MAX */
		d__1 = eps * s[1];
		thr = std::max(d__1,sfmin);
	    }
	    *rank = 0;
	    i__1 = *m;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		if (s[i__] > thr) {
		    drscl_(nrhs, &s[i__], &b[i__ + b_dim1], ldb);
		    ++(*rank);
		} else {
		    dlaset_("F", &c__1, nrhs, &c_b74, &c_b74, &b[i__ + b_dim1]
, ldb);
		}
/* L50: */
	    }

/*        Multiply B by right singular vectors of A */
/*        (Workspace: need N, prefer N*NRHS) */

	    if (*lwork >= *ldb * *nrhs && *nrhs > 1) {
		dgemm_("T", "N", n, nrhs, m, &c_b108, &a[a_offset], lda, &b[
			b_offset], ldb, &c_b74, &work[1], ldb);
		dlacpy_("F", n, nrhs, &work[1], ldb, &b[b_offset], ldb);
	    } else if (*nrhs > 1) {
		chunk = *lwork / *n;
		i__1 = *nrhs;
		i__2 = chunk;
		for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ +=
			i__2) {
/* Computing MIN */
		    i__3 = *nrhs - i__ + 1;
		    bl = std::min(i__3,chunk);
		    dgemm_("T", "N", n, &bl, m, &c_b108, &a[a_offset], lda, &
			    b[i__ * b_dim1 + 1], ldb, &c_b74, &work[1], n);
		    dlacpy_("F", n, &bl, &work[1], n, &b[i__ * b_dim1 + 1],
			    ldb);
/* L60: */
		}
	    } else {
		dgemv_("T", m, n, &c_b108, &a[a_offset], lda, &b[b_offset], &
			c__1, &c_b74, &work[1], &c__1);
		dcopy_(n, &work[1], &c__1, &b[b_offset], &c__1);
	    }
	}
    }

/*     Undo scaling */

    if (iascl == 1) {
	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, n, nrhs, &b[b_offset], ldb,
		 info);
	dlascl_("G", &c__0, &c__0, &smlnum, &anrm, &minmn, &c__1, &s[1], &
		minmn, info);
    } else if (iascl == 2) {
	dlascl_("G", &c__0, &c__0, &anrm, &bignum, n, nrhs, &b[b_offset], ldb,
		 info);
	dlascl_("G", &c__0, &c__0, &bignum, &anrm, &minmn, &c__1, &s[1], &
		minmn, info);
    }
    if (ibscl == 1) {
	dlascl_("G", &c__0, &c__0, &smlnum, &bnrm, n, nrhs, &b[b_offset], ldb,
		 info);
    } else if (ibscl == 2) {
	dlascl_("G", &c__0, &c__0, &bignum, &bnrm, n, nrhs, &b[b_offset], ldb,
		 info);
    }

L70:
    work[1] = (double) maxwrk;
    return 0;

/*     End of DGELSS */

} /* dgelss_ */

/* Subroutine */ int dgelsx_(integer *m, integer *n, integer *nrhs,
	double *a, integer *lda, double *b, integer *ldb, integer *
	jpvt, double *rcond, integer *rank, double *work, integer *
	info)
{
	/* Table of constant values */
	static integer c__0 = 0;
	static double c_b13 = 0.;
	static integer c__2 = 2;
	static integer c__1 = 1;
	static double c_b36 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;
    double d__1;

    /* Local variables */
    integer i__, j, k;
    double c1, c2, s1, s2, t1, t2;
    integer mn;
    double anrm, bnrm, smin, smax;
    integer iascl, ibscl, ismin, ismax;
	double bignum;
    double sminpr, smaxpr, smlnum;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  This routine is deprecated and has been replaced by routine DGELSY. */

/*  DGELSX computes the minimum-norm solution to a real linear least */
/*  squares problem: */
/*      minimize || A * X - B || */
/*  using a complete orthogonal factorization of A.  A is an M-by-N */
/*  matrix which may be rank-deficient. */

/*  Several right hand side vectors b and solution vectors x can be */
/*  handled in a single call; they are stored as the columns of the */
/*  M-by-NRHS right hand side matrix B and the N-by-NRHS solution */
/*  matrix X. */

/*  The routine first computes a QR factorization with column pivoting: */
/*      A * P = Q * [ R11 R12 ] */
/*                  [  0  R22 ] */
/*  with R11 defined as the largest leading submatrix whose estimated */
/*  condition number is less than 1/RCOND.  The order of R11, RANK, */
/*  is the effective rank of A. */

/*  Then, R22 is considered to be negligible, and R12 is annihilated */
/*  by orthogonal transformations from the right, arriving at the */
/*  complete orthogonal factorization: */
/*     A * P = Q * [ T11 0 ] * Z */
/*                 [  0  0 ] */
/*  The minimum-norm solution is then */
/*     X = P * Z' [ inv(T11)*Q1'*B ] */
/*                [        0       ] */
/*  where Q1 consists of the first RANK columns of Q. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of */
/*          columns of matrices B and X. NRHS >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, A has been overwritten by details of its */
/*          complete orthogonal factorization. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the M-by-NRHS right hand side matrix B. */
/*          On exit, the N-by-NRHS solution matrix X. */
/*          If m >= n and RANK = n, the residual sum-of-squares for */
/*          the solution in the i-th column is given by the sum of */
/*          squares of elements N+1:M in that column. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,M,N). */

/*  JPVT    (input/output) INTEGER array, dimension (N) */
/*          On entry, if JPVT(i) .ne. 0, the i-th column of A is an */
/*          initial column, otherwise it is a free column.  Before */
/*          the QR factorization of A, all initial columns are */
/*          permuted to the leading positions; only the remaining */
/*          free columns are moved as a result of column pivoting */
/*          during the factorization. */
/*          On exit, if JPVT(i) = k, then the i-th column of A*P */
/*          was the k-th column of A. */

/*  RCOND   (input) DOUBLE PRECISION */
/*          RCOND is used to determine the effective rank of A, which */
/*          is defined as the order of the largest leading triangular */
/*          submatrix R11 in the QR factorization with pivoting of A, */
/*          whose estimated condition number < 1/RCOND. */

/*  RANK    (output) INTEGER */
/*          The effective rank of A, i.e., the order of the submatrix */
/*          R11.  This is the same as the order of the submatrix T11 */
/*          in the complete orthogonal factorization of A. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension */
/*                      (max( min(M,N)+3*N, 2*min(M,N)+NRHS )), */

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
    --jpvt;
    --work;

    /* Function Body */
    mn = std::min(*m,*n);
    ismin = mn + 1;
    ismax = (mn << 1) + 1;

/*     Test the input arguments. */

    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -5;
    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = std::max(1_integer,*m);
	if (*ldb < std::max(i__1,*n)) {
	    *info = -7;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGELSX", &i__1);
	return 0;
    }

/*     Quick return if possible */

/* Computing MIN */
    i__1 = std::min(*m,*n);
    if (std::min(i__1,*nrhs) == 0) {
	*rank = 0;
	return 0;
    }

/*     Get machine parameters */

    smlnum = dlamch_("S") / dlamch_("P");
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);

/*     Scale A, B if max elements outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", m, n, &a[a_offset], lda, &work[1]);
    iascl = 0;
    if (anrm > 0. && anrm < smlnum) {

/*        Scale matrix norm up to SMLNUM */

	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, m, n, &a[a_offset], lda,
		info);
	iascl = 1;
    } else if (anrm > bignum) {

/*        Scale matrix norm down to BIGNUM */

	dlascl_("G", &c__0, &c__0, &anrm, &bignum, m, n, &a[a_offset], lda,
		info);
	iascl = 2;
    } else if (anrm == 0.) {

/*        Matrix all zero. Return zero solution. */

	i__1 = std::max(*m,*n);
	dlaset_("F", &i__1, nrhs, &c_b13, &c_b13, &b[b_offset], ldb);
	*rank = 0;
	goto L100;
    }

    bnrm = dlange_("M", m, nrhs, &b[b_offset], ldb, &work[1]);
    ibscl = 0;
    if (bnrm > 0. && bnrm < smlnum) {

/*        Scale matrix norm up to SMLNUM */

	dlascl_("G", &c__0, &c__0, &bnrm, &smlnum, m, nrhs, &b[b_offset], ldb,
		 info);
	ibscl = 1;
    } else if (bnrm > bignum) {

/*        Scale matrix norm down to BIGNUM */

	dlascl_("G", &c__0, &c__0, &bnrm, &bignum, m, nrhs, &b[b_offset], ldb,
		 info);
	ibscl = 2;
    }

/*     Compute QR factorization with column pivoting of A: */
/*        A * P = Q * R */

    dgeqpf_(m, n, &a[a_offset], lda, &jpvt[1], &work[1], &work[mn + 1], info);

/*     workspace 3*N. Details of Householder rotations stored */
/*     in WORK(1:MN). */

/*     Determine RANK using incremental condition estimation */

    work[ismin] = 1.;
    work[ismax] = 1.;
    smax = (d__1 = a[a_dim1 + 1], abs(d__1));
    smin = smax;
    if ((d__1 = a[a_dim1 + 1], abs(d__1)) == 0.) {
	*rank = 0;
	i__1 = std::max(*m,*n);
	dlaset_("F", &i__1, nrhs, &c_b13, &c_b13, &b[b_offset], ldb);
	goto L100;
    } else {
	*rank = 1;
    }

L10:
    if (*rank < mn) {
	i__ = *rank + 1;
	dlaic1_(&c__2, rank, &work[ismin], &smin, &a[i__ * a_dim1 + 1], &a[
		i__ + i__ * a_dim1], &sminpr, &s1, &c1);
	dlaic1_(&c__1, rank, &work[ismax], &smax, &a[i__ * a_dim1 + 1], &a[
		i__ + i__ * a_dim1], &smaxpr, &s2, &c2);

	if (smaxpr * *rcond <= sminpr) {
	    i__1 = *rank;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		work[ismin + i__ - 1] = s1 * work[ismin + i__ - 1];
		work[ismax + i__ - 1] = s2 * work[ismax + i__ - 1];
/* L20: */
	    }
	    work[ismin + *rank] = c1;
	    work[ismax + *rank] = c2;
	    smin = sminpr;
	    smax = smaxpr;
	    ++(*rank);
	    goto L10;
	}
    }

/*     Logically partition R = [ R11 R12 ] */
/*                             [  0  R22 ] */
/*     where R11 = R(1:RANK,1:RANK) */

/*     [R11,R12] = [ T11, 0 ] * Y */

    if (*rank < *n) {
	dtzrqf_(rank, n, &a[a_offset], lda, &work[mn + 1], info);
    }

/*     Details of Householder rotations stored in WORK(MN+1:2*MN) */

/*     B(1:M,1:NRHS) := Q' * B(1:M,1:NRHS) */

    dorm2r_("Left", "Transpose", m, nrhs, &mn, &a[a_offset], lda, &work[1], &
	    b[b_offset], ldb, &work[(mn << 1) + 1], info);

/*     workspace NRHS */

/*     B(1:RANK,1:NRHS) := inv(T11) * B(1:RANK,1:NRHS) */

    dtrsm_("Left", "Upper", "No transpose", "Non-unit", rank, nrhs, &c_b36, &
	    a[a_offset], lda, &b[b_offset], ldb);

    i__1 = *n;
    for (i__ = *rank + 1; i__ <= i__1; ++i__) {
	i__2 = *nrhs;
	for (j = 1; j <= i__2; ++j) {
	    b[i__ + j * b_dim1] = 0.;
/* L30: */
	}
/* L40: */
    }

/*     B(1:N,1:NRHS) := Y' * B(1:N,1:NRHS) */

    if (*rank < *n) {
	i__1 = *rank;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *n - *rank + 1;
	    dlatzm_("Left", &i__2, nrhs, &a[i__ + (*rank + 1) * a_dim1], lda,
		    &work[mn + i__], &b[i__ + b_dim1], &b[*rank + 1 + b_dim1],
		     ldb, &work[(mn << 1) + 1]);
/* L50: */
	}
    }

/*     workspace NRHS */

/*     B(1:N,1:NRHS) := P * B(1:N,1:NRHS) */

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[(mn << 1) + i__] = 1.;
/* L60: */
	}
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    if (work[(mn << 1) + i__] == 1.) {
		if (jpvt[i__] != i__) {
		    k = i__;
		    t1 = b[k + j * b_dim1];
		    t2 = b[jpvt[k] + j * b_dim1];
L70:
		    b[jpvt[k] + j * b_dim1] = t1;
		    work[(mn << 1) + k] = 0.;
		    t1 = t2;
		    k = jpvt[k];
		    t2 = b[jpvt[k] + j * b_dim1];
		    if (jpvt[k] != i__) {
			goto L70;
		    }
		    b[i__ + j * b_dim1] = t1;
		    work[(mn << 1) + k] = 0.;
		}
	    }
/* L80: */
	}
/* L90: */
    }

/*     Undo scaling */

    if (iascl == 1) {
	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, n, nrhs, &b[b_offset], ldb,
		 info);
	dlascl_("U", &c__0, &c__0, &smlnum, &anrm, rank, rank, &a[a_offset],
		lda, info);
    } else if (iascl == 2) {
	dlascl_("G", &c__0, &c__0, &anrm, &bignum, n, nrhs, &b[b_offset], ldb,
		 info);
	dlascl_("U", &c__0, &c__0, &bignum, &anrm, rank, rank, &a[a_offset],
		lda, info);
    }
    if (ibscl == 1) {
	dlascl_("G", &c__0, &c__0, &smlnum, &bnrm, n, nrhs, &b[b_offset], ldb,
		 info);
    } else if (ibscl == 2) {
	dlascl_("G", &c__0, &c__0, &bignum, &bnrm, n, nrhs, &b[b_offset], ldb,
		 info);
    }

L100:

    return 0;

/*     End of DGELSX */

} /* dgelsx_ */

/* Subroutine */ int dgelsy_(integer *m, integer *n, integer *nrhs,
	double *a, integer *lda, double *b, integer *ldb, integer *
	jpvt, double *rcond, integer *rank, double *work, integer *
	lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__0 = 0;
	static double c_b31 = 0.;
	static integer c__2 = 2;
	static double c_b54 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j;
    double c1, c2, s1, s2;
    integer nb, mn, nb1, nb2, nb3, nb4;
    double anrm, bnrm, smin, smax;
    integer iascl, ibscl;
    integer ismin, ismax;
    double wsize;
    double bignum;
    integer lwkmin;
    double sminpr, smaxpr, smlnum;
    integer lwkopt;
    bool lquery;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGELSY computes the minimum-norm solution to a real linear least */
/*  squares problem: */
/*      minimize || A * X - B || */
/*  using a complete orthogonal factorization of A.  A is an M-by-N */
/*  matrix which may be rank-deficient. */

/*  Several right hand side vectors b and solution vectors x can be */
/*  handled in a single call; they are stored as the columns of the */
/*  M-by-NRHS right hand side matrix B and the N-by-NRHS solution */
/*  matrix X. */

/*  The routine first computes a QR factorization with column pivoting: */
/*      A * P = Q * [ R11 R12 ] */
/*                  [  0  R22 ] */
/*  with R11 defined as the largest leading submatrix whose estimated */
/*  condition number is less than 1/RCOND.  The order of R11, RANK, */
/*  is the effective rank of A. */

/*  Then, R22 is considered to be negligible, and R12 is annihilated */
/*  by orthogonal transformations from the right, arriving at the */
/*  complete orthogonal factorization: */
/*     A * P = Q * [ T11 0 ] * Z */
/*                 [  0  0 ] */
/*  The minimum-norm solution is then */
/*     X = P * Z' [ inv(T11)*Q1'*B ] */
/*                [        0       ] */
/*  where Q1 consists of the first RANK columns of Q. */

/*  This routine is basically identical to the original xGELSX except */
/*  three differences: */
/*    o The call to the subroutine xGEQPF has been substituted by the */
/*      the call to the subroutine xGEQP3. This subroutine is a Blas-3 */
/*      version of the QR factorization with column pivoting. */
/*    o Matrix B (the right hand side) is updated with Blas-3. */
/*    o The permutation of matrix B (the right hand side) is faster and */
/*      more simple. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of */
/*          columns of matrices B and X. NRHS >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, A has been overwritten by details of its */
/*          complete orthogonal factorization. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the M-by-NRHS right hand side matrix B. */
/*          On exit, the N-by-NRHS solution matrix X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,M,N). */

/*  JPVT    (input/output) INTEGER array, dimension (N) */
/*          On entry, if JPVT(i) .ne. 0, the i-th column of A is permuted */
/*          to the front of AP, otherwise column i is a free column. */
/*          On exit, if JPVT(i) = k, then the i-th column of AP */
/*          was the k-th column of A. */

/*  RCOND   (input) DOUBLE PRECISION */
/*          RCOND is used to determine the effective rank of A, which */
/*          is defined as the order of the largest leading triangular */
/*          submatrix R11 in the QR factorization with pivoting of A, */
/*          whose estimated condition number < 1/RCOND. */

/*  RANK    (output) INTEGER */
/*          The effective rank of A, i.e., the order of the submatrix */
/*          R11.  This is the same as the order of the submatrix T11 */
/*          in the complete orthogonal factorization of A. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          The unblocked strategy requires that: */
/*             LWORK >= MAX( MN+3*N+1, 2*MN+NRHS ), */
/*          where MN = min( M, N ). */
/*          The block algorithm requires that: */
/*             LWORK >= MAX( MN+2*N+NB*(N+1), 2*MN+NB*NRHS ), */
/*          where NB is an upper bound on the blocksize returned */
/*          by ILAENV for the routines DGEQP3, DTZRZF, STZRQF, DORMQR, */
/*          and DORMRZ. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: If INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*    A. Petitet, Computer Science Dept., Univ. of Tenn., Knoxville, USA */
/*    E. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain */
/*    G. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain */

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
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --jpvt;
    --work;

    /* Function Body */
    mn = std::min(*m,*n);
    ismin = mn + 1;
    ismax = (mn << 1) + 1;

/*     Test the input arguments. */

    *info = 0;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -5;
    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = std::max(1_integer,*m);
	if (*ldb < std::max(i__1,*n)) {
	    *info = -7;
	}
    }

/*     Figure out optimal block size */

    if (*info == 0) {
	if (mn == 0 || *nrhs == 0) {
	    lwkmin = 1;
	    lwkopt = 1;
	} else {
	    nb1 = ilaenv_(&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1);
	    nb2 = ilaenv_(&c__1, "DGERQF", " ", m, n, &c_n1, &c_n1);
	    nb3 = ilaenv_(&c__1, "DORMQR", " ", m, n, nrhs, &c_n1);
	    nb4 = ilaenv_(&c__1, "DORMRQ", " ", m, n, nrhs, &c_n1);
/* Computing MAX */
	    i__1 = std::max(nb1,nb2), i__1 = std::max(i__1,nb3);
	    nb = std::max(i__1,nb4);
/* Computing MAX */
	    i__1 = mn << 1, i__2 = *n + 1, i__1 = std::max(i__1,i__2), i__2 = mn +
		    *nrhs;
	    lwkmin = mn + std::max(i__1,i__2);
/* Computing MAX */
	    i__1 = lwkmin, i__2 = mn + (*n << 1) + nb * (*n + 1), i__1 = std::max(
		    i__1,i__2), i__2 = (mn << 1) + nb * *nrhs;
	    lwkopt = std::max(i__1,i__2);
	}
	work[1] = (double) lwkopt;

	if (*lwork < lwkmin && ! lquery) {
	    *info = -12;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGELSY", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (mn == 0 || *nrhs == 0) {
	*rank = 0;
	return 0;
    }

/*     Get machine parameters */

    smlnum = dlamch_("S") / dlamch_("P");
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);

/*     Scale A, B if max entries outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", m, n, &a[a_offset], lda, &work[1]);
    iascl = 0;
    if (anrm > 0. && anrm < smlnum) {

/*        Scale matrix norm up to SMLNUM */

	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, m, n, &a[a_offset], lda,
		info);
	iascl = 1;
    } else if (anrm > bignum) {

/*        Scale matrix norm down to BIGNUM */

	dlascl_("G", &c__0, &c__0, &anrm, &bignum, m, n, &a[a_offset], lda,
		info);
	iascl = 2;
    } else if (anrm == 0.) {

/*        Matrix all zero. Return zero solution. */

	i__1 = std::max(*m,*n);
	dlaset_("F", &i__1, nrhs, &c_b31, &c_b31, &b[b_offset], ldb);
	*rank = 0;
	goto L70;
    }

    bnrm = dlange_("M", m, nrhs, &b[b_offset], ldb, &work[1]);
    ibscl = 0;
    if (bnrm > 0. && bnrm < smlnum) {

/*        Scale matrix norm up to SMLNUM */

	dlascl_("G", &c__0, &c__0, &bnrm, &smlnum, m, nrhs, &b[b_offset], ldb,
		 info);
	ibscl = 1;
    } else if (bnrm > bignum) {

/*        Scale matrix norm down to BIGNUM */

	dlascl_("G", &c__0, &c__0, &bnrm, &bignum, m, nrhs, &b[b_offset], ldb,
		 info);
	ibscl = 2;
    }

/*     Compute QR factorization with column pivoting of A: */
/*        A * P = Q * R */

    i__1 = *lwork - mn;
    dgeqp3_(m, n, &a[a_offset], lda, &jpvt[1], &work[1], &work[mn + 1], &i__1,
	     info);
    wsize = mn + work[mn + 1];

/*     workspace: MN+2*N+NB*(N+1). */
/*     Details of Householder rotations stored in WORK(1:MN). */

/*     Determine RANK using incremental condition estimation */

    work[ismin] = 1.;
    work[ismax] = 1.;
    smax = (d__1 = a[a_dim1 + 1], abs(d__1));
    smin = smax;
    if ((d__1 = a[a_dim1 + 1], abs(d__1)) == 0.) {
	*rank = 0;
	i__1 = std::max(*m,*n);
	dlaset_("F", &i__1, nrhs, &c_b31, &c_b31, &b[b_offset], ldb);
	goto L70;
    } else {
	*rank = 1;
    }

L10:
    if (*rank < mn) {
	i__ = *rank + 1;
	dlaic1_(&c__2, rank, &work[ismin], &smin, &a[i__ * a_dim1 + 1], &a[
		i__ + i__ * a_dim1], &sminpr, &s1, &c1);
	dlaic1_(&c__1, rank, &work[ismax], &smax, &a[i__ * a_dim1 + 1], &a[
		i__ + i__ * a_dim1], &smaxpr, &s2, &c2);

	if (smaxpr * *rcond <= sminpr) {
	    i__1 = *rank;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		work[ismin + i__ - 1] = s1 * work[ismin + i__ - 1];
		work[ismax + i__ - 1] = s2 * work[ismax + i__ - 1];
/* L20: */
	    }
	    work[ismin + *rank] = c1;
	    work[ismax + *rank] = c2;
	    smin = sminpr;
	    smax = smaxpr;
	    ++(*rank);
	    goto L10;
	}
    }

/*     workspace: 3*MN. */

/*     Logically partition R = [ R11 R12 ] */
/*                             [  0  R22 ] */
/*     where R11 = R(1:RANK,1:RANK) */

/*     [R11,R12] = [ T11, 0 ] * Y */

    if (*rank < *n) {
	i__1 = *lwork - (mn << 1);
	dtzrzf_(rank, n, &a[a_offset], lda, &work[mn + 1], &work[(mn << 1) +
		1], &i__1, info);
    }

/*     workspace: 2*MN. */
/*     Details of Householder rotations stored in WORK(MN+1:2*MN) */

/*     B(1:M,1:NRHS) := Q' * B(1:M,1:NRHS) */

    i__1 = *lwork - (mn << 1);
    dormqr_("Left", "Transpose", m, nrhs, &mn, &a[a_offset], lda, &work[1], &
	    b[b_offset], ldb, &work[(mn << 1) + 1], &i__1, info);
/* Computing MAX */
    d__1 = wsize, d__2 = (mn << 1) + work[(mn << 1) + 1];
    wsize = std::max(d__1,d__2);

/*     workspace: 2*MN+NB*NRHS. */

/*     B(1:RANK,1:NRHS) := inv(T11) * B(1:RANK,1:NRHS) */

    dtrsm_("Left", "Upper", "No transpose", "Non-unit", rank, nrhs, &c_b54, &
	    a[a_offset], lda, &b[b_offset], ldb);

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *n;
	for (i__ = *rank + 1; i__ <= i__2; ++i__) {
	    b[i__ + j * b_dim1] = 0.;
/* L30: */
	}
/* L40: */
    }

/*     B(1:N,1:NRHS) := Y' * B(1:N,1:NRHS) */

    if (*rank < *n) {
	i__1 = *n - *rank;
	i__2 = *lwork - (mn << 1);
	dormrz_("Left", "Transpose", n, nrhs, rank, &i__1, &a[a_offset], lda,
		&work[mn + 1], &b[b_offset], ldb, &work[(mn << 1) + 1], &i__2,
		 info);
    }

/*     workspace: 2*MN+NRHS. */

/*     B(1:N,1:NRHS) := P * B(1:N,1:NRHS) */

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[jpvt[i__]] = b[i__ + j * b_dim1];
/* L50: */
	}
	dcopy_(n, &work[1], &c__1, &b[j * b_dim1 + 1], &c__1);
/* L60: */
    }

/*     workspace: N. */

/*     Undo scaling */

    if (iascl == 1) {
	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, n, nrhs, &b[b_offset], ldb,
		 info);
	dlascl_("U", &c__0, &c__0, &smlnum, &anrm, rank, rank, &a[a_offset],
		lda, info);
    } else if (iascl == 2) {
	dlascl_("G", &c__0, &c__0, &anrm, &bignum, n, nrhs, &b[b_offset], ldb,
		 info);
	dlascl_("U", &c__0, &c__0, &bignum, &anrm, rank, rank, &a[a_offset],
		lda, info);
    }
    if (ibscl == 1) {
	dlascl_("G", &c__0, &c__0, &smlnum, &bnrm, n, nrhs, &b[b_offset], ldb,
		 info);
    } else if (ibscl == 2) {
	dlascl_("G", &c__0, &c__0, &bignum, &bnrm, n, nrhs, &b[b_offset], ldb,
		 info);
    }

L70:
    work[1] = (double) lwkopt;

    return 0;

/*     End of DGELSY */

} /* dgelsy_ */

/* Subroutine */ int dgeql2_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    integer i__, k;
    double aii;

/*  -- LAPACK routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGEQL2 computes a QL factorization of a real m by n matrix A: */
/*  A = Q * L. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the m by n matrix A. */
/*          On exit, if m >= n, the lower triangle of the subarray */
/*          A(m-n+1:m,1:n) contains the n by n lower triangular matrix L; */
/*          if m <= n, the elements on and below the (n-m)-th */
/*          superdiagonal contain the m by n lower trapezoidal matrix L; */
/*          the remaining elements, with the array TAU, represent the */
/*          orthogonal matrix Q as a product of elementary reflectors */
/*          (see Further Details). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors (see Further */
/*          Details). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of elementary reflectors */

/*     Q = H(k) . . . H(2) H(1), where k = min(m,n). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(m-k+i+1:m) = 0 and v(m-k+i) = 1; v(1:m-k+i-1) is stored on exit in */
/*  A(1:m-k+i-1,n-k+i), and tau in TAU(i). */

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

/*     Test the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEQL2", &i__1);
	return 0;
    }

    k = std::min(*m,*n);

    for (i__ = k; i__ >= 1; --i__) {

/*        Generate elementary reflector H(i) to annihilate */
/*        A(1:m-k+i-1,n-k+i) */

	i__1 = *m - k + i__;
	dlarfp_(&i__1, &a[*m - k + i__ + (*n - k + i__) * a_dim1], &a[(*n - k
		+ i__) * a_dim1 + 1], &c__1, &tau[i__]);

/*        Apply H(i) to A(1:m-k+i,1:n-k+i-1) from the left */

	aii = a[*m - k + i__ + (*n - k + i__) * a_dim1];
	a[*m - k + i__ + (*n - k + i__) * a_dim1] = 1.;
	i__1 = *m - k + i__;
	i__2 = *n - k + i__ - 1;
	dlarf_("Left", &i__1, &i__2, &a[(*n - k + i__) * a_dim1 + 1], &c__1, &
		tau[i__], &a[a_offset], lda, &work[1]);
	a[*m - k + i__ + (*n - k + i__) * a_dim1] = aii;
/* L10: */
    }
    return 0;

/*     End of DGEQL2 */

} /* dgeql2_ */


/* Subroutine */ int dgeqlf_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__3 = 3;
	static integer c__2 = 2;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    integer i__, k, ib, nb, ki, kk, mu, nu, nx, iws, nbmin, iinfo;
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

/*  DGEQLF computes a QL factorization of a real M-by-N matrix A: */
/*  A = Q * L. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, */
/*          if m >= n, the lower triangle of the subarray */
/*          A(m-n+1:m,1:n) contains the N-by-N lower triangular matrix L; */
/*          if m <= n, the elements on and below the (n-m)-th */
/*          superdiagonal contain the M-by-N lower trapezoidal matrix L; */
/*          the remaining elements, with the array TAU, represent the */
/*          orthogonal matrix Q as a product of elementary reflectors */
/*          (see Further Details). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors (see Further */
/*          Details). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= max(1,N). */
/*          For optimum performance LWORK >= N*NB, where NB is the */
/*          optimal blocksize. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of elementary reflectors */

/*     Q = H(k) . . . H(2) H(1), where k = min(m,n). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(m-k+i+1:m) = 0 and v(m-k+i) = 1; v(1:m-k+i-1) is stored on exit in */
/*  A(1:m-k+i-1,n-k+i), and tau in TAU(i). */

/*  ===================================================================== */

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
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    }

    if (*info == 0) {
	k = std::min(*m,*n);
	if (k == 0) {
	    lwkopt = 1;
	} else {
	    nb = ilaenv_(&c__1, "DGEQLF", " ", m, n, &c_n1, &c_n1);
	    lwkopt = *n * nb;
	}
	work[1] = (double) lwkopt;

	if (*lwork < std::max(1_integer,*n) && ! lquery) {
	    *info = -7;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEQLF", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (k == 0) {
	return 0;
    }

    nbmin = 2;
    nx = 1;
    iws = *n;
    if (nb > 1 && nb < k) {

/*        Determine when to cross over from blocked to unblocked code. */

/* Computing MAX */
	i__1 = 0, i__2 = ilaenv_(&c__3, "DGEQLF", " ", m, n, &c_n1, &c_n1);
	nx = std::max(i__1,i__2);
	if (nx < k) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *n;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*              Not enough workspace to use optimal NB:  reduce NB and */
/*              determine the minimum value of NB. */

		nb = *lwork / ldwork;
/* Computing MAX */
		i__1 = 2, i__2 = ilaenv_(&c__2, "DGEQLF", " ", m, n, &c_n1, &
			c_n1);
		nbmin = std::max(i__1,i__2);
	    }
	}
    }

    if (nb >= nbmin && nb < k && nx < k) {

/*        Use blocked code initially. */
/*        The last kk columns are handled by the block method. */

	ki = (k - nx - 1) / nb * nb;
/* Computing MIN */
	i__1 = k, i__2 = ki + nb;
	kk = std::min(i__1,i__2);

	i__1 = k - kk + 1;
	i__2 = -nb;
	for (i__ = k - kk + ki + 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__
		+= i__2) {
/* Computing MIN */
	    i__3 = k - i__ + 1;
	    ib = std::min(i__3,nb);

/*           Compute the QL factorization of the current block */
/*           A(1:m-k+i+ib-1,n-k+i:n-k+i+ib-1) */

	    i__3 = *m - k + i__ + ib - 1;
	    dgeql2_(&i__3, &ib, &a[(*n - k + i__) * a_dim1 + 1], lda, &tau[
		    i__], &work[1], &iinfo);
	    if (*n - k + i__ > 1) {

/*              Form the triangular factor of the block reflector */
/*              H = H(i+ib-1) . . . H(i+1) H(i) */

		i__3 = *m - k + i__ + ib - 1;
		dlarft_("Backward", "Columnwise", &i__3, &ib, &a[(*n - k +
			i__) * a_dim1 + 1], lda, &tau[i__], &work[1], &ldwork);

/*              Apply H' to A(1:m-k+i+ib-1,1:n-k+i-1) from the left */

		i__3 = *m - k + i__ + ib - 1;
		i__4 = *n - k + i__ - 1;
		dlarfb_("Left", "Transpose", "Backward", "Columnwise", &i__3,
			&i__4, &ib, &a[(*n - k + i__) * a_dim1 + 1], lda, &
			work[1], &ldwork, &a[a_offset], lda, &work[ib + 1], &
			ldwork);
	    }
/* L10: */
	}
	mu = *m - k + i__ + nb - 1;
	nu = *n - k + i__ + nb - 1;
    } else {
	mu = *m;
	nu = *n;
    }

/*     Use unblocked code to factor the last or only block */

    if (mu > 0 && nu > 0) {
	dgeql2_(&mu, &nu, &a[a_offset], lda, &tau[1], &work[1], &iinfo);
    }

    work[1] = (double) iws;
    return 0;

/*     End of DGEQLF */

} /* dgeqlf_ */

/* Subroutine */ int dgeqp3_(integer *m, integer *n, double *a, integer *
	lda, integer *jpvt, double *tau, double *work, integer *lwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__3 = 3;
	static integer c__2 = 2;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    integer j, jb, na, nb, sm, sn, nx, fjb, iws, nfxd;
    integer nbmin, minmn;
    integer minws;
    integer topbmn, sminmn;
    integer lwkopt;
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

/*  DGEQP3 computes a QR factorization with column pivoting of a */
/*  matrix A:  A*P = Q*R  using Level 3 BLAS. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A. M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, the upper triangle of the array contains the */
/*          min(M,N)-by-N upper trapezoidal matrix R; the elements below */
/*          the diagonal, together with the array TAU, represent the */
/*          orthogonal matrix Q as a product of min(M,N) elementary */
/*          reflectors. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,M). */

/*  JPVT    (input/output) INTEGER array, dimension (N) */
/*          On entry, if JPVT(J).ne.0, the J-th column of A is permuted */
/*          to the front of A*P (a leading column); if JPVT(J)=0, */
/*          the J-th column of A is a free column. */
/*          On exit, if JPVT(J)=K, then the J-th column of A*P was the */
/*          the K-th column of A. */

/*  TAU     (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO=0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. LWORK >= 3*N+1. */
/*          For optimal performance LWORK >= 2*N+( N+1 )*NB, where NB */
/*          is the optimal blocksize. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit. */
/*          < 0: if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of elementary reflectors */

/*     Q = H(1) H(2) . . . H(k), where k = min(m,n). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real/complex scalar, and v is a real/complex vector */
/*  with v(1:i-1) = 0 and v(i) = 1; v(i+1:m) is stored on exit in */
/*  A(i+1:m,i), and tau in TAU(i). */

/*  Based on contributions by */
/*    G. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain */
/*    X. Sun, Computer Science Dept., Duke University, USA */

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

/*     Test input arguments */
/*     ==================== */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --jpvt;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    }

    if (*info == 0) {
	minmn = std::min(*m,*n);
	if (minmn == 0) {
	    iws = 1;
	    lwkopt = 1;
	} else {
	    iws = *n * 3 + 1;
	    nb = ilaenv_(&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1);
	    lwkopt = (*n << 1) + (*n + 1) * nb;
	}
	work[1] = (double) lwkopt;

	if (*lwork < iws && ! lquery) {
	    *info = -8;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEQP3", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible. */

    if (minmn == 0) {
	return 0;
    }

/*     Move initial columns up front. */

    nfxd = 1;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	if (jpvt[j] != 0) {
	    if (j != nfxd) {
		dswap_(m, &a[j * a_dim1 + 1], &c__1, &a[nfxd * a_dim1 + 1], &
			c__1);
		jpvt[j] = jpvt[nfxd];
		jpvt[nfxd] = j;
	    } else {
		jpvt[j] = j;
	    }
	    ++nfxd;
	} else {
	    jpvt[j] = j;
	}
/* L10: */
    }
    --nfxd;

/*     Factorize fixed columns */
/*     ======================= */

/*     Compute the QR factorization of fixed columns and update */
/*     remaining columns. */

    if (nfxd > 0) {
	na = std::min(*m,nfxd);
/* CC      CALL DGEQR2( M, NA, A, LDA, TAU, WORK, INFO ) */
	dgeqrf_(m, &na, &a[a_offset], lda, &tau[1], &work[1], lwork, info);
/* Computing MAX */
	i__1 = iws, i__2 = (integer) work[1];
	iws = std::max(i__1,i__2);
	if (na < *n) {
/* CC         CALL DORM2R( 'Left', 'Transpose', M, N-NA, NA, A, LDA, */
/* CC  $                   TAU, A( 1, NA+1 ), LDA, WORK, INFO ) */
	    i__1 = *n - na;
	    dormqr_("Left", "Transpose", m, &i__1, &na, &a[a_offset], lda, &
		    tau[1], &a[(na + 1) * a_dim1 + 1], lda, &work[1], lwork,
		    info);
/* Computing MAX */
	    i__1 = iws, i__2 = (integer) work[1];
	    iws = std::max(i__1,i__2);
	}
    }

/*     Factorize free columns */
/*     ====================== */

    if (nfxd < minmn) {

	sm = *m - nfxd;
	sn = *n - nfxd;
	sminmn = minmn - nfxd;

/*        Determine the block size. */

	nb = ilaenv_(&c__1, "DGEQRF", " ", &sm, &sn, &c_n1, &c_n1);
	nbmin = 2;
	nx = 0;

	if (nb > 1 && nb < sminmn) {

/*           Determine when to cross over from blocked to unblocked code. */

/* Computing MAX */
	    i__1 = 0, i__2 = ilaenv_(&c__3, "DGEQRF", " ", &sm, &sn, &c_n1, &
		    c_n1);
	    nx = std::max(i__1,i__2);


	    if (nx < sminmn) {

/*              Determine if workspace is large enough for blocked code. */

		minws = (sn << 1) + (sn + 1) * nb;
		iws = std::max(iws,minws);
		if (*lwork < minws) {

/*                 Not enough workspace to use optimal NB: Reduce NB and */
/*                 determine the minimum value of NB. */

		    nb = (*lwork - (sn << 1)) / (sn + 1);
/* Computing MAX */
		    i__1 = 2, i__2 = ilaenv_(&c__2, "DGEQRF", " ", &sm, &sn, &
			    c_n1, &c_n1);
		    nbmin = std::max(i__1,i__2);


		}
	    }
	}

/*        Initialize partial column norms. The first N elements of work */
/*        store the exact column norms. */

	i__1 = *n;
	for (j = nfxd + 1; j <= i__1; ++j) {
	    work[j] = dnrm2_(&sm, &a[nfxd + 1 + j * a_dim1], &c__1);
	    work[*n + j] = work[j];
/* L20: */
	}

	if (nb >= nbmin && nb < sminmn && nx < sminmn) {

/*           Use blocked code initially. */

	    j = nfxd + 1;

/*           Compute factorization: while loop. */


	    topbmn = minmn - nx;
L30:
	    if (j <= topbmn) {
/* Computing MIN */
		i__1 = nb, i__2 = topbmn - j + 1;
		jb = std::min(i__1,i__2);

/*              Factorize JB columns among columns J:N. */

		i__1 = *n - j + 1;
		i__2 = j - 1;
		i__3 = *n - j + 1;
		dlaqps_(m, &i__1, &i__2, &jb, &fjb, &a[j * a_dim1 + 1], lda, &
			jpvt[j], &tau[j], &work[j], &work[*n + j], &work[(*n
			<< 1) + 1], &work[(*n << 1) + jb + 1], &i__3);

		j += fjb;
		goto L30;
	    }
	} else {
	    j = nfxd + 1;
	}

/*        Use unblocked code to factor the last or only block. */


	if (j <= minmn) {
	    i__1 = *n - j + 1;
	    i__2 = j - 1;
	    dlaqp2_(m, &i__1, &i__2, &a[j * a_dim1 + 1], lda, &jpvt[j], &tau[
		    j], &work[j], &work[*n + j], &work[(*n << 1) + 1]);
	}

    }

    work[1] = (double) iws;
    return 0;

/*     End of DGEQP3 */

} /* dgeqp3_ */

/* Subroutine */ int dgeqpf_(integer *m, integer *n, double *a, integer *
	lda, integer *jpvt, double *tau, double *work, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    double d__1, d__2;

    /* Local variables */
    integer i__, j, ma, mn;
    double aii;
    integer pvt;
    double temp;
    double temp2, tol3z;
    integer itemp;

/*  -- LAPACK deprecated driver routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  This routine is deprecated and has been replaced by routine DGEQP3. */

/*  DGEQPF computes a QR factorization with column pivoting of a */
/*  real M-by-N matrix A: A*P = Q*R. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A. M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A. N >= 0 */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, the upper triangle of the array contains the */
/*          min(M,N)-by-N upper triangular matrix R; the elements */
/*          below the diagonal, together with the array TAU, */
/*          represent the orthogonal matrix Q as a product of */
/*          min(m,n) elementary reflectors. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,M). */

/*  JPVT    (input/output) INTEGER array, dimension (N) */
/*          On entry, if JPVT(i) .ne. 0, the i-th column of A is permuted */
/*          to the front of A*P (a leading column); if JPVT(i) = 0, */
/*          the i-th column of A is a free column. */
/*          On exit, if JPVT(i) = k, then the i-th column of A*P */
/*          was the k-th column of A. */

/*  TAU     (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (3*N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of elementary reflectors */

/*     Q = H(1) H(2) . . . H(n) */

/*  Each H(i) has the form */

/*     H = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(1:i-1) = 0 and v(i) = 1; v(i+1:m) is stored on exit in A(i+1:m,i). */

/*  The matrix P is represented in jpvt as follows: If */
/*     jpvt(j) = i */
/*  then the jth column of P is the ith canonical unit vector. */

/*  Partial column norm updating strategy modified by */
/*    Z. Drmac and Z. Bujanovic, Dept. of Mathematics, */
/*    University of Zagreb, Croatia. */
/*    June 2006. */
/*  For more details see LAPACK Working Note 176. */

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
    --jpvt;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEQPF", &i__1);
	return 0;
    }

    mn = std::min(*m,*n);
    tol3z = sqrt(dlamch_("Epsilon"));

/*     Move initial columns up front */

    itemp = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (jpvt[i__] != 0) {
	    if (i__ != itemp) {
		dswap_(m, &a[i__ * a_dim1 + 1], &c__1, &a[itemp * a_dim1 + 1],
			 &c__1);
		jpvt[i__] = jpvt[itemp];
		jpvt[itemp] = i__;
	    } else {
		jpvt[i__] = i__;
	    }
	    ++itemp;
	} else {
	    jpvt[i__] = i__;
	}
/* L10: */
    }
    --itemp;

/*     Compute the QR factorization and update remaining columns */

    if (itemp > 0) {
	ma = std::min(itemp,*m);
	dgeqr2_(m, &ma, &a[a_offset], lda, &tau[1], &work[1], info);
	if (ma < *n) {
	    i__1 = *n - ma;
	    dorm2r_("Left", "Transpose", m, &i__1, &ma, &a[a_offset], lda, &
		    tau[1], &a[(ma + 1) * a_dim1 + 1], lda, &work[1], info);
	}
    }

    if (itemp < mn) {

/*        Initialize partial column norms. The first n elements of */
/*        work store the exact column norms. */

	i__1 = *n;
	for (i__ = itemp + 1; i__ <= i__1; ++i__) {
	    i__2 = *m - itemp;
	    work[i__] = dnrm2_(&i__2, &a[itemp + 1 + i__ * a_dim1], &c__1);
	    work[*n + i__] = work[i__];
/* L20: */
	}

/*        Compute factorization */

	i__1 = mn;
	for (i__ = itemp + 1; i__ <= i__1; ++i__) {

/*           Determine ith pivot column and swap if necessary */

	    i__2 = *n - i__ + 1;
	    pvt = i__ - 1 + idamax_(&i__2, &work[i__], &c__1);

	    if (pvt != i__) {
		dswap_(m, &a[pvt * a_dim1 + 1], &c__1, &a[i__ * a_dim1 + 1], &
			c__1);
		itemp = jpvt[pvt];
		jpvt[pvt] = jpvt[i__];
		jpvt[i__] = itemp;
		work[pvt] = work[i__];
		work[*n + pvt] = work[*n + i__];
	    }

/*           Generate elementary reflector H(i) */

	    if (i__ < *m) {
		i__2 = *m - i__ + 1;
		dlarfp_(&i__2, &a[i__ + i__ * a_dim1], &a[i__ + 1 + i__ *
			a_dim1], &c__1, &tau[i__]);
	    } else {
		dlarfp_(&c__1, &a[*m + *m * a_dim1], &a[*m + *m * a_dim1], &
			c__1, &tau[*m]);
	    }

	    if (i__ < *n) {

/*              Apply H(i) to A(i:m,i+1:n) from the left */

		aii = a[i__ + i__ * a_dim1];
		a[i__ + i__ * a_dim1] = 1.;
		i__2 = *m - i__ + 1;
		i__3 = *n - i__;
		dlarf_("LEFT", &i__2, &i__3, &a[i__ + i__ * a_dim1], &c__1, &
			tau[i__], &a[i__ + (i__ + 1) * a_dim1], lda, &work[(*
			n << 1) + 1]);
		a[i__ + i__ * a_dim1] = aii;
	    }

/*           Update partial column norms */

	    i__2 = *n;
	    for (j = i__ + 1; j <= i__2; ++j) {
		if (work[j] != 0.) {

/*                 NOTE: The following 4 lines follow from the analysis in */
/*                 Lapack Working Note 176. */

		    temp = (d__1 = a[i__ + j * a_dim1], abs(d__1)) / work[j];
/* Computing MAX */
		    d__1 = 0., d__2 = (temp + 1.) * (1. - temp);
		    temp = std::max(d__1,d__2);
/* Computing 2nd power */
		    d__1 = work[j] / work[*n + j];
		    temp2 = temp * (d__1 * d__1);
		    if (temp2 <= tol3z) {
			if (*m - i__ > 0) {
			    i__3 = *m - i__;
			    work[j] = dnrm2_(&i__3, &a[i__ + 1 + j * a_dim1],
				    &c__1);
			    work[*n + j] = work[j];
			} else {
			    work[j] = 0.;
			    work[*n + j] = 0.;
			}
		    } else {
			work[j] *= sqrt(temp);
		    }
		}
/* L30: */
	    }

/* L40: */
	}
    }
    return 0;

/*     End of DGEQPF */

} /* dgeqpf_ */

/* Subroutine */ int dgeqr2_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    integer i__, k;
    double aii;

/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGEQR2 computes a QR factorization of a real m by n matrix A: */
/*  A = Q * R. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the m by n matrix A. */
/*          On exit, the elements on and above the diagonal of the array */
/*          contain the min(m,n) by n upper trapezoidal matrix R (R is */
/*          upper triangular if m >= n); the elements below the diagonal, */
/*          with the array TAU, represent the orthogonal matrix Q as a */
/*          product of elementary reflectors (see Further Details). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors (see Further */
/*          Details). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of elementary reflectors */

/*     Q = H(1) H(2) . . . H(k), where k = min(m,n). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(1:i-1) = 0 and v(i) = 1; v(i+1:m) is stored on exit in A(i+1:m,i), */
/*  and tau in TAU(i). */

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

/*     Test the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEQR2", &i__1);
	return 0;
    }

    k = std::min(*m,*n);

    i__1 = k;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*        Generate elementary reflector H(i) to annihilate A(i+1:m,i) */

	i__2 = *m - i__ + 1;
/* Computing MIN */
	i__3 = i__ + 1;
	dlarfp_(&i__2, &a[i__ + i__ * a_dim1], &a[std::min(i__3, *m)+ i__ * a_dim1]
, &c__1, &tau[i__]);
	if (i__ < *n) {

/*           Apply H(i) to A(i:m,i+1:n) from the left */

	    aii = a[i__ + i__ * a_dim1];
	    a[i__ + i__ * a_dim1] = 1.;
	    i__2 = *m - i__ + 1;
	    i__3 = *n - i__;
	    dlarf_("Left", &i__2, &i__3, &a[i__ + i__ * a_dim1], &c__1, &tau[
		    i__], &a[i__ + (i__ + 1) * a_dim1], lda, &work[1]);
	    a[i__ + i__ * a_dim1] = aii;
	}
/* L10: */
    }
    return 0;

/*     End of DGEQR2 */

} /* dgeqr2_ */


/* Subroutine */ int dgeqrf_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__3 = 3;
	static integer c__2 = 2;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    integer i__, k, ib, nb, nx, iws, nbmin, iinfo;
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

/*  DGEQRF computes a QR factorization of a real M-by-N matrix A: */
/*  A = Q * R. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, the elements on and above the diagonal of the array */
/*          contain the min(M,N)-by-N upper trapezoidal matrix R (R is */
/*          upper triangular if m >= n); the elements below the diagonal, */
/*          with the array TAU, represent the orthogonal matrix Q as a */
/*          product of min(m,n) elementary reflectors (see Further */
/*          Details). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors (see Further */
/*          Details). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= max(1,N). */
/*          For optimum performance LWORK >= N*NB, where NB is */
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

/*  The matrix Q is represented as a product of elementary reflectors */

/*     Q = H(1) H(2) . . . H(k), where k = min(m,n). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(1:i-1) = 0 and v(i) = 1; v(i+1:m) is stored on exit in A(i+1:m,i), */
/*  and tau in TAU(i). */

/*  ===================================================================== */

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
    nb = ilaenv_(&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1);
    lwkopt = *n * nb;
    work[1] = (double) lwkopt;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    } else if (*lwork < std::max(1_integer,*n) && ! lquery) {
	*info = -7;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGEQRF", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    k = std::min(*m,*n);
    if (k == 0) {
	work[1] = 1.;
	return 0;
    }

    nbmin = 2;
    nx = 0;
    iws = *n;
    if (nb > 1 && nb < k) {

/*        Determine when to cross over from blocked to unblocked code. */

/* Computing MAX */
	i__1 = 0, i__2 = ilaenv_(&c__3, "DGEQRF", " ", m, n, &c_n1, &c_n1);
	nx = std::max(i__1,i__2);
	if (nx < k) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *n;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*              Not enough workspace to use optimal NB:  reduce NB and */
/*              determine the minimum value of NB. */

		nb = *lwork / ldwork;
/* Computing MAX */
		i__1 = 2, i__2 = ilaenv_(&c__2, "DGEQRF", " ", m, n, &c_n1, &
			c_n1);
		nbmin = std::max(i__1,i__2);
	    }
	}
    }

    if (nb >= nbmin && nb < k && nx < k) {

/*        Use blocked code initially */

	i__1 = k - nx;
	i__2 = nb;
	for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ += i__2) {
/* Computing MIN */
	    i__3 = k - i__ + 1;
	    ib = std::min(i__3,nb);

/*           Compute the QR factorization of the current block */
/*           A(i:m,i:i+ib-1) */

	    i__3 = *m - i__ + 1;
	    dgeqr2_(&i__3, &ib, &a[i__ + i__ * a_dim1], lda, &tau[i__], &work[
		    1], &iinfo);
	    if (i__ + ib <= *n) {

/*              Form the triangular factor of the block reflector */
/*              H = H(i) H(i+1) . . . H(i+ib-1) */

		i__3 = *m - i__ + 1;
		dlarft_("Forward", "Columnwise", &i__3, &ib, &a[i__ + i__ *
			a_dim1], lda, &tau[i__], &work[1], &ldwork);

/*              Apply H' to A(i:m,i+ib:n) from the left */

		i__3 = *m - i__ + 1;
		i__4 = *n - i__ - ib + 1;
		dlarfb_("Left", "Transpose", "Forward", "Columnwise", &i__3, &
			i__4, &ib, &a[i__ + i__ * a_dim1], lda, &work[1], &
			ldwork, &a[i__ + (i__ + ib) * a_dim1], lda, &work[ib
			+ 1], &ldwork);
	    }
/* L10: */
	}
    } else {
	i__ = 1;
    }

/*     Use unblocked code to factor the last or only block. */

    if (i__ <= k) {
	i__2 = *m - i__ + 1;
	i__1 = *n - i__ + 1;
	dgeqr2_(&i__2, &i__1, &a[i__ + i__ * a_dim1], lda, &tau[i__], &work[1]
, &iinfo);
    }

    work[1] = (double) iws;
    return 0;

/*     End of DGEQRF */

} /* dgeqrf_ */

/* Subroutine */ int dgerfs_(const char *trans, integer *n, integer *nrhs,
	double *a, integer *lda, double *af, integer *ldaf, integer *
	ipiv, double *b, integer *ldb, double *x, integer *ldx,
	double *ferr, double *berr, double *work, integer *iwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b15 = -1.;
	static double c_b17 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, b_dim1, b_offset, x_dim1,
	    x_offset, i__1, i__2, i__3;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, k;
    double s, xk;
    integer nz;
    double eps;
    integer kase;
    double safe1, safe2;
    integer isave[3];
    integer count;
    double safmin;
    bool notran;
    char transt[1];
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

/*  DGERFS improves the computed solution to a system of linear */
/*  equations and provides error bounds and backward error estimates for */
/*  the solution. */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations: */
/*          = 'N':  A * X = B     (No transpose) */
/*          = 'T':  A**T * X = B  (Transpose) */
/*          = 'C':  A**H * X = B  (Conjugate transpose = Transpose) */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X.  NRHS >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The original N-by-N matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  AF      (input) DOUBLE PRECISION array, dimension (LDAF,N) */
/*          The factors L and U from the factorization A = P*L*U */
/*          as computed by DGETRF. */

/*  LDAF    (input) INTEGER */
/*          The leading dimension of the array AF.  LDAF >= max(1,N). */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices from DGETRF; for 1<=i<=N, row i of the */
/*          matrix was interchanged with row IPIV(i). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          The right hand side matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (input/output) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          On entry, the solution matrix X, as computed by DGETRS. */
/*          On exit, the improved solution matrix X. */

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

/*  Internal Parameters */
/*  =================== */

/*  ITMAX is the maximum number of steps of iterative refinement. */

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
    af_dim1 = *ldaf;
    af_offset = 1 + af_dim1;
    af -= af_offset;
    --ipiv;
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
    notran = lsame_(trans, "N");
    if (! notran && ! lsame_(trans, "T") && ! lsame_(
	    trans, "C")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    } else if (*ldaf < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -10;
    } else if (*ldx < std::max(1_integer,*n)) {
	*info = -12;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGERFS", &i__1);
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

	count = 1;
	lstres = 3.;
L20:

/*        Loop until stopping criterion is satisfied. */

/*        Compute residual R = B - op(A) * X, */
/*        where op(A) = A, A**T, or A**H, depending on TRANS. */

	dcopy_(n, &b[j * b_dim1 + 1], &c__1, &work[*n + 1], &c__1);
	dgemv_(trans, n, n, &c_b15, &a[a_offset], lda, &x[j * x_dim1 + 1], &
		c__1, &c_b17, &work[*n + 1], &c__1);

/*        Compute componentwise relative backward error from formula */

/*        max(i) ( abs(R(i)) / ( abs(op(A))*abs(X) + abs(B) )(i) ) */

/*        where abs(Z) is the componentwise absolute value of the matrix */
/*        or vector Z.  If the i-th component of the denominator is less */
/*        than SAFE2, then SAFE1 is added to the i-th components of the */
/*        numerator and denominator before dividing. */

	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[i__] = (d__1 = b[i__ + j * b_dim1], abs(d__1));
/* L30: */
	}

/*        Compute abs(op(A))*abs(X) + abs(B). */

	if (notran) {
	    i__2 = *n;
	    for (k = 1; k <= i__2; ++k) {
		xk = (d__1 = x[k + j * x_dim1], abs(d__1));
		i__3 = *n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    work[i__] += (d__1 = a[i__ + k * a_dim1], abs(d__1)) * xk;
/* L40: */
		}
/* L50: */
	    }
	} else {
	    i__2 = *n;
	    for (k = 1; k <= i__2; ++k) {
		s = 0.;
		i__3 = *n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    s += (d__1 = a[i__ + k * a_dim1], abs(d__1)) * (d__2 = x[
			    i__ + j * x_dim1], abs(d__2));
/* L60: */
		}
		work[k] += s;
/* L70: */
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
/* L80: */
	}
	berr[j] = s;

/*        Test stopping criterion. Continue iterating if */
/*           1) The residual BERR(J) is larger than machine epsilon, and */
/*           2) BERR(J) decreased by at least a factor of 2 during the */
/*              last iteration, and */
/*           3) At most ITMAX iterations tried. */

	if (berr[j] > eps && berr[j] * 2. <= lstres && count <= 5) {

/*           Update solution and try again. */

	    dgetrs_(trans, n, &c__1, &af[af_offset], ldaf, &ipiv[1], &work[*n
		    + 1], n, info);
	    daxpy_(n, &c_b17, &work[*n + 1], &c__1, &x[j * x_dim1 + 1], &c__1)
		    ;
	    lstres = berr[j];
	    ++count;
	    goto L20;
	}

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
/* L90: */
	}

	kase = 0;
L100:
	dlacn2_(n, &work[(*n << 1) + 1], &work[*n + 1], &iwork[1], &ferr[j], &
		kase, isave);
	if (kase != 0) {
	    if (kase == 1) {

/*              Multiply by diag(W)*inv(op(A)**T). */

		dgetrs_(transt, n, &c__1, &af[af_offset], ldaf, &ipiv[1], &
			work[*n + 1], n, info);
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = work[i__] * work[*n + i__];
/* L110: */
		}
	    } else {

/*              Multiply by inv(op(A))*diag(W). */

		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = work[i__] * work[*n + i__];
/* L120: */
		}
		dgetrs_(trans, n, &c__1, &af[af_offset], ldaf, &ipiv[1], &
			work[*n + 1], n, info);
	    }
	    goto L100;
	}

/*        Normalize error. */

	lstres = 0.;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = lstres, d__3 = (d__1 = x[i__ + j * x_dim1], abs(d__1));
	    lstres = std::max(d__2,d__3);
/* L130: */
	}
	if (lstres != 0.) {
	    ferr[j] /= lstres;
	}

/* L140: */
    }

    return 0;

/*     End of DGERFS */

} /* dgerfs_ */

/* Subroutine */ int dgerq2_(integer *m, integer *n, double *a, integer *lda, double *tau, double *work, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    integer i__, k;
    double aii;

/*  -- LAPACK routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGERQ2 computes an RQ factorization of a real m by n matrix A: */
/*  A = R * Q. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the m by n matrix A. */
/*          On exit, if m <= n, the upper triangle of the subarray */
/*          A(1:m,n-m+1:n) contains the m by m upper triangular matrix R; */
/*          if m >= n, the elements on and above the (m-n)-th subdiagonal */
/*          contain the m by n upper trapezoidal matrix R; the remaining */
/*          elements, with the array TAU, represent the orthogonal matrix */
/*          Q as a product of elementary reflectors (see Further */
/*          Details). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors (see Further */
/*          Details). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (M) */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of elementary reflectors */

/*     Q = H(1) H(2) . . . H(k), where k = min(m,n). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(n-k+i+1:n) = 0 and v(n-k+i) = 1; v(1:n-k+i-1) is stored on exit in */
/*  A(m-k+i,1:n-k+i-1), and tau in TAU(i). */

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

/*     Test the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGERQ2", &i__1);
	return 0;
    }

    k = std::min(*m,*n);

    for (i__ = k; i__ >= 1; --i__) {

/*        Generate elementary reflector H(i) to annihilate */
/*        A(m-k+i,1:n-k+i-1) */

	i__1 = *n - k + i__;
	dlarfp_(&i__1, &a[*m - k + i__ + (*n - k + i__) * a_dim1], &a[*m - k
		+ i__ + a_dim1], lda, &tau[i__]);

/*        Apply H(i) to A(1:m-k+i-1,1:n-k+i) from the right */

	aii = a[*m - k + i__ + (*n - k + i__) * a_dim1];
	a[*m - k + i__ + (*n - k + i__) * a_dim1] = 1.;
	i__1 = *m - k + i__ - 1;
	i__2 = *n - k + i__;
	dlarf_("Right", &i__1, &i__2, &a[*m - k + i__ + a_dim1], lda, &tau[
		i__], &a[a_offset], lda, &work[1]);
	a[*m - k + i__ + (*n - k + i__) * a_dim1] = aii;
/* L10: */
    }
    return 0;

/*     End of DGERQ2 */

} /* dgerq2_ */

/* Subroutine */ int dgerqf_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__3 = 3;
	static integer c__2 = 2;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    integer i__, k, ib, nb, ki, kk, mu, nu, nx, iws, nbmin, iinfo;
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

/*  DGERQF computes an RQ factorization of a real M-by-N matrix A: */
/*  A = R * Q. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, */
/*          if m <= n, the upper triangle of the subarray */
/*          A(1:m,n-m+1:n) contains the M-by-M upper triangular matrix R; */
/*          if m >= n, the elements on and above the (m-n)-th subdiagonal */
/*          contain the M-by-N upper trapezoidal matrix R; */
/*          the remaining elements, with the array TAU, represent the */
/*          orthogonal matrix Q as a product of min(m,n) elementary */
/*          reflectors (see Further Details). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors (see Further */
/*          Details). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= max(1,M). */
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

/*  The matrix Q is represented as a product of elementary reflectors */

/*     Q = H(1) H(2) . . . H(k), where k = min(m,n). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(n-k+i+1:n) = 0 and v(n-k+i) = 1; v(1:n-k+i-1) is stored on exit in */
/*  A(m-k+i,1:n-k+i-1), and tau in TAU(i). */

/*  ===================================================================== */

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
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    }

    if (*info == 0) {
	k = std::min(*m,*n);
	if (k == 0) {
	    lwkopt = 1;
	} else {
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
	xerbla_("DGERQF", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (k == 0) {
	return 0;
    }

    nbmin = 2;
    nx = 1;
    iws = *m;
    if (nb > 1 && nb < k) {

/*        Determine when to cross over from blocked to unblocked code. */

/* Computing MAX */
	i__1 = 0, i__2 = ilaenv_(&c__3, "DGERQF", " ", m, n, &c_n1, &c_n1);
	nx = std::max(i__1,i__2);
	if (nx < k) {

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

    if (nb >= nbmin && nb < k && nx < k) {

/*        Use blocked code initially. */
/*        The last kk rows are handled by the block method. */

	ki = (k - nx - 1) / nb * nb;
/* Computing MIN */
	i__1 = k, i__2 = ki + nb;
	kk = std::min(i__1,i__2);

	i__1 = k - kk + 1;
	i__2 = -nb;
	for (i__ = k - kk + ki + 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__
		+= i__2) {
/* Computing MIN */
	    i__3 = k - i__ + 1;
	    ib = std::min(i__3,nb);

/*           Compute the RQ factorization of the current block */
/*           A(m-k+i:m-k+i+ib-1,1:n-k+i+ib-1) */

	    i__3 = *n - k + i__ + ib - 1;
	    dgerq2_(&ib, &i__3, &a[*m - k + i__ + a_dim1], lda, &tau[i__], &
		    work[1], &iinfo);
	    if (*m - k + i__ > 1) {

/*              Form the triangular factor of the block reflector */
/*              H = H(i+ib-1) . . . H(i+1) H(i) */

		i__3 = *n - k + i__ + ib - 1;
		dlarft_("Backward", "Rowwise", &i__3, &ib, &a[*m - k + i__ +
			a_dim1], lda, &tau[i__], &work[1], &ldwork);

/*              Apply H to A(1:m-k+i-1,1:n-k+i+ib-1) from the right */

		i__3 = *m - k + i__ - 1;
		i__4 = *n - k + i__ + ib - 1;
		dlarfb_("Right", "No transpose", "Backward", "Rowwise", &i__3,
			 &i__4, &ib, &a[*m - k + i__ + a_dim1], lda, &work[1],
			 &ldwork, &a[a_offset], lda, &work[ib + 1], &ldwork);
	    }
/* L10: */
	}
	mu = *m - k + i__ + nb - 1;
	nu = *n - k + i__ + nb - 1;
    } else {
	mu = *m;
	nu = *n;
    }

/*     Use unblocked code to factor the last or only block */

    if (mu > 0 && nu > 0) {
	dgerq2_(&mu, &nu, &a[a_offset], lda, &tau[1], &work[1], &iinfo);
    }

    work[1] = (double) iws;
    return 0;

/*     End of DGERQF */

} /* dgerqf_ */

/* Subroutine */ int dgesc2_(integer *n, double *a, integer *lda,
	double *rhs, integer *ipiv, integer *jpiv, double *scale)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j;
    double eps, temp;
    double bignum;
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

/*  DGESC2 solves a system of linear equations */

/*            A * X = scale* RHS */

/*  with a general N-by-N matrix A using the LU factorization with */
/*  complete pivoting computed by DGETC2. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the  LU part of the factorization of the n-by-n */
/*          matrix A computed by DGETC2:  A = P * L * U * Q */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1, N). */

/*  RHS     (input/output) DOUBLE PRECISION array, dimension (N). */
/*          On entry, the right hand side vector b. */
/*          On exit, the solution vector X. */

/*  IPIV    (input) INTEGER array, dimension (N). */
/*          The pivot indices; for 1 <= i <= N, row i of the */
/*          matrix has been interchanged with row IPIV(i). */

/*  JPIV    (input) INTEGER array, dimension (N). */
/*          The pivot indices; for 1 <= j <= N, column j of the */
/*          matrix has been interchanged with column JPIV(j). */

/*  SCALE    (output) DOUBLE PRECISION */
/*           On exit, SCALE contains the scale factor. SCALE is chosen */
/*           0 <= SCALE <= 1 to prevent owerflow in the solution. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Bo Kagstrom and Peter Poromaa, Department of Computing Science, */
/*     Umea University, S-901 87 Umea, Sweden. */

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

/*      Set constant to control owerflow */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --rhs;
    --ipiv;
    --jpiv;

    /* Function Body */
    eps = dlamch_("P");
    smlnum = dlamch_("S") / eps;
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);

/*     Apply permutations IPIV to RHS */

    i__1 = *n - 1;
    dlaswp_(&c__1, &rhs[1], lda, &c__1, &i__1, &ipiv[1], &c__1);

/*     Solve for L part */

    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *n;
	for (j = i__ + 1; j <= i__2; ++j) {
	    rhs[j] -= a[j + i__ * a_dim1] * rhs[i__];
/* L10: */
	}
/* L20: */
    }

/*     Solve for U part */

    *scale = 1.;

/*     Check for scaling */

    i__ = idamax_(n, &rhs[1], &c__1);
    if (smlnum * 2. * (d__1 = rhs[i__], abs(d__1)) > (d__2 = a[*n + *n *
	    a_dim1], abs(d__2))) {
	temp = .5 / (d__1 = rhs[i__], abs(d__1));
	dscal_(n, &temp, &rhs[1], &c__1);
	*scale *= temp;
    }

    for (i__ = *n; i__ >= 1; --i__) {
	temp = 1. / a[i__ + i__ * a_dim1];
	rhs[i__] *= temp;
	i__1 = *n;
	for (j = i__ + 1; j <= i__1; ++j) {
	    rhs[i__] -= rhs[j] * (a[i__ + j * a_dim1] * temp);
/* L30: */
	}
/* L40: */
    }

/*     Apply permutations JPIV to the solution (RHS) */

    i__1 = *n - 1;
    dlaswp_(&c__1, &rhs[1], lda, &c__1, &i__1, &jpiv[1], &c_n1);
    return 0;

/*     End of DGESC2 */

} /* dgesc2_ */

/* Subroutine */ int dgesdd_(const char *jobz, integer *m, integer *n, double *
	a, integer *lda, double *s, double *u, integer *ldu,
	double *vt, integer *ldvt, double *work, integer *lwork,
	integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__0 = 0;
	static double c_b227 = 0.;
	static double c_b248 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, u_dim1, u_offset, vt_dim1, vt_offset, i__1,
	    i__2, i__3;

    /* Local variables */
    integer i__, ie, il, ir, iu, blk;
    double dum[1], eps;
    integer ivt, iscl;
    double anrm;
    integer idum[1], ierr, itau;
    integer chunk, minmn, wrkbl, itaup, itauq, mnthr;
    bool wntqa;
    integer nwork;
    bool wntqn, wntqo, wntqs;
    integer bdspac;
    double bignum;
    integer ldwrkl, ldwrkr, minwrk, ldwrku, maxwrk, ldwkvt;
    double smlnum;
    bool wntqas, lquery;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGESDD computes the singular value decomposition (SVD) of a real */
/*  M-by-N matrix A, optionally computing the left and right singular */
/*  vectors.  If singular vectors are desired, it uses a */
/*  divide-and-conquer algorithm. */

/*  The SVD is written */

/*       A = U * SIGMA * transpose(V) */

/*  where SIGMA is an M-by-N matrix which is zero except for its */
/*  min(m,n) diagonal elements, U is an M-by-M orthogonal matrix, and */
/*  V is an N-by-N orthogonal matrix.  The diagonal elements of SIGMA */
/*  are the singular values of A; they are real and non-negative, and */
/*  are returned in descending order.  The first min(m,n) columns of */
/*  U and V are the left and right singular vectors of A. */

/*  Note that the routine returns VT = V**T, not V. */

/*  The divide and conquer algorithm makes very mild assumptions about */
/*  floating point arithmetic. It will work on machines with a guard */
/*  digit in add/subtract, or on those binary machines without guard */
/*  digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or */
/*  Cray-2. It could conceivably fail on hexadecimal or decimal machines */
/*  without guard digits, but we know of none. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          Specifies options for computing all or part of the matrix U: */
/*          = 'A':  all M columns of U and all N rows of V**T are */
/*                  returned in the arrays U and VT; */
/*          = 'S':  the first min(M,N) columns of U and the first */
/*                  min(M,N) rows of V**T are returned in the arrays U */
/*                  and VT; */
/*          = 'O':  If M >= N, the first N columns of U are overwritten */
/*                  on the array A and all rows of V**T are returned in */
/*                  the array VT; */
/*                  otherwise, all columns of U are returned in the */
/*                  array U and the first M rows of V**T are overwritten */
/*                  in the array A; */
/*          = 'N':  no columns of U or rows of V**T are computed. */

/*  M       (input) INTEGER */
/*          The number of rows of the input matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the input matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, */
/*          if JOBZ = 'O',  A is overwritten with the first N columns */
/*                          of U (the left singular vectors, stored */
/*                          columnwise) if M >= N; */
/*                          A is overwritten with the first M rows */
/*                          of V**T (the right singular vectors, stored */
/*                          rowwise) otherwise. */
/*          if JOBZ .ne. 'O', the contents of A are destroyed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  S       (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The singular values of A, sorted so that S(i) >= S(i+1). */

/*  U       (output) DOUBLE PRECISION array, dimension (LDU,UCOL) */
/*          UCOL = M if JOBZ = 'A' or JOBZ = 'O' and M < N; */
/*          UCOL = min(M,N) if JOBZ = 'S'. */
/*          If JOBZ = 'A' or JOBZ = 'O' and M < N, U contains the M-by-M */
/*          orthogonal matrix U; */
/*          if JOBZ = 'S', U contains the first min(M,N) columns of U */
/*          (the left singular vectors, stored columnwise); */
/*          if JOBZ = 'O' and M >= N, or JOBZ = 'N', U is not referenced. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U.  LDU >= 1; if */
/*          JOBZ = 'S' or 'A' or JOBZ = 'O' and M < N, LDU >= M. */

/*  VT      (output) DOUBLE PRECISION array, dimension (LDVT,N) */
/*          If JOBZ = 'A' or JOBZ = 'O' and M >= N, VT contains the */
/*          N-by-N orthogonal matrix V**T; */
/*          if JOBZ = 'S', VT contains the first min(M,N) rows of */
/*          V**T (the right singular vectors, stored rowwise); */
/*          if JOBZ = 'O' and M < N, or JOBZ = 'N', VT is not referenced. */

/*  LDVT    (input) INTEGER */
/*          The leading dimension of the array VT.  LDVT >= 1; if */
/*          JOBZ = 'A' or JOBZ = 'O' and M >= N, LDVT >= N; */
/*          if JOBZ = 'S', LDVT >= min(M,N). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK; */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. LWORK >= 1. */
/*          If JOBZ = 'N', */
/*            LWORK >= 3*min(M,N) + max(max(M,N),7*min(M,N)). */
/*          If JOBZ = 'O', */
/*            LWORK >= 3*min(M,N)*min(M,N) + */
/*                     max(max(M,N),5*min(M,N)*min(M,N)+4*min(M,N)). */
/*          If JOBZ = 'S' or 'A' */
/*            LWORK >= 3*min(M,N)*min(M,N) + */
/*                     max(max(M,N),4*min(M,N)*min(M,N)+4*min(M,N)). */
/*          For good performance, LWORK should generally be larger. */
/*          If LWORK = -1 but other input arguments are legal, WORK(1) */
/*          returns the optimal LWORK. */

/*  IWORK   (workspace) INTEGER array, dimension (8*min(M,N)) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  DBDSDC did not converge, updating process failed. */

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
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --s;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1;
    vt -= vt_offset;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    minmn = std::min(*m,*n);
    wntqa = lsame_(jobz, "A");
    wntqs = lsame_(jobz, "S");
    wntqas = wntqa || wntqs;
    wntqo = lsame_(jobz, "O");
    wntqn = lsame_(jobz, "N");
    lquery = *lwork == -1;

    if (! (wntqa || wntqs || wntqo || wntqn)) {
	*info = -1;
    } else if (*m < 0) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -5;
    } else if (*ldu < 1 || wntqas && *ldu < *m || wntqo && *m < *n && *ldu < *
	    m) {
	*info = -8;
    } else if (*ldvt < 1 || wntqa && *ldvt < *n || wntqs && *ldvt < minmn ||
	    wntqo && *m >= *n && *ldvt < *n) {
	*info = -10;
    }

/*     Compute workspace */
/*      (Note: Comments in the code beginning "Workspace:" describe the */
/*       minimal amount of workspace needed at that point in the code, */
/*       as well as the preferred amount for good performance. */
/*       NB refers to the optimal block size for the immediately */
/*       following subroutine, as returned by ILAENV.) */

    if (*info == 0) {
	minwrk = 1;
	maxwrk = 1;
	if (*m >= *n && minmn > 0) {

/*           Compute space needed for DBDSDC */

	    mnthr = (integer) (minmn * 11. / 6.);
	    if (wntqn) {
		bdspac = *n * 7;
	    } else {
		bdspac = *n * 3 * *n + (*n << 2);
	    }
	    if (*m >= mnthr) {
		if (wntqn) {

/*                 Path 1 (M much larger than N, JOBZ='N') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + (*n << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", n, n, &c_n1, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *n;
		    maxwrk = std::max(i__1,i__2);
		    minwrk = bdspac + *n;
		} else if (wntqo) {

/*                 Path 2 (M much larger than N, JOBZ='O') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n + *n * ilaenv_(&c__1, "DORGQR",
			    " ", m, n, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + (*n << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", n, n, &c_n1, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
, "QLN", n, n, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
, "PRT", n, n, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *n * 3;
		    wrkbl = std::max(i__1,i__2);
		    maxwrk = wrkbl + (*n << 1) * *n;
		    minwrk = bdspac + (*n << 1) * *n + *n * 3;
		} else if (wntqs) {

/*                 Path 3 (M much larger than N, JOBZ='S') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n + *n * ilaenv_(&c__1, "DORGQR",
			    " ", m, n, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + (*n << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", n, n, &c_n1, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
, "QLN", n, n, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
, "PRT", n, n, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *n * 3;
		    wrkbl = std::max(i__1,i__2);
		    maxwrk = wrkbl + *n * *n;
		    minwrk = bdspac + *n * *n + *n * 3;
		} else if (wntqa) {

/*                 Path 4 (M much larger than N, JOBZ='A') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n + *m * ilaenv_(&c__1, "DORGQR",
			    " ", m, m, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + (*n << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", n, n, &c_n1, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
, "QLN", n, n, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
, "PRT", n, n, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *n * 3;
		    wrkbl = std::max(i__1,i__2);
		    maxwrk = wrkbl + *n * *n;
		    minwrk = bdspac + *n * *n + *n * 3;
		}
	    } else {

/*              Path 5 (M at least N, but not much larger) */

		wrkbl = *n * 3 + (*m + *n) * ilaenv_(&c__1, "DGEBRD", " ", m,
			n, &c_n1, &c_n1);
		if (wntqn) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *n * 3;
		    maxwrk = std::max(i__1,i__2);
		    minwrk = *n * 3 + std::max(*m,bdspac);
		} else if (wntqo) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
, "QLN", m, n, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
, "PRT", n, n, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *n * 3;
		    wrkbl = std::max(i__1,i__2);
		    maxwrk = wrkbl + *m * *n;
/* Computing MAX */
		    i__1 = *m, i__2 = *n * *n + bdspac;
		    minwrk = *n * 3 + std::max(i__1,i__2);
		} else if (wntqs) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
, "QLN", m, n, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
, "PRT", n, n, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *n * 3;
		    maxwrk = std::max(i__1,i__2);
		    minwrk = *n * 3 + std::max(*m,bdspac);
		} else if (wntqa) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *m * ilaenv_(&c__1, "DORMBR"
, "QLN", m, m, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *n * 3 + *n * ilaenv_(&c__1, "DORMBR"
, "PRT", n, n, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = maxwrk, i__2 = bdspac + *n * 3;
		    maxwrk = std::max(i__1,i__2);
		    minwrk = *n * 3 + std::max(*m,bdspac);
		}
	    }
	} else if (minmn > 0) {

/*           Compute space needed for DBDSDC */

	    mnthr = (integer) (minmn * 11. / 6.);
	    if (wntqn) {
		bdspac = *m * 7;
	    } else {
		bdspac = *m * 3 * *m + (*m << 2);
	    }
	    if (*n >= mnthr) {
		if (wntqn) {

/*                 Path 1t (N much larger than M, JOBZ='N') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + (*m << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", m, m, &c_n1, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m;
		    maxwrk = std::max(i__1,i__2);
		    minwrk = bdspac + *m;
		} else if (wntqo) {

/*                 Path 2t (N much larger than M, JOBZ='O') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m + *m * ilaenv_(&c__1, "DORGLQ",
			    " ", m, n, m, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + (*m << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", m, m, &c_n1, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
, "QLN", m, m, m, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
, "PRT", m, m, m, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m * 3;
		    wrkbl = std::max(i__1,i__2);
		    maxwrk = wrkbl + (*m << 1) * *m;
		    minwrk = bdspac + (*m << 1) * *m + *m * 3;
		} else if (wntqs) {

/*                 Path 3t (N much larger than M, JOBZ='S') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m + *m * ilaenv_(&c__1, "DORGLQ",
			    " ", m, n, m, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + (*m << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", m, m, &c_n1, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
, "QLN", m, m, m, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
, "PRT", m, m, m, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m * 3;
		    wrkbl = std::max(i__1,i__2);
		    maxwrk = wrkbl + *m * *m;
		    minwrk = bdspac + *m * *m + *m * 3;
		} else if (wntqa) {

/*                 Path 4t (N much larger than M, JOBZ='A') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m + *n * ilaenv_(&c__1, "DORGLQ",
			    " ", n, n, m, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + (*m << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", m, m, &c_n1, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
, "QLN", m, m, m, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
, "PRT", m, m, m, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m * 3;
		    wrkbl = std::max(i__1,i__2);
		    maxwrk = wrkbl + *m * *m;
		    minwrk = bdspac + *m * *m + *m * 3;
		}
	    } else {

/*              Path 5t (N greater than M, but not much larger) */

		wrkbl = *m * 3 + (*m + *n) * ilaenv_(&c__1, "DGEBRD", " ", m,
			n, &c_n1, &c_n1);
		if (wntqn) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m * 3;
		    maxwrk = std::max(i__1,i__2);
		    minwrk = *m * 3 + std::max(*n,bdspac);
		} else if (wntqo) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
, "QLN", m, m, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
, "PRT", m, n, m, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m * 3;
		    wrkbl = std::max(i__1,i__2);
		    maxwrk = wrkbl + *m * *n;
/* Computing MAX */
		    i__1 = *n, i__2 = *m * *m + bdspac;
		    minwrk = *m * 3 + std::max(i__1,i__2);
		} else if (wntqs) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
, "QLN", m, m, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
, "PRT", m, n, m, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m * 3;
		    maxwrk = std::max(i__1,i__2);
		    minwrk = *m * 3 + std::max(*n,bdspac);
		} else if (wntqa) {
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
, "QLN", m, m, n, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = *m * 3 + *m * ilaenv_(&c__1, "DORMBR"
, "PRT", n, n, m, &c_n1);
		    wrkbl = std::max(i__1,i__2);
/* Computing MAX */
		    i__1 = wrkbl, i__2 = bdspac + *m * 3;
		    maxwrk = std::max(i__1,i__2);
		    minwrk = *m * 3 + std::max(*n,bdspac);
		}
	    }
	}
	maxwrk = std::max(maxwrk,minwrk);
	work[1] = (double) maxwrk;

	if (*lwork < minwrk && ! lquery) {
	    *info = -12;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGESDD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	return 0;
    }

/*     Get machine constants */

    eps = dlamch_("P");
    smlnum = sqrt(dlamch_("S")) / eps;
    bignum = 1. / smlnum;

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", m, n, &a[a_offset], lda, dum);
    iscl = 0;
    if (anrm > 0. && anrm < smlnum) {
	iscl = 1;
	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, m, n, &a[a_offset], lda, &
		ierr);
    } else if (anrm > bignum) {
	iscl = 1;
	dlascl_("G", &c__0, &c__0, &anrm, &bignum, m, n, &a[a_offset], lda, &
		ierr);
    }

    if (*m >= *n) {

/*        A has at least as many rows as columns. If A has sufficiently */
/*        more rows than columns, first reduce using the QR */
/*        decomposition (if sufficient workspace available) */

	if (*m >= mnthr) {

	    if (wntqn) {

/*              Path 1 (M much larger than N, JOBZ='N') */
/*              No singular vectors to be computed */

		itau = 1;
		nwork = itau + *n;

/*              Compute A=Q*R */
/*              (Workspace: need 2*N, prefer N+N*NB) */

		i__1 = *lwork - nwork + 1;
		dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__1, &ierr);

/*              Zero out below R */

		i__1 = *n - 1;
		i__2 = *n - 1;
		dlaset_("L", &i__1, &i__2, &c_b227, &c_b227, &a[a_dim1 + 2],
			lda);
		ie = 1;
		itauq = ie + *n;
		itaup = itauq + *n;
		nwork = itaup + *n;

/*              Bidiagonalize R in A */
/*              (Workspace: need 4*N, prefer 3*N+2*N*NB) */

		i__1 = *lwork - nwork + 1;
		dgebrd_(n, n, &a[a_offset], lda, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__1, &ierr);
		nwork = ie + *n;

/*              Perform bidiagonal SVD, computing singular values only */
/*              (Workspace: need N+BDSPAC) */

		dbdsdc_("U", "N", n, &s[1], &work[ie], dum, &c__1, dum, &c__1,
			 dum, idum, &work[nwork], &iwork[1], info);

	    } else if (wntqo) {

/*              Path 2 (M much larger than N, JOBZ = 'O') */
/*              N left singular vectors to be overwritten on A and */
/*              N right singular vectors to be computed in VT */

		ir = 1;

/*              WORK(IR) is LDWRKR by N */

		if (*lwork >= *lda * *n + *n * *n + *n * 3 + bdspac) {
		    ldwrkr = *lda;
		} else {
		    ldwrkr = (*lwork - *n * *n - *n * 3 - bdspac) / *n;
		}
		itau = ir + ldwrkr * *n;
		nwork = itau + *n;

/*              Compute A=Q*R */
/*              (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		i__1 = *lwork - nwork + 1;
		dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__1, &ierr);

/*              Copy R to WORK(IR), zeroing out below it */

		dlacpy_("U", n, n, &a[a_offset], lda, &work[ir], &ldwrkr);
		i__1 = *n - 1;
		i__2 = *n - 1;
		dlaset_("L", &i__1, &i__2, &c_b227, &c_b227, &work[ir + 1], &
			ldwrkr);

/*              Generate Q in A */
/*              (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		i__1 = *lwork - nwork + 1;
		dorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &work[nwork],
			 &i__1, &ierr);
		ie = itau;
		itauq = ie + *n;
		itaup = itauq + *n;
		nwork = itaup + *n;

/*              Bidiagonalize R in VT, copying result to WORK(IR) */
/*              (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

		i__1 = *lwork - nwork + 1;
		dgebrd_(n, n, &work[ir], &ldwrkr, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__1, &ierr);

/*              WORK(IU) is N by N */

		iu = nwork;
		nwork = iu + *n * *n;

/*              Perform bidiagonal SVD, computing left singular vectors */
/*              of bidiagonal matrix in WORK(IU) and computing right */
/*              singular vectors of bidiagonal matrix in VT */
/*              (Workspace: need N+N*N+BDSPAC) */

		dbdsdc_("U", "I", n, &s[1], &work[ie], &work[iu], n, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*              Overwrite WORK(IU) by left singular vectors of R */
/*              and VT by right singular vectors of R */
/*              (Workspace: need 2*N*N+3*N, prefer 2*N*N+2*N+N*NB) */

		i__1 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", n, n, n, &work[ir], &ldwrkr, &work[
			itauq], &work[iu], n, &work[nwork], &i__1, &ierr);
		i__1 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", n, n, n, &work[ir], &ldwrkr, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__1, &
			ierr);

/*              Multiply Q in A by left singular vectors of R in */
/*              WORK(IU), storing result in WORK(IR) and copying to A */
/*              (Workspace: need 2*N*N, prefer N*N+M*N) */

		i__1 = *m;
		i__2 = ldwrkr;
		for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ +=
			i__2) {
/* Computing MIN */
		    i__3 = *m - i__ + 1;
		    chunk = std::min(i__3,ldwrkr);
		    dgemm_("N", "N", &chunk, n, n, &c_b248, &a[i__ + a_dim1],
			    lda, &work[iu], n, &c_b227, &work[ir], &ldwrkr);
		    dlacpy_("F", &chunk, n, &work[ir], &ldwrkr, &a[i__ +
			    a_dim1], lda);
/* L10: */
		}

	    } else if (wntqs) {

/*              Path 3 (M much larger than N, JOBZ='S') */
/*              N left singular vectors to be computed in U and */
/*              N right singular vectors to be computed in VT */

		ir = 1;

/*              WORK(IR) is N by N */

		ldwrkr = *n;
		itau = ir + ldwrkr * *n;
		nwork = itau + *n;

/*              Compute A=Q*R */
/*              (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		i__2 = *lwork - nwork + 1;
		dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__2, &ierr);

/*              Copy R to WORK(IR), zeroing out below it */

		dlacpy_("U", n, n, &a[a_offset], lda, &work[ir], &ldwrkr);
		i__2 = *n - 1;
		i__1 = *n - 1;
		dlaset_("L", &i__2, &i__1, &c_b227, &c_b227, &work[ir + 1], &
			ldwrkr);

/*              Generate Q in A */
/*              (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		i__2 = *lwork - nwork + 1;
		dorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &work[nwork],
			 &i__2, &ierr);
		ie = itau;
		itauq = ie + *n;
		itaup = itauq + *n;
		nwork = itaup + *n;

/*              Bidiagonalize R in WORK(IR) */
/*              (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

		i__2 = *lwork - nwork + 1;
		dgebrd_(n, n, &work[ir], &ldwrkr, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__2, &ierr);

/*              Perform bidiagonal SVD, computing left singular vectors */
/*              of bidiagoal matrix in U and computing right singular */
/*              vectors of bidiagonal matrix in VT */
/*              (Workspace: need N+BDSPAC) */

		dbdsdc_("U", "I", n, &s[1], &work[ie], &u[u_offset], ldu, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*              Overwrite U by left singular vectors of R and VT */
/*              by right singular vectors of R */
/*              (Workspace: need N*N+3*N, prefer N*N+2*N+N*NB) */

		i__2 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", n, n, n, &work[ir], &ldwrkr, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__2, &ierr);

		i__2 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", n, n, n, &work[ir], &ldwrkr, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__2, &
			ierr);

/*              Multiply Q in A by left singular vectors of R in */
/*              WORK(IR), storing result in U */
/*              (Workspace: need N*N) */

		dlacpy_("F", n, n, &u[u_offset], ldu, &work[ir], &ldwrkr);
		dgemm_("N", "N", m, n, n, &c_b248, &a[a_offset], lda, &work[
			ir], &ldwrkr, &c_b227, &u[u_offset], ldu);

	    } else if (wntqa) {

/*              Path 4 (M much larger than N, JOBZ='A') */
/*              M left singular vectors to be computed in U and */
/*              N right singular vectors to be computed in VT */

		iu = 1;

/*              WORK(IU) is N by N */

		ldwrku = *n;
		itau = iu + ldwrku * *n;
		nwork = itau + *n;

/*              Compute A=Q*R, copying result to U */
/*              (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		i__2 = *lwork - nwork + 1;
		dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__2, &ierr);
		dlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], ldu);

/*              Generate Q in U */
/*              (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */
		i__2 = *lwork - nwork + 1;
		dorgqr_(m, m, n, &u[u_offset], ldu, &work[itau], &work[nwork],
			 &i__2, &ierr);

/*              Produce R in A, zeroing out other entries */

		i__2 = *n - 1;
		i__1 = *n - 1;
		dlaset_("L", &i__2, &i__1, &c_b227, &c_b227, &a[a_dim1 + 2],
			lda);
		ie = itau;
		itauq = ie + *n;
		itaup = itauq + *n;
		nwork = itaup + *n;

/*              Bidiagonalize R in A */
/*              (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

		i__2 = *lwork - nwork + 1;
		dgebrd_(n, n, &a[a_offset], lda, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__2, &ierr);

/*              Perform bidiagonal SVD, computing left singular vectors */
/*              of bidiagonal matrix in WORK(IU) and computing right */
/*              singular vectors of bidiagonal matrix in VT */
/*              (Workspace: need N+N*N+BDSPAC) */

		dbdsdc_("U", "I", n, &s[1], &work[ie], &work[iu], n, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*              Overwrite WORK(IU) by left singular vectors of R and VT */
/*              by right singular vectors of R */
/*              (Workspace: need N*N+3*N, prefer N*N+2*N+N*NB) */

		i__2 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", n, n, n, &a[a_offset], lda, &work[
			itauq], &work[iu], &ldwrku, &work[nwork], &i__2, &
			ierr);
		i__2 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", n, n, n, &a[a_offset], lda, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__2, &
			ierr);

/*              Multiply Q in U by left singular vectors of R in */
/*              WORK(IU), storing result in A */
/*              (Workspace: need N*N) */

		dgemm_("N", "N", m, n, n, &c_b248, &u[u_offset], ldu, &work[
			iu], &ldwrku, &c_b227, &a[a_offset], lda);

/*              Copy left singular vectors of A from A to U */

		dlacpy_("F", m, n, &a[a_offset], lda, &u[u_offset], ldu);

	    }

	} else {

/*           M .LT. MNTHR */

/*           Path 5 (M at least N, but not much larger) */
/*           Reduce to bidiagonal form without QR decomposition */

	    ie = 1;
	    itauq = ie + *n;
	    itaup = itauq + *n;
	    nwork = itaup + *n;

/*           Bidiagonalize A */
/*           (Workspace: need 3*N+M, prefer 3*N+(M+N)*NB) */

	    i__2 = *lwork - nwork + 1;
	    dgebrd_(m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &
		    work[itaup], &work[nwork], &i__2, &ierr);
	    if (wntqn) {

/*              Perform bidiagonal SVD, only computing singular values */
/*              (Workspace: need N+BDSPAC) */

		dbdsdc_("U", "N", n, &s[1], &work[ie], dum, &c__1, dum, &c__1,
			 dum, idum, &work[nwork], &iwork[1], info);
	    } else if (wntqo) {
		iu = nwork;
		if (*lwork >= *m * *n + *n * 3 + bdspac) {

/*                 WORK( IU ) is M by N */

		    ldwrku = *m;
		    nwork = iu + ldwrku * *n;
		    dlaset_("F", m, n, &c_b227, &c_b227, &work[iu], &ldwrku);
		} else {

/*                 WORK( IU ) is N by N */

		    ldwrku = *n;
		    nwork = iu + ldwrku * *n;

/*                 WORK(IR) is LDWRKR by N */

		    ir = nwork;
		    ldwrkr = (*lwork - *n * *n - *n * 3) / *n;
		}
		nwork = iu + ldwrku * *n;

/*              Perform bidiagonal SVD, computing left singular vectors */
/*              of bidiagonal matrix in WORK(IU) and computing right */
/*              singular vectors of bidiagonal matrix in VT */
/*              (Workspace: need N+N*N+BDSPAC) */

		dbdsdc_("U", "I", n, &s[1], &work[ie], &work[iu], &ldwrku, &
			vt[vt_offset], ldvt, dum, idum, &work[nwork], &iwork[
			1], info);

/*              Overwrite VT by right singular vectors of A */
/*              (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		i__2 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", n, n, n, &a[a_offset], lda, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__2, &
			ierr);

		if (*lwork >= *m * *n + *n * 3 + bdspac) {

/*                 Overwrite WORK(IU) by left singular vectors of A */
/*                 (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		    i__2 = *lwork - nwork + 1;
		    dormbr_("Q", "L", "N", m, n, n, &a[a_offset], lda, &work[
			    itauq], &work[iu], &ldwrku, &work[nwork], &i__2, &
			    ierr);

/*                 Copy left singular vectors of A from WORK(IU) to A */

		    dlacpy_("F", m, n, &work[iu], &ldwrku, &a[a_offset], lda);
		} else {

/*                 Generate Q in A */
/*                 (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		    i__2 = *lwork - nwork + 1;
		    dorgbr_("Q", m, n, n, &a[a_offset], lda, &work[itauq], &
			    work[nwork], &i__2, &ierr);

/*                 Multiply Q in A by left singular vectors of */
/*                 bidiagonal matrix in WORK(IU), storing result in */
/*                 WORK(IR) and copying to A */
/*                 (Workspace: need 2*N*N, prefer N*N+M*N) */

		    i__2 = *m;
		    i__1 = ldwrkr;
		    for (i__ = 1; i__1 < 0 ? i__ >= i__2 : i__ <= i__2; i__ +=
			     i__1) {
/* Computing MIN */
			i__3 = *m - i__ + 1;
			chunk = std::min(i__3,ldwrkr);
			dgemm_("N", "N", &chunk, n, n, &c_b248, &a[i__ +
				a_dim1], lda, &work[iu], &ldwrku, &c_b227, &
				work[ir], &ldwrkr);
			dlacpy_("F", &chunk, n, &work[ir], &ldwrkr, &a[i__ +
				a_dim1], lda);
/* L20: */
		    }
		}

	    } else if (wntqs) {

/*              Perform bidiagonal SVD, computing left singular vectors */
/*              of bidiagonal matrix in U and computing right singular */
/*              vectors of bidiagonal matrix in VT */
/*              (Workspace: need N+BDSPAC) */

		dlaset_("F", m, n, &c_b227, &c_b227, &u[u_offset], ldu);
		dbdsdc_("U", "I", n, &s[1], &work[ie], &u[u_offset], ldu, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*              Overwrite U by left singular vectors of A and VT */
/*              by right singular vectors of A */
/*              (Workspace: need 3*N, prefer 2*N+N*NB) */

		i__1 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, n, n, &a[a_offset], lda, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__1, &ierr);
		i__1 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", n, n, n, &a[a_offset], lda, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__1, &
			ierr);
	    } else if (wntqa) {

/*              Perform bidiagonal SVD, computing left singular vectors */
/*              of bidiagonal matrix in U and computing right singular */
/*              vectors of bidiagonal matrix in VT */
/*              (Workspace: need N+BDSPAC) */

		dlaset_("F", m, m, &c_b227, &c_b227, &u[u_offset], ldu);
		dbdsdc_("U", "I", n, &s[1], &work[ie], &u[u_offset], ldu, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*              Set the right corner of U to identity matrix */

		if (*m > *n) {
		    i__1 = *m - *n;
		    i__2 = *m - *n;
		    dlaset_("F", &i__1, &i__2, &c_b227, &c_b248, &u[*n + 1 + (
			    *n + 1) * u_dim1], ldu);
		}

/*              Overwrite U by left singular vectors of A and VT */
/*              by right singular vectors of A */
/*              (Workspace: need N*N+2*N+M, prefer N*N+2*N+M*NB) */

		i__1 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, m, n, &a[a_offset], lda, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__1, &ierr);
		i__1 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", n, n, m, &a[a_offset], lda, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__1, &
			ierr);
	    }

	}

    } else {

/*        A has more columns than rows. If A has sufficiently more */
/*        columns than rows, first reduce using the LQ decomposition (if */
/*        sufficient workspace available) */

	if (*n >= mnthr) {

	    if (wntqn) {

/*              Path 1t (N much larger than M, JOBZ='N') */
/*              No singular vectors to be computed */

		itau = 1;
		nwork = itau + *m;

/*              Compute A=L*Q */
/*              (Workspace: need 2*M, prefer M+M*NB) */

		i__1 = *lwork - nwork + 1;
		dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__1, &ierr);

/*              Zero out above L */

		i__1 = *m - 1;
		i__2 = *m - 1;
		dlaset_("U", &i__1, &i__2, &c_b227, &c_b227, &a[(a_dim1 << 1)
			+ 1], lda);
		ie = 1;
		itauq = ie + *m;
		itaup = itauq + *m;
		nwork = itaup + *m;

/*              Bidiagonalize L in A */
/*              (Workspace: need 4*M, prefer 3*M+2*M*NB) */

		i__1 = *lwork - nwork + 1;
		dgebrd_(m, m, &a[a_offset], lda, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__1, &ierr);
		nwork = ie + *m;

/*              Perform bidiagonal SVD, computing singular values only */
/*              (Workspace: need M+BDSPAC) */

		dbdsdc_("U", "N", m, &s[1], &work[ie], dum, &c__1, dum, &c__1,
			 dum, idum, &work[nwork], &iwork[1], info);

	    } else if (wntqo) {

/*              Path 2t (N much larger than M, JOBZ='O') */
/*              M right singular vectors to be overwritten on A and */
/*              M left singular vectors to be computed in U */

		ivt = 1;

/*              IVT is M by M */

		il = ivt + *m * *m;
		if (*lwork >= *m * *n + *m * *m + *m * 3 + bdspac) {

/*                 WORK(IL) is M by N */

		    ldwrkl = *m;
		    chunk = *n;
		} else {
		    ldwrkl = *m;
		    chunk = (*lwork - *m * *m) / *m;
		}
		itau = il + ldwrkl * *m;
		nwork = itau + *m;

/*              Compute A=L*Q */
/*              (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		i__1 = *lwork - nwork + 1;
		dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__1, &ierr);

/*              Copy L to WORK(IL), zeroing about above it */

		dlacpy_("L", m, m, &a[a_offset], lda, &work[il], &ldwrkl);
		i__1 = *m - 1;
		i__2 = *m - 1;
		dlaset_("U", &i__1, &i__2, &c_b227, &c_b227, &work[il +
			ldwrkl], &ldwrkl);

/*              Generate Q in A */
/*              (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		i__1 = *lwork - nwork + 1;
		dorglq_(m, n, m, &a[a_offset], lda, &work[itau], &work[nwork],
			 &i__1, &ierr);
		ie = itau;
		itauq = ie + *m;
		itaup = itauq + *m;
		nwork = itaup + *m;

/*              Bidiagonalize L in WORK(IL) */
/*              (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

		i__1 = *lwork - nwork + 1;
		dgebrd_(m, m, &work[il], &ldwrkl, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__1, &ierr);

/*              Perform bidiagonal SVD, computing left singular vectors */
/*              of bidiagonal matrix in U, and computing right singular */
/*              vectors of bidiagonal matrix in WORK(IVT) */
/*              (Workspace: need M+M*M+BDSPAC) */

		dbdsdc_("U", "I", m, &s[1], &work[ie], &u[u_offset], ldu, &
			work[ivt], m, dum, idum, &work[nwork], &iwork[1],
			info);

/*              Overwrite U by left singular vectors of L and WORK(IVT) */
/*              by right singular vectors of L */
/*              (Workspace: need 2*M*M+3*M, prefer 2*M*M+2*M+M*NB) */

		i__1 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, m, m, &work[il], &ldwrkl, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__1, &ierr);
		i__1 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", m, m, m, &work[il], &ldwrkl, &work[
			itaup], &work[ivt], m, &work[nwork], &i__1, &ierr);

/*              Multiply right singular vectors of L in WORK(IVT) by Q */
/*              in A, storing result in WORK(IL) and copying to A */
/*              (Workspace: need 2*M*M, prefer M*M+M*N) */

		i__1 = *n;
		i__2 = chunk;
		for (i__ = 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ +=
			i__2) {
/* Computing MIN */
		    i__3 = *n - i__ + 1;
		    blk = std::min(i__3,chunk);
		    dgemm_("N", "N", m, &blk, m, &c_b248, &work[ivt], m, &a[
			    i__ * a_dim1 + 1], lda, &c_b227, &work[il], &
			    ldwrkl);
		    dlacpy_("F", m, &blk, &work[il], &ldwrkl, &a[i__ * a_dim1
			    + 1], lda);
/* L30: */
		}

	    } else if (wntqs) {

/*              Path 3t (N much larger than M, JOBZ='S') */
/*              M right singular vectors to be computed in VT and */
/*              M left singular vectors to be computed in U */

		il = 1;

/*              WORK(IL) is M by M */

		ldwrkl = *m;
		itau = il + ldwrkl * *m;
		nwork = itau + *m;

/*              Compute A=L*Q */
/*              (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		i__2 = *lwork - nwork + 1;
		dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__2, &ierr);

/*              Copy L to WORK(IL), zeroing out above it */

		dlacpy_("L", m, m, &a[a_offset], lda, &work[il], &ldwrkl);
		i__2 = *m - 1;
		i__1 = *m - 1;
		dlaset_("U", &i__2, &i__1, &c_b227, &c_b227, &work[il +
			ldwrkl], &ldwrkl);

/*              Generate Q in A */
/*              (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		i__2 = *lwork - nwork + 1;
		dorglq_(m, n, m, &a[a_offset], lda, &work[itau], &work[nwork],
			 &i__2, &ierr);
		ie = itau;
		itauq = ie + *m;
		itaup = itauq + *m;
		nwork = itaup + *m;

/*              Bidiagonalize L in WORK(IU), copying result to U */
/*              (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

		i__2 = *lwork - nwork + 1;
		dgebrd_(m, m, &work[il], &ldwrkl, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__2, &ierr);

/*              Perform bidiagonal SVD, computing left singular vectors */
/*              of bidiagonal matrix in U and computing right singular */
/*              vectors of bidiagonal matrix in VT */
/*              (Workspace: need M+BDSPAC) */

		dbdsdc_("U", "I", m, &s[1], &work[ie], &u[u_offset], ldu, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*              Overwrite U by left singular vectors of L and VT */
/*              by right singular vectors of L */
/*              (Workspace: need M*M+3*M, prefer M*M+2*M+M*NB) */

		i__2 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, m, m, &work[il], &ldwrkl, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__2, &ierr);
		i__2 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", m, m, m, &work[il], &ldwrkl, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__2, &
			ierr);

/*              Multiply right singular vectors of L in WORK(IL) by */
/*              Q in A, storing result in VT */
/*              (Workspace: need M*M) */

		dlacpy_("F", m, m, &vt[vt_offset], ldvt, &work[il], &ldwrkl);
		dgemm_("N", "N", m, n, m, &c_b248, &work[il], &ldwrkl, &a[
			a_offset], lda, &c_b227, &vt[vt_offset], ldvt);

	    } else if (wntqa) {

/*              Path 4t (N much larger than M, JOBZ='A') */
/*              N right singular vectors to be computed in VT and */
/*              M left singular vectors to be computed in U */

		ivt = 1;

/*              WORK(IVT) is M by M */

		ldwkvt = *m;
		itau = ivt + ldwkvt * *m;
		nwork = itau + *m;

/*              Compute A=L*Q, copying result to VT */
/*              (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		i__2 = *lwork - nwork + 1;
		dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[nwork], &
			i__2, &ierr);
		dlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

/*              Generate Q in VT */
/*              (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		i__2 = *lwork - nwork + 1;
		dorglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &work[
			nwork], &i__2, &ierr);

/*              Produce L in A, zeroing out other entries */

		i__2 = *m - 1;
		i__1 = *m - 1;
		dlaset_("U", &i__2, &i__1, &c_b227, &c_b227, &a[(a_dim1 << 1)
			+ 1], lda);
		ie = itau;
		itauq = ie + *m;
		itaup = itauq + *m;
		nwork = itaup + *m;

/*              Bidiagonalize L in A */
/*              (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

		i__2 = *lwork - nwork + 1;
		dgebrd_(m, m, &a[a_offset], lda, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[nwork], &i__2, &ierr);

/*              Perform bidiagonal SVD, computing left singular vectors */
/*              of bidiagonal matrix in U and computing right singular */
/*              vectors of bidiagonal matrix in WORK(IVT) */
/*              (Workspace: need M+M*M+BDSPAC) */

		dbdsdc_("U", "I", m, &s[1], &work[ie], &u[u_offset], ldu, &
			work[ivt], &ldwkvt, dum, idum, &work[nwork], &iwork[1]
, info);

/*              Overwrite U by left singular vectors of L and WORK(IVT) */
/*              by right singular vectors of L */
/*              (Workspace: need M*M+3*M, prefer M*M+2*M+M*NB) */

		i__2 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, m, m, &a[a_offset], lda, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__2, &ierr);
		i__2 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", m, m, m, &a[a_offset], lda, &work[
			itaup], &work[ivt], &ldwkvt, &work[nwork], &i__2, &
			ierr);

/*              Multiply right singular vectors of L in WORK(IVT) by */
/*              Q in VT, storing result in A */
/*              (Workspace: need M*M) */

		dgemm_("N", "N", m, n, m, &c_b248, &work[ivt], &ldwkvt, &vt[
			vt_offset], ldvt, &c_b227, &a[a_offset], lda);

/*              Copy right singular vectors of A from A to VT */

		dlacpy_("F", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);

	    }

	} else {

/*           N .LT. MNTHR */

/*           Path 5t (N greater than M, but not much larger) */
/*           Reduce to bidiagonal form without LQ decomposition */

	    ie = 1;
	    itauq = ie + *m;
	    itaup = itauq + *m;
	    nwork = itaup + *m;

/*           Bidiagonalize A */
/*           (Workspace: need 3*M+N, prefer 3*M+(M+N)*NB) */

	    i__2 = *lwork - nwork + 1;
	    dgebrd_(m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &
		    work[itaup], &work[nwork], &i__2, &ierr);
	    if (wntqn) {

/*              Perform bidiagonal SVD, only computing singular values */
/*              (Workspace: need M+BDSPAC) */

		dbdsdc_("L", "N", m, &s[1], &work[ie], dum, &c__1, dum, &c__1,
			 dum, idum, &work[nwork], &iwork[1], info);
	    } else if (wntqo) {
		ldwkvt = *m;
		ivt = nwork;
		if (*lwork >= *m * *n + *m * 3 + bdspac) {

/*                 WORK( IVT ) is M by N */

		    dlaset_("F", m, n, &c_b227, &c_b227, &work[ivt], &ldwkvt);
		    nwork = ivt + ldwkvt * *n;
		} else {

/*                 WORK( IVT ) is M by M */

		    nwork = ivt + ldwkvt * *m;
		    il = nwork;

/*                 WORK(IL) is M by CHUNK */

		    chunk = (*lwork - *m * *m - *m * 3) / *m;
		}

/*              Perform bidiagonal SVD, computing left singular vectors */
/*              of bidiagonal matrix in U and computing right singular */
/*              vectors of bidiagonal matrix in WORK(IVT) */
/*              (Workspace: need M*M+BDSPAC) */

		dbdsdc_("L", "I", m, &s[1], &work[ie], &u[u_offset], ldu, &
			work[ivt], &ldwkvt, dum, idum, &work[nwork], &iwork[1]
, info);

/*              Overwrite U by left singular vectors of A */
/*              (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		i__2 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, m, n, &a[a_offset], lda, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__2, &ierr);

		if (*lwork >= *m * *n + *m * 3 + bdspac) {

/*                 Overwrite WORK(IVT) by left singular vectors of A */
/*                 (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		    i__2 = *lwork - nwork + 1;
		    dormbr_("P", "R", "T", m, n, m, &a[a_offset], lda, &work[
			    itaup], &work[ivt], &ldwkvt, &work[nwork], &i__2,
			    &ierr);

/*                 Copy right singular vectors of A from WORK(IVT) to A */

		    dlacpy_("F", m, n, &work[ivt], &ldwkvt, &a[a_offset], lda);
		} else {

/*                 Generate P**T in A */
/*                 (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		    i__2 = *lwork - nwork + 1;
		    dorgbr_("P", m, n, m, &a[a_offset], lda, &work[itaup], &
			    work[nwork], &i__2, &ierr);

/*                 Multiply Q in A by right singular vectors of */
/*                 bidiagonal matrix in WORK(IVT), storing result in */
/*                 WORK(IL) and copying to A */
/*                 (Workspace: need 2*M*M, prefer M*M+M*N) */

		    i__2 = *n;
		    i__1 = chunk;
		    for (i__ = 1; i__1 < 0 ? i__ >= i__2 : i__ <= i__2; i__ +=
			     i__1) {
/* Computing MIN */
			i__3 = *n - i__ + 1;
			blk = std::min(i__3,chunk);
			dgemm_("N", "N", m, &blk, m, &c_b248, &work[ivt], &
				ldwkvt, &a[i__ * a_dim1 + 1], lda, &c_b227, &
				work[il], m);
			dlacpy_("F", m, &blk, &work[il], m, &a[i__ * a_dim1 +
				1], lda);
/* L40: */
		    }
		}
	    } else if (wntqs) {

/*              Perform bidiagonal SVD, computing left singular vectors */
/*              of bidiagonal matrix in U and computing right singular */
/*              vectors of bidiagonal matrix in VT */
/*              (Workspace: need M+BDSPAC) */

		dlaset_("F", m, n, &c_b227, &c_b227, &vt[vt_offset], ldvt);
		dbdsdc_("L", "I", m, &s[1], &work[ie], &u[u_offset], ldu, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*              Overwrite U by left singular vectors of A and VT */
/*              by right singular vectors of A */
/*              (Workspace: need 3*M, prefer 2*M+M*NB) */

		i__1 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, m, n, &a[a_offset], lda, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__1, &ierr);
		i__1 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", m, n, m, &a[a_offset], lda, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__1, &
			ierr);
	    } else if (wntqa) {

/*              Perform bidiagonal SVD, computing left singular vectors */
/*              of bidiagonal matrix in U and computing right singular */
/*              vectors of bidiagonal matrix in VT */
/*              (Workspace: need M+BDSPAC) */

		dlaset_("F", n, n, &c_b227, &c_b227, &vt[vt_offset], ldvt);
		dbdsdc_("L", "I", m, &s[1], &work[ie], &u[u_offset], ldu, &vt[
			vt_offset], ldvt, dum, idum, &work[nwork], &iwork[1],
			info);

/*              Set the right corner of VT to identity matrix */

		if (*n > *m) {
		    i__1 = *n - *m;
		    i__2 = *n - *m;
		    dlaset_("F", &i__1, &i__2, &c_b227, &c_b248, &vt[*m + 1 +
			    (*m + 1) * vt_dim1], ldvt);
		}

/*              Overwrite U by left singular vectors of A and VT */
/*              by right singular vectors of A */
/*              (Workspace: need 2*M+N, prefer 2*M+N*NB) */

		i__1 = *lwork - nwork + 1;
		dormbr_("Q", "L", "N", m, m, n, &a[a_offset], lda, &work[
			itauq], &u[u_offset], ldu, &work[nwork], &i__1, &ierr);
		i__1 = *lwork - nwork + 1;
		dormbr_("P", "R", "T", n, n, m, &a[a_offset], lda, &work[
			itaup], &vt[vt_offset], ldvt, &work[nwork], &i__1, &
			ierr);
	    }

	}

    }

/*     Undo scaling if necessary */

    if (iscl == 1) {
	if (anrm > bignum) {
	    dlascl_("G", &c__0, &c__0, &bignum, &anrm, &minmn, &c__1, &s[1], &
		    minmn, &ierr);
	}
	if (anrm < smlnum) {
	    dlascl_("G", &c__0, &c__0, &smlnum, &anrm, &minmn, &c__1, &s[1], &
		    minmn, &ierr);
	}
    }

/*     Return optimal workspace in WORK(1) */

    work[1] = (double) maxwrk;

    return 0;

/*     End of DGESDD */

} /* dgesdd_ */

/* Subroutine */ int dgesv_(integer *n, integer *nrhs, double *a, integer
	*lda, integer *ipiv, double *b, integer *ldb, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1;

/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGESV computes the solution to a real system of linear equations */
/*     A * X = B, */
/*  where A is an N-by-N matrix and X and B are N-by-NRHS matrices. */

/*  The LU decomposition with partial pivoting and row interchanges is */
/*  used to factor A as */
/*     A = P * L * U, */
/*  where P is a permutation matrix, L is unit lower triangular, and U is */
/*  upper triangular.  The factored form of A is then used to solve the */
/*  system of equations A * X = B. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of linear equations, i.e., the order of the */
/*          matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the N-by-N coefficient matrix A. */
/*          On exit, the factors L and U from the factorization */
/*          A = P*L*U; the unit diagonal elements of L are not stored. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  IPIV    (output) INTEGER array, dimension (N) */
/*          The pivot indices that define the permutation matrix P; */
/*          row i of the matrix was interchanged with row IPIV(i). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the N-by-NRHS matrix of right hand side matrix B. */
/*          On exit, if INFO = 0, the N-by-NRHS solution matrix X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, U(i,i) is exactly zero.  The factorization */
/*                has been completed, but the factor U is exactly */
/*                singular, so the solution could not be computed. */

/*  ===================================================================== */

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
    --ipiv;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    *info = 0;
    if (*n < 0) {
	*info = -1;
    } else if (*nrhs < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -4;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -7;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGESV ", &i__1);
	return 0;
    }

/*     Compute the LU factorization of A. */

    dgetrf_(n, n, &a[a_offset], lda, &ipiv[1], info);
    if (*info == 0) {

/*        Solve the system A*X = B, overwriting B with X. */

	dgetrs_("No transpose", n, nrhs, &a[a_offset], lda, &ipiv[1], &b[
		b_offset], ldb, info);
    }
    return 0;

/*     End of DGESV */

} /* dgesv_ */

/* Subroutine */ int dgesvd_(const char *jobu, const char *jobvt, integer *m, integer *n,
	double *a, integer *lda, double *s, double *u, integer *
	ldu, double *vt, integer *ldvt, double *work, integer *lwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__6 = 6;
	static integer c__0 = 0;
	static integer c__2 = 2;
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static double c_b421 = 0.;
	static double c_b443 = 1.;

    /* System generated locals */
    char * a__1[2];
    integer a_dim1, a_offset, u_dim1, u_offset, vt_dim1, vt_offset, i__1[2],
	    i__2, i__3, i__4;
    char ch__1[3];

    /* Local variables */
    integer i__, ie, ir, iu, blk, ncu;
    double dum[1], eps;
    integer nru, iscl;
    double anrm;
    integer ierr, itau, ncvt, nrvt;
    integer chunk, minmn, wrkbl, itaup, itauq, mnthr, iwork;
    bool wntua, wntva, wntun, wntuo, wntvn, wntvo, wntus, wntvs;
    integer bdspac;
    double bignum;
    integer ldwrkr, minwrk, ldwrku, maxwrk;
    double smlnum;
    bool lquery, wntuas, wntvas;

/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGESVD computes the singular value decomposition (SVD) of a real */
/*  M-by-N matrix A, optionally computing the left and/or right singular */
/*  vectors. The SVD is written */

/*       A = U * SIGMA * transpose(V) */

/*  where SIGMA is an M-by-N matrix which is zero except for its */
/*  min(m,n) diagonal elements, U is an M-by-M orthogonal matrix, and */
/*  V is an N-by-N orthogonal matrix.  The diagonal elements of SIGMA */
/*  are the singular values of A; they are real and non-negative, and */
/*  are returned in descending order.  The first min(m,n) columns of */
/*  U and V are the left and right singular vectors of A. */

/*  Note that the routine returns V**T, not V. */

/*  Arguments */
/*  ========= */

/*  JOBU    (input) CHARACTER*1 */
/*          Specifies options for computing all or part of the matrix U: */
/*          = 'A':  all M columns of U are returned in array U: */
/*          = 'S':  the first min(m,n) columns of U (the left singular */
/*                  vectors) are returned in the array U; */
/*          = 'O':  the first min(m,n) columns of U (the left singular */
/*                  vectors) are overwritten on the array A; */
/*          = 'N':  no columns of U (no left singular vectors) are */
/*                  computed. */

/*  JOBVT   (input) CHARACTER*1 */
/*          Specifies options for computing all or part of the matrix */
/*          V**T: */
/*          = 'A':  all N rows of V**T are returned in the array VT; */
/*          = 'S':  the first min(m,n) rows of V**T (the right singular */
/*                  vectors) are returned in the array VT; */
/*          = 'O':  the first min(m,n) rows of V**T (the right singular */
/*                  vectors) are overwritten on the array A; */
/*          = 'N':  no rows of V**T (no right singular vectors) are */
/*                  computed. */

/*          JOBVT and JOBU cannot both be 'O'. */

/*  M       (input) INTEGER */
/*          The number of rows of the input matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the input matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, */
/*          if JOBU = 'O',  A is overwritten with the first min(m,n) */
/*                          columns of U (the left singular vectors, */
/*                          stored columnwise); */
/*          if JOBVT = 'O', A is overwritten with the first min(m,n) */
/*                          rows of V**T (the right singular vectors, */
/*                          stored rowwise); */
/*          if JOBU .ne. 'O' and JOBVT .ne. 'O', the contents of A */
/*                          are destroyed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  S       (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The singular values of A, sorted so that S(i) >= S(i+1). */

/*  U       (output) DOUBLE PRECISION array, dimension (LDU,UCOL) */
/*          (LDU,M) if JOBU = 'A' or (LDU,min(M,N)) if JOBU = 'S'. */
/*          If JOBU = 'A', U contains the M-by-M orthogonal matrix U; */
/*          if JOBU = 'S', U contains the first min(m,n) columns of U */
/*          (the left singular vectors, stored columnwise); */
/*          if JOBU = 'N' or 'O', U is not referenced. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U.  LDU >= 1; if */
/*          JOBU = 'S' or 'A', LDU >= M. */

/*  VT      (output) DOUBLE PRECISION array, dimension (LDVT,N) */
/*          If JOBVT = 'A', VT contains the N-by-N orthogonal matrix */
/*          V**T; */
/*          if JOBVT = 'S', VT contains the first min(m,n) rows of */
/*          V**T (the right singular vectors, stored rowwise); */
/*          if JOBVT = 'N' or 'O', VT is not referenced. */

/*  LDVT    (input) INTEGER */
/*          The leading dimension of the array VT.  LDVT >= 1; if */
/*          JOBVT = 'A', LDVT >= N; if JOBVT = 'S', LDVT >= min(M,N). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK; */
/*          if INFO > 0, WORK(2:MIN(M,N)) contains the unconverged */
/*          superdiagonal elements of an upper bidiagonal matrix B */
/*          whose diagonal is in S (not necessarily sorted). B */
/*          satisfies A = U * B * VT, so it has the same singular values */
/*          as A, and singular vectors related by U and VT. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          LWORK >= MAX(1,3*MIN(M,N)+MAX(M,N),5*MIN(M,N)). */
/*          For good performance, LWORK should generally be larger. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if DBDSQR did not converge, INFO specifies how many */
/*                superdiagonals of an intermediate bidiagonal form B */
/*                did not converge to zero. See the description of WORK */
/*                above for details. */

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

/*     Test the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --s;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    vt_dim1 = *ldvt;
    vt_offset = 1 + vt_dim1;
    vt -= vt_offset;
    --work;

    /* Function Body */
    *info = 0;
    minmn = std::min(*m,*n);
    wntua = lsame_(jobu, "A");
    wntus = lsame_(jobu, "S");
    wntuas = wntua || wntus;
    wntuo = lsame_(jobu, "O");
    wntun = lsame_(jobu, "N");
    wntva = lsame_(jobvt, "A");
    wntvs = lsame_(jobvt, "S");
    wntvas = wntva || wntvs;
    wntvo = lsame_(jobvt, "O");
    wntvn = lsame_(jobvt, "N");
    lquery = *lwork == -1;

    if (! (wntua || wntus || wntuo || wntun)) {
	*info = -1;
    } else if (! (wntva || wntvs || wntvo || wntvn) || wntvo && wntuo) {
	*info = -2;
    } else if (*m < 0) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -6;
    } else if (*ldu < 1 || wntuas && *ldu < *m) {
	*info = -9;
    } else if (*ldvt < 1 || wntva && *ldvt < *n || wntvs && *ldvt < minmn) {
	*info = -11;
    }

/*     Compute workspace */
/*      (Note: Comments in the code beginning "Workspace:" describe the */
/*       minimal amount of workspace needed at that point in the code, */
/*       as well as the preferred amount for good performance. */
/*       NB refers to the optimal block size for the immediately */
/*       following subroutine, as returned by ILAENV.) */

    if (*info == 0) {
	minwrk = 1;
	maxwrk = 1;
	if (*m >= *n && minmn > 0) {

/*           Compute space needed for DBDSQR */

/* Writing concatenation */
	    i__1[0] = 1, a__1[0] = const_cast<char *>(jobu);
	    i__1[1] = 1, a__1[1] = const_cast<char *>(jobvt);
	    s_cat(ch__1, a__1, i__1, &c__2, 2_integer);
		ch__1 [2] = '\0';
	    mnthr = ilaenv_(&c__6, "DGESVD", ch__1, m, n, &c__0, &c__0);
	    bdspac = *n * 5;
	    if (*m >= mnthr) {
		if (wntun) {

/*                 Path 1 (M much larger than N, JOBU='N') */

		    maxwrk = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", n, n, &c_n1, &c_n1);
		    maxwrk = std::max(i__2,i__3);
		    if (wntvo || wntvas) {
/* Computing MAX */
			i__2 = maxwrk, i__3 = *n * 3 + (*n - 1) * ilaenv_(&
				c__1, "DORGBR", "P", n, n, n, &c_n1);
			maxwrk = std::max(i__2,i__3);
		    }
		    maxwrk = std::max(maxwrk,bdspac);
/* Computing MAX */
		    i__2 = *n << 2;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntuo && wntvn) {

/*                 Path 2 (M much larger than N, JOBU='O', JOBVT='N') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *n * ilaenv_(&c__1, "DORGQR",
			    " ", m, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", n, n, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "DORGBR"
, "Q", n, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
/* Computing MAX */
		    i__2 = *n * *n + wrkbl, i__3 = *n * *n + *m * *n + *n;
		    maxwrk = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntuo && wntvas) {

/*                 Path 3 (M much larger than N, JOBU='O', JOBVT='S' or */
/*                 'A') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *n * ilaenv_(&c__1, "DORGQR",
			    " ", m, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", n, n, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "DORGBR"
, "Q", n, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n - 1) * ilaenv_(&c__1,
			    "DORGBR", "P", n, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
/* Computing MAX */
		    i__2 = *n * *n + wrkbl, i__3 = *n * *n + *m * *n + *n;
		    maxwrk = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntus && wntvn) {

/*                 Path 4 (M much larger than N, JOBU='S', JOBVT='N') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *n * ilaenv_(&c__1, "DORGQR",
			    " ", m, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", n, n, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "DORGBR"
, "Q", n, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
		    maxwrk = *n * *n + wrkbl;
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntus && wntvo) {

/*                 Path 5 (M much larger than N, JOBU='S', JOBVT='O') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *n * ilaenv_(&c__1, "DORGQR",
			    " ", m, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", n, n, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "DORGBR"
, "Q", n, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n - 1) * ilaenv_(&c__1,
			    "DORGBR", "P", n, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
		    maxwrk = (*n << 1) * *n + wrkbl;
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntus && wntvas) {

/*                 Path 6 (M much larger than N, JOBU='S', JOBVT='S' or */
/*                 'A') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *n * ilaenv_(&c__1, "DORGQR",
			    " ", m, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", n, n, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "DORGBR"
, "Q", n, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n - 1) * ilaenv_(&c__1,
			    "DORGBR", "P", n, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
		    maxwrk = *n * *n + wrkbl;
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntua && wntvn) {

/*                 Path 7 (M much larger than N, JOBU='A', JOBVT='N') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *m * ilaenv_(&c__1, "DORGQR",
			    " ", m, m, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", n, n, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "DORGBR"
, "Q", n, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
		    maxwrk = *n * *n + wrkbl;
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntua && wntvo) {

/*                 Path 8 (M much larger than N, JOBU='A', JOBVT='O') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *m * ilaenv_(&c__1, "DORGQR",
			    " ", m, m, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", n, n, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "DORGBR"
, "Q", n, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n - 1) * ilaenv_(&c__1,
			    "DORGBR", "P", n, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
		    maxwrk = (*n << 1) * *n + wrkbl;
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntua && wntvas) {

/*                 Path 9 (M much larger than N, JOBU='A', JOBVT='S' or */
/*                 'A') */

		    wrkbl = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n + *m * ilaenv_(&c__1, "DORGQR",
			    " ", m, m, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", n, n, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + *n * ilaenv_(&c__1, "DORGBR"
, "Q", n, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *n * 3 + (*n - 1) * ilaenv_(&c__1,
			    "DORGBR", "P", n, n, n, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
		    maxwrk = *n * *n + wrkbl;
/* Computing MAX */
		    i__2 = *n * 3 + *m;
		    minwrk = std::max(i__2,bdspac);
		}
	    } else {

/*              Path 10 (M at least N, but not much larger) */

		maxwrk = *n * 3 + (*m + *n) * ilaenv_(&c__1, "DGEBRD", " ", m,
			 n, &c_n1, &c_n1);
		if (wntus || wntuo) {
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *n * 3 + *n * ilaenv_(&c__1, "DORG"
			    "BR", "Q", m, n, n, &c_n1);
		    maxwrk = std::max(i__2,i__3);
		}
		if (wntua) {
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *n * 3 + *m * ilaenv_(&c__1, "DORG"
			    "BR", "Q", m, m, n, &c_n1);
		    maxwrk = std::max(i__2,i__3);
		}
		if (! wntvn) {
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *n * 3 + (*n - 1) * ilaenv_(&c__1,
			    "DORGBR", "P", n, n, n, &c_n1);
		    maxwrk = std::max(i__2,i__3);
		}
		maxwrk = std::max(maxwrk,bdspac);
/* Computing MAX */
		i__2 = *n * 3 + *m;
		minwrk = std::max(i__2,bdspac);
	    }
	} else if (minmn > 0) {

/*           Compute space needed for DBDSQR */

/* Writing concatenation */
	    i__1[0] = 1, a__1[0] = const_cast<char *>(jobu);
	    i__1[1] = 1, a__1[1] = const_cast<char *>(jobvt);
	    s_cat(ch__1, a__1, i__1, &c__2, 2_integer);
		ch__1 [2] = '\0';
	    mnthr = ilaenv_(&c__6, "DGESVD", ch__1, m, n, &c__0, &c__0);
	    bdspac = *m * 5;
	    if (*n >= mnthr) {
		if (wntvn) {

/*                 Path 1t(N much larger than M, JOBVT='N') */

		    maxwrk = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", m, m, &c_n1, &c_n1);
		    maxwrk = std::max(i__2,i__3);
		    if (wntuo || wntuas) {
/* Computing MAX */
			i__2 = maxwrk, i__3 = *m * 3 + *m * ilaenv_(&c__1,
				"DORGBR", "Q", m, m, m, &c_n1);
			maxwrk = std::max(i__2,i__3);
		    }
		    maxwrk = std::max(maxwrk,bdspac);
/* Computing MAX */
		    i__2 = *m << 2;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntvo && wntun) {

/*                 Path 2t(N much larger than M, JOBU='N', JOBVT='O') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *m * ilaenv_(&c__1, "DORGLQ",
			    " ", m, n, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", m, m, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1,
			    "DORGBR", "P", m, m, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
/* Computing MAX */
		    i__2 = *m * *m + wrkbl, i__3 = *m * *m + *m * *n + *m;
		    maxwrk = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntvo && wntuas) {

/*                 Path 3t(N much larger than M, JOBU='S' or 'A', */
/*                 JOBVT='O') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *m * ilaenv_(&c__1, "DORGLQ",
			    " ", m, n, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", m, m, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1,
			    "DORGBR", "P", m, m, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + *m * ilaenv_(&c__1, "DORGBR"
, "Q", m, m, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
/* Computing MAX */
		    i__2 = *m * *m + wrkbl, i__3 = *m * *m + *m * *n + *m;
		    maxwrk = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntvs && wntun) {

/*                 Path 4t(N much larger than M, JOBU='N', JOBVT='S') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *m * ilaenv_(&c__1, "DORGLQ",
			    " ", m, n, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", m, m, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1,
			    "DORGBR", "P", m, m, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
		    maxwrk = *m * *m + wrkbl;
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntvs && wntuo) {

/*                 Path 5t(N much larger than M, JOBU='O', JOBVT='S') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *m * ilaenv_(&c__1, "DORGLQ",
			    " ", m, n, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", m, m, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1,
			    "DORGBR", "P", m, m, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + *m * ilaenv_(&c__1, "DORGBR"
, "Q", m, m, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
		    maxwrk = (*m << 1) * *m + wrkbl;
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntvs && wntuas) {

/*                 Path 6t(N much larger than M, JOBU='S' or 'A', */
/*                 JOBVT='S') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *m * ilaenv_(&c__1, "DORGLQ",
			    " ", m, n, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", m, m, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1,
			    "DORGBR", "P", m, m, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + *m * ilaenv_(&c__1, "DORGBR"
, "Q", m, m, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
		    maxwrk = *m * *m + wrkbl;
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntva && wntun) {

/*                 Path 7t(N much larger than M, JOBU='N', JOBVT='A') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *n * ilaenv_(&c__1, "DORGLQ",
			    " ", n, n, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", m, m, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1,
			    "DORGBR", "P", m, m, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
		    maxwrk = *m * *m + wrkbl;
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntva && wntuo) {

/*                 Path 8t(N much larger than M, JOBU='O', JOBVT='A') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *n * ilaenv_(&c__1, "DORGLQ",
			    " ", n, n, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", m, m, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1,
			    "DORGBR", "P", m, m, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + *m * ilaenv_(&c__1, "DORGBR"
, "Q", m, m, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
		    maxwrk = (*m << 1) * *m + wrkbl;
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = std::max(i__2,bdspac);
		} else if (wntva && wntuas) {

/*                 Path 9t(N much larger than M, JOBU='S' or 'A', */
/*                 JOBVT='A') */

		    wrkbl = *m + *m * ilaenv_(&c__1, "DGELQF", " ", m, n, &
			    c_n1, &c_n1);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m + *n * ilaenv_(&c__1, "DORGLQ",
			    " ", n, n, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m << 1) * ilaenv_(&c__1,
			    "DGEBRD", " ", m, m, &c_n1, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1,
			    "DORGBR", "P", m, m, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *m * 3 + *m * ilaenv_(&c__1, "DORGBR"
, "Q", m, m, m, &c_n1);
		    wrkbl = std::max(i__2,i__3);
		    wrkbl = std::max(wrkbl,bdspac);
		    maxwrk = *m * *m + wrkbl;
/* Computing MAX */
		    i__2 = *m * 3 + *n;
		    minwrk = std::max(i__2,bdspac);
		}
	    } else {

/*              Path 10t(N greater than M, but not much larger) */

		maxwrk = *m * 3 + (*m + *n) * ilaenv_(&c__1, "DGEBRD", " ", m,
			 n, &c_n1, &c_n1);
		if (wntvs || wntvo) {
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *m * 3 + *m * ilaenv_(&c__1, "DORG"
			    "BR", "P", m, n, m, &c_n1);
		    maxwrk = std::max(i__2,i__3);
		}
		if (wntva) {
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *m * 3 + *n * ilaenv_(&c__1, "DORG"
			    "BR", "P", n, n, m, &c_n1);
		    maxwrk = std::max(i__2,i__3);
		}
		if (! wntun) {
/* Computing MAX */
		    i__2 = maxwrk, i__3 = *m * 3 + (*m - 1) * ilaenv_(&c__1,
			    "DORGBR", "Q", m, m, m, &c_n1);
		    maxwrk = std::max(i__2,i__3);
		}
		maxwrk = std::max(maxwrk,bdspac);
/* Computing MAX */
		i__2 = *m * 3 + *n;
		minwrk = std::max(i__2,bdspac);
	    }
	}
	maxwrk = std::max(maxwrk,minwrk);
	work[1] = (double) maxwrk;

	if (*lwork < minwrk && ! lquery) {
	    *info = -13;
	}
    }

    if (*info != 0) {
	i__2 = -(*info);
	xerbla_("DGESVD", &i__2);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	return 0;
    }

/*     Get machine constants */

    eps = dlamch_("P");
    smlnum = sqrt(dlamch_("S")) / eps;
    bignum = 1. / smlnum;

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", m, n, &a[a_offset], lda, dum);
    iscl = 0;
    if (anrm > 0. && anrm < smlnum) {
	iscl = 1;
	dlascl_("G", &c__0, &c__0, &anrm, &smlnum, m, n, &a[a_offset], lda, &
		ierr);
    } else if (anrm > bignum) {
	iscl = 1;
	dlascl_("G", &c__0, &c__0, &anrm, &bignum, m, n, &a[a_offset], lda, &
		ierr);
    }

    if (*m >= *n) {

/*        A has at least as many rows as columns. If A has sufficiently */
/*        more rows than columns, first reduce using the QR */
/*        decomposition (if sufficient workspace available) */

	if (*m >= mnthr) {

	    if (wntun) {

/*              Path 1 (M much larger than N, JOBU='N') */
/*              No left singular vectors to be computed */

		itau = 1;
		iwork = itau + *n;

/*              Compute A=Q*R */
/*              (Workspace: need 2*N, prefer N+N*NB) */

		i__2 = *lwork - iwork + 1;
		dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork], &
			i__2, &ierr);

/*              Zero out below R */

		i__2 = *n - 1;
		i__3 = *n - 1;
		dlaset_("L", &i__2, &i__3, &c_b421, &c_b421, &a[a_dim1 + 2],
			lda);
		ie = 1;
		itauq = ie + *n;
		itaup = itauq + *n;
		iwork = itaup + *n;

/*              Bidiagonalize R in A */
/*              (Workspace: need 4*N, prefer 3*N+2*N*NB) */

		i__2 = *lwork - iwork + 1;
		dgebrd_(n, n, &a[a_offset], lda, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[iwork], &i__2, &ierr);
		ncvt = 0;
		if (wntvo || wntvas) {

/*                 If right singular vectors desired, generate P'. */
/*                 (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

		    i__2 = *lwork - iwork + 1;
		    dorgbr_("P", n, n, n, &a[a_offset], lda, &work[itaup], &
			    work[iwork], &i__2, &ierr);
		    ncvt = *n;
		}
		iwork = ie + *n;

/*              Perform bidiagonal QR iteration, computing right */
/*              singular vectors of A in A if desired */
/*              (Workspace: need BDSPAC) */

		dbdsqr_("U", n, &ncvt, &c__0, &c__0, &s[1], &work[ie], &a[
			a_offset], lda, dum, &c__1, dum, &c__1, &work[iwork],
			info);

/*              If right singular vectors desired in VT, copy them there */

		if (wntvas) {
		    dlacpy_("F", n, n, &a[a_offset], lda, &vt[vt_offset],
			    ldvt);
		}

	    } else if (wntuo && wntvn) {

/*              Path 2 (M much larger than N, JOBU='O', JOBVT='N') */
/*              N left singular vectors to be overwritten on A and */
/*              no right singular vectors to be computed */

/* Computing MAX */
		i__2 = *n << 2;
		if (*lwork >= *n * *n + std::max(i__2,bdspac)) {

/*                 Sufficient workspace for a fast algorithm */

		    ir = 1;
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *lda * *n + *n;
		    if (*lwork >= std::max(i__2,i__3) + *lda * *n) {

/*                    WORK(IU) is LDA by N, WORK(IR) is LDA by N */

			ldwrku = *lda;
			ldwrkr = *lda;
		    } else /* if(complicated condition) */ {
/* Computing MAX */
			i__2 = wrkbl, i__3 = *lda * *n + *n;
			if (*lwork >= std::max(i__2,i__3) + *n * *n) {

/*                    WORK(IU) is LDA by N, WORK(IR) is N by N */

			    ldwrku = *lda;
			    ldwrkr = *n;
			} else {

/*                    WORK(IU) is LDWRKU by N, WORK(IR) is N by N */

			    ldwrku = (*lwork - *n * *n - *n) / *n;
			    ldwrkr = *n;
			}
		    }
		    itau = ir + ldwrkr * *n;
		    iwork = itau + *n;

/*                 Compute A=Q*R */
/*                 (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		    i__2 = *lwork - iwork + 1;
		    dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
, &i__2, &ierr);

/*                 Copy R to WORK(IR) and zero out below it */

		    dlacpy_("U", n, n, &a[a_offset], lda, &work[ir], &ldwrkr);
		    i__2 = *n - 1;
		    i__3 = *n - 1;
		    dlaset_("L", &i__2, &i__3, &c_b421, &c_b421, &work[ir + 1]
, &ldwrkr);

/*                 Generate Q in A */
/*                 (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		    i__2 = *lwork - iwork + 1;
		    dorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &work[
			    iwork], &i__2, &ierr);
		    ie = itau;
		    itauq = ie + *n;
		    itaup = itauq + *n;
		    iwork = itaup + *n;

/*                 Bidiagonalize R in WORK(IR) */
/*                 (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

		    i__2 = *lwork - iwork + 1;
		    dgebrd_(n, n, &work[ir], &ldwrkr, &s[1], &work[ie], &work[
			    itauq], &work[itaup], &work[iwork], &i__2, &ierr);

/*                 Generate left vectors bidiagonalizing R */
/*                 (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

		    i__2 = *lwork - iwork + 1;
		    dorgbr_("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq], &
			    work[iwork], &i__2, &ierr);
		    iwork = ie + *n;

/*                 Perform bidiagonal QR iteration, computing left */
/*                 singular vectors of R in WORK(IR) */
/*                 (Workspace: need N*N+BDSPAC) */

		    dbdsqr_("U", n, &c__0, n, &c__0, &s[1], &work[ie], dum, &
			    c__1, &work[ir], &ldwrkr, dum, &c__1, &work[iwork]
, info);
		    iu = ie + *n;

/*                 Multiply Q in A by left singular vectors of R in */
/*                 WORK(IR), storing result in WORK(IU) and copying to A */
/*                 (Workspace: need N*N+2*N, prefer N*N+M*N+N) */

		    i__2 = *m;
		    i__3 = ldwrku;
		    for (i__ = 1; i__3 < 0 ? i__ >= i__2 : i__ <= i__2; i__ +=
			     i__3) {
/* Computing MIN */
			i__4 = *m - i__ + 1;
			chunk = std::min(i__4,ldwrku);
			dgemm_("N", "N", &chunk, n, n, &c_b443, &a[i__ +
				a_dim1], lda, &work[ir], &ldwrkr, &c_b421, &
				work[iu], &ldwrku);
			dlacpy_("F", &chunk, n, &work[iu], &ldwrku, &a[i__ +
				a_dim1], lda);
/* L10: */
		    }

		} else {

/*                 Insufficient workspace for a fast algorithm */

		    ie = 1;
		    itauq = ie + *n;
		    itaup = itauq + *n;
		    iwork = itaup + *n;

/*                 Bidiagonalize A */
/*                 (Workspace: need 3*N+M, prefer 3*N+(M+N)*NB) */

		    i__3 = *lwork - iwork + 1;
		    dgebrd_(m, n, &a[a_offset], lda, &s[1], &work[ie], &work[
			    itauq], &work[itaup], &work[iwork], &i__3, &ierr);

/*                 Generate left vectors bidiagonalizing A */
/*                 (Workspace: need 4*N, prefer 3*N+N*NB) */

		    i__3 = *lwork - iwork + 1;
		    dorgbr_("Q", m, n, n, &a[a_offset], lda, &work[itauq], &
			    work[iwork], &i__3, &ierr);
		    iwork = ie + *n;

/*                 Perform bidiagonal QR iteration, computing left */
/*                 singular vectors of A in A */
/*                 (Workspace: need BDSPAC) */

		    dbdsqr_("U", n, &c__0, m, &c__0, &s[1], &work[ie], dum, &
			    c__1, &a[a_offset], lda, dum, &c__1, &work[iwork],
			     info);

		}

	    } else if (wntuo && wntvas) {

/*              Path 3 (M much larger than N, JOBU='O', JOBVT='S' or 'A') */
/*              N left singular vectors to be overwritten on A and */
/*              N right singular vectors to be computed in VT */

/* Computing MAX */
		i__3 = *n << 2;
		if (*lwork >= *n * *n + std::max(i__3,bdspac)) {

/*                 Sufficient workspace for a fast algorithm */

		    ir = 1;
/* Computing MAX */
		    i__3 = wrkbl, i__2 = *lda * *n + *n;
		    if (*lwork >= std::max(i__3,i__2) + *lda * *n) {

/*                    WORK(IU) is LDA by N and WORK(IR) is LDA by N */

			ldwrku = *lda;
			ldwrkr = *lda;
		    } else /* if(complicated condition) */ {
/* Computing MAX */
			i__3 = wrkbl, i__2 = *lda * *n + *n;
			if (*lwork >= std::max(i__3,i__2) + *n * *n) {

/*                    WORK(IU) is LDA by N and WORK(IR) is N by N */

			    ldwrku = *lda;
			    ldwrkr = *n;
			} else {

/*                    WORK(IU) is LDWRKU by N and WORK(IR) is N by N */

			    ldwrku = (*lwork - *n * *n - *n) / *n;
			    ldwrkr = *n;
			}
		    }
		    itau = ir + ldwrkr * *n;
		    iwork = itau + *n;

/*                 Compute A=Q*R */
/*                 (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		    i__3 = *lwork - iwork + 1;
		    dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
, &i__3, &ierr);

/*                 Copy R to VT, zeroing out below it */

		    dlacpy_("U", n, n, &a[a_offset], lda, &vt[vt_offset],
			    ldvt);
		    if (*n > 1) {
			i__3 = *n - 1;
			i__2 = *n - 1;
			dlaset_("L", &i__3, &i__2, &c_b421, &c_b421, &vt[
				vt_dim1 + 2], ldvt);
		    }

/*                 Generate Q in A */
/*                 (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

		    i__3 = *lwork - iwork + 1;
		    dorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &work[
			    iwork], &i__3, &ierr);
		    ie = itau;
		    itauq = ie + *n;
		    itaup = itauq + *n;
		    iwork = itaup + *n;

/*                 Bidiagonalize R in VT, copying result to WORK(IR) */
/*                 (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

		    i__3 = *lwork - iwork + 1;
		    dgebrd_(n, n, &vt[vt_offset], ldvt, &s[1], &work[ie], &
			    work[itauq], &work[itaup], &work[iwork], &i__3, &
			    ierr);
		    dlacpy_("L", n, n, &vt[vt_offset], ldvt, &work[ir], &
			    ldwrkr);

/*                 Generate left vectors bidiagonalizing R in WORK(IR) */
/*                 (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

		    i__3 = *lwork - iwork + 1;
		    dorgbr_("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq], &
			    work[iwork], &i__3, &ierr);

/*                 Generate right vectors bidiagonalizing R in VT */
/*                 (Workspace: need N*N+4*N-1, prefer N*N+3*N+(N-1)*NB) */

		    i__3 = *lwork - iwork + 1;
		    dorgbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup],
			    &work[iwork], &i__3, &ierr);
		    iwork = ie + *n;

/*                 Perform bidiagonal QR iteration, computing left */
/*                 singular vectors of R in WORK(IR) and computing right */
/*                 singular vectors of R in VT */
/*                 (Workspace: need N*N+BDSPAC) */

		    dbdsqr_("U", n, n, n, &c__0, &s[1], &work[ie], &vt[
			    vt_offset], ldvt, &work[ir], &ldwrkr, dum, &c__1,
			    &work[iwork], info);
		    iu = ie + *n;

/*                 Multiply Q in A by left singular vectors of R in */
/*                 WORK(IR), storing result in WORK(IU) and copying to A */
/*                 (Workspace: need N*N+2*N, prefer N*N+M*N+N) */

		    i__3 = *m;
		    i__2 = ldwrku;
		    for (i__ = 1; i__2 < 0 ? i__ >= i__3 : i__ <= i__3; i__ +=
			     i__2) {
/* Computing MIN */
			i__4 = *m - i__ + 1;
			chunk = std::min(i__4,ldwrku);
			dgemm_("N", "N", &chunk, n, n, &c_b443, &a[i__ +
				a_dim1], lda, &work[ir], &ldwrkr, &c_b421, &
				work[iu], &ldwrku);
			dlacpy_("F", &chunk, n, &work[iu], &ldwrku, &a[i__ +
				a_dim1], lda);
/* L20: */
		    }

		} else {

/*                 Insufficient workspace for a fast algorithm */

		    itau = 1;
		    iwork = itau + *n;

/*                 Compute A=Q*R */
/*                 (Workspace: need 2*N, prefer N+N*NB) */

		    i__2 = *lwork - iwork + 1;
		    dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
, &i__2, &ierr);

/*                 Copy R to VT, zeroing out below it */

		    dlacpy_("U", n, n, &a[a_offset], lda, &vt[vt_offset],
			    ldvt);
		    if (*n > 1) {
			i__2 = *n - 1;
			i__3 = *n - 1;
			dlaset_("L", &i__2, &i__3, &c_b421, &c_b421, &vt[
				vt_dim1 + 2], ldvt);
		    }

/*                 Generate Q in A */
/*                 (Workspace: need 2*N, prefer N+N*NB) */

		    i__2 = *lwork - iwork + 1;
		    dorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &work[
			    iwork], &i__2, &ierr);
		    ie = itau;
		    itauq = ie + *n;
		    itaup = itauq + *n;
		    iwork = itaup + *n;

/*                 Bidiagonalize R in VT */
/*                 (Workspace: need 4*N, prefer 3*N+2*N*NB) */

		    i__2 = *lwork - iwork + 1;
		    dgebrd_(n, n, &vt[vt_offset], ldvt, &s[1], &work[ie], &
			    work[itauq], &work[itaup], &work[iwork], &i__2, &
			    ierr);

/*                 Multiply Q in A by left vectors bidiagonalizing R */
/*                 (Workspace: need 3*N+M, prefer 3*N+M*NB) */

		    i__2 = *lwork - iwork + 1;
		    dormbr_("Q", "R", "N", m, n, n, &vt[vt_offset], ldvt, &
			    work[itauq], &a[a_offset], lda, &work[iwork], &
			    i__2, &ierr);

/*                 Generate right vectors bidiagonalizing R in VT */
/*                 (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

		    i__2 = *lwork - iwork + 1;
		    dorgbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup],
			    &work[iwork], &i__2, &ierr);
		    iwork = ie + *n;

/*                 Perform bidiagonal QR iteration, computing left */
/*                 singular vectors of A in A and computing right */
/*                 singular vectors of A in VT */
/*                 (Workspace: need BDSPAC) */

		    dbdsqr_("U", n, n, m, &c__0, &s[1], &work[ie], &vt[
			    vt_offset], ldvt, &a[a_offset], lda, dum, &c__1, &
			    work[iwork], info);

		}

	    } else if (wntus) {

		if (wntvn) {

/*                 Path 4 (M much larger than N, JOBU='S', JOBVT='N') */
/*                 N left singular vectors to be computed in U and */
/*                 no right singular vectors to be computed */

/* Computing MAX */
		    i__2 = *n << 2;
		    if (*lwork >= *n * *n + std::max(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			ir = 1;
			if (*lwork >= wrkbl + *lda * *n) {

/*                       WORK(IR) is LDA by N */

			    ldwrkr = *lda;
			} else {

/*                       WORK(IR) is N by N */

			    ldwrkr = *n;
			}
			itau = ir + ldwrkr * *n;
			iwork = itau + *n;

/*                    Compute A=Q*R */
/*                    (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);

/*                    Copy R to WORK(IR), zeroing out below it */

			dlacpy_("U", n, n, &a[a_offset], lda, &work[ir], &
				ldwrkr);
			i__2 = *n - 1;
			i__3 = *n - 1;
			dlaset_("L", &i__2, &i__3, &c_b421, &c_b421, &work[ir
				+ 1], &ldwrkr);

/*                    Generate Q in A */
/*                    (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IR) */
/*                    (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(n, n, &work[ir], &ldwrkr, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Generate left vectors bidiagonalizing R in WORK(IR) */
/*                    (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq]
, &work[iwork], &i__2, &ierr);
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of R in WORK(IR) */
/*                    (Workspace: need N*N+BDSPAC) */

			dbdsqr_("U", n, &c__0, n, &c__0, &s[1], &work[ie],
				dum, &c__1, &work[ir], &ldwrkr, dum, &c__1, &
				work[iwork], info);

/*                    Multiply Q in A by left singular vectors of R in */
/*                    WORK(IR), storing result in U */
/*                    (Workspace: need N*N) */

			dgemm_("N", "N", m, n, n, &c_b443, &a[a_offset], lda,
				&work[ir], &ldwrkr, &c_b421, &u[u_offset],
				ldu);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset],
				ldu);

/*                    Generate Q in U */
/*                    (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorgqr_(m, n, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Zero out below R in A */

			i__2 = *n - 1;
			i__3 = *n - 1;
			dlaset_("L", &i__2, &i__3, &c_b421, &c_b421, &a[
				a_dim1 + 2], lda);

/*                    Bidiagonalize R in A */
/*                    (Workspace: need 4*N, prefer 3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(n, n, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply Q in U by left vectors bidiagonalizing R */
/*                    (Workspace: need 3*N+M, prefer 3*N+M*NB) */

			i__2 = *lwork - iwork + 1;
			dormbr_("Q", "R", "N", m, n, n, &a[a_offset], lda, &
				work[itauq], &u[u_offset], ldu, &work[iwork],
				&i__2, &ierr)
				;
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U */
/*                    (Workspace: need BDSPAC) */

			dbdsqr_("U", n, &c__0, m, &c__0, &s[1], &work[ie],
				dum, &c__1, &u[u_offset], ldu, dum, &c__1, &
				work[iwork], info);

		    }

		} else if (wntvo) {

/*                 Path 5 (M much larger than N, JOBU='S', JOBVT='O') */
/*                 N left singular vectors to be computed in U and */
/*                 N right singular vectors to be overwritten on A */

/* Computing MAX */
		    i__2 = *n << 2;
		    if (*lwork >= (*n << 1) * *n + std::max(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + (*lda << 1) * *n) {

/*                       WORK(IU) is LDA by N and WORK(IR) is LDA by N */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *n;
			    ldwrkr = *lda;
			} else if (*lwork >= wrkbl + (*lda + *n) * *n) {

/*                       WORK(IU) is LDA by N and WORK(IR) is N by N */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *n;
			    ldwrkr = *n;
			} else {

/*                       WORK(IU) is N by N and WORK(IR) is N by N */

			    ldwrku = *n;
			    ir = iu + ldwrku * *n;
			    ldwrkr = *n;
			}
			itau = ir + ldwrkr * *n;
			iwork = itau + *n;

/*                    Compute A=Q*R */
/*                    (Workspace: need 2*N*N+2*N, prefer 2*N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);

/*                    Copy R to WORK(IU), zeroing out below it */

			dlacpy_("U", n, n, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *n - 1;
			i__3 = *n - 1;
			dlaset_("L", &i__2, &i__3, &c_b421, &c_b421, &work[iu
				+ 1], &ldwrku);

/*                    Generate Q in A */
/*                    (Workspace: need 2*N*N+2*N, prefer 2*N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IU), copying result to */
/*                    WORK(IR) */
/*                    (Workspace: need 2*N*N+4*N, */
/*                                prefer 2*N*N+3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(n, n, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			dlacpy_("U", n, n, &work[iu], &ldwrku, &work[ir], &
				ldwrkr);

/*                    Generate left bidiagonalizing vectors in WORK(IU) */
/*                    (Workspace: need 2*N*N+4*N, prefer 2*N*N+3*N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("Q", n, n, n, &work[iu], &ldwrku, &work[itauq]
, &work[iwork], &i__2, &ierr);

/*                    Generate right bidiagonalizing vectors in WORK(IR) */
/*                    (Workspace: need 2*N*N+4*N-1, */
/*                                prefer 2*N*N+3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("P", n, n, n, &work[ir], &ldwrkr, &work[itaup]
, &work[iwork], &i__2, &ierr);
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of R in WORK(IU) and computing */
/*                    right singular vectors of R in WORK(IR) */
/*                    (Workspace: need 2*N*N+BDSPAC) */

			dbdsqr_("U", n, n, n, &c__0, &s[1], &work[ie], &work[
				ir], &ldwrkr, &work[iu], &ldwrku, dum, &c__1,
				&work[iwork], info);

/*                    Multiply Q in A by left singular vectors of R in */
/*                    WORK(IU), storing result in U */
/*                    (Workspace: need N*N) */

			dgemm_("N", "N", m, n, n, &c_b443, &a[a_offset], lda,
				&work[iu], &ldwrku, &c_b421, &u[u_offset],
				ldu);

/*                    Copy right singular vectors of R to A */
/*                    (Workspace: need N*N) */

			dlacpy_("F", n, n, &work[ir], &ldwrkr, &a[a_offset],
				lda);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset],
				ldu);

/*                    Generate Q in U */
/*                    (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorgqr_(m, n, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Zero out below R in A */

			i__2 = *n - 1;
			i__3 = *n - 1;
			dlaset_("L", &i__2, &i__3, &c_b421, &c_b421, &a[
				a_dim1 + 2], lda);

/*                    Bidiagonalize R in A */
/*                    (Workspace: need 4*N, prefer 3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(n, n, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply Q in U by left vectors bidiagonalizing R */
/*                    (Workspace: need 3*N+M, prefer 3*N+M*NB) */

			i__2 = *lwork - iwork + 1;
			dormbr_("Q", "R", "N", m, n, n, &a[a_offset], lda, &
				work[itauq], &u[u_offset], ldu, &work[iwork],
				&i__2, &ierr)
				;

/*                    Generate right vectors bidiagonalizing R in A */
/*                    (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("P", n, n, n, &a[a_offset], lda, &work[itaup],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U and computing right */
/*                    singular vectors of A in A */
/*                    (Workspace: need BDSPAC) */

			dbdsqr_("U", n, n, m, &c__0, &s[1], &work[ie], &a[
				a_offset], lda, &u[u_offset], ldu, dum, &c__1,
				 &work[iwork], info);

		    }

		} else if (wntvas) {

/*                 Path 6 (M much larger than N, JOBU='S', JOBVT='S' */
/*                         or 'A') */
/*                 N left singular vectors to be computed in U and */
/*                 N right singular vectors to be computed in VT */

/* Computing MAX */
		    i__2 = *n << 2;
		    if (*lwork >= *n * *n + std::max(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + *lda * *n) {

/*                       WORK(IU) is LDA by N */

			    ldwrku = *lda;
			} else {

/*                       WORK(IU) is N by N */

			    ldwrku = *n;
			}
			itau = iu + ldwrku * *n;
			iwork = itau + *n;

/*                    Compute A=Q*R */
/*                    (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);

/*                    Copy R to WORK(IU), zeroing out below it */

			dlacpy_("U", n, n, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *n - 1;
			i__3 = *n - 1;
			dlaset_("L", &i__2, &i__3, &c_b421, &c_b421, &work[iu
				+ 1], &ldwrku);

/*                    Generate Q in A */
/*                    (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorgqr_(m, n, n, &a[a_offset], lda, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IU), copying result to VT */
/*                    (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(n, n, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			dlacpy_("U", n, n, &work[iu], &ldwrku, &vt[vt_offset],
				 ldvt);

/*                    Generate left bidiagonalizing vectors in WORK(IU) */
/*                    (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("Q", n, n, n, &work[iu], &ldwrku, &work[itauq]
, &work[iwork], &i__2, &ierr);

/*                    Generate right bidiagonalizing vectors in VT */
/*                    (Workspace: need N*N+4*N-1, */
/*                                prefer N*N+3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[
				itaup], &work[iwork], &i__2, &ierr)
				;
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of R in WORK(IU) and computing */
/*                    right singular vectors of R in VT */
/*                    (Workspace: need N*N+BDSPAC) */

			dbdsqr_("U", n, n, n, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &work[iu], &ldwrku, dum, &
				c__1, &work[iwork], info);

/*                    Multiply Q in A by left singular vectors of R in */
/*                    WORK(IU), storing result in U */
/*                    (Workspace: need N*N) */

			dgemm_("N", "N", m, n, n, &c_b443, &a[a_offset], lda,
				&work[iu], &ldwrku, &c_b421, &u[u_offset],
				ldu);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset],
				ldu);

/*                    Generate Q in U */
/*                    (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorgqr_(m, n, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy R to VT, zeroing out below it */

			dlacpy_("U", n, n, &a[a_offset], lda, &vt[vt_offset],
				ldvt);
			if (*n > 1) {
			    i__2 = *n - 1;
			    i__3 = *n - 1;
			    dlaset_("L", &i__2, &i__3, &c_b421, &c_b421, &vt[
				    vt_dim1 + 2], ldvt);
			}
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in VT */
/*                    (Workspace: need 4*N, prefer 3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(n, n, &vt[vt_offset], ldvt, &s[1], &work[ie],
				&work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply Q in U by left bidiagonalizing vectors */
/*                    in VT */
/*                    (Workspace: need 3*N+M, prefer 3*N+M*NB) */

			i__2 = *lwork - iwork + 1;
			dormbr_("Q", "R", "N", m, n, n, &vt[vt_offset], ldvt,
				&work[itauq], &u[u_offset], ldu, &work[iwork],
				 &i__2, &ierr);

/*                    Generate right bidiagonalizing vectors in VT */
/*                    (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[
				itaup], &work[iwork], &i__2, &ierr)
				;
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U and computing right */
/*                    singular vectors of A in VT */
/*                    (Workspace: need BDSPAC) */

			dbdsqr_("U", n, n, m, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &u[u_offset], ldu, dum, &
				c__1, &work[iwork], info);

		    }

		}

	    } else if (wntua) {

		if (wntvn) {

/*                 Path 7 (M much larger than N, JOBU='A', JOBVT='N') */
/*                 M left singular vectors to be computed in U and */
/*                 no right singular vectors to be computed */

/* Computing MAX */
		    i__2 = *n + *m, i__3 = *n << 2, i__2 = std::max(i__2,i__3);
		    if (*lwork >= *n * *n + std::max(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			ir = 1;
			if (*lwork >= wrkbl + *lda * *n) {

/*                       WORK(IR) is LDA by N */

			    ldwrkr = *lda;
			} else {

/*                       WORK(IR) is N by N */

			    ldwrkr = *n;
			}
			itau = ir + ldwrkr * *n;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset],
				ldu);

/*                    Copy R to WORK(IR), zeroing out below it */

			dlacpy_("U", n, n, &a[a_offset], lda, &work[ir], &
				ldwrkr);
			i__2 = *n - 1;
			i__3 = *n - 1;
			dlaset_("L", &i__2, &i__3, &c_b421, &c_b421, &work[ir
				+ 1], &ldwrkr);

/*                    Generate Q in U */
/*                    (Workspace: need N*N+N+M, prefer N*N+N+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorgqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IR) */
/*                    (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(n, n, &work[ir], &ldwrkr, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in WORK(IR) */
/*                    (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("Q", n, n, n, &work[ir], &ldwrkr, &work[itauq]
, &work[iwork], &i__2, &ierr);
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of R in WORK(IR) */
/*                    (Workspace: need N*N+BDSPAC) */

			dbdsqr_("U", n, &c__0, n, &c__0, &s[1], &work[ie],
				dum, &c__1, &work[ir], &ldwrkr, dum, &c__1, &
				work[iwork], info);

/*                    Multiply Q in U by left singular vectors of R in */
/*                    WORK(IR), storing result in A */
/*                    (Workspace: need N*N) */

			dgemm_("N", "N", m, n, n, &c_b443, &u[u_offset], ldu,
				&work[ir], &ldwrkr, &c_b421, &a[a_offset],
				lda);

/*                    Copy left singular vectors of A from A to U */

			dlacpy_("F", m, n, &a[a_offset], lda, &u[u_offset],
				ldu);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset],
				ldu);

/*                    Generate Q in U */
/*                    (Workspace: need N+M, prefer N+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorgqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Zero out below R in A */

			i__2 = *n - 1;
			i__3 = *n - 1;
			dlaset_("L", &i__2, &i__3, &c_b421, &c_b421, &a[
				a_dim1 + 2], lda);

/*                    Bidiagonalize R in A */
/*                    (Workspace: need 4*N, prefer 3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(n, n, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply Q in U by left bidiagonalizing vectors */
/*                    in A */
/*                    (Workspace: need 3*N+M, prefer 3*N+M*NB) */

			i__2 = *lwork - iwork + 1;
			dormbr_("Q", "R", "N", m, n, n, &a[a_offset], lda, &
				work[itauq], &u[u_offset], ldu, &work[iwork],
				&i__2, &ierr)
				;
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U */
/*                    (Workspace: need BDSPAC) */

			dbdsqr_("U", n, &c__0, m, &c__0, &s[1], &work[ie],
				dum, &c__1, &u[u_offset], ldu, dum, &c__1, &
				work[iwork], info);

		    }

		} else if (wntvo) {

/*                 Path 8 (M much larger than N, JOBU='A', JOBVT='O') */
/*                 M left singular vectors to be computed in U and */
/*                 N right singular vectors to be overwritten on A */

/* Computing MAX */
		    i__2 = *n + *m, i__3 = *n << 2, i__2 = std::max(i__2,i__3);
		    if (*lwork >= (*n << 1) * *n + std::max(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + (*lda << 1) * *n) {

/*                       WORK(IU) is LDA by N and WORK(IR) is LDA by N */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *n;
			    ldwrkr = *lda;
			} else if (*lwork >= wrkbl + (*lda + *n) * *n) {

/*                       WORK(IU) is LDA by N and WORK(IR) is N by N */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *n;
			    ldwrkr = *n;
			} else {

/*                       WORK(IU) is N by N and WORK(IR) is N by N */

			    ldwrku = *n;
			    ir = iu + ldwrku * *n;
			    ldwrkr = *n;
			}
			itau = ir + ldwrkr * *n;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (Workspace: need 2*N*N+2*N, prefer 2*N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset],
				ldu);

/*                    Generate Q in U */
/*                    (Workspace: need 2*N*N+N+M, prefer 2*N*N+N+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorgqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy R to WORK(IU), zeroing out below it */

			dlacpy_("U", n, n, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *n - 1;
			i__3 = *n - 1;
			dlaset_("L", &i__2, &i__3, &c_b421, &c_b421, &work[iu
				+ 1], &ldwrku);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IU), copying result to */
/*                    WORK(IR) */
/*                    (Workspace: need 2*N*N+4*N, */
/*                                prefer 2*N*N+3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(n, n, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			dlacpy_("U", n, n, &work[iu], &ldwrku, &work[ir], &
				ldwrkr);

/*                    Generate left bidiagonalizing vectors in WORK(IU) */
/*                    (Workspace: need 2*N*N+4*N, prefer 2*N*N+3*N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("Q", n, n, n, &work[iu], &ldwrku, &work[itauq]
, &work[iwork], &i__2, &ierr);

/*                    Generate right bidiagonalizing vectors in WORK(IR) */
/*                    (Workspace: need 2*N*N+4*N-1, */
/*                                prefer 2*N*N+3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("P", n, n, n, &work[ir], &ldwrkr, &work[itaup]
, &work[iwork], &i__2, &ierr);
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of R in WORK(IU) and computing */
/*                    right singular vectors of R in WORK(IR) */
/*                    (Workspace: need 2*N*N+BDSPAC) */

			dbdsqr_("U", n, n, n, &c__0, &s[1], &work[ie], &work[
				ir], &ldwrkr, &work[iu], &ldwrku, dum, &c__1,
				&work[iwork], info);

/*                    Multiply Q in U by left singular vectors of R in */
/*                    WORK(IU), storing result in A */
/*                    (Workspace: need N*N) */

			dgemm_("N", "N", m, n, n, &c_b443, &u[u_offset], ldu,
				&work[iu], &ldwrku, &c_b421, &a[a_offset],
				lda);

/*                    Copy left singular vectors of A from A to U */

			dlacpy_("F", m, n, &a[a_offset], lda, &u[u_offset],
				ldu);

/*                    Copy right singular vectors of R from WORK(IR) to A */

			dlacpy_("F", n, n, &work[ir], &ldwrkr, &a[a_offset],
				lda);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset],
				ldu);

/*                    Generate Q in U */
/*                    (Workspace: need N+M, prefer N+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorgqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Zero out below R in A */

			i__2 = *n - 1;
			i__3 = *n - 1;
			dlaset_("L", &i__2, &i__3, &c_b421, &c_b421, &a[
				a_dim1 + 2], lda);

/*                    Bidiagonalize R in A */
/*                    (Workspace: need 4*N, prefer 3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(n, n, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply Q in U by left bidiagonalizing vectors */
/*                    in A */
/*                    (Workspace: need 3*N+M, prefer 3*N+M*NB) */

			i__2 = *lwork - iwork + 1;
			dormbr_("Q", "R", "N", m, n, n, &a[a_offset], lda, &
				work[itauq], &u[u_offset], ldu, &work[iwork],
				&i__2, &ierr)
				;

/*                    Generate right bidiagonalizing vectors in A */
/*                    (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("P", n, n, n, &a[a_offset], lda, &work[itaup],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U and computing right */
/*                    singular vectors of A in A */
/*                    (Workspace: need BDSPAC) */

			dbdsqr_("U", n, n, m, &c__0, &s[1], &work[ie], &a[
				a_offset], lda, &u[u_offset], ldu, dum, &c__1,
				 &work[iwork], info);

		    }

		} else if (wntvas) {

/*                 Path 9 (M much larger than N, JOBU='A', JOBVT='S' */
/*                         or 'A') */
/*                 M left singular vectors to be computed in U and */
/*                 N right singular vectors to be computed in VT */

/* Computing MAX */
		    i__2 = *n + *m, i__3 = *n << 2, i__2 = std::max(i__2,i__3);
		    if (*lwork >= *n * *n + std::max(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + *lda * *n) {

/*                       WORK(IU) is LDA by N */

			    ldwrku = *lda;
			} else {

/*                       WORK(IU) is N by N */

			    ldwrku = *n;
			}
			itau = iu + ldwrku * *n;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (Workspace: need N*N+2*N, prefer N*N+N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset],
				ldu);

/*                    Generate Q in U */
/*                    (Workspace: need N*N+N+M, prefer N*N+N+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorgqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy R to WORK(IU), zeroing out below it */

			dlacpy_("U", n, n, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *n - 1;
			i__3 = *n - 1;
			dlaset_("L", &i__2, &i__3, &c_b421, &c_b421, &work[iu
				+ 1], &ldwrku);
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in WORK(IU), copying result to VT */
/*                    (Workspace: need N*N+4*N, prefer N*N+3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(n, n, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			dlacpy_("U", n, n, &work[iu], &ldwrku, &vt[vt_offset],
				 ldvt);

/*                    Generate left bidiagonalizing vectors in WORK(IU) */
/*                    (Workspace: need N*N+4*N, prefer N*N+3*N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("Q", n, n, n, &work[iu], &ldwrku, &work[itauq]
, &work[iwork], &i__2, &ierr);

/*                    Generate right bidiagonalizing vectors in VT */
/*                    (Workspace: need N*N+4*N-1, */
/*                                prefer N*N+3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[
				itaup], &work[iwork], &i__2, &ierr)
				;
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of R in WORK(IU) and computing */
/*                    right singular vectors of R in VT */
/*                    (Workspace: need N*N+BDSPAC) */

			dbdsqr_("U", n, n, n, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &work[iu], &ldwrku, dum, &
				c__1, &work[iwork], info);

/*                    Multiply Q in U by left singular vectors of R in */
/*                    WORK(IU), storing result in A */
/*                    (Workspace: need N*N) */

			dgemm_("N", "N", m, n, n, &c_b443, &u[u_offset], ldu,
				&work[iu], &ldwrku, &c_b421, &a[a_offset],
				lda);

/*                    Copy left singular vectors of A from A to U */

			dlacpy_("F", m, n, &a[a_offset], lda, &u[u_offset],
				ldu);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *n;

/*                    Compute A=Q*R, copying result to U */
/*                    (Workspace: need 2*N, prefer N+N*NB) */

			i__2 = *lwork - iwork + 1;
			dgeqrf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset],
				ldu);

/*                    Generate Q in U */
/*                    (Workspace: need N+M, prefer N+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorgqr_(m, m, n, &u[u_offset], ldu, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy R from A to VT, zeroing out below it */

			dlacpy_("U", n, n, &a[a_offset], lda, &vt[vt_offset],
				ldvt);
			if (*n > 1) {
			    i__2 = *n - 1;
			    i__3 = *n - 1;
			    dlaset_("L", &i__2, &i__3, &c_b421, &c_b421, &vt[
				    vt_dim1 + 2], ldvt);
			}
			ie = itau;
			itauq = ie + *n;
			itaup = itauq + *n;
			iwork = itaup + *n;

/*                    Bidiagonalize R in VT */
/*                    (Workspace: need 4*N, prefer 3*N+2*N*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(n, n, &vt[vt_offset], ldvt, &s[1], &work[ie],
				&work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply Q in U by left bidiagonalizing vectors */
/*                    in VT */
/*                    (Workspace: need 3*N+M, prefer 3*N+M*NB) */

			i__2 = *lwork - iwork + 1;
			dormbr_("Q", "R", "N", m, n, n, &vt[vt_offset], ldvt,
				&work[itauq], &u[u_offset], ldu, &work[iwork],
				 &i__2, &ierr);

/*                    Generate right bidiagonalizing vectors in VT */
/*                    (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[
				itaup], &work[iwork], &i__2, &ierr)
				;
			iwork = ie + *n;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U and computing right */
/*                    singular vectors of A in VT */
/*                    (Workspace: need BDSPAC) */

			dbdsqr_("U", n, n, m, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &u[u_offset], ldu, dum, &
				c__1, &work[iwork], info);

		    }

		}

	    }

	} else {

/*           M .LT. MNTHR */

/*           Path 10 (M at least N, but not much larger) */
/*           Reduce to bidiagonal form without QR decomposition */

	    ie = 1;
	    itauq = ie + *n;
	    itaup = itauq + *n;
	    iwork = itaup + *n;

/*           Bidiagonalize A */
/*           (Workspace: need 3*N+M, prefer 3*N+(M+N)*NB) */

	    i__2 = *lwork - iwork + 1;
	    dgebrd_(m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &
		    work[itaup], &work[iwork], &i__2, &ierr);
	    if (wntuas) {

/*              If left singular vectors desired in U, copy result to U */
/*              and generate left bidiagonalizing vectors in U */
/*              (Workspace: need 3*N+NCU, prefer 3*N+NCU*NB) */

		dlacpy_("L", m, n, &a[a_offset], lda, &u[u_offset], ldu);
		if (wntus) {
		    ncu = *n;
		}
		if (wntua) {
		    ncu = *m;
		}
		i__2 = *lwork - iwork + 1;
		dorgbr_("Q", m, &ncu, n, &u[u_offset], ldu, &work[itauq], &
			work[iwork], &i__2, &ierr);
	    }
	    if (wntvas) {

/*              If right singular vectors desired in VT, copy result to */
/*              VT and generate right bidiagonalizing vectors in VT */
/*              (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

		dlacpy_("U", n, n, &a[a_offset], lda, &vt[vt_offset], ldvt);
		i__2 = *lwork - iwork + 1;
		dorgbr_("P", n, n, n, &vt[vt_offset], ldvt, &work[itaup], &
			work[iwork], &i__2, &ierr);
	    }
	    if (wntuo) {

/*              If left singular vectors desired in A, generate left */
/*              bidiagonalizing vectors in A */
/*              (Workspace: need 4*N, prefer 3*N+N*NB) */

		i__2 = *lwork - iwork + 1;
		dorgbr_("Q", m, n, n, &a[a_offset], lda, &work[itauq], &work[
			iwork], &i__2, &ierr);
	    }
	    if (wntvo) {

/*              If right singular vectors desired in A, generate right */
/*              bidiagonalizing vectors in A */
/*              (Workspace: need 4*N-1, prefer 3*N+(N-1)*NB) */

		i__2 = *lwork - iwork + 1;
		dorgbr_("P", n, n, n, &a[a_offset], lda, &work[itaup], &work[
			iwork], &i__2, &ierr);
	    }
	    iwork = ie + *n;
	    if (wntuas || wntuo) {
		nru = *m;
	    }
	    if (wntun) {
		nru = 0;
	    }
	    if (wntvas || wntvo) {
		ncvt = *n;
	    }
	    if (wntvn) {
		ncvt = 0;
	    }
	    if (! wntuo && ! wntvo) {

/*              Perform bidiagonal QR iteration, if desired, computing */
/*              left singular vectors in U and computing right singular */
/*              vectors in VT */
/*              (Workspace: need BDSPAC) */

		dbdsqr_("U", n, &ncvt, &nru, &c__0, &s[1], &work[ie], &vt[
			vt_offset], ldvt, &u[u_offset], ldu, dum, &c__1, &
			work[iwork], info);
	    } else if (! wntuo && wntvo) {

/*              Perform bidiagonal QR iteration, if desired, computing */
/*              left singular vectors in U and computing right singular */
/*              vectors in A */
/*              (Workspace: need BDSPAC) */

		dbdsqr_("U", n, &ncvt, &nru, &c__0, &s[1], &work[ie], &a[
			a_offset], lda, &u[u_offset], ldu, dum, &c__1, &work[
			iwork], info);
	    } else {

/*              Perform bidiagonal QR iteration, if desired, computing */
/*              left singular vectors in A and computing right singular */
/*              vectors in VT */
/*              (Workspace: need BDSPAC) */

		dbdsqr_("U", n, &ncvt, &nru, &c__0, &s[1], &work[ie], &vt[
			vt_offset], ldvt, &a[a_offset], lda, dum, &c__1, &
			work[iwork], info);
	    }

	}

    } else {

/*        A has more columns than rows. If A has sufficiently more */
/*        columns than rows, first reduce using the LQ decomposition (if */
/*        sufficient workspace available) */

	if (*n >= mnthr) {

	    if (wntvn) {

/*              Path 1t(N much larger than M, JOBVT='N') */
/*              No right singular vectors to be computed */

		itau = 1;
		iwork = itau + *m;

/*              Compute A=L*Q */
/*              (Workspace: need 2*M, prefer M+M*NB) */

		i__2 = *lwork - iwork + 1;
		dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork], &
			i__2, &ierr);

/*              Zero out above L */

		i__2 = *m - 1;
		i__3 = *m - 1;
		dlaset_("U", &i__2, &i__3, &c_b421, &c_b421, &a[(a_dim1 << 1)
			+ 1], lda);
		ie = 1;
		itauq = ie + *m;
		itaup = itauq + *m;
		iwork = itaup + *m;

/*              Bidiagonalize L in A */
/*              (Workspace: need 4*M, prefer 3*M+2*M*NB) */

		i__2 = *lwork - iwork + 1;
		dgebrd_(m, m, &a[a_offset], lda, &s[1], &work[ie], &work[
			itauq], &work[itaup], &work[iwork], &i__2, &ierr);
		if (wntuo || wntuas) {

/*                 If left singular vectors desired, generate Q */
/*                 (Workspace: need 4*M, prefer 3*M+M*NB) */

		    i__2 = *lwork - iwork + 1;
		    dorgbr_("Q", m, m, m, &a[a_offset], lda, &work[itauq], &
			    work[iwork], &i__2, &ierr);
		}
		iwork = ie + *m;
		nru = 0;
		if (wntuo || wntuas) {
		    nru = *m;
		}

/*              Perform bidiagonal QR iteration, computing left singular */
/*              vectors of A in A if desired */
/*              (Workspace: need BDSPAC) */

		dbdsqr_("U", m, &c__0, &nru, &c__0, &s[1], &work[ie], dum, &
			c__1, &a[a_offset], lda, dum, &c__1, &work[iwork],
			info);

/*              If left singular vectors desired in U, copy them there */

		if (wntuas) {
		    dlacpy_("F", m, m, &a[a_offset], lda, &u[u_offset], ldu);
		}

	    } else if (wntvo && wntun) {

/*              Path 2t(N much larger than M, JOBU='N', JOBVT='O') */
/*              M right singular vectors to be overwritten on A and */
/*              no left singular vectors to be computed */

/* Computing MAX */
		i__2 = *m << 2;
		if (*lwork >= *m * *m + std::max(i__2,bdspac)) {

/*                 Sufficient workspace for a fast algorithm */

		    ir = 1;
/* Computing MAX */
		    i__2 = wrkbl, i__3 = *lda * *n + *m;
		    if (*lwork >= std::max(i__2,i__3) + *lda * *m) {

/*                    WORK(IU) is LDA by N and WORK(IR) is LDA by M */

			ldwrku = *lda;
			chunk = *n;
			ldwrkr = *lda;
		    } else /* if(complicated condition) */ {
/* Computing MAX */
			i__2 = wrkbl, i__3 = *lda * *n + *m;
			if (*lwork >= std::max(i__2,i__3) + *m * *m) {

/*                    WORK(IU) is LDA by N and WORK(IR) is M by M */

			    ldwrku = *lda;
			    chunk = *n;
			    ldwrkr = *m;
			} else {

/*                    WORK(IU) is M by CHUNK and WORK(IR) is M by M */

			    ldwrku = *m;
			    chunk = (*lwork - *m * *m - *m) / *m;
			    ldwrkr = *m;
			}
		    }
		    itau = ir + ldwrkr * *m;
		    iwork = itau + *m;

/*                 Compute A=L*Q */
/*                 (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		    i__2 = *lwork - iwork + 1;
		    dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
, &i__2, &ierr);

/*                 Copy L to WORK(IR) and zero out above it */

		    dlacpy_("L", m, m, &a[a_offset], lda, &work[ir], &ldwrkr);
		    i__2 = *m - 1;
		    i__3 = *m - 1;
		    dlaset_("U", &i__2, &i__3, &c_b421, &c_b421, &work[ir +
			    ldwrkr], &ldwrkr);

/*                 Generate Q in A */
/*                 (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		    i__2 = *lwork - iwork + 1;
		    dorglq_(m, n, m, &a[a_offset], lda, &work[itau], &work[
			    iwork], &i__2, &ierr);
		    ie = itau;
		    itauq = ie + *m;
		    itaup = itauq + *m;
		    iwork = itaup + *m;

/*                 Bidiagonalize L in WORK(IR) */
/*                 (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

		    i__2 = *lwork - iwork + 1;
		    dgebrd_(m, m, &work[ir], &ldwrkr, &s[1], &work[ie], &work[
			    itauq], &work[itaup], &work[iwork], &i__2, &ierr);

/*                 Generate right vectors bidiagonalizing L */
/*                 (Workspace: need M*M+4*M-1, prefer M*M+3*M+(M-1)*NB) */

		    i__2 = *lwork - iwork + 1;
		    dorgbr_("P", m, m, m, &work[ir], &ldwrkr, &work[itaup], &
			    work[iwork], &i__2, &ierr);
		    iwork = ie + *m;

/*                 Perform bidiagonal QR iteration, computing right */
/*                 singular vectors of L in WORK(IR) */
/*                 (Workspace: need M*M+BDSPAC) */

		    dbdsqr_("U", m, m, &c__0, &c__0, &s[1], &work[ie], &work[
			    ir], &ldwrkr, dum, &c__1, dum, &c__1, &work[iwork]
, info);
		    iu = ie + *m;

/*                 Multiply right singular vectors of L in WORK(IR) by Q */
/*                 in A, storing result in WORK(IU) and copying to A */
/*                 (Workspace: need M*M+2*M, prefer M*M+M*N+M) */

		    i__2 = *n;
		    i__3 = chunk;
		    for (i__ = 1; i__3 < 0 ? i__ >= i__2 : i__ <= i__2; i__ +=
			     i__3) {
/* Computing MIN */
			i__4 = *n - i__ + 1;
			blk = std::min(i__4,chunk);
			dgemm_("N", "N", m, &blk, m, &c_b443, &work[ir], &
				ldwrkr, &a[i__ * a_dim1 + 1], lda, &c_b421, &
				work[iu], &ldwrku);
			dlacpy_("F", m, &blk, &work[iu], &ldwrku, &a[i__ *
				a_dim1 + 1], lda);
/* L30: */
		    }

		} else {

/*                 Insufficient workspace for a fast algorithm */

		    ie = 1;
		    itauq = ie + *m;
		    itaup = itauq + *m;
		    iwork = itaup + *m;

/*                 Bidiagonalize A */
/*                 (Workspace: need 3*M+N, prefer 3*M+(M+N)*NB) */

		    i__3 = *lwork - iwork + 1;
		    dgebrd_(m, n, &a[a_offset], lda, &s[1], &work[ie], &work[
			    itauq], &work[itaup], &work[iwork], &i__3, &ierr);

/*                 Generate right vectors bidiagonalizing A */
/*                 (Workspace: need 4*M, prefer 3*M+M*NB) */

		    i__3 = *lwork - iwork + 1;
		    dorgbr_("P", m, n, m, &a[a_offset], lda, &work[itaup], &
			    work[iwork], &i__3, &ierr);
		    iwork = ie + *m;

/*                 Perform bidiagonal QR iteration, computing right */
/*                 singular vectors of A in A */
/*                 (Workspace: need BDSPAC) */

		    dbdsqr_("L", m, n, &c__0, &c__0, &s[1], &work[ie], &a[
			    a_offset], lda, dum, &c__1, dum, &c__1, &work[
			    iwork], info);

		}

	    } else if (wntvo && wntuas) {

/*              Path 3t(N much larger than M, JOBU='S' or 'A', JOBVT='O') */
/*              M right singular vectors to be overwritten on A and */
/*              M left singular vectors to be computed in U */

/* Computing MAX */
		i__3 = *m << 2;
		if (*lwork >= *m * *m + std::max(i__3,bdspac)) {

/*                 Sufficient workspace for a fast algorithm */

		    ir = 1;
/* Computing MAX */
		    i__3 = wrkbl, i__2 = *lda * *n + *m;
		    if (*lwork >= std::max(i__3,i__2) + *lda * *m) {

/*                    WORK(IU) is LDA by N and WORK(IR) is LDA by M */

			ldwrku = *lda;
			chunk = *n;
			ldwrkr = *lda;
		    } else /* if(complicated condition) */ {
/* Computing MAX */
			i__3 = wrkbl, i__2 = *lda * *n + *m;
			if (*lwork >= std::max(i__3,i__2) + *m * *m) {

/*                    WORK(IU) is LDA by N and WORK(IR) is M by M */

			    ldwrku = *lda;
			    chunk = *n;
			    ldwrkr = *m;
			} else {

/*                    WORK(IU) is M by CHUNK and WORK(IR) is M by M */

			    ldwrku = *m;
			    chunk = (*lwork - *m * *m - *m) / *m;
			    ldwrkr = *m;
			}
		    }
		    itau = ir + ldwrkr * *m;
		    iwork = itau + *m;

/*                 Compute A=L*Q */
/*                 (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		    i__3 = *lwork - iwork + 1;
		    dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
, &i__3, &ierr);

/*                 Copy L to U, zeroing about above it */

		    dlacpy_("L", m, m, &a[a_offset], lda, &u[u_offset], ldu);
		    i__3 = *m - 1;
		    i__2 = *m - 1;
		    dlaset_("U", &i__3, &i__2, &c_b421, &c_b421, &u[(u_dim1 <<
			     1) + 1], ldu);

/*                 Generate Q in A */
/*                 (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

		    i__3 = *lwork - iwork + 1;
		    dorglq_(m, n, m, &a[a_offset], lda, &work[itau], &work[
			    iwork], &i__3, &ierr);
		    ie = itau;
		    itauq = ie + *m;
		    itaup = itauq + *m;
		    iwork = itaup + *m;

/*                 Bidiagonalize L in U, copying result to WORK(IR) */
/*                 (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

		    i__3 = *lwork - iwork + 1;
		    dgebrd_(m, m, &u[u_offset], ldu, &s[1], &work[ie], &work[
			    itauq], &work[itaup], &work[iwork], &i__3, &ierr);
		    dlacpy_("U", m, m, &u[u_offset], ldu, &work[ir], &ldwrkr);

/*                 Generate right vectors bidiagonalizing L in WORK(IR) */
/*                 (Workspace: need M*M+4*M-1, prefer M*M+3*M+(M-1)*NB) */

		    i__3 = *lwork - iwork + 1;
		    dorgbr_("P", m, m, m, &work[ir], &ldwrkr, &work[itaup], &
			    work[iwork], &i__3, &ierr);

/*                 Generate left vectors bidiagonalizing L in U */
/*                 (Workspace: need M*M+4*M, prefer M*M+3*M+M*NB) */

		    i__3 = *lwork - iwork + 1;
		    dorgbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq], &
			    work[iwork], &i__3, &ierr);
		    iwork = ie + *m;

/*                 Perform bidiagonal QR iteration, computing left */
/*                 singular vectors of L in U, and computing right */
/*                 singular vectors of L in WORK(IR) */
/*                 (Workspace: need M*M+BDSPAC) */

		    dbdsqr_("U", m, m, m, &c__0, &s[1], &work[ie], &work[ir],
			    &ldwrkr, &u[u_offset], ldu, dum, &c__1, &work[
			    iwork], info);
		    iu = ie + *m;

/*                 Multiply right singular vectors of L in WORK(IR) by Q */
/*                 in A, storing result in WORK(IU) and copying to A */
/*                 (Workspace: need M*M+2*M, prefer M*M+M*N+M)) */

		    i__3 = *n;
		    i__2 = chunk;
		    for (i__ = 1; i__2 < 0 ? i__ >= i__3 : i__ <= i__3; i__ +=
			     i__2) {
/* Computing MIN */
			i__4 = *n - i__ + 1;
			blk = std::min(i__4,chunk);
			dgemm_("N", "N", m, &blk, m, &c_b443, &work[ir], &
				ldwrkr, &a[i__ * a_dim1 + 1], lda, &c_b421, &
				work[iu], &ldwrku);
			dlacpy_("F", m, &blk, &work[iu], &ldwrku, &a[i__ *
				a_dim1 + 1], lda);
/* L40: */
		    }

		} else {

/*                 Insufficient workspace for a fast algorithm */

		    itau = 1;
		    iwork = itau + *m;

/*                 Compute A=L*Q */
/*                 (Workspace: need 2*M, prefer M+M*NB) */

		    i__2 = *lwork - iwork + 1;
		    dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[iwork]
, &i__2, &ierr);

/*                 Copy L to U, zeroing out above it */

		    dlacpy_("L", m, m, &a[a_offset], lda, &u[u_offset], ldu);
		    i__2 = *m - 1;
		    i__3 = *m - 1;
		    dlaset_("U", &i__2, &i__3, &c_b421, &c_b421, &u[(u_dim1 <<
			     1) + 1], ldu);

/*                 Generate Q in A */
/*                 (Workspace: need 2*M, prefer M+M*NB) */

		    i__2 = *lwork - iwork + 1;
		    dorglq_(m, n, m, &a[a_offset], lda, &work[itau], &work[
			    iwork], &i__2, &ierr);
		    ie = itau;
		    itauq = ie + *m;
		    itaup = itauq + *m;
		    iwork = itaup + *m;

/*                 Bidiagonalize L in U */
/*                 (Workspace: need 4*M, prefer 3*M+2*M*NB) */

		    i__2 = *lwork - iwork + 1;
		    dgebrd_(m, m, &u[u_offset], ldu, &s[1], &work[ie], &work[
			    itauq], &work[itaup], &work[iwork], &i__2, &ierr);

/*                 Multiply right vectors bidiagonalizing L by Q in A */
/*                 (Workspace: need 3*M+N, prefer 3*M+N*NB) */

		    i__2 = *lwork - iwork + 1;
		    dormbr_("P", "L", "T", m, n, m, &u[u_offset], ldu, &work[
			    itaup], &a[a_offset], lda, &work[iwork], &i__2, &
			    ierr);

/*                 Generate left vectors bidiagonalizing L in U */
/*                 (Workspace: need 4*M, prefer 3*M+M*NB) */

		    i__2 = *lwork - iwork + 1;
		    dorgbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq], &
			    work[iwork], &i__2, &ierr);
		    iwork = ie + *m;

/*                 Perform bidiagonal QR iteration, computing left */
/*                 singular vectors of A in U and computing right */
/*                 singular vectors of A in A */
/*                 (Workspace: need BDSPAC) */

		    dbdsqr_("U", m, n, m, &c__0, &s[1], &work[ie], &a[
			    a_offset], lda, &u[u_offset], ldu, dum, &c__1, &
			    work[iwork], info);

		}

	    } else if (wntvs) {

		if (wntun) {

/*                 Path 4t(N much larger than M, JOBU='N', JOBVT='S') */
/*                 M right singular vectors to be computed in VT and */
/*                 no left singular vectors to be computed */

/* Computing MAX */
		    i__2 = *m << 2;
		    if (*lwork >= *m * *m + std::max(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			ir = 1;
			if (*lwork >= wrkbl + *lda * *m) {

/*                       WORK(IR) is LDA by M */

			    ldwrkr = *lda;
			} else {

/*                       WORK(IR) is M by M */

			    ldwrkr = *m;
			}
			itau = ir + ldwrkr * *m;
			iwork = itau + *m;

/*                    Compute A=L*Q */
/*                    (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);

/*                    Copy L to WORK(IR), zeroing out above it */

			dlacpy_("L", m, m, &a[a_offset], lda, &work[ir], &
				ldwrkr);
			i__2 = *m - 1;
			i__3 = *m - 1;
			dlaset_("U", &i__2, &i__3, &c_b421, &c_b421, &work[ir
				+ ldwrkr], &ldwrkr);

/*                    Generate Q in A */
/*                    (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorglq_(m, n, m, &a[a_offset], lda, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IR) */
/*                    (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(m, m, &work[ir], &ldwrkr, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Generate right vectors bidiagonalizing L in */
/*                    WORK(IR) */
/*                    (Workspace: need M*M+4*M, prefer M*M+3*M+(M-1)*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("P", m, m, m, &work[ir], &ldwrkr, &work[itaup]
, &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing right */
/*                    singular vectors of L in WORK(IR) */
/*                    (Workspace: need M*M+BDSPAC) */

			dbdsqr_("U", m, m, &c__0, &c__0, &s[1], &work[ie], &
				work[ir], &ldwrkr, dum, &c__1, dum, &c__1, &
				work[iwork], info);

/*                    Multiply right singular vectors of L in WORK(IR) by */
/*                    Q in A, storing result in VT */
/*                    (Workspace: need M*M) */

			dgemm_("N", "N", m, n, m, &c_b443, &work[ir], &ldwrkr,
				 &a[a_offset], lda, &c_b421, &vt[vt_offset],
				ldvt);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *m;

/*                    Compute A=L*Q */
/*                    (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);

/*                    Copy result to VT */

			dlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset],
				ldvt);

/*                    Generate Q in VT */
/*                    (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorglq_(m, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Zero out above L in A */

			i__2 = *m - 1;
			i__3 = *m - 1;
			dlaset_("U", &i__2, &i__3, &c_b421, &c_b421, &a[(
				a_dim1 << 1) + 1], lda);

/*                    Bidiagonalize L in A */
/*                    (Workspace: need 4*M, prefer 3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(m, m, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply right vectors bidiagonalizing L by Q in VT */
/*                    (Workspace: need 3*M+N, prefer 3*M+N*NB) */

			i__2 = *lwork - iwork + 1;
			dormbr_("P", "L", "T", m, n, m, &a[a_offset], lda, &
				work[itaup], &vt[vt_offset], ldvt, &work[
				iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing right */
/*                    singular vectors of A in VT */
/*                    (Workspace: need BDSPAC) */

			dbdsqr_("U", m, n, &c__0, &c__0, &s[1], &work[ie], &
				vt[vt_offset], ldvt, dum, &c__1, dum, &c__1, &
				work[iwork], info);

		    }

		} else if (wntuo) {

/*                 Path 5t(N much larger than M, JOBU='O', JOBVT='S') */
/*                 M right singular vectors to be computed in VT and */
/*                 M left singular vectors to be overwritten on A */

/* Computing MAX */
		    i__2 = *m << 2;
		    if (*lwork >= (*m << 1) * *m + std::max(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + (*lda << 1) * *m) {

/*                       WORK(IU) is LDA by M and WORK(IR) is LDA by M */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *m;
			    ldwrkr = *lda;
			} else if (*lwork >= wrkbl + (*lda + *m) * *m) {

/*                       WORK(IU) is LDA by M and WORK(IR) is M by M */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *m;
			    ldwrkr = *m;
			} else {

/*                       WORK(IU) is M by M and WORK(IR) is M by M */

			    ldwrku = *m;
			    ir = iu + ldwrku * *m;
			    ldwrkr = *m;
			}
			itau = ir + ldwrkr * *m;
			iwork = itau + *m;

/*                    Compute A=L*Q */
/*                    (Workspace: need 2*M*M+2*M, prefer 2*M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);

/*                    Copy L to WORK(IU), zeroing out below it */

			dlacpy_("L", m, m, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *m - 1;
			i__3 = *m - 1;
			dlaset_("U", &i__2, &i__3, &c_b421, &c_b421, &work[iu
				+ ldwrku], &ldwrku);

/*                    Generate Q in A */
/*                    (Workspace: need 2*M*M+2*M, prefer 2*M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorglq_(m, n, m, &a[a_offset], lda, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IU), copying result to */
/*                    WORK(IR) */
/*                    (Workspace: need 2*M*M+4*M, */
/*                                prefer 2*M*M+3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(m, m, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			dlacpy_("L", m, m, &work[iu], &ldwrku, &work[ir], &
				ldwrkr);

/*                    Generate right bidiagonalizing vectors in WORK(IU) */
/*                    (Workspace: need 2*M*M+4*M-1, */
/*                                prefer 2*M*M+3*M+(M-1)*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("P", m, m, m, &work[iu], &ldwrku, &work[itaup]
, &work[iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in WORK(IR) */
/*                    (Workspace: need 2*M*M+4*M, prefer 2*M*M+3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("Q", m, m, m, &work[ir], &ldwrkr, &work[itauq]
, &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of L in WORK(IR) and computing */
/*                    right singular vectors of L in WORK(IU) */
/*                    (Workspace: need 2*M*M+BDSPAC) */

			dbdsqr_("U", m, m, m, &c__0, &s[1], &work[ie], &work[
				iu], &ldwrku, &work[ir], &ldwrkr, dum, &c__1,
				&work[iwork], info);

/*                    Multiply right singular vectors of L in WORK(IU) by */
/*                    Q in A, storing result in VT */
/*                    (Workspace: need M*M) */

			dgemm_("N", "N", m, n, m, &c_b443, &work[iu], &ldwrku,
				 &a[a_offset], lda, &c_b421, &vt[vt_offset],
				ldvt);

/*                    Copy left singular vectors of L to A */
/*                    (Workspace: need M*M) */

			dlacpy_("F", m, m, &work[ir], &ldwrkr, &a[a_offset],
				lda);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset],
				ldvt);

/*                    Generate Q in VT */
/*                    (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorglq_(m, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Zero out above L in A */

			i__2 = *m - 1;
			i__3 = *m - 1;
			dlaset_("U", &i__2, &i__3, &c_b421, &c_b421, &a[(
				a_dim1 << 1) + 1], lda);

/*                    Bidiagonalize L in A */
/*                    (Workspace: need 4*M, prefer 3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(m, m, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply right vectors bidiagonalizing L by Q in VT */
/*                    (Workspace: need 3*M+N, prefer 3*M+N*NB) */

			i__2 = *lwork - iwork + 1;
			dormbr_("P", "L", "T", m, n, m, &a[a_offset], lda, &
				work[itaup], &vt[vt_offset], ldvt, &work[
				iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors of L in A */
/*                    (Workspace: need 4*M, prefer 3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("Q", m, m, m, &a[a_offset], lda, &work[itauq],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, compute left */
/*                    singular vectors of A in A and compute right */
/*                    singular vectors of A in VT */
/*                    (Workspace: need BDSPAC) */

			dbdsqr_("U", m, n, m, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &a[a_offset], lda, dum, &
				c__1, &work[iwork], info);

		    }

		} else if (wntuas) {

/*                 Path 6t(N much larger than M, JOBU='S' or 'A', */
/*                         JOBVT='S') */
/*                 M right singular vectors to be computed in VT and */
/*                 M left singular vectors to be computed in U */

/* Computing MAX */
		    i__2 = *m << 2;
		    if (*lwork >= *m * *m + std::max(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + *lda * *m) {

/*                       WORK(IU) is LDA by N */

			    ldwrku = *lda;
			} else {

/*                       WORK(IU) is LDA by M */

			    ldwrku = *m;
			}
			itau = iu + ldwrku * *m;
			iwork = itau + *m;

/*                    Compute A=L*Q */
/*                    (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);

/*                    Copy L to WORK(IU), zeroing out above it */

			dlacpy_("L", m, m, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *m - 1;
			i__3 = *m - 1;
			dlaset_("U", &i__2, &i__3, &c_b421, &c_b421, &work[iu
				+ ldwrku], &ldwrku);

/*                    Generate Q in A */
/*                    (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorglq_(m, n, m, &a[a_offset], lda, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IU), copying result to U */
/*                    (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(m, m, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			dlacpy_("L", m, m, &work[iu], &ldwrku, &u[u_offset],
				ldu);

/*                    Generate right bidiagonalizing vectors in WORK(IU) */
/*                    (Workspace: need M*M+4*M-1, */
/*                                prefer M*M+3*M+(M-1)*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("P", m, m, m, &work[iu], &ldwrku, &work[itaup]
, &work[iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in U */
/*                    (Workspace: need M*M+4*M, prefer M*M+3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of L in U and computing right */
/*                    singular vectors of L in WORK(IU) */
/*                    (Workspace: need M*M+BDSPAC) */

			dbdsqr_("U", m, m, m, &c__0, &s[1], &work[ie], &work[
				iu], &ldwrku, &u[u_offset], ldu, dum, &c__1, &
				work[iwork], info);

/*                    Multiply right singular vectors of L in WORK(IU) by */
/*                    Q in A, storing result in VT */
/*                    (Workspace: need M*M) */

			dgemm_("N", "N", m, n, m, &c_b443, &work[iu], &ldwrku,
				 &a[a_offset], lda, &c_b421, &vt[vt_offset],
				ldvt);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset],
				ldvt);

/*                    Generate Q in VT */
/*                    (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorglq_(m, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy L to U, zeroing out above it */

			dlacpy_("L", m, m, &a[a_offset], lda, &u[u_offset],
				ldu);
			i__2 = *m - 1;
			i__3 = *m - 1;
			dlaset_("U", &i__2, &i__3, &c_b421, &c_b421, &u[(
				u_dim1 << 1) + 1], ldu);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in U */
/*                    (Workspace: need 4*M, prefer 3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(m, m, &u[u_offset], ldu, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply right bidiagonalizing vectors in U by Q */
/*                    in VT */
/*                    (Workspace: need 3*M+N, prefer 3*M+N*NB) */

			i__2 = *lwork - iwork + 1;
			dormbr_("P", "L", "T", m, n, m, &u[u_offset], ldu, &
				work[itaup], &vt[vt_offset], ldvt, &work[
				iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in U */
/*                    (Workspace: need 4*M, prefer 3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U and computing right */
/*                    singular vectors of A in VT */
/*                    (Workspace: need BDSPAC) */

			dbdsqr_("U", m, n, m, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &u[u_offset], ldu, dum, &
				c__1, &work[iwork], info);

		    }

		}

	    } else if (wntva) {

		if (wntun) {

/*                 Path 7t(N much larger than M, JOBU='N', JOBVT='A') */
/*                 N right singular vectors to be computed in VT and */
/*                 no left singular vectors to be computed */

/* Computing MAX */
		    i__2 = *n + *m, i__3 = *m << 2, i__2 = std::max(i__2,i__3);
		    if (*lwork >= *m * *m + std::max(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			ir = 1;
			if (*lwork >= wrkbl + *lda * *m) {

/*                       WORK(IR) is LDA by M */

			    ldwrkr = *lda;
			} else {

/*                       WORK(IR) is M by M */

			    ldwrkr = *m;
			}
			itau = ir + ldwrkr * *m;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset],
				ldvt);

/*                    Copy L to WORK(IR), zeroing out above it */

			dlacpy_("L", m, m, &a[a_offset], lda, &work[ir], &
				ldwrkr);
			i__2 = *m - 1;
			i__3 = *m - 1;
			dlaset_("U", &i__2, &i__3, &c_b421, &c_b421, &work[ir
				+ ldwrkr], &ldwrkr);

/*                    Generate Q in VT */
/*                    (Workspace: need M*M+M+N, prefer M*M+M+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IR) */
/*                    (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(m, m, &work[ir], &ldwrkr, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Generate right bidiagonalizing vectors in WORK(IR) */
/*                    (Workspace: need M*M+4*M-1, */
/*                                prefer M*M+3*M+(M-1)*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("P", m, m, m, &work[ir], &ldwrkr, &work[itaup]
, &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing right */
/*                    singular vectors of L in WORK(IR) */
/*                    (Workspace: need M*M+BDSPAC) */

			dbdsqr_("U", m, m, &c__0, &c__0, &s[1], &work[ie], &
				work[ir], &ldwrkr, dum, &c__1, dum, &c__1, &
				work[iwork], info);

/*                    Multiply right singular vectors of L in WORK(IR) by */
/*                    Q in VT, storing result in A */
/*                    (Workspace: need M*M) */

			dgemm_("N", "N", m, n, m, &c_b443, &work[ir], &ldwrkr,
				 &vt[vt_offset], ldvt, &c_b421, &a[a_offset],
				lda);

/*                    Copy right singular vectors of A from A to VT */

			dlacpy_("F", m, n, &a[a_offset], lda, &vt[vt_offset],
				ldvt);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset],
				ldvt);

/*                    Generate Q in VT */
/*                    (Workspace: need M+N, prefer M+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Zero out above L in A */

			i__2 = *m - 1;
			i__3 = *m - 1;
			dlaset_("U", &i__2, &i__3, &c_b421, &c_b421, &a[(
				a_dim1 << 1) + 1], lda);

/*                    Bidiagonalize L in A */
/*                    (Workspace: need 4*M, prefer 3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(m, m, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply right bidiagonalizing vectors in A by Q */
/*                    in VT */
/*                    (Workspace: need 3*M+N, prefer 3*M+N*NB) */

			i__2 = *lwork - iwork + 1;
			dormbr_("P", "L", "T", m, n, m, &a[a_offset], lda, &
				work[itaup], &vt[vt_offset], ldvt, &work[
				iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing right */
/*                    singular vectors of A in VT */
/*                    (Workspace: need BDSPAC) */

			dbdsqr_("U", m, n, &c__0, &c__0, &s[1], &work[ie], &
				vt[vt_offset], ldvt, dum, &c__1, dum, &c__1, &
				work[iwork], info);

		    }

		} else if (wntuo) {

/*                 Path 8t(N much larger than M, JOBU='O', JOBVT='A') */
/*                 N right singular vectors to be computed in VT and */
/*                 M left singular vectors to be overwritten on A */

/* Computing MAX */
		    i__2 = *n + *m, i__3 = *m << 2, i__2 = std::max(i__2,i__3);
		    if (*lwork >= (*m << 1) * *m + std::max(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + (*lda << 1) * *m) {

/*                       WORK(IU) is LDA by M and WORK(IR) is LDA by M */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *m;
			    ldwrkr = *lda;
			} else if (*lwork >= wrkbl + (*lda + *m) * *m) {

/*                       WORK(IU) is LDA by M and WORK(IR) is M by M */

			    ldwrku = *lda;
			    ir = iu + ldwrku * *m;
			    ldwrkr = *m;
			} else {

/*                       WORK(IU) is M by M and WORK(IR) is M by M */

			    ldwrku = *m;
			    ir = iu + ldwrku * *m;
			    ldwrkr = *m;
			}
			itau = ir + ldwrkr * *m;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (Workspace: need 2*M*M+2*M, prefer 2*M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset],
				ldvt);

/*                    Generate Q in VT */
/*                    (Workspace: need 2*M*M+M+N, prefer 2*M*M+M+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy L to WORK(IU), zeroing out above it */

			dlacpy_("L", m, m, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *m - 1;
			i__3 = *m - 1;
			dlaset_("U", &i__2, &i__3, &c_b421, &c_b421, &work[iu
				+ ldwrku], &ldwrku);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IU), copying result to */
/*                    WORK(IR) */
/*                    (Workspace: need 2*M*M+4*M, */
/*                                prefer 2*M*M+3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(m, m, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			dlacpy_("L", m, m, &work[iu], &ldwrku, &work[ir], &
				ldwrkr);

/*                    Generate right bidiagonalizing vectors in WORK(IU) */
/*                    (Workspace: need 2*M*M+4*M-1, */
/*                                prefer 2*M*M+3*M+(M-1)*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("P", m, m, m, &work[iu], &ldwrku, &work[itaup]
, &work[iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in WORK(IR) */
/*                    (Workspace: need 2*M*M+4*M, prefer 2*M*M+3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("Q", m, m, m, &work[ir], &ldwrkr, &work[itauq]
, &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of L in WORK(IR) and computing */
/*                    right singular vectors of L in WORK(IU) */
/*                    (Workspace: need 2*M*M+BDSPAC) */

			dbdsqr_("U", m, m, m, &c__0, &s[1], &work[ie], &work[
				iu], &ldwrku, &work[ir], &ldwrkr, dum, &c__1,
				&work[iwork], info);

/*                    Multiply right singular vectors of L in WORK(IU) by */
/*                    Q in VT, storing result in A */
/*                    (Workspace: need M*M) */

			dgemm_("N", "N", m, n, m, &c_b443, &work[iu], &ldwrku,
				 &vt[vt_offset], ldvt, &c_b421, &a[a_offset],
				lda);

/*                    Copy right singular vectors of A from A to VT */

			dlacpy_("F", m, n, &a[a_offset], lda, &vt[vt_offset],
				ldvt);

/*                    Copy left singular vectors of A from WORK(IR) to A */

			dlacpy_("F", m, m, &work[ir], &ldwrkr, &a[a_offset],
				lda);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset],
				ldvt);

/*                    Generate Q in VT */
/*                    (Workspace: need M+N, prefer M+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Zero out above L in A */

			i__2 = *m - 1;
			i__3 = *m - 1;
			dlaset_("U", &i__2, &i__3, &c_b421, &c_b421, &a[(
				a_dim1 << 1) + 1], lda);

/*                    Bidiagonalize L in A */
/*                    (Workspace: need 4*M, prefer 3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(m, m, &a[a_offset], lda, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply right bidiagonalizing vectors in A by Q */
/*                    in VT */
/*                    (Workspace: need 3*M+N, prefer 3*M+N*NB) */

			i__2 = *lwork - iwork + 1;
			dormbr_("P", "L", "T", m, n, m, &a[a_offset], lda, &
				work[itaup], &vt[vt_offset], ldvt, &work[
				iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in A */
/*                    (Workspace: need 4*M, prefer 3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("Q", m, m, m, &a[a_offset], lda, &work[itauq],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in A and computing right */
/*                    singular vectors of A in VT */
/*                    (Workspace: need BDSPAC) */

			dbdsqr_("U", m, n, m, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &a[a_offset], lda, dum, &
				c__1, &work[iwork], info);

		    }

		} else if (wntuas) {

/*                 Path 9t(N much larger than M, JOBU='S' or 'A', */
/*                         JOBVT='A') */
/*                 N right singular vectors to be computed in VT and */
/*                 M left singular vectors to be computed in U */

/* Computing MAX */
		    i__2 = *n + *m, i__3 = *m << 2, i__2 = std::max(i__2,i__3);
		    if (*lwork >= *m * *m + std::max(i__2,bdspac)) {

/*                    Sufficient workspace for a fast algorithm */

			iu = 1;
			if (*lwork >= wrkbl + *lda * *m) {

/*                       WORK(IU) is LDA by M */

			    ldwrku = *lda;
			} else {

/*                       WORK(IU) is M by M */

			    ldwrku = *m;
			}
			itau = iu + ldwrku * *m;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (Workspace: need M*M+2*M, prefer M*M+M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset],
				ldvt);

/*                    Generate Q in VT */
/*                    (Workspace: need M*M+M+N, prefer M*M+M+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy L to WORK(IU), zeroing out above it */

			dlacpy_("L", m, m, &a[a_offset], lda, &work[iu], &
				ldwrku);
			i__2 = *m - 1;
			i__3 = *m - 1;
			dlaset_("U", &i__2, &i__3, &c_b421, &c_b421, &work[iu
				+ ldwrku], &ldwrku);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in WORK(IU), copying result to U */
/*                    (Workspace: need M*M+4*M, prefer M*M+3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(m, m, &work[iu], &ldwrku, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);
			dlacpy_("L", m, m, &work[iu], &ldwrku, &u[u_offset],
				ldu);

/*                    Generate right bidiagonalizing vectors in WORK(IU) */
/*                    (Workspace: need M*M+4*M, prefer M*M+3*M+(M-1)*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("P", m, m, m, &work[iu], &ldwrku, &work[itaup]
, &work[iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in U */
/*                    (Workspace: need M*M+4*M, prefer M*M+3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of L in U and computing right */
/*                    singular vectors of L in WORK(IU) */
/*                    (Workspace: need M*M+BDSPAC) */

			dbdsqr_("U", m, m, m, &c__0, &s[1], &work[ie], &work[
				iu], &ldwrku, &u[u_offset], ldu, dum, &c__1, &
				work[iwork], info);

/*                    Multiply right singular vectors of L in WORK(IU) by */
/*                    Q in VT, storing result in A */
/*                    (Workspace: need M*M) */

			dgemm_("N", "N", m, n, m, &c_b443, &work[iu], &ldwrku,
				 &vt[vt_offset], ldvt, &c_b421, &a[a_offset],
				lda);

/*                    Copy right singular vectors of A from A to VT */

			dlacpy_("F", m, n, &a[a_offset], lda, &vt[vt_offset],
				ldvt);

		    } else {

/*                    Insufficient workspace for a fast algorithm */

			itau = 1;
			iwork = itau + *m;

/*                    Compute A=L*Q, copying result to VT */
/*                    (Workspace: need 2*M, prefer M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dgelqf_(m, n, &a[a_offset], lda, &work[itau], &work[
				iwork], &i__2, &ierr);
			dlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset],
				ldvt);

/*                    Generate Q in VT */
/*                    (Workspace: need M+N, prefer M+N*NB) */

			i__2 = *lwork - iwork + 1;
			dorglq_(n, n, m, &vt[vt_offset], ldvt, &work[itau], &
				work[iwork], &i__2, &ierr);

/*                    Copy L to U, zeroing out above it */

			dlacpy_("L", m, m, &a[a_offset], lda, &u[u_offset],
				ldu);
			i__2 = *m - 1;
			i__3 = *m - 1;
			dlaset_("U", &i__2, &i__3, &c_b421, &c_b421, &u[(
				u_dim1 << 1) + 1], ldu);
			ie = itau;
			itauq = ie + *m;
			itaup = itauq + *m;
			iwork = itaup + *m;

/*                    Bidiagonalize L in U */
/*                    (Workspace: need 4*M, prefer 3*M+2*M*NB) */

			i__2 = *lwork - iwork + 1;
			dgebrd_(m, m, &u[u_offset], ldu, &s[1], &work[ie], &
				work[itauq], &work[itaup], &work[iwork], &
				i__2, &ierr);

/*                    Multiply right bidiagonalizing vectors in U by Q */
/*                    in VT */
/*                    (Workspace: need 3*M+N, prefer 3*M+N*NB) */

			i__2 = *lwork - iwork + 1;
			dormbr_("P", "L", "T", m, n, m, &u[u_offset], ldu, &
				work[itaup], &vt[vt_offset], ldvt, &work[
				iwork], &i__2, &ierr);

/*                    Generate left bidiagonalizing vectors in U */
/*                    (Workspace: need 4*M, prefer 3*M+M*NB) */

			i__2 = *lwork - iwork + 1;
			dorgbr_("Q", m, m, m, &u[u_offset], ldu, &work[itauq],
				 &work[iwork], &i__2, &ierr);
			iwork = ie + *m;

/*                    Perform bidiagonal QR iteration, computing left */
/*                    singular vectors of A in U and computing right */
/*                    singular vectors of A in VT */
/*                    (Workspace: need BDSPAC) */

			dbdsqr_("U", m, n, m, &c__0, &s[1], &work[ie], &vt[
				vt_offset], ldvt, &u[u_offset], ldu, dum, &
				c__1, &work[iwork], info);

		    }

		}

	    }

	} else {

/*           N .LT. MNTHR */

/*           Path 10t(N greater than M, but not much larger) */
/*           Reduce to bidiagonal form without LQ decomposition */

	    ie = 1;
	    itauq = ie + *m;
	    itaup = itauq + *m;
	    iwork = itaup + *m;

/*           Bidiagonalize A */
/*           (Workspace: need 3*M+N, prefer 3*M+(M+N)*NB) */

	    i__2 = *lwork - iwork + 1;
	    dgebrd_(m, n, &a[a_offset], lda, &s[1], &work[ie], &work[itauq], &
		    work[itaup], &work[iwork], &i__2, &ierr);
	    if (wntuas) {

/*              If left singular vectors desired in U, copy result to U */
/*              and generate left bidiagonalizing vectors in U */
/*              (Workspace: need 4*M-1, prefer 3*M+(M-1)*NB) */

		dlacpy_("L", m, m, &a[a_offset], lda, &u[u_offset], ldu);
		i__2 = *lwork - iwork + 1;
		dorgbr_("Q", m, m, n, &u[u_offset], ldu, &work[itauq], &work[
			iwork], &i__2, &ierr);
	    }
	    if (wntvas) {

/*              If right singular vectors desired in VT, copy result to */
/*              VT and generate right bidiagonalizing vectors in VT */
/*              (Workspace: need 3*M+NRVT, prefer 3*M+NRVT*NB) */

		dlacpy_("U", m, n, &a[a_offset], lda, &vt[vt_offset], ldvt);
		if (wntva) {
		    nrvt = *n;
		}
		if (wntvs) {
		    nrvt = *m;
		}
		i__2 = *lwork - iwork + 1;
		dorgbr_("P", &nrvt, n, m, &vt[vt_offset], ldvt, &work[itaup],
			&work[iwork], &i__2, &ierr);
	    }
	    if (wntuo) {

/*              If left singular vectors desired in A, generate left */
/*              bidiagonalizing vectors in A */
/*              (Workspace: need 4*M-1, prefer 3*M+(M-1)*NB) */

		i__2 = *lwork - iwork + 1;
		dorgbr_("Q", m, m, n, &a[a_offset], lda, &work[itauq], &work[
			iwork], &i__2, &ierr);
	    }
	    if (wntvo) {

/*              If right singular vectors desired in A, generate right */
/*              bidiagonalizing vectors in A */
/*              (Workspace: need 4*M, prefer 3*M+M*NB) */

		i__2 = *lwork - iwork + 1;
		dorgbr_("P", m, n, m, &a[a_offset], lda, &work[itaup], &work[
			iwork], &i__2, &ierr);
	    }
	    iwork = ie + *m;
	    if (wntuas || wntuo) {
		nru = *m;
	    }
	    if (wntun) {
		nru = 0;
	    }
	    if (wntvas || wntvo) {
		ncvt = *n;
	    }
	    if (wntvn) {
		ncvt = 0;
	    }
	    if (! wntuo && ! wntvo) {

/*              Perform bidiagonal QR iteration, if desired, computing */
/*              left singular vectors in U and computing right singular */
/*              vectors in VT */
/*              (Workspace: need BDSPAC) */

		dbdsqr_("L", m, &ncvt, &nru, &c__0, &s[1], &work[ie], &vt[
			vt_offset], ldvt, &u[u_offset], ldu, dum, &c__1, &
			work[iwork], info);
	    } else if (! wntuo && wntvo) {

/*              Perform bidiagonal QR iteration, if desired, computing */
/*              left singular vectors in U and computing right singular */
/*              vectors in A */
/*              (Workspace: need BDSPAC) */

		dbdsqr_("L", m, &ncvt, &nru, &c__0, &s[1], &work[ie], &a[
			a_offset], lda, &u[u_offset], ldu, dum, &c__1, &work[
			iwork], info);
	    } else {

/*              Perform bidiagonal QR iteration, if desired, computing */
/*              left singular vectors in A and computing right singular */
/*              vectors in VT */
/*              (Workspace: need BDSPAC) */

		dbdsqr_("L", m, &ncvt, &nru, &c__0, &s[1], &work[ie], &vt[
			vt_offset], ldvt, &a[a_offset], lda, dum, &c__1, &
			work[iwork], info);
	    }

	}

    }

/*     If DBDSQR failed to converge, copy unconverged superdiagonals */
/*     to WORK( 2:MINMN ) */

    if (*info != 0) {
	if (ie > 2) {
	    i__2 = minmn - 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		work[i__ + 1] = work[i__ + ie - 1];
/* L50: */
	    }
	}
	if (ie < 2) {
	    for (i__ = minmn - 1; i__ >= 1; --i__) {
		work[i__ + 1] = work[i__ + ie - 1];
/* L60: */
	    }
	}
    }

/*     Undo scaling if necessary */

    if (iscl == 1) {
	if (anrm > bignum) {
	    dlascl_("G", &c__0, &c__0, &bignum, &anrm, &minmn, &c__1, &s[1], &
		    minmn, &ierr);
	}
	if (*info != 0 && anrm > bignum) {
	    i__2 = minmn - 1;
	    dlascl_("G", &c__0, &c__0, &bignum, &anrm, &i__2, &c__1, &work[2],
		     &minmn, &ierr);
	}
	if (anrm < smlnum) {
	    dlascl_("G", &c__0, &c__0, &smlnum, &anrm, &minmn, &c__1, &s[1], &
		    minmn, &ierr);
	}
	if (*info != 0 && anrm < smlnum) {
	    i__2 = minmn - 1;
	    dlascl_("G", &c__0, &c__0, &smlnum, &anrm, &i__2, &c__1, &work[2],
		     &minmn, &ierr);
	}
    }

/*     Return optimal workspace in WORK(1) */

    work[1] = (double) maxwrk;

    return 0;

/*     End of DGESVD */

} /* dgesvd_ */


/* Subroutine */ int dgesvx_(const char *fact, const char *trans, integer *n, integer *
	nrhs, double *a, integer *lda, double *af, integer *ldaf,
	integer *ipiv, char *equed, double *r__, double *c__,
	double *b, integer *ldb, double *x, integer *ldx, double *
	rcond, double *ferr, double *berr, double *work, integer *
	iwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, b_dim1, b_offset, x_dim1,
	    x_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j;
    double amax;
    char norm[1];
    double rcmin, rcmax, anorm;
    bool equil;
    double colcnd;
    bool nofact;
    double bignum;
    integer infequ;
    bool colequ;
    double rowcnd;
    bool notran;
    double smlnum;
    bool rowequ;
    double rpvgrw;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGESVX uses the LU factorization to compute the solution to a real */
/*  system of linear equations */
/*     A * X = B, */
/*  where A is an N-by-N matrix and X and B are N-by-NRHS matrices. */

/*  Error bounds on the solution and a condition estimate are also */
/*  provided. */

/*  Description */
/*  =========== */

/*  The following steps are performed: */

/*  1. If FACT = 'E', real scaling factors are computed to equilibrate */
/*     the system: */
/*        TRANS = 'N':  diag(R)*A*diag(C)     *inv(diag(C))*X = diag(R)*B */
/*        TRANS = 'T': (diag(R)*A*diag(C))**T *inv(diag(R))*X = diag(C)*B */
/*        TRANS = 'C': (diag(R)*A*diag(C))**H *inv(diag(R))*X = diag(C)*B */
/*     Whether or not the system will be equilibrated depends on the */
/*     scaling of the matrix A, but if equilibration is used, A is */
/*     overwritten by diag(R)*A*diag(C) and B by diag(R)*B (if TRANS='N') */
/*     or diag(C)*B (if TRANS = 'T' or 'C'). */

/*  2. If FACT = 'N' or 'E', the LU decomposition is used to factor the */
/*     matrix A (after equilibration if FACT = 'E') as */
/*        A = P * L * U, */
/*     where P is a permutation matrix, L is a unit lower triangular */
/*     matrix, and U is upper triangular. */

/*  3. If some U(i,i)=0, so that U is exactly singular, then the routine */
/*     returns with INFO = i. Otherwise, the factored form of A is used */
/*     to estimate the condition number of the matrix A.  If the */
/*     reciprocal of the condition number is less than machine precision, */
/*     INFO = N+1 is returned as a warning, but the routine still goes on */
/*     to solve for X and compute error bounds as described below. */

/*  4. The system of equations is solved for X using the factored form */
/*     of A. */

/*  5. Iterative refinement is applied to improve the computed solution */
/*     matrix and calculate error bounds and backward error estimates */
/*     for it. */

/*  6. If equilibration was used, the matrix X is premultiplied by */
/*     diag(C) (if TRANS = 'N') or diag(R) (if TRANS = 'T' or 'C') so */
/*     that it solves the original system before equilibration. */

/*  Arguments */
/*  ========= */

/*  FACT    (input) CHARACTER*1 */
/*          Specifies whether or not the factored form of the matrix A is */
/*          supplied on entry, and if not, whether the matrix A should be */
/*          equilibrated before it is factored. */
/*          = 'F':  On entry, AF and IPIV contain the factored form of A. */
/*                  If EQUED is not 'N', the matrix A has been */
/*                  equilibrated with scaling factors given by R and C. */
/*                  A, AF, and IPIV are not modified. */
/*          = 'N':  The matrix A will be copied to AF and factored. */
/*          = 'E':  The matrix A will be equilibrated if necessary, then */
/*                  copied to AF and factored. */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations: */
/*          = 'N':  A * X = B     (No transpose) */
/*          = 'T':  A**T * X = B  (Transpose) */
/*          = 'C':  A**H * X = B  (Transpose) */

/*  N       (input) INTEGER */
/*          The number of linear equations, i.e., the order of the */
/*          matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X.  NRHS >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the N-by-N matrix A.  If FACT = 'F' and EQUED is */
/*          not 'N', then A must have been equilibrated by the scaling */
/*          factors in R and/or C.  A is not modified if FACT = 'F' or */
/*          'N', or if FACT = 'E' and EQUED = 'N' on exit. */

/*          On exit, if EQUED .ne. 'N', A is scaled as follows: */
/*          EQUED = 'R':  A := diag(R) * A */
/*          EQUED = 'C':  A := A * diag(C) */
/*          EQUED = 'B':  A := diag(R) * A * diag(C). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  AF      (input or output) DOUBLE PRECISION array, dimension (LDAF,N) */
/*          If FACT = 'F', then AF is an input argument and on entry */
/*          contains the factors L and U from the factorization */
/*          A = P*L*U as computed by DGETRF.  If EQUED .ne. 'N', then */
/*          AF is the factored form of the equilibrated matrix A. */

/*          If FACT = 'N', then AF is an output argument and on exit */
/*          returns the factors L and U from the factorization A = P*L*U */
/*          of the original matrix A. */

/*          If FACT = 'E', then AF is an output argument and on exit */
/*          returns the factors L and U from the factorization A = P*L*U */
/*          of the equilibrated matrix A (see the description of A for */
/*          the form of the equilibrated matrix). */

/*  LDAF    (input) INTEGER */
/*          The leading dimension of the array AF.  LDAF >= max(1,N). */

/*  IPIV    (input or output) INTEGER array, dimension (N) */
/*          If FACT = 'F', then IPIV is an input argument and on entry */
/*          contains the pivot indices from the factorization A = P*L*U */
/*          as computed by DGETRF; row i of the matrix was interchanged */
/*          with row IPIV(i). */

/*          If FACT = 'N', then IPIV is an output argument and on exit */
/*          contains the pivot indices from the factorization A = P*L*U */
/*          of the original matrix A. */

/*          If FACT = 'E', then IPIV is an output argument and on exit */
/*          contains the pivot indices from the factorization A = P*L*U */
/*          of the equilibrated matrix A. */

/*  EQUED   (input or output) CHARACTER*1 */
/*          Specifies the form of equilibration that was done. */
/*          = 'N':  No equilibration (always true if FACT = 'N'). */
/*          = 'R':  Row equilibration, i.e., A has been premultiplied by */
/*                  diag(R). */
/*          = 'C':  Column equilibration, i.e., A has been postmultiplied */
/*                  by diag(C). */
/*          = 'B':  Both row and column equilibration, i.e., A has been */
/*                  replaced by diag(R) * A * diag(C). */
/*          EQUED is an input argument if FACT = 'F'; otherwise, it is an */
/*          output argument. */

/*  R       (input or output) DOUBLE PRECISION array, dimension (N) */
/*          The row scale factors for A.  If EQUED = 'R' or 'B', A is */
/*          multiplied on the left by diag(R); if EQUED = 'N' or 'C', R */
/*          is not accessed.  R is an input argument if FACT = 'F'; */
/*          otherwise, R is an output argument.  If FACT = 'F' and */
/*          EQUED = 'R' or 'B', each element of R must be positive. */

/*  C       (input or output) DOUBLE PRECISION array, dimension (N) */
/*          The column scale factors for A.  If EQUED = 'C' or 'B', A is */
/*          multiplied on the right by diag(C); if EQUED = 'N' or 'R', C */
/*          is not accessed.  C is an input argument if FACT = 'F'; */
/*          otherwise, C is an output argument.  If FACT = 'F' and */
/*          EQUED = 'C' or 'B', each element of C must be positive. */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the N-by-NRHS right hand side matrix B. */
/*          On exit, */
/*          if EQUED = 'N', B is not modified; */
/*          if TRANS = 'N' and EQUED = 'R' or 'B', B is overwritten by */
/*          diag(R)*B; */
/*          if TRANS = 'T' or 'C' and EQUED = 'C' or 'B', B is */
/*          overwritten by diag(C)*B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (output) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          If INFO = 0 or INFO = N+1, the N-by-NRHS solution matrix X */
/*          to the original system of equations.  Note that A and B are */
/*          modified on exit if EQUED .ne. 'N', and the solution to the */
/*          equilibrated system is inv(diag(C))*X if TRANS = 'N' and */
/*          EQUED = 'C' or 'B', or inv(diag(R))*X if TRANS = 'T' or 'C' */
/*          and EQUED = 'R' or 'B'. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  RCOND   (output) DOUBLE PRECISION */
/*          The estimate of the reciprocal condition number of the matrix */
/*          A after equilibration (if done).  If RCOND is less than the */
/*          machine precision (in particular, if RCOND = 0), the matrix */
/*          is singular to working precision.  This condition is */
/*          indicated by a return code of INFO > 0. */

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

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (4*N) */
/*          On exit, WORK(1) contains the reciprocal pivot growth */
/*          factor norm(A)/norm(U). The "max absolute element" norm is */
/*          used. If WORK(1) is much less than 1, then the stability */
/*          of the LU factorization of the (equilibrated) matrix A */
/*          could be poor. This also means that the solution X, condition */
/*          estimator RCOND, and forward error bound FERR could be */
/*          unreliable. If factorization fails with 0<INFO<=N, then */
/*          WORK(1) contains the reciprocal pivot growth factor for the */
/*          leading INFO columns of A. */

/*  IWORK   (workspace) INTEGER array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, and i is */
/*                <= N:  U(i,i) is exactly zero.  The factorization has */
/*                       been completed, but the factor U is exactly */
/*                       singular, so the solution and error bounds */
/*                       could not be computed. RCOND = 0 is returned. */
/*                = N+1: U is nonsingular, but RCOND is less than machine */
/*                       precision, meaning that the matrix is singular */
/*                       to working precision.  Nevertheless, the */
/*                       solution and error bounds are computed because */
/*                       there are a number of situations where the */
/*                       computed solution can be more accurate than the */
/*                       value of RCOND would suggest. */

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
    af_dim1 = *ldaf;
    af_offset = 1 + af_dim1;
    af -= af_offset;
    --ipiv;
    --r__;
    --c__;
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
    nofact = lsame_(fact, "N");
    equil = lsame_(fact, "E");
    notran = lsame_(trans, "N");
    if (nofact || equil) {
	*(unsigned char *)equed = 'N';
	rowequ = false;
	colequ = false;
    } else {
	rowequ = lsame_(equed, "R") || lsame_(equed,
		"B");
	colequ = lsame_(equed, "C") || lsame_(equed,
		"B");
	smlnum = dlamch_("Safe minimum");
	bignum = 1. / smlnum;
    }

/*     Test the input parameters. */

    if (! nofact && ! equil && ! lsame_(fact, "F")) {
	*info = -1;
    } else if (! notran && ! lsame_(trans, "T") && !
	    lsame_(trans, "C")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*nrhs < 0) {
	*info = -4;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -6;
    } else if (*ldaf < std::max(1_integer,*n)) {
	*info = -8;
    } else if (lsame_(fact, "F") && ! (rowequ || colequ
	    || lsame_(equed, "N"))) {
	*info = -10;
    } else {
	if (rowequ) {
	    rcmin = bignum;
	    rcmax = 0.;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		d__1 = rcmin, d__2 = r__[j];
		rcmin = std::min(d__1,d__2);
/* Computing MAX */
		d__1 = rcmax, d__2 = r__[j];
		rcmax = std::max(d__1,d__2);
/* L10: */
	    }
	    if (rcmin <= 0.) {
		*info = -11;
	    } else if (*n > 0) {
		rowcnd = std::max(rcmin,smlnum) / std::min(rcmax,bignum);
	    } else {
		rowcnd = 1.;
	    }
	}
	if (colequ && *info == 0) {
	    rcmin = bignum;
	    rcmax = 0.;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		d__1 = rcmin, d__2 = c__[j];
		rcmin = std::min(d__1,d__2);
/* Computing MAX */
		d__1 = rcmax, d__2 = c__[j];
		rcmax = std::max(d__1,d__2);
/* L20: */
	    }
	    if (rcmin <= 0.) {
		*info = -12;
	    } else if (*n > 0) {
		colcnd = std::max(rcmin,smlnum) / std::min(rcmax,bignum);
	    } else {
		colcnd = 1.;
	    }
	}
	if (*info == 0) {
	    if (*ldb < std::max(1_integer,*n)) {
		*info = -14;
	    } else if (*ldx < std::max(1_integer,*n)) {
		*info = -16;
	    }
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGESVX", &i__1);
	return 0;
    }

    if (equil) {

/*        Compute row and column scalings to equilibrate the matrix A. */

	dgeequ_(n, n, &a[a_offset], lda, &r__[1], &c__[1], &rowcnd, &colcnd, &
		amax, &infequ);
	if (infequ == 0) {

/*           Equilibrate the matrix. */

	    dlaqge_(n, n, &a[a_offset], lda, &r__[1], &c__[1], &rowcnd, &
		    colcnd, &amax, equed);
	    rowequ = lsame_(equed, "R") || lsame_(equed,
		     "B");
	    colequ = lsame_(equed, "C") || lsame_(equed,
		     "B");
	}
    }

/*     Scale the right hand side. */

    if (notran) {
	if (rowequ) {
	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    b[i__ + j * b_dim1] = r__[i__] * b[i__ + j * b_dim1];
/* L30: */
		}
/* L40: */
	    }
	}
    } else if (colequ) {
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		b[i__ + j * b_dim1] = c__[i__] * b[i__ + j * b_dim1];
/* L50: */
	    }
/* L60: */
	}
    }

    if (nofact || equil) {

/*        Compute the LU factorization of A. */

	dlacpy_("Full", n, n, &a[a_offset], lda, &af[af_offset], ldaf);
	dgetrf_(n, n, &af[af_offset], ldaf, &ipiv[1], info);

/*        Return if INFO is non-zero. */

	if (*info > 0) {

/*           Compute the reciprocal pivot growth factor of the */
/*           leading rank-deficient INFO columns of A. */

	    rpvgrw = dlantr_("M", "U", "N", info, info, &af[af_offset], ldaf,
		    &work[1]);
	    if (rpvgrw == 0.) {
		rpvgrw = 1.;
	    } else {
		rpvgrw = dlange_("M", n, info, &a[a_offset], lda, &work[1]) / rpvgrw;
	    }
	    work[1] = rpvgrw;
	    *rcond = 0.;
	    return 0;
	}
    }

/*     Compute the norm of the matrix A and the */
/*     reciprocal pivot growth factor RPVGRW. */

    if (notran) {
	*(unsigned char *)norm = '1';
    } else {
	*(unsigned char *)norm = 'I';
    }
    anorm = dlange_(norm, n, n, &a[a_offset], lda, &work[1]);
    rpvgrw = dlantr_("M", "U", "N", n, n, &af[af_offset], ldaf, &work[1]);
    if (rpvgrw == 0.) {
	rpvgrw = 1.;
    } else {
	rpvgrw = dlange_("M", n, n, &a[a_offset], lda, &work[1]) /
		rpvgrw;
    }

/*     Compute the reciprocal of the condition number of A. */

    dgecon_(norm, n, &af[af_offset], ldaf, &anorm, rcond, &work[1], &iwork[1],
	     info);

/*     Compute the solution matrix X. */

    dlacpy_("Full", n, nrhs, &b[b_offset], ldb, &x[x_offset], ldx);
    dgetrs_(trans, n, nrhs, &af[af_offset], ldaf, &ipiv[1], &x[x_offset], ldx,
	     info);

/*     Use iterative refinement to improve the computed solution and */
/*     compute error bounds and backward error estimates for it. */

    dgerfs_(trans, n, nrhs, &a[a_offset], lda, &af[af_offset], ldaf, &ipiv[1],
	     &b[b_offset], ldb, &x[x_offset], ldx, &ferr[1], &berr[1], &work[
	    1], &iwork[1], info);

/*     Transform the solution matrix X to a solution of the original */
/*     system. */

    if (notran) {
	if (colequ) {
	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    x[i__ + j * x_dim1] = c__[i__] * x[i__ + j * x_dim1];
/* L70: */
		}
/* L80: */
	    }
	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		ferr[j] /= colcnd;
/* L90: */
	    }
	}
    } else if (rowequ) {
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		x[i__ + j * x_dim1] = r__[i__] * x[i__ + j * x_dim1];
/* L100: */
	    }
/* L110: */
	}
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    ferr[j] /= rowcnd;
/* L120: */
	}
    }

    work[1] = rpvgrw;

/*     Set INFO = N+1 if the matrix is singular to working precision. */

    if (*rcond < dlamch_("Epsilon")) {
	*info = *n + 1;
    }
    return 0;

/*     End of DGESVX */

} /* dgesvx_ */

/* Subroutine */ int dgetc2_(integer *n, double *a, integer *lda, integer
	*ipiv, integer *jpiv, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b10 = -1.;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    double d__1;

    /* Local variables */
    integer i__, j, ip, jp;
    double eps;
    integer ipv, jpv;
    double smin, xmax;
    double bignum, smlnum;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGETC2 computes an LU factorization with complete pivoting of the */
/*  n-by-n matrix A. The factorization has the form A = P * L * U * Q, */
/*  where P and Q are permutation matrices, L is lower triangular with */
/*  unit diagonal elements and U is upper triangular. */

/*  This is the Level 2 BLAS algorithm. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A. N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the n-by-n matrix A to be factored. */
/*          On exit, the factors L and U from the factorization */
/*          A = P*L*U*Q; the unit diagonal elements of L are not stored. */
/*          If U(k, k) appears to be less than SMIN, U(k, k) is given the */
/*          value of SMIN, i.e., giving a nonsingular perturbed system. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  IPIV    (output) INTEGER array, dimension(N). */
/*          The pivot indices; for 1 <= i <= N, row i of the */
/*          matrix has been interchanged with row IPIV(i). */

/*  JPIV    (output) INTEGER array, dimension(N). */
/*          The pivot indices; for 1 <= j <= N, column j of the */
/*          matrix has been interchanged with column JPIV(j). */

/*  INFO    (output) INTEGER */
/*           = 0: successful exit */
/*           > 0: if INFO = k, U(k, k) is likely to produce owerflow if */
/*                we try to solve for x in Ax = b. So U is perturbed to */
/*                avoid the overflow. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Bo Kagstrom and Peter Poromaa, Department of Computing Science, */
/*     Umea University, S-901 87 Umea, Sweden. */

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

/*     Set constants to control overflow */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --ipiv;
    --jpiv;

    /* Function Body */
    *info = 0;
    eps = dlamch_("P");
    smlnum = dlamch_("S") / eps;
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);

/*     Factorize A using complete pivoting. */
/*     Set pivots less than SMIN to SMIN. */

    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*        Find max element in matrix A */

	xmax = 0.;
	i__2 = *n;
	for (ip = i__; ip <= i__2; ++ip) {
	    i__3 = *n;
	    for (jp = i__; jp <= i__3; ++jp) {
		if ((d__1 = a[ip + jp * a_dim1], abs(d__1)) >= xmax) {
		    xmax = (d__1 = a[ip + jp * a_dim1], abs(d__1));
		    ipv = ip;
		    jpv = jp;
		}
/* L10: */
	    }
/* L20: */
	}
	if (i__ == 1) {
/* Computing MAX */
	    d__1 = eps * xmax;
	    smin = std::max(d__1,smlnum);
	}

/*        Swap rows */

	if (ipv != i__) {
	    dswap_(n, &a[ipv + a_dim1], lda, &a[i__ + a_dim1], lda);
	}
	ipiv[i__] = ipv;

/*        Swap columns */

	if (jpv != i__) {
	    dswap_(n, &a[jpv * a_dim1 + 1], &c__1, &a[i__ * a_dim1 + 1], &
		    c__1);
	}
	jpiv[i__] = jpv;

/*        Check for singularity */

	if ((d__1 = a[i__ + i__ * a_dim1], abs(d__1)) < smin) {
	    *info = i__;
	    a[i__ + i__ * a_dim1] = smin;
	}
	i__2 = *n;
	for (j = i__ + 1; j <= i__2; ++j) {
	    a[j + i__ * a_dim1] /= a[i__ + i__ * a_dim1];
/* L30: */
	}
	i__2 = *n - i__;
	i__3 = *n - i__;
	dger_(&i__2, &i__3, &c_b10, &a[i__ + 1 + i__ * a_dim1], &c__1, &a[i__
		+ (i__ + 1) * a_dim1], lda, &a[i__ + 1 + (i__ + 1) * a_dim1],
		lda);
/* L40: */
    }

    if ((d__1 = a[*n + *n * a_dim1], abs(d__1)) < smin) {
	*info = *n;
	a[*n + *n * a_dim1] = smin;
    }

    return 0;

/*     End of DGETC2 */

} /* dgetc2_ */

/* Subroutine */ int dgetf2_(integer *m, integer *n, double *a, integer *
	lda, integer *ipiv, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b8 = -1.;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    double d__1;

    /* Local variables */
    integer i__, j, jp;
    double sfmin;

/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGETF2 computes an LU factorization of a general m-by-n matrix A */
/*  using partial pivoting with row interchanges. */

/*  The factorization has the form */
/*     A = P * L * U */
/*  where P is a permutation matrix, L is lower triangular with unit */
/*  diagonal elements (lower trapezoidal if m > n), and U is upper */
/*  triangular (upper trapezoidal if m < n). */

/*  This is the right-looking Level 2 BLAS version of the algorithm. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the m by n matrix to be factored. */
/*          On exit, the factors L and U from the factorization */
/*          A = P*L*U; the unit diagonal elements of L are not stored. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  IPIV    (output) INTEGER array, dimension (min(M,N)) */
/*          The pivot indices; for 1 <= i <= min(M,N), row i of the */
/*          matrix was interchanged with row IPIV(i). */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -k, the k-th argument had an illegal value */
/*          > 0: if INFO = k, U(k,k) is exactly zero. The factorization */
/*               has been completed, but the factor U is exactly */
/*               singular, and division by zero will occur if it is used */
/*               to solve a system of equations. */

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
    --ipiv;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGETF2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	return 0;
    }

/*     Compute machine safe minimum */

    sfmin = dlamch_("S");

    i__1 = std::min(*m,*n);
    for (j = 1; j <= i__1; ++j) {

/*        Find pivot and test for singularity. */

	i__2 = *m - j + 1;
	jp = j - 1 + idamax_(&i__2, &a[j + j * a_dim1], &c__1);
	ipiv[j] = jp;
	if (a[jp + j * a_dim1] != 0.) {

/*           Apply the interchange to columns 1:N. */

	    if (jp != j) {
		dswap_(n, &a[j + a_dim1], lda, &a[jp + a_dim1], lda);
	    }

/*           Compute elements J+1:M of J-th column. */

	    if (j < *m) {
		if ((d__1 = a[j + j * a_dim1], abs(d__1)) >= sfmin) {
		    i__2 = *m - j;
		    d__1 = 1. / a[j + j * a_dim1];
		    dscal_(&i__2, &d__1, &a[j + 1 + j * a_dim1], &c__1);
		} else {
		    i__2 = *m - j;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			a[j + i__ + j * a_dim1] /= a[j + j * a_dim1];
/* L20: */
		    }
		}
	    }

	} else if (*info == 0) {

	    *info = j;
	}

	if (j < std::min(*m,*n)) {

/*           Update trailing submatrix. */

	    i__2 = *m - j;
	    i__3 = *n - j;
	    dger_(&i__2, &i__3, &c_b8, &a[j + 1 + j * a_dim1], &c__1, &a[j + (
		    j + 1) * a_dim1], lda, &a[j + 1 + (j + 1) * a_dim1], lda);
	}
/* L10: */
    }
    return 0;

/*     End of DGETF2 */

} /* dgetf2_ */

/* Subroutine */ int dgetrf_(integer *m, integer *n, double *a, integer *
	lda, integer *ipiv, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static double c_b16 = 1.;
	static double c_b19 = -1.;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4, i__5;

    /* Local variables */
    integer i__, j, jb, nb;
    integer iinfo;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGETRF computes an LU factorization of a general M-by-N matrix A */
/*  using partial pivoting with row interchanges. */

/*  The factorization has the form */
/*     A = P * L * U */
/*  where P is a permutation matrix, L is lower triangular with unit */
/*  diagonal elements (lower trapezoidal if m > n), and U is upper */
/*  triangular (upper trapezoidal if m < n). */

/*  This is the right-looking Level 3 BLAS version of the algorithm. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix to be factored. */
/*          On exit, the factors L and U from the factorization */
/*          A = P*L*U; the unit diagonal elements of L are not stored. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  IPIV    (output) INTEGER array, dimension (min(M,N)) */
/*          The pivot indices; for 1 <= i <= min(M,N), row i of the */
/*          matrix was interchanged with row IPIV(i). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, U(i,i) is exactly zero. The factorization */
/*                has been completed, but the factor U is exactly */
/*                singular, and division by zero will occur if it is used */
/*                to solve a system of equations. */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --ipiv;

    /* Function Body */
    *info = 0;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGETRF", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*m == 0 || *n == 0) {
	return 0;
    }

/*     Determine the block size for this environment. */

    nb = ilaenv_(&c__1, "DGETRF", " ", m, n, &c_n1, &c_n1);
    if (nb <= 1 || nb >= std::min(*m,*n)) {

/*        Use unblocked code. */

	dgetf2_(m, n, &a[a_offset], lda, &ipiv[1], info);
    } else {

/*        Use blocked code. */

	i__1 = std::min(*m,*n);
	i__2 = nb;
	for (j = 1; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {
/* Computing MIN */
	    i__3 = std::min(*m,*n) - j + 1;
	    jb = std::min(i__3,nb);

/*           Factor diagonal and subdiagonal blocks and test for exact */
/*           singularity. */

	    i__3 = *m - j + 1;
	    dgetf2_(&i__3, &jb, &a[j + j * a_dim1], lda, &ipiv[j], &iinfo);

/*           Adjust INFO and the pivot indices. */

	    if (*info == 0 && iinfo > 0) {
		*info = iinfo + j - 1;
	    }
/* Computing MIN */
	    i__4 = *m, i__5 = j + jb - 1;
	    i__3 = std::min(i__4,i__5);
	    for (i__ = j; i__ <= i__3; ++i__) {
		ipiv[i__] = j - 1 + ipiv[i__];
/* L10: */
	    }

/*           Apply interchanges to columns 1:J-1. */

	    i__3 = j - 1;
	    i__4 = j + jb - 1;
	    dlaswp_(&i__3, &a[a_offset], lda, &j, &i__4, &ipiv[1], &c__1);

	    if (j + jb <= *n) {

/*              Apply interchanges to columns J+JB:N. */

		i__3 = *n - j - jb + 1;
		i__4 = j + jb - 1;
		dlaswp_(&i__3, &a[(j + jb) * a_dim1 + 1], lda, &j, &i__4, &
			ipiv[1], &c__1);

/*              Compute block row of U. */

		i__3 = *n - j - jb + 1;
		dtrsm_("Left", "Lower", "No transpose", "Unit", &jb, &i__3, &
			c_b16, &a[j + j * a_dim1], lda, &a[j + (j + jb) *
			a_dim1], lda);
		if (j + jb <= *m) {

/*                 Update trailing submatrix. */

		    i__3 = *m - j - jb + 1;
		    i__4 = *n - j - jb + 1;
		    dgemm_("No transpose", "No transpose", &i__3, &i__4, &jb,
			    &c_b19, &a[j + jb + j * a_dim1], lda, &a[j + (j +
			    jb) * a_dim1], lda, &c_b16, &a[j + jb + (j + jb) *
			     a_dim1], lda);
		}
	    }
/* L20: */
	}
    }
    return 0;

/*     End of DGETRF */

} /* dgetrf_ */

/* Subroutine */ int dgetri_(integer *n, double *a, integer *lda, integer
	*ipiv, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__2 = 2;
	static double c_b20 = -1.;
	static double c_b22 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    integer i__, j, jb, nb, jj, jp, nn, iws;
    integer nbmin;
    integer ldwork;
	integer lwkopt;
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

/*  DGETRI computes the inverse of a matrix using the LU factorization */
/*  computed by DGETRF. */

/*  This method inverts U and then computes inv(A) by solving the system */
/*  inv(A)*L = inv(U) for inv(A). */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the factors L and U from the factorization */
/*          A = P*L*U as computed by DGETRF. */
/*          On exit, if INFO = 0, the inverse of the original matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices from DGETRF; for 1<=i<=N, row i of the */
/*          matrix was interchanged with row IPIV(i). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO=0, then WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= max(1,N). */
/*          For optimal performance LWORK >= N*NB, where NB is */
/*          the optimal blocksize returned by ILAENV. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, U(i,i) is exactly zero; the matrix is */
/*                singular and its inverse could not be computed. */

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
    --ipiv;
    --work;

    /* Function Body */
    *info = 0;
    nb = ilaenv_(&c__1, "DGETRI", " ", n, &c_n1, &c_n1, &c_n1);
    lwkopt = *n * nb;
    work[1] = (double) lwkopt;
    lquery = *lwork == -1;
    if (*n < 0) {
	*info = -1;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -3;
    } else if (*lwork < std::max(1_integer,*n) && ! lquery) {
	*info = -6;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGETRI", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Form inv(U).  If INFO > 0 from DTRTRI, then U is singular, */
/*     and the inverse is not computed. */

    dtrtri_("Upper", "Non-unit", n, &a[a_offset], lda, info);
    if (*info > 0) {
	return 0;
    }

    nbmin = 2;
    ldwork = *n;
    if (nb > 1 && nb < *n) {
/* Computing MAX */
	i__1 = ldwork * nb;
	iws = std::max(i__1,1_integer);
	if (*lwork < iws) {
	    nb = *lwork / ldwork;
/* Computing MAX */
	    i__1 = 2, i__2 = ilaenv_(&c__2, "DGETRI", " ", n, &c_n1, &c_n1, &
		    c_n1);
	    nbmin = std::max(i__1,i__2);
	}
    } else {
	iws = *n;
    }

/*     Solve the equation inv(A)*L = inv(U) for inv(A). */

    if (nb < nbmin || nb >= *n) {

/*        Use unblocked code. */

	for (j = *n; j >= 1; --j) {

/*           Copy current column of L to WORK and replace with zeros. */

	    i__1 = *n;
	    for (i__ = j + 1; i__ <= i__1; ++i__) {
		work[i__] = a[i__ + j * a_dim1];
		a[i__ + j * a_dim1] = 0.;
/* L10: */
	    }

/*           Compute current column of inv(A). */

	    if (j < *n) {
		i__1 = *n - j;
		dgemv_("No transpose", n, &i__1, &c_b20, &a[(j + 1) * a_dim1
			+ 1], lda, &work[j + 1], &c__1, &c_b22, &a[j * a_dim1
			+ 1], &c__1);
	    }
/* L20: */
	}
    } else {

/*        Use blocked code. */

	nn = (*n - 1) / nb * nb + 1;
	i__1 = -nb;
	for (j = nn; i__1 < 0 ? j >= 1 : j <= 1; j += i__1) {
/* Computing MIN */
	    i__2 = nb, i__3 = *n - j + 1;
	    jb = std::min(i__2,i__3);

/*           Copy current block column of L to WORK and replace with */
/*           zeros. */

	    i__2 = j + jb - 1;
	    for (jj = j; jj <= i__2; ++jj) {
		i__3 = *n;
		for (i__ = jj + 1; i__ <= i__3; ++i__) {
		    work[i__ + (jj - j) * ldwork] = a[i__ + jj * a_dim1];
		    a[i__ + jj * a_dim1] = 0.;
/* L30: */
		}
/* L40: */
	    }

/*           Compute current block column of inv(A). */

	    if (j + jb <= *n) {
		i__2 = *n - j - jb + 1;
		dgemm_("No transpose", "No transpose", n, &jb, &i__2, &c_b20,
			&a[(j + jb) * a_dim1 + 1], lda, &work[j + jb], &
			ldwork, &c_b22, &a[j * a_dim1 + 1], lda);
	    }
	    dtrsm_("Right", "Lower", "No transpose", "Unit", n, &jb, &c_b22, &
		    work[j], &ldwork, &a[j * a_dim1 + 1], lda);
/* L50: */
	}
    }

/*     Apply column interchanges. */

    for (j = *n - 1; j >= 1; --j) {
	jp = ipiv[j];
	if (jp != j) {
	    dswap_(n, &a[j * a_dim1 + 1], &c__1, &a[jp * a_dim1 + 1], &c__1);
	}
/* L60: */
    }

    work[1] = (double) iws;
    return 0;

/*     End of DGETRI */

} /* dgetri_ */

/* Subroutine */ int dgetrs_(const char *trans, integer *n, integer *nrhs,
	double *a, integer *lda, integer *ipiv, double *b, integer *
	ldb, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b12 = 1.;
	static integer c_n1 = -1;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1;

    /* Local variables */
    bool notran;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGETRS solves a system of linear equations */
/*     A * X = B  or  A' * X = B */
/*  with a general N-by-N matrix A using the LU factorization computed */
/*  by DGETRF. */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations: */
/*          = 'N':  A * X = B  (No transpose) */
/*          = 'T':  A'* X = B  (Transpose) */
/*          = 'C':  A'* X = B  (Conjugate transpose = Transpose) */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The factors L and U from the factorization A = P*L*U */
/*          as computed by DGETRF. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices from DGETRF; for 1<=i<=N, row i of the */
/*          matrix was interchanged with row IPIV(i). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the right hand side matrix B. */
/*          On exit, the solution matrix X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

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
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --ipiv;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    *info = 0;
    notran = lsame_(trans, "N");
    if (! notran && ! lsame_(trans, "T") && ! lsame_(
	    trans, "C")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGETRS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *nrhs == 0) {
	return 0;
    }

    if (notran) {

/*        Solve A * X = B. */

/*        Apply row interchanges to the right hand sides. */

	dlaswp_(nrhs, &b[b_offset], ldb, &c__1, n, &ipiv[1], &c__1);

/*        Solve L*X = B, overwriting B with X. */

	dtrsm_("Left", "Lower", "No transpose", "Unit", n, nrhs, &c_b12, &a[
		a_offset], lda, &b[b_offset], ldb);

/*        Solve U*X = B, overwriting B with X. */

	dtrsm_("Left", "Upper", "No transpose", "Non-unit", n, nrhs, &c_b12, &
		a[a_offset], lda, &b[b_offset], ldb);
    } else {

/*        Solve A' * X = B. */

/*        Solve U'*X = B, overwriting B with X. */

	dtrsm_("Left", "Upper", "Transpose", "Non-unit", n, nrhs, &c_b12, &a[
		a_offset], lda, &b[b_offset], ldb);

/*        Solve L'*X = B, overwriting B with X. */

	dtrsm_("Left", "Lower", "Transpose", "Unit", n, nrhs, &c_b12, &a[
		a_offset], lda, &b[b_offset], ldb);

/*        Apply row interchanges to the solution vectors. */

	dlaswp_(nrhs, &b[b_offset], ldb, &c__1, n, &ipiv[1], &c_n1);
    }

    return 0;

/*     End of DGETRS */

} /* dgetrs_ */

/* Subroutine */ int dggbak_(const char *job, const char *side, integer *n, integer *ilo,
	integer *ihi, double *lscale, double *rscale, integer *m,
	double *v, integer *ldv, integer *info)
{
    /* System generated locals */
    integer v_dim1, v_offset, i__1;

    /* Local variables */
    integer i__, k;
    bool leftv;
    bool rightv;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGGBAK forms the right or left eigenvectors of a real generalized */
/*  eigenvalue problem A*x = lambda*B*x, by backward transformation on */
/*  the computed eigenvectors of the balanced pair of matrices output by */
/*  DGGBAL. */

/*  Arguments */
/*  ========= */

/*  JOB     (input) CHARACTER*1 */
/*          Specifies the type of backward transformation required: */
/*          = 'N':  do nothing, return immediately; */
/*          = 'P':  do backward transformation for permutation only; */
/*          = 'S':  do backward transformation for scaling only; */
/*          = 'B':  do backward transformations for both permutation and */
/*                  scaling. */
/*          JOB must be the same as the argument JOB supplied to DGGBAL. */

/*  SIDE    (input) CHARACTER*1 */
/*          = 'R':  V contains right eigenvectors; */
/*          = 'L':  V contains left eigenvectors. */

/*  N       (input) INTEGER */
/*          The number of rows of the matrix V.  N >= 0. */

/*  ILO     (input) INTEGER */
/*  IHI     (input) INTEGER */
/*          The integers ILO and IHI determined by DGGBAL. */
/*          1 <= ILO <= IHI <= N, if N > 0; ILO=1 and IHI=0, if N=0. */

/*  LSCALE  (input) DOUBLE PRECISION array, dimension (N) */
/*          Details of the permutations and/or scaling factors applied */
/*          to the left side of A and B, as returned by DGGBAL. */

/*  RSCALE  (input) DOUBLE PRECISION array, dimension (N) */
/*          Details of the permutations and/or scaling factors applied */
/*          to the right side of A and B, as returned by DGGBAL. */

/*  M       (input) INTEGER */
/*          The number of columns of the matrix V.  M >= 0. */

/*  V       (input/output) DOUBLE PRECISION array, dimension (LDV,M) */
/*          On entry, the matrix of right or left eigenvectors to be */
/*          transformed, as returned by DTGEVC. */
/*          On exit, V is overwritten by the transformed eigenvectors. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of the matrix V. LDV >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  See R.C. Ward, Balancing the generalized eigenvalue problem, */
/*                 SIAM J. Sci. Stat. Comp. 2 (1981), 141-152. */

/*  ===================================================================== */

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
    --lscale;
    --rscale;
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;

    /* Function Body */
    rightv = lsame_(side, "R");
    leftv = lsame_(side, "L");

    *info = 0;
    if (! lsame_(job, "N") && ! lsame_(job, "P") && ! lsame_(job, "S")
	    && ! lsame_(job, "B")) {
	*info = -1;
    } else if (! rightv && ! leftv) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ilo < 1) {
	*info = -4;
    } else if (*n == 0 && *ihi == 0 && *ilo != 1) {
	*info = -4;
    } else if (*n > 0 && (*ihi < *ilo || *ihi > std::max(1_integer,*n))) {
	*info = -5;
    } else if (*n == 0 && *ilo == 1 && *ihi != 0) {
	*info = -5;
    } else if (*m < 0) {
	*info = -8;
    } else if (*ldv < std::max(1_integer,*n)) {
	*info = -10;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGGBAK", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }
    if (*m == 0) {
	return 0;
    }
    if (lsame_(job, "N")) {
	return 0;
    }

    if (*ilo == *ihi) {
	goto L30;
    }

/*     Backward balance */

    if (lsame_(job, "S") || lsame_(job, "B")) {

/*        Backward transformation on right eigenvectors */

	if (rightv) {
	    i__1 = *ihi;
	    for (i__ = *ilo; i__ <= i__1; ++i__) {
		dscal_(m, &rscale[i__], &v[i__ + v_dim1], ldv);
/* L10: */
	    }
	}

/*        Backward transformation on left eigenvectors */

	if (leftv) {
	    i__1 = *ihi;
	    for (i__ = *ilo; i__ <= i__1; ++i__) {
		dscal_(m, &lscale[i__], &v[i__ + v_dim1], ldv);
/* L20: */
	    }
	}
    }

/*     Backward permutation */

L30:
    if (lsame_(job, "P") || lsame_(job, "B")) {

/*        Backward permutation on right eigenvectors */

	if (rightv) {
	    if (*ilo == 1) {
		goto L50;
	    }

	    for (i__ = *ilo - 1; i__ >= 1; --i__) {
		k = (integer) rscale[i__];
		if (k == i__) {
		    goto L40;
		}
		dswap_(m, &v[i__ + v_dim1], ldv, &v[k + v_dim1], ldv);
L40:
		;
	    }

L50:
	    if (*ihi == *n) {
		goto L70;
	    }
	    i__1 = *n;
	    for (i__ = *ihi + 1; i__ <= i__1; ++i__) {
		k = (integer) rscale[i__];
		if (k == i__) {
		    goto L60;
		}
		dswap_(m, &v[i__ + v_dim1], ldv, &v[k + v_dim1], ldv);
L60:
		;
	    }
	}

/*        Backward permutation on left eigenvectors */

L70:
	if (leftv) {
	    if (*ilo == 1) {
		goto L90;
	    }
	    for (i__ = *ilo - 1; i__ >= 1; --i__) {
		k = (integer) lscale[i__];
		if (k == i__) {
		    goto L80;
		}
		dswap_(m, &v[i__ + v_dim1], ldv, &v[k + v_dim1], ldv);
L80:
		;
	    }

L90:
	    if (*ihi == *n) {
		goto L110;
	    }
	    i__1 = *n;
	    for (i__ = *ihi + 1; i__ <= i__1; ++i__) {
		k = (integer) lscale[i__];
		if (k == i__) {
		    goto L100;
		}
		dswap_(m, &v[i__ + v_dim1], ldv, &v[k + v_dim1], ldv);
L100:
		;
	    }
	}
    }

L110:

    return 0;

/*     End of DGGBAK */

} /* dggbak_ */


/* Subroutine */ int dggbal_(const char *job, integer *n, double *a, integer *
	lda, double *b, integer *ldb, integer *ilo, integer *ihi,
	double *lscale, double *rscale, double *work, integer *
	info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b35 = 10.;
	static double c_b71 = .5;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2, i__3;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, k, l, m;
    double t;
    integer jc;
    double ta, tb, tc;
    integer ir;
    double ew;
    integer it, nr, ip1, jp1, lm1;
    double cab, rab, ewc, cor, sum;
    integer nrp2, icab, lcab;
    double beta, coef;
    integer irab, lrab;
    double basl, cmax;
    double coef2, coef5, gamma, alpha;
    double sfmin, sfmax;
    integer iflow;
     integer kount;
    double pgamma;
    integer lsfmin, lsfmax;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGGBAL balances a pair of general real matrices (A,B).  This */
/*  involves, first, permuting A and B by similarity transformations to */
/*  isolate eigenvalues in the first 1 to ILO$-$1 and last IHI+1 to N */
/*  elements on the diagonal; and second, applying a diagonal similarity */
/*  transformation to rows and columns ILO to IHI to make the rows */
/*  and columns as close in norm as possible. Both steps are optional. */

/*  Balancing may reduce the 1-norm of the matrices, and improve the */
/*  accuracy of the computed eigenvalues and/or eigenvectors in the */
/*  generalized eigenvalue problem A*x = lambda*B*x. */

/*  Arguments */
/*  ========= */

/*  JOB     (input) CHARACTER*1 */
/*          Specifies the operations to be performed on A and B: */
/*          = 'N':  none:  simply set ILO = 1, IHI = N, LSCALE(I) = 1.0 */
/*                  and RSCALE(I) = 1.0 for i = 1,...,N. */
/*          = 'P':  permute only; */
/*          = 'S':  scale only; */
/*          = 'B':  both permute and scale. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the input matrix A. */
/*          On exit,  A is overwritten by the balanced matrix. */
/*          If JOB = 'N', A is not referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,N) */
/*          On entry, the input matrix B. */
/*          On exit,  B is overwritten by the balanced matrix. */
/*          If JOB = 'N', B is not referenced. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,N). */

/*  ILO     (output) INTEGER */
/*  IHI     (output) INTEGER */
/*          ILO and IHI are set to integers such that on exit */
/*          A(i,j) = 0 and B(i,j) = 0 if i > j and */
/*          j = 1,...,ILO-1 or i = IHI+1,...,N. */
/*          If JOB = 'N' or 'S', ILO = 1 and IHI = N. */

/*  LSCALE  (output) DOUBLE PRECISION array, dimension (N) */
/*          Details of the permutations and scaling factors applied */
/*          to the left side of A and B.  If P(j) is the index of the */
/*          row interchanged with row j, and D(j) */
/*          is the scaling factor applied to row j, then */
/*            LSCALE(j) = P(j)    for J = 1,...,ILO-1 */
/*                      = D(j)    for J = ILO,...,IHI */
/*                      = P(j)    for J = IHI+1,...,N. */
/*          The order in which the interchanges are made is N to IHI+1, */
/*          then 1 to ILO-1. */

/*  RSCALE  (output) DOUBLE PRECISION array, dimension (N) */
/*          Details of the permutations and scaling factors applied */
/*          to the right side of A and B.  If P(j) is the index of the */
/*          column interchanged with column j, and D(j) */
/*          is the scaling factor applied to column j, then */
/*            LSCALE(j) = P(j)    for J = 1,...,ILO-1 */
/*                      = D(j)    for J = ILO,...,IHI */
/*                      = P(j)    for J = IHI+1,...,N. */
/*          The order in which the interchanges are made is N to IHI+1, */
/*          then 1 to ILO-1. */

/*  WORK    (workspace) REAL array, dimension (lwork) */
/*          lwork must be at least max(1,6*N) when JOB = 'S' or 'B', and */
/*          at least 1 when JOB = 'N' or 'P'. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  See R.C. WARD, Balancing the generalized eigenvalue problem, */
/*                 SIAM J. Sci. Stat. Comp. 2 (1981), 141-152. */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --lscale;
    --rscale;
    --work;

    /* Function Body */
    *info = 0;
    if (! lsame_(job, "N") && ! lsame_(job, "P") && ! lsame_(job, "S")
	    && ! lsame_(job, "B")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -4;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -6;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGGBAL", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	*ilo = 1;
	*ihi = *n;
	return 0;
    }

    if (*n == 1) {
	*ilo = 1;
	*ihi = *n;
	lscale[1] = 1.;
	rscale[1] = 1.;
	return 0;
    }

    if (lsame_(job, "N")) {
	*ilo = 1;
	*ihi = *n;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    lscale[i__] = 1.;
	    rscale[i__] = 1.;
/* L10: */
	}
	return 0;
    }

    k = 1;
    l = *n;
    if (lsame_(job, "S")) {
	goto L190;
    }

    goto L30;

/*     Permute the matrices A and B to isolate the eigenvalues. */

/*     Find row with one nonzero in columns 1 through L */

L20:
    l = lm1;
    if (l != 1) {
	goto L30;
    }

    rscale[1] = 1.;
    lscale[1] = 1.;
    goto L190;

L30:
    lm1 = l - 1;
    for (i__ = l; i__ >= 1; --i__) {
	i__1 = lm1;
	for (j = 1; j <= i__1; ++j) {
	    jp1 = j + 1;
	    if (a[i__ + j * a_dim1] != 0. || b[i__ + j * b_dim1] != 0.) {
		goto L50;
	    }
/* L40: */
	}
	j = l;
	goto L70;

L50:
	i__1 = l;
	for (j = jp1; j <= i__1; ++j) {
	    if (a[i__ + j * a_dim1] != 0. || b[i__ + j * b_dim1] != 0.) {
		goto L80;
	    }
/* L60: */
	}
	j = jp1 - 1;

L70:
	m = l;
	iflow = 1;
	goto L160;
L80:
	;
    }
    goto L100;

/*     Find column with one nonzero in rows K through N */

L90:
    ++k;

L100:
    i__1 = l;
    for (j = k; j <= i__1; ++j) {
	i__2 = lm1;
	for (i__ = k; i__ <= i__2; ++i__) {
	    ip1 = i__ + 1;
	    if (a[i__ + j * a_dim1] != 0. || b[i__ + j * b_dim1] != 0.) {
		goto L120;
	    }
/* L110: */
	}
	i__ = l;
	goto L140;
L120:
	i__2 = l;
	for (i__ = ip1; i__ <= i__2; ++i__) {
	    if (a[i__ + j * a_dim1] != 0. || b[i__ + j * b_dim1] != 0.) {
		goto L150;
	    }
/* L130: */
	}
	i__ = ip1 - 1;
L140:
	m = k;
	iflow = 2;
	goto L160;
L150:
	;
    }
    goto L190;

/*     Permute rows M and I */

L160:
    lscale[m] = (double) i__;
    if (i__ == m) {
	goto L170;
    }
    i__1 = *n - k + 1;
    dswap_(&i__1, &a[i__ + k * a_dim1], lda, &a[m + k * a_dim1], lda);
    i__1 = *n - k + 1;
    dswap_(&i__1, &b[i__ + k * b_dim1], ldb, &b[m + k * b_dim1], ldb);

/*     Permute columns M and J */

L170:
    rscale[m] = (double) j;
    if (j == m) {
	goto L180;
    }
    dswap_(&l, &a[j * a_dim1 + 1], &c__1, &a[m * a_dim1 + 1], &c__1);
    dswap_(&l, &b[j * b_dim1 + 1], &c__1, &b[m * b_dim1 + 1], &c__1);

L180:
    switch (iflow) {
	case 1:  goto L20;
	case 2:  goto L90;
    }

L190:
    *ilo = k;
    *ihi = l;

    if (lsame_(job, "P")) {
	i__1 = *ihi;
	for (i__ = *ilo; i__ <= i__1; ++i__) {
	    lscale[i__] = 1.;
	    rscale[i__] = 1.;
/* L195: */
	}
	return 0;
    }

    if (*ilo == *ihi) {
	return 0;
    }

/*     Balance the submatrix in rows ILO to IHI. */

    nr = *ihi - *ilo + 1;
    i__1 = *ihi;
    for (i__ = *ilo; i__ <= i__1; ++i__) {
	rscale[i__] = 0.;
	lscale[i__] = 0.;

	work[i__] = 0.;
	work[i__ + *n] = 0.;
	work[i__ + (*n << 1)] = 0.;
	work[i__ + *n * 3] = 0.;
	work[i__ + (*n << 2)] = 0.;
	work[i__ + *n * 5] = 0.;
/* L200: */
    }

/*     Compute right side vector in resulting linear equations */

    basl = d_lg10(&c_b35);
    i__1 = *ihi;
    for (i__ = *ilo; i__ <= i__1; ++i__) {
	i__2 = *ihi;
	for (j = *ilo; j <= i__2; ++j) {
	    tb = b[i__ + j * b_dim1];
	    ta = a[i__ + j * a_dim1];
	    if (ta == 0.) {
		goto L210;
	    }
	    d__1 = abs(ta);
	    ta = d_lg10(&d__1) / basl;
L210:
	    if (tb == 0.) {
		goto L220;
	    }
	    d__1 = abs(tb);
	    tb = d_lg10(&d__1) / basl;
L220:
	    work[i__ + (*n << 2)] = work[i__ + (*n << 2)] - ta - tb;
	    work[j + *n * 5] = work[j + *n * 5] - ta - tb;
/* L230: */
	}
/* L240: */
    }

    coef = 1. / (double) (nr << 1);
    coef2 = coef * coef;
    coef5 = coef2 * .5;
    nrp2 = nr + 2;
    beta = 0.;
    it = 1;

/*     Start generalized conjugate gradient iteration */

L250:

    gamma = ddot_(&nr, &work[*ilo + (*n << 2)], &c__1, &work[*ilo + (*n << 2)]
, &c__1) + ddot_(&nr, &work[*ilo + *n * 5], &c__1, &work[*ilo + *
	    n * 5], &c__1);

    ew = 0.;
    ewc = 0.;
    i__1 = *ihi;
    for (i__ = *ilo; i__ <= i__1; ++i__) {
	ew += work[i__ + (*n << 2)];
	ewc += work[i__ + *n * 5];
/* L260: */
    }

/* Computing 2nd power */
    d__1 = ew;
/* Computing 2nd power */
    d__2 = ewc;
/* Computing 2nd power */
    d__3 = ew - ewc;
    gamma = coef * gamma - coef2 * (d__1 * d__1 + d__2 * d__2) - coef5 * (
	    d__3 * d__3);
    if (gamma == 0.) {
	goto L350;
    }
    if (it != 1) {
	beta = gamma / pgamma;
    }
    t = coef5 * (ewc - ew * 3.);
    tc = coef5 * (ew - ewc * 3.);

    dscal_(&nr, &beta, &work[*ilo], &c__1);
    dscal_(&nr, &beta, &work[*ilo + *n], &c__1);

    daxpy_(&nr, &coef, &work[*ilo + (*n << 2)], &c__1, &work[*ilo + *n], &
	    c__1);
    daxpy_(&nr, &coef, &work[*ilo + *n * 5], &c__1, &work[*ilo], &c__1);

    i__1 = *ihi;
    for (i__ = *ilo; i__ <= i__1; ++i__) {
	work[i__] += tc;
	work[i__ + *n] += t;
/* L270: */
    }

/*     Apply matrix to vector */

    i__1 = *ihi;
    for (i__ = *ilo; i__ <= i__1; ++i__) {
	kount = 0;
	sum = 0.;
	i__2 = *ihi;
	for (j = *ilo; j <= i__2; ++j) {
	    if (a[i__ + j * a_dim1] == 0.) {
		goto L280;
	    }
	    ++kount;
	    sum += work[j];
L280:
	    if (b[i__ + j * b_dim1] == 0.) {
		goto L290;
	    }
	    ++kount;
	    sum += work[j];
L290:
	    ;
	}
	work[i__ + (*n << 1)] = (double) kount * work[i__ + *n] + sum;
/* L300: */
    }

    i__1 = *ihi;
    for (j = *ilo; j <= i__1; ++j) {
	kount = 0;
	sum = 0.;
	i__2 = *ihi;
	for (i__ = *ilo; i__ <= i__2; ++i__) {
	    if (a[i__ + j * a_dim1] == 0.) {
		goto L310;
	    }
	    ++kount;
	    sum += work[i__ + *n];
L310:
	    if (b[i__ + j * b_dim1] == 0.) {
		goto L320;
	    }
	    ++kount;
	    sum += work[i__ + *n];
L320:
	    ;
	}
	work[j + *n * 3] = (double) kount * work[j] + sum;
/* L330: */
    }

    sum = ddot_(&nr, &work[*ilo + *n], &c__1, &work[*ilo + (*n << 1)], &c__1)
	    + ddot_(&nr, &work[*ilo], &c__1, &work[*ilo + *n * 3], &c__1);
    alpha = gamma / sum;

/*     Determine correction to current iteration */

    cmax = 0.;
    i__1 = *ihi;
    for (i__ = *ilo; i__ <= i__1; ++i__) {
	cor = alpha * work[i__ + *n];
	if (abs(cor) > cmax) {
	    cmax = abs(cor);
	}
	lscale[i__] += cor;
	cor = alpha * work[i__];
	if (abs(cor) > cmax) {
	    cmax = abs(cor);
	}
	rscale[i__] += cor;
/* L340: */
    }
    if (cmax < .5) {
	goto L350;
    }

    d__1 = -alpha;
    daxpy_(&nr, &d__1, &work[*ilo + (*n << 1)], &c__1, &work[*ilo + (*n << 2)]
, &c__1);
    d__1 = -alpha;
    daxpy_(&nr, &d__1, &work[*ilo + *n * 3], &c__1, &work[*ilo + *n * 5], &
	    c__1);

    pgamma = gamma;
    ++it;
    if (it <= nrp2) {
	goto L250;
    }

/*     End generalized conjugate gradient iteration */

L350:
    sfmin = dlamch_("S");
    sfmax = 1. / sfmin;
    lsfmin = (integer) (d_lg10(&sfmin) / basl + 1.);
    lsfmax = (integer) (d_lg10(&sfmax) / basl);
    i__1 = *ihi;
    for (i__ = *ilo; i__ <= i__1; ++i__) {
	i__2 = *n - *ilo + 1;
	irab = idamax_(&i__2, &a[i__ + *ilo * a_dim1], lda);
	rab = (d__1 = a[i__ + (irab + *ilo - 1) * a_dim1], abs(d__1));
	i__2 = *n - *ilo + 1;
	irab = idamax_(&i__2, &b[i__ + *ilo * b_dim1], ldb);
/* Computing MAX */
	d__2 = rab, d__3 = (d__1 = b[i__ + (irab + *ilo - 1) * b_dim1], abs(
		d__1));
	rab = std::max(d__2,d__3);
	d__1 = rab + sfmin;
	lrab = (integer) (d_lg10(&d__1) / basl + 1.);
	ir = (integer) (lscale[i__] + d_sign(&c_b71, &lscale[i__]));
/* Computing MIN */
	i__2 = std::max(ir,lsfmin), i__2 = std::min(i__2,lsfmax), i__3 = lsfmax - lrab;
	ir = std::min(i__2,i__3);
	lscale[i__] = pow_di(&c_b35, &ir);
	icab = idamax_(ihi, &a[i__ * a_dim1 + 1], &c__1);
	cab = (d__1 = a[icab + i__ * a_dim1], abs(d__1));
	icab = idamax_(ihi, &b[i__ * b_dim1 + 1], &c__1);
/* Computing MAX */
	d__2 = cab, d__3 = (d__1 = b[icab + i__ * b_dim1], abs(d__1));
	cab = std::max(d__2,d__3);
	d__1 = cab + sfmin;
	lcab = (integer) (d_lg10(&d__1) / basl + 1.);
	jc = (integer) (rscale[i__] + d_sign(&c_b71, &rscale[i__]));
/* Computing MIN */
	i__2 = std::max(jc,lsfmin), i__2 = std::min(i__2,lsfmax), i__3 = lsfmax - lcab;
	jc = std::min(i__2,i__3);
	rscale[i__] = pow_di(&c_b35, &jc);
/* L360: */
    }

/*     Row scaling of matrices A and B */

    i__1 = *ihi;
    for (i__ = *ilo; i__ <= i__1; ++i__) {
	i__2 = *n - *ilo + 1;
	dscal_(&i__2, &lscale[i__], &a[i__ + *ilo * a_dim1], lda);
	i__2 = *n - *ilo + 1;
	dscal_(&i__2, &lscale[i__], &b[i__ + *ilo * b_dim1], ldb);
/* L370: */
    }

/*     Column scaling of matrices A and B */

    i__1 = *ihi;
    for (j = *ilo; j <= i__1; ++j) {
	dscal_(ihi, &rscale[j], &a[j * a_dim1 + 1], &c__1);
	dscal_(ihi, &rscale[j], &b[j * b_dim1 + 1], &c__1);
/* L380: */
    }

    return 0;

/*     End of DGGBAL */

} /* dggbal_ */

/* Subroutine */ int dgges_(const char *jobvsl, const char *jobvsr, const char *sort,
	bool (*selctg)(const double *, const double *, const double *),
	integer *n, double *a, integer *lda, double *b,
	integer *ldb, integer *sdim, double *alphar, double *alphai,
	double *beta, double *vsl, integer *ldvsl, double *vsr,
	integer *ldvsr, double *work, integer *lwork, bool *bwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__0 = 0;
	static integer c_n1 = -1;
	static double c_b38 = 0.;
	static double c_b39 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, vsl_dim1, vsl_offset,
	    vsr_dim1, vsr_offset, i__1, i__2;
    double d__1;

    /* Builtin functions
    double sqrt(double);*/

    /* Local variables */
    integer i__, ip;
    double dif[2];
    integer ihi, ilo;
    double eps, anrm, bnrm;
    integer idum[1], ierr, itau, iwrk;
    double pvsl, pvsr;
    integer ileft, icols;
    bool cursl, ilvsl, ilvsr;
    integer irows;
    bool lst2sl;
    bool ilascl, ilbscl;
    double safmin;
     double safmax;
    double bignum;
    integer ijobvl, iright;
    integer ijobvr;
    double anrmto, bnrmto;
    bool lastsl;
    integer minwrk, maxwrk;
    double smlnum;
    bool wantst, lquery;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */
/*     .. Function Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGGES computes for a pair of N-by-N real nonsymmetric matrices (A,B), */
/*  the generalized eigenvalues, the generalized real Schur form (S,T), */
/*  optionally, the left and/or right matrices of Schur vectors (VSL and */
/*  VSR). This gives the generalized Schur factorization */

/*           (A,B) = ( (VSL)*S*(VSR)**T, (VSL)*T*(VSR)**T ) */

/*  Optionally, it also orders the eigenvalues so that a selected cluster */
/*  of eigenvalues appears in the leading diagonal blocks of the upper */
/*  quasi-triangular matrix S and the upper triangular matrix T.The */
/*  leading columns of VSL and VSR then form an orthonormal basis for the */
/*  corresponding left and right eigenspaces (deflating subspaces). */

/*  (If only the generalized eigenvalues are needed, use the driver */
/*  DGGEV instead, which is faster.) */

/*  A generalized eigenvalue for a pair of matrices (A,B) is a scalar w */
/*  or a ratio alpha/beta = w, such that  A - w*B is singular.  It is */
/*  usually represented as the pair (alpha,beta), as there is a */
/*  reasonable interpretation for beta=0 or both being zero. */

/*  A pair of matrices (S,T) is in generalized real Schur form if T is */
/*  upper triangular with non-negative diagonal and S is block upper */
/*  triangular with 1-by-1 and 2-by-2 blocks.  1-by-1 blocks correspond */
/*  to real generalized eigenvalues, while 2-by-2 blocks of S will be */
/*  "standardized" by making the corresponding elements of T have the */
/*  form: */
/*          [  a  0  ] */
/*          [  0  b  ] */

/*  and the pair of corresponding 2-by-2 blocks in S and T will have a */
/*  complex conjugate pair of generalized eigenvalues. */


/*  Arguments */
/*  ========= */

/*  JOBVSL  (input) CHARACTER*1 */
/*          = 'N':  do not compute the left Schur vectors; */
/*          = 'V':  compute the left Schur vectors. */

/*  JOBVSR  (input) CHARACTER*1 */
/*          = 'N':  do not compute the right Schur vectors; */
/*          = 'V':  compute the right Schur vectors. */

/*  SORT    (input) CHARACTER*1 */
/*          Specifies whether or not to order the eigenvalues on the */
/*          diagonal of the generalized Schur form. */
/*          = 'N':  Eigenvalues are not ordered; */
/*          = 'S':  Eigenvalues are ordered (see SELCTG); */

/*  SELCTG  (external procedure) LOGICAL FUNCTION of three DOUBLE PRECISION arguments */
/*          SELCTG must be declared EXTERNAL in the calling subroutine. */
/*          If SORT = 'N', SELCTG is not referenced. */
/*          If SORT = 'S', SELCTG is used to select eigenvalues to sort */
/*          to the top left of the Schur form. */
/*          An eigenvalue (ALPHAR(j)+ALPHAI(j))/BETA(j) is selected if */
/*          SELCTG(ALPHAR(j),ALPHAI(j),BETA(j)) is true; i.e. if either */
/*          one of a complex conjugate pair of eigenvalues is selected, */
/*          then both complex eigenvalues are selected. */

/*          Note that in the ill-conditioned case, a selected complex */
/*          eigenvalue may no longer satisfy SELCTG(ALPHAR(j),ALPHAI(j), */
/*          BETA(j)) = .TRUE. after ordering. INFO is to be set to N+2 */
/*          in this case. */

/*  N       (input) INTEGER */
/*          The order of the matrices A, B, VSL, and VSR.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the first of the pair of matrices. */
/*          On exit, A has been overwritten by its generalized Schur */
/*          form S. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  LDA >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB, N) */
/*          On entry, the second of the pair of matrices. */
/*          On exit, B has been overwritten by its generalized Schur */
/*          form T. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  LDB >= max(1,N). */

/*  SDIM    (output) INTEGER */
/*          If SORT = 'N', SDIM = 0. */
/*          If SORT = 'S', SDIM = number of eigenvalues (after sorting) */
/*          for which SELCTG is true.  (Complex conjugate pairs for which */
/*          SELCTG is true for either eigenvalue count as 2.) */

/*  ALPHAR  (output) DOUBLE PRECISION array, dimension (N) */
/*  ALPHAI  (output) DOUBLE PRECISION array, dimension (N) */
/*  BETA    (output) DOUBLE PRECISION array, dimension (N) */
/*          On exit, (ALPHAR(j) + ALPHAI(j)*i)/BETA(j), j=1,...,N, will */
/*          be the generalized eigenvalues.  ALPHAR(j) + ALPHAI(j)*i, */
/*          and  BETA(j),j=1,...,N are the diagonals of the complex Schur */
/*          form (S,T) that would result if the 2-by-2 diagonal blocks of */
/*          the real Schur form of (A,B) were further reduced to */
/*          triangular form using 2-by-2 complex unitary transformations. */
/*          If ALPHAI(j) is zero, then the j-th eigenvalue is real; if */
/*          positive, then the j-th and (j+1)-st eigenvalues are a */
/*          complex conjugate pair, with ALPHAI(j+1) negative. */

/*          Note: the quotients ALPHAR(j)/BETA(j) and ALPHAI(j)/BETA(j) */
/*          may easily over- or underflow, and BETA(j) may even be zero. */
/*          Thus, the user should avoid naively computing the ratio. */
/*          However, ALPHAR and ALPHAI will be always less than and */
/*          usually comparable with norm(A) in magnitude, and BETA always */
/*          less than and usually comparable with norm(B). */

/*  VSL     (output) DOUBLE PRECISION array, dimension (LDVSL,N) */
/*          If JOBVSL = 'V', VSL will contain the left Schur vectors. */
/*          Not referenced if JOBVSL = 'N'. */

/*  LDVSL   (input) INTEGER */
/*          The leading dimension of the matrix VSL. LDVSL >=1, and */
/*          if JOBVSL = 'V', LDVSL >= N. */

/*  VSR     (output) DOUBLE PRECISION array, dimension (LDVSR,N) */
/*          If JOBVSR = 'V', VSR will contain the right Schur vectors. */
/*          Not referenced if JOBVSR = 'N'. */

/*  LDVSR   (input) INTEGER */
/*          The leading dimension of the matrix VSR. LDVSR >= 1, and */
/*          if JOBVSR = 'V', LDVSR >= N. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          If N = 0, LWORK >= 1, else LWORK >= 8*N+16. */
/*          For good performance , LWORK must generally be larger. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  BWORK   (workspace) LOGICAL array, dimension (N) */
/*          Not referenced if SORT = 'N'. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          = 1,...,N: */
/*                The QZ iteration failed.  (A,B) are not in Schur */
/*                form, but ALPHAR(j), ALPHAI(j), and BETA(j) should */
/*                be correct for j=INFO+1,...,N. */
/*          > N:  =N+1: other than QZ iteration failed in DHGEQZ. */
/*                =N+2: after reordering, roundoff changed values of */
/*                      some complex eigenvalues so that leading */
/*                      eigenvalues in the Generalized Schur form no */
/*                      longer satisfy SELCTG=.TRUE.  This could also */
/*                      be caused due to scaling. */
/*                =N+3: reordering failed in DTGSEN. */

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

/*     Decode the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --alphar;
    --alphai;
    --beta;
    vsl_dim1 = *ldvsl;
    vsl_offset = 1 + vsl_dim1;
    vsl -= vsl_offset;
    vsr_dim1 = *ldvsr;
    vsr_offset = 1 + vsr_dim1;
    vsr -= vsr_offset;
    --work;
    --bwork;

    /* Function Body */
    if (lsame_(jobvsl, "N")) {
	ijobvl = 1;
	ilvsl = false;
    } else if (lsame_(jobvsl, "V")) {
	ijobvl = 2;
	ilvsl = true;
    } else {
	ijobvl = -1;
	ilvsl = false;
    }

    if (lsame_(jobvsr, "N")) {
	ijobvr = 1;
	ilvsr = false;
    } else if (lsame_(jobvsr, "V")) {
	ijobvr = 2;
	ilvsr = true;
    } else {
	ijobvr = -1;
	ilvsr = false;
    }

    wantst = lsame_(sort, "S");

/*     Test the input arguments */

    *info = 0;
    lquery = *lwork == -1;
    if (ijobvl <= 0) {
	*info = -1;
    } else if (ijobvr <= 0) {
	*info = -2;
    } else if (! wantst && ! lsame_(sort, "N")) {
	*info = -3;
    } else if (*n < 0) {
	*info = -5;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -9;
    } else if (*ldvsl < 1 || ilvsl && *ldvsl < *n) {
	*info = -15;
    } else if (*ldvsr < 1 || ilvsr && *ldvsr < *n) {
	*info = -17;
    }

/*     Compute workspace */
/*      (Note: Comments in the code beginning "Workspace:" describe the */
/*       minimal amount of workspace needed at that point in the code, */
/*       as well as the preferred amount for good performance. */
/*       NB refers to the optimal block size for the immediately */
/*       following subroutine, as returned by ILAENV.) */

    if (*info == 0) {
	if (*n > 0) {
/* Computing MAX */
	    i__1 = *n << 3, i__2 = *n * 6 + 16;
	    minwrk = std::max(i__1,i__2);
	    maxwrk = minwrk - *n + *n * ilaenv_(&c__1, "DGEQRF", " ", n, &
		    c__1, n, &c__0);
/* Computing MAX */
	    i__1 = maxwrk, i__2 = minwrk - *n + *n * ilaenv_(&c__1, "DORMQR",
		    " ", n, &c__1, n, &c_n1);
	    maxwrk = std::max(i__1,i__2);
	    if (ilvsl) {
/* Computing MAX */
		i__1 = maxwrk, i__2 = minwrk - *n + *n * ilaenv_(&c__1, "DOR"
			"GQR", " ", n, &c__1, n, &c_n1);
		maxwrk = std::max(i__1,i__2);
	    }
	} else {
	    minwrk = 1;
	    maxwrk = 1;
	}
	work[1] = (double) maxwrk;

	if (*lwork < minwrk && ! lquery) {
	    *info = -19;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGGES ", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	*sdim = 0;
	return 0;
    }

/*     Get machine constants */

    eps = dlamch_("P");
    safmin = dlamch_("S");
    safmax = 1. / safmin;
    dlabad_(&safmin, &safmax);
    smlnum = sqrt(safmin) / eps;
    bignum = 1. / smlnum;

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", n, n, &a[a_offset], lda, &work[1]);
    ilascl = false;
    if (anrm > 0. && anrm < smlnum) {
	anrmto = smlnum;
	ilascl = true;
    } else if (anrm > bignum) {
	anrmto = bignum;
	ilascl = true;
    }
    if (ilascl) {
	dlascl_("G", &c__0, &c__0, &anrm, &anrmto, n, n, &a[a_offset], lda, &
		ierr);
    }

/*     Scale B if max element outside range [SMLNUM,BIGNUM] */

    bnrm = dlange_("M", n, n, &b[b_offset], ldb, &work[1]);
    ilbscl = false;
    if (bnrm > 0. && bnrm < smlnum) {
	bnrmto = smlnum;
	ilbscl = true;
    } else if (bnrm > bignum) {
	bnrmto = bignum;
	ilbscl = true;
    }
    if (ilbscl) {
	dlascl_("G", &c__0, &c__0, &bnrm, &bnrmto, n, n, &b[b_offset], ldb, &
		ierr);
    }

/*     Permute the matrix to make it more nearly triangular */
/*     (Workspace: need 6*N + 2*N space for storing balancing factors) */

    ileft = 1;
    iright = *n + 1;
    iwrk = iright + *n;
    dggbal_("P", n, &a[a_offset], lda, &b[b_offset], ldb, &ilo, &ihi, &work[
	    ileft], &work[iright], &work[iwrk], &ierr);

/*     Reduce B to triangular form (QR decomposition of B) */
/*     (Workspace: need N, prefer N*NB) */

    irows = ihi + 1 - ilo;
    icols = *n + 1 - ilo;
    itau = iwrk;
    iwrk = itau + irows;
    i__1 = *lwork + 1 - iwrk;
    dgeqrf_(&irows, &icols, &b[ilo + ilo * b_dim1], ldb, &work[itau], &work[
	    iwrk], &i__1, &ierr);

/*     Apply the orthogonal transformation to matrix A */
/*     (Workspace: need N, prefer N*NB) */

    i__1 = *lwork + 1 - iwrk;
    dormqr_("L", "T", &irows, &icols, &irows, &b[ilo + ilo * b_dim1], ldb, &
	    work[itau], &a[ilo + ilo * a_dim1], lda, &work[iwrk], &i__1, &
	    ierr);

/*     Initialize VSL */
/*     (Workspace: need N, prefer N*NB) */

    if (ilvsl) {
	dlaset_("Full", n, n, &c_b38, &c_b39, &vsl[vsl_offset], ldvsl);
	if (irows > 1) {
	    i__1 = irows - 1;
	    i__2 = irows - 1;
	    dlacpy_("L", &i__1, &i__2, &b[ilo + 1 + ilo * b_dim1], ldb, &vsl[
		    ilo + 1 + ilo * vsl_dim1], ldvsl);
	}
	i__1 = *lwork + 1 - iwrk;
	dorgqr_(&irows, &irows, &irows, &vsl[ilo + ilo * vsl_dim1], ldvsl, &
		work[itau], &work[iwrk], &i__1, &ierr);
    }

/*     Initialize VSR */

    if (ilvsr) {
	dlaset_("Full", n, n, &c_b38, &c_b39, &vsr[vsr_offset], ldvsr);
    }

/*     Reduce to generalized Hessenberg form */
/*     (Workspace: none needed) */

    dgghrd_(jobvsl, jobvsr, n, &ilo, &ihi, &a[a_offset], lda, &b[b_offset],
	    ldb, &vsl[vsl_offset], ldvsl, &vsr[vsr_offset], ldvsr, &ierr);

/*     Perform QZ algorithm, computing Schur vectors if desired */
/*     (Workspace: need N) */

    iwrk = itau;
    i__1 = *lwork + 1 - iwrk;
    dhgeqz_("S", jobvsl, jobvsr, n, &ilo, &ihi, &a[a_offset], lda, &b[
	    b_offset], ldb, &alphar[1], &alphai[1], &beta[1], &vsl[vsl_offset]
, ldvsl, &vsr[vsr_offset], ldvsr, &work[iwrk], &i__1, &ierr);
    if (ierr != 0) {
	if (ierr > 0 && ierr <= *n) {
	    *info = ierr;
	} else if (ierr > *n && ierr <= *n << 1) {
	    *info = ierr - *n;
	} else {
	    *info = *n + 1;
	}
	goto L50;
    }

/*     Sort eigenvalues ALPHA/BETA if desired */
/*     (Workspace: need 4*N+16 ) */

    *sdim = 0;
    if (wantst) {

/*        Undo scaling on eigenvalues before SELCTGing */

	if (ilascl) {
	    dlascl_("G", &c__0, &c__0, &anrmto, &anrm, n, &c__1, &alphar[1],
		    n, &ierr);
	    dlascl_("G", &c__0, &c__0, &anrmto, &anrm, n, &c__1, &alphai[1],
		    n, &ierr);
	}
	if (ilbscl) {
	    dlascl_("G", &c__0, &c__0, &bnrmto, &bnrm, n, &c__1, &beta[1], n,
		    &ierr);
	}

/*        Select eigenvalues */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    bwork[i__] = (*selctg)(&alphar[i__], &alphai[i__], &beta[i__]);
/* L10: */
	}

	i__1 = *lwork - iwrk + 1;
	dtgsen_(&c__0, &ilvsl, &ilvsr, &bwork[1], n, &a[a_offset], lda, &b[
		b_offset], ldb, &alphar[1], &alphai[1], &beta[1], &vsl[
		vsl_offset], ldvsl, &vsr[vsr_offset], ldvsr, sdim, &pvsl, &
		pvsr, dif, &work[iwrk], &i__1, idum, &c__1, &ierr);
	if (ierr == 1) {
	    *info = *n + 3;
	}

    }

/*     Apply back-permutation to VSL and VSR */
/*     (Workspace: none needed) */

    if (ilvsl) {
	dggbak_("P", "L", n, &ilo, &ihi, &work[ileft], &work[iright], n, &vsl[
		vsl_offset], ldvsl, &ierr);
    }

    if (ilvsr) {
	dggbak_("P", "R", n, &ilo, &ihi, &work[ileft], &work[iright], n, &vsr[
		vsr_offset], ldvsr, &ierr);
    }

/*     Check if unscaling would cause over/underflow, if so, rescale */
/*     (ALPHAR(I),ALPHAI(I),BETA(I)) so BETA(I) is on the order of */
/*     B(I,I) and ALPHAR(I) and ALPHAI(I) are on the order of A(I,I) */

    if (ilascl) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (alphai[i__] != 0.) {
		if (alphar[i__] / safmax > anrmto / anrm || safmin / alphar[
			i__] > anrm / anrmto) {
		    work[1] = (d__1 = a[i__ + i__ * a_dim1] / alphar[i__],
			    abs(d__1));
		    beta[i__] *= work[1];
		    alphar[i__] *= work[1];
		    alphai[i__] *= work[1];
		} else if (alphai[i__] / safmax > anrmto / anrm || safmin /
			alphai[i__] > anrm / anrmto) {
		    work[1] = (d__1 = a[i__ + (i__ + 1) * a_dim1] / alphai[
			    i__], abs(d__1));
		    beta[i__] *= work[1];
		    alphar[i__] *= work[1];
		    alphai[i__] *= work[1];
		}
	    }
/* L20: */
	}
    }

    if (ilbscl) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (alphai[i__] != 0.) {
		if (beta[i__] / safmax > bnrmto / bnrm || safmin / beta[i__]
			> bnrm / bnrmto) {
		    work[1] = (d__1 = b[i__ + i__ * b_dim1] / beta[i__], abs(
			    d__1));
		    beta[i__] *= work[1];
		    alphar[i__] *= work[1];
		    alphai[i__] *= work[1];
		}
	    }
/* L30: */
	}
    }

/*     Undo scaling */

    if (ilascl) {
	dlascl_("H", &c__0, &c__0, &anrmto, &anrm, n, n, &a[a_offset], lda, &
		ierr);
	dlascl_("G", &c__0, &c__0, &anrmto, &anrm, n, &c__1, &alphar[1], n, &
		ierr);
	dlascl_("G", &c__0, &c__0, &anrmto, &anrm, n, &c__1, &alphai[1], n, &
		ierr);
    }

    if (ilbscl) {
	dlascl_("U", &c__0, &c__0, &bnrmto, &bnrm, n, n, &b[b_offset], ldb, &
		ierr);
	dlascl_("G", &c__0, &c__0, &bnrmto, &bnrm, n, &c__1, &beta[1], n, &
		ierr);
    }

    if (wantst) {

/*        Check if reordering is correct */

	lastsl = true;
	lst2sl = true;
	*sdim = 0;
	ip = 0;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    cursl = (*selctg)(&alphar[i__], &alphai[i__], &beta[i__]);
	    if (alphai[i__] == 0.) {
		if (cursl) {
		    ++(*sdim);
		}
		ip = 0;
		if (cursl && ! lastsl) {
		    *info = *n + 2;
		}
	    } else {
		if (ip == 1) {

/*                 Last eigenvalue of conjugate pair */

		    cursl = cursl || lastsl;
		    lastsl = cursl;
		    if (cursl) {
			*sdim += 2;
		    }
		    ip = -1;
		    if (cursl && ! lst2sl) {
			*info = *n + 2;
		    }
		} else {

/*                 First eigenvalue of conjugate pair */

		    ip = 1;
		}
	    }
	    lst2sl = lastsl;
	    lastsl = cursl;
/* L40: */
	}

    }

L50:

    work[1] = (double) maxwrk;

    return 0;

/*     End of DGGES */

} /* dgges_ */

/* Subroutine */ int dggesx_(const char *jobvsl, const char *jobvsr, const char *sort,
	bool (*selctg)(const double *, const double *, const double *),
	const char *sense, integer *n, double *a, integer *lda,
	double *b, integer *ldb, integer *sdim, double *alphar,
	double *alphai, double *beta, double *vsl, integer *ldvsl,
	 double *vsr, integer *ldvsr, double *rconde, double *
	rcondv, double *work, integer *lwork, integer *iwork, integer *
	liwork, bool *bwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__0 = 0;
	static integer c_n1 = -1;
	static double c_b42 = 0.;
	static double c_b43 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, vsl_dim1, vsl_offset,
	    vsr_dim1, vsr_offset, i__1, i__2;
    double d__1;

    /* Local variables */
    integer i__, ip;
    double pl, pr, dif[2];
    integer ihi, ilo;
    double eps;
    integer ijob;
    double anrm, bnrm;
    integer ierr, itau, iwrk, lwrk;
    integer ileft, icols;
    bool cursl, ilvsl, ilvsr;
    integer irows;
    bool lst2sl;
    bool ilascl, ilbscl;
    double safmin;
    double safmax;
    double bignum;
	integer ijobvl, iright;
	integer ijobvr;
    bool wantsb;
    integer liwmin;
    bool wantse, lastsl;
    double anrmto, bnrmto;
    integer minwrk, maxwrk;
    bool wantsn;
    double smlnum;
    bool wantst, lquery, wantsv;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */
/*     .. Function Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGGESX computes for a pair of N-by-N real nonsymmetric matrices */
/*  (A,B), the generalized eigenvalues, the real Schur form (S,T), and, */
/*  optionally, the left and/or right matrices of Schur vectors (VSL and */
/*  VSR).  This gives the generalized Schur factorization */

/*       (A,B) = ( (VSL) S (VSR)**T, (VSL) T (VSR)**T ) */

/*  Optionally, it also orders the eigenvalues so that a selected cluster */
/*  of eigenvalues appears in the leading diagonal blocks of the upper */
/*  quasi-triangular matrix S and the upper triangular matrix T; computes */
/*  a reciprocal condition number for the average of the selected */
/*  eigenvalues (RCONDE); and computes a reciprocal condition number for */
/*  the right and left deflating subspaces corresponding to the selected */
/*  eigenvalues (RCONDV). The leading columns of VSL and VSR then form */
/*  an orthonormal basis for the corresponding left and right eigenspaces */
/*  (deflating subspaces). */

/*  A generalized eigenvalue for a pair of matrices (A,B) is a scalar w */
/*  or a ratio alpha/beta = w, such that  A - w*B is singular.  It is */
/*  usually represented as the pair (alpha,beta), as there is a */
/*  reasonable interpretation for beta=0 or for both being zero. */

/*  A pair of matrices (S,T) is in generalized real Schur form if T is */
/*  upper triangular with non-negative diagonal and S is block upper */
/*  triangular with 1-by-1 and 2-by-2 blocks.  1-by-1 blocks correspond */
/*  to real generalized eigenvalues, while 2-by-2 blocks of S will be */
/*  "standardized" by making the corresponding elements of T have the */
/*  form: */
/*          [  a  0  ] */
/*          [  0  b  ] */

/*  and the pair of corresponding 2-by-2 blocks in S and T will have a */
/*  complex conjugate pair of generalized eigenvalues. */


/*  Arguments */
/*  ========= */

/*  JOBVSL  (input) CHARACTER*1 */
/*          = 'N':  do not compute the left Schur vectors; */
/*          = 'V':  compute the left Schur vectors. */

/*  JOBVSR  (input) CHARACTER*1 */
/*          = 'N':  do not compute the right Schur vectors; */
/*          = 'V':  compute the right Schur vectors. */

/*  SORT    (input) CHARACTER*1 */
/*          Specifies whether or not to order the eigenvalues on the */
/*          diagonal of the generalized Schur form. */
/*          = 'N':  Eigenvalues are not ordered; */
/*          = 'S':  Eigenvalues are ordered (see SELCTG). */

/*  SELCTG  (external procedure) LOGICAL FUNCTION of three DOUBLE PRECISION arguments */
/*          SELCTG must be declared EXTERNAL in the calling subroutine. */
/*          If SORT = 'N', SELCTG is not referenced. */
/*          If SORT = 'S', SELCTG is used to select eigenvalues to sort */
/*          to the top left of the Schur form. */
/*          An eigenvalue (ALPHAR(j)+ALPHAI(j))/BETA(j) is selected if */
/*          SELCTG(ALPHAR(j),ALPHAI(j),BETA(j)) is true; i.e. if either */
/*          one of a complex conjugate pair of eigenvalues is selected, */
/*          then both complex eigenvalues are selected. */
/*          Note that a selected complex eigenvalue may no longer satisfy */
/*          SELCTG(ALPHAR(j),ALPHAI(j),BETA(j)) = .TRUE. after ordering, */
/*          since ordering may change the value of complex eigenvalues */
/*          (especially if the eigenvalue is ill-conditioned), in this */
/*          case INFO is set to N+3. */

/*  SENSE   (input) CHARACTER*1 */
/*          Determines which reciprocal condition numbers are computed. */
/*          = 'N' : None are computed; */
/*          = 'E' : Computed for average of selected eigenvalues only; */
/*          = 'V' : Computed for selected deflating subspaces only; */
/*          = 'B' : Computed for both. */
/*          If SENSE = 'E', 'V', or 'B', SORT must equal 'S'. */

/*  N       (input) INTEGER */
/*          The order of the matrices A, B, VSL, and VSR.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the first of the pair of matrices. */
/*          On exit, A has been overwritten by its generalized Schur */
/*          form S. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  LDA >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB, N) */
/*          On entry, the second of the pair of matrices. */
/*          On exit, B has been overwritten by its generalized Schur */
/*          form T. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  LDB >= max(1,N). */

/*  SDIM    (output) INTEGER */
/*          If SORT = 'N', SDIM = 0. */
/*          If SORT = 'S', SDIM = number of eigenvalues (after sorting) */
/*          for which SELCTG is true.  (Complex conjugate pairs for which */
/*          SELCTG is true for either eigenvalue count as 2.) */

/*  ALPHAR  (output) DOUBLE PRECISION array, dimension (N) */
/*  ALPHAI  (output) DOUBLE PRECISION array, dimension (N) */
/*  BETA    (output) DOUBLE PRECISION array, dimension (N) */
/*          On exit, (ALPHAR(j) + ALPHAI(j)*i)/BETA(j), j=1,...,N, will */
/*          be the generalized eigenvalues.  ALPHAR(j) + ALPHAI(j)*i */
/*          and BETA(j),j=1,...,N  are the diagonals of the complex Schur */
/*          form (S,T) that would result if the 2-by-2 diagonal blocks of */
/*          the real Schur form of (A,B) were further reduced to */
/*          triangular form using 2-by-2 complex unitary transformations. */
/*          If ALPHAI(j) is zero, then the j-th eigenvalue is real; if */
/*          positive, then the j-th and (j+1)-st eigenvalues are a */
/*          complex conjugate pair, with ALPHAI(j+1) negative. */

/*          Note: the quotients ALPHAR(j)/BETA(j) and ALPHAI(j)/BETA(j) */
/*          may easily over- or underflow, and BETA(j) may even be zero. */
/*          Thus, the user should avoid naively computing the ratio. */
/*          However, ALPHAR and ALPHAI will be always less than and */
/*          usually comparable with norm(A) in magnitude, and BETA always */
/*          less than and usually comparable with norm(B). */

/*  VSL     (output) DOUBLE PRECISION array, dimension (LDVSL,N) */
/*          If JOBVSL = 'V', VSL will contain the left Schur vectors. */
/*          Not referenced if JOBVSL = 'N'. */

/*  LDVSL   (input) INTEGER */
/*          The leading dimension of the matrix VSL. LDVSL >=1, and */
/*          if JOBVSL = 'V', LDVSL >= N. */

/*  VSR     (output) DOUBLE PRECISION array, dimension (LDVSR,N) */
/*          If JOBVSR = 'V', VSR will contain the right Schur vectors. */
/*          Not referenced if JOBVSR = 'N'. */

/*  LDVSR   (input) INTEGER */
/*          The leading dimension of the matrix VSR. LDVSR >= 1, and */
/*          if JOBVSR = 'V', LDVSR >= N. */

/*  RCONDE  (output) DOUBLE PRECISION array, dimension ( 2 ) */
/*          If SENSE = 'E' or 'B', RCONDE(1) and RCONDE(2) contain the */
/*          reciprocal condition numbers for the average of the selected */
/*          eigenvalues. */
/*          Not referenced if SENSE = 'N' or 'V'. */

/*  RCONDV  (output) DOUBLE PRECISION array, dimension ( 2 ) */
/*          If SENSE = 'V' or 'B', RCONDV(1) and RCONDV(2) contain the */
/*          reciprocal condition numbers for the selected deflating */
/*          subspaces. */
/*          Not referenced if SENSE = 'N' or 'E'. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          If N = 0, LWORK >= 1, else if SENSE = 'E', 'V', or 'B', */
/*          LWORK >= max( 8*N, 6*N+16, 2*SDIM*(N-SDIM) ), else */
/*          LWORK >= max( 8*N, 6*N+16 ). */
/*          Note that 2*SDIM*(N-SDIM) <= N*N/2. */
/*          Note also that an error is only returned if */
/*          LWORK < max( 8*N, 6*N+16), but if SENSE = 'E' or 'V' or 'B' */
/*          this may not be large enough. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the bound on the optimal size of the WORK */
/*          array and the minimum size of the IWORK array, returns these */
/*          values as the first entries of the WORK and IWORK arrays, and */
/*          no error message related to LWORK or LIWORK is issued by */
/*          XERBLA. */

/*  IWORK   (workspace) INTEGER array, dimension (MAX(1,LIWORK)) */
/*          On exit, if INFO = 0, IWORK(1) returns the minimum LIWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK. */
/*          If SENSE = 'N' or N = 0, LIWORK >= 1, otherwise */
/*          LIWORK >= N+6. */

/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates the bound on the optimal size of the */
/*          WORK array and the minimum size of the IWORK array, returns */
/*          these values as the first entries of the WORK and IWORK */
/*          arrays, and no error message related to LWORK or LIWORK is */
/*          issued by XERBLA. */

/*  BWORK   (workspace) LOGICAL array, dimension (N) */
/*          Not referenced if SORT = 'N'. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          = 1,...,N: */
/*                The QZ iteration failed.  (A,B) are not in Schur */
/*                form, but ALPHAR(j), ALPHAI(j), and BETA(j) should */
/*                be correct for j=INFO+1,...,N. */
/*          > N:  =N+1: other than QZ iteration failed in DHGEQZ */
/*                =N+2: after reordering, roundoff changed values of */
/*                      some complex eigenvalues so that leading */
/*                      eigenvalues in the Generalized Schur form no */
/*                      longer satisfy SELCTG=.TRUE.  This could also */
/*                      be caused due to scaling. */
/*                =N+3: reordering failed in DTGSEN. */

/*  Further details */
/*  =============== */

/*  An approximate (asymptotic) bound on the average absolute error of */
/*  the selected eigenvalues is */

/*       EPS * norm((A, B)) / RCONDE( 1 ). */

/*  An approximate (asymptotic) bound on the maximum angular error in */
/*  the computed deflating subspaces is */

/*       EPS * norm((A, B)) / RCONDV( 2 ). */

/*  See LAPACK User's Guide, section 4.11 for more information. */

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

/*     Decode the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --alphar;
    --alphai;
    --beta;
    vsl_dim1 = *ldvsl;
    vsl_offset = 1 + vsl_dim1;
    vsl -= vsl_offset;
    vsr_dim1 = *ldvsr;
    vsr_offset = 1 + vsr_dim1;
    vsr -= vsr_offset;
    --rconde;
    --rcondv;
    --work;
    --iwork;
    --bwork;

    /* Function Body */
    if (lsame_(jobvsl, "N")) {
	ijobvl = 1;
	ilvsl = false;
    } else if (lsame_(jobvsl, "V")) {
	ijobvl = 2;
	ilvsl = true;
    } else {
	ijobvl = -1;
	ilvsl = false;
    }

    if (lsame_(jobvsr, "N")) {
	ijobvr = 1;
	ilvsr = false;
    } else if (lsame_(jobvsr, "V")) {
	ijobvr = 2;
	ilvsr = true;
    } else {
	ijobvr = -1;
	ilvsr = false;
    }

    wantst = lsame_(sort, "S");
    wantsn = lsame_(sense, "N");
    wantse = lsame_(sense, "E");
    wantsv = lsame_(sense, "V");
    wantsb = lsame_(sense, "B");
    lquery = *lwork == -1 || *liwork == -1;
    if (wantsn) {
	ijob = 0;
    } else if (wantse) {
	ijob = 1;
    } else if (wantsv) {
	ijob = 2;
    } else if (wantsb) {
	ijob = 4;
    }

/*     Test the input arguments */

    *info = 0;
    if (ijobvl <= 0) {
	*info = -1;
    } else if (ijobvr <= 0) {
	*info = -2;
    } else if (! wantst && ! lsame_(sort, "N")) {
	*info = -3;
    } else if (! (wantsn || wantse || wantsv || wantsb) || ! wantst && !
	    wantsn) {
	*info = -5;
    } else if (*n < 0) {
	*info = -6;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -8;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -10;
    } else if (*ldvsl < 1 || ilvsl && *ldvsl < *n) {
	*info = -16;
    } else if (*ldvsr < 1 || ilvsr && *ldvsr < *n) {
	*info = -18;
    }

/*     Compute workspace */
/*      (Note: Comments in the code beginning "Workspace:" describe the */
/*       minimal amount of workspace needed at that point in the code, */
/*       as well as the preferred amount for good performance. */
/*       NB refers to the optimal block size for the immediately */
/*       following subroutine, as returned by ILAENV.) */

    if (*info == 0) {
	if (*n > 0) {
/* Computing MAX */
	    i__1 = *n << 3, i__2 = *n * 6 + 16;
	    minwrk = std::max(i__1,i__2);
	    maxwrk = minwrk - *n + *n * ilaenv_(&c__1, "DGEQRF", " ", n, &
		    c__1, n, &c__0);
/* Computing MAX */
	    i__1 = maxwrk, i__2 = minwrk - *n + *n * ilaenv_(&c__1, "DORMQR",
		    " ", n, &c__1, n, &c_n1);
	    maxwrk = std::max(i__1,i__2);
	    if (ilvsl) {
/* Computing MAX */
		i__1 = maxwrk, i__2 = minwrk - *n + *n * ilaenv_(&c__1, "DOR"
			"GQR", " ", n, &c__1, n, &c_n1);
		maxwrk = std::max(i__1,i__2);
	    }
	    lwrk = maxwrk;
	    if (ijob >= 1) {
/* Computing MAX */
		i__1 = lwrk, i__2 = *n * *n / 2;
		lwrk = std::max(i__1,i__2);
	    }
	} else {
	    minwrk = 1;
	    maxwrk = 1;
	    lwrk = 1;
	}
	work[1] = (double) lwrk;
	if (wantsn || *n == 0) {
	    liwmin = 1;
	} else {
	    liwmin = *n + 6;
	}
	iwork[1] = liwmin;

	if (*lwork < minwrk && ! lquery) {
	    *info = -22;
	} else if (*liwork < liwmin && ! lquery) {
	    *info = -24;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGGESX", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	*sdim = 0;
	return 0;
    }

/*     Get machine constants */

    eps = dlamch_("P");
    safmin = dlamch_("S");
    safmax = 1. / safmin;
    dlabad_(&safmin, &safmax);
    smlnum = sqrt(safmin) / eps;
    bignum = 1. / smlnum;

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", n, n, &a[a_offset], lda, &work[1]);
    ilascl = false;
    if (anrm > 0. && anrm < smlnum) {
	anrmto = smlnum;
	ilascl = true;
    } else if (anrm > bignum) {
	anrmto = bignum;
	ilascl = true;
    }
    if (ilascl) {
	dlascl_("G", &c__0, &c__0, &anrm, &anrmto, n, n, &a[a_offset], lda, &
		ierr);
    }

/*     Scale B if max element outside range [SMLNUM,BIGNUM] */

    bnrm = dlange_("M", n, n, &b[b_offset], ldb, &work[1]);
    ilbscl = false;
    if (bnrm > 0. && bnrm < smlnum) {
	bnrmto = smlnum;
	ilbscl = true;
    } else if (bnrm > bignum) {
	bnrmto = bignum;
	ilbscl = true;
    }
    if (ilbscl) {
	dlascl_("G", &c__0, &c__0, &bnrm, &bnrmto, n, n, &b[b_offset], ldb, &
		ierr);
    }

/*     Permute the matrix to make it more nearly triangular */
/*     (Workspace: need 6*N + 2*N for permutation parameters) */

    ileft = 1;
    iright = *n + 1;
    iwrk = iright + *n;
    dggbal_("P", n, &a[a_offset], lda, &b[b_offset], ldb, &ilo, &ihi, &work[
	    ileft], &work[iright], &work[iwrk], &ierr);

/*     Reduce B to triangular form (QR decomposition of B) */
/*     (Workspace: need N, prefer N*NB) */

    irows = ihi + 1 - ilo;
    icols = *n + 1 - ilo;
    itau = iwrk;
    iwrk = itau + irows;
    i__1 = *lwork + 1 - iwrk;
    dgeqrf_(&irows, &icols, &b[ilo + ilo * b_dim1], ldb, &work[itau], &work[
	    iwrk], &i__1, &ierr);

/*     Apply the orthogonal transformation to matrix A */
/*     (Workspace: need N, prefer N*NB) */

    i__1 = *lwork + 1 - iwrk;
    dormqr_("L", "T", &irows, &icols, &irows, &b[ilo + ilo * b_dim1], ldb, &
	    work[itau], &a[ilo + ilo * a_dim1], lda, &work[iwrk], &i__1, &
	    ierr);

/*     Initialize VSL */
/*     (Workspace: need N, prefer N*NB) */

    if (ilvsl) {
	dlaset_("Full", n, n, &c_b42, &c_b43, &vsl[vsl_offset], ldvsl);
	if (irows > 1) {
	    i__1 = irows - 1;
	    i__2 = irows - 1;
	    dlacpy_("L", &i__1, &i__2, &b[ilo + 1 + ilo * b_dim1], ldb, &vsl[
		    ilo + 1 + ilo * vsl_dim1], ldvsl);
	}
	i__1 = *lwork + 1 - iwrk;
	dorgqr_(&irows, &irows, &irows, &vsl[ilo + ilo * vsl_dim1], ldvsl, &
		work[itau], &work[iwrk], &i__1, &ierr);
    }

/*     Initialize VSR */

    if (ilvsr) {
	dlaset_("Full", n, n, &c_b42, &c_b43, &vsr[vsr_offset], ldvsr);
    }

/*     Reduce to generalized Hessenberg form */
/*     (Workspace: none needed) */

    dgghrd_(jobvsl, jobvsr, n, &ilo, &ihi, &a[a_offset], lda, &b[b_offset],
	    ldb, &vsl[vsl_offset], ldvsl, &vsr[vsr_offset], ldvsr, &ierr);

    *sdim = 0;

/*     Perform QZ algorithm, computing Schur vectors if desired */
/*     (Workspace: need N) */

    iwrk = itau;
    i__1 = *lwork + 1 - iwrk;
    dhgeqz_("S", jobvsl, jobvsr, n, &ilo, &ihi, &a[a_offset], lda, &b[
	    b_offset], ldb, &alphar[1], &alphai[1], &beta[1], &vsl[vsl_offset]
, ldvsl, &vsr[vsr_offset], ldvsr, &work[iwrk], &i__1, &ierr);
    if (ierr != 0) {
	if (ierr > 0 && ierr <= *n) {
	    *info = ierr;
	} else if (ierr > *n && ierr <= *n << 1) {
	    *info = ierr - *n;
	} else {
	    *info = *n + 1;
	}
	goto L60;
    }

/*     Sort eigenvalues ALPHA/BETA and compute the reciprocal of */
/*     condition number(s) */
/*     (Workspace: If IJOB >= 1, need MAX( 8*(N+1), 2*SDIM*(N-SDIM) ) */
/*                 otherwise, need 8*(N+1) ) */

    if (wantst) {

/*        Undo scaling on eigenvalues before SELCTGing */

	if (ilascl) {
	    dlascl_("G", &c__0, &c__0, &anrmto, &anrm, n, &c__1, &alphar[1],
		    n, &ierr);
	    dlascl_("G", &c__0, &c__0, &anrmto, &anrm, n, &c__1, &alphai[1],
		    n, &ierr);
	}
	if (ilbscl) {
	    dlascl_("G", &c__0, &c__0, &bnrmto, &bnrm, n, &c__1, &beta[1], n,
		    &ierr);
	}

/*        Select eigenvalues */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    bwork[i__] = (*selctg)(&alphar[i__], &alphai[i__], &beta[i__]);
/* L10: */
	}

/*        Reorder eigenvalues, transform Generalized Schur vectors, and */
/*        compute reciprocal condition numbers */

	i__1 = *lwork - iwrk + 1;
	dtgsen_(&ijob, &ilvsl, &ilvsr, &bwork[1], n, &a[a_offset], lda, &b[
		b_offset], ldb, &alphar[1], &alphai[1], &beta[1], &vsl[
		vsl_offset], ldvsl, &vsr[vsr_offset], ldvsr, sdim, &pl, &pr,
		dif, &work[iwrk], &i__1, &iwork[1], liwork, &ierr);

	if (ijob >= 1) {
/* Computing MAX */
	    i__1 = maxwrk, i__2 = (*sdim << 1) * (*n - *sdim);
	    maxwrk = std::max(i__1,i__2);
	}
	if (ierr == -22) {

/*            not enough real workspace */

	    *info = -22;
	} else {
	    if (ijob == 1 || ijob == 4) {
		rconde[1] = pl;
		rconde[2] = pr;
	    }
	    if (ijob == 2 || ijob == 4) {
		rcondv[1] = dif[0];
		rcondv[2] = dif[1];
	    }
	    if (ierr == 1) {
		*info = *n + 3;
	    }
	}

    }

/*     Apply permutation to VSL and VSR */
/*     (Workspace: none needed) */

    if (ilvsl) {
	dggbak_("P", "L", n, &ilo, &ihi, &work[ileft], &work[iright], n, &vsl[
		vsl_offset], ldvsl, &ierr);
    }

    if (ilvsr) {
	dggbak_("P", "R", n, &ilo, &ihi, &work[ileft], &work[iright], n, &vsr[
		vsr_offset], ldvsr, &ierr);
    }

/*     Check if unscaling would cause over/underflow, if so, rescale */
/*     (ALPHAR(I),ALPHAI(I),BETA(I)) so BETA(I) is on the order of */
/*     B(I,I) and ALPHAR(I) and ALPHAI(I) are on the order of A(I,I) */

    if (ilascl) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (alphai[i__] != 0.) {
		if (alphar[i__] / safmax > anrmto / anrm || safmin / alphar[
			i__] > anrm / anrmto) {
		    work[1] = (d__1 = a[i__ + i__ * a_dim1] / alphar[i__],
			    abs(d__1));
		    beta[i__] *= work[1];
		    alphar[i__] *= work[1];
		    alphai[i__] *= work[1];
		} else if (alphai[i__] / safmax > anrmto / anrm || safmin /
			alphai[i__] > anrm / anrmto) {
		    work[1] = (d__1 = a[i__ + (i__ + 1) * a_dim1] / alphai[
			    i__], abs(d__1));
		    beta[i__] *= work[1];
		    alphar[i__] *= work[1];
		    alphai[i__] *= work[1];
		}
	    }
/* L20: */
	}
    }

    if (ilbscl) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (alphai[i__] != 0.) {
		if (beta[i__] / safmax > bnrmto / bnrm || safmin / beta[i__]
			> bnrm / bnrmto) {
		    work[1] = (d__1 = b[i__ + i__ * b_dim1] / beta[i__], abs(
			    d__1));
		    beta[i__] *= work[1];
		    alphar[i__] *= work[1];
		    alphai[i__] *= work[1];
		}
	    }
/* L30: */
	}
    }

/*     Undo scaling */

    if (ilascl) {
	dlascl_("H", &c__0, &c__0, &anrmto, &anrm, n, n, &a[a_offset], lda, &
		ierr);
	dlascl_("G", &c__0, &c__0, &anrmto, &anrm, n, &c__1, &alphar[1], n, &
		ierr);
	dlascl_("G", &c__0, &c__0, &anrmto, &anrm, n, &c__1, &alphai[1], n, &
		ierr);
    }

    if (ilbscl) {
	dlascl_("U", &c__0, &c__0, &bnrmto, &bnrm, n, n, &b[b_offset], ldb, &
		ierr);
	dlascl_("G", &c__0, &c__0, &bnrmto, &bnrm, n, &c__1, &beta[1], n, &
		ierr);
    }

    if (wantst) {

/*        Check if reordering is correct */

	lastsl = true;
	lst2sl = true;
	*sdim = 0;
	ip = 0;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    cursl = (*selctg)(&alphar[i__], &alphai[i__], &beta[i__]);
	    if (alphai[i__] == 0.) {
		if (cursl) {
		    ++(*sdim);
		}
		ip = 0;
		if (cursl && ! lastsl) {
		    *info = *n + 2;
		}
	    } else {
		if (ip == 1) {

/*                 Last eigenvalue of conjugate pair */

		    cursl = cursl || lastsl;
		    lastsl = cursl;
		    if (cursl) {
			*sdim += 2;
		    }
		    ip = -1;
		    if (cursl && ! lst2sl) {
			*info = *n + 2;
		    }
		} else {

/*                 First eigenvalue of conjugate pair */

		    ip = 1;
		}
	    }
	    lst2sl = lastsl;
	    lastsl = cursl;
/* L50: */
	}

    }

L60:

    work[1] = (double) maxwrk;
    iwork[1] = liwmin;

    return 0;

/*     End of DGGESX */

} /* dggesx_ */

/* Subroutine */ int dggev_(const char *jobvl, const char *jobvr, integer *n, double *
	a, integer *lda, double *b, integer *ldb, double *alphar,
	double *alphai, double *beta, double *vl, integer *ldvl,
	double *vr, integer *ldvr, double *work, integer *lwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__0 = 0;
	static integer c_n1 = -1;
	static double c_b36 = 0.;
	static double c_b37 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, vl_dim1, vl_offset, vr_dim1,
	    vr_offset, i__1, i__2;
    double d__1, d__2, d__3, d__4;

    /* Local variables */
    integer jc, in, jr, ihi, ilo;
    double eps;
    bool ilv;
    double anrm, bnrm;
    integer ierr, itau;
    double temp;
    bool ilvl, ilvr;
    integer iwrk;
    integer ileft, icols, irows;
    bool ilascl, ilbscl;
    bool ldumma[1];
    char chtemp[1];
    double bignum;
    integer ijobvl, iright, ijobvr;
    double anrmto, bnrmto;
    integer minwrk, maxwrk;
    double smlnum;
    bool lquery;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGGEV computes for a pair of N-by-N real nonsymmetric matrices (A,B) */
/*  the generalized eigenvalues, and optionally, the left and/or right */
/*  generalized eigenvectors. */

/*  A generalized eigenvalue for a pair of matrices (A,B) is a scalar */
/*  lambda or a ratio alpha/beta = lambda, such that A - lambda*B is */
/*  singular. It is usually represented as the pair (alpha,beta), as */
/*  there is a reasonable interpretation for beta=0, and even for both */
/*  being zero. */

/*  The right eigenvector v(j) corresponding to the eigenvalue lambda(j) */
/*  of (A,B) satisfies */

/*                   A * v(j) = lambda(j) * B * v(j). */

/*  The left eigenvector u(j) corresponding to the eigenvalue lambda(j) */
/*  of (A,B) satisfies */

/*                   u(j)**H * A  = lambda(j) * u(j)**H * B . */

/*  where u(j)**H is the conjugate-transpose of u(j). */


/*  Arguments */
/*  ========= */

/*  JOBVL   (input) CHARACTER*1 */
/*          = 'N':  do not compute the left generalized eigenvectors; */
/*          = 'V':  compute the left generalized eigenvectors. */

/*  JOBVR   (input) CHARACTER*1 */
/*          = 'N':  do not compute the right generalized eigenvectors; */
/*          = 'V':  compute the right generalized eigenvectors. */

/*  N       (input) INTEGER */
/*          The order of the matrices A, B, VL, and VR.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the matrix A in the pair (A,B). */
/*          On exit, A has been overwritten. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  LDA >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB, N) */
/*          On entry, the matrix B in the pair (A,B). */
/*          On exit, B has been overwritten. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  LDB >= max(1,N). */

/*  ALPHAR  (output) DOUBLE PRECISION array, dimension (N) */
/*  ALPHAI  (output) DOUBLE PRECISION array, dimension (N) */
/*  BETA    (output) DOUBLE PRECISION array, dimension (N) */
/*          On exit, (ALPHAR(j) + ALPHAI(j)*i)/BETA(j), j=1,...,N, will */
/*          be the generalized eigenvalues.  If ALPHAI(j) is zero, then */
/*          the j-th eigenvalue is real; if positive, then the j-th and */
/*          (j+1)-st eigenvalues are a complex conjugate pair, with */
/*          ALPHAI(j+1) negative. */

/*          Note: the quotients ALPHAR(j)/BETA(j) and ALPHAI(j)/BETA(j) */
/*          may easily over- or underflow, and BETA(j) may even be zero. */
/*          Thus, the user should avoid naively computing the ratio */
/*          alpha/beta.  However, ALPHAR and ALPHAI will be always less */
/*          than and usually comparable with norm(A) in magnitude, and */
/*          BETA always less than and usually comparable with norm(B). */

/*  VL      (output) DOUBLE PRECISION array, dimension (LDVL,N) */
/*          If JOBVL = 'V', the left eigenvectors u(j) are stored one */
/*          after another in the columns of VL, in the same order as */
/*          their eigenvalues. If the j-th eigenvalue is real, then */
/*          u(j) = VL(:,j), the j-th column of VL. If the j-th and */
/*          (j+1)-th eigenvalues form a complex conjugate pair, then */
/*          u(j) = VL(:,j)+i*VL(:,j+1) and u(j+1) = VL(:,j)-i*VL(:,j+1). */
/*          Each eigenvector is scaled so the largest component has */
/*          abs(real part)+abs(imag. part)=1. */
/*          Not referenced if JOBVL = 'N'. */

/*  LDVL    (input) INTEGER */
/*          The leading dimension of the matrix VL. LDVL >= 1, and */
/*          if JOBVL = 'V', LDVL >= N. */

/*  VR      (output) DOUBLE PRECISION array, dimension (LDVR,N) */
/*          If JOBVR = 'V', the right eigenvectors v(j) are stored one */
/*          after another in the columns of VR, in the same order as */
/*          their eigenvalues. If the j-th eigenvalue is real, then */
/*          v(j) = VR(:,j), the j-th column of VR. If the j-th and */
/*          (j+1)-th eigenvalues form a complex conjugate pair, then */
/*          v(j) = VR(:,j)+i*VR(:,j+1) and v(j+1) = VR(:,j)-i*VR(:,j+1). */
/*          Each eigenvector is scaled so the largest component has */
/*          abs(real part)+abs(imag. part)=1. */
/*          Not referenced if JOBVR = 'N'. */

/*  LDVR    (input) INTEGER */
/*          The leading dimension of the matrix VR. LDVR >= 1, and */
/*          if JOBVR = 'V', LDVR >= N. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= max(1,8*N). */
/*          For good performance, LWORK must generally be larger. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          = 1,...,N: */
/*                The QZ iteration failed.  No eigenvectors have been */
/*                calculated, but ALPHAR(j), ALPHAI(j), and BETA(j) */
/*                should be correct for j=INFO+1,...,N. */
/*          > N:  =N+1: other than QZ iteration failed in DHGEQZ. */
/*                =N+2: error return from DTGEVC. */

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

/*     Decode the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --alphar;
    --alphai;
    --beta;
    vl_dim1 = *ldvl;
    vl_offset = 1 + vl_dim1;
    vl -= vl_offset;
    vr_dim1 = *ldvr;
    vr_offset = 1 + vr_dim1;
    vr -= vr_offset;
    --work;

    /* Function Body */
    if (lsame_(jobvl, "N")) {
	ijobvl = 1;
	ilvl = false;
    } else if (lsame_(jobvl, "V")) {
	ijobvl = 2;
	ilvl = true;
    } else {
	ijobvl = -1;
	ilvl = false;
    }

    if (lsame_(jobvr, "N")) {
	ijobvr = 1;
	ilvr = false;
    } else if (lsame_(jobvr, "V")) {
	ijobvr = 2;
	ilvr = true;
    } else {
	ijobvr = -1;
	ilvr = false;
    }
    ilv = ilvl || ilvr;

/*     Test the input arguments */

    *info = 0;
    lquery = *lwork == -1;
    if (ijobvl <= 0) {
	*info = -1;
    } else if (ijobvr <= 0) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldvl < 1 || ilvl && *ldvl < *n) {
	*info = -12;
    } else if (*ldvr < 1 || ilvr && *ldvr < *n) {
	*info = -14;
    }

/*     Compute workspace */
/*      (Note: Comments in the code beginning "Workspace:" describe the */
/*       minimal amount of workspace needed at that point in the code, */
/*       as well as the preferred amount for good performance. */
/*       NB refers to the optimal block size for the immediately */
/*       following subroutine, as returned by ILAENV. The workspace is */
/*       computed assuming ILO = 1 and IHI = N, the worst case.) */

    if (*info == 0) {
/* Computing MAX */
	i__1 = 1, i__2 = *n << 3;
	minwrk = std::max(i__1,i__2);
/* Computing MAX */
	i__1 = 1, i__2 = *n * (ilaenv_(&c__1, "DGEQRF", " ", n, &c__1, n, &
		c__0) + 7);
	maxwrk = std::max(i__1,i__2);
/* Computing MAX */
	i__1 = maxwrk, i__2 = *n * (ilaenv_(&c__1, "DORMQR", " ", n, &c__1, n,
		 &c__0) + 7);
	maxwrk = std::max(i__1,i__2);
	if (ilvl) {
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n * (ilaenv_(&c__1, "DORGQR", " ", n, &
		    c__1, n, &c_n1) + 7);
	    maxwrk = std::max(i__1,i__2);
	}
	work[1] = (double) maxwrk;

	if (*lwork < minwrk && ! lquery) {
	    *info = -16;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGGEV ", &i__1);
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
    smlnum = dlamch_("S");
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);
    smlnum = sqrt(smlnum) / eps;
    bignum = 1. / smlnum;

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", n, n, &a[a_offset], lda, &work[1]);
    ilascl = false;
    if (anrm > 0. && anrm < smlnum) {
	anrmto = smlnum;
	ilascl = true;
    } else if (anrm > bignum) {
	anrmto = bignum;
	ilascl = true;
    }
    if (ilascl) {
	dlascl_("G", &c__0, &c__0, &anrm, &anrmto, n, n, &a[a_offset], lda, &
		ierr);
    }

/*     Scale B if max element outside range [SMLNUM,BIGNUM] */

    bnrm = dlange_("M", n, n, &b[b_offset], ldb, &work[1]);
    ilbscl = false;
    if (bnrm > 0. && bnrm < smlnum) {
	bnrmto = smlnum;
	ilbscl = true;
    } else if (bnrm > bignum) {
	bnrmto = bignum;
	ilbscl = true;
    }
    if (ilbscl) {
	dlascl_("G", &c__0, &c__0, &bnrm, &bnrmto, n, n, &b[b_offset], ldb, &
		ierr);
    }

/*     Permute the matrices A, B to isolate eigenvalues if possible */
/*     (Workspace: need 6*N) */

    ileft = 1;
    iright = *n + 1;
    iwrk = iright + *n;
    dggbal_("P", n, &a[a_offset], lda, &b[b_offset], ldb, &ilo, &ihi, &work[
	    ileft], &work[iright], &work[iwrk], &ierr);

/*     Reduce B to triangular form (QR decomposition of B) */
/*     (Workspace: need N, prefer N*NB) */

    irows = ihi + 1 - ilo;
    if (ilv) {
	icols = *n + 1 - ilo;
    } else {
	icols = irows;
    }
    itau = iwrk;
    iwrk = itau + irows;
    i__1 = *lwork + 1 - iwrk;
    dgeqrf_(&irows, &icols, &b[ilo + ilo * b_dim1], ldb, &work[itau], &work[
	    iwrk], &i__1, &ierr);

/*     Apply the orthogonal transformation to matrix A */
/*     (Workspace: need N, prefer N*NB) */

    i__1 = *lwork + 1 - iwrk;
    dormqr_("L", "T", &irows, &icols, &irows, &b[ilo + ilo * b_dim1], ldb, &
	    work[itau], &a[ilo + ilo * a_dim1], lda, &work[iwrk], &i__1, &
	    ierr);

/*     Initialize VL */
/*     (Workspace: need N, prefer N*NB) */

    if (ilvl) {
	dlaset_("Full", n, n, &c_b36, &c_b37, &vl[vl_offset], ldvl)
		;
	if (irows > 1) {
	    i__1 = irows - 1;
	    i__2 = irows - 1;
	    dlacpy_("L", &i__1, &i__2, &b[ilo + 1 + ilo * b_dim1], ldb, &vl[
		    ilo + 1 + ilo * vl_dim1], ldvl);
	}
	i__1 = *lwork + 1 - iwrk;
	dorgqr_(&irows, &irows, &irows, &vl[ilo + ilo * vl_dim1], ldvl, &work[
		itau], &work[iwrk], &i__1, &ierr);
    }

/*     Initialize VR */

    if (ilvr) {
	dlaset_("Full", n, n, &c_b36, &c_b37, &vr[vr_offset], ldvr)
		;
    }

/*     Reduce to generalized Hessenberg form */
/*     (Workspace: none needed) */

    if (ilv) {

/*        Eigenvectors requested -- work on whole matrix. */

	dgghrd_(jobvl, jobvr, n, &ilo, &ihi, &a[a_offset], lda, &b[b_offset],
		ldb, &vl[vl_offset], ldvl, &vr[vr_offset], ldvr, &ierr);
    } else {
	dgghrd_("N", "N", &irows, &c__1, &irows, &a[ilo + ilo * a_dim1], lda,
		&b[ilo + ilo * b_dim1], ldb, &vl[vl_offset], ldvl, &vr[
		vr_offset], ldvr, &ierr);
    }

/*     Perform QZ algorithm (Compute eigenvalues, and optionally, the */
/*     Schur forms and Schur vectors) */
/*     (Workspace: need N) */

    iwrk = itau;
    if (ilv) {
	*(unsigned char *)chtemp = 'S';
    } else {
	*(unsigned char *)chtemp = 'E';
    }
    i__1 = *lwork + 1 - iwrk;
    dhgeqz_(chtemp, jobvl, jobvr, n, &ilo, &ihi, &a[a_offset], lda, &b[
	    b_offset], ldb, &alphar[1], &alphai[1], &beta[1], &vl[vl_offset],
	    ldvl, &vr[vr_offset], ldvr, &work[iwrk], &i__1, &ierr);
    if (ierr != 0) {
	if (ierr > 0 && ierr <= *n) {
	    *info = ierr;
	} else if (ierr > *n && ierr <= *n << 1) {
	    *info = ierr - *n;
	} else {
	    *info = *n + 1;
	}
	goto L110;
    }

/*     Compute Eigenvectors */
/*     (Workspace: need 6*N) */

    if (ilv) {
	if (ilvl) {
	    if (ilvr) {
		*(unsigned char *)chtemp = 'B';
	    } else {
		*(unsigned char *)chtemp = 'L';
	    }
	} else {
	    *(unsigned char *)chtemp = 'R';
	}
	dtgevc_(chtemp, "B", ldumma, n, &a[a_offset], lda, &b[b_offset], ldb,
		&vl[vl_offset], ldvl, &vr[vr_offset], ldvr, n, &in, &work[
		iwrk], &ierr);
	if (ierr != 0) {
	    *info = *n + 2;
	    goto L110;
	}

/*        Undo balancing on VL and VR and normalization */
/*        (Workspace: none needed) */

	if (ilvl) {
	    dggbak_("P", "L", n, &ilo, &ihi, &work[ileft], &work[iright], n, &
		    vl[vl_offset], ldvl, &ierr);
	    i__1 = *n;
	    for (jc = 1; jc <= i__1; ++jc) {
		if (alphai[jc] < 0.) {
		    goto L50;
		}
		temp = 0.;
		if (alphai[jc] == 0.) {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
/* Computing MAX */
			d__2 = temp, d__3 = (d__1 = vl[jr + jc * vl_dim1],
				abs(d__1));
			temp = std::max(d__2,d__3);
/* L10: */
		    }
		} else {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
/* Computing MAX */
			d__3 = temp, d__4 = (d__1 = vl[jr + jc * vl_dim1],
				abs(d__1)) + (d__2 = vl[jr + (jc + 1) *
				vl_dim1], abs(d__2));
			temp = std::max(d__3,d__4);
/* L20: */
		    }
		}
		if (temp < smlnum) {
		    goto L50;
		}
		temp = 1. / temp;
		if (alphai[jc] == 0.) {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
			vl[jr + jc * vl_dim1] *= temp;
/* L30: */
		    }
		} else {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
			vl[jr + jc * vl_dim1] *= temp;
			vl[jr + (jc + 1) * vl_dim1] *= temp;
/* L40: */
		    }
		}
L50:
		;
	    }
	}
	if (ilvr) {
	    dggbak_("P", "R", n, &ilo, &ihi, &work[ileft], &work[iright], n, &
		    vr[vr_offset], ldvr, &ierr);
	    i__1 = *n;
	    for (jc = 1; jc <= i__1; ++jc) {
		if (alphai[jc] < 0.) {
		    goto L100;
		}
		temp = 0.;
		if (alphai[jc] == 0.) {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
/* Computing MAX */
			d__2 = temp, d__3 = (d__1 = vr[jr + jc * vr_dim1],
				abs(d__1));
			temp = std::max(d__2,d__3);
/* L60: */
		    }
		} else {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
/* Computing MAX */
			d__3 = temp, d__4 = (d__1 = vr[jr + jc * vr_dim1],
				abs(d__1)) + (d__2 = vr[jr + (jc + 1) *
				vr_dim1], abs(d__2));
			temp = std::max(d__3,d__4);
/* L70: */
		    }
		}
		if (temp < smlnum) {
		    goto L100;
		}
		temp = 1. / temp;
		if (alphai[jc] == 0.) {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
			vr[jr + jc * vr_dim1] *= temp;
/* L80: */
		    }
		} else {
		    i__2 = *n;
		    for (jr = 1; jr <= i__2; ++jr) {
			vr[jr + jc * vr_dim1] *= temp;
			vr[jr + (jc + 1) * vr_dim1] *= temp;
/* L90: */
		    }
		}
L100:
		;
	    }
	}

/*        End of eigenvector calculation */

    }

/*     Undo scaling if necessary */

    if (ilascl) {
	dlascl_("G", &c__0, &c__0, &anrmto, &anrm, n, &c__1, &alphar[1], n, &
		ierr);
	dlascl_("G", &c__0, &c__0, &anrmto, &anrm, n, &c__1, &alphai[1], n, &
		ierr);
    }

    if (ilbscl) {
	dlascl_("G", &c__0, &c__0, &bnrmto, &bnrm, n, &c__1, &beta[1], n, &
		ierr);
    }

L110:

    work[1] = (double) maxwrk;

    return 0;

/*     End of DGGEV */

} /* dggev_ */

/* Subroutine */ int dggevx_(const char *balanc, const char *jobvl, const char *jobvr, const char *
	sense, integer *n, double *a, integer *lda, double *b,
	integer *ldb, double *alphar, double *alphai, double *
	beta, double *vl, integer *ldvl, double *vr, integer *ldvr,
	integer *ilo, integer *ihi, double *lscale, double *rscale,
	double *abnrm, double *bbnrm, double *rconde, double *
	rcondv, double *work, integer *lwork, integer *iwork, bool *
	bwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__0 = 0;
	static double c_b59 = 0.;
	static double c_b60 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, vl_dim1, vl_offset, vr_dim1,
	    vr_offset, i__1, i__2;
    double d__1, d__2, d__3, d__4;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    integer i__, j, m, jc, in, mm, jr;
    double eps;
    bool ilv, pair;
    double anrm, bnrm;
    integer ierr, itau;
    double temp;
    bool ilvl, ilvr;
    integer iwrk, iwrk1;
    integer icols;
    bool noscl;
    integer irows;
    bool ilascl, ilbscl;
    bool ldumma[1];
    char chtemp[1];
    double bignum;
    integer ijobvl;
    integer ijobvr;
    bool wantsb;
    double anrmto;
    bool wantse;
    double bnrmto;
    integer minwrk, maxwrk;
    bool wantsn;
    double smlnum;
    bool lquery, wantsv;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGGEVX computes for a pair of N-by-N real nonsymmetric matrices (A,B) */
/*  the generalized eigenvalues, and optionally, the left and/or right */
/*  generalized eigenvectors. */

/*  Optionally also, it computes a balancing transformation to improve */
/*  the conditioning of the eigenvalues and eigenvectors (ILO, IHI, */
/*  LSCALE, RSCALE, ABNRM, and BBNRM), reciprocal condition numbers for */
/*  the eigenvalues (RCONDE), and reciprocal condition numbers for the */
/*  right eigenvectors (RCONDV). */

/*  A generalized eigenvalue for a pair of matrices (A,B) is a scalar */
/*  lambda or a ratio alpha/beta = lambda, such that A - lambda*B is */
/*  singular. It is usually represented as the pair (alpha,beta), as */
/*  there is a reasonable interpretation for beta=0, and even for both */
/*  being zero. */

/*  The right eigenvector v(j) corresponding to the eigenvalue lambda(j) */
/*  of (A,B) satisfies */

/*                   A * v(j) = lambda(j) * B * v(j) . */

/*  The left eigenvector u(j) corresponding to the eigenvalue lambda(j) */
/*  of (A,B) satisfies */

/*                   u(j)**H * A  = lambda(j) * u(j)**H * B. */

/*  where u(j)**H is the conjugate-transpose of u(j). */


/*  Arguments */
/*  ========= */

/*  BALANC  (input) CHARACTER*1 */
/*          Specifies the balance option to be performed. */
/*          = 'N':  do not diagonally scale or permute; */
/*          = 'P':  permute only; */
/*          = 'S':  scale only; */
/*          = 'B':  both permute and scale. */
/*          Computed reciprocal condition numbers will be for the */
/*          matrices after permuting and/or balancing. Permuting does */
/*          not change condition numbers (in exact arithmetic), but */
/*          balancing does. */

/*  JOBVL   (input) CHARACTER*1 */
/*          = 'N':  do not compute the left generalized eigenvectors; */
/*          = 'V':  compute the left generalized eigenvectors. */

/*  JOBVR   (input) CHARACTER*1 */
/*          = 'N':  do not compute the right generalized eigenvectors; */
/*          = 'V':  compute the right generalized eigenvectors. */

/*  SENSE   (input) CHARACTER*1 */
/*          Determines which reciprocal condition numbers are computed. */
/*          = 'N': none are computed; */
/*          = 'E': computed for eigenvalues only; */
/*          = 'V': computed for eigenvectors only; */
/*          = 'B': computed for eigenvalues and eigenvectors. */

/*  N       (input) INTEGER */
/*          The order of the matrices A, B, VL, and VR.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the matrix A in the pair (A,B). */
/*          On exit, A has been overwritten. If JOBVL='V' or JOBVR='V' */
/*          or both, then A contains the first part of the real Schur */
/*          form of the "balanced" versions of the input A and B. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  LDA >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB, N) */
/*          On entry, the matrix B in the pair (A,B). */
/*          On exit, B has been overwritten. If JOBVL='V' or JOBVR='V' */
/*          or both, then B contains the second part of the real Schur */
/*          form of the "balanced" versions of the input A and B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  LDB >= max(1,N). */

/*  ALPHAR  (output) DOUBLE PRECISION array, dimension (N) */
/*  ALPHAI  (output) DOUBLE PRECISION array, dimension (N) */
/*  BETA    (output) DOUBLE PRECISION array, dimension (N) */
/*          On exit, (ALPHAR(j) + ALPHAI(j)*i)/BETA(j), j=1,...,N, will */
/*          be the generalized eigenvalues.  If ALPHAI(j) is zero, then */
/*          the j-th eigenvalue is real; if positive, then the j-th and */
/*          (j+1)-st eigenvalues are a complex conjugate pair, with */
/*          ALPHAI(j+1) negative. */

/*          Note: the quotients ALPHAR(j)/BETA(j) and ALPHAI(j)/BETA(j) */
/*          may easily over- or underflow, and BETA(j) may even be zero. */
/*          Thus, the user should avoid naively computing the ratio */
/*          ALPHA/BETA. However, ALPHAR and ALPHAI will be always less */
/*          than and usually comparable with norm(A) in magnitude, and */
/*          BETA always less than and usually comparable with norm(B). */

/*  VL      (output) DOUBLE PRECISION array, dimension (LDVL,N) */
/*          If JOBVL = 'V', the left eigenvectors u(j) are stored one */
/*          after another in the columns of VL, in the same order as */
/*          their eigenvalues. If the j-th eigenvalue is real, then */
/*          u(j) = VL(:,j), the j-th column of VL. If the j-th and */
/*          (j+1)-th eigenvalues form a complex conjugate pair, then */
/*          u(j) = VL(:,j)+i*VL(:,j+1) and u(j+1) = VL(:,j)-i*VL(:,j+1). */
/*          Each eigenvector will be scaled so the largest component have */
/*          abs(real part) + abs(imag. part) = 1. */
/*          Not referenced if JOBVL = 'N'. */

/*  LDVL    (input) INTEGER */
/*          The leading dimension of the matrix VL. LDVL >= 1, and */
/*          if JOBVL = 'V', LDVL >= N. */

/*  VR      (output) DOUBLE PRECISION array, dimension (LDVR,N) */
/*          If JOBVR = 'V', the right eigenvectors v(j) are stored one */
/*          after another in the columns of VR, in the same order as */
/*          their eigenvalues. If the j-th eigenvalue is real, then */
/*          v(j) = VR(:,j), the j-th column of VR. If the j-th and */
/*          (j+1)-th eigenvalues form a complex conjugate pair, then */
/*          v(j) = VR(:,j)+i*VR(:,j+1) and v(j+1) = VR(:,j)-i*VR(:,j+1). */
/*          Each eigenvector will be scaled so the largest component have */
/*          abs(real part) + abs(imag. part) = 1. */
/*          Not referenced if JOBVR = 'N'. */

/*  LDVR    (input) INTEGER */
/*          The leading dimension of the matrix VR. LDVR >= 1, and */
/*          if JOBVR = 'V', LDVR >= N. */

/*  ILO     (output) INTEGER */
/*  IHI     (output) INTEGER */
/*          ILO and IHI are integer values such that on exit */
/*          A(i,j) = 0 and B(i,j) = 0 if i > j and */
/*          j = 1,...,ILO-1 or i = IHI+1,...,N. */
/*          If BALANC = 'N' or 'S', ILO = 1 and IHI = N. */

/*  LSCALE  (output) DOUBLE PRECISION array, dimension (N) */
/*          Details of the permutations and scaling factors applied */
/*          to the left side of A and B.  If PL(j) is the index of the */
/*          row interchanged with row j, and DL(j) is the scaling */
/*          factor applied to row j, then */
/*            LSCALE(j) = PL(j)  for j = 1,...,ILO-1 */
/*                      = DL(j)  for j = ILO,...,IHI */
/*                      = PL(j)  for j = IHI+1,...,N. */
/*          The order in which the interchanges are made is N to IHI+1, */
/*          then 1 to ILO-1. */

/*  RSCALE  (output) DOUBLE PRECISION array, dimension (N) */
/*          Details of the permutations and scaling factors applied */
/*          to the right side of A and B.  If PR(j) is the index of the */
/*          column interchanged with column j, and DR(j) is the scaling */
/*          factor applied to column j, then */
/*            RSCALE(j) = PR(j)  for j = 1,...,ILO-1 */
/*                      = DR(j)  for j = ILO,...,IHI */
/*                      = PR(j)  for j = IHI+1,...,N */
/*          The order in which the interchanges are made is N to IHI+1, */
/*          then 1 to ILO-1. */

/*  ABNRM   (output) DOUBLE PRECISION */
/*          The one-norm of the balanced matrix A. */

/*  BBNRM   (output) DOUBLE PRECISION */
/*          The one-norm of the balanced matrix B. */

/*  RCONDE  (output) DOUBLE PRECISION array, dimension (N) */
/*          If SENSE = 'E' or 'B', the reciprocal condition numbers of */
/*          the eigenvalues, stored in consecutive elements of the array. */
/*          For a complex conjugate pair of eigenvalues two consecutive */
/*          elements of RCONDE are set to the same value. Thus RCONDE(j), */
/*          RCONDV(j), and the j-th columns of VL and VR all correspond */
/*          to the j-th eigenpair. */
/*          If SENSE = 'N or 'V', RCONDE is not referenced. */

/*  RCONDV  (output) DOUBLE PRECISION array, dimension (N) */
/*          If SENSE = 'V' or 'B', the estimated reciprocal condition */
/*          numbers of the eigenvectors, stored in consecutive elements */
/*          of the array. For a complex eigenvector two consecutive */
/*          elements of RCONDV are set to the same value. If the */
/*          eigenvalues cannot be reordered to compute RCONDV(j), */
/*          RCONDV(j) is set to 0; this can only occur when the true */
/*          value would be very small anyway. */
/*          If SENSE = 'N' or 'E', RCONDV is not referenced. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. LWORK >= max(1,2*N). */
/*          If BALANC = 'S' or 'B', or JOBVL = 'V', or JOBVR = 'V', */
/*          LWORK >= max(1,6*N). */
/*          If SENSE = 'E' or 'B', LWORK >= max(1,10*N). */
/*          If SENSE = 'V' or 'B', LWORK >= 2*N*N+8*N+16. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  IWORK   (workspace) INTEGER array, dimension (N+6) */
/*          If SENSE = 'E', IWORK is not referenced. */

/*  BWORK   (workspace) LOGICAL array, dimension (N) */
/*          If SENSE = 'N', BWORK is not referenced. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          = 1,...,N: */
/*                The QZ iteration failed.  No eigenvectors have been */
/*                calculated, but ALPHAR(j), ALPHAI(j), and BETA(j) */
/*                should be correct for j=INFO+1,...,N. */
/*          > N:  =N+1: other than QZ iteration failed in DHGEQZ. */
/*                =N+2: error return from DTGEVC. */

/*  Further Details */
/*  =============== */

/*  Balancing a matrix pair (A,B) includes, first, permuting rows and */
/*  columns to isolate eigenvalues, second, applying diagonal similarity */
/*  transformation to the rows and columns to make the rows and columns */
/*  as close in norm as possible. The computed reciprocal condition */
/*  numbers correspond to the balanced matrix. Permuting rows and columns */
/*  will not change the condition numbers (in exact arithmetic) but */
/*  diagonal scaling will.  For further explanation of balancing, see */
/*  section 4.11.1.2 of LAPACK Users' Guide. */

/*  An approximate error bound on the chordal distance between the i-th */
/*  computed generalized eigenvalue w and the corresponding exact */
/*  eigenvalue lambda is */

/*       chord(w, lambda) <= EPS * norm(ABNRM, BBNRM) / RCONDE(I) */

/*  An approximate error bound for the angle between the i-th computed */
/*  eigenvector VL(i) or VR(i) is given by */

/*       EPS * norm(ABNRM, BBNRM) / DIF(i). */

/*  For further explanation of the reciprocal condition numbers RCONDE */
/*  and RCONDV, see section 4.11 of LAPACK User's Guide. */

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

/*     Decode the input arguments */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --alphar;
    --alphai;
    --beta;
    vl_dim1 = *ldvl;
    vl_offset = 1 + vl_dim1;
    vl -= vl_offset;
    vr_dim1 = *ldvr;
    vr_offset = 1 + vr_dim1;
    vr -= vr_offset;
    --lscale;
    --rscale;
    --rconde;
    --rcondv;
    --work;
    --iwork;
    --bwork;

    /* Function Body */
    if (lsame_(jobvl, "N")) {
	ijobvl = 1;
	ilvl = false;
    } else if (lsame_(jobvl, "V")) {
	ijobvl = 2;
	ilvl = true;
    } else {
	ijobvl = -1;
	ilvl = false;
    }

    if (lsame_(jobvr, "N")) {
	ijobvr = 1;
	ilvr = false;
    } else if (lsame_(jobvr, "V")) {
	ijobvr = 2;
	ilvr = true;
    } else {
	ijobvr = -1;
	ilvr = false;
    }
    ilv = ilvl || ilvr;

    noscl = lsame_(balanc, "N") || lsame_(balanc, "P");
    wantsn = lsame_(sense, "N");
    wantse = lsame_(sense, "E");
    wantsv = lsame_(sense, "V");
    wantsb = lsame_(sense, "B");

/*     Test the input arguments */

    *info = 0;
    lquery = *lwork == -1;
    if (! (lsame_(balanc, "N") || lsame_(balanc, "S") || lsame_(balanc, "P")
	    || lsame_(balanc, "B"))) {
	*info = -1;
    } else if (ijobvl <= 0) {
	*info = -2;
    } else if (ijobvr <= 0) {
	*info = -3;
    } else if (! (wantsn || wantse || wantsb || wantsv)) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -9;
    } else if (*ldvl < 1 || ilvl && *ldvl < *n) {
	*info = -14;
    } else if (*ldvr < 1 || ilvr && *ldvr < *n) {
	*info = -16;
    }

/*     Compute workspace */
/*      (Note: Comments in the code beginning "Workspace:" describe the */
/*       minimal amount of workspace needed at that point in the code, */
/*       as well as the preferred amount for good performance. */
/*       NB refers to the optimal block size for the immediately */
/*       following subroutine, as returned by ILAENV. The workspace is */
/*       computed assuming ILO = 1 and IHI = N, the worst case.) */

    if (*info == 0) {
	if (*n == 0) {
	    minwrk = 1;
	    maxwrk = 1;
	} else {
	    if (noscl && ! ilv) {
		minwrk = *n << 1;
	    } else {
		minwrk = *n * 6;
	    }
	    if (wantse || wantsb) {
		minwrk = *n * 10;
	    }
	    if (wantsv || wantsb) {
/* Computing MAX */
		i__1 = minwrk, i__2 = (*n << 1) * (*n + 4) + 16;
		minwrk = std::max(i__1,i__2);
	    }
	    maxwrk = minwrk;
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n + *n * ilaenv_(&c__1, "DGEQRF", " ", n, &
		    c__1, n, &c__0);
	    maxwrk = std::max(i__1,i__2);
/* Computing MAX */
	    i__1 = maxwrk, i__2 = *n + *n * ilaenv_(&c__1, "DORMQR", " ", n, &
		    c__1, n, &c__0);
	    maxwrk = std::max(i__1,i__2);
	    if (ilvl) {
/* Computing MAX */
		i__1 = maxwrk, i__2 = *n + *n * ilaenv_(&c__1, "DORGQR",
			" ", n, &c__1, n, &c__0);
		maxwrk = std::max(i__1,i__2);
	    }
	}
	work[1] = (double) maxwrk;

	if (*lwork < minwrk && ! lquery) {
	    *info = -26;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGGEVX", &i__1);
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
    smlnum = dlamch_("S");
    bignum = 1. / smlnum;
    dlabad_(&smlnum, &bignum);
    smlnum = sqrt(smlnum) / eps;
    bignum = 1. / smlnum;

/*     Scale A if max element outside range [SMLNUM,BIGNUM] */

    anrm = dlange_("M", n, n, &a[a_offset], lda, &work[1]);
    ilascl = false;
    if (anrm > 0. && anrm < smlnum) {
	anrmto = smlnum;
	ilascl = true;
    } else if (anrm > bignum) {
	anrmto = bignum;
	ilascl = true;
    }
    if (ilascl) {
	dlascl_("G", &c__0, &c__0, &anrm, &anrmto, n, n, &a[a_offset], lda, &
		ierr);
    }

/*     Scale B if max element outside range [SMLNUM,BIGNUM] */

    bnrm = dlange_("M", n, n, &b[b_offset], ldb, &work[1]);
    ilbscl = false;
    if (bnrm > 0. && bnrm < smlnum) {
	bnrmto = smlnum;
	ilbscl = true;
    } else if (bnrm > bignum) {
	bnrmto = bignum;
	ilbscl = true;
    }
    if (ilbscl) {
	dlascl_("G", &c__0, &c__0, &bnrm, &bnrmto, n, n, &b[b_offset], ldb, &
		ierr);
    }

/*     Permute and/or balance the matrix pair (A,B) */
/*     (Workspace: need 6*N if BALANC = 'S' or 'B', 1 otherwise) */

    dggbal_(balanc, n, &a[a_offset], lda, &b[b_offset], ldb, ilo, ihi, &
	    lscale[1], &rscale[1], &work[1], &ierr);

/*     Compute ABNRM and BBNRM */

    *abnrm = dlange_("1", n, n, &a[a_offset], lda, &work[1]);
    if (ilascl) {
	work[1] = *abnrm;
	dlascl_("G", &c__0, &c__0, &anrmto, &anrm, &c__1, &c__1, &work[1], &
		c__1, &ierr);
	*abnrm = work[1];
    }

    *bbnrm = dlange_("1", n, n, &b[b_offset], ldb, &work[1]);
    if (ilbscl) {
	work[1] = *bbnrm;
	dlascl_("G", &c__0, &c__0, &bnrmto, &bnrm, &c__1, &c__1, &work[1], &
		c__1, &ierr);
	*bbnrm = work[1];
    }

/*     Reduce B to triangular form (QR decomposition of B) */
/*     (Workspace: need N, prefer N*NB ) */

    irows = *ihi + 1 - *ilo;
    if (ilv || ! wantsn) {
	icols = *n + 1 - *ilo;
    } else {
	icols = irows;
    }
    itau = 1;
    iwrk = itau + irows;
    i__1 = *lwork + 1 - iwrk;
    dgeqrf_(&irows, &icols, &b[*ilo + *ilo * b_dim1], ldb, &work[itau], &work[
	    iwrk], &i__1, &ierr);

/*     Apply the orthogonal transformation to A */
/*     (Workspace: need N, prefer N*NB) */

    i__1 = *lwork + 1 - iwrk;
    dormqr_("L", "T", &irows, &icols, &irows, &b[*ilo + *ilo * b_dim1], ldb, &
	    work[itau], &a[*ilo + *ilo * a_dim1], lda, &work[iwrk], &i__1, &
	    ierr);

/*     Initialize VL and/or VR */
/*     (Workspace: need N, prefer N*NB) */

    if (ilvl) {
	dlaset_("Full", n, n, &c_b59, &c_b60, &vl[vl_offset], ldvl)
		;
	if (irows > 1) {
	    i__1 = irows - 1;
	    i__2 = irows - 1;
	    dlacpy_("L", &i__1, &i__2, &b[*ilo + 1 + *ilo * b_dim1], ldb, &vl[
		    *ilo + 1 + *ilo * vl_dim1], ldvl);
	}
	i__1 = *lwork + 1 - iwrk;
	dorgqr_(&irows, &irows, &irows, &vl[*ilo + *ilo * vl_dim1], ldvl, &
		work[itau], &work[iwrk], &i__1, &ierr);
    }

    if (ilvr) {
	dlaset_("Full", n, n, &c_b59, &c_b60, &vr[vr_offset], ldvr)
		;
    }

/*     Reduce to generalized Hessenberg form */
/*     (Workspace: none needed) */

    if (ilv || ! wantsn) {

/*        Eigenvectors requested -- work on whole matrix. */

	dgghrd_(jobvl, jobvr, n, ilo, ihi, &a[a_offset], lda, &b[b_offset],
		ldb, &vl[vl_offset], ldvl, &vr[vr_offset], ldvr, &ierr);
    } else {
	dgghrd_("N", "N", &irows, &c__1, &irows, &a[*ilo + *ilo * a_dim1],
		lda, &b[*ilo + *ilo * b_dim1], ldb, &vl[vl_offset], ldvl, &vr[
		vr_offset], ldvr, &ierr);
    }

/*     Perform QZ algorithm (Compute eigenvalues, and optionally, the */
/*     Schur forms and Schur vectors) */
/*     (Workspace: need N) */

    if (ilv || ! wantsn) {
	*(unsigned char *)chtemp = 'S';
    } else {
	*(unsigned char *)chtemp = 'E';
    }

    dhgeqz_(chtemp, jobvl, jobvr, n, ilo, ihi, &a[a_offset], lda, &b[b_offset]
, ldb, &alphar[1], &alphai[1], &beta[1], &vl[vl_offset], ldvl, &
	    vr[vr_offset], ldvr, &work[1], lwork, &ierr);
    if (ierr != 0) {
	if (ierr > 0 && ierr <= *n) {
	    *info = ierr;
	} else if (ierr > *n && ierr <= *n << 1) {
	    *info = ierr - *n;
	} else {
	    *info = *n + 1;
	}
	goto L130;
    }

/*     Compute Eigenvectors and estimate condition numbers if desired */
/*     (Workspace: DTGEVC: need 6*N */
/*                 DTGSNA: need 2*N*(N+2)+16 if SENSE = 'V' or 'B', */
/*                         need N otherwise ) */

    if (ilv || ! wantsn) {
	if (ilv) {
	    if (ilvl) {
		if (ilvr) {
		    *(unsigned char *)chtemp = 'B';
		} else {
		    *(unsigned char *)chtemp = 'L';
		}
	    } else {
		*(unsigned char *)chtemp = 'R';
	    }

	    dtgevc_(chtemp, "B", ldumma, n, &a[a_offset], lda, &b[b_offset],
		    ldb, &vl[vl_offset], ldvl, &vr[vr_offset], ldvr, n, &in, &
		    work[1], &ierr);
	    if (ierr != 0) {
		*info = *n + 2;
		goto L130;
	    }
	}

	if (! wantsn) {

/*           compute eigenvectors (DTGEVC) and estimate condition */
/*           numbers (DTGSNA). Note that the definition of the condition */
/*           number is not invariant under transformation (u,v) to */
/*           (Q*u, Z*v), where (u,v) are eigenvectors of the generalized */
/*           Schur form (S,T), Q and Z are orthogonal matrices. In order */
/*           to avoid using extra 2*N*N workspace, we have to recalculate */
/*           eigenvectors and estimate one condition numbers at a time. */

	    pair = false;
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {

		if (pair) {
		    pair = false;
		    goto L20;
		}
		mm = 1;
		if (i__ < *n) {
		    if (a[i__ + 1 + i__ * a_dim1] != 0.) {
			pair = true;
			mm = 2;
		    }
		}

		i__2 = *n;
		for (j = 1; j <= i__2; ++j) {
		    bwork[j] = false;
/* L10: */
		}
		if (mm == 1) {
		    bwork[i__] = true;
		} else if (mm == 2) {
		    bwork[i__] = true;
		    bwork[i__ + 1] = true;
		}

		iwrk = mm * *n + 1;
		iwrk1 = iwrk + mm * *n;

/*              Compute a pair of left and right eigenvectors. */
/*              (compute workspace: need up to 4*N + 6*N) */

		if (wantse || wantsb) {
		    dtgevc_("B", "S", &bwork[1], n, &a[a_offset], lda, &b[
			    b_offset], ldb, &work[1], n, &work[iwrk], n, &mm,
			    &m, &work[iwrk1], &ierr);
		    if (ierr != 0) {
			*info = *n + 2;
			goto L130;
		    }
		}

		i__2 = *lwork - iwrk1 + 1;
		dtgsna_(sense, "S", &bwork[1], n, &a[a_offset], lda, &b[
			b_offset], ldb, &work[1], n, &work[iwrk], n, &rconde[
			i__], &rcondv[i__], &mm, &m, &work[iwrk1], &i__2, &
			iwork[1], &ierr);

L20:
		;
	    }
	}
    }

/*     Undo balancing on VL and VR and normalization */
/*     (Workspace: none needed) */

    if (ilvl) {
	dggbak_(balanc, "L", n, ilo, ihi, &lscale[1], &rscale[1], n, &vl[
		vl_offset], ldvl, &ierr);

	i__1 = *n;
	for (jc = 1; jc <= i__1; ++jc) {
	    if (alphai[jc] < 0.) {
		goto L70;
	    }
	    temp = 0.;
	    if (alphai[jc] == 0.) {
		i__2 = *n;
		for (jr = 1; jr <= i__2; ++jr) {
/* Computing MAX */
		    d__2 = temp, d__3 = (d__1 = vl[jr + jc * vl_dim1], abs(
			    d__1));
		    temp = std::max(d__2,d__3);
/* L30: */
		}
	    } else {
		i__2 = *n;
		for (jr = 1; jr <= i__2; ++jr) {
/* Computing MAX */
		    d__3 = temp, d__4 = (d__1 = vl[jr + jc * vl_dim1], abs(
			    d__1)) + (d__2 = vl[jr + (jc + 1) * vl_dim1], abs(
			    d__2));
		    temp = std::max(d__3,d__4);
/* L40: */
		}
	    }
	    if (temp < smlnum) {
		goto L70;
	    }
	    temp = 1. / temp;
	    if (alphai[jc] == 0.) {
		i__2 = *n;
		for (jr = 1; jr <= i__2; ++jr) {
		    vl[jr + jc * vl_dim1] *= temp;
/* L50: */
		}
	    } else {
		i__2 = *n;
		for (jr = 1; jr <= i__2; ++jr) {
		    vl[jr + jc * vl_dim1] *= temp;
		    vl[jr + (jc + 1) * vl_dim1] *= temp;
/* L60: */
		}
	    }
L70:
	    ;
	}
    }
    if (ilvr) {
	dggbak_(balanc, "R", n, ilo, ihi, &lscale[1], &rscale[1], n, &vr[
		vr_offset], ldvr, &ierr);
	i__1 = *n;
	for (jc = 1; jc <= i__1; ++jc) {
	    if (alphai[jc] < 0.) {
		goto L120;
	    }
	    temp = 0.;
	    if (alphai[jc] == 0.) {
		i__2 = *n;
		for (jr = 1; jr <= i__2; ++jr) {
/* Computing MAX */
		    d__2 = temp, d__3 = (d__1 = vr[jr + jc * vr_dim1], abs(
			    d__1));
		    temp = std::max(d__2,d__3);
/* L80: */
		}
	    } else {
		i__2 = *n;
		for (jr = 1; jr <= i__2; ++jr) {
/* Computing MAX */
		    d__3 = temp, d__4 = (d__1 = vr[jr + jc * vr_dim1], abs(
			    d__1)) + (d__2 = vr[jr + (jc + 1) * vr_dim1], abs(
			    d__2));
		    temp = std::max(d__3,d__4);
/* L90: */
		}
	    }
	    if (temp < smlnum) {
		goto L120;
	    }
	    temp = 1. / temp;
	    if (alphai[jc] == 0.) {
		i__2 = *n;
		for (jr = 1; jr <= i__2; ++jr) {
		    vr[jr + jc * vr_dim1] *= temp;
/* L100: */
		}
	    } else {
		i__2 = *n;
		for (jr = 1; jr <= i__2; ++jr) {
		    vr[jr + jc * vr_dim1] *= temp;
		    vr[jr + (jc + 1) * vr_dim1] *= temp;
/* L110: */
		}
	    }
L120:
	    ;
	}
    }

/*     Undo scaling if necessary */

    if (ilascl) {
	dlascl_("G", &c__0, &c__0, &anrmto, &anrm, n, &c__1, &alphar[1], n, &
		ierr);
	dlascl_("G", &c__0, &c__0, &anrmto, &anrm, n, &c__1, &alphai[1], n, &
		ierr);
    }

    if (ilbscl) {
	dlascl_("G", &c__0, &c__0, &bnrmto, &bnrm, n, &c__1, &beta[1], n, &
		ierr);
    }

L130:
    work[1] = (double) maxwrk;

    return 0;

/*     End of DGGEVX */

} /* dggevx_ */

/* Subroutine */ int dggglm_(integer *n, integer *m, integer *p, double *
	a, integer *lda, double *b, integer *ldb, double *d__,
	double *x, double *y, double *work, integer *lwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static double c_b32 = -1.;
	static double c_b34 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    integer i__, nb, np, nb1, nb2, nb3, nb4, lopt;
    integer lwkmin;
	integer lwkopt;
    bool lquery;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGGGLM solves a general Gauss-Markov linear model (GLM) problem: */

/*          minimize || y ||_2   subject to   d = A*x + B*y */
/*              x */

/*  where A is an N-by-M matrix, B is an N-by-P matrix, and d is a */
/*  given N-vector. It is assumed that M <= N <= M+P, and */

/*             rank(A) = M    and    rank( A B ) = N. */

/*  Under these assumptions, the constrained equation is always */
/*  consistent, and there is a unique solution x and a minimal 2-norm */
/*  solution y, which is obtained using a generalized QR factorization */
/*  of the matrices (A, B) given by */

/*     A = Q*(R),   B = Q*T*Z. */
/*           (0) */

/*  In particular, if matrix B is square nonsingular, then the problem */
/*  GLM is equivalent to the following weighted linear least squares */
/*  problem */

/*               minimize || inv(B)*(d-A*x) ||_2 */
/*                   x */

/*  where inv(B) denotes the inverse of B. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of rows of the matrices A and B.  N >= 0. */

/*  M       (input) INTEGER */
/*          The number of columns of the matrix A.  0 <= M <= N. */

/*  P       (input) INTEGER */
/*          The number of columns of the matrix B.  P >= N-M. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,M) */
/*          On entry, the N-by-M matrix A. */
/*          On exit, the upper triangular part of the array A contains */
/*          the M-by-M upper triangular matrix R. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,P) */
/*          On entry, the N-by-P matrix B. */
/*          On exit, if N <= P, the upper triangle of the subarray */
/*          B(1:N,P-N+1:P) contains the N-by-N upper triangular matrix T; */
/*          if N > P, the elements on and above the (N-P)th subdiagonal */
/*          contain the N-by-P upper trapezoidal matrix T. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,N). */

/*  D       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, D is the left hand side of the GLM equation. */
/*          On exit, D is destroyed. */

/*  X       (output) DOUBLE PRECISION array, dimension (M) */
/*  Y       (output) DOUBLE PRECISION array, dimension (P) */
/*          On exit, X and Y are the solutions of the GLM problem. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. LWORK >= max(1,N+M+P). */
/*          For optimum performance, LWORK >= M+min(N,P)+max(N,P)*NB, */
/*          where NB is an upper bound for the optimal blocksizes for */
/*          DGEQRF, SGERQF, DORMQR and SORMRQ. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          = 1:  the upper triangular factor R associated with A in the */
/*                generalized QR factorization of the pair (A, B) is */
/*                singular, so that rank(A) < M; the least squares */
/*                solution could not be computed. */
/*          = 2:  the bottom (N-M) by (N-M) part of the upper trapezoidal */
/*                factor T associated with B in the generalized QR */
/*                factorization of the pair (A, B) is singular, so that */
/*                rank( A B ) < N; the least squares solution could not */
/*                be computed. */

/*  =================================================================== */

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

/*     Test the input parameters */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --d__;
    --x;
    --y;
    --work;

    /* Function Body */
    *info = 0;
    np = std::min(*n,*p);
    lquery = *lwork == -1;
    if (*n < 0) {
	*info = -1;
    } else if (*m < 0 || *m > *n) {
	*info = -2;
    } else if (*p < 0 || *p < *n - *m) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -7;
    }

/*     Calculate workspace */

    if (*info == 0) {
	if (*n == 0) {
	    lwkmin = 1;
	    lwkopt = 1;
	} else {
	    nb1 = ilaenv_(&c__1, "DGEQRF", " ", n, m, &c_n1, &c_n1);
	    nb2 = ilaenv_(&c__1, "DGERQF", " ", n, m, &c_n1, &c_n1);
	    nb3 = ilaenv_(&c__1, "DORMQR", " ", n, m, p, &c_n1);
	    nb4 = ilaenv_(&c__1, "DORMRQ", " ", n, m, p, &c_n1);
/* Computing MAX */
	    i__1 = std::max(nb1,nb2), i__1 = std::max(i__1,nb3);
	    nb = std::max(i__1,nb4);
	    lwkmin = *m + *n + *p;
	    lwkopt = *m + np + std::max(*n,*p) * nb;
	}
	work[1] = (double) lwkopt;

	if (*lwork < lwkmin && ! lquery) {
	    *info = -12;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGGGLM", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Compute the GQR factorization of matrices A and B: */

/*            Q'*A = ( R11 ) M,    Q'*B*Z' = ( T11   T12 ) M */
/*                   (  0  ) N-M             (  0    T22 ) N-M */
/*                      M                     M+P-N  N-M */

/*     where R11 and T22 are upper triangular, and Q and Z are */
/*     orthogonal. */

    i__1 = *lwork - *m - np;
    dggqrf_(n, m, p, &a[a_offset], lda, &work[1], &b[b_offset], ldb, &work[*m
	    + 1], &work[*m + np + 1], &i__1, info);
    lopt = (integer) work[*m + np + 1];

/*     Update left-hand-side vector d = Q'*d = ( d1 ) M */
/*                                             ( d2 ) N-M */

    i__1 = std::max(1_integer,*n);
    i__2 = *lwork - *m - np;
    dormqr_("Left", "Transpose", n, &c__1, m, &a[a_offset], lda, &work[1], &
	    d__[1], &i__1, &work[*m + np + 1], &i__2, info);
/* Computing MAX */
    i__1 = lopt, i__2 = (integer) work[*m + np + 1];
    lopt = std::max(i__1,i__2);

/*     Solve T22*y2 = d2 for y2 */

    if (*n > *m) {
	i__1 = *n - *m;
	i__2 = *n - *m;
	dtrtrs_("Upper", "No transpose", "Non unit", &i__1, &c__1, &b[*m + 1
		+ (*m + *p - *n + 1) * b_dim1], ldb, &d__[*m + 1], &i__2,
		info);

	if (*info > 0) {
	    *info = 1;
	    return 0;
	}

	i__1 = *n - *m;
	dcopy_(&i__1, &d__[*m + 1], &c__1, &y[*m + *p - *n + 1], &c__1);
    }

/*     Set y1 = 0 */

    i__1 = *m + *p - *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	y[i__] = 0.;
/* L10: */
    }

/*     Update d1 = d1 - T12*y2 */

    i__1 = *n - *m;
    dgemv_("No transpose", m, &i__1, &c_b32, &b[(*m + *p - *n + 1) * b_dim1 +
	    1], ldb, &y[*m + *p - *n + 1], &c__1, &c_b34, &d__[1], &c__1);

/*     Solve triangular system: R11*x = d1 */

    if (*m > 0) {
	dtrtrs_("Upper", "No Transpose", "Non unit", m, &c__1, &a[a_offset],
		lda, &d__[1], m, info);

	if (*info > 0) {
	    *info = 2;
	    return 0;
	}

/*        Copy D to X */

	dcopy_(m, &d__[1], &c__1, &x[1], &c__1);
    }

/*     Backward transformation y = Z'*y */

/* Computing MAX */
    i__1 = 1, i__2 = *n - *p + 1;
    i__3 = std::max(1_integer,*p);
    i__4 = *lwork - *m - np;
    dormrq_("Left", "Transpose", p, &c__1, &np, &b[std::max(i__1, i__2)+ b_dim1],
	    ldb, &work[*m + 1], &y[1], &i__3, &work[*m + np + 1], &i__4, info);
/* Computing MAX */
    i__1 = lopt, i__2 = (integer) work[*m + np + 1];
    work[1] = (double) (*m + np + std::max(i__1,i__2));

    return 0;

/*     End of DGGGLM */

} /* dggglm_ */

/* Subroutine */ int dgghrd_(const char *compq, const char *compz, integer *n, integer *
	ilo, integer *ihi, double *a, integer *lda, double *b,
	integer *ldb, double *q, integer *ldq, double *z__, integer *
	ldz, integer *info)
{
	/* Table of constant values */
	static double c_b10 = 0.;
	static double c_b11 = 1.;
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, q_dim1, q_offset, z_dim1,
	    z_offset, i__1, i__2, i__3;

    /* Local variables */
    double c__, s;
    bool ilq, ilz;
    integer jcol;
    double temp;
    integer jrow;
    integer icompq, icompz;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGGHRD reduces a pair of real matrices (A,B) to generalized upper */
/*  Hessenberg form using orthogonal transformations, where A is a */
/*  general matrix and B is upper triangular.  The form of the */
/*  generalized eigenvalue problem is */
/*     A*x = lambda*B*x, */
/*  and B is typically made upper triangular by computing its QR */
/*  factorization and moving the orthogonal matrix Q to the left side */
/*  of the equation. */

/*  This subroutine simultaneously reduces A to a Hessenberg matrix H: */
/*     Q**T*A*Z = H */
/*  and transforms B to another upper triangular matrix T: */
/*     Q**T*B*Z = T */
/*  in order to reduce the problem to its standard form */
/*     H*y = lambda*T*y */
/*  where y = Z**T*x. */

/*  The orthogonal matrices Q and Z are determined as products of Givens */
/*  rotations.  They may either be formed explicitly, or they may be */
/*  postmultiplied into input matrices Q1 and Z1, so that */

/*       Q1 * A * Z1**T = (Q1*Q) * H * (Z1*Z)**T */

/*       Q1 * B * Z1**T = (Q1*Q) * T * (Z1*Z)**T */

/*  If Q1 is the orthogonal matrix from the QR factorization of B in the */
/*  original equation A*x = lambda*B*x, then DGGHRD reduces the original */
/*  problem to generalized Hessenberg form. */

/*  Arguments */
/*  ========= */

/*  COMPQ   (input) CHARACTER*1 */
/*          = 'N': do not compute Q; */
/*          = 'I': Q is initialized to the unit matrix, and the */
/*                 orthogonal matrix Q is returned; */
/*          = 'V': Q must contain an orthogonal matrix Q1 on entry, */
/*                 and the product Q1*Q is returned. */

/*  COMPZ   (input) CHARACTER*1 */
/*          = 'N': do not compute Z; */
/*          = 'I': Z is initialized to the unit matrix, and the */
/*                 orthogonal matrix Z is returned; */
/*          = 'V': Z must contain an orthogonal matrix Z1 on entry, */
/*                 and the product Z1*Z is returned. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B.  N >= 0. */

/*  ILO     (input) INTEGER */
/*  IHI     (input) INTEGER */
/*          ILO and IHI mark the rows and columns of A which are to be */
/*          reduced.  It is assumed that A is already upper triangular */
/*          in rows and columns 1:ILO-1 and IHI+1:N.  ILO and IHI are */
/*          normally set by a previous call to SGGBAL; otherwise they */
/*          should be set to 1 and N respectively. */
/*          1 <= ILO <= IHI <= N, if N > 0; ILO=1 and IHI=0, if N=0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the N-by-N general matrix to be reduced. */
/*          On exit, the upper triangle and the first subdiagonal of A */
/*          are overwritten with the upper Hessenberg matrix H, and the */
/*          rest is set to zero. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB, N) */
/*          On entry, the N-by-N upper triangular matrix B. */
/*          On exit, the upper triangular matrix T = Q**T B Z.  The */
/*          elements below the diagonal are set to zero. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  Q       (input/output) DOUBLE PRECISION array, dimension (LDQ, N) */
/*          On entry, if COMPQ = 'V', the orthogonal matrix Q1, */
/*          typically from the QR factorization of B. */
/*          On exit, if COMPQ='I', the orthogonal matrix Q, and if */
/*          COMPQ = 'V', the product Q1*Q. */
/*          Not referenced if COMPQ='N'. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q. */
/*          LDQ >= N if COMPQ='V' or 'I'; LDQ >= 1 otherwise. */

/*  Z       (input/output) DOUBLE PRECISION array, dimension (LDZ, N) */
/*          On entry, if COMPZ = 'V', the orthogonal matrix Z1. */
/*          On exit, if COMPZ='I', the orthogonal matrix Z, and if */
/*          COMPZ = 'V', the product Z1*Z. */
/*          Not referenced if COMPZ='N'. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z. */
/*          LDZ >= N if COMPZ='V' or 'I'; LDZ >= 1 otherwise. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  This routine reduces A to Hessenberg and B to triangular form by */
/*  an unblocked reduction, as described in _Matrix_Computations_, */
/*  by Golub and Van Loan (Johns Hopkins Press.) */

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

/*     Decode COMPQ */

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

    /* Function Body */
    if (lsame_(compq, "N")) {
	ilq = false;
	icompq = 1;
    } else if (lsame_(compq, "V")) {
	ilq = true;
	icompq = 2;
    } else if (lsame_(compq, "I")) {
	ilq = true;
	icompq = 3;
    } else {
	icompq = 0;
    }

/*     Decode COMPZ */

    if (lsame_(compz, "N")) {
	ilz = false;
	icompz = 1;
    } else if (lsame_(compz, "V")) {
	ilz = true;
	icompz = 2;
    } else if (lsame_(compz, "I")) {
	ilz = true;
	icompz = 3;
    } else {
	icompz = 0;
    }

/*     Test the input parameters. */

    *info = 0;
    if (icompq <= 0) {
	*info = -1;
    } else if (icompz <= 0) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ilo < 1) {
	*info = -4;
    } else if (*ihi > *n || *ihi < *ilo - 1) {
	*info = -5;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -9;
    } else if (ilq && *ldq < *n || *ldq < 1) {
	*info = -11;
    } else if (ilz && *ldz < *n || *ldz < 1) {
	*info = -13;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGGHRD", &i__1);
	return 0;
    }

/*     Initialize Q and Z if desired. */

    if (icompq == 3) {
	dlaset_("Full", n, n, &c_b10, &c_b11, &q[q_offset], ldq);
    }
    if (icompz == 3) {
	dlaset_("Full", n, n, &c_b10, &c_b11, &z__[z_offset], ldz);
    }

/*     Quick return if possible */

    if (*n <= 1) {
	return 0;
    }

/*     Zero out lower triangle of B */

    i__1 = *n - 1;
    for (jcol = 1; jcol <= i__1; ++jcol) {
	i__2 = *n;
	for (jrow = jcol + 1; jrow <= i__2; ++jrow) {
	    b[jrow + jcol * b_dim1] = 0.;
/* L10: */
	}
/* L20: */
    }

/*     Reduce A and B */

    i__1 = *ihi - 2;
    for (jcol = *ilo; jcol <= i__1; ++jcol) {

	i__2 = jcol + 2;
	for (jrow = *ihi; jrow >= i__2; --jrow) {

/*           Step 1: rotate rows JROW-1, JROW to kill A(JROW,JCOL) */

	    temp = a[jrow - 1 + jcol * a_dim1];
	    dlartg_(&temp, &a[jrow + jcol * a_dim1], &c__, &s, &a[jrow - 1 +
		    jcol * a_dim1]);
	    a[jrow + jcol * a_dim1] = 0.;
	    i__3 = *n - jcol;
	    drot_(&i__3, &a[jrow - 1 + (jcol + 1) * a_dim1], lda, &a[jrow + (
		    jcol + 1) * a_dim1], lda, &c__, &s);
	    i__3 = *n + 2 - jrow;
	    drot_(&i__3, &b[jrow - 1 + (jrow - 1) * b_dim1], ldb, &b[jrow + (
		    jrow - 1) * b_dim1], ldb, &c__, &s);
	    if (ilq) {
		drot_(n, &q[(jrow - 1) * q_dim1 + 1], &c__1, &q[jrow * q_dim1
			+ 1], &c__1, &c__, &s);
	    }

/*           Step 2: rotate columns JROW, JROW-1 to kill B(JROW,JROW-1) */

	    temp = b[jrow + jrow * b_dim1];
	    dlartg_(&temp, &b[jrow + (jrow - 1) * b_dim1], &c__, &s, &b[jrow
		    + jrow * b_dim1]);
	    b[jrow + (jrow - 1) * b_dim1] = 0.;
	    drot_(ihi, &a[jrow * a_dim1 + 1], &c__1, &a[(jrow - 1) * a_dim1 +
		    1], &c__1, &c__, &s);
	    i__3 = jrow - 1;
	    drot_(&i__3, &b[jrow * b_dim1 + 1], &c__1, &b[(jrow - 1) * b_dim1
		    + 1], &c__1, &c__, &s);
	    if (ilz) {
		drot_(n, &z__[jrow * z_dim1 + 1], &c__1, &z__[(jrow - 1) *
			z_dim1 + 1], &c__1, &c__, &s);
	    }
/* L30: */
	}
/* L40: */
    }

    return 0;

/*     End of DGGHRD */

} /* dgghrd_ */

/* Subroutine */ int dgglse_(integer *m, integer *n, integer *p, double *
	a, integer *lda, double *b, integer *ldb, double *c__,
	double *d__, double *x, double *work, integer *lwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static double c_b31 = -1.;
	static double c_b33 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;

    /* Local variables */
    integer nb, mn, nr, nb1, nb2, nb3, nb4, lopt;
    integer lwkmin;
    integer lwkopt;
    bool lquery;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGGLSE solves the linear equality-constrained least squares (LSE) */
/*  problem: */

/*          minimize || c - A*x ||_2   subject to   B*x = d */

/*  where A is an M-by-N matrix, B is a P-by-N matrix, c is a given */
/*  M-vector, and d is a given P-vector. It is assumed that */
/*  P <= N <= M+P, and */

/*           rank(B) = P and  rank( (A) ) = N. */
/*                                ( (B) ) */

/*  These conditions ensure that the LSE problem has a unique solution, */
/*  which is obtained using a generalized RQ factorization of the */
/*  matrices (B, A) given by */

/*     B = (0 R)*Q,   A = Z*T*Q. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and B. N >= 0. */

/*  P       (input) INTEGER */
/*          The number of rows of the matrix B. 0 <= P <= N <= M+P. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, the elements on and above the diagonal of the array */
/*          contain the min(M,N)-by-N upper trapezoidal matrix T. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,M). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,N) */
/*          On entry, the P-by-N matrix B. */
/*          On exit, the upper triangle of the subarray B(1:P,N-P+1:N) */
/*          contains the P-by-P upper triangular matrix R. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,P). */

/*  C       (input/output) DOUBLE PRECISION array, dimension (M) */
/*          On entry, C contains the right hand side vector for the */
/*          least squares part of the LSE problem. */
/*          On exit, the residual sum of squares for the solution */
/*          is given by the sum of squares of elements N-P+1 to M of */
/*          vector C. */

/*  D       (input/output) DOUBLE PRECISION array, dimension (P) */
/*          On entry, D contains the right hand side vector for the */
/*          constrained equation. */
/*          On exit, D is destroyed. */

/*  X       (output) DOUBLE PRECISION array, dimension (N) */
/*          On exit, X is the solution of the LSE problem. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. LWORK >= max(1,M+N+P). */
/*          For optimum performance LWORK >= P+min(M,N)+max(M,N)*NB, */
/*          where NB is an upper bound for the optimal blocksizes for */
/*          DGEQRF, SGERQF, DORMQR and SORMRQ. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          = 1:  the upper triangular factor R associated with B in the */
/*                generalized RQ factorization of the pair (B, A) is */
/*                singular, so that rank(B) < P; the least squares */
/*                solution could not be computed. */
/*          = 2:  the (N-P) by (N-P) part of the upper trapezoidal factor */
/*                T associated with A in the generalized RQ factorization */
/*                of the pair (B, A) is singular, so that */
/*                rank( (A) ) < N; the least squares solution could not */
/*                    ( (B) ) */
/*                be computed. */

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

/*     Test the input parameters */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --c__;
    --d__;
    --x;
    --work;

    /* Function Body */
    *info = 0;
    mn = std::min(*m,*n);
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*p < 0 || *p > *n || *p < *n - *m) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,*p)) {
	*info = -7;
    }

/*     Calculate workspace */

    if (*info == 0) {
	if (*n == 0) {
	    lwkmin = 1;
	    lwkopt = 1;
	} else {
	    nb1 = ilaenv_(&c__1, "DGEQRF", " ", m, n, &c_n1, &c_n1);
	    nb2 = ilaenv_(&c__1, "DGERQF", " ", m, n, &c_n1, &c_n1);
	    nb3 = ilaenv_(&c__1, "DORMQR", " ", m, n, p, &c_n1);
	    nb4 = ilaenv_(&c__1, "DORMRQ", " ", m, n, p, &c_n1);
/* Computing MAX */
	    i__1 = std::max(nb1,nb2), i__1 = std::max(i__1,nb3);
	    nb = std::max(i__1,nb4);
	    lwkmin = *m + *n + *p;
	    lwkopt = *p + mn + std::max(*m,*n) * nb;
	}
	work[1] = (double) lwkopt;

	if (*lwork < lwkmin && ! lquery) {
	    *info = -12;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGGLSE", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Compute the GRQ factorization of matrices B and A: */

/*            B*Q' = (  0  T12 ) P   Z'*A*Q' = ( R11 R12 ) N-P */
/*                     N-P  P                  (  0  R22 ) M+P-N */
/*                                               N-P  P */

/*     where T12 and R11 are upper triangular, and Q and Z are */
/*     orthogonal. */

    i__1 = *lwork - *p - mn;
    dggrqf_(p, m, n, &b[b_offset], ldb, &work[1], &a[a_offset], lda, &work[*p
	    + 1], &work[*p + mn + 1], &i__1, info);
    lopt = (integer) work[*p + mn + 1];

/*     Update c = Z'*c = ( c1 ) N-P */
/*                       ( c2 ) M+P-N */

    i__1 = std::max(1_integer,*m);
    i__2 = *lwork - *p - mn;
    dormqr_("Left", "Transpose", m, &c__1, &mn, &a[a_offset], lda, &work[*p +
	    1], &c__[1], &i__1, &work[*p + mn + 1], &i__2, info);
/* Computing MAX */
    i__1 = lopt, i__2 = (integer) work[*p + mn + 1];
    lopt = std::max(i__1,i__2);

/*     Solve T12*x2 = d for x2 */

    if (*p > 0) {
	dtrtrs_("Upper", "No transpose", "Non-unit", p, &c__1, &b[(*n - *p +
		1) * b_dim1 + 1], ldb, &d__[1], p, info);

	if (*info > 0) {
	    *info = 1;
	    return 0;
	}

/*        Put the solution in X */

	dcopy_(p, &d__[1], &c__1, &x[*n - *p + 1], &c__1);

/*        Update c1 */

	i__1 = *n - *p;
	dgemv_("No transpose", &i__1, p, &c_b31, &a[(*n - *p + 1) * a_dim1 +
		1], lda, &d__[1], &c__1, &c_b33, &c__[1], &c__1);
    }

/*     Solve R11*x1 = c1 for x1 */

    if (*n > *p) {
	i__1 = *n - *p;
	i__2 = *n - *p;
	dtrtrs_("Upper", "No transpose", "Non-unit", &i__1, &c__1, &a[
		a_offset], lda, &c__[1], &i__2, info);

	if (*info > 0) {
	    *info = 2;
	    return 0;
	}

/*        Put the solutions in X */

	i__1 = *n - *p;
	dcopy_(&i__1, &c__[1], &c__1, &x[1], &c__1);
    }

/*     Compute the residual vector: */

    if (*m < *n) {
	nr = *m + *p - *n;
	if (nr > 0) {
	    i__1 = *n - *m;
	    dgemv_("No transpose", &nr, &i__1, &c_b31, &a[*n - *p + 1 + (*m +
		    1) * a_dim1], lda, &d__[nr + 1], &c__1, &c_b33, &c__[*n -
		    *p + 1], &c__1);
	}
    } else {
	nr = *p;
    }
    if (nr > 0) {
	dtrmv_("Upper", "No transpose", "Non unit", &nr, &a[*n - *p + 1 + (*n
		- *p + 1) * a_dim1], lda, &d__[1], &c__1);
	daxpy_(&nr, &c_b31, &d__[1], &c__1, &c__[*n - *p + 1], &c__1);
    }

/*     Backward transformation x = Q'*x */

    i__1 = *lwork - *p - mn;
    dormrq_("Left", "Transpose", n, &c__1, p, &b[b_offset], ldb, &work[1], &x[
	    1], n, &work[*p + mn + 1], &i__1, info);
/* Computing MAX */
    i__1 = lopt, i__2 = (integer) work[*p + mn + 1];
    work[1] = (double) (*p + mn + std::max(i__1,i__2));

    return 0;

/*     End of DGGLSE */

} /* dgglse_ */

/* Subroutine */ int dggqrf_(integer *n, integer *m, integer *p, double *
	a, integer *lda, double *taua, double *b, integer *ldb,
	double *taub, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;

    /* Local variables */
    integer nb, nb1, nb2, nb3, lopt;
    integer lwkopt;
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

/*  DGGQRF computes a generalized QR factorization of an N-by-M matrix A */
/*  and an N-by-P matrix B: */

/*              A = Q*R,        B = Q*T*Z, */

/*  where Q is an N-by-N orthogonal matrix, Z is a P-by-P orthogonal */
/*  matrix, and R and T assume one of the forms: */

/*  if N >= M,  R = ( R11 ) M  ,   or if N < M,  R = ( R11  R12 ) N, */
/*                  (  0  ) N-M                         N   M-N */
/*                     M */

/*  where R11 is upper triangular, and */

/*  if N <= P,  T = ( 0  T12 ) N,   or if N > P,  T = ( T11 ) N-P, */
/*                   P-N  N                           ( T21 ) P */
/*                                                       P */

/*  where T12 or T21 is upper triangular. */

/*  In particular, if B is square and nonsingular, the GQR factorization */
/*  of A and B implicitly gives the QR factorization of inv(B)*A: */

/*               inv(B)*A = Z'*(inv(T)*R) */

/*  where inv(B) denotes the inverse of the matrix B, and Z' denotes the */
/*  transpose of the matrix Z. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of rows of the matrices A and B. N >= 0. */

/*  M       (input) INTEGER */
/*          The number of columns of the matrix A.  M >= 0. */

/*  P       (input) INTEGER */
/*          The number of columns of the matrix B.  P >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,M) */
/*          On entry, the N-by-M matrix A. */
/*          On exit, the elements on and above the diagonal of the array */
/*          contain the min(N,M)-by-M upper trapezoidal matrix R (R is */
/*          upper triangular if N >= M); the elements below the diagonal, */
/*          with the array TAUA, represent the orthogonal matrix Q as a */
/*          product of min(N,M) elementary reflectors (see Further */
/*          Details). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,N). */

/*  TAUA    (output) DOUBLE PRECISION array, dimension (min(N,M)) */
/*          The scalar factors of the elementary reflectors which */
/*          represent the orthogonal matrix Q (see Further Details). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,P) */
/*          On entry, the N-by-P matrix B. */
/*          On exit, if N <= P, the upper triangle of the subarray */
/*          B(1:N,P-N+1:P) contains the N-by-N upper triangular matrix T; */
/*          if N > P, the elements on and above the (N-P)-th subdiagonal */
/*          contain the N-by-P upper trapezoidal matrix T; the remaining */
/*          elements, with the array TAUB, represent the orthogonal */
/*          matrix Z as a product of elementary reflectors (see Further */
/*          Details). */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,N). */

/*  TAUB    (output) DOUBLE PRECISION array, dimension (min(N,P)) */
/*          The scalar factors of the elementary reflectors which */
/*          represent the orthogonal matrix Z (see Further Details). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. LWORK >= max(1,N,M,P). */
/*          For optimum performance LWORK >= max(N,M,P)*max(NB1,NB2,NB3), */
/*          where NB1 is the optimal blocksize for the QR factorization */
/*          of an N-by-M matrix, NB2 is the optimal blocksize for the */
/*          RQ factorization of an N-by-P matrix, and NB3 is the optimal */
/*          blocksize for a call of DORMQR. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of elementary reflectors */

/*     Q = H(1) H(2) . . . H(k), where k = min(n,m). */

/*  Each H(i) has the form */

/*     H(i) = I - taua * v * v' */

/*  where taua is a real scalar, and v is a real vector with */
/*  v(1:i-1) = 0 and v(i) = 1; v(i+1:n) is stored on exit in A(i+1:n,i), */
/*  and taua in TAUA(i). */
/*  To form Q explicitly, use LAPACK subroutine DORGQR. */
/*  To use Q to update another matrix, use LAPACK subroutine DORMQR. */

/*  The matrix Z is represented as a product of elementary reflectors */

/*     Z = H(1) H(2) . . . H(k), where k = min(n,p). */

/*  Each H(i) has the form */

/*     H(i) = I - taub * v * v' */

/*  where taub is a real scalar, and v is a real vector with */
/*  v(p-k+i+1:p) = 0 and v(p-k+i) = 1; v(1:p-k+i-1) is stored on exit in */
/*  B(n-k+i,1:p-k+i-1), and taub in TAUB(i). */
/*  To form Z explicitly, use LAPACK subroutine DORGRQ. */
/*  To use Z to update another matrix, use LAPACK subroutine DORMRQ. */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --taua;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --taub;
    --work;

    /* Function Body */
    *info = 0;
    nb1 = ilaenv_(&c__1, "DGEQRF", " ", n, m, &c_n1, &c_n1);
    nb2 = ilaenv_(&c__1, "DGERQF", " ", n, p, &c_n1, &c_n1);
    nb3 = ilaenv_(&c__1, "DORMQR", " ", n, m, p, &c_n1);
/* Computing MAX */
    i__1 = std::max(nb1,nb2);
    nb = std::max(i__1,nb3);
/* Computing MAX */
    i__1 = std::max(*n,*m);
    lwkopt = std::max(i__1,*p) * nb;
    work[1] = (double) lwkopt;
    lquery = *lwork == -1;
    if (*n < 0) {
	*info = -1;
    } else if (*m < 0) {
	*info = -2;
    } else if (*p < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -8;
    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = std::max(1_integer,*n), i__1 = std::max(i__1,*m);
	if (*lwork < std::max(i__1,*p) && ! lquery) {
	    *info = -11;
	}
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGGQRF", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     QR factorization of N-by-M matrix A: A = Q*R */

    dgeqrf_(n, m, &a[a_offset], lda, &taua[1], &work[1], lwork, info);
    lopt = (integer) work[1];

/*     Update B := Q'*B. */

    i__1 = std::min(*n,*m);
    dormqr_("Left", "Transpose", n, p, &i__1, &a[a_offset], lda, &taua[1], &b[
	    b_offset], ldb, &work[1], lwork, info);
/* Computing MAX */
    i__1 = lopt, i__2 = (integer) work[1];
    lopt = std::max(i__1,i__2);

/*     RQ factorization of N-by-P matrix B: B = T*Z. */

    dgerqf_(n, p, &b[b_offset], ldb, &taub[1], &work[1], lwork, info);
/* Computing MAX */
    i__1 = lopt, i__2 = (integer) work[1];
    work[1] = (double) std::max(i__1,i__2);

    return 0;

/*     End of DGGQRF */

} /* dggqrf_ */

/* Subroutine */ int dggrqf_(integer *m, integer *p, integer *n, double *
	a, integer *lda, double *taua, double *b, integer *ldb,
	double *taub, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2, i__3;

    /* Local variables */
    integer nb, nb1, nb2, nb3, lopt;
    integer lwkopt;
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

/*  DGGRQF computes a generalized RQ factorization of an M-by-N matrix A */
/*  and a P-by-N matrix B: */

/*              A = R*Q,        B = Z*T*Q, */

/*  where Q is an N-by-N orthogonal matrix, Z is a P-by-P orthogonal */
/*  matrix, and R and T assume one of the forms: */

/*  if M <= N,  R = ( 0  R12 ) M,   or if M > N,  R = ( R11 ) M-N, */
/*                   N-M  M                           ( R21 ) N */
/*                                                       N */

/*  where R12 or R21 is upper triangular, and */

/*  if P >= N,  T = ( T11 ) N  ,   or if P < N,  T = ( T11  T12 ) P, */
/*                  (  0  ) P-N                         P   N-P */
/*                     N */

/*  where T11 is upper triangular. */

/*  In particular, if B is square and nonsingular, the GRQ factorization */
/*  of A and B implicitly gives the RQ factorization of A*inv(B): */

/*               A*inv(B) = (R*inv(T))*Z' */

/*  where inv(B) denotes the inverse of the matrix B, and Z' denotes the */
/*  transpose of the matrix Z. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  P       (input) INTEGER */
/*          The number of rows of the matrix B.  P >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and B. N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, if M <= N, the upper triangle of the subarray */
/*          A(1:M,N-M+1:N) contains the M-by-M upper triangular matrix R; */
/*          if M > N, the elements on and above the (M-N)-th subdiagonal */
/*          contain the M-by-N upper trapezoidal matrix R; the remaining */
/*          elements, with the array TAUA, represent the orthogonal */
/*          matrix Q as a product of elementary reflectors (see Further */
/*          Details). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,M). */

/*  TAUA    (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The scalar factors of the elementary reflectors which */
/*          represent the orthogonal matrix Q (see Further Details). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,N) */
/*          On entry, the P-by-N matrix B. */
/*          On exit, the elements on and above the diagonal of the array */
/*          contain the min(P,N)-by-N upper trapezoidal matrix T (T is */
/*          upper triangular if P >= N); the elements below the diagonal, */
/*          with the array TAUB, represent the orthogonal matrix Z as a */
/*          product of elementary reflectors (see Further Details). */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,P). */

/*  TAUB    (output) DOUBLE PRECISION array, dimension (min(P,N)) */
/*          The scalar factors of the elementary reflectors which */
/*          represent the orthogonal matrix Z (see Further Details). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. LWORK >= max(1,N,M,P). */
/*          For optimum performance LWORK >= max(N,M,P)*max(NB1,NB2,NB3), */
/*          where NB1 is the optimal blocksize for the RQ factorization */
/*          of an M-by-N matrix, NB2 is the optimal blocksize for the */
/*          QR factorization of a P-by-N matrix, and NB3 is the optimal */
/*          blocksize for a call of DORMRQ. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INF0= -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of elementary reflectors */

/*     Q = H(1) H(2) . . . H(k), where k = min(m,n). */

/*  Each H(i) has the form */

/*     H(i) = I - taua * v * v' */

/*  where taua is a real scalar, and v is a real vector with */
/*  v(n-k+i+1:n) = 0 and v(n-k+i) = 1; v(1:n-k+i-1) is stored on exit in */
/*  A(m-k+i,1:n-k+i-1), and taua in TAUA(i). */
/*  To form Q explicitly, use LAPACK subroutine DORGRQ. */
/*  To use Q to update another matrix, use LAPACK subroutine DORMRQ. */

/*  The matrix Z is represented as a product of elementary reflectors */

/*     Z = H(1) H(2) . . . H(k), where k = min(p,n). */

/*  Each H(i) has the form */

/*     H(i) = I - taub * v * v' */

/*  where taub is a real scalar, and v is a real vector with */
/*  v(1:i-1) = 0 and v(i) = 1; v(i+1:p) is stored on exit in B(i+1:p,i), */
/*  and taub in TAUB(i). */
/*  To form Z explicitly, use LAPACK subroutine DORGQR. */
/*  To use Z to update another matrix, use LAPACK subroutine DORMQR. */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --taua;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --taub;
    --work;

    /* Function Body */
    *info = 0;
    nb1 = ilaenv_(&c__1, "DGERQF", " ", m, n, &c_n1, &c_n1);
    nb2 = ilaenv_(&c__1, "DGEQRF", " ", p, n, &c_n1, &c_n1);
    nb3 = ilaenv_(&c__1, "DORMRQ", " ", m, n, p, &c_n1);
/* Computing MAX */
    i__1 = std::max(nb1,nb2);
    nb = std::max(i__1,nb3);
/* Computing MAX */
    i__1 = std::max(*n,*m);
    lwkopt = std::max(i__1,*p) * nb;
    work[1] = (double) lwkopt;
    lquery = *lwork == -1;
    if (*m < 0) {
	*info = -1;
    } else if (*p < 0) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,*p)) {
	*info = -8;
    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = std::max(1_integer,*m), i__1 = std::max(i__1,*p);
	if (*lwork < std::max(i__1,*n) && ! lquery) {
	    *info = -11;
	}
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGGRQF", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     RQ factorization of M-by-N matrix A: A = R*Q */

    dgerqf_(m, n, &a[a_offset], lda, &taua[1], &work[1], lwork, info);
    lopt = (integer) work[1];

/*     Update B := B*Q' */

    i__1 = std::min(*m,*n);
/* Computing MAX */
    i__2 = 1, i__3 = *m - *n + 1;
    dormrq_("Right", "Transpose", p, n, &i__1, &a[std::max(i__2, i__3)+ a_dim1],
	    lda, &taua[1], &b[b_offset], ldb, &work[1], lwork, info);
/* Computing MAX */
    i__1 = lopt, i__2 = (integer) work[1];
    lopt = std::max(i__1,i__2);

/*     QR factorization of P-by-N matrix B: B = Z*T */

    dgeqrf_(p, n, &b[b_offset], ldb, &taub[1], &work[1], lwork, info);
/* Computing MAX */
    i__1 = lopt, i__2 = (integer) work[1];
    work[1] = (double) std::max(i__1,i__2);

    return 0;

/*     End of DGGRQF */

} /* dggrqf_ */

/* Subroutine */ int dggsvd_(const char *jobu, const char *jobv, const char *jobq, integer *m,
	integer *n, integer *p, integer *k, integer *l, double *a,
	integer *lda, double *b, integer *ldb, double *alpha,
	double *beta, double *u, integer *ldu, double *v, integer
	*ldv, double *q, integer *ldq, double *work, integer *iwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, q_dim1, q_offset, u_dim1,
	    u_offset, v_dim1, v_offset, i__1, i__2;

    /* Local variables */
    integer i__, j;
    double ulp;
    integer ibnd;
    double tola;
    integer isub;
    double tolb, unfl, temp, smax;
    double anorm, bnorm;
    bool wantq, wantu, wantv;
    integer ncycle;

/*  -- LAPACK driver routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGGSVD computes the generalized singular value decomposition (GSVD) */
/*  of an M-by-N real matrix A and P-by-N real matrix B: */

/*      U'*A*Q = D1*( 0 R ),    V'*B*Q = D2*( 0 R ) */

/*  where U, V and Q are orthogonal matrices, and Z' is the transpose */
/*  of Z.  Let K+L = the effective numerical rank of the matrix (A',B')', */
/*  then R is a K+L-by-K+L nonsingular upper triangular matrix, D1 and */
/*  D2 are M-by-(K+L) and P-by-(K+L) "diagonal" matrices and of the */
/*  following structures, respectively: */

/*  If M-K-L >= 0, */

/*                      K  L */
/*         D1 =     K ( I  0 ) */
/*                  L ( 0  C ) */
/*              M-K-L ( 0  0 ) */

/*                    K  L */
/*         D2 =   L ( 0  S ) */
/*              P-L ( 0  0 ) */

/*                  N-K-L  K    L */
/*    ( 0 R ) = K (  0   R11  R12 ) */
/*              L (  0    0   R22 ) */

/*  where */

/*    C = diag( ALPHA(K+1), ... , ALPHA(K+L) ), */
/*    S = diag( BETA(K+1),  ... , BETA(K+L) ), */
/*    C**2 + S**2 = I. */

/*    R is stored in A(1:K+L,N-K-L+1:N) on exit. */

/*  If M-K-L < 0, */

/*                    K M-K K+L-M */
/*         D1 =   K ( I  0    0   ) */
/*              M-K ( 0  C    0   ) */

/*                      K M-K K+L-M */
/*         D2 =   M-K ( 0  S    0  ) */
/*              K+L-M ( 0  0    I  ) */
/*                P-L ( 0  0    0  ) */

/*                     N-K-L  K   M-K  K+L-M */
/*    ( 0 R ) =     K ( 0    R11  R12  R13  ) */
/*                M-K ( 0     0   R22  R23  ) */
/*              K+L-M ( 0     0    0   R33  ) */

/*  where */

/*    C = diag( ALPHA(K+1), ... , ALPHA(M) ), */
/*    S = diag( BETA(K+1),  ... , BETA(M) ), */
/*    C**2 + S**2 = I. */

/*    (R11 R12 R13 ) is stored in A(1:M, N-K-L+1:N), and R33 is stored */
/*    ( 0  R22 R23 ) */
/*    in B(M-K+1:L,N+M-K-L+1:N) on exit. */

/*  The routine computes C, S, R, and optionally the orthogonal */
/*  transformation matrices U, V and Q. */

/*  In particular, if B is an N-by-N nonsingular matrix, then the GSVD of */
/*  A and B implicitly gives the SVD of A*inv(B): */
/*                       A*inv(B) = U*(D1*inv(D2))*V'. */
/*  If ( A',B')' has orthonormal columns, then the GSVD of A and B is */
/*  also equal to the CS decomposition of A and B. Furthermore, the GSVD */
/*  can be used to derive the solution of the eigenvalue problem: */
/*                       A'*A x = lambda* B'*B x. */
/*  In some literature, the GSVD of A and B is presented in the form */
/*                   U'*A*X = ( 0 D1 ),   V'*B*X = ( 0 D2 ) */
/*  where U and V are orthogonal and X is nonsingular, D1 and D2 are */
/*  ``diagonal''.  The former GSVD form can be converted to the latter */
/*  form by taking the nonsingular matrix X as */

/*                       X = Q*( I   0    ) */
/*                             ( 0 inv(R) ). */

/*  Arguments */
/*  ========= */

/*  JOBU    (input) CHARACTER*1 */
/*          = 'U':  Orthogonal matrix U is computed; */
/*          = 'N':  U is not computed. */

/*  JOBV    (input) CHARACTER*1 */
/*          = 'V':  Orthogonal matrix V is computed; */
/*          = 'N':  V is not computed. */

/*  JOBQ    (input) CHARACTER*1 */
/*          = 'Q':  Orthogonal matrix Q is computed; */
/*          = 'N':  Q is not computed. */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and B.  N >= 0. */

/*  P       (input) INTEGER */
/*          The number of rows of the matrix B.  P >= 0. */

/*  K       (output) INTEGER */
/*  L       (output) INTEGER */
/*          On exit, K and L specify the dimension of the subblocks */
/*          described in the Purpose section. */
/*          K + L = effective numerical rank of (A',B')'. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, A contains the triangular matrix R, or part of R. */
/*          See Purpose for details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,M). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,N) */
/*          On entry, the P-by-N matrix B. */
/*          On exit, B contains the triangular matrix R if M-K-L < 0. */
/*          See Purpose for details. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,P). */

/*  ALPHA   (output) DOUBLE PRECISION array, dimension (N) */
/*  BETA    (output) DOUBLE PRECISION array, dimension (N) */
/*          On exit, ALPHA and BETA contain the generalized singular */
/*          value pairs of A and B; */
/*            ALPHA(1:K) = 1, */
/*            BETA(1:K)  = 0, */
/*          and if M-K-L >= 0, */
/*            ALPHA(K+1:K+L) = C, */
/*            BETA(K+1:K+L)  = S, */
/*          or if M-K-L < 0, */
/*            ALPHA(K+1:M)=C, ALPHA(M+1:K+L)=0 */
/*            BETA(K+1:M) =S, BETA(M+1:K+L) =1 */
/*          and */
/*            ALPHA(K+L+1:N) = 0 */
/*            BETA(K+L+1:N)  = 0 */

/*  U       (output) DOUBLE PRECISION array, dimension (LDU,M) */
/*          If JOBU = 'U', U contains the M-by-M orthogonal matrix U. */
/*          If JOBU = 'N', U is not referenced. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U. LDU >= max(1,M) if */
/*          JOBU = 'U'; LDU >= 1 otherwise. */

/*  V       (output) DOUBLE PRECISION array, dimension (LDV,P) */
/*          If JOBV = 'V', V contains the P-by-P orthogonal matrix V. */
/*          If JOBV = 'N', V is not referenced. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of the array V. LDV >= max(1,P) if */
/*          JOBV = 'V'; LDV >= 1 otherwise. */

/*  Q       (output) DOUBLE PRECISION array, dimension (LDQ,N) */
/*          If JOBQ = 'Q', Q contains the N-by-N orthogonal matrix Q. */
/*          If JOBQ = 'N', Q is not referenced. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q. LDQ >= max(1,N) if */
/*          JOBQ = 'Q'; LDQ >= 1 otherwise. */

/*  WORK    (workspace) DOUBLE PRECISION array, */
/*                      dimension (max(3*N,M,P)+N) */

/*  IWORK   (workspace/output) INTEGER array, dimension (N) */
/*          On exit, IWORK stores the sorting information. More */
/*          precisely, the following loop will sort ALPHA */
/*             for I = K+1, min(M,K+L) */
/*                 swap ALPHA(I) and ALPHA(IWORK(I)) */
/*             endfor */
/*          such that ALPHA(1) >= ALPHA(2) >= ... >= ALPHA(N). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = 1, the Jacobi-type procedure failed to */
/*                converge.  For further details, see subroutine DTGSJA. */

/*  Internal Parameters */
/*  =================== */

/*  TOLA    DOUBLE PRECISION */
/*  TOLB    DOUBLE PRECISION */
/*          TOLA and TOLB are the thresholds to determine the effective */
/*          rank of (A',B')'. Generally, they are set to */
/*                   TOLA = MAX(M,N)*norm(A)*MAZHEPS, */
/*                   TOLB = MAX(P,N)*norm(B)*MAZHEPS. */
/*          The size of TOLA and TOLB may affect the size of backward */
/*          errors of the decomposition. */

/*  Further Details */
/*  =============== */

/*  2-96 Based on modifications by */
/*     Ming Gu and Huan Ren, Computer Science Division, University of */
/*     California at Berkeley, USA */

/*  ===================================================================== */

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
    --iwork;

    /* Function Body */
    wantu = lsame_(jobu, "U");
    wantv = lsame_(jobv, "V");
    wantq = lsame_(jobq, "Q");

    *info = 0;
    if (! (wantu || lsame_(jobu, "N"))) {
	*info = -1;
    } else if (! (wantv || lsame_(jobv, "N"))) {
	*info = -2;
    } else if (! (wantq || lsame_(jobq, "N"))) {
	*info = -3;
    } else if (*m < 0) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    } else if (*p < 0) {
	*info = -6;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -10;
    } else if (*ldb < std::max(1_integer,*p)) {
	*info = -12;
    } else if (*ldu < 1 || wantu && *ldu < *m) {
	*info = -16;
    } else if (*ldv < 1 || wantv && *ldv < *p) {
	*info = -18;
    } else if (*ldq < 1 || wantq && *ldq < *n) {
	*info = -20;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGGSVD", &i__1);
	return 0;
    }

/*     Compute the Frobenius norm of matrices A and B */

    anorm = dlange_("1", m, n, &a[a_offset], lda, &work[1]);
    bnorm = dlange_("1", p, n, &b[b_offset], ldb, &work[1]);

/*     Get machine precision and set up threshold for determining */
/*     the effective numerical rank of the matrices A and B. */

    ulp = dlamch_("Precision");
    unfl = dlamch_("Safe Minimum");
    tola = std::max(*m,*n) * std::max(anorm,unfl) * ulp;
    tolb = std::max(*p,*n) * std::max(bnorm,unfl) * ulp;

/*     Preprocessing */

    dggsvp_(jobu, jobv, jobq, m, p, n, &a[a_offset], lda, &b[b_offset], ldb, &
	    tola, &tolb, k, l, &u[u_offset], ldu, &v[v_offset], ldv, &q[
	    q_offset], ldq, &iwork[1], &work[1], &work[*n + 1], info);

/*     Compute the GSVD of two upper "triangular" matrices */

    dtgsja_(jobu, jobv, jobq, m, p, n, k, l, &a[a_offset], lda, &b[b_offset],
	    ldb, &tola, &tolb, &alpha[1], &beta[1], &u[u_offset], ldu, &v[
	    v_offset], ldv, &q[q_offset], ldq, &work[1], &ncycle, info);

/*     Sort the singular values and store the pivot indices in IWORK */
/*     Copy ALPHA to WORK, then sort ALPHA in WORK */

    dcopy_(n, &alpha[1], &c__1, &work[1], &c__1);
/* Computing MIN */
    i__1 = *l, i__2 = *m - *k;
    ibnd = std::min(i__1,i__2);
    i__1 = ibnd;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*        Scan for largest ALPHA(K+I) */

	isub = i__;
	smax = work[*k + i__];
	i__2 = ibnd;
	for (j = i__ + 1; j <= i__2; ++j) {
	    temp = work[*k + j];
	    if (temp > smax) {
		isub = j;
		smax = temp;
	    }
/* L10: */
	}
	if (isub != i__) {
	    work[*k + isub] = work[*k + i__];
	    work[*k + i__] = smax;
	    iwork[*k + i__] = *k + isub;
	} else {
	    iwork[*k + i__] = *k + i__;
	}
/* L20: */
    }

    return 0;

/*     End of DGGSVD */

} /* dggsvd_ */

/* Subroutine */ int dggsvp_(const char *jobu, const char *jobv, const char *jobq, integer *m,
	integer *p, integer *n, double *a, integer *lda, double *b,
	integer *ldb, double *tola, double *tolb, integer *k, integer
	*l, double *u, integer *ldu, double *v, integer *ldv,
	double *q, integer *ldq, integer *iwork, double *tau,
	double *work, integer *info)
{
	/* Table of constant values */
	static double c_b12 = 0.;
	static double c_b22 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, q_dim1, q_offset, u_dim1,
	    u_offset, v_dim1, v_offset, i__1, i__2, i__3;
    double d__1;

    /* Local variables */
    integer i__, j;
    bool wantq, wantu, wantv;
	bool forwrd;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGGSVP computes orthogonal matrices U, V and Q such that */

/*                   N-K-L  K    L */
/*   U'*A*Q =     K ( 0    A12  A13 )  if M-K-L >= 0; */
/*                L ( 0     0   A23 ) */
/*            M-K-L ( 0     0    0  ) */

/*                   N-K-L  K    L */
/*          =     K ( 0    A12  A13 )  if M-K-L < 0; */
/*              M-K ( 0     0   A23 ) */

/*                 N-K-L  K    L */
/*   V'*B*Q =   L ( 0     0   B13 ) */
/*            P-L ( 0     0    0  ) */

/*  where the K-by-K matrix A12 and L-by-L matrix B13 are nonsingular */
/*  upper triangular; A23 is L-by-L upper triangular if M-K-L >= 0, */
/*  otherwise A23 is (M-K)-by-L upper trapezoidal.  K+L = the effective */
/*  numerical rank of the (M+P)-by-N matrix (A',B')'.  Z' denotes the */
/*  transpose of Z. */

/*  This decomposition is the preprocessing step for computing the */
/*  Generalized Singular Value Decomposition (GSVD), see subroutine */
/*  DGGSVD. */

/*  Arguments */
/*  ========= */

/*  JOBU    (input) CHARACTER*1 */
/*          = 'U':  Orthogonal matrix U is computed; */
/*          = 'N':  U is not computed. */

/*  JOBV    (input) CHARACTER*1 */
/*          = 'V':  Orthogonal matrix V is computed; */
/*          = 'N':  V is not computed. */

/*  JOBQ    (input) CHARACTER*1 */
/*          = 'Q':  Orthogonal matrix Q is computed; */
/*          = 'N':  Q is not computed. */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  P       (input) INTEGER */
/*          The number of rows of the matrix B.  P >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and B.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, A contains the triangular (or trapezoidal) matrix */
/*          described in the Purpose section. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,M). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,N) */
/*          On entry, the P-by-N matrix B. */
/*          On exit, B contains the triangular matrix described in */
/*          the Purpose section. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,P). */

/*  TOLA    (input) DOUBLE PRECISION */
/*  TOLB    (input) DOUBLE PRECISION */
/*          TOLA and TOLB are the thresholds to determine the effective */
/*          numerical rank of matrix B and a subblock of A. Generally, */
/*          they are set to */
/*             TOLA = MAX(M,N)*norm(A)*MAZHEPS, */
/*             TOLB = MAX(P,N)*norm(B)*MAZHEPS. */
/*          The size of TOLA and TOLB may affect the size of backward */
/*          errors of the decomposition. */

/*  K       (output) INTEGER */
/*  L       (output) INTEGER */
/*          On exit, K and L specify the dimension of the subblocks */
/*          described in Purpose. */
/*          K + L = effective numerical rank of (A',B')'. */

/*  U       (output) DOUBLE PRECISION array, dimension (LDU,M) */
/*          If JOBU = 'U', U contains the orthogonal matrix U. */
/*          If JOBU = 'N', U is not referenced. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U. LDU >= max(1,M) if */
/*          JOBU = 'U'; LDU >= 1 otherwise. */

/*  V       (output) DOUBLE PRECISION array, dimension (LDV,M) */
/*          If JOBV = 'V', V contains the orthogonal matrix V. */
/*          If JOBV = 'N', V is not referenced. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of the array V. LDV >= max(1,P) if */
/*          JOBV = 'V'; LDV >= 1 otherwise. */

/*  Q       (output) DOUBLE PRECISION array, dimension (LDQ,N) */
/*          If JOBQ = 'Q', Q contains the orthogonal matrix Q. */
/*          If JOBQ = 'N', Q is not referenced. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q. LDQ >= max(1,N) if */
/*          JOBQ = 'Q'; LDQ >= 1 otherwise. */

/*  IWORK   (workspace) INTEGER array, dimension (N) */

/*  TAU     (workspace) DOUBLE PRECISION array, dimension (N) */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (max(3*N,M,P)) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */


/*  Further Details */
/*  =============== */

/*  The subroutine uses LAPACK subroutine DGEQPF for the QR factorization */
/*  with column pivoting to detect the effective numerical rank of the */
/*  a matrix. It may be replaced by a better rank determination strategy. */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --iwork;
    --tau;
    --work;

    /* Function Body */
    wantu = lsame_(jobu, "U");
    wantv = lsame_(jobv, "V");
    wantq = lsame_(jobq, "Q");
    forwrd = true;

    *info = 0;
    if (! (wantu || lsame_(jobu, "N"))) {
	*info = -1;
    } else if (! (wantv || lsame_(jobv, "N"))) {
	*info = -2;
    } else if (! (wantq || lsame_(jobq, "N"))) {
	*info = -3;
    } else if (*m < 0) {
	*info = -4;
    } else if (*p < 0) {
	*info = -5;
    } else if (*n < 0) {
	*info = -6;
    } else if (*lda < std::max(1_integer,*m)) {
	*info = -8;
    } else if (*ldb < std::max(1_integer,*p)) {
	*info = -10;
    } else if (*ldu < 1 || wantu && *ldu < *m) {
	*info = -16;
    } else if (*ldv < 1 || wantv && *ldv < *p) {
	*info = -18;
    } else if (*ldq < 1 || wantq && *ldq < *n) {
	*info = -20;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGGSVP", &i__1);
	return 0;
    }

/*     QR with column pivoting of B: B*P = V*( S11 S12 ) */
/*                                           (  0   0  ) */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	iwork[i__] = 0;
/* L10: */
    }
    dgeqpf_(p, n, &b[b_offset], ldb, &iwork[1], &tau[1], &work[1], info);

/*     Update A := A*P */

    dlapmt_(&forwrd, m, n, &a[a_offset], lda, &iwork[1]);

/*     Determine the effective rank of matrix B. */

    *l = 0;
    i__1 = std::min(*p,*n);
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = b[i__ + i__ * b_dim1], abs(d__1)) > *tolb) {
	    ++(*l);
	}
/* L20: */
    }

    if (wantv) {

/*        Copy the details of V, and form V. */

	dlaset_("Full", p, p, &c_b12, &c_b12, &v[v_offset], ldv);
	if (*p > 1) {
	    i__1 = *p - 1;
	    dlacpy_("Lower", &i__1, n, &b[b_dim1 + 2], ldb, &v[v_dim1 + 2],
		    ldv);
	}
	i__1 = std::min(*p,*n);
	dorg2r_(p, p, &i__1, &v[v_offset], ldv, &tau[1], &work[1], info);
    }

/*     Clean up B */

    i__1 = *l - 1;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *l;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    b[i__ + j * b_dim1] = 0.;
/* L30: */
	}
/* L40: */
    }
    if (*p > *l) {
	i__1 = *p - *l;
	dlaset_("Full", &i__1, n, &c_b12, &c_b12, &b[*l + 1 + b_dim1], ldb);
    }

    if (wantq) {

/*        Set Q = I and Update Q := Q*P */

	dlaset_("Full", n, n, &c_b12, &c_b22, &q[q_offset], ldq);
	dlapmt_(&forwrd, n, n, &q[q_offset], ldq, &iwork[1]);
    }

    if (*p >= *l && *n != *l) {

/*        RQ factorization of (S11 S12): ( S11 S12 ) = ( 0 S12 )*Z */

	dgerq2_(l, n, &b[b_offset], ldb, &tau[1], &work[1], info);

/*        Update A := A*Z' */

	dormr2_("Right", "Transpose", m, n, l, &b[b_offset], ldb, &tau[1], &a[
		a_offset], lda, &work[1], info);

	if (wantq) {

/*           Update Q := Q*Z' */

	    dormr2_("Right", "Transpose", n, n, l, &b[b_offset], ldb, &tau[1],
		     &q[q_offset], ldq, &work[1], info);
	}

/*        Clean up B */

	i__1 = *n - *l;
	dlaset_("Full", l, &i__1, &c_b12, &c_b12, &b[b_offset], ldb);
	i__1 = *n;
	for (j = *n - *l + 1; j <= i__1; ++j) {
	    i__2 = *l;
	    for (i__ = j - *n + *l + 1; i__ <= i__2; ++i__) {
		b[i__ + j * b_dim1] = 0.;
/* L50: */
	    }
/* L60: */
	}

    }

/*     Let              N-L     L */
/*                A = ( A11    A12 ) M, */

/*     then the following does the complete QR decomposition of A11: */

/*              A11 = U*(  0  T12 )*P1' */
/*                      (  0   0  ) */

    i__1 = *n - *l;
    for (i__ = 1; i__ <= i__1; ++i__) {
	iwork[i__] = 0;
/* L70: */
    }
    i__1 = *n - *l;
    dgeqpf_(m, &i__1, &a[a_offset], lda, &iwork[1], &tau[1], &work[1], info);

/*     Determine the effective rank of A11 */

    *k = 0;
/* Computing MIN */
    i__2 = *m, i__3 = *n - *l;
    i__1 = std::min(i__2,i__3);
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = a[i__ + i__ * a_dim1], abs(d__1)) > *tola) {
	    ++(*k);
	}
/* L80: */
    }

/*     Update A12 := U'*A12, where A12 = A( 1:M, N-L+1:N ) */

/* Computing MIN */
    i__2 = *m, i__3 = *n - *l;
    i__1 = std::min(i__2,i__3);
    dorm2r_("Left", "Transpose", m, l, &i__1, &a[a_offset], lda, &tau[1], &a[(
	    *n - *l + 1) * a_dim1 + 1], lda, &work[1], info);

    if (wantu) {

/*        Copy the details of U, and form U */

	dlaset_("Full", m, m, &c_b12, &c_b12, &u[u_offset], ldu);
	if (*m > 1) {
	    i__1 = *m - 1;
	    i__2 = *n - *l;
	    dlacpy_("Lower", &i__1, &i__2, &a[a_dim1 + 2], lda, &u[u_dim1 + 2]
, ldu);
	}
/* Computing MIN */
	i__2 = *m, i__3 = *n - *l;
	i__1 = std::min(i__2,i__3);
	dorg2r_(m, m, &i__1, &u[u_offset], ldu, &tau[1], &work[1], info);
    }

    if (wantq) {

/*        Update Q( 1:N, 1:N-L )  = Q( 1:N, 1:N-L )*P1 */

	i__1 = *n - *l;
	dlapmt_(&forwrd, n, &i__1, &q[q_offset], ldq, &iwork[1]);
    }

/*     Clean up A: set the strictly lower triangular part of */
/*     A(1:K, 1:K) = 0, and A( K+1:M, 1:N-L ) = 0. */

    i__1 = *k - 1;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *k;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    a[i__ + j * a_dim1] = 0.;
/* L90: */
	}
/* L100: */
    }
    if (*m > *k) {
	i__1 = *m - *k;
	i__2 = *n - *l;
	dlaset_("Full", &i__1, &i__2, &c_b12, &c_b12, &a[*k + 1 + a_dim1],
		lda);
    }

    if (*n - *l > *k) {

/*        RQ factorization of ( T11 T12 ) = ( 0 T12 )*Z1 */

	i__1 = *n - *l;
	dgerq2_(k, &i__1, &a[a_offset], lda, &tau[1], &work[1], info);

	if (wantq) {

/*           Update Q( 1:N,1:N-L ) = Q( 1:N,1:N-L )*Z1' */

	    i__1 = *n - *l;
	    dormr2_("Right", "Transpose", n, &i__1, k, &a[a_offset], lda, &
		    tau[1], &q[q_offset], ldq, &work[1], info);
	}

/*        Clean up A */

	i__1 = *n - *l - *k;
	dlaset_("Full", k, &i__1, &c_b12, &c_b12, &a[a_offset], lda);
	i__1 = *n - *l;
	for (j = *n - *l - *k + 1; j <= i__1; ++j) {
	    i__2 = *k;
	    for (i__ = j - *n + *l + *k + 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] = 0.;
/* L110: */
	    }
/* L120: */
	}

    }

    if (*m > *k) {

/*        QR factorization of A( K+1:M,N-L+1:N ) */

	i__1 = *m - *k;
	dgeqr2_(&i__1, l, &a[*k + 1 + (*n - *l + 1) * a_dim1], lda, &tau[1], &
		work[1], info);

	if (wantu) {

/*           Update U(:,K+1:M) := U(:,K+1:M)*U1 */

	    i__1 = *m - *k;
/* Computing MIN */
	    i__3 = *m - *k;
	    i__2 = std::min(i__3,*l);
	    dorm2r_("Right", "No transpose", m, &i__1, &i__2, &a[*k + 1 + (*n
		    - *l + 1) * a_dim1], lda, &tau[1], &u[(*k + 1) * u_dim1 +
		    1], ldu, &work[1], info);
	}

/*        Clean up */

	i__1 = *n;
	for (j = *n - *l + 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = j - *n + *k + *l + 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] = 0.;
/* L130: */
	    }
/* L140: */
	}

    }

    return 0;

/*     End of DGGSVP */

} /* dggsvp_ */

/* Subroutine */ int dgtcon_(const char *norm, integer *n, double *dl,
	double *d__, double *du, double *du2, integer *ipiv,
	double *anorm, double *rcond, double *work, integer *
	iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, kase, kase1;
    integer isave[3];
    double ainvnm;
    bool onenrm;

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

/*  DGTCON estimates the reciprocal of the condition number of a real */
/*  tridiagonal matrix A using the LU factorization as computed by */
/*  DGTTRF. */

/*  An estimate is obtained for norm(inv(A)), and the reciprocal of the */
/*  condition number is computed as RCOND = 1 / (ANORM * norm(inv(A))). */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies whether the 1-norm condition number or the */
/*          infinity-norm condition number is required: */
/*          = '1' or 'O':  1-norm; */
/*          = 'I':         Infinity-norm. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  DL      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) multipliers that define the matrix L from the */
/*          LU factorization of A as computed by DGTTRF. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The n diagonal elements of the upper triangular matrix U from */
/*          the LU factorization of A. */

/*  DU      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) elements of the first superdiagonal of U. */

/*  DU2     (input) DOUBLE PRECISION array, dimension (N-2) */
/*          The (n-2) elements of the second superdiagonal of U. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices; for 1 <= i <= n, row i of the matrix was */
/*          interchanged with row IPIV(i).  IPIV(i) will always be either */
/*          i or i+1; IPIV(i) = i indicates a row interchange was not */
/*          required. */

/*  ANORM   (input) DOUBLE PRECISION */
/*          If NORM = '1' or 'O', the 1-norm of the original matrix A. */
/*          If NORM = 'I', the infinity-norm of the original matrix A. */

/*  RCOND   (output) DOUBLE PRECISION */
/*          The reciprocal of the condition number of the matrix A, */
/*          computed as RCOND = 1/(ANORM * AINVNM), where AINVNM is an */
/*          estimate of the 1-norm of inv(A) computed in this routine. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (2*N) */

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
/*     .. Executable Statements .. */

/*     Test the input arguments. */

    /* Parameter adjustments */
    --iwork;
    --work;
    --ipiv;
    --du2;
    --du;
    --d__;
    --dl;

    /* Function Body */
    *info = 0;
    onenrm = *(unsigned char *)norm == '1' || lsame_(norm, "O");
    if (! onenrm && ! lsame_(norm, "I")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*anorm < 0.) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGTCON", &i__1);
	return 0;
    }

/*     Quick return if possible */

    *rcond = 0.;
    if (*n == 0) {
	*rcond = 1.;
	return 0;
    } else if (*anorm == 0.) {
	return 0;
    }

/*     Check that D(1:N) is non-zero. */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (d__[i__] == 0.) {
	    return 0;
	}
/* L10: */
    }

    ainvnm = 0.;
    if (onenrm) {
	kase1 = 1;
    } else {
	kase1 = 2;
    }
    kase = 0;
L20:
    dlacn2_(n, &work[*n + 1], &work[1], &iwork[1], &ainvnm, &kase, isave);
    if (kase != 0) {
	if (kase == kase1) {

/*           Multiply by inv(U)*inv(L). */

	    dgttrs_("No transpose", n, &c__1, &dl[1], &d__[1], &du[1], &du2[1]
, &ipiv[1], &work[1], n, info);
	} else {

/*           Multiply by inv(L')*inv(U'). */

	    dgttrs_("Transpose", n, &c__1, &dl[1], &d__[1], &du[1], &du2[1], &
		    ipiv[1], &work[1], n, info);
	}
	goto L20;
    }

/*     Compute the estimate of the reciprocal condition number. */

    if (ainvnm != 0.) {
	*rcond = 1. / ainvnm / *anorm;
    }

    return 0;

/*     End of DGTCON */

} /* dgtcon_ */

/* Subroutine */ int dgtrfs_(const char *trans, integer *n, integer *nrhs,
	double *dl, double *d__, double *du, double *dlf,
	double *df, double *duf, double *du2, integer *ipiv,
	double *b, integer *ldb, double *x, integer *ldx, double *
	ferr, double *berr, double *work, integer *iwork, integer *
	info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b18 = -1.;
	static double c_b19 = 1.;

    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, i__1, i__2;
    double d__1, d__2, d__3, d__4;

    /* Local variables */
    integer i__, j;
    double s;
    integer nz;
    double eps;
    integer kase;
    double safe1, safe2;
    integer isave[3];
    integer count;
    double safmin;
    bool notran;
    char transn[1];
    char transt[1];
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

/*  DGTRFS improves the computed solution to a system of linear */
/*  equations when the coefficient matrix is tridiagonal, and provides */
/*  error bounds and backward error estimates for the solution. */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations: */
/*          = 'N':  A * X = B     (No transpose) */
/*          = 'T':  A**T * X = B  (Transpose) */
/*          = 'C':  A**H * X = B  (Conjugate transpose = Transpose) */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  DL      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) subdiagonal elements of A. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal elements of A. */

/*  DU      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) superdiagonal elements of A. */

/*  DLF     (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) multipliers that define the matrix L from the */
/*          LU factorization of A as computed by DGTTRF. */

/*  DF      (input) DOUBLE PRECISION array, dimension (N) */
/*          The n diagonal elements of the upper triangular matrix U from */
/*          the LU factorization of A. */

/*  DUF     (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) elements of the first superdiagonal of U. */

/*  DU2     (input) DOUBLE PRECISION array, dimension (N-2) */
/*          The (n-2) elements of the second superdiagonal of U. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices; for 1 <= i <= n, row i of the matrix was */
/*          interchanged with row IPIV(i).  IPIV(i) will always be either */
/*          i or i+1; IPIV(i) = i indicates a row interchange was not */
/*          required. */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          The right hand side matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (input/output) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          On entry, the solution matrix X, as computed by DGTTRS. */
/*          On exit, the improved solution matrix X. */

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

/*  Internal Parameters */
/*  =================== */

/*  ITMAX is the maximum number of steps of iterative refinement. */

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
    --dl;
    --d__;
    --du;
    --dlf;
    --df;
    --duf;
    --du2;
    --ipiv;
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
    notran = lsame_(trans, "N");
    if (! notran && ! lsame_(trans, "T") && ! lsame_(
	    trans, "C")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -13;
    } else if (*ldx < std::max(1_integer,*n)) {
	*info = -15;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGTRFS", &i__1);
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
	*(unsigned char *)transn = 'N';
	*(unsigned char *)transt = 'T';
    } else {
	*(unsigned char *)transn = 'T';
	*(unsigned char *)transt = 'N';
    }

/*     NZ = maximum number of nonzero elements in each row of A, plus 1 */

    nz = 4;
    eps = dlamch_("Epsilon");
    safmin = dlamch_("Safe minimum");
    safe1 = nz * safmin;
    safe2 = safe1 / eps;

/*     Do for each right hand side */

    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {

	count = 1;
	lstres = 3.;
L20:

/*        Loop until stopping criterion is satisfied. */

/*        Compute residual R = B - op(A) * X, */
/*        where op(A) = A, A**T, or A**H, depending on TRANS. */

	dcopy_(n, &b[j * b_dim1 + 1], &c__1, &work[*n + 1], &c__1);
	dlagtm_(trans, n, &c__1, &c_b18, &dl[1], &d__[1], &du[1], &x[j *
		x_dim1 + 1], ldx, &c_b19, &work[*n + 1], n);

/*        Compute abs(op(A))*abs(x) + abs(b) for use in the backward */
/*        error bound. */

	if (notran) {
	    if (*n == 1) {
		work[1] = (d__1 = b[j * b_dim1 + 1], abs(d__1)) + (d__2 = d__[
			1] * x[j * x_dim1 + 1], abs(d__2));
	    } else {
		work[1] = (d__1 = b[j * b_dim1 + 1], abs(d__1)) + (d__2 = d__[
			1] * x[j * x_dim1 + 1], abs(d__2)) + (d__3 = du[1] *
			x[j * x_dim1 + 2], abs(d__3));
		i__2 = *n - 1;
		for (i__ = 2; i__ <= i__2; ++i__) {
		    work[i__] = (d__1 = b[i__ + j * b_dim1], abs(d__1)) + (
			    d__2 = dl[i__ - 1] * x[i__ - 1 + j * x_dim1], abs(
			    d__2)) + (d__3 = d__[i__] * x[i__ + j * x_dim1],
			    abs(d__3)) + (d__4 = du[i__] * x[i__ + 1 + j *
			    x_dim1], abs(d__4));
/* L30: */
		}
		work[*n] = (d__1 = b[*n + j * b_dim1], abs(d__1)) + (d__2 =
			dl[*n - 1] * x[*n - 1 + j * x_dim1], abs(d__2)) + (
			d__3 = d__[*n] * x[*n + j * x_dim1], abs(d__3));
	    }
	} else {
	    if (*n == 1) {
		work[1] = (d__1 = b[j * b_dim1 + 1], abs(d__1)) + (d__2 = d__[
			1] * x[j * x_dim1 + 1], abs(d__2));
	    } else {
		work[1] = (d__1 = b[j * b_dim1 + 1], abs(d__1)) + (d__2 = d__[
			1] * x[j * x_dim1 + 1], abs(d__2)) + (d__3 = dl[1] *
			x[j * x_dim1 + 2], abs(d__3));
		i__2 = *n - 1;
		for (i__ = 2; i__ <= i__2; ++i__) {
		    work[i__] = (d__1 = b[i__ + j * b_dim1], abs(d__1)) + (
			    d__2 = du[i__ - 1] * x[i__ - 1 + j * x_dim1], abs(
			    d__2)) + (d__3 = d__[i__] * x[i__ + j * x_dim1],
			    abs(d__3)) + (d__4 = dl[i__] * x[i__ + 1 + j *
			    x_dim1], abs(d__4));
/* L40: */
		}
		work[*n] = (d__1 = b[*n + j * b_dim1], abs(d__1)) + (d__2 =
			du[*n - 1] * x[*n - 1 + j * x_dim1], abs(d__2)) + (
			d__3 = d__[*n] * x[*n + j * x_dim1], abs(d__3));
	    }
	}

/*        Compute componentwise relative backward error from formula */

/*        max(i) ( abs(R(i)) / ( abs(op(A))*abs(X) + abs(B) )(i) ) */

/*        where abs(Z) is the componentwise absolute value of the matrix */
/*        or vector Z.  If the i-th component of the denominator is less */
/*        than SAFE2, then SAFE1 is added to the i-th components of the */
/*        numerator and denominator before dividing. */

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
/* L50: */
	}
	berr[j] = s;

/*        Test stopping criterion. Continue iterating if */
/*           1) The residual BERR(J) is larger than machine epsilon, and */
/*           2) BERR(J) decreased by at least a factor of 2 during the */
/*              last iteration, and */
/*           3) At most ITMAX iterations tried. */

	if (berr[j] > eps && berr[j] * 2. <= lstres && count <= 5) {

/*           Update solution and try again. */

	    dgttrs_(trans, n, &c__1, &dlf[1], &df[1], &duf[1], &du2[1], &ipiv[
		    1], &work[*n + 1], n, info);
	    daxpy_(n, &c_b19, &work[*n + 1], &c__1, &x[j * x_dim1 + 1], &c__1)
		    ;
	    lstres = berr[j];
	    ++count;
	    goto L20;
	}

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
/* L60: */
	}

	kase = 0;
L70:
	dlacn2_(n, &work[(*n << 1) + 1], &work[*n + 1], &iwork[1], &ferr[j], &
		kase, isave);
	if (kase != 0) {
	    if (kase == 1) {

/*              Multiply by diag(W)*inv(op(A)**T). */

		dgttrs_(transt, n, &c__1, &dlf[1], &df[1], &duf[1], &du2[1], &
			ipiv[1], &work[*n + 1], n, info);
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = work[i__] * work[*n + i__];
/* L80: */
		}
	    } else {

/*              Multiply by inv(op(A))*diag(W). */

		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = work[i__] * work[*n + i__];
/* L90: */
		}
		dgttrs_(transn, n, &c__1, &dlf[1], &df[1], &duf[1], &du2[1], &
			ipiv[1], &work[*n + 1], n, info);
	    }
	    goto L70;
	}

/*        Normalize error. */

	lstres = 0.;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = lstres, d__3 = (d__1 = x[i__ + j * x_dim1], abs(d__1));
	    lstres = std::max(d__2,d__3);
/* L100: */
	}
	if (lstres != 0.) {
	    ferr[j] /= lstres;
	}

/* L110: */
    }

    return 0;

/*     End of DGTRFS */

} /* dgtrfs_ */

/* Subroutine */ int dgtsv_(integer *n, integer *nrhs, double *dl,
	double *d__, double *du, double *b, integer *ldb, integer
	*info)
{
    /* System generated locals */
    integer b_dim1, b_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j;
    double fact, temp;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGTSV  solves the equation */

/*     A*X = B, */

/*  where A is an n by n tridiagonal matrix, by Gaussian elimination with */
/*  partial pivoting. */

/*  Note that the equation  A'*X = B  may be solved by interchanging the */
/*  order of the arguments DU and DL. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  DL      (input/output) DOUBLE PRECISION array, dimension (N-1) */
/*          On entry, DL must contain the (n-1) sub-diagonal elements of */
/*          A. */

/*          On exit, DL is overwritten by the (n-2) elements of the */
/*          second super-diagonal of the upper triangular matrix U from */
/*          the LU factorization of A, in DL(1), ..., DL(n-2). */

/*  D       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, D must contain the diagonal elements of A. */

/*          On exit, D is overwritten by the n diagonal elements of U. */

/*  DU      (input/output) DOUBLE PRECISION array, dimension (N-1) */
/*          On entry, DU must contain the (n-1) super-diagonal elements */
/*          of A. */

/*          On exit, DU is overwritten by the (n-1) elements of the first */
/*          super-diagonal of U. */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the N by NRHS matrix of right hand side matrix B. */
/*          On exit, if INFO = 0, the N by NRHS solution matrix X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          > 0: if INFO = i, U(i,i) is exactly zero, and the solution */
/*               has not been computed.  The factorization has not been */
/*               completed unless i = N. */

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

    /* Parameter adjustments */
    --dl;
    --d__;
    --du;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    *info = 0;
    if (*n < 0) {
	*info = -1;
    } else if (*nrhs < 0) {
	*info = -2;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -7;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGTSV ", &i__1);
	return 0;
    }

    if (*n == 0) {
	return 0;
    }

    if (*nrhs == 1) {
	i__1 = *n - 2;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if ((d__1 = d__[i__], abs(d__1)) >= (d__2 = dl[i__], abs(d__2))) {

/*              No row interchange required */

		if (d__[i__] != 0.) {
		    fact = dl[i__] / d__[i__];
		    d__[i__ + 1] -= fact * du[i__];
		    b[i__ + 1 + b_dim1] -= fact * b[i__ + b_dim1];
		} else {
		    *info = i__;
		    return 0;
		}
		dl[i__] = 0.;
	    } else {

/*              Interchange rows I and I+1 */

		fact = d__[i__] / dl[i__];
		d__[i__] = dl[i__];
		temp = d__[i__ + 1];
		d__[i__ + 1] = du[i__] - fact * temp;
		dl[i__] = du[i__ + 1];
		du[i__ + 1] = -fact * dl[i__];
		du[i__] = temp;
		temp = b[i__ + b_dim1];
		b[i__ + b_dim1] = b[i__ + 1 + b_dim1];
		b[i__ + 1 + b_dim1] = temp - fact * b[i__ + 1 + b_dim1];
	    }
/* L10: */
	}
	if (*n > 1) {
	    i__ = *n - 1;
	    if ((d__1 = d__[i__], abs(d__1)) >= (d__2 = dl[i__], abs(d__2))) {
		if (d__[i__] != 0.) {
		    fact = dl[i__] / d__[i__];
		    d__[i__ + 1] -= fact * du[i__];
		    b[i__ + 1 + b_dim1] -= fact * b[i__ + b_dim1];
		} else {
		    *info = i__;
		    return 0;
		}
	    } else {
		fact = d__[i__] / dl[i__];
		d__[i__] = dl[i__];
		temp = d__[i__ + 1];
		d__[i__ + 1] = du[i__] - fact * temp;
		du[i__] = temp;
		temp = b[i__ + b_dim1];
		b[i__ + b_dim1] = b[i__ + 1 + b_dim1];
		b[i__ + 1 + b_dim1] = temp - fact * b[i__ + 1 + b_dim1];
	    }
	}
	if (d__[*n] == 0.) {
	    *info = *n;
	    return 0;
	}
    } else {
	i__1 = *n - 2;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if ((d__1 = d__[i__], abs(d__1)) >= (d__2 = dl[i__], abs(d__2))) {

/*              No row interchange required */

		if (d__[i__] != 0.) {
		    fact = dl[i__] / d__[i__];
		    d__[i__ + 1] -= fact * du[i__];
		    i__2 = *nrhs;
		    for (j = 1; j <= i__2; ++j) {
			b[i__ + 1 + j * b_dim1] -= fact * b[i__ + j * b_dim1];
/* L20: */
		    }
		} else {
		    *info = i__;
		    return 0;
		}
		dl[i__] = 0.;
	    } else {

/*              Interchange rows I and I+1 */

		fact = d__[i__] / dl[i__];
		d__[i__] = dl[i__];
		temp = d__[i__ + 1];
		d__[i__ + 1] = du[i__] - fact * temp;
		dl[i__] = du[i__ + 1];
		du[i__ + 1] = -fact * dl[i__];
		du[i__] = temp;
		i__2 = *nrhs;
		for (j = 1; j <= i__2; ++j) {
		    temp = b[i__ + j * b_dim1];
		    b[i__ + j * b_dim1] = b[i__ + 1 + j * b_dim1];
		    b[i__ + 1 + j * b_dim1] = temp - fact * b[i__ + 1 + j *
			    b_dim1];
/* L30: */
		}
	    }
/* L40: */
	}
	if (*n > 1) {
	    i__ = *n - 1;
	    if ((d__1 = d__[i__], abs(d__1)) >= (d__2 = dl[i__], abs(d__2))) {
		if (d__[i__] != 0.) {
		    fact = dl[i__] / d__[i__];
		    d__[i__ + 1] -= fact * du[i__];
		    i__1 = *nrhs;
		    for (j = 1; j <= i__1; ++j) {
			b[i__ + 1 + j * b_dim1] -= fact * b[i__ + j * b_dim1];
/* L50: */
		    }
		} else {
		    *info = i__;
		    return 0;
		}
	    } else {
		fact = d__[i__] / dl[i__];
		d__[i__] = dl[i__];
		temp = d__[i__ + 1];
		d__[i__ + 1] = du[i__] - fact * temp;
		du[i__] = temp;
		i__1 = *nrhs;
		for (j = 1; j <= i__1; ++j) {
		    temp = b[i__ + j * b_dim1];
		    b[i__ + j * b_dim1] = b[i__ + 1 + j * b_dim1];
		    b[i__ + 1 + j * b_dim1] = temp - fact * b[i__ + 1 + j *
			    b_dim1];
/* L60: */
		}
	    }
	}
	if (d__[*n] == 0.) {
	    *info = *n;
	    return 0;
	}
    }

/*     Back solve with the matrix U from the factorization. */

    if (*nrhs <= 2) {
	j = 1;
L70:
	b[*n + j * b_dim1] /= d__[*n];
	if (*n > 1) {
	    b[*n - 1 + j * b_dim1] = (b[*n - 1 + j * b_dim1] - du[*n - 1] * b[
		    *n + j * b_dim1]) / d__[*n - 1];
	}
	for (i__ = *n - 2; i__ >= 1; --i__) {
	    b[i__ + j * b_dim1] = (b[i__ + j * b_dim1] - du[i__] * b[i__ + 1
		    + j * b_dim1] - dl[i__] * b[i__ + 2 + j * b_dim1]) / d__[
		    i__];
/* L80: */
	}
	if (j < *nrhs) {
	    ++j;
	    goto L70;
	}
    } else {
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    b[*n + j * b_dim1] /= d__[*n];
	    if (*n > 1) {
		b[*n - 1 + j * b_dim1] = (b[*n - 1 + j * b_dim1] - du[*n - 1]
			* b[*n + j * b_dim1]) / d__[*n - 1];
	    }
	    for (i__ = *n - 2; i__ >= 1; --i__) {
		b[i__ + j * b_dim1] = (b[i__ + j * b_dim1] - du[i__] * b[i__
			+ 1 + j * b_dim1] - dl[i__] * b[i__ + 2 + j * b_dim1])
			 / d__[i__];
/* L90: */
	    }
/* L100: */
	}
    }

    return 0;

/*     End of DGTSV */

} /* dgtsv_ */

/* Subroutine */ int dgtsvx_(const char *fact, const char *trans, integer *n, integer *
	nrhs, double *dl, double *d__, double *du, double *
	dlf, double *df, double *duf, double *du2, integer *ipiv,
	double *b, integer *ldb, double *x, integer *ldx, double *
	rcond, double *ferr, double *berr, double *work, integer *
	iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, i__1;

    /* Local variables */
    char norm[1];
    double anorm;
    bool nofact;
    bool notran;

/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGTSVX uses the LU factorization to compute the solution to a real */
/*  system of linear equations A * X = B or A**T * X = B, */
/*  where A is a tridiagonal matrix of order N and X and B are N-by-NRHS */
/*  matrices. */

/*  Error bounds on the solution and a condition estimate are also */
/*  provided. */

/*  Description */
/*  =========== */

/*  The following steps are performed: */

/*  1. If FACT = 'N', the LU decomposition is used to factor the matrix A */
/*     as A = L * U, where L is a product of permutation and unit lower */
/*     bidiagonal matrices and U is upper triangular with nonzeros in */
/*     only the main diagonal and first two superdiagonals. */

/*  2. If some U(i,i)=0, so that U is exactly singular, then the routine */
/*     returns with INFO = i. Otherwise, the factored form of A is used */
/*     to estimate the condition number of the matrix A.  If the */
/*     reciprocal of the condition number is less than machine precision, */
/*     INFO = N+1 is returned as a warning, but the routine still goes on */
/*     to solve for X and compute error bounds as described below. */

/*  3. The system of equations is solved for X using the factored form */
/*     of A. */

/*  4. Iterative refinement is applied to improve the computed solution */
/*     matrix and calculate error bounds and backward error estimates */
/*     for it. */

/*  Arguments */
/*  ========= */

/*  FACT    (input) CHARACTER*1 */
/*          Specifies whether or not the factored form of A has been */
/*          supplied on entry. */
/*          = 'F':  DLF, DF, DUF, DU2, and IPIV contain the factored */
/*                  form of A; DL, D, DU, DLF, DF, DUF, DU2 and IPIV */
/*                  will not be modified. */
/*          = 'N':  The matrix will be copied to DLF, DF, and DUF */
/*                  and factored. */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations: */
/*          = 'N':  A * X = B     (No transpose) */
/*          = 'T':  A**T * X = B  (Transpose) */
/*          = 'C':  A**H * X = B  (Conjugate transpose = Transpose) */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  DL      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) subdiagonal elements of A. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The n diagonal elements of A. */

/*  DU      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) superdiagonal elements of A. */

/*  DLF     (input or output) DOUBLE PRECISION array, dimension (N-1) */
/*          If FACT = 'F', then DLF is an input argument and on entry */
/*          contains the (n-1) multipliers that define the matrix L from */
/*          the LU factorization of A as computed by DGTTRF. */

/*          If FACT = 'N', then DLF is an output argument and on exit */
/*          contains the (n-1) multipliers that define the matrix L from */
/*          the LU factorization of A. */

/*  DF      (input or output) DOUBLE PRECISION array, dimension (N) */
/*          If FACT = 'F', then DF is an input argument and on entry */
/*          contains the n diagonal elements of the upper triangular */
/*          matrix U from the LU factorization of A. */

/*          If FACT = 'N', then DF is an output argument and on exit */
/*          contains the n diagonal elements of the upper triangular */
/*          matrix U from the LU factorization of A. */

/*  DUF     (input or output) DOUBLE PRECISION array, dimension (N-1) */
/*          If FACT = 'F', then DUF is an input argument and on entry */
/*          contains the (n-1) elements of the first superdiagonal of U. */

/*          If FACT = 'N', then DUF is an output argument and on exit */
/*          contains the (n-1) elements of the first superdiagonal of U. */

/*  DU2     (input or output) DOUBLE PRECISION array, dimension (N-2) */
/*          If FACT = 'F', then DU2 is an input argument and on entry */
/*          contains the (n-2) elements of the second superdiagonal of */
/*          U. */

/*          If FACT = 'N', then DU2 is an output argument and on exit */
/*          contains the (n-2) elements of the second superdiagonal of */
/*          U. */

/*  IPIV    (input or output) INTEGER array, dimension (N) */
/*          If FACT = 'F', then IPIV is an input argument and on entry */
/*          contains the pivot indices from the LU factorization of A as */
/*          computed by DGTTRF. */

/*          If FACT = 'N', then IPIV is an output argument and on exit */
/*          contains the pivot indices from the LU factorization of A; */
/*          row i of the matrix was interchanged with row IPIV(i). */
/*          IPIV(i) will always be either i or i+1; IPIV(i) = i indicates */
/*          a row interchange was not required. */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          The N-by-NRHS right hand side matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (output) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          If INFO = 0 or INFO = N+1, the N-by-NRHS solution matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  RCOND   (output) DOUBLE PRECISION */
/*          The estimate of the reciprocal condition number of the matrix */
/*          A.  If RCOND is less than the machine precision (in */
/*          particular, if RCOND = 0), the matrix is singular to working */
/*          precision.  This condition is indicated by a return code of */
/*          INFO > 0. */

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
/*          > 0:  if INFO = i, and i is */
/*                <= N:  U(i,i) is exactly zero.  The factorization */
/*                       has not been completed unless i = N, but the */
/*                       factor U is exactly singular, so the solution */
/*                       and error bounds could not be computed. */
/*                       RCOND = 0 is returned. */
/*                = N+1: U is nonsingular, but RCOND is less than machine */
/*                       precision, meaning that the matrix is singular */
/*                       to working precision.  Nevertheless, the */
/*                       solution and error bounds are computed because */
/*                       there are a number of situations where the */
/*                       computed solution can be more accurate than the */
/*                       value of RCOND would suggest. */

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
    --dl;
    --d__;
    --du;
    --dlf;
    --df;
    --duf;
    --du2;
    --ipiv;
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
    nofact = lsame_(fact, "N");
    notran = lsame_(trans, "N");
    if (! nofact && ! lsame_(fact, "F")) {
	*info = -1;
    } else if (! notran && ! lsame_(trans, "T") && !
	    lsame_(trans, "C")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*nrhs < 0) {
	*info = -4;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -14;
    } else if (*ldx < std::max(1_integer,*n)) {
	*info = -16;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGTSVX", &i__1);
	return 0;
    }

    if (nofact) {

/*        Compute the LU factorization of A. */

	dcopy_(n, &d__[1], &c__1, &df[1], &c__1);
	if (*n > 1) {
	    i__1 = *n - 1;
	    dcopy_(&i__1, &dl[1], &c__1, &dlf[1], &c__1);
	    i__1 = *n - 1;
	    dcopy_(&i__1, &du[1], &c__1, &duf[1], &c__1);
	}
	dgttrf_(n, &dlf[1], &df[1], &duf[1], &du2[1], &ipiv[1], info);

/*        Return if INFO is non-zero. */

	if (*info > 0) {
	    *rcond = 0.;
	    return 0;
	}
    }

/*     Compute the norm of the matrix A. */

    if (notran) {
	*(unsigned char *)norm = '1';
    } else {
	*(unsigned char *)norm = 'I';
    }
    anorm = dlangt_(norm, n, &dl[1], &d__[1], &du[1]);

/*     Compute the reciprocal of the condition number of A. */

    dgtcon_(norm, n, &dlf[1], &df[1], &duf[1], &du2[1], &ipiv[1], &anorm,
	    rcond, &work[1], &iwork[1], info);

/*     Compute the solution vectors X. */

    dlacpy_("Full", n, nrhs, &b[b_offset], ldb, &x[x_offset], ldx);
    dgttrs_(trans, n, nrhs, &dlf[1], &df[1], &duf[1], &du2[1], &ipiv[1], &x[
	    x_offset], ldx, info);

/*     Use iterative refinement to improve the computed solutions and */
/*     compute error bounds and backward error estimates for them. */

    dgtrfs_(trans, n, nrhs, &dl[1], &d__[1], &du[1], &dlf[1], &df[1], &duf[1],
	     &du2[1], &ipiv[1], &b[b_offset], ldb, &x[x_offset], ldx, &ferr[1]
, &berr[1], &work[1], &iwork[1], info);

/*     Set INFO = N+1 if the matrix is singular to working precision. */

    if (*rcond < dlamch_("Epsilon")) {
	*info = *n + 1;
    }

    return 0;

/*     End of DGTSVX */

} /* dgtsvx_ */

/* Subroutine */ int dgttrf_(integer *n, double *dl, double *d__,
	double *du, double *du2, integer *ipiv, integer *info)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2;

    /* Local variables */
    integer i__;
    double fact, temp;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGTTRF computes an LU factorization of a real tridiagonal matrix A */
/*  using elimination with partial pivoting and row interchanges. */

/*  The factorization has the form */
/*     A = L * U */
/*  where L is a product of permutation and unit lower bidiagonal */
/*  matrices and U is upper triangular with nonzeros in only the main */
/*  diagonal and first two superdiagonals. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A. */

/*  DL      (input/output) DOUBLE PRECISION array, dimension (N-1) */
/*          On entry, DL must contain the (n-1) sub-diagonal elements of */
/*          A. */

/*          On exit, DL is overwritten by the (n-1) multipliers that */
/*          define the matrix L from the LU factorization of A. */

/*  D       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, D must contain the diagonal elements of A. */

/*          On exit, D is overwritten by the n diagonal elements of the */
/*          upper triangular matrix U from the LU factorization of A. */

/*  DU      (input/output) DOUBLE PRECISION array, dimension (N-1) */
/*          On entry, DU must contain the (n-1) super-diagonal elements */
/*          of A. */

/*          On exit, DU is overwritten by the (n-1) elements of the first */
/*          super-diagonal of U. */

/*  DU2     (output) DOUBLE PRECISION array, dimension (N-2) */
/*          On exit, DU2 is overwritten by the (n-2) elements of the */
/*          second super-diagonal of U. */

/*  IPIV    (output) INTEGER array, dimension (N) */
/*          The pivot indices; for 1 <= i <= n, row i of the matrix was */
/*          interchanged with row IPIV(i).  IPIV(i) will always be either */
/*          i or i+1; IPIV(i) = i indicates a row interchange was not */
/*          required. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -k, the k-th argument had an illegal value */
/*          > 0:  if INFO = k, U(k,k) is exactly zero. The factorization */
/*                has been completed, but the factor U is exactly */
/*                singular, and division by zero will occur if it is used */
/*                to solve a system of equations. */

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

    /* Parameter adjustments */
    --ipiv;
    --du2;
    --du;
    --d__;
    --dl;

    /* Function Body */
    *info = 0;
    if (*n < 0) {
	*info = -1;
	i__1 = -(*info);
	xerbla_("DGTTRF", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Initialize IPIV(i) = i and DU2(I) = 0 */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ipiv[i__] = i__;
/* L10: */
    }
    i__1 = *n - 2;
    for (i__ = 1; i__ <= i__1; ++i__) {
	du2[i__] = 0.;
/* L20: */
    }

    i__1 = *n - 2;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = d__[i__], abs(d__1)) >= (d__2 = dl[i__], abs(d__2))) {

/*           No row interchange required, eliminate DL(I) */

	    if (d__[i__] != 0.) {
		fact = dl[i__] / d__[i__];
		dl[i__] = fact;
		d__[i__ + 1] -= fact * du[i__];
	    }
	} else {

/*           Interchange rows I and I+1, eliminate DL(I) */

	    fact = d__[i__] / dl[i__];
	    d__[i__] = dl[i__];
	    dl[i__] = fact;
	    temp = du[i__];
	    du[i__] = d__[i__ + 1];
	    d__[i__ + 1] = temp - fact * d__[i__ + 1];
	    du2[i__] = du[i__ + 1];
	    du[i__ + 1] = -fact * du[i__ + 1];
	    ipiv[i__] = i__ + 1;
	}
/* L30: */
    }
    if (*n > 1) {
	i__ = *n - 1;
	if ((d__1 = d__[i__], abs(d__1)) >= (d__2 = dl[i__], abs(d__2))) {
	    if (d__[i__] != 0.) {
		fact = dl[i__] / d__[i__];
		dl[i__] = fact;
		d__[i__ + 1] -= fact * du[i__];
	    }
	} else {
	    fact = d__[i__] / dl[i__];
	    d__[i__] = dl[i__];
	    dl[i__] = fact;
	    temp = du[i__];
	    du[i__] = d__[i__ + 1];
	    d__[i__ + 1] = temp - fact * d__[i__ + 1];
	    ipiv[i__] = i__ + 1;
	}
    }

/*     Check for a zero on the diagonal of U. */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (d__[i__] == 0.) {
	    *info = i__;
	    goto L50;
	}
/* L40: */
    }
L50:

    return 0;

/*     End of DGTTRF */

} /* dgttrf_ */

/* Subroutine */ int dgttrs_(const char *trans, integer *n, integer *nrhs,
	double *dl, double *d__, double *du, double *du2,
	integer *ipiv, double *b, integer *ldb, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;

    /* System generated locals */
    integer b_dim1, b_offset, i__1, i__2, i__3;

    /* Local variables */
    integer j, jb, nb;
    integer itrans;
    bool notran;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGTTRS solves one of the systems of equations */
/*     A*X = B  or  A'*X = B, */
/*  with a tridiagonal matrix A using the LU factorization computed */
/*  by DGTTRF. */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the form of the system of equations. */
/*          = 'N':  A * X = B  (No transpose) */
/*          = 'T':  A'* X = B  (Transpose) */
/*          = 'C':  A'* X = B  (Conjugate transpose = Transpose) */

/*  N       (input) INTEGER */
/*          The order of the matrix A. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  DL      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) multipliers that define the matrix L from the */
/*          LU factorization of A. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The n diagonal elements of the upper triangular matrix U from */
/*          the LU factorization of A. */

/*  DU      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) elements of the first super-diagonal of U. */

/*  DU2     (input) DOUBLE PRECISION array, dimension (N-2) */
/*          The (n-2) elements of the second super-diagonal of U. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices; for 1 <= i <= n, row i of the matrix was */
/*          interchanged with row IPIV(i).  IPIV(i) will always be either */
/*          i or i+1; IPIV(i) = i indicates a row interchange was not */
/*          required. */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the matrix of right hand side vectors B. */
/*          On exit, B is overwritten by the solution vectors X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  ===================================================================== */

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
    --dl;
    --d__;
    --du;
    --du2;
    --ipiv;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    *info = 0;
    notran = *(unsigned char *)trans == 'N' || *(unsigned char *)trans == 'n';
    if (! notran && ! (*(unsigned char *)trans == 'T' || *(unsigned char *)
	    trans == 't') && ! (*(unsigned char *)trans == 'C' || *(unsigned
	    char *)trans == 'c')) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*ldb < std::max(*n,1_integer)) {
	*info = -10;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DGTTRS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *nrhs == 0) {
	return 0;
    }

/*     Decode TRANS */

    if (notran) {
	itrans = 0;
    } else {
	itrans = 1;
    }

/*     Determine the number of right-hand sides to solve at a time. */

    if (*nrhs == 1) {
	nb = 1;
    } else {
/* Computing MAX */
	i__1 = 1, i__2 = ilaenv_(&c__1, "DGTTRS", trans, n, nrhs, &c_n1, &
		c_n1);
	nb = std::max(i__1,i__2);
    }

    if (nb >= *nrhs) {
	dgtts2_(&itrans, n, nrhs, &dl[1], &d__[1], &du[1], &du2[1], &ipiv[1],
		&b[b_offset], ldb);
    } else {
	i__1 = *nrhs;
	i__2 = nb;
	for (j = 1; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {
/* Computing MIN */
	    i__3 = *nrhs - j + 1;
	    jb = std::min(i__3,nb);
	    dgtts2_(&itrans, n, &jb, &dl[1], &d__[1], &du[1], &du2[1], &ipiv[
		    1], &b[j * b_dim1 + 1], ldb);
/* L10: */
	}
    }

/*     End of DGTTRS */

    return 0;
} /* dgttrs_ */

/* Subroutine */ int dgtts2_(integer *itrans, integer *n, integer *nrhs,
	double *dl, double *d__, double *du, double *du2,
	integer *ipiv, double *b, integer *ldb)
{
    /* System generated locals */
    integer b_dim1, b_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, ip;
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

/*  DGTTS2 solves one of the systems of equations */
/*     A*X = B  or  A'*X = B, */
/*  with a tridiagonal matrix A using the LU factorization computed */
/*  by DGTTRF. */

/*  Arguments */
/*  ========= */

/*  ITRANS  (input) INTEGER */
/*          Specifies the form of the system of equations. */
/*          = 0:  A * X = B  (No transpose) */
/*          = 1:  A'* X = B  (Transpose) */
/*          = 2:  A'* X = B  (Conjugate transpose = Transpose) */

/*  N       (input) INTEGER */
/*          The order of the matrix A. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  DL      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) multipliers that define the matrix L from the */
/*          LU factorization of A. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The n diagonal elements of the upper triangular matrix U from */
/*          the LU factorization of A. */

/*  DU      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) elements of the first super-diagonal of U. */

/*  DU2     (input) DOUBLE PRECISION array, dimension (N-2) */
/*          The (n-2) elements of the second super-diagonal of U. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          The pivot indices; for 1 <= i <= n, row i of the matrix was */
/*          interchanged with row IPIV(i).  IPIV(i) will always be either */
/*          i or i+1; IPIV(i) = i indicates a row interchange was not */
/*          required. */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the matrix of right hand side vectors B. */
/*          On exit, B is overwritten by the solution vectors X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick return if possible */

    /* Parameter adjustments */
    --dl;
    --d__;
    --du;
    --du2;
    --ipiv;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    if (*n == 0 || *nrhs == 0) {
	return 0;
    }

    if (*itrans == 0) {

/*        Solve A*X = B using the LU factorization of A, */
/*        overwriting each right hand side vector with its solution. */

	if (*nrhs <= 1) {
	    j = 1;
L10:

/*           Solve L*x = b. */

	    i__1 = *n - 1;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		ip = ipiv[i__];
		temp = b[i__ + 1 - ip + i__ + j * b_dim1] - dl[i__] * b[ip +
			j * b_dim1];
		b[i__ + j * b_dim1] = b[ip + j * b_dim1];
		b[i__ + 1 + j * b_dim1] = temp;
/* L20: */
	    }

/*           Solve U*x = b. */

	    b[*n + j * b_dim1] /= d__[*n];
	    if (*n > 1) {
		b[*n - 1 + j * b_dim1] = (b[*n - 1 + j * b_dim1] - du[*n - 1]
			* b[*n + j * b_dim1]) / d__[*n - 1];
	    }
	    for (i__ = *n - 2; i__ >= 1; --i__) {
		b[i__ + j * b_dim1] = (b[i__ + j * b_dim1] - du[i__] * b[i__
			+ 1 + j * b_dim1] - du2[i__] * b[i__ + 2 + j * b_dim1]
			) / d__[i__];
/* L30: */
	    }
	    if (j < *nrhs) {
		++j;
		goto L10;
	    }
	} else {
	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {

/*              Solve L*x = b. */

		i__2 = *n - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    if (ipiv[i__] == i__) {
			b[i__ + 1 + j * b_dim1] -= dl[i__] * b[i__ + j *
				b_dim1];
		    } else {
			temp = b[i__ + j * b_dim1];
			b[i__ + j * b_dim1] = b[i__ + 1 + j * b_dim1];
			b[i__ + 1 + j * b_dim1] = temp - dl[i__] * b[i__ + j *
				 b_dim1];
		    }
/* L40: */
		}

/*              Solve U*x = b. */

		b[*n + j * b_dim1] /= d__[*n];
		if (*n > 1) {
		    b[*n - 1 + j * b_dim1] = (b[*n - 1 + j * b_dim1] - du[*n
			    - 1] * b[*n + j * b_dim1]) / d__[*n - 1];
		}
		for (i__ = *n - 2; i__ >= 1; --i__) {
		    b[i__ + j * b_dim1] = (b[i__ + j * b_dim1] - du[i__] * b[
			    i__ + 1 + j * b_dim1] - du2[i__] * b[i__ + 2 + j *
			     b_dim1]) / d__[i__];
/* L50: */
		}
/* L60: */
	    }
	}
    } else {

/*        Solve A' * X = B. */

	if (*nrhs <= 1) {

/*           Solve U'*x = b. */

	    j = 1;
L70:
	    b[j * b_dim1 + 1] /= d__[1];
	    if (*n > 1) {
		b[j * b_dim1 + 2] = (b[j * b_dim1 + 2] - du[1] * b[j * b_dim1
			+ 1]) / d__[2];
	    }
	    i__1 = *n;
	    for (i__ = 3; i__ <= i__1; ++i__) {
		b[i__ + j * b_dim1] = (b[i__ + j * b_dim1] - du[i__ - 1] * b[
			i__ - 1 + j * b_dim1] - du2[i__ - 2] * b[i__ - 2 + j *
			 b_dim1]) / d__[i__];
/* L80: */
	    }

/*           Solve L'*x = b. */

	    for (i__ = *n - 1; i__ >= 1; --i__) {
		ip = ipiv[i__];
		temp = b[i__ + j * b_dim1] - dl[i__] * b[i__ + 1 + j * b_dim1]
			;
		b[i__ + j * b_dim1] = b[ip + j * b_dim1];
		b[ip + j * b_dim1] = temp;
/* L90: */
	    }
	    if (j < *nrhs) {
		++j;
		goto L70;
	    }

	} else {
	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {

/*              Solve U'*x = b. */

		b[j * b_dim1 + 1] /= d__[1];
		if (*n > 1) {
		    b[j * b_dim1 + 2] = (b[j * b_dim1 + 2] - du[1] * b[j *
			    b_dim1 + 1]) / d__[2];
		}
		i__2 = *n;
		for (i__ = 3; i__ <= i__2; ++i__) {
		    b[i__ + j * b_dim1] = (b[i__ + j * b_dim1] - du[i__ - 1] *
			     b[i__ - 1 + j * b_dim1] - du2[i__ - 2] * b[i__ -
			    2 + j * b_dim1]) / d__[i__];
/* L100: */
		}
		for (i__ = *n - 1; i__ >= 1; --i__) {
		    if (ipiv[i__] == i__) {
			b[i__ + j * b_dim1] -= dl[i__] * b[i__ + 1 + j *
				b_dim1];
		    } else {
			temp = b[i__ + 1 + j * b_dim1];
			b[i__ + 1 + j * b_dim1] = b[i__ + j * b_dim1] - dl[
				i__] * temp;
			b[i__ + j * b_dim1] = temp;
		    }
/* L110: */
		}
/* L120: */
	    }
	}
    }

/*     End of DGTTS2 */

    return 0;
} /* dgtts2_ */
