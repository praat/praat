#include "clapack.h"
#include "f2cP.h"

/* Subroutine */ int dsbev_(const char *jobz, const char *uplo, integer *n, integer *kd,
	double *ab, integer *ldab, double *w, double *z__,
	integer *ldz, double *work, integer *info)
{
	/* Table of constant values */
	static double c_b11 = 1.;
	static integer c__1 = 1;

    /* System generated locals */
    integer ab_dim1, ab_offset, z_dim1, z_offset, i__1;
    double d__1;

    /* Local variables */
    double eps;
    integer inde;
    double anrm;
    integer imax;
    double rmin, rmax;
    double sigma;
    integer iinfo;
    bool lower, wantz;
    integer iscale;
    double safmin;
    double bignum;
    integer indwrk;
    double smlnum;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSBEV computes all the eigenvalues and, optionally, eigenvectors of */
/*  a real symmetric band matrix A. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  KD      (input) INTEGER */
/*          The number of superdiagonals of the matrix A if UPLO = 'U', */
/*          or the number of subdiagonals if UPLO = 'L'.  KD >= 0. */

/*  AB      (input/output) DOUBLE PRECISION array, dimension (LDAB, N) */
/*          On entry, the upper or lower triangle of the symmetric band */
/*          matrix A, stored in the first KD+1 rows of the array.  The */
/*          j-th column of A is stored in the j-th column of the array AB */
/*          as follows: */
/*          if UPLO = 'U', AB(kd+1+i-j,j) = A(i,j) for max(1,j-kd)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+kd). */

/*          On exit, AB is overwritten by values generated during the */
/*          reduction to tridiagonal form.  If UPLO = 'U', the first */
/*          superdiagonal and the diagonal of the tridiagonal matrix T */
/*          are returned in rows KD and KD+1 of AB, and if UPLO = 'L', */
/*          the diagonal and first subdiagonal of T are returned in the */
/*          first two rows of AB. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KD + 1. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, the eigenvalues in ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, N) */
/*          If JOBZ = 'V', then if INFO = 0, Z contains the orthonormal */
/*          eigenvectors of the matrix A, with the i-th column of Z */
/*          holding the eigenvector associated with W(i). */
/*          If JOBZ = 'N', then Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (max(1,3*N-2)) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, the algorithm failed to converge; i */
/*                off-diagonal elements of an intermediate tridiagonal */
/*                form did not converge to zero. */

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
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    lower = lsame_(uplo, "L");

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (lower || lsame_(uplo, "U"))) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*kd < 0) {
	*info = -4;
    } else if (*ldab < *kd + 1) {
	*info = -6;
    } else if (*ldz < 1 || wantz && *ldz < *n) {
	*info = -9;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSBEV ", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (lower) {
	    w[1] = ab[ab_dim1 + 1];
	} else {
	    w[1] = ab[*kd + 1 + ab_dim1];
	}
	if (wantz) {
	    z__[z_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
    rmax = sqrt(bignum);

/*     Scale matrix to allowable range, if necessary. */

    anrm = dlansb_("M", uplo, n, kd, &ab[ab_offset], ldab, &work[1]);
    iscale = 0;
    if (anrm > 0. && anrm < rmin) {
	iscale = 1;
	sigma = rmin / anrm;
    } else if (anrm > rmax) {
	iscale = 1;
	sigma = rmax / anrm;
    }
    if (iscale == 1) {
	if (lower) {
	    dlascl_("B", kd, kd, &c_b11, &sigma, n, n, &ab[ab_offset], ldab,
		    info);
	} else {
	    dlascl_("Q", kd, kd, &c_b11, &sigma, n, n, &ab[ab_offset], ldab,
		    info);
	}
    }

/*     Call DSBTRD to reduce symmetric band matrix to tridiagonal form. */

    inde = 1;
    indwrk = inde + *n;
    dsbtrd_(jobz, uplo, n, kd, &ab[ab_offset], ldab, &w[1], &work[inde], &z__[
	    z_offset], ldz, &work[indwrk], &iinfo);

/*     For eigenvalues only, call DSTERF.  For eigenvectors, call SSTEQR. */

    if (! wantz) {
	dsterf_(n, &w[1], &work[inde], info);
    } else {
	dsteqr_(jobz, n, &w[1], &work[inde], &z__[z_offset], ldz, &work[
		indwrk], info);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

    if (iscale == 1) {
	if (*info == 0) {
	    imax = *n;
	} else {
	    imax = *info - 1;
	}
	d__1 = 1. / sigma;
	dscal_(&imax, &d__1, &w[1], &c__1);
    }

    return 0;

/*     End of DSBEV */

} /* dsbev_ */

/* Subroutine */ int dsbevd_(const char *jobz, const char *uplo, integer *n, integer *kd,
	double *ab, integer *ldab, double *w, double *z__,
	integer *ldz, double *work, integer *lwork, integer *iwork,
	integer *liwork, integer *info)
{
	/* Table of constant values */
	static double c_b11 = 1.;
	static double c_b18 = 0.;
	static integer c__1 = 1;

    /* System generated locals */
    integer ab_dim1, ab_offset, z_dim1, z_offset, i__1;
    double d__1;

    /* Local variables */
    double eps;
    integer inde;
    double anrm, rmin, rmax;
    double sigma;
    integer iinfo, lwmin;
    bool lower, wantz;
    integer indwk2, llwrk2;
    integer iscale;
    double safmin;
    double bignum;
	integer indwrk, liwmin;
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

/*  DSBEVD computes all the eigenvalues and, optionally, eigenvectors of */
/*  a real symmetric band matrix A. If eigenvectors are desired, it uses */
/*  a divide and conquer algorithm. */

/*  The divide and conquer algorithm makes very mild assumptions about */
/*  floating point arithmetic. It will work on machines with a guard */
/*  digit in add/subtract, or on those binary machines without guard */
/*  digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or */
/*  Cray-2. It could conceivably fail on hexadecimal or decimal machines */
/*  without guard digits, but we know of none. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  KD      (input) INTEGER */
/*          The number of superdiagonals of the matrix A if UPLO = 'U', */
/*          or the number of subdiagonals if UPLO = 'L'.  KD >= 0. */

/*  AB      (input/output) DOUBLE PRECISION array, dimension (LDAB, N) */
/*          On entry, the upper or lower triangle of the symmetric band */
/*          matrix A, stored in the first KD+1 rows of the array.  The */
/*          j-th column of A is stored in the j-th column of the array AB */
/*          as follows: */
/*          if UPLO = 'U', AB(kd+1+i-j,j) = A(i,j) for max(1,j-kd)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+kd). */

/*          On exit, AB is overwritten by values generated during the */
/*          reduction to tridiagonal form.  If UPLO = 'U', the first */
/*          superdiagonal and the diagonal of the tridiagonal matrix T */
/*          are returned in rows KD and KD+1 of AB, and if UPLO = 'L', */
/*          the diagonal and first subdiagonal of T are returned in the */
/*          first two rows of AB. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KD + 1. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, the eigenvalues in ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, N) */
/*          If JOBZ = 'V', then if INFO = 0, Z contains the orthonormal */
/*          eigenvectors of the matrix A, with the i-th column of Z */
/*          holding the eigenvector associated with W(i). */
/*          If JOBZ = 'N', then Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, */
/*                                         dimension (LWORK) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          IF N <= 1,                LWORK must be at least 1. */
/*          If JOBZ  = 'N' and N > 2, LWORK must be at least 2*N. */
/*          If JOBZ  = 'V' and N > 2, LWORK must be at least */
/*                         ( 1 + 5*N + 2*N**2 ). */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal sizes of the WORK and IWORK */
/*          arrays, returns these values as the first entries of the WORK */
/*          and IWORK arrays, and no error message related to LWORK or */
/*          LIWORK is issued by XERBLA. */

/*  IWORK   (workspace/output) INTEGER array, dimension (MAX(1,LIWORK)) */
/*          On exit, if INFO = 0, IWORK(1) returns the optimal LIWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array LIWORK. */
/*          If JOBZ  = 'N' or N <= 1, LIWORK must be at least 1. */
/*          If JOBZ  = 'V' and N > 2, LIWORK must be at least 3 + 5*N. */

/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates the optimal sizes of the WORK and */
/*          IWORK arrays, returns these values as the first entries of */
/*          the WORK and IWORK arrays, and no error message related to */
/*          LWORK or LIWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, the algorithm failed to converge; i */
/*                off-diagonal elements of an intermediate tridiagonal */
/*                form did not converge to zero. */

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
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;
    --iwork;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    lower = lsame_(uplo, "L");
    lquery = *lwork == -1 || *liwork == -1;

    *info = 0;
    if (*n <= 1) {
	liwmin = 1;
	lwmin = 1;
    } else {
	if (wantz) {
	    liwmin = *n * 5 + 3;
/* Computing 2nd power */
	    i__1 = *n;
	    lwmin = *n * 5 + 1 + (i__1 * i__1 << 1);
	} else {
	    liwmin = 1;
	    lwmin = *n << 1;
	}
    }
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (lower || lsame_(uplo, "U"))) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*kd < 0) {
	*info = -4;
    } else if (*ldab < *kd + 1) {
	*info = -6;
    } else if (*ldz < 1 || wantz && *ldz < *n) {
	*info = -9;
    }

    if (*info == 0) {
	work[1] = (double) lwmin;
	iwork[1] = liwmin;

	if (*lwork < lwmin && ! lquery) {
	    *info = -11;
	} else if (*liwork < liwmin && ! lquery) {
	    *info = -13;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSBEVD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	w[1] = ab[ab_dim1 + 1];
	if (wantz) {
	    z__[z_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
    rmax = sqrt(bignum);

/*     Scale matrix to allowable range, if necessary. */

    anrm = dlansb_("M", uplo, n, kd, &ab[ab_offset], ldab, &work[1]);
    iscale = 0;
    if (anrm > 0. && anrm < rmin) {
	iscale = 1;
	sigma = rmin / anrm;
    } else if (anrm > rmax) {
	iscale = 1;
	sigma = rmax / anrm;
    }
    if (iscale == 1) {
	if (lower) {
	    dlascl_("B", kd, kd, &c_b11, &sigma, n, n, &ab[ab_offset], ldab,
		    info);
	} else {
	    dlascl_("Q", kd, kd, &c_b11, &sigma, n, n, &ab[ab_offset], ldab,
		    info);
	}
    }

/*     Call DSBTRD to reduce symmetric band matrix to tridiagonal form. */

    inde = 1;
    indwrk = inde + *n;
    indwk2 = indwrk + *n * *n;
    llwrk2 = *lwork - indwk2 + 1;
    dsbtrd_(jobz, uplo, n, kd, &ab[ab_offset], ldab, &w[1], &work[inde], &z__[
	    z_offset], ldz, &work[indwrk], &iinfo);

/*     For eigenvalues only, call DSTERF.  For eigenvectors, call SSTEDC. */

    if (! wantz) {
	dsterf_(n, &w[1], &work[inde], info);
    } else {
	dstedc_("I", n, &w[1], &work[inde], &work[indwrk], n, &work[indwk2], &
		llwrk2, &iwork[1], liwork, info);
	dgemm_("N", "N", n, n, n, &c_b11, &z__[z_offset], ldz, &work[indwrk],
		n, &c_b18, &work[indwk2], n);
	dlacpy_("A", n, n, &work[indwk2], n, &z__[z_offset], ldz);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

    if (iscale == 1) {
	d__1 = 1. / sigma;
	dscal_(n, &d__1, &w[1], &c__1);
    }

    work[1] = (double) lwmin;
    iwork[1] = liwmin;
    return 0;

/*     End of DSBEVD */

} /* dsbevd_ */

/* Subroutine */ int dsbevx_(const char *jobz, const char *range, const char *uplo, integer *n,
	integer *kd, double *ab, integer *ldab, double *q, integer *
	ldq, double *vl, double *vu, integer *il, integer *iu,
	double *abstol, integer *m, double *w, double *z__,
	integer *ldz, double *work, integer *iwork, integer *ifail,
	integer *info)
{
	/* Table of constant values */
	static double c_b14 = 1.;
	static integer c__1 = 1;
	static double c_b34 = 0.;

    /* System generated locals */
    integer ab_dim1, ab_offset, q_dim1, q_offset, z_dim1, z_offset, i__1,
	    i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j, jj;
    double eps, vll, vuu, tmp1;
    integer indd, inde;
    double anrm;
    integer imax;
    double rmin, rmax;
    bool test;
    integer itmp1, indee;
    double sigma;
	integer iinfo;
    char order[1];
    bool lower, wantz;
    bool alleig, indeig;
    integer iscale, indibl;
    bool valeig;
    double safmin;
    double abstll, bignum;
    integer indisp;
    integer indiwo;
    integer indwrk;
    integer nsplit;
    double smlnum;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSBEVX computes selected eigenvalues and, optionally, eigenvectors */
/*  of a real symmetric band matrix A.  Eigenvalues and eigenvectors can */
/*  be selected by specifying either a range of values or a range of */
/*  indices for the desired eigenvalues. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  RANGE   (input) CHARACTER*1 */
/*          = 'A': all eigenvalues will be found; */
/*          = 'V': all eigenvalues in the half-open interval (VL,VU] */
/*                 will be found; */
/*          = 'I': the IL-th through IU-th eigenvalues will be found. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  KD      (input) INTEGER */
/*          The number of superdiagonals of the matrix A if UPLO = 'U', */
/*          or the number of subdiagonals if UPLO = 'L'.  KD >= 0. */

/*  AB      (input/output) DOUBLE PRECISION array, dimension (LDAB, N) */
/*          On entry, the upper or lower triangle of the symmetric band */
/*          matrix A, stored in the first KD+1 rows of the array.  The */
/*          j-th column of A is stored in the j-th column of the array AB */
/*          as follows: */
/*          if UPLO = 'U', AB(kd+1+i-j,j) = A(i,j) for max(1,j-kd)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+kd). */

/*          On exit, AB is overwritten by values generated during the */
/*          reduction to tridiagonal form.  If UPLO = 'U', the first */
/*          superdiagonal and the diagonal of the tridiagonal matrix T */
/*          are returned in rows KD and KD+1 of AB, and if UPLO = 'L', */
/*          the diagonal and first subdiagonal of T are returned in the */
/*          first two rows of AB. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KD + 1. */

/*  Q       (output) DOUBLE PRECISION array, dimension (LDQ, N) */
/*          If JOBZ = 'V', the N-by-N orthogonal matrix used in the */
/*                         reduction to tridiagonal form. */
/*          If JOBZ = 'N', the array Q is not referenced. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q.  If JOBZ = 'V', then */
/*          LDQ >= max(1,N). */

/*  VL      (input) DOUBLE PRECISION */
/*  VU      (input) DOUBLE PRECISION */
/*          If RANGE='V', the lower and upper bounds of the interval to */
/*          be searched for eigenvalues. VL < VU. */
/*          Not referenced if RANGE = 'A' or 'I'. */

/*  IL      (input) INTEGER */
/*  IU      (input) INTEGER */
/*          If RANGE='I', the indices (in ascending order) of the */
/*          smallest and largest eigenvalues to be returned. */
/*          1 <= IL <= IU <= N, if N > 0; IL = 1 and IU = 0 if N = 0. */
/*          Not referenced if RANGE = 'A' or 'V'. */

/*  ABSTOL  (input) DOUBLE PRECISION */
/*          The absolute error tolerance for the eigenvalues. */
/*          An approximate eigenvalue is accepted as converged */
/*          when it is determined to lie in an interval [a,b] */
/*          of width less than or equal to */

/*                  ABSTOL + EPS *   max( |a|,|b| ) , */

/*          where EPS is the machine precision.  If ABSTOL is less than */
/*          or equal to zero, then  EPS*|T|  will be used in its place, */
/*          where |T| is the 1-norm of the tridiagonal matrix obtained */
/*          by reducing AB to tridiagonal form. */

/*          Eigenvalues will be computed most accurately when ABSTOL is */
/*          set to twice the underflow threshold 2*DLAMCH('S'), not zero. */
/*          If this routine returns with INFO>0, indicating that some */
/*          eigenvectors did not converge, try setting ABSTOL to */
/*          2*DLAMCH('S'). */

/*          See "Computing Small Singular Values of Bidiagonal Matrices */
/*          with Guaranteed High Relative Accuracy," by Demmel and */
/*          Kahan, LAPACK Working Note #3. */

/*  M       (output) INTEGER */
/*          The total number of eigenvalues found.  0 <= M <= N. */
/*          If RANGE = 'A', M = N, and if RANGE = 'I', M = IU-IL+1. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          The first M elements contain the selected eigenvalues in */
/*          ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, max(1,M)) */
/*          If JOBZ = 'V', then if INFO = 0, the first M columns of Z */
/*          contain the orthonormal eigenvectors of the matrix A */
/*          corresponding to the selected eigenvalues, with the i-th */
/*          column of Z holding the eigenvector associated with W(i). */
/*          If an eigenvector fails to converge, then that column of Z */
/*          contains the latest approximation to the eigenvector, and the */
/*          index of the eigenvector is returned in IFAIL. */
/*          If JOBZ = 'N', then Z is not referenced. */
/*          Note: the user must ensure that at least max(1,M) columns are */
/*          supplied in the array Z; if RANGE = 'V', the exact value of M */
/*          is not known in advance and an upper bound must be used. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (7*N) */

/*  IWORK   (workspace) INTEGER array, dimension (5*N) */

/*  IFAIL   (output) INTEGER array, dimension (N) */
/*          If JOBZ = 'V', then if INFO = 0, the first M elements of */
/*          IFAIL are zero.  If INFO > 0, then IFAIL contains the */
/*          indices of the eigenvectors that failed to converge. */
/*          If JOBZ = 'N', then IFAIL is not referenced. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = i, then i eigenvectors failed to converge. */
/*                Their indices are stored in array IFAIL. */

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
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;
    --iwork;
    --ifail;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    alleig = lsame_(range, "A");
    valeig = lsame_(range, "V");
    indeig = lsame_(range, "I");
    lower = lsame_(uplo, "L");

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (alleig || valeig || indeig)) {
	*info = -2;
    } else if (! (lower || lsame_(uplo, "U"))) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*kd < 0) {
	*info = -5;
    } else if (*ldab < *kd + 1) {
	*info = -7;
    } else if (wantz && *ldq < std::max(1_integer,*n)) {
	*info = -9;
    } else {
	if (valeig) {
	    if (*n > 0 && *vu <= *vl) {
		*info = -11;
	    }
	} else if (indeig) {
	    if (*il < 1 || *il > std::max(1_integer,*n)) {
		*info = -12;
	    } else if (*iu < std::min(*n,*il) || *iu > *n) {
		*info = -13;
	    }
	}
    }
    if (*info == 0) {
	if (*ldz < 1 || wantz && *ldz < *n) {
	    *info = -18;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSBEVX", &i__1);
	return 0;
    }

/*     Quick return if possible */

    *m = 0;
    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	*m = 1;
	if (lower) {
	    tmp1 = ab[ab_dim1 + 1];
	} else {
	    tmp1 = ab[*kd + 1 + ab_dim1];
	}
	if (valeig) {
	    if (! (*vl < tmp1 && *vu >= tmp1)) {
		*m = 0;
	    }
	}
	if (*m == 1) {
	    w[1] = tmp1;
	    if (wantz) {
		z__[z_dim1 + 1] = 1.;
	    }
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
/* Computing MIN */
    d__1 = sqrt(bignum), d__2 = 1. / sqrt(sqrt(safmin));
    rmax = std::min(d__1,d__2);

/*     Scale matrix to allowable range, if necessary. */

    iscale = 0;
    abstll = *abstol;
    if (valeig) {
	vll = *vl;
	vuu = *vu;
    } else {
	vll = 0.;
	vuu = 0.;
    }
    anrm = dlansb_("M", uplo, n, kd, &ab[ab_offset], ldab, &work[1]);
    if (anrm > 0. && anrm < rmin) {
	iscale = 1;
	sigma = rmin / anrm;
    } else if (anrm > rmax) {
	iscale = 1;
	sigma = rmax / anrm;
    }
    if (iscale == 1) {
	if (lower) {
	    dlascl_("B", kd, kd, &c_b14, &sigma, n, n, &ab[ab_offset], ldab,
		    info);
	} else {
	    dlascl_("Q", kd, kd, &c_b14, &sigma, n, n, &ab[ab_offset], ldab,
		    info);
	}
	if (*abstol > 0.) {
	    abstll = *abstol * sigma;
	}
	if (valeig) {
	    vll = *vl * sigma;
	    vuu = *vu * sigma;
	}
    }

/*     Call DSBTRD to reduce symmetric band matrix to tridiagonal form. */

    indd = 1;
    inde = indd + *n;
    indwrk = inde + *n;
    dsbtrd_(jobz, uplo, n, kd, &ab[ab_offset], ldab, &work[indd], &work[inde],
	     &q[q_offset], ldq, &work[indwrk], &iinfo);

/*     If all eigenvalues are desired and ABSTOL is less than or equal */
/*     to zero, then call DSTERF or SSTEQR.  If this fails for some */
/*     eigenvalue, then try DSTEBZ. */

    test = false;
    if (indeig) {
	if (*il == 1 && *iu == *n) {
	    test = true;
	}
    }
    if ((alleig || test) && *abstol <= 0.) {
	dcopy_(n, &work[indd], &c__1, &w[1], &c__1);
	indee = indwrk + (*n << 1);
	if (! wantz) {
	    i__1 = *n - 1;
	    dcopy_(&i__1, &work[inde], &c__1, &work[indee], &c__1);
	    dsterf_(n, &w[1], &work[indee], info);
	} else {
	    dlacpy_("A", n, n, &q[q_offset], ldq, &z__[z_offset], ldz);
	    i__1 = *n - 1;
	    dcopy_(&i__1, &work[inde], &c__1, &work[indee], &c__1);
	    dsteqr_(jobz, n, &w[1], &work[indee], &z__[z_offset], ldz, &work[
		    indwrk], info);
	    if (*info == 0) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    ifail[i__] = 0;
/* L10: */
		}
	    }
	}
	if (*info == 0) {
	    *m = *n;
	    goto L30;
	}
	*info = 0;
    }

/*     Otherwise, call DSTEBZ and, if eigenvectors are desired, SSTEIN. */

    if (wantz) {
	*(unsigned char *)order = 'B';
    } else {
	*(unsigned char *)order = 'E';
    }
    indibl = 1;
    indisp = indibl + *n;
    indiwo = indisp + *n;
    dstebz_(range, order, n, &vll, &vuu, il, iu, &abstll, &work[indd], &work[
	    inde], m, &nsplit, &w[1], &iwork[indibl], &iwork[indisp], &work[
	    indwrk], &iwork[indiwo], info);

    if (wantz) {
	dstein_(n, &work[indd], &work[inde], m, &w[1], &iwork[indibl], &iwork[
		indisp], &z__[z_offset], ldz, &work[indwrk], &iwork[indiwo], &
		ifail[1], info);

/*        Apply orthogonal matrix used in reduction to tridiagonal */
/*        form to eigenvectors returned by DSTEIN. */

	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    dcopy_(n, &z__[j * z_dim1 + 1], &c__1, &work[1], &c__1);
	    dgemv_("N", n, n, &c_b14, &q[q_offset], ldq, &work[1], &c__1, &
		    c_b34, &z__[j * z_dim1 + 1], &c__1);
/* L20: */
	}
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

L30:
    if (iscale == 1) {
	if (*info == 0) {
	    imax = *m;
	} else {
	    imax = *info - 1;
	}
	d__1 = 1. / sigma;
	dscal_(&imax, &d__1, &w[1], &c__1);
    }

/*     If eigenvalues are not in order, then sort them, along with */
/*     eigenvectors. */

    if (wantz) {
	i__1 = *m - 1;
	for (j = 1; j <= i__1; ++j) {
	    i__ = 0;
	    tmp1 = w[j];
	    i__2 = *m;
	    for (jj = j + 1; jj <= i__2; ++jj) {
		if (w[jj] < tmp1) {
		    i__ = jj;
		    tmp1 = w[jj];
		}
/* L40: */
	    }

	    if (i__ != 0) {
		itmp1 = iwork[indibl + i__ - 1];
		w[i__] = w[j];
		iwork[indibl + i__ - 1] = iwork[indibl + j - 1];
		w[j] = tmp1;
		iwork[indibl + j - 1] = itmp1;
		dswap_(n, &z__[i__ * z_dim1 + 1], &c__1, &z__[j * z_dim1 + 1],
			 &c__1);
		if (*info != 0) {
		    itmp1 = ifail[i__];
		    ifail[i__] = ifail[j];
		    ifail[j] = itmp1;
		}
	    }
/* L50: */
	}
    }

    return 0;

/*     End of DSBEVX */

} /* dsbevx_ */

/* Subroutine */ int dsbgst_(const char *vect, const char *uplo, integer *n, integer *ka,
	integer *kb, double *ab, integer *ldab, double *bb, integer *
	ldbb, double *x, integer *ldx, double *work, integer *info)
{
	/* Table of constant values */
	static double c_b8 = 0.;
	static double c_b9 = 1.;
	static integer c__1 = 1;
	static double c_b20 = -1.;

    /* System generated locals */
    integer ab_dim1, ab_offset, bb_dim1, bb_offset, x_dim1, x_offset, i__1,
	    i__2, i__3, i__4;
    double d__1;

    /* Local variables */
    integer i__, j, k, l, m;
    double t;
    integer i0, i1, i2, j1, j2;
    double ra;
    integer nr, nx, ka1, kb1;
    double ra1;
    integer j1t, j2t;
    double bii;
    integer kbt, nrt, inca;
    bool upper, wantx;
    bool update;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSBGST reduces a real symmetric-definite banded generalized */
/*  eigenproblem  A*x = lambda*B*x  to standard form  C*y = lambda*y, */
/*  such that C has the same bandwidth as A. */

/*  B must have been previously factorized as S**T*S by DPBSTF, using a */
/*  split Cholesky factorization. A is overwritten by C = X**T*A*X, where */
/*  X = S**(-1)*Q and Q is an orthogonal matrix chosen to preserve the */
/*  bandwidth of A. */

/*  Arguments */
/*  ========= */

/*  VECT    (input) CHARACTER*1 */
/*          = 'N':  do not form the transformation matrix X; */
/*          = 'V':  form X. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B.  N >= 0. */

/*  KA      (input) INTEGER */
/*          The number of superdiagonals of the matrix A if UPLO = 'U', */
/*          or the number of subdiagonals if UPLO = 'L'.  KA >= 0. */

/*  KB      (input) INTEGER */
/*          The number of superdiagonals of the matrix B if UPLO = 'U', */
/*          or the number of subdiagonals if UPLO = 'L'.  KA >= KB >= 0. */

/*  AB      (input/output) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          On entry, the upper or lower triangle of the symmetric band */
/*          matrix A, stored in the first ka+1 rows of the array.  The */
/*          j-th column of A is stored in the j-th column of the array AB */
/*          as follows: */
/*          if UPLO = 'U', AB(ka+1+i-j,j) = A(i,j) for max(1,j-ka)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+ka). */

/*          On exit, the transformed matrix X**T*A*X, stored in the same */
/*          format as A. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KA+1. */

/*  BB      (input) DOUBLE PRECISION array, dimension (LDBB,N) */
/*          The banded factor S from the split Cholesky factorization of */
/*          B, as returned by DPBSTF, stored in the first KB+1 rows of */
/*          the array. */

/*  LDBB    (input) INTEGER */
/*          The leading dimension of the array BB.  LDBB >= KB+1. */

/*  X       (output) DOUBLE PRECISION array, dimension (LDX,N) */
/*          If VECT = 'V', the n-by-n matrix X. */
/*          If VECT = 'N', the array X is not referenced. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X. */
/*          LDX >= max(1,N) if VECT = 'V'; LDX >= 1 otherwise. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (2*N) */

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

/*     Test the input parameters */

    /* Parameter adjustments */
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    bb_dim1 = *ldbb;
    bb_offset = 1 + bb_dim1;
    bb -= bb_offset;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    --work;

    /* Function Body */
    wantx = lsame_(vect, "V");
    upper = lsame_(uplo, "U");
    ka1 = *ka + 1;
    kb1 = *kb + 1;
    *info = 0;
    if (! wantx && ! lsame_(vect, "N")) {
	*info = -1;
    } else if (! upper && ! lsame_(uplo, "L")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ka < 0) {
	*info = -4;
    } else if (*kb < 0 || *kb > *ka) {
	*info = -5;
    } else if (*ldab < *ka + 1) {
	*info = -7;
    } else if (*ldbb < *kb + 1) {
	*info = -9;
    } else if (*ldx < 1 || wantx && *ldx < std::max(1_integer,*n)) {
	*info = -11;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSBGST", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    inca = *ldab * ka1;

/*     Initialize X to the unit matrix, if needed */

    if (wantx) {
	dlaset_("Full", n, n, &c_b8, &c_b9, &x[x_offset], ldx);
    }

/*     Set M to the splitting point m. It must be the same value as is */
/*     used in DPBSTF. The chosen value allows the arrays WORK and RWORK */
/*     to be of dimension (N). */

    m = (*n + *kb) / 2;

/*     The routine works in two phases, corresponding to the two halves */
/*     of the split Cholesky factorization of B as S**T*S where */

/*     S = ( U    ) */
/*         ( M  L ) */

/*     with U upper triangular of order m, and L lower triangular of */
/*     order n-m. S has the same bandwidth as B. */

/*     S is treated as a product of elementary matrices: */

/*     S = S(m)*S(m-1)*...*S(2)*S(1)*S(m+1)*S(m+2)*...*S(n-1)*S(n) */

/*     where S(i) is determined by the i-th row of S. */

/*     In phase 1, the index i takes the values n, n-1, ... , m+1; */
/*     in phase 2, it takes the values 1, 2, ... , m. */

/*     For each value of i, the current matrix A is updated by forming */
/*     inv(S(i))**T*A*inv(S(i)). This creates a triangular bulge outside */
/*     the band of A. The bulge is then pushed down toward the bottom of */
/*     A in phase 1, and up toward the top of A in phase 2, by applying */
/*     plane rotations. */

/*     There are kb*(kb+1)/2 elements in the bulge, but at most 2*kb-1 */
/*     of them are linearly independent, so annihilating a bulge requires */
/*     only 2*kb-1 plane rotations. The rotations are divided into a 1st */
/*     set of kb-1 rotations, and a 2nd set of kb rotations. */

/*     Wherever possible, rotations are generated and applied in vector */
/*     operations of length NR between the indices J1 and J2 (sometimes */
/*     replaced by modified values NRT, J1T or J2T). */

/*     The cosines and sines of the rotations are stored in the array */
/*     WORK. The cosines of the 1st set of rotations are stored in */
/*     elements n+2:n+m-kb-1 and the sines of the 1st set in elements */
/*     2:m-kb-1; the cosines of the 2nd set are stored in elements */
/*     n+m-kb+1:2*n and the sines of the second set in elements m-kb+1:n. */

/*     The bulges are not formed explicitly; nonzero elements outside the */
/*     band are created only when they are required for generating new */
/*     rotations; they are stored in the array WORK, in positions where */
/*     they are later overwritten by the sines of the rotations which */
/*     annihilate them. */

/*     **************************** Phase 1 ***************************** */

/*     The logical structure of this phase is: */

/*     UPDATE = .TRUE. */
/*     DO I = N, M + 1, -1 */
/*        use S(i) to update A and create a new bulge */
/*        apply rotations to push all bulges KA positions downward */
/*     END DO */
/*     UPDATE = .FALSE. */
/*     DO I = M + KA + 1, N - 1 */
/*        apply rotations to push all bulges KA positions downward */
/*     END DO */

/*     To avoid duplicating code, the two loops are merged. */

    update = true;
    i__ = *n + 1;
L10:
    if (update) {
	--i__;
/* Computing MIN */
	i__1 = *kb, i__2 = i__ - 1;
	kbt = std::min(i__1,i__2);
	i0 = i__ - 1;
/* Computing MIN */
	i__1 = *n, i__2 = i__ + *ka;
	i1 = std::min(i__1,i__2);
	i2 = i__ - kbt + ka1;
	if (i__ < m + 1) {
	    update = false;
	    ++i__;
	    i0 = m;
	    if (*ka == 0) {
		goto L480;
	    }
	    goto L10;
	}
    } else {
	i__ += *ka;
	if (i__ > *n - 1) {
	    goto L480;
	}
    }

    if (upper) {

/*        Transform A, working with the upper triangle */

	if (update) {

/*           Form  inv(S(i))**T * A * inv(S(i)) */

	    bii = bb[kb1 + i__ * bb_dim1];
	    i__1 = i1;
	    for (j = i__; j <= i__1; ++j) {
		ab[i__ - j + ka1 + j * ab_dim1] /= bii;
/* L20: */
	    }
/* Computing MAX */
	    i__1 = 1, i__2 = i__ - *ka;
	    i__3 = i__;
	    for (j = std::max(i__1,i__2); j <= i__3; ++j) {
		ab[j - i__ + ka1 + i__ * ab_dim1] /= bii;
/* L30: */
	    }
	    i__3 = i__ - 1;
	    for (k = i__ - kbt; k <= i__3; ++k) {
		i__1 = k;
		for (j = i__ - kbt; j <= i__1; ++j) {
		    ab[j - k + ka1 + k * ab_dim1] = ab[j - k + ka1 + k *
			    ab_dim1] - bb[j - i__ + kb1 + i__ * bb_dim1] * ab[
			    k - i__ + ka1 + i__ * ab_dim1] - bb[k - i__ + kb1
			    + i__ * bb_dim1] * ab[j - i__ + ka1 + i__ *
			    ab_dim1] + ab[ka1 + i__ * ab_dim1] * bb[j - i__ +
			    kb1 + i__ * bb_dim1] * bb[k - i__ + kb1 + i__ *
			    bb_dim1];
/* L40: */
		}
/* Computing MAX */
		i__1 = 1, i__2 = i__ - *ka;
		i__4 = i__ - kbt - 1;
		for (j = std::max(i__1,i__2); j <= i__4; ++j) {
		    ab[j - k + ka1 + k * ab_dim1] -= bb[k - i__ + kb1 + i__ *
			    bb_dim1] * ab[j - i__ + ka1 + i__ * ab_dim1];
/* L50: */
		}
/* L60: */
	    }
	    i__3 = i1;
	    for (j = i__; j <= i__3; ++j) {
/* Computing MAX */
		i__4 = j - *ka, i__1 = i__ - kbt;
		i__2 = i__ - 1;
		for (k = std::max(i__4,i__1); k <= i__2; ++k) {
		    ab[k - j + ka1 + j * ab_dim1] -= bb[k - i__ + kb1 + i__ *
			    bb_dim1] * ab[i__ - j + ka1 + j * ab_dim1];
/* L70: */
		}
/* L80: */
	    }

	    if (wantx) {

/*              post-multiply X by inv(S(i)) */

		i__3 = *n - m;
		d__1 = 1. / bii;
		dscal_(&i__3, &d__1, &x[m + 1 + i__ * x_dim1], &c__1);
		if (kbt > 0) {
		    i__3 = *n - m;
		    dger_(&i__3, &kbt, &c_b20, &x[m + 1 + i__ * x_dim1], &
			    c__1, &bb[kb1 - kbt + i__ * bb_dim1], &c__1, &x[m
			    + 1 + (i__ - kbt) * x_dim1], ldx);
		}
	    }

/*           store a(i,i1) in RA1 for use in next loop over K */

	    ra1 = ab[i__ - i1 + ka1 + i1 * ab_dim1];
	}

/*        Generate and apply vectors of rotations to chase all the */
/*        existing bulges KA positions down toward the bottom of the */
/*        band */

	i__3 = *kb - 1;
	for (k = 1; k <= i__3; ++k) {
	    if (update) {

/*              Determine the rotations which would annihilate the bulge */
/*              which has in theory just been created */

		if (i__ - k + *ka < *n && i__ - k > 1) {

/*                 generate rotation to annihilate a(i,i-k+ka+1) */

		    dlartg_(&ab[k + 1 + (i__ - k + *ka) * ab_dim1], &ra1, &
			    work[*n + i__ - k + *ka - m], &work[i__ - k + *ka
			    - m], &ra);

/*                 create nonzero element a(i-k,i-k+ka+1) outside the */
/*                 band and store it in WORK(i-k) */

		    t = -bb[kb1 - k + i__ * bb_dim1] * ra1;
		    work[i__ - k] = work[*n + i__ - k + *ka - m] * t - work[
			    i__ - k + *ka - m] * ab[(i__ - k + *ka) * ab_dim1
			    + 1];
		    ab[(i__ - k + *ka) * ab_dim1 + 1] = work[i__ - k + *ka -
			    m] * t + work[*n + i__ - k + *ka - m] * ab[(i__ -
			    k + *ka) * ab_dim1 + 1];
		    ra1 = ra;
		}
	    }
/* Computing MAX */
	    i__2 = 1, i__4 = k - i0 + 2;
	    j2 = i__ - k - 1 + std::max(i__2,i__4) * ka1;
	    nr = (*n - j2 + *ka) / ka1;
	    j1 = j2 + (nr - 1) * ka1;
	    if (update) {
/* Computing MAX */
		i__2 = j2, i__4 = i__ + (*ka << 1) - k + 1;
		j2t = std::max(i__2,i__4);
	    } else {
		j2t = j2;
	    }
	    nrt = (*n - j2t + *ka) / ka1;
	    i__2 = j1;
	    i__4 = ka1;
	    for (j = j2t; i__4 < 0 ? j >= i__2 : j <= i__2; j += i__4) {

/*              create nonzero element a(j-ka,j+1) outside the band */
/*              and store it in WORK(j-m) */

		work[j - m] *= ab[(j + 1) * ab_dim1 + 1];
		ab[(j + 1) * ab_dim1 + 1] = work[*n + j - m] * ab[(j + 1) *
			ab_dim1 + 1];
/* L90: */
	    }

/*           generate rotations in 1st set to annihilate elements which */
/*           have been created outside the band */

	    if (nrt > 0) {
		dlargv_(&nrt, &ab[j2t * ab_dim1 + 1], &inca, &work[j2t - m], &
			ka1, &work[*n + j2t - m], &ka1);
	    }
	    if (nr > 0) {

/*              apply rotations in 1st set from the right */

		i__4 = *ka - 1;
		for (l = 1; l <= i__4; ++l) {
		    dlartv_(&nr, &ab[ka1 - l + j2 * ab_dim1], &inca, &ab[*ka
			    - l + (j2 + 1) * ab_dim1], &inca, &work[*n + j2 -
			    m], &work[j2 - m], &ka1);
/* L100: */
		}

/*              apply rotations in 1st set from both sides to diagonal */
/*              blocks */

		dlar2v_(&nr, &ab[ka1 + j2 * ab_dim1], &ab[ka1 + (j2 + 1) *
			ab_dim1], &ab[*ka + (j2 + 1) * ab_dim1], &inca, &work[
			*n + j2 - m], &work[j2 - m], &ka1);

	    }

/*           start applying rotations in 1st set from the left */

	    i__4 = *kb - k + 1;
	    for (l = *ka - 1; l >= i__4; --l) {
		nrt = (*n - j2 + l) / ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[l + (j2 + ka1 - l) * ab_dim1], &inca, &
			    ab[l + 1 + (j2 + ka1 - l) * ab_dim1], &inca, &
			    work[*n + j2 - m], &work[j2 - m], &ka1);
		}
/* L110: */
	    }

	    if (wantx) {

/*              post-multiply X by product of rotations in 1st set */

		i__4 = j1;
		i__2 = ka1;
		for (j = j2; i__2 < 0 ? j >= i__4 : j <= i__4; j += i__2) {
		    i__1 = *n - m;
		    drot_(&i__1, &x[m + 1 + j * x_dim1], &c__1, &x[m + 1 + (j
			    + 1) * x_dim1], &c__1, &work[*n + j - m], &work[j
			    - m]);
/* L120: */
		}
	    }
/* L130: */
	}

	if (update) {
	    if (i2 <= *n && kbt > 0) {

/*              create nonzero element a(i-kbt,i-kbt+ka+1) outside the */
/*              band and store it in WORK(i-kbt) */

		work[i__ - kbt] = -bb[kb1 - kbt + i__ * bb_dim1] * ra1;
	    }
	}

	for (k = *kb; k >= 1; --k) {
	    if (update) {
/* Computing MAX */
		i__3 = 2, i__2 = k - i0 + 1;
		j2 = i__ - k - 1 + std::max(i__3,i__2) * ka1;
	    } else {
/* Computing MAX */
		i__3 = 1, i__2 = k - i0 + 1;
		j2 = i__ - k - 1 + std::max(i__3,i__2) * ka1;
	    }

/*           finish applying rotations in 2nd set from the left */

	    for (l = *kb - k; l >= 1; --l) {
		nrt = (*n - j2 + *ka + l) / ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[l + (j2 - l + 1) * ab_dim1], &inca, &ab[
			    l + 1 + (j2 - l + 1) * ab_dim1], &inca, &work[*n
			    + j2 - *ka], &work[j2 - *ka], &ka1);
		}
/* L140: */
	    }
	    nr = (*n - j2 + *ka) / ka1;
	    j1 = j2 + (nr - 1) * ka1;
	    i__3 = j2;
	    i__2 = -ka1;
	    for (j = j1; i__2 < 0 ? j >= i__3 : j <= i__3; j += i__2) {
		work[j] = work[j - *ka];
		work[*n + j] = work[*n + j - *ka];
/* L150: */
	    }
	    i__2 = j1;
	    i__3 = ka1;
	    for (j = j2; i__3 < 0 ? j >= i__2 : j <= i__2; j += i__3) {

/*              create nonzero element a(j-ka,j+1) outside the band */
/*              and store it in WORK(j) */

		work[j] *= ab[(j + 1) * ab_dim1 + 1];
		ab[(j + 1) * ab_dim1 + 1] = work[*n + j] * ab[(j + 1) *
			ab_dim1 + 1];
/* L160: */
	    }
	    if (update) {
		if (i__ - k < *n - *ka && k <= kbt) {
		    work[i__ - k + *ka] = work[i__ - k];
		}
	    }
/* L170: */
	}

	for (k = *kb; k >= 1; --k) {
/* Computing MAX */
	    i__3 = 1, i__2 = k - i0 + 1;
	    j2 = i__ - k - 1 + std::max(i__3,i__2) * ka1;
	    nr = (*n - j2 + *ka) / ka1;
	    j1 = j2 + (nr - 1) * ka1;
	    if (nr > 0) {

/*              generate rotations in 2nd set to annihilate elements */
/*              which have been created outside the band */

		dlargv_(&nr, &ab[j2 * ab_dim1 + 1], &inca, &work[j2], &ka1, &
			work[*n + j2], &ka1);

/*              apply rotations in 2nd set from the right */

		i__3 = *ka - 1;
		for (l = 1; l <= i__3; ++l) {
		    dlartv_(&nr, &ab[ka1 - l + j2 * ab_dim1], &inca, &ab[*ka
			    - l + (j2 + 1) * ab_dim1], &inca, &work[*n + j2],
			    &work[j2], &ka1);
/* L180: */
		}

/*              apply rotations in 2nd set from both sides to diagonal */
/*              blocks */

		dlar2v_(&nr, &ab[ka1 + j2 * ab_dim1], &ab[ka1 + (j2 + 1) *
			ab_dim1], &ab[*ka + (j2 + 1) * ab_dim1], &inca, &work[
			*n + j2], &work[j2], &ka1);

	    }

/*           start applying rotations in 2nd set from the left */

	    i__3 = *kb - k + 1;
	    for (l = *ka - 1; l >= i__3; --l) {
		nrt = (*n - j2 + l) / ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[l + (j2 + ka1 - l) * ab_dim1], &inca, &
			    ab[l + 1 + (j2 + ka1 - l) * ab_dim1], &inca, &
			    work[*n + j2], &work[j2], &ka1);
		}
/* L190: */
	    }

	    if (wantx) {

/*              post-multiply X by product of rotations in 2nd set */

		i__3 = j1;
		i__2 = ka1;
		for (j = j2; i__2 < 0 ? j >= i__3 : j <= i__3; j += i__2) {
		    i__4 = *n - m;
		    drot_(&i__4, &x[m + 1 + j * x_dim1], &c__1, &x[m + 1 + (j
			    + 1) * x_dim1], &c__1, &work[*n + j], &work[j]);
/* L200: */
		}
	    }
/* L210: */
	}

	i__2 = *kb - 1;
	for (k = 1; k <= i__2; ++k) {
/* Computing MAX */
	    i__3 = 1, i__4 = k - i0 + 2;
	    j2 = i__ - k - 1 + std::max(i__3,i__4) * ka1;

/*           finish applying rotations in 1st set from the left */

	    for (l = *kb - k; l >= 1; --l) {
		nrt = (*n - j2 + l) / ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[l + (j2 + ka1 - l) * ab_dim1], &inca, &
			    ab[l + 1 + (j2 + ka1 - l) * ab_dim1], &inca, &
			    work[*n + j2 - m], &work[j2 - m], &ka1);
		}
/* L220: */
	    }
/* L230: */
	}

	if (*kb > 1) {
	    i__2 = i__ - *kb + (*ka << 1) + 1;
	    for (j = *n - 1; j >= i__2; --j) {
		work[*n + j - m] = work[*n + j - *ka - m];
		work[j - m] = work[j - *ka - m];
/* L240: */
	    }
	}

    } else {

/*        Transform A, working with the lower triangle */

	if (update) {

/*           Form  inv(S(i))**T * A * inv(S(i)) */

	    bii = bb[i__ * bb_dim1 + 1];
	    i__2 = i1;
	    for (j = i__; j <= i__2; ++j) {
		ab[j - i__ + 1 + i__ * ab_dim1] /= bii;
/* L250: */
	    }
/* Computing MAX */
	    i__2 = 1, i__3 = i__ - *ka;
	    i__4 = i__;
	    for (j = std::max(i__2,i__3); j <= i__4; ++j) {
		ab[i__ - j + 1 + j * ab_dim1] /= bii;
/* L260: */
	    }
	    i__4 = i__ - 1;
	    for (k = i__ - kbt; k <= i__4; ++k) {
		i__2 = k;
		for (j = i__ - kbt; j <= i__2; ++j) {
		    ab[k - j + 1 + j * ab_dim1] = ab[k - j + 1 + j * ab_dim1]
			    - bb[i__ - j + 1 + j * bb_dim1] * ab[i__ - k + 1
			    + k * ab_dim1] - bb[i__ - k + 1 + k * bb_dim1] *
			    ab[i__ - j + 1 + j * ab_dim1] + ab[i__ * ab_dim1
			    + 1] * bb[i__ - j + 1 + j * bb_dim1] * bb[i__ - k
			    + 1 + k * bb_dim1];
/* L270: */
		}
/* Computing MAX */
		i__2 = 1, i__3 = i__ - *ka;
		i__1 = i__ - kbt - 1;
		for (j = std::max(i__2,i__3); j <= i__1; ++j) {
		    ab[k - j + 1 + j * ab_dim1] -= bb[i__ - k + 1 + k *
			    bb_dim1] * ab[i__ - j + 1 + j * ab_dim1];
/* L280: */
		}
/* L290: */
	    }
	    i__4 = i1;
	    for (j = i__; j <= i__4; ++j) {
/* Computing MAX */
		i__1 = j - *ka, i__2 = i__ - kbt;
		i__3 = i__ - 1;
		for (k = std::max(i__1,i__2); k <= i__3; ++k) {
		    ab[j - k + 1 + k * ab_dim1] -= bb[i__ - k + 1 + k *
			    bb_dim1] * ab[j - i__ + 1 + i__ * ab_dim1];
/* L300: */
		}
/* L310: */
	    }

	    if (wantx) {

/*              post-multiply X by inv(S(i)) */

		i__4 = *n - m;
		d__1 = 1. / bii;
		dscal_(&i__4, &d__1, &x[m + 1 + i__ * x_dim1], &c__1);
		if (kbt > 0) {
		    i__4 = *n - m;
		    i__3 = *ldbb - 1;
		    dger_(&i__4, &kbt, &c_b20, &x[m + 1 + i__ * x_dim1], &
			    c__1, &bb[kbt + 1 + (i__ - kbt) * bb_dim1], &i__3,
			     &x[m + 1 + (i__ - kbt) * x_dim1], ldx);
		}
	    }

/*           store a(i1,i) in RA1 for use in next loop over K */

	    ra1 = ab[i1 - i__ + 1 + i__ * ab_dim1];
	}

/*        Generate and apply vectors of rotations to chase all the */
/*        existing bulges KA positions down toward the bottom of the */
/*        band */

	i__4 = *kb - 1;
	for (k = 1; k <= i__4; ++k) {
	    if (update) {

/*              Determine the rotations which would annihilate the bulge */
/*              which has in theory just been created */

		if (i__ - k + *ka < *n && i__ - k > 1) {

/*                 generate rotation to annihilate a(i-k+ka+1,i) */

		    dlartg_(&ab[ka1 - k + i__ * ab_dim1], &ra1, &work[*n +
			    i__ - k + *ka - m], &work[i__ - k + *ka - m], &ra)
			    ;

/*                 create nonzero element a(i-k+ka+1,i-k) outside the */
/*                 band and store it in WORK(i-k) */

		    t = -bb[k + 1 + (i__ - k) * bb_dim1] * ra1;
		    work[i__ - k] = work[*n + i__ - k + *ka - m] * t - work[
			    i__ - k + *ka - m] * ab[ka1 + (i__ - k) * ab_dim1]
			    ;
		    ab[ka1 + (i__ - k) * ab_dim1] = work[i__ - k + *ka - m] *
			    t + work[*n + i__ - k + *ka - m] * ab[ka1 + (i__
			    - k) * ab_dim1];
		    ra1 = ra;
		}
	    }
/* Computing MAX */
	    i__3 = 1, i__1 = k - i0 + 2;
	    j2 = i__ - k - 1 + std::max(i__3,i__1) * ka1;
	    nr = (*n - j2 + *ka) / ka1;
	    j1 = j2 + (nr - 1) * ka1;
	    if (update) {
/* Computing MAX */
		i__3 = j2, i__1 = i__ + (*ka << 1) - k + 1;
		j2t = std::max(i__3,i__1);
	    } else {
		j2t = j2;
	    }
	    nrt = (*n - j2t + *ka) / ka1;
	    i__3 = j1;
	    i__1 = ka1;
	    for (j = j2t; i__1 < 0 ? j >= i__3 : j <= i__3; j += i__1) {

/*              create nonzero element a(j+1,j-ka) outside the band */
/*              and store it in WORK(j-m) */

		work[j - m] *= ab[ka1 + (j - *ka + 1) * ab_dim1];
		ab[ka1 + (j - *ka + 1) * ab_dim1] = work[*n + j - m] * ab[ka1
			+ (j - *ka + 1) * ab_dim1];
/* L320: */
	    }

/*           generate rotations in 1st set to annihilate elements which */
/*           have been created outside the band */

	    if (nrt > 0) {
		dlargv_(&nrt, &ab[ka1 + (j2t - *ka) * ab_dim1], &inca, &work[
			j2t - m], &ka1, &work[*n + j2t - m], &ka1);
	    }
	    if (nr > 0) {

/*              apply rotations in 1st set from the left */

		i__1 = *ka - 1;
		for (l = 1; l <= i__1; ++l) {
		    dlartv_(&nr, &ab[l + 1 + (j2 - l) * ab_dim1], &inca, &ab[
			    l + 2 + (j2 - l) * ab_dim1], &inca, &work[*n + j2
			    - m], &work[j2 - m], &ka1);
/* L330: */
		}

/*              apply rotations in 1st set from both sides to diagonal */
/*              blocks */

		dlar2v_(&nr, &ab[j2 * ab_dim1 + 1], &ab[(j2 + 1) * ab_dim1 +
			1], &ab[j2 * ab_dim1 + 2], &inca, &work[*n + j2 - m],
			&work[j2 - m], &ka1);

	    }

/*           start applying rotations in 1st set from the right */

	    i__1 = *kb - k + 1;
	    for (l = *ka - 1; l >= i__1; --l) {
		nrt = (*n - j2 + l) / ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[ka1 - l + 1 + j2 * ab_dim1], &inca, &ab[
			    ka1 - l + (j2 + 1) * ab_dim1], &inca, &work[*n +
			    j2 - m], &work[j2 - m], &ka1);
		}
/* L340: */
	    }

	    if (wantx) {

/*              post-multiply X by product of rotations in 1st set */

		i__1 = j1;
		i__3 = ka1;
		for (j = j2; i__3 < 0 ? j >= i__1 : j <= i__1; j += i__3) {
		    i__2 = *n - m;
		    drot_(&i__2, &x[m + 1 + j * x_dim1], &c__1, &x[m + 1 + (j
			    + 1) * x_dim1], &c__1, &work[*n + j - m], &work[j
			    - m]);
/* L350: */
		}
	    }
/* L360: */
	}

	if (update) {
	    if (i2 <= *n && kbt > 0) {

/*              create nonzero element a(i-kbt+ka+1,i-kbt) outside the */
/*              band and store it in WORK(i-kbt) */

		work[i__ - kbt] = -bb[kbt + 1 + (i__ - kbt) * bb_dim1] * ra1;
	    }
	}

	for (k = *kb; k >= 1; --k) {
	    if (update) {
/* Computing MAX */
		i__4 = 2, i__3 = k - i0 + 1;
		j2 = i__ - k - 1 + std::max(i__4,i__3) * ka1;
	    } else {
/* Computing MAX */
		i__4 = 1, i__3 = k - i0 + 1;
		j2 = i__ - k - 1 + std::max(i__4,i__3) * ka1;
	    }

/*           finish applying rotations in 2nd set from the right */

	    for (l = *kb - k; l >= 1; --l) {
		nrt = (*n - j2 + *ka + l) / ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[ka1 - l + 1 + (j2 - *ka) * ab_dim1], &
			    inca, &ab[ka1 - l + (j2 - *ka + 1) * ab_dim1], &
			    inca, &work[*n + j2 - *ka], &work[j2 - *ka], &ka1)
			    ;
		}
/* L370: */
	    }
	    nr = (*n - j2 + *ka) / ka1;
	    j1 = j2 + (nr - 1) * ka1;
	    i__4 = j2;
	    i__3 = -ka1;
	    for (j = j1; i__3 < 0 ? j >= i__4 : j <= i__4; j += i__3) {
		work[j] = work[j - *ka];
		work[*n + j] = work[*n + j - *ka];
/* L380: */
	    }
	    i__3 = j1;
	    i__4 = ka1;
	    for (j = j2; i__4 < 0 ? j >= i__3 : j <= i__3; j += i__4) {

/*              create nonzero element a(j+1,j-ka) outside the band */
/*              and store it in WORK(j) */

		work[j] *= ab[ka1 + (j - *ka + 1) * ab_dim1];
		ab[ka1 + (j - *ka + 1) * ab_dim1] = work[*n + j] * ab[ka1 + (
			j - *ka + 1) * ab_dim1];
/* L390: */
	    }
	    if (update) {
		if (i__ - k < *n - *ka && k <= kbt) {
		    work[i__ - k + *ka] = work[i__ - k];
		}
	    }
/* L400: */
	}

	for (k = *kb; k >= 1; --k) {
/* Computing MAX */
	    i__4 = 1, i__3 = k - i0 + 1;
	    j2 = i__ - k - 1 + std::max(i__4,i__3) * ka1;
	    nr = (*n - j2 + *ka) / ka1;
	    j1 = j2 + (nr - 1) * ka1;
	    if (nr > 0) {

/*              generate rotations in 2nd set to annihilate elements */
/*              which have been created outside the band */

		dlargv_(&nr, &ab[ka1 + (j2 - *ka) * ab_dim1], &inca, &work[j2]
, &ka1, &work[*n + j2], &ka1);

/*              apply rotations in 2nd set from the left */

		i__4 = *ka - 1;
		for (l = 1; l <= i__4; ++l) {
		    dlartv_(&nr, &ab[l + 1 + (j2 - l) * ab_dim1], &inca, &ab[
			    l + 2 + (j2 - l) * ab_dim1], &inca, &work[*n + j2]
, &work[j2], &ka1);
/* L410: */
		}

/*              apply rotations in 2nd set from both sides to diagonal */
/*              blocks */

		dlar2v_(&nr, &ab[j2 * ab_dim1 + 1], &ab[(j2 + 1) * ab_dim1 +
			1], &ab[j2 * ab_dim1 + 2], &inca, &work[*n + j2], &
			work[j2], &ka1);

	    }

/*           start applying rotations in 2nd set from the right */

	    i__4 = *kb - k + 1;
	    for (l = *ka - 1; l >= i__4; --l) {
		nrt = (*n - j2 + l) / ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[ka1 - l + 1 + j2 * ab_dim1], &inca, &ab[
			    ka1 - l + (j2 + 1) * ab_dim1], &inca, &work[*n +
			    j2], &work[j2], &ka1);
		}
/* L420: */
	    }

	    if (wantx) {

/*              post-multiply X by product of rotations in 2nd set */

		i__4 = j1;
		i__3 = ka1;
		for (j = j2; i__3 < 0 ? j >= i__4 : j <= i__4; j += i__3) {
		    i__1 = *n - m;
		    drot_(&i__1, &x[m + 1 + j * x_dim1], &c__1, &x[m + 1 + (j
			    + 1) * x_dim1], &c__1, &work[*n + j], &work[j]);
/* L430: */
		}
	    }
/* L440: */
	}

	i__3 = *kb - 1;
	for (k = 1; k <= i__3; ++k) {
/* Computing MAX */
	    i__4 = 1, i__1 = k - i0 + 2;
	    j2 = i__ - k - 1 + std::max(i__4,i__1) * ka1;

/*           finish applying rotations in 1st set from the right */

	    for (l = *kb - k; l >= 1; --l) {
		nrt = (*n - j2 + l) / ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[ka1 - l + 1 + j2 * ab_dim1], &inca, &ab[
			    ka1 - l + (j2 + 1) * ab_dim1], &inca, &work[*n +
			    j2 - m], &work[j2 - m], &ka1);
		}
/* L450: */
	    }
/* L460: */
	}

	if (*kb > 1) {
	    i__3 = i__ - *kb + (*ka << 1) + 1;
	    for (j = *n - 1; j >= i__3; --j) {
		work[*n + j - m] = work[*n + j - *ka - m];
		work[j - m] = work[j - *ka - m];
/* L470: */
	    }
	}

    }

    goto L10;

L480:

/*     **************************** Phase 2 ***************************** */

/*     The logical structure of this phase is: */

/*     UPDATE = .TRUE. */
/*     DO I = 1, M */
/*        use S(i) to update A and create a new bulge */
/*        apply rotations to push all bulges KA positions upward */
/*     END DO */
/*     UPDATE = .FALSE. */
/*     DO I = M - KA - 1, 2, -1 */
/*        apply rotations to push all bulges KA positions upward */
/*     END DO */

/*     To avoid duplicating code, the two loops are merged. */

    update = true;
    i__ = 0;
L490:
    if (update) {
	++i__;
/* Computing MIN */
	i__3 = *kb, i__4 = m - i__;
	kbt = std::min(i__3,i__4);
	i0 = i__ + 1;
/* Computing MAX */
	i__3 = 1, i__4 = i__ - *ka;
	i1 = std::max(i__3,i__4);
	i2 = i__ + kbt - ka1;
	if (i__ > m) {
	    update = false;
	    --i__;
	    i0 = m + 1;
	    if (*ka == 0) {
		return 0;
	    }
	    goto L490;
	}
    } else {
	i__ -= *ka;
	if (i__ < 2) {
	    return 0;
	}
    }

    if (i__ < m - kbt) {
	nx = m;
    } else {
	nx = *n;
    }

    if (upper) {

/*        Transform A, working with the upper triangle */

	if (update) {

/*           Form  inv(S(i))**T * A * inv(S(i)) */

	    bii = bb[kb1 + i__ * bb_dim1];
	    i__3 = i__;
	    for (j = i1; j <= i__3; ++j) {
		ab[j - i__ + ka1 + i__ * ab_dim1] /= bii;
/* L500: */
	    }
/* Computing MIN */
	    i__4 = *n, i__1 = i__ + *ka;
	    i__3 = std::min(i__4,i__1);
	    for (j = i__; j <= i__3; ++j) {
		ab[i__ - j + ka1 + j * ab_dim1] /= bii;
/* L510: */
	    }
	    i__3 = i__ + kbt;
	    for (k = i__ + 1; k <= i__3; ++k) {
		i__4 = i__ + kbt;
		for (j = k; j <= i__4; ++j) {
		    ab[k - j + ka1 + j * ab_dim1] = ab[k - j + ka1 + j *
			    ab_dim1] - bb[i__ - j + kb1 + j * bb_dim1] * ab[
			    i__ - k + ka1 + k * ab_dim1] - bb[i__ - k + kb1 +
			    k * bb_dim1] * ab[i__ - j + ka1 + j * ab_dim1] +
			    ab[ka1 + i__ * ab_dim1] * bb[i__ - j + kb1 + j *
			    bb_dim1] * bb[i__ - k + kb1 + k * bb_dim1];
/* L520: */
		}
/* Computing MIN */
		i__1 = *n, i__2 = i__ + *ka;
		i__4 = std::min(i__1,i__2);
		for (j = i__ + kbt + 1; j <= i__4; ++j) {
		    ab[k - j + ka1 + j * ab_dim1] -= bb[i__ - k + kb1 + k *
			    bb_dim1] * ab[i__ - j + ka1 + j * ab_dim1];
/* L530: */
		}
/* L540: */
	    }
	    i__3 = i__;
	    for (j = i1; j <= i__3; ++j) {
/* Computing MIN */
		i__1 = j + *ka, i__2 = i__ + kbt;
		i__4 = std::min(i__1,i__2);
		for (k = i__ + 1; k <= i__4; ++k) {
		    ab[j - k + ka1 + k * ab_dim1] -= bb[i__ - k + kb1 + k *
			    bb_dim1] * ab[j - i__ + ka1 + i__ * ab_dim1];
/* L550: */
		}
/* L560: */
	    }

	    if (wantx) {

/*              post-multiply X by inv(S(i)) */

		d__1 = 1. / bii;
		dscal_(&nx, &d__1, &x[i__ * x_dim1 + 1], &c__1);
		if (kbt > 0) {
		    i__3 = *ldbb - 1;
		    dger_(&nx, &kbt, &c_b20, &x[i__ * x_dim1 + 1], &c__1, &bb[
			    *kb + (i__ + 1) * bb_dim1], &i__3, &x[(i__ + 1) *
			    x_dim1 + 1], ldx);
		}
	    }

/*           store a(i1,i) in RA1 for use in next loop over K */

	    ra1 = ab[i1 - i__ + ka1 + i__ * ab_dim1];
	}

/*        Generate and apply vectors of rotations to chase all the */
/*        existing bulges KA positions up toward the top of the band */

	i__3 = *kb - 1;
	for (k = 1; k <= i__3; ++k) {
	    if (update) {

/*              Determine the rotations which would annihilate the bulge */
/*              which has in theory just been created */

		if (i__ + k - ka1 > 0 && i__ + k < m) {

/*                 generate rotation to annihilate a(i+k-ka-1,i) */

		    dlartg_(&ab[k + 1 + i__ * ab_dim1], &ra1, &work[*n + i__
			    + k - *ka], &work[i__ + k - *ka], &ra);

/*                 create nonzero element a(i+k-ka-1,i+k) outside the */
/*                 band and store it in WORK(m-kb+i+k) */

		    t = -bb[kb1 - k + (i__ + k) * bb_dim1] * ra1;
		    work[m - *kb + i__ + k] = work[*n + i__ + k - *ka] * t -
			    work[i__ + k - *ka] * ab[(i__ + k) * ab_dim1 + 1];
		    ab[(i__ + k) * ab_dim1 + 1] = work[i__ + k - *ka] * t +
			    work[*n + i__ + k - *ka] * ab[(i__ + k) * ab_dim1
			    + 1];
		    ra1 = ra;
		}
	    }
/* Computing MAX */
	    i__4 = 1, i__1 = k + i0 - m + 1;
	    j2 = i__ + k + 1 - std::max(i__4,i__1) * ka1;
	    nr = (j2 + *ka - 1) / ka1;
	    j1 = j2 - (nr - 1) * ka1;
	    if (update) {
/* Computing MIN */
		i__4 = j2, i__1 = i__ - (*ka << 1) + k - 1;
		j2t = std::min(i__4,i__1);
	    } else {
		j2t = j2;
	    }
	    nrt = (j2t + *ka - 1) / ka1;
	    i__4 = j2t;
	    i__1 = ka1;
	    for (j = j1; i__1 < 0 ? j >= i__4 : j <= i__4; j += i__1) {

/*              create nonzero element a(j-1,j+ka) outside the band */
/*              and store it in WORK(j) */

		work[j] *= ab[(j + *ka - 1) * ab_dim1 + 1];
		ab[(j + *ka - 1) * ab_dim1 + 1] = work[*n + j] * ab[(j + *ka
			- 1) * ab_dim1 + 1];
/* L570: */
	    }

/*           generate rotations in 1st set to annihilate elements which */
/*           have been created outside the band */

	    if (nrt > 0) {
		dlargv_(&nrt, &ab[(j1 + *ka) * ab_dim1 + 1], &inca, &work[j1],
			 &ka1, &work[*n + j1], &ka1);
	    }
	    if (nr > 0) {

/*              apply rotations in 1st set from the left */

		i__1 = *ka - 1;
		for (l = 1; l <= i__1; ++l) {
		    dlartv_(&nr, &ab[ka1 - l + (j1 + l) * ab_dim1], &inca, &
			    ab[*ka - l + (j1 + l) * ab_dim1], &inca, &work[*n
			    + j1], &work[j1], &ka1);
/* L580: */
		}

/*              apply rotations in 1st set from both sides to diagonal */
/*              blocks */

		dlar2v_(&nr, &ab[ka1 + j1 * ab_dim1], &ab[ka1 + (j1 - 1) *
			ab_dim1], &ab[*ka + j1 * ab_dim1], &inca, &work[*n +
			j1], &work[j1], &ka1);

	    }

/*           start applying rotations in 1st set from the right */

	    i__1 = *kb - k + 1;
	    for (l = *ka - 1; l >= i__1; --l) {
		nrt = (j2 + l - 1) / ka1;
		j1t = j2 - (nrt - 1) * ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[l + j1t * ab_dim1], &inca, &ab[l + 1 + (
			    j1t - 1) * ab_dim1], &inca, &work[*n + j1t], &
			    work[j1t], &ka1);
		}
/* L590: */
	    }

	    if (wantx) {

/*              post-multiply X by product of rotations in 1st set */

		i__1 = j2;
		i__4 = ka1;
		for (j = j1; i__4 < 0 ? j >= i__1 : j <= i__1; j += i__4) {
		    drot_(&nx, &x[j * x_dim1 + 1], &c__1, &x[(j - 1) * x_dim1
			    + 1], &c__1, &work[*n + j], &work[j]);
/* L600: */
		}
	    }
/* L610: */
	}

	if (update) {
	    if (i2 > 0 && kbt > 0) {

/*              create nonzero element a(i+kbt-ka-1,i+kbt) outside the */
/*              band and store it in WORK(m-kb+i+kbt) */

		work[m - *kb + i__ + kbt] = -bb[kb1 - kbt + (i__ + kbt) *
			bb_dim1] * ra1;
	    }
	}

	for (k = *kb; k >= 1; --k) {
	    if (update) {
/* Computing MAX */
		i__3 = 2, i__4 = k + i0 - m;
		j2 = i__ + k + 1 - std::max(i__3,i__4) * ka1;
	    } else {
/* Computing MAX */
		i__3 = 1, i__4 = k + i0 - m;
		j2 = i__ + k + 1 - std::max(i__3,i__4) * ka1;
	    }

/*           finish applying rotations in 2nd set from the right */

	    for (l = *kb - k; l >= 1; --l) {
		nrt = (j2 + *ka + l - 1) / ka1;
		j1t = j2 - (nrt - 1) * ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[l + (j1t + *ka) * ab_dim1], &inca, &ab[
			    l + 1 + (j1t + *ka - 1) * ab_dim1], &inca, &work[*
			    n + m - *kb + j1t + *ka], &work[m - *kb + j1t + *
			    ka], &ka1);
		}
/* L620: */
	    }
	    nr = (j2 + *ka - 1) / ka1;
	    j1 = j2 - (nr - 1) * ka1;
	    i__3 = j2;
	    i__4 = ka1;
	    for (j = j1; i__4 < 0 ? j >= i__3 : j <= i__3; j += i__4) {
		work[m - *kb + j] = work[m - *kb + j + *ka];
		work[*n + m - *kb + j] = work[*n + m - *kb + j + *ka];
/* L630: */
	    }
	    i__4 = j2;
	    i__3 = ka1;
	    for (j = j1; i__3 < 0 ? j >= i__4 : j <= i__4; j += i__3) {

/*              create nonzero element a(j-1,j+ka) outside the band */
/*              and store it in WORK(m-kb+j) */

		work[m - *kb + j] *= ab[(j + *ka - 1) * ab_dim1 + 1];
		ab[(j + *ka - 1) * ab_dim1 + 1] = work[*n + m - *kb + j] * ab[
			(j + *ka - 1) * ab_dim1 + 1];
/* L640: */
	    }
	    if (update) {
		if (i__ + k > ka1 && k <= kbt) {
		    work[m - *kb + i__ + k - *ka] = work[m - *kb + i__ + k];
		}
	    }
/* L650: */
	}

	for (k = *kb; k >= 1; --k) {
/* Computing MAX */
	    i__3 = 1, i__4 = k + i0 - m;
	    j2 = i__ + k + 1 - std::max(i__3,i__4) * ka1;
	    nr = (j2 + *ka - 1) / ka1;
	    j1 = j2 - (nr - 1) * ka1;
	    if (nr > 0) {

/*              generate rotations in 2nd set to annihilate elements */
/*              which have been created outside the band */

		dlargv_(&nr, &ab[(j1 + *ka) * ab_dim1 + 1], &inca, &work[m - *
			kb + j1], &ka1, &work[*n + m - *kb + j1], &ka1);

/*              apply rotations in 2nd set from the left */

		i__3 = *ka - 1;
		for (l = 1; l <= i__3; ++l) {
		    dlartv_(&nr, &ab[ka1 - l + (j1 + l) * ab_dim1], &inca, &
			    ab[*ka - l + (j1 + l) * ab_dim1], &inca, &work[*n
			    + m - *kb + j1], &work[m - *kb + j1], &ka1);
/* L660: */
		}

/*              apply rotations in 2nd set from both sides to diagonal */
/*              blocks */

		dlar2v_(&nr, &ab[ka1 + j1 * ab_dim1], &ab[ka1 + (j1 - 1) *
			ab_dim1], &ab[*ka + j1 * ab_dim1], &inca, &work[*n +
			m - *kb + j1], &work[m - *kb + j1], &ka1);

	    }

/*           start applying rotations in 2nd set from the right */

	    i__3 = *kb - k + 1;
	    for (l = *ka - 1; l >= i__3; --l) {
		nrt = (j2 + l - 1) / ka1;
		j1t = j2 - (nrt - 1) * ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[l + j1t * ab_dim1], &inca, &ab[l + 1 + (
			    j1t - 1) * ab_dim1], &inca, &work[*n + m - *kb +
			    j1t], &work[m - *kb + j1t], &ka1);
		}
/* L670: */
	    }

	    if (wantx) {

/*              post-multiply X by product of rotations in 2nd set */

		i__3 = j2;
		i__4 = ka1;
		for (j = j1; i__4 < 0 ? j >= i__3 : j <= i__3; j += i__4) {
		    drot_(&nx, &x[j * x_dim1 + 1], &c__1, &x[(j - 1) * x_dim1
			    + 1], &c__1, &work[*n + m - *kb + j], &work[m - *
			    kb + j]);
/* L680: */
		}
	    }
/* L690: */
	}

	i__4 = *kb - 1;
	for (k = 1; k <= i__4; ++k) {
/* Computing MAX */
	    i__3 = 1, i__1 = k + i0 - m + 1;
	    j2 = i__ + k + 1 - std::max(i__3,i__1) * ka1;

/*           finish applying rotations in 1st set from the right */

	    for (l = *kb - k; l >= 1; --l) {
		nrt = (j2 + l - 1) / ka1;
		j1t = j2 - (nrt - 1) * ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[l + j1t * ab_dim1], &inca, &ab[l + 1 + (
			    j1t - 1) * ab_dim1], &inca, &work[*n + j1t], &
			    work[j1t], &ka1);
		}
/* L700: */
	    }
/* L710: */
	}

	if (*kb > 1) {
/* Computing MIN */
	    i__3 = i__ + *kb;
	    i__4 = std::min(i__3,m) - (*ka << 1) - 1;
	    for (j = 2; j <= i__4; ++j) {
		work[*n + j] = work[*n + j + *ka];
		work[j] = work[j + *ka];
/* L720: */
	    }
	}

    } else {

/*        Transform A, working with the lower triangle */

	if (update) {

/*           Form  inv(S(i))**T * A * inv(S(i)) */

	    bii = bb[i__ * bb_dim1 + 1];
	    i__4 = i__;
	    for (j = i1; j <= i__4; ++j) {
		ab[i__ - j + 1 + j * ab_dim1] /= bii;
/* L730: */
	    }
/* Computing MIN */
	    i__3 = *n, i__1 = i__ + *ka;
	    i__4 = std::min(i__3,i__1);
	    for (j = i__; j <= i__4; ++j) {
		ab[j - i__ + 1 + i__ * ab_dim1] /= bii;
/* L740: */
	    }
	    i__4 = i__ + kbt;
	    for (k = i__ + 1; k <= i__4; ++k) {
		i__3 = i__ + kbt;
		for (j = k; j <= i__3; ++j) {
		    ab[j - k + 1 + k * ab_dim1] = ab[j - k + 1 + k * ab_dim1]
			    - bb[j - i__ + 1 + i__ * bb_dim1] * ab[k - i__ +
			    1 + i__ * ab_dim1] - bb[k - i__ + 1 + i__ *
			    bb_dim1] * ab[j - i__ + 1 + i__ * ab_dim1] + ab[
			    i__ * ab_dim1 + 1] * bb[j - i__ + 1 + i__ *
			    bb_dim1] * bb[k - i__ + 1 + i__ * bb_dim1];
/* L750: */
		}
/* Computing MIN */
		i__1 = *n, i__2 = i__ + *ka;
		i__3 = std::min(i__1,i__2);
		for (j = i__ + kbt + 1; j <= i__3; ++j) {
		    ab[j - k + 1 + k * ab_dim1] -= bb[k - i__ + 1 + i__ *
			    bb_dim1] * ab[j - i__ + 1 + i__ * ab_dim1];
/* L760: */
		}
/* L770: */
	    }
	    i__4 = i__;
	    for (j = i1; j <= i__4; ++j) {
/* Computing MIN */
		i__1 = j + *ka, i__2 = i__ + kbt;
		i__3 = std::min(i__1,i__2);
		for (k = i__ + 1; k <= i__3; ++k) {
		    ab[k - j + 1 + j * ab_dim1] -= bb[k - i__ + 1 + i__ *
			    bb_dim1] * ab[i__ - j + 1 + j * ab_dim1];
/* L780: */
		}
/* L790: */
	    }

	    if (wantx) {

/*              post-multiply X by inv(S(i)) */

		d__1 = 1. / bii;
		dscal_(&nx, &d__1, &x[i__ * x_dim1 + 1], &c__1);
		if (kbt > 0) {
		    dger_(&nx, &kbt, &c_b20, &x[i__ * x_dim1 + 1], &c__1, &bb[
			    i__ * bb_dim1 + 2], &c__1, &x[(i__ + 1) * x_dim1
			    + 1], ldx);
		}
	    }

/*           store a(i,i1) in RA1 for use in next loop over K */

	    ra1 = ab[i__ - i1 + 1 + i1 * ab_dim1];
	}

/*        Generate and apply vectors of rotations to chase all the */
/*        existing bulges KA positions up toward the top of the band */

	i__4 = *kb - 1;
	for (k = 1; k <= i__4; ++k) {
	    if (update) {

/*              Determine the rotations which would annihilate the bulge */
/*              which has in theory just been created */

		if (i__ + k - ka1 > 0 && i__ + k < m) {

/*                 generate rotation to annihilate a(i,i+k-ka-1) */

		    dlartg_(&ab[ka1 - k + (i__ + k - *ka) * ab_dim1], &ra1, &
			    work[*n + i__ + k - *ka], &work[i__ + k - *ka], &
			    ra);

/*                 create nonzero element a(i+k,i+k-ka-1) outside the */
/*                 band and store it in WORK(m-kb+i+k) */

		    t = -bb[k + 1 + i__ * bb_dim1] * ra1;
		    work[m - *kb + i__ + k] = work[*n + i__ + k - *ka] * t -
			    work[i__ + k - *ka] * ab[ka1 + (i__ + k - *ka) *
			    ab_dim1];
		    ab[ka1 + (i__ + k - *ka) * ab_dim1] = work[i__ + k - *ka]
			    * t + work[*n + i__ + k - *ka] * ab[ka1 + (i__ +
			    k - *ka) * ab_dim1];
		    ra1 = ra;
		}
	    }
/* Computing MAX */
	    i__3 = 1, i__1 = k + i0 - m + 1;
	    j2 = i__ + k + 1 - std::max(i__3,i__1) * ka1;
	    nr = (j2 + *ka - 1) / ka1;
	    j1 = j2 - (nr - 1) * ka1;
	    if (update) {
/* Computing MIN */
		i__3 = j2, i__1 = i__ - (*ka << 1) + k - 1;
		j2t = std::min(i__3,i__1);
	    } else {
		j2t = j2;
	    }
	    nrt = (j2t + *ka - 1) / ka1;
	    i__3 = j2t;
	    i__1 = ka1;
	    for (j = j1; i__1 < 0 ? j >= i__3 : j <= i__3; j += i__1) {

/*              create nonzero element a(j+ka,j-1) outside the band */
/*              and store it in WORK(j) */

		work[j] *= ab[ka1 + (j - 1) * ab_dim1];
		ab[ka1 + (j - 1) * ab_dim1] = work[*n + j] * ab[ka1 + (j - 1)
			* ab_dim1];
/* L800: */
	    }

/*           generate rotations in 1st set to annihilate elements which */
/*           have been created outside the band */

	    if (nrt > 0) {
		dlargv_(&nrt, &ab[ka1 + j1 * ab_dim1], &inca, &work[j1], &ka1,
			 &work[*n + j1], &ka1);
	    }
	    if (nr > 0) {

/*              apply rotations in 1st set from the right */

		i__1 = *ka - 1;
		for (l = 1; l <= i__1; ++l) {
		    dlartv_(&nr, &ab[l + 1 + j1 * ab_dim1], &inca, &ab[l + 2
			    + (j1 - 1) * ab_dim1], &inca, &work[*n + j1], &
			    work[j1], &ka1);
/* L810: */
		}

/*              apply rotations in 1st set from both sides to diagonal */
/*              blocks */

		dlar2v_(&nr, &ab[j1 * ab_dim1 + 1], &ab[(j1 - 1) * ab_dim1 +
			1], &ab[(j1 - 1) * ab_dim1 + 2], &inca, &work[*n + j1]
, &work[j1], &ka1);

	    }

/*           start applying rotations in 1st set from the left */

	    i__1 = *kb - k + 1;
	    for (l = *ka - 1; l >= i__1; --l) {
		nrt = (j2 + l - 1) / ka1;
		j1t = j2 - (nrt - 1) * ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[ka1 - l + 1 + (j1t - ka1 + l) * ab_dim1]
, &inca, &ab[ka1 - l + (j1t - ka1 + l) * ab_dim1],
			     &inca, &work[*n + j1t], &work[j1t], &ka1);
		}
/* L820: */
	    }

	    if (wantx) {

/*              post-multiply X by product of rotations in 1st set */

		i__1 = j2;
		i__3 = ka1;
		for (j = j1; i__3 < 0 ? j >= i__1 : j <= i__1; j += i__3) {
		    drot_(&nx, &x[j * x_dim1 + 1], &c__1, &x[(j - 1) * x_dim1
			    + 1], &c__1, &work[*n + j], &work[j]);
/* L830: */
		}
	    }
/* L840: */
	}

	if (update) {
	    if (i2 > 0 && kbt > 0) {

/*              create nonzero element a(i+kbt,i+kbt-ka-1) outside the */
/*              band and store it in WORK(m-kb+i+kbt) */

		work[m - *kb + i__ + kbt] = -bb[kbt + 1 + i__ * bb_dim1] *
			ra1;
	    }
	}

	for (k = *kb; k >= 1; --k) {
	    if (update) {
/* Computing MAX */
		i__4 = 2, i__3 = k + i0 - m;
		j2 = i__ + k + 1 - std::max(i__4,i__3) * ka1;
	    } else {
/* Computing MAX */
		i__4 = 1, i__3 = k + i0 - m;
		j2 = i__ + k + 1 - std::max(i__4,i__3) * ka1;
	    }

/*           finish applying rotations in 2nd set from the left */

	    for (l = *kb - k; l >= 1; --l) {
		nrt = (j2 + *ka + l - 1) / ka1;
		j1t = j2 - (nrt - 1) * ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[ka1 - l + 1 + (j1t + l - 1) * ab_dim1],
			    &inca, &ab[ka1 - l + (j1t + l - 1) * ab_dim1], &
			    inca, &work[*n + m - *kb + j1t + *ka], &work[m - *
			    kb + j1t + *ka], &ka1);
		}
/* L850: */
	    }
	    nr = (j2 + *ka - 1) / ka1;
	    j1 = j2 - (nr - 1) * ka1;
	    i__4 = j2;
	    i__3 = ka1;
	    for (j = j1; i__3 < 0 ? j >= i__4 : j <= i__4; j += i__3) {
		work[m - *kb + j] = work[m - *kb + j + *ka];
		work[*n + m - *kb + j] = work[*n + m - *kb + j + *ka];
/* L860: */
	    }
	    i__3 = j2;
	    i__4 = ka1;
	    for (j = j1; i__4 < 0 ? j >= i__3 : j <= i__3; j += i__4) {

/*              create nonzero element a(j+ka,j-1) outside the band */
/*              and store it in WORK(m-kb+j) */

		work[m - *kb + j] *= ab[ka1 + (j - 1) * ab_dim1];
		ab[ka1 + (j - 1) * ab_dim1] = work[*n + m - *kb + j] * ab[ka1
			+ (j - 1) * ab_dim1];
/* L870: */
	    }
	    if (update) {
		if (i__ + k > ka1 && k <= kbt) {
		    work[m - *kb + i__ + k - *ka] = work[m - *kb + i__ + k];
		}
	    }
/* L880: */
	}

	for (k = *kb; k >= 1; --k) {
/* Computing MAX */
	    i__4 = 1, i__3 = k + i0 - m;
	    j2 = i__ + k + 1 - std::max(i__4,i__3) * ka1;
	    nr = (j2 + *ka - 1) / ka1;
	    j1 = j2 - (nr - 1) * ka1;
	    if (nr > 0) {

/*              generate rotations in 2nd set to annihilate elements */
/*              which have been created outside the band */

		dlargv_(&nr, &ab[ka1 + j1 * ab_dim1], &inca, &work[m - *kb +
			j1], &ka1, &work[*n + m - *kb + j1], &ka1);

/*              apply rotations in 2nd set from the right */

		i__4 = *ka - 1;
		for (l = 1; l <= i__4; ++l) {
		    dlartv_(&nr, &ab[l + 1 + j1 * ab_dim1], &inca, &ab[l + 2
			    + (j1 - 1) * ab_dim1], &inca, &work[*n + m - *kb
			    + j1], &work[m - *kb + j1], &ka1);
/* L890: */
		}

/*              apply rotations in 2nd set from both sides to diagonal */
/*              blocks */

		dlar2v_(&nr, &ab[j1 * ab_dim1 + 1], &ab[(j1 - 1) * ab_dim1 +
			1], &ab[(j1 - 1) * ab_dim1 + 2], &inca, &work[*n + m
			- *kb + j1], &work[m - *kb + j1], &ka1);

	    }

/*           start applying rotations in 2nd set from the left */

	    i__4 = *kb - k + 1;
	    for (l = *ka - 1; l >= i__4; --l) {
		nrt = (j2 + l - 1) / ka1;
		j1t = j2 - (nrt - 1) * ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[ka1 - l + 1 + (j1t - ka1 + l) * ab_dim1]
, &inca, &ab[ka1 - l + (j1t - ka1 + l) * ab_dim1],
			     &inca, &work[*n + m - *kb + j1t], &work[m - *kb
			    + j1t], &ka1);
		}
/* L900: */
	    }

	    if (wantx) {

/*              post-multiply X by product of rotations in 2nd set */

		i__4 = j2;
		i__3 = ka1;
		for (j = j1; i__3 < 0 ? j >= i__4 : j <= i__4; j += i__3) {
		    drot_(&nx, &x[j * x_dim1 + 1], &c__1, &x[(j - 1) * x_dim1
			    + 1], &c__1, &work[*n + m - *kb + j], &work[m - *
			    kb + j]);
/* L910: */
		}
	    }
/* L920: */
	}

	i__3 = *kb - 1;
	for (k = 1; k <= i__3; ++k) {
/* Computing MAX */
	    i__4 = 1, i__1 = k + i0 - m + 1;
	    j2 = i__ + k + 1 - std::max(i__4,i__1) * ka1;

/*           finish applying rotations in 1st set from the left */

	    for (l = *kb - k; l >= 1; --l) {
		nrt = (j2 + l - 1) / ka1;
		j1t = j2 - (nrt - 1) * ka1;
		if (nrt > 0) {
		    dlartv_(&nrt, &ab[ka1 - l + 1 + (j1t - ka1 + l) * ab_dim1]
, &inca, &ab[ka1 - l + (j1t - ka1 + l) * ab_dim1],
			     &inca, &work[*n + j1t], &work[j1t], &ka1);
		}
/* L930: */
	    }
/* L940: */
	}

	if (*kb > 1) {
/* Computing MIN */
	    i__4 = i__ + *kb;
	    i__3 = std::min(i__4,m) - (*ka << 1) - 1;
	    for (j = 2; j <= i__3; ++j) {
		work[*n + j] = work[*n + j + *ka];
		work[j] = work[j + *ka];
/* L950: */
	    }
	}

    }

    goto L490;

/*     End of DSBGST */

} /* dsbgst_ */

/* Subroutine */ int dsbgv_(const char *jobz, const char *uplo, integer *n, integer *ka,
	integer *kb, double *ab, integer *ldab, double *bb, integer *
	ldbb, double *w, double *z__, integer *ldz, double *work,
	integer *info)
{
    /* System generated locals */
    integer ab_dim1, ab_offset, bb_dim1, bb_offset, z_dim1, z_offset, i__1;

    /* Local variables */
    integer inde;
    char vect[1];
    integer iinfo;
    bool upper, wantz;
    integer indwrk;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSBGV computes all the eigenvalues, and optionally, the eigenvectors */
/*  of a real generalized symmetric-definite banded eigenproblem, of */
/*  the form A*x=(lambda)*B*x. Here A and B are assumed to be symmetric */
/*  and banded, and B is also positive definite. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangles of A and B are stored; */
/*          = 'L':  Lower triangles of A and B are stored. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B.  N >= 0. */

/*  KA      (input) INTEGER */
/*          The number of superdiagonals of the matrix A if UPLO = 'U', */
/*          or the number of subdiagonals if UPLO = 'L'. KA >= 0. */

/*  KB      (input) INTEGER */
/*          The number of superdiagonals of the matrix B if UPLO = 'U', */
/*          or the number of subdiagonals if UPLO = 'L'. KB >= 0. */

/*  AB      (input/output) DOUBLE PRECISION array, dimension (LDAB, N) */
/*          On entry, the upper or lower triangle of the symmetric band */
/*          matrix A, stored in the first ka+1 rows of the array.  The */
/*          j-th column of A is stored in the j-th column of the array AB */
/*          as follows: */
/*          if UPLO = 'U', AB(ka+1+i-j,j) = A(i,j) for max(1,j-ka)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+ka). */

/*          On exit, the contents of AB are destroyed. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KA+1. */

/*  BB      (input/output) DOUBLE PRECISION array, dimension (LDBB, N) */
/*          On entry, the upper or lower triangle of the symmetric band */
/*          matrix B, stored in the first kb+1 rows of the array.  The */
/*          j-th column of B is stored in the j-th column of the array BB */
/*          as follows: */
/*          if UPLO = 'U', BB(kb+1+i-j,j) = B(i,j) for max(1,j-kb)<=i<=j; */
/*          if UPLO = 'L', BB(1+i-j,j)    = B(i,j) for j<=i<=min(n,j+kb). */

/*          On exit, the factor S from the split Cholesky factorization */
/*          B = S**T*S, as returned by DPBSTF. */

/*  LDBB    (input) INTEGER */
/*          The leading dimension of the array BB.  LDBB >= KB+1. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, the eigenvalues in ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, N) */
/*          If JOBZ = 'V', then if INFO = 0, Z contains the matrix Z of */
/*          eigenvectors, with the i-th column of Z holding the */
/*          eigenvector associated with W(i). The eigenvectors are */
/*          normalized so that Z**T*B*Z = I. */
/*          If JOBZ = 'N', then Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= N. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (3*N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, and i is: */
/*             <= N:  the algorithm failed to converge: */
/*                    i off-diagonal elements of an intermediate */
/*                    tridiagonal form did not converge to zero; */
/*             > N:   if INFO = N + i, for 1 <= i <= N, then DPBSTF */
/*                    returned INFO = i: B is not positive definite. */
/*                    The factorization of B could not be completed and */
/*                    no eigenvalues or eigenvectors were computed. */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    bb_dim1 = *ldbb;
    bb_offset = 1 + bb_dim1;
    bb -= bb_offset;
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    upper = lsame_(uplo, "U");

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (upper || lsame_(uplo, "L"))) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ka < 0) {
	*info = -4;
    } else if (*kb < 0 || *kb > *ka) {
	*info = -5;
    } else if (*ldab < *ka + 1) {
	*info = -7;
    } else if (*ldbb < *kb + 1) {
	*info = -9;
    } else if (*ldz < 1 || wantz && *ldz < *n) {
	*info = -12;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSBGV ", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Form a split Cholesky factorization of B. */

    dpbstf_(uplo, n, kb, &bb[bb_offset], ldbb, info);
    if (*info != 0) {
	*info = *n + *info;
	return 0;
    }

/*     Transform problem to standard eigenvalue problem. */

    inde = 1;
    indwrk = inde + *n;
    dsbgst_(jobz, uplo, n, ka, kb, &ab[ab_offset], ldab, &bb[bb_offset], ldbb,
	     &z__[z_offset], ldz, &work[indwrk], &iinfo)
	    ;

/*     Reduce to tridiagonal form. */

    if (wantz) {
	*(unsigned char *)vect = 'U';
    } else {
	*(unsigned char *)vect = 'N';
    }
    dsbtrd_(vect, uplo, n, ka, &ab[ab_offset], ldab, &w[1], &work[inde], &z__[
	    z_offset], ldz, &work[indwrk], &iinfo);

/*     For eigenvalues only, call DSTERF.  For eigenvectors, call SSTEQR. */

    if (! wantz) {
	dsterf_(n, &w[1], &work[inde], info);
    } else {
	dsteqr_(jobz, n, &w[1], &work[inde], &z__[z_offset], ldz, &work[
		indwrk], info);
    }
    return 0;

/*     End of DSBGV */

} /* dsbgv_ */

/* Subroutine */ int dsbgvd_(const char *jobz, const char *uplo, integer *n, integer *ka,
	integer *kb, double *ab, integer *ldab, double *bb, integer *
	ldbb, double *w, double *z__, integer *ldz, double *work,
	integer *lwork, integer *iwork, integer *liwork, integer *info)
{
	/* Table of constant values */
	static double c_b12 = 1.;
	static double c_b13 = 0.;

    /* System generated locals */
    integer ab_dim1, ab_offset, bb_dim1, bb_offset, z_dim1, z_offset, i__1;

    /* Local variables */
    integer inde;
    char vect[1];
    integer iinfo, lwmin;
    bool upper, wantz;
    integer indwk2, llwrk2;
    integer indwrk, liwmin;
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

/*  DSBGVD computes all the eigenvalues, and optionally, the eigenvectors */
/*  of a real generalized symmetric-definite banded eigenproblem, of the */
/*  form A*x=(lambda)*B*x.  Here A and B are assumed to be symmetric and */
/*  banded, and B is also positive definite.  If eigenvectors are */
/*  desired, it uses a divide and conquer algorithm. */

/*  The divide and conquer algorithm makes very mild assumptions about */
/*  floating point arithmetic. It will work on machines with a guard */
/*  digit in add/subtract, or on those binary machines without guard */
/*  digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or */
/*  Cray-2. It could conceivably fail on hexadecimal or decimal machines */
/*  without guard digits, but we know of none. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangles of A and B are stored; */
/*          = 'L':  Lower triangles of A and B are stored. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B.  N >= 0. */

/*  KA      (input) INTEGER */
/*          The number of superdiagonals of the matrix A if UPLO = 'U', */
/*          or the number of subdiagonals if UPLO = 'L'.  KA >= 0. */

/*  KB      (input) INTEGER */
/*          The number of superdiagonals of the matrix B if UPLO = 'U', */
/*          or the number of subdiagonals if UPLO = 'L'.  KB >= 0. */

/*  AB      (input/output) DOUBLE PRECISION array, dimension (LDAB, N) */
/*          On entry, the upper or lower triangle of the symmetric band */
/*          matrix A, stored in the first ka+1 rows of the array.  The */
/*          j-th column of A is stored in the j-th column of the array AB */
/*          as follows: */
/*          if UPLO = 'U', AB(ka+1+i-j,j) = A(i,j) for max(1,j-ka)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+ka). */

/*          On exit, the contents of AB are destroyed. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KA+1. */

/*  BB      (input/output) DOUBLE PRECISION array, dimension (LDBB, N) */
/*          On entry, the upper or lower triangle of the symmetric band */
/*          matrix B, stored in the first kb+1 rows of the array.  The */
/*          j-th column of B is stored in the j-th column of the array BB */
/*          as follows: */
/*          if UPLO = 'U', BB(ka+1+i-j,j) = B(i,j) for max(1,j-kb)<=i<=j; */
/*          if UPLO = 'L', BB(1+i-j,j)    = B(i,j) for j<=i<=min(n,j+kb). */

/*          On exit, the factor S from the split Cholesky factorization */
/*          B = S**T*S, as returned by DPBSTF. */

/*  LDBB    (input) INTEGER */
/*          The leading dimension of the array BB.  LDBB >= KB+1. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, the eigenvalues in ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, N) */
/*          If JOBZ = 'V', then if INFO = 0, Z contains the matrix Z of */
/*          eigenvectors, with the i-th column of Z holding the */
/*          eigenvector associated with W(i).  The eigenvectors are */
/*          normalized so Z**T*B*Z = I. */
/*          If JOBZ = 'N', then Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          If N <= 1,               LWORK >= 1. */
/*          If JOBZ = 'N' and N > 1, LWORK >= 3*N. */
/*          If JOBZ = 'V' and N > 1, LWORK >= 1 + 5*N + 2*N**2. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal sizes of the WORK and IWORK */
/*          arrays, returns these values as the first entries of the WORK */
/*          and IWORK arrays, and no error message related to LWORK or */
/*          LIWORK is issued by XERBLA. */

/*  IWORK   (workspace/output) INTEGER array, dimension (MAX(1,LIWORK)) */
/*          On exit, if LIWORK > 0, IWORK(1) returns the optimal LIWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK. */
/*          If JOBZ  = 'N' or N <= 1, LIWORK >= 1. */
/*          If JOBZ  = 'V' and N > 1, LIWORK >= 3 + 5*N. */

/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates the optimal sizes of the WORK and */
/*          IWORK arrays, returns these values as the first entries of */
/*          the WORK and IWORK arrays, and no error message related to */
/*          LWORK or LIWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, and i is: */
/*             <= N:  the algorithm failed to converge: */
/*                    i off-diagonal elements of an intermediate */
/*                    tridiagonal form did not converge to zero; */
/*             > N:   if INFO = N + i, for 1 <= i <= N, then DPBSTF */
/*                    returned INFO = i: B is not positive definite. */
/*                    The factorization of B could not be completed and */
/*                    no eigenvalues or eigenvectors were computed. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Mark Fahey, Department of Mathematics, Univ. of Kentucky, USA */

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
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    bb_dim1 = *ldbb;
    bb_offset = 1 + bb_dim1;
    bb -= bb_offset;
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;
    --iwork;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    upper = lsame_(uplo, "U");
    lquery = *lwork == -1 || *liwork == -1;

    *info = 0;
    if (*n <= 1) {
	liwmin = 1;
	lwmin = 1;
    } else if (wantz) {
	liwmin = *n * 5 + 3;
/* Computing 2nd power */
	i__1 = *n;
	lwmin = *n * 5 + 1 + (i__1 * i__1 << 1);
    } else {
	liwmin = 1;
	lwmin = *n << 1;
    }

    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (upper || lsame_(uplo, "L"))) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ka < 0) {
	*info = -4;
    } else if (*kb < 0 || *kb > *ka) {
	*info = -5;
    } else if (*ldab < *ka + 1) {
	*info = -7;
    } else if (*ldbb < *kb + 1) {
	*info = -9;
    } else if (*ldz < 1 || wantz && *ldz < *n) {
	*info = -12;
    }

    if (*info == 0) {
	work[1] = (double) lwmin;
	iwork[1] = liwmin;

	if (*lwork < lwmin && ! lquery) {
	    *info = -14;
	} else if (*liwork < liwmin && ! lquery) {
	    *info = -16;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSBGVD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Form a split Cholesky factorization of B. */

    dpbstf_(uplo, n, kb, &bb[bb_offset], ldbb, info);
    if (*info != 0) {
	*info = *n + *info;
	return 0;
    }

/*     Transform problem to standard eigenvalue problem. */

    inde = 1;
    indwrk = inde + *n;
    indwk2 = indwrk + *n * *n;
    llwrk2 = *lwork - indwk2 + 1;
    dsbgst_(jobz, uplo, n, ka, kb, &ab[ab_offset], ldab, &bb[bb_offset], ldbb,
	     &z__[z_offset], ldz, &work[indwrk], &iinfo)
	    ;

/*     Reduce to tridiagonal form. */

    if (wantz) {
	*(unsigned char *)vect = 'U';
    } else {
	*(unsigned char *)vect = 'N';
    }
    dsbtrd_(vect, uplo, n, ka, &ab[ab_offset], ldab, &w[1], &work[inde], &z__[
	    z_offset], ldz, &work[indwrk], &iinfo);

/*     For eigenvalues only, call DSTERF. For eigenvectors, call SSTEDC. */

    if (! wantz) {
	dsterf_(n, &w[1], &work[inde], info);
    } else {
	dstedc_("I", n, &w[1], &work[inde], &work[indwrk], n, &work[indwk2], &
		llwrk2, &iwork[1], liwork, info);
	dgemm_("N", "N", n, n, n, &c_b12, &z__[z_offset], ldz, &work[indwrk],
		n, &c_b13, &work[indwk2], n);
	dlacpy_("A", n, n, &work[indwk2], n, &z__[z_offset], ldz);
    }

    work[1] = (double) lwmin;
    iwork[1] = liwmin;

    return 0;

/*     End of DSBGVD */

} /* dsbgvd_ */

/* Subroutine */ int dsbgvx_(const char *jobz, const char *range, const char *uplo, integer *n,
	integer *ka, integer *kb, double *ab, integer *ldab, double *
	bb, integer *ldbb, double *q, integer *ldq, double *vl,
	double *vu, integer *il, integer *iu, double *abstol, integer
	*m, double *w, double *z__, integer *ldz, double *work,
	integer *iwork, integer *ifail, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b25 = 1.;
	static double c_b27 = 0.;

    /* System generated locals */
    integer ab_dim1, ab_offset, bb_dim1, bb_offset, q_dim1, q_offset, z_dim1,
	    z_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, jj;
    double tmp1;
    integer indd, inde;
    char vect[1];
    bool test;
    integer itmp1, indee;
    integer iinfo;
    char order[1];
    bool upper, wantz, alleig, indeig;
    integer indibl;
    bool valeig;
    integer indisp;
    integer indiwo;
    integer indwrk;
    integer nsplit;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSBGVX computes selected eigenvalues, and optionally, eigenvectors */
/*  of a real generalized symmetric-definite banded eigenproblem, of */
/*  the form A*x=(lambda)*B*x.  Here A and B are assumed to be symmetric */
/*  and banded, and B is also positive definite.  Eigenvalues and */
/*  eigenvectors can be selected by specifying either all eigenvalues, */
/*  a range of values or a range of indices for the desired eigenvalues. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  RANGE   (input) CHARACTER*1 */
/*          = 'A': all eigenvalues will be found. */
/*          = 'V': all eigenvalues in the half-open interval (VL,VU] */
/*                 will be found. */
/*          = 'I': the IL-th through IU-th eigenvalues will be found. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangles of A and B are stored; */
/*          = 'L':  Lower triangles of A and B are stored. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B.  N >= 0. */

/*  KA      (input) INTEGER */
/*          The number of superdiagonals of the matrix A if UPLO = 'U', */
/*          or the number of subdiagonals if UPLO = 'L'.  KA >= 0. */

/*  KB      (input) INTEGER */
/*          The number of superdiagonals of the matrix B if UPLO = 'U', */
/*          or the number of subdiagonals if UPLO = 'L'.  KB >= 0. */

/*  AB      (input/output) DOUBLE PRECISION array, dimension (LDAB, N) */
/*          On entry, the upper or lower triangle of the symmetric band */
/*          matrix A, stored in the first ka+1 rows of the array.  The */
/*          j-th column of A is stored in the j-th column of the array AB */
/*          as follows: */
/*          if UPLO = 'U', AB(ka+1+i-j,j) = A(i,j) for max(1,j-ka)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+ka). */

/*          On exit, the contents of AB are destroyed. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KA+1. */

/*  BB      (input/output) DOUBLE PRECISION array, dimension (LDBB, N) */
/*          On entry, the upper or lower triangle of the symmetric band */
/*          matrix B, stored in the first kb+1 rows of the array.  The */
/*          j-th column of B is stored in the j-th column of the array BB */
/*          as follows: */
/*          if UPLO = 'U', BB(ka+1+i-j,j) = B(i,j) for max(1,j-kb)<=i<=j; */
/*          if UPLO = 'L', BB(1+i-j,j)    = B(i,j) for j<=i<=min(n,j+kb). */

/*          On exit, the factor S from the split Cholesky factorization */
/*          B = S**T*S, as returned by DPBSTF. */

/*  LDBB    (input) INTEGER */
/*          The leading dimension of the array BB.  LDBB >= KB+1. */

/*  Q       (output) DOUBLE PRECISION array, dimension (LDQ, N) */
/*          If JOBZ = 'V', the n-by-n matrix used in the reduction of */
/*          A*x = (lambda)*B*x to standard form, i.e. C*x = (lambda)*x, */
/*          and consequently C to tridiagonal form. */
/*          If JOBZ = 'N', the array Q is not referenced. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q.  If JOBZ = 'N', */
/*          LDQ >= 1. If JOBZ = 'V', LDQ >= max(1,N). */

/*  VL      (input) DOUBLE PRECISION */
/*  VU      (input) DOUBLE PRECISION */
/*          If RANGE='V', the lower and upper bounds of the interval to */
/*          be searched for eigenvalues. VL < VU. */
/*          Not referenced if RANGE = 'A' or 'I'. */

/*  IL      (input) INTEGER */
/*  IU      (input) INTEGER */
/*          If RANGE='I', the indices (in ascending order) of the */
/*          smallest and largest eigenvalues to be returned. */
/*          1 <= IL <= IU <= N, if N > 0; IL = 1 and IU = 0 if N = 0. */
/*          Not referenced if RANGE = 'A' or 'V'. */

/*  ABSTOL  (input) DOUBLE PRECISION */
/*          The absolute error tolerance for the eigenvalues. */
/*          An approximate eigenvalue is accepted as converged */
/*          when it is determined to lie in an interval [a,b] */
/*          of width less than or equal to */

/*                  ABSTOL + EPS *   max( |a|,|b| ) , */

/*          where EPS is the machine precision.  If ABSTOL is less than */
/*          or equal to zero, then  EPS*|T|  will be used in its place, */
/*          where |T| is the 1-norm of the tridiagonal matrix obtained */
/*          by reducing A to tridiagonal form. */

/*          Eigenvalues will be computed most accurately when ABSTOL is */
/*          set to twice the underflow threshold 2*DLAMCH('S'), not zero. */
/*          If this routine returns with INFO>0, indicating that some */
/*          eigenvectors did not converge, try setting ABSTOL to */
/*          2*DLAMCH('S'). */

/*  M       (output) INTEGER */
/*          The total number of eigenvalues found.  0 <= M <= N. */
/*          If RANGE = 'A', M = N, and if RANGE = 'I', M = IU-IL+1. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, the eigenvalues in ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, N) */
/*          If JOBZ = 'V', then if INFO = 0, Z contains the matrix Z of */
/*          eigenvectors, with the i-th column of Z holding the */
/*          eigenvector associated with W(i).  The eigenvectors are */
/*          normalized so Z**T*B*Z = I. */
/*          If JOBZ = 'N', then Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (7*N) */

/*  IWORK   (workspace/output) INTEGER array, dimension (5*N) */

/*  IFAIL   (output) INTEGER array, dimension (M) */
/*          If JOBZ = 'V', then if INFO = 0, the first M elements of */
/*          IFAIL are zero.  If INFO > 0, then IFAIL contains the */
/*          indices of the eigenvalues that failed to converge. */
/*          If JOBZ = 'N', then IFAIL is not referenced. */

/*  INFO    (output) INTEGER */
/*          = 0 : successful exit */
/*          < 0 : if INFO = -i, the i-th argument had an illegal value */
/*          <= N: if INFO = i, then i eigenvectors failed to converge. */
/*                  Their indices are stored in IFAIL. */
/*          > N : DPBSTF returned an error code; i.e., */
/*                if INFO = N + i, for 1 <= i <= N, then the leading */
/*                minor of order i of B is not positive definite. */
/*                The factorization of B could not be completed and */
/*                no eigenvalues or eigenvectors were computed. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Mark Fahey, Department of Mathematics, Univ. of Kentucky, USA */

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
    bb_dim1 = *ldbb;
    bb_offset = 1 + bb_dim1;
    bb -= bb_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;
    --iwork;
    --ifail;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    upper = lsame_(uplo, "U");
    alleig = lsame_(range, "A");
    valeig = lsame_(range, "V");
    indeig = lsame_(range, "I");

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (alleig || valeig || indeig)) {
	*info = -2;
    } else if (! (upper || lsame_(uplo, "L"))) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*ka < 0) {
	*info = -5;
    } else if (*kb < 0 || *kb > *ka) {
	*info = -6;
    } else if (*ldab < *ka + 1) {
	*info = -8;
    } else if (*ldbb < *kb + 1) {
	*info = -10;
    } else if (*ldq < 1 || wantz && *ldq < *n) {
	*info = -12;
    } else {
	if (valeig) {
	    if (*n > 0 && *vu <= *vl) {
		*info = -14;
	    }
	} else if (indeig) {
	    if (*il < 1 || *il > std::max(1_integer,*n)) {
		*info = -15;
	    } else if (*iu < std::min(*n,*il) || *iu > *n) {
		*info = -16;
	    }
	}
    }
    if (*info == 0) {
	if (*ldz < 1 || wantz && *ldz < *n) {
	    *info = -21;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSBGVX", &i__1);
	return 0;
    }

/*     Quick return if possible */

    *m = 0;
    if (*n == 0) {
	return 0;
    }

/*     Form a split Cholesky factorization of B. */

    dpbstf_(uplo, n, kb, &bb[bb_offset], ldbb, info);
    if (*info != 0) {
	*info = *n + *info;
	return 0;
    }

/*     Transform problem to standard eigenvalue problem. */

    dsbgst_(jobz, uplo, n, ka, kb, &ab[ab_offset], ldab, &bb[bb_offset], ldbb,
	     &q[q_offset], ldq, &work[1], &iinfo);

/*     Reduce symmetric band matrix to tridiagonal form. */

    indd = 1;
    inde = indd + *n;
    indwrk = inde + *n;
    if (wantz) {
	*(unsigned char *)vect = 'U';
    } else {
	*(unsigned char *)vect = 'N';
    }
    dsbtrd_(vect, uplo, n, ka, &ab[ab_offset], ldab, &work[indd], &work[inde],
	     &q[q_offset], ldq, &work[indwrk], &iinfo);

/*     If all eigenvalues are desired and ABSTOL is less than or equal */
/*     to zero, then call DSTERF or SSTEQR.  If this fails for some */
/*     eigenvalue, then try DSTEBZ. */

    test = false;
    if (indeig) {
	if (*il == 1 && *iu == *n) {
	    test = true;
	}
    }
    if ((alleig || test) && *abstol <= 0.) {
	dcopy_(n, &work[indd], &c__1, &w[1], &c__1);
	indee = indwrk + (*n << 1);
	i__1 = *n - 1;
	dcopy_(&i__1, &work[inde], &c__1, &work[indee], &c__1);
	if (! wantz) {
	    dsterf_(n, &w[1], &work[indee], info);
	} else {
	    dlacpy_("A", n, n, &q[q_offset], ldq, &z__[z_offset], ldz);
	    dsteqr_(jobz, n, &w[1], &work[indee], &z__[z_offset], ldz, &work[
		    indwrk], info);
	    if (*info == 0) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    ifail[i__] = 0;
/* L10: */
		}
	    }
	}
	if (*info == 0) {
	    *m = *n;
	    goto L30;
	}
	*info = 0;
    }

/*     Otherwise, call DSTEBZ and, if eigenvectors are desired, */
/*     call DSTEIN. */

    if (wantz) {
	*(unsigned char *)order = 'B';
    } else {
	*(unsigned char *)order = 'E';
    }
    indibl = 1;
    indisp = indibl + *n;
    indiwo = indisp + *n;
    dstebz_(range, order, n, vl, vu, il, iu, abstol, &work[indd], &work[inde],
	     m, &nsplit, &w[1], &iwork[indibl], &iwork[indisp], &work[indwrk],
	     &iwork[indiwo], info);

    if (wantz) {
	dstein_(n, &work[indd], &work[inde], m, &w[1], &iwork[indibl], &iwork[
		indisp], &z__[z_offset], ldz, &work[indwrk], &iwork[indiwo], &
		ifail[1], info);

/*        Apply transformation matrix used in reduction to tridiagonal */
/*        form to eigenvectors returned by DSTEIN. */

	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    dcopy_(n, &z__[j * z_dim1 + 1], &c__1, &work[1], &c__1);
	    dgemv_("N", n, n, &c_b25, &q[q_offset], ldq, &work[1], &c__1, &
		    c_b27, &z__[j * z_dim1 + 1], &c__1);
/* L20: */
	}
    }

L30:

/*     If eigenvalues are not in order, then sort them, along with */
/*     eigenvectors. */

    if (wantz) {
	i__1 = *m - 1;
	for (j = 1; j <= i__1; ++j) {
	    i__ = 0;
	    tmp1 = w[j];
	    i__2 = *m;
	    for (jj = j + 1; jj <= i__2; ++jj) {
		if (w[jj] < tmp1) {
		    i__ = jj;
		    tmp1 = w[jj];
		}
/* L40: */
	    }

	    if (i__ != 0) {
		itmp1 = iwork[indibl + i__ - 1];
		w[i__] = w[j];
		iwork[indibl + i__ - 1] = iwork[indibl + j - 1];
		w[j] = tmp1;
		iwork[indibl + j - 1] = itmp1;
		dswap_(n, &z__[i__ * z_dim1 + 1], &c__1, &z__[j * z_dim1 + 1],
			 &c__1);
		if (*info != 0) {
		    itmp1 = ifail[i__];
		    ifail[i__] = ifail[j];
		    ifail[j] = itmp1;
		}
	    }
/* L50: */
	}
    }

    return 0;

/*     End of DSBGVX */

} /* dsbgvx_ */

/* Subroutine */ int dsbtrd_(const char *vect, const char *uplo, integer *n, integer *kd,
	double *ab, integer *ldab, double *d__, double *e,
	double *q, integer *ldq, double *work, integer *info)
{
	/* Table of constant values */
	static double c_b9 = 0.;
	static double c_b10 = 1.;
	static integer c__1 = 1;

    /* System generated locals */
    integer ab_dim1, ab_offset, q_dim1, q_offset, i__1, i__2, i__3, i__4,
	    i__5;

    /* Local variables */
    integer i__, j, k, l, i2, j1, j2, nq, nr, kd1, ibl, iqb, kdn, jin, nrt,
	    kdm1, inca, jend, lend, jinc, incx, last;
    double temp;
    integer j1end, j1inc, iqend;
    bool initq, wantq, upper;
    integer iqaend;

/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSBTRD reduces a real symmetric band matrix A to symmetric */
/*  tridiagonal form T by an orthogonal similarity transformation: */
/*  Q**T * A * Q = T. */

/*  Arguments */
/*  ========= */

/*  VECT    (input) CHARACTER*1 */
/*          = 'N':  do not form Q; */
/*          = 'V':  form Q; */
/*          = 'U':  update a matrix X, by forming X*Q. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  KD      (input) INTEGER */
/*          The number of superdiagonals of the matrix A if UPLO = 'U', */
/*          or the number of subdiagonals if UPLO = 'L'.  KD >= 0. */

/*  AB      (input/output) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          On entry, the upper or lower triangle of the symmetric band */
/*          matrix A, stored in the first KD+1 rows of the array.  The */
/*          j-th column of A is stored in the j-th column of the array AB */
/*          as follows: */
/*          if UPLO = 'U', AB(kd+1+i-j,j) = A(i,j) for max(1,j-kd)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+kd). */
/*          On exit, the diagonal elements of AB are overwritten by the */
/*          diagonal elements of the tridiagonal matrix T; if KD > 0, the */
/*          elements on the first superdiagonal (if UPLO = 'U') or the */
/*          first subdiagonal (if UPLO = 'L') are overwritten by the */
/*          off-diagonal elements of T; the rest of AB is overwritten by */
/*          values generated during the reduction. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KD+1. */

/*  D       (output) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal elements of the tridiagonal matrix T. */

/*  E       (output) DOUBLE PRECISION array, dimension (N-1) */
/*          The off-diagonal elements of the tridiagonal matrix T: */
/*          E(i) = T(i,i+1) if UPLO = 'U'; E(i) = T(i+1,i) if UPLO = 'L'. */

/*  Q       (input/output) DOUBLE PRECISION array, dimension (LDQ,N) */
/*          On entry, if VECT = 'U', then Q must contain an N-by-N */
/*          matrix X; if VECT = 'N' or 'V', then Q need not be set. */

/*          On exit: */
/*          if VECT = 'V', Q contains the N-by-N orthogonal matrix Q; */
/*          if VECT = 'U', Q contains the product X*Q; */
/*          if VECT = 'N', the array Q is not referenced. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q. */
/*          LDQ >= 1, and LDQ >= N if VECT = 'V' or 'U'. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  Further Details */
/*  =============== */

/*  Modified by Linda Kaufman, Bell Labs. */

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
    --work;

    /* Function Body */
    initq = lsame_(vect, "V");
    wantq = initq || lsame_(vect, "U");
    upper = lsame_(uplo, "U");
    kd1 = *kd + 1;
    kdm1 = *kd - 1;
    incx = *ldab - 1;
    iqend = 1;

    *info = 0;
    if (! wantq && ! lsame_(vect, "N")) {
	*info = -1;
    } else if (! upper && ! lsame_(uplo, "L")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*kd < 0) {
	*info = -4;
    } else if (*ldab < kd1) {
	*info = -6;
    } else if (*ldq < std::max(1_integer,*n) && wantq) {
	*info = -10;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSBTRD", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Initialize Q to the unit matrix, if needed */

    if (initq) {
	dlaset_("Full", n, n, &c_b9, &c_b10, &q[q_offset], ldq);
    }

/*     Wherever possible, plane rotations are generated and applied in */
/*     vector operations of length NR over the index set J1:J2:KD1. */

/*     The cosines and sines of the plane rotations are stored in the */
/*     arrays D and WORK. */

    inca = kd1 * *ldab;
/* Computing MIN */
    i__1 = *n - 1;
    kdn = std::min(i__1,*kd);
    if (upper) {

	if (*kd > 1) {

/*           Reduce to tridiagonal form, working with upper triangle */

	    nr = 0;
	    j1 = kdn + 2;
	    j2 = 1;

	    i__1 = *n - 2;
	    for (i__ = 1; i__ <= i__1; ++i__) {

/*              Reduce i-th row of matrix to tridiagonal form */

		for (k = kdn + 1; k >= 2; --k) {
		    j1 += kdn;
		    j2 += kdn;

		    if (nr > 0) {

/*                    generate plane rotations to annihilate nonzero */
/*                    elements which have been created outside the band */

			dlargv_(&nr, &ab[(j1 - 1) * ab_dim1 + 1], &inca, &
				work[j1], &kd1, &d__[j1], &kd1);

/*                    apply rotations from the right */


/*                    Dependent on the the number of diagonals either */
/*                    DLARTV or DROT is used */

			if (nr >= (*kd << 1) - 1) {
			    i__2 = *kd - 1;
			    for (l = 1; l <= i__2; ++l) {
				dlartv_(&nr, &ab[l + 1 + (j1 - 1) * ab_dim1],
					&inca, &ab[l + j1 * ab_dim1], &inca, &
					d__[j1], &work[j1], &kd1);
/* L10: */
			    }

			} else {
			    jend = j1 + (nr - 1) * kd1;
			    i__2 = jend;
			    i__3 = kd1;
			    for (jinc = j1; i__3 < 0 ? jinc >= i__2 : jinc <=
				    i__2; jinc += i__3) {
				drot_(&kdm1, &ab[(jinc - 1) * ab_dim1 + 2], &
					c__1, &ab[jinc * ab_dim1 + 1], &c__1,
					&d__[jinc], &work[jinc]);
/* L20: */
			    }
			}
		    }


		    if (k > 2) {
			if (k <= *n - i__ + 1) {

/*                       generate plane rotation to annihilate a(i,i+k-1) */
/*                       within the band */

			    dlartg_(&ab[*kd - k + 3 + (i__ + k - 2) * ab_dim1]
, &ab[*kd - k + 2 + (i__ + k - 1) *
				    ab_dim1], &d__[i__ + k - 1], &work[i__ +
				    k - 1], &temp);
			    ab[*kd - k + 3 + (i__ + k - 2) * ab_dim1] = temp;

/*                       apply rotation from the right */

			    i__3 = k - 3;
			    drot_(&i__3, &ab[*kd - k + 4 + (i__ + k - 2) *
				    ab_dim1], &c__1, &ab[*kd - k + 3 + (i__ +
				    k - 1) * ab_dim1], &c__1, &d__[i__ + k -
				    1], &work[i__ + k - 1]);
			}
			++nr;
			j1 = j1 - kdn - 1;
		    }

/*                 apply plane rotations from both sides to diagonal */
/*                 blocks */

		    if (nr > 0) {
			dlar2v_(&nr, &ab[kd1 + (j1 - 1) * ab_dim1], &ab[kd1 +
				j1 * ab_dim1], &ab[*kd + j1 * ab_dim1], &inca,
				 &d__[j1], &work[j1], &kd1);
		    }

/*                 apply plane rotations from the left */

		    if (nr > 0) {
			if ((*kd << 1) - 1 < nr) {

/*                    Dependent on the the number of diagonals either */
/*                    DLARTV or DROT is used */

			    i__3 = *kd - 1;
			    for (l = 1; l <= i__3; ++l) {
				if (j2 + l > *n) {
				    nrt = nr - 1;
				} else {
				    nrt = nr;
				}
				if (nrt > 0) {
				    dlartv_(&nrt, &ab[*kd - l + (j1 + l) *
					    ab_dim1], &inca, &ab[*kd - l + 1
					    + (j1 + l) * ab_dim1], &inca, &
					    d__[j1], &work[j1], &kd1);
				}
/* L30: */
			    }
			} else {
			    j1end = j1 + kd1 * (nr - 2);
			    if (j1end >= j1) {
				i__3 = j1end;
				i__2 = kd1;
				for (jin = j1; i__2 < 0 ? jin >= i__3 : jin <=
					 i__3; jin += i__2) {
				    i__4 = *kd - 1;
				    drot_(&i__4, &ab[*kd - 1 + (jin + 1) *
					    ab_dim1], &incx, &ab[*kd + (jin +
					    1) * ab_dim1], &incx, &d__[jin], &
					    work[jin]);
/* L40: */
				}
			    }
/* Computing MIN */
			    i__2 = kdm1, i__3 = *n - j2;
			    lend = std::min(i__2,i__3);
			    last = j1end + kd1;
			    if (lend > 0) {
				drot_(&lend, &ab[*kd - 1 + (last + 1) *
					ab_dim1], &incx, &ab[*kd + (last + 1)
					* ab_dim1], &incx, &d__[last], &work[
					last]);
			    }
			}
		    }

		    if (wantq) {

/*                    accumulate product of plane rotations in Q */

			if (initq) {

/*                 take advantage of the fact that Q was */
/*                 initially the Identity matrix */

			    iqend = std::max(iqend,j2);
/* Computing MAX */
			    i__2 = 0, i__3 = k - 3;
			    i2 = std::max(i__2,i__3);
			    iqaend = i__ * *kd + 1;
			    if (k == 2) {
				iqaend += *kd;
			    }
			    iqaend = std::min(iqaend,iqend);
			    i__2 = j2;
			    i__3 = kd1;
			    for (j = j1; i__3 < 0 ? j >= i__2 : j <= i__2; j
				    += i__3) {
				ibl = i__ - i2 / kdm1;
				++i2;
/* Computing MAX */
				i__4 = 1, i__5 = j - ibl;
				iqb = std::max(i__4,i__5);
				nq = iqaend + 1 - iqb;
/* Computing MIN */
				i__4 = iqaend + *kd;
				iqaend = std::min(i__4,iqend);
				drot_(&nq, &q[iqb + (j - 1) * q_dim1], &c__1,
					&q[iqb + j * q_dim1], &c__1, &d__[j],
					&work[j]);
/* L50: */
			    }
			} else {

			    i__3 = j2;
			    i__2 = kd1;
			    for (j = j1; i__2 < 0 ? j >= i__3 : j <= i__3; j
				    += i__2) {
				drot_(n, &q[(j - 1) * q_dim1 + 1], &c__1, &q[
					j * q_dim1 + 1], &c__1, &d__[j], &
					work[j]);
/* L60: */
			    }
			}

		    }

		    if (j2 + kdn > *n) {

/*                    adjust J2 to keep within the bounds of the matrix */

			--nr;
			j2 = j2 - kdn - 1;
		    }

		    i__2 = j2;
		    i__3 = kd1;
		    for (j = j1; i__3 < 0 ? j >= i__2 : j <= i__2; j += i__3)
			    {

/*                    create nonzero element a(j-1,j+kd) outside the band */
/*                    and store it in WORK */

			work[j + *kd] = work[j] * ab[(j + *kd) * ab_dim1 + 1];
			ab[(j + *kd) * ab_dim1 + 1] = d__[j] * ab[(j + *kd) *
				ab_dim1 + 1];
/* L70: */
		    }
/* L80: */
		}
/* L90: */
	    }
	}

	if (*kd > 0) {

/*           copy off-diagonal elements to E */

	    i__1 = *n - 1;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		e[i__] = ab[*kd + (i__ + 1) * ab_dim1];
/* L100: */
	    }
	} else {

/*           set E to zero if original matrix was diagonal */

	    i__1 = *n - 1;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		e[i__] = 0.;
/* L110: */
	    }
	}

/*        copy diagonal elements to D */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    d__[i__] = ab[kd1 + i__ * ab_dim1];
/* L120: */
	}

    } else {

	if (*kd > 1) {

/*           Reduce to tridiagonal form, working with lower triangle */

	    nr = 0;
	    j1 = kdn + 2;
	    j2 = 1;

	    i__1 = *n - 2;
	    for (i__ = 1; i__ <= i__1; ++i__) {

/*              Reduce i-th column of matrix to tridiagonal form */

		for (k = kdn + 1; k >= 2; --k) {
		    j1 += kdn;
		    j2 += kdn;

		    if (nr > 0) {

/*                    generate plane rotations to annihilate nonzero */
/*                    elements which have been created outside the band */

			dlargv_(&nr, &ab[kd1 + (j1 - kd1) * ab_dim1], &inca, &
				work[j1], &kd1, &d__[j1], &kd1);

/*                    apply plane rotations from one side */


/*                    Dependent on the the number of diagonals either */
/*                    DLARTV or DROT is used */

			if (nr > (*kd << 1) - 1) {
			    i__3 = *kd - 1;
			    for (l = 1; l <= i__3; ++l) {
				dlartv_(&nr, &ab[kd1 - l + (j1 - kd1 + l) *
					ab_dim1], &inca, &ab[kd1 - l + 1 + (
					j1 - kd1 + l) * ab_dim1], &inca, &d__[
					j1], &work[j1], &kd1);
/* L130: */
			    }
			} else {
			    jend = j1 + kd1 * (nr - 1);
			    i__3 = jend;
			    i__2 = kd1;
			    for (jinc = j1; i__2 < 0 ? jinc >= i__3 : jinc <=
				    i__3; jinc += i__2) {
				drot_(&kdm1, &ab[*kd + (jinc - *kd) * ab_dim1]
, &incx, &ab[kd1 + (jinc - *kd) *
					ab_dim1], &incx, &d__[jinc], &work[
					jinc]);
/* L140: */
			    }
			}

		    }

		    if (k > 2) {
			if (k <= *n - i__ + 1) {

/*                       generate plane rotation to annihilate a(i+k-1,i) */
/*                       within the band */

			    dlartg_(&ab[k - 1 + i__ * ab_dim1], &ab[k + i__ *
				    ab_dim1], &d__[i__ + k - 1], &work[i__ +
				    k - 1], &temp);
			    ab[k - 1 + i__ * ab_dim1] = temp;

/*                       apply rotation from the left */

			    i__2 = k - 3;
			    i__3 = *ldab - 1;
			    i__4 = *ldab - 1;
			    drot_(&i__2, &ab[k - 2 + (i__ + 1) * ab_dim1], &
				    i__3, &ab[k - 1 + (i__ + 1) * ab_dim1], &
				    i__4, &d__[i__ + k - 1], &work[i__ + k -
				    1]);
			}
			++nr;
			j1 = j1 - kdn - 1;
		    }

/*                 apply plane rotations from both sides to diagonal */
/*                 blocks */

		    if (nr > 0) {
			dlar2v_(&nr, &ab[(j1 - 1) * ab_dim1 + 1], &ab[j1 *
				ab_dim1 + 1], &ab[(j1 - 1) * ab_dim1 + 2], &
				inca, &d__[j1], &work[j1], &kd1);
		    }

/*                 apply plane rotations from the right */


/*                    Dependent on the the number of diagonals either */
/*                    DLARTV or DROT is used */

		    if (nr > 0) {
			if (nr > (*kd << 1) - 1) {
			    i__2 = *kd - 1;
			    for (l = 1; l <= i__2; ++l) {
				if (j2 + l > *n) {
				    nrt = nr - 1;
				} else {
				    nrt = nr;
				}
				if (nrt > 0) {
				    dlartv_(&nrt, &ab[l + 2 + (j1 - 1) *
					    ab_dim1], &inca, &ab[l + 1 + j1 *
					    ab_dim1], &inca, &d__[j1], &work[
					    j1], &kd1);
				}
/* L150: */
			    }
			} else {
			    j1end = j1 + kd1 * (nr - 2);
			    if (j1end >= j1) {
				i__2 = j1end;
				i__3 = kd1;
				for (j1inc = j1; i__3 < 0 ? j1inc >= i__2 :
					j1inc <= i__2; j1inc += i__3) {
				    drot_(&kdm1, &ab[(j1inc - 1) * ab_dim1 +
					    3], &c__1, &ab[j1inc * ab_dim1 +
					    2], &c__1, &d__[j1inc], &work[
					    j1inc]);
/* L160: */
				}
			    }
/* Computing MIN */
			    i__3 = kdm1, i__2 = *n - j2;
			    lend = std::min(i__3,i__2);
			    last = j1end + kd1;
			    if (lend > 0) {
				drot_(&lend, &ab[(last - 1) * ab_dim1 + 3], &
					c__1, &ab[last * ab_dim1 + 2], &c__1,
					&d__[last], &work[last]);
			    }
			}
		    }



		    if (wantq) {

/*                    accumulate product of plane rotations in Q */

			if (initq) {

/*                 take advantage of the fact that Q was */
/*                 initially the Identity matrix */

			    iqend = std::max(iqend,j2);
/* Computing MAX */
			    i__3 = 0, i__2 = k - 3;
			    i2 = std::max(i__3,i__2);
			    iqaend = i__ * *kd + 1;
			    if (k == 2) {
				iqaend += *kd;
			    }
			    iqaend = std::min(iqaend,iqend);
			    i__3 = j2;
			    i__2 = kd1;
			    for (j = j1; i__2 < 0 ? j >= i__3 : j <= i__3; j
				    += i__2) {
				ibl = i__ - i2 / kdm1;
				++i2;
/* Computing MAX */
				i__4 = 1, i__5 = j - ibl;
				iqb = std::max(i__4,i__5);
				nq = iqaend + 1 - iqb;
/* Computing MIN */
				i__4 = iqaend + *kd;
				iqaend = std::min(i__4,iqend);
				drot_(&nq, &q[iqb + (j - 1) * q_dim1], &c__1,
					&q[iqb + j * q_dim1], &c__1, &d__[j],
					&work[j]);
/* L170: */
			    }
			} else {

			    i__2 = j2;
			    i__3 = kd1;
			    for (j = j1; i__3 < 0 ? j >= i__2 : j <= i__2; j
				    += i__3) {
				drot_(n, &q[(j - 1) * q_dim1 + 1], &c__1, &q[
					j * q_dim1 + 1], &c__1, &d__[j], &
					work[j]);
/* L180: */
			    }
			}
		    }

		    if (j2 + kdn > *n) {

/*                    adjust J2 to keep within the bounds of the matrix */

			--nr;
			j2 = j2 - kdn - 1;
		    }

		    i__3 = j2;
		    i__2 = kd1;
		    for (j = j1; i__2 < 0 ? j >= i__3 : j <= i__3; j += i__2)
			    {

/*                    create nonzero element a(j+kd,j-1) outside the */
/*                    band and store it in WORK */

			work[j + *kd] = work[j] * ab[kd1 + j * ab_dim1];
			ab[kd1 + j * ab_dim1] = d__[j] * ab[kd1 + j * ab_dim1]
				;
/* L190: */
		    }
/* L200: */
		}
/* L210: */
	    }
	}

	if (*kd > 0) {

/*           copy off-diagonal elements to E */

	    i__1 = *n - 1;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		e[i__] = ab[i__ * ab_dim1 + 2];
/* L220: */
	    }
	} else {

/*           set E to zero if original matrix was diagonal */

	    i__1 = *n - 1;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		e[i__] = 0.;
/* L230: */
	    }
	}

/*        copy diagonal elements to D */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    d__[i__] = ab[i__ * ab_dim1 + 1];
/* L240: */
	}
    }

    return 0;

/*     End of DSBTRD */

} /* dsbtrd_ */

int dsfrk_(const char *transr, const char *uplo, const char *trans, integer *n,
	 integer *k, double *alpha, double *a, integer *lda,
	double *beta, double *c__)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1;

    /* Local variables */
    integer j, n1, n2, nk, info;
    bool normaltransr;
    integer nrowa;
    bool lower, nisodd, notrans;


/*  -- LAPACK routine (version 3.2)                                    -- */

/*  -- Contributed by Julien Langou of the Univ. of Colorado Denver    -- */
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

/*  Level 3 BLAS like routine for C in RFP Format. */

/*  DSFRK performs one of the symmetric rank--k operations */

/*     C := alpha*A*A' + beta*C, */

/*  or */

/*     C := alpha*A'*A + beta*C, */

/*  where alpha and beta are real scalars, C is an n--by--n symmetric */
/*  matrix and A is an n--by--k matrix in the first case and a k--by--n */
/*  matrix in the second case. */

/*  Arguments */
/*  ========== */

/*  TRANSR    (input) CHARACTER */
/*          = 'N':  The Normal Form of RFP A is stored; */
/*          = 'T':  The Transpose Form of RFP A is stored. */

/*  UPLO   - (input) CHARACTER */
/*           On  entry, UPLO specifies whether the upper or lower */
/*           triangular part of the array C is to be referenced as */
/*           follows: */

/*              UPLO = 'U' or 'u'   Only the upper triangular part of C */
/*                                  is to be referenced. */

/*              UPLO = 'L' or 'l'   Only the lower triangular part of C */
/*                                  is to be referenced. */

/*           Unchanged on exit. */

/*  TRANS  - (input) CHARACTER */
/*           On entry, TRANS specifies the operation to be performed as */
/*           follows: */

/*              TRANS = 'N' or 'n'   C := alpha*A*A' + beta*C. */

/*              TRANS = 'T' or 't'   C := alpha*A'*A + beta*C. */

/*           Unchanged on exit. */

/*  N      - (input) INTEGER. */
/*           On entry, N specifies the order of the matrix C. N must be */
/*           at least zero. */
/*           Unchanged on exit. */

/*  K      - (input) INTEGER. */
/*           On entry with TRANS = 'N' or 'n', K specifies the number */
/*           of  columns of the matrix A, and on entry with TRANS = 'T' */
/*           or 't', K specifies the number of rows of the matrix A. K */
/*           must be at least zero. */
/*           Unchanged on exit. */

/*  ALPHA  - (input) DOUBLE PRECISION. */
/*           On entry, ALPHA specifies the scalar alpha. */
/*           Unchanged on exit. */

/*  A      - (input) DOUBLE PRECISION array of DIMENSION ( LDA, ka ), where KA */
/*           is K  when TRANS = 'N' or 'n', and is N otherwise. Before */
/*           entry with TRANS = 'N' or 'n', the leading N--by--K part of */
/*           the array A must contain the matrix A, otherwise the leading */
/*           K--by--N part of the array A must contain the matrix A. */
/*           Unchanged on exit. */

/*  LDA    - (input) INTEGER. */
/*           On entry, LDA specifies the first dimension of A as declared */
/*           in  the  calling  (sub)  program.   When  TRANS = 'N' or 'n' */
/*           then  LDA must be at least  max( 1, n ), otherwise  LDA must */
/*           be at least  max( 1, k ). */
/*           Unchanged on exit. */

/*  BETA   - (input) DOUBLE PRECISION. */
/*           On entry, BETA specifies the scalar beta. */
/*           Unchanged on exit. */


/*  C      - (input/output) DOUBLE PRECISION array, dimension ( NT ); */
/*           NT = N*(N+1)/2. On entry, the symmetric matrix C in RFP */
/*           Format. RFP Format is described by TRANSR, UPLO and N. */

/*  Arguments */
/*  ========== */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --c__;

    /* Function Body */
    info = 0;
    normaltransr = lsame_(transr, "N");
    lower = lsame_(uplo, "L");
    notrans = lsame_(trans, "N");

    if (notrans) {
	nrowa = *n;
    } else {
	nrowa = *k;
    }

    if (! normaltransr && ! lsame_(transr, "T")) {
	info = -1;
    } else if (! lower && ! lsame_(uplo, "U")) {
	info = -2;
    } else if (! notrans && ! lsame_(trans, "T")) {
	info = -3;
    } else if (*n < 0) {
	info = -4;
    } else if (*k < 0) {
	info = -5;
    } else if (*lda < std::max(1_integer,nrowa)) {
	info = -8;
    }
    if (info != 0) {
	i__1 = -info;
	xerbla_("DSFRK ", &i__1);
	return 0;
    }

/*     Quick return if possible. */

/*     The quick return case: ((ALPHA.EQ.0).AND.(BETA.NE.ZERO)) is not */
/*     done (it is in DSYRK for example) and left in the general case. */

    if (*n == 0 || (*alpha == 0. || *k == 0) && *beta == 1.) {
	return 0;
    }

    if (*alpha == 0. && *beta == 0.) {
	i__1 = *n * (*n + 1) / 2;
	for (j = 1; j <= i__1; ++j) {
	    c__[j] = 0.;
	}
	return 0;
    }

/*     C is N-by-N. */
/*     If N is odd, set NISODD = .TRUE., and N1 and N2. */
/*     If N is even, NISODD = .FALSE., and NK. */

    if (*n % 2 == 0) {
	nisodd = false;
	nk = *n / 2;
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

/*        N is odd */

	if (normaltransr) {

/*           N is odd and TRANSR = 'N' */

	    if (lower) {

/*              N is odd, TRANSR = 'N', and UPLO = 'L' */

		if (notrans) {

/*                 N is odd, TRANSR = 'N', UPLO = 'L', and TRANS = 'N' */

		    dsyrk_("L", "N", &n1, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[1], n);
		    dsyrk_("U", "N", &n2, k, alpha, &a[n1 + 1 + a_dim1], lda,
			    beta, &c__[*n + 1], n);
		    dgemm_("N", "T", &n2, &n1, k, alpha, &a[n1 + 1 + a_dim1],
			    lda, &a[a_dim1 + 1], lda, beta, &c__[n1 + 1], n);

		} else {

/*                 N is odd, TRANSR = 'N', UPLO = 'L', and TRANS = 'T' */

		    dsyrk_("L", "T", &n1, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[1], n);
		    dsyrk_("U", "T", &n2, k, alpha, &a[(n1 + 1) * a_dim1 + 1],
			     lda, beta, &c__[*n + 1], n)
			    ;
		    dgemm_("T", "N", &n2, &n1, k, alpha, &a[(n1 + 1) * a_dim1
			    + 1], lda, &a[a_dim1 + 1], lda, beta, &c__[n1 + 1], n);

		}

	    } else {

/*              N is odd, TRANSR = 'N', and UPLO = 'U' */

		if (notrans) {

/*                 N is odd, TRANSR = 'N', UPLO = 'U', and TRANS = 'N' */

		    dsyrk_("L", "N", &n1, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[n2 + 1], n);
		    dsyrk_("U", "N", &n2, k, alpha, &a[n2 + a_dim1], lda,
			    beta, &c__[n1 + 1], n);
		    dgemm_("N", "T", &n1, &n2, k, alpha, &a[a_dim1 + 1], lda,
			    &a[n2 + a_dim1], lda, beta, &c__[1], n);

		} else {

/*                 N is odd, TRANSR = 'N', UPLO = 'U', and TRANS = 'T' */

		    dsyrk_("L", "T", &n1, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[n2 + 1], n);
		    dsyrk_("U", "T", &n2, k, alpha, &a[n2 * a_dim1 + 1], lda,
			    beta, &c__[n1 + 1], n);
		    dgemm_("T", "N", &n1, &n2, k, alpha, &a[a_dim1 + 1], lda,
			    &a[n2 * a_dim1 + 1], lda, beta, &c__[1], n);

		}

	    }

	} else {

/*           N is odd, and TRANSR = 'T' */

	    if (lower) {

/*              N is odd, TRANSR = 'T', and UPLO = 'L' */

		if (notrans) {

/*                 N is odd, TRANSR = 'T', UPLO = 'L', and TRANS = 'N' */

		    dsyrk_("U", "N", &n1, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[1], &n1);
		    dsyrk_("L", "N", &n2, k, alpha, &a[n1 + 1 + a_dim1], lda,
			    beta, &c__[2], &n1);
		    dgemm_("N", "T", &n1, &n2, k, alpha, &a[a_dim1 + 1], lda,
			    &a[n1 + 1 + a_dim1], lda, beta, &c__[n1 * n1 + 1],
			     &n1);

		} else {

/*                 N is odd, TRANSR = 'T', UPLO = 'L', and TRANS = 'T' */

		    dsyrk_("U", "T", &n1, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[1], &n1);
		    dsyrk_("L", "T", &n2, k, alpha, &a[(n1 + 1) * a_dim1 + 1],
			     lda, beta, &c__[2], &n1);
		    dgemm_("T", "N", &n1, &n2, k, alpha, &a[a_dim1 + 1], lda,
			    &a[(n1 + 1) * a_dim1 + 1], lda, beta, &c__[n1 *
			    n1 + 1], &n1);

		}

	    } else {

/*              N is odd, TRANSR = 'T', and UPLO = 'U' */

		if (notrans) {

/*                 N is odd, TRANSR = 'T', UPLO = 'U', and TRANS = 'N' */

		    dsyrk_("U", "N", &n1, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[n2 * n2 + 1], &n2);
		    dsyrk_("L", "N", &n2, k, alpha, &a[n1 + 1 + a_dim1], lda,
			    beta, &c__[n1 * n2 + 1], &n2);
		    dgemm_("N", "T", &n2, &n1, k, alpha, &a[n1 + 1 + a_dim1],
			    lda, &a[a_dim1 + 1], lda, beta, &c__[1], &n2);

		} else {

/*                 N is odd, TRANSR = 'T', UPLO = 'U', and TRANS = 'T' */

		    dsyrk_("U", "T", &n1, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[n2 * n2 + 1], &n2);
		    dsyrk_("L", "T", &n2, k, alpha, &a[(n1 + 1) * a_dim1 + 1],
			     lda, beta, &c__[n1 * n2 + 1], &n2);
		    dgemm_("T", "N", &n2, &n1, k, alpha, &a[(n1 + 1) * a_dim1
			    + 1], lda, &a[a_dim1 + 1], lda, beta, &c__[1], &
			    n2);

		}

	    }

	}

    } else {

/*        N is even */

	if (normaltransr) {

/*           N is even and TRANSR = 'N' */

	    if (lower) {

/*              N is even, TRANSR = 'N', and UPLO = 'L' */

		if (notrans) {

/*                 N is even, TRANSR = 'N', UPLO = 'L', and TRANS = 'N' */

		    i__1 = *n + 1;
		    dsyrk_("L", "N", &nk, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[2], &i__1);
		    i__1 = *n + 1;
		    dsyrk_("U", "N", &nk, k, alpha, &a[nk + 1 + a_dim1], lda,
			    beta, &c__[1], &i__1);
		    i__1 = *n + 1;
		    dgemm_("N", "T", &nk, &nk, k, alpha, &a[nk + 1 + a_dim1],
			    lda, &a[a_dim1 + 1], lda, beta, &c__[nk + 2], &
			    i__1);

		} else {

/*                 N is even, TRANSR = 'N', UPLO = 'L', and TRANS = 'T' */

		    i__1 = *n + 1;
		    dsyrk_("L", "T", &nk, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[2], &i__1);
		    i__1 = *n + 1;
		    dsyrk_("U", "T", &nk, k, alpha, &a[(nk + 1) * a_dim1 + 1],
			     lda, beta, &c__[1], &i__1);
		    i__1 = *n + 1;
		    dgemm_("T", "N", &nk, &nk, k, alpha, &a[(nk + 1) * a_dim1
			    + 1], lda, &a[a_dim1 + 1], lda, beta, &c__[nk + 2], &i__1);

		}

	    } else {

/*              N is even, TRANSR = 'N', and UPLO = 'U' */

		if (notrans) {

/*                 N is even, TRANSR = 'N', UPLO = 'U', and TRANS = 'N' */

		    i__1 = *n + 1;
		    dsyrk_("L", "N", &nk, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[nk + 2], &i__1);
		    i__1 = *n + 1;
		    dsyrk_("U", "N", &nk, k, alpha, &a[nk + 1 + a_dim1], lda,
			    beta, &c__[nk + 1], &i__1);
		    i__1 = *n + 1;
		    dgemm_("N", "T", &nk, &nk, k, alpha, &a[a_dim1 + 1], lda,
			    &a[nk + 1 + a_dim1], lda, beta, &c__[1], &i__1);

		} else {

/*                 N is even, TRANSR = 'N', UPLO = 'U', and TRANS = 'T' */

		    i__1 = *n + 1;
		    dsyrk_("L", "T", &nk, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[nk + 2], &i__1);
		    i__1 = *n + 1;
		    dsyrk_("U", "T", &nk, k, alpha, &a[(nk + 1) * a_dim1 + 1],
			     lda, beta, &c__[nk + 1], &i__1);
		    i__1 = *n + 1;
		    dgemm_("T", "N", &nk, &nk, k, alpha, &a[a_dim1 + 1], lda,
			    &a[(nk + 1) * a_dim1 + 1], lda, beta, &c__[1], &i__1);

		}

	    }

	} else {

/*           N is even, and TRANSR = 'T' */

	    if (lower) {

/*              N is even, TRANSR = 'T', and UPLO = 'L' */

		if (notrans) {

/*                 N is even, TRANSR = 'T', UPLO = 'L', and TRANS = 'N' */

		    dsyrk_("U", "N", &nk, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[nk + 1], &nk);
		    dsyrk_("L", "N", &nk, k, alpha, &a[nk + 1 + a_dim1], lda,
			    beta, &c__[1], &nk);
		    dgemm_("N", "T", &nk, &nk, k, alpha, &a[a_dim1 + 1], lda,
			    &a[nk + 1 + a_dim1], lda, beta, &c__[(nk + 1) *
			    nk + 1], &nk);

		} else {

/*                 N is even, TRANSR = 'T', UPLO = 'L', and TRANS = 'T' */

		    dsyrk_("U", "T", &nk, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[nk + 1], &nk);
		    dsyrk_("L", "T", &nk, k, alpha, &a[(nk + 1) * a_dim1 + 1],
			     lda, beta, &c__[1], &nk);
		    dgemm_("T", "N", &nk, &nk, k, alpha, &a[a_dim1 + 1], lda,
			    &a[(nk + 1) * a_dim1 + 1], lda, beta, &c__[(nk + 1) * nk + 1], &nk);

		}

	    } else {

/*              N is even, TRANSR = 'T', and UPLO = 'U' */

		if (notrans) {

/*                 N is even, TRANSR = 'T', UPLO = 'U', and TRANS = 'N' */

		    dsyrk_("U", "N", &nk, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[nk * (nk + 1) + 1], &nk);
		    dsyrk_("L", "N", &nk, k, alpha, &a[nk + 1 + a_dim1], lda,
			    beta, &c__[nk * nk + 1], &nk);
		    dgemm_("N", "T", &nk, &nk, k, alpha, &a[nk + 1 + a_dim1],
			    lda, &a[a_dim1 + 1], lda, beta, &c__[1], &nk);

		} else {

/*                 N is even, TRANSR = 'T', UPLO = 'U', and TRANS = 'T' */

		    dsyrk_("U", "T", &nk, k, alpha, &a[a_dim1 + 1], lda, beta,
			     &c__[nk * (nk + 1) + 1], &nk);
		    dsyrk_("L", "T", &nk, k, alpha, &a[(nk + 1) * a_dim1 + 1],
			     lda, beta, &c__[nk * nk + 1], &nk);
		    dgemm_("T", "N", &nk, &nk, k, alpha, &a[(nk + 1) * a_dim1
			    + 1], lda, &a[a_dim1 + 1], lda, beta, &c__[1], &
			    nk);

		}

	    }

	}

    }

    return 0;

/*     End of DSFRK */

} /* dsfrk_ */

#if 0
/* Subroutine */ int dsgesv_(integer *n, integer *nrhs, double *a, integer *lda, integer *ipiv, double *b, integer *ldb,
	double *x, integer *ldx, double *work, float *swork, integer *iter, integer *info)
{
	/* Table of constant values */
	static double c_b10 = -1.;
	static double c_b11 = 1.;
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, work_dim1, work_offset, x_dim1, x_offset, i__1;
    double d__1;


    /* Local variables */
    double cte, eps, anrm, rnrm, xnrm;
    integer i__, ptsa, ptsx, iiter;

/*  -- LAPACK PROTOTYPE driver routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     February 2007 */

/*     .. */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSGESV computes the solution to a real system of linear equations */
/*     A * X = B, */
/*  where A is an N-by-N matrix and X and B are N-by-NRHS matrices. */

/*  DSGESV first attempts to factorize the matrix in SINGLE PRECISION */
/*  and use this factorization within an iterative refinement procedure */
/*  to produce a solution with DOUBLE PRECISION normwise backward error */
/*  quality (see below). If the approach fails the method switches to a */
/*  DOUBLE PRECISION factorization and solve. */

/*  The iterative refinement is not going to be a winning strategy if */
/*  the ratio SINGLE PRECISION performance over DOUBLE PRECISION */
/*  performance is too small. A reasonable strategy should take the */
/*  number of right-hand sides and the size of the matrix into account. */
/*  This might be done with a call to ILAENV in the future. Up to now, we */
/*  always try iterative refinement. */

/*  The iterative refinement process is stopped if */
/*      ITER > ITERMAX */
/*  or for all the RHS we have: */
/*      RNRM < SQRT(N)*XNRM*ANRM*EPS*BWDMAX */
/*  where */
/*      o ITER is the number of the current iteration in the iterative */
/*        refinement process */
/*      o RNRM is the infinity-norm of the residual */
/*      o XNRM is the infinity-norm of the solution */
/*      o ANRM is the infinity-operator-norm of the matrix A */
/*      o EPS is the machine epsilon returned by DLAMCH('Epsilon') */
/*  The value ITERMAX and BWDMAX are fixed to 30 and 1.0D+00 */
/*  respectively. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of linear equations, i.e., the order of the */
/*          matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  A       (input or input/ouptut) DOUBLE PRECISION array, */
/*          dimension (LDA,N) */
/*          On entry, the N-by-N coefficient matrix A. */
/*          On exit, if iterative refinement has been successfully used */
/*          (INFO.EQ.0 and ITER.GE.0, see description below), then A is */
/*          unchanged, if double precision factorization has been used */
/*          (INFO.EQ.0 and ITER.LT.0, see description below), then the */
/*          array A contains the factors L and U from the factorization */
/*          A = P*L*U; the unit diagonal elements of L are not stored. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  IPIV    (output) INTEGER array, dimension (N) */
/*          The pivot indices that define the permutation matrix P; */
/*          row i of the matrix was interchanged with row IPIV(i). */
/*          Corresponds either to the single precision factorization */
/*          (if INFO.EQ.0 and ITER.GE.0) or the double precision */
/*          factorization (if INFO.EQ.0 and ITER.LT.0). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          The N-by-NRHS right hand side matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (output) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          If INFO = 0, the N-by-NRHS solution matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(1,N). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N*NRHS) */
/*          This array is used to hold the residual vectors. */

/*  SWORK   (workspace) REAL array, dimension (N*(N+NRHS)) */
/*          This array is used to use the single precision matrix and the */
/*          right-hand sides or solutions in single precision. */

/*  ITER    (output) INTEGER */
/*          < 0: iterative refinement has failed, double precision */
/*               factorization has been performed */
/*               -1 : the routine fell back to full precision for */
/*                    implementation- or machine-specific reasons */
/*               -2 : narrowing the precision induced an overflow, */
/*                    the routine fell back to full precision */
/*               -3 : failure of SGETRF */
/*               -31: stop the iterative refinement after the 30th */
/*                    iterations */
/*          > 0: iterative refinement has been sucessfully used. */
/*               Returns the number of iterations */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, U(i,i) computed in DOUBLE PRECISION is */
/*                exactly zero.  The factorization has been completed, */
/*                but the factor U is exactly singular, so the solution */
/*                could not be computed. */

/*  ========= */

/*     .. Parameters .. */




/*     .. Local Scalars .. */

/*     .. External Subroutines .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    work_dim1 = *n;
    work_offset = 1 + work_dim1;
    work -= work_offset;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --ipiv;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    --swork;

    /* Function Body */
    *info = 0;
    *iter = 0;

/*     Test the input parameters. */

    if (*n < 0) {
	*info = -1;
    } else if (*nrhs < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -4;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldx < std::max(1_integer,*n)) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSGESV", &i__1);
	return 0;
    }

/*     Quick return if (N.EQ.0). */

    if (*n == 0) {
	return 0;
    }

/*     Skip single precision iterative refinement if a priori slower */
/*     than double precision factorization. */

    if (false) {
	*iter = -1;
	goto L40;
    }

/*     Compute some constants. */

    anrm = dlange_("I", n, n, &a[a_offset], lda, &work[work_offset]);
    eps = dlamch_("Epsilon");
    cte = anrm * eps * sqrt((double) (*n)) * 1.;

/*     Set the indices PTSA, PTSX for referencing SA and SX in SWORK. */

    ptsa = 1;
    ptsx = ptsa + *n * *n;

/*     Convert B from double precision to single precision and store the */
/*     result in SX. */

    dlag2s_(n, nrhs, &b[b_offset], ldb, &swork[ptsx], n, info);

    if (*info != 0) {
	*iter = -2;
	goto L40;
    }

/*     Convert A from double precision to single precision and store the */
/*     result in SA. */

    dlag2s_(n, n, &a[a_offset], lda, &swork[ptsa], n, info);

    if (*info != 0) {
	*iter = -2;
	goto L40;
    }

/*     Compute the LU factorization of SA. */

    sgetrf_(n, n, &swork[ptsa], n, &ipiv[1], info);

    if (*info != 0) {
	*iter = -3;
	goto L40;
    }

/*     Solve the system SA*SX = SB. */

    sgetrs_("No transpose", n, nrhs, &swork[ptsa], n, &ipiv[1], &swork[ptsx],
	    n, info);

/*     Convert SX back to double precision */

    slag2d_(n, nrhs, &swork[ptsx], n, &x[x_offset], ldx, info);

/*     Compute R = B - AX (R is WORK). */

    dlacpy_("All", n, nrhs, &b[b_offset], ldb, &work[work_offset], n);

    dgemm_("No Transpose", "No Transpose", n, nrhs, n, &c_b10, &a[a_offset],
	    lda, &x[x_offset], ldx, &c_b11, &work[work_offset], n);

/*     Check whether the NRHS normwise backward errors satisfy the */
/*     stopping criterion. If yes, set ITER=0 and return. */

    i__1 = *nrhs;
    for (i__ = 1; i__ <= i__1; ++i__) {
	xnrm = (d__1 = x[idamax_(n, &x[i__ * x_dim1 + 1], &c__1) + i__ *
		x_dim1], abs(d__1));
	rnrm = (d__1 = work[idamax_(n, &work[i__ * work_dim1 + 1], &c__1) +
		i__ * work_dim1], abs(d__1));
	if (rnrm > xnrm * cte) {
	    goto L10;
	}
    }

/*     If we are here, the NRHS normwise backward errors satisfy the */
/*     stopping criterion. We are good to exit. */

    *iter = 0;
    return 0;

L10:

    for (iiter = 1; iiter <= 30; ++iiter) {

/*        Convert R (in WORK) from double precision to single precision */
/*        and store the result in SX. */

	dlag2s_(n, nrhs, &work[work_offset], n, &swork[ptsx], n, info);

	if (*info != 0) {
	    *iter = -2;
	    goto L40;
	}

/*        Solve the system SA*SX = SR. */

	sgetrs_("No transpose", n, nrhs, &swork[ptsa], n, &ipiv[1], &swork[
		ptsx], n, info);

/*        Convert SX back to double precision and update the current */
/*        iterate. */

	slag2d_(n, nrhs, &swork[ptsx], n, &work[work_offset], n, info);

	i__1 = *nrhs;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    daxpy_(n, &c_b11, &work[i__ * work_dim1 + 1], &c__1, &x[i__ *
		    x_dim1 + 1], &c__1);
	}

/*        Compute R = B - AX (R is WORK). */

	dlacpy_("All", n, nrhs, &b[b_offset], ldb, &work[work_offset], n);

	dgemm_("No Transpose", "No Transpose", n, nrhs, n, &c_b10, &a[
		a_offset], lda, &x[x_offset], ldx, &c_b11, &work[work_offset],
		 n);

/*        Check whether the NRHS normwise backward errors satisfy the */
/*        stopping criterion. If yes, set ITER=IITER>0 and return. */

	i__1 = *nrhs;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    xnrm = (d__1 = x[idamax_(n, &x[i__ * x_dim1 + 1], &c__1) + i__ *
		    x_dim1], abs(d__1));
	    rnrm = (d__1 = work[idamax_(n, &work[i__ * work_dim1 + 1], &c__1)
		    + i__ * work_dim1], abs(d__1));
	    if (rnrm > xnrm * cte) {
		goto L20;
	    }
	}

/*        If we are here, the NRHS normwise backward errors satisfy the */
/*        stopping criterion, we are good to exit. */

	*iter = iiter;

	return 0;

L20:

/* L30: */
	;
    }

/*     If we are at this place of the code, this is because we have */
/*     performed ITER=ITERMAX iterations and never satisified the */
/*     stopping criterion, set up the ITER flag accordingly and follow up */
/*     on double precision routine. */

    *iter = -31;

L40:

/*     Single-precision iterative refinement failed to converge to a */
/*     satisfactory solution, so we resort to double precision. */

    dgetrf_(n, n, &a[a_offset], lda, &ipiv[1], info);

    if (*info != 0) {
	return 0;
    }

    dlacpy_("All", n, nrhs, &b[b_offset], ldb, &x[x_offset], ldx);
    dgetrs_("No transpose", n, nrhs, &a[a_offset], lda, &ipiv[1], &x[x_offset]
, ldx, info);

    return 0;

/*     End of DSGESV. */

} /* dsgesv_ */
#endif

/* Subroutine */ int dspcon_(const char *uplo, integer *n, double *ap, integer *
	ipiv, double *anorm, double *rcond, double *work, integer
	*iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, ip, kase;
    integer isave[3];
    bool upper;
    double ainvnm;

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

/*  DSPCON estimates the reciprocal of the condition number (in the */
/*  1-norm) of a real symmetric packed matrix A using the factorization */
/*  A = U*D*U**T or A = L*D*L**T computed by DSPTRF. */

/*  An estimate is obtained for norm(inv(A)), and the reciprocal of the */
/*  condition number is computed as RCOND = 1 / (ANORM * norm(inv(A))). */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the details of the factorization are stored */
/*          as an upper or lower triangular matrix. */
/*          = 'U':  Upper triangular, form is A = U*D*U**T; */
/*          = 'L':  Lower triangular, form is A = L*D*L**T. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  AP      (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The block diagonal matrix D and the multipliers used to */
/*          obtain the factor U or L as computed by DSPTRF, stored as a */
/*          packed triangular matrix. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          Details of the interchanges and the block structure of D */
/*          as determined by DSPTRF. */

/*  ANORM   (input) DOUBLE PRECISION */
/*          The 1-norm of the original matrix A. */

/*  RCOND   (output) DOUBLE PRECISION */
/*          The reciprocal of the condition number of the matrix A, */
/*          computed as RCOND = 1/(ANORM * AINVNM), where AINVNM is an */
/*          estimate of the 1-norm of inv(A) computed in this routine. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (2*N) */

/*  IWORK    (workspace) INTEGER array, dimension (N) */

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

/*     Test the input parameters. */

    /* Parameter adjustments */
    --iwork;
    --work;
    --ipiv;
    --ap;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*anorm < 0.) {
	*info = -5;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPCON", &i__1);
	return 0;
    }

/*     Quick return if possible */

    *rcond = 0.;
    if (*n == 0) {
	*rcond = 1.;
	return 0;
    } else if (*anorm <= 0.) {
	return 0;
    }

/*     Check that the diagonal matrix D is nonsingular. */

    if (upper) {

/*        Upper triangular storage: examine D from bottom to top */

	ip = *n * (*n + 1) / 2;
	for (i__ = *n; i__ >= 1; --i__) {
	    if (ipiv[i__] > 0 && ap[ip] == 0.) {
		return 0;
	    }
	    ip -= i__;
/* L10: */
	}
    } else {

/*        Lower triangular storage: examine D from top to bottom. */

	ip = 1;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (ipiv[i__] > 0 && ap[ip] == 0.) {
		return 0;
	    }
	    ip = ip + *n - i__ + 1;
/* L20: */
	}
    }

/*     Estimate the 1-norm of the inverse. */

    kase = 0;
L30:
    dlacn2_(n, &work[*n + 1], &work[1], &iwork[1], &ainvnm, &kase, isave);
    if (kase != 0) {

/*        Multiply by inv(L*D*L') or inv(U*D*U'). */

	dsptrs_(uplo, n, &c__1, &ap[1], &ipiv[1], &work[1], n, info);
	goto L30;
    }

/*     Compute the estimate of the reciprocal condition number. */

    if (ainvnm != 0.) {
	*rcond = 1. / ainvnm / *anorm;
    }

    return 0;

/*     End of DSPCON */

} /* dspcon_ */

/* Subroutine */ int dspev_(const char *jobz, const char *uplo, integer *n, double *
	ap, double *w, double *z__, integer *ldz, double *work,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer z_dim1, z_offset, i__1;
    double d__1;

    /* Local variables */
    double eps;
    integer inde;
    double anrm;
    integer imax;
    double rmin, rmax;
    double sigma;
    integer iinfo;
    bool wantz;
    integer iscale;
    double safmin;
    double bignum;
    integer indtau;
    integer indwrk;
    double smlnum;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSPEV computes all the eigenvalues and, optionally, eigenvectors of a */
/*  real symmetric matrix A in packed storage. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  AP      (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the upper or lower triangle of the symmetric matrix */
/*          A, packed columnwise in a linear array.  The j-th column of A */
/*          is stored in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2*n-j)/2) = A(i,j) for j<=i<=n. */

/*          On exit, AP is overwritten by values generated during the */
/*          reduction to tridiagonal form.  If UPLO = 'U', the diagonal */
/*          and first superdiagonal of the tridiagonal matrix T overwrite */
/*          the corresponding elements of A, and if UPLO = 'L', the */
/*          diagonal and first subdiagonal of T overwrite the */
/*          corresponding elements of A. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, the eigenvalues in ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, N) */
/*          If JOBZ = 'V', then if INFO = 0, Z contains the orthonormal */
/*          eigenvectors of the matrix A, with the i-th column of Z */
/*          holding the eigenvector associated with W(i). */
/*          If JOBZ = 'N', then Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (3*N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = i, the algorithm failed to converge; i */
/*                off-diagonal elements of an intermediate tridiagonal */
/*                form did not converge to zero. */

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
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;

    /* Function Body */
    wantz = lsame_(jobz, "V");

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (lsame_(uplo, "U") || lsame_(uplo,
	    "L"))) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ldz < 1 || wantz && *ldz < *n) {
	*info = -7;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPEV ", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	w[1] = ap[1];
	if (wantz) {
	    z__[z_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
    rmax = sqrt(bignum);

/*     Scale matrix to allowable range, if necessary. */

    anrm = dlansp_("M", uplo, n, &ap[1], &work[1]);
    iscale = 0;
    if (anrm > 0. && anrm < rmin) {
	iscale = 1;
	sigma = rmin / anrm;
    } else if (anrm > rmax) {
	iscale = 1;
	sigma = rmax / anrm;
    }
    if (iscale == 1) {
	i__1 = *n * (*n + 1) / 2;
	dscal_(&i__1, &sigma, &ap[1], &c__1);
    }

/*     Call DSPTRD to reduce symmetric packed matrix to tridiagonal form. */

    inde = 1;
    indtau = inde + *n;
    dsptrd_(uplo, n, &ap[1], &w[1], &work[inde], &work[indtau], &iinfo);

/*     For eigenvalues only, call DSTERF.  For eigenvectors, first call */
/*     DOPGTR to generate the orthogonal matrix, then call DSTEQR. */

    if (! wantz) {
	dsterf_(n, &w[1], &work[inde], info);
    } else {
	indwrk = indtau + *n;
	dopgtr_(uplo, n, &ap[1], &work[indtau], &z__[z_offset], ldz, &work[
		indwrk], &iinfo);
	dsteqr_(jobz, n, &w[1], &work[inde], &z__[z_offset], ldz, &work[
		indtau], info);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

    if (iscale == 1) {
	if (*info == 0) {
	    imax = *n;
	} else {
	    imax = *info - 1;
	}
	d__1 = 1. / sigma;
	dscal_(&imax, &d__1, &w[1], &c__1);
    }

    return 0;

/*     End of DSPEV */

} /* dspev_ */

/* Subroutine */ int dspevd_(const char *jobz, const char *uplo, integer *n, double *
	ap, double *w, double *z__, integer *ldz, double *work,
	integer *lwork, integer *iwork, integer *liwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer z_dim1, z_offset, i__1;
    double d__1;

    /* Local variables */
    double eps;
    integer inde;
    double anrm, rmin, rmax;
    double sigma;
    integer iinfo, lwmin;
    bool wantz;
    integer iscale;
    double safmin;
    double bignum;
    integer indtau;
    integer indwrk, liwmin;
    integer llwork;
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

/*  DSPEVD computes all the eigenvalues and, optionally, eigenvectors */
/*  of a real symmetric matrix A in packed storage. If eigenvectors are */
/*  desired, it uses a divide and conquer algorithm. */

/*  The divide and conquer algorithm makes very mild assumptions about */
/*  floating point arithmetic. It will work on machines with a guard */
/*  digit in add/subtract, or on those binary machines without guard */
/*  digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or */
/*  Cray-2. It could conceivably fail on hexadecimal or decimal machines */
/*  without guard digits, but we know of none. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  AP      (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the upper or lower triangle of the symmetric matrix */
/*          A, packed columnwise in a linear array.  The j-th column of A */
/*          is stored in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2*n-j)/2) = A(i,j) for j<=i<=n. */

/*          On exit, AP is overwritten by values generated during the */
/*          reduction to tridiagonal form.  If UPLO = 'U', the diagonal */
/*          and first superdiagonal of the tridiagonal matrix T overwrite */
/*          the corresponding elements of A, and if UPLO = 'L', the */
/*          diagonal and first subdiagonal of T overwrite the */
/*          corresponding elements of A. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, the eigenvalues in ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, N) */
/*          If JOBZ = 'V', then if INFO = 0, Z contains the orthonormal */
/*          eigenvectors of the matrix A, with the i-th column of Z */
/*          holding the eigenvector associated with W(i). */
/*          If JOBZ = 'N', then Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, */
/*                                         dimension (LWORK) */
/*          On exit, if INFO = 0, WORK(1) returns the required LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          If N <= 1,               LWORK must be at least 1. */
/*          If JOBZ = 'N' and N > 1, LWORK must be at least 2*N. */
/*          If JOBZ = 'V' and N > 1, LWORK must be at least */
/*                                                 1 + 6*N + N**2. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the required sizes of the WORK and IWORK */
/*          arrays, returns these values as the first entries of the WORK */
/*          and IWORK arrays, and no error message related to LWORK or */
/*          LIWORK is issued by XERBLA. */

/*  IWORK   (workspace/output) INTEGER array, dimension (MAX(1,LIWORK)) */
/*          On exit, if INFO = 0, IWORK(1) returns the required LIWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK. */
/*          If JOBZ  = 'N' or N <= 1, LIWORK must be at least 1. */
/*          If JOBZ  = 'V' and N > 1, LIWORK must be at least 3 + 5*N. */

/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates the required sizes of the WORK and */
/*          IWORK arrays, returns these values as the first entries of */
/*          the WORK and IWORK arrays, and no error message related to */
/*          LWORK or LIWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = i, the algorithm failed to converge; i */
/*                off-diagonal elements of an intermediate tridiagonal */
/*                form did not converge to zero. */

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
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;
    --iwork;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    lquery = *lwork == -1 || *liwork == -1;

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (lsame_(uplo, "U") || lsame_(uplo,
	    "L"))) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ldz < 1 || wantz && *ldz < *n) {
	*info = -7;
    }

    if (*info == 0) {
	if (*n <= 1) {
	    liwmin = 1;
	    lwmin = 1;
	} else {
	    if (wantz) {
		liwmin = *n * 5 + 3;
/* Computing 2nd power */
		i__1 = *n;
		lwmin = *n * 6 + 1 + i__1 * i__1;
	    } else {
		liwmin = 1;
		lwmin = *n << 1;
	    }
	}
	iwork[1] = liwmin;
	work[1] = (double) lwmin;

	if (*lwork < lwmin && ! lquery) {
	    *info = -9;
	} else if (*liwork < liwmin && ! lquery) {
	    *info = -11;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPEVD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	w[1] = ap[1];
	if (wantz) {
	    z__[z_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
    rmax = sqrt(bignum);

/*     Scale matrix to allowable range, if necessary. */

    anrm = dlansp_("M", uplo, n, &ap[1], &work[1]);
    iscale = 0;
    if (anrm > 0. && anrm < rmin) {
	iscale = 1;
	sigma = rmin / anrm;
    } else if (anrm > rmax) {
	iscale = 1;
	sigma = rmax / anrm;
    }
    if (iscale == 1) {
	i__1 = *n * (*n + 1) / 2;
	dscal_(&i__1, &sigma, &ap[1], &c__1);
    }

/*     Call DSPTRD to reduce symmetric packed matrix to tridiagonal form. */

    inde = 1;
    indtau = inde + *n;
    dsptrd_(uplo, n, &ap[1], &w[1], &work[inde], &work[indtau], &iinfo);

/*     For eigenvalues only, call DSTERF.  For eigenvectors, first call */
/*     DSTEDC to generate the eigenvector matrix, WORK(INDWRK), of the */
/*     tridiagonal matrix, then call DOPMTR to multiply it by the */
/*     Householder transformations represented in AP. */

    if (! wantz) {
	dsterf_(n, &w[1], &work[inde], info);
    } else {
	indwrk = indtau + *n;
	llwork = *lwork - indwrk + 1;
	dstedc_("I", n, &w[1], &work[inde], &z__[z_offset], ldz, &work[indwrk]
, &llwork, &iwork[1], liwork, info);
	dopmtr_("L", uplo, "N", n, n, &ap[1], &work[indtau], &z__[z_offset],
		ldz, &work[indwrk], &iinfo);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

    if (iscale == 1) {
	d__1 = 1. / sigma;
	dscal_(n, &d__1, &w[1], &c__1);
    }

    work[1] = (double) lwmin;
    iwork[1] = liwmin;
    return 0;

/*     End of DSPEVD */

} /* dspevd_ */

/* Subroutine */ int dspevx_(const char *jobz, const char *range, const char *uplo, integer *n,
	double *ap, double *vl, double *vu, integer *il, integer *
	iu, double *abstol, integer *m, double *w, double *z__,
	integer *ldz, double *work, integer *iwork, integer *ifail,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer z_dim1, z_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j, jj;
    double eps, vll, vuu, tmp1;
    integer indd, inde;
    double anrm;
    integer imax;
    double rmin, rmax;
    bool test;
    integer itmp1, indee;
    double sigma;
    integer iinfo;
    char order[1];
    bool wantz;
    bool alleig, indeig;
    integer iscale, indibl;
    bool valeig;
    double safmin;
    double abstll, bignum;
    integer indtau, indisp;
    integer indiwo;
    integer indwrk;
    integer nsplit;
    double smlnum;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSPEVX computes selected eigenvalues and, optionally, eigenvectors */
/*  of a real symmetric matrix A in packed storage.  Eigenvalues/vectors */
/*  can be selected by specifying either a range of values or a range of */
/*  indices for the desired eigenvalues. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  RANGE   (input) CHARACTER*1 */
/*          = 'A': all eigenvalues will be found; */
/*          = 'V': all eigenvalues in the half-open interval (VL,VU] */
/*                 will be found; */
/*          = 'I': the IL-th through IU-th eigenvalues will be found. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  AP      (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the upper or lower triangle of the symmetric matrix */
/*          A, packed columnwise in a linear array.  The j-th column of A */
/*          is stored in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2*n-j)/2) = A(i,j) for j<=i<=n. */

/*          On exit, AP is overwritten by values generated during the */
/*          reduction to tridiagonal form.  If UPLO = 'U', the diagonal */
/*          and first superdiagonal of the tridiagonal matrix T overwrite */
/*          the corresponding elements of A, and if UPLO = 'L', the */
/*          diagonal and first subdiagonal of T overwrite the */
/*          corresponding elements of A. */

/*  VL      (input) DOUBLE PRECISION */
/*  VU      (input) DOUBLE PRECISION */
/*          If RANGE='V', the lower and upper bounds of the interval to */
/*          be searched for eigenvalues. VL < VU. */
/*          Not referenced if RANGE = 'A' or 'I'. */

/*  IL      (input) INTEGER */
/*  IU      (input) INTEGER */
/*          If RANGE='I', the indices (in ascending order) of the */
/*          smallest and largest eigenvalues to be returned. */
/*          1 <= IL <= IU <= N, if N > 0; IL = 1 and IU = 0 if N = 0. */
/*          Not referenced if RANGE = 'A' or 'V'. */

/*  ABSTOL  (input) DOUBLE PRECISION */
/*          The absolute error tolerance for the eigenvalues. */
/*          An approximate eigenvalue is accepted as converged */
/*          when it is determined to lie in an interval [a,b] */
/*          of width less than or equal to */

/*                  ABSTOL + EPS *   max( |a|,|b| ) , */

/*          where EPS is the machine precision.  If ABSTOL is less than */
/*          or equal to zero, then  EPS*|T|  will be used in its place, */
/*          where |T| is the 1-norm of the tridiagonal matrix obtained */
/*          by reducing AP to tridiagonal form. */

/*          Eigenvalues will be computed most accurately when ABSTOL is */
/*          set to twice the underflow threshold 2*DLAMCH('S'), not zero. */
/*          If this routine returns with INFO>0, indicating that some */
/*          eigenvectors did not converge, try setting ABSTOL to */
/*          2*DLAMCH('S'). */

/*          See "Computing Small Singular Values of Bidiagonal Matrices */
/*          with Guaranteed High Relative Accuracy," by Demmel and */
/*          Kahan, LAPACK Working Note #3. */

/*  M       (output) INTEGER */
/*          The total number of eigenvalues found.  0 <= M <= N. */
/*          If RANGE = 'A', M = N, and if RANGE = 'I', M = IU-IL+1. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, the selected eigenvalues in ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, max(1,M)) */
/*          If JOBZ = 'V', then if INFO = 0, the first M columns of Z */
/*          contain the orthonormal eigenvectors of the matrix A */
/*          corresponding to the selected eigenvalues, with the i-th */
/*          column of Z holding the eigenvector associated with W(i). */
/*          If an eigenvector fails to converge, then that column of Z */
/*          contains the latest approximation to the eigenvector, and the */
/*          index of the eigenvector is returned in IFAIL. */
/*          If JOBZ = 'N', then Z is not referenced. */
/*          Note: the user must ensure that at least max(1,M) columns are */
/*          supplied in the array Z; if RANGE = 'V', the exact value of M */
/*          is not known in advance and an upper bound must be used. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (8*N) */

/*  IWORK   (workspace) INTEGER array, dimension (5*N) */

/*  IFAIL   (output) INTEGER array, dimension (N) */
/*          If JOBZ = 'V', then if INFO = 0, the first M elements of */
/*          IFAIL are zero.  If INFO > 0, then IFAIL contains the */
/*          indices of the eigenvectors that failed to converge. */
/*          If JOBZ = 'N', then IFAIL is not referenced. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, then i eigenvectors failed to converge. */
/*                Their indices are stored in array IFAIL. */

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
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;
    --iwork;
    --ifail;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    alleig = lsame_(range, "A");
    valeig = lsame_(range, "V");
    indeig = lsame_(range, "I");

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (alleig || valeig || indeig)) {
	*info = -2;
    } else if (! (lsame_(uplo, "L") || lsame_(uplo,
	    "U"))) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else {
	if (valeig) {
	    if (*n > 0 && *vu <= *vl) {
		*info = -7;
	    }
	} else if (indeig) {
	    if (*il < 1 || *il > std::max(1_integer,*n)) {
		*info = -8;
	    } else if (*iu < std::min(*n,*il) || *iu > *n) {
		*info = -9;
	    }
	}
    }
    if (*info == 0) {
	if (*ldz < 1 || wantz && *ldz < *n) {
	    *info = -14;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPEVX", &i__1);
	return 0;
    }

/*     Quick return if possible */

    *m = 0;
    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (alleig || indeig) {
	    *m = 1;
	    w[1] = ap[1];
	} else {
	    if (*vl < ap[1] && *vu >= ap[1]) {
		*m = 1;
		w[1] = ap[1];
	    }
	}
	if (wantz) {
	    z__[z_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
/* Computing MIN */
    d__1 = sqrt(bignum), d__2 = 1. / sqrt(sqrt(safmin));
    rmax = std::min(d__1,d__2);

/*     Scale matrix to allowable range, if necessary. */

    iscale = 0;
    abstll = *abstol;
    if (valeig) {
	vll = *vl;
	vuu = *vu;
    } else {
	vll = 0.;
	vuu = 0.;
    }
    anrm = dlansp_("M", uplo, n, &ap[1], &work[1]);
    if (anrm > 0. && anrm < rmin) {
	iscale = 1;
	sigma = rmin / anrm;
    } else if (anrm > rmax) {
	iscale = 1;
	sigma = rmax / anrm;
    }
    if (iscale == 1) {
	i__1 = *n * (*n + 1) / 2;
	dscal_(&i__1, &sigma, &ap[1], &c__1);
	if (*abstol > 0.) {
	    abstll = *abstol * sigma;
	}
	if (valeig) {
	    vll = *vl * sigma;
	    vuu = *vu * sigma;
	}
    }

/*     Call DSPTRD to reduce symmetric packed matrix to tridiagonal form. */

    indtau = 1;
    inde = indtau + *n;
    indd = inde + *n;
    indwrk = indd + *n;
    dsptrd_(uplo, n, &ap[1], &work[indd], &work[inde], &work[indtau], &iinfo);

/*     If all eigenvalues are desired and ABSTOL is less than or equal */
/*     to zero, then call DSTERF or DOPGTR and SSTEQR.  If this fails */
/*     for some eigenvalue, then try DSTEBZ. */

    test = false;
    if (indeig) {
	if (*il == 1 && *iu == *n) {
	    test = true;
	}
    }
    if ((alleig || test) && *abstol <= 0.) {
	dcopy_(n, &work[indd], &c__1, &w[1], &c__1);
	indee = indwrk + (*n << 1);
	if (! wantz) {
	    i__1 = *n - 1;
	    dcopy_(&i__1, &work[inde], &c__1, &work[indee], &c__1);
	    dsterf_(n, &w[1], &work[indee], info);
	} else {
	    dopgtr_(uplo, n, &ap[1], &work[indtau], &z__[z_offset], ldz, &
		    work[indwrk], &iinfo);
	    i__1 = *n - 1;
	    dcopy_(&i__1, &work[inde], &c__1, &work[indee], &c__1);
	    dsteqr_(jobz, n, &w[1], &work[indee], &z__[z_offset], ldz, &work[
		    indwrk], info);
	    if (*info == 0) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    ifail[i__] = 0;
/* L10: */
		}
	    }
	}
	if (*info == 0) {
	    *m = *n;
	    goto L20;
	}
	*info = 0;
    }

/*     Otherwise, call DSTEBZ and, if eigenvectors are desired, SSTEIN. */

    if (wantz) {
	*(unsigned char *)order = 'B';
    } else {
	*(unsigned char *)order = 'E';
    }
    indibl = 1;
    indisp = indibl + *n;
    indiwo = indisp + *n;
    dstebz_(range, order, n, &vll, &vuu, il, iu, &abstll, &work[indd], &work[
	    inde], m, &nsplit, &w[1], &iwork[indibl], &iwork[indisp], &work[
	    indwrk], &iwork[indiwo], info);

    if (wantz) {
	dstein_(n, &work[indd], &work[inde], m, &w[1], &iwork[indibl], &iwork[
		indisp], &z__[z_offset], ldz, &work[indwrk], &iwork[indiwo], &
		ifail[1], info);

/*        Apply orthogonal matrix used in reduction to tridiagonal */
/*        form to eigenvectors returned by DSTEIN. */

	dopmtr_("L", uplo, "N", n, m, &ap[1], &work[indtau], &z__[z_offset],
		ldz, &work[indwrk], info);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

L20:
    if (iscale == 1) {
	if (*info == 0) {
	    imax = *m;
	} else {
	    imax = *info - 1;
	}
	d__1 = 1. / sigma;
	dscal_(&imax, &d__1, &w[1], &c__1);
    }

/*     If eigenvalues are not in order, then sort them, along with */
/*     eigenvectors. */

    if (wantz) {
	i__1 = *m - 1;
	for (j = 1; j <= i__1; ++j) {
	    i__ = 0;
	    tmp1 = w[j];
	    i__2 = *m;
	    for (jj = j + 1; jj <= i__2; ++jj) {
		if (w[jj] < tmp1) {
		    i__ = jj;
		    tmp1 = w[jj];
		}
/* L30: */
	    }

	    if (i__ != 0) {
		itmp1 = iwork[indibl + i__ - 1];
		w[i__] = w[j];
		iwork[indibl + i__ - 1] = iwork[indibl + j - 1];
		w[j] = tmp1;
		iwork[indibl + j - 1] = itmp1;
		dswap_(n, &z__[i__ * z_dim1 + 1], &c__1, &z__[j * z_dim1 + 1],
			 &c__1);
		if (*info != 0) {
		    itmp1 = ifail[i__];
		    ifail[i__] = ifail[j];
		    ifail[j] = itmp1;
		}
	    }
/* L40: */
	}
    }

    return 0;

/*     End of DSPEVX */

} /* dspevx_ */

/* Subroutine */ int dspgst_(integer *itype, const char *uplo, integer *n,
	double *ap, double *bp, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b9 = -1.;
	static double c_b11 = 1.;

    /* System generated locals */
    integer i__1, i__2;
    double d__1;

    /* Local variables */
    integer j, k, j1, k1, jj, kk;
    double ct, ajj;
    integer j1j1;
    double akk;
    integer k1k1;
    double bjj, bkk;
    bool upper;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSPGST reduces a real symmetric-definite generalized eigenproblem */
/*  to standard form, using packed storage. */

/*  If ITYPE = 1, the problem is A*x = lambda*B*x, */
/*  and A is overwritten by inv(U**T)*A*inv(U) or inv(L)*A*inv(L**T) */

/*  If ITYPE = 2 or 3, the problem is A*B*x = lambda*x or */
/*  B*A*x = lambda*x, and A is overwritten by U*A*U**T or L**T*A*L. */

/*  B must have been previously factorized as U**T*U or L*L**T by DPPTRF. */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          = 1: compute inv(U**T)*A*inv(U) or inv(L)*A*inv(L**T); */
/*          = 2 or 3: compute U*A*U**T or L**T*A*L. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored and B is factored as */
/*                  U**T*U; */
/*          = 'L':  Lower triangle of A is stored and B is factored as */
/*                  L*L**T. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B.  N >= 0. */

/*  AP      (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the upper or lower triangle of the symmetric matrix */
/*          A, packed columnwise in a linear array.  The j-th column of A */
/*          is stored in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2n-j)/2) = A(i,j) for j<=i<=n. */

/*          On exit, if INFO = 0, the transformed matrix, stored in the */
/*          same format as A. */

/*  BP      (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The triangular factor from the Cholesky factorization of B, */
/*          stored in the same format as A, as returned by DPPTRF. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

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

/*     Test the input parameters. */

    /* Parameter adjustments */
    --bp;
    --ap;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    if (*itype < 1 || *itype > 3) {
	*info = -1;
    } else if (! upper && ! lsame_(uplo, "L")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPGST", &i__1);
	return 0;
    }

    if (*itype == 1) {
	if (upper) {

/*           Compute inv(U')*A*inv(U) */

/*           J1 and JJ are the indices of A(1,j) and A(j,j) */

	    jj = 0;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		j1 = jj + 1;
		jj += j;

/*              Compute the j-th column of the upper triangle of A */

		bjj = bp[jj];
		dtpsv_(uplo, "Transpose", "Nonunit", &j, &bp[1], &ap[j1], &
			c__1);
		i__2 = j - 1;
		dspmv_(uplo, &i__2, &c_b9, &ap[1], &bp[j1], &c__1, &c_b11, &
			ap[j1], &c__1);
		i__2 = j - 1;
		d__1 = 1. / bjj;
		dscal_(&i__2, &d__1, &ap[j1], &c__1);
		i__2 = j - 1;
		ap[jj] = (ap[jj] - ddot_(&i__2, &ap[j1], &c__1, &bp[j1], &
			c__1)) / bjj;
/* L10: */
	    }
	} else {

/*           Compute inv(L)*A*inv(L') */

/*           KK and K1K1 are the indices of A(k,k) and A(k+1,k+1) */

	    kk = 1;
	    i__1 = *n;
	    for (k = 1; k <= i__1; ++k) {
		k1k1 = kk + *n - k + 1;

/*              Update the lower triangle of A(k:n,k:n) */

		akk = ap[kk];
		bkk = bp[kk];
/* Computing 2nd power */
		d__1 = bkk;
		akk /= d__1 * d__1;
		ap[kk] = akk;
		if (k < *n) {
		    i__2 = *n - k;
		    d__1 = 1. / bkk;
		    dscal_(&i__2, &d__1, &ap[kk + 1], &c__1);
		    ct = akk * -.5;
		    i__2 = *n - k;
		    daxpy_(&i__2, &ct, &bp[kk + 1], &c__1, &ap[kk + 1], &c__1)
			    ;
		    i__2 = *n - k;
		    dspr2_(uplo, &i__2, &c_b9, &ap[kk + 1], &c__1, &bp[kk + 1]
, &c__1, &ap[k1k1]);
		    i__2 = *n - k;
		    daxpy_(&i__2, &ct, &bp[kk + 1], &c__1, &ap[kk + 1], &c__1)
			    ;
		    i__2 = *n - k;
		    dtpsv_(uplo, "No transpose", "Non-unit", &i__2, &bp[k1k1],
			     &ap[kk + 1], &c__1);
		}
		kk = k1k1;
/* L20: */
	    }
	}
    } else {
	if (upper) {

/*           Compute U*A*U' */

/*           K1 and KK are the indices of A(1,k) and A(k,k) */

	    kk = 0;
	    i__1 = *n;
	    for (k = 1; k <= i__1; ++k) {
		k1 = kk + 1;
		kk += k;

/*              Update the upper triangle of A(1:k,1:k) */

		akk = ap[kk];
		bkk = bp[kk];
		i__2 = k - 1;
		dtpmv_(uplo, "No transpose", "Non-unit", &i__2, &bp[1], &ap[
			k1], &c__1);
		ct = akk * .5;
		i__2 = k - 1;
		daxpy_(&i__2, &ct, &bp[k1], &c__1, &ap[k1], &c__1);
		i__2 = k - 1;
		dspr2_(uplo, &i__2, &c_b11, &ap[k1], &c__1, &bp[k1], &c__1, &
			ap[1]);
		i__2 = k - 1;
		daxpy_(&i__2, &ct, &bp[k1], &c__1, &ap[k1], &c__1);
		i__2 = k - 1;
		dscal_(&i__2, &bkk, &ap[k1], &c__1);
/* Computing 2nd power */
		d__1 = bkk;
		ap[kk] = akk * (d__1 * d__1);
/* L30: */
	    }
	} else {

/*           Compute L'*A*L */

/*           JJ and J1J1 are the indices of A(j,j) and A(j+1,j+1) */

	    jj = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		j1j1 = jj + *n - j + 1;

/*              Compute the j-th column of the lower triangle of A */

		ajj = ap[jj];
		bjj = bp[jj];
		i__2 = *n - j;
		ap[jj] = ajj * bjj + ddot_(&i__2, &ap[jj + 1], &c__1, &bp[jj
			+ 1], &c__1);
		i__2 = *n - j;
		dscal_(&i__2, &bjj, &ap[jj + 1], &c__1);
		i__2 = *n - j;
		dspmv_(uplo, &i__2, &c_b11, &ap[j1j1], &bp[jj + 1], &c__1, &
			c_b11, &ap[jj + 1], &c__1);
		i__2 = *n - j + 1;
		dtpmv_(uplo, "Transpose", "Non-unit", &i__2, &bp[jj], &ap[jj],
			 &c__1);
		jj = j1j1;
/* L40: */
	    }
	}
    }
    return 0;

/*     End of DSPGST */

} /* dspgst_ */

/* Subroutine */ int dspgv_(integer *itype, const char *jobz, const char *uplo, integer *
	n, double *ap, double *bp, double *w, double *z__,
	integer *ldz, double *work, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer z_dim1, z_offset, i__1;

    /* Local variables */
    integer j, neig;
    char trans[1];
    bool upper;
    bool wantz;

/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSPGV computes all the eigenvalues and, optionally, the eigenvectors */
/*  of a real generalized symmetric-definite eigenproblem, of the form */
/*  A*x=(lambda)*B*x,  A*Bx=(lambda)*x,  or B*A*x=(lambda)*x. */
/*  Here A and B are assumed to be symmetric, stored in packed format, */
/*  and B is also positive definite. */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          Specifies the problem type to be solved: */
/*          = 1:  A*x = (lambda)*B*x */
/*          = 2:  A*B*x = (lambda)*x */
/*          = 3:  B*A*x = (lambda)*x */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangles of A and B are stored; */
/*          = 'L':  Lower triangles of A and B are stored. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B.  N >= 0. */

/*  AP      (input/output) DOUBLE PRECISION array, dimension */
/*                            (N*(N+1)/2) */
/*          On entry, the upper or lower triangle of the symmetric matrix */
/*          A, packed columnwise in a linear array.  The j-th column of A */
/*          is stored in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2*n-j)/2) = A(i,j) for j<=i<=n. */

/*          On exit, the contents of AP are destroyed. */

/*  BP      (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the upper or lower triangle of the symmetric matrix */
/*          B, packed columnwise in a linear array.  The j-th column of B */
/*          is stored in the array BP as follows: */
/*          if UPLO = 'U', BP(i + (j-1)*j/2) = B(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', BP(i + (j-1)*(2*n-j)/2) = B(i,j) for j<=i<=n. */

/*          On exit, the triangular factor U or L from the Cholesky */
/*          factorization B = U**T*U or B = L*L**T, in the same storage */
/*          format as B. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, the eigenvalues in ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, N) */
/*          If JOBZ = 'V', then if INFO = 0, Z contains the matrix Z of */
/*          eigenvectors.  The eigenvectors are normalized as follows: */
/*          if ITYPE = 1 or 2, Z**T*B*Z = I; */
/*          if ITYPE = 3, Z**T*inv(B)*Z = I. */
/*          If JOBZ = 'N', then Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (3*N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  DPPTRF or DSPEV returned an error code: */
/*             <= N:  if INFO = i, DSPEV failed to converge; */
/*                    i off-diagonal elements of an intermediate */
/*                    tridiagonal form did not converge to zero. */
/*             > N:   if INFO = n + i, for 1 <= i <= n, then the leading */
/*                    minor of order i of B is not positive definite. */
/*                    The factorization of B could not be completed and */
/*                    no eigenvalues or eigenvectors were computed. */

/*  ===================================================================== */

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
    --bp;
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    upper = lsame_(uplo, "U");

    *info = 0;
    if (*itype < 1 || *itype > 3) {
	*info = -1;
    } else if (! (wantz || lsame_(jobz, "N"))) {
	*info = -2;
    } else if (! (upper || lsame_(uplo, "L"))) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*ldz < 1 || wantz && *ldz < *n) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPGV ", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Form a Cholesky factorization of B. */

    dpptrf_(uplo, n, &bp[1], info);
    if (*info != 0) {
	*info = *n + *info;
	return 0;
    }

/*     Transform problem to standard eigenvalue problem and solve. */

    dspgst_(itype, uplo, n, &ap[1], &bp[1], info);
    dspev_(jobz, uplo, n, &ap[1], &w[1], &z__[z_offset], ldz, &work[1], info);

    if (wantz) {

/*        Backtransform eigenvectors to the original problem. */

	neig = *n;
	if (*info > 0) {
	    neig = *info - 1;
	}
	if (*itype == 1 || *itype == 2) {

/*           For A*x=(lambda)*B*x and A*B*x=(lambda)*x; */
/*           backtransform eigenvectors: x = inv(L)'*y or inv(U)*y */

	    if (upper) {
		*(unsigned char *)trans = 'N';
	    } else {
		*(unsigned char *)trans = 'T';
	    }

	    i__1 = neig;
	    for (j = 1; j <= i__1; ++j) {
		dtpsv_(uplo, trans, "Non-unit", n, &bp[1], &z__[j * z_dim1 +
			1], &c__1);
/* L10: */
	    }

	} else if (*itype == 3) {

/*           For B*A*x=(lambda)*x; */
/*           backtransform eigenvectors: x = L*y or U'*y */

	    if (upper) {
		*(unsigned char *)trans = 'T';
	    } else {
		*(unsigned char *)trans = 'N';
	    }

	    i__1 = neig;
	    for (j = 1; j <= i__1; ++j) {
		dtpmv_(uplo, trans, "Non-unit", n, &bp[1], &z__[j * z_dim1 +
			1], &c__1);
/* L20: */
	    }
	}
    }
    return 0;

/*     End of DSPGV */

} /* dspgv_ */

/* Subroutine */ int dspgvd_(integer *itype, const char *jobz, const char *uplo, integer *
	n, double *ap, double *bp, double *w, double *z__,
	integer *ldz, double *work, integer *lwork, integer *iwork,
	integer *liwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer z_dim1, z_offset, i__1;
    double d__1, d__2;

    /* Local variables */
    integer j, neig;
    integer lwmin;
    char trans[1];
    bool upper;
    bool wantz;
    integer liwmin;
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

/*  DSPGVD computes all the eigenvalues, and optionally, the eigenvectors */
/*  of a real generalized symmetric-definite eigenproblem, of the form */
/*  A*x=(lambda)*B*x,  A*Bx=(lambda)*x,  or B*A*x=(lambda)*x.  Here A and */
/*  B are assumed to be symmetric, stored in packed format, and B is also */
/*  positive definite. */
/*  If eigenvectors are desired, it uses a divide and conquer algorithm. */

/*  The divide and conquer algorithm makes very mild assumptions about */
/*  floating point arithmetic. It will work on machines with a guard */
/*  digit in add/subtract, or on those binary machines without guard */
/*  digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or */
/*  Cray-2. It could conceivably fail on hexadecimal or decimal machines */
/*  without guard digits, but we know of none. */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          Specifies the problem type to be solved: */
/*          = 1:  A*x = (lambda)*B*x */
/*          = 2:  A*B*x = (lambda)*x */
/*          = 3:  B*A*x = (lambda)*x */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangles of A and B are stored; */
/*          = 'L':  Lower triangles of A and B are stored. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B.  N >= 0. */

/*  AP      (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the upper or lower triangle of the symmetric matrix */
/*          A, packed columnwise in a linear array.  The j-th column of A */
/*          is stored in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2*n-j)/2) = A(i,j) for j<=i<=n. */

/*          On exit, the contents of AP are destroyed. */

/*  BP      (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the upper or lower triangle of the symmetric matrix */
/*          B, packed columnwise in a linear array.  The j-th column of B */
/*          is stored in the array BP as follows: */
/*          if UPLO = 'U', BP(i + (j-1)*j/2) = B(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', BP(i + (j-1)*(2*n-j)/2) = B(i,j) for j<=i<=n. */

/*          On exit, the triangular factor U or L from the Cholesky */
/*          factorization B = U**T*U or B = L*L**T, in the same storage */
/*          format as B. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, the eigenvalues in ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, N) */
/*          If JOBZ = 'V', then if INFO = 0, Z contains the matrix Z of */
/*          eigenvectors.  The eigenvectors are normalized as follows: */
/*          if ITYPE = 1 or 2, Z**T*B*Z = I; */
/*          if ITYPE = 3, Z**T*inv(B)*Z = I. */
/*          If JOBZ = 'N', then Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the required LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          If N <= 1,               LWORK >= 1. */
/*          If JOBZ = 'N' and N > 1, LWORK >= 2*N. */
/*          If JOBZ = 'V' and N > 1, LWORK >= 1 + 6*N + 2*N**2. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the required sizes of the WORK and IWORK */
/*          arrays, returns these values as the first entries of the WORK */
/*          and IWORK arrays, and no error message related to LWORK or */
/*          LIWORK is issued by XERBLA. */

/*  IWORK   (workspace/output) INTEGER array, dimension (MAX(1,LIWORK)) */
/*          On exit, if INFO = 0, IWORK(1) returns the required LIWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK. */
/*          If JOBZ  = 'N' or N <= 1, LIWORK >= 1. */
/*          If JOBZ  = 'V' and N > 1, LIWORK >= 3 + 5*N. */

/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates the required sizes of the WORK and */
/*          IWORK arrays, returns these values as the first entries of */
/*          the WORK and IWORK arrays, and no error message related to */
/*          LWORK or LIWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  DPPTRF or DSPEVD returned an error code: */
/*             <= N:  if INFO = i, DSPEVD failed to converge; */
/*                    i off-diagonal elements of an intermediate */
/*                    tridiagonal form did not converge to zero; */
/*             > N:   if INFO = N + i, for 1 <= i <= N, then the leading */
/*                    minor of order i of B is not positive definite. */
/*                    The factorization of B could not be completed and */
/*                    no eigenvalues or eigenvectors were computed. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Mark Fahey, Department of Mathematics, Univ. of Kentucky, USA */

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
    --bp;
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;
    --iwork;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    upper = lsame_(uplo, "U");
    lquery = *lwork == -1 || *liwork == -1;

    *info = 0;
    if (*itype < 1 || *itype > 3) {
	*info = -1;
    } else if (! (wantz || lsame_(jobz, "N"))) {
	*info = -2;
    } else if (! (upper || lsame_(uplo, "L"))) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*ldz < 1 || wantz && *ldz < *n) {
	*info = -9;
    }

    if (*info == 0) {
	if (*n <= 1) {
	    liwmin = 1;
	    lwmin = 1;
	} else {
	    if (wantz) {
		liwmin = *n * 5 + 3;
/* Computing 2nd power */
		i__1 = *n;
		lwmin = *n * 6 + 1 + (i__1 * i__1 << 1);
	    } else {
		liwmin = 1;
		lwmin = *n << 1;
	    }
	}
	work[1] = (double) lwmin;
	iwork[1] = liwmin;

	if (*lwork < lwmin && ! lquery) {
	    *info = -11;
	} else if (*liwork < liwmin && ! lquery) {
	    *info = -13;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPGVD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Form a Cholesky factorization of BP. */

    dpptrf_(uplo, n, &bp[1], info);
    if (*info != 0) {
	*info = *n + *info;
	return 0;
    }

/*     Transform problem to standard eigenvalue problem and solve. */

    dspgst_(itype, uplo, n, &ap[1], &bp[1], info);
    dspevd_(jobz, uplo, n, &ap[1], &w[1], &z__[z_offset], ldz, &work[1],
	    lwork, &iwork[1], liwork, info);
/* Computing MAX */
    d__1 = (double) lwmin;
    lwmin = (integer) std::max(d__1,work[1]);
/* Computing MAX */
    d__1 = (double) liwmin, d__2 = (double) iwork[1];
    liwmin = (integer) std::max(d__1,d__2);

    if (wantz) {

/*        Backtransform eigenvectors to the original problem. */

	neig = *n;
	if (*info > 0) {
	    neig = *info - 1;
	}
	if (*itype == 1 || *itype == 2) {

/*           For A*x=(lambda)*B*x and A*B*x=(lambda)*x; */
/*           backtransform eigenvectors: x = inv(L)'*y or inv(U)*y */

	    if (upper) {
		*(unsigned char *)trans = 'N';
	    } else {
		*(unsigned char *)trans = 'T';
	    }

	    i__1 = neig;
	    for (j = 1; j <= i__1; ++j) {
		dtpsv_(uplo, trans, "Non-unit", n, &bp[1], &z__[j * z_dim1 +
			1], &c__1);
/* L10: */
	    }

	} else if (*itype == 3) {

/*           For B*A*x=(lambda)*x; */
/*           backtransform eigenvectors: x = L*y or U'*y */

	    if (upper) {
		*(unsigned char *)trans = 'T';
	    } else {
		*(unsigned char *)trans = 'N';
	    }

	    i__1 = neig;
	    for (j = 1; j <= i__1; ++j) {
		dtpmv_(uplo, trans, "Non-unit", n, &bp[1], &z__[j * z_dim1 +
			1], &c__1);
/* L20: */
	    }
	}
    }

    work[1] = (double) lwmin;
    iwork[1] = liwmin;

    return 0;

/*     End of DSPGVD */

} /* dspgvd_ */

/* Subroutine */ int dspgvx_(integer *itype, const char *jobz, const char *range, const char *
	uplo, integer *n, double *ap, double *bp, double *vl,
	double *vu, integer *il, integer *iu, double *abstol, integer
	*m, double *w, double *z__, integer *ldz, double *work,
	integer *iwork, integer *ifail, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer z_dim1, z_offset, i__1;

    /* Local variables */
    integer j;
    char trans[1];
    bool upper;
	bool wantz, alleig, indeig, valeig;

/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSPGVX computes selected eigenvalues, and optionally, eigenvectors */
/*  of a real generalized symmetric-definite eigenproblem, of the form */
/*  A*x=(lambda)*B*x,  A*Bx=(lambda)*x,  or B*A*x=(lambda)*x.  Here A */
/*  and B are assumed to be symmetric, stored in packed storage, and B */
/*  is also positive definite.  Eigenvalues and eigenvectors can be */
/*  selected by specifying either a range of values or a range of indices */
/*  for the desired eigenvalues. */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          Specifies the problem type to be solved: */
/*          = 1:  A*x = (lambda)*B*x */
/*          = 2:  A*B*x = (lambda)*x */
/*          = 3:  B*A*x = (lambda)*x */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  RANGE   (input) CHARACTER*1 */
/*          = 'A': all eigenvalues will be found. */
/*          = 'V': all eigenvalues in the half-open interval (VL,VU] */
/*                 will be found. */
/*          = 'I': the IL-th through IU-th eigenvalues will be found. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A and B are stored; */
/*          = 'L':  Lower triangle of A and B are stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix pencil (A,B).  N >= 0. */

/*  AP      (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the upper or lower triangle of the symmetric matrix */
/*          A, packed columnwise in a linear array.  The j-th column of A */
/*          is stored in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2*n-j)/2) = A(i,j) for j<=i<=n. */

/*          On exit, the contents of AP are destroyed. */

/*  BP      (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the upper or lower triangle of the symmetric matrix */
/*          B, packed columnwise in a linear array.  The j-th column of B */
/*          is stored in the array BP as follows: */
/*          if UPLO = 'U', BP(i + (j-1)*j/2) = B(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', BP(i + (j-1)*(2*n-j)/2) = B(i,j) for j<=i<=n. */

/*          On exit, the triangular factor U or L from the Cholesky */
/*          factorization B = U**T*U or B = L*L**T, in the same storage */
/*          format as B. */

/*  VL      (input) DOUBLE PRECISION */
/*  VU      (input) DOUBLE PRECISION */
/*          If RANGE='V', the lower and upper bounds of the interval to */
/*          be searched for eigenvalues. VL < VU. */
/*          Not referenced if RANGE = 'A' or 'I'. */

/*  IL      (input) INTEGER */
/*  IU      (input) INTEGER */
/*          If RANGE='I', the indices (in ascending order) of the */
/*          smallest and largest eigenvalues to be returned. */
/*          1 <= IL <= IU <= N, if N > 0; IL = 1 and IU = 0 if N = 0. */
/*          Not referenced if RANGE = 'A' or 'V'. */

/*  ABSTOL  (input) DOUBLE PRECISION */
/*          The absolute error tolerance for the eigenvalues. */
/*          An approximate eigenvalue is accepted as converged */
/*          when it is determined to lie in an interval [a,b] */
/*          of width less than or equal to */

/*                  ABSTOL + EPS *   max( |a|,|b| ) , */

/*          where EPS is the machine precision.  If ABSTOL is less than */
/*          or equal to zero, then  EPS*|T|  will be used in its place, */
/*          where |T| is the 1-norm of the tridiagonal matrix obtained */
/*          by reducing A to tridiagonal form. */

/*          Eigenvalues will be computed most accurately when ABSTOL is */
/*          set to twice the underflow threshold 2*DLAMCH('S'), not zero. */
/*          If this routine returns with INFO>0, indicating that some */
/*          eigenvectors did not converge, try setting ABSTOL to */
/*          2*DLAMCH('S'). */

/*  M       (output) INTEGER */
/*          The total number of eigenvalues found.  0 <= M <= N. */
/*          If RANGE = 'A', M = N, and if RANGE = 'I', M = IU-IL+1. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          On normal exit, the first M elements contain the selected */
/*          eigenvalues in ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, max(1,M)) */
/*          If JOBZ = 'N', then Z is not referenced. */
/*          If JOBZ = 'V', then if INFO = 0, the first M columns of Z */
/*          contain the orthonormal eigenvectors of the matrix A */
/*          corresponding to the selected eigenvalues, with the i-th */
/*          column of Z holding the eigenvector associated with W(i). */
/*          The eigenvectors are normalized as follows: */
/*          if ITYPE = 1 or 2, Z**T*B*Z = I; */
/*          if ITYPE = 3, Z**T*inv(B)*Z = I. */

/*          If an eigenvector fails to converge, then that column of Z */
/*          contains the latest approximation to the eigenvector, and the */
/*          index of the eigenvector is returned in IFAIL. */
/*          Note: the user must ensure that at least max(1,M) columns are */
/*          supplied in the array Z; if RANGE = 'V', the exact value of M */
/*          is not known in advance and an upper bound must be used. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (8*N) */

/*  IWORK   (workspace) INTEGER array, dimension (5*N) */

/*  IFAIL   (output) INTEGER array, dimension (N) */
/*          If JOBZ = 'V', then if INFO = 0, the first M elements of */
/*          IFAIL are zero.  If INFO > 0, then IFAIL contains the */
/*          indices of the eigenvectors that failed to converge. */
/*          If JOBZ = 'N', then IFAIL is not referenced. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  DPPTRF or DSPEVX returned an error code: */
/*             <= N:  if INFO = i, DSPEVX failed to converge; */
/*                    i eigenvectors failed to converge.  Their indices */
/*                    are stored in array IFAIL. */
/*             > N:   if INFO = N + i, for 1 <= i <= N, then the leading */
/*                    minor of order i of B is not positive definite. */
/*                    The factorization of B could not be completed and */
/*                    no eigenvalues or eigenvectors were computed. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Mark Fahey, Department of Mathematics, Univ. of Kentucky, USA */

/* ===================================================================== */

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
    --bp;
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;
    --iwork;
    --ifail;

    /* Function Body */
    upper = lsame_(uplo, "U");
    wantz = lsame_(jobz, "V");
    alleig = lsame_(range, "A");
    valeig = lsame_(range, "V");
    indeig = lsame_(range, "I");

    *info = 0;
    if (*itype < 1 || *itype > 3) {
	*info = -1;
    } else if (! (wantz || lsame_(jobz, "N"))) {
	*info = -2;
    } else if (! (alleig || valeig || indeig)) {
	*info = -3;
    } else if (! (upper || lsame_(uplo, "L"))) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    } else {
	if (valeig) {
	    if (*n > 0 && *vu <= *vl) {
		*info = -9;
	    }
	} else if (indeig) {
	    if (*il < 1) {
		*info = -10;
	    } else if (*iu < std::min(*n,*il) || *iu > *n) {
		*info = -11;
	    }
	}
    }
    if (*info == 0) {
	if (*ldz < 1 || wantz && *ldz < *n) {
	    *info = -16;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPGVX", &i__1);
	return 0;
    }

/*     Quick return if possible */

    *m = 0;
    if (*n == 0) {
	return 0;
    }

/*     Form a Cholesky factorization of B. */

    dpptrf_(uplo, n, &bp[1], info);
    if (*info != 0) {
	*info = *n + *info;
	return 0;
    }

/*     Transform problem to standard eigenvalue problem and solve. */

    dspgst_(itype, uplo, n, &ap[1], &bp[1], info);
    dspevx_(jobz, range, uplo, n, &ap[1], vl, vu, il, iu, abstol, m, &w[1], &
	    z__[z_offset], ldz, &work[1], &iwork[1], &ifail[1], info);

    if (wantz) {

/*        Backtransform eigenvectors to the original problem. */

	if (*info > 0) {
	    *m = *info - 1;
	}
	if (*itype == 1 || *itype == 2) {

/*           For A*x=(lambda)*B*x and A*B*x=(lambda)*x; */
/*           backtransform eigenvectors: x = inv(L)'*y or inv(U)*y */

	    if (upper) {
		*(unsigned char *)trans = 'N';
	    } else {
		*(unsigned char *)trans = 'T';
	    }

	    i__1 = *m;
	    for (j = 1; j <= i__1; ++j) {
		dtpsv_(uplo, trans, "Non-unit", n, &bp[1], &z__[j * z_dim1 +
			1], &c__1);
/* L10: */
	    }

	} else if (*itype == 3) {

/*           For B*A*x=(lambda)*x; */
/*           backtransform eigenvectors: x = L*y or U'*y */

	    if (upper) {
		*(unsigned char *)trans = 'T';
	    } else {
		*(unsigned char *)trans = 'N';
	    }

	    i__1 = *m;
	    for (j = 1; j <= i__1; ++j) {
		dtpmv_(uplo, trans, "Non-unit", n, &bp[1], &z__[j * z_dim1 +
			1], &c__1);
/* L20: */
	    }
	}
    }

    return 0;

/*     End of DSPGVX */

} /* dspgvx_ */

/* Subroutine */ int dsprfs_(const char *uplo, integer *n, integer *nrhs,
	double *ap, double *afp, integer *ipiv, double *b,
	integer *ldb, double *x, integer *ldx, double *ferr,
	double *berr, double *work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b12 = -1.;
	static double c_b14 = 1.;

    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, i__1, i__2, i__3;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, k;
    double s;
    integer ik, kk;
    double xk;
    integer nz;
    double eps;
    integer kase;
    double safe1, safe2;
    integer isave[3];
    integer count;
    bool upper;
    double safmin;
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

/*  DSPRFS improves the computed solution to a system of linear */
/*  equations when the coefficient matrix is symmetric indefinite */
/*  and packed, and provides error bounds and backward error estimates */
/*  for the solution. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X.  NRHS >= 0. */

/*  AP      (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The upper or lower triangle of the symmetric matrix A, packed */
/*          columnwise in a linear array.  The j-th column of A is stored */
/*          in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2*n-j)/2) = A(i,j) for j<=i<=n. */

/*  AFP     (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The factored form of the matrix A.  AFP contains the block */
/*          diagonal matrix D and the multipliers used to obtain the */
/*          factor U or L from the factorization A = U*D*U**T or */
/*          A = L*D*L**T as computed by DSPTRF, stored as a packed */
/*          triangular matrix. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          Details of the interchanges and the block structure of D */
/*          as determined by DSPTRF. */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          The right hand side matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (input/output) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          On entry, the solution matrix X, as computed by DSPTRS. */
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
    --ap;
    --afp;
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
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -8;
    } else if (*ldx < std::max(1_integer,*n)) {
	*info = -10;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPRFS", &i__1);
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

/*        Compute residual R = B - A * X */

	dcopy_(n, &b[j * b_dim1 + 1], &c__1, &work[*n + 1], &c__1);
	dspmv_(uplo, n, &c_b12, &ap[1], &x[j * x_dim1 + 1], &c__1, &c_b14, &
		work[*n + 1], &c__1);

/*        Compute componentwise relative backward error from formula */

/*        max(i) ( abs(R(i)) / ( abs(A)*abs(X) + abs(B) )(i) ) */

/*        where abs(Z) is the componentwise absolute value of the matrix */
/*        or vector Z.  If the i-th component of the denominator is less */
/*        than SAFE2, then SAFE1 is added to the i-th components of the */
/*        numerator and denominator before dividing. */

	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[i__] = (d__1 = b[i__ + j * b_dim1], abs(d__1));
/* L30: */
	}

/*        Compute abs(A)*abs(X) + abs(B). */

	kk = 1;
	if (upper) {
	    i__2 = *n;
	    for (k = 1; k <= i__2; ++k) {
		s = 0.;
		xk = (d__1 = x[k + j * x_dim1], abs(d__1));
		ik = kk;
		i__3 = k - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    work[i__] += (d__1 = ap[ik], abs(d__1)) * xk;
		    s += (d__1 = ap[ik], abs(d__1)) * (d__2 = x[i__ + j *
			    x_dim1], abs(d__2));
		    ++ik;
/* L40: */
		}
		work[k] = work[k] + (d__1 = ap[kk + k - 1], abs(d__1)) * xk +
			s;
		kk += k;
/* L50: */
	    }
	} else {
	    i__2 = *n;
	    for (k = 1; k <= i__2; ++k) {
		s = 0.;
		xk = (d__1 = x[k + j * x_dim1], abs(d__1));
		work[k] += (d__1 = ap[kk], abs(d__1)) * xk;
		ik = kk + 1;
		i__3 = *n;
		for (i__ = k + 1; i__ <= i__3; ++i__) {
		    work[i__] += (d__1 = ap[ik], abs(d__1)) * xk;
		    s += (d__1 = ap[ik], abs(d__1)) * (d__2 = x[i__ + j *
			    x_dim1], abs(d__2));
		    ++ik;
/* L60: */
		}
		work[k] += s;
		kk += *n - k + 1;
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

	    dsptrs_(uplo, n, &c__1, &afp[1], &ipiv[1], &work[*n + 1], n, info);
	    daxpy_(n, &c_b14, &work[*n + 1], &c__1, &x[j * x_dim1 + 1], &c__1)
		    ;
	    lstres = berr[j];
	    ++count;
	    goto L20;
	}

/*        Bound error from formula */

/*        norm(X - XTRUE) / norm(X) .le. FERR = */
/*        norm( abs(inv(A))* */
/*           ( abs(R) + NZ*EPS*( abs(A)*abs(X)+abs(B) ))) / norm(X) */

/*        where */
/*          norm(Z) is the magnitude of the largest component of Z */
/*          inv(A) is the inverse of A */
/*          abs(Z) is the componentwise absolute value of the matrix or */
/*             vector Z */
/*          NZ is the maximum number of nonzeros in any row of A, plus 1 */
/*          EPS is machine epsilon */

/*        The i-th component of abs(R)+NZ*EPS*(abs(A)*abs(X)+abs(B)) */
/*        is incremented by SAFE1 if the i-th component of */
/*        abs(A)*abs(X) + abs(B) is less than SAFE2. */

/*        Use DLACN2 to estimate the infinity-norm of the matrix */
/*           inv(A) * diag(W), */
/*        where W = abs(R) + NZ*EPS*( abs(A)*abs(X)+abs(B) ))) */

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

/*              Multiply by diag(W)*inv(A'). */

		dsptrs_(uplo, n, &c__1, &afp[1], &ipiv[1], &work[*n + 1], n,
			info);
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = work[i__] * work[*n + i__];
/* L110: */
		}
	    } else if (kase == 2) {

/*              Multiply by inv(A)*diag(W). */

		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = work[i__] * work[*n + i__];
/* L120: */
		}
		dsptrs_(uplo, n, &c__1, &afp[1], &ipiv[1], &work[*n + 1], n,
			info);
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

/*     End of DSPRFS */

} /* dsprfs_ */

/* Subroutine */ int dspsv_(const char *uplo, integer *n, integer *nrhs, double
	*ap, integer *ipiv, double *b, integer *ldb, integer *info)
{
    /* System generated locals */
    integer b_dim1, b_offset, i__1;

/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSPSV computes the solution to a real system of linear equations */
/*     A * X = B, */
/*  where A is an N-by-N symmetric matrix stored in packed format and X */
/*  and B are N-by-NRHS matrices. */

/*  The diagonal pivoting method is used to factor A as */
/*     A = U * D * U**T,  if UPLO = 'U', or */
/*     A = L * D * L**T,  if UPLO = 'L', */
/*  where U (or L) is a product of permutation and unit upper (lower) */
/*  triangular matrices, D is symmetric and block diagonal with 1-by-1 */
/*  and 2-by-2 diagonal blocks.  The factored form of A is then used to */
/*  solve the system of equations A * X = B. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The number of linear equations, i.e., the order of the */
/*          matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  AP      (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the upper or lower triangle of the symmetric matrix */
/*          A, packed columnwise in a linear array.  The j-th column of A */
/*          is stored in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2n-j)/2) = A(i,j) for j<=i<=n. */
/*          See below for further details. */

/*          On exit, the block diagonal matrix D and the multipliers used */
/*          to obtain the factor U or L from the factorization */
/*          A = U*D*U**T or A = L*D*L**T as computed by DSPTRF, stored as */
/*          a packed triangular matrix in the same storage format as A. */

/*  IPIV    (output) INTEGER array, dimension (N) */
/*          Details of the interchanges and the block structure of D, as */
/*          determined by DSPTRF.  If IPIV(k) > 0, then rows and columns */
/*          k and IPIV(k) were interchanged, and D(k,k) is a 1-by-1 */
/*          diagonal block.  If UPLO = 'U' and IPIV(k) = IPIV(k-1) < 0, */
/*          then rows and columns k-1 and -IPIV(k) were interchanged and */
/*          D(k-1:k,k-1:k) is a 2-by-2 diagonal block.  If UPLO = 'L' and */
/*          IPIV(k) = IPIV(k+1) < 0, then rows and columns k+1 and */
/*          -IPIV(k) were interchanged and D(k:k+1,k:k+1) is a 2-by-2 */
/*          diagonal block. */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the N-by-NRHS right hand side matrix B. */
/*          On exit, if INFO = 0, the N-by-NRHS solution matrix X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, D(i,i) is exactly zero.  The factorization */
/*                has been completed, but the block diagonal matrix D is */
/*                exactly singular, so the solution could not be */
/*                computed. */

/*  Further Details */
/*  =============== */

/*  The packed storage scheme is illustrated by the following example */
/*  when N = 4, UPLO = 'U': */

/*  Two-dimensional storage of the symmetric matrix A: */

/*     a11 a12 a13 a14 */
/*         a22 a23 a24 */
/*             a33 a34     (aij = aji) */
/*                 a44 */

/*  Packed storage of the upper triangle of A: */

/*  AP = [ a11, a12, a22, a13, a23, a33, a14, a24, a34, a44 ] */

/*  ===================================================================== */

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
    --ipiv;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    *info = 0;
    if (! lsame_(uplo, "U") && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -7;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPSV ", &i__1);
	return 0;
    }

/*     Compute the factorization A = U*D*U' or A = L*D*L'. */

    dsptrf_(uplo, n, &ap[1], &ipiv[1], info);
    if (*info == 0) {

/*        Solve the system A*X = B, overwriting B with X. */

	dsptrs_(uplo, n, nrhs, &ap[1], &ipiv[1], &b[b_offset], ldb, info);

    }
    return 0;

/*     End of DSPSV */

} /* dspsv_ */

/* Subroutine */ int dspsvx_(const char *fact, const char *uplo, integer *n, integer *
	nrhs, double *ap, double *afp, integer *ipiv, double *b,
	integer *ldb, double *x, integer *ldx, double *rcond,
	double *ferr, double *berr, double *work, integer *iwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, i__1;

    /* Local variables */
    double anorm;
    bool nofact;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSPSVX uses the diagonal pivoting factorization A = U*D*U**T or */
/*  A = L*D*L**T to compute the solution to a real system of linear */
/*  equations A * X = B, where A is an N-by-N symmetric matrix stored */
/*  in packed format and X and B are N-by-NRHS matrices. */

/*  Error bounds on the solution and a condition estimate are also */
/*  provided. */

/*  Description */
/*  =========== */

/*  The following steps are performed: */

/*  1. If FACT = 'N', the diagonal pivoting method is used to factor A as */
/*        A = U * D * U**T,  if UPLO = 'U', or */
/*        A = L * D * L**T,  if UPLO = 'L', */
/*     where U (or L) is a product of permutation and unit upper (lower) */
/*     triangular matrices and D is symmetric and block diagonal with */
/*     1-by-1 and 2-by-2 diagonal blocks. */

/*  2. If some D(i,i)=0, so that D is exactly singular, then the routine */
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
/*          = 'F':  On entry, AFP and IPIV contain the factored form of */
/*                  A.  AP, AFP and IPIV will not be modified. */
/*          = 'N':  The matrix A will be copied to AFP and factored. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The number of linear equations, i.e., the order of the */
/*          matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X.  NRHS >= 0. */

/*  AP      (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The upper or lower triangle of the symmetric matrix A, packed */
/*          columnwise in a linear array.  The j-th column of A is stored */
/*          in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2*n-j)/2) = A(i,j) for j<=i<=n. */
/*          See below for further details. */

/*  AFP     (input or output) DOUBLE PRECISION array, dimension */
/*                            (N*(N+1)/2) */
/*          If FACT = 'F', then AFP is an input argument and on entry */
/*          contains the block diagonal matrix D and the multipliers used */
/*          to obtain the factor U or L from the factorization */
/*          A = U*D*U**T or A = L*D*L**T as computed by DSPTRF, stored as */
/*          a packed triangular matrix in the same storage format as A. */

/*          If FACT = 'N', then AFP is an output argument and on exit */
/*          contains the block diagonal matrix D and the multipliers used */
/*          to obtain the factor U or L from the factorization */
/*          A = U*D*U**T or A = L*D*L**T as computed by DSPTRF, stored as */
/*          a packed triangular matrix in the same storage format as A. */

/*  IPIV    (input or output) INTEGER array, dimension (N) */
/*          If FACT = 'F', then IPIV is an input argument and on entry */
/*          contains details of the interchanges and the block structure */
/*          of D, as determined by DSPTRF. */
/*          If IPIV(k) > 0, then rows and columns k and IPIV(k) were */
/*          interchanged and D(k,k) is a 1-by-1 diagonal block. */
/*          If UPLO = 'U' and IPIV(k) = IPIV(k-1) < 0, then rows and */
/*          columns k-1 and -IPIV(k) were interchanged and D(k-1:k,k-1:k) */
/*          is a 2-by-2 diagonal block.  If UPLO = 'L' and IPIV(k) = */
/*          IPIV(k+1) < 0, then rows and columns k+1 and -IPIV(k) were */
/*          interchanged and D(k:k+1,k:k+1) is a 2-by-2 diagonal block. */

/*          If FACT = 'N', then IPIV is an output argument and on exit */
/*          contains details of the interchanges and the block structure */
/*          of D, as determined by DSPTRF. */

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
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, and i is */
/*                <= N:  D(i,i) is exactly zero.  The factorization */
/*                       has been completed but the factor D is exactly */
/*                       singular, so the solution and error bounds could */
/*                       not be computed. RCOND = 0 is returned. */
/*                = N+1: D is nonsingular, but RCOND is less than machine */
/*                       precision, meaning that the matrix is singular */
/*                       to working precision.  Nevertheless, the */
/*                       solution and error bounds are computed because */
/*                       there are a number of situations where the */
/*                       computed solution can be more accurate than the */
/*                       value of RCOND would suggest. */

/*  Further Details */
/*  =============== */

/*  The packed storage scheme is illustrated by the following example */
/*  when N = 4, UPLO = 'U': */

/*  Two-dimensional storage of the symmetric matrix A: */

/*     a11 a12 a13 a14 */
/*         a22 a23 a24 */
/*             a33 a34     (aij = aji) */
/*                 a44 */

/*  Packed storage of the upper triangle of A: */

/*  AP = [ a11, a12, a22, a13, a23, a33, a14, a24, a34, a44 ] */

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
    --afp;
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
    if (! nofact && ! lsame_(fact, "F")) {
	*info = -1;
    } else if (! lsame_(uplo, "U") && ! lsame_(uplo,
	    "L")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*nrhs < 0) {
	*info = -4;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -9;
    } else if (*ldx < std::max(1_integer,*n)) {
	*info = -11;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPSVX", &i__1);
	return 0;
    }

    if (nofact) {

/*        Compute the factorization A = U*D*U' or A = L*D*L'. */

	i__1 = *n * (*n + 1) / 2;
	dcopy_(&i__1, &ap[1], &c__1, &afp[1], &c__1);
	dsptrf_(uplo, n, &afp[1], &ipiv[1], info);

/*        Return if INFO is non-zero. */

	if (*info > 0) {
	    *rcond = 0.;
	    return 0;
	}
    }

/*     Compute the norm of the matrix A. */

    anorm = dlansp_("I", uplo, n, &ap[1], &work[1]);

/*     Compute the reciprocal of the condition number of A. */

    dspcon_(uplo, n, &afp[1], &ipiv[1], &anorm, rcond, &work[1], &iwork[1],
	    info);

/*     Compute the solution vectors X. */

    dlacpy_("Full", n, nrhs, &b[b_offset], ldb, &x[x_offset], ldx);
    dsptrs_(uplo, n, nrhs, &afp[1], &ipiv[1], &x[x_offset], ldx, info);

/*     Use iterative refinement to improve the computed solutions and */
/*     compute error bounds and backward error estimates for them. */

    dsprfs_(uplo, n, nrhs, &ap[1], &afp[1], &ipiv[1], &b[b_offset], ldb, &x[
	    x_offset], ldx, &ferr[1], &berr[1], &work[1], &iwork[1], info);

/*     Set INFO = N+1 if the matrix is singular to working precision. */

    if (*rcond < dlamch_("Epsilon")) {
	*info = *n + 1;
    }

    return 0;

/*     End of DSPSVX */

} /* dspsvx_ */

/* Subroutine */ int dsptrd_(const char *uplo, integer *n, double *ap,
	double *d__, double *e, double *tau, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b8 = 0.;
	static double c_b14 = -1.;

    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer i__, i1, ii, i1i1;
    double taui;
    double alpha;
    bool upper;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSPTRD reduces a real symmetric matrix A stored in packed form to */
/*  symmetric tridiagonal form T by an orthogonal similarity */
/*  transformation: Q**T * A * Q = T. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  AP      (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the upper or lower triangle of the symmetric matrix */
/*          A, packed columnwise in a linear array.  The j-th column of A */
/*          is stored in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2*n-j)/2) = A(i,j) for j<=i<=n. */
/*          On exit, if UPLO = 'U', the diagonal and first superdiagonal */
/*          of A are overwritten by the corresponding elements of the */
/*          tridiagonal matrix T, and the elements above the first */
/*          superdiagonal, with the array TAU, represent the orthogonal */
/*          matrix Q as a product of elementary reflectors; if UPLO */
/*          = 'L', the diagonal and first subdiagonal of A are over- */
/*          written by the corresponding elements of the tridiagonal */
/*          matrix T, and the elements below the first subdiagonal, with */
/*          the array TAU, represent the orthogonal matrix Q as a product */
/*          of elementary reflectors. See Further Details. */

/*  D       (output) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal elements of the tridiagonal matrix T: */
/*          D(i) = A(i,i). */

/*  E       (output) DOUBLE PRECISION array, dimension (N-1) */
/*          The off-diagonal elements of the tridiagonal matrix T: */
/*          E(i) = A(i,i+1) if UPLO = 'U', E(i) = A(i+1,i) if UPLO = 'L'. */

/*  TAU     (output) DOUBLE PRECISION array, dimension (N-1) */
/*          The scalar factors of the elementary reflectors (see Further */
/*          Details). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */

/*  Further Details */
/*  =============== */

/*  If UPLO = 'U', the matrix Q is represented as a product of elementary */
/*  reflectors */

/*     Q = H(n-1) . . . H(2) H(1). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(i+1:n) = 0 and v(i) = 1; v(1:i-1) is stored on exit in AP, */
/*  overwriting A(1:i-1,i+1), and tau is stored in TAU(i). */

/*  If UPLO = 'L', the matrix Q is represented as a product of elementary */
/*  reflectors */

/*     Q = H(1) H(2) . . . H(n-1). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(1:i) = 0 and v(i+1) = 1; v(i+2:n) is stored on exit in AP, */
/*  overwriting A(i+2:n,i), and tau is stored in TAU(i). */

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

/*     Test the input parameters */

    /* Parameter adjustments */
    --tau;
    --e;
    --d__;
    --ap;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPTRD", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 0) {
	return 0;
    }

    if (upper) {

/*        Reduce the upper triangle of A. */
/*        I1 is the index in AP of A(1,I+1). */

	i1 = *n * (*n - 1) / 2 + 1;
	for (i__ = *n - 1; i__ >= 1; --i__) {

/*           Generate elementary reflector H(i) = I - tau * v * v' */
/*           to annihilate A(1:i-1,i+1) */

	    dlarfg_(&i__, &ap[i1 + i__ - 1], &ap[i1], &c__1, &taui);
	    e[i__] = ap[i1 + i__ - 1];

	    if (taui != 0.) {

/*              Apply H(i) from both sides to A(1:i,1:i) */

		ap[i1 + i__ - 1] = 1.;

/*              Compute  y := tau * A * v  storing y in TAU(1:i) */

		dspmv_(uplo, &i__, &taui, &ap[1], &ap[i1], &c__1, &c_b8, &tau[
			1], &c__1);

/*              Compute  w := y - 1/2 * tau * (y'*v) * v */

		alpha = taui * -.5 * ddot_(&i__, &tau[1], &c__1, &ap[i1], &
			c__1);
		daxpy_(&i__, &alpha, &ap[i1], &c__1, &tau[1], &c__1);

/*              Apply the transformation as a rank-2 update: */
/*                 A := A - v * w' - w * v' */

		dspr2_(uplo, &i__, &c_b14, &ap[i1], &c__1, &tau[1], &c__1, &
			ap[1]);

		ap[i1 + i__ - 1] = e[i__];
	    }
	    d__[i__ + 1] = ap[i1 + i__];
	    tau[i__] = taui;
	    i1 -= i__;
/* L10: */
	}
	d__[1] = ap[1];
    } else {

/*        Reduce the lower triangle of A. II is the index in AP of */
/*        A(i,i) and I1I1 is the index of A(i+1,i+1). */

	ii = 1;
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i1i1 = ii + *n - i__ + 1;

/*           Generate elementary reflector H(i) = I - tau * v * v' */
/*           to annihilate A(i+2:n,i) */

	    i__2 = *n - i__;
	    dlarfg_(&i__2, &ap[ii + 1], &ap[ii + 2], &c__1, &taui);
	    e[i__] = ap[ii + 1];

	    if (taui != 0.) {

/*              Apply H(i) from both sides to A(i+1:n,i+1:n) */

		ap[ii + 1] = 1.;

/*              Compute  y := tau * A * v  storing y in TAU(i:n-1) */

		i__2 = *n - i__;
		dspmv_(uplo, &i__2, &taui, &ap[i1i1], &ap[ii + 1], &c__1, &
			c_b8, &tau[i__], &c__1);

/*              Compute  w := y - 1/2 * tau * (y'*v) * v */

		i__2 = *n - i__;
		alpha = taui * -.5 * ddot_(&i__2, &tau[i__], &c__1, &ap[ii +
			1], &c__1);
		i__2 = *n - i__;
		daxpy_(&i__2, &alpha, &ap[ii + 1], &c__1, &tau[i__], &c__1);

/*              Apply the transformation as a rank-2 update: */
/*                 A := A - v * w' - w * v' */

		i__2 = *n - i__;
		dspr2_(uplo, &i__2, &c_b14, &ap[ii + 1], &c__1, &tau[i__], &
			c__1, &ap[i1i1]);

		ap[ii + 1] = e[i__];
	    }
	    d__[i__] = ap[ii];
	    tau[i__] = taui;
	    ii = i1i1;
/* L20: */
	}
	d__[*n] = ap[ii];
    }

    return 0;

/*     End of DSPTRD */

} /* dsptrd_ */

/* Subroutine */ int dsptrf_(const char *uplo, integer *n, double *ap, integer *
	ipiv, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer i__1, i__2;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, k;
    double t, r1, d11, d12, d21, d22;
    integer kc, kk, kp;
    double wk;
    integer kx, knc, kpc, npp;
    double wkm1, wkp1;
    integer imax, jmax;
    double alpha;
    integer kstep;
    bool upper;
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

/*  DSPTRF computes the factorization of a real symmetric matrix A stored */
/*  in packed format using the Bunch-Kaufman diagonal pivoting method: */

/*     A = U*D*U**T  or  A = L*D*L**T */

/*  where U (or L) is a product of permutation and unit upper (lower) */
/*  triangular matrices, and D is symmetric and block diagonal with */
/*  1-by-1 and 2-by-2 diagonal blocks. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  AP      (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the upper or lower triangle of the symmetric matrix */
/*          A, packed columnwise in a linear array.  The j-th column of A */
/*          is stored in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2n-j)/2) = A(i,j) for j<=i<=n. */

/*          On exit, the block diagonal matrix D and the multipliers used */
/*          to obtain the factor U or L, stored as a packed triangular */
/*          matrix overwriting A (see below for further details). */

/*  IPIV    (output) INTEGER array, dimension (N) */
/*          Details of the interchanges and the block structure of D. */
/*          If IPIV(k) > 0, then rows and columns k and IPIV(k) were */
/*          interchanged and D(k,k) is a 1-by-1 diagonal block. */
/*          If UPLO = 'U' and IPIV(k) = IPIV(k-1) < 0, then rows and */
/*          columns k-1 and -IPIV(k) were interchanged and D(k-1:k,k-1:k) */
/*          is a 2-by-2 diagonal block.  If UPLO = 'L' and IPIV(k) = */
/*          IPIV(k+1) < 0, then rows and columns k+1 and -IPIV(k) were */
/*          interchanged and D(k:k+1,k:k+1) is a 2-by-2 diagonal block. */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          > 0: if INFO = i, D(i,i) is exactly zero.  The factorization */
/*               has been completed, but the block diagonal matrix D is */
/*               exactly singular, and division by zero will occur if it */
/*               is used to solve a system of equations. */

/*  Further Details */
/*  =============== */

/*  5-96 - Based on modifications by J. Lewis, Boeing Computer Services */
/*         Company */

/*  If UPLO = 'U', then A = U*D*U', where */
/*     U = P(n)*U(n)* ... *P(k)U(k)* ..., */
/*  i.e., U is a product of terms P(k)*U(k), where k decreases from n to */
/*  1 in steps of 1 or 2, and D is a block diagonal matrix with 1-by-1 */
/*  and 2-by-2 diagonal blocks D(k).  P(k) is a permutation matrix as */
/*  defined by IPIV(k), and U(k) is a unit upper triangular matrix, such */
/*  that if the diagonal block D(k) is of order s (s = 1 or 2), then */

/*             (   I    v    0   )   k-s */
/*     U(k) =  (   0    I    0   )   s */
/*             (   0    0    I   )   n-k */
/*                k-s   s   n-k */

/*  If s = 1, D(k) overwrites A(k,k), and v overwrites A(1:k-1,k). */
/*  If s = 2, the upper triangle of D(k) overwrites A(k-1,k-1), A(k-1,k), */
/*  and A(k,k), and v overwrites A(1:k-2,k-1:k). */

/*  If UPLO = 'L', then A = L*D*L', where */
/*     L = P(1)*L(1)* ... *P(k)*L(k)* ..., */
/*  i.e., L is a product of terms P(k)*L(k), where k increases from 1 to */
/*  n in steps of 1 or 2, and D is a block diagonal matrix with 1-by-1 */
/*  and 2-by-2 diagonal blocks D(k).  P(k) is a permutation matrix as */
/*  defined by IPIV(k), and L(k) is a unit lower triangular matrix, such */
/*  that if the diagonal block D(k) is of order s (s = 1 or 2), then */

/*             (   I    0     0   )  k-1 */
/*     L(k) =  (   0    I     0   )  s */
/*             (   0    v     I   )  n-k-s+1 */
/*                k-1   s  n-k-s+1 */

/*  If s = 1, D(k) overwrites A(k,k), and v overwrites A(k+1:n,k). */
/*  If s = 2, the lower triangle of D(k) overwrites A(k,k), A(k+1,k), */
/*  and A(k+1,k+1), and v overwrites A(k+2:n,k:k+1). */

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
    --ipiv;
    --ap;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPTRF", &i__1);
	return 0;
    }

/*     Initialize ALPHA for use in choosing pivot block size. */

    alpha = (sqrt(17.) + 1.) / 8.;

    if (upper) {

/*        Factorize A as U*D*U' using the upper triangle of A */

/*        K is the main loop index, decreasing from N to 1 in steps of */
/*        1 or 2 */

	k = *n;
	kc = (*n - 1) * *n / 2 + 1;
L10:
	knc = kc;

/*        If K < 1, exit from loop */

	if (k < 1) {
	    goto L110;
	}
	kstep = 1;

/*        Determine rows and columns to be interchanged and whether */
/*        a 1-by-1 or 2-by-2 pivot block will be used */

	absakk = (d__1 = ap[kc + k - 1], abs(d__1));

/*        IMAX is the row-index of the largest off-diagonal element in */
/*        column K, and COLMAX is its absolute value */

	if (k > 1) {
	    i__1 = k - 1;
	    imax = idamax_(&i__1, &ap[kc], &c__1);
	    colmax = (d__1 = ap[kc + imax - 1], abs(d__1));
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

/*              JMAX is the column-index of the largest off-diagonal */
/*              element in row IMAX, and ROWMAX is its absolute value */

		rowmax = 0.;
		jmax = imax;
		kx = imax * (imax + 1) / 2 + imax;
		i__1 = k;
		for (j = imax + 1; j <= i__1; ++j) {
		    if ((d__1 = ap[kx], abs(d__1)) > rowmax) {
			rowmax = (d__1 = ap[kx], abs(d__1));
			jmax = j;
		    }
		    kx += j;
/* L20: */
		}
		kpc = (imax - 1) * imax / 2 + 1;
		if (imax > 1) {
		    i__1 = imax - 1;
		    jmax = idamax_(&i__1, &ap[kpc], &c__1);
/* Computing MAX */
		    d__2 = rowmax, d__3 = (d__1 = ap[kpc + jmax - 1], abs(
			    d__1));
		    rowmax = std::max(d__2,d__3);
		}

		if (absakk >= alpha * colmax * (colmax / rowmax)) {

/*                 no interchange, use 1-by-1 pivot block */

		    kp = k;
		} else if ((d__1 = ap[kpc + imax - 1], abs(d__1)) >= alpha *
			rowmax) {

/*                 interchange rows and columns K and IMAX, use 1-by-1 */
/*                 pivot block */

		    kp = imax;
		} else {

/*                 interchange rows and columns K-1 and IMAX, use 2-by-2 */
/*                 pivot block */

		    kp = imax;
		    kstep = 2;
		}
	    }

	    kk = k - kstep + 1;
	    if (kstep == 2) {
		knc = knc - k + 1;
	    }
	    if (kp != kk) {

/*              Interchange rows and columns KK and KP in the leading */
/*              submatrix A(1:k,1:k) */

		i__1 = kp - 1;
		dswap_(&i__1, &ap[knc], &c__1, &ap[kpc], &c__1);
		kx = kpc + kp - 1;
		i__1 = kk - 1;
		for (j = kp + 1; j <= i__1; ++j) {
		    kx = kx + j - 1;
		    t = ap[knc + j - 1];
		    ap[knc + j - 1] = ap[kx];
		    ap[kx] = t;
/* L30: */
		}
		t = ap[knc + kk - 1];
		ap[knc + kk - 1] = ap[kpc + kp - 1];
		ap[kpc + kp - 1] = t;
		if (kstep == 2) {
		    t = ap[kc + k - 2];
		    ap[kc + k - 2] = ap[kc + kp - 1];
		    ap[kc + kp - 1] = t;
		}
	    }

/*           Update the leading submatrix */

	    if (kstep == 1) {

/*              1-by-1 pivot block D(k): column k now holds */

/*              W(k) = U(k)*D(k) */

/*              where U(k) is the k-th column of U */

/*              Perform a rank-1 update of A(1:k-1,1:k-1) as */

/*              A := A - U(k)*D(k)*U(k)' = A - W(k)*1/D(k)*W(k)' */

		r1 = 1. / ap[kc + k - 1];
		i__1 = k - 1;
		d__1 = -r1;
		dspr_(uplo, &i__1, &d__1, &ap[kc], &c__1, &ap[1]);

/*              Store U(k) in column k */

		i__1 = k - 1;
		dscal_(&i__1, &r1, &ap[kc], &c__1);
	    } else {

/*              2-by-2 pivot block D(k): columns k and k-1 now hold */

/*              ( W(k-1) W(k) ) = ( U(k-1) U(k) )*D(k) */

/*              where U(k) and U(k-1) are the k-th and (k-1)-th columns */
/*              of U */

/*              Perform a rank-2 update of A(1:k-2,1:k-2) as */

/*              A := A - ( U(k-1) U(k) )*D(k)*( U(k-1) U(k) )' */
/*                 = A - ( W(k-1) W(k) )*inv(D(k))*( W(k-1) W(k) )' */

		if (k > 2) {

		    d12 = ap[k - 1 + (k - 1) * k / 2];
		    d22 = ap[k - 1 + (k - 2) * (k - 1) / 2] / d12;
		    d11 = ap[k + (k - 1) * k / 2] / d12;
		    t = 1. / (d11 * d22 - 1.);
		    d12 = t / d12;

		    for (j = k - 2; j >= 1; --j) {
			wkm1 = d12 * (d11 * ap[j + (k - 2) * (k - 1) / 2] -
				ap[j + (k - 1) * k / 2]);
			wk = d12 * (d22 * ap[j + (k - 1) * k / 2] - ap[j + (k
				- 2) * (k - 1) / 2]);
			for (i__ = j; i__ >= 1; --i__) {
			    ap[i__ + (j - 1) * j / 2] = ap[i__ + (j - 1) * j /
				     2] - ap[i__ + (k - 1) * k / 2] * wk - ap[
				    i__ + (k - 2) * (k - 1) / 2] * wkm1;
/* L40: */
			}
			ap[j + (k - 1) * k / 2] = wk;
			ap[j + (k - 2) * (k - 1) / 2] = wkm1;
/* L50: */
		    }

		}

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
	kc = knc - k;
	goto L10;

    } else {

/*        Factorize A as L*D*L' using the lower triangle of A */

/*        K is the main loop index, increasing from 1 to N in steps of */
/*        1 or 2 */

	k = 1;
	kc = 1;
	npp = *n * (*n + 1) / 2;
L60:
	knc = kc;

/*        If K > N, exit from loop */

	if (k > *n) {
	    goto L110;
	}
	kstep = 1;

/*        Determine rows and columns to be interchanged and whether */
/*        a 1-by-1 or 2-by-2 pivot block will be used */

	absakk = (d__1 = ap[kc], abs(d__1));

/*        IMAX is the row-index of the largest off-diagonal element in */
/*        column K, and COLMAX is its absolute value */

	if (k < *n) {
	    i__1 = *n - k;
	    imax = k + idamax_(&i__1, &ap[kc + 1], &c__1);
	    colmax = (d__1 = ap[kc + imax - k], abs(d__1));
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

/*              JMAX is the column-index of the largest off-diagonal */
/*              element in row IMAX, and ROWMAX is its absolute value */

		rowmax = 0.;
		kx = kc + imax - k;
		i__1 = imax - 1;
		for (j = k; j <= i__1; ++j) {
		    if ((d__1 = ap[kx], abs(d__1)) > rowmax) {
			rowmax = (d__1 = ap[kx], abs(d__1));
			jmax = j;
		    }
		    kx = kx + *n - j;
/* L70: */
		}
		kpc = npp - (*n - imax + 1) * (*n - imax + 2) / 2 + 1;
		if (imax < *n) {
		    i__1 = *n - imax;
		    jmax = imax + idamax_(&i__1, &ap[kpc + 1], &c__1);
/* Computing MAX */
		    d__2 = rowmax, d__3 = (d__1 = ap[kpc + jmax - imax], abs(
			    d__1));
		    rowmax = std::max(d__2,d__3);
		}

		if (absakk >= alpha * colmax * (colmax / rowmax)) {

/*                 no interchange, use 1-by-1 pivot block */

		    kp = k;
		} else if ((d__1 = ap[kpc], abs(d__1)) >= alpha * rowmax) {

/*                 interchange rows and columns K and IMAX, use 1-by-1 */
/*                 pivot block */

		    kp = imax;
		} else {

/*                 interchange rows and columns K+1 and IMAX, use 2-by-2 */
/*                 pivot block */

		    kp = imax;
		    kstep = 2;
		}
	    }

	    kk = k + kstep - 1;
	    if (kstep == 2) {
		knc = knc + *n - k + 1;
	    }
	    if (kp != kk) {

/*              Interchange rows and columns KK and KP in the trailing */
/*              submatrix A(k:n,k:n) */

		if (kp < *n) {
		    i__1 = *n - kp;
		    dswap_(&i__1, &ap[knc + kp - kk + 1], &c__1, &ap[kpc + 1],
			     &c__1);
		}
		kx = knc + kp - kk;
		i__1 = kp - 1;
		for (j = kk + 1; j <= i__1; ++j) {
		    kx = kx + *n - j + 1;
		    t = ap[knc + j - kk];
		    ap[knc + j - kk] = ap[kx];
		    ap[kx] = t;
/* L80: */
		}
		t = ap[knc];
		ap[knc] = ap[kpc];
		ap[kpc] = t;
		if (kstep == 2) {
		    t = ap[kc + 1];
		    ap[kc + 1] = ap[kc + kp - k];
		    ap[kc + kp - k] = t;
		}
	    }

/*           Update the trailing submatrix */

	    if (kstep == 1) {

/*              1-by-1 pivot block D(k): column k now holds */

/*              W(k) = L(k)*D(k) */

/*              where L(k) is the k-th column of L */

		if (k < *n) {

/*                 Perform a rank-1 update of A(k+1:n,k+1:n) as */

/*                 A := A - L(k)*D(k)*L(k)' = A - W(k)*(1/D(k))*W(k)' */

		    r1 = 1. / ap[kc];
		    i__1 = *n - k;
		    d__1 = -r1;
		    dspr_(uplo, &i__1, &d__1, &ap[kc + 1], &c__1, &ap[kc + *n
			    - k + 1]);

/*                 Store L(k) in column K */

		    i__1 = *n - k;
		    dscal_(&i__1, &r1, &ap[kc + 1], &c__1);
		}
	    } else {

/*              2-by-2 pivot block D(k): columns K and K+1 now hold */

/*              ( W(k) W(k+1) ) = ( L(k) L(k+1) )*D(k) */

/*              where L(k) and L(k+1) are the k-th and (k+1)-th columns */
/*              of L */

		if (k < *n - 1) {

/*                 Perform a rank-2 update of A(k+2:n,k+2:n) as */

/*                 A := A - ( L(k) L(k+1) )*D(k)*( L(k) L(k+1) )' */
/*                    = A - ( W(k) W(k+1) )*inv(D(k))*( W(k) W(k+1) )' */

		    d21 = ap[k + 1 + (k - 1) * ((*n << 1) - k) / 2];
		    d11 = ap[k + 1 + k * ((*n << 1) - k - 1) / 2] / d21;
		    d22 = ap[k + (k - 1) * ((*n << 1) - k) / 2] / d21;
		    t = 1. / (d11 * d22 - 1.);
		    d21 = t / d21;

		    i__1 = *n;
		    for (j = k + 2; j <= i__1; ++j) {
			wk = d21 * (d11 * ap[j + (k - 1) * ((*n << 1) - k) /
				2] - ap[j + k * ((*n << 1) - k - 1) / 2]);
			wkp1 = d21 * (d22 * ap[j + k * ((*n << 1) - k - 1) /
				2] - ap[j + (k - 1) * ((*n << 1) - k) / 2]);

			i__2 = *n;
			for (i__ = j; i__ <= i__2; ++i__) {
			    ap[i__ + (j - 1) * ((*n << 1) - j) / 2] = ap[i__
				    + (j - 1) * ((*n << 1) - j) / 2] - ap[i__
				    + (k - 1) * ((*n << 1) - k) / 2] * wk -
				    ap[i__ + k * ((*n << 1) - k - 1) / 2] *
				    wkp1;
/* L90: */
			}

			ap[j + (k - 1) * ((*n << 1) - k) / 2] = wk;
			ap[j + k * ((*n << 1) - k - 1) / 2] = wkp1;

/* L100: */
		    }
		}
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
	kc = knc + *n - k + 2;
	goto L60;

    }

L110:
    return 0;

/*     End of DSPTRF */

} /* dsptrf_ */

/* Subroutine */ int dsptri_(const char *uplo, integer *n, double *ap, integer *
	ipiv, double *work, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b11 = -1.;
	static double c_b13 = 0.;

    /* System generated locals */
    integer i__1;
    double d__1;

    /* Local variables */
    double d__;
    integer j, k;
    double t, ak;
    integer kc, kp, kx, kpc, npp;
    double akp1;
    double temp, akkp1;
    integer kstep;
    bool upper;
    integer kcnext;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSPTRI computes the inverse of a real symmetric indefinite matrix */
/*  A in packed storage using the factorization A = U*D*U**T or */
/*  A = L*D*L**T computed by DSPTRF. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the details of the factorization are stored */
/*          as an upper or lower triangular matrix. */
/*          = 'U':  Upper triangular, form is A = U*D*U**T; */
/*          = 'L':  Lower triangular, form is A = L*D*L**T. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  AP      (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the block diagonal matrix D and the multipliers */
/*          used to obtain the factor U or L as computed by DSPTRF, */
/*          stored as a packed triangular matrix. */

/*          On exit, if INFO = 0, the (symmetric) inverse of the original */
/*          matrix, stored as a packed triangular matrix. The j-th column */
/*          of inv(A) is stored in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = inv(A)(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', */
/*             AP(i + (j-1)*(2n-j)/2) = inv(A)(i,j) for j<=i<=n. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          Details of the interchanges and the block structure of D */
/*          as determined by DSPTRF. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          > 0: if INFO = i, D(i,i) = 0; the matrix is singular and its */
/*               inverse could not be computed. */

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
    --work;
    --ipiv;
    --ap;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPTRI", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Check that the diagonal matrix D is nonsingular. */

    if (upper) {

/*        Upper triangular storage: examine D from bottom to top */

	kp = *n * (*n + 1) / 2;
	for (*info = *n; *info >= 1; --(*info)) {
	    if (ipiv[*info] > 0 && ap[kp] == 0.) {
		return 0;
	    }
	    kp -= *info;
/* L10: */
	}
    } else {

/*        Lower triangular storage: examine D from top to bottom. */

	kp = 1;
	i__1 = *n;
	for (*info = 1; *info <= i__1; ++(*info)) {
	    if (ipiv[*info] > 0 && ap[kp] == 0.) {
		return 0;
	    }
	    kp = kp + *n - *info + 1;
/* L20: */
	}
    }
    *info = 0;

    if (upper) {

/*        Compute inv(A) from the factorization A = U*D*U'. */

/*        K is the main loop index, increasing from 1 to N in steps of */
/*        1 or 2, depending on the size of the diagonal blocks. */

	k = 1;
	kc = 1;
L30:

/*        If K > N, exit from loop. */

	if (k > *n) {
	    goto L50;
	}

	kcnext = kc + k;
	if (ipiv[k] > 0) {

/*           1 x 1 diagonal block */

/*           Invert the diagonal block. */

	    ap[kc + k - 1] = 1. / ap[kc + k - 1];

/*           Compute column K of the inverse. */

	    if (k > 1) {
		i__1 = k - 1;
		dcopy_(&i__1, &ap[kc], &c__1, &work[1], &c__1);
		i__1 = k - 1;
		dspmv_(uplo, &i__1, &c_b11, &ap[1], &work[1], &c__1, &c_b13, &
			ap[kc], &c__1);
		i__1 = k - 1;
		ap[kc + k - 1] -= ddot_(&i__1, &work[1], &c__1, &ap[kc], &
			c__1);
	    }
	    kstep = 1;
	} else {

/*           2 x 2 diagonal block */

/*           Invert the diagonal block. */

	    t = (d__1 = ap[kcnext + k - 1], abs(d__1));
	    ak = ap[kc + k - 1] / t;
	    akp1 = ap[kcnext + k] / t;
	    akkp1 = ap[kcnext + k - 1] / t;
	    d__ = t * (ak * akp1 - 1.);
	    ap[kc + k - 1] = akp1 / d__;
	    ap[kcnext + k] = ak / d__;
	    ap[kcnext + k - 1] = -akkp1 / d__;

/*           Compute columns K and K+1 of the inverse. */

	    if (k > 1) {
		i__1 = k - 1;
		dcopy_(&i__1, &ap[kc], &c__1, &work[1], &c__1);
		i__1 = k - 1;
		dspmv_(uplo, &i__1, &c_b11, &ap[1], &work[1], &c__1, &c_b13, &
			ap[kc], &c__1);
		i__1 = k - 1;
		ap[kc + k - 1] -= ddot_(&i__1, &work[1], &c__1, &ap[kc], &
			c__1);
		i__1 = k - 1;
		ap[kcnext + k - 1] -= ddot_(&i__1, &ap[kc], &c__1, &ap[kcnext]
, &c__1);
		i__1 = k - 1;
		dcopy_(&i__1, &ap[kcnext], &c__1, &work[1], &c__1);
		i__1 = k - 1;
		dspmv_(uplo, &i__1, &c_b11, &ap[1], &work[1], &c__1, &c_b13, &
			ap[kcnext], &c__1);
		i__1 = k - 1;
		ap[kcnext + k] -= ddot_(&i__1, &work[1], &c__1, &ap[kcnext], &
			c__1);
	    }
	    kstep = 2;
	    kcnext = kcnext + k + 1;
	}

	kp = (i__1 = ipiv[k], abs(i__1));
	if (kp != k) {

/*           Interchange rows and columns K and KP in the leading */
/*           submatrix A(1:k+1,1:k+1) */

	    kpc = (kp - 1) * kp / 2 + 1;
	    i__1 = kp - 1;
	    dswap_(&i__1, &ap[kc], &c__1, &ap[kpc], &c__1);
	    kx = kpc + kp - 1;
	    i__1 = k - 1;
	    for (j = kp + 1; j <= i__1; ++j) {
		kx = kx + j - 1;
		temp = ap[kc + j - 1];
		ap[kc + j - 1] = ap[kx];
		ap[kx] = temp;
/* L40: */
	    }
	    temp = ap[kc + k - 1];
	    ap[kc + k - 1] = ap[kpc + kp - 1];
	    ap[kpc + kp - 1] = temp;
	    if (kstep == 2) {
		temp = ap[kc + k + k - 1];
		ap[kc + k + k - 1] = ap[kc + k + kp - 1];
		ap[kc + k + kp - 1] = temp;
	    }
	}

	k += kstep;
	kc = kcnext;
	goto L30;
L50:

	;
    } else {

/*        Compute inv(A) from the factorization A = L*D*L'. */

/*        K is the main loop index, increasing from 1 to N in steps of */
/*        1 or 2, depending on the size of the diagonal blocks. */

	npp = *n * (*n + 1) / 2;
	k = *n;
	kc = npp;
L60:

/*        If K < 1, exit from loop. */

	if (k < 1) {
	    goto L80;
	}

	kcnext = kc - (*n - k + 2);
	if (ipiv[k] > 0) {

/*           1 x 1 diagonal block */

/*           Invert the diagonal block. */

	    ap[kc] = 1. / ap[kc];

/*           Compute column K of the inverse. */

	    if (k < *n) {
		i__1 = *n - k;
		dcopy_(&i__1, &ap[kc + 1], &c__1, &work[1], &c__1);
		i__1 = *n - k;
		dspmv_(uplo, &i__1, &c_b11, &ap[kc + *n - k + 1], &work[1], &
			c__1, &c_b13, &ap[kc + 1], &c__1);
		i__1 = *n - k;
		ap[kc] -= ddot_(&i__1, &work[1], &c__1, &ap[kc + 1], &c__1);
	    }
	    kstep = 1;
	} else {

/*           2 x 2 diagonal block */

/*           Invert the diagonal block. */

	    t = (d__1 = ap[kcnext + 1], abs(d__1));
	    ak = ap[kcnext] / t;
	    akp1 = ap[kc] / t;
	    akkp1 = ap[kcnext + 1] / t;
	    d__ = t * (ak * akp1 - 1.);
	    ap[kcnext] = akp1 / d__;
	    ap[kc] = ak / d__;
	    ap[kcnext + 1] = -akkp1 / d__;

/*           Compute columns K-1 and K of the inverse. */

	    if (k < *n) {
		i__1 = *n - k;
		dcopy_(&i__1, &ap[kc + 1], &c__1, &work[1], &c__1);
		i__1 = *n - k;
		dspmv_(uplo, &i__1, &c_b11, &ap[kc + (*n - k + 1)], &work[1],
			&c__1, &c_b13, &ap[kc + 1], &c__1);
		i__1 = *n - k;
		ap[kc] -= ddot_(&i__1, &work[1], &c__1, &ap[kc + 1], &c__1);
		i__1 = *n - k;
		ap[kcnext + 1] -= ddot_(&i__1, &ap[kc + 1], &c__1, &ap[kcnext
			+ 2], &c__1);
		i__1 = *n - k;
		dcopy_(&i__1, &ap[kcnext + 2], &c__1, &work[1], &c__1);
		i__1 = *n - k;
		dspmv_(uplo, &i__1, &c_b11, &ap[kc + (*n - k + 1)], &work[1],
			&c__1, &c_b13, &ap[kcnext + 2], &c__1);
		i__1 = *n - k;
		ap[kcnext] -= ddot_(&i__1, &work[1], &c__1, &ap[kcnext + 2], &
			c__1);
	    }
	    kstep = 2;
	    kcnext -= *n - k + 3;
	}

	kp = (i__1 = ipiv[k], abs(i__1));
	if (kp != k) {

/*           Interchange rows and columns K and KP in the trailing */
/*           submatrix A(k-1:n,k-1:n) */

	    kpc = npp - (*n - kp + 1) * (*n - kp + 2) / 2 + 1;
	    if (kp < *n) {
		i__1 = *n - kp;
		dswap_(&i__1, &ap[kc + kp - k + 1], &c__1, &ap[kpc + 1], &
			c__1);
	    }
	    kx = kc + kp - k;
	    i__1 = kp - 1;
	    for (j = k + 1; j <= i__1; ++j) {
		kx = kx + *n - j + 1;
		temp = ap[kc + j - k];
		ap[kc + j - k] = ap[kx];
		ap[kx] = temp;
/* L70: */
	    }
	    temp = ap[kc];
	    ap[kc] = ap[kpc];
	    ap[kpc] = temp;
	    if (kstep == 2) {
		temp = ap[kc - *n + k - 1];
		ap[kc - *n + k - 1] = ap[kc - *n + kp - 1];
		ap[kc - *n + kp - 1] = temp;
	    }
	}

	k -= kstep;
	kc = kcnext;
	goto L60;
L80:
	;
    }

    return 0;

/*     End of DSPTRI */

} /* dsptri_ */

/* Subroutine */ int dsptrs_(const char *uplo, integer *n, integer *nrhs,
	double *ap, integer *ipiv, double *b, integer *ldb, integer *
	info)
{
	/* Table of constant values */
	static double c_b7 = -1.;
	static integer c__1 = 1;
	static double c_b19 = 1.;

    /* System generated locals */
    integer b_dim1, b_offset, i__1;
    double d__1;

    /* Local variables */
    integer j, k;
    double ak, bk;
    integer kc, kp;
    double akm1, bkm1;
    double akm1k;
    double denom;
    bool upper;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSPTRS solves a system of linear equations A*X = B with a real */
/*  symmetric matrix A stored in packed format using the factorization */
/*  A = U*D*U**T or A = L*D*L**T computed by DSPTRF. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the details of the factorization are stored */
/*          as an upper or lower triangular matrix. */
/*          = 'U':  Upper triangular, form is A = U*D*U**T; */
/*          = 'L':  Lower triangular, form is A = L*D*L**T. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  AP      (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The block diagonal matrix D and the multipliers used to */
/*          obtain the factor U or L as computed by DSPTRF, stored as a */
/*          packed triangular matrix. */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          Details of the interchanges and the block structure of D */
/*          as determined by DSPTRF. */

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

    /* Parameter adjustments */
    --ap;
    --ipiv;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -7;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSPTRS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *nrhs == 0) {
	return 0;
    }

    if (upper) {

/*        Solve A*X = B, where A = U*D*U'. */

/*        First solve U*D*X = B, overwriting B with X. */

/*        K is the main loop index, decreasing from N to 1 in steps of */
/*        1 or 2, depending on the size of the diagonal blocks. */

	k = *n;
	kc = *n * (*n + 1) / 2 + 1;
L10:

/*        If K < 1, exit from loop. */

	if (k < 1) {
	    goto L30;
	}

	kc -= k;
	if (ipiv[k] > 0) {

/*           1 x 1 diagonal block */

/*           Interchange rows K and IPIV(K). */

	    kp = ipiv[k];
	    if (kp != k) {
		dswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }

/*           Multiply by inv(U(K)), where U(K) is the transformation */
/*           stored in column K of A. */

	    i__1 = k - 1;
	    dger_(&i__1, nrhs, &c_b7, &ap[kc], &c__1, &b[k + b_dim1], ldb, &b[
		    b_dim1 + 1], ldb);

/*           Multiply by the inverse of the diagonal block. */

	    d__1 = 1. / ap[kc + k - 1];
	    dscal_(nrhs, &d__1, &b[k + b_dim1], ldb);
	    --k;
	} else {

/*           2 x 2 diagonal block */

/*           Interchange rows K-1 and -IPIV(K). */

	    kp = -ipiv[k];
	    if (kp != k - 1) {
		dswap_(nrhs, &b[k - 1 + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }

/*           Multiply by inv(U(K)), where U(K) is the transformation */
/*           stored in columns K-1 and K of A. */

	    i__1 = k - 2;
	    dger_(&i__1, nrhs, &c_b7, &ap[kc], &c__1, &b[k + b_dim1], ldb, &b[
		    b_dim1 + 1], ldb);
	    i__1 = k - 2;
	    dger_(&i__1, nrhs, &c_b7, &ap[kc - (k - 1)], &c__1, &b[k - 1 +
		    b_dim1], ldb, &b[b_dim1 + 1], ldb);

/*           Multiply by the inverse of the diagonal block. */

	    akm1k = ap[kc + k - 2];
	    akm1 = ap[kc - 1] / akm1k;
	    ak = ap[kc + k - 1] / akm1k;
	    denom = akm1 * ak - 1.;
	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		bkm1 = b[k - 1 + j * b_dim1] / akm1k;
		bk = b[k + j * b_dim1] / akm1k;
		b[k - 1 + j * b_dim1] = (ak * bkm1 - bk) / denom;
		b[k + j * b_dim1] = (akm1 * bk - bkm1) / denom;
/* L20: */
	    }
	    kc = kc - k + 1;
	    k += -2;
	}

	goto L10;
L30:

/*        Next solve U'*X = B, overwriting B with X. */

/*        K is the main loop index, increasing from 1 to N in steps of */
/*        1 or 2, depending on the size of the diagonal blocks. */

	k = 1;
	kc = 1;
L40:

/*        If K > N, exit from loop. */

	if (k > *n) {
	    goto L50;
	}

	if (ipiv[k] > 0) {

/*           1 x 1 diagonal block */

/*           Multiply by inv(U'(K)), where U(K) is the transformation */
/*           stored in column K of A. */

	    i__1 = k - 1;
	    dgemv_("Transpose", &i__1, nrhs, &c_b7, &b[b_offset], ldb, &ap[kc]
, &c__1, &c_b19, &b[k + b_dim1], ldb);

/*           Interchange rows K and IPIV(K). */

	    kp = ipiv[k];
	    if (kp != k) {
		dswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }
	    kc += k;
	    ++k;
	} else {

/*           2 x 2 diagonal block */

/*           Multiply by inv(U'(K+1)), where U(K+1) is the transformation */
/*           stored in columns K and K+1 of A. */

	    i__1 = k - 1;
	    dgemv_("Transpose", &i__1, nrhs, &c_b7, &b[b_offset], ldb, &ap[kc]
, &c__1, &c_b19, &b[k + b_dim1], ldb);
	    i__1 = k - 1;
	    dgemv_("Transpose", &i__1, nrhs, &c_b7, &b[b_offset], ldb, &ap[kc
		    + k], &c__1, &c_b19, &b[k + 1 + b_dim1], ldb);

/*           Interchange rows K and -IPIV(K). */

	    kp = -ipiv[k];
	    if (kp != k) {
		dswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }
	    kc = kc + (k << 1) + 1;
	    k += 2;
	}

	goto L40;
L50:

	;
    } else {

/*        Solve A*X = B, where A = L*D*L'. */

/*        First solve L*D*X = B, overwriting B with X. */

/*        K is the main loop index, increasing from 1 to N in steps of */
/*        1 or 2, depending on the size of the diagonal blocks. */

	k = 1;
	kc = 1;
L60:

/*        If K > N, exit from loop. */

	if (k > *n) {
	    goto L80;
	}

	if (ipiv[k] > 0) {

/*           1 x 1 diagonal block */

/*           Interchange rows K and IPIV(K). */

	    kp = ipiv[k];
	    if (kp != k) {
		dswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }

/*           Multiply by inv(L(K)), where L(K) is the transformation */
/*           stored in column K of A. */

	    if (k < *n) {
		i__1 = *n - k;
		dger_(&i__1, nrhs, &c_b7, &ap[kc + 1], &c__1, &b[k + b_dim1],
			ldb, &b[k + 1 + b_dim1], ldb);
	    }

/*           Multiply by the inverse of the diagonal block. */

	    d__1 = 1. / ap[kc];
	    dscal_(nrhs, &d__1, &b[k + b_dim1], ldb);
	    kc = kc + *n - k + 1;
	    ++k;
	} else {

/*           2 x 2 diagonal block */

/*           Interchange rows K+1 and -IPIV(K). */

	    kp = -ipiv[k];
	    if (kp != k + 1) {
		dswap_(nrhs, &b[k + 1 + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }

/*           Multiply by inv(L(K)), where L(K) is the transformation */
/*           stored in columns K and K+1 of A. */

	    if (k < *n - 1) {
		i__1 = *n - k - 1;
		dger_(&i__1, nrhs, &c_b7, &ap[kc + 2], &c__1, &b[k + b_dim1],
			ldb, &b[k + 2 + b_dim1], ldb);
		i__1 = *n - k - 1;
		dger_(&i__1, nrhs, &c_b7, &ap[kc + *n - k + 2], &c__1, &b[k +
			1 + b_dim1], ldb, &b[k + 2 + b_dim1], ldb);
	    }

/*           Multiply by the inverse of the diagonal block. */

	    akm1k = ap[kc + 1];
	    akm1 = ap[kc] / akm1k;
	    ak = ap[kc + *n - k + 1] / akm1k;
	    denom = akm1 * ak - 1.;
	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		bkm1 = b[k + j * b_dim1] / akm1k;
		bk = b[k + 1 + j * b_dim1] / akm1k;
		b[k + j * b_dim1] = (ak * bkm1 - bk) / denom;
		b[k + 1 + j * b_dim1] = (akm1 * bk - bkm1) / denom;
/* L70: */
	    }
	    kc = kc + (*n - k << 1) + 1;
	    k += 2;
	}

	goto L60;
L80:

/*        Next solve L'*X = B, overwriting B with X. */

/*        K is the main loop index, decreasing from N to 1 in steps of */
/*        1 or 2, depending on the size of the diagonal blocks. */

	k = *n;
	kc = *n * (*n + 1) / 2 + 1;
L90:

/*        If K < 1, exit from loop. */

	if (k < 1) {
	    goto L100;
	}

	kc -= *n - k + 1;
	if (ipiv[k] > 0) {

/*           1 x 1 diagonal block */

/*           Multiply by inv(L'(K)), where L(K) is the transformation */
/*           stored in column K of A. */

	    if (k < *n) {
		i__1 = *n - k;
		dgemv_("Transpose", &i__1, nrhs, &c_b7, &b[k + 1 + b_dim1],
			ldb, &ap[kc + 1], &c__1, &c_b19, &b[k + b_dim1], ldb);
	    }

/*           Interchange rows K and IPIV(K). */

	    kp = ipiv[k];
	    if (kp != k) {
		dswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }
	    --k;
	} else {

/*           2 x 2 diagonal block */

/*           Multiply by inv(L'(K-1)), where L(K-1) is the transformation */
/*           stored in columns K-1 and K of A. */

	    if (k < *n) {
		i__1 = *n - k;
		dgemv_("Transpose", &i__1, nrhs, &c_b7, &b[k + 1 + b_dim1],
			ldb, &ap[kc + 1], &c__1, &c_b19, &b[k + b_dim1], ldb);
		i__1 = *n - k;
		dgemv_("Transpose", &i__1, nrhs, &c_b7, &b[k + 1 + b_dim1],
			ldb, &ap[kc - (*n - k)], &c__1, &c_b19, &b[k - 1 +
			b_dim1], ldb);
	    }

/*           Interchange rows K and -IPIV(K). */

	    kp = -ipiv[k];
	    if (kp != k) {
		dswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }
	    kc -= *n - k + 2;
	    k += -2;
	}

	goto L90;
L100:
	;
    }

    return 0;

/*     End of DSPTRS */

} /* dsptrs_ */

/* Subroutine */ int dstebz_(const char *range, const char *order, integer *n, double
	*vl, double *vu, integer *il, integer *iu, double *abstol,
	double *d__, double *e, integer *m, integer *nsplit,
	double *w, integer *iblock, integer *isplit, double *work,
	integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__3 = 3;
	static integer c__2 = 2;
	static integer c__0 = 0;

    /* System generated locals */
    integer i__1, i__2, i__3;
    double d__1, d__2, d__3, d__4, d__5;

    /* Local variables */
    integer j, ib, jb, ie, je, nb;
    double gl;
    integer im, in;
    double gu;
    integer iw;
    double wl, wu;
    integer nwl;
    double ulp, wlu, wul;
    integer nwu;
    double tmp1, tmp2;
    integer iend, ioff, iout, itmp1, jdisc;
    integer iinfo;
    double atoli;
    integer iwoff;
    double bnorm;
    integer itmax;
    double wkill, rtoli, tnorm;
    integer ibegin;
    integer irange, idiscl;
    double safemn;
    integer idumma[1];
    integer idiscu, iorder;
    bool ncnvrg;
    double pivmin;
    bool toofew;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */
/*     8-18-00:  Increase FUDGE factor for T3E (eca) */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSTEBZ computes the eigenvalues of a symmetric tridiagonal */
/*  matrix T.  The user may ask for all eigenvalues, all eigenvalues */
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

/*  RANGE   (input) CHARACTER*1 */
/*          = 'A': ("All")   all eigenvalues will be found. */
/*          = 'V': ("Value") all eigenvalues in the half-open interval */
/*                           (VL, VU] will be found. */
/*          = 'I': ("Index") the IL-th through IU-th eigenvalues (of the */
/*                           entire matrix) will be found. */

/*  ORDER   (input) CHARACTER*1 */
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

/*  ABSTOL  (input) DOUBLE PRECISION */
/*          The absolute tolerance for the eigenvalues.  An eigenvalue */
/*          (or cluster) is considered to be located if it has been */
/*          determined to lie in an interval whose width is ABSTOL or */
/*          less.  If ABSTOL is less than or equal to zero, then ULP*|T| */
/*          will be used, where |T| means the 1-norm of T. */

/*          Eigenvalues will be computed most accurately when ABSTOL is */
/*          set to twice the underflow threshold 2*DLAMCH('S'), not zero. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The n diagonal elements of the tridiagonal matrix T. */

/*  E       (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) off-diagonal elements of the tridiagonal matrix T. */

/*  M       (output) INTEGER */
/*          The actual number of eigenvalues found. 0 <= M <= N. */
/*          (See also the description of INFO=2,3.) */

/*  NSPLIT  (output) INTEGER */
/*          The number of diagonal blocks in the matrix T. */
/*          1 <= NSPLIT <= N. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          On exit, the first M elements of W will contain the */
/*          eigenvalues.  (DSTEBZ may use the remaining N-M elements as */
/*          workspace.) */

/*  IBLOCK  (output) INTEGER array, dimension (N) */
/*          At each row/column j where E(j) is zero or small, the */
/*          matrix T is considered to split into a block diagonal */
/*          matrix.  On exit, if INFO = 0, IBLOCK(i) specifies to which */
/*          block (from 1 to the number of blocks) the eigenvalue W(i) */
/*          belongs.  (DSTEBZ may use the remaining N-M elements as */
/*          workspace.) */

/*  ISPLIT  (output) INTEGER array, dimension (N) */
/*          The splitting points, at which T breaks up into submatrices. */
/*          The first submatrix consists of rows/columns 1 to ISPLIT(1), */
/*          the second of rows/columns ISPLIT(1)+1 through ISPLIT(2), */
/*          etc., and the NSPLIT-th consists of rows/columns */
/*          ISPLIT(NSPLIT-1)+1 through ISPLIT(NSPLIT)=N. */
/*          (Only the first NSPLIT elements will actually be used, but */
/*          since the user cannot know a priori what value NSPLIT will */
/*          have, N words must be reserved for ISPLIT.) */

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

/*  RELFAC  DOUBLE PRECISION, default = 2.0e0 */
/*          The relative tolerance.  An interval (a,b] lies within */
/*          "relative tolerance" if  b-a < RELFAC*ulp*max(|a|,|b|), */
/*          where "ulp" is the machine precision (distance from 1 to */
/*          the next larger floating point number.) */

/*  FUDGE   DOUBLE PRECISION, default = 2 */
/*          A "fudge factor" to widen the Gershgorin intervals.  Ideally, */
/*          a value of 1 should work, but on machines with sloppy */
/*          arithmetic, this needs to be larger.  The default for */
/*          publicly released versions should be large enough to handle */
/*          the worst machine around.  Note that this has no effect */
/*          on accuracy of the solution. */

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
    --isplit;
    --iblock;
    --w;
    --e;
    --d__;

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

/*     Decode ORDER */

    if (lsame_(order, "B")) {
	iorder = 2;
    } else if (lsame_(order, "E")) {
	iorder = 1;
    } else {
	iorder = 0;
    }

/*     Check for Errors */

    if (irange <= 0) {
	*info = -1;
    } else if (iorder <= 0) {
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
	i__1 = -(*info);
	xerbla_("DSTEBZ", &i__1);
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

/*     Simplifications: */

    if (irange == 3 && *il == 1 && *iu == *n) {
	irange = 1;
    }

/*     Get machine constants */
/*     NB is the minimum vector length for vector bisection, or 0 */
/*     if only scalar is to be done. */

    safemn = dlamch_("S");
    ulp = dlamch_("P");
    rtoli = ulp * 2.;
    nb = ilaenv_(&c__1, "DSTEBZ", " ", n, &c_n1, &c_n1, &c_n1);
    if (nb <= 1) {
	nb = 0;
    }

/*     Special Case when N=1 */

    if (*n == 1) {
	*nsplit = 1;
	isplit[1] = 1;
	if (irange == 2 && (*vl >= d__[1] || *vu < d__[1])) {
	    *m = 0;
	} else {
	    w[1] = d__[1];
	    iblock[1] = 1;
	    *m = 1;
	}
	return 0;
    }

/*     Compute Splitting Points */

    *nsplit = 1;
    work[*n] = 0.;
    pivmin = 1.;

/* DIR$ NOVECTOR */
    i__1 = *n;
    for (j = 2; j <= i__1; ++j) {
/* Computing 2nd power */
	d__1 = e[j - 1];
	tmp1 = d__1 * d__1;
/* Computing 2nd power */
	d__2 = ulp;
	if ((d__1 = d__[j] * d__[j - 1], abs(d__1)) * (d__2 * d__2) + safemn
		> tmp1) {
	    isplit[*nsplit] = j - 1;
	    ++(*nsplit);
	    work[j - 1] = 0.;
	} else {
	    work[j - 1] = tmp1;
	    pivmin = std::max(pivmin,tmp1);
	}
/* L10: */
    }
    isplit[*nsplit] = *n;
    pivmin *= safemn;

/*     Compute Interval and ATOLI */

    if (irange == 3) {

/*        RANGE='I': Compute the interval containing eigenvalues */
/*                   IL through IU. */

/*        Compute Gershgorin interval for entire (split) matrix */
/*        and use it as the initial interval */

	gu = d__[1];
	gl = d__[1];
	tmp1 = 0.;

	i__1 = *n - 1;
	for (j = 1; j <= i__1; ++j) {
	    tmp2 = sqrt(work[j]);
/* Computing MAX */
	    d__1 = gu, d__2 = d__[j] + tmp1 + tmp2;
	    gu = std::max(d__1,d__2);
/* Computing MIN */
	    d__1 = gl, d__2 = d__[j] - tmp1 - tmp2;
	    gl = std::min(d__1,d__2);
	    tmp1 = tmp2;
/* L20: */
	}

/* Computing MAX */
	d__1 = gu, d__2 = d__[*n] + tmp1;
	gu = std::max(d__1,d__2);
/* Computing MIN */
	d__1 = gl, d__2 = d__[*n] - tmp1;
	gl = std::min(d__1,d__2);
/* Computing MAX */
	d__1 = abs(gl), d__2 = abs(gu);
	tnorm = std::max(d__1,d__2);
	gl = gl - tnorm * 2.1 * ulp * *n - pivmin * 4.2000000000000002;
	gu = gu + tnorm * 2.1 * ulp * *n + pivmin * 2.1;

/*        Compute Iteration parameters */

	itmax = (integer) ((log(tnorm + pivmin) - log(pivmin)) / log(2.)) + 2;
	if (*abstol <= 0.) {
	    atoli = ulp * tnorm;
	} else {
	    atoli = *abstol;
	}

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

	dlaebz_(&c__3, &itmax, n, &c__2, &c__2, &nb, &atoli, &rtoli, &pivmin,
		&d__[1], &e[1], &work[1], &iwork[5], &work[*n + 1], &work[*n
		+ 5], &iout, &iwork[1], &w[1], &iblock[1], &iinfo);

	if (iwork[6] == *iu) {
	    wl = work[*n + 1];
	    wlu = work[*n + 3];
	    nwl = iwork[1];
	    wu = work[*n + 4];
	    wul = work[*n + 2];
	    nwu = iwork[4];
	} else {
	    wl = work[*n + 2];
	    wlu = work[*n + 4];
	    nwl = iwork[2];
	    wu = work[*n + 3];
	    wul = work[*n + 1];
	    nwu = iwork[3];
	}

	if (nwl < 0 || nwl >= *n || nwu < 1 || nwu > *n) {
	    *info = 4;
	    return 0;
	}
    } else {

/*        RANGE='A' or 'V' -- Set ATOLI */

/* Computing MAX */
	d__3 = abs(d__[1]) + abs(e[1]), d__4 = (d__1 = d__[*n], abs(d__1)) + (
		d__2 = e[*n - 1], abs(d__2));
	tnorm = std::max(d__3,d__4);

	i__1 = *n - 1;
	for (j = 2; j <= i__1; ++j) {
/* Computing MAX */
	    d__4 = tnorm, d__5 = (d__1 = d__[j], abs(d__1)) + (d__2 = e[j - 1]
		    , abs(d__2)) + (d__3 = e[j], abs(d__3));
	    tnorm = std::max(d__4,d__5);
/* L30: */
	}

	if (*abstol <= 0.) {
	    atoli = ulp * tnorm;
	} else {
	    atoli = *abstol;
	}

	if (irange == 2) {
	    wl = *vl;
	    wu = *vu;
	} else {
	    wl = 0.;
	    wu = 0.;
	}
    }

/*     Find Eigenvalues -- Loop Over Blocks and recompute NWL and NWU. */
/*     NWL accumulates the number of eigenvalues .le. WL, */
/*     NWU accumulates the number of eigenvalues .le. WU */

    *m = 0;
    iend = 0;
    *info = 0;
    nwl = 0;
    nwu = 0;

    i__1 = *nsplit;
    for (jb = 1; jb <= i__1; ++jb) {
	ioff = iend;
	ibegin = ioff + 1;
	iend = isplit[jb];
	in = iend - ioff;

	if (in == 1) {

/*           Special Case -- IN=1 */

	    if (irange == 1 || wl >= d__[ibegin] - pivmin) {
		++nwl;
	    }
	    if (irange == 1 || wu >= d__[ibegin] - pivmin) {
		++nwu;
	    }
	    if (irange == 1 || wl < d__[ibegin] - pivmin && wu >= d__[ibegin]
		    - pivmin) {
		++(*m);
		w[*m] = d__[ibegin];
		iblock[*m] = jb;
	    }
	} else {

/*           General Case -- IN > 1 */

/*           Compute Gershgorin Interval */
/*           and use it as the initial interval */

	    gu = d__[ibegin];
	    gl = d__[ibegin];
	    tmp1 = 0.;

	    i__2 = iend - 1;
	    for (j = ibegin; j <= i__2; ++j) {
		tmp2 = (d__1 = e[j], abs(d__1));
/* Computing MAX */
		d__1 = gu, d__2 = d__[j] + tmp1 + tmp2;
		gu = std::max(d__1,d__2);
/* Computing MIN */
		d__1 = gl, d__2 = d__[j] - tmp1 - tmp2;
		gl = std::min(d__1,d__2);
		tmp1 = tmp2;
/* L40: */
	    }

/* Computing MAX */
	    d__1 = gu, d__2 = d__[iend] + tmp1;
	    gu = std::max(d__1,d__2);
/* Computing MIN */
	    d__1 = gl, d__2 = d__[iend] - tmp1;
	    gl = std::min(d__1,d__2);
/* Computing MAX */
	    d__1 = abs(gl), d__2 = abs(gu);
	    bnorm = std::max(d__1,d__2);
	    gl = gl - bnorm * 2.1 * ulp * in - pivmin * 2.1;
	    gu = gu + bnorm * 2.1 * ulp * in + pivmin * 2.1;

/*           Compute ATOLI for the current submatrix */

	    if (*abstol <= 0.) {
/* Computing MAX */
		d__1 = abs(gl), d__2 = abs(gu);
		atoli = ulp * std::max(d__1,d__2);
	    } else {
		atoli = *abstol;
	    }

	    if (irange > 1) {
		if (gu < wl) {
		    nwl += in;
		    nwu += in;
		    goto L70;
		}
		gl = std::max(gl,wl);
		gu = std::min(gu,wu);
		if (gl >= gu) {
		    goto L70;
		}
	    }

/*           Set Up Initial Interval */

	    work[*n + 1] = gl;
	    work[*n + in + 1] = gu;
	    dlaebz_(&c__1, &c__0, &in, &in, &c__1, &nb, &atoli, &rtoli, &
		    pivmin, &d__[ibegin], &e[ibegin], &work[ibegin], idumma, &
		    work[*n + 1], &work[*n + (in << 1) + 1], &im, &iwork[1], &
		    w[*m + 1], &iblock[*m + 1], &iinfo);

	    nwl += iwork[1];
	    nwu += iwork[in + 1];
	    iwoff = *m - iwork[1];

/*           Compute Eigenvalues */

	    itmax = (integer) ((log(gu - gl + pivmin) - log(pivmin)) / log(2.)
		    ) + 2;
	    dlaebz_(&c__2, &itmax, &in, &in, &c__1, &nb, &atoli, &rtoli, &
		    pivmin, &d__[ibegin], &e[ibegin], &work[ibegin], idumma, &
		    work[*n + 1], &work[*n + (in << 1) + 1], &iout, &iwork[1],
		     &w[*m + 1], &iblock[*m + 1], &iinfo);

/*           Copy Eigenvalues Into W and IBLOCK */
/*           Use -JB for block number for unconverged eigenvalues. */

	    i__2 = iout;
	    for (j = 1; j <= i__2; ++j) {
		tmp1 = (work[j + *n] + work[j + in + *n]) * .5;

/*              Flag non-convergence. */

		if (j > iout - iinfo) {
		    ncnvrg = true;
		    ib = -jb;
		} else {
		    ib = jb;
		}
		i__3 = iwork[j + in] + iwoff;
		for (je = iwork[j] + 1 + iwoff; je <= i__3; ++je) {
		    w[je] = tmp1;
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
	im = 0;
	idiscl = *il - 1 - nwl;
	idiscu = nwu - *iu;

	if (idiscl > 0 || idiscu > 0) {
	    i__1 = *m;
	    for (je = 1; je <= i__1; ++je) {
		if (w[je] <= wlu && idiscl > 0) {
		    --idiscl;
		} else if (w[je] >= wul && idiscu > 0) {
		    --idiscu;
		} else {
		    ++im;
		    w[im] = w[je];
		    iblock[im] = iblock[je];
		}
/* L80: */
	    }
	    *m = im;
	}
	if (idiscl > 0 || idiscu > 0) {

/*           Code to deal with effects of bad arithmetic: */
/*           Some low eigenvalues to be discarded are not in (WL,WLU], */
/*           or high eigenvalues to be discarded are not in (WUL,WU] */
/*           so just kill off the smallest IDISCL/largest IDISCU */
/*           eigenvalues, by simply finding the smallest/largest */
/*           eigenvalue(s). */

/*           (If N(w) is monotone non-decreasing, this should never */
/*               happen.) */

	    if (idiscl > 0) {
		wkill = wu;
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

		wkill = wl;
		i__1 = idiscu;
		for (jdisc = 1; jdisc <= i__1; ++jdisc) {
		    iw = 0;
		    i__2 = *m;
		    for (je = 1; je <= i__2; ++je) {
			if (iblock[je] != 0 && (w[je] > wkill || iw == 0)) {
			    iw = je;
			    wkill = w[je];
			}
/* L110: */
		    }
		    iblock[iw] = 0;
/* L120: */
		}
	    }
	    im = 0;
	    i__1 = *m;
	    for (je = 1; je <= i__1; ++je) {
		if (iblock[je] != 0) {
		    ++im;
		    w[im] = w[je];
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

/*     If ORDER='B', do nothing -- the eigenvalues are already sorted */
/*        by block. */
/*     If ORDER='E', sort the eigenvalues from smallest to largest */

    if (iorder == 1 && *nsplit > 1) {
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
		itmp1 = iblock[ie];
		w[ie] = w[je];
		iblock[ie] = iblock[je];
		w[je] = tmp1;
		iblock[je] = itmp1;
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

/*     End of DSTEBZ */

} /* dstebz_ */

/* Subroutine */ int dstedc_(const char *compz, integer *n, double *d__,
	double *e, double *z__, integer *ldz, double *work,
	integer *lwork, integer *iwork, integer *liwork, integer *info)
{
	/* Table of constant values */
	static integer c__9 = 9;
	static integer c__0 = 0;
	static integer c__2 = 2;
	static double c_b17 = 0.;
	static double c_b18 = 1.;
	static integer c__1 = 1;

    /* System generated locals */
    integer z_dim1, z_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j, k, m;
    double p;
    integer ii, lgn;
    double eps, tiny;
    integer lwmin;
     integer start;
    integer finish;
    integer liwmin, icompz;
	double orgnrm;
    bool lquery;
    integer smlsiz, storez, strtrw;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSTEDC computes all eigenvalues and, optionally, eigenvectors of a */
/*  symmetric tridiagonal matrix using the divide and conquer method. */
/*  The eigenvectors of a full or band real symmetric matrix can also be */
/*  found if DSYTRD or DSPTRD or DSBTRD has been used to reduce this */
/*  matrix to tridiagonal form. */

/*  This code makes very mild assumptions about floating point */
/*  arithmetic. It will work on machines with a guard digit in */
/*  add/subtract, or on those binary machines without guard digits */
/*  which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or Cray-2. */
/*  It could conceivably fail on hexadecimal or decimal machines */
/*  without guard digits, but we know of none.  See DLAED3 for details. */

/*  Arguments */
/*  ========= */

/*  COMPZ   (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only. */
/*          = 'I':  Compute eigenvectors of tridiagonal matrix also. */
/*          = 'V':  Compute eigenvectors of original dense symmetric */
/*                  matrix also.  On entry, Z contains the orthogonal */
/*                  matrix used to reduce the original matrix to */
/*                  tridiagonal form. */

/*  N       (input) INTEGER */
/*          The dimension of the symmetric tridiagonal matrix.  N >= 0. */

/*  D       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the diagonal elements of the tridiagonal matrix. */
/*          On exit, if INFO = 0, the eigenvalues in ascending order. */

/*  E       (input/output) DOUBLE PRECISION array, dimension (N-1) */
/*          On entry, the subdiagonal elements of the tridiagonal matrix. */
/*          On exit, E has been destroyed. */

/*  Z       (input/output) DOUBLE PRECISION array, dimension (LDZ,N) */
/*          On entry, if COMPZ = 'V', then Z contains the orthogonal */
/*          matrix used in the reduction to tridiagonal form. */
/*          On exit, if INFO = 0, then if COMPZ = 'V', Z contains the */
/*          orthonormal eigenvectors of the original symmetric matrix, */
/*          and if COMPZ = 'I', Z contains the orthonormal eigenvectors */
/*          of the symmetric tridiagonal matrix. */
/*          If  COMPZ = 'N', then Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1. */
/*          If eigenvectors are desired, then LDZ >= max(1,N). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, */
/*                                         dimension (LWORK) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          If COMPZ = 'N' or N <= 1 then LWORK must be at least 1. */
/*          If COMPZ = 'V' and N > 1 then LWORK must be at least */
/*                         ( 1 + 3*N + 2*N*lg N + 3*N**2 ), */
/*                         where lg( N ) = smallest integer k such */
/*                         that 2**k >= N. */
/*          If COMPZ = 'I' and N > 1 then LWORK must be at least */
/*                         ( 1 + 4*N + N**2 ). */
/*          Note that for COMPZ = 'I' or 'V', then if N is less than or */
/*          equal to the minimum divide size, usually 25, then LWORK need */
/*          only be max(1,2*(N-1)). */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  IWORK   (workspace/output) INTEGER array, dimension (MAX(1,LIWORK)) */
/*          On exit, if INFO = 0, IWORK(1) returns the optimal LIWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK. */
/*          If COMPZ = 'N' or N <= 1 then LIWORK must be at least 1. */
/*          If COMPZ = 'V' and N > 1 then LIWORK must be at least */
/*                         ( 6 + 6*N + 5*N*lg N ). */
/*          If COMPZ = 'I' and N > 1 then LIWORK must be at least */
/*                         ( 3 + 5*N ). */
/*          Note that for COMPZ = 'I' or 'V', then if N is less than or */
/*          equal to the minimum divide size, usually 25, then LIWORK */
/*          need only be 1. */

/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates the optimal size of the IWORK array, */
/*          returns this value as the first entry of the IWORK array, and */
/*          no error message related to LIWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  The algorithm failed to compute an eigenvalue while */
/*                working on the submatrix lying in rows and columns */
/*                INFO/(N+1) through mod(INFO,N+1). */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Jeff Rutter, Computer Science Division, University of California */
/*     at Berkeley, USA */
/*  Modified by Francoise Tisseur, University of Tennessee. */

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
    --e;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    lquery = *lwork == -1 || *liwork == -1;

    if (lsame_(compz, "N")) {
	icompz = 0;
    } else if (lsame_(compz, "V")) {
	icompz = 1;
    } else if (lsame_(compz, "I")) {
	icompz = 2;
    } else {
	icompz = -1;
    }
    if (icompz < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*ldz < 1 || icompz > 0 && *ldz < std::max(1_integer,*n)) {
	*info = -6;
    }

    if (*info == 0) {

/*        Compute the workspace requirements */

	smlsiz = ilaenv_(&c__9, "DSTEDC", " ", &c__0, &c__0, &c__0, &c__0);
	if (*n <= 1 || icompz == 0) {
	    liwmin = 1;
	    lwmin = 1;
	} else if (*n <= smlsiz) {
	    liwmin = 1;
	    lwmin = *n - 1 << 1;
	} else {
	    lgn = (integer) (log((double) (*n)) / log(2.));
	    if (pow_ii(&c__2, &lgn) < *n) {
		++lgn;
	    }
	    if (pow_ii(&c__2, &lgn) < *n) {
		++lgn;
	    }
	    if (icompz == 1) {
/* Computing 2nd power */
		i__1 = *n;
		lwmin = *n * 3 + 1 + (*n << 1) * lgn + i__1 * i__1 * 3;
		liwmin = *n * 6 + 6 + *n * 5 * lgn;
	    } else if (icompz == 2) {
/* Computing 2nd power */
		i__1 = *n;
		lwmin = (*n << 2) + 1 + i__1 * i__1;
		liwmin = *n * 5 + 3;
	    }
	}
	work[1] = (double) lwmin;
	iwork[1] = liwmin;

	if (*lwork < lwmin && ! lquery) {
	    *info = -8;
	} else if (*liwork < liwmin && ! lquery) {
	    *info = -10;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSTEDC", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }
    if (*n == 1) {
	if (icompz != 0) {
	    z__[z_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     If the following conditional clause is removed, then the routine */
/*     will use the Divide and Conquer routine to compute only the */
/*     eigenvalues, which requires (3N + 3N**2) real workspace and */
/*     (2 + 5N + 2N lg(N)) integer workspace. */
/*     Since on many architectures DSTERF is much faster than any other */
/*     algorithm for finding eigenvalues only, it is used here */
/*     as the default. If the conditional clause is removed, then */
/*     information on the size of workspace needs to be changed. */

/*     If COMPZ = 'N', use DSTERF to compute the eigenvalues. */

    if (icompz == 0) {
	dsterf_(n, &d__[1], &e[1], info);
	goto L50;
    }

/*     If N is smaller than the minimum divide size (SMLSIZ+1), then */
/*     solve the problem with another solver. */

    if (*n <= smlsiz) {

	dsteqr_(compz, n, &d__[1], &e[1], &z__[z_offset], ldz, &work[1], info);

    } else {

/*        If COMPZ = 'V', the Z matrix must be stored elsewhere for later */
/*        use. */

	if (icompz == 1) {
	    storez = *n * *n + 1;
	} else {
	    storez = 1;
	}

	if (icompz == 2) {
	    dlaset_("Full", n, n, &c_b17, &c_b18, &z__[z_offset], ldz);
	}

/*        Scale. */

	orgnrm = dlanst_("M", n, &d__[1], &e[1]);
	if (orgnrm == 0.) {
	    goto L50;
	}

	eps = dlamch_("Epsilon");

	start = 1;

/*        while ( START <= N ) */

L10:
	if (start <= *n) {

/*           Let FINISH be the position of the next subdiagonal entry */
/*           such that E( FINISH ) <= TINY or FINISH = N if no such */
/*           subdiagonal exists.  The matrix identified by the elements */
/*           between START and FINISH constitutes an independent */
/*           sub-problem. */

	    finish = start;
L20:
	    if (finish < *n) {
		tiny = eps * sqrt((d__1 = d__[finish], abs(d__1))) * sqrt((
			d__2 = d__[finish + 1], abs(d__2)));
		if ((d__1 = e[finish], abs(d__1)) > tiny) {
		    ++finish;
		    goto L20;
		}
	    }

/*           (Sub) Problem determined.  Compute its size and solve it. */

	    m = finish - start + 1;
	    if (m == 1) {
		start = finish + 1;
		goto L10;
	    }
	    if (m > smlsiz) {

/*              Scale. */

		orgnrm = dlanst_("M", &m, &d__[start], &e[start]);
		dlascl_("G", &c__0, &c__0, &orgnrm, &c_b18, &m, &c__1, &d__[
			start], &m, info);
		i__1 = m - 1;
		i__2 = m - 1;
		dlascl_("G", &c__0, &c__0, &orgnrm, &c_b18, &i__1, &c__1, &e[
			start], &i__2, info);

		if (icompz == 1) {
		    strtrw = 1;
		} else {
		    strtrw = start;
		}
		dlaed0_(&icompz, n, &m, &d__[start], &e[start], &z__[strtrw +
			start * z_dim1], ldz, &work[1], n, &work[storez], &
			iwork[1], info);
		if (*info != 0) {
		    *info = (*info / (m + 1) + start - 1) * (*n + 1) + *info %
			     (m + 1) + start - 1;
		    goto L50;
		}

/*              Scale back. */

		dlascl_("G", &c__0, &c__0, &c_b18, &orgnrm, &m, &c__1, &d__[
			start], &m, info);

	    } else {
		if (icompz == 1) {

/*                 Since QR won't update a Z matrix which is larger than */
/*                 the length of D, we must solve the sub-problem in a */
/*                 workspace and then multiply back into Z. */

		    dsteqr_("I", &m, &d__[start], &e[start], &work[1], &m, &
			    work[m * m + 1], info);
		    dlacpy_("A", n, &m, &z__[start * z_dim1 + 1], ldz, &work[
			    storez], n);
		    dgemm_("N", "N", n, &m, &m, &c_b18, &work[storez], n, &
			    work[1], &m, &c_b17, &z__[start * z_dim1 + 1],
			    ldz);
		} else if (icompz == 2) {
		    dsteqr_("I", &m, &d__[start], &e[start], &z__[start +
			    start * z_dim1], ldz, &work[1], info);
		} else {
		    dsterf_(&m, &d__[start], &e[start], info);
		}
		if (*info != 0) {
		    *info = start * (*n + 1) + finish;
		    goto L50;
		}
	    }

	    start = finish + 1;
	    goto L10;
	}

/*        endwhile */

/*        If the problem split any number of times, then the eigenvalues */
/*        will not be properly ordered.  Here we permute the eigenvalues */
/*        (and the associated eigenvectors) into ascending order. */

	if (m != *n) {
	    if (icompz == 0) {

/*              Use Quick Sort */

		dlasrt_("I", n, &d__[1], info);

	    } else {

/*              Use Selection Sort to minimize swaps of eigenvectors */

		i__1 = *n;
		for (ii = 2; ii <= i__1; ++ii) {
		    i__ = ii - 1;
		    k = i__;
		    p = d__[i__];
		    i__2 = *n;
		    for (j = ii; j <= i__2; ++j) {
			if (d__[j] < p) {
			    k = j;
			    p = d__[j];
			}
/* L30: */
		    }
		    if (k != i__) {
			d__[k] = d__[i__];
			d__[i__] = p;
			dswap_(n, &z__[i__ * z_dim1 + 1], &c__1, &z__[k *
				z_dim1 + 1], &c__1);
		    }
/* L40: */
		}
	    }
	}
    }

L50:
    work[1] = (double) lwmin;
    iwork[1] = liwmin;

    return 0;

/*     End of DSTEDC */

} /* dstedc_ */

/* Subroutine */ int dstegr_(const char *jobz, const char *range, integer *n, double *
	d__, double *e, double *vl, double *vu, integer *il,
	integer *iu, double *abstol, integer *m, double *w,
	double *z__, integer *ldz, integer *isuppz, double *work,
	integer *lwork, integer *iwork, integer *liwork, integer *info)
{
    /* System generated locals */
    integer z_dim1, z_offset;

    /* Local variables */
    bool tryrac;


/*  -- LAPACK computational routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSTEGR computes selected eigenvalues and, optionally, eigenvectors */
/*  of a real symmetric tridiagonal matrix T. Any such unreduced matrix has */
/*  a well defined set of pairwise different real eigenvalues, the corresponding */
/*  real eigenvectors are pairwise orthogonal. */

/*  The spectrum may be computed either completely or partially by specifying */
/*  either an interval (VL,VU] or a range of indices IL:IU for the desired */
/*  eigenvalues. */

/*  DSTEGR is a compatability wrapper around the improved DSTEMR routine. */
/*  See DSTEMR for further details. */

/*  One important change is that the ABSTOL parameter no longer provides any */
/*  benefit and hence is no longer used. */

/*  Note : DSTEGR and DSTEMR work only on machines which follow */
/*  IEEE-754 floating-point standard in their handling of infinities and */
/*  NaNs.  Normal execution may create these exceptiona values and hence */
/*  may abort due to a floating point exception in environments which */
/*  do not conform to the IEEE-754 standard. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  RANGE   (input) CHARACTER*1 */
/*          = 'A': all eigenvalues will be found. */
/*          = 'V': all eigenvalues in the half-open interval (VL,VU] */
/*                 will be found. */
/*          = 'I': the IL-th through IU-th eigenvalues will be found. */

/*  N       (input) INTEGER */
/*          The order of the matrix.  N >= 0. */

/*  D       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the N diagonal elements of the tridiagonal matrix */
/*          T. On exit, D is overwritten. */

/*  E       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the (N-1) subdiagonal elements of the tridiagonal */
/*          matrix T in elements 1 to N-1 of E. E(N) need not be set on */
/*          input, but is used internally as workspace. */
/*          On exit, E is overwritten. */

/*  VL      (input) DOUBLE PRECISION */
/*  VU      (input) DOUBLE PRECISION */
/*          If RANGE='V', the lower and upper bounds of the interval to */
/*          be searched for eigenvalues. VL < VU. */
/*          Not referenced if RANGE = 'A' or 'I'. */

/*  IL      (input) INTEGER */
/*  IU      (input) INTEGER */
/*          If RANGE='I', the indices (in ascending order) of the */
/*          smallest and largest eigenvalues to be returned. */
/*          1 <= IL <= IU <= N, if N > 0. */
/*          Not referenced if RANGE = 'A' or 'V'. */

/*  ABSTOL  (input) DOUBLE PRECISION */
/*          Unused.  Was the absolute error tolerance for the */
/*          eigenvalues/eigenvectors in previous versions. */

/*  M       (output) INTEGER */
/*          The total number of eigenvalues found.  0 <= M <= N. */
/*          If RANGE = 'A', M = N, and if RANGE = 'I', M = IU-IL+1. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          The first M elements contain the selected eigenvalues in */
/*          ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, max(1,M) ) */
/*          If JOBZ = 'V', and if INFO = 0, then the first M columns of Z */
/*          contain the orthonormal eigenvectors of the matrix T */
/*          corresponding to the selected eigenvalues, with the i-th */
/*          column of Z holding the eigenvector associated with W(i). */
/*          If JOBZ = 'N', then Z is not referenced. */
/*          Note: the user must ensure that at least max(1,M) columns are */
/*          supplied in the array Z; if RANGE = 'V', the exact value of M */
/*          is not known in advance and an upper bound must be used. */
/*          Supplying N columns is always safe. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', then LDZ >= max(1,N). */

/*  ISUPPZ  (output) INTEGER ARRAY, dimension ( 2*max(1,M) ) */
/*          The support of the eigenvectors in Z, i.e., the indices */
/*          indicating the nonzero elements in Z. The i-th computed eigenvector */
/*          is nonzero only in elements ISUPPZ( 2*i-1 ) through */
/*          ISUPPZ( 2*i ). This is relevant in the case when the matrix */
/*          is split. ISUPPZ is only accessed when JOBZ is 'V' and N > 0. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal */
/*          (and minimal) LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. LWORK >= max(1,18*N) */
/*          if JOBZ = 'V', and LWORK >= max(1,12*N) if JOBZ = 'N'. */
/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  IWORK   (workspace/output) INTEGER array, dimension (LIWORK) */
/*          On exit, if INFO = 0, IWORK(1) returns the optimal LIWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK.  LIWORK >= max(1,10*N) */
/*          if the eigenvectors are desired, and LIWORK >= max(1,8*N) */
/*          if only the eigenvalues are to be computed. */
/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates the optimal size of the IWORK array, */
/*          returns this value as the first entry of the IWORK array, and */
/*          no error message related to LIWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          On exit, INFO */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = 1X, internal error in DLARRE, */
/*                if INFO = 2X, internal error in DLARRV. */
/*                Here, the digit X = ABS( IINFO ) < 10, where IINFO is */
/*                the nonzero error code returned by DLARRE or */
/*                DLARRV, respectively. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Inderjit Dhillon, IBM Almaden, USA */
/*     Osni Marques, LBNL/NERSC, USA */
/*     Christof Voemel, LBNL/NERSC, USA */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */
    /* Parameter adjustments */
    --d__;
    --e;
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --isuppz;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    tryrac = false;
    dstemr_(jobz, range, n, &d__[1], &e[1], vl, vu, il, iu, m, &w[1], &z__[
	    z_offset], ldz, n, &isuppz[1], &tryrac, &work[1], lwork, &iwork[1]
, liwork, info);

/*     End of DSTEGR */

    return 0;
} /* dstegr_ */

/* Subroutine */ int dstein_(integer *n, double *d__, double *e,
	integer *m, double *w, integer *iblock, integer *isplit,
	double *z__, integer *ldz, double *work, integer *iwork,
	integer *ifail, integer *info)
{
	/* Table of constant values */
	static integer c__2 = 2;
	static integer c__1 = 1;
	static integer c_n1 = -1;

    /* System generated locals */
    integer z_dim1, z_offset, i__1, i__2, i__3;
    double d__1, d__2, d__3, d__4, d__5;

    /* Local variables */
    integer i__, j, b1, j1, bn;
    double xj, scl, eps, sep, nrm, tol;
    integer its;
    double xjm, ztr, eps1;
    integer jblk, nblk;
    integer jmax;
    integer iseed[4], gpind, iinfo;
    double ortol;
    integer indrv1, indrv2, indrv3, indrv4, indrv5;
    integer nrmchk;
    integer blksiz;
    double onenrm, dtpcrt, pertol;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSTEIN computes the eigenvectors of a real symmetric tridiagonal */
/*  matrix T corresponding to specified eigenvalues, using inverse */
/*  iteration. */

/*  The maximum number of iterations allowed for each eigenvector is */
/*  specified by an internal parameter MAXITS (currently set to 5). */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix.  N >= 0. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The n diagonal elements of the tridiagonal matrix T. */

/*  E       (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) subdiagonal elements of the tridiagonal matrix */
/*          T, in elements 1 to N-1. */

/*  M       (input) INTEGER */
/*          The number of eigenvectors to be found.  0 <= M <= N. */

/*  W       (input) DOUBLE PRECISION array, dimension (N) */
/*          The first M elements of W contain the eigenvalues for */
/*          which eigenvectors are to be computed.  The eigenvalues */
/*          should be grouped by split-off block and ordered from */
/*          smallest to largest within the block.  ( The output array */
/*          W from DSTEBZ with ORDER = 'B' is expected here. ) */

/*  IBLOCK  (input) INTEGER array, dimension (N) */
/*          The submatrix indices associated with the corresponding */
/*          eigenvalues in W; IBLOCK(i)=1 if eigenvalue W(i) belongs to */
/*          the first submatrix from the top, =2 if W(i) belongs to */
/*          the second submatrix, etc.  ( The output array IBLOCK */
/*          from DSTEBZ is expected here. ) */

/*  ISPLIT  (input) INTEGER array, dimension (N) */
/*          The splitting points, at which T breaks up into submatrices. */
/*          The first submatrix consists of rows/columns 1 to */
/*          ISPLIT( 1 ), the second of rows/columns ISPLIT( 1 )+1 */
/*          through ISPLIT( 2 ), etc. */
/*          ( The output array ISPLIT from DSTEBZ is expected here. ) */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, M) */
/*          The computed eigenvectors.  The eigenvector associated */
/*          with the eigenvalue W(i) is stored in the i-th column of */
/*          Z.  Any vector which fails to converge is set to its current */
/*          iterate after MAXITS iterations. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= max(1,N). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (5*N) */

/*  IWORK   (workspace) INTEGER array, dimension (N) */

/*  IFAIL   (output) INTEGER array, dimension (M) */
/*          On normal exit, all elements of IFAIL are zero. */
/*          If one or more eigenvectors fail to converge after */
/*          MAXITS iterations, then their indices are stored in */
/*          array IFAIL. */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit. */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          > 0: if INFO = i, then i eigenvectors failed to converge */
/*               in MAXITS iterations.  Their indices are stored in */
/*               array IFAIL. */

/*  Internal Parameters */
/*  =================== */

/*  MAXITS  INTEGER, default = 5 */
/*          The maximum number of iterations performed. */

/*  EXTRA   INTEGER, default = 2 */
/*          The number of iterations performed after norm growth */
/*          criterion is satisfied, should be at least 1. */

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
    --d__;
    --e;
    --w;
    --iblock;
    --isplit;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;
    --iwork;
    --ifail;

    /* Function Body */
    *info = 0;
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ifail[i__] = 0;
/* L10: */
    }

    if (*n < 0) {
	*info = -1;
    } else if (*m < 0 || *m > *n) {
	*info = -4;
    } else if (*ldz < std::max(1_integer,*n)) {
	*info = -9;
    } else {
	i__1 = *m;
	for (j = 2; j <= i__1; ++j) {
	    if (iblock[j] < iblock[j - 1]) {
		*info = -6;
		goto L30;
	    }
	    if (iblock[j] == iblock[j - 1] && w[j] < w[j - 1]) {
		*info = -5;
		goto L30;
	    }
/* L20: */
	}
L30:
	;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSTEIN", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *m == 0) {
	return 0;
    } else if (*n == 1) {
	z__[z_dim1 + 1] = 1.;
	return 0;
    }

/*     Get machine constants. */

    eps = dlamch_("Precision");

/*     Initialize seed for random number generator DLARNV. */

    for (i__ = 1; i__ <= 4; ++i__) {
	iseed[i__ - 1] = 1;
/* L40: */
    }

/*     Initialize pointers. */

    indrv1 = 0;
    indrv2 = indrv1 + *n;
    indrv3 = indrv2 + *n;
    indrv4 = indrv3 + *n;
    indrv5 = indrv4 + *n;

/*     Compute eigenvectors of matrix blocks. */

    j1 = 1;
    i__1 = iblock[*m];
    for (nblk = 1; nblk <= i__1; ++nblk) {

/*        Find starting and ending indices of block nblk. */

	if (nblk == 1) {
	    b1 = 1;
	} else {
	    b1 = isplit[nblk - 1] + 1;
	}
	bn = isplit[nblk];
	blksiz = bn - b1 + 1;
	if (blksiz == 1) {
	    goto L60;
	}
	gpind = b1;

/*        Compute reorthogonalization criterion and stopping criterion. */

	onenrm = (d__1 = d__[b1], abs(d__1)) + (d__2 = e[b1], abs(d__2));
/* Computing MAX */
	d__3 = onenrm, d__4 = (d__1 = d__[bn], abs(d__1)) + (d__2 = e[bn - 1],
		 abs(d__2));
	onenrm = std::max(d__3,d__4);
	i__2 = bn - 1;
	for (i__ = b1 + 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__4 = onenrm, d__5 = (d__1 = d__[i__], abs(d__1)) + (d__2 = e[
		    i__ - 1], abs(d__2)) + (d__3 = e[i__], abs(d__3));
	    onenrm = std::max(d__4,d__5);
/* L50: */
	}
	ortol = onenrm * .001;

	dtpcrt = sqrt(.1 / blksiz);

/*        Loop through eigenvalues of block nblk. */

L60:
	jblk = 0;
	i__2 = *m;
	for (j = j1; j <= i__2; ++j) {
	    if (iblock[j] != nblk) {
		j1 = j;
		goto L160;
	    }
	    ++jblk;
	    xj = w[j];

/*           Skip all the work if the block size is one. */

	    if (blksiz == 1) {
		work[indrv1 + 1] = 1.;
		goto L120;
	    }

/*           If eigenvalues j and j-1 are too close, add a relatively */
/*           small perturbation. */

	    if (jblk > 1) {
		eps1 = (d__1 = eps * xj, abs(d__1));
		pertol = eps1 * 10.;
		sep = xj - xjm;
		if (sep < pertol) {
		    xj = xjm + pertol;
		}
	    }

	    its = 0;
	    nrmchk = 0;

/*           Get random starting vector. */

	    dlarnv_(&c__2, iseed, &blksiz, &work[indrv1 + 1]);

/*           Copy the matrix T so it won't be destroyed in factorization. */

	    dcopy_(&blksiz, &d__[b1], &c__1, &work[indrv4 + 1], &c__1);
	    i__3 = blksiz - 1;
	    dcopy_(&i__3, &e[b1], &c__1, &work[indrv2 + 2], &c__1);
	    i__3 = blksiz - 1;
	    dcopy_(&i__3, &e[b1], &c__1, &work[indrv3 + 1], &c__1);

/*           Compute LU factors with partial pivoting  ( PT = LU ) */

	    tol = 0.;
	    dlagtf_(&blksiz, &work[indrv4 + 1], &xj, &work[indrv2 + 2], &work[
		    indrv3 + 1], &tol, &work[indrv5 + 1], &iwork[1], &iinfo);

/*           Update iteration count. */

L70:
	    ++its;
	    if (its > 5) {
		goto L100;
	    }

/*           Normalize and scale the righthand side vector Pb. */

/* Computing MAX */
	    d__2 = eps, d__3 = (d__1 = work[indrv4 + blksiz], abs(d__1));
	    scl = blksiz * onenrm * std::max(d__2,d__3) / dasum_(&blksiz, &work[
		    indrv1 + 1], &c__1);
	    dscal_(&blksiz, &scl, &work[indrv1 + 1], &c__1);

/*           Solve the system LU = Pb. */

	    dlagts_(&c_n1, &blksiz, &work[indrv4 + 1], &work[indrv2 + 2], &
		    work[indrv3 + 1], &work[indrv5 + 1], &iwork[1], &work[
		    indrv1 + 1], &tol, &iinfo);

/*           Reorthogonalize by modified Gram-Schmidt if eigenvalues are */
/*           close enough. */

	    if (jblk == 1) {
		goto L90;
	    }
	    if ((d__1 = xj - xjm, abs(d__1)) > ortol) {
		gpind = j;
	    }
	    if (gpind != j) {
		i__3 = j - 1;
		for (i__ = gpind; i__ <= i__3; ++i__) {
		    ztr = -ddot_(&blksiz, &work[indrv1 + 1], &c__1, &z__[b1 +
			    i__ * z_dim1], &c__1);
		    daxpy_(&blksiz, &ztr, &z__[b1 + i__ * z_dim1], &c__1, &
			    work[indrv1 + 1], &c__1);
/* L80: */
		}
	    }

/*           Check the infinity norm of the iterate. */

L90:
	    jmax = idamax_(&blksiz, &work[indrv1 + 1], &c__1);
	    nrm = (d__1 = work[indrv1 + jmax], abs(d__1));

/*           Continue for additional iterations after norm reaches */
/*           stopping criterion. */

	    if (nrm < dtpcrt) {
		goto L70;
	    }
	    ++nrmchk;
	    if (nrmchk < 3) {
		goto L70;
	    }

	    goto L110;

/*           If stopping criterion was not satisfied, update info and */
/*           store eigenvector number in array ifail. */

L100:
	    ++(*info);
	    ifail[*info] = j;

/*           Accept iterate as jth eigenvector. */

L110:
	    scl = 1. / dnrm2_(&blksiz, &work[indrv1 + 1], &c__1);
	    jmax = idamax_(&blksiz, &work[indrv1 + 1], &c__1);
	    if (work[indrv1 + jmax] < 0.) {
		scl = -scl;
	    }
	    dscal_(&blksiz, &scl, &work[indrv1 + 1], &c__1);
L120:
	    i__3 = *n;
	    for (i__ = 1; i__ <= i__3; ++i__) {
		z__[i__ + j * z_dim1] = 0.;
/* L130: */
	    }
	    i__3 = blksiz;
	    for (i__ = 1; i__ <= i__3; ++i__) {
		z__[b1 + i__ - 1 + j * z_dim1] = work[indrv1 + i__];
/* L140: */
	    }

/*           Save the shift to check eigenvalue spacing at next */
/*           iteration. */

	    xjm = xj;

/* L150: */
	}
L160:
	;
    }

    return 0;

/*     End of DSTEIN */

} /* dstein_ */

/* Subroutine */ int dstemr_(const char *jobz, const char *range, integer *n, double *
	d__, double *e, double *vl, double *vu, integer *il,
	integer *iu, integer *m, double *w, double *z__, integer *ldz,
	integer *nzc, integer *isuppz, bool *tryrac, double *work,
	integer *lwork, integer *iwork, integer *liwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b18 = .001;

    /* System generated locals */
    integer z_dim1, z_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j;
    double r1, r2;
    integer jj;
    double cs;
    integer in;
    double sn, wl, wu;
    integer iil, iiu;
    double eps, tmp;
    integer indd, iend, jblk, wend;
    double rmin, rmax;
    integer itmp;
    double tnrm;
    integer inde2, itmp2;
    double rtol1, rtol2;
    double scale;
    integer indgp;
    integer iinfo, iindw, ilast;
    integer lwmin;
    bool wantz;
	bool alleig;
    integer ibegin;
    bool indeig;
    integer iindbl;
    bool valeig;
    integer wbegin;
    double safmin;
    double bignum;
    integer inderr, iindwk, indgrs, offset;
    double thresh;
    integer iinspl, ifirst, indwrk, liwmin, nzcmin;
    double pivmin;
    integer nsplit;
    double smlnum;
    bool lquery, zquery;


/*  -- LAPACK computational routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSTEMR computes selected eigenvalues and, optionally, eigenvectors */
/*  of a real symmetric tridiagonal matrix T. Any such unreduced matrix has */
/*  a well defined set of pairwise different real eigenvalues, the corresponding */
/*  real eigenvectors are pairwise orthogonal. */

/*  The spectrum may be computed either completely or partially by specifying */
/*  either an interval (VL,VU] or a range of indices IL:IU for the desired */
/*  eigenvalues. */

/*  Depending on the number of desired eigenvalues, these are computed either */
/*  by bisection or the dqds algorithm. Numerically orthogonal eigenvectors are */
/*  computed by the use of various suitable L D L^T factorizations near clusters */
/*  of close eigenvalues (referred to as RRRs, Relatively Robust */
/*  Representations). An informal sketch of the algorithm follows. */

/*  For each unreduced block (submatrix) of T, */
/*     (a) Compute T - sigma I  = L D L^T, so that L and D */
/*         define all the wanted eigenvalues to high relative accuracy. */
/*         This means that small relative changes in the entries of D and L */
/*         cause only small relative changes in the eigenvalues and */
/*         eigenvectors. The standard (unfactored) representation of the */
/*         tridiagonal matrix T does not have this property in general. */
/*     (b) Compute the eigenvalues to suitable accuracy. */
/*         If the eigenvectors are desired, the algorithm attains full */
/*         accuracy of the computed eigenvalues only right before */
/*         the corresponding vectors have to be computed, see steps c) and d). */
/*     (c) For each cluster of close eigenvalues, select a new */
/*         shift close to the cluster, find a new factorization, and refine */
/*         the shifted eigenvalues to suitable accuracy. */
/*     (d) For each eigenvalue with a large enough relative separation compute */
/*         the corresponding eigenvector by forming a rank revealing twisted */
/*         factorization. Go back to (c) for any clusters that remain. */

/*  For more details, see: */
/*  - Inderjit S. Dhillon and Beresford N. Parlett: "Multiple representations */
/*    to compute orthogonal eigenvectors of symmetric tridiagonal matrices," */
/*    Linear Algebra and its Applications, 387(1), pp. 1-28, August 2004. */
/*  - Inderjit Dhillon and Beresford Parlett: "Orthogonal Eigenvectors and */
/*    Relative Gaps," SIAM Journal on Matrix Analysis and Applications, Vol. 25, */
/*    2004.  Also LAPACK Working Note 154. */
/*  - Inderjit Dhillon: "A new O(n^2) algorithm for the symmetric */
/*    tridiagonal eigenvalue/eigenvector problem", */
/*    Computer Science Division Technical Report No. UCB/CSD-97-971, */
/*    UC Berkeley, May 1997. */

/*  Notes: */
/*  1.DSTEMR works only on machines which follow IEEE-754 */
/*  floating-point standard in their handling of infinities and NaNs. */
/*  This permits the use of efficient inner loops avoiding a check for */
/*  zero divisors. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  RANGE   (input) CHARACTER*1 */
/*          = 'A': all eigenvalues will be found. */
/*          = 'V': all eigenvalues in the half-open interval (VL,VU] */
/*                 will be found. */
/*          = 'I': the IL-th through IU-th eigenvalues will be found. */

/*  N       (input) INTEGER */
/*          The order of the matrix.  N >= 0. */

/*  D       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the N diagonal elements of the tridiagonal matrix */
/*          T. On exit, D is overwritten. */

/*  E       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the (N-1) subdiagonal elements of the tridiagonal */
/*          matrix T in elements 1 to N-1 of E. E(N) need not be set on */
/*          input, but is used internally as workspace. */
/*          On exit, E is overwritten. */

/*  VL      (input) DOUBLE PRECISION */
/*  VU      (input) DOUBLE PRECISION */
/*          If RANGE='V', the lower and upper bounds of the interval to */
/*          be searched for eigenvalues. VL < VU. */
/*          Not referenced if RANGE = 'A' or 'I'. */

/*  IL      (input) INTEGER */
/*  IU      (input) INTEGER */
/*          If RANGE='I', the indices (in ascending order) of the */
/*          smallest and largest eigenvalues to be returned. */
/*          1 <= IL <= IU <= N, if N > 0. */
/*          Not referenced if RANGE = 'A' or 'V'. */

/*  M       (output) INTEGER */
/*          The total number of eigenvalues found.  0 <= M <= N. */
/*          If RANGE = 'A', M = N, and if RANGE = 'I', M = IU-IL+1. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          The first M elements contain the selected eigenvalues in */
/*          ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, max(1,M) ) */
/*          If JOBZ = 'V', and if INFO = 0, then the first M columns of Z */
/*          contain the orthonormal eigenvectors of the matrix T */
/*          corresponding to the selected eigenvalues, with the i-th */
/*          column of Z holding the eigenvector associated with W(i). */
/*          If JOBZ = 'N', then Z is not referenced. */
/*          Note: the user must ensure that at least max(1,M) columns are */
/*          supplied in the array Z; if RANGE = 'V', the exact value of M */
/*          is not known in advance and can be computed with a workspace */
/*          query by setting NZC = -1, see below. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', then LDZ >= max(1,N). */

/*  NZC     (input) INTEGER */
/*          The number of eigenvectors to be held in the array Z. */
/*          If RANGE = 'A', then NZC >= max(1,N). */
/*          If RANGE = 'V', then NZC >= the number of eigenvalues in (VL,VU]. */
/*          If RANGE = 'I', then NZC >= IU-IL+1. */
/*          If NZC = -1, then a workspace query is assumed; the */
/*          routine calculates the number of columns of the array Z that */
/*          are needed to hold the eigenvectors. */
/*          This value is returned as the first entry of the Z array, and */
/*          no error message related to NZC is issued by XERBLA. */

/*  ISUPPZ  (output) INTEGER ARRAY, dimension ( 2*max(1,M) ) */
/*          The support of the eigenvectors in Z, i.e., the indices */
/*          indicating the nonzero elements in Z. The i-th computed eigenvector */
/*          is nonzero only in elements ISUPPZ( 2*i-1 ) through */
/*          ISUPPZ( 2*i ). This is relevant in the case when the matrix */
/*          is split. ISUPPZ is only accessed when JOBZ is 'V' and N > 0. */

/*  TRYRAC  (input/output) LOGICAL */
/*          If TRYRAC.EQ..TRUE., indicates that the code should check whether */
/*          the tridiagonal matrix defines its eigenvalues to high relative */
/*          accuracy.  If so, the code uses relative-accuracy preserving */
/*          algorithms that might be (a bit) slower depending on the matrix. */
/*          If the matrix does not define its eigenvalues to high relative */
/*          accuracy, the code can uses possibly faster algorithms. */
/*          If TRYRAC.EQ..FALSE., the code is not required to guarantee */
/*          relatively accurate eigenvalues and can use the fastest possible */
/*          techniques. */
/*          On exit, a .TRUE. TRYRAC will be set to .FALSE. if the matrix */
/*          does not define its eigenvalues to high relative accuracy. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (LWORK) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal */
/*          (and minimal) LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. LWORK >= max(1,18*N) */
/*          if JOBZ = 'V', and LWORK >= max(1,12*N) if JOBZ = 'N'. */
/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  IWORK   (workspace/output) INTEGER array, dimension (LIWORK) */
/*          On exit, if INFO = 0, IWORK(1) returns the optimal LIWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK.  LIWORK >= max(1,10*N) */
/*          if the eigenvectors are desired, and LIWORK >= max(1,8*N) */
/*          if only the eigenvalues are to be computed. */
/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates the optimal size of the IWORK array, */
/*          returns this value as the first entry of the IWORK array, and */
/*          no error message related to LIWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          On exit, INFO */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = 1X, internal error in DLARRE, */
/*                if INFO = 2X, internal error in DLARRV. */
/*                Here, the digit X = ABS( IINFO ) < 10, where IINFO is */
/*                the nonzero error code returned by DLARRE or */
/*                DLARRV, respectively. */


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
    --e;
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --isuppz;
    --work;
    --iwork;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    alleig = lsame_(range, "A");
    valeig = lsame_(range, "V");
    indeig = lsame_(range, "I");

    lquery = *lwork == -1 || *liwork == -1;
    zquery = *nzc == -1;
    *tryrac = *info != 0;
/*     DSTEMR needs WORK of size 6*N, IWORK of size 3*N. */
/*     In addition, DLARRE needs WORK of size 6*N, IWORK of size 5*N. */
/*     Furthermore, DLARRV needs WORK of size 12*N, IWORK of size 7*N. */
    if (wantz) {
	lwmin = *n * 18;
	liwmin = *n * 10;
    } else {
/*        need less workspace if only the eigenvalues are wanted */
	lwmin = *n * 12;
	liwmin = *n << 3;
    }
    wl = 0.;
    wu = 0.;
    iil = 0;
    iiu = 0;
    if (valeig) {
/*        We do not reference VL, VU in the cases RANGE = 'I','A' */
/*        The interval (WL, WU] contains all the wanted eigenvalues. */
/*        It is either given by the user or computed in DLARRE. */
	wl = *vl;
	wu = *vu;
    } else if (indeig) {
/*        We do not reference IL, IU in the cases RANGE = 'V','A' */
	iil = *il;
	iiu = *iu;
    }

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (alleig || valeig || indeig)) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (valeig && *n > 0 && wu <= wl) {
	*info = -7;
    } else if (indeig && (iil < 1 || iil > *n)) {
	*info = -8;
    } else if (indeig && (iiu < iil || iiu > *n)) {
	*info = -9;
    } else if (*ldz < 1 || wantz && *ldz < *n) {
	*info = -13;
    } else if (*lwork < lwmin && ! lquery) {
	*info = -17;
    } else if (*liwork < liwmin && ! lquery) {
	*info = -19;
    }

/*     Get machine constants. */

    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
/* Computing MIN */
    d__1 = sqrt(bignum), d__2 = 1. / sqrt(sqrt(safmin));
    rmax = std::min(d__1,d__2);

    if (*info == 0) {
	work[1] = (double) lwmin;
	iwork[1] = liwmin;

	if (wantz && alleig) {
	    nzcmin = *n;
	} else if (wantz && valeig) {
	    dlarrc_("T", n, vl, vu, &d__[1], &e[1], &safmin, &nzcmin, &itmp, &
		    itmp2, info);
	} else if (wantz && indeig) {
	    nzcmin = iiu - iil + 1;
	} else {
/*           WANTZ .EQ. FALSE. */
	    nzcmin = 0;
	}
	if (zquery && *info == 0) {
	    z__[z_dim1 + 1] = (double) nzcmin;
	} else if (*nzc < nzcmin && ! zquery) {
	    *info = -14;
	}
    }
    if (*info != 0) {

	i__1 = -(*info);
	xerbla_("DSTEMR", &i__1);

	return 0;
    } else if (lquery || zquery) {
	return 0;
    }

/*     Handle N = 0, 1, and 2 cases immediately */

    *m = 0;
    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (alleig || indeig) {
	    *m = 1;
	    w[1] = d__[1];
	} else {
	    if (wl < d__[1] && wu >= d__[1]) {
		*m = 1;
		w[1] = d__[1];
	    }
	}
	if (wantz && ! zquery) {
	    z__[z_dim1 + 1] = 1.;
	    isuppz[1] = 1;
	    isuppz[2] = 1;
	}
	return 0;
    }

    if (*n == 2) {
	if (! wantz) {
	    dlae2_(&d__[1], &e[1], &d__[2], &r1, &r2);
	} else if (wantz && ! zquery) {
	    dlaev2_(&d__[1], &e[1], &d__[2], &r1, &r2, &cs, &sn);
	}
	if (alleig || valeig && r2 > wl && r2 <= wu || indeig && iil == 1) {
	    ++(*m);
	    w[*m] = r2;
	    if (wantz && ! zquery) {
		z__[*m * z_dim1 + 1] = -sn;
		z__[*m * z_dim1 + 2] = cs;
/*              Note: At most one of SN and CS can be zero. */
		if (sn != 0.) {
		    if (cs != 0.) {
			isuppz[(*m << 1) - 1] = 1;
			isuppz[(*m << 1) - 1] = 2;
		    } else {
			isuppz[(*m << 1) - 1] = 1;
			isuppz[(*m << 1) - 1] = 1;
		    }
		} else {
		    isuppz[(*m << 1) - 1] = 2;
		    isuppz[*m * 2] = 2;
		}
	    }
	}
	if (alleig || valeig && r1 > wl && r1 <= wu || indeig && iiu == 2) {
	    ++(*m);
	    w[*m] = r1;
	    if (wantz && ! zquery) {
		z__[*m * z_dim1 + 1] = cs;
		z__[*m * z_dim1 + 2] = sn;
/*              Note: At most one of SN and CS can be zero. */
		if (sn != 0.) {
		    if (cs != 0.) {
			isuppz[(*m << 1) - 1] = 1;
			isuppz[(*m << 1) - 1] = 2;
		    } else {
			isuppz[(*m << 1) - 1] = 1;
			isuppz[(*m << 1) - 1] = 1;
		    }
		} else {
		    isuppz[(*m << 1) - 1] = 2;
		    isuppz[*m * 2] = 2;
		}
	    }
	}
	return 0;
    }
/*     Continue with general N */
    indgrs = 1;
    inderr = (*n << 1) + 1;
    indgp = *n * 3 + 1;
    indd = (*n << 2) + 1;
    inde2 = *n * 5 + 1;
    indwrk = *n * 6 + 1;

    iinspl = 1;
    iindbl = *n + 1;
    iindw = (*n << 1) + 1;
    iindwk = *n * 3 + 1;

/*     Scale matrix to allowable range, if necessary. */
/*     The allowable range is related to the PIVMIN parameter; see the */
/*     comments in DLARRD.  The preference for scaling small values */
/*     up is heuristic; we expect users' matrices not to be close to the */
/*     RMAX threshold. */

    scale = 1.;
    tnrm = dlanst_("M", n, &d__[1], &e[1]);
    if (tnrm > 0. && tnrm < rmin) {
	scale = rmin / tnrm;
    } else if (tnrm > rmax) {
	scale = rmax / tnrm;
    }
    if (scale != 1.) {
	dscal_(n, &scale, &d__[1], &c__1);
	i__1 = *n - 1;
	dscal_(&i__1, &scale, &e[1], &c__1);
	tnrm *= scale;
	if (valeig) {
/*           If eigenvalues in interval have to be found, */
/*           scale (WL, WU] accordingly */
	    wl *= scale;
	    wu *= scale;
	}
    }

/*     Compute the desired eigenvalues of the tridiagonal after splitting */
/*     into smaller subblocks if the corresponding off-diagonal elements */
/*     are small */
/*     THRESH is the splitting parameter for DLARRE */
/*     A negative THRESH forces the old splitting criterion based on the */
/*     size of the off-diagonal. A positive THRESH switches to splitting */
/*     which preserves relative accuracy. */

    if (*tryrac) {
/*        Test whether the matrix warrants the more expensive relative approach. */
	dlarrr_(n, &d__[1], &e[1], &iinfo);
    } else {
/*        The user does not care about relative accurately eigenvalues */
	iinfo = -1;
    }
/*     Set the splitting criterion */
    if (iinfo == 0) {
	thresh = eps;
    } else {
	thresh = -eps;
/*        relative accuracy is desired but T does not guarantee it */
	*tryrac = false;
    }

    if (*tryrac) {
/*        Copy original diagonal, needed to guarantee relative accuracy */
	dcopy_(n, &d__[1], &c__1, &work[indd], &c__1);
    }
/*     Store the squares of the offdiagonal values of T */
    i__1 = *n - 1;
    for (j = 1; j <= i__1; ++j) {
/* Computing 2nd power */
	d__1 = e[j];
	work[inde2 + j - 1] = d__1 * d__1;
/* L5: */
    }
/*     Set the tolerance parameters for bisection */
    if (! wantz) {
/*        DLARRE computes the eigenvalues to full precision. */
	rtol1 = eps * 4.;
	rtol2 = eps * 4.;
    } else {
/*        DLARRE computes the eigenvalues to less than full precision. */
/*        DLARRV will refine the eigenvalue approximations, and we can */
/*        need less accurate initial bisection in DLARRE. */
/*        Note: these settings do only affect the subset case and DLARRE */
	rtol1 = sqrt(eps);
/* Computing MAX */
	d__1 = sqrt(eps) * .005, d__2 = eps * 4.;
	rtol2 = std::max(d__1,d__2);
    }
    dlarre_(range, n, &wl, &wu, &iil, &iiu, &d__[1], &e[1], &work[inde2], &
	    rtol1, &rtol2, &thresh, &nsplit, &iwork[iinspl], m, &w[1], &work[
	    inderr], &work[indgp], &iwork[iindbl], &iwork[iindw], &work[
	    indgrs], &pivmin, &work[indwrk], &iwork[iindwk], &iinfo);
    if (iinfo != 0) {
	*info = abs(iinfo) + 10;
	return 0;
    }
/*     Note that if RANGE .NE. 'V', DLARRE computes bounds on the desired */
/*     part of the spectrum. All desired eigenvalues are contained in */
/*     (WL,WU] */
    if (wantz) {

/*        Compute the desired eigenvectors corresponding to the computed */
/*        eigenvalues */

	dlarrv_(n, &wl, &wu, &d__[1], &e[1], &pivmin, &iwork[iinspl], m, &
		c__1, m, &c_b18, &rtol1, &rtol2, &w[1], &work[inderr], &work[
		indgp], &iwork[iindbl], &iwork[iindw], &work[indgrs], &z__[
		z_offset], ldz, &isuppz[1], &work[indwrk], &iwork[iindwk], &
		iinfo);
	if (iinfo != 0) {
	    *info = abs(iinfo) + 20;
	    return 0;
	}
    } else {
/*        DLARRE computes eigenvalues of the (shifted) root representation */
/*        DLARRV returns the eigenvalues of the unshifted matrix. */
/*        However, if the eigenvectors are not desired by the user, we need */
/*        to apply the corresponding shifts from DLARRE to obtain the */
/*        eigenvalues of the original matrix. */
	i__1 = *m;
	for (j = 1; j <= i__1; ++j) {
	    itmp = iwork[iindbl + j - 1];
	    w[j] += e[iwork[iinspl + itmp - 1]];
/* L20: */
	}
    }

    if (*tryrac) {
/*        Refine computed eigenvalues so that they are relatively accurate */
/*        with respect to the original matrix T. */
	ibegin = 1;
	wbegin = 1;
	i__1 = iwork[iindbl + *m - 1];
	for (jblk = 1; jblk <= i__1; ++jblk) {
	    iend = iwork[iinspl + jblk - 1];
	    in = iend - ibegin + 1;
	    wend = wbegin - 1;
/*           check if any eigenvalues have to be refined in this block */
L36:
	    if (wend < *m) {
		if (iwork[iindbl + wend] == jblk) {
		    ++wend;
		    goto L36;
		}
	    }
	    if (wend < wbegin) {
		ibegin = iend + 1;
		goto L39;
	    }
	    offset = iwork[iindw + wbegin - 1] - 1;
	    ifirst = iwork[iindw + wbegin - 1];
	    ilast = iwork[iindw + wend - 1];
	    rtol2 = eps * 4.;
	    dlarrj_(&in, &work[indd + ibegin - 1], &work[inde2 + ibegin - 1],
		    &ifirst, &ilast, &rtol2, &offset, &w[wbegin], &work[
		    inderr + wbegin - 1], &work[indwrk], &iwork[iindwk], &
		    pivmin, &tnrm, &iinfo);
	    ibegin = iend + 1;
	    wbegin = wend + 1;
L39:
	    ;
	}
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

    if (scale != 1.) {
	d__1 = 1. / scale;
	dscal_(m, &d__1, &w[1], &c__1);
    }

/*     If eigenvalues are not in increasing order, then sort them, */
/*     possibly along with eigenvectors. */

    if (nsplit > 1) {
	if (! wantz) {
	    dlasrt_("I", m, &w[1], &iinfo);
	    if (iinfo != 0) {
		*info = 3;
		return 0;
	    }
	} else {
	    i__1 = *m - 1;
	    for (j = 1; j <= i__1; ++j) {
		i__ = 0;
		tmp = w[j];
		i__2 = *m;
		for (jj = j + 1; jj <= i__2; ++jj) {
		    if (w[jj] < tmp) {
			i__ = jj;
			tmp = w[jj];
		    }
/* L50: */
		}
		if (i__ != 0) {
		    w[i__] = w[j];
		    w[j] = tmp;
		    if (wantz) {
			dswap_(n, &z__[i__ * z_dim1 + 1], &c__1, &z__[j *
				z_dim1 + 1], &c__1);
			itmp = isuppz[(i__ << 1) - 1];
			isuppz[(i__ << 1) - 1] = isuppz[(j << 1) - 1];
			isuppz[(j << 1) - 1] = itmp;
			itmp = isuppz[i__ * 2];
			isuppz[i__ * 2] = isuppz[j * 2];
			isuppz[j * 2] = itmp;
		    }
		}
/* L60: */
	    }
	}
    }


    work[1] = (double) lwmin;
    iwork[1] = liwmin;
    return 0;

/*     End of DSTEMR */

} /* dstemr_ */

/* Subroutine */ int dsteqr_(const char *compz, integer *n, double *d__,
	double *e, double *z__, integer *ldz, double *work,
	integer *info)
{
	/* Table of constant values */
	static double c_b9 = 0.;
	static double c_b10 = 1.;
	static integer c__0 = 0;
	static integer c__1 = 1;
	static integer c__2 = 2;

    /* System generated locals */
    integer z_dim1, z_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    double b, c__, f, g;
    integer i__, j, k, l, m;
    double p, r__, s;
    integer l1, ii, mm, lm1, mm1, nm1;
    double rt1, rt2, eps;
    integer lsv;
    double tst, eps2;
    integer lend, jtot;
    double anorm;
    integer lendm1, lendp1;
    integer iscale;
    double safmin;
    double safmax;
    integer lendsv;
    double ssfmin;
    integer nmaxit, icompz;
    double ssfmax;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSTEQR computes all eigenvalues and, optionally, eigenvectors of a */
/*  symmetric tridiagonal matrix using the implicit QL or QR method. */
/*  The eigenvectors of a full or band symmetric matrix can also be found */
/*  if DSYTRD or DSPTRD or DSBTRD has been used to reduce this matrix to */
/*  tridiagonal form. */

/*  Arguments */
/*  ========= */

/*  COMPZ   (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only. */
/*          = 'V':  Compute eigenvalues and eigenvectors of the original */
/*                  symmetric matrix.  On entry, Z must contain the */
/*                  orthogonal matrix used to reduce the original matrix */
/*                  to tridiagonal form. */
/*          = 'I':  Compute eigenvalues and eigenvectors of the */
/*                  tridiagonal matrix.  Z is initialized to the identity */
/*                  matrix. */

/*  N       (input) INTEGER */
/*          The order of the matrix.  N >= 0. */

/*  D       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the diagonal elements of the tridiagonal matrix. */
/*          On exit, if INFO = 0, the eigenvalues in ascending order. */

/*  E       (input/output) DOUBLE PRECISION array, dimension (N-1) */
/*          On entry, the (n-1) subdiagonal elements of the tridiagonal */
/*          matrix. */
/*          On exit, E has been destroyed. */

/*  Z       (input/output) DOUBLE PRECISION array, dimension (LDZ, N) */
/*          On entry, if  COMPZ = 'V', then Z contains the orthogonal */
/*          matrix used in the reduction to tridiagonal form. */
/*          On exit, if INFO = 0, then if  COMPZ = 'V', Z contains the */
/*          orthonormal eigenvectors of the original symmetric matrix, */
/*          and if COMPZ = 'I', Z contains the orthonormal eigenvectors */
/*          of the symmetric tridiagonal matrix. */
/*          If COMPZ = 'N', then Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          eigenvectors are desired, then  LDZ >= max(1,N). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (max(1,2*N-2)) */
/*          If COMPZ = 'N', then WORK is not referenced. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  the algorithm has failed to find all the eigenvalues in */
/*                a total of 30*N iterations; if INFO = i, then i */
/*                elements of E have not converged to zero; on exit, D */
/*                and E contain the elements of a symmetric tridiagonal */
/*                matrix which is orthogonally similar to the original */
/*                matrix. */

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
    --e;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;

    /* Function Body */
    *info = 0;

    if (lsame_(compz, "N")) {
	icompz = 0;
    } else if (lsame_(compz, "V")) {
	icompz = 1;
    } else if (lsame_(compz, "I")) {
	icompz = 2;
    } else {
	icompz = -1;
    }
    if (icompz < 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*ldz < 1 || icompz > 0 && *ldz < std::max(1_integer,*n)) {
	*info = -6;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSTEQR", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (icompz == 2) {
	    z__[z_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Determine the unit roundoff and over/underflow thresholds. */

    eps = dlamch_("E");
/* Computing 2nd power */
    d__1 = eps;
    eps2 = d__1 * d__1;
    safmin = dlamch_("S");
    safmax = 1. / safmin;
    ssfmax = sqrt(safmax) / 3.;
    ssfmin = sqrt(safmin) / eps2;

/*     Compute the eigenvalues and eigenvectors of the tridiagonal */
/*     matrix. */

    if (icompz == 2) {
	dlaset_("Full", n, n, &c_b9, &c_b10, &z__[z_offset], ldz);
    }

    nmaxit = *n * 30;
    jtot = 0;

/*     Determine where the matrix splits and choose QL or QR iteration */
/*     for each block, according to whether top or bottom diagonal */
/*     element is smaller. */

    l1 = 1;
    nm1 = *n - 1;

L10:
    if (l1 > *n) {
	goto L160;
    }
    if (l1 > 1) {
	e[l1 - 1] = 0.;
    }
    if (l1 <= nm1) {
	i__1 = nm1;
	for (m = l1; m <= i__1; ++m) {
	    tst = (d__1 = e[m], abs(d__1));
	    if (tst == 0.) {
		goto L30;
	    }
	    if (tst <= sqrt((d__1 = d__[m], abs(d__1))) * sqrt((d__2 = d__[m
		    + 1], abs(d__2))) * eps) {
		e[m] = 0.;
		goto L30;
	    }
/* L20: */
	}
    }
    m = *n;

L30:
    l = l1;
    lsv = l;
    lend = m;
    lendsv = lend;
    l1 = m + 1;
    if (lend == l) {
	goto L10;
    }

/*     Scale submatrix in rows and columns L to LEND */

    i__1 = lend - l + 1;
    anorm = dlanst_("I", &i__1, &d__[l], &e[l]);
    iscale = 0;
    if (anorm == 0.) {
	goto L10;
    }
    if (anorm > ssfmax) {
	iscale = 1;
	i__1 = lend - l + 1;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &d__[l], n,
		info);
	i__1 = lend - l;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &e[l], n,
		info);
    } else if (anorm < ssfmin) {
	iscale = 2;
	i__1 = lend - l + 1;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &d__[l], n,
		info);
	i__1 = lend - l;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &e[l], n,
		info);
    }

/*     Choose between QL and QR iteration */

    if ((d__1 = d__[lend], abs(d__1)) < (d__2 = d__[l], abs(d__2))) {
	lend = lsv;
	l = lendsv;
    }

    if (lend > l) {

/*        QL Iteration */

/*        Look for small subdiagonal element. */

L40:
	if (l != lend) {
	    lendm1 = lend - 1;
	    i__1 = lendm1;
	    for (m = l; m <= i__1; ++m) {
/* Computing 2nd power */
		d__2 = (d__1 = e[m], abs(d__1));
		tst = d__2 * d__2;
		if (tst <= eps2 * (d__1 = d__[m], abs(d__1)) * (d__2 = d__[m
			+ 1], abs(d__2)) + safmin) {
		    goto L60;
		}
/* L50: */
	    }
	}

	m = lend;

L60:
	if (m < lend) {
	    e[m] = 0.;
	}
	p = d__[l];
	if (m == l) {
	    goto L80;
	}

/*        If remaining matrix is 2-by-2, use DLAE2 or SLAEV2 */
/*        to compute its eigensystem. */

	if (m == l + 1) {
	    if (icompz > 0) {
		dlaev2_(&d__[l], &e[l], &d__[l + 1], &rt1, &rt2, &c__, &s);
		work[l] = c__;
		work[*n - 1 + l] = s;
		dlasr_("R", "V", "B", n, &c__2, &work[l], &work[*n - 1 + l], &
			z__[l * z_dim1 + 1], ldz);
	    } else {
		dlae2_(&d__[l], &e[l], &d__[l + 1], &rt1, &rt2);
	    }
	    d__[l] = rt1;
	    d__[l + 1] = rt2;
	    e[l] = 0.;
	    l += 2;
	    if (l <= lend) {
		goto L40;
	    }
	    goto L140;
	}

	if (jtot == nmaxit) {
	    goto L140;
	}
	++jtot;

/*        Form shift. */

	g = (d__[l + 1] - p) / (e[l] * 2.);
	r__ = dlapy2_(&g, &c_b10);
	g = d__[m] - p + e[l] / (g + d_sign(&r__, &g));

	s = 1.;
	c__ = 1.;
	p = 0.;

/*        Inner loop */

	mm1 = m - 1;
	i__1 = l;
	for (i__ = mm1; i__ >= i__1; --i__) {
	    f = s * e[i__];
	    b = c__ * e[i__];
	    dlartg_(&g, &f, &c__, &s, &r__);
	    if (i__ != m - 1) {
		e[i__ + 1] = r__;
	    }
	    g = d__[i__ + 1] - p;
	    r__ = (d__[i__] - g) * s + c__ * 2. * b;
	    p = s * r__;
	    d__[i__ + 1] = g + p;
	    g = c__ * r__ - b;

/*           If eigenvectors are desired, then save rotations. */

	    if (icompz > 0) {
		work[i__] = c__;
		work[*n - 1 + i__] = -s;
	    }

/* L70: */
	}

/*        If eigenvectors are desired, then apply saved rotations. */

	if (icompz > 0) {
	    mm = m - l + 1;
	    dlasr_("R", "V", "B", n, &mm, &work[l], &work[*n - 1 + l], &z__[l
		    * z_dim1 + 1], ldz);
	}

	d__[l] -= p;
	e[l] = g;
	goto L40;

/*        Eigenvalue found. */

L80:
	d__[l] = p;

	++l;
	if (l <= lend) {
	    goto L40;
	}
	goto L140;

    } else {

/*        QR Iteration */

/*        Look for small superdiagonal element. */

L90:
	if (l != lend) {
	    lendp1 = lend + 1;
	    i__1 = lendp1;
	    for (m = l; m >= i__1; --m) {
/* Computing 2nd power */
		d__2 = (d__1 = e[m - 1], abs(d__1));
		tst = d__2 * d__2;
		if (tst <= eps2 * (d__1 = d__[m], abs(d__1)) * (d__2 = d__[m
			- 1], abs(d__2)) + safmin) {
		    goto L110;
		}
/* L100: */
	    }
	}

	m = lend;

L110:
	if (m > lend) {
	    e[m - 1] = 0.;
	}
	p = d__[l];
	if (m == l) {
	    goto L130;
	}

/*        If remaining matrix is 2-by-2, use DLAE2 or SLAEV2 */
/*        to compute its eigensystem. */

	if (m == l - 1) {
	    if (icompz > 0) {
		dlaev2_(&d__[l - 1], &e[l - 1], &d__[l], &rt1, &rt2, &c__, &s)
			;
		work[m] = c__;
		work[*n - 1 + m] = s;
		dlasr_("R", "V", "F", n, &c__2, &work[m], &work[*n - 1 + m], &
			z__[(l - 1) * z_dim1 + 1], ldz);
	    } else {
		dlae2_(&d__[l - 1], &e[l - 1], &d__[l], &rt1, &rt2);
	    }
	    d__[l - 1] = rt1;
	    d__[l] = rt2;
	    e[l - 1] = 0.;
	    l += -2;
	    if (l >= lend) {
		goto L90;
	    }
	    goto L140;
	}

	if (jtot == nmaxit) {
	    goto L140;
	}
	++jtot;

/*        Form shift. */

	g = (d__[l - 1] - p) / (e[l - 1] * 2.);
	r__ = dlapy2_(&g, &c_b10);
	g = d__[m] - p + e[l - 1] / (g + d_sign(&r__, &g));

	s = 1.;
	c__ = 1.;
	p = 0.;

/*        Inner loop */

	lm1 = l - 1;
	i__1 = lm1;
	for (i__ = m; i__ <= i__1; ++i__) {
	    f = s * e[i__];
	    b = c__ * e[i__];
	    dlartg_(&g, &f, &c__, &s, &r__);
	    if (i__ != m) {
		e[i__ - 1] = r__;
	    }
	    g = d__[i__] - p;
	    r__ = (d__[i__ + 1] - g) * s + c__ * 2. * b;
	    p = s * r__;
	    d__[i__] = g + p;
	    g = c__ * r__ - b;

/*           If eigenvectors are desired, then save rotations. */

	    if (icompz > 0) {
		work[i__] = c__;
		work[*n - 1 + i__] = s;
	    }

/* L120: */
	}

/*        If eigenvectors are desired, then apply saved rotations. */

	if (icompz > 0) {
	    mm = l - m + 1;
	    dlasr_("R", "V", "F", n, &mm, &work[m], &work[*n - 1 + m], &z__[m
		    * z_dim1 + 1], ldz);
	}

	d__[l] -= p;
	e[lm1] = g;
	goto L90;

/*        Eigenvalue found. */

L130:
	d__[l] = p;

	--l;
	if (l >= lend) {
	    goto L90;
	}
	goto L140;

    }

/*     Undo scaling if necessary */

L140:
    if (iscale == 1) {
	i__1 = lendsv - lsv + 1;
	dlascl_("G", &c__0, &c__0, &ssfmax, &anorm, &i__1, &c__1, &d__[lsv],
		n, info);
	i__1 = lendsv - lsv;
	dlascl_("G", &c__0, &c__0, &ssfmax, &anorm, &i__1, &c__1, &e[lsv], n,
		info);
    } else if (iscale == 2) {
	i__1 = lendsv - lsv + 1;
	dlascl_("G", &c__0, &c__0, &ssfmin, &anorm, &i__1, &c__1, &d__[lsv],
		n, info);
	i__1 = lendsv - lsv;
	dlascl_("G", &c__0, &c__0, &ssfmin, &anorm, &i__1, &c__1, &e[lsv], n,
		info);
    }

/*     Check for no convergence to an eigenvalue after a total */
/*     of N*MAXIT iterations. */

    if (jtot < nmaxit) {
	goto L10;
    }
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (e[i__] != 0.) {
	    ++(*info);
	}
/* L150: */
    }
    goto L190;

/*     Order eigenvalues and eigenvectors. */

L160:
    if (icompz == 0) {

/*        Use Quick Sort */

	dlasrt_("I", n, &d__[1], info);

    } else {

/*        Use Selection Sort to minimize swaps of eigenvectors */

	i__1 = *n;
	for (ii = 2; ii <= i__1; ++ii) {
	    i__ = ii - 1;
	    k = i__;
	    p = d__[i__];
	    i__2 = *n;
	    for (j = ii; j <= i__2; ++j) {
		if (d__[j] < p) {
		    k = j;
		    p = d__[j];
		}
/* L170: */
	    }
	    if (k != i__) {
		d__[k] = d__[i__];
		d__[i__] = p;
		dswap_(n, &z__[i__ * z_dim1 + 1], &c__1, &z__[k * z_dim1 + 1],
			 &c__1);
	    }
/* L180: */
	}
    }

L190:
    return 0;

/*     End of DSTEQR */

} /* dsteqr_ */

/* Subroutine */ int dsterf_(integer *n, double *d__, double *e,
	integer *info)
{
	/* Table of constant values */
	static integer c__0 = 0;
	static integer c__1 = 1;
	static double c_b32 = 1.;

    /* System generated locals */
    integer i__1;
    double d__1, d__2, d__3;

    /* Local variables */
    double c__;
    integer i__, l, m;
    double p, r__, s;
    integer l1;
    double bb, rt1, rt2, eps, rte;
    integer lsv;
    double eps2, oldc;
    integer lend, jtot;
    double gamma, alpha, sigma, anorm;
    integer iscale;
    double oldgam, safmin;
    double safmax;
    integer lendsv;
    double ssfmin;
    integer nmaxit;
    double ssfmax;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSTERF computes all eigenvalues of a symmetric tridiagonal matrix */
/*  using the Pal-Walker-Kahan variant of the QL or QR algorithm. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix.  N >= 0. */

/*  D       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the n diagonal elements of the tridiagonal matrix. */
/*          On exit, if INFO = 0, the eigenvalues in ascending order. */

/*  E       (input/output) DOUBLE PRECISION array, dimension (N-1) */
/*          On entry, the (n-1) subdiagonal elements of the tridiagonal */
/*          matrix. */
/*          On exit, E has been destroyed. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  the algorithm failed to find all of the eigenvalues in */
/*                a total of 30*N iterations; if INFO = i, then i */
/*                elements of E have not converged to zero. */

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
    --e;
    --d__;

    /* Function Body */
    *info = 0;

/*     Quick return if possible */

    if (*n < 0) {
	*info = -1;
	i__1 = -(*info);
	xerbla_("DSTERF", &i__1);
	return 0;
    }
    if (*n <= 1) {
	return 0;
    }

/*     Determine the unit roundoff for this environment. */

    eps = dlamch_("E");
/* Computing 2nd power */
    d__1 = eps;
    eps2 = d__1 * d__1;
    safmin = dlamch_("S");
    safmax = 1. / safmin;
    ssfmax = sqrt(safmax) / 3.;
    ssfmin = sqrt(safmin) / eps2;

/*     Compute the eigenvalues of the tridiagonal matrix. */

    nmaxit = *n * 30;
    sigma = 0.;
    jtot = 0;

/*     Determine where the matrix splits and choose QL or QR iteration */
/*     for each block, according to whether top or bottom diagonal */
/*     element is smaller. */

    l1 = 1;

L10:
    if (l1 > *n) {
	goto L170;
    }
    if (l1 > 1) {
	e[l1 - 1] = 0.;
    }
    i__1 = *n - 1;
    for (m = l1; m <= i__1; ++m) {
	if ((d__3 = e[m], abs(d__3)) <= sqrt((d__1 = d__[m], abs(d__1))) *
		sqrt((d__2 = d__[m + 1], abs(d__2))) * eps) {
	    e[m] = 0.;
	    goto L30;
	}
/* L20: */
    }
    m = *n;

L30:
    l = l1;
    lsv = l;
    lend = m;
    lendsv = lend;
    l1 = m + 1;
    if (lend == l) {
	goto L10;
    }

/*     Scale submatrix in rows and columns L to LEND */

    i__1 = lend - l + 1;
    anorm = dlanst_("I", &i__1, &d__[l], &e[l]);
    iscale = 0;
    if (anorm > ssfmax) {
	iscale = 1;
	i__1 = lend - l + 1;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &d__[l], n,
		info);
	i__1 = lend - l;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmax, &i__1, &c__1, &e[l], n,
		info);
    } else if (anorm < ssfmin) {
	iscale = 2;
	i__1 = lend - l + 1;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &d__[l], n,
		info);
	i__1 = lend - l;
	dlascl_("G", &c__0, &c__0, &anorm, &ssfmin, &i__1, &c__1, &e[l], n,
		info);
    }

    i__1 = lend - 1;
    for (i__ = l; i__ <= i__1; ++i__) {
/* Computing 2nd power */
	d__1 = e[i__];
	e[i__] = d__1 * d__1;
/* L40: */
    }

/*     Choose between QL and QR iteration */

    if ((d__1 = d__[lend], abs(d__1)) < (d__2 = d__[l], abs(d__2))) {
	lend = lsv;
	l = lendsv;
    }

    if (lend >= l) {

/*        QL Iteration */

/*        Look for small subdiagonal element. */

L50:
	if (l != lend) {
	    i__1 = lend - 1;
	    for (m = l; m <= i__1; ++m) {
		if ((d__2 = e[m], abs(d__2)) <= eps2 * (d__1 = d__[m] * d__[m
			+ 1], abs(d__1))) {
		    goto L70;
		}
/* L60: */
	    }
	}
	m = lend;

L70:
	if (m < lend) {
	    e[m] = 0.;
	}
	p = d__[l];
	if (m == l) {
	    goto L90;
	}

/*        If remaining matrix is 2 by 2, use DLAE2 to compute its */
/*        eigenvalues. */

	if (m == l + 1) {
	    rte = sqrt(e[l]);
	    dlae2_(&d__[l], &rte, &d__[l + 1], &rt1, &rt2);
	    d__[l] = rt1;
	    d__[l + 1] = rt2;
	    e[l] = 0.;
	    l += 2;
	    if (l <= lend) {
		goto L50;
	    }
	    goto L150;
	}

	if (jtot == nmaxit) {
	    goto L150;
	}
	++jtot;

/*        Form shift. */

	rte = sqrt(e[l]);
	sigma = (d__[l + 1] - p) / (rte * 2.);
	r__ = dlapy2_(&sigma, &c_b32);
	sigma = p - rte / (sigma + d_sign(&r__, &sigma));

	c__ = 1.;
	s = 0.;
	gamma = d__[m] - sigma;
	p = gamma * gamma;

/*        Inner loop */

	i__1 = l;
	for (i__ = m - 1; i__ >= i__1; --i__) {
	    bb = e[i__];
	    r__ = p + bb;
	    if (i__ != m - 1) {
		e[i__ + 1] = s * r__;
	    }
	    oldc = c__;
	    c__ = p / r__;
	    s = bb / r__;
	    oldgam = gamma;
	    alpha = d__[i__];
	    gamma = c__ * (alpha - sigma) - s * oldgam;
	    d__[i__ + 1] = oldgam + (alpha - gamma);
	    if (c__ != 0.) {
		p = gamma * gamma / c__;
	    } else {
		p = oldc * bb;
	    }
/* L80: */
	}

	e[l] = s * p;
	d__[l] = sigma + gamma;
	goto L50;

/*        Eigenvalue found. */

L90:
	d__[l] = p;

	++l;
	if (l <= lend) {
	    goto L50;
	}
	goto L150;

    } else {

/*        QR Iteration */

/*        Look for small superdiagonal element. */

L100:
	i__1 = lend + 1;
	for (m = l; m >= i__1; --m) {
	    if ((d__2 = e[m - 1], abs(d__2)) <= eps2 * (d__1 = d__[m] * d__[m
		    - 1], abs(d__1))) {
		goto L120;
	    }
/* L110: */
	}
	m = lend;

L120:
	if (m > lend) {
	    e[m - 1] = 0.;
	}
	p = d__[l];
	if (m == l) {
	    goto L140;
	}

/*        If remaining matrix is 2 by 2, use DLAE2 to compute its */
/*        eigenvalues. */

	if (m == l - 1) {
	    rte = sqrt(e[l - 1]);
	    dlae2_(&d__[l], &rte, &d__[l - 1], &rt1, &rt2);
	    d__[l] = rt1;
	    d__[l - 1] = rt2;
	    e[l - 1] = 0.;
	    l += -2;
	    if (l >= lend) {
		goto L100;
	    }
	    goto L150;
	}

	if (jtot == nmaxit) {
	    goto L150;
	}
	++jtot;

/*        Form shift. */

	rte = sqrt(e[l - 1]);
	sigma = (d__[l - 1] - p) / (rte * 2.);
	r__ = dlapy2_(&sigma, &c_b32);
	sigma = p - rte / (sigma + d_sign(&r__, &sigma));

	c__ = 1.;
	s = 0.;
	gamma = d__[m] - sigma;
	p = gamma * gamma;

/*        Inner loop */

	i__1 = l - 1;
	for (i__ = m; i__ <= i__1; ++i__) {
	    bb = e[i__];
	    r__ = p + bb;
	    if (i__ != m) {
		e[i__ - 1] = s * r__;
	    }
	    oldc = c__;
	    c__ = p / r__;
	    s = bb / r__;
	    oldgam = gamma;
	    alpha = d__[i__ + 1];
	    gamma = c__ * (alpha - sigma) - s * oldgam;
	    d__[i__] = oldgam + (alpha - gamma);
	    if (c__ != 0.) {
		p = gamma * gamma / c__;
	    } else {
		p = oldc * bb;
	    }
/* L130: */
	}

	e[l - 1] = s * p;
	d__[l] = sigma + gamma;
	goto L100;

/*        Eigenvalue found. */

L140:
	d__[l] = p;

	--l;
	if (l >= lend) {
	    goto L100;
	}
	goto L150;

    }

/*     Undo scaling if necessary */

L150:
    if (iscale == 1) {
	i__1 = lendsv - lsv + 1;
	dlascl_("G", &c__0, &c__0, &ssfmax, &anorm, &i__1, &c__1, &d__[lsv],
		n, info);
    }
    if (iscale == 2) {
	i__1 = lendsv - lsv + 1;
	dlascl_("G", &c__0, &c__0, &ssfmin, &anorm, &i__1, &c__1, &d__[lsv],
		n, info);
    }

/*     Check for no convergence to an eigenvalue after a total */
/*     of N*MAXIT iterations. */

    if (jtot < nmaxit) {
	goto L10;
    }
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (e[i__] != 0.) {
	    ++(*info);
	}
/* L160: */
    }
    goto L180;

/*     Sort eigenvalues in increasing order. */

L170:
    dlasrt_("I", n, &d__[1], info);

L180:
    return 0;

/*     End of DSTERF */

} /* dsterf_ */

/* Subroutine */ int dstev_(const char *jobz, integer *n, double *d__,
	double *e, double *z__, integer *ldz, double *work,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer z_dim1, z_offset, i__1;
    double d__1;

    /* Local variables */
    double eps;
    integer imax;
    double rmin, rmax, tnrm;
    double sigma;
    bool wantz;
    integer iscale;
    double safmin;
    double bignum;
    double smlnum;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSTEV computes all eigenvalues and, optionally, eigenvectors of a */
/*  real symmetric tridiagonal matrix A. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  N       (input) INTEGER */
/*          The order of the matrix.  N >= 0. */

/*  D       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the n diagonal elements of the tridiagonal matrix */
/*          A. */
/*          On exit, if INFO = 0, the eigenvalues in ascending order. */

/*  E       (input/output) DOUBLE PRECISION array, dimension (N-1) */
/*          On entry, the (n-1) subdiagonal elements of the tridiagonal */
/*          matrix A, stored in elements 1 to N-1 of E. */
/*          On exit, the contents of E are destroyed. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, N) */
/*          If JOBZ = 'V', then if INFO = 0, Z contains the orthonormal */
/*          eigenvectors of the matrix A, with the i-th column of Z */
/*          holding the eigenvector associated with D(i). */
/*          If JOBZ = 'N', then Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (max(1,2*N-2)) */
/*          If JOBZ = 'N', WORK is not referenced. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, the algorithm failed to converge; i */
/*                off-diagonal elements of E did not converge to zero. */

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
    --e;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;

    /* Function Body */
    wantz = lsame_(jobz, "V");

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*ldz < 1 || wantz && *ldz < *n) {
	*info = -6;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSTEV ", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (wantz) {
	    z__[z_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
    rmax = sqrt(bignum);

/*     Scale matrix to allowable range, if necessary. */

    iscale = 0;
    tnrm = dlanst_("M", n, &d__[1], &e[1]);
    if (tnrm > 0. && tnrm < rmin) {
	iscale = 1;
	sigma = rmin / tnrm;
    } else if (tnrm > rmax) {
	iscale = 1;
	sigma = rmax / tnrm;
    }
    if (iscale == 1) {
	dscal_(n, &sigma, &d__[1], &c__1);
	i__1 = *n - 1;
	dscal_(&i__1, &sigma, &e[1], &c__1);
    }

/*     For eigenvalues only, call DSTERF.  For eigenvalues and */
/*     eigenvectors, call DSTEQR. */

    if (! wantz) {
	dsterf_(n, &d__[1], &e[1], info);
    } else {
	dsteqr_("I", n, &d__[1], &e[1], &z__[z_offset], ldz, &work[1], info);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

    if (iscale == 1) {
	if (*info == 0) {
	    imax = *n;
	} else {
	    imax = *info - 1;
	}
	d__1 = 1. / sigma;
	dscal_(&imax, &d__1, &d__[1], &c__1);
    }

    return 0;

/*     End of DSTEV */

} /* dstev_ */

/* Subroutine */ int dstevd_(const char *jobz, integer *n, double *d__,
	double *e, double *z__, integer *ldz, double *work,
	integer *lwork, integer *iwork, integer *liwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer z_dim1, z_offset, i__1;
    double d__1;

    /* Local variables */
    double eps, rmin, rmax, tnrm;
    double sigma;
    integer lwmin;
    bool wantz;
    integer iscale;
	double safmin;
    double bignum;
    integer liwmin;
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

/*  DSTEVD computes all eigenvalues and, optionally, eigenvectors of a */
/*  real symmetric tridiagonal matrix. If eigenvectors are desired, it */
/*  uses a divide and conquer algorithm. */

/*  The divide and conquer algorithm makes very mild assumptions about */
/*  floating point arithmetic. It will work on machines with a guard */
/*  digit in add/subtract, or on those binary machines without guard */
/*  digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or */
/*  Cray-2. It could conceivably fail on hexadecimal or decimal machines */
/*  without guard digits, but we know of none. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  N       (input) INTEGER */
/*          The order of the matrix.  N >= 0. */

/*  D       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the n diagonal elements of the tridiagonal matrix */
/*          A. */
/*          On exit, if INFO = 0, the eigenvalues in ascending order. */

/*  E       (input/output) DOUBLE PRECISION array, dimension (N-1) */
/*          On entry, the (n-1) subdiagonal elements of the tridiagonal */
/*          matrix A, stored in elements 1 to N-1 of E. */
/*          On exit, the contents of E are destroyed. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, N) */
/*          If JOBZ = 'V', then if INFO = 0, Z contains the orthonormal */
/*          eigenvectors of the matrix A, with the i-th column of Z */
/*          holding the eigenvector associated with D(i). */
/*          If JOBZ = 'N', then Z is not referenced. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, */
/*                                         dimension (LWORK) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          If JOBZ  = 'N' or N <= 1 then LWORK must be at least 1. */
/*          If JOBZ  = 'V' and N > 1 then LWORK must be at least */
/*                         ( 1 + 4*N + N**2 ). */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal sizes of the WORK and IWORK */
/*          arrays, returns these values as the first entries of the WORK */
/*          and IWORK arrays, and no error message related to LWORK or */
/*          LIWORK is issued by XERBLA. */

/*  IWORK   (workspace/output) INTEGER array, dimension (MAX(1,LIWORK)) */
/*          On exit, if INFO = 0, IWORK(1) returns the optimal LIWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK. */
/*          If JOBZ  = 'N' or N <= 1 then LIWORK must be at least 1. */
/*          If JOBZ  = 'V' and N > 1 then LIWORK must be at least 3+5*N. */

/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates the optimal sizes of the WORK and */
/*          IWORK arrays, returns these values as the first entries of */
/*          the WORK and IWORK arrays, and no error message related to */
/*          LWORK or LIWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, the algorithm failed to converge; i */
/*                off-diagonal elements of E did not converge to zero. */

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
    --e;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;
    --iwork;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    lquery = *lwork == -1 || *liwork == -1;

    *info = 0;
    liwmin = 1;
    lwmin = 1;
    if (*n > 1 && wantz) {
/* Computing 2nd power */
	i__1 = *n;
	lwmin = (*n << 2) + 1 + i__1 * i__1;
	liwmin = *n * 5 + 3;
    }

    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*ldz < 1 || wantz && *ldz < *n) {
	*info = -6;
    }

    if (*info == 0) {
	work[1] = (double) lwmin;
	iwork[1] = liwmin;

	if (*lwork < lwmin && ! lquery) {
	    *info = -8;
	} else if (*liwork < liwmin && ! lquery) {
	    *info = -10;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSTEVD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (wantz) {
	    z__[z_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
    rmax = sqrt(bignum);

/*     Scale matrix to allowable range, if necessary. */

    iscale = 0;
    tnrm = dlanst_("M", n, &d__[1], &e[1]);
    if (tnrm > 0. && tnrm < rmin) {
	iscale = 1;
	sigma = rmin / tnrm;
    } else if (tnrm > rmax) {
	iscale = 1;
	sigma = rmax / tnrm;
    }
    if (iscale == 1) {
	dscal_(n, &sigma, &d__[1], &c__1);
	i__1 = *n - 1;
	dscal_(&i__1, &sigma, &e[1], &c__1);
    }

/*     For eigenvalues only, call DSTERF.  For eigenvalues and */
/*     eigenvectors, call DSTEDC. */

    if (! wantz) {
	dsterf_(n, &d__[1], &e[1], info);
    } else {
	dstedc_("I", n, &d__[1], &e[1], &z__[z_offset], ldz, &work[1], lwork,
		&iwork[1], liwork, info);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

    if (iscale == 1) {
	d__1 = 1. / sigma;
	dscal_(n, &d__1, &d__[1], &c__1);
    }

    work[1] = (double) lwmin;
    iwork[1] = liwmin;

    return 0;

/*     End of DSTEVD */

} /* dstevd_ */

/* Subroutine */ int dstevr_(const char *jobz, const char *range, integer *n, double *
	d__, double *e, double *vl, double *vu, integer *il,
	integer *iu, double *abstol, integer *m, double *w,
	double *z__, integer *ldz, integer *isuppz, double *work,
	integer *lwork, integer *iwork, integer *liwork, integer *info)
{
	/* Table of constant values */
	static integer c__10 = 10;
	static integer c__1 = 1;
	static integer c__2 = 2;
	static integer c__3 = 3;
	static integer c__4 = 4;

    /* System generated locals */
    integer z_dim1, z_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j, jj;
    double eps, vll, vuu, tmp1;
    integer imax;
    double rmin, rmax;
    bool test;
    double tnrm;
    integer itmp1;
    double sigma;
    char order[1];
    integer lwmin;
    bool wantz;
    bool alleig, indeig;
    integer iscale, ieeeok, indibl, indifl;
    bool valeig;
    double safmin;
    double bignum;
    integer indisp;
    integer indiwo;
    integer liwmin;
    bool tryrac;
    integer nsplit;
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

/*  DSTEVR computes selected eigenvalues and, optionally, eigenvectors */
/*  of a real symmetric tridiagonal matrix T.  Eigenvalues and */
/*  eigenvectors can be selected by specifying either a range of values */
/*  or a range of indices for the desired eigenvalues. */

/*  Whenever possible, DSTEVR calls DSTEMR to compute the */
/*  eigenspectrum using Relatively Robust Representations.  DSTEMR */
/*  computes eigenvalues by the dqds algorithm, while orthogonal */
/*  eigenvectors are computed from various "good" L D L^T representations */
/*  (also known as Relatively Robust Representations). Gram-Schmidt */
/*  orthogonalization is avoided as far as possible. More specifically, */
/*  the various steps of the algorithm are as follows. For the i-th */
/*  unreduced block of T, */
/*     (a) Compute T - sigma_i = L_i D_i L_i^T, such that L_i D_i L_i^T */
/*          is a relatively robust representation, */
/*     (b) Compute the eigenvalues, lambda_j, of L_i D_i L_i^T to high */
/*         relative accuracy by the dqds algorithm, */
/*     (c) If there is a cluster of close eigenvalues, "choose" sigma_i */
/*         close to the cluster, and go to step (a), */
/*     (d) Given the approximate eigenvalue lambda_j of L_i D_i L_i^T, */
/*         compute the corresponding eigenvector by forming a */
/*         rank-revealing twisted factorization. */
/*  The desired accuracy of the output can be specified by the input */
/*  parameter ABSTOL. */

/*  For more details, see "A new O(n^2) algorithm for the symmetric */
/*  tridiagonal eigenvalue/eigenvector problem", by Inderjit Dhillon, */
/*  Computer Science Division Technical Report No. UCB//CSD-97-971, */
/*  UC Berkeley, May 1997. */


/*  Note 1 : DSTEVR calls DSTEMR when the full spectrum is requested */
/*  on machines which conform to the ieee-754 floating point standard. */
/*  DSTEVR calls DSTEBZ and DSTEIN on non-ieee machines and */
/*  when partial spectrum requests are made. */

/*  Normal execution of DSTEMR may create NaNs and infinities and */
/*  hence may abort due to a floating point exception in environments */
/*  which do not handle NaNs and infinities in the ieee standard default */
/*  manner. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  RANGE   (input) CHARACTER*1 */
/*          = 'A': all eigenvalues will be found. */
/*          = 'V': all eigenvalues in the half-open interval (VL,VU] */
/*                 will be found. */
/*          = 'I': the IL-th through IU-th eigenvalues will be found. */
/* ********* For RANGE = 'V' or 'I' and IU - IL < N - 1, DSTEBZ and */
/* ********* DSTEIN are called */

/*  N       (input) INTEGER */
/*          The order of the matrix.  N >= 0. */

/*  D       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the n diagonal elements of the tridiagonal matrix */
/*          A. */
/*          On exit, D may be multiplied by a constant factor chosen */
/*          to avoid over/underflow in computing the eigenvalues. */

/*  E       (input/output) DOUBLE PRECISION array, dimension (max(1,N-1)) */
/*          On entry, the (n-1) subdiagonal elements of the tridiagonal */
/*          matrix A in elements 1 to N-1 of E. */
/*          On exit, E may be multiplied by a constant factor chosen */
/*          to avoid over/underflow in computing the eigenvalues. */

/*  VL      (input) DOUBLE PRECISION */
/*  VU      (input) DOUBLE PRECISION */
/*          If RANGE='V', the lower and upper bounds of the interval to */
/*          be searched for eigenvalues. VL < VU. */
/*          Not referenced if RANGE = 'A' or 'I'. */

/*  IL      (input) INTEGER */
/*  IU      (input) INTEGER */
/*          If RANGE='I', the indices (in ascending order) of the */
/*          smallest and largest eigenvalues to be returned. */
/*          1 <= IL <= IU <= N, if N > 0; IL = 1 and IU = 0 if N = 0. */
/*          Not referenced if RANGE = 'A' or 'V'. */

/*  ABSTOL  (input) DOUBLE PRECISION */
/*          The absolute error tolerance for the eigenvalues. */
/*          An approximate eigenvalue is accepted as converged */
/*          when it is determined to lie in an interval [a,b] */
/*          of width less than or equal to */

/*                  ABSTOL + EPS *   max( |a|,|b| ) , */

/*          where EPS is the machine precision.  If ABSTOL is less than */
/*          or equal to zero, then  EPS*|T|  will be used in its place, */
/*          where |T| is the 1-norm of the tridiagonal matrix obtained */
/*          by reducing A to tridiagonal form. */

/*          See "Computing Small Singular Values of Bidiagonal Matrices */
/*          with Guaranteed High Relative Accuracy," by Demmel and */
/*          Kahan, LAPACK Working Note #3. */

/*          If high relative accuracy is important, set ABSTOL to */
/*          DLAMCH( 'Safe minimum' ).  Doing so will guarantee that */
/*          eigenvalues are computed to high relative accuracy when */
/*          possible in future releases.  The current code does not */
/*          make any guarantees about high relative accuracy, but */
/*          future releases will. See J. Barlow and J. Demmel, */
/*          "Computing Accurate Eigensystems of Scaled Diagonally */
/*          Dominant Matrices", LAPACK Working Note #7, for a discussion */
/*          of which matrices define their eigenvalues to high relative */
/*          accuracy. */

/*  M       (output) INTEGER */
/*          The total number of eigenvalues found.  0 <= M <= N. */
/*          If RANGE = 'A', M = N, and if RANGE = 'I', M = IU-IL+1. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          The first M elements contain the selected eigenvalues in */
/*          ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, max(1,M) ) */
/*          If JOBZ = 'V', then if INFO = 0, the first M columns of Z */
/*          contain the orthonormal eigenvectors of the matrix A */
/*          corresponding to the selected eigenvalues, with the i-th */
/*          column of Z holding the eigenvector associated with W(i). */
/*          Note: the user must ensure that at least max(1,M) columns are */
/*          supplied in the array Z; if RANGE = 'V', the exact value of M */
/*          is not known in advance and an upper bound must be used. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  ISUPPZ  (output) INTEGER array, dimension ( 2*max(1,M) ) */
/*          The support of the eigenvectors in Z, i.e., the indices */
/*          indicating the nonzero elements in Z. The i-th eigenvector */
/*          is nonzero only in elements ISUPPZ( 2*i-1 ) through */
/*          ISUPPZ( 2*i ). */
/* ********* Implemented only for RANGE = 'A' or 'I' and IU - IL = N - 1 */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal (and */
/*          minimal) LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= max(1,20*N). */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal sizes of the WORK and IWORK */
/*          arrays, returns these values as the first entries of the WORK */
/*          and IWORK arrays, and no error message related to LWORK or */
/*          LIWORK is issued by XERBLA. */

/*  IWORK   (workspace/output) INTEGER array, dimension (MAX(1,LIWORK)) */
/*          On exit, if INFO = 0, IWORK(1) returns the optimal (and */
/*          minimal) LIWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK.  LIWORK >= max(1,10*N). */

/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates the optimal sizes of the WORK and */
/*          IWORK arrays, returns these values as the first entries of */
/*          the WORK and IWORK arrays, and no error message related to */
/*          LWORK or LIWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  Internal error */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Inderjit Dhillon, IBM Almaden, USA */
/*     Osni Marques, LBNL/NERSC, USA */
/*     Ken Stanley, Computer Science Division, University of */
/*       California at Berkeley, USA */

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
    --e;
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --isuppz;
    --work;
    --iwork;

    /* Function Body */
    ieeeok = ilaenv_(&c__10, "DSTEVR", "N", &c__1, &c__2, &c__3, &c__4);

    wantz = lsame_(jobz, "V");
    alleig = lsame_(range, "A");
    valeig = lsame_(range, "V");
    indeig = lsame_(range, "I");

    lquery = *lwork == -1 || *liwork == -1;
/* Computing MAX */
    i__1 = 1, i__2 = *n * 20;
    lwmin = std::max(i__1,i__2);
/* Computing MAX */
    i__1 = 1, i__2 = *n * 10;
    liwmin = std::max(i__1,i__2);


    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (alleig || valeig || indeig)) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else {
	if (valeig) {
	    if (*n > 0 && *vu <= *vl) {
		*info = -7;
	    }
	} else if (indeig) {
	    if (*il < 1 || *il > std::max(1_integer,*n)) {
		*info = -8;
	    } else if (*iu < std::min(*n,*il) || *iu > *n) {
		*info = -9;
	    }
	}
    }
    if (*info == 0) {
	if (*ldz < 1 || wantz && *ldz < *n) {
	    *info = -14;
	}
    }

    if (*info == 0) {
	work[1] = (double) lwmin;
	iwork[1] = liwmin;

	if (*lwork < lwmin && ! lquery) {
	    *info = -17;
	} else if (*liwork < liwmin && ! lquery) {
	    *info = -19;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSTEVR", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    *m = 0;
    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (alleig || indeig) {
	    *m = 1;
	    w[1] = d__[1];
	} else {
	    if (*vl < d__[1] && *vu >= d__[1]) {
		*m = 1;
		w[1] = d__[1];
	    }
	}
	if (wantz) {
	    z__[z_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
/* Computing MIN */
    d__1 = sqrt(bignum), d__2 = 1. / sqrt(sqrt(safmin));
    rmax = std::min(d__1,d__2);


/*     Scale matrix to allowable range, if necessary. */

    iscale = 0;
    vll = *vl;
    vuu = *vu;

    tnrm = dlanst_("M", n, &d__[1], &e[1]);
    if (tnrm > 0. && tnrm < rmin) {
	iscale = 1;
	sigma = rmin / tnrm;
    } else if (tnrm > rmax) {
	iscale = 1;
	sigma = rmax / tnrm;
    }
    if (iscale == 1) {
	dscal_(n, &sigma, &d__[1], &c__1);
	i__1 = *n - 1;
	dscal_(&i__1, &sigma, &e[1], &c__1);
	if (valeig) {
	    vll = *vl * sigma;
	    vuu = *vu * sigma;
	}
    }
/*     Initialize indices into workspaces.  Note: These indices are used only */
/*     if DSTERF or DSTEMR fail. */
/*     IWORK(INDIBL:INDIBL+M-1) corresponds to IBLOCK in DSTEBZ and */
/*     stores the block indices of each of the M<=N eigenvalues. */
    indibl = 1;
/*     IWORK(INDISP:INDISP+NSPLIT-1) corresponds to ISPLIT in DSTEBZ and */
/*     stores the starting and finishing indices of each block. */
    indisp = indibl + *n;
/*     IWORK(INDIFL:INDIFL+N-1) stores the indices of eigenvectors */
/*     that corresponding to eigenvectors that fail to converge in */
/*     DSTEIN.  This information is discarded; if any fail, the driver */
/*     returns INFO > 0. */
    indifl = indisp + *n;
/*     INDIWO is the offset of the remaining integer workspace. */
    indiwo = indisp + *n;

/*     If all eigenvalues are desired, then */
/*     call DSTERF or DSTEMR.  If this fails for some eigenvalue, then */
/*     try DSTEBZ. */


    test = false;
    if (indeig) {
	if (*il == 1 && *iu == *n) {
	    test = true;
	}
    }
    if ((alleig || test) && ieeeok == 1) {
	i__1 = *n - 1;
	dcopy_(&i__1, &e[1], &c__1, &work[1], &c__1);
	if (! wantz) {
	    dcopy_(n, &d__[1], &c__1, &w[1], &c__1);
	    dsterf_(n, &w[1], &work[1], info);
	} else {
	    dcopy_(n, &d__[1], &c__1, &work[*n + 1], &c__1);
	    if (*abstol <= *n * 2. * eps) {
		tryrac = true;
	    } else {
		tryrac = false;
	    }
	    i__1 = *lwork - (*n << 1);
	    dstemr_(jobz, "A", n, &work[*n + 1], &work[1], vl, vu, il, iu, m,
		    &w[1], &z__[z_offset], ldz, n, &isuppz[1], &tryrac, &work[
		    (*n << 1) + 1], &i__1, &iwork[1], liwork, info);

	}
	if (*info == 0) {
	    *m = *n;
	    goto L10;
	}
	*info = 0;
    }

/*     Otherwise, call DSTEBZ and, if eigenvectors are desired, DSTEIN. */

    if (wantz) {
	*(unsigned char *)order = 'B';
    } else {
	*(unsigned char *)order = 'E';
    }
    dstebz_(range, order, n, &vll, &vuu, il, iu, abstol, &d__[1], &e[1], m, &
	    nsplit, &w[1], &iwork[indibl], &iwork[indisp], &work[1], &iwork[
	    indiwo], info);

    if (wantz) {
	dstein_(n, &d__[1], &e[1], m, &w[1], &iwork[indibl], &iwork[indisp], &
		z__[z_offset], ldz, &work[1], &iwork[indiwo], &iwork[indifl],
		info);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

L10:
    if (iscale == 1) {
	if (*info == 0) {
	    imax = *m;
	} else {
	    imax = *info - 1;
	}
	d__1 = 1. / sigma;
	dscal_(&imax, &d__1, &w[1], &c__1);
    }

/*     If eigenvalues are not in order, then sort them, along with */
/*     eigenvectors. */

    if (wantz) {
	i__1 = *m - 1;
	for (j = 1; j <= i__1; ++j) {
	    i__ = 0;
	    tmp1 = w[j];
	    i__2 = *m;
	    for (jj = j + 1; jj <= i__2; ++jj) {
		if (w[jj] < tmp1) {
		    i__ = jj;
		    tmp1 = w[jj];
		}
/* L20: */
	    }

	    if (i__ != 0) {
		itmp1 = iwork[i__];
		w[i__] = w[j];
		iwork[i__] = iwork[j];
		w[j] = tmp1;
		iwork[j] = itmp1;
		dswap_(n, &z__[i__ * z_dim1 + 1], &c__1, &z__[j * z_dim1 + 1],
			 &c__1);
	    }
/* L30: */
	}
    }

/*      Causes problems with tests 19 & 20: */
/*      IF (wantz .and. INDEIG ) Z( 1,1) = Z(1,1) / 1.002 + .002 */


    work[1] = (double) lwmin;
    iwork[1] = liwmin;
    return 0;

/*     End of DSTEVR */

} /* dstevr_ */

/* Subroutine */ int dstevx_(const char *jobz, const char *range, integer *n, double *
	d__, double *e, double *vl, double *vu, integer *il,
	integer *iu, double *abstol, integer *m, double *w,
	double *z__, integer *ldz, double *work, integer *iwork,
	integer *ifail, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer z_dim1, z_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j, jj;
    double eps, vll, vuu, tmp1;
    integer imax;
    double rmin, rmax;
    bool test;
    double tnrm;
    integer itmp1;
    double sigma;
    char order[1];
    bool wantz;
    bool alleig, indeig;
    integer iscale, indibl;
    bool valeig;
    double safmin;
    double bignum;
    integer indisp;
    integer indiwo;
    integer indwrk;
    integer nsplit;
    double smlnum;


/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSTEVX computes selected eigenvalues and, optionally, eigenvectors */
/*  of a real symmetric tridiagonal matrix A.  Eigenvalues and */
/*  eigenvectors can be selected by specifying either a range of values */
/*  or a range of indices for the desired eigenvalues. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  RANGE   (input) CHARACTER*1 */
/*          = 'A': all eigenvalues will be found. */
/*          = 'V': all eigenvalues in the half-open interval (VL,VU] */
/*                 will be found. */
/*          = 'I': the IL-th through IU-th eigenvalues will be found. */

/*  N       (input) INTEGER */
/*          The order of the matrix.  N >= 0. */

/*  D       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the n diagonal elements of the tridiagonal matrix */
/*          A. */
/*          On exit, D may be multiplied by a constant factor chosen */
/*          to avoid over/underflow in computing the eigenvalues. */

/*  E       (input/output) DOUBLE PRECISION array, dimension (max(1,N-1)) */
/*          On entry, the (n-1) subdiagonal elements of the tridiagonal */
/*          matrix A in elements 1 to N-1 of E. */
/*          On exit, E may be multiplied by a constant factor chosen */
/*          to avoid over/underflow in computing the eigenvalues. */

/*  VL      (input) DOUBLE PRECISION */
/*  VU      (input) DOUBLE PRECISION */
/*          If RANGE='V', the lower and upper bounds of the interval to */
/*          be searched for eigenvalues. VL < VU. */
/*          Not referenced if RANGE = 'A' or 'I'. */

/*  IL      (input) INTEGER */
/*  IU      (input) INTEGER */
/*          If RANGE='I', the indices (in ascending order) of the */
/*          smallest and largest eigenvalues to be returned. */
/*          1 <= IL <= IU <= N, if N > 0; IL = 1 and IU = 0 if N = 0. */
/*          Not referenced if RANGE = 'A' or 'V'. */

/*  ABSTOL  (input) DOUBLE PRECISION */
/*          The absolute error tolerance for the eigenvalues. */
/*          An approximate eigenvalue is accepted as converged */
/*          when it is determined to lie in an interval [a,b] */
/*          of width less than or equal to */

/*                  ABSTOL + EPS *   max( |a|,|b| ) , */

/*          where EPS is the machine precision.  If ABSTOL is less */
/*          than or equal to zero, then  EPS*|T|  will be used in */
/*          its place, where |T| is the 1-norm of the tridiagonal */
/*          matrix. */

/*          Eigenvalues will be computed most accurately when ABSTOL is */
/*          set to twice the underflow threshold 2*DLAMCH('S'), not zero. */
/*          If this routine returns with INFO>0, indicating that some */
/*          eigenvectors did not converge, try setting ABSTOL to */
/*          2*DLAMCH('S'). */

/*          See "Computing Small Singular Values of Bidiagonal Matrices */
/*          with Guaranteed High Relative Accuracy," by Demmel and */
/*          Kahan, LAPACK Working Note #3. */

/*  M       (output) INTEGER */
/*          The total number of eigenvalues found.  0 <= M <= N. */
/*          If RANGE = 'A', M = N, and if RANGE = 'I', M = IU-IL+1. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          The first M elements contain the selected eigenvalues in */
/*          ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, max(1,M) ) */
/*          If JOBZ = 'V', then if INFO = 0, the first M columns of Z */
/*          contain the orthonormal eigenvectors of the matrix A */
/*          corresponding to the selected eigenvalues, with the i-th */
/*          column of Z holding the eigenvector associated with W(i). */
/*          If an eigenvector fails to converge (INFO > 0), then that */
/*          column of Z contains the latest approximation to the */
/*          eigenvector, and the index of the eigenvector is returned */
/*          in IFAIL.  If JOBZ = 'N', then Z is not referenced. */
/*          Note: the user must ensure that at least max(1,M) columns are */
/*          supplied in the array Z; if RANGE = 'V', the exact value of M */
/*          is not known in advance and an upper bound must be used. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (5*N) */

/*  IWORK   (workspace) INTEGER array, dimension (5*N) */

/*  IFAIL   (output) INTEGER array, dimension (N) */
/*          If JOBZ = 'V', then if INFO = 0, the first M elements of */
/*          IFAIL are zero.  If INFO > 0, then IFAIL contains the */
/*          indices of the eigenvectors that failed to converge. */
/*          If JOBZ = 'N', then IFAIL is not referenced. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, then i eigenvectors failed to converge. */
/*                Their indices are stored in array IFAIL. */

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
    --e;
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;
    --iwork;
    --ifail;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    alleig = lsame_(range, "A");
    valeig = lsame_(range, "V");
    indeig = lsame_(range, "I");

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (alleig || valeig || indeig)) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else {
	if (valeig) {
	    if (*n > 0 && *vu <= *vl) {
		*info = -7;
	    }
	} else if (indeig) {
	    if (*il < 1 || *il > std::max(1_integer,*n)) {
		*info = -8;
	    } else if (*iu < std::min(*n,*il) || *iu > *n) {
		*info = -9;
	    }
	}
    }
    if (*info == 0) {
	if (*ldz < 1 || wantz && *ldz < *n) {
	    *info = -14;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSTEVX", &i__1);
	return 0;
    }

/*     Quick return if possible */

    *m = 0;
    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (alleig || indeig) {
	    *m = 1;
	    w[1] = d__[1];
	} else {
	    if (*vl < d__[1] && *vu >= d__[1]) {
		*m = 1;
		w[1] = d__[1];
	    }
	}
	if (wantz) {
	    z__[z_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
/* Computing MIN */
    d__1 = sqrt(bignum), d__2 = 1. / sqrt(sqrt(safmin));
    rmax = std::min(d__1,d__2);

/*     Scale matrix to allowable range, if necessary. */

    iscale = 0;
    if (valeig) {
	vll = *vl;
	vuu = *vu;
    } else {
	vll = 0.;
	vuu = 0.;
    }
    tnrm = dlanst_("M", n, &d__[1], &e[1]);
    if (tnrm > 0. && tnrm < rmin) {
	iscale = 1;
	sigma = rmin / tnrm;
    } else if (tnrm > rmax) {
	iscale = 1;
	sigma = rmax / tnrm;
    }
    if (iscale == 1) {
	dscal_(n, &sigma, &d__[1], &c__1);
	i__1 = *n - 1;
	dscal_(&i__1, &sigma, &e[1], &c__1);
	if (valeig) {
	    vll = *vl * sigma;
	    vuu = *vu * sigma;
	}
    }

/*     If all eigenvalues are desired and ABSTOL is less than zero, then */
/*     call DSTERF or SSTEQR.  If this fails for some eigenvalue, then */
/*     try DSTEBZ. */

    test = false;
    if (indeig) {
	if (*il == 1 && *iu == *n) {
	    test = true;
	}
    }
    if ((alleig || test) && *abstol <= 0.) {
	dcopy_(n, &d__[1], &c__1, &w[1], &c__1);
	i__1 = *n - 1;
	dcopy_(&i__1, &e[1], &c__1, &work[1], &c__1);
	indwrk = *n + 1;
	if (! wantz) {
	    dsterf_(n, &w[1], &work[1], info);
	} else {
	    dsteqr_("I", n, &w[1], &work[1], &z__[z_offset], ldz, &work[
		    indwrk], info);
	    if (*info == 0) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    ifail[i__] = 0;
/* L10: */
		}
	    }
	}
	if (*info == 0) {
	    *m = *n;
	    goto L20;
	}
	*info = 0;
    }

/*     Otherwise, call DSTEBZ and, if eigenvectors are desired, SSTEIN. */

    if (wantz) {
	*(unsigned char *)order = 'B';
    } else {
	*(unsigned char *)order = 'E';
    }
    indwrk = 1;
    indibl = 1;
    indisp = indibl + *n;
    indiwo = indisp + *n;
    dstebz_(range, order, n, &vll, &vuu, il, iu, abstol, &d__[1], &e[1], m, &
	    nsplit, &w[1], &iwork[indibl], &iwork[indisp], &work[indwrk], &
	    iwork[indiwo], info);

    if (wantz) {
	dstein_(n, &d__[1], &e[1], m, &w[1], &iwork[indibl], &iwork[indisp], &
		z__[z_offset], ldz, &work[indwrk], &iwork[indiwo], &ifail[1],
		info);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

L20:
    if (iscale == 1) {
	if (*info == 0) {
	    imax = *m;
	} else {
	    imax = *info - 1;
	}
	d__1 = 1. / sigma;
	dscal_(&imax, &d__1, &w[1], &c__1);
    }

/*     If eigenvalues are not in order, then sort them, along with */
/*     eigenvectors. */

    if (wantz) {
	i__1 = *m - 1;
	for (j = 1; j <= i__1; ++j) {
	    i__ = 0;
	    tmp1 = w[j];
	    i__2 = *m;
	    for (jj = j + 1; jj <= i__2; ++jj) {
		if (w[jj] < tmp1) {
		    i__ = jj;
		    tmp1 = w[jj];
		}
/* L30: */
	    }

	    if (i__ != 0) {
		itmp1 = iwork[indibl + i__ - 1];
		w[i__] = w[j];
		iwork[indibl + i__ - 1] = iwork[indibl + j - 1];
		w[j] = tmp1;
		iwork[indibl + j - 1] = itmp1;
		dswap_(n, &z__[i__ * z_dim1 + 1], &c__1, &z__[j * z_dim1 + 1],
			 &c__1);
		if (*info != 0) {
		    itmp1 = ifail[i__];
		    ifail[i__] = ifail[j];
		    ifail[j] = itmp1;
		}
	    }
/* L40: */
	}
    }

    return 0;

/*     End of DSTEVX */

} /* dstevx_ */

/* Subroutine */ int dsycon_(const char *uplo, integer *n, double *a, integer *
	lda, integer *ipiv, double *anorm, double *rcond, double *
	work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1;

    /* Local variables */
    integer i__, kase;
    integer isave[3];
    bool upper;
    double ainvnm;

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

/*  DSYCON estimates the reciprocal of the condition number (in the */
/*  1-norm) of a real symmetric matrix A using the factorization */
/*  A = U*D*U**T or A = L*D*L**T computed by DSYTRF. */

/*  An estimate is obtained for norm(inv(A)), and the reciprocal of the */
/*  condition number is computed as RCOND = 1 / (ANORM * norm(inv(A))). */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the details of the factorization are stored */
/*          as an upper or lower triangular matrix. */
/*          = 'U':  Upper triangular, form is A = U*D*U**T; */
/*          = 'L':  Lower triangular, form is A = L*D*L**T. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The block diagonal matrix D and the multipliers used to */
/*          obtain the factor U or L as computed by DSYTRF. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          Details of the interchanges and the block structure of D */
/*          as determined by DSYTRF. */

/*  ANORM   (input) DOUBLE PRECISION */
/*          The 1-norm of the original matrix A. */

/*  RCOND   (output) DOUBLE PRECISION */
/*          The reciprocal of the condition number of the matrix A, */
/*          computed as RCOND = 1/(ANORM * AINVNM), where AINVNM is an */
/*          estimate of the 1-norm of inv(A) computed in this routine. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (2*N) */

/*  IWORK    (workspace) INTEGER array, dimension (N) */

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
    --ipiv;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -4;
    } else if (*anorm < 0.) {
	*info = -6;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYCON", &i__1);
	return 0;
    }

/*     Quick return if possible */

    *rcond = 0.;
    if (*n == 0) {
	*rcond = 1.;
	return 0;
    } else if (*anorm <= 0.) {
	return 0;
    }

/*     Check that the diagonal matrix D is nonsingular. */

    if (upper) {

/*        Upper triangular storage: examine D from bottom to top */

	for (i__ = *n; i__ >= 1; --i__) {
	    if (ipiv[i__] > 0 && a[i__ + i__ * a_dim1] == 0.) {
		return 0;
	    }
/* L10: */
	}
    } else {

/*        Lower triangular storage: examine D from top to bottom. */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (ipiv[i__] > 0 && a[i__ + i__ * a_dim1] == 0.) {
		return 0;
	    }
/* L20: */
	}
    }

/*     Estimate the 1-norm of the inverse. */

    kase = 0;
L30:
    dlacn2_(n, &work[*n + 1], &work[1], &iwork[1], &ainvnm, &kase, isave);
    if (kase != 0) {

/*        Multiply by inv(L*D*L') or inv(U*D*U'). */

	dsytrs_(uplo, n, &c__1, &a[a_offset], lda, &ipiv[1], &work[1], n,
		info);
	goto L30;
    }

/*     Compute the estimate of the reciprocal condition number. */

    if (ainvnm != 0.) {
	*rcond = 1. / ainvnm / *anorm;
    }

    return 0;

/*     End of DSYCON */

} /* dsycon_ */

int dsyequb_(const char *uplo, integer *n, double *a, integer *lda, double *s, double *scond,
	double *amax, double *work, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    double d__1, d__2, d__3;

    /* Local variables */
    double d__;
    integer i__, j;
    double t, u, c0, c1, c2, si;
    bool up;
    double avg, std, tol, base;
    integer iter;
    double smin, smax, scale, sumsq, bignum, smlnum;


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

/*  DSYEQUB computes row and column scalings intended to equilibrate a */
/*  symmetric matrix A and reduce its condition number */
/*  (with respect to the two-norm).  S contains the scale factors, */
/*  S(i) = 1/sqrt(A(i,i)), chosen so that the scaled matrix B with */
/*  elements B(i,j) = S(i)*A(i,j)*S(j) has ones on the diagonal.  This */
/*  choice of S puts the condition number of B within a factor N of the */
/*  smallest possible condition number over all possible diagonal */
/*  scalings. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The N-by-N symmetric matrix whose scaling */
/*          factors are to be computed.  Only the diagonal elements of A */
/*          are referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  S       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, S contains the scale factors for A. */

/*  SCOND   (output) DOUBLE PRECISION */
/*          If INFO = 0, S contains the ratio of the smallest S(i) to */
/*          the largest S(i).  If SCOND >= 0.1 and AMAX is neither too */
/*          large nor too small, it is not worth scaling by S. */

/*  AMAX    (output) DOUBLE PRECISION */
/*          Absolute value of largest matrix element.  If AMAX is very */
/*          close to overflow or very close to underflow, the matrix */
/*          should be scaled. */
/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, the i-th diagonal element is nonpositive. */

/*  Further Details */
/*  ======= ======= */

/*  Reference: Livne, O.E. and Golub, G.H., "Scaling by Binormalization", */
/*  Numerical Algorithms, vol. 35, no. 1, pp. 97-120, January 2004. */
/*  DOI 10.1023/B:NUMA.0000016606.32820.69 */
/*  Tech report version: http://ruready.utah.edu/archive/papers/bin.pdf */

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

/*     Test input parameters. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --s;
    --work;

    /* Function Body */
    *info = 0;
    if (! (lsame_(uplo, "U") || lsame_(uplo, "L"))) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -4;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYEQUB", &i__1);
	return 0;
    }
    up = lsame_(uplo, "U");
    *amax = 0.;

/*     Quick return if possible. */

    if (*n == 0) {
	*scond = 1.;
	return 0;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	s[i__] = 0.;
    }
    *amax = 0.;
    if (up) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j - 1;
	    for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
		d__2 = s[i__], d__3 = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		s[i__] = std::max(d__2,d__3);
/* Computing MAX */
		d__2 = s[j], d__3 = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		s[j] = std::max(d__2,d__3);
/* Computing MAX */
		d__2 = *amax, d__3 = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		*amax = std::max(d__2,d__3);
	    }
/* Computing MAX */
	    d__2 = s[j], d__3 = (d__1 = a[j + j * a_dim1], abs(d__1));
	    s[j] = std::max(d__2,d__3);
/* Computing MAX */
	    d__2 = *amax, d__3 = (d__1 = a[j + j * a_dim1], abs(d__1));
	    *amax = std::max(d__2,d__3);
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	    d__2 = s[j], d__3 = (d__1 = a[j + j * a_dim1], abs(d__1));
	    s[j] = std::max(d__2,d__3);
/* Computing MAX */
	    d__2 = *amax, d__3 = (d__1 = a[j + j * a_dim1], abs(d__1));
	    *amax = std::max(d__2,d__3);
	    i__2 = *n;
	    for (i__ = j + 1; i__ <= i__2; ++i__) {
/* Computing MAX */
		d__2 = s[i__], d__3 = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		s[i__] = std::max(d__2,d__3);
/* Computing MAX */
		d__2 = s[j], d__3 = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		s[j] = std::max(d__2,d__3);
/* Computing MAX */
		d__2 = *amax, d__3 = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		*amax = std::max(d__2,d__3);
	    }
	}
    }
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	s[j] = 1. / s[j];
    }
    tol = 1. / sqrt(*n * 2.);
    for (iter = 1; iter <= 100; ++iter) {
	scale = 0.;
	sumsq = 0.;
/*       BETA = |A|S */
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    work[i__] = 0.;
	}
	if (up) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    t = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		    work[i__] += (d__1 = a[i__ + j * a_dim1], abs(d__1)) * s[
			    j];
		    work[j] += (d__1 = a[i__ + j * a_dim1], abs(d__1)) * s[
			    i__];
		}
		work[j] += (d__1 = a[j + j * a_dim1], abs(d__1)) * s[j];
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		work[j] += (d__1 = a[j + j * a_dim1], abs(d__1)) * s[j];
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    t = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		    work[i__] += (d__1 = a[i__ + j * a_dim1], abs(d__1)) * s[
			    j];
		    work[j] += (d__1 = a[i__ + j * a_dim1], abs(d__1)) * s[
			    i__];
		}
	    }
	}
/*       avg = s^T beta / n */
	avg = 0.;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    avg += s[i__] * work[i__];
	}
	avg /= *n;
	std = 0.;
	i__1 = *n * 3;
	for (i__ = (*n << 1) + 1; i__ <= i__1; ++i__) {
	    work[i__] = s[i__ - (*n << 1)] * work[i__ - (*n << 1)] - avg;
	}
	dlassq_(n, &work[(*n << 1) + 1], &c__1, &scale, &sumsq);
	std = scale * sqrt(sumsq / *n);
	if (std < tol * avg) {
	    goto L999;
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    t = (d__1 = a[i__ + i__ * a_dim1], abs(d__1));
	    si = s[i__];
	    c2 = (*n - 1) * t;
	    c1 = (*n - 2) * (work[i__] - t * si);
	    c0 = -(t * si) * si + work[i__] * 2 * si - *n * avg;
	    d__ = c1 * c1 - c0 * 4 * c2;
	    if (d__ <= 0.) {
		*info = -1;
		return 0;
	    }
	    si = c0 * -2 / (c1 + sqrt(d__));
	    d__ = si - s[i__];
	    u = 0.;
	    if (up) {
		i__2 = i__;
		for (j = 1; j <= i__2; ++j) {
		    t = (d__1 = a[j + i__ * a_dim1], abs(d__1));
		    u += s[j] * t;
		    work[j] += d__ * t;
		}
		i__2 = *n;
		for (j = i__ + 1; j <= i__2; ++j) {
		    t = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		    u += s[j] * t;
		    work[j] += d__ * t;
		}
	    } else {
		i__2 = i__;
		for (j = 1; j <= i__2; ++j) {
		    t = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		    u += s[j] * t;
		    work[j] += d__ * t;
		}
		i__2 = *n;
		for (j = i__ + 1; j <= i__2; ++j) {
		    t = (d__1 = a[j + i__ * a_dim1], abs(d__1));
		    u += s[j] * t;
		    work[j] += d__ * t;
		}
	    }
	    avg += (u + work[i__]) * d__ / *n;
	    s[i__] = si;
	}
    }
L999:
    smlnum = dlamch_("SAFEMIN");
    bignum = 1. / smlnum;
    smin = bignum;
    smax = 0.;
    t = 1. / sqrt(avg);
    base = dlamch_("B");
    u = 1. / log(base);
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = (integer) (u * log(s[i__] * t));
	s[i__] = pow_di(&base, &i__2);
/* Computing MIN */
	d__1 = smin, d__2 = s[i__];
	smin = std::min(d__1,d__2);
/* Computing MAX */
	d__1 = smax, d__2 = s[i__];
	smax = std::max(d__1,d__2);
    }
    *scond = std::max(smin,smlnum) / std::min(smax,bignum);

    return 0;
} /* dsyequb_ */

/* Subroutine */ int dsyev_(const char *jobz, const char *uplo, integer *n, double *a,
	integer *lda, double *w, double *work, integer *lwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__0 = 0;
	static double c_b17 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    double d__1;

    /* Local variables */
    integer nb;
    double eps;
    integer inde;
    double anrm;
    integer imax;
    double rmin, rmax;
    double sigma;
    integer iinfo;
    bool lower, wantz;
    integer iscale;
    double safmin;
    double bignum;
    integer indtau;
    integer indwrk;
    integer llwork;
    double smlnum;
    integer lwkopt;
    bool lquery;


/*  -- LAPACK driver routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSYEV computes all eigenvalues and, optionally, eigenvectors of a */
/*  real symmetric matrix A. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the */
/*          leading N-by-N upper triangular part of A contains the */
/*          upper triangular part of the matrix A.  If UPLO = 'L', */
/*          the leading N-by-N lower triangular part of A contains */
/*          the lower triangular part of the matrix A. */
/*          On exit, if JOBZ = 'V', then if INFO = 0, A contains the */
/*          orthonormal eigenvectors of the matrix A. */
/*          If JOBZ = 'N', then on exit the lower triangle (if UPLO='L') */
/*          or the upper triangle (if UPLO='U') of A, including the */
/*          diagonal, is destroyed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, the eigenvalues in ascending order. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= max(1,3*N-1). */
/*          For optimal efficiency, LWORK >= (NB+2)*N, */
/*          where NB is the blocksize for DSYTRD returned by ILAENV. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, the algorithm failed to converge; i */
/*                off-diagonal elements of an intermediate tridiagonal */
/*                form did not converge to zero. */

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
    --w;
    --work;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    lower = lsame_(uplo, "L");
    lquery = *lwork == -1;

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (lower || lsame_(uplo, "U"))) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    }

    if (*info == 0) {
	nb = ilaenv_(&c__1, "DSYTRD", uplo, n, &c_n1, &c_n1, &c_n1);
/* Computing MAX */
	i__1 = 1, i__2 = (nb + 2) * *n;
	lwkopt = std::max(i__1,i__2);
	work[1] = (double) lwkopt;

/* Computing MAX */
	i__1 = 1, i__2 = *n * 3 - 1;
	if (*lwork < std::max(i__1,i__2) && ! lquery) {
	    *info = -8;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYEV ", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	w[1] = a[a_dim1 + 1];
	work[1] = 2.;
	if (wantz) {
	    a[a_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
    rmax = sqrt(bignum);

/*     Scale matrix to allowable range, if necessary. */

    anrm = dlansy_("M", uplo, n, &a[a_offset], lda, &work[1]);
    iscale = 0;
    if (anrm > 0. && anrm < rmin) {
	iscale = 1;
	sigma = rmin / anrm;
    } else if (anrm > rmax) {
	iscale = 1;
	sigma = rmax / anrm;
    }
    if (iscale == 1) {
	dlascl_(uplo, &c__0, &c__0, &c_b17, &sigma, n, n, &a[a_offset], lda,
		info);
    }

/*     Call DSYTRD to reduce symmetric matrix to tridiagonal form. */

    inde = 1;
    indtau = inde + *n;
    indwrk = indtau + *n;
    llwork = *lwork - indwrk + 1;
    dsytrd_(uplo, n, &a[a_offset], lda, &w[1], &work[inde], &work[indtau], &
	    work[indwrk], &llwork, &iinfo);

/*     For eigenvalues only, call DSTERF.  For eigenvectors, first call */
/*     DORGTR to generate the orthogonal matrix, then call DSTEQR. */

    if (! wantz) {
	dsterf_(n, &w[1], &work[inde], info);
    } else {
	dorgtr_(uplo, n, &a[a_offset], lda, &work[indtau], &work[indwrk], &
		llwork, &iinfo);
	dsteqr_(jobz, n, &w[1], &work[inde], &a[a_offset], lda, &work[indtau],
		 info);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

    if (iscale == 1) {
	if (*info == 0) {
	    imax = *n;
	} else {
	    imax = *info - 1;
	}
	d__1 = 1. / sigma;
	dscal_(&imax, &d__1, &w[1], &c__1);
    }

/*     Set WORK(1) to optimal workspace size. */

    work[1] = (double) lwkopt;

    return 0;

/*     End of DSYEV */

} /* dsyev_ */

/* Subroutine */ int dsyevd_(const char *jobz, const char *uplo, integer *n, double *
	a, integer *lda, double *w, double *work, integer *lwork,
	integer *iwork, integer *liwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__0 = 0;
	static double c_b17 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    double d__1;

    /* Local variables */
    double eps;
    integer inde;
    double anrm, rmin, rmax;
    integer lopt;
    double sigma;
    integer iinfo, lwmin, liopt;
    bool lower, wantz;
    integer indwk2, llwrk2;
    integer iscale;
    double safmin;
    double bignum;
    integer indtau;
    integer indwrk, liwmin;
    integer llwork;
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

/*  DSYEVD computes all eigenvalues and, optionally, eigenvectors of a */
/*  real symmetric matrix A. If eigenvectors are desired, it uses a */
/*  divide and conquer algorithm. */

/*  The divide and conquer algorithm makes very mild assumptions about */
/*  floating point arithmetic. It will work on machines with a guard */
/*  digit in add/subtract, or on those binary machines without guard */
/*  digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or */
/*  Cray-2. It could conceivably fail on hexadecimal or decimal machines */
/*  without guard digits, but we know of none. */

/*  Because of large use of BLAS of level 3, DSYEVD needs N**2 more */
/*  workspace than DSYEVX. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the */
/*          leading N-by-N upper triangular part of A contains the */
/*          upper triangular part of the matrix A.  If UPLO = 'L', */
/*          the leading N-by-N lower triangular part of A contains */
/*          the lower triangular part of the matrix A. */
/*          On exit, if JOBZ = 'V', then if INFO = 0, A contains the */
/*          orthonormal eigenvectors of the matrix A. */
/*          If JOBZ = 'N', then on exit the lower triangle (if UPLO='L') */
/*          or the upper triangle (if UPLO='U') of A, including the */
/*          diagonal, is destroyed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, the eigenvalues in ascending order. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, */
/*                                         dimension (LWORK) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          If N <= 1,               LWORK must be at least 1. */
/*          If JOBZ = 'N' and N > 1, LWORK must be at least 2*N+1. */
/*          If JOBZ = 'V' and N > 1, LWORK must be at least */
/*                                                1 + 6*N + 2*N**2. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal sizes of the WORK and IWORK */
/*          arrays, returns these values as the first entries of the WORK */
/*          and IWORK arrays, and no error message related to LWORK or */
/*          LIWORK is issued by XERBLA. */

/*  IWORK   (workspace/output) INTEGER array, dimension (MAX(1,LIWORK)) */
/*          On exit, if INFO = 0, IWORK(1) returns the optimal LIWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK. */
/*          If N <= 1,                LIWORK must be at least 1. */
/*          If JOBZ  = 'N' and N > 1, LIWORK must be at least 1. */
/*          If JOBZ  = 'V' and N > 1, LIWORK must be at least 3 + 5*N. */

/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates the optimal sizes of the WORK and */
/*          IWORK arrays, returns these values as the first entries of */
/*          the WORK and IWORK arrays, and no error message related to */
/*          LWORK or LIWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i and JOBZ = 'N', then the algorithm failed */
/*                to converge; i off-diagonal elements of an intermediate */
/*                tridiagonal form did not converge to zero; */
/*                if INFO = i and JOBZ = 'V', then the algorithm failed */
/*                to compute an eigenvalue while working on the submatrix */
/*                lying in rows and columns INFO/(N+1) through */
/*                mod(INFO,N+1). */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Jeff Rutter, Computer Science Division, University of California */
/*     at Berkeley, USA */
/*  Modified by Francoise Tisseur, University of Tennessee. */

/*  Modified description of INFO. Sven, 16 Feb 05. */
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
    --w;
    --work;
    --iwork;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    lower = lsame_(uplo, "L");
    lquery = *lwork == -1 || *liwork == -1;

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (lower || lsame_(uplo, "U"))) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    }

    if (*info == 0) {
	if (*n <= 1) {
	    liwmin = 1;
	    lwmin = 1;
	    lopt = lwmin;
	    liopt = liwmin;
	} else {
	    if (wantz) {
		liwmin = *n * 5 + 3;
/* Computing 2nd power */
		i__1 = *n;
		lwmin = *n * 6 + 1 + (i__1 * i__1 << 1);
	    } else {
		liwmin = 1;
		lwmin = (*n << 1) + 1;
	    }
/* Computing MAX */
	    i__1 = lwmin, i__2 = (*n << 1) + ilaenv_(&c__1, "DSYTRD", uplo, n,
		     &c_n1, &c_n1, &c_n1);
	    lopt = std::max(i__1,i__2);
	    liopt = liwmin;
	}
	work[1] = (double) lopt;
	iwork[1] = liopt;

	if (*lwork < lwmin && ! lquery) {
	    *info = -8;
	} else if (*liwork < liwmin && ! lquery) {
	    *info = -10;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYEVD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	w[1] = a[a_dim1 + 1];
	if (wantz) {
	    a[a_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
    rmax = sqrt(bignum);

/*     Scale matrix to allowable range, if necessary. */

    anrm = dlansy_("M", uplo, n, &a[a_offset], lda, &work[1]);
    iscale = 0;
    if (anrm > 0. && anrm < rmin) {
	iscale = 1;
	sigma = rmin / anrm;
    } else if (anrm > rmax) {
	iscale = 1;
	sigma = rmax / anrm;
    }
    if (iscale == 1) {
	dlascl_(uplo, &c__0, &c__0, &c_b17, &sigma, n, n, &a[a_offset], lda,
		info);
    }

/*     Call DSYTRD to reduce symmetric matrix to tridiagonal form. */

    inde = 1;
    indtau = inde + *n;
    indwrk = indtau + *n;
    llwork = *lwork - indwrk + 1;
    indwk2 = indwrk + *n * *n;
    llwrk2 = *lwork - indwk2 + 1;

    dsytrd_(uplo, n, &a[a_offset], lda, &w[1], &work[inde], &work[indtau], &
	    work[indwrk], &llwork, &iinfo);
    lopt = (integer) ((*n << 1) + work[indwrk]);

/*     For eigenvalues only, call DSTERF.  For eigenvectors, first call */
/*     DSTEDC to generate the eigenvector matrix, WORK(INDWRK), of the */
/*     tridiagonal matrix, then call DORMTR to multiply it by the */
/*     Householder transformations stored in A. */

    if (! wantz) {
	dsterf_(n, &w[1], &work[inde], info);
    } else {
	dstedc_("I", n, &w[1], &work[inde], &work[indwrk], n, &work[indwk2], &
		llwrk2, &iwork[1], liwork, info);
	dormtr_("L", uplo, "N", n, n, &a[a_offset], lda, &work[indtau], &work[
		indwrk], n, &work[indwk2], &llwrk2, &iinfo);
	dlacpy_("A", n, n, &work[indwrk], n, &a[a_offset], lda);
/* Computing MAX */
/* Computing 2nd power */
	i__3 = *n;
	i__1 = lopt, i__2 = *n * 6 + 1 + (i__3 * i__3 << 1);
	lopt = std::max(i__1,i__2);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

    if (iscale == 1) {
	d__1 = 1. / sigma;
	dscal_(n, &d__1, &w[1], &c__1);
    }

    work[1] = (double) lopt;
    iwork[1] = liopt;

    return 0;

/*     End of DSYEVD */

} /* dsyevd_ */

/* Subroutine */ int dsyevr_(const char *jobz, const char *range, const char *uplo, integer *n,
	double *a, integer *lda, double *vl, double *vu, integer *
	il, integer *iu, double *abstol, integer *m, double *w,
	double *z__, integer *ldz, integer *isuppz, double *work,
	integer *lwork, integer *iwork, integer *liwork, integer *info)
{
	/* Table of constant values */
	static integer c__10 = 10;
	static integer c__1 = 1;
	static integer c__2 = 2;
	static integer c__3 = 3;
	static integer c__4 = 4;
	static integer c_n1 = -1;

    /* System generated locals */
    integer a_dim1, a_offset, z_dim1, z_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j, nb, jj;
    double eps, vll, vuu, tmp1;
    integer indd, inde;
    double anrm;
    integer imax;
    double rmin, rmax;
    integer inddd, indee;
    double sigma;
    integer iinfo;
    char order[1];
    integer indwk;
    integer lwmin;
    bool lower, wantz;
    bool alleig, indeig;
    integer iscale, ieeeok, indibl, indifl;
    bool valeig;
    double safmin;
    double abstll, bignum;
    integer indtau, indisp;
    integer indiwo, indwkn;
    integer liwmin;
    bool tryrac;
    integer llwrkn, llwork, nsplit;
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

/*  DSYEVR computes selected eigenvalues and, optionally, eigenvectors */
/*  of a real symmetric matrix A.  Eigenvalues and eigenvectors can be */
/*  selected by specifying either a range of values or a range of */
/*  indices for the desired eigenvalues. */

/*  DSYEVR first reduces the matrix A to tridiagonal form T with a call */
/*  to DSYTRD.  Then, whenever possible, DSYEVR calls DSTEMR to compute */
/*  the eigenspectrum using Relatively Robust Representations.  DSTEMR */
/*  computes eigenvalues by the dqds algorithm, while orthogonal */
/*  eigenvectors are computed from various "good" L D L^T representations */
/*  (also known as Relatively Robust Representations). Gram-Schmidt */
/*  orthogonalization is avoided as far as possible. More specifically, */
/*  the various steps of the algorithm are as follows. */

/*  For each unreduced block (submatrix) of T, */
/*     (a) Compute T - sigma I  = L D L^T, so that L and D */
/*         define all the wanted eigenvalues to high relative accuracy. */
/*         This means that small relative changes in the entries of D and L */
/*         cause only small relative changes in the eigenvalues and */
/*         eigenvectors. The standard (unfactored) representation of the */
/*         tridiagonal matrix T does not have this property in general. */
/*     (b) Compute the eigenvalues to suitable accuracy. */
/*         If the eigenvectors are desired, the algorithm attains full */
/*         accuracy of the computed eigenvalues only right before */
/*         the corresponding vectors have to be computed, see steps c) and d). */
/*     (c) For each cluster of close eigenvalues, select a new */
/*         shift close to the cluster, find a new factorization, and refine */
/*         the shifted eigenvalues to suitable accuracy. */
/*     (d) For each eigenvalue with a large enough relative separation compute */
/*         the corresponding eigenvector by forming a rank revealing twisted */
/*         factorization. Go back to (c) for any clusters that remain. */

/*  The desired accuracy of the output can be specified by the input */
/*  parameter ABSTOL. */

/*  For more details, see DSTEMR's documentation and: */
/*  - Inderjit S. Dhillon and Beresford N. Parlett: "Multiple representations */
/*    to compute orthogonal eigenvectors of symmetric tridiagonal matrices," */
/*    Linear Algebra and its Applications, 387(1), pp. 1-28, August 2004. */
/*  - Inderjit Dhillon and Beresford Parlett: "Orthogonal Eigenvectors and */
/*    Relative Gaps," SIAM Journal on Matrix Analysis and Applications, Vol. 25, */
/*    2004.  Also LAPACK Working Note 154. */
/*  - Inderjit Dhillon: "A new O(n^2) algorithm for the symmetric */
/*    tridiagonal eigenvalue/eigenvector problem", */
/*    Computer Science Division Technical Report No. UCB/CSD-97-971, */
/*    UC Berkeley, May 1997. */


/*  Note 1 : DSYEVR calls DSTEMR when the full spectrum is requested */
/*  on machines which conform to the ieee-754 floating point standard. */
/*  DSYEVR calls DSTEBZ and SSTEIN on non-ieee machines and */
/*  when partial spectrum requests are made. */

/*  Normal execution of DSTEMR may create NaNs and infinities and */
/*  hence may abort due to a floating point exception in environments */
/*  which do not handle NaNs and infinities in the ieee standard default */
/*  manner. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  RANGE   (input) CHARACTER*1 */
/*          = 'A': all eigenvalues will be found. */
/*          = 'V': all eigenvalues in the half-open interval (VL,VU] */
/*                 will be found. */
/*          = 'I': the IL-th through IU-th eigenvalues will be found. */
/* ********* For RANGE = 'V' or 'I' and IU - IL < N - 1, DSTEBZ and */
/* ********* DSTEIN are called */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the */
/*          leading N-by-N upper triangular part of A contains the */
/*          upper triangular part of the matrix A.  If UPLO = 'L', */
/*          the leading N-by-N lower triangular part of A contains */
/*          the lower triangular part of the matrix A. */
/*          On exit, the lower triangle (if UPLO='L') or the upper */
/*          triangle (if UPLO='U') of A, including the diagonal, is */
/*          destroyed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  VL      (input) DOUBLE PRECISION */
/*  VU      (input) DOUBLE PRECISION */
/*          If RANGE='V', the lower and upper bounds of the interval to */
/*          be searched for eigenvalues. VL < VU. */
/*          Not referenced if RANGE = 'A' or 'I'. */

/*  IL      (input) INTEGER */
/*  IU      (input) INTEGER */
/*          If RANGE='I', the indices (in ascending order) of the */
/*          smallest and largest eigenvalues to be returned. */
/*          1 <= IL <= IU <= N, if N > 0; IL = 1 and IU = 0 if N = 0. */
/*          Not referenced if RANGE = 'A' or 'V'. */

/*  ABSTOL  (input) DOUBLE PRECISION */
/*          The absolute error tolerance for the eigenvalues. */
/*          An approximate eigenvalue is accepted as converged */
/*          when it is determined to lie in an interval [a,b] */
/*          of width less than or equal to */

/*                  ABSTOL + EPS *   max( |a|,|b| ) , */

/*          where EPS is the machine precision.  If ABSTOL is less than */
/*          or equal to zero, then  EPS*|T|  will be used in its place, */
/*          where |T| is the 1-norm of the tridiagonal matrix obtained */
/*          by reducing A to tridiagonal form. */

/*          See "Computing Small Singular Values of Bidiagonal Matrices */
/*          with Guaranteed High Relative Accuracy," by Demmel and */
/*          Kahan, LAPACK Working Note #3. */

/*          If high relative accuracy is important, set ABSTOL to */
/*          DLAMCH( 'Safe minimum' ).  Doing so will guarantee that */
/*          eigenvalues are computed to high relative accuracy when */
/*          possible in future releases.  The current code does not */
/*          make any guarantees about high relative accuracy, but */
/*          future releases will. See J. Barlow and J. Demmel, */
/*          "Computing Accurate Eigensystems of Scaled Diagonally */
/*          Dominant Matrices", LAPACK Working Note #7, for a discussion */
/*          of which matrices define their eigenvalues to high relative */
/*          accuracy. */

/*  M       (output) INTEGER */
/*          The total number of eigenvalues found.  0 <= M <= N. */
/*          If RANGE = 'A', M = N, and if RANGE = 'I', M = IU-IL+1. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          The first M elements contain the selected eigenvalues in */
/*          ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, max(1,M)) */
/*          If JOBZ = 'V', then if INFO = 0, the first M columns of Z */
/*          contain the orthonormal eigenvectors of the matrix A */
/*          corresponding to the selected eigenvalues, with the i-th */
/*          column of Z holding the eigenvector associated with W(i). */
/*          If JOBZ = 'N', then Z is not referenced. */
/*          Note: the user must ensure that at least max(1,M) columns are */
/*          supplied in the array Z; if RANGE = 'V', the exact value of M */
/*          is not known in advance and an upper bound must be used. */
/*          Supplying N columns is always safe. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  ISUPPZ  (output) INTEGER array, dimension ( 2*max(1,M) ) */
/*          The support of the eigenvectors in Z, i.e., the indices */
/*          indicating the nonzero elements in Z. The i-th eigenvector */
/*          is nonzero only in elements ISUPPZ( 2*i-1 ) through */
/*          ISUPPZ( 2*i ). */
/* ********* Implemented only for RANGE = 'A' or 'I' and IU - IL = N - 1 */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= max(1,26*N). */
/*          For optimal efficiency, LWORK >= (NB+6)*N, */
/*          where NB is the max of the blocksize for DSYTRD and DORMTR */
/*          returned by ILAENV. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  IWORK   (workspace/output) INTEGER array, dimension (MAX(1,LIWORK)) */
/*          On exit, if INFO = 0, IWORK(1) returns the optimal LWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK.  LIWORK >= max(1,10*N). */

/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates the optimal size of the IWORK array, */
/*          returns this value as the first entry of the IWORK array, and */
/*          no error message related to LIWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  Internal error */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Inderjit Dhillon, IBM Almaden, USA */
/*     Osni Marques, LBNL/NERSC, USA */
/*     Ken Stanley, Computer Science Division, University of */
/*       California at Berkeley, USA */
/*     Jason Riedy, Computer Science Division, University of */
/*       California at Berkeley, USA */

/* ===================================================================== */

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
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --isuppz;
    --work;
    --iwork;

    /* Function Body */
    ieeeok = ilaenv_(&c__10, "DSYEVR", "N", &c__1, &c__2, &c__3, &c__4);

    lower = lsame_(uplo, "L");
    wantz = lsame_(jobz, "V");
    alleig = lsame_(range, "A");
    valeig = lsame_(range, "V");
    indeig = lsame_(range, "I");

    lquery = *lwork == -1 || *liwork == -1;

/* Computing MAX */
    i__1 = 1, i__2 = *n * 26;
    lwmin = std::max(i__1,i__2);
/* Computing MAX */
    i__1 = 1, i__2 = *n * 10;
    liwmin = std::max(i__1,i__2);

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (alleig || valeig || indeig)) {
	*info = -2;
    } else if (! (lower || lsame_(uplo, "U"))) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -6;
    } else {
	if (valeig) {
	    if (*n > 0 && *vu <= *vl) {
		*info = -8;
	    }
	} else if (indeig) {
	    if (*il < 1 || *il > std::max(1_integer,*n)) {
		*info = -9;
	    } else if (*iu < std::min(*n,*il) || *iu > *n) {
		*info = -10;
	    }
	}
    }
    if (*info == 0) {
	if (*ldz < 1 || wantz && *ldz < *n) {
	    *info = -15;
	} else if (*lwork < lwmin && ! lquery) {
	    *info = -18;
	} else if (*liwork < liwmin && ! lquery) {
	    *info = -20;
	}
    }

    if (*info == 0) {
	nb = ilaenv_(&c__1, "DSYTRD", uplo, n, &c_n1, &c_n1, &c_n1);
/* Computing MAX */
	i__1 = nb, i__2 = ilaenv_(&c__1, "DORMTR", uplo, n, &c_n1, &c_n1, &
		c_n1);
	nb = std::max(i__1,i__2);
/* Computing MAX */
	i__1 = (nb + 1) * *n;
	lwkopt = std::max(i__1,lwmin);
	work[1] = (double) lwkopt;
	iwork[1] = liwmin;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYEVR", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    *m = 0;
    if (*n == 0) {
	work[1] = 1.;
	return 0;
    }

    if (*n == 1) {
	work[1] = 7.;
	if (alleig || indeig) {
	    *m = 1;
	    w[1] = a[a_dim1 + 1];
	} else {
	    if (*vl < a[a_dim1 + 1] && *vu >= a[a_dim1 + 1]) {
		*m = 1;
		w[1] = a[a_dim1 + 1];
	    }
	}
	if (wantz) {
	    z__[z_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
/* Computing MIN */
    d__1 = sqrt(bignum), d__2 = 1. / sqrt(sqrt(safmin));
    rmax = std::min(d__1,d__2);

/*     Scale matrix to allowable range, if necessary. */

    iscale = 0;
    abstll = *abstol;
    vll = *vl;
    vuu = *vu;
    anrm = dlansy_("M", uplo, n, &a[a_offset], lda, &work[1]);
    if (anrm > 0. && anrm < rmin) {
	iscale = 1;
	sigma = rmin / anrm;
    } else if (anrm > rmax) {
	iscale = 1;
	sigma = rmax / anrm;
    }
    if (iscale == 1) {
	if (lower) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j + 1;
		dscal_(&i__2, &sigma, &a[j + j * a_dim1], &c__1);
/* L10: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		dscal_(&j, &sigma, &a[j * a_dim1 + 1], &c__1);
/* L20: */
	    }
	}
	if (*abstol > 0.) {
	    abstll = *abstol * sigma;
	}
	if (valeig) {
	    vll = *vl * sigma;
	    vuu = *vu * sigma;
	}
    }
/*     Initialize indices into workspaces.  Note: The IWORK indices are */
/*     used only if DSTERF or DSTEMR fail. */
/*     WORK(INDTAU:INDTAU+N-1) stores the scalar factors of the */
/*     elementary reflectors used in DSYTRD. */
    indtau = 1;
/*     WORK(INDD:INDD+N-1) stores the tridiagonal's diagonal entries. */
    indd = indtau + *n;
/*     WORK(INDE:INDE+N-1) stores the off-diagonal entries of the */
/*     tridiagonal matrix from DSYTRD. */
    inde = indd + *n;
/*     WORK(INDDD:INDDD+N-1) is a copy of the diagonal entries over */
/*     -written by DSTEMR (the DSTERF path copies the diagonal to W). */
    inddd = inde + *n;
/*     WORK(INDEE:INDEE+N-1) is a copy of the off-diagonal entries over */
/*     -written while computing the eigenvalues in DSTERF and DSTEMR. */
    indee = inddd + *n;
/*     INDWK is the starting offset of the left-over workspace, and */
/*     LLWORK is the remaining workspace size. */
    indwk = indee + *n;
    llwork = *lwork - indwk + 1;
/*     IWORK(INDIBL:INDIBL+M-1) corresponds to IBLOCK in DSTEBZ and */
/*     stores the block indices of each of the M<=N eigenvalues. */
    indibl = 1;
/*     IWORK(INDISP:INDISP+NSPLIT-1) corresponds to ISPLIT in DSTEBZ and */
/*     stores the starting and finishing indices of each block. */
    indisp = indibl + *n;
/*     IWORK(INDIFL:INDIFL+N-1) stores the indices of eigenvectors */
/*     that corresponding to eigenvectors that fail to converge in */
/*     DSTEIN.  This information is discarded; if any fail, the driver */
/*     returns INFO > 0. */
    indifl = indisp + *n;
/*     INDIWO is the offset of the remaining integer workspace. */
    indiwo = indisp + *n;

/*     Call DSYTRD to reduce symmetric matrix to tridiagonal form. */

    dsytrd_(uplo, n, &a[a_offset], lda, &work[indd], &work[inde], &work[
	    indtau], &work[indwk], &llwork, &iinfo);

/*     If all eigenvalues are desired */
/*     then call DSTERF or DSTEMR and DORMTR. */

    if ((alleig || indeig && *il == 1 && *iu == *n) && ieeeok == 1) {
	if (! wantz) {
	    dcopy_(n, &work[indd], &c__1, &w[1], &c__1);
	    i__1 = *n - 1;
	    dcopy_(&i__1, &work[inde], &c__1, &work[indee], &c__1);
	    dsterf_(n, &w[1], &work[indee], info);
	} else {
	    i__1 = *n - 1;
	    dcopy_(&i__1, &work[inde], &c__1, &work[indee], &c__1);
	    dcopy_(n, &work[indd], &c__1, &work[inddd], &c__1);

	    if (*abstol <= *n * 0. * eps) {
		tryrac = true;
	    } else {
		tryrac = false;
	    }
	    dstemr_(jobz, "A", n, &work[inddd], &work[indee], vl, vu, il, iu,
		    m, &w[1], &z__[z_offset], ldz, n, &isuppz[1], &tryrac, &
		    work[indwk], lwork, &iwork[1], liwork, info);



/*        Apply orthogonal matrix used in reduction to tridiagonal */
/*        form to eigenvectors returned by DSTEIN. */

	    if (wantz && *info == 0) {
		indwkn = inde;
		llwrkn = *lwork - indwkn + 1;
		dormtr_("L", uplo, "N", n, m, &a[a_offset], lda, &work[indtau]
, &z__[z_offset], ldz, &work[indwkn], &llwrkn, &iinfo);
	    }
	}


	if (*info == 0) {
/*           Everything worked.  Skip DSTEBZ/DSTEIN.  IWORK(:) are */
/*           undefined. */
	    *m = *n;
	    goto L30;
	}
	*info = 0;
    }

/*     Otherwise, call DSTEBZ and, if eigenvectors are desired, DSTEIN. */
/*     Also call DSTEBZ and DSTEIN if DSTEMR fails. */

    if (wantz) {
	*(unsigned char *)order = 'B';
    } else {
	*(unsigned char *)order = 'E';
    }
    dstebz_(range, order, n, &vll, &vuu, il, iu, &abstll, &work[indd], &work[
	    inde], m, &nsplit, &w[1], &iwork[indibl], &iwork[indisp], &work[
	    indwk], &iwork[indiwo], info);

    if (wantz) {
	dstein_(n, &work[indd], &work[inde], m, &w[1], &iwork[indibl], &iwork[
		indisp], &z__[z_offset], ldz, &work[indwk], &iwork[indiwo], &
		iwork[indifl], info);

/*        Apply orthogonal matrix used in reduction to tridiagonal */
/*        form to eigenvectors returned by DSTEIN. */

	indwkn = inde;
	llwrkn = *lwork - indwkn + 1;
	dormtr_("L", uplo, "N", n, m, &a[a_offset], lda, &work[indtau], &z__[
		z_offset], ldz, &work[indwkn], &llwrkn, &iinfo);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

/*  Jump here if DSTEMR/DSTEIN succeeded. */
L30:
    if (iscale == 1) {
	if (*info == 0) {
	    imax = *m;
	} else {
	    imax = *info - 1;
	}
	d__1 = 1. / sigma;
	dscal_(&imax, &d__1, &w[1], &c__1);
    }

/*     If eigenvalues are not in order, then sort them, along with */
/*     eigenvectors.  Note: We do not sort the IFAIL portion of IWORK. */
/*     It may not be initialized (if DSTEMR/DSTEIN succeeded), and we do */
/*     not return this detailed information to the user. */

    if (wantz) {
	i__1 = *m - 1;
	for (j = 1; j <= i__1; ++j) {
	    i__ = 0;
	    tmp1 = w[j];
	    i__2 = *m;
	    for (jj = j + 1; jj <= i__2; ++jj) {
		if (w[jj] < tmp1) {
		    i__ = jj;
		    tmp1 = w[jj];
		}
/* L40: */
	    }

	    if (i__ != 0) {
		w[i__] = w[j];
		w[j] = tmp1;
		dswap_(n, &z__[i__ * z_dim1 + 1], &c__1, &z__[j * z_dim1 + 1],
			 &c__1);
	    }
/* L50: */
	}
    }

/*     Set WORK(1) to optimal workspace size. */

    work[1] = (double) lwkopt;
    iwork[1] = liwmin;

    return 0;

/*     End of DSYEVR */

} /* dsyevr_ */

/* Subroutine */ int dsyevx_(const char *jobz, const char *range, const char *uplo, integer *n,
	double *a, integer *lda, double *vl, double *vu, integer *
	il, integer *iu, double *abstol, integer *m, double *w,
	double *z__, integer *ldz, double *work, integer *lwork,
	integer *iwork, integer *ifail, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;

    /* System generated locals */
    integer a_dim1, a_offset, z_dim1, z_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer i__, j, nb, jj;
    double eps, vll, vuu, tmp1;
    integer indd, inde;
    double anrm;
    integer imax;
    double rmin, rmax;
    bool test;
    integer itmp1, indee;
    double sigma;
    integer iinfo;
    char order[1];
    bool lower, wantz;
    bool alleig, indeig;
    integer iscale, indibl;
    bool valeig;
    double safmin;
    double abstll, bignum;
    integer indtau, indisp;
    integer indiwo, indwkn;
    integer indwrk, lwkmin;
    integer llwrkn, llwork, nsplit;
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

/*  DSYEVX computes selected eigenvalues and, optionally, eigenvectors */
/*  of a real symmetric matrix A.  Eigenvalues and eigenvectors can be */
/*  selected by specifying either a range of values or a range of indices */
/*  for the desired eigenvalues. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  RANGE   (input) CHARACTER*1 */
/*          = 'A': all eigenvalues will be found. */
/*          = 'V': all eigenvalues in the half-open interval (VL,VU] */
/*                 will be found. */
/*          = 'I': the IL-th through IU-th eigenvalues will be found. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the */
/*          leading N-by-N upper triangular part of A contains the */
/*          upper triangular part of the matrix A.  If UPLO = 'L', */
/*          the leading N-by-N lower triangular part of A contains */
/*          the lower triangular part of the matrix A. */
/*          On exit, the lower triangle (if UPLO='L') or the upper */
/*          triangle (if UPLO='U') of A, including the diagonal, is */
/*          destroyed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  VL      (input) DOUBLE PRECISION */
/*  VU      (input) DOUBLE PRECISION */
/*          If RANGE='V', the lower and upper bounds of the interval to */
/*          be searched for eigenvalues. VL < VU. */
/*          Not referenced if RANGE = 'A' or 'I'. */

/*  IL      (input) INTEGER */
/*  IU      (input) INTEGER */
/*          If RANGE='I', the indices (in ascending order) of the */
/*          smallest and largest eigenvalues to be returned. */
/*          1 <= IL <= IU <= N, if N > 0; IL = 1 and IU = 0 if N = 0. */
/*          Not referenced if RANGE = 'A' or 'V'. */

/*  ABSTOL  (input) DOUBLE PRECISION */
/*          The absolute error tolerance for the eigenvalues. */
/*          An approximate eigenvalue is accepted as converged */
/*          when it is determined to lie in an interval [a,b] */
/*          of width less than or equal to */

/*                  ABSTOL + EPS *   max( |a|,|b| ) , */

/*          where EPS is the machine precision.  If ABSTOL is less than */
/*          or equal to zero, then  EPS*|T|  will be used in its place, */
/*          where |T| is the 1-norm of the tridiagonal matrix obtained */
/*          by reducing A to tridiagonal form. */

/*          Eigenvalues will be computed most accurately when ABSTOL is */
/*          set to twice the underflow threshold 2*DLAMCH('S'), not zero. */
/*          If this routine returns with INFO>0, indicating that some */
/*          eigenvectors did not converge, try setting ABSTOL to */
/*          2*DLAMCH('S'). */

/*          See "Computing Small Singular Values of Bidiagonal Matrices */
/*          with Guaranteed High Relative Accuracy," by Demmel and */
/*          Kahan, LAPACK Working Note #3. */

/*  M       (output) INTEGER */
/*          The total number of eigenvalues found.  0 <= M <= N. */
/*          If RANGE = 'A', M = N, and if RANGE = 'I', M = IU-IL+1. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          On normal exit, the first M elements contain the selected */
/*          eigenvalues in ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, max(1,M)) */
/*          If JOBZ = 'V', then if INFO = 0, the first M columns of Z */
/*          contain the orthonormal eigenvectors of the matrix A */
/*          corresponding to the selected eigenvalues, with the i-th */
/*          column of Z holding the eigenvector associated with W(i). */
/*          If an eigenvector fails to converge, then that column of Z */
/*          contains the latest approximation to the eigenvector, and the */
/*          index of the eigenvector is returned in IFAIL. */
/*          If JOBZ = 'N', then Z is not referenced. */
/*          Note: the user must ensure that at least max(1,M) columns are */
/*          supplied in the array Z; if RANGE = 'V', the exact value of M */
/*          is not known in advance and an upper bound must be used. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= 1, when N <= 1; */
/*          otherwise 8*N. */
/*          For optimal efficiency, LWORK >= (NB+3)*N, */
/*          where NB is the max of the blocksize for DSYTRD and DORMTR */
/*          returned by ILAENV. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  IWORK   (workspace) INTEGER array, dimension (5*N) */

/*  IFAIL   (output) INTEGER array, dimension (N) */
/*          If JOBZ = 'V', then if INFO = 0, the first M elements of */
/*          IFAIL are zero.  If INFO > 0, then IFAIL contains the */
/*          indices of the eigenvectors that failed to converge. */
/*          If JOBZ = 'N', then IFAIL is not referenced. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, then i eigenvectors failed to converge. */
/*                Their indices are stored in array IFAIL. */

/* ===================================================================== */

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
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;
    --iwork;
    --ifail;

    /* Function Body */
    lower = lsame_(uplo, "L");
    wantz = lsame_(jobz, "V");
    alleig = lsame_(range, "A");
    valeig = lsame_(range, "V");
    indeig = lsame_(range, "I");
    lquery = *lwork == -1;

    *info = 0;
    if (! (wantz || lsame_(jobz, "N"))) {
	*info = -1;
    } else if (! (alleig || valeig || indeig)) {
	*info = -2;
    } else if (! (lower || lsame_(uplo, "U"))) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -6;
    } else {
	if (valeig) {
	    if (*n > 0 && *vu <= *vl) {
		*info = -8;
	    }
	} else if (indeig) {
	    if (*il < 1 || *il > std::max(1_integer,*n)) {
		*info = -9;
	    } else if (*iu < std::min(*n,*il) || *iu > *n) {
		*info = -10;
	    }
	}
    }
    if (*info == 0) {
	if (*ldz < 1 || wantz && *ldz < *n) {
	    *info = -15;
	}
    }

    if (*info == 0) {
	if (*n <= 1) {
	    lwkmin = 1;
	    work[1] = (double) lwkmin;
	} else {
	    lwkmin = *n << 3;
	    nb = ilaenv_(&c__1, "DSYTRD", uplo, n, &c_n1, &c_n1, &c_n1);
/* Computing MAX */
	    i__1 = nb, i__2 = ilaenv_(&c__1, "DORMTR", uplo, n, &c_n1, &c_n1,
		    &c_n1);
	    nb = std::max(i__1,i__2);
/* Computing MAX */
	    i__1 = lwkmin, i__2 = (nb + 3) * *n;
	    lwkopt = std::max(i__1,i__2);
	    work[1] = (double) lwkopt;
	}

	if (*lwork < lwkmin && ! lquery) {
	    *info = -17;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYEVX", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    *m = 0;
    if (*n == 0) {
	return 0;
    }

    if (*n == 1) {
	if (alleig || indeig) {
	    *m = 1;
	    w[1] = a[a_dim1 + 1];
	} else {
	    if (*vl < a[a_dim1 + 1] && *vu >= a[a_dim1 + 1]) {
		*m = 1;
		w[1] = a[a_dim1 + 1];
	    }
	}
	if (wantz) {
	    z__[z_dim1 + 1] = 1.;
	}
	return 0;
    }

/*     Get machine constants. */

    safmin = dlamch_("Safe minimum");
    eps = dlamch_("Precision");
    smlnum = safmin / eps;
    bignum = 1. / smlnum;
    rmin = sqrt(smlnum);
/* Computing MIN */
    d__1 = sqrt(bignum), d__2 = 1. / sqrt(sqrt(safmin));
    rmax = std::min(d__1,d__2);

/*     Scale matrix to allowable range, if necessary. */

    iscale = 0;
    abstll = *abstol;
    if (valeig) {
	vll = *vl;
	vuu = *vu;
    }
    anrm = dlansy_("M", uplo, n, &a[a_offset], lda, &work[1]);
    if (anrm > 0. && anrm < rmin) {
	iscale = 1;
	sigma = rmin / anrm;
    } else if (anrm > rmax) {
	iscale = 1;
	sigma = rmax / anrm;
    }
    if (iscale == 1) {
	if (lower) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j + 1;
		dscal_(&i__2, &sigma, &a[j + j * a_dim1], &c__1);
/* L10: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		dscal_(&j, &sigma, &a[j * a_dim1 + 1], &c__1);
/* L20: */
	    }
	}
	if (*abstol > 0.) {
	    abstll = *abstol * sigma;
	}
	if (valeig) {
	    vll = *vl * sigma;
	    vuu = *vu * sigma;
	}
    }

/*     Call DSYTRD to reduce symmetric matrix to tridiagonal form. */

    indtau = 1;
    inde = indtau + *n;
    indd = inde + *n;
    indwrk = indd + *n;
    llwork = *lwork - indwrk + 1;
    dsytrd_(uplo, n, &a[a_offset], lda, &work[indd], &work[inde], &work[
	    indtau], &work[indwrk], &llwork, &iinfo);

/*     If all eigenvalues are desired and ABSTOL is less than or equal to */
/*     zero, then call DSTERF or DORGTR and SSTEQR.  If this fails for */
/*     some eigenvalue, then try DSTEBZ. */

    test = false;
    if (indeig) {
	if (*il == 1 && *iu == *n) {
	    test = true;
	}
    }
    if ((alleig || test) && *abstol <= 0.) {
	dcopy_(n, &work[indd], &c__1, &w[1], &c__1);
	indee = indwrk + (*n << 1);
	if (! wantz) {
	    i__1 = *n - 1;
	    dcopy_(&i__1, &work[inde], &c__1, &work[indee], &c__1);
	    dsterf_(n, &w[1], &work[indee], info);
	} else {
	    dlacpy_("A", n, n, &a[a_offset], lda, &z__[z_offset], ldz);
	    dorgtr_(uplo, n, &z__[z_offset], ldz, &work[indtau], &work[indwrk]
, &llwork, &iinfo);
	    i__1 = *n - 1;
	    dcopy_(&i__1, &work[inde], &c__1, &work[indee], &c__1);
	    dsteqr_(jobz, n, &w[1], &work[indee], &z__[z_offset], ldz, &work[
		    indwrk], info);
	    if (*info == 0) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    ifail[i__] = 0;
/* L30: */
		}
	    }
	}
	if (*info == 0) {
	    *m = *n;
	    goto L40;
	}
	*info = 0;
    }

/*     Otherwise, call DSTEBZ and, if eigenvectors are desired, SSTEIN. */

    if (wantz) {
	*(unsigned char *)order = 'B';
    } else {
	*(unsigned char *)order = 'E';
    }
    indibl = 1;
    indisp = indibl + *n;
    indiwo = indisp + *n;
    dstebz_(range, order, n, &vll, &vuu, il, iu, &abstll, &work[indd], &work[
	    inde], m, &nsplit, &w[1], &iwork[indibl], &iwork[indisp], &work[
	    indwrk], &iwork[indiwo], info);

    if (wantz) {
	dstein_(n, &work[indd], &work[inde], m, &w[1], &iwork[indibl], &iwork[
		indisp], &z__[z_offset], ldz, &work[indwrk], &iwork[indiwo], &
		ifail[1], info);

/*        Apply orthogonal matrix used in reduction to tridiagonal */
/*        form to eigenvectors returned by DSTEIN. */

	indwkn = inde;
	llwrkn = *lwork - indwkn + 1;
	dormtr_("L", uplo, "N", n, m, &a[a_offset], lda, &work[indtau], &z__[
		z_offset], ldz, &work[indwkn], &llwrkn, &iinfo);
    }

/*     If matrix was scaled, then rescale eigenvalues appropriately. */

L40:
    if (iscale == 1) {
	if (*info == 0) {
	    imax = *m;
	} else {
	    imax = *info - 1;
	}
	d__1 = 1. / sigma;
	dscal_(&imax, &d__1, &w[1], &c__1);
    }

/*     If eigenvalues are not in order, then sort them, along with */
/*     eigenvectors. */

    if (wantz) {
	i__1 = *m - 1;
	for (j = 1; j <= i__1; ++j) {
	    i__ = 0;
	    tmp1 = w[j];
	    i__2 = *m;
	    for (jj = j + 1; jj <= i__2; ++jj) {
		if (w[jj] < tmp1) {
		    i__ = jj;
		    tmp1 = w[jj];
		}
/* L50: */
	    }

	    if (i__ != 0) {
		itmp1 = iwork[indibl + i__ - 1];
		w[i__] = w[j];
		iwork[indibl + i__ - 1] = iwork[indibl + j - 1];
		w[j] = tmp1;
		iwork[indibl + j - 1] = itmp1;
		dswap_(n, &z__[i__ * z_dim1 + 1], &c__1, &z__[j * z_dim1 + 1],
			 &c__1);
		if (*info != 0) {
		    itmp1 = ifail[i__];
		    ifail[i__] = ifail[j];
		    ifail[j] = itmp1;
		}
	    }
/* L60: */
	}
    }

/*     Set WORK(1) to optimal workspace size. */

    work[1] = (double) lwkopt;

    return 0;

/*     End of DSYEVX */

} /* dsyevx_ */

/* Subroutine */ int dsygs2_(integer *itype, const char *uplo, integer *n,
	double *a, integer *lda, double *b, integer *ldb, integer *
	info)
{
	/* Table of constant values */
	static double c_b6 = -1.;
	static integer c__1 = 1;
	static double c_b27 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;
    double d__1;

    /* Local variables */
    integer k;
    double ct, akk, bkk;
    bool upper;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSYGS2 reduces a real symmetric-definite generalized eigenproblem */
/*  to standard form. */

/*  If ITYPE = 1, the problem is A*x = lambda*B*x, */
/*  and A is overwritten by inv(U')*A*inv(U) or inv(L)*A*inv(L') */

/*  If ITYPE = 2 or 3, the problem is A*B*x = lambda*x or */
/*  B*A*x = lambda*x, and A is overwritten by U*A*U` or L'*A*L. */

/*  B must have been previously factorized as U'*U or L*L' by DPOTRF. */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          = 1: compute inv(U')*A*inv(U) or inv(L)*A*inv(L'); */
/*          = 2 or 3: compute U*A*U' or L'*A*L. */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is stored, and how B has been factorized. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the leading */
/*          n by n upper triangular part of A contains the upper */
/*          triangular part of the matrix A, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading n by n lower triangular part of A contains the lower */
/*          triangular part of the matrix A, and the strictly upper */
/*          triangular part of A is not referenced. */

/*          On exit, if INFO = 0, the transformed matrix, stored in the */
/*          same format as A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,N) */
/*          The triangular factor from the Cholesky factorization of B, */
/*          as returned by DPOTRF. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

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
    upper = lsame_(uplo, "U");
    if (*itype < 1 || *itype > 3) {
	*info = -1;
    } else if (! upper && ! lsame_(uplo, "L")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -7;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYGS2", &i__1);
	return 0;
    }

    if (*itype == 1) {
	if (upper) {

/*           Compute inv(U')*A*inv(U) */

	    i__1 = *n;
	    for (k = 1; k <= i__1; ++k) {

/*              Update the upper triangle of A(k:n,k:n) */

		akk = a[k + k * a_dim1];
		bkk = b[k + k * b_dim1];
/* Computing 2nd power */
		d__1 = bkk;
		akk /= d__1 * d__1;
		a[k + k * a_dim1] = akk;
		if (k < *n) {
		    i__2 = *n - k;
		    d__1 = 1. / bkk;
		    dscal_(&i__2, &d__1, &a[k + (k + 1) * a_dim1], lda);
		    ct = akk * -.5;
		    i__2 = *n - k;
		    daxpy_(&i__2, &ct, &b[k + (k + 1) * b_dim1], ldb, &a[k + (
			    k + 1) * a_dim1], lda);
		    i__2 = *n - k;
		    dsyr2_(uplo, &i__2, &c_b6, &a[k + (k + 1) * a_dim1], lda,
			    &b[k + (k + 1) * b_dim1], ldb, &a[k + 1 + (k + 1)
			    * a_dim1], lda);
		    i__2 = *n - k;
		    daxpy_(&i__2, &ct, &b[k + (k + 1) * b_dim1], ldb, &a[k + (
			    k + 1) * a_dim1], lda);
		    i__2 = *n - k;
		    dtrsv_(uplo, "Transpose", "Non-unit", &i__2, &b[k + 1 + (
			    k + 1) * b_dim1], ldb, &a[k + (k + 1) * a_dim1],
			    lda);
		}
/* L10: */
	    }
	} else {

/*           Compute inv(L)*A*inv(L') */

	    i__1 = *n;
	    for (k = 1; k <= i__1; ++k) {

/*              Update the lower triangle of A(k:n,k:n) */

		akk = a[k + k * a_dim1];
		bkk = b[k + k * b_dim1];
/* Computing 2nd power */
		d__1 = bkk;
		akk /= d__1 * d__1;
		a[k + k * a_dim1] = akk;
		if (k < *n) {
		    i__2 = *n - k;
		    d__1 = 1. / bkk;
		    dscal_(&i__2, &d__1, &a[k + 1 + k * a_dim1], &c__1);
		    ct = akk * -.5;
		    i__2 = *n - k;
		    daxpy_(&i__2, &ct, &b[k + 1 + k * b_dim1], &c__1, &a[k +
			    1 + k * a_dim1], &c__1);
		    i__2 = *n - k;
		    dsyr2_(uplo, &i__2, &c_b6, &a[k + 1 + k * a_dim1], &c__1,
			    &b[k + 1 + k * b_dim1], &c__1, &a[k + 1 + (k + 1)
			    * a_dim1], lda);
		    i__2 = *n - k;
		    daxpy_(&i__2, &ct, &b[k + 1 + k * b_dim1], &c__1, &a[k +
			    1 + k * a_dim1], &c__1);
		    i__2 = *n - k;
		    dtrsv_(uplo, "No transpose", "Non-unit", &i__2, &b[k + 1
			    + (k + 1) * b_dim1], ldb, &a[k + 1 + k * a_dim1],
			    &c__1);
		}
/* L20: */
	    }
	}
    } else {
	if (upper) {

/*           Compute U*A*U' */

	    i__1 = *n;
	    for (k = 1; k <= i__1; ++k) {

/*              Update the upper triangle of A(1:k,1:k) */

		akk = a[k + k * a_dim1];
		bkk = b[k + k * b_dim1];
		i__2 = k - 1;
		dtrmv_(uplo, "No transpose", "Non-unit", &i__2, &b[b_offset],
			ldb, &a[k * a_dim1 + 1], &c__1);
		ct = akk * .5;
		i__2 = k - 1;
		daxpy_(&i__2, &ct, &b[k * b_dim1 + 1], &c__1, &a[k * a_dim1 +
			1], &c__1);
		i__2 = k - 1;
		dsyr2_(uplo, &i__2, &c_b27, &a[k * a_dim1 + 1], &c__1, &b[k *
			b_dim1 + 1], &c__1, &a[a_offset], lda);
		i__2 = k - 1;
		daxpy_(&i__2, &ct, &b[k * b_dim1 + 1], &c__1, &a[k * a_dim1 +
			1], &c__1);
		i__2 = k - 1;
		dscal_(&i__2, &bkk, &a[k * a_dim1 + 1], &c__1);
/* Computing 2nd power */
		d__1 = bkk;
		a[k + k * a_dim1] = akk * (d__1 * d__1);
/* L30: */
	    }
	} else {

/*           Compute L'*A*L */

	    i__1 = *n;
	    for (k = 1; k <= i__1; ++k) {

/*              Update the lower triangle of A(1:k,1:k) */

		akk = a[k + k * a_dim1];
		bkk = b[k + k * b_dim1];
		i__2 = k - 1;
		dtrmv_(uplo, "Transpose", "Non-unit", &i__2, &b[b_offset],
			ldb, &a[k + a_dim1], lda);
		ct = akk * .5;
		i__2 = k - 1;
		daxpy_(&i__2, &ct, &b[k + b_dim1], ldb, &a[k + a_dim1], lda);
		i__2 = k - 1;
		dsyr2_(uplo, &i__2, &c_b27, &a[k + a_dim1], lda, &b[k +
			b_dim1], ldb, &a[a_offset], lda);
		i__2 = k - 1;
		daxpy_(&i__2, &ct, &b[k + b_dim1], ldb, &a[k + a_dim1], lda);
		i__2 = k - 1;
		dscal_(&i__2, &bkk, &a[k + a_dim1], lda);
/* Computing 2nd power */
		d__1 = bkk;
		a[k + k * a_dim1] = akk * (d__1 * d__1);
/* L40: */
	    }
	}
    }
    return 0;

/*     End of DSYGS2 */

} /* dsygs2_ */

/* Subroutine */ int dsygst_(integer *itype, const char *uplo, integer *n,
	double *a, integer *lda, double *b, integer *ldb, integer *
	info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static double c_b14 = 1.;
	static double c_b16 = -.5;
	static double c_b19 = -1.;
	static double c_b52 = .5;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2, i__3;

    /* Local variables */
    integer k, kb, nb;
    bool upper;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSYGST reduces a real symmetric-definite generalized eigenproblem */
/*  to standard form. */

/*  If ITYPE = 1, the problem is A*x = lambda*B*x, */
/*  and A is overwritten by inv(U**T)*A*inv(U) or inv(L)*A*inv(L**T) */

/*  If ITYPE = 2 or 3, the problem is A*B*x = lambda*x or */
/*  B*A*x = lambda*x, and A is overwritten by U*A*U**T or L**T*A*L. */

/*  B must have been previously factorized as U**T*U or L*L**T by DPOTRF. */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          = 1: compute inv(U**T)*A*inv(U) or inv(L)*A*inv(L**T); */
/*          = 2 or 3: compute U*A*U**T or L**T*A*L. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored and B is factored as */
/*                  U**T*U; */
/*          = 'L':  Lower triangle of A is stored and B is factored as */
/*                  L*L**T. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the leading */
/*          N-by-N upper triangular part of A contains the upper */
/*          triangular part of the matrix A, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading N-by-N lower triangular part of A contains the lower */
/*          triangular part of the matrix A, and the strictly upper */
/*          triangular part of A is not referenced. */

/*          On exit, if INFO = 0, the transformed matrix, stored in the */
/*          same format as A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,N) */
/*          The triangular factor from the Cholesky factorization of B, */
/*          as returned by DPOTRF. */

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

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    if (*itype < 1 || *itype > 3) {
	*info = -1;
    } else if (! upper && ! lsame_(uplo, "L")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -7;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYGST", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Determine the block size for this environment. */

    nb = ilaenv_(&c__1, "DSYGST", uplo, n, &c_n1, &c_n1, &c_n1);

    if (nb <= 1 || nb >= *n) {

/*        Use unblocked code */

	dsygs2_(itype, uplo, n, &a[a_offset], lda, &b[b_offset], ldb, info);
    } else {

/*        Use blocked code */

	if (*itype == 1) {
	    if (upper) {

/*              Compute inv(U')*A*inv(U) */

		i__1 = *n;
		i__2 = nb;
		for (k = 1; i__2 < 0 ? k >= i__1 : k <= i__1; k += i__2) {
/* Computing MIN */
		    i__3 = *n - k + 1;
		    kb = std::min(i__3,nb);

/*                 Update the upper triangle of A(k:n,k:n) */

		    dsygs2_(itype, uplo, &kb, &a[k + k * a_dim1], lda, &b[k +
			    k * b_dim1], ldb, info);
		    if (k + kb <= *n) {
			i__3 = *n - k - kb + 1;
			dtrsm_("Left", uplo, "Transpose", "Non-unit", &kb, &
				i__3, &c_b14, &b[k + k * b_dim1], ldb, &a[k +
				(k + kb) * a_dim1], lda);
			i__3 = *n - k - kb + 1;
			dsymm_("Left", uplo, &kb, &i__3, &c_b16, &a[k + k *
				a_dim1], lda, &b[k + (k + kb) * b_dim1], ldb,
				&c_b14, &a[k + (k + kb) * a_dim1], lda);
			i__3 = *n - k - kb + 1;
			dsyr2k_(uplo, "Transpose", &i__3, &kb, &c_b19, &a[k +
				(k + kb) * a_dim1], lda, &b[k + (k + kb) *
				b_dim1], ldb, &c_b14, &a[k + kb + (k + kb) *
				a_dim1], lda);
			i__3 = *n - k - kb + 1;
			dsymm_("Left", uplo, &kb, &i__3, &c_b16, &a[k + k *
				a_dim1], lda, &b[k + (k + kb) * b_dim1], ldb,
				&c_b14, &a[k + (k + kb) * a_dim1], lda);
			i__3 = *n - k - kb + 1;
			dtrsm_("Right", uplo, "No transpose", "Non-unit", &kb,
				 &i__3, &c_b14, &b[k + kb + (k + kb) * b_dim1]
, ldb, &a[k + (k + kb) * a_dim1], lda);
		    }
/* L10: */
		}
	    } else {

/*              Compute inv(L)*A*inv(L') */

		i__2 = *n;
		i__1 = nb;
		for (k = 1; i__1 < 0 ? k >= i__2 : k <= i__2; k += i__1) {
/* Computing MIN */
		    i__3 = *n - k + 1;
		    kb = std::min(i__3,nb);

/*                 Update the lower triangle of A(k:n,k:n) */

		    dsygs2_(itype, uplo, &kb, &a[k + k * a_dim1], lda, &b[k +
			    k * b_dim1], ldb, info);
		    if (k + kb <= *n) {
			i__3 = *n - k - kb + 1;
			dtrsm_("Right", uplo, "Transpose", "Non-unit", &i__3,
				&kb, &c_b14, &b[k + k * b_dim1], ldb, &a[k +
				kb + k * a_dim1], lda);
			i__3 = *n - k - kb + 1;
			dsymm_("Right", uplo, &i__3, &kb, &c_b16, &a[k + k *
				a_dim1], lda, &b[k + kb + k * b_dim1], ldb, &
				c_b14, &a[k + kb + k * a_dim1], lda);
			i__3 = *n - k - kb + 1;
			dsyr2k_(uplo, "No transpose", &i__3, &kb, &c_b19, &a[
				k + kb + k * a_dim1], lda, &b[k + kb + k *
				b_dim1], ldb, &c_b14, &a[k + kb + (k + kb) *
				a_dim1], lda);
			i__3 = *n - k - kb + 1;
			dsymm_("Right", uplo, &i__3, &kb, &c_b16, &a[k + k *
				a_dim1], lda, &b[k + kb + k * b_dim1], ldb, &
				c_b14, &a[k + kb + k * a_dim1], lda);
			i__3 = *n - k - kb + 1;
			dtrsm_("Left", uplo, "No transpose", "Non-unit", &
				i__3, &kb, &c_b14, &b[k + kb + (k + kb) *
				b_dim1], ldb, &a[k + kb + k * a_dim1], lda);
		    }
/* L20: */
		}
	    }
	} else {
	    if (upper) {

/*              Compute U*A*U' */

		i__1 = *n;
		i__2 = nb;
		for (k = 1; i__2 < 0 ? k >= i__1 : k <= i__1; k += i__2) {
/* Computing MIN */
		    i__3 = *n - k + 1;
		    kb = std::min(i__3,nb);

/*                 Update the upper triangle of A(1:k+kb-1,1:k+kb-1) */

		    i__3 = k - 1;
		    dtrmm_("Left", uplo, "No transpose", "Non-unit", &i__3, &
			    kb, &c_b14, &b[b_offset], ldb, &a[k * a_dim1 + 1],
			     lda)
			    ;
		    i__3 = k - 1;
		    dsymm_("Right", uplo, &i__3, &kb, &c_b52, &a[k + k *
			    a_dim1], lda, &b[k * b_dim1 + 1], ldb, &c_b14, &a[
			    k * a_dim1 + 1], lda);
		    i__3 = k - 1;
		    dsyr2k_(uplo, "No transpose", &i__3, &kb, &c_b14, &a[k *
			    a_dim1 + 1], lda, &b[k * b_dim1 + 1], ldb, &c_b14,
			     &a[a_offset], lda);
		    i__3 = k - 1;
		    dsymm_("Right", uplo, &i__3, &kb, &c_b52, &a[k + k *
			    a_dim1], lda, &b[k * b_dim1 + 1], ldb, &c_b14, &a[
			    k * a_dim1 + 1], lda);
		    i__3 = k - 1;
		    dtrmm_("Right", uplo, "Transpose", "Non-unit", &i__3, &kb,
			     &c_b14, &b[k + k * b_dim1], ldb, &a[k * a_dim1 +
			    1], lda);
		    dsygs2_(itype, uplo, &kb, &a[k + k * a_dim1], lda, &b[k +
			    k * b_dim1], ldb, info);
/* L30: */
		}
	    } else {

/*              Compute L'*A*L */

		i__2 = *n;
		i__1 = nb;
		for (k = 1; i__1 < 0 ? k >= i__2 : k <= i__2; k += i__1) {
/* Computing MIN */
		    i__3 = *n - k + 1;
		    kb = std::min(i__3,nb);

/*                 Update the lower triangle of A(1:k+kb-1,1:k+kb-1) */

		    i__3 = k - 1;
		    dtrmm_("Right", uplo, "No transpose", "Non-unit", &kb, &
			    i__3, &c_b14, &b[b_offset], ldb, &a[k + a_dim1],
			    lda);
		    i__3 = k - 1;
		    dsymm_("Left", uplo, &kb, &i__3, &c_b52, &a[k + k *
			    a_dim1], lda, &b[k + b_dim1], ldb, &c_b14, &a[k +
			    a_dim1], lda);
		    i__3 = k - 1;
		    dsyr2k_(uplo, "Transpose", &i__3, &kb, &c_b14, &a[k +
			    a_dim1], lda, &b[k + b_dim1], ldb, &c_b14, &a[
			    a_offset], lda);
		    i__3 = k - 1;
		    dsymm_("Left", uplo, &kb, &i__3, &c_b52, &a[k + k *
			    a_dim1], lda, &b[k + b_dim1], ldb, &c_b14, &a[k +
			    a_dim1], lda);
		    i__3 = k - 1;
		    dtrmm_("Left", uplo, "Transpose", "Non-unit", &kb, &i__3,
			    &c_b14, &b[k + k * b_dim1], ldb, &a[k + a_dim1],
			    lda);
		    dsygs2_(itype, uplo, &kb, &a[k + k * a_dim1], lda, &b[k +
			    k * b_dim1], ldb, info);
/* L40: */
		}
	    }
	}
    }
    return 0;

/*     End of DSYGST */

} /* dsygst_ */

/* Subroutine */ int dsygv_(integer *itype, const char *jobz, const char *uplo, integer *
	n, double *a, integer *lda, double *b, integer *ldb,
	double *w, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static double c_b16 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;

    /* Local variables */
    integer nb, neig;
    char trans[1];
    bool upper;
    bool wantz;
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

/*  DSYGV computes all the eigenvalues, and optionally, the eigenvectors */
/*  of a real generalized symmetric-definite eigenproblem, of the form */
/*  A*x=(lambda)*B*x,  A*Bx=(lambda)*x,  or B*A*x=(lambda)*x. */
/*  Here A and B are assumed to be symmetric and B is also */
/*  positive definite. */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          Specifies the problem type to be solved: */
/*          = 1:  A*x = (lambda)*B*x */
/*          = 2:  A*B*x = (lambda)*x */
/*          = 3:  B*A*x = (lambda)*x */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangles of A and B are stored; */
/*          = 'L':  Lower triangles of A and B are stored. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the */
/*          leading N-by-N upper triangular part of A contains the */
/*          upper triangular part of the matrix A.  If UPLO = 'L', */
/*          the leading N-by-N lower triangular part of A contains */
/*          the lower triangular part of the matrix A. */

/*          On exit, if JOBZ = 'V', then if INFO = 0, A contains the */
/*          matrix Z of eigenvectors.  The eigenvectors are normalized */
/*          as follows: */
/*          if ITYPE = 1 or 2, Z**T*B*Z = I; */
/*          if ITYPE = 3, Z**T*inv(B)*Z = I. */
/*          If JOBZ = 'N', then on exit the upper triangle (if UPLO='U') */
/*          or the lower triangle (if UPLO='L') of A, including the */
/*          diagonal, is destroyed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB, N) */
/*          On entry, the symmetric positive definite matrix B. */
/*          If UPLO = 'U', the leading N-by-N upper triangular part of B */
/*          contains the upper triangular part of the matrix B. */
/*          If UPLO = 'L', the leading N-by-N lower triangular part of B */
/*          contains the lower triangular part of the matrix B. */

/*          On exit, if INFO <= N, the part of B containing the matrix is */
/*          overwritten by the triangular factor U or L from the Cholesky */
/*          factorization B = U**T*U or B = L*L**T. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, the eigenvalues in ascending order. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= max(1,3*N-1). */
/*          For optimal efficiency, LWORK >= (NB+2)*N, */
/*          where NB is the blocksize for DSYTRD returned by ILAENV. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  DPOTRF or DSYEV returned an error code: */
/*             <= N:  if INFO = i, DSYEV failed to converge; */
/*                    i off-diagonal elements of an intermediate */
/*                    tridiagonal form did not converge to zero; */
/*             > N:   if INFO = N + i, for 1 <= i <= N, then the leading */
/*                    minor of order i of B is not positive definite. */
/*                    The factorization of B could not be completed and */
/*                    no eigenvalues or eigenvectors were computed. */

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
    --w;
    --work;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    upper = lsame_(uplo, "U");
    lquery = *lwork == -1;

    *info = 0;
    if (*itype < 1 || *itype > 3) {
	*info = -1;
    } else if (! (wantz || lsame_(jobz, "N"))) {
	*info = -2;
    } else if (! (upper || lsame_(uplo, "L"))) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -6;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -8;
    }

    if (*info == 0) {
/* Computing MAX */
	i__1 = 1, i__2 = *n * 3 - 1;
	lwkmin = std::max(i__1,i__2);
	nb = ilaenv_(&c__1, "DSYTRD", uplo, n, &c_n1, &c_n1, &c_n1);
/* Computing MAX */
	i__1 = lwkmin, i__2 = (nb + 2) * *n;
	lwkopt = std::max(i__1,i__2);
	work[1] = (double) lwkopt;

	if (*lwork < lwkmin && ! lquery) {
	    *info = -11;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYGV ", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Form a Cholesky factorization of B. */

    dpotrf_(uplo, n, &b[b_offset], ldb, info);
    if (*info != 0) {
	*info = *n + *info;
	return 0;
    }

/*     Transform problem to standard eigenvalue problem and solve. */

    dsygst_(itype, uplo, n, &a[a_offset], lda, &b[b_offset], ldb, info);
    dsyev_(jobz, uplo, n, &a[a_offset], lda, &w[1], &work[1], lwork, info);

    if (wantz) {

/*        Backtransform eigenvectors to the original problem. */

	neig = *n;
	if (*info > 0) {
	    neig = *info - 1;
	}
	if (*itype == 1 || *itype == 2) {

/*           For A*x=(lambda)*B*x and A*B*x=(lambda)*x; */
/*           backtransform eigenvectors: x = inv(L)'*y or inv(U)*y */

	    if (upper) {
		*(unsigned char *)trans = 'N';
	    } else {
		*(unsigned char *)trans = 'T';
	    }

	    dtrsm_("Left", uplo, trans, "Non-unit", n, &neig, &c_b16, &b[
		    b_offset], ldb, &a[a_offset], lda);

	} else if (*itype == 3) {

/*           For B*A*x=(lambda)*x; */
/*           backtransform eigenvectors: x = L*y or U'*y */

	    if (upper) {
		*(unsigned char *)trans = 'T';
	    } else {
		*(unsigned char *)trans = 'N';
	    }

	    dtrmm_("Left", uplo, trans, "Non-unit", n, &neig, &c_b16, &b[
		    b_offset], ldb, &a[a_offset], lda);
	}
    }

    work[1] = (double) lwkopt;
    return 0;

/*     End of DSYGV */

} /* dsygv_ */

/* Subroutine */ int dsygvd_(integer *itype, const char *jobz, const char *uplo, integer *
	n, double *a, integer *lda, double *b, integer *ldb,
	double *w, double *work, integer *lwork, integer *iwork,
	integer *liwork, integer *info)
{
	/* Table of constant values */
	static double c_b11 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1;
    double d__1, d__2;

    /* Local variables */
    integer lopt;
    integer lwmin;
    char trans[1];
    integer liopt;
    bool upper, wantz;
    integer liwmin;
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

/*  DSYGVD computes all the eigenvalues, and optionally, the eigenvectors */
/*  of a real generalized symmetric-definite eigenproblem, of the form */
/*  A*x=(lambda)*B*x,  A*Bx=(lambda)*x,  or B*A*x=(lambda)*x.  Here A and */
/*  B are assumed to be symmetric and B is also positive definite. */
/*  If eigenvectors are desired, it uses a divide and conquer algorithm. */

/*  The divide and conquer algorithm makes very mild assumptions about */
/*  floating point arithmetic. It will work on machines with a guard */
/*  digit in add/subtract, or on those binary machines without guard */
/*  digits which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or */
/*  Cray-2. It could conceivably fail on hexadecimal or decimal machines */
/*  without guard digits, but we know of none. */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          Specifies the problem type to be solved: */
/*          = 1:  A*x = (lambda)*B*x */
/*          = 2:  A*B*x = (lambda)*x */
/*          = 3:  B*A*x = (lambda)*x */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangles of A and B are stored; */
/*          = 'L':  Lower triangles of A and B are stored. */

/*  N       (input) INTEGER */
/*          The order of the matrices A and B.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the */
/*          leading N-by-N upper triangular part of A contains the */
/*          upper triangular part of the matrix A.  If UPLO = 'L', */
/*          the leading N-by-N lower triangular part of A contains */
/*          the lower triangular part of the matrix A. */

/*          On exit, if JOBZ = 'V', then if INFO = 0, A contains the */
/*          matrix Z of eigenvectors.  The eigenvectors are normalized */
/*          as follows: */
/*          if ITYPE = 1 or 2, Z**T*B*Z = I; */
/*          if ITYPE = 3, Z**T*inv(B)*Z = I. */
/*          If JOBZ = 'N', then on exit the upper triangle (if UPLO='U') */
/*          or the lower triangle (if UPLO='L') of A, including the */
/*          diagonal, is destroyed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB, N) */
/*          On entry, the symmetric matrix B.  If UPLO = 'U', the */
/*          leading N-by-N upper triangular part of B contains the */
/*          upper triangular part of the matrix B.  If UPLO = 'L', */
/*          the leading N-by-N lower triangular part of B contains */
/*          the lower triangular part of the matrix B. */

/*          On exit, if INFO <= N, the part of B containing the matrix is */
/*          overwritten by the triangular factor U or L from the Cholesky */
/*          factorization B = U**T*U or B = L*L**T. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, the eigenvalues in ascending order. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          If N <= 1,               LWORK >= 1. */
/*          If JOBZ = 'N' and N > 1, LWORK >= 2*N+1. */
/*          If JOBZ = 'V' and N > 1, LWORK >= 1 + 6*N + 2*N**2. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal sizes of the WORK and IWORK */
/*          arrays, returns these values as the first entries of the WORK */
/*          and IWORK arrays, and no error message related to LWORK or */
/*          LIWORK is issued by XERBLA. */

/*  IWORK   (workspace/output) INTEGER array, dimension (MAX(1,LIWORK)) */
/*          On exit, if INFO = 0, IWORK(1) returns the optimal LIWORK. */

/*  LIWORK  (input) INTEGER */
/*          The dimension of the array IWORK. */
/*          If N <= 1,                LIWORK >= 1. */
/*          If JOBZ  = 'N' and N > 1, LIWORK >= 1. */
/*          If JOBZ  = 'V' and N > 1, LIWORK >= 3 + 5*N. */

/*          If LIWORK = -1, then a workspace query is assumed; the */
/*          routine only calculates the optimal sizes of the WORK and */
/*          IWORK arrays, returns these values as the first entries of */
/*          the WORK and IWORK arrays, and no error message related to */
/*          LWORK or LIWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  DPOTRF or DSYEVD returned an error code: */
/*             <= N:  if INFO = i and JOBZ = 'N', then the algorithm */
/*                    failed to converge; i off-diagonal elements of an */
/*                    intermediate tridiagonal form did not converge to */
/*                    zero; */
/*                    if INFO = i and JOBZ = 'V', then the algorithm */
/*                    failed to compute an eigenvalue while working on */
/*                    the submatrix lying in rows and columns INFO/(N+1) */
/*                    through mod(INFO,N+1); */
/*             > N:   if INFO = N + i, for 1 <= i <= N, then the leading */
/*                    minor of order i of B is not positive definite. */
/*                    The factorization of B could not be completed and */
/*                    no eigenvalues or eigenvectors were computed. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Mark Fahey, Department of Mathematics, Univ. of Kentucky, USA */

/*  Modified so that no backsubstitution is performed if DSYEVD fails to */
/*  converge (NEIG in old code could be greater than N causing out of */
/*  bounds reference to A - reported by Ralf Meyer).  Also corrected the */
/*  description of INFO and the test on ITYPE. Sven, 16 Feb 05. */
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
    --w;
    --work;
    --iwork;

    /* Function Body */
    wantz = lsame_(jobz, "V");
    upper = lsame_(uplo, "U");
    lquery = *lwork == -1 || *liwork == -1;

    *info = 0;
    if (*n <= 1) {
	liwmin = 1;
	lwmin = 1;
    } else if (wantz) {
	liwmin = *n * 5 + 3;
/* Computing 2nd power */
	i__1 = *n;
	lwmin = *n * 6 + 1 + (i__1 * i__1 << 1);
    } else {
	liwmin = 1;
	lwmin = (*n << 1) + 1;
    }
    lopt = lwmin;
    liopt = liwmin;
    if (*itype < 1 || *itype > 3) {
	*info = -1;
    } else if (! (wantz || lsame_(jobz, "N"))) {
	*info = -2;
    } else if (! (upper || lsame_(uplo, "L"))) {
	*info = -3;
    } else if (*n < 0) {
	*info = -4;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -6;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -8;
    }

    if (*info == 0) {
	work[1] = (double) lopt;
	iwork[1] = liopt;

	if (*lwork < lwmin && ! lquery) {
	    *info = -11;
	} else if (*liwork < liwmin && ! lquery) {
	    *info = -13;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYGVD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Form a Cholesky factorization of B. */

    dpotrf_(uplo, n, &b[b_offset], ldb, info);
    if (*info != 0) {
	*info = *n + *info;
	return 0;
    }

/*     Transform problem to standard eigenvalue problem and solve. */

    dsygst_(itype, uplo, n, &a[a_offset], lda, &b[b_offset], ldb, info);
    dsyevd_(jobz, uplo, n, &a[a_offset], lda, &w[1], &work[1], lwork, &iwork[
	    1], liwork, info);
/* Computing MAX */
    d__1 = (double) lopt;
    lopt = (integer) std::max(d__1,work[1]);
/* Computing MAX */
    d__1 = (double) liopt, d__2 = (double) iwork[1];
    liopt = (integer) std::max(d__1,d__2);

    if (wantz && *info == 0) {

/*        Backtransform eigenvectors to the original problem. */

	if (*itype == 1 || *itype == 2) {

/*           For A*x=(lambda)*B*x and A*B*x=(lambda)*x; */
/*           backtransform eigenvectors: x = inv(L)'*y or inv(U)*y */

	    if (upper) {
		*(unsigned char *)trans = 'N';
	    } else {
		*(unsigned char *)trans = 'T';
	    }

	    dtrsm_("Left", uplo, trans, "Non-unit", n, n, &c_b11, &b[b_offset]
, ldb, &a[a_offset], lda);

	} else if (*itype == 3) {

/*           For B*A*x=(lambda)*x; */
/*           backtransform eigenvectors: x = L*y or U'*y */

	    if (upper) {
		*(unsigned char *)trans = 'T';
	    } else {
		*(unsigned char *)trans = 'N';
	    }

	    dtrmm_("Left", uplo, trans, "Non-unit", n, n, &c_b11, &b[b_offset]
, ldb, &a[a_offset], lda);
	}
    }

    work[1] = (double) lopt;
    iwork[1] = liopt;

    return 0;

/*     End of DSYGVD */

} /* dsygvd_ */

/* Subroutine */ int dsygvx_(integer *itype, const char *jobz, const char *range, const char *
	uplo, integer *n, double *a, integer *lda, double *b, integer
	*ldb, double *vl, double *vu, integer *il, integer *iu,
	double *abstol, integer *m, double *w, double *z__,
	integer *ldz, double *work, integer *lwork, integer *iwork,
	integer *ifail, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static double c_b19 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, z_dim1, z_offset, i__1, i__2;

    /* Local variables */
    integer nb;
    char trans[1];
    bool upper, wantz, alleig, indeig, valeig;
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

/*  DSYGVX computes selected eigenvalues, and optionally, eigenvectors */
/*  of a real generalized symmetric-definite eigenproblem, of the form */
/*  A*x=(lambda)*B*x,  A*Bx=(lambda)*x,  or B*A*x=(lambda)*x.  Here A */
/*  and B are assumed to be symmetric and B is also positive definite. */
/*  Eigenvalues and eigenvectors can be selected by specifying either a */
/*  range of values or a range of indices for the desired eigenvalues. */

/*  Arguments */
/*  ========= */

/*  ITYPE   (input) INTEGER */
/*          Specifies the problem type to be solved: */
/*          = 1:  A*x = (lambda)*B*x */
/*          = 2:  A*B*x = (lambda)*x */
/*          = 3:  B*A*x = (lambda)*x */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  RANGE   (input) CHARACTER*1 */
/*          = 'A': all eigenvalues will be found. */
/*          = 'V': all eigenvalues in the half-open interval (VL,VU] */
/*                 will be found. */
/*          = 'I': the IL-th through IU-th eigenvalues will be found. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A and B are stored; */
/*          = 'L':  Lower triangle of A and B are stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix pencil (A,B).  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the */
/*          leading N-by-N upper triangular part of A contains the */
/*          upper triangular part of the matrix A.  If UPLO = 'L', */
/*          the leading N-by-N lower triangular part of A contains */
/*          the lower triangular part of the matrix A. */

/*          On exit, the lower triangle (if UPLO='L') or the upper */
/*          triangle (if UPLO='U') of A, including the diagonal, is */
/*          destroyed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the symmetric matrix B.  If UPLO = 'U', the */
/*          leading N-by-N upper triangular part of B contains the */
/*          upper triangular part of the matrix B.  If UPLO = 'L', */
/*          the leading N-by-N lower triangular part of B contains */
/*          the lower triangular part of the matrix B. */

/*          On exit, if INFO <= N, the part of B containing the matrix is */
/*          overwritten by the triangular factor U or L from the Cholesky */
/*          factorization B = U**T*U or B = L*L**T. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  VL      (input) DOUBLE PRECISION */
/*  VU      (input) DOUBLE PRECISION */
/*          If RANGE='V', the lower and upper bounds of the interval to */
/*          be searched for eigenvalues. VL < VU. */
/*          Not referenced if RANGE = 'A' or 'I'. */

/*  IL      (input) INTEGER */
/*  IU      (input) INTEGER */
/*          If RANGE='I', the indices (in ascending order) of the */
/*          smallest and largest eigenvalues to be returned. */
/*          1 <= IL <= IU <= N, if N > 0; IL = 1 and IU = 0 if N = 0. */
/*          Not referenced if RANGE = 'A' or 'V'. */

/*  ABSTOL  (input) DOUBLE PRECISION */
/*          The absolute error tolerance for the eigenvalues. */
/*          An approximate eigenvalue is accepted as converged */
/*          when it is determined to lie in an interval [a,b] */
/*          of width less than or equal to */

/*                  ABSTOL + EPS *   max( |a|,|b| ) , */

/*          where EPS is the machine precision.  If ABSTOL is less than */
/*          or equal to zero, then  EPS*|T|  will be used in its place, */
/*          where |T| is the 1-norm of the tridiagonal matrix obtained */
/*          by reducing A to tridiagonal form. */

/*          Eigenvalues will be computed most accurately when ABSTOL is */
/*          set to twice the underflow threshold 2*DLAMCH('S'), not zero. */
/*          If this routine returns with INFO>0, indicating that some */
/*          eigenvectors did not converge, try setting ABSTOL to */
/*          2*DLAMCH('S'). */

/*  M       (output) INTEGER */
/*          The total number of eigenvalues found.  0 <= M <= N. */
/*          If RANGE = 'A', M = N, and if RANGE = 'I', M = IU-IL+1. */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          On normal exit, the first M elements contain the selected */
/*          eigenvalues in ascending order. */

/*  Z       (output) DOUBLE PRECISION array, dimension (LDZ, max(1,M)) */
/*          If JOBZ = 'N', then Z is not referenced. */
/*          If JOBZ = 'V', then if INFO = 0, the first M columns of Z */
/*          contain the orthonormal eigenvectors of the matrix A */
/*          corresponding to the selected eigenvalues, with the i-th */
/*          column of Z holding the eigenvector associated with W(i). */
/*          The eigenvectors are normalized as follows: */
/*          if ITYPE = 1 or 2, Z**T*B*Z = I; */
/*          if ITYPE = 3, Z**T*inv(B)*Z = I. */

/*          If an eigenvector fails to converge, then that column of Z */
/*          contains the latest approximation to the eigenvector, and the */
/*          index of the eigenvector is returned in IFAIL. */
/*          Note: the user must ensure that at least max(1,M) columns are */
/*          supplied in the array Z; if RANGE = 'V', the exact value of M */
/*          is not known in advance and an upper bound must be used. */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDZ >= 1, and if */
/*          JOBZ = 'V', LDZ >= max(1,N). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= max(1,8*N). */
/*          For optimal efficiency, LWORK >= (NB+3)*N, */
/*          where NB is the blocksize for DSYTRD returned by ILAENV. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  IWORK   (workspace) INTEGER array, dimension (5*N) */

/*  IFAIL   (output) INTEGER array, dimension (N) */
/*          If JOBZ = 'V', then if INFO = 0, the first M elements of */
/*          IFAIL are zero.  If INFO > 0, then IFAIL contains the */
/*          indices of the eigenvectors that failed to converge. */
/*          If JOBZ = 'N', then IFAIL is not referenced. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  DPOTRF or DSYEVX returned an error code: */
/*             <= N:  if INFO = i, DSYEVX failed to converge; */
/*                    i eigenvectors failed to converge.  Their indices */
/*                    are stored in array IFAIL. */
/*             > N:   if INFO = N + i, for 1 <= i <= N, then the leading */
/*                    minor of order i of B is not positive definite. */
/*                    The factorization of B could not be completed and */
/*                    no eigenvalues or eigenvectors were computed. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Mark Fahey, Department of Mathematics, Univ. of Kentucky, USA */

/* ===================================================================== */

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
    --w;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;
    --iwork;
    --ifail;

    /* Function Body */
    upper = lsame_(uplo, "U");
    wantz = lsame_(jobz, "V");
    alleig = lsame_(range, "A");
    valeig = lsame_(range, "V");
    indeig = lsame_(range, "I");
    lquery = *lwork == -1;

    *info = 0;
    if (*itype < 1 || *itype > 3) {
	*info = -1;
    } else if (! (wantz || lsame_(jobz, "N"))) {
	*info = -2;
    } else if (! (alleig || valeig || indeig)) {
	*info = -3;
    } else if (! (upper || lsame_(uplo, "L"))) {
	*info = -4;
    } else if (*n < 0) {
	*info = -5;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -9;
    } else {
	if (valeig) {
	    if (*n > 0 && *vu <= *vl) {
		*info = -11;
	    }
	} else if (indeig) {
	    if (*il < 1 || *il > std::max(1_integer,*n)) {
		*info = -12;
	    } else if (*iu < std::min(*n,*il) || *iu > *n) {
		*info = -13;
	    }
	}
    }
    if (*info == 0) {
	if (*ldz < 1 || wantz && *ldz < *n) {
	    *info = -18;
	}
    }

    if (*info == 0) {
/* Computing MAX */
	i__1 = 1, i__2 = *n << 3;
	lwkmin = std::max(i__1,i__2);
	nb = ilaenv_(&c__1, "DSYTRD", uplo, n, &c_n1, &c_n1, &c_n1);
/* Computing MAX */
	i__1 = lwkmin, i__2 = (nb + 3) * *n;
	lwkopt = std::max(i__1,i__2);
	work[1] = (double) lwkopt;

	if (*lwork < lwkmin && ! lquery) {
	    *info = -20;
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYGVX", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    *m = 0;
    if (*n == 0) {
	return 0;
    }

/*     Form a Cholesky factorization of B. */

    dpotrf_(uplo, n, &b[b_offset], ldb, info);
    if (*info != 0) {
	*info = *n + *info;
	return 0;
    }

/*     Transform problem to standard eigenvalue problem and solve. */

    dsygst_(itype, uplo, n, &a[a_offset], lda, &b[b_offset], ldb, info);
    dsyevx_(jobz, range, uplo, n, &a[a_offset], lda, vl, vu, il, iu, abstol,
	    m, &w[1], &z__[z_offset], ldz, &work[1], lwork, &iwork[1], &ifail[
	    1], info);

    if (wantz) {

/*        Backtransform eigenvectors to the original problem. */

	if (*info > 0) {
	    *m = *info - 1;
	}
	if (*itype == 1 || *itype == 2) {

/*           For A*x=(lambda)*B*x and A*B*x=(lambda)*x; */
/*           backtransform eigenvectors: x = inv(L)'*y or inv(U)*y */

	    if (upper) {
		*(unsigned char *)trans = 'N';
	    } else {
		*(unsigned char *)trans = 'T';
	    }

	    dtrsm_("Left", uplo, trans, "Non-unit", n, m, &c_b19, &b[b_offset]
, ldb, &z__[z_offset], ldz);

	} else if (*itype == 3) {

/*           For B*A*x=(lambda)*x; */
/*           backtransform eigenvectors: x = L*y or U'*y */

	    if (upper) {
		*(unsigned char *)trans = 'T';
	    } else {
		*(unsigned char *)trans = 'N';
	    }

	    dtrmm_("Left", uplo, trans, "Non-unit", n, m, &c_b19, &b[b_offset]
, ldb, &z__[z_offset], ldz);
	}
    }

/*     Set WORK(1) to optimal workspace size. */

    work[1] = (double) lwkopt;

    return 0;

/*     End of DSYGVX */

} /* dsygvx_ */

/* Subroutine */ int dsyrfs_(const char *uplo, integer *n, integer *nrhs,
	double *a, integer *lda, double *af, integer *ldaf, integer *
	ipiv, double *b, integer *ldb, double *x, integer *ldx,
	double *ferr, double *berr, double *work, integer *iwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b12 = -1.;
	static double c_b14 = 1.;

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
    bool upper;
    double safmin;
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

/*  DSYRFS improves the computed solution to a system of linear */
/*  equations when the coefficient matrix is symmetric indefinite, and */
/*  provides error bounds and backward error estimates for the solution. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X.  NRHS >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The symmetric matrix A.  If UPLO = 'U', the leading N-by-N */
/*          upper triangular part of A contains the upper triangular part */
/*          of the matrix A, and the strictly lower triangular part of A */
/*          is not referenced.  If UPLO = 'L', the leading N-by-N lower */
/*          triangular part of A contains the lower triangular part of */
/*          the matrix A, and the strictly upper triangular part of A is */
/*          not referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  AF      (input) DOUBLE PRECISION array, dimension (LDAF,N) */
/*          The factored form of the matrix A.  AF contains the block */
/*          diagonal matrix D and the multipliers used to obtain the */
/*          factor U or L from the factorization A = U*D*U**T or */
/*          A = L*D*L**T as computed by DSYTRF. */

/*  LDAF    (input) INTEGER */
/*          The leading dimension of the array AF.  LDAF >= max(1,N). */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          Details of the interchanges and the block structure of D */
/*          as determined by DSYTRF. */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          The right hand side matrix B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  X       (input/output) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          On entry, the solution matrix X, as computed by DSYTRS. */
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
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
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
	xerbla_("DSYRFS", &i__1);
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

/*        Compute residual R = B - A * X */

	dcopy_(n, &b[j * b_dim1 + 1], &c__1, &work[*n + 1], &c__1);
	dsymv_(uplo, n, &c_b12, &a[a_offset], lda, &x[j * x_dim1 + 1], &c__1,
		&c_b14, &work[*n + 1], &c__1);

/*        Compute componentwise relative backward error from formula */

/*        max(i) ( abs(R(i)) / ( abs(A)*abs(X) + abs(B) )(i) ) */

/*        where abs(Z) is the componentwise absolute value of the matrix */
/*        or vector Z.  If the i-th component of the denominator is less */
/*        than SAFE2, then SAFE1 is added to the i-th components of the */
/*        numerator and denominator before dividing. */

	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    work[i__] = (d__1 = b[i__ + j * b_dim1], abs(d__1));
/* L30: */
	}

/*        Compute abs(A)*abs(X) + abs(B). */

	if (upper) {
	    i__2 = *n;
	    for (k = 1; k <= i__2; ++k) {
		s = 0.;
		xk = (d__1 = x[k + j * x_dim1], abs(d__1));
		i__3 = k - 1;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    work[i__] += (d__1 = a[i__ + k * a_dim1], abs(d__1)) * xk;
		    s += (d__1 = a[i__ + k * a_dim1], abs(d__1)) * (d__2 = x[
			    i__ + j * x_dim1], abs(d__2));
/* L40: */
		}
		work[k] = work[k] + (d__1 = a[k + k * a_dim1], abs(d__1)) *
			xk + s;
/* L50: */
	    }
	} else {
	    i__2 = *n;
	    for (k = 1; k <= i__2; ++k) {
		s = 0.;
		xk = (d__1 = x[k + j * x_dim1], abs(d__1));
		work[k] += (d__1 = a[k + k * a_dim1], abs(d__1)) * xk;
		i__3 = *n;
		for (i__ = k + 1; i__ <= i__3; ++i__) {
		    work[i__] += (d__1 = a[i__ + k * a_dim1], abs(d__1)) * xk;
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

	    dsytrs_(uplo, n, &c__1, &af[af_offset], ldaf, &ipiv[1], &work[*n
		    + 1], n, info);
	    daxpy_(n, &c_b14, &work[*n + 1], &c__1, &x[j * x_dim1 + 1], &c__1)
		    ;
	    lstres = berr[j];
	    ++count;
	    goto L20;
	}

/*        Bound error from formula */

/*        norm(X - XTRUE) / norm(X) .le. FERR = */
/*        norm( abs(inv(A))* */
/*           ( abs(R) + NZ*EPS*( abs(A)*abs(X)+abs(B) ))) / norm(X) */

/*        where */
/*          norm(Z) is the magnitude of the largest component of Z */
/*          inv(A) is the inverse of A */
/*          abs(Z) is the componentwise absolute value of the matrix or */
/*             vector Z */
/*          NZ is the maximum number of nonzeros in any row of A, plus 1 */
/*          EPS is machine epsilon */

/*        The i-th component of abs(R)+NZ*EPS*(abs(A)*abs(X)+abs(B)) */
/*        is incremented by SAFE1 if the i-th component of */
/*        abs(A)*abs(X) + abs(B) is less than SAFE2. */

/*        Use DLACN2 to estimate the infinity-norm of the matrix */
/*           inv(A) * diag(W), */
/*        where W = abs(R) + NZ*EPS*( abs(A)*abs(X)+abs(B) ))) */

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

/*              Multiply by diag(W)*inv(A'). */

		dsytrs_(uplo, n, &c__1, &af[af_offset], ldaf, &ipiv[1], &work[
			*n + 1], n, info);
		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = work[i__] * work[*n + i__];
/* L110: */
		}
	    } else if (kase == 2) {

/*              Multiply by inv(A)*diag(W). */

		i__2 = *n;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    work[*n + i__] = work[i__] * work[*n + i__];
/* L120: */
		}
		dsytrs_(uplo, n, &c__1, &af[af_offset], ldaf, &ipiv[1], &work[
			*n + 1], n, info);
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

/*     End of DSYRFS */

} /* dsyrfs_ */

#if 0
int dsyrfsx_(const char *uplo, const char *equed, integer *n, integer *nrhs, double *a,
	integer *lda, double *af, integer *ldaf, integer *ipiv, double *s, double *b, integer *ldb,
	double *x, integer *ldx, double *rcond, double *berr, integer *n_err_bnds__,
	double *err_bnds_norm__, double *err_bnds_comp__, integer *nparams, double *params,
	double *work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c_n1 = -1;
	static integer c__0 = 0;
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, b_dim1, b_offset, x_dim1,
	    x_offset, err_bnds_norm_dim1, err_bnds_norm_offset,
	    err_bnds_comp_dim1, err_bnds_comp_offset, i__1;
    double d__1, d__2;

    /* Local variables */
    double illrcond_thresh__, unstable_thresh__, err_lbnd__;
    integer ref_type__, j;
    double rcond_tmp__;
    integer prec_type__;
	double cwise_wrong__;
	char norm[1];
    bool ignore_cwise__;
    double anorm;
    bool rcequ;
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

/*     DSYRFSX improves the computed solution to a system of linear */
/*     equations when the coefficient matrix is symmetric indefinite, and */
/*     provides error bounds and backward error estimates for the */
/*     solution.  In addition to normwise error bound, the code provides */
/*     maximum componentwise error bound if possible.  See comments for */
/*     ERR_BNDS_NORM and ERR_BNDS_COMP for details of the error bounds. */

/*     The original system of linear equations may have been equilibrated */
/*     before calling this routine, as described by arguments EQUED and S */
/*     below. In this case, the solution and error bounds returned are */
/*     for the original unequilibrated system. */

/*     Arguments */
/*     ========= */

/*     Some optional parameters are bundled in the PARAMS array.  These */
/*     settings determine how refinement is performed, but often the */
/*     defaults are acceptable.  If the defaults are acceptable, users */
/*     can pass NPARAMS = 0 which prevents the source code from accessing */
/*     the PARAMS argument. */

/*     UPLO    (input) CHARACTER*1 */
/*       = 'U':  Upper triangle of A is stored; */
/*       = 'L':  Lower triangle of A is stored. */

/*     EQUED   (input) CHARACTER*1 */
/*     Specifies the form of equilibration that was done to A */
/*     before calling this routine. This is needed to compute */
/*     the solution and error bounds correctly. */
/*       = 'N':  No equilibration */
/*       = 'Y':  Both row and column equilibration, i.e., A has been */
/*               replaced by diag(S) * A * diag(S). */
/*               The right hand side B has been changed accordingly. */

/*     N       (input) INTEGER */
/*     The order of the matrix A.  N >= 0. */

/*     NRHS    (input) INTEGER */
/*     The number of right hand sides, i.e., the number of columns */
/*     of the matrices B and X.  NRHS >= 0. */

/*     A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*     The symmetric matrix A.  If UPLO = 'U', the leading N-by-N */
/*     upper triangular part of A contains the upper triangular */
/*     part of the matrix A, and the strictly lower triangular */
/*     part of A is not referenced.  If UPLO = 'L', the leading */
/*     N-by-N lower triangular part of A contains the lower */
/*     triangular part of the matrix A, and the strictly upper */
/*     triangular part of A is not referenced. */

/*     LDA     (input) INTEGER */
/*     The leading dimension of the array A.  LDA >= max(1,N). */

/*     AF      (input) DOUBLE PRECISION array, dimension (LDAF,N) */
/*     The factored form of the matrix A.  AF contains the block */
/*     diagonal matrix D and the multipliers used to obtain the */
/*     factor U or L from the factorization A = U*D*U**T or A = */
/*     L*D*L**T as computed by DSYTRF. */

/*     LDAF    (input) INTEGER */
/*     The leading dimension of the array AF.  LDAF >= max(1,N). */

/*     IPIV    (input) INTEGER array, dimension (N) */
/*     Details of the interchanges and the block structure of D */
/*     as determined by DSYTRF. */

/*     S       (input or output) DOUBLE PRECISION array, dimension (N) */
/*     The scale factors for A.  If EQUED = 'Y', A is multiplied on */
/*     the left and right by diag(S).  S is an input argument if FACT = */
/*     'F'; otherwise, S is an output argument.  If FACT = 'F' and EQUED */
/*     = 'Y', each element of S must be positive.  If S is output, each */
/*     element of S is a power of the radix. If S is input, each element */
/*     of S should be a power of the radix to ensure a reliable solution */
/*     and error estimates. Scaling by powers of the radix does not cause */
/*     rounding errors unless the result underflows or overflows. */
/*     Rounding errors during scaling lead to refining with a matrix that */
/*     is not equivalent to the input matrix, producing error estimates */
/*     that may not be reliable. */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    af_dim1 = *ldaf;
    af_offset = 1 + af_dim1;
    af -= af_offset;
    --ipiv;
    --s;
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

    rcequ = lsame_(equed, "Y");

/*     Test input parameters. */

    if (! lsame_(uplo, "U") && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (! rcequ && ! lsame_(equed, "N")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*nrhs < 0) {
	*info = -4;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -6;
    } else if (*ldaf < std::max(1_integer,*n)) {
	*info = -8;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -11;
    } else if (*ldx < std::max(1_integer,*n)) {
	*info = -13;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYRFSX", &i__1);
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

    *(unsigned char *)norm = 'I';
    anorm = dlansy_(norm, uplo, n, &a[a_offset], lda, &work[1]);
    dsycon_(uplo, n, &af[af_offset], ldaf, &ipiv[1], &anorm, rcond, &work[1],
	    &iwork[1], info);

/*     Perform refinement on each right-hand side */

    if (ref_type__ != 0) {
	prec_type__ = ilaprec_("E");
	dla_syrfsx_extended__(&prec_type__, uplo, n, nrhs, &a[a_offset], lda,
		&af[af_offset], ldaf, &ipiv[1], &rcequ, &s[1], &b[b_offset],
		ldb, &x[x_offset], ldx, &berr[1], &n_norms__, &
		err_bnds_norm__[err_bnds_norm_offset], &err_bnds_comp__[
		err_bnds_comp_offset], &work[*n + 1], &work[1], &work[(*n <<
		1) + 1], &work[1], rcond, &ithresh, &rthresh, &
		unstable_thresh__, &ignore_cwise__, info, 1_integer);
    }
/* Computing MAX */
    d__1 = 10., d__2 = sqrt((double) (*n));
    err_lbnd__ = std::max(d__1,d__2) * dlamch_("Epsilon");
    if (*n_err_bnds__ >= 1 && n_norms__ >= 1) {

/*     Compute scaled normwise condition number cond(A*C). */

	if (rcequ) {
	    rcond_tmp__ = dla_syrcond__(uplo, n, &a[a_offset], lda, &af[
		    af_offset], ldaf, &ipiv[1], &c_n1, &s[1], info, &work[1],
		    &iwork[1], 1_integer);
	} else {
	    rcond_tmp__ = dla_syrcond__(uplo, n, &a[a_offset], lda, &af[
		    af_offset], ldaf, &ipiv[1], &c__0, &s[1], info, &work[1],
		    &iwork[1], 1_integer);
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
		rcond_tmp__ = dla_syrcond__(uplo, n, &a[a_offset], lda, &af[
			af_offset], ldaf, &ipiv[1], &c__1, &x[j * x_dim1 + 1],
			 info, &work[1], &iwork[1], 1_integer);
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

/*     End of DSYRFSX */

} /* dsyrfsx_ */
#endif

/* Subroutine */ int dsysv_(const char *uplo, integer *n, integer *nrhs, double
	*a, integer *lda, integer *ipiv, double *b, integer *ldb,
	double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1;

    /* Local variables */
    integer nb;
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

/*  DSYSV computes the solution to a real system of linear equations */
/*     A * X = B, */
/*  where A is an N-by-N symmetric matrix and X and B are N-by-NRHS */
/*  matrices. */

/*  The diagonal pivoting method is used to factor A as */
/*     A = U * D * U**T,  if UPLO = 'U', or */
/*     A = L * D * L**T,  if UPLO = 'L', */
/*  where U (or L) is a product of permutation and unit upper (lower) */
/*  triangular matrices, and D is symmetric and block diagonal with */
/*  1-by-1 and 2-by-2 diagonal blocks.  The factored form of A is then */
/*  used to solve the system of equations A * X = B. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The number of linear equations, i.e., the order of the */
/*          matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the leading */
/*          N-by-N upper triangular part of A contains the upper */
/*          triangular part of the matrix A, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading N-by-N lower triangular part of A contains the lower */
/*          triangular part of the matrix A, and the strictly upper */
/*          triangular part of A is not referenced. */

/*          On exit, if INFO = 0, the block diagonal matrix D and the */
/*          multipliers used to obtain the factor U or L from the */
/*          factorization A = U*D*U**T or A = L*D*L**T as computed by */
/*          DSYTRF. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  IPIV    (output) INTEGER array, dimension (N) */
/*          Details of the interchanges and the block structure of D, as */
/*          determined by DSYTRF.  If IPIV(k) > 0, then rows and columns */
/*          k and IPIV(k) were interchanged, and D(k,k) is a 1-by-1 */
/*          diagonal block.  If UPLO = 'U' and IPIV(k) = IPIV(k-1) < 0, */
/*          then rows and columns k-1 and -IPIV(k) were interchanged and */
/*          D(k-1:k,k-1:k) is a 2-by-2 diagonal block.  If UPLO = 'L' and */
/*          IPIV(k) = IPIV(k+1) < 0, then rows and columns k+1 and */
/*          -IPIV(k) were interchanged and D(k:k+1,k:k+1) is a 2-by-2 */
/*          diagonal block. */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the N-by-NRHS right hand side matrix B. */
/*          On exit, if INFO = 0, the N-by-NRHS solution matrix X. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,N). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The length of WORK.  LWORK >= 1, and for best performance */
/*          LWORK >= max(1,N*NB), where NB is the optimal blocksize for */
/*          DSYTRF. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          > 0: if INFO = i, D(i,i) is exactly zero.  The factorization */
/*               has been completed, but the block diagonal matrix D is */
/*               exactly singular, so the solution could not be computed. */

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

/*     Test the input parameters. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --ipiv;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --work;

    /* Function Body */
    *info = 0;
    lquery = *lwork == -1;
    if (! lsame_(uplo, "U") && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*nrhs < 0) {
	*info = -3;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -5;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -8;
    } else if (*lwork < 1 && ! lquery) {
	*info = -10;
    }

    if (*info == 0) {
	if (*n == 0) {
	    lwkopt = 1;
	} else {
	    nb = ilaenv_(&c__1, "DSYTRF", uplo, n, &c_n1, &c_n1, &c_n1);
	    lwkopt = *n * nb;
	}
	work[1] = (double) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYSV ", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Compute the factorization A = U*D*U' or A = L*D*L'. */

    dsytrf_(uplo, n, &a[a_offset], lda, &ipiv[1], &work[1], lwork, info);
    if (*info == 0) {

/*        Solve the system A*X = B, overwriting B with X. */

	dsytrs_(uplo, n, nrhs, &a[a_offset], lda, &ipiv[1], &b[b_offset], ldb,
		 info);

    }

    work[1] = (double) lwkopt;

    return 0;

/*     End of DSYSV */

} /* dsysv_ */

/* Subroutine */ int dsysvx_(const char *fact, const char *uplo, integer *n, integer *
	nrhs, double *a, integer *lda, double *af, integer *ldaf,
	integer *ipiv, double *b, integer *ldb, double *x, integer *
	ldx, double *rcond, double *ferr, double *berr,
	double *work, integer *lwork, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;

    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, b_dim1, b_offset, x_dim1,
	    x_offset, i__1, i__2;

    /* Local variables */
    integer nb;
    double anorm;
    bool nofact;
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

/*  DSYSVX uses the diagonal pivoting factorization to compute the */
/*  solution to a real system of linear equations A * X = B, */
/*  where A is an N-by-N symmetric matrix and X and B are N-by-NRHS */
/*  matrices. */

/*  Error bounds on the solution and a condition estimate are also */
/*  provided. */

/*  Description */
/*  =========== */

/*  The following steps are performed: */

/*  1. If FACT = 'N', the diagonal pivoting method is used to factor A. */
/*     The form of the factorization is */
/*        A = U * D * U**T,  if UPLO = 'U', or */
/*        A = L * D * L**T,  if UPLO = 'L', */
/*     where U (or L) is a product of permutation and unit upper (lower) */
/*     triangular matrices, and D is symmetric and block diagonal with */
/*     1-by-1 and 2-by-2 diagonal blocks. */

/*  2. If some D(i,i)=0, so that D is exactly singular, then the routine */
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
/*          = 'F':  On entry, AF and IPIV contain the factored form of */
/*                  A.  AF and IPIV will not be modified. */
/*          = 'N':  The matrix A will be copied to AF and factored. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The number of linear equations, i.e., the order of the */
/*          matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices B and X.  NRHS >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The symmetric matrix A.  If UPLO = 'U', the leading N-by-N */
/*          upper triangular part of A contains the upper triangular part */
/*          of the matrix A, and the strictly lower triangular part of A */
/*          is not referenced.  If UPLO = 'L', the leading N-by-N lower */
/*          triangular part of A contains the lower triangular part of */
/*          the matrix A, and the strictly upper triangular part of A is */
/*          not referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  AF      (input or output) DOUBLE PRECISION array, dimension (LDAF,N) */
/*          If FACT = 'F', then AF is an input argument and on entry */
/*          contains the block diagonal matrix D and the multipliers used */
/*          to obtain the factor U or L from the factorization */
/*          A = U*D*U**T or A = L*D*L**T as computed by DSYTRF. */

/*          If FACT = 'N', then AF is an output argument and on exit */
/*          returns the block diagonal matrix D and the multipliers used */
/*          to obtain the factor U or L from the factorization */
/*          A = U*D*U**T or A = L*D*L**T. */

/*  LDAF    (input) INTEGER */
/*          The leading dimension of the array AF.  LDAF >= max(1,N). */

/*  IPIV    (input or output) INTEGER array, dimension (N) */
/*          If FACT = 'F', then IPIV is an input argument and on entry */
/*          contains details of the interchanges and the block structure */
/*          of D, as determined by DSYTRF. */
/*          If IPIV(k) > 0, then rows and columns k and IPIV(k) were */
/*          interchanged and D(k,k) is a 1-by-1 diagonal block. */
/*          If UPLO = 'U' and IPIV(k) = IPIV(k-1) < 0, then rows and */
/*          columns k-1 and -IPIV(k) were interchanged and D(k-1:k,k-1:k) */
/*          is a 2-by-2 diagonal block.  If UPLO = 'L' and IPIV(k) = */
/*          IPIV(k+1) < 0, then rows and columns k+1 and -IPIV(k) were */
/*          interchanged and D(k:k+1,k:k+1) is a 2-by-2 diagonal block. */

/*          If FACT = 'N', then IPIV is an output argument and on exit */
/*          contains details of the interchanges and the block structure */
/*          of D, as determined by DSYTRF. */

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

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The length of WORK.  LWORK >= max(1,3*N), and for best */
/*          performance, when FACT = 'N', LWORK >= max(1,3*N,N*NB), where */
/*          NB is the optimal blocksize for DSYTRF. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  IWORK   (workspace) INTEGER array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          > 0: if INFO = i, and i is */
/*                <= N:  D(i,i) is exactly zero.  The factorization */
/*                       has been completed but the factor D is exactly */
/*                       singular, so the solution and error bounds could */
/*                       not be computed. RCOND = 0 is returned. */
/*                = N+1: D is nonsingular, but RCOND is less than machine */
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
    nofact = lsame_(fact, "N");
    lquery = *lwork == -1;
    if (! nofact && ! lsame_(fact, "F")) {
	*info = -1;
    } else if (! lsame_(uplo, "U") && ! lsame_(uplo,
	    "L")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*nrhs < 0) {
	*info = -4;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -6;
    } else if (*ldaf < std::max(1_integer,*n)) {
	*info = -8;
    } else if (*ldb < std::max(1_integer,*n)) {
	*info = -11;
    } else if (*ldx < std::max(1_integer,*n)) {
	*info = -13;
    } else /* if(complicated condition) */ {
/* Computing MAX */
	i__1 = 1, i__2 = *n * 3;
	if (*lwork < std::max(i__1,i__2) && ! lquery) {
	    *info = -18;
	}
    }

    if (*info == 0) {
/* Computing MAX */
	i__1 = 1, i__2 = *n * 3;
	lwkopt = std::max(i__1,i__2);
	if (nofact) {
	    nb = ilaenv_(&c__1, "DSYTRF", uplo, n, &c_n1, &c_n1, &c_n1);
/* Computing MAX */
	    i__1 = lwkopt, i__2 = *n * nb;
	    lwkopt = std::max(i__1,i__2);
	}
	work[1] = (double) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYSVX", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

    if (nofact) {

/*        Compute the factorization A = U*D*U' or A = L*D*L'. */

	dlacpy_(uplo, n, n, &a[a_offset], lda, &af[af_offset], ldaf);
	dsytrf_(uplo, n, &af[af_offset], ldaf, &ipiv[1], &work[1], lwork,
		info);

/*        Return if INFO is non-zero. */

	if (*info > 0) {
	    *rcond = 0.;
	    return 0;
	}
    }

/*     Compute the norm of the matrix A. */

    anorm = dlansy_("I", uplo, n, &a[a_offset], lda, &work[1]);

/*     Compute the reciprocal of the condition number of A. */

    dsycon_(uplo, n, &af[af_offset], ldaf, &ipiv[1], &anorm, rcond, &work[1],
	    &iwork[1], info);

/*     Compute the solution vectors X. */

    dlacpy_("Full", n, nrhs, &b[b_offset], ldb, &x[x_offset], ldx);
    dsytrs_(uplo, n, nrhs, &af[af_offset], ldaf, &ipiv[1], &x[x_offset], ldx,
	    info);

/*     Use iterative refinement to improve the computed solutions and */
/*     compute error bounds and backward error estimates for them. */

    dsyrfs_(uplo, n, nrhs, &a[a_offset], lda, &af[af_offset], ldaf, &ipiv[1],
	    &b[b_offset], ldb, &x[x_offset], ldx, &ferr[1], &berr[1], &work[1]
, &iwork[1], info);

/*     Set INFO = N+1 if the matrix is singular to working precision. */

    if (*rcond < dlamch_("Epsilon")) {
	*info = *n + 1;
    }

    work[1] = (double) lwkopt;

    return 0;

/*     End of DSYSVX */

} /* dsysvx_ */

#if 0
int dsysvxx_(const char *fact, const char *uplo, integer *n, integer *
	nrhs, double *a, integer *lda, double *af, integer *ldaf,
	integer *ipiv, char *equed, double *s, double *b, integer *
	ldb, double *x, integer *ldx, double *rcond, double *
	rpvgrw, double *berr, integer *n_err_bnds__, double *
	err_bnds_norm__, double *err_bnds_comp__, integer *nparams,
	double *params, double *work, integer *iwork, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, b_dim1, b_offset, x_dim1,
	    x_offset, err_bnds_norm_dim1, err_bnds_norm_offset,
	    err_bnds_comp_dim1, err_bnds_comp_offset, i__1;
    double d__1, d__2;

    /* Local variables */
    integer j;
    double amax, smin, smax;
    double scond;
    bool equil, rcequ, nofact;
    double bignum;
    integer infequ;
    double smlnum;


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

/*     Purpose */
/*     ======= */

/*     DSYSVXX uses the diagonal pivoting factorization to compute the */
/*     solution to a double precision system of linear equations A * X = B, where A */
/*     is an N-by-N symmetric matrix and X and B are N-by-NRHS matrices. */

/*     If requested, both normwise and maximum componentwise error bounds */
/*     are returned. DSYSVXX will return a solution with a tiny */
/*     guaranteed error (O(eps) where eps is the working machine */
/*     precision) unless the matrix is very ill-conditioned, in which */
/*     case a warning is returned. Relevant condition numbers also are */
/*     calculated and returned. */

/*     DSYSVXX accepts user-provided factorizations and equilibration */
/*     factors; see the definitions of the FACT and EQUED options. */
/*     Solving with refinement and using a factorization from a previous */
/*     DSYSVXX call will also produce a solution with either O(eps) */
/*     errors or warnings, but we cannot make that claim for general */
/*     user-provided factorizations and equilibration factors if they */
/*     differ from what DSYSVXX would itself produce. */

/*     Description */
/*     =========== */

/*     The following steps are performed: */

/*     1. If FACT = 'E', double precision scaling factors are computed to equilibrate */
/*     the system: */

/*       diag(S)*A*diag(S)     *inv(diag(S))*X = diag(S)*B */

/*     Whether or not the system will be equilibrated depends on the */
/*     scaling of the matrix A, but if equilibration is used, A is */
/*     overwritten by diag(S)*A*diag(S) and B by diag(S)*B. */

/*     2. If FACT = 'N' or 'E', the LU decomposition is used to factor */
/*     the matrix A (after equilibration if FACT = 'E') as */

/*        A = U * D * U**T,  if UPLO = 'U', or */
/*        A = L * D * L**T,  if UPLO = 'L', */

/*     where U (or L) is a product of permutation and unit upper (lower) */
/*     triangular matrices, and D is symmetric and block diagonal with */
/*     1-by-1 and 2-by-2 diagonal blocks. */

/*     3. If some D(i,i)=0, so that D is exactly singular, then the */
/*     routine returns with INFO = i. Otherwise, the factored form of A */
/*     is used to estimate the condition number of the matrix A (see */
/*     argument RCOND).  If the reciprocal of the condition number is */
/*     less than machine precision, the routine still goes on to solve */
/*     for X and compute error bounds as described below. */

/*     4. The system of equations is solved for X using the factored form */
/*     of A. */

/*     5. By default (unless PARAMS(LA_LINRX_ITREF_I) is set to zero), */
/*     the routine will use iterative refinement to try to get a small */
/*     error and error bounds.  Refinement calculates the residual to at */
/*     least twice the working precision. */

/*     6. If equilibration was used, the matrix X is premultiplied by */
/*     diag(R) so that it solves the original system before */
/*     equilibration. */

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
/*               equilibrated with scaling factors given by S. */
/*               A, AF, and IPIV are not modified. */
/*       = 'N':  The matrix A will be copied to AF and factored. */
/*       = 'E':  The matrix A will be equilibrated if necessary, then */
/*               copied to AF and factored. */

/*     UPLO    (input) CHARACTER*1 */
/*       = 'U':  Upper triangle of A is stored; */
/*       = 'L':  Lower triangle of A is stored. */

/*     N       (input) INTEGER */
/*     The number of linear equations, i.e., the order of the */
/*     matrix A.  N >= 0. */

/*     NRHS    (input) INTEGER */
/*     The number of right hand sides, i.e., the number of columns */
/*     of the matrices B and X.  NRHS >= 0. */

/*     A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*     The symmetric matrix A.  If UPLO = 'U', the leading N-by-N */
/*     upper triangular part of A contains the upper triangular */
/*     part of the matrix A, and the strictly lower triangular */
/*     part of A is not referenced.  If UPLO = 'L', the leading */
/*     N-by-N lower triangular part of A contains the lower */
/*     triangular part of the matrix A, and the strictly upper */
/*     triangular part of A is not referenced. */

/*     On exit, if FACT = 'E' and EQUED = 'Y', A is overwritten by */
/*     diag(S)*A*diag(S). */

/*     LDA     (input) INTEGER */
/*     The leading dimension of the array A.  LDA >= max(1,N). */

/*     AF      (input or output) DOUBLE PRECISION array, dimension (LDAF,N) */
/*     If FACT = 'F', then AF is an input argument and on entry */
/*     contains the block diagonal matrix D and the multipliers */
/*     used to obtain the factor U or L from the factorization A = */
/*     U*D*U**T or A = L*D*L**T as computed by DSYTRF. */

/*     If FACT = 'N', then AF is an output argument and on exit */
/*     returns the block diagonal matrix D and the multipliers */
/*     used to obtain the factor U or L from the factorization A = */
/*     U*D*U**T or A = L*D*L**T. */

/*     LDAF    (input) INTEGER */
/*     The leading dimension of the array AF.  LDAF >= max(1,N). */

/*     IPIV    (input or output) INTEGER array, dimension (N) */
/*     If FACT = 'F', then IPIV is an input argument and on entry */
/*     contains details of the interchanges and the block */
/*     structure of D, as determined by DSYTRF.  If IPIV(k) > 0, */
/*     then rows and columns k and IPIV(k) were interchanged and */
/*     D(k,k) is a 1-by-1 diagonal block.  If UPLO = 'U' and */
/*     IPIV(k) = IPIV(k-1) < 0, then rows and columns k-1 and */
/*     -IPIV(k) were interchanged and D(k-1:k,k-1:k) is a 2-by-2 */
/*     diagonal block.  If UPLO = 'L' and IPIV(k) = IPIV(k+1) < 0, */
/*     then rows and columns k+1 and -IPIV(k) were interchanged */
/*     and D(k:k+1,k:k+1) is a 2-by-2 diagonal block. */

/*     If FACT = 'N', then IPIV is an output argument and on exit */
/*     contains details of the interchanges and the block */
/*     structure of D, as determined by DSYTRF. */

/*     EQUED   (input or output) CHARACTER*1 */
/*     Specifies the form of equilibration that was done. */
/*       = 'N':  No equilibration (always true if FACT = 'N'). */
/*       = 'Y':  Both row and column equilibration, i.e., A has been */
/*               replaced by diag(S) * A * diag(S). */
/*     EQUED is an input argument if FACT = 'F'; otherwise, it is an */
/*     output argument. */

/*     S       (input or output) DOUBLE PRECISION array, dimension (N) */
/*     The scale factors for A.  If EQUED = 'Y', A is multiplied on */
/*     the left and right by diag(S).  S is an input argument if FACT = */
/*     'F'; otherwise, S is an output argument.  If FACT = 'F' and EQUED */
/*     = 'Y', each element of S must be positive.  If S is output, each */
/*     element of S is a power of the radix. If S is input, each element */
/*     of S should be a power of the radix to ensure a reliable solution */
/*     and error estimates. Scaling by powers of the radix does not cause */
/*     rounding errors unless the result underflows or overflows. */
/*     Rounding errors during scaling lead to refining with a matrix that */
/*     is not equivalent to the input matrix, producing error estimates */
/*     that may not be reliable. */

/*     B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*     On entry, the N-by-NRHS right hand side matrix B. */
/*     On exit, */
/*     if EQUED = 'N', B is not modified; */
/*     if EQUED = 'Y', B is overwritten by diag(S)*B; */

/*     LDB     (input) INTEGER */
/*     The leading dimension of the array B.  LDB >= max(1,N). */

/*     X       (output) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*     If INFO = 0, the N-by-NRHS solution matrix X to the original */
/*     system of equations.  Note that A and B are modified on exit if */
/*     EQUED .ne. 'N', and the solution to the equilibrated system is */
/*     inv(diag(S))*X. */

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
/*     for the leading INFO columns of A. */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    af_dim1 = *ldaf;
    af_offset = 1 + af_dim1;
    af -= af_offset;
    --ipiv;
    --s;
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
    smlnum = dlamch_("Safe minimum");
    bignum = 1. / smlnum;
    if (nofact || equil) {
	*(unsigned char *)equed = 'N';
	rcequ = false;
    } else {
	rcequ = lsame_(equed, "Y");
    }

/*     Default is failure.  If an input parameter is wrong or */
/*     factorization fails, make everything look horrible.  Only the */
/*     pivot growth is set here, the rest is initialized in DSYRFSX. */

    *rpvgrw = 0.;

/*     Test the input parameters.  PARAMS is not tested until DSYRFSX. */

    if (! nofact && ! equil && ! lsame_(fact, "F")) {
	*info = -1;
    } else if (! lsame_(uplo, "U") && ! lsame_(uplo,
	    "L")) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*nrhs < 0) {
	*info = -4;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -6;
    } else if (*ldaf < std::max(1_integer,*n)) {
	*info = -8;
    } else if (lsame_(fact, "F") && ! (rcequ || lsame_(
	    equed, "N"))) {
	*info = -9;
    } else {
	if (rcequ) {
	    smin = bignum;
	    smax = 0.;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		d__1 = smin, d__2 = s[j];
		smin = std::min(d__1,d__2);
/* Computing MAX */
		d__1 = smax, d__2 = s[j];
		smax = std::max(d__1,d__2);
/* L10: */
	    }
	    if (smin <= 0.) {
		*info = -10;
	    } else if (*n > 0) {
		scond = std::max(smin,smlnum) / min(smax,bignum);
	    } else {
		scond = 1.;
	    }
	}
	if (*info == 0) {
	    if (*ldb < std::max(1_integer,*n)) {
		*info = -12;
	    } else if (*ldx < std::max(1_integer,*n)) {
		*info = -14;
	    }
	}
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYSVXX", &i__1);
	return 0;
    }

    if (equil) {

/*     Compute row and column scalings to equilibrate the matrix A. */

	dsyequb_(uplo, n, &a[a_offset], lda, &s[1], &scond, &amax, &work[1], &
		infequ);
	if (infequ == 0) {

/*     Equilibrate the matrix. */

	    dlaqsy_(uplo, n, &a[a_offset], lda, &s[1], &scond, &amax, equed);
	    rcequ = lsame_(equed, "Y");
	}
    }

/*     Scale the right-hand side. */

    if (rcequ) {
	dlascl2_(n, nrhs, &s[1], &b[b_offset], ldb);
    }

    if (nofact || equil) {

/*        Compute the LU factorization of A. */

	dlacpy_(uplo, n, n, &a[a_offset], lda, &af[af_offset], ldaf);
	i__1 = std::max(1_integer,*n) * 5;
	dsytrf_(uplo, n, &af[af_offset], ldaf, &ipiv[1], &work[1], &i__1,
		info);

/*        Return if INFO is non-zero. */

	if (*info > 0) {

/*           Pivot in column INFO is exactly 0 */
/*           Compute the reciprocal pivot growth factor of the */
/*           leading rank-deficient INFO columns of A. */

	    if (*n > 0) {
		*rpvgrw = dla_syrpvgrw__(uplo, n, info, &a[a_offset], lda, &
			af[af_offset], ldaf, &ipiv[1], &work[1], 1_integer);
	    }
	    return 0;
	}
    }

/*     Compute the reciprocal pivot growth factor RPVGRW. */

    if (*n > 0) {
	*rpvgrw = dla_syrpvgrw__(uplo, n, info, &a[a_offset], lda, &af[
		af_offset], ldaf, &ipiv[1], &work[1], 1_integer);
    }

/*     Compute the solution matrix X. */

    dlacpy_("Full", n, nrhs, &b[b_offset], ldb, &x[x_offset], ldx);
    dsytrs_(uplo, n, nrhs, &af[af_offset], ldaf, &ipiv[1], &x[x_offset], ldx, info);

/*     Use iterative refinement to improve the computed solution and */
/*     compute error bounds and backward error estimates for it. */

    dsyrfsx_(uplo, equed, n, nrhs, &a[a_offset], lda, &af[af_offset], ldaf, &
	    ipiv[1], &s[1], &b[b_offset], ldb, &x[x_offset], ldx, rcond, &
	    berr[1], n_err_bnds__, &err_bnds_norm__[err_bnds_norm_offset], &
	    err_bnds_comp__[err_bnds_comp_offset], nparams, &params[1], &work[
	    1], &iwork[1], info);

/*     Scale solutions. */

    if (rcequ) {
	dlascl2_(n, nrhs, &s[1], &x[x_offset], ldx);
    }

    return 0;

/*     End of DSYSVXX */

} /* dsysvxx_ */
#endif

/* Subroutine */ int dsytd2_(const char *uplo, integer *n, double *a, integer *
	lda, double *d__, double *e, double *tau, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b8 = 0.;
	static double c_b14 = -1.;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    integer i__;
    double taui;
    double alpha;
    bool upper;

/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSYTD2 reduces a real symmetric matrix A to symmetric tridiagonal */
/*  form T by an orthogonal similarity transformation: Q' * A * Q = T. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is stored: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the leading */
/*          n-by-n upper triangular part of A contains the upper */
/*          triangular part of the matrix A, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading n-by-n lower triangular part of A contains the lower */
/*          triangular part of the matrix A, and the strictly upper */
/*          triangular part of A is not referenced. */
/*          On exit, if UPLO = 'U', the diagonal and first superdiagonal */
/*          of A are overwritten by the corresponding elements of the */
/*          tridiagonal matrix T, and the elements above the first */
/*          superdiagonal, with the array TAU, represent the orthogonal */
/*          matrix Q as a product of elementary reflectors; if UPLO */
/*          = 'L', the diagonal and first subdiagonal of A are over- */
/*          written by the corresponding elements of the tridiagonal */
/*          matrix T, and the elements below the first subdiagonal, with */
/*          the array TAU, represent the orthogonal matrix Q as a product */
/*          of elementary reflectors. See Further Details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  D       (output) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal elements of the tridiagonal matrix T: */
/*          D(i) = A(i,i). */

/*  E       (output) DOUBLE PRECISION array, dimension (N-1) */
/*          The off-diagonal elements of the tridiagonal matrix T: */
/*          E(i) = A(i,i+1) if UPLO = 'U', E(i) = A(i+1,i) if UPLO = 'L'. */

/*  TAU     (output) DOUBLE PRECISION array, dimension (N-1) */
/*          The scalar factors of the elementary reflectors (see Further */
/*          Details). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  If UPLO = 'U', the matrix Q is represented as a product of elementary */
/*  reflectors */

/*     Q = H(n-1) . . . H(2) H(1). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(i+1:n) = 0 and v(i) = 1; v(1:i-1) is stored on exit in */
/*  A(1:i-1,i+1), and tau in TAU(i). */

/*  If UPLO = 'L', the matrix Q is represented as a product of elementary */
/*  reflectors */

/*     Q = H(1) H(2) . . . H(n-1). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(1:i) = 0 and v(i+1) = 1; v(i+2:n) is stored on exit in A(i+2:n,i), */
/*  and tau in TAU(i). */

/*  The contents of A on exit are illustrated by the following examples */
/*  with n = 5: */

/*  if UPLO = 'U':                       if UPLO = 'L': */

/*    (  d   e   v2  v3  v4 )              (  d                  ) */
/*    (      d   e   v3  v4 )              (  e   d              ) */
/*    (          d   e   v4 )              (  v1  e   d          ) */
/*    (              d   e  )              (  v1  v2  e   d      ) */
/*    (                  d  )              (  v1  v2  v3  e   d  ) */

/*  where d and e denote diagonal and off-diagonal elements of T, and vi */
/*  denotes an element of the vector defining H(i). */

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
    --d__;
    --e;
    --tau;

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
	xerbla_("DSYTD2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n <= 0) {
	return 0;
    }

    if (upper) {

/*        Reduce the upper triangle of A */

	for (i__ = *n - 1; i__ >= 1; --i__) {

/*           Generate elementary reflector H(i) = I - tau * v * v' */
/*           to annihilate A(1:i-1,i+1) */

	    dlarfg_(&i__, &a[i__ + (i__ + 1) * a_dim1], &a[(i__ + 1) * a_dim1
		    + 1], &c__1, &taui);
	    e[i__] = a[i__ + (i__ + 1) * a_dim1];

	    if (taui != 0.) {

/*              Apply H(i) from both sides to A(1:i,1:i) */

		a[i__ + (i__ + 1) * a_dim1] = 1.;

/*              Compute  x := tau * A * v  storing x in TAU(1:i) */

		dsymv_(uplo, &i__, &taui, &a[a_offset], lda, &a[(i__ + 1) *
			a_dim1 + 1], &c__1, &c_b8, &tau[1], &c__1);

/*              Compute  w := x - 1/2 * tau * (x'*v) * v */

		alpha = taui * -.5 * ddot_(&i__, &tau[1], &c__1, &a[(i__ + 1)
			* a_dim1 + 1], &c__1);
		daxpy_(&i__, &alpha, &a[(i__ + 1) * a_dim1 + 1], &c__1, &tau[
			1], &c__1);

/*              Apply the transformation as a rank-2 update: */
/*                 A := A - v * w' - w * v' */

		dsyr2_(uplo, &i__, &c_b14, &a[(i__ + 1) * a_dim1 + 1], &c__1,
			&tau[1], &c__1, &a[a_offset], lda);

		a[i__ + (i__ + 1) * a_dim1] = e[i__];
	    }
	    d__[i__ + 1] = a[i__ + 1 + (i__ + 1) * a_dim1];
	    tau[i__] = taui;
/* L10: */
	}
	d__[1] = a[a_dim1 + 1];
    } else {

/*        Reduce the lower triangle of A */

	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Generate elementary reflector H(i) = I - tau * v * v' */
/*           to annihilate A(i+2:n,i) */

	    i__2 = *n - i__;
/* Computing MIN */
	    i__3 = i__ + 2;
	    dlarfg_(&i__2, &a[i__ + 1 + i__ * a_dim1], &a[std::min(i__3, *n)+ i__ *
		     a_dim1], &c__1, &taui);
	    e[i__] = a[i__ + 1 + i__ * a_dim1];

	    if (taui != 0.) {

/*              Apply H(i) from both sides to A(i+1:n,i+1:n) */

		a[i__ + 1 + i__ * a_dim1] = 1.;

/*              Compute  x := tau * A * v  storing y in TAU(i:n-1) */

		i__2 = *n - i__;
		dsymv_(uplo, &i__2, &taui, &a[i__ + 1 + (i__ + 1) * a_dim1],
			lda, &a[i__ + 1 + i__ * a_dim1], &c__1, &c_b8, &tau[
			i__], &c__1);

/*              Compute  w := x - 1/2 * tau * (x'*v) * v */

		i__2 = *n - i__;
		alpha = taui * -.5 * ddot_(&i__2, &tau[i__], &c__1, &a[i__ +
			1 + i__ * a_dim1], &c__1);
		i__2 = *n - i__;
		daxpy_(&i__2, &alpha, &a[i__ + 1 + i__ * a_dim1], &c__1, &tau[
			i__], &c__1);

/*              Apply the transformation as a rank-2 update: */
/*                 A := A - v * w' - w * v' */

		i__2 = *n - i__;
		dsyr2_(uplo, &i__2, &c_b14, &a[i__ + 1 + i__ * a_dim1], &c__1,
			 &tau[i__], &c__1, &a[i__ + 1 + (i__ + 1) * a_dim1],
			lda);

		a[i__ + 1 + i__ * a_dim1] = e[i__];
	    }
	    d__[i__] = a[i__ + i__ * a_dim1];
	    tau[i__] = taui;
/* L20: */
	}
	d__[*n] = a[*n + *n * a_dim1];
    }

    return 0;

/*     End of DSYTD2 */

} /* dsytd2_ */

/* Subroutine */ int dsytf2_(const char *uplo, integer *n, double *a, integer *
	lda, integer *ipiv, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    double d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, k;
    double t, r1, d11, d12, d21, d22;
    integer kk, kp;
    double wk, wkm1, wkp1;
    integer imax, jmax;
    double alpha;
    integer kstep;
    bool upper;
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

/*  DSYTF2 computes the factorization of a real symmetric matrix A using */
/*  the Bunch-Kaufman diagonal pivoting method: */

/*     A = U*D*U'  or  A = L*D*L' */

/*  where U (or L) is a product of permutation and unit upper (lower) */
/*  triangular matrices, U' is the transpose of U, and D is symmetric and */
/*  block diagonal with 1-by-1 and 2-by-2 diagonal blocks. */

/*  This is the unblocked version of the algorithm, calling Level 2 BLAS. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is stored: */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the leading */
/*          n-by-n upper triangular part of A contains the upper */
/*          triangular part of the matrix A, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading n-by-n lower triangular part of A contains the lower */
/*          triangular part of the matrix A, and the strictly upper */
/*          triangular part of A is not referenced. */

/*          On exit, the block diagonal matrix D and the multipliers used */
/*          to obtain the factor U or L (see below for further details). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  IPIV    (output) INTEGER array, dimension (N) */
/*          Details of the interchanges and the block structure of D. */
/*          If IPIV(k) > 0, then rows and columns k and IPIV(k) were */
/*          interchanged and D(k,k) is a 1-by-1 diagonal block. */
/*          If UPLO = 'U' and IPIV(k) = IPIV(k-1) < 0, then rows and */
/*          columns k-1 and -IPIV(k) were interchanged and D(k-1:k,k-1:k) */
/*          is a 2-by-2 diagonal block.  If UPLO = 'L' and IPIV(k) = */
/*          IPIV(k+1) < 0, then rows and columns k+1 and -IPIV(k) were */
/*          interchanged and D(k:k+1,k:k+1) is a 2-by-2 diagonal block. */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -k, the k-th argument had an illegal value */
/*          > 0: if INFO = k, D(k,k) is exactly zero.  The factorization */
/*               has been completed, but the block diagonal matrix D is */
/*               exactly singular, and division by zero will occur if it */
/*               is used to solve a system of equations. */

/*  Further Details */
/*  =============== */

/*  09-29-06 - patch from */
/*    Bobby Cheng, MathWorks */

/*    Replace l.204 and l.372 */
/*         IF( MAX( ABSAKK, COLMAX ).EQ.ZERO ) THEN */
/*    by */
/*         IF( (MAX( ABSAKK, COLMAX ).EQ.ZERO) .OR. DISNAN(ABSAKK) ) THEN */

/*  01-01-96 - Based on modifications by */
/*    J. Lewis, Boeing Computer Services Company */
/*    A. Petitet, Computer Science Dept., Univ. of Tenn., Knoxville, USA */
/*  1-96 - Based on modifications by J. Lewis, Boeing Computer Services */
/*         Company */

/*  If UPLO = 'U', then A = U*D*U', where */
/*     U = P(n)*U(n)* ... *P(k)U(k)* ..., */
/*  i.e., U is a product of terms P(k)*U(k), where k decreases from n to */
/*  1 in steps of 1 or 2, and D is a block diagonal matrix with 1-by-1 */
/*  and 2-by-2 diagonal blocks D(k).  P(k) is a permutation matrix as */
/*  defined by IPIV(k), and U(k) is a unit upper triangular matrix, such */
/*  that if the diagonal block D(k) is of order s (s = 1 or 2), then */

/*             (   I    v    0   )   k-s */
/*     U(k) =  (   0    I    0   )   s */
/*             (   0    0    I   )   n-k */
/*                k-s   s   n-k */

/*  If s = 1, D(k) overwrites A(k,k), and v overwrites A(1:k-1,k). */
/*  If s = 2, the upper triangle of D(k) overwrites A(k-1,k-1), A(k-1,k), */
/*  and A(k,k), and v overwrites A(1:k-2,k-1:k). */

/*  If UPLO = 'L', then A = L*D*L', where */
/*     L = P(1)*L(1)* ... *P(k)*L(k)* ..., */
/*  i.e., L is a product of terms P(k)*L(k), where k increases from 1 to */
/*  n in steps of 1 or 2, and D is a block diagonal matrix with 1-by-1 */
/*  and 2-by-2 diagonal blocks D(k).  P(k) is a permutation matrix as */
/*  defined by IPIV(k), and L(k) is a unit lower triangular matrix, such */
/*  that if the diagonal block D(k) is of order s (s = 1 or 2), then */

/*             (   I    0     0   )  k-1 */
/*     L(k) =  (   0    I     0   )  s */
/*             (   0    v     I   )  n-k-s+1 */
/*                k-1   s  n-k-s+1 */

/*  If s = 1, D(k) overwrites A(k,k), and v overwrites A(k+1:n,k). */
/*  If s = 2, the lower triangle of D(k) overwrites A(k,k), A(k+1,k), */
/*  and A(k+1,k+1), and v overwrites A(k+2:n,k:k+1). */

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
	xerbla_("DSYTF2", &i__1);
	return 0;
    }

/*     Initialize ALPHA for use in choosing pivot block size. */

    alpha = (sqrt(17.) + 1.) / 8.;

    if (upper) {

/*        Factorize A as U*D*U' using the upper triangle of A */

/*        K is the main loop index, decreasing from N to 1 in steps of */
/*        1 or 2 */

	k = *n;
L10:

/*        If K < 1, exit from loop */

	if (k < 1) {
	    goto L70;
	}
	kstep = 1;

/*        Determine rows and columns to be interchanged and whether */
/*        a 1-by-1 or 2-by-2 pivot block will be used */

	absakk = (d__1 = a[k + k * a_dim1], abs(d__1));

/*        IMAX is the row-index of the largest off-diagonal element in */
/*        column K, and COLMAX is its absolute value */

	if (k > 1) {
	    i__1 = k - 1;
	    imax = idamax_(&i__1, &a[k * a_dim1 + 1], &c__1);
	    colmax = (d__1 = a[imax + k * a_dim1], abs(d__1));
	} else {
	    colmax = 0.;
	}

	if (std::max(absakk,colmax) == 0. || disnan_(&absakk)) {

/*           Column K is zero or contains a NaN: set INFO and continue */

	    if (*info == 0) {
		*info = k;
	    }
	    kp = k;
	} else {
	    if (absakk >= alpha * colmax) {

/*              no interchange, use 1-by-1 pivot block */

		kp = k;
	    } else {

/*              JMAX is the column-index of the largest off-diagonal */
/*              element in row IMAX, and ROWMAX is its absolute value */

		i__1 = k - imax;
		jmax = imax + idamax_(&i__1, &a[imax + (imax + 1) * a_dim1],
			lda);
		rowmax = (d__1 = a[imax + jmax * a_dim1], abs(d__1));
		if (imax > 1) {
		    i__1 = imax - 1;
		    jmax = idamax_(&i__1, &a[imax * a_dim1 + 1], &c__1);
/* Computing MAX */
		    d__2 = rowmax, d__3 = (d__1 = a[jmax + imax * a_dim1],
			    abs(d__1));
		    rowmax = std::max(d__2,d__3);
		}

		if (absakk >= alpha * colmax * (colmax / rowmax)) {

/*                 no interchange, use 1-by-1 pivot block */

		    kp = k;
		} else if ((d__1 = a[imax + imax * a_dim1], abs(d__1)) >=
			alpha * rowmax) {

/*                 interchange rows and columns K and IMAX, use 1-by-1 */
/*                 pivot block */

		    kp = imax;
		} else {

/*                 interchange rows and columns K-1 and IMAX, use 2-by-2 */
/*                 pivot block */

		    kp = imax;
		    kstep = 2;
		}
	    }

	    kk = k - kstep + 1;
	    if (kp != kk) {

/*              Interchange rows and columns KK and KP in the leading */
/*              submatrix A(1:k,1:k) */

		i__1 = kp - 1;
		dswap_(&i__1, &a[kk * a_dim1 + 1], &c__1, &a[kp * a_dim1 + 1],
			 &c__1);
		i__1 = kk - kp - 1;
		dswap_(&i__1, &a[kp + 1 + kk * a_dim1], &c__1, &a[kp + (kp +
			1) * a_dim1], lda);
		t = a[kk + kk * a_dim1];
		a[kk + kk * a_dim1] = a[kp + kp * a_dim1];
		a[kp + kp * a_dim1] = t;
		if (kstep == 2) {
		    t = a[k - 1 + k * a_dim1];
		    a[k - 1 + k * a_dim1] = a[kp + k * a_dim1];
		    a[kp + k * a_dim1] = t;
		}
	    }

/*           Update the leading submatrix */

	    if (kstep == 1) {

/*              1-by-1 pivot block D(k): column k now holds */

/*              W(k) = U(k)*D(k) */

/*              where U(k) is the k-th column of U */

/*              Perform a rank-1 update of A(1:k-1,1:k-1) as */

/*              A := A - U(k)*D(k)*U(k)' = A - W(k)*1/D(k)*W(k)' */

		r1 = 1. / a[k + k * a_dim1];
		i__1 = k - 1;
		d__1 = -r1;
		dsyr_(uplo, &i__1, &d__1, &a[k * a_dim1 + 1], &c__1, &a[
			a_offset], lda);

/*              Store U(k) in column k */

		i__1 = k - 1;
		dscal_(&i__1, &r1, &a[k * a_dim1 + 1], &c__1);
	    } else {

/*              2-by-2 pivot block D(k): columns k and k-1 now hold */

/*              ( W(k-1) W(k) ) = ( U(k-1) U(k) )*D(k) */

/*              where U(k) and U(k-1) are the k-th and (k-1)-th columns */
/*              of U */

/*              Perform a rank-2 update of A(1:k-2,1:k-2) as */

/*              A := A - ( U(k-1) U(k) )*D(k)*( U(k-1) U(k) )' */
/*                 = A - ( W(k-1) W(k) )*inv(D(k))*( W(k-1) W(k) )' */

		if (k > 2) {

		    d12 = a[k - 1 + k * a_dim1];
		    d22 = a[k - 1 + (k - 1) * a_dim1] / d12;
		    d11 = a[k + k * a_dim1] / d12;
		    t = 1. / (d11 * d22 - 1.);
		    d12 = t / d12;

		    for (j = k - 2; j >= 1; --j) {
			wkm1 = d12 * (d11 * a[j + (k - 1) * a_dim1] - a[j + k
				* a_dim1]);
			wk = d12 * (d22 * a[j + k * a_dim1] - a[j + (k - 1) *
				a_dim1]);
			for (i__ = j; i__ >= 1; --i__) {
			    a[i__ + j * a_dim1] = a[i__ + j * a_dim1] - a[i__
				    + k * a_dim1] * wk - a[i__ + (k - 1) *
				    a_dim1] * wkm1;
/* L20: */
			}
			a[j + k * a_dim1] = wk;
			a[j + (k - 1) * a_dim1] = wkm1;
/* L30: */
		    }

		}

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

    } else {

/*        Factorize A as L*D*L' using the lower triangle of A */

/*        K is the main loop index, increasing from 1 to N in steps of */
/*        1 or 2 */

	k = 1;
L40:

/*        If K > N, exit from loop */

	if (k > *n) {
	    goto L70;
	}
	kstep = 1;

/*        Determine rows and columns to be interchanged and whether */
/*        a 1-by-1 or 2-by-2 pivot block will be used */

	absakk = (d__1 = a[k + k * a_dim1], abs(d__1));

/*        IMAX is the row-index of the largest off-diagonal element in */
/*        column K, and COLMAX is its absolute value */

	if (k < *n) {
	    i__1 = *n - k;
	    imax = k + idamax_(&i__1, &a[k + 1 + k * a_dim1], &c__1);
	    colmax = (d__1 = a[imax + k * a_dim1], abs(d__1));
	} else {
	    colmax = 0.;
	}

	if (std::max(absakk,colmax) == 0. || disnan_(&absakk)) {

/*           Column K is zero or contains a NaN: set INFO and continue */

	    if (*info == 0) {
		*info = k;
	    }
	    kp = k;
	} else {
	    if (absakk >= alpha * colmax) {

/*              no interchange, use 1-by-1 pivot block */

		kp = k;
	    } else {

/*              JMAX is the column-index of the largest off-diagonal */
/*              element in row IMAX, and ROWMAX is its absolute value */

		i__1 = imax - k;
		jmax = k - 1 + idamax_(&i__1, &a[imax + k * a_dim1], lda);
		rowmax = (d__1 = a[imax + jmax * a_dim1], abs(d__1));
		if (imax < *n) {
		    i__1 = *n - imax;
		    jmax = imax + idamax_(&i__1, &a[imax + 1 + imax * a_dim1],
			     &c__1);
/* Computing MAX */
		    d__2 = rowmax, d__3 = (d__1 = a[jmax + imax * a_dim1],
			    abs(d__1));
		    rowmax = std::max(d__2,d__3);
		}

		if (absakk >= alpha * colmax * (colmax / rowmax)) {

/*                 no interchange, use 1-by-1 pivot block */

		    kp = k;
		} else if ((d__1 = a[imax + imax * a_dim1], abs(d__1)) >=
			alpha * rowmax) {

/*                 interchange rows and columns K and IMAX, use 1-by-1 */
/*                 pivot block */

		    kp = imax;
		} else {

/*                 interchange rows and columns K+1 and IMAX, use 2-by-2 */
/*                 pivot block */

		    kp = imax;
		    kstep = 2;
		}
	    }

	    kk = k + kstep - 1;
	    if (kp != kk) {

/*              Interchange rows and columns KK and KP in the trailing */
/*              submatrix A(k:n,k:n) */

		if (kp < *n) {
		    i__1 = *n - kp;
		    dswap_(&i__1, &a[kp + 1 + kk * a_dim1], &c__1, &a[kp + 1
			    + kp * a_dim1], &c__1);
		}
		i__1 = kp - kk - 1;
		dswap_(&i__1, &a[kk + 1 + kk * a_dim1], &c__1, &a[kp + (kk +
			1) * a_dim1], lda);
		t = a[kk + kk * a_dim1];
		a[kk + kk * a_dim1] = a[kp + kp * a_dim1];
		a[kp + kp * a_dim1] = t;
		if (kstep == 2) {
		    t = a[k + 1 + k * a_dim1];
		    a[k + 1 + k * a_dim1] = a[kp + k * a_dim1];
		    a[kp + k * a_dim1] = t;
		}
	    }

/*           Update the trailing submatrix */

	    if (kstep == 1) {

/*              1-by-1 pivot block D(k): column k now holds */

/*              W(k) = L(k)*D(k) */

/*              where L(k) is the k-th column of L */

		if (k < *n) {

/*                 Perform a rank-1 update of A(k+1:n,k+1:n) as */

/*                 A := A - L(k)*D(k)*L(k)' = A - W(k)*(1/D(k))*W(k)' */

		    d11 = 1. / a[k + k * a_dim1];
		    i__1 = *n - k;
		    d__1 = -d11;
		    dsyr_(uplo, &i__1, &d__1, &a[k + 1 + k * a_dim1], &c__1, &
			    a[k + 1 + (k + 1) * a_dim1], lda);

/*                 Store L(k) in column K */

		    i__1 = *n - k;
		    dscal_(&i__1, &d11, &a[k + 1 + k * a_dim1], &c__1);
		}
	    } else {

/*              2-by-2 pivot block D(k) */

		if (k < *n - 1) {

/*                 Perform a rank-2 update of A(k+2:n,k+2:n) as */

/*                 A := A - ( (A(k) A(k+1))*D(k)**(-1) ) * (A(k) A(k+1))' */

/*                 where L(k) and L(k+1) are the k-th and (k+1)-th */
/*                 columns of L */

		    d21 = a[k + 1 + k * a_dim1];
		    d11 = a[k + 1 + (k + 1) * a_dim1] / d21;
		    d22 = a[k + k * a_dim1] / d21;
		    t = 1. / (d11 * d22 - 1.);
		    d21 = t / d21;

		    i__1 = *n;
		    for (j = k + 2; j <= i__1; ++j) {

			wk = d21 * (d11 * a[j + k * a_dim1] - a[j + (k + 1) *
				a_dim1]);
			wkp1 = d21 * (d22 * a[j + (k + 1) * a_dim1] - a[j + k
				* a_dim1]);

			i__2 = *n;
			for (i__ = j; i__ <= i__2; ++i__) {
			    a[i__ + j * a_dim1] = a[i__ + j * a_dim1] - a[i__
				    + k * a_dim1] * wk - a[i__ + (k + 1) *
				    a_dim1] * wkp1;
/* L50: */
			}

			a[j + k * a_dim1] = wk;
			a[j + (k + 1) * a_dim1] = wkp1;

/* L60: */
		    }
		}
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
	goto L40;

    }

L70:

    return 0;

/*     End of DSYTF2 */

} /* dsytf2_ */

/* Subroutine */ int dsytrd_(const char *uplo, integer *n, double *a, integer *
	lda, double *d__, double *e, double *tau, double *
	work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__3 = 3;
	static integer c__2 = 2;
	static double c_b22 = -1.;
	static double c_b23 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;

    /* Local variables */
    integer i__, j, nb, kk, nx, iws;
    integer nbmin, iinfo;
    bool upper;
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

/*  DSYTRD reduces a real symmetric matrix A to real symmetric */
/*  tridiagonal form T by an orthogonal similarity transformation: */
/*  Q**T * A * Q = T. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the leading */
/*          N-by-N upper triangular part of A contains the upper */
/*          triangular part of the matrix A, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading N-by-N lower triangular part of A contains the lower */
/*          triangular part of the matrix A, and the strictly upper */
/*          triangular part of A is not referenced. */
/*          On exit, if UPLO = 'U', the diagonal and first superdiagonal */
/*          of A are overwritten by the corresponding elements of the */
/*          tridiagonal matrix T, and the elements above the first */
/*          superdiagonal, with the array TAU, represent the orthogonal */
/*          matrix Q as a product of elementary reflectors; if UPLO */
/*          = 'L', the diagonal and first subdiagonal of A are over- */
/*          written by the corresponding elements of the tridiagonal */
/*          matrix T, and the elements below the first subdiagonal, with */
/*          the array TAU, represent the orthogonal matrix Q as a product */
/*          of elementary reflectors. See Further Details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  D       (output) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal elements of the tridiagonal matrix T: */
/*          D(i) = A(i,i). */

/*  E       (output) DOUBLE PRECISION array, dimension (N-1) */
/*          The off-diagonal elements of the tridiagonal matrix T: */
/*          E(i) = A(i,i+1) if UPLO = 'U', E(i) = A(i+1,i) if UPLO = 'L'. */

/*  TAU     (output) DOUBLE PRECISION array, dimension (N-1) */
/*          The scalar factors of the elementary reflectors (see Further */
/*          Details). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= 1. */
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

/*  If UPLO = 'U', the matrix Q is represented as a product of elementary */
/*  reflectors */

/*     Q = H(n-1) . . . H(2) H(1). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(i+1:n) = 0 and v(i) = 1; v(1:i-1) is stored on exit in */
/*  A(1:i-1,i+1), and tau in TAU(i). */

/*  If UPLO = 'L', the matrix Q is represented as a product of elementary */
/*  reflectors */

/*     Q = H(1) H(2) . . . H(n-1). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(1:i) = 0 and v(i+1) = 1; v(i+2:n) is stored on exit in A(i+2:n,i), */
/*  and tau in TAU(i). */

/*  The contents of A on exit are illustrated by the following examples */
/*  with n = 5: */

/*  if UPLO = 'U':                       if UPLO = 'L': */

/*    (  d   e   v2  v3  v4 )              (  d                  ) */
/*    (      d   e   v3  v4 )              (  e   d              ) */
/*    (          d   e   v4 )              (  v1  e   d          ) */
/*    (              d   e  )              (  v1  v2  e   d      ) */
/*    (                  d  )              (  v1  v2  v3  e   d  ) */

/*  where d and e denote diagonal and off-diagonal elements of T, and vi */
/*  denotes an element of the vector defining H(i). */

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
    --tau;
    --work;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    lquery = *lwork == -1;
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -4;
    } else if (*lwork < 1 && ! lquery) {
	*info = -9;
    }

    if (*info == 0) {

/*        Determine the block size. */

	nb = ilaenv_(&c__1, "DSYTRD", uplo, n, &c_n1, &c_n1, &c_n1);
	lwkopt = *n * nb;
	work[1] = (double) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYTRD", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	work[1] = 1.;
	return 0;
    }

    nx = *n;
    iws = 1;
    if (nb > 1 && nb < *n) {

/*        Determine when to cross over from blocked to unblocked code */
/*        (last block is always handled by unblocked code). */

/* Computing MAX */
	i__1 = nb, i__2 = ilaenv_(&c__3, "DSYTRD", uplo, n, &c_n1, &c_n1, &
		c_n1);
	nx = std::max(i__1,i__2);
	if (nx < *n) {

/*           Determine if workspace is large enough for blocked code. */

	    ldwork = *n;
	    iws = ldwork * nb;
	    if (*lwork < iws) {

/*              Not enough workspace to use optimal NB:  determine the */
/*              minimum value of NB, and reduce NB or force use of */
/*              unblocked code by setting NX = N. */

/* Computing MAX */
		i__1 = *lwork / ldwork;
		nb = std::max(i__1,1_integer);
		nbmin = ilaenv_(&c__2, "DSYTRD", uplo, n, &c_n1, &c_n1, &c_n1);
		if (nb < nbmin) {
		    nx = *n;
		}
	    }
	} else {
	    nx = *n;
	}
    } else {
	nb = 1;
    }

    if (upper) {

/*        Reduce the upper triangle of A. */
/*        Columns 1:kk are handled by the unblocked method. */

	kk = *n - (*n - nx + nb - 1) / nb * nb;
	i__1 = kk + 1;
	i__2 = -nb;
	for (i__ = *n - nb + 1; i__2 < 0 ? i__ >= i__1 : i__ <= i__1; i__ +=
		i__2) {

/*           Reduce columns i:i+nb-1 to tridiagonal form and form the */
/*           matrix W which is needed to update the unreduced part of */
/*           the matrix */

	    i__3 = i__ + nb - 1;
	    dlatrd_(uplo, &i__3, &nb, &a[a_offset], lda, &e[1], &tau[1], &
		    work[1], &ldwork);

/*           Update the unreduced submatrix A(1:i-1,1:i-1), using an */
/*           update of the form:  A := A - V*W' - W*V' */

	    i__3 = i__ - 1;
	    dsyr2k_(uplo, "No transpose", &i__3, &nb, &c_b22, &a[i__ * a_dim1
		    + 1], lda, &work[1], &ldwork, &c_b23, &a[a_offset], lda);

/*           Copy superdiagonal elements back into A, and diagonal */
/*           elements into D */

	    i__3 = i__ + nb - 1;
	    for (j = i__; j <= i__3; ++j) {
		a[j - 1 + j * a_dim1] = e[j - 1];
		d__[j] = a[j + j * a_dim1];
/* L10: */
	    }
/* L20: */
	}

/*        Use unblocked code to reduce the last or only block */

	dsytd2_(uplo, &kk, &a[a_offset], lda, &d__[1], &e[1], &tau[1], &iinfo);
    } else {

/*        Reduce the lower triangle of A */

	i__2 = *n - nx;
	i__1 = nb;
	for (i__ = 1; i__1 < 0 ? i__ >= i__2 : i__ <= i__2; i__ += i__1) {

/*           Reduce columns i:i+nb-1 to tridiagonal form and form the */
/*           matrix W which is needed to update the unreduced part of */
/*           the matrix */

	    i__3 = *n - i__ + 1;
	    dlatrd_(uplo, &i__3, &nb, &a[i__ + i__ * a_dim1], lda, &e[i__], &
		    tau[i__], &work[1], &ldwork);

/*           Update the unreduced submatrix A(i+ib:n,i+ib:n), using */
/*           an update of the form:  A := A - V*W' - W*V' */

	    i__3 = *n - i__ - nb + 1;
	    dsyr2k_(uplo, "No transpose", &i__3, &nb, &c_b22, &a[i__ + nb +
		    i__ * a_dim1], lda, &work[nb + 1], &ldwork, &c_b23, &a[
		    i__ + nb + (i__ + nb) * a_dim1], lda);

/*           Copy subdiagonal elements back into A, and diagonal */
/*           elements into D */

	    i__3 = i__ + nb - 1;
	    for (j = i__; j <= i__3; ++j) {
		a[j + 1 + j * a_dim1] = e[j];
		d__[j] = a[j + j * a_dim1];
/* L30: */
	    }
/* L40: */
	}

/*        Use unblocked code to reduce the last or only block */

	i__1 = *n - i__ + 1;
	dsytd2_(uplo, &i__1, &a[i__ + i__ * a_dim1], lda, &d__[i__], &e[i__],
		&tau[i__], &iinfo);
    }

    work[1] = (double) lwkopt;
    return 0;

/*     End of DSYTRD */

} /* dsytrd_ */

/* Subroutine */ int dsytrf_(const char *uplo, integer *n, double *a, integer *
	lda, integer *ipiv, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static integer c__2 = 2;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    integer j, k, kb, nb, iws;

    integer nbmin, iinfo;
    bool upper;
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

/*  DSYTRF computes the factorization of a real symmetric matrix A using */
/*  the Bunch-Kaufman diagonal pivoting method.  The form of the */
/*  factorization is */

/*     A = U*D*U**T  or  A = L*D*L**T */

/*  where U (or L) is a product of permutation and unit upper (lower) */
/*  triangular matrices, and D is symmetric and block diagonal with */
/*  1-by-1 and 2-by-2 diagonal blocks. */

/*  This is the blocked version of the algorithm, calling Level 3 BLAS. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the leading */
/*          N-by-N upper triangular part of A contains the upper */
/*          triangular part of the matrix A, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading N-by-N lower triangular part of A contains the lower */
/*          triangular part of the matrix A, and the strictly upper */
/*          triangular part of A is not referenced. */

/*          On exit, the block diagonal matrix D and the multipliers used */
/*          to obtain the factor U or L (see below for further details). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  IPIV    (output) INTEGER array, dimension (N) */
/*          Details of the interchanges and the block structure of D. */
/*          If IPIV(k) > 0, then rows and columns k and IPIV(k) were */
/*          interchanged and D(k,k) is a 1-by-1 diagonal block. */
/*          If UPLO = 'U' and IPIV(k) = IPIV(k-1) < 0, then rows and */
/*          columns k-1 and -IPIV(k) were interchanged and D(k-1:k,k-1:k) */
/*          is a 2-by-2 diagonal block.  If UPLO = 'L' and IPIV(k) = */
/*          IPIV(k+1) < 0, then rows and columns k+1 and -IPIV(k) were */
/*          interchanged and D(k:k+1,k:k+1) is a 2-by-2 diagonal block. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The length of WORK.  LWORK >=1.  For best performance */
/*          LWORK >= N*NB, where NB is the block size returned by ILAENV. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, D(i,i) is exactly zero.  The factorization */
/*                has been completed, but the block diagonal matrix D is */
/*                exactly singular, and division by zero will occur if it */
/*                is used to solve a system of equations. */

/*  Further Details */
/*  =============== */

/*  If UPLO = 'U', then A = U*D*U', where */
/*     U = P(n)*U(n)* ... *P(k)U(k)* ..., */
/*  i.e., U is a product of terms P(k)*U(k), where k decreases from n to */
/*  1 in steps of 1 or 2, and D is a block diagonal matrix with 1-by-1 */
/*  and 2-by-2 diagonal blocks D(k).  P(k) is a permutation matrix as */
/*  defined by IPIV(k), and U(k) is a unit upper triangular matrix, such */
/*  that if the diagonal block D(k) is of order s (s = 1 or 2), then */

/*             (   I    v    0   )   k-s */
/*     U(k) =  (   0    I    0   )   s */
/*             (   0    0    I   )   n-k */
/*                k-s   s   n-k */

/*  If s = 1, D(k) overwrites A(k,k), and v overwrites A(1:k-1,k). */
/*  If s = 2, the upper triangle of D(k) overwrites A(k-1,k-1), A(k-1,k), */
/*  and A(k,k), and v overwrites A(1:k-2,k-1:k). */

/*  If UPLO = 'L', then A = L*D*L', where */
/*     L = P(1)*L(1)* ... *P(k)*L(k)* ..., */
/*  i.e., L is a product of terms P(k)*L(k), where k increases from 1 to */
/*  n in steps of 1 or 2, and D is a block diagonal matrix with 1-by-1 */
/*  and 2-by-2 diagonal blocks D(k).  P(k) is a permutation matrix as */
/*  defined by IPIV(k), and L(k) is a unit lower triangular matrix, such */
/*  that if the diagonal block D(k) is of order s (s = 1 or 2), then */

/*             (   I    0     0   )  k-1 */
/*     L(k) =  (   0    I     0   )  s */
/*             (   0    v     I   )  n-k-s+1 */
/*                k-1   s  n-k-s+1 */

/*  If s = 1, D(k) overwrites A(k,k), and v overwrites A(k+1:n,k). */
/*  If s = 2, the lower triangle of D(k) overwrites A(k,k), A(k+1,k), */
/*  and A(k+1,k+1), and v overwrites A(k+2:n,k:k+1). */

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

/*     Test the input parameters. */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --ipiv;
    --work;

    /* Function Body */
    *info = 0;
    upper = lsame_(uplo, "U");
    lquery = *lwork == -1;
    if (! upper && ! lsame_(uplo, "L")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*lda < std::max(1_integer,*n)) {
	*info = -4;
    } else if (*lwork < 1 && ! lquery) {
	*info = -7;
    }

    if (*info == 0) {

/*        Determine the block size */

	nb = ilaenv_(&c__1, "DSYTRF", uplo, n, &c_n1, &c_n1, &c_n1);
	lwkopt = *n * nb;
	work[1] = (double) lwkopt;
    }

    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DSYTRF", &i__1);
	return 0;
    } else if (lquery) {
	return 0;
    }

    nbmin = 2;
    ldwork = *n;
    if (nb > 1 && nb < *n) {
	iws = ldwork * nb;
	if (*lwork < iws) {
/* Computing MAX */
	    i__1 = *lwork / ldwork;
	    nb = std::max(i__1,1_integer);
/* Computing MAX */
	    i__1 = 2, i__2 = ilaenv_(&c__2, "DSYTRF", uplo, n, &c_n1, &c_n1, &
		    c_n1);
	    nbmin = std::max(i__1,i__2);
	}
    } else {
	iws = 1;
    }
    if (nb < nbmin) {
	nb = *n;
    }

    if (upper) {

/*        Factorize A as U*D*U' using the upper triangle of A */

/*        K is the main loop index, decreasing from N to 1 in steps of */
/*        KB, where KB is the number of columns factorized by DLASYF; */
/*        KB is either NB or NB-1, or K for the last block */

	k = *n;
L10:

/*        If K < 1, exit from loop */

	if (k < 1) {
	    goto L40;
	}

	if (k > nb) {

/*           Factorize columns k-kb+1:k of A and use blocked code to */
/*           update columns 1:k-kb */

	    dlasyf_(uplo, &k, &nb, &kb, &a[a_offset], lda, &ipiv[1], &work[1],
		     &ldwork, &iinfo);
	} else {

/*           Use unblocked code to factorize columns 1:k of A */

	    dsytf2_(uplo, &k, &a[a_offset], lda, &ipiv[1], &iinfo);
	    kb = k;
	}

/*        Set INFO on the first occurrence of a zero pivot */

	if (*info == 0 && iinfo > 0) {
	    *info = iinfo;
	}

/*        Decrease K and return to the start of the main loop */

	k -= kb;
	goto L10;

    } else {

/*        Factorize A as L*D*L' using the lower triangle of A */

/*        K is the main loop index, increasing from 1 to N in steps of */
/*        KB, where KB is the number of columns factorized by DLASYF; */
/*        KB is either NB or NB-1, or N-K+1 for the last block */

	k = 1;
L20:

/*        If K > N, exit from loop */

	if (k > *n) {
	    goto L40;
	}

	if (k <= *n - nb) {

/*           Factorize columns k:k+kb-1 of A and use blocked code to */
/*           update columns k+kb:n */

	    i__1 = *n - k + 1;
	    dlasyf_(uplo, &i__1, &nb, &kb, &a[k + k * a_dim1], lda, &ipiv[k],
		    &work[1], &ldwork, &iinfo);
	} else {

/*           Use unblocked code to factorize columns k:n of A */

	    i__1 = *n - k + 1;
	    dsytf2_(uplo, &i__1, &a[k + k * a_dim1], lda, &ipiv[k], &iinfo);
	    kb = *n - k + 1;
	}

/*        Set INFO on the first occurrence of a zero pivot */

	if (*info == 0 && iinfo > 0) {
	    *info = iinfo + k - 1;
	}

/*        Adjust IPIV */

	i__1 = k + kb - 1;
	for (j = k; j <= i__1; ++j) {
	    if (ipiv[j] > 0) {
		ipiv[j] = ipiv[j] + k - 1;
	    } else {
		ipiv[j] = ipiv[j] - k + 1;
	    }
/* L30: */
	}

/*        Increase K and return to the start of the main loop */

	k += kb;
	goto L20;

    }

L40:
    work[1] = (double) lwkopt;
    return 0;

/*     End of DSYTRF */

} /* dsytrf_ */

/* Subroutine */ int dsytri_(const char *uplo, integer *n, double *a, integer *
	lda, integer *ipiv, double *work, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b11 = -1.;
	static double c_b13 = 0.;

    /* System generated locals */
    integer a_dim1, a_offset, i__1;
    double d__1;

    /* Local variables */
    double d__;
    integer k;
    double t, ak;
    integer kp;
    double akp1;
    double temp, akkp1;
    integer kstep;
    bool upper;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSYTRI computes the inverse of a real symmetric indefinite matrix */
/*  A using the factorization A = U*D*U**T or A = L*D*L**T computed by */
/*  DSYTRF. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the details of the factorization are stored */
/*          as an upper or lower triangular matrix. */
/*          = 'U':  Upper triangular, form is A = U*D*U**T; */
/*          = 'L':  Lower triangular, form is A = L*D*L**T. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the block diagonal matrix D and the multipliers */
/*          used to obtain the factor U or L as computed by DSYTRF. */

/*          On exit, if INFO = 0, the (symmetric) inverse of the original */
/*          matrix.  If UPLO = 'U', the upper triangular part of the */
/*          inverse is formed and the part of A below the diagonal is not */
/*          referenced; if UPLO = 'L' the lower triangular part of the */
/*          inverse is formed and the part of A above the diagonal is */
/*          not referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          Details of the interchanges and the block structure of D */
/*          as determined by DSYTRF. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          > 0: if INFO = i, D(i,i) = 0; the matrix is singular and its */
/*               inverse could not be computed. */

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
	xerbla_("DSYTRI", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Check that the diagonal matrix D is nonsingular. */

    if (upper) {

/*        Upper triangular storage: examine D from bottom to top */

	for (*info = *n; *info >= 1; --(*info)) {
	    if (ipiv[*info] > 0 && a[*info + *info * a_dim1] == 0.) {
		return 0;
	    }
/* L10: */
	}
    } else {

/*        Lower triangular storage: examine D from top to bottom. */

	i__1 = *n;
	for (*info = 1; *info <= i__1; ++(*info)) {
	    if (ipiv[*info] > 0 && a[*info + *info * a_dim1] == 0.) {
		return 0;
	    }
/* L20: */
	}
    }
    *info = 0;

    if (upper) {

/*        Compute inv(A) from the factorization A = U*D*U'. */

/*        K is the main loop index, increasing from 1 to N in steps of */
/*        1 or 2, depending on the size of the diagonal blocks. */

	k = 1;
L30:

/*        If K > N, exit from loop. */

	if (k > *n) {
	    goto L40;
	}

	if (ipiv[k] > 0) {

/*           1 x 1 diagonal block */

/*           Invert the diagonal block. */

	    a[k + k * a_dim1] = 1. / a[k + k * a_dim1];

/*           Compute column K of the inverse. */

	    if (k > 1) {
		i__1 = k - 1;
		dcopy_(&i__1, &a[k * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__1 = k - 1;
		dsymv_(uplo, &i__1, &c_b11, &a[a_offset], lda, &work[1], &
			c__1, &c_b13, &a[k * a_dim1 + 1], &c__1);
		i__1 = k - 1;
		a[k + k * a_dim1] -= ddot_(&i__1, &work[1], &c__1, &a[k *
			a_dim1 + 1], &c__1);
	    }
	    kstep = 1;
	} else {

/*           2 x 2 diagonal block */

/*           Invert the diagonal block. */

	    t = (d__1 = a[k + (k + 1) * a_dim1], abs(d__1));
	    ak = a[k + k * a_dim1] / t;
	    akp1 = a[k + 1 + (k + 1) * a_dim1] / t;
	    akkp1 = a[k + (k + 1) * a_dim1] / t;
	    d__ = t * (ak * akp1 - 1.);
	    a[k + k * a_dim1] = akp1 / d__;
	    a[k + 1 + (k + 1) * a_dim1] = ak / d__;
	    a[k + (k + 1) * a_dim1] = -akkp1 / d__;

/*           Compute columns K and K+1 of the inverse. */

	    if (k > 1) {
		i__1 = k - 1;
		dcopy_(&i__1, &a[k * a_dim1 + 1], &c__1, &work[1], &c__1);
		i__1 = k - 1;
		dsymv_(uplo, &i__1, &c_b11, &a[a_offset], lda, &work[1], &
			c__1, &c_b13, &a[k * a_dim1 + 1], &c__1);
		i__1 = k - 1;
		a[k + k * a_dim1] -= ddot_(&i__1, &work[1], &c__1, &a[k *
			a_dim1 + 1], &c__1);
		i__1 = k - 1;
		a[k + (k + 1) * a_dim1] -= ddot_(&i__1, &a[k * a_dim1 + 1], &
			c__1, &a[(k + 1) * a_dim1 + 1], &c__1);
		i__1 = k - 1;
		dcopy_(&i__1, &a[(k + 1) * a_dim1 + 1], &c__1, &work[1], &
			c__1);
		i__1 = k - 1;
		dsymv_(uplo, &i__1, &c_b11, &a[a_offset], lda, &work[1], &
			c__1, &c_b13, &a[(k + 1) * a_dim1 + 1], &c__1);
		i__1 = k - 1;
		a[k + 1 + (k + 1) * a_dim1] -= ddot_(&i__1, &work[1], &c__1, &
			a[(k + 1) * a_dim1 + 1], &c__1);
	    }
	    kstep = 2;
	}

	kp = (i__1 = ipiv[k], abs(i__1));
	if (kp != k) {

/*           Interchange rows and columns K and KP in the leading */
/*           submatrix A(1:k+1,1:k+1) */

	    i__1 = kp - 1;
	    dswap_(&i__1, &a[k * a_dim1 + 1], &c__1, &a[kp * a_dim1 + 1], &
		    c__1);
	    i__1 = k - kp - 1;
	    dswap_(&i__1, &a[kp + 1 + k * a_dim1], &c__1, &a[kp + (kp + 1) *
		    a_dim1], lda);
	    temp = a[k + k * a_dim1];
	    a[k + k * a_dim1] = a[kp + kp * a_dim1];
	    a[kp + kp * a_dim1] = temp;
	    if (kstep == 2) {
		temp = a[k + (k + 1) * a_dim1];
		a[k + (k + 1) * a_dim1] = a[kp + (k + 1) * a_dim1];
		a[kp + (k + 1) * a_dim1] = temp;
	    }
	}

	k += kstep;
	goto L30;
L40:

	;
    } else {

/*        Compute inv(A) from the factorization A = L*D*L'. */

/*        K is the main loop index, increasing from 1 to N in steps of */
/*        1 or 2, depending on the size of the diagonal blocks. */

	k = *n;
L50:

/*        If K < 1, exit from loop. */

	if (k < 1) {
	    goto L60;
	}

	if (ipiv[k] > 0) {

/*           1 x 1 diagonal block */

/*           Invert the diagonal block. */

	    a[k + k * a_dim1] = 1. / a[k + k * a_dim1];

/*           Compute column K of the inverse. */

	    if (k < *n) {
		i__1 = *n - k;
		dcopy_(&i__1, &a[k + 1 + k * a_dim1], &c__1, &work[1], &c__1);
		i__1 = *n - k;
		dsymv_(uplo, &i__1, &c_b11, &a[k + 1 + (k + 1) * a_dim1], lda,
			 &work[1], &c__1, &c_b13, &a[k + 1 + k * a_dim1], &
			c__1);
		i__1 = *n - k;
		a[k + k * a_dim1] -= ddot_(&i__1, &work[1], &c__1, &a[k + 1 +
			k * a_dim1], &c__1);
	    }
	    kstep = 1;
	} else {

/*           2 x 2 diagonal block */

/*           Invert the diagonal block. */

	    t = (d__1 = a[k + (k - 1) * a_dim1], abs(d__1));
	    ak = a[k - 1 + (k - 1) * a_dim1] / t;
	    akp1 = a[k + k * a_dim1] / t;
	    akkp1 = a[k + (k - 1) * a_dim1] / t;
	    d__ = t * (ak * akp1 - 1.);
	    a[k - 1 + (k - 1) * a_dim1] = akp1 / d__;
	    a[k + k * a_dim1] = ak / d__;
	    a[k + (k - 1) * a_dim1] = -akkp1 / d__;

/*           Compute columns K-1 and K of the inverse. */

	    if (k < *n) {
		i__1 = *n - k;
		dcopy_(&i__1, &a[k + 1 + k * a_dim1], &c__1, &work[1], &c__1);
		i__1 = *n - k;
		dsymv_(uplo, &i__1, &c_b11, &a[k + 1 + (k + 1) * a_dim1], lda,
			 &work[1], &c__1, &c_b13, &a[k + 1 + k * a_dim1], &
			c__1);
		i__1 = *n - k;
		a[k + k * a_dim1] -= ddot_(&i__1, &work[1], &c__1, &a[k + 1 +
			k * a_dim1], &c__1);
		i__1 = *n - k;
		a[k + (k - 1) * a_dim1] -= ddot_(&i__1, &a[k + 1 + k * a_dim1]
, &c__1, &a[k + 1 + (k - 1) * a_dim1], &c__1);
		i__1 = *n - k;
		dcopy_(&i__1, &a[k + 1 + (k - 1) * a_dim1], &c__1, &work[1], &
			c__1);
		i__1 = *n - k;
		dsymv_(uplo, &i__1, &c_b11, &a[k + 1 + (k + 1) * a_dim1], lda,
			 &work[1], &c__1, &c_b13, &a[k + 1 + (k - 1) * a_dim1]
, &c__1);
		i__1 = *n - k;
		a[k - 1 + (k - 1) * a_dim1] -= ddot_(&i__1, &work[1], &c__1, &
			a[k + 1 + (k - 1) * a_dim1], &c__1);
	    }
	    kstep = 2;
	}

	kp = (i__1 = ipiv[k], abs(i__1));
	if (kp != k) {

/*           Interchange rows and columns K and KP in the trailing */
/*           submatrix A(k-1:n,k-1:n) */

	    if (kp < *n) {
		i__1 = *n - kp;
		dswap_(&i__1, &a[kp + 1 + k * a_dim1], &c__1, &a[kp + 1 + kp *
			 a_dim1], &c__1);
	    }
	    i__1 = kp - k - 1;
	    dswap_(&i__1, &a[k + 1 + k * a_dim1], &c__1, &a[kp + (k + 1) *
		    a_dim1], lda);
	    temp = a[k + k * a_dim1];
	    a[k + k * a_dim1] = a[kp + kp * a_dim1];
	    a[kp + kp * a_dim1] = temp;
	    if (kstep == 2) {
		temp = a[k + (k - 1) * a_dim1];
		a[k + (k - 1) * a_dim1] = a[kp + (k - 1) * a_dim1];
		a[kp + (k - 1) * a_dim1] = temp;
	    }
	}

	k -= kstep;
	goto L50;
L60:
	;
    }

    return 0;

/*     End of DSYTRI */

} /* dsytri_ */

/* Subroutine */ int dsytrs_(const char *uplo, integer *n, integer *nrhs,
	double *a, integer *lda, integer *ipiv, double *b, integer *
	ldb, integer *info)
{
	/* Table of constant values */
	static double c_b7 = -1.;
	static integer c__1 = 1;
	static double c_b19 = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1;
    double d__1;

    /* Local variables */
    integer j, k;
    double ak, bk;
    integer kp;
    double akm1, bkm1;
    double akm1k;
    double denom;
    bool upper;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSYTRS solves a system of linear equations A*X = B with a real */
/*  symmetric matrix A using the factorization A = U*D*U**T or */
/*  A = L*D*L**T computed by DSYTRF. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the details of the factorization are stored */
/*          as an upper or lower triangular matrix. */
/*          = 'U':  Upper triangular, form is A = U*D*U**T; */
/*          = 'L':  Lower triangular, form is A = L*D*L**T. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrix B.  NRHS >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The block diagonal matrix D and the multipliers used to */
/*          obtain the factor U or L as computed by DSYTRF. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  IPIV    (input) INTEGER array, dimension (N) */
/*          Details of the interchanges and the block structure of D */
/*          as determined by DSYTRF. */

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
    upper = lsame_(uplo, "U");
    if (! upper && ! lsame_(uplo, "L")) {
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
	xerbla_("DSYTRS", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0 || *nrhs == 0) {
	return 0;
    }

    if (upper) {

/*        Solve A*X = B, where A = U*D*U'. */

/*        First solve U*D*X = B, overwriting B with X. */

/*        K is the main loop index, decreasing from N to 1 in steps of */
/*        1 or 2, depending on the size of the diagonal blocks. */

	k = *n;
L10:

/*        If K < 1, exit from loop. */

	if (k < 1) {
	    goto L30;
	}

	if (ipiv[k] > 0) {

/*           1 x 1 diagonal block */

/*           Interchange rows K and IPIV(K). */

	    kp = ipiv[k];
	    if (kp != k) {
		dswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }

/*           Multiply by inv(U(K)), where U(K) is the transformation */
/*           stored in column K of A. */

	    i__1 = k - 1;
	    dger_(&i__1, nrhs, &c_b7, &a[k * a_dim1 + 1], &c__1, &b[k +
		    b_dim1], ldb, &b[b_dim1 + 1], ldb);

/*           Multiply by the inverse of the diagonal block. */

	    d__1 = 1. / a[k + k * a_dim1];
	    dscal_(nrhs, &d__1, &b[k + b_dim1], ldb);
	    --k;
	} else {

/*           2 x 2 diagonal block */

/*           Interchange rows K-1 and -IPIV(K). */

	    kp = -ipiv[k];
	    if (kp != k - 1) {
		dswap_(nrhs, &b[k - 1 + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }

/*           Multiply by inv(U(K)), where U(K) is the transformation */
/*           stored in columns K-1 and K of A. */

	    i__1 = k - 2;
	    dger_(&i__1, nrhs, &c_b7, &a[k * a_dim1 + 1], &c__1, &b[k +
		    b_dim1], ldb, &b[b_dim1 + 1], ldb);
	    i__1 = k - 2;
	    dger_(&i__1, nrhs, &c_b7, &a[(k - 1) * a_dim1 + 1], &c__1, &b[k -
		    1 + b_dim1], ldb, &b[b_dim1 + 1], ldb);

/*           Multiply by the inverse of the diagonal block. */

	    akm1k = a[k - 1 + k * a_dim1];
	    akm1 = a[k - 1 + (k - 1) * a_dim1] / akm1k;
	    ak = a[k + k * a_dim1] / akm1k;
	    denom = akm1 * ak - 1.;
	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		bkm1 = b[k - 1 + j * b_dim1] / akm1k;
		bk = b[k + j * b_dim1] / akm1k;
		b[k - 1 + j * b_dim1] = (ak * bkm1 - bk) / denom;
		b[k + j * b_dim1] = (akm1 * bk - bkm1) / denom;
/* L20: */
	    }
	    k += -2;
	}

	goto L10;
L30:

/*        Next solve U'*X = B, overwriting B with X. */

/*        K is the main loop index, increasing from 1 to N in steps of */
/*        1 or 2, depending on the size of the diagonal blocks. */

	k = 1;
L40:

/*        If K > N, exit from loop. */

	if (k > *n) {
	    goto L50;
	}

	if (ipiv[k] > 0) {

/*           1 x 1 diagonal block */

/*           Multiply by inv(U'(K)), where U(K) is the transformation */
/*           stored in column K of A. */

	    i__1 = k - 1;
	    dgemv_("Transpose", &i__1, nrhs, &c_b7, &b[b_offset], ldb, &a[k *
		    a_dim1 + 1], &c__1, &c_b19, &b[k + b_dim1], ldb);

/*           Interchange rows K and IPIV(K). */

	    kp = ipiv[k];
	    if (kp != k) {
		dswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }
	    ++k;
	} else {

/*           2 x 2 diagonal block */

/*           Multiply by inv(U'(K+1)), where U(K+1) is the transformation */
/*           stored in columns K and K+1 of A. */

	    i__1 = k - 1;
	    dgemv_("Transpose", &i__1, nrhs, &c_b7, &b[b_offset], ldb, &a[k *
		    a_dim1 + 1], &c__1, &c_b19, &b[k + b_dim1], ldb);
	    i__1 = k - 1;
	    dgemv_("Transpose", &i__1, nrhs, &c_b7, &b[b_offset], ldb, &a[(k
		    + 1) * a_dim1 + 1], &c__1, &c_b19, &b[k + 1 + b_dim1],
		    ldb);

/*           Interchange rows K and -IPIV(K). */

	    kp = -ipiv[k];
	    if (kp != k) {
		dswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }
	    k += 2;
	}

	goto L40;
L50:

	;
    } else {

/*        Solve A*X = B, where A = L*D*L'. */

/*        First solve L*D*X = B, overwriting B with X. */

/*        K is the main loop index, increasing from 1 to N in steps of */
/*        1 or 2, depending on the size of the diagonal blocks. */

	k = 1;
L60:

/*        If K > N, exit from loop. */

	if (k > *n) {
	    goto L80;
	}

	if (ipiv[k] > 0) {

/*           1 x 1 diagonal block */

/*           Interchange rows K and IPIV(K). */

	    kp = ipiv[k];
	    if (kp != k) {
		dswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }

/*           Multiply by inv(L(K)), where L(K) is the transformation */
/*           stored in column K of A. */

	    if (k < *n) {
		i__1 = *n - k;
		dger_(&i__1, nrhs, &c_b7, &a[k + 1 + k * a_dim1], &c__1, &b[k
			+ b_dim1], ldb, &b[k + 1 + b_dim1], ldb);
	    }

/*           Multiply by the inverse of the diagonal block. */

	    d__1 = 1. / a[k + k * a_dim1];
	    dscal_(nrhs, &d__1, &b[k + b_dim1], ldb);
	    ++k;
	} else {

/*           2 x 2 diagonal block */

/*           Interchange rows K+1 and -IPIV(K). */

	    kp = -ipiv[k];
	    if (kp != k + 1) {
		dswap_(nrhs, &b[k + 1 + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }

/*           Multiply by inv(L(K)), where L(K) is the transformation */
/*           stored in columns K and K+1 of A. */

	    if (k < *n - 1) {
		i__1 = *n - k - 1;
		dger_(&i__1, nrhs, &c_b7, &a[k + 2 + k * a_dim1], &c__1, &b[k
			+ b_dim1], ldb, &b[k + 2 + b_dim1], ldb);
		i__1 = *n - k - 1;
		dger_(&i__1, nrhs, &c_b7, &a[k + 2 + (k + 1) * a_dim1], &c__1,
			 &b[k + 1 + b_dim1], ldb, &b[k + 2 + b_dim1], ldb);
	    }

/*           Multiply by the inverse of the diagonal block. */

	    akm1k = a[k + 1 + k * a_dim1];
	    akm1 = a[k + k * a_dim1] / akm1k;
	    ak = a[k + 1 + (k + 1) * a_dim1] / akm1k;
	    denom = akm1 * ak - 1.;
	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		bkm1 = b[k + j * b_dim1] / akm1k;
		bk = b[k + 1 + j * b_dim1] / akm1k;
		b[k + j * b_dim1] = (ak * bkm1 - bk) / denom;
		b[k + 1 + j * b_dim1] = (akm1 * bk - bkm1) / denom;
/* L70: */
	    }
	    k += 2;
	}

	goto L60;
L80:

/*        Next solve L'*X = B, overwriting B with X. */

/*        K is the main loop index, decreasing from N to 1 in steps of */
/*        1 or 2, depending on the size of the diagonal blocks. */

	k = *n;
L90:

/*        If K < 1, exit from loop. */

	if (k < 1) {
	    goto L100;
	}

	if (ipiv[k] > 0) {

/*           1 x 1 diagonal block */

/*           Multiply by inv(L'(K)), where L(K) is the transformation */
/*           stored in column K of A. */

	    if (k < *n) {
		i__1 = *n - k;
		dgemv_("Transpose", &i__1, nrhs, &c_b7, &b[k + 1 + b_dim1],
			ldb, &a[k + 1 + k * a_dim1], &c__1, &c_b19, &b[k +
			b_dim1], ldb);
	    }

/*           Interchange rows K and IPIV(K). */

	    kp = ipiv[k];
	    if (kp != k) {
		dswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }
	    --k;
	} else {

/*           2 x 2 diagonal block */

/*           Multiply by inv(L'(K-1)), where L(K-1) is the transformation */
/*           stored in columns K-1 and K of A. */

	    if (k < *n) {
		i__1 = *n - k;
		dgemv_("Transpose", &i__1, nrhs, &c_b7, &b[k + 1 + b_dim1],
			ldb, &a[k + 1 + k * a_dim1], &c__1, &c_b19, &b[k +
			b_dim1], ldb);
		i__1 = *n - k;
		dgemv_("Transpose", &i__1, nrhs, &c_b7, &b[k + 1 + b_dim1],
			ldb, &a[k + 1 + (k - 1) * a_dim1], &c__1, &c_b19, &b[
			k - 1 + b_dim1], ldb);
	    }

/*           Interchange rows K and -IPIV(K). */

	    kp = -ipiv[k];
	    if (kp != k) {
		dswap_(nrhs, &b[k + b_dim1], ldb, &b[kp + b_dim1], ldb);
	    }
	    k += -2;
	}

	goto L90;
L100:
	;
    }

    return 0;

/*     End of DSYTRS */

} /* dsytrs_ */
