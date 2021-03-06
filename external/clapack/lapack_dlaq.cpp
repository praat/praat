#include "clapack.h"
#include "f2cP.h"

double dla_gbrcond__(const char *trans, integer *n, integer *kl, integer *ku,
	double *ab, integer *ldab, double *afb, integer *ldafb,
	integer *ipiv, integer *cmode, double *c__, integer *info,
	double *work, integer *iwork, integer trans_len)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer ab_dim1, ab_offset, afb_dim1, afb_offset, i__1, i__2, i__3, i__4;
    double ret_val, d__1;

    /* Local variables */
    integer i__, j, kd, ke;
    double tmp;
    integer kase;
    integer isave[3];
    double ainvnm;
    bool notrans;


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

/*     DLA_GERCOND Estimates the Skeel condition number of  op(A) * op2(C) */
/*     where op2 is determined by CMODE as follows */
/*     CMODE =  1    op2(C) = C */
/*     CMODE =  0    op2(C) = I */
/*     CMODE = -1    op2(C) = inv(C) */
/*     The Skeel condition number  cond(A) = norminf( |inv(A)||A| ) */
/*     is computed by computing scaling factors R such that */
/*     diag(R)*A*op2(C) is row equilibrated and computing the standard */
/*     infinity-norm condition number. */

/*  Arguments */
/*  ========= */

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

/*     AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*     On entry, the matrix A in band storage, in rows 1 to KL+KU+1. */
/*     The j-th column of A is stored in the j-th column of the */
/*     array AB as follows: */
/*     AB(KU+1+i-j,j) = A(i,j) for max(1,j-KU)<=i<=min(N,j+kl) */

/*     LDAB    (input) INTEGER */
/*     The leading dimension of the array AB.  LDAB >= KL+KU+1. */

/*     AFB     (input) DOUBLE PRECISION array, dimension (LDAFB,N) */
/*     Details of the LU factorization of the band matrix A, as */
/*     computed by DGBTRF.  U is stored as an upper triangular */
/*     band matrix with KL+KU superdiagonals in rows 1 to KL+KU+1, */
/*     and the multipliers used during the factorization are stored */
/*     in rows KL+KU+2 to 2*KL+KU+1. */

/*     LDAFB   (input) INTEGER */
/*     The leading dimension of the array AFB.  LDAFB >= 2*KL+KU+1. */

/*     IPIV    (input) INTEGER array, dimension (N) */
/*     The pivot indices from the factorization A = P*L*U */
/*     as computed by DGBTRF; row i of the matrix was interchanged */
/*     with row IPIV(i). */

/*     CMODE   (input) INTEGER */
/*     Determines op2(C) in the formula op(A) * op2(C) as follows: */
/*     CMODE =  1    op2(C) = C */
/*     CMODE =  0    op2(C) = I */
/*     CMODE = -1    op2(C) = inv(C) */

/*     C       (input) DOUBLE PRECISION array, dimension (N) */
/*     The vector C in the formula op(A) * op2(C). */

/*     INFO    (output) INTEGER */
/*       = 0:  Successful exit. */
/*     i > 0:  The ith argument is invalid. */

/*     WORK    (input) DOUBLE PRECISION array, dimension (5*N). */
/*     Workspace. */

/*     IWORK   (input) INTEGER array, dimension (N). */
/*     Workspace. */

/*  ===================================================================== */

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
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    afb_dim1 = *ldafb;
    afb_offset = 1 + afb_dim1;
    afb -= afb_offset;
    --ipiv;
    --c__;
    --work;
    --iwork;

    /* Function Body */
    ret_val = 0.;

    *info = 0;
    notrans = lsame_(trans, "N");
    if (! notrans && ! lsame_(trans, "T") && ! lsame_(trans, "C")) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*kl < 0 || *kl > *n - 1) {
	*info = -3;
    } else if (*ku < 0 || *ku > *n - 1) {
	*info = -4;
    } else if (*ldab < *kl + *ku + 1) {
	*info = -6;
    } else if (*ldafb < (*kl << 1) + *ku + 1) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLA_GBRCOND", &i__1);
	return ret_val;
    }
    if (*n == 0) {
	ret_val = 1.;
	return ret_val;
    }

/*     Compute the equilibration matrix R such that */
/*     inv(R)*A*C has unit 1-norm. */

    kd = *ku + 1;
    ke = *kl + 1;
    if (notrans) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    tmp = 0.;
	    if (*cmode == 1) {
/* Computing MAX */
		i__2 = i__ - *kl;
/* Computing MIN */
		i__4 = i__ + *ku;
		i__3 = std::min(i__4,*n);
		for (j = std::max(i__2,1_integer); j <= i__3; ++j) {
		    tmp += (d__1 = ab[kd + i__ - j + j * ab_dim1] * c__[j],
			    abs(d__1));
		}
	    } else if (*cmode == 0) {
/* Computing MAX */
		i__3 = i__ - *kl;
/* Computing MIN */
		i__4 = i__ + *ku;
		i__2 = std::min(i__4,*n);
		for (j = std::max(i__3,1_integer); j <= i__2; ++j) {
		    tmp += (d__1 = ab[kd + i__ - j + j * ab_dim1], abs(d__1));
		}
	    } else {
/* Computing MAX */
		i__2 = i__ - *kl;
/* Computing MIN */
		i__4 = i__ + *ku;
		i__3 = std::min(i__4,*n);
		for (j = std::max(i__2,1_integer); j <= i__3; ++j) {
		    tmp += (d__1 = ab[kd + i__ - j + j * ab_dim1] / c__[j],
			    abs(d__1));
		}
	    }
	    work[(*n << 1) + i__] = tmp;
	}
    } else {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    tmp = 0.;
	    if (*cmode == 1) {
/* Computing MAX */
		i__3 = i__ - *kl;
/* Computing MIN */
		i__4 = i__ + *ku;
		i__2 = std::min(i__4,*n);
		for (j = std::max(i__3,1_integer); j <= i__2; ++j) {
		    tmp += (d__1 = ab[ke - i__ + j + i__ * ab_dim1] * c__[j],
			    abs(d__1));
		}
	    } else if (*cmode == 0) {
/* Computing MAX */
		i__2 = i__ - *kl;
/* Computing MIN */
		i__4 = i__ + *ku;
		i__3 = std::min(i__4,*n);
		for (j = std::max(i__2,1_integer); j <= i__3; ++j) {
		    tmp += (d__1 = ab[ke - i__ + j + i__ * ab_dim1], abs(d__1)
			    );
		}
	    } else {
/* Computing MAX */
		i__3 = i__ - *kl;
/* Computing MIN */
		i__4 = i__ + *ku;
		i__2 = std::min(i__4,*n);
		for (j = std::max(i__3,1_integer); j <= i__2; ++j) {
		    tmp += (d__1 = ab[ke - i__ + j + i__ * ab_dim1] / c__[j],
			    abs(d__1));
		}
	    }
	    work[(*n << 1) + i__] = tmp;
	}
    }

/*     Estimate the norm of inv(op(A)). */

    ainvnm = 0.;
    kase = 0;
L10:
    dlacn2_(n, &work[*n + 1], &work[1], &iwork[1], &ainvnm, &kase, isave);
    if (kase != 0) {
	if (kase == 2) {

/*           Multiply by R. */

	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		work[i__] *= work[(*n << 1) + i__];
	    }
	    if (notrans) {
		dgbtrs_("No transpose", n, kl, ku, &c__1, &afb[afb_offset],
			ldafb, &ipiv[1], &work[1], n, info);
	    } else {
		dgbtrs_("Transpose", n, kl, ku, &c__1, &afb[afb_offset],
			ldafb, &ipiv[1], &work[1], n, info);
	    }

/*           Multiply by inv(C). */

	    if (*cmode == 1) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] /= c__[i__];
		}
	    } else if (*cmode == -1) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] *= c__[i__];
		}
	    }
	} else {

/*           Multiply by inv(C'). */

	    if (*cmode == 1) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] /= c__[i__];
		}
	    } else if (*cmode == -1) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] *= c__[i__];
		}
	    }
	    if (notrans) {
		dgbtrs_("Transpose", n, kl, ku, &c__1, &afb[afb_offset],
			ldafb, &ipiv[1], &work[1], n, info);
	    } else {
		dgbtrs_("No transpose", n, kl, ku, &c__1, &afb[afb_offset],
			ldafb, &ipiv[1], &work[1], n, info);
	    }

/*           Multiply by R. */

	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		work[i__] *= work[(*n << 1) + i__];
	    }
	}
	goto L10;
    }

/*     Compute the estimate of the reciprocal condition number. */

    if (ainvnm != 0.) {
	ret_val = 1. / ainvnm;
    }

    return ret_val;

} /* dla_gbrcond__ */

#if 0
/* Subroutine */ int dla_gbrfsx_extended__(integer *prec_type__, integer *
	trans_type__, integer *n, integer *kl, integer *ku, integer *nrhs,
	double *ab, integer *ldab, double *afb, integer *ldafb,
	integer *ipiv, bool *colequ, double *c__, double *b,
	integer *ldb, double *y, integer *ldy, double *berr_out__,
	integer *n_norms__, double *err_bnds_norm__, double *
	err_bnds_comp__, double *res, double *ayb, double *dy,
	double *y_tail__, double *rcond, integer *ithresh, double
	*rthresh, double *dz_ub__, bool *ignore_cwise__, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b6 = -1.;
	static double c_b8 = 1.;

    /* System generated locals */
    integer ab_dim1, ab_offset, afb_dim1, afb_offset, b_dim1, b_offset,
	    y_dim1, y_offset, err_bnds_norm_dim1, err_bnds_norm_offset,
	    err_bnds_comp_dim1, err_bnds_comp_offset, i__1, i__2, i__3;
    double d__1, d__2;
    char ch__1[1];

    /* Local variables */
    double dxratmax, dzratmax;
    integer i__, j, m;
    bool incr_prec__;
    double prev_dz_z__, yk, final_dx_x__;
    double final_dz_z__, prevnormdx;
    integer cnt;
    double dyk, eps, incr_thresh__, dx_x__, dz_z__;
    double ymin;
    integer y_prec_state__;
    double dxrat, dzrat;
    char trans[1];
    double normx, normy;
    double normdx;
    double hugeval;
    integer x_state__, z_state__;


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

/*  Purpose */
/*  ======= */

/*  DLA_GBRFSX_EXTENDED improves the computed solution to a system of */
/*  linear equations by performing extra-precise iterative refinement */
/*  and provides error bounds and backward error estimates for the solution. */
/*  This subroutine is called by DGBRFSX to perform iterative refinement. */
/*  In addition to normwise error bound, the code provides maximum */
/*  componentwise error bound if possible. See comments for ERR_BNDS_NORM */
/*  and ERR_BNDS_COMP for details of the error bounds. Note that this */
/*  subroutine is only resonsible for setting the second fields of */
/*  ERR_BNDS_NORM and ERR_BNDS_COMP. */

/*  Arguments */
/*  ========= */

/*     PREC_TYPE      (input) INTEGER */
/*     Specifies the intermediate precision to be used in refinement. */
/*     The value is defined by ILAPREC(P) where P is a CHARACTER and */
/*     P    = 'S':  Single */
/*          = 'D':  Double */
/*          = 'I':  Indigenous */
/*          = 'X', 'E':  Extra */

/*     TRANS_TYPE     (input) INTEGER */
/*     Specifies the transposition operation on A. */
/*     The value is defined by ILATRANS(T) where T is a CHARACTER and */
/*     T    = 'N':  No transpose */
/*          = 'T':  Transpose */
/*          = 'C':  Conjugate transpose */

/*     N              (input) INTEGER */
/*     The number of linear equations, i.e., the order of the */
/*     matrix A.  N >= 0. */

/*     KL             (input) INTEGER */
/*     The number of subdiagonals within the band of A.  KL >= 0. */

/*     KU             (input) INTEGER */
/*     The number of superdiagonals within the band of A.  KU >= 0 */

/*     NRHS           (input) INTEGER */
/*     The number of right-hand-sides, i.e., the number of columns of the */
/*     matrix B. */

/*     A              (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*     On entry, the N-by-N matrix A. */

/*     LDA            (input) INTEGER */
/*     The leading dimension of the array A.  LDA >= max(1,N). */

/*     AF             (input) DOUBLE PRECISION array, dimension (LDAF,N) */
/*     The factors L and U from the factorization */
/*     A = P*L*U as computed by DGBTRF. */

/*     LDAF           (input) INTEGER */
/*     The leading dimension of the array AF.  LDAF >= max(1,N). */

/*     IPIV           (input) INTEGER array, dimension (N) */
/*     The pivot indices from the factorization A = P*L*U */
/*     as computed by DGBTRF; row i of the matrix was interchanged */
/*     with row IPIV(i). */

/*     COLEQU         (input) LOGICAL */
/*     If .TRUE. then column equilibration was done to A before calling */
/*     this routine. This is needed to compute the solution and error */
/*     bounds correctly. */

/*     C              (input) DOUBLE PRECISION array, dimension (N) */
/*     The column scale factors for A. If COLEQU = .FALSE., C */
/*     is not accessed. If C is input, each element of C should be a power */
/*     of the radix to ensure a reliable solution and error estimates. */
/*     Scaling by powers of the radix does not cause rounding errors unless */
/*     the result underflows or overflows. Rounding errors during scaling */
/*     lead to refining with a matrix that is not equivalent to the */
/*     input matrix, producing error estimates that may not be */
/*     reliable. */

/*     B              (input) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*     The right-hand-side matrix B. */

/*     LDB            (input) INTEGER */
/*     The leading dimension of the array B.  LDB >= max(1,N). */

/*     Y              (input/output) DOUBLE PRECISION array, dimension */
/*                    (LDY,NRHS) */
/*     On entry, the solution matrix X, as computed by DGBTRS. */
/*     On exit, the improved solution matrix Y. */

/*     LDY            (input) INTEGER */
/*     The leading dimension of the array Y.  LDY >= max(1,N). */

/*     BERR_OUT       (output) DOUBLE PRECISION array, dimension (NRHS) */
/*     On exit, BERR_OUT(j) contains the componentwise relative backward */
/*     error for right-hand-side j from the formula */
/*         max(i) ( abs(RES(i)) / ( abs(op(A_s))*abs(Y) + abs(B_s) )(i) ) */
/*     where abs(Z) is the componentwise absolute value of the matrix */
/*     or vector Z. This is computed by DLA_LIN_BERR. */

/*     N_NORMS        (input) INTEGER */
/*     Determines which error bounds to return (see ERR_BNDS_NORM */
/*     and ERR_BNDS_COMP). */
/*     If N_NORMS >= 1 return normwise error bounds. */
/*     If N_NORMS >= 2 return componentwise error bounds. */

/*     ERR_BNDS_NORM  (input/output) DOUBLE PRECISION array, dimension */
/*                    (NRHS, N_ERR_BNDS) */
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
/*              sqrt(n) * slamch('Epsilon'). */

/*     err = 2 "Guaranteed" error bound: The estimated forward error, */
/*              almost certainly within a factor of 10 of the true error */
/*              so long as the next entry is greater than the threshold */
/*              sqrt(n) * slamch('Epsilon'). This error bound should only */
/*              be trusted if the previous boolean is true. */

/*     err = 3  Reciprocal condition number: Estimated normwise */
/*              reciprocal condition number.  Compared with the threshold */
/*              sqrt(n) * slamch('Epsilon') to determine if the error */
/*              estimate is "guaranteed". These reciprocal condition */
/*              numbers are 1 / (norm(Z^{-1},inf) * norm(Z,inf)) for some */
/*              appropriately scaled matrix Z. */
/*              Let Z = S*A, where S scales each row by a power of the */
/*              radix so all absolute row sums of Z are approximately 1. */

/*     This subroutine is only responsible for setting the second field */
/*     above. */
/*     See Lapack Working Note 165 for further details and extra */
/*     cautions. */

/*     ERR_BNDS_COMP  (input/output) DOUBLE PRECISION array, dimension */
/*                    (NRHS, N_ERR_BNDS) */
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
/*              sqrt(n) * slamch('Epsilon'). */

/*     err = 2 "Guaranteed" error bound: The estimated forward error, */
/*              almost certainly within a factor of 10 of the true error */
/*              so long as the next entry is greater than the threshold */
/*              sqrt(n) * slamch('Epsilon'). This error bound should only */
/*              be trusted if the previous boolean is true. */

/*     err = 3  Reciprocal condition number: Estimated componentwise */
/*              reciprocal condition number.  Compared with the threshold */
/*              sqrt(n) * slamch('Epsilon') to determine if the error */
/*              estimate is "guaranteed". These reciprocal condition */
/*              numbers are 1 / (norm(Z^{-1},inf) * norm(Z,inf)) for some */
/*              appropriately scaled matrix Z. */
/*              Let Z = S*(A*diag(x)), where x is the solution for the */
/*              current right-hand side and S scales each row of */
/*              A*diag(x) by a power of the radix so all absolute row */
/*              sums of Z are approximately 1. */

/*     This subroutine is only responsible for setting the second field */
/*     above. */
/*     See Lapack Working Note 165 for further details and extra */
/*     cautions. */

/*     RES            (input) DOUBLE PRECISION array, dimension (N) */
/*     Workspace to hold the intermediate residual. */

/*     AYB            (input) DOUBLE PRECISION array, dimension (N) */
/*     Workspace. This can be the same workspace passed for Y_TAIL. */

/*     DY             (input) DOUBLE PRECISION array, dimension (N) */
/*     Workspace to hold the intermediate solution. */

/*     Y_TAIL         (input) DOUBLE PRECISION array, dimension (N) */
/*     Workspace to hold the trailing bits of the intermediate solution. */

/*     RCOND          (input) DOUBLE PRECISION */
/*     Reciprocal scaled condition number.  This is an estimate of the */
/*     reciprocal Skeel condition number of the matrix A after */
/*     equilibration (if done).  If this is less than the machine */
/*     precision (in particular, if it is zero), the matrix is singular */
/*     to working precision.  Note that the error may still be small even */
/*     if this number is very small and the matrix appears ill- */
/*     conditioned. */

/*     ITHRESH        (input) INTEGER */
/*     The maximum number of residual computations allowed for */
/*     refinement. The default is 10. For 'aggressive' set to 100 to */
/*     permit convergence using approximate factorizations or */
/*     factorizations other than LU. If the factorization uses a */
/*     technique other than Gaussian elimination, the guarantees in */
/*     ERR_BNDS_NORM and ERR_BNDS_COMP may no longer be trustworthy. */

/*     RTHRESH        (input) DOUBLE PRECISION */
/*     Determines when to stop refinement if the error estimate stops */
/*     decreasing. Refinement will stop when the next solution no longer */
/*     satisfies norm(dx_{i+1}) < RTHRESH * norm(dx_i) where norm(Z) is */
/*     the infinity norm of Z. RTHRESH satisfies 0 < RTHRESH <= 1. The */
/*     default value is 0.5. For 'aggressive' set to 0.9 to permit */
/*     convergence on extremely ill-conditioned matrices. See LAWN 165 */
/*     for more details. */

/*     DZ_UB          (input) DOUBLE PRECISION */
/*     Determines when to start considering componentwise convergence. */
/*     Componentwise convergence is only considered after each component */
/*     of the solution Y is stable, which we definte as the relative */
/*     change in each component being less than DZ_UB. The default value */
/*     is 0.25, requiring the first bit to be stable. See LAWN 165 for */
/*     more details. */

/*     IGNORE_CWISE   (input) LOGICAL */
/*     If .TRUE. then ignore componentwise convergence. Default value */
/*     is .FALSE.. */

/*     INFO           (output) INTEGER */
/*       = 0:  Successful exit. */
/*       < 0:  if INFO = -i, the ith argument to DGBTRS had an illegal */
/*             value */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Parameters .. */
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
    --c__;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    y_dim1 = *ldy;
    y_offset = 1 + y_dim1;
    y -= y_offset;
    --berr_out__;
    --res;
    --ayb;
    --dy;
    --y_tail__;

    /* Function Body */
    if (*info != 0) {
	return 0;
    }
    chla_transtype__(ch__1, 1_integer, trans_type__);
    *(unsigned char *)trans = *(unsigned char *)&ch__1[0];
    eps = dlamch_("Epsilon");
    hugeval = dlamch_("Overflow");
/*     Force HUGEVAL to Inf */
    hugeval *= hugeval;
/*     Using HUGEVAL may lead to spurious underflows. */
    incr_thresh__ = (double) (*n) * eps;
    m = *kl + *ku + 1;
    i__1 = *nrhs;
    for (j = 1; j <= i__1; ++j) {
	y_prec_state__ = 1;
	if (y_prec_state__ == 2) {
	    i__2 = *n;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		y_tail__[i__] = 0.;
	    }
	}
	dxrat = 0.;
	dxratmax = 0.;
	dzrat = 0.;
	dzratmax = 0.;
	final_dx_x__ = hugeval;
	final_dz_z__ = hugeval;
	prevnormdx = hugeval;
	prev_dz_z__ = hugeval;
	dz_z__ = hugeval;
	dx_x__ = hugeval;
	x_state__ = 1;
	z_state__ = 0;
	incr_prec__ = false;
	i__2 = *ithresh;
	for (cnt = 1; cnt <= i__2; ++cnt) {

/*        Compute residual RES = B_s - op(A_s) * Y, */
/*            op(A) = A, A**T, or A**H depending on TRANS (and type). */

	    dcopy_(n, &b[j * b_dim1 + 1], &c__1, &res[1], &c__1);
	    if (y_prec_state__ == 0) {
		dgbmv_(trans, &m, n, kl, ku, &c_b6, &ab[ab_offset], ldab, &y[
			j * y_dim1 + 1], &c__1, &c_b8, &res[1], &c__1);
	    } else if (y_prec_state__ == 1) {
		blas_dgbmv_x__(trans_type__, n, n, kl, ku, &c_b6, &ab[
			ab_offset], ldab, &y[j * y_dim1 + 1], &c__1, &c_b8, &
			res[1], &c__1, prec_type__);
	    } else {
		blas_dgbmv2_x__(trans_type__, n, n, kl, ku, &c_b6, &ab[
			ab_offset], ldab, &y[j * y_dim1 + 1], &y_tail__[1], &
			c__1, &c_b8, &res[1], &c__1, prec_type__);
	    }
/*        XXX: RES is no longer needed. */
	    dcopy_(n, &res[1], &c__1, &dy[1], &c__1);
	    dgbtrs_(trans, n, kl, ku, &c__1, &afb[afb_offset], ldafb, &ipiv[1]
, &dy[1], n, info);

/*         Calculate relative changes DX_X, DZ_Z and ratios DXRAT, DZRAT. */

	    normx = 0.;
	    normy = 0.;
	    normdx = 0.;
	    dz_z__ = 0.;
	    ymin = hugeval;
	    i__3 = *n;
	    for (i__ = 1; i__ <= i__3; ++i__) {
		yk = (d__1 = y[i__ + j * y_dim1], abs(d__1));
		dyk = (d__1 = dy[i__], abs(d__1));
		if (yk != 0.) {
/* Computing MAX */
		    d__1 = dz_z__, d__2 = dyk / yk;
		    dz_z__ = std::max(d__1,d__2);
		} else if (dyk != 0.) {
		    dz_z__ = hugeval;
		}
		ymin = std::min(ymin,yk);
		normy = std::max(normy,yk);
		if (*colequ) {
/* Computing MAX */
		    d__1 = normx, d__2 = yk * c__[i__];
		    normx = std::max(d__1,d__2);
/* Computing MAX */
		    d__1 = normdx, d__2 = dyk * c__[i__];
		    normdx = std::max(d__1,d__2);
		} else {
		    normx = normy;
		    normdx = std::max(normdx,dyk);
		}
	    }
	    if (normx != 0.) {
		dx_x__ = normdx / normx;
	    } else if (normdx == 0.) {
		dx_x__ = 0.;
	    } else {
		dx_x__ = hugeval;
	    }
	    dxrat = normdx / prevnormdx;
	    dzrat = dz_z__ / prev_dz_z__;

/*         Check termination criteria. */

	    if (! (*ignore_cwise__) && ymin * *rcond < incr_thresh__ * normy
		    && y_prec_state__ < 2) {
		incr_prec__ = true;
	    }
	    if (x_state__ == 3 && dxrat <= *rthresh) {
		x_state__ = 1;
	    }
	    if (x_state__ == 1) {
		if (dx_x__ <= eps) {
		    x_state__ = 2;
		} else if (dxrat > *rthresh) {
		    if (y_prec_state__ != 2) {
			incr_prec__ = true;
		    } else {
			x_state__ = 3;
		    }
		} else {
		    if (dxrat > dxratmax) {
			dxratmax = dxrat;
		    }
		}
		if (x_state__ > 1) {
		    final_dx_x__ = dx_x__;
		}
	    }
	    if (z_state__ == 0 && dz_z__ <= *dz_ub__) {
		z_state__ = 1;
	    }
	    if (z_state__ == 3 && dzrat <= *rthresh) {
		z_state__ = 1;
	    }
	    if (z_state__ == 1) {
		if (dz_z__ <= eps) {
		    z_state__ = 2;
		} else if (dz_z__ > *dz_ub__) {
		    z_state__ = 0;
		    dzratmax = 0.;
		    final_dz_z__ = hugeval;
		} else if (dzrat > *rthresh) {
		    if (y_prec_state__ != 2) {
			incr_prec__ = true;
		    } else {
			z_state__ = 3;
		    }
		} else {
		    if (dzrat > dzratmax) {
			dzratmax = dzrat;
		    }
		}
		if (z_state__ > 1) {
		    final_dz_z__ = dz_z__;
		}
	    }

/*           Exit if both normwise and componentwise stopped working, */
/*           but if componentwise is unstable, let it go at least two */
/*           iterations. */

	    if (x_state__ != 1) {
		if (*ignore_cwise__) {
		    goto L666;
		}
		if (z_state__ == 3 || z_state__ == 2) {
		    goto L666;
		}
		if (z_state__ == 0 && cnt > 1) {
		    goto L666;
		}
	    }
	    if (incr_prec__) {
		incr_prec__ = false;
		++y_prec_state__;
		i__3 = *n;
		for (i__ = 1; i__ <= i__3; ++i__) {
		    y_tail__[i__] = 0.;
		}
	    }
	    prevnormdx = normdx;
	    prev_dz_z__ = dz_z__;

/*           Update soluton. */

	    if (y_prec_state__ < 2) {
		daxpy_(n, &c_b8, &dy[1], &c__1, &y[j * y_dim1 + 1], &c__1);
	    } else {
		dla_wwaddw__(n, &y[j * y_dim1 + 1], &y_tail__[1], &dy[1]);
	    }
	}
/*        Target of "IF (Z_STOP .AND. X_STOP)".  Sun's f77 won't EXIT. */
L666:

/*     Set final_* when cnt hits ithresh. */

	if (x_state__ == 1) {
	    final_dx_x__ = dx_x__;
	}
	if (z_state__ == 1) {
	    final_dz_z__ = dz_z__;
	}

/*     Compute error bounds. */

	if (*n_norms__ >= 1) {
	    err_bnds_norm__[j + (err_bnds_norm_dim1 << 1)] = final_dx_x__ / (
		    1 - dxratmax);
	}
	if (*n_norms__ >= 2) {
	    err_bnds_comp__[j + (err_bnds_comp_dim1 << 1)] = final_dz_z__ / (
		    1 - dzratmax);
	}

/*     Compute componentwise relative backward error from formula */
/*         max(i) ( abs(R(i)) / ( abs(op(A_s))*abs(Y) + abs(B_s) )(i) ) */
/*     where abs(Z) is the componentwise absolute value of the matrix */
/*     or vector Z. */

/*        Compute residual RES = B_s - op(A_s) * Y, */
/*            op(A) = A, A**T, or A**H depending on TRANS (and type). */

	dcopy_(n, &b[j * b_dim1 + 1], &c__1, &res[1], &c__1);
	dgbmv_(trans, n, n, kl, ku, &c_b6, &ab[ab_offset], ldab, &y[j *
		y_dim1 + 1], &c__1, &c_b8, &res[1], &c__1);
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    ayb[i__] = (d__1 = b[i__ + j * b_dim1], abs(d__1));
	}

/*     Compute abs(op(A_s))*abs(Y) + abs(B_s). */

	dla_gbamv__(trans_type__, n, n, kl, ku, &c_b8, &ab[ab_offset], ldab, &
		y[j * y_dim1 + 1], &c__1, &c_b8, &ayb[1], &c__1);
	dla_lin_berr__(n, n, &c__1, &res[1], &ayb[1], &berr_out__[j]);

/*     End of loop for each RHS */

    }

    return 0;
} /* dla_gbrfsx_extended__ */
#endif

double dla_gbrpvgrw__(integer *n, integer *kl, integer *ku, integer *
	ncols, double *ab, integer *ldab, double *afb, integer *ldafb)
{
    /* System generated locals */
    integer ab_dim1, ab_offset, afb_dim1, afb_offset, i__1, i__2, i__3, i__4;
    double ret_val, d__1, d__2;

    /* Local variables */
    integer i__, j, kd;
    double amax, umax, rpvgrw;


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

/*  Purpose */
/*  ======= */

/*  DLA_GBRPVGRW computes the reciprocal pivot growth factor */
/*  norm(A)/norm(U). The "max absolute element" norm is used. If this is */
/*  much less than 1, the stability of the LU factorization of the */
/*  (equilibrated) matrix A could be poor. This also means that the */
/*  solution X, estimated condition numbers, and error bounds could be */
/*  unreliable. */

/*  Arguments */
/*  ========= */

/*     N       (input) INTEGER */
/*     The number of linear equations, i.e., the order of the */
/*     matrix A.  N >= 0. */

/*     KL      (input) INTEGER */
/*     The number of subdiagonals within the band of A.  KL >= 0. */

/*     KU      (input) INTEGER */
/*     The number of superdiagonals within the band of A.  KU >= 0. */

/*     NCOLS   (input) INTEGER */
/*     The number of columns of the matrix A.  NCOLS >= 0. */

/*     AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*     On entry, the matrix A in band storage, in rows 1 to KL+KU+1. */
/*     The j-th column of A is stored in the j-th column of the */
/*     array AB as follows: */
/*     AB(KU+1+i-j,j) = A(i,j) for max(1,j-KU)<=i<=min(N,j+kl) */

/*     LDAB    (input) INTEGER */
/*     The leading dimension of the array AB.  LDAB >= KL+KU+1. */

/*     AFB     (input) DOUBLE PRECISION array, dimension (LDAFB,N) */
/*     Details of the LU factorization of the band matrix A, as */
/*     computed by DGBTRF.  U is stored as an upper triangular */
/*     band matrix with KL+KU superdiagonals in rows 1 to KL+KU+1, */
/*     and the multipliers used during the factorization are stored */
/*     in rows KL+KU+2 to 2*KL+KU+1. */

/*     LDAFB   (input) INTEGER */
/*     The leading dimension of the array AFB.  LDAFB >= 2*KL+KU+1. */

/*  ===================================================================== */

/*     .. Local Scalars .. */
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

    /* Function Body */
    rpvgrw = 1.;
    kd = *ku + 1;
    i__1 = *ncols;
    for (j = 1; j <= i__1; ++j) {
	amax = 0.;
	umax = 0.;
/* Computing MAX */
	i__2 = j - *ku;
/* Computing MIN */
	i__4 = j + *kl;
	i__3 = std::min(i__4,*n);
	for (i__ = std::max(i__2,1_integer); i__ <= i__3; ++i__) {
/* Computing MAX */
	    d__2 = (d__1 = ab[kd + i__ - j + j * ab_dim1], abs(d__1));
	    amax = std::max(d__2,amax);
	}
/* Computing MAX */
	i__3 = j - *ku;
	i__2 = j;
	for (i__ = std::max(i__3,1_integer); i__ <= i__2; ++i__) {
/* Computing MAX */
	    d__2 = (d__1 = afb[kd + i__ - j + j * afb_dim1], abs(d__1));
	    umax = std::max(d__2,umax);
	}
	if (umax != 0.) {
/* Computing MIN */
	    d__1 = amax / umax;
	    rpvgrw = std::min(d__1,rpvgrw);
	}
    }
    ret_val = rpvgrw;
    return ret_val;
} /* dla_gbrpvgrw__ */

double dla_porcond__(const char *uplo, integer *n, double *a, integer *lda,
	double *af, integer *ldaf, integer *cmode, double *c__,
	integer *info, double *work, integer *iwork, integer uplo_len)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, af_dim1, af_offset, i__1, i__2;
    double ret_val, d__1;

    /* Local variables */
    integer i__, j;
    bool up;
    double tmp;
    integer kase;
    integer isave[3];
    double ainvnm;


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

/*  Purpose */
/*  ======= */

/*     DLA_PORCOND Estimates the Skeel condition number of  op(A) * op2(C) */
/*     where op2 is determined by CMODE as follows */
/*     CMODE =  1    op2(C) = C */
/*     CMODE =  0    op2(C) = I */
/*     CMODE = -1    op2(C) = inv(C) */
/*     The Skeel condition number  cond(A) = norminf( |inv(A)||A| ) */
/*     is computed by computing scaling factors R such that */
/*     diag(R)*A*op2(C) is row equilibrated and computing the standard */
/*     infinity-norm condition number. */

/*  Arguments */
/*  ========== */

/*     UPLO    (input) CHARACTER*1 */
/*       = 'U':  Upper triangle of A is stored; */
/*       = 'L':  Lower triangle of A is stored. */

/*     N       (input) INTEGER */
/*     The number of linear equations, i.e., the order of the */
/*     matrix A.  N >= 0. */

/*     A       (input) REAL array, dimension (LDA,N) */
/*     On entry, the N-by-N matrix A. */

/*     LDA     (input) INTEGER */
/*     The leading dimension of the array A.  LDA >= max(1,N). */

/*     AF      (input) DOUBLE PRECISION array, dimension (LDAF,N) */
/*     The triangular factor U or L from the Cholesky factorization */
/*     A = U**T*U or A = L*L**T, as computed by DPOTRF. */

/*     LDAF    (input) INTEGER */
/*     The leading dimension of the array AF.  LDAF >= max(1,N). */

/*     CMODE   (input) INTEGER */
/*     Determines op2(C) in the formula op(A) * op2(C) as follows: */
/*     CMODE =  1    op2(C) = C */
/*     CMODE =  0    op2(C) = I */
/*     CMODE = -1    op2(C) = inv(C) */

/*     C       (input) DOUBLE PRECISION array, dimension (N) */
/*     The vector C in the formula op(A) * op2(C). */

/*     INFO    (output) INTEGER */
/*       = 0:  Successful exit. */
/*     i > 0:  The ith argument is invalid. */

/*     WORK    (input) DOUBLE PRECISION array, dimension (3*N). */
/*     Workspace. */

/*     IWORK   (input) INTEGER array, dimension (N). */
/*     Workspace. */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Array Arguments .. */
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
    --c__;
    --work;
    --iwork;

    /* Function Body */
    ret_val = 0.;

    *info = 0;
    if (*n < 0) {
	*info = -2;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLA_PORCOND", &i__1);
	return ret_val;
    }
    if (*n == 0) {
	ret_val = 1.;
	return ret_val;
    }
    up = false;
    if (lsame_(uplo, "U")) {
	up = true;
    }

/*     Compute the equilibration matrix R such that */
/*     inv(R)*A*C has unit 1-norm. */

    if (up) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    tmp = 0.;
	    if (*cmode == 1) {
		i__2 = i__;
		for (j = 1; j <= i__2; ++j) {
		    tmp += (d__1 = a[j + i__ * a_dim1] * c__[j], abs(d__1));
		}
		i__2 = *n;
		for (j = i__ + 1; j <= i__2; ++j) {
		    tmp += (d__1 = a[i__ + j * a_dim1] * c__[j], abs(d__1));
		}
	    } else if (*cmode == 0) {
		i__2 = i__;
		for (j = 1; j <= i__2; ++j) {
		    tmp += (d__1 = a[j + i__ * a_dim1], abs(d__1));
		}
		i__2 = *n;
		for (j = i__ + 1; j <= i__2; ++j) {
		    tmp += (d__1 = a[i__ + j * a_dim1], abs(d__1));
		}
	    } else {
		i__2 = i__;
		for (j = 1; j <= i__2; ++j) {
		    tmp += (d__1 = a[j + i__ * a_dim1] / c__[j], abs(d__1));
		}
		i__2 = *n;
		for (j = i__ + 1; j <= i__2; ++j) {
		    tmp += (d__1 = a[i__ + j * a_dim1] / c__[j], abs(d__1));
		}
	    }
	    work[(*n << 1) + i__] = tmp;
	}
    } else {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    tmp = 0.;
	    if (*cmode == 1) {
		i__2 = i__;
		for (j = 1; j <= i__2; ++j) {
		    tmp += (d__1 = a[i__ + j * a_dim1] * c__[j], abs(d__1));
		}
		i__2 = *n;
		for (j = i__ + 1; j <= i__2; ++j) {
		    tmp += (d__1 = a[j + i__ * a_dim1] * c__[j], abs(d__1));
		}
	    } else if (*cmode == 0) {
		i__2 = i__;
		for (j = 1; j <= i__2; ++j) {
		    tmp += (d__1 = a[i__ + j * a_dim1], abs(d__1));
		}
		i__2 = *n;
		for (j = i__ + 1; j <= i__2; ++j) {
		    tmp += (d__1 = a[j + i__ * a_dim1], abs(d__1));
		}
	    } else {
		i__2 = i__;
		for (j = 1; j <= i__2; ++j) {
		    tmp += (d__1 = a[i__ + j * a_dim1] / c__[j], abs(d__1));
		}
		i__2 = *n;
		for (j = i__ + 1; j <= i__2; ++j) {
		    tmp += (d__1 = a[j + i__ * a_dim1] / c__[j], abs(d__1));
		}
	    }
	    work[(*n << 1) + i__] = tmp;
	}
    }

/*     Estimate the norm of inv(op(A)). */

    ainvnm = 0.;
    kase = 0;
L10:
    dlacn2_(n, &work[*n + 1], &work[1], &iwork[1], &ainvnm, &kase, isave);
    if (kase != 0) {
	if (kase == 2) {

/*           Multiply by R. */

	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		work[i__] *= work[(*n << 1) + i__];
	    }
	    if (up) {
		dpotrs_("Upper", n, &c__1, &af[af_offset], ldaf, &work[1], n,
			info);
	    } else {
		dpotrs_("Lower", n, &c__1, &af[af_offset], ldaf, &work[1], n,
			info);
	    }

/*           Multiply by inv(C). */

	    if (*cmode == 1) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] /= c__[i__];
		}
	    } else if (*cmode == -1) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] *= c__[i__];
		}
	    }
	} else {

/*           Multiply by inv(C'). */

	    if (*cmode == 1) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] /= c__[i__];
		}
	    } else if (*cmode == -1) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] *= c__[i__];
		}
	    }
	    if (up) {
		dpotrs_("Upper", n, &c__1, &af[af_offset], ldaf, &work[1], n,
			info);
	    } else {
		dpotrs_("Lower", n, &c__1, &af[af_offset], ldaf, &work[1], n,
			info);
	    }

/*           Multiply by R. */

	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		work[i__] *= work[(*n << 1) + i__];
	    }
	}
	goto L10;
    }

/*     Compute the estimate of the reciprocal condition number. */

    if (ainvnm != 0.) {
	ret_val = 1. / ainvnm;
    }

    return ret_val;

} /* dla_porcond__ */

int dla_wwaddw__(integer *n, double *x, double *y, double *w)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__;
    double s;


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

/*     Purpose */
/*     ======= */

/*     DLA_WWADDW adds a vector W into a doubled-single vector (X, Y). */

/*     This works for all extant IBM's hex and binary floating point */
/*     arithmetics, but not for decimal. */

/*     Arguments */
/*     ========= */

/*     N      (input) INTEGER */
/*            The length of vectors X, Y, and W. */

/*     X, Y   (input/output) DOUBLE PRECISION array, length N */
/*            The doubled-single accumulation vector. */

/*     W      (input) DOUBLE PRECISION array, length N */
/*            The vector to be added. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --w;
    --y;
    --x;

    /* Function Body */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	s = x[i__] + w[i__];
	s = s + s - s;
	y[i__] = x[i__] - s + w[i__] + y[i__];
	x[i__] = s;
/* L10: */
    }
    return 0;
} /* dla_wwaddw__ */

/* Subroutine */ int dlabad_(double *small, double *large)
{
    /* Builtin functions
    double d_lg10(double *), sqrt(double);*/


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLABAD takes as input the values computed by DLAMCH for underflow and */
/*  overflow, and returns the square root of each of these values if the */
/*  log of LARGE is sufficiently large.  This subroutine is intended to */
/*  identify machines with a large exponent range, such as the Crays, and */
/*  redefine the underflow and overflow limits to be the square roots of */
/*  the values computed by DLAMCH.  This subroutine is needed because */
/*  DLAMCH does not compensate for poor arithmetic in the upper half of */
/*  the exponent range, as is found on a Cray. */

/*  Arguments */
/*  ========= */

/*  SMALL   (input/output) DOUBLE PRECISION */
/*          On entry, the underflow threshold as computed by DLAMCH. */
/*          On exit, if LOG10(LARGE) is sufficiently large, the square */
/*          root of SMALL, otherwise unchanged. */

/*  LARGE   (input/output) DOUBLE PRECISION */
/*          On entry, the overflow threshold as computed by DLAMCH. */
/*          On exit, if LOG10(LARGE) is sufficiently large, the square */
/*          root of LARGE, otherwise unchanged. */

/*  ===================================================================== */

/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     If it looks like we're on a Cray, take the square root of */
/*     SMALL and LARGE to avoid overflow and underflow problems. */

    if (d_lg10(large) > 2e3) {
	*small = sqrt(*small);
	*large = sqrt(*large);
    }

    return 0;

/*     End of DLABAD */

} /* dlabad_ */

/* Subroutine */ int dlabrd_(integer *m, integer *n, integer *nb, double *
	a, integer *lda, double *d__, double *e, double *tauq,
	double *taup, double *x, integer *ldx, double *y, integer
	*ldy)
{
	/* Table of constant values */
	static double c_b4 = -1.;
	static double c_b5 = 1.;
	static integer c__1 = 1;
	static double c_b16 = 0.;

    /* System generated locals */
    integer a_dim1, a_offset, x_dim1, x_offset, y_dim1, y_offset, i__1, i__2,
	    i__3;

    /* Local variables */
    integer i__;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLABRD reduces the first NB rows and columns of a real general */
/*  m by n matrix A to upper or lower bidiagonal form by an orthogonal */
/*  transformation Q' * A * P, and returns the matrices X and Y which */
/*  are needed to apply the transformation to the unreduced part of A. */

/*  If m >= n, A is reduced to upper bidiagonal form; if m < n, to lower */
/*  bidiagonal form. */

/*  This is an auxiliary routine called by DGEBRD */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows in the matrix A. */

/*  N       (input) INTEGER */
/*          The number of columns in the matrix A. */

/*  NB      (input) INTEGER */
/*          The number of leading rows and columns of A to be reduced. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the m by n general matrix to be reduced. */
/*          On exit, the first NB rows and columns of the matrix are */
/*          overwritten; the rest of the array is unchanged. */
/*          If m >= n, elements on and below the diagonal in the first NB */
/*            columns, with the array TAUQ, represent the orthogonal */
/*            matrix Q as a product of elementary reflectors; and */
/*            elements above the diagonal in the first NB rows, with the */
/*            array TAUP, represent the orthogonal matrix P as a product */
/*            of elementary reflectors. */
/*          If m < n, elements below the diagonal in the first NB */
/*            columns, with the array TAUQ, represent the orthogonal */
/*            matrix Q as a product of elementary reflectors, and */
/*            elements on and above the diagonal in the first NB rows, */
/*            with the array TAUP, represent the orthogonal matrix P as */
/*            a product of elementary reflectors. */
/*          See Further Details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  D       (output) DOUBLE PRECISION array, dimension (NB) */
/*          The diagonal elements of the first NB rows and columns of */
/*          the reduced matrix.  D(i) = A(i,i). */

/*  E       (output) DOUBLE PRECISION array, dimension (NB) */
/*          The off-diagonal elements of the first NB rows and columns of */
/*          the reduced matrix. */

/*  TAUQ    (output) DOUBLE PRECISION array dimension (NB) */
/*          The scalar factors of the elementary reflectors which */
/*          represent the orthogonal matrix Q. See Further Details. */

/*  TAUP    (output) DOUBLE PRECISION array, dimension (NB) */
/*          The scalar factors of the elementary reflectors which */
/*          represent the orthogonal matrix P. See Further Details. */

/*  X       (output) DOUBLE PRECISION array, dimension (LDX,NB) */
/*          The m-by-nb matrix X required to update the unreduced part */
/*          of A. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X. LDX >= M. */

/*  Y       (output) DOUBLE PRECISION array, dimension (LDY,NB) */
/*          The n-by-nb matrix Y required to update the unreduced part */
/*          of A. */

/*  LDY     (input) INTEGER */
/*          The leading dimension of the array Y. LDY >= N. */

/*  Further Details */
/*  =============== */

/*  The matrices Q and P are represented as products of elementary */
/*  reflectors: */

/*     Q = H(1) H(2) . . . H(nb)  and  P = G(1) G(2) . . . G(nb) */

/*  Each H(i) and G(i) has the form: */

/*     H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u' */

/*  where tauq and taup are real scalars, and v and u are real vectors. */

/*  If m >= n, v(1:i-1) = 0, v(i) = 1, and v(i:m) is stored on exit in */
/*  A(i:m,i); u(1:i) = 0, u(i+1) = 1, and u(i+1:n) is stored on exit in */
/*  A(i,i+1:n); tauq is stored in TAUQ(i) and taup in TAUP(i). */

/*  If m < n, v(1:i) = 0, v(i+1) = 1, and v(i+1:m) is stored on exit in */
/*  A(i+2:m,i); u(1:i-1) = 0, u(i) = 1, and u(i:n) is stored on exit in */
/*  A(i,i+1:n); tauq is stored in TAUQ(i) and taup in TAUP(i). */

/*  The elements of the vectors v and u together form the m-by-nb matrix */
/*  V and the nb-by-n matrix U' which are needed, with X and Y, to apply */
/*  the transformation to the unreduced part of the matrix, using a block */
/*  update of the form:  A := A - V*Y' - X*U'. */

/*  The contents of A on exit are illustrated by the following examples */
/*  with nb = 2: */

/*  m = 6 and n = 5 (m > n):          m = 5 and n = 6 (m < n): */

/*    (  1   1   u1  u1  u1 )           (  1   u1  u1  u1  u1  u1 ) */
/*    (  v1  1   1   u2  u2 )           (  1   1   u2  u2  u2  u2 ) */
/*    (  v1  v2  a   a   a  )           (  v1  1   a   a   a   a  ) */
/*    (  v1  v2  a   a   a  )           (  v1  v2  a   a   a   a  ) */
/*    (  v1  v2  a   a   a  )           (  v1  v2  a   a   a   a  ) */
/*    (  v1  v2  a   a   a  ) */

/*  where a denotes an element of the original matrix which is unchanged, */
/*  vi denotes an element of the vector defining H(i), and ui an element */
/*  of the vector defining G(i). */

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

/*     Quick return if possible */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --d__;
    --e;
    --tauq;
    --taup;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    y_dim1 = *ldy;
    y_offset = 1 + y_dim1;
    y -= y_offset;

    /* Function Body */
    if (*m <= 0 || *n <= 0) {
	return 0;
    }

    if (*m >= *n) {

/*        Reduce to upper bidiagonal form */

	i__1 = *nb;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Update A(i:m,i) */

	    i__2 = *m - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("No transpose", &i__2, &i__3, &c_b4, &a[i__ + a_dim1], lda,
		     &y[i__ + y_dim1], ldy, &c_b5, &a[i__ + i__ * a_dim1], &
		    c__1);
	    i__2 = *m - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("No transpose", &i__2, &i__3, &c_b4, &x[i__ + x_dim1], ldx,
		     &a[i__ * a_dim1 + 1], &c__1, &c_b5, &a[i__ + i__ *
		    a_dim1], &c__1);

/*           Generate reflection Q(i) to annihilate A(i+1:m,i) */

	    i__2 = *m - i__ + 1;
/* Computing MIN */
	    i__3 = i__ + 1;
	    dlarfg_(&i__2, &a[i__ + i__ * a_dim1], &a[std::min(i__3, *m)+ i__ *
		    a_dim1], &c__1, &tauq[i__]);
	    d__[i__] = a[i__ + i__ * a_dim1];
	    if (i__ < *n) {
		a[i__ + i__ * a_dim1] = 1.;

/*              Compute Y(i+1:n,i) */

		i__2 = *m - i__ + 1;
		i__3 = *n - i__;
		dgemv_("Transpose", &i__2, &i__3, &c_b5, &a[i__ + (i__ + 1) *
			a_dim1], lda, &a[i__ + i__ * a_dim1], &c__1, &c_b16, &
			y[i__ + 1 + i__ * y_dim1], &c__1);
		i__2 = *m - i__ + 1;
		i__3 = i__ - 1;
		dgemv_("Transpose", &i__2, &i__3, &c_b5, &a[i__ + a_dim1],
			lda, &a[i__ + i__ * a_dim1], &c__1, &c_b16, &y[i__ *
			y_dim1 + 1], &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b4, &y[i__ + 1 +
			y_dim1], ldy, &y[i__ * y_dim1 + 1], &c__1, &c_b5, &y[
			i__ + 1 + i__ * y_dim1], &c__1);
		i__2 = *m - i__ + 1;
		i__3 = i__ - 1;
		dgemv_("Transpose", &i__2, &i__3, &c_b5, &x[i__ + x_dim1],
			ldx, &a[i__ + i__ * a_dim1], &c__1, &c_b16, &y[i__ *
			y_dim1 + 1], &c__1);
		i__2 = i__ - 1;
		i__3 = *n - i__;
		dgemv_("Transpose", &i__2, &i__3, &c_b4, &a[(i__ + 1) *
			a_dim1 + 1], lda, &y[i__ * y_dim1 + 1], &c__1, &c_b5,
			&y[i__ + 1 + i__ * y_dim1], &c__1);
		i__2 = *n - i__;
		dscal_(&i__2, &tauq[i__], &y[i__ + 1 + i__ * y_dim1], &c__1);

/*              Update A(i,i+1:n) */

		i__2 = *n - i__;
		dgemv_("No transpose", &i__2, &i__, &c_b4, &y[i__ + 1 +
			y_dim1], ldy, &a[i__ + a_dim1], lda, &c_b5, &a[i__ + (
			i__ + 1) * a_dim1], lda);
		i__2 = i__ - 1;
		i__3 = *n - i__;
		dgemv_("Transpose", &i__2, &i__3, &c_b4, &a[(i__ + 1) *
			a_dim1 + 1], lda, &x[i__ + x_dim1], ldx, &c_b5, &a[
			i__ + (i__ + 1) * a_dim1], lda);

/*              Generate reflection P(i) to annihilate A(i,i+2:n) */

		i__2 = *n - i__;
/* Computing MIN */
		i__3 = i__ + 2;
		dlarfg_(&i__2, &a[i__ + (i__ + 1) * a_dim1], &a[i__ + std::min(
			i__3, *n)* a_dim1], lda, &taup[i__]);
		e[i__] = a[i__ + (i__ + 1) * a_dim1];
		a[i__ + (i__ + 1) * a_dim1] = 1.;

/*              Compute X(i+1:m,i) */

		i__2 = *m - i__;
		i__3 = *n - i__;
		dgemv_("No transpose", &i__2, &i__3, &c_b5, &a[i__ + 1 + (i__
			+ 1) * a_dim1], lda, &a[i__ + (i__ + 1) * a_dim1],
			lda, &c_b16, &x[i__ + 1 + i__ * x_dim1], &c__1);
		i__2 = *n - i__;
		dgemv_("Transpose", &i__2, &i__, &c_b5, &y[i__ + 1 + y_dim1],
			ldy, &a[i__ + (i__ + 1) * a_dim1], lda, &c_b16, &x[
			i__ * x_dim1 + 1], &c__1);
		i__2 = *m - i__;
		dgemv_("No transpose", &i__2, &i__, &c_b4, &a[i__ + 1 +
			a_dim1], lda, &x[i__ * x_dim1 + 1], &c__1, &c_b5, &x[
			i__ + 1 + i__ * x_dim1], &c__1);
		i__2 = i__ - 1;
		i__3 = *n - i__;
		dgemv_("No transpose", &i__2, &i__3, &c_b5, &a[(i__ + 1) *
			a_dim1 + 1], lda, &a[i__ + (i__ + 1) * a_dim1], lda, &
			c_b16, &x[i__ * x_dim1 + 1], &c__1);
		i__2 = *m - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b4, &x[i__ + 1 +
			x_dim1], ldx, &x[i__ * x_dim1 + 1], &c__1, &c_b5, &x[
			i__ + 1 + i__ * x_dim1], &c__1);
		i__2 = *m - i__;
		dscal_(&i__2, &taup[i__], &x[i__ + 1 + i__ * x_dim1], &c__1);
	    }
/* L10: */
	}
    } else {

/*        Reduce to lower bidiagonal form */

	i__1 = *nb;
	for (i__ = 1; i__ <= i__1; ++i__) {

/*           Update A(i,i:n) */

	    i__2 = *n - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("No transpose", &i__2, &i__3, &c_b4, &y[i__ + y_dim1], ldy,
		     &a[i__ + a_dim1], lda, &c_b5, &a[i__ + i__ * a_dim1],
		    lda);
	    i__2 = i__ - 1;
	    i__3 = *n - i__ + 1;
	    dgemv_("Transpose", &i__2, &i__3, &c_b4, &a[i__ * a_dim1 + 1],
		    lda, &x[i__ + x_dim1], ldx, &c_b5, &a[i__ + i__ * a_dim1],
		     lda);

/*           Generate reflection P(i) to annihilate A(i,i+1:n) */

	    i__2 = *n - i__ + 1;
/* Computing MIN */
	    i__3 = i__ + 1;
	    dlarfg_(&i__2, &a[i__ + i__ * a_dim1], &a[i__ + std::min(i__3, *n)*
		    a_dim1], lda, &taup[i__]);
	    d__[i__] = a[i__ + i__ * a_dim1];
	    if (i__ < *m) {
		a[i__ + i__ * a_dim1] = 1.;

/*              Compute X(i+1:m,i) */

		i__2 = *m - i__;
		i__3 = *n - i__ + 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b5, &a[i__ + 1 + i__ *
			 a_dim1], lda, &a[i__ + i__ * a_dim1], lda, &c_b16, &
			x[i__ + 1 + i__ * x_dim1], &c__1);
		i__2 = *n - i__ + 1;
		i__3 = i__ - 1;
		dgemv_("Transpose", &i__2, &i__3, &c_b5, &y[i__ + y_dim1],
			ldy, &a[i__ + i__ * a_dim1], lda, &c_b16, &x[i__ *
			x_dim1 + 1], &c__1);
		i__2 = *m - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b4, &a[i__ + 1 +
			a_dim1], lda, &x[i__ * x_dim1 + 1], &c__1, &c_b5, &x[
			i__ + 1 + i__ * x_dim1], &c__1);
		i__2 = i__ - 1;
		i__3 = *n - i__ + 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b5, &a[i__ * a_dim1 +
			1], lda, &a[i__ + i__ * a_dim1], lda, &c_b16, &x[i__ *
			 x_dim1 + 1], &c__1);
		i__2 = *m - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b4, &x[i__ + 1 +
			x_dim1], ldx, &x[i__ * x_dim1 + 1], &c__1, &c_b5, &x[
			i__ + 1 + i__ * x_dim1], &c__1);
		i__2 = *m - i__;
		dscal_(&i__2, &taup[i__], &x[i__ + 1 + i__ * x_dim1], &c__1);

/*              Update A(i+1:m,i) */

		i__2 = *m - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b4, &a[i__ + 1 +
			a_dim1], lda, &y[i__ + y_dim1], ldy, &c_b5, &a[i__ +
			1 + i__ * a_dim1], &c__1);
		i__2 = *m - i__;
		dgemv_("No transpose", &i__2, &i__, &c_b4, &x[i__ + 1 +
			x_dim1], ldx, &a[i__ * a_dim1 + 1], &c__1, &c_b5, &a[
			i__ + 1 + i__ * a_dim1], &c__1);

/*              Generate reflection Q(i) to annihilate A(i+2:m,i) */

		i__2 = *m - i__;
/* Computing MIN */
		i__3 = i__ + 2;
		dlarfg_(&i__2, &a[i__ + 1 + i__ * a_dim1], &a[std::min(i__3, *m)+
			i__ * a_dim1], &c__1, &tauq[i__]);
		e[i__] = a[i__ + 1 + i__ * a_dim1];
		a[i__ + 1 + i__ * a_dim1] = 1.;

/*              Compute Y(i+1:n,i) */

		i__2 = *m - i__;
		i__3 = *n - i__;
		dgemv_("Transpose", &i__2, &i__3, &c_b5, &a[i__ + 1 + (i__ +
			1) * a_dim1], lda, &a[i__ + 1 + i__ * a_dim1], &c__1,
			&c_b16, &y[i__ + 1 + i__ * y_dim1], &c__1);
		i__2 = *m - i__;
		i__3 = i__ - 1;
		dgemv_("Transpose", &i__2, &i__3, &c_b5, &a[i__ + 1 + a_dim1],
			 lda, &a[i__ + 1 + i__ * a_dim1], &c__1, &c_b16, &y[
			i__ * y_dim1 + 1], &c__1);
		i__2 = *n - i__;
		i__3 = i__ - 1;
		dgemv_("No transpose", &i__2, &i__3, &c_b4, &y[i__ + 1 +
			y_dim1], ldy, &y[i__ * y_dim1 + 1], &c__1, &c_b5, &y[
			i__ + 1 + i__ * y_dim1], &c__1);
		i__2 = *m - i__;
		dgemv_("Transpose", &i__2, &i__, &c_b5, &x[i__ + 1 + x_dim1],
			ldx, &a[i__ + 1 + i__ * a_dim1], &c__1, &c_b16, &y[
			i__ * y_dim1 + 1], &c__1);
		i__2 = *n - i__;
		dgemv_("Transpose", &i__, &i__2, &c_b4, &a[(i__ + 1) * a_dim1
			+ 1], lda, &y[i__ * y_dim1 + 1], &c__1, &c_b5, &y[i__
			+ 1 + i__ * y_dim1], &c__1);
		i__2 = *n - i__;
		dscal_(&i__2, &tauq[i__], &y[i__ + 1 + i__ * y_dim1], &c__1);
	    }
/* L20: */
	}
    }
    return 0;

/*     End of DLABRD */

} /* dlabrd_ */

/* Subroutine */ int dlacn2_(integer *n, double *v, double *x,
	integer *isgn, double *est, integer *kase, integer *isave)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b11 = 1.;

    /* System generated locals */
    integer i__1;
    double d__1;

    /* Local variables */
    integer i__;
    double temp;
    integer jlast;
    double altsgn, estold;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLACN2 estimates the 1-norm of a square, real matrix A. */
/*  Reverse communication is used for evaluating matrix-vector products. */

/*  Arguments */
/*  ========= */

/*  N      (input) INTEGER */
/*         The order of the matrix.  N >= 1. */

/*  V      (workspace) DOUBLE PRECISION array, dimension (N) */
/*         On the final return, V = A*W,  where  EST = norm(V)/norm(W) */
/*         (W is not returned). */

/*  X      (input/output) DOUBLE PRECISION array, dimension (N) */
/*         On an intermediate return, X should be overwritten by */
/*               A * X,   if KASE=1, */
/*               A' * X,  if KASE=2, */
/*         and DLACN2 must be re-called with all the other parameters */
/*         unchanged. */

/*  ISGN   (workspace) INTEGER array, dimension (N) */

/*  EST    (input/output) DOUBLE PRECISION */
/*         On entry with KASE = 1 or 2 and ISAVE(1) = 3, EST should be */
/*         unchanged from the previous call to DLACN2. */
/*         On exit, EST is an estimate (a lower bound) for norm(A). */

/*  KASE   (input/output) INTEGER */
/*         On the initial call to DLACN2, KASE should be 0. */
/*         On an intermediate return, KASE will be 1 or 2, indicating */
/*         whether X should be overwritten by A * X  or A' * X. */
/*         On the final return from DLACN2, KASE will again be 0. */

/*  ISAVE  (input/output) INTEGER array, dimension (3) */
/*         ISAVE is used to save variables between calls to DLACN2 */

/*  Further Details */
/*  ======= ======= */

/*  Contributed by Nick Higham, University of Manchester. */
/*  Originally named SONEST, dated March 16, 1988. */

/*  Reference: N.J. Higham, "FORTRAN codes for estimating the one-norm of */
/*  a real or complex matrix, with applications to condition estimation", */
/*  ACM Trans. Math. Soft., vol. 14, no. 4, pp. 381-396, December 1988. */

/*  This is a thread safe version of DLACON, which uses the array ISAVE */
/*  in place of a SAVE statement, as follows: */

/*     DLACON     DLACN2 */
/*      JUMP     ISAVE(1) */
/*      J        ISAVE(2) */
/*      ITER     ISAVE(3) */

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
    --isave;
    --isgn;
    --x;
    --v;

    /* Function Body */
    if (*kase == 0) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    x[i__] = 1. / (double) (*n);
/* L10: */
	}
	*kase = 1;
	isave[1] = 1;
	return 0;
    }

    switch (isave[1]) {
	case 1:  goto L20;
	case 2:  goto L40;
	case 3:  goto L70;
	case 4:  goto L110;
	case 5:  goto L140;
    }

/*     ................ ENTRY   (ISAVE( 1 ) = 1) */
/*     FIRST ITERATION.  X HAS BEEN OVERWRITTEN BY A*X. */

L20:
    if (*n == 1) {
	v[1] = x[1];
	*est = abs(v[1]);
/*        ... QUIT */
	goto L150;
    }
    *est = dasum_(n, &x[1], &c__1);

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	x[i__] = d_sign(&c_b11, &x[i__]);
	isgn[i__] = i_dnnt(&x[i__]);
/* L30: */
    }
    *kase = 2;
    isave[1] = 2;
    return 0;

/*     ................ ENTRY   (ISAVE( 1 ) = 2) */
/*     FIRST ITERATION.  X HAS BEEN OVERWRITTEN BY TRANSPOSE(A)*X. */

L40:
    isave[2] = idamax_(n, &x[1], &c__1);
    isave[3] = 2;

/*     MAIN LOOP - ITERATIONS 2,3,...,ITMAX. */

L50:
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	x[i__] = 0.;
/* L60: */
    }
    x[isave[2]] = 1.;
    *kase = 1;
    isave[1] = 3;
    return 0;

/*     ................ ENTRY   (ISAVE( 1 ) = 3) */
/*     X HAS BEEN OVERWRITTEN BY A*X. */

L70:
    dcopy_(n, &x[1], &c__1, &v[1], &c__1);
    estold = *est;
    *est = dasum_(n, &v[1], &c__1);
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__1 = d_sign(&c_b11, &x[i__]);
	if (i_dnnt(&d__1) != isgn[i__]) {
	    goto L90;
	}
/* L80: */
    }
/*     REPEATED SIGN VECTOR DETECTED, HENCE ALGORITHM HAS CONVERGED. */
    goto L120;

L90:
/*     TEST FOR CYCLING. */
    if (*est <= estold) {
	goto L120;
    }

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	x[i__] = d_sign(&c_b11, &x[i__]);
	isgn[i__] = i_dnnt(&x[i__]);
/* L100: */
    }
    *kase = 2;
    isave[1] = 4;
    return 0;

/*     ................ ENTRY   (ISAVE( 1 ) = 4) */
/*     X HAS BEEN OVERWRITTEN BY TRANSPOSE(A)*X. */

L110:
    jlast = isave[2];
    isave[2] = idamax_(n, &x[1], &c__1);
    if (x[jlast] != (d__1 = x[isave[2]], abs(d__1)) && isave[3] < 5) {
	++isave[3];
	goto L50;
    }

/*     ITERATION COMPLETE.  FINAL STAGE. */

L120:
    altsgn = 1.;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	x[i__] = altsgn * ((double) (i__ - 1) / (double) (*n - 1) +
		1.);
	altsgn = -altsgn;
/* L130: */
    }
    *kase = 1;
    isave[1] = 5;
    return 0;

/*     ................ ENTRY   (ISAVE( 1 ) = 5) */
/*     X HAS BEEN OVERWRITTEN BY A*X. */

L140:
    temp = dasum_(n, &x[1], &c__1) / (double) (*n * 3) * 2.;
    if (temp > *est) {
	dcopy_(n, &x[1], &c__1, &v[1], &c__1);
	*est = temp;
    }

L150:
    *kase = 0;
    return 0;

/*     End of DLACN2 */

} /* dlacn2_ */

/* Subroutine */ int dlacon_(integer *n, double *v, double *x,
	integer *isgn, double *est, integer *kase)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b11 = 1.;

    /* System generated locals */
    integer i__1;
    double d__1;

    /* Builtin functions
    double d_sign(double *, double *);
    integer i_dnnt(double *); */

    /* Local variables */
    static integer i__, j, iter;
    static double temp;
    static integer jump;
    static integer jlast;
    static double altsgn, estold;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLACON estimates the 1-norm of a square, real matrix A. */
/*  Reverse communication is used for evaluating matrix-vector products. */

/*  Arguments */
/*  ========= */

/*  N      (input) INTEGER */
/*         The order of the matrix.  N >= 1. */

/*  V      (workspace) DOUBLE PRECISION array, dimension (N) */
/*         On the final return, V = A*W,  where  EST = norm(V)/norm(W) */
/*         (W is not returned). */

/*  X      (input/output) DOUBLE PRECISION array, dimension (N) */
/*         On an intermediate return, X should be overwritten by */
/*               A * X,   if KASE=1, */
/*               A' * X,  if KASE=2, */
/*         and DLACON must be re-called with all the other parameters */
/*         unchanged. */

/*  ISGN   (workspace) INTEGER array, dimension (N) */

/*  EST    (input/output) DOUBLE PRECISION */
/*         On entry with KASE = 1 or 2 and JUMP = 3, EST should be */
/*         unchanged from the previous call to DLACON. */
/*         On exit, EST is an estimate (a lower bound) for norm(A). */

/*  KASE   (input/output) INTEGER */
/*         On the initial call to DLACON, KASE should be 0. */
/*         On an intermediate return, KASE will be 1 or 2, indicating */
/*         whether X should be overwritten by A * X  or A' * X. */
/*         On the final return from DLACON, KASE will again be 0. */

/*  Further Details */
/*  ======= ======= */

/*  Contributed by Nick Higham, University of Manchester. */
/*  Originally named SONEST, dated March 16, 1988. */

/*  Reference: N.J. Higham, "FORTRAN codes for estimating the one-norm of */
/*  a real or complex matrix, with applications to condition estimation", */
/*  ACM Trans. Math. Soft., vol. 14, no. 4, pp. 381-396, December 1988. */

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
/*     .. Save statement .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --isgn;
    --x;
    --v;

    /* Function Body */
    if (*kase == 0) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    x[i__] = 1. / (double) (*n);
/* L10: */
	}
	*kase = 1;
	jump = 1;
	return 0;
    }

    switch (jump) {
	case 1:  goto L20;
	case 2:  goto L40;
	case 3:  goto L70;
	case 4:  goto L110;
	case 5:  goto L140;
    }

/*     ................ ENTRY   (JUMP = 1) */
/*     FIRST ITERATION.  X HAS BEEN OVERWRITTEN BY A*X. */

L20:
    if (*n == 1) {
	v[1] = x[1];
	*est = abs(v[1]);
/*        ... QUIT */
	goto L150;
    }
    *est = dasum_(n, &x[1], &c__1);

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	x[i__] = d_sign(&c_b11, &x[i__]);
	isgn[i__] = i_dnnt(&x[i__]);
/* L30: */
    }
    *kase = 2;
    jump = 2;
    return 0;

/*     ................ ENTRY   (JUMP = 2) */
/*     FIRST ITERATION.  X HAS BEEN OVERWRITTEN BY TRANSPOSE(A)*X. */

L40:
    j = idamax_(n, &x[1], &c__1);
    iter = 2;

/*     MAIN LOOP - ITERATIONS 2,3,...,ITMAX. */

L50:
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	x[i__] = 0.;
/* L60: */
    }
    x[j] = 1.;
    *kase = 1;
    jump = 3;
    return 0;

/*     ................ ENTRY   (JUMP = 3) */
/*     X HAS BEEN OVERWRITTEN BY A*X. */

L70:
    dcopy_(n, &x[1], &c__1, &v[1], &c__1);
    estold = *est;
    *est = dasum_(n, &v[1], &c__1);
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__1 = d_sign(&c_b11, &x[i__]);
	if (i_dnnt(&d__1) != isgn[i__]) {
	    goto L90;
	}
/* L80: */
    }
/*     REPEATED SIGN VECTOR DETECTED, HENCE ALGORITHM HAS CONVERGED. */
    goto L120;

L90:
/*     TEST FOR CYCLING. */
    if (*est <= estold) {
	goto L120;
    }

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	x[i__] = d_sign(&c_b11, &x[i__]);
	isgn[i__] = i_dnnt(&x[i__]);
/* L100: */
    }
    *kase = 2;
    jump = 4;
    return 0;

/*     ................ ENTRY   (JUMP = 4) */
/*     X HAS BEEN OVERWRITTEN BY TRANSPOSE(A)*X. */

L110:
    jlast = j;
    j = idamax_(n, &x[1], &c__1);
    if (x[jlast] != (d__1 = x[j], abs(d__1)) && iter < 5) {
	++iter;
	goto L50;
    }

/*     ITERATION COMPLETE.  FINAL STAGE. */

L120:
    altsgn = 1.;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	x[i__] = altsgn * ((double) (i__ - 1) / (double) (*n - 1) +
		1.);
	altsgn = -altsgn;
/* L130: */
    }
    *kase = 1;
    jump = 5;
    return 0;

/*     ................ ENTRY   (JUMP = 5) */
/*     X HAS BEEN OVERWRITTEN BY A*X. */

L140:
    temp = dasum_(n, &x[1], &c__1) / (double) (*n * 3) * 2.;
    if (temp > *est) {
	dcopy_(n, &x[1], &c__1, &v[1], &c__1);
	*est = temp;
    }

L150:
    *kase = 0;
    return 0;

/*     End of DLACON */

} /* dlacon_ */

/* Subroutine */ int dlacpy_(const char *uplo, integer *m, integer *n, double *
	a, integer *lda, double *b, integer *ldb)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2;

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

/*  DLACPY copies all or part of a two-dimensional matrix A to another */
/*  matrix B. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies the part of the matrix A to be copied to B. */
/*          = 'U':      Upper triangular part */
/*          = 'L':      Lower triangular part */
/*          Otherwise:  All of the matrix A */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The m by n matrix A.  If UPLO = 'U', only the upper triangle */
/*          or trapezoid is accessed; if UPLO = 'L', only the lower */
/*          triangle or trapezoid is accessed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  B       (output) DOUBLE PRECISION array, dimension (LDB,N) */
/*          On exit, B = A in the locations specified by UPLO. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(1,M). */

/*  ===================================================================== */

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
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    if (lsame_(uplo, "U")) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = std::min(j,*m);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		b[i__ + j * b_dim1] = a[i__ + j * a_dim1];
/* L10: */
	    }
/* L20: */
	}
    } else if (lsame_(uplo, "L")) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = j; i__ <= i__2; ++i__) {
		b[i__ + j * b_dim1] = a[i__ + j * a_dim1];
/* L30: */
	    }
/* L40: */
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		b[i__ + j * b_dim1] = a[i__ + j * a_dim1];
/* L50: */
	    }
/* L60: */
	}
    }
    return 0;

/*     End of DLACPY */

} /* dlacpy_ */

/* Subroutine */ int dladiv_(double *a, double *b, double *c__,
	double *d__, double *p, double *q)
{
    double e, f;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLADIV performs complex division in  real arithmetic */

/*                        a + i*b */
/*             p + i*q = --------- */
/*                        c + i*d */

/*  The algorithm is due to Robert L. Smith and can be found */
/*  in D. Knuth, The art of Computer Programming, Vol.2, p.195 */

/*  Arguments */
/*  ========= */

/*  A       (input) DOUBLE PRECISION */
/*  B       (input) DOUBLE PRECISION */
/*  C       (input) DOUBLE PRECISION */
/*  D       (input) DOUBLE PRECISION */
/*          The scalars a, b, c, and d in the above expression. */

/*  P       (output) DOUBLE PRECISION */
/*  Q       (output) DOUBLE PRECISION */
/*          The scalars p and q in the above expression. */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    if (abs(*d__) < abs(*c__)) {
	e = *d__ / *c__;
	f = *c__ + *d__ * e;
	*p = (*a + *b * e) / f;
	*q = (*b - *a * e) / f;
    } else {
	e = *c__ / *d__;
	f = *d__ + *c__ * e;
	*p = (*b + *a * e) / f;
	*q = (-(*a) + *b * e) / f;
    }

    return 0;

/*     End of DLADIV */

} /* dladiv_ */

/* Subroutine */ int dlae2_(double *a, double *b, double *c__,
	double *rt1, double *rt2)
{
    /* System generated locals */
    double d__1;

    /* Builtin functions
    double sqrt(double);*/

    /* Local variables */
    double ab, df, tb, sm, rt, adf, acmn, acmx;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAE2  computes the eigenvalues of a 2-by-2 symmetric matrix */
/*     [  A   B  ] */
/*     [  B   C  ]. */
/*  On return, RT1 is the eigenvalue of larger absolute value, and RT2 */
/*  is the eigenvalue of smaller absolute value. */

/*  Arguments */
/*  ========= */

/*  A       (input) DOUBLE PRECISION */
/*          The (1,1) element of the 2-by-2 matrix. */

/*  B       (input) DOUBLE PRECISION */
/*          The (1,2) and (2,1) elements of the 2-by-2 matrix. */

/*  C       (input) DOUBLE PRECISION */
/*          The (2,2) element of the 2-by-2 matrix. */

/*  RT1     (output) DOUBLE PRECISION */
/*          The eigenvalue of larger absolute value. */

/*  RT2     (output) DOUBLE PRECISION */
/*          The eigenvalue of smaller absolute value. */

/*  Further Details */
/*  =============== */

/*  RT1 is accurate to a few ulps barring over/underflow. */

/*  RT2 may be inaccurate if there is massive cancellation in the */
/*  determinant A*C-B*B; higher precision or correctly rounded or */
/*  correctly truncated arithmetic would be needed to compute RT2 */
/*  accurately in all cases. */

/*  Overflow is possible only if RT1 is within a factor of 5 of overflow. */
/*  Underflow is harmless if the input data is 0 or exceeds */
/*     underflow_threshold / macheps. */

/* ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Compute the eigenvalues */

    sm = *a + *c__;
    df = *a - *c__;
    adf = abs(df);
    tb = *b + *b;
    ab = abs(tb);
    if (abs(*a) > abs(*c__)) {
	acmx = *a;
	acmn = *c__;
    } else {
	acmx = *c__;
	acmn = *a;
    }
    if (adf > ab) {
/* Computing 2nd power */
	d__1 = ab / adf;
	rt = adf * sqrt(d__1 * d__1 + 1.);
    } else if (adf < ab) {
/* Computing 2nd power */
	d__1 = adf / ab;
	rt = ab * sqrt(d__1 * d__1 + 1.);
    } else {

/*        Includes case AB=ADF=0 */

	rt = ab * sqrt(2.);
    }
    if (sm < 0.) {
	*rt1 = (sm - rt) * .5;

/*        Order of execution important. */
/*        To get fully accurate smaller eigenvalue, */
/*        next line needs to be executed in higher precision. */

	*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
    } else if (sm > 0.) {
	*rt1 = (sm + rt) * .5;

/*        Order of execution important. */
/*        To get fully accurate smaller eigenvalue, */
/*        next line needs to be executed in higher precision. */

	*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
    } else {

/*        Includes case RT1 = RT2 = 0 */

	*rt1 = rt * .5;
	*rt2 = rt * -.5;
    }
    return 0;

/*     End of DLAE2 */

} /* dlae2_ */

/* Subroutine */ int dlaebz_(integer *ijob, integer *nitmax, integer *n,
	integer *mmax, integer *minp, integer *nbmin, double *abstol,
	double *reltol, double *pivmin, double *d__, double *
	e, double *e2, integer *nval, double *ab, double *c__,
	integer *mout, integer *nab, double *work, integer *iwork,
	integer *info)
{
    /* System generated locals */
    integer nab_dim1, nab_offset, ab_dim1, ab_offset, i__1, i__2, i__3, i__4,
	    i__5, i__6;
    double d__1, d__2, d__3, d__4;

    /* Local variables */
    integer j, kf, ji, kl, jp, jit;
    double tmp1, tmp2;
    integer itmp1, itmp2, kfnew, klnew;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAEBZ contains the iteration loops which compute and use the */
/*  function N(w), which is the count of eigenvalues of a symmetric */
/*  tridiagonal matrix T less than or equal to its argument  w.  It */
/*  performs a choice of two types of loops: */

/*  IJOB=1, followed by */
/*  IJOB=2: It takes as input a list of intervals and returns a list of */
/*          sufficiently small intervals whose union contains the same */
/*          eigenvalues as the union of the original intervals. */
/*          The input intervals are (AB(j,1),AB(j,2)], j=1,...,MINP. */
/*          The output interval (AB(j,1),AB(j,2)] will contain */
/*          eigenvalues NAB(j,1)+1,...,NAB(j,2), where 1 <= j <= MOUT. */

/*  IJOB=3: It performs a binary search in each input interval */
/*          (AB(j,1),AB(j,2)] for a point  w(j)  such that */
/*          N(w(j))=NVAL(j), and uses  C(j)  as the starting point of */
/*          the search.  If such a w(j) is found, then on output */
/*          AB(j,1)=AB(j,2)=w.  If no such w(j) is found, then on output */
/*          (AB(j,1),AB(j,2)] will be a small interval containing the */
/*          point where N(w) jumps through NVAL(j), unless that point */
/*          lies outside the initial interval. */

/*  Note that the intervals are in all cases half-open intervals, */
/*  i.e., of the form  (a,b] , which includes  b  but not  a . */

/*  To avoid underflow, the matrix should be scaled so that its largest */
/*  element is no greater than  overflow**(1/2) * underflow**(1/4) */
/*  in absolute value.  To assure the most accurate computation */
/*  of small eigenvalues, the matrix should be scaled to be */
/*  not much smaller than that, either. */

/*  See W. Kahan "Accurate Eigenvalues of a Symmetric Tridiagonal */
/*  Matrix", Report CS41, Computer Science Dept., Stanford */
/*  University, July 21, 1966 */

/*  Note: the arguments are, in general, *not* checked for unreasonable */
/*  values. */

/*  Arguments */
/*  ========= */

/*  IJOB    (input) INTEGER */
/*          Specifies what is to be done: */
/*          = 1:  Compute NAB for the initial intervals. */
/*          = 2:  Perform bisection iteration to find eigenvalues of T. */
/*          = 3:  Perform bisection iteration to invert N(w), i.e., */
/*                to find a point which has a specified number of */
/*                eigenvalues of T to its left. */
/*          Other values will cause DLAEBZ to return with INFO=-1. */

/*  NITMAX  (input) INTEGER */
/*          The maximum number of "levels" of bisection to be */
/*          performed, i.e., an interval of width W will not be made */
/*          smaller than 2^(-NITMAX) * W.  If not all intervals */
/*          have converged after NITMAX iterations, then INFO is set */
/*          to the number of non-converged intervals. */

/*  N       (input) INTEGER */
/*          The dimension n of the tridiagonal matrix T.  It must be at */
/*          least 1. */

/*  MMAX    (input) INTEGER */
/*          The maximum number of intervals.  If more than MMAX intervals */
/*          are generated, then DLAEBZ will quit with INFO=MMAX+1. */

/*  MINP    (input) INTEGER */
/*          The initial number of intervals.  It may not be greater than */
/*          MMAX. */

/*  NBMIN   (input) INTEGER */
/*          The smallest number of intervals that should be processed */
/*          using a vector loop.  If zero, then only the scalar loop */
/*          will be used. */

/*  ABSTOL  (input) DOUBLE PRECISION */
/*          The minimum (absolute) width of an interval.  When an */
/*          interval is narrower than ABSTOL, or than RELTOL times the */
/*          larger (in magnitude) endpoint, then it is considered to be */
/*          sufficiently small, i.e., converged.  This must be at least */
/*          zero. */

/*  RELTOL  (input) DOUBLE PRECISION */
/*          The minimum relative width of an interval.  When an interval */
/*          is narrower than ABSTOL, or than RELTOL times the larger (in */
/*          magnitude) endpoint, then it is considered to be */
/*          sufficiently small, i.e., converged.  Note: this should */
/*          always be at least radix*machine epsilon. */

/*  PIVMIN  (input) DOUBLE PRECISION */
/*          The minimum absolute value of a "pivot" in the Sturm */
/*          sequence loop.  This *must* be at least  max |e(j)**2| * */
/*          safe_min  and at least safe_min, where safe_min is at least */
/*          the smallest number that can divide one without overflow. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal elements of the tridiagonal matrix T. */

/*  E       (input) DOUBLE PRECISION array, dimension (N) */
/*          The offdiagonal elements of the tridiagonal matrix T in */
/*          positions 1 through N-1.  E(N) is arbitrary. */

/*  E2      (input) DOUBLE PRECISION array, dimension (N) */
/*          The squares of the offdiagonal elements of the tridiagonal */
/*          matrix T.  E2(N) is ignored. */

/*  NVAL    (input/output) INTEGER array, dimension (MINP) */
/*          If IJOB=1 or 2, not referenced. */
/*          If IJOB=3, the desired values of N(w).  The elements of NVAL */
/*          will be reordered to correspond with the intervals in AB. */
/*          Thus, NVAL(j) on output will not, in general be the same as */
/*          NVAL(j) on input, but it will correspond with the interval */
/*          (AB(j,1),AB(j,2)] on output. */

/*  AB      (input/output) DOUBLE PRECISION array, dimension (MMAX,2) */
/*          The endpoints of the intervals.  AB(j,1) is  a(j), the left */
/*          endpoint of the j-th interval, and AB(j,2) is b(j), the */
/*          right endpoint of the j-th interval.  The input intervals */
/*          will, in general, be modified, split, and reordered by the */
/*          calculation. */

/*  C       (input/output) DOUBLE PRECISION array, dimension (MMAX) */
/*          If IJOB=1, ignored. */
/*          If IJOB=2, workspace. */
/*          If IJOB=3, then on input C(j) should be initialized to the */
/*          first search point in the binary search. */

/*  MOUT    (output) INTEGER */
/*          If IJOB=1, the number of eigenvalues in the intervals. */
/*          If IJOB=2 or 3, the number of intervals output. */
/*          If IJOB=3, MOUT will equal MINP. */

/*  NAB     (input/output) INTEGER array, dimension (MMAX,2) */
/*          If IJOB=1, then on output NAB(i,j) will be set to N(AB(i,j)). */
/*          If IJOB=2, then on input, NAB(i,j) should be set.  It must */
/*             satisfy the condition: */
/*             N(AB(i,1)) <= NAB(i,1) <= NAB(i,2) <= N(AB(i,2)), */
/*             which means that in interval i only eigenvalues */
/*             NAB(i,1)+1,...,NAB(i,2) will be considered.  Usually, */
/*             NAB(i,j)=N(AB(i,j)), from a previous call to DLAEBZ with */
/*             IJOB=1. */
/*             On output, NAB(i,j) will contain */
/*             max(na(k),min(nb(k),N(AB(i,j)))), where k is the index of */
/*             the input interval that the output interval */
/*             (AB(j,1),AB(j,2)] came from, and na(k) and nb(k) are the */
/*             the input values of NAB(k,1) and NAB(k,2). */
/*          If IJOB=3, then on output, NAB(i,j) contains N(AB(i,j)), */
/*             unless N(w) > NVAL(i) for all search points  w , in which */
/*             case NAB(i,1) will not be modified, i.e., the output */
/*             value will be the same as the input value (modulo */
/*             reorderings -- see NVAL and AB), or unless N(w) < NVAL(i) */
/*             for all search points  w , in which case NAB(i,2) will */
/*             not be modified.  Normally, NAB should be set to some */
/*             distinctive value(s) before DLAEBZ is called. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (MMAX) */
/*          Workspace. */

/*  IWORK   (workspace) INTEGER array, dimension (MMAX) */
/*          Workspace. */

/*  INFO    (output) INTEGER */
/*          = 0:       All intervals converged. */
/*          = 1--MMAX: The last INFO intervals did not converge. */
/*          = MMAX+1:  More than MMAX intervals were generated. */

/*  Further Details */
/*  =============== */

/*      This routine is intended to be called only by other LAPACK */
/*  routines, thus the interface is less user-friendly.  It is intended */
/*  for two purposes: */

/*  (a) finding eigenvalues.  In this case, DLAEBZ should have one or */
/*      more initial intervals set up in AB, and DLAEBZ should be called */
/*      with IJOB=1.  This sets up NAB, and also counts the eigenvalues. */
/*      Intervals with no eigenvalues would usually be thrown out at */
/*      this point.  Also, if not all the eigenvalues in an interval i */
/*      are desired, NAB(i,1) can be increased or NAB(i,2) decreased. */
/*      For example, set NAB(i,1)=NAB(i,2)-1 to get the largest */
/*      eigenvalue.  DLAEBZ is then called with IJOB=2 and MMAX */
/*      no smaller than the value of MOUT returned by the call with */
/*      IJOB=1.  After this (IJOB=2) call, eigenvalues NAB(i,1)+1 */
/*      through NAB(i,2) are approximately AB(i,1) (or AB(i,2)) to the */
/*      tolerance specified by ABSTOL and RELTOL. */

/*  (b) finding an interval (a',b'] containing eigenvalues w(f),...,w(l). */
/*      In this case, start with a Gershgorin interval  (a,b).  Set up */
/*      AB to contain 2 search intervals, both initially (a,b).  One */
/*      NVAL element should contain  f-1  and the other should contain  l */
/*      , while C should contain a and b, resp.  NAB(i,1) should be -1 */
/*      and NAB(i,2) should be N+1, to flag an error if the desired */
/*      interval does not lie in (a,b).  DLAEBZ is then called with */
/*      IJOB=3.  On exit, if w(f-1) < w(f), then one of the intervals -- */
/*      j -- will have AB(j,1)=AB(j,2) and NAB(j,1)=NAB(j,2)=f-1, while */
/*      if, to the specified tolerance, w(f-k)=...=w(f+r), k > 0 and r */
/*      >= 0, then the interval will have  N(AB(j,1))=NAB(j,1)=f-k and */
/*      N(AB(j,2))=NAB(j,2)=f+r.  The cases w(l) < w(l+1) and */
/*      w(l-r)=...=w(l+k) are handled similarly. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Check for Errors */

    /* Parameter adjustments */
    nab_dim1 = *mmax;
    nab_offset = 1 + nab_dim1;
    nab -= nab_offset;
    ab_dim1 = *mmax;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    --d__;
    --e;
    --e2;
    --nval;
    --c__;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;
    if (*ijob < 1 || *ijob > 3) {
	*info = -1;
	return 0;
    }

/*     Initialize NAB */

    if (*ijob == 1) {

/*        Compute the number of eigenvalues in the initial intervals. */

	*mout = 0;
/* DIR$ NOVECTOR */
	i__1 = *minp;
	for (ji = 1; ji <= i__1; ++ji) {
	    for (jp = 1; jp <= 2; ++jp) {
		tmp1 = d__[1] - ab[ji + jp * ab_dim1];
		if (abs(tmp1) < *pivmin) {
		    tmp1 = -(*pivmin);
		}
		nab[ji + jp * nab_dim1] = 0;
		if (tmp1 <= 0.) {
		    nab[ji + jp * nab_dim1] = 1;
		}

		i__2 = *n;
		for (j = 2; j <= i__2; ++j) {
		    tmp1 = d__[j] - e2[j - 1] / tmp1 - ab[ji + jp * ab_dim1];
		    if (abs(tmp1) < *pivmin) {
			tmp1 = -(*pivmin);
		    }
		    if (tmp1 <= 0.) {
			++nab[ji + jp * nab_dim1];
		    }
/* L10: */
		}
/* L20: */
	    }
	    *mout = *mout + nab[ji + (nab_dim1 << 1)] - nab[ji + nab_dim1];
/* L30: */
	}
	return 0;
    }

/*     Initialize for loop */

/*     KF and KL have the following meaning: */
/*        Intervals 1,...,KF-1 have converged. */
/*        Intervals KF,...,KL  still need to be refined. */

    kf = 1;
    kl = *minp;

/*     If IJOB=2, initialize C. */
/*     If IJOB=3, use the user-supplied starting point. */

    if (*ijob == 2) {
	i__1 = *minp;
	for (ji = 1; ji <= i__1; ++ji) {
	    c__[ji] = (ab[ji + ab_dim1] + ab[ji + (ab_dim1 << 1)]) * .5;
/* L40: */
	}
    }

/*     Iteration loop */

    i__1 = *nitmax;
    for (jit = 1; jit <= i__1; ++jit) {

/*        Loop over intervals */

	if (kl - kf + 1 >= *nbmin && *nbmin > 0) {

/*           Begin of Parallel Version of the loop */

	    i__2 = kl;
	    for (ji = kf; ji <= i__2; ++ji) {

/*              Compute N(c), the number of eigenvalues less than c */

		work[ji] = d__[1] - c__[ji];
		iwork[ji] = 0;
		if (work[ji] <= *pivmin) {
		    iwork[ji] = 1;
/* Computing MIN */
		    d__1 = work[ji], d__2 = -(*pivmin);
		    work[ji] = std::min(d__1,d__2);
		}

		i__3 = *n;
		for (j = 2; j <= i__3; ++j) {
		    work[ji] = d__[j] - e2[j - 1] / work[ji] - c__[ji];
		    if (work[ji] <= *pivmin) {
			++iwork[ji];
/* Computing MIN */
			d__1 = work[ji], d__2 = -(*pivmin);
			work[ji] = std::min(d__1,d__2);
		    }
/* L50: */
		}
/* L60: */
	    }

	    if (*ijob <= 2) {

/*              IJOB=2: Choose all intervals containing eigenvalues. */

		klnew = kl;
		i__2 = kl;
		for (ji = kf; ji <= i__2; ++ji) {

/*                 Insure that N(w) is monotone */

/* Computing MIN */
/* Computing MAX */
		    i__5 = nab[ji + nab_dim1], i__6 = iwork[ji];
		    i__3 = nab[ji + (nab_dim1 << 1)], i__4 = std::max(i__5,i__6);
		    iwork[ji] = std::min(i__3,i__4);

/*                 Update the Queue -- add intervals if both halves */
/*                 contain eigenvalues. */

		    if (iwork[ji] == nab[ji + (nab_dim1 << 1)]) {

/*                    No eigenvalue in the upper interval: */
/*                    just use the lower interval. */

			ab[ji + (ab_dim1 << 1)] = c__[ji];

		    } else if (iwork[ji] == nab[ji + nab_dim1]) {

/*                    No eigenvalue in the lower interval: */
/*                    just use the upper interval. */

			ab[ji + ab_dim1] = c__[ji];
		    } else {
			++klnew;
			if (klnew <= *mmax) {

/*                       Eigenvalue in both intervals -- add upper to */
/*                       queue. */

			    ab[klnew + (ab_dim1 << 1)] = ab[ji + (ab_dim1 <<
				    1)];
			    nab[klnew + (nab_dim1 << 1)] = nab[ji + (nab_dim1
				    << 1)];
			    ab[klnew + ab_dim1] = c__[ji];
			    nab[klnew + nab_dim1] = iwork[ji];
			    ab[ji + (ab_dim1 << 1)] = c__[ji];
			    nab[ji + (nab_dim1 << 1)] = iwork[ji];
			} else {
			    *info = *mmax + 1;
			}
		    }
/* L70: */
		}
		if (*info != 0) {
		    return 0;
		}
		kl = klnew;
	    } else {

/*              IJOB=3: Binary search.  Keep only the interval containing */
/*                      w   s.t. N(w) = NVAL */

		i__2 = kl;
		for (ji = kf; ji <= i__2; ++ji) {
		    if (iwork[ji] <= nval[ji]) {
			ab[ji + ab_dim1] = c__[ji];
			nab[ji + nab_dim1] = iwork[ji];
		    }
		    if (iwork[ji] >= nval[ji]) {
			ab[ji + (ab_dim1 << 1)] = c__[ji];
			nab[ji + (nab_dim1 << 1)] = iwork[ji];
		    }
/* L80: */
		}
	    }

	} else {

/*           End of Parallel Version of the loop */

/*           Begin of Serial Version of the loop */

	    klnew = kl;
	    i__2 = kl;
	    for (ji = kf; ji <= i__2; ++ji) {

/*              Compute N(w), the number of eigenvalues less than w */

		tmp1 = c__[ji];
		tmp2 = d__[1] - tmp1;
		itmp1 = 0;
		if (tmp2 <= *pivmin) {
		    itmp1 = 1;
/* Computing MIN */
		    d__1 = tmp2, d__2 = -(*pivmin);
		    tmp2 = std::min(d__1,d__2);
		}

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

		i__3 = *n;
		for (j = 2; j <= i__3; ++j) {
		    tmp2 = d__[j] - e2[j - 1] / tmp2 - tmp1;
		    if (tmp2 <= *pivmin) {
			++itmp1;
/* Computing MIN */
			d__1 = tmp2, d__2 = -(*pivmin);
			tmp2 = std::min(d__1,d__2);
		    }
/* L90: */
		}

		if (*ijob <= 2) {

/*                 IJOB=2: Choose all intervals containing eigenvalues. */

/*                 Insure that N(w) is monotone */

/* Computing MIN */
/* Computing MAX */
		    i__5 = nab[ji + nab_dim1];
		    i__3 = nab[ji + (nab_dim1 << 1)], i__4 = std::max(i__5,itmp1);
		    itmp1 = std::min(i__3,i__4);

/*                 Update the Queue -- add intervals if both halves */
/*                 contain eigenvalues. */

		    if (itmp1 == nab[ji + (nab_dim1 << 1)]) {

/*                    No eigenvalue in the upper interval: */
/*                    just use the lower interval. */

			ab[ji + (ab_dim1 << 1)] = tmp1;

		    } else if (itmp1 == nab[ji + nab_dim1]) {

/*                    No eigenvalue in the lower interval: */
/*                    just use the upper interval. */

			ab[ji + ab_dim1] = tmp1;
		    } else if (klnew < *mmax) {

/*                    Eigenvalue in both intervals -- add upper to queue. */

			++klnew;
			ab[klnew + (ab_dim1 << 1)] = ab[ji + (ab_dim1 << 1)];
			nab[klnew + (nab_dim1 << 1)] = nab[ji + (nab_dim1 <<
				1)];
			ab[klnew + ab_dim1] = tmp1;
			nab[klnew + nab_dim1] = itmp1;
			ab[ji + (ab_dim1 << 1)] = tmp1;
			nab[ji + (nab_dim1 << 1)] = itmp1;
		    } else {
			*info = *mmax + 1;
			return 0;
		    }
		} else {

/*                 IJOB=3: Binary search.  Keep only the interval */
/*                         containing  w  s.t. N(w) = NVAL */

		    if (itmp1 <= nval[ji]) {
			ab[ji + ab_dim1] = tmp1;
			nab[ji + nab_dim1] = itmp1;
		    }
		    if (itmp1 >= nval[ji]) {
			ab[ji + (ab_dim1 << 1)] = tmp1;
			nab[ji + (nab_dim1 << 1)] = itmp1;
		    }
		}
/* L100: */
	    }
	    kl = klnew;

/*           End of Serial Version of the loop */

	}

/*        Check for convergence */

	kfnew = kf;
	i__2 = kl;
	for (ji = kf; ji <= i__2; ++ji) {
	    tmp1 = (d__1 = ab[ji + (ab_dim1 << 1)] - ab[ji + ab_dim1], abs(
		    d__1));
/* Computing MAX */
	    d__3 = (d__1 = ab[ji + (ab_dim1 << 1)], abs(d__1)), d__4 = (d__2 =
		     ab[ji + ab_dim1], abs(d__2));
	    tmp2 = std::max(d__3,d__4);
/* Computing MAX */
	    d__1 = std::max(*abstol,*pivmin), d__2 = *reltol * tmp2;
	    if (tmp1 < std::max(d__1,d__2) || nab[ji + nab_dim1] >= nab[ji + (
		    nab_dim1 << 1)]) {

/*              Converged -- Swap with position KFNEW, */
/*                           then increment KFNEW */

		if (ji > kfnew) {
		    tmp1 = ab[ji + ab_dim1];
		    tmp2 = ab[ji + (ab_dim1 << 1)];
		    itmp1 = nab[ji + nab_dim1];
		    itmp2 = nab[ji + (nab_dim1 << 1)];
		    ab[ji + ab_dim1] = ab[kfnew + ab_dim1];
		    ab[ji + (ab_dim1 << 1)] = ab[kfnew + (ab_dim1 << 1)];
		    nab[ji + nab_dim1] = nab[kfnew + nab_dim1];
		    nab[ji + (nab_dim1 << 1)] = nab[kfnew + (nab_dim1 << 1)];
		    ab[kfnew + ab_dim1] = tmp1;
		    ab[kfnew + (ab_dim1 << 1)] = tmp2;
		    nab[kfnew + nab_dim1] = itmp1;
		    nab[kfnew + (nab_dim1 << 1)] = itmp2;
		    if (*ijob == 3) {
			itmp1 = nval[ji];
			nval[ji] = nval[kfnew];
			nval[kfnew] = itmp1;
		    }
		}
		++kfnew;
	    }
/* L110: */
	}
	kf = kfnew;

/*        Choose Midpoints */

	i__2 = kl;
	for (ji = kf; ji <= i__2; ++ji) {
	    c__[ji] = (ab[ji + ab_dim1] + ab[ji + (ab_dim1 << 1)]) * .5;
/* L120: */
	}

/*        If no more intervals to refine, quit. */

	if (kf > kl) {
	    goto L140;
	}
/* L130: */
    }

/*     Converged */

L140:
/* Computing MAX */
    i__1 = kl + 1 - kf;
    *info = std::max(i__1,0_integer);
    *mout = kl;

    return 0;

/*     End of DLAEBZ */

} /* dlaebz_ */

/* Subroutine */ int dlaed0_(integer *icompq, integer *qsiz, integer *n,
	double *d__, double *e, double *q, integer *ldq,
	double *qstore, integer *ldqs, double *work, integer *iwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__9 = 9;
	static integer c__0 = 0;
	static integer c__2 = 2;
	static double c_b23 = 1.;
	static double c_b24 = 0.;
	static integer c__1 = 1;

    /* System generated locals */
    integer q_dim1, q_offset, qstore_dim1, qstore_offset, i__1, i__2;
    double d__1;

    /* Local variables */
    integer i__, j, k, iq, lgn, msd2, smm1, spm1, spm2;
    double temp;
    integer curr;
    integer iperm;
    integer indxq, iwrem;
    integer iqptr;
    integer tlvls;
    integer igivcl;
    integer igivnm, submat, curprb, subpbs, igivpt;
    integer curlvl, matsiz, iprmpt, smlsiz;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAED0 computes all eigenvalues and corresponding eigenvectors of a */
/*  symmetric tridiagonal matrix using the divide and conquer method. */

/*  Arguments */
/*  ========= */

/*  ICOMPQ  (input) INTEGER */
/*          = 0:  Compute eigenvalues only. */
/*          = 1:  Compute eigenvectors of original dense symmetric matrix */
/*                also.  On entry, Q contains the orthogonal matrix used */
/*                to reduce the original matrix to tridiagonal form. */
/*          = 2:  Compute eigenvalues and eigenvectors of tridiagonal */
/*                matrix. */

/*  QSIZ   (input) INTEGER */
/*         The dimension of the orthogonal matrix used to reduce */
/*         the full matrix to tridiagonal form.  QSIZ >= N if ICOMPQ = 1. */

/*  N      (input) INTEGER */
/*         The dimension of the symmetric tridiagonal matrix.  N >= 0. */

/*  D      (input/output) DOUBLE PRECISION array, dimension (N) */
/*         On entry, the main diagonal of the tridiagonal matrix. */
/*         On exit, its eigenvalues. */

/*  E      (input) DOUBLE PRECISION array, dimension (N-1) */
/*         The off-diagonal elements of the tridiagonal matrix. */
/*         On exit, E has been destroyed. */

/*  Q      (input/output) DOUBLE PRECISION array, dimension (LDQ, N) */
/*         On entry, Q must contain an N-by-N orthogonal matrix. */
/*         If ICOMPQ = 0    Q is not referenced. */
/*         If ICOMPQ = 1    On entry, Q is a subset of the columns of the */
/*                          orthogonal matrix used to reduce the full */
/*                          matrix to tridiagonal form corresponding to */
/*                          the subset of the full matrix which is being */
/*                          decomposed at this time. */
/*         If ICOMPQ = 2    On entry, Q will be the identity matrix. */
/*                          On exit, Q contains the eigenvectors of the */
/*                          tridiagonal matrix. */

/*  LDQ    (input) INTEGER */
/*         The leading dimension of the array Q.  If eigenvectors are */
/*         desired, then  LDQ >= max(1,N).  In any case,  LDQ >= 1. */

/*  QSTORE (workspace) DOUBLE PRECISION array, dimension (LDQS, N) */
/*         Referenced only when ICOMPQ = 1.  Used to store parts of */
/*         the eigenvector matrix when the updating matrix multiplies */
/*         take place. */

/*  LDQS   (input) INTEGER */
/*         The leading dimension of the array QSTORE.  If ICOMPQ = 1, */
/*         then  LDQS >= max(1,N).  In any case,  LDQS >= 1. */

/*  WORK   (workspace) DOUBLE PRECISION array, */
/*         If ICOMPQ = 0 or 1, the dimension of WORK must be at least */
/*                     1 + 3*N + 2*N*lg N + 2*N**2 */
/*                     ( lg( N ) = smallest integer k */
/*                                 such that 2^k >= N ) */
/*         If ICOMPQ = 2, the dimension of WORK must be at least */
/*                     4*N + N**2. */

/*  IWORK  (workspace) INTEGER array, */
/*         If ICOMPQ = 0 or 1, the dimension of IWORK must be at least */
/*                        6 + 6*N + 5*N*lg N. */
/*                        ( lg( N ) = smallest integer k */
/*                                    such that 2^k >= N ) */
/*         If ICOMPQ = 2, the dimension of IWORK must be at least */
/*                        3 + 5*N. */

/*  INFO   (output) INTEGER */
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
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    qstore_dim1 = *ldqs;
    qstore_offset = 1 + qstore_dim1;
    qstore -= qstore_offset;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 2) {
	*info = -1;
    } else if (*icompq == 1 && *qsiz < std::max(0_integer,*n)) {
	*info = -2;
    } else if (*n < 0) {
	*info = -3;
    } else if (*ldq < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*ldqs < std::max(1_integer,*n)) {
	*info = -9;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAED0", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    smlsiz = ilaenv_(&c__9, "DLAED0", " ", &c__0, &c__0, &c__0, &c__0);

/*     Determine the size and placement of the submatrices, and save in */
/*     the leading elements of IWORK. */

    iwork[1] = *n;
    subpbs = 1;
    tlvls = 0;
L10:
    if (iwork[subpbs] > smlsiz) {
	for (j = subpbs; j >= 1; --j) {
	    iwork[j * 2] = (iwork[j] + 1) / 2;
	    iwork[(j << 1) - 1] = iwork[j] / 2;
/* L20: */
	}
	++tlvls;
	subpbs  <<=  1;
	goto L10;
    }
    i__1 = subpbs;
    for (j = 2; j <= i__1; ++j) {
	iwork[j] += iwork[j - 1];
/* L30: */
    }

/*     Divide the matrix into SUBPBS submatrices of size at most SMLSIZ+1 */
/*     using rank-1 modifications (cuts). */

    spm1 = subpbs - 1;
    i__1 = spm1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	submat = iwork[i__] + 1;
	smm1 = submat - 1;
	d__[smm1] -= (d__1 = e[smm1], abs(d__1));
	d__[submat] -= (d__1 = e[smm1], abs(d__1));
/* L40: */
    }

    indxq = (*n << 2) + 3;
    if (*icompq != 2) {

/*        Set up workspaces for eigenvalues only/accumulate new vectors */
/*        routine */

	temp = log((double) (*n)) / log(2.);
	lgn = (integer) temp;
	if (pow_ii(&c__2, &lgn) < *n) {
	    ++lgn;
	}
	if (pow_ii(&c__2, &lgn) < *n) {
	    ++lgn;
	}
	iprmpt = indxq + *n + 1;
	iperm = iprmpt + *n * lgn;
	iqptr = iperm + *n * lgn;
	igivpt = iqptr + *n + 2;
	igivcl = igivpt + *n * lgn;

	igivnm = 1;
	iq = igivnm + (*n << 1) * lgn;
/* Computing 2nd power */
	i__1 = *n;
	iwrem = iq + i__1 * i__1 + 1;

/*        Initialize pointers */

	i__1 = subpbs;
	for (i__ = 0; i__ <= i__1; ++i__) {
	    iwork[iprmpt + i__] = 1;
	    iwork[igivpt + i__] = 1;
/* L50: */
	}
	iwork[iqptr] = 1;
    }

/*     Solve each submatrix eigenproblem at the bottom of the divide and */
/*     conquer tree. */

    curr = 0;
    i__1 = spm1;
    for (i__ = 0; i__ <= i__1; ++i__) {
	if (i__ == 0) {
	    submat = 1;
	    matsiz = iwork[1];
	} else {
	    submat = iwork[i__] + 1;
	    matsiz = iwork[i__ + 1] - iwork[i__];
	}
	if (*icompq == 2) {
	    dsteqr_("I", &matsiz, &d__[submat], &e[submat], &q[submat +
		    submat * q_dim1], ldq, &work[1], info);
	    if (*info != 0) {
		goto L130;
	    }
	} else {
	    dsteqr_("I", &matsiz, &d__[submat], &e[submat], &work[iq - 1 +
		    iwork[iqptr + curr]], &matsiz, &work[1], info);
	    if (*info != 0) {
		goto L130;
	    }
	    if (*icompq == 1) {
		dgemm_("N", "N", qsiz, &matsiz, &matsiz, &c_b23, &q[submat *
			q_dim1 + 1], ldq, &work[iq - 1 + iwork[iqptr + curr]],
			 &matsiz, &c_b24, &qstore[submat * qstore_dim1 + 1],
			ldqs);
	    }
/* Computing 2nd power */
	    i__2 = matsiz;
	    iwork[iqptr + curr + 1] = iwork[iqptr + curr] + i__2 * i__2;
	    ++curr;
	}
	k = 1;
	i__2 = iwork[i__ + 1];
	for (j = submat; j <= i__2; ++j) {
	    iwork[indxq + j] = k;
	    ++k;
/* L60: */
	}
/* L70: */
    }

/*     Successively merge eigensystems of adjacent submatrices */
/*     into eigensystem for the corresponding larger matrix. */

/*     while ( SUBPBS > 1 ) */

    curlvl = 1;
L80:
    if (subpbs > 1) {
	spm2 = subpbs - 2;
	i__1 = spm2;
	for (i__ = 0; i__ <= i__1; i__ += 2) {
	    if (i__ == 0) {
		submat = 1;
		matsiz = iwork[2];
		msd2 = iwork[1];
		curprb = 0;
	    } else {
		submat = iwork[i__] + 1;
		matsiz = iwork[i__ + 2] - iwork[i__];
		msd2 = matsiz / 2;
		++curprb;
	    }

/*     Merge lower order eigensystems (of size MSD2 and MATSIZ - MSD2) */
/*     into an eigensystem of size MATSIZ. */
/*     DLAED1 is used only for the full eigensystem of a tridiagonal */
/*     matrix. */
/*     DLAED7 handles the cases in which eigenvalues only or eigenvalues */
/*     and eigenvectors of a full symmetric matrix (which was reduced to */
/*     tridiagonal form) are desired. */

	    if (*icompq == 2) {
		dlaed1_(&matsiz, &d__[submat], &q[submat + submat * q_dim1],
			ldq, &iwork[indxq + submat], &e[submat + msd2 - 1], &
			msd2, &work[1], &iwork[subpbs + 1], info);
	    } else {
		dlaed7_(icompq, &matsiz, qsiz, &tlvls, &curlvl, &curprb, &d__[
			submat], &qstore[submat * qstore_dim1 + 1], ldqs, &
			iwork[indxq + submat], &e[submat + msd2 - 1], &msd2, &
			work[iq], &iwork[iqptr], &iwork[iprmpt], &iwork[iperm]
, &iwork[igivpt], &iwork[igivcl], &work[igivnm], &
			work[iwrem], &iwork[subpbs + 1], info);
	    }
	    if (*info != 0) {
		goto L130;
	    }
	    iwork[i__ / 2 + 1] = iwork[i__ + 2];
/* L90: */
	}
	subpbs /= 2;
	++curlvl;
	goto L80;
    }

/*     end while */

/*     Re-merge the eigenvalues/vectors which were deflated at the final */
/*     merge step. */

    if (*icompq == 1) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j = iwork[indxq + i__];
	    work[i__] = d__[j];
	    dcopy_(qsiz, &qstore[j * qstore_dim1 + 1], &c__1, &q[i__ * q_dim1
		    + 1], &c__1);
/* L100: */
	}
	dcopy_(n, &work[1], &c__1, &d__[1], &c__1);
    } else if (*icompq == 2) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j = iwork[indxq + i__];
	    work[i__] = d__[j];
	    dcopy_(n, &q[j * q_dim1 + 1], &c__1, &work[*n * i__ + 1], &c__1);
/* L110: */
	}
	dcopy_(n, &work[1], &c__1, &d__[1], &c__1);
	dlacpy_("A", n, n, &work[*n + 1], n, &q[q_offset], ldq);
    } else {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    j = iwork[indxq + i__];
	    work[i__] = d__[j];
/* L120: */
	}
	dcopy_(n, &work[1], &c__1, &d__[1], &c__1);
    }
    goto L140;

L130:
    *info = submat * (*n + 1) + submat + matsiz - 1;

L140:
    return 0;

/*     End of DLAED0 */

} /* dlaed0_ */

/* Subroutine */ int dlaed1_(integer *n, double *d__, double *q,
	integer *ldq, integer *indxq, double *rho, integer *cutpnt,
	double *work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;

    /* System generated locals */
    integer q_dim1, q_offset, i__1, i__2;

    /* Local variables */
    integer i__, k, n1, n2, is, iw, iz, iq2, zpp1, indx, indxc;
    integer indxp;
    integer idlmda;
	integer coltyp;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAED1 computes the updated eigensystem of a diagonal */
/*  matrix after modification by a rank-one symmetric matrix.  This */
/*  routine is used only for the eigenproblem which requires all */
/*  eigenvalues and eigenvectors of a tridiagonal matrix.  DLAED7 handles */
/*  the case in which eigenvalues only or eigenvalues and eigenvectors */
/*  of a full symmetric matrix (which was reduced to tridiagonal form) */
/*  are desired. */

/*    T = Q(in) ( D(in) + RHO * Z*Z' ) Q'(in) = Q(out) * D(out) * Q'(out) */

/*     where Z = Q'u, u is a vector of length N with ones in the */
/*     CUTPNT and CUTPNT + 1 th elements and zeros elsewhere. */

/*     The eigenvectors of the original matrix are stored in Q, and the */
/*     eigenvalues are in D.  The algorithm consists of three stages: */

/*        The first stage consists of deflating the size of the problem */
/*        when there are multiple eigenvalues or if there is a zero in */
/*        the Z vector.  For each such occurrence the dimension of the */
/*        secular equation problem is reduced by one.  This stage is */
/*        performed by the routine DLAED2. */

/*        The second stage consists of calculating the updated */
/*        eigenvalues. This is done by finding the roots of the secular */
/*        equation via the routine DLAED4 (as called by DLAED3). */
/*        This routine also calculates the eigenvectors of the current */
/*        problem. */

/*        The final stage consists of computing the updated eigenvectors */
/*        directly using the updated eigenvalues.  The eigenvectors for */
/*        the current problem are multiplied with the eigenvectors from */
/*        the overall problem. */

/*  Arguments */
/*  ========= */

/*  N      (input) INTEGER */
/*         The dimension of the symmetric tridiagonal matrix.  N >= 0. */

/*  D      (input/output) DOUBLE PRECISION array, dimension (N) */
/*         On entry, the eigenvalues of the rank-1-perturbed matrix. */
/*         On exit, the eigenvalues of the repaired matrix. */

/*  Q      (input/output) DOUBLE PRECISION array, dimension (LDQ,N) */
/*         On entry, the eigenvectors of the rank-1-perturbed matrix. */
/*         On exit, the eigenvectors of the repaired tridiagonal matrix. */

/*  LDQ    (input) INTEGER */
/*         The leading dimension of the array Q.  LDQ >= max(1,N). */

/*  INDXQ  (input/output) INTEGER array, dimension (N) */
/*         On entry, the permutation which separately sorts the two */
/*         subproblems in D into ascending order. */
/*         On exit, the permutation which will reintegrate the */
/*         subproblems back into sorted order, */
/*         i.e. D( INDXQ( I = 1, N ) ) will be in ascending order. */

/*  RHO    (input) DOUBLE PRECISION */
/*         The subdiagonal entry used to create the rank-1 modification. */

/*  CUTPNT (input) INTEGER */
/*         The location of the last eigenvalue in the leading sub-matrix. */
/*         min(1,N) <= CUTPNT <= N/2. */

/*  WORK   (workspace) DOUBLE PRECISION array, dimension (4*N + N**2) */

/*  IWORK  (workspace) INTEGER array, dimension (4*N) */

/*  INFO   (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = 1, an eigenvalue did not converge */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Jeff Rutter, Computer Science Division, University of California */
/*     at Berkeley, USA */
/*  Modified by Francoise Tisseur, University of Tennessee. */

/*  ===================================================================== */

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
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --indxq;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;

    if (*n < 0) {
	*info = -1;
    } else if (*ldq < std::max(1_integer,*n)) {
	*info = -4;
    } else /* if(complicated condition) */ {
/* Computing MIN */
	i__1 = 1, i__2 = *n / 2;
	if (std::min(i__1,i__2) > *cutpnt || *n / 2 < *cutpnt) {
	    *info = -7;
	}
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAED1", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     The following values are integer pointers which indicate */
/*     the portion of the workspace */
/*     used by a particular array in DLAED2 and DLAED3. */

    iz = 1;
    idlmda = iz + *n;
    iw = idlmda + *n;
    iq2 = iw + *n;

    indx = 1;
    indxc = indx + *n;
    coltyp = indxc + *n;
    indxp = coltyp + *n;


/*     Form the z-vector which consists of the last row of Q_1 and the */
/*     first row of Q_2. */

    dcopy_(cutpnt, &q[*cutpnt + q_dim1], ldq, &work[iz], &c__1);
    zpp1 = *cutpnt + 1;
    i__1 = *n - *cutpnt;
    dcopy_(&i__1, &q[zpp1 + zpp1 * q_dim1], ldq, &work[iz + *cutpnt], &c__1);

/*     Deflate eigenvalues. */

    dlaed2_(&k, n, cutpnt, &d__[1], &q[q_offset], ldq, &indxq[1], rho, &work[
	    iz], &work[idlmda], &work[iw], &work[iq2], &iwork[indx], &iwork[
	    indxc], &iwork[indxp], &iwork[coltyp], info);

    if (*info != 0) {
	goto L20;
    }

/*     Solve Secular Equation. */

    if (k != 0) {
	is = (iwork[coltyp] + iwork[coltyp + 1]) * *cutpnt + (iwork[coltyp +
		1] + iwork[coltyp + 2]) * (*n - *cutpnt) + iq2;
	dlaed3_(&k, n, cutpnt, &d__[1], &q[q_offset], ldq, rho, &work[idlmda],
		 &work[iq2], &iwork[indxc], &iwork[coltyp], &work[iw], &work[
		is], info);
	if (*info != 0) {
	    goto L20;
	}

/*     Prepare the INDXQ sorting permutation. */

	n1 = k;
	n2 = *n - k;
	dlamrg_(&n1, &n2, &d__[1], &c__1, &c_n1, &indxq[1]);
    } else {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    indxq[i__] = i__;
/* L10: */
	}
    }

L20:
    return 0;

/*     End of DLAED1 */

} /* dlaed1_ */

/* Subroutine */ int dlaed2_(integer *k, integer *n, integer *n1, double *
	d__, double *q, integer *ldq, integer *indxq, double *rho,
	double *z__, double *dlamda, double *w, double *q2,
	integer *indx, integer *indxc, integer *indxp, integer *coltyp,
	integer *info)
{
	/* Table of constant values */
	static double c_b3 = -1.;
	static integer c__1 = 1;

    /* System generated locals */
    integer q_dim1, q_offset, i__1, i__2;
    double d__1, d__2, d__3, d__4;

    /* Builtin functions
    double sqrt(double);*/

    /* Local variables */
    double c__;
    integer i__, j;
    double s, t;
    integer k2, n2, ct, nj, pj, js, iq1, iq2, n1p1;
    double eps, tau, tol;
    integer psm[4], imax, jmax;
    integer ctot[4];


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAED2 merges the two sets of eigenvalues together into a single */
/*  sorted set.  Then it tries to deflate the size of the problem. */
/*  There are two ways in which deflation can occur:  when two or more */
/*  eigenvalues are close together or if there is a tiny entry in the */
/*  Z vector.  For each such occurrence the order of the related secular */
/*  equation problem is reduced by one. */

/*  Arguments */
/*  ========= */

/*  K      (output) INTEGER */
/*         The number of non-deflated eigenvalues, and the order of the */
/*         related secular equation. 0 <= K <=N. */

/*  N      (input) INTEGER */
/*         The dimension of the symmetric tridiagonal matrix.  N >= 0. */

/*  N1     (input) INTEGER */
/*         The location of the last eigenvalue in the leading sub-matrix. */
/*         min(1,N) <= N1 <= N/2. */

/*  D      (input/output) DOUBLE PRECISION array, dimension (N) */
/*         On entry, D contains the eigenvalues of the two submatrices to */
/*         be combined. */
/*         On exit, D contains the trailing (N-K) updated eigenvalues */
/*         (those which were deflated) sorted into increasing order. */

/*  Q      (input/output) DOUBLE PRECISION array, dimension (LDQ, N) */
/*         On entry, Q contains the eigenvectors of two submatrices in */
/*         the two square blocks with corners at (1,1), (N1,N1) */
/*         and (N1+1, N1+1), (N,N). */
/*         On exit, Q contains the trailing (N-K) updated eigenvectors */
/*         (those which were deflated) in its last N-K columns. */

/*  LDQ    (input) INTEGER */
/*         The leading dimension of the array Q.  LDQ >= max(1,N). */

/*  INDXQ  (input/output) INTEGER array, dimension (N) */
/*         The permutation which separately sorts the two sub-problems */
/*         in D into ascending order.  Note that elements in the second */
/*         half of this permutation must first have N1 added to their */
/*         values. Destroyed on exit. */

/*  RHO    (input/output) DOUBLE PRECISION */
/*         On entry, the off-diagonal element associated with the rank-1 */
/*         cut which originally split the two submatrices which are now */
/*         being recombined. */
/*         On exit, RHO has been modified to the value required by */
/*         DLAED3. */

/*  Z      (input) DOUBLE PRECISION array, dimension (N) */
/*         On entry, Z contains the updating vector (the last */
/*         row of the first sub-eigenvector matrix and the first row of */
/*         the second sub-eigenvector matrix). */
/*         On exit, the contents of Z have been destroyed by the updating */
/*         process. */

/*  DLAMDA (output) DOUBLE PRECISION array, dimension (N) */
/*         A copy of the first K eigenvalues which will be used by */
/*         DLAED3 to form the secular equation. */

/*  W      (output) DOUBLE PRECISION array, dimension (N) */
/*         The first k values of the final deflation-altered z-vector */
/*         which will be passed to DLAED3. */

/*  Q2     (output) DOUBLE PRECISION array, dimension (N1**2+(N-N1)**2) */
/*         A copy of the first K eigenvectors which will be used by */
/*         DLAED3 in a matrix multiply (DGEMM) to solve for the new */
/*         eigenvectors. */

/*  INDX   (workspace) INTEGER array, dimension (N) */
/*         The permutation used to sort the contents of DLAMDA into */
/*         ascending order. */

/*  INDXC  (output) INTEGER array, dimension (N) */
/*         The permutation used to arrange the columns of the deflated */
/*         Q matrix into three groups:  the first group contains non-zero */
/*         elements only at and above N1, the second contains */
/*         non-zero elements only below N1, and the third is dense. */

/*  INDXP  (workspace) INTEGER array, dimension (N) */
/*         The permutation used to place deflated values of D at the end */
/*         of the array.  INDXP(1:K) points to the nondeflated D-values */
/*         and INDXP(K+1:N) points to the deflated eigenvalues. */

/*  COLTYP (workspace/output) INTEGER array, dimension (N) */
/*         During execution, a label which will indicate which of the */
/*         following types a column in the Q2 matrix is: */
/*         1 : non-zero in the upper half only; */
/*         2 : dense; */
/*         3 : non-zero in the lower half only; */
/*         4 : deflated. */
/*         On exit, COLTYP(i) is the number of columns of type i, */
/*         for i=1 to 4 only. */

/*  INFO   (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Jeff Rutter, Computer Science Division, University of California */
/*     at Berkeley, USA */
/*  Modified by Francoise Tisseur, University of Tennessee. */

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
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --indxq;
    --z__;
    --dlamda;
    --w;
    --q2;
    --indx;
    --indxc;
    --indxp;
    --coltyp;

    /* Function Body */
    *info = 0;

    if (*n < 0) {
	*info = -2;
    } else if (*ldq < std::max(1_integer,*n)) {
	*info = -6;
    } else /* if(complicated condition) */ {
/* Computing MIN */
	i__1 = 1, i__2 = *n / 2;
	if (std::min(i__1,i__2) > *n1 || *n / 2 < *n1) {
	    *info = -3;
	}
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAED2", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    n2 = *n - *n1;
    n1p1 = *n1 + 1;

    if (*rho < 0.) {
	dscal_(&n2, &c_b3, &z__[n1p1], &c__1);
    }

/*     Normalize z so that norm(z) = 1.  Since z is the concatenation of */
/*     two normalized vectors, norm2(z) = sqrt(2). */

    t = 1. / sqrt(2.);
    dscal_(n, &t, &z__[1], &c__1);

/*     RHO = ABS( norm(z)**2 * RHO ) */

    *rho = (d__1 = *rho * 2., abs(d__1));

/*     Sort the eigenvalues into increasing order */

    i__1 = *n;
    for (i__ = n1p1; i__ <= i__1; ++i__) {
	indxq[i__] += *n1;
/* L10: */
    }

/*     re-integrate the deflated parts from the last pass */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dlamda[i__] = d__[indxq[i__]];
/* L20: */
    }
    dlamrg_(n1, &n2, &dlamda[1], &c__1, &c__1, &indxc[1]);
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	indx[i__] = indxq[indxc[i__]];
/* L30: */
    }

/*     Calculate the allowable deflation tolerance */

    imax = idamax_(n, &z__[1], &c__1);
    jmax = idamax_(n, &d__[1], &c__1);
    eps = dlamch_("Epsilon");
/* Computing MAX */
    d__3 = (d__1 = d__[jmax], abs(d__1)), d__4 = (d__2 = z__[imax], abs(d__2))
	    ;
    tol = eps * 8. * std::max(d__3,d__4);

/*     If the rank-1 modifier is small enough, no more needs to be done */
/*     except to reorganize Q so that its columns correspond with the */
/*     elements in D. */

    if (*rho * (d__1 = z__[imax], abs(d__1)) <= tol) {
	*k = 0;
	iq2 = 1;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__ = indx[j];
	    dcopy_(n, &q[i__ * q_dim1 + 1], &c__1, &q2[iq2], &c__1);
	    dlamda[j] = d__[i__];
	    iq2 += *n;
/* L40: */
	}
	dlacpy_("A", n, n, &q2[1], n, &q[q_offset], ldq);
	dcopy_(n, &dlamda[1], &c__1, &d__[1], &c__1);
	goto L190;
    }

/*     If there are multiple eigenvalues then the problem deflates.  Here */
/*     the number of equal eigenvalues are found.  As each equal */
/*     eigenvalue is found, an elementary reflector is computed to rotate */
/*     the corresponding eigensubspace so that the corresponding */
/*     components of Z are zero in this new basis. */

    i__1 = *n1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	coltyp[i__] = 1;
/* L50: */
    }
    i__1 = *n;
    for (i__ = n1p1; i__ <= i__1; ++i__) {
	coltyp[i__] = 3;
/* L60: */
    }


    *k = 0;
    k2 = *n + 1;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	nj = indx[j];
	if (*rho * (d__1 = z__[nj], abs(d__1)) <= tol) {

/*           Deflate due to small z component. */

	    --k2;
	    coltyp[nj] = 4;
	    indxp[k2] = nj;
	    if (j == *n) {
		goto L100;
	    }
	} else {
	    pj = nj;
	    goto L80;
	}
/* L70: */
    }
L80:
    ++j;
    nj = indx[j];
    if (j > *n) {
	goto L100;
    }
    if (*rho * (d__1 = z__[nj], abs(d__1)) <= tol) {

/*        Deflate due to small z component. */

	--k2;
	coltyp[nj] = 4;
	indxp[k2] = nj;
    } else {

/*        Check if eigenvalues are close enough to allow deflation. */

	s = z__[pj];
	c__ = z__[nj];

/*        Find sqrt(a**2+b**2) without overflow or */
/*        destructive underflow. */

	tau = dlapy2_(&c__, &s);
	t = d__[nj] - d__[pj];
	c__ /= tau;
	s = -s / tau;
	if ((d__1 = t * c__ * s, abs(d__1)) <= tol) {

/*           Deflation is possible. */

	    z__[nj] = tau;
	    z__[pj] = 0.;
	    if (coltyp[nj] != coltyp[pj]) {
		coltyp[nj] = 2;
	    }
	    coltyp[pj] = 4;
	    drot_(n, &q[pj * q_dim1 + 1], &c__1, &q[nj * q_dim1 + 1], &c__1, &
		    c__, &s);
/* Computing 2nd power */
	    d__1 = c__;
/* Computing 2nd power */
	    d__2 = s;
	    t = d__[pj] * (d__1 * d__1) + d__[nj] * (d__2 * d__2);
/* Computing 2nd power */
	    d__1 = s;
/* Computing 2nd power */
	    d__2 = c__;
	    d__[nj] = d__[pj] * (d__1 * d__1) + d__[nj] * (d__2 * d__2);
	    d__[pj] = t;
	    --k2;
	    i__ = 1;
L90:
	    if (k2 + i__ <= *n) {
		if (d__[pj] < d__[indxp[k2 + i__]]) {
		    indxp[k2 + i__ - 1] = indxp[k2 + i__];
		    indxp[k2 + i__] = pj;
		    ++i__;
		    goto L90;
		} else {
		    indxp[k2 + i__ - 1] = pj;
		}
	    } else {
		indxp[k2 + i__ - 1] = pj;
	    }
	    pj = nj;
	} else {
	    ++(*k);
	    dlamda[*k] = d__[pj];
	    w[*k] = z__[pj];
	    indxp[*k] = pj;
	    pj = nj;
	}
    }
    goto L80;
L100:

/*     Record the last eigenvalue. */

    ++(*k);
    dlamda[*k] = d__[pj];
    w[*k] = z__[pj];
    indxp[*k] = pj;

/*     Count up the total number of the various types of columns, then */
/*     form a permutation which positions the four column types into */
/*     four uniform groups (although one or more of these groups may be */
/*     empty). */

    for (j = 1; j <= 4; ++j) {
	ctot[j - 1] = 0;
/* L110: */
    }
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	ct = coltyp[j];
	++ctot[ct - 1];
/* L120: */
    }

/*     PSM(*) = Position in SubMatrix (of types 1 through 4) */

    psm[0] = 1;
    psm[1] = ctot[0] + 1;
    psm[2] = psm[1] + ctot[1];
    psm[3] = psm[2] + ctot[2];
    *k = *n - ctot[3];

/*     Fill out the INDXC array so that the permutation which it induces */
/*     will place all type-1 columns first, all type-2 columns next, */
/*     then all type-3's, and finally all type-4's. */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	js = indxp[j];
	ct = coltyp[js];
	indx[psm[ct - 1]] = js;
	indxc[psm[ct - 1]] = j;
	++psm[ct - 1];
/* L130: */
    }

/*     Sort the eigenvalues and corresponding eigenvectors into DLAMDA */
/*     and Q2 respectively.  The eigenvalues/vectors which were not */
/*     deflated go into the first K slots of DLAMDA and Q2 respectively, */
/*     while those which were deflated go into the last N - K slots. */

    i__ = 1;
    iq1 = 1;
    iq2 = (ctot[0] + ctot[1]) * *n1 + 1;
    i__1 = ctot[0];
    for (j = 1; j <= i__1; ++j) {
	js = indx[i__];
	dcopy_(n1, &q[js * q_dim1 + 1], &c__1, &q2[iq1], &c__1);
	z__[i__] = d__[js];
	++i__;
	iq1 += *n1;
/* L140: */
    }

    i__1 = ctot[1];
    for (j = 1; j <= i__1; ++j) {
	js = indx[i__];
	dcopy_(n1, &q[js * q_dim1 + 1], &c__1, &q2[iq1], &c__1);
	dcopy_(&n2, &q[*n1 + 1 + js * q_dim1], &c__1, &q2[iq2], &c__1);
	z__[i__] = d__[js];
	++i__;
	iq1 += *n1;
	iq2 += n2;
/* L150: */
    }

    i__1 = ctot[2];
    for (j = 1; j <= i__1; ++j) {
	js = indx[i__];
	dcopy_(&n2, &q[*n1 + 1 + js * q_dim1], &c__1, &q2[iq2], &c__1);
	z__[i__] = d__[js];
	++i__;
	iq2 += n2;
/* L160: */
    }

    iq1 = iq2;
    i__1 = ctot[3];
    for (j = 1; j <= i__1; ++j) {
	js = indx[i__];
	dcopy_(n, &q[js * q_dim1 + 1], &c__1, &q2[iq2], &c__1);
	iq2 += *n;
	z__[i__] = d__[js];
	++i__;
/* L170: */
    }

/*     The deflated eigenvalues and their corresponding vectors go back */
/*     into the last N - K slots of D and Q respectively. */

    dlacpy_("A", n, &ctot[3], &q2[iq1], n, &q[(*k + 1) * q_dim1 + 1], ldq);
    i__1 = *n - *k;
    dcopy_(&i__1, &z__[*k + 1], &c__1, &d__[*k + 1], &c__1);

/*     Copy CTOT into COLTYP for referencing in DLAED3. */

    for (j = 1; j <= 4; ++j) {
	coltyp[j] = ctot[j - 1];
/* L180: */
    }

L190:
    return 0;

/*     End of DLAED2 */

} /* dlaed2_ */

/* Subroutine */ int dlaed3_(integer *k, integer *n, integer *n1, double *
	d__, double *q, integer *ldq, double *rho, double *dlamda,
	double *q2, integer *indx, integer *ctot, double *w,
	double *s, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b22 = 1.;
	static double c_b23 = 0.;

    /* System generated locals */
    integer q_dim1, q_offset, i__1, i__2;
    double d__1;

    /* Builtin functions
    double sqrt(double), d_sign(double *, double *);*/

    /* Local variables */
    integer i__, j, n2, n12, ii, n23, iq2;
    double temp;

/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAED3 finds the roots of the secular equation, as defined by the */
/*  values in D, W, and RHO, between 1 and K.  It makes the */
/*  appropriate calls to DLAED4 and then updates the eigenvectors by */
/*  multiplying the matrix of eigenvectors of the pair of eigensystems */
/*  being combined by the matrix of eigenvectors of the K-by-K system */
/*  which is solved here. */

/*  This code makes very mild assumptions about floating point */
/*  arithmetic. It will work on machines with a guard digit in */
/*  add/subtract, or on those binary machines without guard digits */
/*  which subtract like the Cray X-MP, Cray Y-MP, Cray C-90, or Cray-2. */
/*  It could conceivably fail on hexadecimal or decimal machines */
/*  without guard digits, but we know of none. */

/*  Arguments */
/*  ========= */

/*  K       (input) INTEGER */
/*          The number of terms in the rational function to be solved by */
/*          DLAED4.  K >= 0. */

/*  N       (input) INTEGER */
/*          The number of rows and columns in the Q matrix. */
/*          N >= K (deflation may result in N>K). */

/*  N1      (input) INTEGER */
/*          The location of the last eigenvalue in the leading submatrix. */
/*          min(1,N) <= N1 <= N/2. */

/*  D       (output) DOUBLE PRECISION array, dimension (N) */
/*          D(I) contains the updated eigenvalues for */
/*          1 <= I <= K. */

/*  Q       (output) DOUBLE PRECISION array, dimension (LDQ,N) */
/*          Initially the first K columns are used as workspace. */
/*          On output the columns 1 to K contain */
/*          the updated eigenvectors. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q.  LDQ >= max(1,N). */

/*  RHO     (input) DOUBLE PRECISION */
/*          The value of the parameter in the rank one update equation. */
/*          RHO >= 0 required. */

/*  DLAMDA  (input/output) DOUBLE PRECISION array, dimension (K) */
/*          The first K elements of this array contain the old roots */
/*          of the deflated updating problem.  These are the poles */
/*          of the secular equation. May be changed on output by */
/*          having lowest order bit set to zero on Cray X-MP, Cray Y-MP, */
/*          Cray-2, or Cray C-90, as described above. */

/*  Q2      (input) DOUBLE PRECISION array, dimension (LDQ2, N) */
/*          The first K columns of this matrix contain the non-deflated */
/*          eigenvectors for the split problem. */

/*  INDX    (input) INTEGER array, dimension (N) */
/*          The permutation used to arrange the columns of the deflated */
/*          Q matrix into three groups (see DLAED2). */
/*          The rows of the eigenvectors found by DLAED4 must be likewise */
/*          permuted before the matrix multiply can take place. */

/*  CTOT    (input) INTEGER array, dimension (4) */
/*          A count of the total number of the various types of columns */
/*          in Q, as described in INDX.  The fourth column type is any */
/*          column which has been deflated. */

/*  W       (input/output) DOUBLE PRECISION array, dimension (K) */
/*          The first K elements of this array contain the components */
/*          of the deflation-adjusted updating vector. Destroyed on */
/*          output. */

/*  S       (workspace) DOUBLE PRECISION array, dimension (N1 + 1)*K */
/*          Will contain the eigenvectors of the repaired matrix which */
/*          will be multiplied by the previously accumulated eigenvectors */
/*          to update the system. */

/*  LDS     (input) INTEGER */
/*          The leading dimension of S.  LDS >= max(1,K). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = 1, an eigenvalue did not converge */

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
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --dlamda;
    --q2;
    --indx;
    --ctot;
    --w;
    --s;

    /* Function Body */
    *info = 0;

    if (*k < 0) {
	*info = -1;
    } else if (*n < *k) {
	*info = -2;
    } else if (*ldq < std::max(1_integer,*n)) {
	*info = -6;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAED3", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*k == 0) {
	return 0;
    }

/*     Modify values DLAMDA(i) to make sure all DLAMDA(i)-DLAMDA(j) can */
/*     be computed with high relative accuracy (barring over/underflow). */
/*     This is a problem on machines without a guard digit in */
/*     add/subtract (Cray XMP, Cray YMP, Cray C 90 and Cray 2). */
/*     The following code replaces DLAMDA(I) by 2*DLAMDA(I)-DLAMDA(I), */
/*     which on any of these machines zeros out the bottommost */
/*     bit of DLAMDA(I) if it is 1; this makes the subsequent */
/*     subtractions DLAMDA(I)-DLAMDA(J) unproblematic when cancellation */
/*     occurs. On binary machines with a guard digit (almost all */
/*     machines) it does not change DLAMDA(I) at all. On hexadecimal */
/*     and decimal machines with a guard digit, it slightly */
/*     changes the bottommost bits of DLAMDA(I). It does not account */
/*     for hexadecimal or decimal machines without guard digits */
/*     (we know of none). We use a subroutine call to compute */
/*     2*DLAMBDA(I) to prevent optimizing compilers from eliminating */
/*     this code. */

    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dlamda[i__] = dlamc3_(&dlamda[i__], &dlamda[i__]) - dlamda[i__];
/* L10: */
    }

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	dlaed4_(k, &j, &dlamda[1], &w[1], &q[j * q_dim1 + 1], rho, &d__[j],
		info);

/*        If the zero finder fails, the computation is terminated. */

	if (*info != 0) {
	    goto L120;
	}
/* L20: */
    }

    if (*k == 1) {
	goto L110;
    }
    if (*k == 2) {
	i__1 = *k;
	for (j = 1; j <= i__1; ++j) {
	    w[1] = q[j * q_dim1 + 1];
	    w[2] = q[j * q_dim1 + 2];
	    ii = indx[1];
	    q[j * q_dim1 + 1] = w[ii];
	    ii = indx[2];
	    q[j * q_dim1 + 2] = w[ii];
/* L30: */
	}
	goto L110;
    }

/*     Compute updated W. */

    dcopy_(k, &w[1], &c__1, &s[1], &c__1);

/*     Initialize W(I) = Q(I,I) */

    i__1 = *ldq + 1;
    dcopy_(k, &q[q_offset], &i__1, &w[1], &c__1);
    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    w[i__] *= q[i__ + j * q_dim1] / (dlamda[i__] - dlamda[j]);
/* L40: */
	}
	i__2 = *k;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    w[i__] *= q[i__ + j * q_dim1] / (dlamda[i__] - dlamda[j]);
/* L50: */
	}
/* L60: */
    }
    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__1 = sqrt(-w[i__]);
	w[i__] = d_sign(&d__1, &s[i__]);
/* L70: */
    }

/*     Compute eigenvectors of the modified rank-1 modification. */

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *k;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    s[i__] = w[i__] / q[i__ + j * q_dim1];
/* L80: */
	}
	temp = dnrm2_(k, &s[1], &c__1);
	i__2 = *k;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    ii = indx[i__];
	    q[i__ + j * q_dim1] = s[ii] / temp;
/* L90: */
	}
/* L100: */
    }

/*     Compute the updated eigenvectors. */

L110:

    n2 = *n - *n1;
    n12 = ctot[1] + ctot[2];
    n23 = ctot[2] + ctot[3];

    dlacpy_("A", &n23, k, &q[ctot[1] + 1 + q_dim1], ldq, &s[1], &n23);
    iq2 = *n1 * n12 + 1;
    if (n23 != 0) {
	dgemm_("N", "N", &n2, k, &n23, &c_b22, &q2[iq2], &n2, &s[1], &n23, &
		c_b23, &q[*n1 + 1 + q_dim1], ldq);
    } else {
	dlaset_("A", &n2, k, &c_b23, &c_b23, &q[*n1 + 1 + q_dim1], ldq);
    }

    dlacpy_("A", &n12, k, &q[q_offset], ldq, &s[1], &n12);
    if (n12 != 0) {
	dgemm_("N", "N", n1, k, &n12, &c_b22, &q2[1], n1, &s[1], &n12, &c_b23,
		 &q[q_offset], ldq);
    } else {
	dlaset_("A", n1, k, &c_b23, &c_b23, &q[q_dim1 + 1], ldq);
    }


L120:
    return 0;

/*     End of DLAED3 */

} /* dlaed3_ */

/* Subroutine */ int dlaed4_(integer *n, integer *i__, double *d__,
	double *z__, double *delta, double *rho, double *dlam,
	integer *info)
{
    /* System generated locals */
    integer i__1;
    double d__1;

    /* Local variables */
    double a, b, c__;
    integer j;
    double w;
    integer ii;
    double dw, zz[3];
    integer ip1;
    double del, eta, phi, eps, tau, psi;
    integer iim1, iip1;
    double dphi, dpsi;
    integer iter;
    double temp, prew, temp1, dltlb, dltub, midpt;
    integer niter;
    bool swtch;
    bool swtch3;
    bool orgati;
    double erretm, rhoinv;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  This subroutine computes the I-th updated eigenvalue of a symmetric */
/*  rank-one modification to a diagonal matrix whose elements are */
/*  given in the array d, and that */

/*             D(i) < D(j)  for  i < j */

/*  and that RHO > 0.  This is arranged by the calling routine, and is */
/*  no loss in generality.  The rank-one modified system is thus */

/*             diag( D )  +  RHO *  Z * Z_transpose. */

/*  where we assume the Euclidean norm of Z is 1. */

/*  The method consists of approximating the rational functions in the */
/*  secular equation by simpler interpolating rational functions. */

/*  Arguments */
/*  ========= */

/*  N      (input) INTEGER */
/*         The length of all arrays. */

/*  I      (input) INTEGER */
/*         The index of the eigenvalue to be computed.  1 <= I <= N. */

/*  D      (input) DOUBLE PRECISION array, dimension (N) */
/*         The original eigenvalues.  It is assumed that they are in */
/*         order, D(I) < D(J)  for I < J. */

/*  Z      (input) DOUBLE PRECISION array, dimension (N) */
/*         The components of the updating vector. */

/*  DELTA  (output) DOUBLE PRECISION array, dimension (N) */
/*         If N .GT. 2, DELTA contains (D(j) - lambda_I) in its  j-th */
/*         component.  If N = 1, then DELTA(1) = 1. If N = 2, see DLAED5 */
/*         for detail. The vector DELTA contains the information necessary */
/*         to construct the eigenvectors by DLAED3 and DLAED9. */

/*  RHO    (input) DOUBLE PRECISION */
/*         The scalar in the symmetric updating formula. */

/*  DLAM   (output) DOUBLE PRECISION */
/*         The computed lambda_I, the I-th updated eigenvalue. */

/*  INFO   (output) INTEGER */
/*         = 0:  successful exit */
/*         > 0:  if INFO = 1, the updating process failed. */

/*  Internal Parameters */
/*  =================== */

/*  Logical variable ORGATI (origin-at-i?) is used for distinguishing */
/*  whether D(i) or D(i+1) is treated as the origin. */

/*            ORGATI = .true.    origin at i */
/*            ORGATI = .false.   origin at i+1 */

/*   Logical variable SWTCH3 (switch-for-3-poles?) is for noting */
/*   if we are working with THREE poles! */

/*   MAXIT is the maximum number of iterations allowed for each */
/*   eigenvalue. */

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
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Since this routine is called in an inner loop, we do no argument */
/*     checking. */

/*     Quick return for N=1 and 2. */

    /* Parameter adjustments */
    --delta;
    --z__;
    --d__;

    /* Function Body */
    *info = 0;
    if (*n == 1) {

/*         Presumably, I=1 upon entry */

	*dlam = d__[1] + *rho * z__[1] * z__[1];
	delta[1] = 1.;
	return 0;
    }
    if (*n == 2) {
	dlaed5_(i__, &d__[1], &z__[1], &delta[1], rho, dlam);
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

	midpt = *rho / 2.;

/*        If ||Z||_2 is not one, then TEMP should be set to */
/*        RHO * ||Z||_2^2 / TWO */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] = d__[j] - d__[*i__] - midpt;
/* L10: */
	}

	psi = 0.;
	i__1 = *n - 2;
	for (j = 1; j <= i__1; ++j) {
	    psi += z__[j] * z__[j] / delta[j];
/* L20: */
	}

	c__ = rhoinv + psi;
	w = c__ + z__[ii] * z__[ii] / delta[ii] + z__[*n] * z__[*n] / delta[*
		n];

	if (w <= 0.) {
	    temp = z__[*n - 1] * z__[*n - 1] / (d__[*n] - d__[*n - 1] + *rho)
		    + z__[*n] * z__[*n] / *rho;
	    if (c__ <= temp) {
		tau = *rho;
	    } else {
		del = d__[*n] - d__[*n - 1];
		a = -c__ * del + z__[*n - 1] * z__[*n - 1] + z__[*n] * z__[*n]
			;
		b = z__[*n] * z__[*n] * del;
		if (a < 0.) {
		    tau = b * 2. / (sqrt(a * a + b * 4. * c__) - a);
		} else {
		    tau = (a + sqrt(a * a + b * 4. * c__)) / (c__ * 2.);
		}
	    }

/*           It can be proved that */
/*               D(N)+RHO/2 <= LAMBDA(N) < D(N)+TAU <= D(N)+RHO */

	    dltlb = midpt;
	    dltub = *rho;
	} else {
	    del = d__[*n] - d__[*n - 1];
	    a = -c__ * del + z__[*n - 1] * z__[*n - 1] + z__[*n] * z__[*n];
	    b = z__[*n] * z__[*n] * del;
	    if (a < 0.) {
		tau = b * 2. / (sqrt(a * a + b * 4. * c__) - a);
	    } else {
		tau = (a + sqrt(a * a + b * 4. * c__)) / (c__ * 2.);
	    }

/*           It can be proved that */
/*               D(N) < D(N)+TAU < LAMBDA(N) < D(N)+RHO/2 */

	    dltlb = 0.;
	    dltub = midpt;
	}

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] = d__[j] - d__[*i__] - tau;
/* L30: */
	}

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = ii;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / delta[j];
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L40: */
	}
	erretm = abs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	temp = z__[*n] / delta[*n];
	phi = z__[*n] * temp;
	dphi = temp * temp;
	erretm = (-phi - psi) * 8. + erretm - phi + rhoinv + abs(tau) * (dpsi
		+ dphi);

	w = rhoinv + phi + psi;

/*        Test for convergence */

	if (abs(w) <= eps * erretm) {
	    *dlam = d__[*i__] + tau;
	    goto L250;
	}

	if (w <= 0.) {
	    dltlb = std::max(dltlb,tau);
	} else {
	    dltub = std::min(dltub,tau);
	}

/*        Calculate the new step */

	++niter;
	c__ = w - delta[*n - 1] * dpsi - delta[*n] * dphi;
	a = (delta[*n - 1] + delta[*n]) * w - delta[*n - 1] * delta[*n] * (
		dpsi + dphi);
	b = delta[*n - 1] * delta[*n] * w;
	if (c__ < 0.) {
	    c__ = abs(c__);
	}
	if (c__ == 0.) {
/*          ETA = B/A */
/*           ETA = RHO - TAU */
	    eta = dltub - tau;
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
	temp = tau + eta;
	if (temp > dltub || temp < dltlb) {
	    if (w < 0.) {
		eta = (dltub - tau) / 2.;
	    } else {
		eta = (dltlb - tau) / 2.;
	    }
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] -= eta;
/* L50: */
	}

	tau += eta;

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = ii;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / delta[j];
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L60: */
	}
	erretm = abs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	temp = z__[*n] / delta[*n];
	phi = z__[*n] * temp;
	dphi = temp * temp;
	erretm = (-phi - psi) * 8. + erretm - phi + rhoinv + abs(tau) * (dpsi
		+ dphi);

	w = rhoinv + phi + psi;

/*        Main loop to update the values of the array   DELTA */

	iter = niter + 1;

	for (niter = iter; niter <= 30; ++niter) {

/*           Test for convergence */

	    if (abs(w) <= eps * erretm) {
		*dlam = d__[*i__] + tau;
		goto L250;
	    }

	    if (w <= 0.) {
		dltlb = std::max(dltlb,tau);
	    } else {
		dltub = std::min(dltub,tau);
	    }

/*           Calculate the new step */

	    c__ = w - delta[*n - 1] * dpsi - delta[*n] * dphi;
	    a = (delta[*n - 1] + delta[*n]) * w - delta[*n - 1] * delta[*n] *
		    (dpsi + dphi);
	    b = delta[*n - 1] * delta[*n] * w;
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
	    temp = tau + eta;
	    if (temp > dltub || temp < dltlb) {
		if (w < 0.) {
		    eta = (dltub - tau) / 2.;
		} else {
		    eta = (dltlb - tau) / 2.;
		}
	    }
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		delta[j] -= eta;
/* L70: */
	    }

	    tau += eta;

/*           Evaluate PSI and the derivative DPSI */

	    dpsi = 0.;
	    psi = 0.;
	    erretm = 0.;
	    i__1 = ii;
	    for (j = 1; j <= i__1; ++j) {
		temp = z__[j] / delta[j];
		psi += z__[j] * temp;
		dpsi += temp * temp;
		erretm += psi;
/* L80: */
	    }
	    erretm = abs(erretm);

/*           Evaluate PHI and the derivative DPHI */

	    temp = z__[*n] / delta[*n];
	    phi = z__[*n] * temp;
	    dphi = temp * temp;
	    erretm = (-phi - psi) * 8. + erretm - phi + rhoinv + abs(tau) * (
		    dpsi + dphi);

	    w = rhoinv + phi + psi;
/* L90: */
	}

/*        Return with INFO = 1, NITER = MAXIT and not converged */

	*info = 1;
	*dlam = d__[*i__] + tau;
	goto L250;

/*        End for the case I = N */

    } else {

/*        The case for I < N */

	niter = 1;
	ip1 = *i__ + 1;

/*        Calculate initial guess */

	del = d__[ip1] - d__[*i__];
	midpt = del / 2.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] = d__[j] - d__[*i__] - midpt;
/* L100: */
	}

	psi = 0.;
	i__1 = *i__ - 1;
	for (j = 1; j <= i__1; ++j) {
	    psi += z__[j] * z__[j] / delta[j];
/* L110: */
	}

	phi = 0.;
	i__1 = *i__ + 2;
	for (j = *n; j >= i__1; --j) {
	    phi += z__[j] * z__[j] / delta[j];
/* L120: */
	}
	c__ = rhoinv + psi + phi;
	w = c__ + z__[*i__] * z__[*i__] / delta[*i__] + z__[ip1] * z__[ip1] /
		delta[ip1];

	if (w > 0.) {

/*           d(i)< the ith eigenvalue < (d(i)+d(i+1))/2 */

/*           We choose d(i) as origin. */

	    orgati = true;
	    a = c__ * del + z__[*i__] * z__[*i__] + z__[ip1] * z__[ip1];
	    b = z__[*i__] * z__[*i__] * del;
	    if (a > 0.) {
		tau = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__, abs(
			d__1))));
	    } else {
		tau = (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (
			c__ * 2.);
	    }
	    dltlb = 0.;
	    dltub = midpt;
	} else {

/*           (d(i)+d(i+1))/2 <= the ith eigenvalue < d(i+1) */

/*           We choose d(i+1) as origin. */

	    orgati = false;
	    a = c__ * del - z__[*i__] * z__[*i__] - z__[ip1] * z__[ip1];
	    b = z__[ip1] * z__[ip1] * del;
	    if (a < 0.) {
		tau = b * 2. / (a - sqrt((d__1 = a * a + b * 4. * c__, abs(
			d__1))));
	    } else {
		tau = -(a + sqrt((d__1 = a * a + b * 4. * c__, abs(d__1)))) /
			(c__ * 2.);
	    }
	    dltlb = -midpt;
	    dltub = 0.;
	}

	if (orgati) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		delta[j] = d__[j] - d__[*i__] - tau;
/* L130: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		delta[j] = d__[j] - d__[ip1] - tau;
/* L140: */
	    }
	}
	if (orgati) {
	    ii = *i__;
	} else {
	    ii = *i__ + 1;
	}
	iim1 = ii - 1;
	iip1 = ii + 1;

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = iim1;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / delta[j];
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
	    temp = z__[j] / delta[j];
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

	temp = z__[ii] / delta[ii];
	dw = dpsi + dphi + temp * temp;
	temp = z__[ii] * temp;
	w += temp;
	erretm = (phi - psi) * 8. + erretm + rhoinv * 2. + abs(temp) * 3. +
		abs(tau) * dw;

/*        Test for convergence */

	if (abs(w) <= eps * erretm) {
	    if (orgati) {
		*dlam = d__[*i__] + tau;
	    } else {
		*dlam = d__[ip1] + tau;
	    }
	    goto L250;
	}

	if (w <= 0.) {
	    dltlb = std::max(dltlb,tau);
	} else {
	    dltub = std::min(dltub,tau);
	}

/*        Calculate the new step */

	++niter;
	if (! swtch3) {
	    if (orgati) {
/* Computing 2nd power */
		d__1 = z__[*i__] / delta[*i__];
		c__ = w - delta[ip1] * dw - (d__[*i__] - d__[ip1]) * (d__1 *
			d__1);
	    } else {
/* Computing 2nd power */
		d__1 = z__[ip1] / delta[ip1];
		c__ = w - delta[*i__] * dw - (d__[ip1] - d__[*i__]) * (d__1 *
			d__1);
	    }
	    a = (delta[*i__] + delta[ip1]) * w - delta[*i__] * delta[ip1] *
		    dw;
	    b = delta[*i__] * delta[ip1] * w;
	    if (c__ == 0.) {
		if (a == 0.) {
		    if (orgati) {
			a = z__[*i__] * z__[*i__] + delta[ip1] * delta[ip1] *
				(dpsi + dphi);
		    } else {
			a = z__[ip1] * z__[ip1] + delta[*i__] * delta[*i__] *
				(dpsi + dphi);
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

	    temp = rhoinv + psi + phi;
	    if (orgati) {
		temp1 = z__[iim1] / delta[iim1];
		temp1 *= temp1;
		c__ = temp - delta[iip1] * (dpsi + dphi) - (d__[iim1] - d__[
			iip1]) * temp1;
		zz[0] = z__[iim1] * z__[iim1];
		zz[2] = delta[iip1] * delta[iip1] * (dpsi - temp1 + dphi);
	    } else {
		temp1 = z__[iip1] / delta[iip1];
		temp1 *= temp1;
		c__ = temp - delta[iim1] * (dpsi + dphi) - (d__[iip1] - d__[
			iim1]) * temp1;
		zz[0] = delta[iim1] * delta[iim1] * (dpsi + (dphi - temp1));
		zz[2] = z__[iip1] * z__[iip1];
	    }
	    zz[1] = z__[ii] * z__[ii];
	    dlaed6_(&niter, &orgati, &c__, &delta[iim1], zz, &w, &eta, info);
	    if (*info != 0) {
		goto L250;
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
	temp = tau + eta;
	if (temp > dltub || temp < dltlb) {
	    if (w < 0.) {
		eta = (dltub - tau) / 2.;
	    } else {
		eta = (dltlb - tau) / 2.;
	    }
	}

	prew = w;

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    delta[j] -= eta;
/* L180: */
	}

/*        Evaluate PSI and the derivative DPSI */

	dpsi = 0.;
	psi = 0.;
	erretm = 0.;
	i__1 = iim1;
	for (j = 1; j <= i__1; ++j) {
	    temp = z__[j] / delta[j];
	    psi += z__[j] * temp;
	    dpsi += temp * temp;
	    erretm += psi;
/* L190: */
	}
	erretm = abs(erretm);

/*        Evaluate PHI and the derivative DPHI */

	dphi = 0.;
	phi = 0.;
	i__1 = iip1;
	for (j = *n; j >= i__1; --j) {
	    temp = z__[j] / delta[j];
	    phi += z__[j] * temp;
	    dphi += temp * temp;
	    erretm += phi;
/* L200: */
	}

	temp = z__[ii] / delta[ii];
	dw = dpsi + dphi + temp * temp;
	temp = z__[ii] * temp;
	w = rhoinv + phi + psi + temp;
	erretm = (phi - psi) * 8. + erretm + rhoinv * 2. + abs(temp) * 3. + (
		d__1 = tau + eta, abs(d__1)) * dw;

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

	tau += eta;

/*        Main loop to update the values of the array   DELTA */

	iter = niter + 1;

	for (niter = iter; niter <= 30; ++niter) {

/*           Test for convergence */

	    if (abs(w) <= eps * erretm) {
		if (orgati) {
		    *dlam = d__[*i__] + tau;
		} else {
		    *dlam = d__[ip1] + tau;
		}
		goto L250;
	    }

	    if (w <= 0.) {
		dltlb = std::max(dltlb,tau);
	    } else {
		dltub = std::min(dltub,tau);
	    }

/*           Calculate the new step */

	    if (! swtch3) {
		if (! swtch) {
		    if (orgati) {
/* Computing 2nd power */
			d__1 = z__[*i__] / delta[*i__];
			c__ = w - delta[ip1] * dw - (d__[*i__] - d__[ip1]) * (
				d__1 * d__1);
		    } else {
/* Computing 2nd power */
			d__1 = z__[ip1] / delta[ip1];
			c__ = w - delta[*i__] * dw - (d__[ip1] - d__[*i__]) *
				(d__1 * d__1);
		    }
		} else {
		    temp = z__[ii] / delta[ii];
		    if (orgati) {
			dpsi += temp * temp;
		    } else {
			dphi += temp * temp;
		    }
		    c__ = w - delta[*i__] * dpsi - delta[ip1] * dphi;
		}
		a = (delta[*i__] + delta[ip1]) * w - delta[*i__] * delta[ip1]
			* dw;
		b = delta[*i__] * delta[ip1] * w;
		if (c__ == 0.) {
		    if (a == 0.) {
			if (! swtch) {
			    if (orgati) {
				a = z__[*i__] * z__[*i__] + delta[ip1] *
					delta[ip1] * (dpsi + dphi);
			    } else {
				a = z__[ip1] * z__[ip1] + delta[*i__] * delta[
					*i__] * (dpsi + dphi);
			    }
			} else {
			    a = delta[*i__] * delta[*i__] * dpsi + delta[ip1]
				    * delta[ip1] * dphi;
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

		temp = rhoinv + psi + phi;
		if (swtch) {
		    c__ = temp - delta[iim1] * dpsi - delta[iip1] * dphi;
		    zz[0] = delta[iim1] * delta[iim1] * dpsi;
		    zz[2] = delta[iip1] * delta[iip1] * dphi;
		} else {
		    if (orgati) {
			temp1 = z__[iim1] / delta[iim1];
			temp1 *= temp1;
			c__ = temp - delta[iip1] * (dpsi + dphi) - (d__[iim1]
				- d__[iip1]) * temp1;
			zz[0] = z__[iim1] * z__[iim1];
			zz[2] = delta[iip1] * delta[iip1] * (dpsi - temp1 +
				dphi);
		    } else {
			temp1 = z__[iip1] / delta[iip1];
			temp1 *= temp1;
			c__ = temp - delta[iim1] * (dpsi + dphi) - (d__[iip1]
				- d__[iim1]) * temp1;
			zz[0] = delta[iim1] * delta[iim1] * (dpsi + (dphi -
				temp1));
			zz[2] = z__[iip1] * z__[iip1];
		    }
		}
		dlaed6_(&niter, &orgati, &c__, &delta[iim1], zz, &w, &eta,
			info);
		if (*info != 0) {
		    goto L250;
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
	    temp = tau + eta;
	    if (temp > dltub || temp < dltlb) {
		if (w < 0.) {
		    eta = (dltub - tau) / 2.;
		} else {
		    eta = (dltlb - tau) / 2.;
		}
	    }

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		delta[j] -= eta;
/* L210: */
	    }

	    tau += eta;
	    prew = w;

/*           Evaluate PSI and the derivative DPSI */

	    dpsi = 0.;
	    psi = 0.;
	    erretm = 0.;
	    i__1 = iim1;
	    for (j = 1; j <= i__1; ++j) {
		temp = z__[j] / delta[j];
		psi += z__[j] * temp;
		dpsi += temp * temp;
		erretm += psi;
/* L220: */
	    }
	    erretm = abs(erretm);

/*           Evaluate PHI and the derivative DPHI */

	    dphi = 0.;
	    phi = 0.;
	    i__1 = iip1;
	    for (j = *n; j >= i__1; --j) {
		temp = z__[j] / delta[j];
		phi += z__[j] * temp;
		dphi += temp * temp;
		erretm += phi;
/* L230: */
	    }

	    temp = z__[ii] / delta[ii];
	    dw = dpsi + dphi + temp * temp;
	    temp = z__[ii] * temp;
	    w = rhoinv + phi + psi + temp;
	    erretm = (phi - psi) * 8. + erretm + rhoinv * 2. + abs(temp) * 3.
		    + abs(tau) * dw;
	    if (w * prew > 0. && abs(w) > abs(prew) / 10.) {
		swtch = ! swtch;
	    }

/* L240: */
	}

/*        Return with INFO = 1, NITER = MAXIT and not converged */

	*info = 1;
	if (orgati) {
	    *dlam = d__[*i__] + tau;
	} else {
	    *dlam = d__[ip1] + tau;
	}

    }

L250:

    return 0;

/*     End of DLAED4 */

} /* dlaed4_ */

/* Subroutine */ int dlaed5_(integer *i__, double *d__, double *z__,
	double *delta, double *rho, double *dlam)
{
    /* System generated locals */
    double d__1;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    double b, c__, w, del, tau, temp;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  This subroutine computes the I-th eigenvalue of a symmetric rank-one */
/*  modification of a 2-by-2 diagonal matrix */

/*             diag( D )  +  RHO *  Z * transpose(Z) . */

/*  The diagonal elements in the array D are assumed to satisfy */

/*             D(i) < D(j)  for  i < j . */

/*  We also assume RHO > 0 and that the Euclidean norm of the vector */
/*  Z is one. */

/*  Arguments */
/*  ========= */

/*  I      (input) INTEGER */
/*         The index of the eigenvalue to be computed.  I = 1 or I = 2. */

/*  D      (input) DOUBLE PRECISION array, dimension (2) */
/*         The original eigenvalues.  We assume D(1) < D(2). */

/*  Z      (input) DOUBLE PRECISION array, dimension (2) */
/*         The components of the updating vector. */

/*  DELTA  (output) DOUBLE PRECISION array, dimension (2) */
/*         The vector DELTA contains the information necessary */
/*         to construct the eigenvectors. */

/*  RHO    (input) DOUBLE PRECISION */
/*         The scalar in the symmetric updating formula. */

/*  DLAM   (output) DOUBLE PRECISION */
/*         The computed lambda_I, the I-th updated eigenvalue. */

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
    --delta;
    --z__;
    --d__;

    /* Function Body */
    del = d__[2] - d__[1];
    if (*i__ == 1) {
	w = *rho * 2. * (z__[2] * z__[2] - z__[1] * z__[1]) / del + 1.;
	if (w > 0.) {
	    b = del + *rho * (z__[1] * z__[1] + z__[2] * z__[2]);
	    c__ = *rho * z__[1] * z__[1] * del;

/*           B > ZERO, always */

	    tau = c__ * 2. / (b + sqrt((d__1 = b * b - c__ * 4., abs(d__1))));
	    *dlam = d__[1] + tau;
	    delta[1] = -z__[1] / tau;
	    delta[2] = z__[2] / (del - tau);
	} else {
	    b = -del + *rho * (z__[1] * z__[1] + z__[2] * z__[2]);
	    c__ = *rho * z__[2] * z__[2] * del;
	    if (b > 0.) {
		tau = c__ * -2. / (b + sqrt(b * b + c__ * 4.));
	    } else {
		tau = (b - sqrt(b * b + c__ * 4.)) / 2.;
	    }
	    *dlam = d__[2] + tau;
	    delta[1] = -z__[1] / (del + tau);
	    delta[2] = -z__[2] / tau;
	}
	temp = sqrt(delta[1] * delta[1] + delta[2] * delta[2]);
	delta[1] /= temp;
	delta[2] /= temp;
    } else {

/*     Now I=2 */

	b = -del + *rho * (z__[1] * z__[1] + z__[2] * z__[2]);
	c__ = *rho * z__[2] * z__[2] * del;
	if (b > 0.) {
	    tau = (b + sqrt(b * b + c__ * 4.)) / 2.;
	} else {
	    tau = c__ * 2. / (-b + sqrt(b * b + c__ * 4.));
	}
	*dlam = d__[2] + tau;
	delta[1] = -z__[1] / (del + tau);
	delta[2] = -z__[2] / tau;
	temp = sqrt(delta[1] * delta[1] + delta[2] * delta[2]);
	delta[1] /= temp;
	delta[2] /= temp;
    }
    return 0;

/*     End OF DLAED5 */

} /* dlaed5_ */

/* Subroutine */ int dlaed6_(integer *kniter, bool *orgati, double *
	rho, double *d__, double *z__, double *finit, double *
	tau, integer *info)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2, d__3, d__4;

    /* Local variables */
    double a, b, c__, f;
    integer i__;
    double fc, df, ddf, lbd, eta, ubd, eps, base;
    integer iter;
    double temp, temp1, temp2, temp3, temp4;
    bool scale;
    integer niter;
    double small1, small2, sminv1, sminv2;

    double dscale[3], sclfac, zscale[3], erretm, sclinv;


/*  -- LAPACK routine (version 3.1.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     February 2007 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAED6 computes the positive or negative root (closest to the origin) */
/*  of */
/*                   z(1)        z(2)        z(3) */
/*  f(x) =   rho + --------- + ---------- + --------- */
/*                  d(1)-x      d(2)-x      d(3)-x */

/*  It is assumed that */

/*        if ORGATI = .true. the root is between d(2) and d(3); */
/*        otherwise it is between d(1) and d(2) */

/*  This routine will be called by DLAED4 when necessary. In most cases, */
/*  the root sought is the smallest in magnitude, though it might not be */
/*  in some extremely rare situations. */

/*  Arguments */
/*  ========= */

/*  KNITER       (input) INTEGER */
/*               Refer to DLAED4 for its significance. */

/*  ORGATI       (input) LOGICAL */
/*               If ORGATI is true, the needed root is between d(2) and */
/*               d(3); otherwise it is between d(1) and d(2).  See */
/*               DLAED4 for further details. */

/*  RHO          (input) DOUBLE PRECISION */
/*               Refer to the equation f(x) above. */

/*  D            (input) DOUBLE PRECISION array, dimension (3) */
/*               D satisfies d(1) < d(2) < d(3). */

/*  Z            (input) DOUBLE PRECISION array, dimension (3) */
/*               Each of the elements in z must be positive. */

/*  FINIT        (input) DOUBLE PRECISION */
/*               The value of f at 0. It is more accurate than the one */
/*               evaluated inside this routine (if someone wants to do */
/*               so). */

/*  TAU          (output) DOUBLE PRECISION */
/*               The root of the equation f(x). */

/*  INFO         (output) INTEGER */
/*               = 0: successful exit */
/*               > 0: if INFO = 1, failure to converge */

/*  Further Details */
/*  =============== */

/*  30/06/99: Based on contributions by */
/*     Ren-Cang Li, Computer Science Division, University of California */
/*     at Berkeley, USA */

/*  10/02/03: This version has a few statements commented out for thread */
/*  safety (machine parameters are computed on each entry). SJH. */

/*  05/10/06: Modified from a new version of Ren-Cang Li, use */
/*     Gragg-Thornton-Warner cubic convergent scheme for better stability. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --z__;
    --d__;

    /* Function Body */
    *info = 0;

    if (*orgati) {
	lbd = d__[2];
	ubd = d__[3];
    } else {
	lbd = d__[1];
	ubd = d__[2];
    }
    if (*finit < 0.) {
	lbd = 0.;
    } else {
	ubd = 0.;
    }

    niter = 1;
    *tau = 0.;
    if (*kniter == 2) {
	if (*orgati) {
	    temp = (d__[3] - d__[2]) / 2.;
	    c__ = *rho + z__[1] / (d__[1] - d__[2] - temp);
	    a = c__ * (d__[2] + d__[3]) + z__[2] + z__[3];
	    b = c__ * d__[2] * d__[3] + z__[2] * d__[3] + z__[3] * d__[2];
	} else {
	    temp = (d__[1] - d__[2]) / 2.;
	    c__ = *rho + z__[3] / (d__[3] - d__[2] - temp);
	    a = c__ * (d__[1] + d__[2]) + z__[1] + z__[2];
	    b = c__ * d__[1] * d__[2] + z__[1] * d__[2] + z__[2] * d__[1];
	}
/* Computing MAX */
	d__1 = abs(a), d__2 = abs(b), d__1 = std::max(d__1,d__2), d__2 = abs(c__);
	temp = std::max(d__1,d__2);
	a /= temp;
	b /= temp;
	c__ /= temp;
	if (c__ == 0.) {
	    *tau = b / a;
	} else if (a <= 0.) {
	    *tau = (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (
		    c__ * 2.);
	} else {
	    *tau = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__, abs(d__1))
		    ));
	}
	if (*tau < lbd || *tau > ubd) {
	    *tau = (lbd + ubd) / 2.;
	}
	if (d__[1] == *tau || d__[2] == *tau || d__[3] == *tau) {
	    *tau = 0.;
	} else {
	    temp = *finit + *tau * z__[1] / (d__[1] * (d__[1] - *tau)) + *tau
		    * z__[2] / (d__[2] * (d__[2] - *tau)) + *tau * z__[3] / (
		    d__[3] * (d__[3] - *tau));
	    if (temp <= 0.) {
		lbd = *tau;
	    } else {
		ubd = *tau;
	    }
	    if (abs(*finit) <= abs(temp)) {
		*tau = 0.;
	    }
	}
    }

/*     get machine parameters for possible scaling to avoid overflow */

/*     modified by Sven: parameters SMALL1, SMINV1, SMALL2, */
/*     SMINV2, EPS are not SAVEd anymore between one call to the */
/*     others but recomputed at each call */

    eps = dlamch_("Epsilon");
    base = dlamch_("Base");
    i__1 = (integer) (log(dlamch_("SafMin")) / log(base) / 3.);
    small1 = pow_di(&base, &i__1);
    sminv1 = 1. / small1;
    small2 = small1 * small1;
    sminv2 = sminv1 * sminv1;

/*     Determine if scaling of inputs necessary to avoid overflow */
/*     when computing 1/TEMP**3 */

    if (*orgati) {
/* Computing MIN */
	d__3 = (d__1 = d__[2] - *tau, abs(d__1)), d__4 = (d__2 = d__[3] - *
		tau, abs(d__2));
	temp = std::min(d__3,d__4);
    } else {
/* Computing MIN */
	d__3 = (d__1 = d__[1] - *tau, abs(d__1)), d__4 = (d__2 = d__[2] - *
		tau, abs(d__2));
	temp = std::min(d__3,d__4);
    }
    scale = false;
    if (temp <= small1) {
	scale = true;
	if (temp <= small2) {

/*        Scale up by power of radix nearest 1/SAFMIN**(2/3) */

	    sclfac = sminv2;
	    sclinv = small2;
	} else {

/*        Scale up by power of radix nearest 1/SAFMIN**(1/3) */

	    sclfac = sminv1;
	    sclinv = small1;
	}

/*        Scaling up safe because D, Z, TAU scaled elsewhere to be O(1) */

	for (i__ = 1; i__ <= 3; ++i__) {
	    dscale[i__ - 1] = d__[i__] * sclfac;
	    zscale[i__ - 1] = z__[i__] * sclfac;
/* L10: */
	}
	*tau *= sclfac;
	lbd *= sclfac;
	ubd *= sclfac;
    } else {

/*        Copy D and Z to DSCALE and ZSCALE */

	for (i__ = 1; i__ <= 3; ++i__) {
	    dscale[i__ - 1] = d__[i__];
	    zscale[i__ - 1] = z__[i__];
/* L20: */
	}
    }

    fc = 0.;
    df = 0.;
    ddf = 0.;
    for (i__ = 1; i__ <= 3; ++i__) {
	temp = 1. / (dscale[i__ - 1] - *tau);
	temp1 = zscale[i__ - 1] * temp;
	temp2 = temp1 * temp;
	temp3 = temp2 * temp;
	fc += temp1 / dscale[i__ - 1];
	df += temp2;
	ddf += temp3;
/* L30: */
    }
    f = *finit + *tau * fc;

    if (abs(f) <= 0.) {
	goto L60;
    }
    if (f <= 0.) {
	lbd = *tau;
    } else {
	ubd = *tau;
    }

/*        Iteration begins -- Use Gragg-Thornton-Warner cubic convergent */
/*                            scheme */

/*     It is not hard to see that */

/*           1) Iterations will go up monotonically */
/*              if FINIT < 0; */

/*           2) Iterations will go down monotonically */
/*              if FINIT > 0. */

    iter = niter + 1;

    for (niter = iter; niter <= 40; ++niter) {

	if (*orgati) {
	    temp1 = dscale[1] - *tau;
	    temp2 = dscale[2] - *tau;
	} else {
	    temp1 = dscale[0] - *tau;
	    temp2 = dscale[1] - *tau;
	}
	a = (temp1 + temp2) * f - temp1 * temp2 * df;
	b = temp1 * temp2 * f;
	c__ = f - (temp1 + temp2) * df + temp1 * temp2 * ddf;
/* Computing MAX */
	d__1 = abs(a), d__2 = abs(b), d__1 = std::max(d__1,d__2), d__2 = abs(c__);
	temp = std::max(d__1,d__2);
	a /= temp;
	b /= temp;
	c__ /= temp;
	if (c__ == 0.) {
	    eta = b / a;
	} else if (a <= 0.) {
	    eta = (a - sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))) / (c__
		    * 2.);
	} else {
	    eta = b * 2. / (a + sqrt((d__1 = a * a - b * 4. * c__, abs(d__1)))
		    );
	}
	if (f * eta >= 0.) {
	    eta = -f / df;
	}

	*tau += eta;
	if (*tau < lbd || *tau > ubd) {
	    *tau = (lbd + ubd) / 2.;
	}

	fc = 0.;
	erretm = 0.;
	df = 0.;
	ddf = 0.;
	for (i__ = 1; i__ <= 3; ++i__) {
	    temp = 1. / (dscale[i__ - 1] - *tau);
	    temp1 = zscale[i__ - 1] * temp;
	    temp2 = temp1 * temp;
	    temp3 = temp2 * temp;
	    temp4 = temp1 / dscale[i__ - 1];
	    fc += temp4;
	    erretm += abs(temp4);
	    df += temp2;
	    ddf += temp3;
/* L40: */
	}
	f = *finit + *tau * fc;
	erretm = (abs(*finit) + abs(*tau) * erretm) * 8. + abs(*tau) * df;
	if (abs(f) <= eps * erretm) {
	    goto L60;
	}
	if (f <= 0.) {
	    lbd = *tau;
	} else {
	    ubd = *tau;
	}
/* L50: */
    }
    *info = 1;
L60:

/*     Undo scaling */

    if (scale) {
	*tau *= sclinv;
    }
    return 0;

/*     End of DLAED6 */

} /* dlaed6_ */

/* Subroutine */ int dlaed7_(integer *icompq, integer *n, integer *qsiz,
	integer *tlvls, integer *curlvl, integer *curpbm, double *d__,
	double *q, integer *ldq, integer *indxq, double *rho, integer
	*cutpnt, double *qstore, integer *qptr, integer *prmptr, integer *
	perm, integer *givptr, integer *givcol, double *givnum,
	double *work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static integer c__2 = 2;
	static integer c__1 = 1;
	static double c_b10 = 1.;
	static double c_b11 = 0.;
	static integer c_n1 = -1;

    /* System generated locals */
    integer q_dim1, q_offset, i__1, i__2;

    /* Local variables */
    integer i__, k, n1, n2, is, iw, iz, iq2, ptr, ldq2, indx, curr;
    integer indxc, indxp;
    integer idlmda;
    integer coltyp;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAED7 computes the updated eigensystem of a diagonal */
/*  matrix after modification by a rank-one symmetric matrix. This */
/*  routine is used only for the eigenproblem which requires all */
/*  eigenvalues and optionally eigenvectors of a dense symmetric matrix */
/*  that has been reduced to tridiagonal form.  DLAED1 handles */
/*  the case in which all eigenvalues and eigenvectors of a symmetric */
/*  tridiagonal matrix are desired. */

/*    T = Q(in) ( D(in) + RHO * Z*Z' ) Q'(in) = Q(out) * D(out) * Q'(out) */

/*     where Z = Q'u, u is a vector of length N with ones in the */
/*     CUTPNT and CUTPNT + 1 th elements and zeros elsewhere. */

/*     The eigenvectors of the original matrix are stored in Q, and the */
/*     eigenvalues are in D.  The algorithm consists of three stages: */

/*        The first stage consists of deflating the size of the problem */
/*        when there are multiple eigenvalues or if there is a zero in */
/*        the Z vector.  For each such occurrence the dimension of the */
/*        secular equation problem is reduced by one.  This stage is */
/*        performed by the routine DLAED8. */

/*        The second stage consists of calculating the updated */
/*        eigenvalues. This is done by finding the roots of the secular */
/*        equation via the routine DLAED4 (as called by DLAED9). */
/*        This routine also calculates the eigenvectors of the current */
/*        problem. */

/*        The final stage consists of computing the updated eigenvectors */
/*        directly using the updated eigenvalues.  The eigenvectors for */
/*        the current problem are multiplied with the eigenvectors from */
/*        the overall problem. */

/*  Arguments */
/*  ========= */

/*  ICOMPQ  (input) INTEGER */
/*          = 0:  Compute eigenvalues only. */
/*          = 1:  Compute eigenvectors of original dense symmetric matrix */
/*                also.  On entry, Q contains the orthogonal matrix used */
/*                to reduce the original matrix to tridiagonal form. */

/*  N      (input) INTEGER */
/*         The dimension of the symmetric tridiagonal matrix.  N >= 0. */

/*  QSIZ   (input) INTEGER */
/*         The dimension of the orthogonal matrix used to reduce */
/*         the full matrix to tridiagonal form.  QSIZ >= N if ICOMPQ = 1. */

/*  TLVLS  (input) INTEGER */
/*         The total number of merging levels in the overall divide and */
/*         conquer tree. */

/*  CURLVL (input) INTEGER */
/*         The current level in the overall merge routine, */
/*         0 <= CURLVL <= TLVLS. */

/*  CURPBM (input) INTEGER */
/*         The current problem in the current level in the overall */
/*         merge routine (counting from upper left to lower right). */

/*  D      (input/output) DOUBLE PRECISION array, dimension (N) */
/*         On entry, the eigenvalues of the rank-1-perturbed matrix. */
/*         On exit, the eigenvalues of the repaired matrix. */

/*  Q      (input/output) DOUBLE PRECISION array, dimension (LDQ, N) */
/*         On entry, the eigenvectors of the rank-1-perturbed matrix. */
/*         On exit, the eigenvectors of the repaired tridiagonal matrix. */

/*  LDQ    (input) INTEGER */
/*         The leading dimension of the array Q.  LDQ >= max(1,N). */

/*  INDXQ  (output) INTEGER array, dimension (N) */
/*         The permutation which will reintegrate the subproblem just */
/*         solved back into sorted order, i.e., D( INDXQ( I = 1, N ) ) */
/*         will be in ascending order. */

/*  RHO    (input) DOUBLE PRECISION */
/*         The subdiagonal element used to create the rank-1 */
/*         modification. */

/*  CUTPNT (input) INTEGER */
/*         Contains the location of the last eigenvalue in the leading */
/*         sub-matrix.  min(1,N) <= CUTPNT <= N. */

/*  QSTORE (input/output) DOUBLE PRECISION array, dimension (N**2+1) */
/*         Stores eigenvectors of submatrices encountered during */
/*         divide and conquer, packed together. QPTR points to */
/*         beginning of the submatrices. */

/*  QPTR   (input/output) INTEGER array, dimension (N+2) */
/*         List of indices pointing to beginning of submatrices stored */
/*         in QSTORE. The submatrices are numbered starting at the */
/*         bottom left of the divide and conquer tree, from left to */
/*         right and bottom to top. */

/*  PRMPTR (input) INTEGER array, dimension (N lg N) */
/*         Contains a list of pointers which indicate where in PERM a */
/*         level's permutation is stored.  PRMPTR(i+1) - PRMPTR(i) */
/*         indicates the size of the permutation and also the size of */
/*         the full, non-deflated problem. */

/*  PERM   (input) INTEGER array, dimension (N lg N) */
/*         Contains the permutations (from deflation and sorting) to be */
/*         applied to each eigenblock. */

/*  GIVPTR (input) INTEGER array, dimension (N lg N) */
/*         Contains a list of pointers which indicate where in GIVCOL a */
/*         level's Givens rotations are stored.  GIVPTR(i+1) - GIVPTR(i) */
/*         indicates the number of Givens rotations. */

/*  GIVCOL (input) INTEGER array, dimension (2, N lg N) */
/*         Each pair of numbers indicates a pair of columns to take place */
/*         in a Givens rotation. */

/*  GIVNUM (input) DOUBLE PRECISION array, dimension (2, N lg N) */
/*         Each number indicates the S value to be used in the */
/*         corresponding Givens rotation. */

/*  WORK   (workspace) DOUBLE PRECISION array, dimension (3*N+QSIZ*N) */

/*  IWORK  (workspace) INTEGER array, dimension (4*N) */

/*  INFO   (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = 1, an eigenvalue did not converge */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Jeff Rutter, Computer Science Division, University of California */
/*     at Berkeley, USA */

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
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --indxq;
    --qstore;
    --qptr;
    --prmptr;
    --perm;
    --givptr;
    givcol -= 3;
    givnum -= 3;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    } else if (*icompq == 1 && *qsiz < *n) {
	*info = -4;
    } else if (*ldq < std::max(1_integer,*n)) {
	*info = -9;
    } else if (std::min(1_integer,*n) > *cutpnt || *n < *cutpnt) {
	*info = -12;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAED7", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     The following values are for bookkeeping purposes only.  They are */
/*     integer pointers which indicate the portion of the workspace */
/*     used by a particular array in DLAED8 and DLAED9. */

    if (*icompq == 1) {
	ldq2 = *qsiz;
    } else {
	ldq2 = *n;
    }

    iz = 1;
    idlmda = iz + *n;
    iw = idlmda + *n;
    iq2 = iw + *n;
    is = iq2 + *n * ldq2;

    indx = 1;
    indxc = indx + *n;
    coltyp = indxc + *n;
    indxp = coltyp + *n;

/*     Form the z-vector which consists of the last row of Q_1 and the */
/*     first row of Q_2. */

    ptr = pow_ii(&c__2, tlvls) + 1;
    i__1 = *curlvl - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *tlvls - i__;
	ptr += pow_ii(&c__2, &i__2);
/* L10: */
    }
    curr = ptr + *curpbm;
    dlaeda_(n, tlvls, curlvl, curpbm, &prmptr[1], &perm[1], &givptr[1], &
	    givcol[3], &givnum[3], &qstore[1], &qptr[1], &work[iz], &work[iz
	    + *n], info);

/*     When solving the final problem, we no longer need the stored data, */
/*     so we will overwrite the data from this level onto the previously */
/*     used storage space. */

    if (*curlvl == *tlvls) {
	qptr[curr] = 1;
	prmptr[curr] = 1;
	givptr[curr] = 1;
    }

/*     Sort and Deflate eigenvalues. */

    dlaed8_(icompq, &k, n, qsiz, &d__[1], &q[q_offset], ldq, &indxq[1], rho,
	    cutpnt, &work[iz], &work[idlmda], &work[iq2], &ldq2, &work[iw], &
	    perm[prmptr[curr]], &givptr[curr + 1], &givcol[(givptr[curr] << 1)
	     + 1], &givnum[(givptr[curr] << 1) + 1], &iwork[indxp], &iwork[
	    indx], info);
    prmptr[curr + 1] = prmptr[curr] + *n;
    givptr[curr + 1] += givptr[curr];

/*     Solve Secular Equation. */

    if (k != 0) {
	dlaed9_(&k, &c__1, &k, n, &d__[1], &work[is], &k, rho, &work[idlmda],
		&work[iw], &qstore[qptr[curr]], &k, info);
	if (*info != 0) {
	    goto L30;
	}
	if (*icompq == 1) {
	    dgemm_("N", "N", qsiz, &k, &k, &c_b10, &work[iq2], &ldq2, &qstore[
		    qptr[curr]], &k, &c_b11, &q[q_offset], ldq);
	}
/* Computing 2nd power */
	i__1 = k;
	qptr[curr + 1] = qptr[curr] + i__1 * i__1;

/*     Prepare the INDXQ sorting permutation. */

	n1 = k;
	n2 = *n - k;
	dlamrg_(&n1, &n2, &d__[1], &c__1, &c_n1, &indxq[1]);
    } else {
	qptr[curr + 1] = qptr[curr];
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    indxq[i__] = i__;
/* L20: */
	}
    }

L30:
    return 0;

/*     End of DLAED7 */

} /* dlaed7_ */

/* Subroutine */ int dlaed8_(integer *icompq, integer *k, integer *n, integer
	*qsiz, double *d__, double *q, integer *ldq, integer *indxq,
	double *rho, integer *cutpnt, double *z__, double *dlamda,
	double *q2, integer *ldq2, double *w, integer *perm, integer
	*givptr, integer *givcol, double *givnum, integer *indxp, integer
	*indx, integer *info)
{
	/* Table of constant values */
	static double c_b3 = -1.;
	static integer c__1 = 1;

    /* System generated locals */
    integer q_dim1, q_offset, q2_dim1, q2_offset, i__1;
    double d__1;

    /* Local variables */
    double c__;
    integer i__, j;
    double s, t;
    integer k2, n1, n2, jp, n1p1;
    double eps, tau, tol;
    integer jlam, imax, jmax;

/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAED8 merges the two sets of eigenvalues together into a single */
/*  sorted set.  Then it tries to deflate the size of the problem. */
/*  There are two ways in which deflation can occur:  when two or more */
/*  eigenvalues are close together or if there is a tiny element in the */
/*  Z vector.  For each such occurrence the order of the related secular */
/*  equation problem is reduced by one. */

/*  Arguments */
/*  ========= */

/*  ICOMPQ  (input) INTEGER */
/*          = 0:  Compute eigenvalues only. */
/*          = 1:  Compute eigenvectors of original dense symmetric matrix */
/*                also.  On entry, Q contains the orthogonal matrix used */
/*                to reduce the original matrix to tridiagonal form. */

/*  K      (output) INTEGER */
/*         The number of non-deflated eigenvalues, and the order of the */
/*         related secular equation. */

/*  N      (input) INTEGER */
/*         The dimension of the symmetric tridiagonal matrix.  N >= 0. */

/*  QSIZ   (input) INTEGER */
/*         The dimension of the orthogonal matrix used to reduce */
/*         the full matrix to tridiagonal form.  QSIZ >= N if ICOMPQ = 1. */

/*  D      (input/output) DOUBLE PRECISION array, dimension (N) */
/*         On entry, the eigenvalues of the two submatrices to be */
/*         combined.  On exit, the trailing (N-K) updated eigenvalues */
/*         (those which were deflated) sorted into increasing order. */

/*  Q      (input/output) DOUBLE PRECISION array, dimension (LDQ,N) */
/*         If ICOMPQ = 0, Q is not referenced.  Otherwise, */
/*         on entry, Q contains the eigenvectors of the partially solved */
/*         system which has been previously updated in matrix */
/*         multiplies with other partially solved eigensystems. */
/*         On exit, Q contains the trailing (N-K) updated eigenvectors */
/*         (those which were deflated) in its last N-K columns. */

/*  LDQ    (input) INTEGER */
/*         The leading dimension of the array Q.  LDQ >= max(1,N). */

/*  INDXQ  (input) INTEGER array, dimension (N) */
/*         The permutation which separately sorts the two sub-problems */
/*         in D into ascending order.  Note that elements in the second */
/*         half of this permutation must first have CUTPNT added to */
/*         their values in order to be accurate. */

/*  RHO    (input/output) DOUBLE PRECISION */
/*         On entry, the off-diagonal element associated with the rank-1 */
/*         cut which originally split the two submatrices which are now */
/*         being recombined. */
/*         On exit, RHO has been modified to the value required by */
/*         DLAED3. */

/*  CUTPNT (input) INTEGER */
/*         The location of the last eigenvalue in the leading */
/*         sub-matrix.  min(1,N) <= CUTPNT <= N. */

/*  Z      (input) DOUBLE PRECISION array, dimension (N) */
/*         On entry, Z contains the updating vector (the last row of */
/*         the first sub-eigenvector matrix and the first row of the */
/*         second sub-eigenvector matrix). */
/*         On exit, the contents of Z are destroyed by the updating */
/*         process. */

/*  DLAMDA (output) DOUBLE PRECISION array, dimension (N) */
/*         A copy of the first K eigenvalues which will be used by */
/*         DLAED3 to form the secular equation. */

/*  Q2     (output) DOUBLE PRECISION array, dimension (LDQ2,N) */
/*         If ICOMPQ = 0, Q2 is not referenced.  Otherwise, */
/*         a copy of the first K eigenvectors which will be used by */
/*         DLAED7 in a matrix multiply (DGEMM) to update the new */
/*         eigenvectors. */

/*  LDQ2   (input) INTEGER */
/*         The leading dimension of the array Q2.  LDQ2 >= max(1,N). */

/*  W      (output) DOUBLE PRECISION array, dimension (N) */
/*         The first k values of the final deflation-altered z-vector and */
/*         will be passed to DLAED3. */

/*  PERM   (output) INTEGER array, dimension (N) */
/*         The permutations (from deflation and sorting) to be applied */
/*         to each eigenblock. */

/*  GIVPTR (output) INTEGER */
/*         The number of Givens rotations which took place in this */
/*         subproblem. */

/*  GIVCOL (output) INTEGER array, dimension (2, N) */
/*         Each pair of numbers indicates a pair of columns to take place */
/*         in a Givens rotation. */

/*  GIVNUM (output) DOUBLE PRECISION array, dimension (2, N) */
/*         Each number indicates the S value to be used in the */
/*         corresponding Givens rotation. */

/*  INDXP  (workspace) INTEGER array, dimension (N) */
/*         The permutation used to place deflated values of D at the end */
/*         of the array.  INDXP(1:K) points to the nondeflated D-values */
/*         and INDXP(K+1:N) points to the deflated eigenvalues. */

/*  INDX   (workspace) INTEGER array, dimension (N) */
/*         The permutation used to sort the contents of D into ascending */
/*         order. */

/*  INFO   (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Jeff Rutter, Computer Science Division, University of California */
/*     at Berkeley, USA */

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
    --indxq;
    --z__;
    --dlamda;
    q2_dim1 = *ldq2;
    q2_offset = 1 + q2_dim1;
    q2 -= q2_offset;
    --w;
    --perm;
    givcol -= 3;
    givnum -= 3;
    --indxp;
    --indx;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*n < 0) {
	*info = -3;
    } else if (*icompq == 1 && *qsiz < *n) {
	*info = -4;
    } else if (*ldq < std::max(1_integer,*n)) {
	*info = -7;
    } else if (*cutpnt < std::min(1_integer,*n) || *cutpnt > *n) {
	*info = -10;
    } else if (*ldq2 < std::max(1_integer,*n)) {
	*info = -14;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAED8", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

    n1 = *cutpnt;
    n2 = *n - n1;
    n1p1 = n1 + 1;

    if (*rho < 0.) {
	dscal_(&n2, &c_b3, &z__[n1p1], &c__1);
    }

/*     Normalize z so that norm(z) = 1 */

    t = 1. / sqrt(2.);
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	indx[j] = j;
/* L10: */
    }
    dscal_(n, &t, &z__[1], &c__1);
    *rho = (d__1 = *rho * 2., abs(d__1));

/*     Sort the eigenvalues into increasing order */

    i__1 = *n;
    for (i__ = *cutpnt + 1; i__ <= i__1; ++i__) {
	indxq[i__] += *cutpnt;
/* L20: */
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dlamda[i__] = d__[indxq[i__]];
	w[i__] = z__[indxq[i__]];
/* L30: */
    }
    i__ = 1;
    j = *cutpnt + 1;
    dlamrg_(&n1, &n2, &dlamda[1], &c__1, &c__1, &indx[1]);
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__[i__] = dlamda[indx[i__]];
	z__[i__] = w[indx[i__]];
/* L40: */
    }

/*     Calculate the allowable deflation tolerence */

    imax = idamax_(n, &z__[1], &c__1);
    jmax = idamax_(n, &d__[1], &c__1);
    eps = dlamch_("Epsilon");
    tol = eps * 8. * (d__1 = d__[jmax], abs(d__1));

/*     If the rank-1 modifier is small enough, no more needs to be done */
/*     except to reorganize Q so that its columns correspond with the */
/*     elements in D. */

    if (*rho * (d__1 = z__[imax], abs(d__1)) <= tol) {
	*k = 0;
	if (*icompq == 0) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		perm[j] = indxq[indx[j]];
/* L50: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		perm[j] = indxq[indx[j]];
		dcopy_(qsiz, &q[perm[j] * q_dim1 + 1], &c__1, &q2[j * q2_dim1
			+ 1], &c__1);
/* L60: */
	    }
	    dlacpy_("A", qsiz, n, &q2[q2_dim1 + 1], ldq2, &q[q_dim1 + 1], ldq);
	}
	return 0;
    }

/*     If there are multiple eigenvalues then the problem deflates.  Here */
/*     the number of equal eigenvalues are found.  As each equal */
/*     eigenvalue is found, an elementary reflector is computed to rotate */
/*     the corresponding eigensubspace so that the corresponding */
/*     components of Z are zero in this new basis. */

    *k = 0;
    *givptr = 0;
    k2 = *n + 1;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	if (*rho * (d__1 = z__[j], abs(d__1)) <= tol) {

/*           Deflate due to small z component. */

	    --k2;
	    indxp[k2] = j;
	    if (j == *n) {
		goto L110;
	    }
	} else {
	    jlam = j;
	    goto L80;
	}
/* L70: */
    }
L80:
    ++j;
    if (j > *n) {
	goto L100;
    }
    if (*rho * (d__1 = z__[j], abs(d__1)) <= tol) {

/*        Deflate due to small z component. */

	--k2;
	indxp[k2] = j;
    } else {

/*        Check if eigenvalues are close enough to allow deflation. */

	s = z__[jlam];
	c__ = z__[j];

/*        Find sqrt(a**2+b**2) without overflow or */
/*        destructive underflow. */

	tau = dlapy2_(&c__, &s);
	t = d__[j] - d__[jlam];
	c__ /= tau;
	s = -s / tau;
	if ((d__1 = t * c__ * s, abs(d__1)) <= tol) {

/*           Deflation is possible. */

	    z__[j] = tau;
	    z__[jlam] = 0.;

/*           Record the appropriate Givens rotation */

	    ++(*givptr);
	    givcol[(*givptr << 1) + 1] = indxq[indx[jlam]];
	    givcol[(*givptr << 1) + 2] = indxq[indx[j]];
	    givnum[(*givptr << 1) + 1] = c__;
	    givnum[(*givptr << 1) + 2] = s;
	    if (*icompq == 1) {
		drot_(qsiz, &q[indxq[indx[jlam]] * q_dim1 + 1], &c__1, &q[
			indxq[indx[j]] * q_dim1 + 1], &c__1, &c__, &s);
	    }
	    t = d__[jlam] * c__ * c__ + d__[j] * s * s;
	    d__[j] = d__[jlam] * s * s + d__[j] * c__ * c__;
	    d__[jlam] = t;
	    --k2;
	    i__ = 1;
L90:
	    if (k2 + i__ <= *n) {
		if (d__[jlam] < d__[indxp[k2 + i__]]) {
		    indxp[k2 + i__ - 1] = indxp[k2 + i__];
		    indxp[k2 + i__] = jlam;
		    ++i__;
		    goto L90;
		} else {
		    indxp[k2 + i__ - 1] = jlam;
		}
	    } else {
		indxp[k2 + i__ - 1] = jlam;
	    }
	    jlam = j;
	} else {
	    ++(*k);
	    w[*k] = z__[jlam];
	    dlamda[*k] = d__[jlam];
	    indxp[*k] = jlam;
	    jlam = j;
	}
    }
    goto L80;
L100:

/*     Record the last eigenvalue. */

    ++(*k);
    w[*k] = z__[jlam];
    dlamda[*k] = d__[jlam];
    indxp[*k] = jlam;

L110:

/*     Sort the eigenvalues and corresponding eigenvectors into DLAMDA */
/*     and Q2 respectively.  The eigenvalues/vectors which were not */
/*     deflated go into the first K slots of DLAMDA and Q2 respectively, */
/*     while those which were deflated go into the last N - K slots. */

    if (*icompq == 0) {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    jp = indxp[j];
	    dlamda[j] = d__[jp];
	    perm[j] = indxq[indx[jp]];
/* L120: */
	}
    } else {
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    jp = indxp[j];
	    dlamda[j] = d__[jp];
	    perm[j] = indxq[indx[jp]];
	    dcopy_(qsiz, &q[perm[j] * q_dim1 + 1], &c__1, &q2[j * q2_dim1 + 1]
, &c__1);
/* L130: */
	}
    }

/*     The deflated eigenvalues and their corresponding vectors go back */
/*     into the last N - K slots of D and Q respectively. */

    if (*k < *n) {
	if (*icompq == 0) {
	    i__1 = *n - *k;
	    dcopy_(&i__1, &dlamda[*k + 1], &c__1, &d__[*k + 1], &c__1);
	} else {
	    i__1 = *n - *k;
	    dcopy_(&i__1, &dlamda[*k + 1], &c__1, &d__[*k + 1], &c__1);
	    i__1 = *n - *k;
	    dlacpy_("A", qsiz, &i__1, &q2[(*k + 1) * q2_dim1 + 1], ldq2, &q[(*
		    k + 1) * q_dim1 + 1], ldq);
	}
    }

    return 0;

/*     End of DLAED8 */

} /* dlaed8_ */

/* Subroutine */ int dlaed9_(integer *k, integer *kstart, integer *kstop,
	integer *n, double *d__, double *q, integer *ldq, double *
	rho, double *dlamda, double *w, double *s, integer *lds,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer q_dim1, q_offset, s_dim1, s_offset, i__1, i__2;
    double d__1;

    /* Local variables */
    integer i__, j;
    double temp;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAED9 finds the roots of the secular equation, as defined by the */
/*  values in D, Z, and RHO, between KSTART and KSTOP.  It makes the */
/*  appropriate calls to DLAED4 and then stores the new matrix of */
/*  eigenvectors for use in calculating the next level of Z vectors. */

/*  Arguments */
/*  ========= */

/*  K       (input) INTEGER */
/*          The number of terms in the rational function to be solved by */
/*          DLAED4.  K >= 0. */

/*  KSTART  (input) INTEGER */
/*  KSTOP   (input) INTEGER */
/*          The updated eigenvalues Lambda(I), KSTART <= I <= KSTOP */
/*          are to be computed.  1 <= KSTART <= KSTOP <= K. */

/*  N       (input) INTEGER */
/*          The number of rows and columns in the Q matrix. */
/*          N >= K (delation may result in N > K). */

/*  D       (output) DOUBLE PRECISION array, dimension (N) */
/*          D(I) contains the updated eigenvalues */
/*          for KSTART <= I <= KSTOP. */

/*  Q       (workspace) DOUBLE PRECISION array, dimension (LDQ,N) */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q.  LDQ >= max( 1, N ). */

/*  RHO     (input) DOUBLE PRECISION */
/*          The value of the parameter in the rank one update equation. */
/*          RHO >= 0 required. */

/*  DLAMDA  (input) DOUBLE PRECISION array, dimension (K) */
/*          The first K elements of this array contain the old roots */
/*          of the deflated updating problem.  These are the poles */
/*          of the secular equation. */

/*  W       (input) DOUBLE PRECISION array, dimension (K) */
/*          The first K elements of this array contain the components */
/*          of the deflation-adjusted updating vector. */

/*  S       (output) DOUBLE PRECISION array, dimension (LDS, K) */
/*          Will contain the eigenvectors of the repaired matrix which */
/*          will be stored for subsequent Z vector calculation and */
/*          multiplied by the previously accumulated eigenvectors */
/*          to update the system. */

/*  LDS     (input) INTEGER */
/*          The leading dimension of S.  LDS >= max( 1, K ). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = 1, an eigenvalue did not converge */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Jeff Rutter, Computer Science Division, University of California */
/*     at Berkeley, USA */

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
    --d__;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --dlamda;
    --w;
    s_dim1 = *lds;
    s_offset = 1 + s_dim1;
    s -= s_offset;

    /* Function Body */
    *info = 0;

    if (*k < 0) {
	*info = -1;
    } else if (*kstart < 1 || *kstart > std::max(1_integer,*k)) {
	*info = -2;
    } else if (std::max(1_integer,*kstop) < *kstart || *kstop > std::max(1_integer,*k)) {
	*info = -3;
    } else if (*n < *k) {
	*info = -4;
    } else if (*ldq < std::max(1_integer,*k)) {
	*info = -7;
    } else if (*lds < std::max(1_integer,*k)) {
	*info = -12;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAED9", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*k == 0) {
	return 0;
    }

/*     Modify values DLAMDA(i) to make sure all DLAMDA(i)-DLAMDA(j) can */
/*     be computed with high relative accuracy (barring over/underflow). */
/*     This is a problem on machines without a guard digit in */
/*     add/subtract (Cray XMP, Cray YMP, Cray C 90 and Cray 2). */
/*     The following code replaces DLAMDA(I) by 2*DLAMDA(I)-DLAMDA(I), */
/*     which on any of these machines zeros out the bottommost */
/*     bit of DLAMDA(I) if it is 1; this makes the subsequent */
/*     subtractions DLAMDA(I)-DLAMDA(J) unproblematic when cancellation */
/*     occurs. On binary machines with a guard digit (almost all */
/*     machines) it does not change DLAMDA(I) at all. On hexadecimal */
/*     and decimal machines with a guard digit, it slightly */
/*     changes the bottommost bits of DLAMDA(I). It does not account */
/*     for hexadecimal or decimal machines without guard digits */
/*     (we know of none). We use a subroutine call to compute */
/*     2*DLAMBDA(I) to prevent optimizing compilers from eliminating */
/*     this code. */

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dlamda[i__] = dlamc3_(&dlamda[i__], &dlamda[i__]) - dlamda[i__];
/* L10: */
    }

    i__1 = *kstop;
    for (j = *kstart; j <= i__1; ++j) {
	dlaed4_(k, &j, &dlamda[1], &w[1], &q[j * q_dim1 + 1], rho, &d__[j],
		info);

/*        If the zero finder fails, the computation is terminated. */

	if (*info != 0) {
	    goto L120;
	}
/* L20: */
    }

    if (*k == 1 || *k == 2) {
	i__1 = *k;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = *k;
	    for (j = 1; j <= i__2; ++j) {
		s[j + i__ * s_dim1] = q[j + i__ * q_dim1];
/* L30: */
	    }
/* L40: */
	}
	goto L120;
    }

/*     Compute updated W. */

    dcopy_(k, &w[1], &c__1, &s[s_offset], &c__1);

/*     Initialize W(I) = Q(I,I) */

    i__1 = *ldq + 1;
    dcopy_(k, &q[q_offset], &i__1, &w[1], &c__1);
    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    w[i__] *= q[i__ + j * q_dim1] / (dlamda[i__] - dlamda[j]);
/* L50: */
	}
	i__2 = *k;
	for (i__ = j + 1; i__ <= i__2; ++i__) {
	    w[i__] *= q[i__ + j * q_dim1] / (dlamda[i__] - dlamda[j]);
/* L60: */
	}
/* L70: */
    }
    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__1 = sqrt(-w[i__]);
	w[i__] = d_sign(&d__1, &s[i__ + s_dim1]);
/* L80: */
    }

/*     Compute eigenvectors of the modified rank-1 modification. */

    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *k;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    q[i__ + j * q_dim1] = w[i__] / q[i__ + j * q_dim1];
/* L90: */
	}
	temp = dnrm2_(k, &q[j * q_dim1 + 1], &c__1);
	i__2 = *k;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    s[i__ + j * s_dim1] = q[i__ + j * q_dim1] / temp;
/* L100: */
	}
/* L110: */
    }

L120:
    return 0;

/*     End of DLAED9 */

} /* dlaed9_ */

/* Subroutine */ int dlaeda_(integer *n, integer *tlvls, integer *curlvl,
	integer *curpbm, integer *prmptr, integer *perm, integer *givptr,
	integer *givcol, double *givnum, double *q, integer *qptr,
	double *z__, double *ztemp, integer *info)
{
	/* Table of constant values */
	static integer c__2 = 2;
	static integer c__1 = 1;
	static double c_b24 = 1.;
	static double c_b26 = 0.;

    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    integer i__, k, mid, ptr;
    integer curr, bsiz1, bsiz2, psiz1, psiz2, zptr1;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAEDA computes the Z vector corresponding to the merge step in the */
/*  CURLVLth step of the merge process with TLVLS steps for the CURPBMth */
/*  problem. */

/*  Arguments */
/*  ========= */

/*  N      (input) INTEGER */
/*         The dimension of the symmetric tridiagonal matrix.  N >= 0. */

/*  TLVLS  (input) INTEGER */
/*         The total number of merging levels in the overall divide and */
/*         conquer tree. */

/*  CURLVL (input) INTEGER */
/*         The current level in the overall merge routine, */
/*         0 <= curlvl <= tlvls. */

/*  CURPBM (input) INTEGER */
/*         The current problem in the current level in the overall */
/*         merge routine (counting from upper left to lower right). */

/*  PRMPTR (input) INTEGER array, dimension (N lg N) */
/*         Contains a list of pointers which indicate where in PERM a */
/*         level's permutation is stored.  PRMPTR(i+1) - PRMPTR(i) */
/*         indicates the size of the permutation and incidentally the */
/*         size of the full, non-deflated problem. */

/*  PERM   (input) INTEGER array, dimension (N lg N) */
/*         Contains the permutations (from deflation and sorting) to be */
/*         applied to each eigenblock. */

/*  GIVPTR (input) INTEGER array, dimension (N lg N) */
/*         Contains a list of pointers which indicate where in GIVCOL a */
/*         level's Givens rotations are stored.  GIVPTR(i+1) - GIVPTR(i) */
/*         indicates the number of Givens rotations. */

/*  GIVCOL (input) INTEGER array, dimension (2, N lg N) */
/*         Each pair of numbers indicates a pair of columns to take place */
/*         in a Givens rotation. */

/*  GIVNUM (input) DOUBLE PRECISION array, dimension (2, N lg N) */
/*         Each number indicates the S value to be used in the */
/*         corresponding Givens rotation. */

/*  Q      (input) DOUBLE PRECISION array, dimension (N**2) */
/*         Contains the square eigenblocks from previous levels, the */
/*         starting positions for blocks are given by QPTR. */

/*  QPTR   (input) INTEGER array, dimension (N+2) */
/*         Contains a list of pointers which indicate where in Q an */
/*         eigenblock is stored.  SQRT( QPTR(i+1) - QPTR(i) ) indicates */
/*         the size of the block. */

/*  Z      (output) DOUBLE PRECISION array, dimension (N) */
/*         On output this vector contains the updating vector (the last */
/*         row of the first sub-eigenvector matrix and the first row of */
/*         the second sub-eigenvector matrix). */

/*  ZTEMP  (workspace) DOUBLE PRECISION array, dimension (N) */

/*  INFO   (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Jeff Rutter, Computer Science Division, University of California */
/*     at Berkeley, USA */

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
    --ztemp;
    --z__;
    --qptr;
    --q;
    givnum -= 3;
    givcol -= 3;
    --givptr;
    --perm;
    --prmptr;

    /* Function Body */
    *info = 0;

    if (*n < 0) {
	*info = -1;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAEDA", &i__1);
	return 0;
    }

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Determine location of first number in second half. */

    mid = *n / 2 + 1;

/*     Gather last/first rows of appropriate eigenblocks into center of Z */

    ptr = 1;

/*     Determine location of lowest level subproblem in the full storage */
/*     scheme */

    i__1 = *curlvl - 1;
    curr = ptr + *curpbm * pow_ii(&c__2, curlvl) + pow_ii(&c__2, &i__1) - 1;

/*     Determine size of these matrices.  We add HALF to the value of */
/*     the SQRT in case the machine underestimates one of these square */
/*     roots. */

    bsiz1 = (integer) (sqrt((double) (qptr[curr + 1] - qptr[curr])) + .5);
    bsiz2 = (integer) (sqrt((double) (qptr[curr + 2] - qptr[curr + 1])) +
	    .5);
    i__1 = mid - bsiz1 - 1;
    for (k = 1; k <= i__1; ++k) {
	z__[k] = 0.;
/* L10: */
    }
    dcopy_(&bsiz1, &q[qptr[curr] + bsiz1 - 1], &bsiz1, &z__[mid - bsiz1], &
	    c__1);
    dcopy_(&bsiz2, &q[qptr[curr + 1]], &bsiz2, &z__[mid], &c__1);
    i__1 = *n;
    for (k = mid + bsiz2; k <= i__1; ++k) {
	z__[k] = 0.;
/* L20: */
    }

/*     Loop thru remaining levels 1 -> CURLVL applying the Givens */
/*     rotations and permutation and then multiplying the center matrices */
/*     against the current Z. */

    ptr = pow_ii(&c__2, tlvls) + 1;
    i__1 = *curlvl - 1;
    for (k = 1; k <= i__1; ++k) {
	i__2 = *curlvl - k;
	i__3 = *curlvl - k - 1;
	curr = ptr + *curpbm * pow_ii(&c__2, &i__2) + pow_ii(&c__2, &i__3) -
		1;
	psiz1 = prmptr[curr + 1] - prmptr[curr];
	psiz2 = prmptr[curr + 2] - prmptr[curr + 1];
	zptr1 = mid - psiz1;

/*       Apply Givens at CURR and CURR+1 */

	i__2 = givptr[curr + 1] - 1;
	for (i__ = givptr[curr]; i__ <= i__2; ++i__) {
	    drot_(&c__1, &z__[zptr1 + givcol[(i__ << 1) + 1] - 1], &c__1, &
		    z__[zptr1 + givcol[(i__ << 1) + 2] - 1], &c__1, &givnum[(
		    i__ << 1) + 1], &givnum[(i__ << 1) + 2]);
/* L30: */
	}
	i__2 = givptr[curr + 2] - 1;
	for (i__ = givptr[curr + 1]; i__ <= i__2; ++i__) {
	    drot_(&c__1, &z__[mid - 1 + givcol[(i__ << 1) + 1]], &c__1, &z__[
		    mid - 1 + givcol[(i__ << 1) + 2]], &c__1, &givnum[(i__ <<
		    1) + 1], &givnum[(i__ << 1) + 2]);
/* L40: */
	}
	psiz1 = prmptr[curr + 1] - prmptr[curr];
	psiz2 = prmptr[curr + 2] - prmptr[curr + 1];
	i__2 = psiz1 - 1;
	for (i__ = 0; i__ <= i__2; ++i__) {
	    ztemp[i__ + 1] = z__[zptr1 + perm[prmptr[curr] + i__] - 1];
/* L50: */
	}
	i__2 = psiz2 - 1;
	for (i__ = 0; i__ <= i__2; ++i__) {
	    ztemp[psiz1 + i__ + 1] = z__[mid + perm[prmptr[curr + 1] + i__] -
		    1];
/* L60: */
	}

/*        Multiply Blocks at CURR and CURR+1 */

/*        Determine size of these matrices.  We add HALF to the value of */
/*        the SQRT in case the machine underestimates one of these */
/*        square roots. */

	bsiz1 = (integer) (sqrt((double) (qptr[curr + 1] - qptr[curr])) +
		.5);
	bsiz2 = (integer) (sqrt((double) (qptr[curr + 2] - qptr[curr + 1])
		) + .5);
	if (bsiz1 > 0) {
	    dgemv_("T", &bsiz1, &bsiz1, &c_b24, &q[qptr[curr]], &bsiz1, &
		    ztemp[1], &c__1, &c_b26, &z__[zptr1], &c__1);
	}
	i__2 = psiz1 - bsiz1;
	dcopy_(&i__2, &ztemp[bsiz1 + 1], &c__1, &z__[zptr1 + bsiz1], &c__1);
	if (bsiz2 > 0) {
	    dgemv_("T", &bsiz2, &bsiz2, &c_b24, &q[qptr[curr + 1]], &bsiz2, &
		    ztemp[psiz1 + 1], &c__1, &c_b26, &z__[mid], &c__1);
	}
	i__2 = psiz2 - bsiz2;
	dcopy_(&i__2, &ztemp[psiz1 + bsiz2 + 1], &c__1, &z__[mid + bsiz2], &
		c__1);

	i__2 = *tlvls - k;
	ptr += pow_ii(&c__2, &i__2);
/* L70: */
    }

    return 0;

/*     End of DLAEDA */

} /* dlaeda_ */

/* Subroutine */ int dlaein_(bool *rightv, bool *noinit, integer *n,
	double *h__, integer *ldh, double *wr, double *wi,
	double *vr, double *vi, double *b, integer *ldb,
	double *work, double *eps3, double *smlnum, double *
	bignum, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer b_dim1, b_offset, h_dim1, h_offset, i__1, i__2, i__3, i__4;
    double d__1, d__2, d__3, d__4;

    /* Local variables */
    integer i__, j;
    double w, x, y;
    integer i1, i2, i3;
    double w1, ei, ej, xi, xr, rec;
    integer its, ierr;
    double temp, norm, vmax;
    double scale;
    char trans[1];
    double vcrit, rootn, vnorm;
    double absbii, absbjj;
    char normin[1];
    double nrmsml, growto;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAEIN uses inverse iteration to find a right or left eigenvector */
/*  corresponding to the eigenvalue (WR,WI) of a real upper Hessenberg */
/*  matrix H. */

/*  Arguments */
/*  ========= */

/*  RIGHTV   (input) LOGICAL */
/*          = .TRUE. : compute right eigenvector; */
/*          = .FALSE.: compute left eigenvector. */

/*  NOINIT   (input) LOGICAL */
/*          = .TRUE. : no initial vector supplied in (VR,VI). */
/*          = .FALSE.: initial vector supplied in (VR,VI). */

/*  N       (input) INTEGER */
/*          The order of the matrix H.  N >= 0. */

/*  H       (input) DOUBLE PRECISION array, dimension (LDH,N) */
/*          The upper Hessenberg matrix H. */

/*  LDH     (input) INTEGER */
/*          The leading dimension of the array H.  LDH >= max(1,N). */

/*  WR      (input) DOUBLE PRECISION */
/*  WI      (input) DOUBLE PRECISION */
/*          The real and imaginary parts of the eigenvalue of H whose */
/*          corresponding right or left eigenvector is to be computed. */

/*  VR      (input/output) DOUBLE PRECISION array, dimension (N) */
/*  VI      (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, if NOINIT = .FALSE. and WI = 0.0, VR must contain */
/*          a real starting vector for inverse iteration using the real */
/*          eigenvalue WR; if NOINIT = .FALSE. and WI.ne.0.0, VR and VI */
/*          must contain the real and imaginary parts of a complex */
/*          starting vector for inverse iteration using the complex */
/*          eigenvalue (WR,WI); otherwise VR and VI need not be set. */
/*          On exit, if WI = 0.0 (real eigenvalue), VR contains the */
/*          computed real eigenvector; if WI.ne.0.0 (complex eigenvalue), */
/*          VR and VI contain the real and imaginary parts of the */
/*          computed complex eigenvector. The eigenvector is normalized */
/*          so that the component of largest magnitude has magnitude 1; */
/*          here the magnitude of a complex number (x,y) is taken to be */
/*          |x| + |y|. */
/*          VI is not referenced if WI = 0.0. */

/*  B       (workspace) DOUBLE PRECISION array, dimension (LDB,N) */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= N+1. */

/*  WORK   (workspace) DOUBLE PRECISION array, dimension (N) */

/*  EPS3    (input) DOUBLE PRECISION */
/*          A small machine-dependent value which is used to perturb */
/*          close eigenvalues, and to replace zero pivots. */

/*  SMLNUM  (input) DOUBLE PRECISION */
/*          A machine-dependent value close to the underflow threshold. */

/*  BIGNUM  (input) DOUBLE PRECISION */
/*          A machine-dependent value close to the overflow threshold. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          = 1:  inverse iteration did not converge; VR is set to the */
/*                last iterate, and so is VI if WI.ne.0.0. */

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
    h_dim1 = *ldh;
    h_offset = 1 + h_dim1;
    h__ -= h_offset;
    --vr;
    --vi;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --work;

    /* Function Body */
    *info = 0;

/*     GROWTO is the threshold used in the acceptance test for an */
/*     eigenvector. */

    rootn = sqrt((double) (*n));
    growto = .1 / rootn;
/* Computing MAX */
    d__1 = 1., d__2 = *eps3 * rootn;
    nrmsml = std::max(d__1,d__2) * *smlnum;

/*     Form B = H - (WR,WI)*I (except that the subdiagonal elements and */
/*     the imaginary parts of the diagonal elements are not stored). */

    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = j - 1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    b[i__ + j * b_dim1] = h__[i__ + j * h_dim1];
/* L10: */
	}
	b[j + j * b_dim1] = h__[j + j * h_dim1] - *wr;
/* L20: */
    }

    if (*wi == 0.) {

/*        Real eigenvalue. */

	if (*noinit) {

/*           Set initial vector. */

	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		vr[i__] = *eps3;
/* L30: */
	    }
	} else {

/*           Scale supplied initial vector. */

	    vnorm = dnrm2_(n, &vr[1], &c__1);
	    d__1 = *eps3 * rootn / std::max(vnorm,nrmsml);
	    dscal_(n, &d__1, &vr[1], &c__1);
	}

	if (*rightv) {

/*           LU decomposition with partial pivoting of B, replacing zero */
/*           pivots by EPS3. */

	    i__1 = *n - 1;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		ei = h__[i__ + 1 + i__ * h_dim1];
		if ((d__1 = b[i__ + i__ * b_dim1], abs(d__1)) < abs(ei)) {

/*                 Interchange rows and eliminate. */

		    x = b[i__ + i__ * b_dim1] / ei;
		    b[i__ + i__ * b_dim1] = ei;
		    i__2 = *n;
		    for (j = i__ + 1; j <= i__2; ++j) {
			temp = b[i__ + 1 + j * b_dim1];
			b[i__ + 1 + j * b_dim1] = b[i__ + j * b_dim1] - x *
				temp;
			b[i__ + j * b_dim1] = temp;
/* L40: */
		    }
		} else {

/*                 Eliminate without interchange. */

		    if (b[i__ + i__ * b_dim1] == 0.) {
			b[i__ + i__ * b_dim1] = *eps3;
		    }
		    x = ei / b[i__ + i__ * b_dim1];
		    if (x != 0.) {
			i__2 = *n;
			for (j = i__ + 1; j <= i__2; ++j) {
			    b[i__ + 1 + j * b_dim1] -= x * b[i__ + j * b_dim1]
				    ;
/* L50: */
			}
		    }
		}
/* L60: */
	    }
	    if (b[*n + *n * b_dim1] == 0.) {
		b[*n + *n * b_dim1] = *eps3;
	    }

	    *(unsigned char *)trans = 'N';

	} else {

/*           UL decomposition with partial pivoting of B, replacing zero */
/*           pivots by EPS3. */

	    for (j = *n; j >= 2; --j) {
		ej = h__[j + (j - 1) * h_dim1];
		if ((d__1 = b[j + j * b_dim1], abs(d__1)) < abs(ej)) {

/*                 Interchange columns and eliminate. */

		    x = b[j + j * b_dim1] / ej;
		    b[j + j * b_dim1] = ej;
		    i__1 = j - 1;
		    for (i__ = 1; i__ <= i__1; ++i__) {
			temp = b[i__ + (j - 1) * b_dim1];
			b[i__ + (j - 1) * b_dim1] = b[i__ + j * b_dim1] - x *
				temp;
			b[i__ + j * b_dim1] = temp;
/* L70: */
		    }
		} else {

/*                 Eliminate without interchange. */

		    if (b[j + j * b_dim1] == 0.) {
			b[j + j * b_dim1] = *eps3;
		    }
		    x = ej / b[j + j * b_dim1];
		    if (x != 0.) {
			i__1 = j - 1;
			for (i__ = 1; i__ <= i__1; ++i__) {
			    b[i__ + (j - 1) * b_dim1] -= x * b[i__ + j *
				    b_dim1];
/* L80: */
			}
		    }
		}
/* L90: */
	    }
	    if (b[b_dim1 + 1] == 0.) {
		b[b_dim1 + 1] = *eps3;
	    }

	    *(unsigned char *)trans = 'T';

	}

	*(unsigned char *)normin = 'N';
	i__1 = *n;
	for (its = 1; its <= i__1; ++its) {

/*           Solve U*x = scale*v for a right eigenvector */
/*             or U'*x = scale*v for a left eigenvector, */
/*           overwriting x on v. */

	    dlatrs_("Upper", trans, "Nonunit", normin, n, &b[b_offset], ldb, &
		    vr[1], &scale, &work[1], &ierr);
	    *(unsigned char *)normin = 'Y';

/*           Test for sufficient growth in the norm of v. */

	    vnorm = dasum_(n, &vr[1], &c__1);
	    if (vnorm >= growto * scale) {
		goto L120;
	    }

/*           Choose new orthogonal starting vector and try again. */

	    temp = *eps3 / (rootn + 1.);
	    vr[1] = *eps3;
	    i__2 = *n;
	    for (i__ = 2; i__ <= i__2; ++i__) {
		vr[i__] = temp;
/* L100: */
	    }
	    vr[*n - its + 1] -= *eps3 * rootn;
/* L110: */
	}

/*        Failure to find eigenvector in N iterations. */

	*info = 1;

L120:

/*        Normalize eigenvector. */

	i__ = idamax_(n, &vr[1], &c__1);
	d__2 = 1. / (d__1 = vr[i__], abs(d__1));
	dscal_(n, &d__2, &vr[1], &c__1);
    } else {

/*        Complex eigenvalue. */

	if (*noinit) {

/*           Set initial vector. */

	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		vr[i__] = *eps3;
		vi[i__] = 0.;
/* L130: */
	    }
	} else {

/*           Scale supplied initial vector. */

	    d__1 = dnrm2_(n, &vr[1], &c__1);
	    d__2 = dnrm2_(n, &vi[1], &c__1);
	    norm = dlapy2_(&d__1, &d__2);
	    rec = *eps3 * rootn / std::max(norm,nrmsml);
	    dscal_(n, &rec, &vr[1], &c__1);
	    dscal_(n, &rec, &vi[1], &c__1);
	}

	if (*rightv) {

/*           LU decomposition with partial pivoting of B, replacing zero */
/*           pivots by EPS3. */

/*           The imaginary part of the (i,j)-th element of U is stored in */
/*           B(j+1,i). */

	    b[b_dim1 + 2] = -(*wi);
	    i__1 = *n;
	    for (i__ = 2; i__ <= i__1; ++i__) {
		b[i__ + 1 + b_dim1] = 0.;
/* L140: */
	    }

	    i__1 = *n - 1;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		absbii = dlapy2_(&b[i__ + i__ * b_dim1], &b[i__ + 1 + i__ *
			b_dim1]);
		ei = h__[i__ + 1 + i__ * h_dim1];
		if (absbii < abs(ei)) {

/*                 Interchange rows and eliminate. */

		    xr = b[i__ + i__ * b_dim1] / ei;
		    xi = b[i__ + 1 + i__ * b_dim1] / ei;
		    b[i__ + i__ * b_dim1] = ei;
		    b[i__ + 1 + i__ * b_dim1] = 0.;
		    i__2 = *n;
		    for (j = i__ + 1; j <= i__2; ++j) {
			temp = b[i__ + 1 + j * b_dim1];
			b[i__ + 1 + j * b_dim1] = b[i__ + j * b_dim1] - xr *
				temp;
			b[j + 1 + (i__ + 1) * b_dim1] = b[j + 1 + i__ *
				b_dim1] - xi * temp;
			b[i__ + j * b_dim1] = temp;
			b[j + 1 + i__ * b_dim1] = 0.;
/* L150: */
		    }
		    b[i__ + 2 + i__ * b_dim1] = -(*wi);
		    b[i__ + 1 + (i__ + 1) * b_dim1] -= xi * *wi;
		    b[i__ + 2 + (i__ + 1) * b_dim1] += xr * *wi;
		} else {

/*                 Eliminate without interchanging rows. */

		    if (absbii == 0.) {
			b[i__ + i__ * b_dim1] = *eps3;
			b[i__ + 1 + i__ * b_dim1] = 0.;
			absbii = *eps3;
		    }
		    ei = ei / absbii / absbii;
		    xr = b[i__ + i__ * b_dim1] * ei;
		    xi = -b[i__ + 1 + i__ * b_dim1] * ei;
		    i__2 = *n;
		    for (j = i__ + 1; j <= i__2; ++j) {
			b[i__ + 1 + j * b_dim1] = b[i__ + 1 + j * b_dim1] -
				xr * b[i__ + j * b_dim1] + xi * b[j + 1 + i__
				* b_dim1];
			b[j + 1 + (i__ + 1) * b_dim1] = -xr * b[j + 1 + i__ *
				b_dim1] - xi * b[i__ + j * b_dim1];
/* L160: */
		    }
		    b[i__ + 2 + (i__ + 1) * b_dim1] -= *wi;
		}

/*              Compute 1-norm of offdiagonal elements of i-th row. */

		i__2 = *n - i__;
		i__3 = *n - i__;
		work[i__] = dasum_(&i__2, &b[i__ + (i__ + 1) * b_dim1], ldb)
			+ dasum_(&i__3, &b[i__ + 2 + i__ * b_dim1], &c__1);
/* L170: */
	    }
	    if (b[*n + *n * b_dim1] == 0. && b[*n + 1 + *n * b_dim1] == 0.) {
		b[*n + *n * b_dim1] = *eps3;
	    }
	    work[*n] = 0.;

	    i1 = *n;
	    i2 = 1;
	    i3 = -1;
	} else {

/*           UL decomposition with partial pivoting of conjg(B), */
/*           replacing zero pivots by EPS3. */

/*           The imaginary part of the (i,j)-th element of U is stored in */
/*           B(j+1,i). */

	    b[*n + 1 + *n * b_dim1] = *wi;
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		b[*n + 1 + j * b_dim1] = 0.;
/* L180: */
	    }

	    for (j = *n; j >= 2; --j) {
		ej = h__[j + (j - 1) * h_dim1];
		absbjj = dlapy2_(&b[j + j * b_dim1], &b[j + 1 + j * b_dim1]);
		if (absbjj < abs(ej)) {

/*                 Interchange columns and eliminate */

		    xr = b[j + j * b_dim1] / ej;
		    xi = b[j + 1 + j * b_dim1] / ej;
		    b[j + j * b_dim1] = ej;
		    b[j + 1 + j * b_dim1] = 0.;
		    i__1 = j - 1;
		    for (i__ = 1; i__ <= i__1; ++i__) {
			temp = b[i__ + (j - 1) * b_dim1];
			b[i__ + (j - 1) * b_dim1] = b[i__ + j * b_dim1] - xr *
				 temp;
			b[j + i__ * b_dim1] = b[j + 1 + i__ * b_dim1] - xi *
				temp;
			b[i__ + j * b_dim1] = temp;
			b[j + 1 + i__ * b_dim1] = 0.;
/* L190: */
		    }
		    b[j + 1 + (j - 1) * b_dim1] = *wi;
		    b[j - 1 + (j - 1) * b_dim1] += xi * *wi;
		    b[j + (j - 1) * b_dim1] -= xr * *wi;
		} else {

/*                 Eliminate without interchange. */

		    if (absbjj == 0.) {
			b[j + j * b_dim1] = *eps3;
			b[j + 1 + j * b_dim1] = 0.;
			absbjj = *eps3;
		    }
		    ej = ej / absbjj / absbjj;
		    xr = b[j + j * b_dim1] * ej;
		    xi = -b[j + 1 + j * b_dim1] * ej;
		    i__1 = j - 1;
		    for (i__ = 1; i__ <= i__1; ++i__) {
			b[i__ + (j - 1) * b_dim1] = b[i__ + (j - 1) * b_dim1]
				- xr * b[i__ + j * b_dim1] + xi * b[j + 1 +
				i__ * b_dim1];
			b[j + i__ * b_dim1] = -xr * b[j + 1 + i__ * b_dim1] -
				xi * b[i__ + j * b_dim1];
/* L200: */
		    }
		    b[j + (j - 1) * b_dim1] += *wi;
		}

/*              Compute 1-norm of offdiagonal elements of j-th column. */

		i__1 = j - 1;
		i__2 = j - 1;
		work[j] = dasum_(&i__1, &b[j * b_dim1 + 1], &c__1) + dasum_(&
			i__2, &b[j + 1 + b_dim1], ldb);
/* L210: */
	    }
	    if (b[b_dim1 + 1] == 0. && b[b_dim1 + 2] == 0.) {
		b[b_dim1 + 1] = *eps3;
	    }
	    work[1] = 0.;

	    i1 = 1;
	    i2 = *n;
	    i3 = 1;
	}

	i__1 = *n;
	for (its = 1; its <= i__1; ++its) {
	    scale = 1.;
	    vmax = 1.;
	    vcrit = *bignum;

/*           Solve U*(xr,xi) = scale*(vr,vi) for a right eigenvector, */
/*             or U'*(xr,xi) = scale*(vr,vi) for a left eigenvector, */
/*           overwriting (xr,xi) on (vr,vi). */

	    i__2 = i2;
	    i__3 = i3;
	    for (i__ = i1; i__3 < 0 ? i__ >= i__2 : i__ <= i__2; i__ += i__3)
		    {

		if (work[i__] > vcrit) {
		    rec = 1. / vmax;
		    dscal_(n, &rec, &vr[1], &c__1);
		    dscal_(n, &rec, &vi[1], &c__1);
		    scale *= rec;
		    vmax = 1.;
		    vcrit = *bignum;
		}

		xr = vr[i__];
		xi = vi[i__];
		if (*rightv) {
		    i__4 = *n;
		    for (j = i__ + 1; j <= i__4; ++j) {
			xr = xr - b[i__ + j * b_dim1] * vr[j] + b[j + 1 + i__
				* b_dim1] * vi[j];
			xi = xi - b[i__ + j * b_dim1] * vi[j] - b[j + 1 + i__
				* b_dim1] * vr[j];
/* L220: */
		    }
		} else {
		    i__4 = i__ - 1;
		    for (j = 1; j <= i__4; ++j) {
			xr = xr - b[j + i__ * b_dim1] * vr[j] + b[i__ + 1 + j
				* b_dim1] * vi[j];
			xi = xi - b[j + i__ * b_dim1] * vi[j] - b[i__ + 1 + j
				* b_dim1] * vr[j];
/* L230: */
		    }
		}

		w = (d__1 = b[i__ + i__ * b_dim1], abs(d__1)) + (d__2 = b[i__
			+ 1 + i__ * b_dim1], abs(d__2));
		if (w > *smlnum) {
		    if (w < 1.) {
			w1 = abs(xr) + abs(xi);
			if (w1 > w * *bignum) {
			    rec = 1. / w1;
			    dscal_(n, &rec, &vr[1], &c__1);
			    dscal_(n, &rec, &vi[1], &c__1);
			    xr = vr[i__];
			    xi = vi[i__];
			    scale *= rec;
			    vmax *= rec;
			}
		    }

/*                 Divide by diagonal element of B. */

		    dladiv_(&xr, &xi, &b[i__ + i__ * b_dim1], &b[i__ + 1 +
			    i__ * b_dim1], &vr[i__], &vi[i__]);
/* Computing MAX */
		    d__3 = (d__1 = vr[i__], abs(d__1)) + (d__2 = vi[i__], abs(
			    d__2));
		    vmax = std::max(d__3,vmax);
		    vcrit = *bignum / vmax;
		} else {
		    i__4 = *n;
		    for (j = 1; j <= i__4; ++j) {
			vr[j] = 0.;
			vi[j] = 0.;
/* L240: */
		    }
		    vr[i__] = 1.;
		    vi[i__] = 1.;
		    scale = 0.;
		    vmax = 1.;
		    vcrit = *bignum;
		}
/* L250: */
	    }

/*           Test for sufficient growth in the norm of (VR,VI). */

	    vnorm = dasum_(n, &vr[1], &c__1) + dasum_(n, &vi[1], &c__1);
	    if (vnorm >= growto * scale) {
		goto L280;
	    }

/*           Choose a new orthogonal starting vector and try again. */

	    y = *eps3 / (rootn + 1.);
	    vr[1] = *eps3;
	    vi[1] = 0.;

	    i__3 = *n;
	    for (i__ = 2; i__ <= i__3; ++i__) {
		vr[i__] = y;
		vi[i__] = 0.;
/* L260: */
	    }
	    vr[*n - its + 1] -= *eps3 * rootn;
/* L270: */
	}

/*        Failure to find eigenvector in N iterations */

	*info = 1;

L280:

/*        Normalize eigenvector. */

	vnorm = 0.;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	    d__3 = vnorm, d__4 = (d__1 = vr[i__], abs(d__1)) + (d__2 = vi[i__]
		    , abs(d__2));
	    vnorm = std::max(d__3,d__4);
/* L290: */
	}
	d__1 = 1. / vnorm;
	dscal_(n, &d__1, &vr[1], &c__1);
	d__1 = 1. / vnorm;
	dscal_(n, &d__1, &vi[1], &c__1);

    }

    return 0;

/*     End of DLAEIN */

} /* dlaein_ */

/* Subroutine */ int dlaev2_(double *a, double *b, double *c__,
	double *rt1, double *rt2, double *cs1, double *sn1)
{
    /* System generated locals */
    double d__1;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    double ab, df, cs, ct, tb, sm, tn, rt, adf, acs;
    integer sgn1, sgn2;
    double acmn, acmx;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAEV2 computes the eigendecomposition of a 2-by-2 symmetric matrix */
/*     [  A   B  ] */
/*     [  B   C  ]. */
/*  On return, RT1 is the eigenvalue of larger absolute value, RT2 is the */
/*  eigenvalue of smaller absolute value, and (CS1,SN1) is the unit right */
/*  eigenvector for RT1, giving the decomposition */

/*     [ CS1  SN1 ] [  A   B  ] [ CS1 -SN1 ]  =  [ RT1  0  ] */
/*     [-SN1  CS1 ] [  B   C  ] [ SN1  CS1 ]     [  0  RT2 ]. */

/*  Arguments */
/*  ========= */

/*  A       (input) DOUBLE PRECISION */
/*          The (1,1) element of the 2-by-2 matrix. */

/*  B       (input) DOUBLE PRECISION */
/*          The (1,2) element and the conjugate of the (2,1) element of */
/*          the 2-by-2 matrix. */

/*  C       (input) DOUBLE PRECISION */
/*          The (2,2) element of the 2-by-2 matrix. */

/*  RT1     (output) DOUBLE PRECISION */
/*          The eigenvalue of larger absolute value. */

/*  RT2     (output) DOUBLE PRECISION */
/*          The eigenvalue of smaller absolute value. */

/*  CS1     (output) DOUBLE PRECISION */
/*  SN1     (output) DOUBLE PRECISION */
/*          The vector (CS1, SN1) is a unit right eigenvector for RT1. */

/*  Further Details */
/*  =============== */

/*  RT1 is accurate to a few ulps barring over/underflow. */

/*  RT2 may be inaccurate if there is massive cancellation in the */
/*  determinant A*C-B*B; higher precision or correctly rounded or */
/*  correctly truncated arithmetic would be needed to compute RT2 */
/*  accurately in all cases. */

/*  CS1 and SN1 are accurate to a few ulps barring over/underflow. */

/*  Overflow is possible only if RT1 is within a factor of 5 of overflow. */
/*  Underflow is harmless if the input data is 0 or exceeds */
/*     underflow_threshold / macheps. */

/* ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Compute the eigenvalues */

    sm = *a + *c__;
    df = *a - *c__;
    adf = abs(df);
    tb = *b + *b;
    ab = abs(tb);
    if (abs(*a) > abs(*c__)) {
	acmx = *a;
	acmn = *c__;
    } else {
	acmx = *c__;
	acmn = *a;
    }
    if (adf > ab) {
/* Computing 2nd power */
	d__1 = ab / adf;
	rt = adf * sqrt(d__1 * d__1 + 1.);
    } else if (adf < ab) {
/* Computing 2nd power */
	d__1 = adf / ab;
	rt = ab * sqrt(d__1 * d__1 + 1.);
    } else {

/*        Includes case AB=ADF=0 */

	rt = ab * sqrt(2.);
    }
    if (sm < 0.) {
	*rt1 = (sm - rt) * .5;
	sgn1 = -1;

/*        Order of execution important. */
/*        To get fully accurate smaller eigenvalue, */
/*        next line needs to be executed in higher precision. */

	*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
    } else if (sm > 0.) {
	*rt1 = (sm + rt) * .5;
	sgn1 = 1;

/*        Order of execution important. */
/*        To get fully accurate smaller eigenvalue, */
/*        next line needs to be executed in higher precision. */

	*rt2 = acmx / *rt1 * acmn - *b / *rt1 * *b;
    } else {

/*        Includes case RT1 = RT2 = 0 */

	*rt1 = rt * .5;
	*rt2 = rt * -.5;
	sgn1 = 1;
    }

/*     Compute the eigenvector */

    if (df >= 0.) {
	cs = df + rt;
	sgn2 = 1;
    } else {
	cs = df - rt;
	sgn2 = -1;
    }
    acs = abs(cs);
    if (acs > ab) {
	ct = -tb / cs;
	*sn1 = 1. / sqrt(ct * ct + 1.);
	*cs1 = ct * *sn1;
    } else {
	if (ab == 0.) {
	    *cs1 = 1.;
	    *sn1 = 0.;
	} else {
	    tn = -cs / tb;
	    *cs1 = 1. / sqrt(tn * tn + 1.);
	    *sn1 = tn * *cs1;
	}
    }
    if (sgn1 == sgn2) {
	tn = *cs1;
	*cs1 = -(*sn1);
	*sn1 = tn;
    }
    return 0;

/*     End of DLAEV2 */

} /* dlaev2_ */

/* Subroutine */ int dlaexc_(bool *wantq, integer *n, double *t,
	integer *ldt, double *q, integer *ldq, integer *j1, integer *n1,
	integer *n2, double *work, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c__4 = 4;
	static bool c_false = false;
	static integer c_n1 = -1;
	static integer c__2 = 2;
	static integer c__3 = 3;

    /* System generated locals */
    integer q_dim1, q_offset, t_dim1, t_offset, i__1;
    double d__1, d__2, d__3;

    /* Local variables */
    double d__[16]	/* was [4][4] */;
    integer k;
    double u[3], x[4]	/* was [2][2] */;
    integer j2, j3, j4;
    double u1[3], u2[3];
    integer nd;
    double cs, t11, t22, t33, sn, wi1, wi2, wr1, wr2, eps, tau, tau1,
	    tau2;
    integer ierr;
    double temp;
    double scale, dnorm, xnorm;
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

/*  DLAEXC swaps adjacent diagonal blocks T11 and T22 of order 1 or 2 in */
/*  an upper quasi-triangular matrix T by an orthogonal similarity */
/*  transformation. */

/*  T must be in Schur canonical form, that is, block upper triangular */
/*  with 1-by-1 and 2-by-2 diagonal blocks; each 2-by-2 diagonal block */
/*  has its diagonal elemnts equal and its off-diagonal elements of */
/*  opposite sign. */

/*  Arguments */
/*  ========= */

/*  WANTQ   (input) LOGICAL */
/*          = .TRUE. : accumulate the transformation in the matrix Q; */
/*          = .FALSE.: do not accumulate the transformation. */

/*  N       (input) INTEGER */
/*          The order of the matrix T. N >= 0. */

/*  T       (input/output) DOUBLE PRECISION array, dimension (LDT,N) */
/*          On entry, the upper quasi-triangular matrix T, in Schur */
/*          canonical form. */
/*          On exit, the updated matrix T, again in Schur canonical form. */

/*  LDT     (input)  INTEGER */
/*          The leading dimension of the array T. LDT >= max(1,N). */

/*  Q       (input/output) DOUBLE PRECISION array, dimension (LDQ,N) */
/*          On entry, if WANTQ is .TRUE., the orthogonal matrix Q. */
/*          On exit, if WANTQ is .TRUE., the updated matrix Q. */
/*          If WANTQ is .FALSE., Q is not referenced. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q. */
/*          LDQ >= 1; and if WANTQ is .TRUE., LDQ >= N. */

/*  J1      (input) INTEGER */
/*          The index of the first row of the first block T11. */

/*  N1      (input) INTEGER */
/*          The order of the first block T11. N1 = 0, 1 or 2. */

/*  N2      (input) INTEGER */
/*          The order of the second block T22. N2 = 0, 1 or 2. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          = 1: the transformed matrix T would be too far from Schur */
/*               form; the blocks are not swapped and T and Q are */
/*               unchanged. */

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
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    q_dim1 = *ldq;
    q_offset = 1 + q_dim1;
    q -= q_offset;
    --work;

    /* Function Body */
    *info = 0;

/*     Quick return if possible */

    if (*n == 0 || *n1 == 0 || *n2 == 0) {
	return 0;
    }
    if (*j1 + *n1 > *n) {
	return 0;
    }

    j2 = *j1 + 1;
    j3 = *j1 + 2;
    j4 = *j1 + 3;

    if (*n1 == 1 && *n2 == 1) {

/*        Swap two 1-by-1 blocks. */

	t11 = t[*j1 + *j1 * t_dim1];
	t22 = t[j2 + j2 * t_dim1];

/*        Determine the transformation to perform the interchange. */

	d__1 = t22 - t11;
	dlartg_(&t[*j1 + j2 * t_dim1], &d__1, &cs, &sn, &temp);

/*        Apply transformation to the matrix T. */

	if (j3 <= *n) {
	    i__1 = *n - *j1 - 1;
	    drot_(&i__1, &t[*j1 + j3 * t_dim1], ldt, &t[j2 + j3 * t_dim1],
		    ldt, &cs, &sn);
	}
	i__1 = *j1 - 1;
	drot_(&i__1, &t[*j1 * t_dim1 + 1], &c__1, &t[j2 * t_dim1 + 1], &c__1,
		&cs, &sn);

	t[*j1 + *j1 * t_dim1] = t22;
	t[j2 + j2 * t_dim1] = t11;

	if (*wantq) {

/*           Accumulate transformation in the matrix Q. */

	    drot_(n, &q[*j1 * q_dim1 + 1], &c__1, &q[j2 * q_dim1 + 1], &c__1,
		    &cs, &sn);
	}

    } else {

/*        Swapping involves at least one 2-by-2 block. */

/*        Copy the diagonal block of order N1+N2 to the local array D */
/*        and compute its norm. */

	nd = *n1 + *n2;
	dlacpy_("Full", &nd, &nd, &t[*j1 + *j1 * t_dim1], ldt, d__, &c__4);
	dnorm = dlange_("Max", &nd, &nd, d__, &c__4, &work[1]);

/*        Compute machine-dependent threshold for test for accepting */
/*        swap. */

	eps = dlamch_("P");
	smlnum = dlamch_("S") / eps;
/* Computing MAX */
	d__1 = eps * 10. * dnorm;
	thresh = std::max(d__1,smlnum);

/*        Solve T11*X - X*T22 = scale*T12 for X. */

	dlasy2_(&c_false, &c_false, &c_n1, n1, n2, d__, &c__4, &d__[*n1 + 1 +
		(*n1 + 1 << 2) - 5], &c__4, &d__[(*n1 + 1 << 2) - 4], &c__4, &
		scale, x, &c__2, &xnorm, &ierr);

/*        Swap the adjacent diagonal blocks. */

	k = *n1 + *n1 + *n2 - 3;
	switch (k) {
	    case 1:  goto L10;
	    case 2:  goto L20;
	    case 3:  goto L30;
	}

L10:

/*        N1 = 1, N2 = 2: generate elementary reflector H so that: */

/*        ( scale, X11, X12 ) H = ( 0, 0, * ) */

	u[0] = scale;
	u[1] = x[0];
	u[2] = x[2];
	dlarfg_(&c__3, &u[2], u, &c__1, &tau);
	u[2] = 1.;
	t11 = t[*j1 + *j1 * t_dim1];

/*        Perform swap provisionally on diagonal block in D. */

	dlarfx_("L", &c__3, &c__3, u, &tau, d__, &c__4, &work[1]);
	dlarfx_("R", &c__3, &c__3, u, &tau, d__, &c__4, &work[1]);

/*        Test whether to reject swap. */

/* Computing MAX */
	d__2 = abs(d__[2]), d__3 = abs(d__[6]), d__2 = std::max(d__2,d__3), d__3 =
		(d__1 = d__[10] - t11, abs(d__1));
	if (std::max(d__2,d__3) > thresh) {
	    goto L50;
	}

/*        Accept swap: apply transformation to the entire matrix T. */

	i__1 = *n - *j1 + 1;
	dlarfx_("L", &c__3, &i__1, u, &tau, &t[*j1 + *j1 * t_dim1], ldt, &
		work[1]);
	dlarfx_("R", &j2, &c__3, u, &tau, &t[*j1 * t_dim1 + 1], ldt, &work[1]);

	t[j3 + *j1 * t_dim1] = 0.;
	t[j3 + j2 * t_dim1] = 0.;
	t[j3 + j3 * t_dim1] = t11;

	if (*wantq) {

/*           Accumulate transformation in the matrix Q. */

	    dlarfx_("R", n, &c__3, u, &tau, &q[*j1 * q_dim1 + 1], ldq, &work[
		    1]);
	}
	goto L40;

L20:

/*        N1 = 2, N2 = 1: generate elementary reflector H so that: */

/*        H (  -X11 ) = ( * ) */
/*          (  -X21 ) = ( 0 ) */
/*          ( scale ) = ( 0 ) */

	u[0] = -x[0];
	u[1] = -x[1];
	u[2] = scale;
	dlarfg_(&c__3, u, &u[1], &c__1, &tau);
	u[0] = 1.;
	t33 = t[j3 + j3 * t_dim1];

/*        Perform swap provisionally on diagonal block in D. */

	dlarfx_("L", &c__3, &c__3, u, &tau, d__, &c__4, &work[1]);
	dlarfx_("R", &c__3, &c__3, u, &tau, d__, &c__4, &work[1]);

/*        Test whether to reject swap. */

/* Computing MAX */
	d__2 = abs(d__[1]), d__3 = abs(d__[2]), d__2 = std::max(d__2,d__3), d__3 =
		(d__1 = d__[0] - t33, abs(d__1));
	if (std::max(d__2,d__3) > thresh) {
	    goto L50;
	}

/*        Accept swap: apply transformation to the entire matrix T. */

	dlarfx_("R", &j3, &c__3, u, &tau, &t[*j1 * t_dim1 + 1], ldt, &work[1]);
	i__1 = *n - *j1;
	dlarfx_("L", &c__3, &i__1, u, &tau, &t[*j1 + j2 * t_dim1], ldt, &work[
		1]);

	t[*j1 + *j1 * t_dim1] = t33;
	t[j2 + *j1 * t_dim1] = 0.;
	t[j3 + *j1 * t_dim1] = 0.;

	if (*wantq) {

/*           Accumulate transformation in the matrix Q. */

	    dlarfx_("R", n, &c__3, u, &tau, &q[*j1 * q_dim1 + 1], ldq, &work[
		    1]);
	}
	goto L40;

L30:

/*        N1 = 2, N2 = 2: generate elementary reflectors H(1) and H(2) so */
/*        that: */

/*        H(2) H(1) (  -X11  -X12 ) = (  *  * ) */
/*                  (  -X21  -X22 )   (  0  * ) */
/*                  ( scale    0  )   (  0  0 ) */
/*                  (    0  scale )   (  0  0 ) */

	u1[0] = -x[0];
	u1[1] = -x[1];
	u1[2] = scale;
	dlarfg_(&c__3, u1, &u1[1], &c__1, &tau1);
	u1[0] = 1.;

	temp = -tau1 * (x[2] + u1[1] * x[3]);
	u2[0] = -temp * u1[1] - x[3];
	u2[1] = -temp * u1[2];
	u2[2] = scale;
	dlarfg_(&c__3, u2, &u2[1], &c__1, &tau2);
	u2[0] = 1.;

/*        Perform swap provisionally on diagonal block in D. */

	dlarfx_("L", &c__3, &c__4, u1, &tau1, d__, &c__4, &work[1])
		;
	dlarfx_("R", &c__4, &c__3, u1, &tau1, d__, &c__4, &work[1])
		;
	dlarfx_("L", &c__3, &c__4, u2, &tau2, &d__[1], &c__4, &work[1]);
	dlarfx_("R", &c__4, &c__3, u2, &tau2, &d__[4], &c__4, &work[1]);

/*        Test whether to reject swap. */

/* Computing MAX */
	d__1 = abs(d__[2]), d__2 = abs(d__[6]), d__1 = std::max(d__1,d__2), d__2 =
		abs(d__[3]), d__1 = std::max(d__1,d__2), d__2 = abs(d__[7]);
	if (std::max(d__1,d__2) > thresh) {
	    goto L50;
	}

/*        Accept swap: apply transformation to the entire matrix T. */

	i__1 = *n - *j1 + 1;
	dlarfx_("L", &c__3, &i__1, u1, &tau1, &t[*j1 + *j1 * t_dim1], ldt, &
		work[1]);
	dlarfx_("R", &j4, &c__3, u1, &tau1, &t[*j1 * t_dim1 + 1], ldt, &work[
		1]);
	i__1 = *n - *j1 + 1;
	dlarfx_("L", &c__3, &i__1, u2, &tau2, &t[j2 + *j1 * t_dim1], ldt, &
		work[1]);
	dlarfx_("R", &j4, &c__3, u2, &tau2, &t[j2 * t_dim1 + 1], ldt, &work[1]
);

	t[j3 + *j1 * t_dim1] = 0.;
	t[j3 + j2 * t_dim1] = 0.;
	t[j4 + *j1 * t_dim1] = 0.;
	t[j4 + j2 * t_dim1] = 0.;

	if (*wantq) {

/*           Accumulate transformation in the matrix Q. */

	    dlarfx_("R", n, &c__3, u1, &tau1, &q[*j1 * q_dim1 + 1], ldq, &
		    work[1]);
	    dlarfx_("R", n, &c__3, u2, &tau2, &q[j2 * q_dim1 + 1], ldq, &work[
		    1]);
	}

L40:

	if (*n2 == 2) {

/*           Standardize new 2-by-2 block T11 */

	    dlanv2_(&t[*j1 + *j1 * t_dim1], &t[*j1 + j2 * t_dim1], &t[j2 + *
		    j1 * t_dim1], &t[j2 + j2 * t_dim1], &wr1, &wi1, &wr2, &
		    wi2, &cs, &sn);
	    i__1 = *n - *j1 - 1;
	    drot_(&i__1, &t[*j1 + (*j1 + 2) * t_dim1], ldt, &t[j2 + (*j1 + 2)
		    * t_dim1], ldt, &cs, &sn);
	    i__1 = *j1 - 1;
	    drot_(&i__1, &t[*j1 * t_dim1 + 1], &c__1, &t[j2 * t_dim1 + 1], &
		    c__1, &cs, &sn);
	    if (*wantq) {
		drot_(n, &q[*j1 * q_dim1 + 1], &c__1, &q[j2 * q_dim1 + 1], &
			c__1, &cs, &sn);
	    }
	}

	if (*n1 == 2) {

/*           Standardize new 2-by-2 block T22 */

	    j3 = *j1 + *n2;
	    j4 = j3 + 1;
	    dlanv2_(&t[j3 + j3 * t_dim1], &t[j3 + j4 * t_dim1], &t[j4 + j3 *
		    t_dim1], &t[j4 + j4 * t_dim1], &wr1, &wi1, &wr2, &wi2, &
		    cs, &sn);
	    if (j3 + 2 <= *n) {
		i__1 = *n - j3 - 1;
		drot_(&i__1, &t[j3 + (j3 + 2) * t_dim1], ldt, &t[j4 + (j3 + 2)
			 * t_dim1], ldt, &cs, &sn);
	    }
	    i__1 = j3 - 1;
	    drot_(&i__1, &t[j3 * t_dim1 + 1], &c__1, &t[j4 * t_dim1 + 1], &
		    c__1, &cs, &sn);
	    if (*wantq) {
		drot_(n, &q[j3 * q_dim1 + 1], &c__1, &q[j4 * q_dim1 + 1], &
			c__1, &cs, &sn);
	    }
	}

    }
    return 0;

/*     Exit with INFO = 1 if swap was rejected. */

L50:
    *info = 1;
    return 0;

/*     End of DLAEXC */

} /* dlaexc_ */

/* Subroutine */ int dlag2_(double *a, integer *lda, double *b,
	integer *ldb, double *safmin, double *scale1, double *
	scale2, double *wr1, double *wr2, double *wi)
{
    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset;
    double d__1, d__2, d__3, d__4, d__5, d__6;

    /* Builtin functions
    double sqrt(double), d_sign(double *, double *); */

    /* Local variables */
    double r__, c1, c2, c3, c4, c5, s1, s2, a11, a12, a21, a22, b11, b12,
	    b22, pp, qq, ss, as11, as12, as22, sum, abi22, diff, bmin, wbig,
	    wabs, wdet, binv11, binv22, discr, anorm, bnorm, bsize, shift,
	    rtmin, rtmax, wsize, ascale, bscale, wscale, safmax, wsmall;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAG2 computes the eigenvalues of a 2 x 2 generalized eigenvalue */
/*  problem  A - w B, with scaling as necessary to avoid over-/underflow. */

/*  The scaling factor "s" results in a modified eigenvalue equation */

/*      s A - w B */

/*  where  s  is a non-negative scaling factor chosen so that  w,  w B, */
/*  and  s A  do not overflow and, if possible, do not underflow, either. */

/*  Arguments */
/*  ========= */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA, 2) */
/*          On entry, the 2 x 2 matrix A.  It is assumed that its 1-norm */
/*          is less than 1/SAFMIN.  Entries less than */
/*          sqrt(SAFMIN)*norm(A) are subject to being treated as zero. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= 2. */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB, 2) */
/*          On entry, the 2 x 2 upper triangular matrix B.  It is */
/*          assumed that the one-norm of B is less than 1/SAFMIN.  The */
/*          diagonals should be at least sqrt(SAFMIN) times the largest */
/*          element of B (in absolute value); if a diagonal is smaller */
/*          than that, then  +/- sqrt(SAFMIN) will be used instead of */
/*          that diagonal. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= 2. */

/*  SAFMIN  (input) DOUBLE PRECISION */
/*          The smallest positive number s.t. 1/SAFMIN does not */
/*          overflow.  (This should always be DLAMCH('S') -- it is an */
/*          argument in order to avoid having to call DLAMCH frequently.) */

/*  SCALE1  (output) DOUBLE PRECISION */
/*          A scaling factor used to avoid over-/underflow in the */
/*          eigenvalue equation which defines the first eigenvalue.  If */
/*          the eigenvalues are complex, then the eigenvalues are */
/*          ( WR1  +/-  WI i ) / SCALE1  (which may lie outside the */
/*          exponent range of the machine), SCALE1=SCALE2, and SCALE1 */
/*          will always be positive.  If the eigenvalues are real, then */
/*          the first (real) eigenvalue is  WR1 / SCALE1 , but this may */
/*          overflow or underflow, and in fact, SCALE1 may be zero or */
/*          less than the underflow threshhold if the exact eigenvalue */
/*          is sufficiently large. */

/*  SCALE2  (output) DOUBLE PRECISION */
/*          A scaling factor used to avoid over-/underflow in the */
/*          eigenvalue equation which defines the second eigenvalue.  If */
/*          the eigenvalues are complex, then SCALE2=SCALE1.  If the */
/*          eigenvalues are real, then the second (real) eigenvalue is */
/*          WR2 / SCALE2 , but this may overflow or underflow, and in */
/*          fact, SCALE2 may be zero or less than the underflow */
/*          threshhold if the exact eigenvalue is sufficiently large. */

/*  WR1     (output) DOUBLE PRECISION */
/*          If the eigenvalue is real, then WR1 is SCALE1 times the */
/*          eigenvalue closest to the (2,2) element of A B**(-1).  If the */
/*          eigenvalue is complex, then WR1=WR2 is SCALE1 times the real */
/*          part of the eigenvalues. */

/*  WR2     (output) DOUBLE PRECISION */
/*          If the eigenvalue is real, then WR2 is SCALE2 times the */
/*          other eigenvalue.  If the eigenvalue is complex, then */
/*          WR1=WR2 is SCALE1 times the real part of the eigenvalues. */

/*  WI      (output) DOUBLE PRECISION */
/*          If the eigenvalue is real, then WI is zero.  If the */
/*          eigenvalue is complex, then WI is SCALE1 times the imaginary */
/*          part of the eigenvalues.  WI will always be non-negative. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
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

    /* Function Body */
    rtmin = sqrt(*safmin);
    rtmax = 1. / rtmin;
    safmax = 1. / *safmin;

/*     Scale A */

/* Computing MAX */
    d__5 = (d__1 = a[a_dim1 + 1], abs(d__1)) + (d__2 = a[a_dim1 + 2], abs(
	    d__2)), d__6 = (d__3 = a[(a_dim1 << 1) + 1], abs(d__3)) + (d__4 =
	    a[(a_dim1 << 1) + 2], abs(d__4)), d__5 = std::max(d__5,d__6);
    anorm = std::max(d__5,*safmin);
    ascale = 1. / anorm;
    a11 = ascale * a[a_dim1 + 1];
    a21 = ascale * a[a_dim1 + 2];
    a12 = ascale * a[(a_dim1 << 1) + 1];
    a22 = ascale * a[(a_dim1 << 1) + 2];

/*     Perturb B if necessary to insure non-singularity */

    b11 = b[b_dim1 + 1];
    b12 = b[(b_dim1 << 1) + 1];
    b22 = b[(b_dim1 << 1) + 2];
/* Computing MAX */
    d__1 = abs(b11), d__2 = abs(b12), d__1 = std::max(d__1,d__2), d__2 = abs(b22),
	    d__1 = std::max(d__1,d__2);
    bmin = rtmin * std::max(d__1,rtmin);
    if (abs(b11) < bmin) {
	b11 = d_sign(&bmin, &b11);
    }
    if (abs(b22) < bmin) {
	b22 = d_sign(&bmin, &b22);
    }

/*     Scale B */

/* Computing MAX */
    d__1 = abs(b11), d__2 = abs(b12) + abs(b22), d__1 = std::max(d__1,d__2);
    bnorm = std::max(d__1,*safmin);
/* Computing MAX */
    d__1 = abs(b11), d__2 = abs(b22);
    bsize = std::max(d__1,d__2);
    bscale = 1. / bsize;
    b11 *= bscale;
    b12 *= bscale;
    b22 *= bscale;

/*     Compute larger eigenvalue by method described by C. van Loan */

/*     ( AS is A shifted by -SHIFT*B ) */

    binv11 = 1. / b11;
    binv22 = 1. / b22;
    s1 = a11 * binv11;
    s2 = a22 * binv22;
    if (abs(s1) <= abs(s2)) {
	as12 = a12 - s1 * b12;
	as22 = a22 - s1 * b22;
	ss = a21 * (binv11 * binv22);
	abi22 = as22 * binv22 - ss * b12;
	pp = abi22 * .5;
	shift = s1;
    } else {
	as12 = a12 - s2 * b12;
	as11 = a11 - s2 * b11;
	ss = a21 * (binv11 * binv22);
	abi22 = -ss * b12;
	pp = (as11 * binv11 + abi22) * .5;
	shift = s2;
    }
    qq = ss * as12;
    if ((d__1 = pp * rtmin, abs(d__1)) >= 1.) {
/* Computing 2nd power */
	d__1 = rtmin * pp;
	discr = d__1 * d__1 + qq * *safmin;
	r__ = sqrt((abs(discr))) * rtmax;
    } else {
/* Computing 2nd power */
	d__1 = pp;
	if (d__1 * d__1 + abs(qq) <= *safmin) {
/* Computing 2nd power */
	    d__1 = rtmax * pp;
	    discr = d__1 * d__1 + qq * safmax;
	    r__ = sqrt((abs(discr))) * rtmin;
	} else {
/* Computing 2nd power */
	    d__1 = pp;
	    discr = d__1 * d__1 + qq;
	    r__ = sqrt((abs(discr)));
	}
    }

/*     Note: the test of R in the following IF is to cover the case when */
/*           DISCR is small and negative and is flushed to zero during */
/*           the calculation of R.  On machines which have a consistent */
/*           flush-to-zero threshhold and handle numbers above that */
/*           threshhold correctly, it would not be necessary. */

    if (discr >= 0. || r__ == 0.) {
	sum = pp + d_sign(&r__, &pp);
	diff = pp - d_sign(&r__, &pp);
	wbig = shift + sum;

/*        Compute smaller eigenvalue */

	wsmall = shift + diff;
/* Computing MAX */
	d__1 = abs(wsmall);
	if (abs(wbig) * .5 > std::max(d__1,*safmin)) {
	    wdet = (a11 * a22 - a12 * a21) * (binv11 * binv22);
	    wsmall = wdet / wbig;
	}

/*        Choose (real) eigenvalue closest to 2,2 element of A*B**(-1) */
/*        for WR1. */

	if (pp > abi22) {
	    *wr1 = std::min(wbig,wsmall);
	    *wr2 = std::max(wbig,wsmall);
	} else {
	    *wr1 = std::max(wbig,wsmall);
	    *wr2 = std::min(wbig,wsmall);
	}
	*wi = 0.;
    } else {

/*        Complex eigenvalues */

	*wr1 = shift + pp;
	*wr2 = *wr1;
	*wi = r__;
    }

/*     Further scaling to avoid underflow and overflow in computing */
/*     SCALE1 and overflow in computing w*B. */

/*     This scale factor (WSCALE) is bounded from above using C1 and C2, */
/*     and from below using C3 and C4. */
/*        C1 implements the condition  s A  must never overflow. */
/*        C2 implements the condition  w B  must never overflow. */
/*        C3, with C2, */
/*           implement the condition that s A - w B must never overflow. */
/*        C4 implements the condition  s    should not underflow. */
/*        C5 implements the condition  max(s,|w|) should be at least 2. */

    c1 = bsize * (*safmin * std::max(1.,ascale));
    c2 = *safmin * std::max(1.,bnorm);
    c3 = bsize * *safmin;
    if (ascale <= 1. && bsize <= 1.) {
/* Computing MIN */
	d__1 = 1., d__2 = ascale / *safmin * bsize;
	c4 = std::min(d__1,d__2);
    } else {
	c4 = 1.;
    }
    if (ascale <= 1. || bsize <= 1.) {
/* Computing MIN */
	d__1 = 1., d__2 = ascale * bsize;
	c5 = std::min(d__1,d__2);
    } else {
	c5 = 1.;
    }

/*     Scale first eigenvalue */

    wabs = abs(*wr1) + abs(*wi);
/* Computing MAX */
/* Computing MIN */
    d__3 = c4, d__4 = std::max(wabs,c5) * .5;
    d__1 = std::max(*safmin,c1), d__2 = (wabs * c2 + c3) * 1.0000100000000001,
	    d__1 = std::max(d__1,d__2), d__2 = std::min(d__3,d__4);
    wsize = std::max(d__1,d__2);
    if (wsize != 1.) {
	wscale = 1. / wsize;
	if (wsize > 1.) {
	    *scale1 = std::max(ascale,bsize) * wscale * std::min(ascale,bsize);
	} else {
	    *scale1 = std::min(ascale,bsize) * wscale * std::max(ascale,bsize);
	}
	*wr1 *= wscale;
	if (*wi != 0.) {
	    *wi *= wscale;
	    *wr2 = *wr1;
	    *scale2 = *scale1;
	}
    } else {
	*scale1 = ascale * bsize;
	*scale2 = *scale1;
    }

/*     Scale second eigenvalue (if real) */

    if (*wi == 0.) {
/* Computing MAX */
/* Computing MIN */
/* Computing MAX */
	d__5 = abs(*wr2);
	d__3 = c4, d__4 = std::max(d__5,c5) * .5;
	d__1 = std::max(*safmin,c1), d__2 = (abs(*wr2) * c2 + c3) *
		1.0000100000000001, d__1 = std::max(d__1,d__2), d__2 = std::min(d__3,
		d__4);
	wsize = std::max(d__1,d__2);
	if (wsize != 1.) {
	    wscale = 1. / wsize;
	    if (wsize > 1.) {
		*scale2 = std::max(ascale,bsize) * wscale * std::min(ascale,bsize);
	    } else {
		*scale2 = std::min(ascale,bsize) * wscale * std::max(ascale,bsize);
	    }
	    *wr2 *= wscale;
	} else {
	    *scale2 = ascale * bsize;
	}
    }

/*     End of DLAG2 */

    return 0;
} /* dlag2_ */

/* Subroutine */ int dlag2s_(integer *m, integer *n, double *a, integer *lda, float *sa, integer *ldsa, integer *info)
{
    /* System generated locals */
    integer sa_dim1, sa_offset, a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    integer i__, j;
    double rmax;


/*  -- LAPACK PROTOTYPE auxiliary routine (version 3.1.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     August 2007 */

/*     .. */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAG2S converts a DOUBLE PRECISION matrix, SA, to a SINGLE */
/*  PRECISION matrix, A. */

/*  RMAX is the overflow for the SINGLE PRECISION arithmetic */
/*  DLAG2S checks that all the entries of A are between -RMAX and */
/*  RMAX. If not the convertion is aborted and a flag is raised. */

/*  This is an auxiliary routine so there is no argument checking. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of lines of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N coefficient matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  SA      (output) REAL array, dimension (LDSA,N) */
/*          On exit, if INFO=0, the M-by-N coefficient matrix SA; if */
/*          INFO>0, the content of SA is unspecified. */

/*  LDSA    (input) INTEGER */
/*          The leading dimension of the array SA.  LDSA >= max(1,M). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          = 1:  an entry of the matrix A is greater than the SINGLE */
/*                PRECISION overflow threshold, in this case, the content */
/*                of SA in exit is unspecified. */

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
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    if (a[i__ + j * a_dim1] < -rmax || a[i__ + j * a_dim1] > rmax) {
		*info = 1;
		goto L30;
	    }
	    sa[i__ + j * sa_dim1] = a[i__ + j * a_dim1];
/* L10: */
	}
/* L20: */
    }
    *info = 0;
L30:
    return 0;

/*     End of DLAG2S */

} /* dlag2s_ */

/* Subroutine */ int dlags2_(bool *upper, double *a1, double *a2,
	double *a3, double *b1, double *b2, double *b3,
	double *csu, double *snu, double *csv, double *snv,
	double *csq, double *snq)
{
    /* System generated locals */
    double d__1;

    /* Local variables */
    double a, b, c__, d__, r__, s1, s2, ua11, ua12, ua21, ua22, vb11,
	    vb12, vb21, vb22, csl, csr, snl, snr, aua11, aua12, aua21, aua22,
	    avb11, avb12, avb21, avb22, ua11r, ua22r, vb11r, vb22r;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAGS2 computes 2-by-2 orthogonal matrices U, V and Q, such */
/*  that if ( UPPER ) then */

/*            U'*A*Q = U'*( A1 A2 )*Q = ( x  0  ) */
/*                        ( 0  A3 )     ( x  x  ) */
/*  and */
/*            V'*B*Q = V'*( B1 B2 )*Q = ( x  0  ) */
/*                        ( 0  B3 )     ( x  x  ) */

/*  or if ( .NOT.UPPER ) then */

/*            U'*A*Q = U'*( A1 0  )*Q = ( x  x  ) */
/*                        ( A2 A3 )     ( 0  x  ) */
/*  and */
/*            V'*B*Q = V'*( B1 0  )*Q = ( x  x  ) */
/*                        ( B2 B3 )     ( 0  x  ) */

/*  The rows of the transformed A and B are parallel, where */

/*    U = (  CSU  SNU ), V = (  CSV SNV ), Q = (  CSQ   SNQ ) */
/*        ( -SNU  CSU )      ( -SNV CSV )      ( -SNQ   CSQ ) */

/*  Z' denotes the transpose of Z. */


/*  Arguments */
/*  ========= */

/*  UPPER   (input) LOGICAL */
/*          = .TRUE.: the input matrices A and B are upper triangular. */
/*          = .FALSE.: the input matrices A and B are lower triangular. */

/*  A1      (input) DOUBLE PRECISION */
/*  A2      (input) DOUBLE PRECISION */
/*  A3      (input) DOUBLE PRECISION */
/*          On entry, A1, A2 and A3 are elements of the input 2-by-2 */
/*          upper (lower) triangular matrix A. */

/*  B1      (input) DOUBLE PRECISION */
/*  B2      (input) DOUBLE PRECISION */
/*  B3      (input) DOUBLE PRECISION */
/*          On entry, B1, B2 and B3 are elements of the input 2-by-2 */
/*          upper (lower) triangular matrix B. */

/*  CSU     (output) DOUBLE PRECISION */
/*  SNU     (output) DOUBLE PRECISION */
/*          The desired orthogonal matrix U. */

/*  CSV     (output) DOUBLE PRECISION */
/*  SNV     (output) DOUBLE PRECISION */
/*          The desired orthogonal matrix V. */

/*  CSQ     (output) DOUBLE PRECISION */
/*  SNQ     (output) DOUBLE PRECISION */
/*          The desired orthogonal matrix Q. */

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

    if (*upper) {

/*        Input matrices A and B are upper triangular matrices */

/*        Form matrix C = A*adj(B) = ( a b ) */
/*                                   ( 0 d ) */

	a = *a1 * *b3;
	d__ = *a3 * *b1;
	b = *a2 * *b1 - *a1 * *b2;

/*        The SVD of real 2-by-2 triangular C */

/*         ( CSL -SNL )*( A B )*(  CSR  SNR ) = ( R 0 ) */
/*         ( SNL  CSL ) ( 0 D ) ( -SNR  CSR )   ( 0 T ) */

	dlasv2_(&a, &b, &d__, &s1, &s2, &snr, &csr, &snl, &csl);

	if (abs(csl) >= abs(snl) || abs(csr) >= abs(snr)) {

/*           Compute the (1,1) and (1,2) elements of U'*A and V'*B, */
/*           and (1,2) element of |U|'*|A| and |V|'*|B|. */

	    ua11r = csl * *a1;
	    ua12 = csl * *a2 + snl * *a3;

	    vb11r = csr * *b1;
	    vb12 = csr * *b2 + snr * *b3;

	    aua12 = abs(csl) * abs(*a2) + abs(snl) * abs(*a3);
	    avb12 = abs(csr) * abs(*b2) + abs(snr) * abs(*b3);

/*           zero (1,2) elements of U'*A and V'*B */

	    if (abs(ua11r) + abs(ua12) != 0.) {
		if (aua12 / (abs(ua11r) + abs(ua12)) <= avb12 / (abs(vb11r) +
			abs(vb12))) {
		    d__1 = -ua11r;
		    dlartg_(&d__1, &ua12, csq, snq, &r__);
		} else {
		    d__1 = -vb11r;
		    dlartg_(&d__1, &vb12, csq, snq, &r__);
		}
	    } else {
		d__1 = -vb11r;
		dlartg_(&d__1, &vb12, csq, snq, &r__);
	    }

	    *csu = csl;
	    *snu = -snl;
	    *csv = csr;
	    *snv = -snr;

	} else {

/*           Compute the (2,1) and (2,2) elements of U'*A and V'*B, */
/*           and (2,2) element of |U|'*|A| and |V|'*|B|. */

	    ua21 = -snl * *a1;
	    ua22 = -snl * *a2 + csl * *a3;

	    vb21 = -snr * *b1;
	    vb22 = -snr * *b2 + csr * *b3;

	    aua22 = abs(snl) * abs(*a2) + abs(csl) * abs(*a3);
	    avb22 = abs(snr) * abs(*b2) + abs(csr) * abs(*b3);

/*           zero (2,2) elements of U'*A and V'*B, and then swap. */

	    if (abs(ua21) + abs(ua22) != 0.) {
		if (aua22 / (abs(ua21) + abs(ua22)) <= avb22 / (abs(vb21) +
			abs(vb22))) {
		    d__1 = -ua21;
		    dlartg_(&d__1, &ua22, csq, snq, &r__);
		} else {
		    d__1 = -vb21;
		    dlartg_(&d__1, &vb22, csq, snq, &r__);
		}
	    } else {
		d__1 = -vb21;
		dlartg_(&d__1, &vb22, csq, snq, &r__);
	    }

	    *csu = snl;
	    *snu = csl;
	    *csv = snr;
	    *snv = csr;

	}

    } else {

/*        Input matrices A and B are lower triangular matrices */

/*        Form matrix C = A*adj(B) = ( a 0 ) */
/*                                   ( c d ) */

	a = *a1 * *b3;
	d__ = *a3 * *b1;
	c__ = *a2 * *b3 - *a3 * *b2;

/*        The SVD of real 2-by-2 triangular C */

/*         ( CSL -SNL )*( A 0 )*(  CSR  SNR ) = ( R 0 ) */
/*         ( SNL  CSL ) ( C D ) ( -SNR  CSR )   ( 0 T ) */

	dlasv2_(&a, &c__, &d__, &s1, &s2, &snr, &csr, &snl, &csl);

	if (abs(csr) >= abs(snr) || abs(csl) >= abs(snl)) {

/*           Compute the (2,1) and (2,2) elements of U'*A and V'*B, */
/*           and (2,1) element of |U|'*|A| and |V|'*|B|. */

	    ua21 = -snr * *a1 + csr * *a2;
	    ua22r = csr * *a3;

	    vb21 = -snl * *b1 + csl * *b2;
	    vb22r = csl * *b3;

	    aua21 = abs(snr) * abs(*a1) + abs(csr) * abs(*a2);
	    avb21 = abs(snl) * abs(*b1) + abs(csl) * abs(*b2);

/*           zero (2,1) elements of U'*A and V'*B. */

	    if (abs(ua21) + abs(ua22r) != 0.) {
		if (aua21 / (abs(ua21) + abs(ua22r)) <= avb21 / (abs(vb21) +
			abs(vb22r))) {
		    dlartg_(&ua22r, &ua21, csq, snq, &r__);
		} else {
		    dlartg_(&vb22r, &vb21, csq, snq, &r__);
		}
	    } else {
		dlartg_(&vb22r, &vb21, csq, snq, &r__);
	    }

	    *csu = csr;
	    *snu = -snr;
	    *csv = csl;
	    *snv = -snl;

	} else {

/*           Compute the (1,1) and (1,2) elements of U'*A and V'*B, */
/*           and (1,1) element of |U|'*|A| and |V|'*|B|. */

	    ua11 = csr * *a1 + snr * *a2;
	    ua12 = snr * *a3;

	    vb11 = csl * *b1 + snl * *b2;
	    vb12 = snl * *b3;

	    aua11 = abs(csr) * abs(*a1) + abs(snr) * abs(*a2);
	    avb11 = abs(csl) * abs(*b1) + abs(snl) * abs(*b2);

/*           zero (1,1) elements of U'*A and V'*B, and then swap. */

	    if (abs(ua11) + abs(ua12) != 0.) {
		if (aua11 / (abs(ua11) + abs(ua12)) <= avb11 / (abs(vb11) +
			abs(vb12))) {
		    dlartg_(&ua12, &ua11, csq, snq, &r__);
		} else {
		    dlartg_(&vb12, &vb11, csq, snq, &r__);
		}
	    } else {
		dlartg_(&vb12, &vb11, csq, snq, &r__);
	    }

	    *csu = snr;
	    *snu = csr;
	    *csv = snl;
	    *snv = csl;

	}

    }

    return 0;

/*     End of DLAGS2 */

} /* dlags2_ */

/* Subroutine */ int dlagtf_(integer *n, double *a, double *lambda,
	double *b, double *c__, double *tol, double *d__,
	integer *in, integer *info)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2;

    /* Local variables */
    integer k;
    double tl, eps, piv1, piv2, temp, mult, scale1, scale2;




/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAGTF factorizes the matrix (T - lambda*I), where T is an n by n */
/*  tridiagonal matrix and lambda is a scalar, as */

/*     T - lambda*I = PLU, */

/*  where P is a permutation matrix, L is a unit lower tridiagonal matrix */
/*  with at most one non-zero sub-diagonal elements per column and U is */
/*  an upper triangular matrix with at most two non-zero super-diagonal */
/*  elements per column. */

/*  The factorization is obtained by Gaussian elimination with partial */
/*  pivoting and implicit row scaling. */

/*  The parameter LAMBDA is included in the routine so that DLAGTF may */
/*  be used, in conjunction with DLAGTS, to obtain eigenvectors of T by */
/*  inverse iteration. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix T. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, A must contain the diagonal elements of T. */

/*          On exit, A is overwritten by the n diagonal elements of the */
/*          upper triangular matrix U of the factorization of T. */

/*  LAMBDA  (input) DOUBLE PRECISION */
/*          On entry, the scalar lambda. */

/*  B       (input/output) DOUBLE PRECISION array, dimension (N-1) */
/*          On entry, B must contain the (n-1) super-diagonal elements of */
/*          T. */

/*          On exit, B is overwritten by the (n-1) super-diagonal */
/*          elements of the matrix U of the factorization of T. */

/*  C       (input/output) DOUBLE PRECISION array, dimension (N-1) */
/*          On entry, C must contain the (n-1) sub-diagonal elements of */
/*          T. */

/*          On exit, C is overwritten by the (n-1) sub-diagonal elements */
/*          of the matrix L of the factorization of T. */

/*  TOL     (input) DOUBLE PRECISION */
/*          On entry, a relative tolerance used to indicate whether or */
/*          not the matrix (T - lambda*I) is nearly singular. TOL should */
/*          normally be chose as approximately the largest relative error */
/*          in the elements of T. For example, if the elements of T are */
/*          correct to about 4 significant figures, then TOL should be */
/*          set to about 5*10**(-4). If TOL is supplied as less than eps, */
/*          where eps is the relative machine precision, then the value */
/*          eps is used in place of TOL. */

/*  D       (output) DOUBLE PRECISION array, dimension (N-2) */
/*          On exit, D is overwritten by the (n-2) second super-diagonal */
/*          elements of the matrix U of the factorization of T. */

/*  IN      (output) INTEGER array, dimension (N) */
/*          On exit, IN contains details of the permutation matrix P. If */
/*          an interchange occurred at the kth step of the elimination, */
/*          then IN(k) = 1, otherwise IN(k) = 0. The element IN(n) */
/*          returns the smallest positive integer j such that */

/*             abs( u(j,j) ).le. norm( (T - lambda*I)(j) )*TOL, */

/*          where norm( A(j) ) denotes the sum of the absolute values of */
/*          the jth row of the matrix A. If no such j exists then IN(n) */
/*          is returned as zero. If IN(n) is returned as positive, then a */
/*          diagonal element of U is small, indicating that */
/*          (T - lambda*I) is singular or nearly singular, */

/*  INFO    (output) INTEGER */
/*          = 0   : successful exit */
/*          .lt. 0: if INFO = -k, the kth argument had an illegal value */

/* ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --in;
    --d__;
    --c__;
    --b;
    --a;

    /* Function Body */
    *info = 0;
    if (*n < 0) {
	*info = -1;
	i__1 = -(*info);
	xerbla_("DLAGTF", &i__1);
	return 0;
    }

    if (*n == 0) {
	return 0;
    }

    a[1] -= *lambda;
    in[*n] = 0;
    if (*n == 1) {
	if (a[1] == 0.) {
	    in[1] = 1;
	}
	return 0;
    }

    eps = dlamch_("Epsilon");

    tl = std::max(*tol,eps);
    scale1 = abs(a[1]) + abs(b[1]);
    i__1 = *n - 1;
    for (k = 1; k <= i__1; ++k) {
	a[k + 1] -= *lambda;
	scale2 = (d__1 = c__[k], abs(d__1)) + (d__2 = a[k + 1], abs(d__2));
	if (k < *n - 1) {
	    scale2 += (d__1 = b[k + 1], abs(d__1));
	}
	if (a[k] == 0.) {
	    piv1 = 0.;
	} else {
	    piv1 = (d__1 = a[k], abs(d__1)) / scale1;
	}
	if (c__[k] == 0.) {
	    in[k] = 0;
	    piv2 = 0.;
	    scale1 = scale2;
	    if (k < *n - 1) {
		d__[k] = 0.;
	    }
	} else {
	    piv2 = (d__1 = c__[k], abs(d__1)) / scale2;
	    if (piv2 <= piv1) {
		in[k] = 0;
		scale1 = scale2;
		c__[k] /= a[k];
		a[k + 1] -= c__[k] * b[k];
		if (k < *n - 1) {
		    d__[k] = 0.;
		}
	    } else {
		in[k] = 1;
		mult = a[k] / c__[k];
		a[k] = c__[k];
		temp = a[k + 1];
		a[k + 1] = b[k] - mult * temp;
		if (k < *n - 1) {
		    d__[k] = b[k + 1];
		    b[k + 1] = -mult * d__[k];
		}
		b[k] = temp;
		c__[k] = mult;
	    }
	}
	if (std::max(piv1,piv2) <= tl && in[*n] == 0) {
	    in[*n] = k;
	}
/* L10: */
    }
    if ((d__1 = a[*n], abs(d__1)) <= scale1 * tl && in[*n] == 0) {
	in[*n] = *n;
    }

    return 0;

/*     End of DLAGTF */

} /* dlagtf_ */

/* Subroutine */ int dlagtm_(const char *trans, integer *n, integer *nrhs,
	double *alpha, double *dl, double *d__, double *du,
	double *x, integer *ldx, double *beta, double *b, integer
	*ldb)
{
    /* System generated locals */
    integer b_dim1, b_offset, x_dim1, x_offset, i__1, i__2;

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

/*  DLAGTM performs a matrix-vector product of the form */

/*     B := alpha * A * X + beta * B */

/*  where A is a tridiagonal matrix of order N, B and X are N by NRHS */
/*  matrices, and alpha and beta are real scalars, each of which may be */
/*  0., 1., or -1. */

/*  Arguments */
/*  ========= */

/*  TRANS   (input) CHARACTER*1 */
/*          Specifies the operation applied to A. */
/*          = 'N':  No transpose, B := alpha * A * X + beta * B */
/*          = 'T':  Transpose,    B := alpha * A'* X + beta * B */
/*          = 'C':  Conjugate transpose = Transpose */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  NRHS    (input) INTEGER */
/*          The number of right hand sides, i.e., the number of columns */
/*          of the matrices X and B. */

/*  ALPHA   (input) DOUBLE PRECISION */
/*          The scalar alpha.  ALPHA must be 0., 1., or -1.; otherwise, */
/*          it is assumed to be 0. */

/*  DL      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) sub-diagonal elements of T. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal elements of T. */

/*  DU      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) super-diagonal elements of T. */

/*  X       (input) DOUBLE PRECISION array, dimension (LDX,NRHS) */
/*          The N by NRHS matrix X. */
/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X.  LDX >= max(N,1). */

/*  BETA    (input) DOUBLE PRECISION */
/*          The scalar beta.  BETA must be 0., 1., or -1.; otherwise, */
/*          it is assumed to be 1. */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*          On entry, the N by NRHS matrix B. */
/*          On exit, B is overwritten by the matrix expression */
/*          B := alpha * A * X + beta * B. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B.  LDB >= max(N,1). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --dl;
    --d__;
    --du;
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;

    /* Function Body */
    if (*n == 0) {
	return 0;
    }

/*     Multiply B by BETA if BETA.NE.1. */

    if (*beta == 0.) {
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		b[i__ + j * b_dim1] = 0.;
/* L10: */
	    }
/* L20: */
	}
    } else if (*beta == -1.) {
	i__1 = *nrhs;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *n;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		b[i__ + j * b_dim1] = -b[i__ + j * b_dim1];
/* L30: */
	    }
/* L40: */
	}
    }

    if (*alpha == 1.) {
	if (lsame_(trans, "N")) {

/*           Compute B := B + A*X */

	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		if (*n == 1) {
		    b[j * b_dim1 + 1] += d__[1] * x[j * x_dim1 + 1];
		} else {
		    b[j * b_dim1 + 1] = b[j * b_dim1 + 1] + d__[1] * x[j *
			    x_dim1 + 1] + du[1] * x[j * x_dim1 + 2];
		    b[*n + j * b_dim1] = b[*n + j * b_dim1] + dl[*n - 1] * x[*
			    n - 1 + j * x_dim1] + d__[*n] * x[*n + j * x_dim1]
			    ;
		    i__2 = *n - 1;
		    for (i__ = 2; i__ <= i__2; ++i__) {
			b[i__ + j * b_dim1] = b[i__ + j * b_dim1] + dl[i__ -
				1] * x[i__ - 1 + j * x_dim1] + d__[i__] * x[
				i__ + j * x_dim1] + du[i__] * x[i__ + 1 + j *
				x_dim1];
/* L50: */
		    }
		}
/* L60: */
	    }
	} else {

/*           Compute B := B + A'*X */

	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		if (*n == 1) {
		    b[j * b_dim1 + 1] += d__[1] * x[j * x_dim1 + 1];
		} else {
		    b[j * b_dim1 + 1] = b[j * b_dim1 + 1] + d__[1] * x[j *
			    x_dim1 + 1] + dl[1] * x[j * x_dim1 + 2];
		    b[*n + j * b_dim1] = b[*n + j * b_dim1] + du[*n - 1] * x[*
			    n - 1 + j * x_dim1] + d__[*n] * x[*n + j * x_dim1]
			    ;
		    i__2 = *n - 1;
		    for (i__ = 2; i__ <= i__2; ++i__) {
			b[i__ + j * b_dim1] = b[i__ + j * b_dim1] + du[i__ -
				1] * x[i__ - 1 + j * x_dim1] + d__[i__] * x[
				i__ + j * x_dim1] + dl[i__] * x[i__ + 1 + j *
				x_dim1];
/* L70: */
		    }
		}
/* L80: */
	    }
	}
    } else if (*alpha == -1.) {
	if (lsame_(trans, "N")) {

/*           Compute B := B - A*X */

	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		if (*n == 1) {
		    b[j * b_dim1 + 1] -= d__[1] * x[j * x_dim1 + 1];
		} else {
		    b[j * b_dim1 + 1] = b[j * b_dim1 + 1] - d__[1] * x[j *
			    x_dim1 + 1] - du[1] * x[j * x_dim1 + 2];
		    b[*n + j * b_dim1] = b[*n + j * b_dim1] - dl[*n - 1] * x[*
			    n - 1 + j * x_dim1] - d__[*n] * x[*n + j * x_dim1]
			    ;
		    i__2 = *n - 1;
		    for (i__ = 2; i__ <= i__2; ++i__) {
			b[i__ + j * b_dim1] = b[i__ + j * b_dim1] - dl[i__ -
				1] * x[i__ - 1 + j * x_dim1] - d__[i__] * x[
				i__ + j * x_dim1] - du[i__] * x[i__ + 1 + j *
				x_dim1];
/* L90: */
		    }
		}
/* L100: */
	    }
	} else {

/*           Compute B := B - A'*X */

	    i__1 = *nrhs;
	    for (j = 1; j <= i__1; ++j) {
		if (*n == 1) {
		    b[j * b_dim1 + 1] -= d__[1] * x[j * x_dim1 + 1];
		} else {
		    b[j * b_dim1 + 1] = b[j * b_dim1 + 1] - d__[1] * x[j *
			    x_dim1 + 1] - dl[1] * x[j * x_dim1 + 2];
		    b[*n + j * b_dim1] = b[*n + j * b_dim1] - du[*n - 1] * x[*
			    n - 1 + j * x_dim1] - d__[*n] * x[*n + j * x_dim1]
			    ;
		    i__2 = *n - 1;
		    for (i__ = 2; i__ <= i__2; ++i__) {
			b[i__ + j * b_dim1] = b[i__ + j * b_dim1] - du[i__ -
				1] * x[i__ - 1 + j * x_dim1] - d__[i__] * x[
				i__ + j * x_dim1] - dl[i__] * x[i__ + 1 + j *
				x_dim1];
/* L110: */
		    }
		}
/* L120: */
	    }
	}
    }
    return 0;

/*     End of DLAGTM */

} /* dlagtm_ */

/* Subroutine */ int dlagts_(integer *job, integer *n, double *a,
	double *b, double *c__, double *d__, integer *in,
	double *y, double *tol, integer *info)
{
    /* System generated locals */
    integer i__1;
    double d__1, d__2, d__3, d__4, d__5;

    /* Local variables */
    integer k;
    double ak, eps, temp, pert, absak, sfmin;


    double bignum;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAGTS may be used to solve one of the systems of equations */

/*     (T - lambda*I)*x = y   or   (T - lambda*I)'*x = y, */

/*  where T is an n by n tridiagonal matrix, for x, following the */
/*  factorization of (T - lambda*I) as */

/*     (T - lambda*I) = P*L*U , */

/*  by routine DLAGTF. The choice of equation to be solved is */
/*  controlled by the argument JOB, and in each case there is an option */
/*  to perturb zero or very small diagonal elements of U, this option */
/*  being intended for use in applications such as inverse iteration. */

/*  Arguments */
/*  ========= */

/*  JOB     (input) INTEGER */
/*          Specifies the job to be performed by DLAGTS as follows: */
/*          =  1: The equations  (T - lambda*I)x = y  are to be solved, */
/*                but diagonal elements of U are not to be perturbed. */
/*          = -1: The equations  (T - lambda*I)x = y  are to be solved */
/*                and, if overflow would otherwise occur, the diagonal */
/*                elements of U are to be perturbed. See argument TOL */
/*                below. */
/*          =  2: The equations  (T - lambda*I)'x = y  are to be solved, */
/*                but diagonal elements of U are not to be perturbed. */
/*          = -2: The equations  (T - lambda*I)'x = y  are to be solved */
/*                and, if overflow would otherwise occur, the diagonal */
/*                elements of U are to be perturbed. See argument TOL */
/*                below. */

/*  N       (input) INTEGER */
/*          The order of the matrix T. */

/*  A       (input) DOUBLE PRECISION array, dimension (N) */
/*          On entry, A must contain the diagonal elements of U as */
/*          returned from DLAGTF. */

/*  B       (input) DOUBLE PRECISION array, dimension (N-1) */
/*          On entry, B must contain the first super-diagonal elements of */
/*          U as returned from DLAGTF. */

/*  C       (input) DOUBLE PRECISION array, dimension (N-1) */
/*          On entry, C must contain the sub-diagonal elements of L as */
/*          returned from DLAGTF. */

/*  D       (input) DOUBLE PRECISION array, dimension (N-2) */
/*          On entry, D must contain the second super-diagonal elements */
/*          of U as returned from DLAGTF. */

/*  IN      (input) INTEGER array, dimension (N) */
/*          On entry, IN must contain details of the matrix P as returned */
/*          from DLAGTF. */

/*  Y       (input/output) DOUBLE PRECISION array, dimension (N) */
/*          On entry, the right hand side vector y. */
/*          On exit, Y is overwritten by the solution vector x. */

/*  TOL     (input/output) DOUBLE PRECISION */
/*          On entry, with  JOB .lt. 0, TOL should be the minimum */
/*          perturbation to be made to very small diagonal elements of U. */
/*          TOL should normally be chosen as about eps*norm(U), where eps */
/*          is the relative machine precision, but if TOL is supplied as */
/*          non-positive, then it is reset to eps*max( abs( u(i,j) ) ). */
/*          If  JOB .gt. 0  then TOL is not referenced. */

/*          On exit, TOL is changed as described above, only if TOL is */
/*          non-positive on entry. Otherwise TOL is unchanged. */

/*  INFO    (output) INTEGER */
/*          = 0   : successful exit */
/*          .lt. 0: if INFO = -i, the i-th argument had an illegal value */
/*          .gt. 0: overflow would occur when computing the INFO(th) */
/*                  element of the solution vector x. This can only occur */
/*                  when JOB is supplied as positive and either means */
/*                  that a diagonal element of U is very small, or that */
/*                  the elements of the right-hand side vector y are very */
/*                  large. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --y;
    --in;
    --d__;
    --c__;
    --b;
    --a;

    /* Function Body */
    *info = 0;
    if (abs(*job) > 2 || *job == 0) {
	*info = -1;
    } else if (*n < 0) {
	*info = -2;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLAGTS", &i__1);
	return 0;
    }

    if (*n == 0) {
	return 0;
    }

    eps = dlamch_("Epsilon");
    sfmin = dlamch_("Safe minimum");
    bignum = 1. / sfmin;

    if (*job < 0) {
	if (*tol <= 0.) {
	    *tol = abs(a[1]);
	    if (*n > 1) {
/* Computing MAX */
		d__1 = *tol, d__2 = abs(a[2]), d__1 = std::max(d__1,d__2), d__2 =
			abs(b[1]);
		*tol = std::max(d__1,d__2);
	    }
	    i__1 = *n;
	    for (k = 3; k <= i__1; ++k) {
/* Computing MAX */
		d__4 = *tol, d__5 = (d__1 = a[k], abs(d__1)), d__4 = std::max(d__4,
			d__5), d__5 = (d__2 = b[k - 1], abs(d__2)), d__4 =
			std::max(d__4,d__5), d__5 = (d__3 = d__[k - 2], abs(d__3));
		*tol = std::max(d__4,d__5);
/* L10: */
	    }
	    *tol *= eps;
	    if (*tol == 0.) {
		*tol = eps;
	    }
	}
    }

    if (abs(*job) == 1) {
	i__1 = *n;
	for (k = 2; k <= i__1; ++k) {
	    if (in[k - 1] == 0) {
		y[k] -= c__[k - 1] * y[k - 1];
	    } else {
		temp = y[k - 1];
		y[k - 1] = y[k];
		y[k] = temp - c__[k - 1] * y[k];
	    }
/* L20: */
	}
	if (*job == 1) {
	    for (k = *n; k >= 1; --k) {
		if (k <= *n - 2) {
		    temp = y[k] - b[k] * y[k + 1] - d__[k] * y[k + 2];
		} else if (k == *n - 1) {
		    temp = y[k] - b[k] * y[k + 1];
		} else {
		    temp = y[k];
		}
		ak = a[k];
		absak = abs(ak);
		if (absak < 1.) {
		    if (absak < sfmin) {
			if (absak == 0. || abs(temp) * sfmin > absak) {
			    *info = k;
			    return 0;
			} else {
			    temp *= bignum;
			    ak *= bignum;
			}
		    } else if (abs(temp) > absak * bignum) {
			*info = k;
			return 0;
		    }
		}
		y[k] = temp / ak;
/* L30: */
	    }
	} else {
	    for (k = *n; k >= 1; --k) {
		if (k <= *n - 2) {
		    temp = y[k] - b[k] * y[k + 1] - d__[k] * y[k + 2];
		} else if (k == *n - 1) {
		    temp = y[k] - b[k] * y[k + 1];
		} else {
		    temp = y[k];
		}
		ak = a[k];
		pert = d_sign(tol, &ak);
L40:
		absak = abs(ak);
		if (absak < 1.) {
		    if (absak < sfmin) {
			if (absak == 0. || abs(temp) * sfmin > absak) {
			    ak += pert;
			    pert *= 2;
			    goto L40;
			} else {
			    temp *= bignum;
			    ak *= bignum;
			}
		    } else if (abs(temp) > absak * bignum) {
			ak += pert;
			pert *= 2;
			goto L40;
		    }
		}
		y[k] = temp / ak;
/* L50: */
	    }
	}
    } else {

/*        Come to here if  JOB = 2 or -2 */

	if (*job == 2) {
	    i__1 = *n;
	    for (k = 1; k <= i__1; ++k) {
		if (k >= 3) {
		    temp = y[k] - b[k - 1] * y[k - 1] - d__[k - 2] * y[k - 2];
		} else if (k == 2) {
		    temp = y[k] - b[k - 1] * y[k - 1];
		} else {
		    temp = y[k];
		}
		ak = a[k];
		absak = abs(ak);
		if (absak < 1.) {
		    if (absak < sfmin) {
			if (absak == 0. || abs(temp) * sfmin > absak) {
			    *info = k;
			    return 0;
			} else {
			    temp *= bignum;
			    ak *= bignum;
			}
		    } else if (abs(temp) > absak * bignum) {
			*info = k;
			return 0;
		    }
		}
		y[k] = temp / ak;
/* L60: */
	    }
	} else {
	    i__1 = *n;
	    for (k = 1; k <= i__1; ++k) {
		if (k >= 3) {
		    temp = y[k] - b[k - 1] * y[k - 1] - d__[k - 2] * y[k - 2];
		} else if (k == 2) {
		    temp = y[k] - b[k - 1] * y[k - 1];
		} else {
		    temp = y[k];
		}
		ak = a[k];
		pert = d_sign(tol, &ak);
L70:
		absak = abs(ak);
		if (absak < 1.) {
		    if (absak < sfmin) {
			if (absak == 0. || abs(temp) * sfmin > absak) {
			    ak += pert;
			    pert *= 2;
			    goto L70;
			} else {
			    temp *= bignum;
			    ak *= bignum;
			}
		    } else if (abs(temp) > absak * bignum) {
			ak += pert;
			pert *= 2;
			goto L70;
		    }
		}
		y[k] = temp / ak;
/* L80: */
	    }
	}

	for (k = *n; k >= 2; --k) {
	    if (in[k - 1] == 0) {
		y[k - 1] -= c__[k - 1] * y[k];
	    } else {
		temp = y[k - 1];
		y[k - 1] = y[k];
		y[k] = temp - c__[k - 1] * y[k];
	    }
/* L90: */
	}
    }

/*     End of DLAGTS */

    return 0;
} /* dlagts_ */

/* Subroutine */ int dlagv2_(double *a, integer *lda, double *b,
	integer *ldb, double *alphar, double *alphai, double *
	beta, double *csl, double *snl, double *csr, double *
	snr)
{
	/* Table of constant values */
	static integer c__2 = 2;
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset;
    double d__1, d__2, d__3, d__4, d__5, d__6;

    /* Local variables */
    double r__, t, h1, h2, h3, wi, qq, rr, wr1, wr2, ulp;
    double anorm, bnorm, scale1, scale2;
    double ascale, bscale;
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

/*  DLAGV2 computes the Generalized Schur factorization of a real 2-by-2 */
/*  matrix pencil (A,B) where B is upper triangular. This routine */
/*  computes orthogonal (rotation) matrices given by CSL, SNL and CSR, */
/*  SNR such that */

/*  1) if the pencil (A,B) has two real eigenvalues (include 0/0 or 1/0 */
/*     types), then */

/*     [ a11 a12 ] := [  CSL  SNL ] [ a11 a12 ] [  CSR -SNR ] */
/*     [  0  a22 ]    [ -SNL  CSL ] [ a21 a22 ] [  SNR  CSR ] */

/*     [ b11 b12 ] := [  CSL  SNL ] [ b11 b12 ] [  CSR -SNR ] */
/*     [  0  b22 ]    [ -SNL  CSL ] [  0  b22 ] [  SNR  CSR ], */

/*  2) if the pencil (A,B) has a pair of complex conjugate eigenvalues, */
/*     then */

/*     [ a11 a12 ] := [  CSL  SNL ] [ a11 a12 ] [  CSR -SNR ] */
/*     [ a21 a22 ]    [ -SNL  CSL ] [ a21 a22 ] [  SNR  CSR ] */

/*     [ b11  0  ] := [  CSL  SNL ] [ b11 b12 ] [  CSR -SNR ] */
/*     [  0  b22 ]    [ -SNL  CSL ] [  0  b22 ] [  SNR  CSR ] */

/*     where b11 >= b22 > 0. */


/*  Arguments */
/*  ========= */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, 2) */
/*          On entry, the 2 x 2 matrix A. */
/*          On exit, A is overwritten by the ``A-part'' of the */
/*          generalized Schur form. */

/*  LDA     (input) INTEGER */
/*          THe leading dimension of the array A.  LDA >= 2. */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB, 2) */
/*          On entry, the upper triangular 2 x 2 matrix B. */
/*          On exit, B is overwritten by the ``B-part'' of the */
/*          generalized Schur form. */

/*  LDB     (input) INTEGER */
/*          THe leading dimension of the array B.  LDB >= 2. */

/*  ALPHAR  (output) DOUBLE PRECISION array, dimension (2) */
/*  ALPHAI  (output) DOUBLE PRECISION array, dimension (2) */
/*  BETA    (output) DOUBLE PRECISION array, dimension (2) */
/*          (ALPHAR(k)+i*ALPHAI(k))/BETA(k) are the eigenvalues of the */
/*          pencil (A,B), k=1,2, i = sqrt(-1).  Note that BETA(k) may */
/*          be zero. */

/*  CSL     (output) DOUBLE PRECISION */
/*          The cosine of the left rotation matrix. */

/*  SNL     (output) DOUBLE PRECISION */
/*          The sine of the left rotation matrix. */

/*  CSR     (output) DOUBLE PRECISION */
/*          The cosine of the right rotation matrix. */

/*  SNR     (output) DOUBLE PRECISION */
/*          The sine of the right rotation matrix. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Mark Fahey, Department of Mathematics, Univ. of Kentucky, USA */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --alphar;
    --alphai;
    --beta;

    /* Function Body */
    safmin = dlamch_("S");
    ulp = dlamch_("P");

/*     Scale A */

/* Computing MAX */
    d__5 = (d__1 = a[a_dim1 + 1], abs(d__1)) + (d__2 = a[a_dim1 + 2], abs(
	    d__2)), d__6 = (d__3 = a[(a_dim1 << 1) + 1], abs(d__3)) + (d__4 =
	    a[(a_dim1 << 1) + 2], abs(d__4)), d__5 = std::max(d__5,d__6);
    anorm = std::max(d__5,safmin);
    ascale = 1. / anorm;
    a[a_dim1 + 1] = ascale * a[a_dim1 + 1];
    a[(a_dim1 << 1) + 1] = ascale * a[(a_dim1 << 1) + 1];
    a[a_dim1 + 2] = ascale * a[a_dim1 + 2];
    a[(a_dim1 << 1) + 2] = ascale * a[(a_dim1 << 1) + 2];

/*     Scale B */

/* Computing MAX */
    d__4 = (d__3 = b[b_dim1 + 1], abs(d__3)), d__5 = (d__1 = b[(b_dim1 << 1)
	    + 1], abs(d__1)) + (d__2 = b[(b_dim1 << 1) + 2], abs(d__2)), d__4
	    = std::max(d__4,d__5);
    bnorm = std::max(d__4,safmin);
    bscale = 1. / bnorm;
    b[b_dim1 + 1] = bscale * b[b_dim1 + 1];
    b[(b_dim1 << 1) + 1] = bscale * b[(b_dim1 << 1) + 1];
    b[(b_dim1 << 1) + 2] = bscale * b[(b_dim1 << 1) + 2];

/*     Check if A can be deflated */

    if ((d__1 = a[a_dim1 + 2], abs(d__1)) <= ulp) {
	*csl = 1.;
	*snl = 0.;
	*csr = 1.;
	*snr = 0.;
	a[a_dim1 + 2] = 0.;
	b[b_dim1 + 2] = 0.;

/*     Check if B is singular */

    } else if ((d__1 = b[b_dim1 + 1], abs(d__1)) <= ulp) {
	dlartg_(&a[a_dim1 + 1], &a[a_dim1 + 2], csl, snl, &r__);
	*csr = 1.;
	*snr = 0.;
	drot_(&c__2, &a[a_dim1 + 1], lda, &a[a_dim1 + 2], lda, csl, snl);
	drot_(&c__2, &b[b_dim1 + 1], ldb, &b[b_dim1 + 2], ldb, csl, snl);
	a[a_dim1 + 2] = 0.;
	b[b_dim1 + 1] = 0.;
	b[b_dim1 + 2] = 0.;

    } else if ((d__1 = b[(b_dim1 << 1) + 2], abs(d__1)) <= ulp) {
	dlartg_(&a[(a_dim1 << 1) + 2], &a[a_dim1 + 2], csr, snr, &t);
	*snr = -(*snr);
	drot_(&c__2, &a[a_dim1 + 1], &c__1, &a[(a_dim1 << 1) + 1], &c__1, csr,
		 snr);
	drot_(&c__2, &b[b_dim1 + 1], &c__1, &b[(b_dim1 << 1) + 1], &c__1, csr,
		 snr);
	*csl = 1.;
	*snl = 0.;
	a[a_dim1 + 2] = 0.;
	b[b_dim1 + 2] = 0.;
	b[(b_dim1 << 1) + 2] = 0.;

    } else {

/*        B is nonsingular, first compute the eigenvalues of (A,B) */

	dlag2_(&a[a_offset], lda, &b[b_offset], ldb, &safmin, &scale1, &
		scale2, &wr1, &wr2, &wi);

	if (wi == 0.) {

/*           two real eigenvalues, compute s*A-w*B */

	    h1 = scale1 * a[a_dim1 + 1] - wr1 * b[b_dim1 + 1];
	    h2 = scale1 * a[(a_dim1 << 1) + 1] - wr1 * b[(b_dim1 << 1) + 1];
	    h3 = scale1 * a[(a_dim1 << 1) + 2] - wr1 * b[(b_dim1 << 1) + 2];

	    rr = dlapy2_(&h1, &h2);
	    d__1 = scale1 * a[a_dim1 + 2];
	    qq = dlapy2_(&d__1, &h3);

	    if (rr > qq) {

/*              find right rotation matrix to zero 1,1 element of */
/*              (sA - wB) */

		dlartg_(&h2, &h1, csr, snr, &t);

	    } else {

/*              find right rotation matrix to zero 2,1 element of */
/*              (sA - wB) */

		d__1 = scale1 * a[a_dim1 + 2];
		dlartg_(&h3, &d__1, csr, snr, &t);

	    }

	    *snr = -(*snr);
	    drot_(&c__2, &a[a_dim1 + 1], &c__1, &a[(a_dim1 << 1) + 1], &c__1,
		    csr, snr);
	    drot_(&c__2, &b[b_dim1 + 1], &c__1, &b[(b_dim1 << 1) + 1], &c__1,
		    csr, snr);

/*           compute inf norms of A and B */

/* Computing MAX */
	    d__5 = (d__1 = a[a_dim1 + 1], abs(d__1)) + (d__2 = a[(a_dim1 << 1)
		     + 1], abs(d__2)), d__6 = (d__3 = a[a_dim1 + 2], abs(d__3)
		    ) + (d__4 = a[(a_dim1 << 1) + 2], abs(d__4));
	    h1 = std::max(d__5,d__6);
/* Computing MAX */
	    d__5 = (d__1 = b[b_dim1 + 1], abs(d__1)) + (d__2 = b[(b_dim1 << 1)
		     + 1], abs(d__2)), d__6 = (d__3 = b[b_dim1 + 2], abs(d__3)
		    ) + (d__4 = b[(b_dim1 << 1) + 2], abs(d__4));
	    h2 = std::max(d__5,d__6);

	    if (scale1 * h1 >= abs(wr1) * h2) {

/*              find left rotation matrix Q to zero out B(2,1) */

		dlartg_(&b[b_dim1 + 1], &b[b_dim1 + 2], csl, snl, &r__);

	    } else {

/*              find left rotation matrix Q to zero out A(2,1) */

		dlartg_(&a[a_dim1 + 1], &a[a_dim1 + 2], csl, snl, &r__);

	    }

	    drot_(&c__2, &a[a_dim1 + 1], lda, &a[a_dim1 + 2], lda, csl, snl);
	    drot_(&c__2, &b[b_dim1 + 1], ldb, &b[b_dim1 + 2], ldb, csl, snl);

	    a[a_dim1 + 2] = 0.;
	    b[b_dim1 + 2] = 0.;

	} else {

/*           a pair of complex conjugate eigenvalues */
/*           first compute the SVD of the matrix B */

	    dlasv2_(&b[b_dim1 + 1], &b[(b_dim1 << 1) + 1], &b[(b_dim1 << 1) +
		    2], &r__, &t, snr, csr, snl, csl);

/*           Form (A,B) := Q(A,B)Z' where Q is left rotation matrix and */
/*           Z is right rotation matrix computed from DLASV2 */

	    drot_(&c__2, &a[a_dim1 + 1], lda, &a[a_dim1 + 2], lda, csl, snl);
	    drot_(&c__2, &b[b_dim1 + 1], ldb, &b[b_dim1 + 2], ldb, csl, snl);
	    drot_(&c__2, &a[a_dim1 + 1], &c__1, &a[(a_dim1 << 1) + 1], &c__1,
		    csr, snr);
	    drot_(&c__2, &b[b_dim1 + 1], &c__1, &b[(b_dim1 << 1) + 1], &c__1,
		    csr, snr);

	    b[b_dim1 + 2] = 0.;
	    b[(b_dim1 << 1) + 1] = 0.;

	}

    }

/*     Unscaling */

    a[a_dim1 + 1] = anorm * a[a_dim1 + 1];
    a[a_dim1 + 2] = anorm * a[a_dim1 + 2];
    a[(a_dim1 << 1) + 1] = anorm * a[(a_dim1 << 1) + 1];
    a[(a_dim1 << 1) + 2] = anorm * a[(a_dim1 << 1) + 2];
    b[b_dim1 + 1] = bnorm * b[b_dim1 + 1];
    b[b_dim1 + 2] = bnorm * b[b_dim1 + 2];
    b[(b_dim1 << 1) + 1] = bnorm * b[(b_dim1 << 1) + 1];
    b[(b_dim1 << 1) + 2] = bnorm * b[(b_dim1 << 1) + 2];

    if (wi == 0.) {
	alphar[1] = a[a_dim1 + 1];
	alphar[2] = a[(a_dim1 << 1) + 2];
	alphai[1] = 0.;
	alphai[2] = 0.;
	beta[1] = b[b_dim1 + 1];
	beta[2] = b[(b_dim1 << 1) + 2];
    } else {
	alphar[1] = anorm * wr1 / scale1 / bnorm;
	alphai[1] = anorm * wi / scale1 / bnorm;
	alphar[2] = alphar[1];
	alphai[2] = -alphai[1];
	beta[1] = 1.;
	beta[2] = 1.;
    }

    return 0;

/*     End of DLAGV2 */

} /* dlagv2_ */

/* Subroutine */ int dlahqr_(bool *wantt, bool *wantz, integer *n,
	integer *ilo, integer *ihi, double *h__, integer *ldh, double
	*wr, double *wi, integer *iloz, integer *ihiz, double *z__,
	integer *ldz, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer h_dim1, h_offset, z_dim1, z_offset, i__1, i__2, i__3;
    double d__1, d__2, d__3, d__4;

    /* Local variables */
    integer i__, j, k, l, m;
    double s, v[3];
    integer i1, i2;
    double t1, t2, t3, v2, v3, aa, ab, ba, bb, h11, h12, h21, h22, cs;
    integer nh;
    double sn;
    integer nr;
    double tr;
    integer nz;
    double det, h21s;
    integer its;
    double ulp, sum, tst, rt1i, rt2i, rt1r, rt2r;
	double safmin, safmax, rtdisc, smlnum;


/*  -- LAPACK auxiliary routine (version 3.2.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*     Purpose */
/*     ======= */

/*     DLAHQR is an auxiliary routine called by DHSEQR to update the */
/*     eigenvalues and Schur decomposition already computed by DHSEQR, by */
/*     dealing with the Hessenberg submatrix in rows and columns ILO to */
/*     IHI. */

/*     Arguments */
/*     ========= */

/*     WANTT   (input) LOGICAL */
/*          = .TRUE. : the full Schur form T is required; */
/*          = .FALSE.: only eigenvalues are required. */

/*     WANTZ   (input) LOGICAL */
/*          = .TRUE. : the matrix of Schur vectors Z is required; */
/*          = .FALSE.: Schur vectors are not required. */

/*     N       (input) INTEGER */
/*          The order of the matrix H.  N >= 0. */

/*     ILO     (input) INTEGER */
/*     IHI     (input) INTEGER */
/*          It is assumed that H is already upper quasi-triangular in */
/*          rows and columns IHI+1:N, and that H(ILO,ILO-1) = 0 (unless */
/*          ILO = 1). DLAHQR works primarily with the Hessenberg */
/*          submatrix in rows and columns ILO to IHI, but applies */
/*          transformations to all of H if WANTT is .TRUE.. */
/*          1 <= ILO <= max(1,IHI); IHI <= N. */

/*     H       (input/output) DOUBLE PRECISION array, dimension (LDH,N) */
/*          On entry, the upper Hessenberg matrix H. */
/*          On exit, if INFO is zero and if WANTT is .TRUE., H is upper */
/*          quasi-triangular in rows and columns ILO:IHI, with any */
/*          2-by-2 diagonal blocks in standard form. If INFO is zero */
/*          and WANTT is .FALSE., the contents of H are unspecified on */
/*          exit.  The output state of H if INFO is nonzero is given */
/*          below under the description of INFO. */

/*     LDH     (input) INTEGER */
/*          The leading dimension of the array H. LDH >= max(1,N). */

/*     WR      (output) DOUBLE PRECISION array, dimension (N) */
/*     WI      (output) DOUBLE PRECISION array, dimension (N) */
/*          The real and imaginary parts, respectively, of the computed */
/*          eigenvalues ILO to IHI are stored in the corresponding */
/*          elements of WR and WI. If two eigenvalues are computed as a */
/*          complex conjugate pair, they are stored in consecutive */
/*          elements of WR and WI, say the i-th and (i+1)th, with */
/*          WI(i) > 0 and WI(i+1) < 0. If WANTT is .TRUE., the */
/*          eigenvalues are stored in the same order as on the diagonal */
/*          of the Schur form returned in H, with WR(i) = H(i,i), and, if */
/*          H(i:i+1,i:i+1) is a 2-by-2 diagonal block, */
/*          WI(i) = sqrt(H(i+1,i)*H(i,i+1)) and WI(i+1) = -WI(i). */

/*     ILOZ    (input) INTEGER */
/*     IHIZ    (input) INTEGER */
/*          Specify the rows of Z to which transformations must be */
/*          applied if WANTZ is .TRUE.. */
/*          1 <= ILOZ <= ILO; IHI <= IHIZ <= N. */

/*     Z       (input/output) DOUBLE PRECISION array, dimension (LDZ,N) */
/*          If WANTZ is .TRUE., on entry Z must contain the current */
/*          matrix Z of transformations accumulated by DHSEQR, and on */
/*          exit Z has been updated; transformations are applied only to */
/*          the submatrix Z(ILOZ:IHIZ,ILO:IHI). */
/*          If WANTZ is .FALSE., Z is not referenced. */

/*     LDZ     (input) INTEGER */
/*          The leading dimension of the array Z. LDZ >= max(1,N). */

/*     INFO    (output) INTEGER */
/*           =   0: successful exit */
/*          .GT. 0: If INFO = i, DLAHQR failed to compute all the */
/*                  eigenvalues ILO to IHI in a total of 30 iterations */
/*                  per eigenvalue; elements i+1:ihi of WR and WI */
/*                  contain those eigenvalues which have been */
/*                  successfully computed. */

/*                  If INFO .GT. 0 and WANTT is .FALSE., then on exit, */
/*                  the remaining unconverged eigenvalues are the */
/*                  eigenvalues of the upper Hessenberg matrix rows */
/*                  and columns ILO thorugh INFO of the final, output */
/*                  value of H. */

/*                  If INFO .GT. 0 and WANTT is .TRUE., then on exit */
/*          (*)       (initial value of H)*U  = U*(final value of H) */
/*                  where U is an orthognal matrix.    The final */
/*                  value of H is upper Hessenberg and triangular in */
/*                  rows and columns INFO+1 through IHI. */

/*                  If INFO .GT. 0 and WANTZ is .TRUE., then on exit */
/*                      (final value of Z)  = (initial value of Z)*U */
/*                  where U is the orthogonal matrix in (*) */
/*                  (regardless of the value of WANTT.) */

/*     Further Details */
/*     =============== */

/*     02-96 Based on modifications by */
/*     David Day, Sandia National Laboratory, USA */

/*     12-04 Further modifications by */
/*     Ralph Byers, University of Kansas, USA */

/*       This is a modified version of DLAHQR from LAPACK version 3.0. */
/*       It is (1) more robust against overflow and underflow and */
/*       (2) adopts the more conservative Ahues & Tisseur stopping */
/*       criterion (LAWN 122, 1997). */

/*     ========================================================= */

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
    h_dim1 = *ldh;
    h_offset = 1 + h_dim1;
    h__ -= h_offset;
    --wr;
    --wi;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;

    /* Function Body */
    *info = 0;

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }
    if (*ilo == *ihi) {
	wr[*ilo] = h__[*ilo + *ilo * h_dim1];
	wi[*ilo] = 0.;
	return 0;
    }

/*     ==== clear out the trash ==== */
    i__1 = *ihi - 3;
    for (j = *ilo; j <= i__1; ++j) {
	h__[j + 2 + j * h_dim1] = 0.;
	h__[j + 3 + j * h_dim1] = 0.;
/* L10: */
    }
    if (*ilo <= *ihi - 2) {
	h__[*ihi + (*ihi - 2) * h_dim1] = 0.;
    }

    nh = *ihi - *ilo + 1;
    nz = *ihiz - *iloz + 1;

/*     Set machine-dependent constants for the stopping criterion. */

    safmin = dlamch_("SAFE MINIMUM");
    safmax = 1. / safmin;
    dlabad_(&safmin, &safmax);
    ulp = dlamch_("PRECISION");
    smlnum = safmin * ((double) nh / ulp);

/*     I1 and I2 are the indices of the first row and last column of H */
/*     to which transformations must be applied. If eigenvalues only are */
/*     being computed, I1 and I2 are set inside the main loop. */

    if (*wantt) {
	i1 = 1;
	i2 = *n;
    }

/*     The main loop begins here. I is the loop index and decreases from */
/*     IHI to ILO in steps of 1 or 2. Each iteration of the loop works */
/*     with the active submatrix in rows and columns L to I. */
/*     Eigenvalues I+1 to IHI have already converged. Either L = ILO or */
/*     H(L,L-1) is negligible so that the matrix splits. */

    i__ = *ihi;
L20:
    l = *ilo;
    if (i__ < *ilo) {
	goto L160;
    }

/*     Perform QR iterations on rows and columns ILO to I until a */
/*     submatrix of order 1 or 2 splits off at the bottom because a */
/*     subdiagonal element has become negligible. */

    for (its = 0; its <= 30; ++its) {

/*        Look for a single small subdiagonal element. */

	i__1 = l + 1;
	for (k = i__; k >= i__1; --k) {
	    if ((d__1 = h__[k + (k - 1) * h_dim1], abs(d__1)) <= smlnum) {
		goto L40;
	    }
	    tst = (d__1 = h__[k - 1 + (k - 1) * h_dim1], abs(d__1)) + (d__2 =
		    h__[k + k * h_dim1], abs(d__2));
	    if (tst == 0.) {
		if (k - 2 >= *ilo) {
		    tst += (d__1 = h__[k - 1 + (k - 2) * h_dim1], abs(d__1));
		}
		if (k + 1 <= *ihi) {
		    tst += (d__1 = h__[k + 1 + k * h_dim1], abs(d__1));
		}
	    }
/*           ==== The following is a conservative small subdiagonal */
/*           .    deflation  criterion due to Ahues & Tisseur (LAWN 122, */
/*           .    1997). It has better mathematical foundation and */
/*           .    improves accuracy in some cases.  ==== */
	    if ((d__1 = h__[k + (k - 1) * h_dim1], abs(d__1)) <= ulp * tst) {
/* Computing MAX */
		d__3 = (d__1 = h__[k + (k - 1) * h_dim1], abs(d__1)), d__4 = (
			d__2 = h__[k - 1 + k * h_dim1], abs(d__2));
		ab = std::max(d__3,d__4);
/* Computing MIN */
		d__3 = (d__1 = h__[k + (k - 1) * h_dim1], abs(d__1)), d__4 = (
			d__2 = h__[k - 1 + k * h_dim1], abs(d__2));
		ba = std::min(d__3,d__4);
/* Computing MAX */
		d__3 = (d__1 = h__[k + k * h_dim1], abs(d__1)), d__4 = (d__2 =
			 h__[k - 1 + (k - 1) * h_dim1] - h__[k + k * h_dim1],
			abs(d__2));
		aa = std::max(d__3,d__4);
/* Computing MIN */
		d__3 = (d__1 = h__[k + k * h_dim1], abs(d__1)), d__4 = (d__2 =
			 h__[k - 1 + (k - 1) * h_dim1] - h__[k + k * h_dim1],
			abs(d__2));
		bb = std::min(d__3,d__4);
		s = aa + ab;
/* Computing MAX */
		d__1 = smlnum, d__2 = ulp * (bb * (aa / s));
		if (ba * (ab / s) <= std::max(d__1,d__2)) {
		    goto L40;
		}
	    }
/* L30: */
	}
L40:
	l = k;
	if (l > *ilo) {

/*           H(L,L-1) is negligible */

	    h__[l + (l - 1) * h_dim1] = 0.;
	}

/*        Exit from loop if a submatrix of order 1 or 2 has split off. */

	if (l >= i__ - 1) {
	    goto L150;
	}

/*        Now the active submatrix is in rows and columns L to I. If */
/*        eigenvalues only are being computed, only the active submatrix */
/*        need be transformed. */

	if (! (*wantt)) {
	    i1 = l;
	    i2 = i__;
	}

	if (its == 10) {

/*           Exceptional shift. */

		s = (d__1 = h__[l + 1 + l * h_dim1], abs(d__1)) + (d__2 = h__[l +
		2 + (l + 1) * h_dim1], abs(d__2));
	    h11 = s * .75 + h__[l + l * h_dim1];
	    h12 = s * -.4375;
	    h21 = s;
	    h22 = h11;
	} else if (its == 20) {

/*           Exceptional shift. */

	    s = (d__1 = h__[i__ + (i__ - 1) * h_dim1], abs(d__1)) + (d__2 =
		    h__[i__ - 1 + (i__ - 2) * h_dim1], abs(d__2));
	    h11 = s * .75 + h__[i__ + i__ * h_dim1];
	    h12 = s * -.4375;
	    h21 = s;
	    h22 = h11;
	} else {

/*           Prepare to use Francis' double shift */
/*           (i.e. 2nd degree generalized Rayleigh quotient) */
	    h11 = h__[i__ - 1 + (i__ - 1) * h_dim1];
	    h21 = h__[i__ + (i__ - 1) * h_dim1];
	    h12 = h__[i__ - 1 + i__ * h_dim1];
	    h22 = h__[i__ + i__ * h_dim1];
	}
	s = abs(h11) + abs(h12) + abs(h21) + abs(h22);
	if (s == 0.) {
	    rt1r = 0.;
	    rt1i = 0.;
	    rt2r = 0.;
	    rt2i = 0.;
	} else {
	    h11 /= s;
	    h21 /= s;
	    h12 /= s;
	    h22 /= s;
	    tr = (h11 + h22) / 2.;
	    det = (h11 - tr) * (h22 - tr) - h12 * h21;
	    rtdisc = sqrt((abs(det)));
	    if (det >= 0.) {

/*              ==== complex conjugate shifts ==== */

		rt1r = tr * s;
		rt2r = rt1r;
		rt1i = rtdisc * s;
		rt2i = -rt1i;
	    } else {

/*              ==== real shifts (use only one of them)  ==== */

		rt1r = tr + rtdisc;
		rt2r = tr - rtdisc;
		if ((d__1 = rt1r - h22, abs(d__1)) <= (d__2 = rt2r - h22, abs(
			d__2))) {
		    rt1r *= s;
		    rt2r = rt1r;
		} else {
		    rt2r *= s;
		    rt1r = rt2r;
		}
		rt1i = 0.;
		rt2i = 0.;
	    }
	}

/*        Look for two consecutive small subdiagonal elements. */

	i__1 = l;
	for (m = i__ - 2; m >= i__1; --m) {
/*           Determine the effect of starting the double-shift QR */
/*           iteration at row M, and see if this would make H(M,M-1) */
/*           negligible.  (The following uses scaling to avoid */
/*           overflows and most underflows.) */

	    h21s = h__[m + 1 + m * h_dim1];
	    s = (d__1 = h__[m + m * h_dim1] - rt2r, abs(d__1)) + abs(rt2i) +
		    abs(h21s);
	    h21s = h__[m + 1 + m * h_dim1] / s;
	    v[0] = h21s * h__[m + (m + 1) * h_dim1] + (h__[m + m * h_dim1] -
		    rt1r) * ((h__[m + m * h_dim1] - rt2r) / s) - rt1i * (rt2i
		    / s);
	    v[1] = h21s * (h__[m + m * h_dim1] + h__[m + 1 + (m + 1) * h_dim1]
		     - rt1r - rt2r);
	    v[2] = h21s * h__[m + 2 + (m + 1) * h_dim1];
	    s = abs(v[0]) + abs(v[1]) + abs(v[2]);
	    v[0] /= s;
	    v[1] /= s;
	    v[2] /= s;
	    if (m == l) {
		goto L60;
	    }
	    if ((d__1 = h__[m + (m - 1) * h_dim1], abs(d__1)) * (abs(v[1]) +
		    abs(v[2])) <= ulp * abs(v[0]) * ((d__2 = h__[m - 1 + (m -
		    1) * h_dim1], abs(d__2)) + (d__3 = h__[m + m * h_dim1],
		    abs(d__3)) + (d__4 = h__[m + 1 + (m + 1) * h_dim1], abs(
		    d__4)))) {
		goto L60;
	    }
/* L50: */
	}
L60:

/*        Double-shift QR step */

	i__1 = i__ - 1;
	for (k = m; k <= i__1; ++k) {

/*           The first iteration of this loop determines a reflection G */
/*           from the vector V and applies it from left and right to H, */
/*           thus creating a nonzero bulge below the subdiagonal. */

/*           Each subsequent iteration determines a reflection G to */
/*           restore the Hessenberg form in the (K-1)th column, and thus */
/*           chases the bulge one step toward the bottom of the active */
/*           submatrix. NR is the order of G. */

/* Computing MIN */
	    i__2 = 3, i__3 = i__ - k + 1;
	    nr = std::min(i__2,i__3);
	    if (k > m) {
		dcopy_(&nr, &h__[k + (k - 1) * h_dim1], &c__1, v, &c__1);
	    }
	    dlarfg_(&nr, v, &v[1], &c__1, &t1);
	    if (k > m) {
		h__[k + (k - 1) * h_dim1] = v[0];
		h__[k + 1 + (k - 1) * h_dim1] = 0.;
		if (k < i__ - 1) {
		    h__[k + 2 + (k - 1) * h_dim1] = 0.;
		}
	    } else if (m > l) {
/*               ==== Use the following instead of */
/*               .    H( K, K-1 ) = -H( K, K-1 ) to */
/*               .    avoid a bug when v(2) and v(3) */
/*               .    underflow. ==== */
		h__[k + (k - 1) * h_dim1] *= 1. - t1;
	    }
	    v2 = v[1];
	    t2 = t1 * v2;
	    if (nr == 3) {
		v3 = v[2];
		t3 = t1 * v3;

/*              Apply G from the left to transform the rows of the matrix */
/*              in columns K to I2. */

		i__2 = i2;
		for (j = k; j <= i__2; ++j) {
		    sum = h__[k + j * h_dim1] + v2 * h__[k + 1 + j * h_dim1]
			    + v3 * h__[k + 2 + j * h_dim1];
		    h__[k + j * h_dim1] -= sum * t1;
		    h__[k + 1 + j * h_dim1] -= sum * t2;
		    h__[k + 2 + j * h_dim1] -= sum * t3;
/* L70: */
		}

/*              Apply G from the right to transform the columns of the */
/*              matrix in rows I1 to min(K+3,I). */

/* Computing MIN */
		i__3 = k + 3;
		i__2 = std::min(i__3,i__);
		for (j = i1; j <= i__2; ++j) {
		    sum = h__[j + k * h_dim1] + v2 * h__[j + (k + 1) * h_dim1]
			     + v3 * h__[j + (k + 2) * h_dim1];
		    h__[j + k * h_dim1] -= sum * t1;
		    h__[j + (k + 1) * h_dim1] -= sum * t2;
		    h__[j + (k + 2) * h_dim1] -= sum * t3;
/* L80: */
		}

		if (*wantz) {

/*                 Accumulate transformations in the matrix Z */

		    i__2 = *ihiz;
		    for (j = *iloz; j <= i__2; ++j) {
			sum = z__[j + k * z_dim1] + v2 * z__[j + (k + 1) *
				z_dim1] + v3 * z__[j + (k + 2) * z_dim1];
			z__[j + k * z_dim1] -= sum * t1;
			z__[j + (k + 1) * z_dim1] -= sum * t2;
			z__[j + (k + 2) * z_dim1] -= sum * t3;
/* L90: */
		    }
		}
	    } else if (nr == 2) {

/*              Apply G from the left to transform the rows of the matrix */
/*              in columns K to I2. */

		i__2 = i2;
		for (j = k; j <= i__2; ++j) {
		    sum = h__[k + j * h_dim1] + v2 * h__[k + 1 + j * h_dim1];
		    h__[k + j * h_dim1] -= sum * t1;
		    h__[k + 1 + j * h_dim1] -= sum * t2;
/* L100: */
		}

/*              Apply G from the right to transform the columns of the */
/*              matrix in rows I1 to min(K+3,I). */

		i__2 = i__;
		for (j = i1; j <= i__2; ++j) {
		    sum = h__[j + k * h_dim1] + v2 * h__[j + (k + 1) * h_dim1]
			    ;
		    h__[j + k * h_dim1] -= sum * t1;
		    h__[j + (k + 1) * h_dim1] -= sum * t2;
/* L110: */
		}

		if (*wantz) {

/*                 Accumulate transformations in the matrix Z */

		    i__2 = *ihiz;
		    for (j = *iloz; j <= i__2; ++j) {
			sum = z__[j + k * z_dim1] + v2 * z__[j + (k + 1) *
				z_dim1];
			z__[j + k * z_dim1] -= sum * t1;
			z__[j + (k + 1) * z_dim1] -= sum * t2;
/* L120: */
		    }
		}
	    }
/* L130: */
	}

/* L140: */
    }

/*     Failure to converge in remaining number of iterations */

    *info = i__;
    return 0;

L150:

    if (l == i__) {

/*        H(I,I-1) is negligible: one eigenvalue has converged. */

	wr[i__] = h__[i__ + i__ * h_dim1];
	wi[i__] = 0.;
    } else if (l == i__ - 1) {

/*        H(I-1,I-2) is negligible: a pair of eigenvalues have converged. */

/*        Transform the 2-by-2 submatrix to standard Schur form, */
/*        and compute and store the eigenvalues. */

	dlanv2_(&h__[i__ - 1 + (i__ - 1) * h_dim1], &h__[i__ - 1 + i__ *
		h_dim1], &h__[i__ + (i__ - 1) * h_dim1], &h__[i__ + i__ *
		h_dim1], &wr[i__ - 1], &wi[i__ - 1], &wr[i__], &wi[i__], &cs,
		&sn);

	if (*wantt) {

/*           Apply the transformation to the rest of H. */

	    if (i2 > i__) {
		i__1 = i2 - i__;
		drot_(&i__1, &h__[i__ - 1 + (i__ + 1) * h_dim1], ldh, &h__[
			i__ + (i__ + 1) * h_dim1], ldh, &cs, &sn);
	    }
	    i__1 = i__ - i1 - 1;
	    drot_(&i__1, &h__[i1 + (i__ - 1) * h_dim1], &c__1, &h__[i1 + i__ *
		     h_dim1], &c__1, &cs, &sn);
	}
	if (*wantz) {

/*           Apply the transformation to Z. */

	    drot_(&nz, &z__[*iloz + (i__ - 1) * z_dim1], &c__1, &z__[*iloz +
		    i__ * z_dim1], &c__1, &cs, &sn);
	}
    }

/*     return to start of the main loop with new value of I. */

    i__ = l - 1;
    goto L20;

L160:
    return 0;

/*     End of DLAHQR */

} /* dlahqr_ */

/* Subroutine */ int dlahr2_(integer *n, integer *k, integer *nb, double *
	a, integer *lda, double *tau, double *t, integer *ldt,
	double *y, integer *ldy)
{
	/* Table of constant values */
	static double c_b4 = -1.;
	static double c_b5 = 1.;
	static integer c__1 = 1;
	static double c_b38 = 0.;

    /* System generated locals */
    integer a_dim1, a_offset, t_dim1, t_offset, y_dim1, y_offset, i__1, i__2,
	    i__3;
    double d__1;

    /* Local variables */
    integer i__;
    double ei;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAHR2 reduces the first NB columns of A real general n-BY-(n-k+1) */
/*  matrix A so that elements below the k-th subdiagonal are zero. The */
/*  reduction is performed by an orthogonal similarity transformation */
/*  Q' * A * Q. The routine returns the matrices V and T which determine */
/*  Q as a block reflector I - V*T*V', and also the matrix Y = A * V * T. */

/*  This is an auxiliary routine called by DGEHRD. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A. */

/*  K       (input) INTEGER */
/*          The offset for the reduction. Elements below the k-th */
/*          subdiagonal in the first NB columns are reduced to zero. */
/*          K < N. */

/*  NB      (input) INTEGER */
/*          The number of columns to be reduced. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N-K+1) */
/*          On entry, the n-by-(n-k+1) general matrix A. */
/*          On exit, the elements on and above the k-th subdiagonal in */
/*          the first NB columns are overwritten with the corresponding */
/*          elements of the reduced matrix; the elements below the k-th */
/*          subdiagonal, with the array TAU, represent the matrix Q as a */
/*          product of elementary reflectors. The other columns of A are */
/*          unchanged. See Further Details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (NB) */
/*          The scalar factors of the elementary reflectors. See Further */
/*          Details. */

/*  T       (output) DOUBLE PRECISION array, dimension (LDT,NB) */
/*          The upper triangular matrix T. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of the array T.  LDT >= NB. */

/*  Y       (output) DOUBLE PRECISION array, dimension (LDY,NB) */
/*          The n-by-nb matrix Y. */

/*  LDY     (input) INTEGER */
/*          The leading dimension of the array Y. LDY >= N. */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of nb elementary reflectors */

/*     Q = H(1) H(2) . . . H(nb). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(1:i+k-1) = 0, v(i+k) = 1; v(i+k+1:n) is stored on exit in */
/*  A(i+k+1:n,i), and tau in TAU(i). */

/*  The elements of the vectors v together form the (n-k+1)-by-nb matrix */
/*  V which is needed, with T and Y, to apply the transformation to the */
/*  unreduced part of the matrix, using an update of the form: */
/*  A := (I - V*T*V') * (A - Y*V'). */

/*  The contents of A on exit are illustrated by the following example */
/*  with n = 7, k = 3 and nb = 2: */

/*     ( a   a   a   a   a ) */
/*     ( a   a   a   a   a ) */
/*     ( a   a   a   a   a ) */
/*     ( h   h   a   a   a ) */
/*     ( v1  h   a   a   a ) */
/*     ( v1  v2  a   a   a ) */
/*     ( v1  v2  a   a   a ) */

/*  where a denotes an element of the original matrix A, h denotes a */
/*  modified element of the upper Hessenberg matrix H, and vi denotes an */
/*  element of the vector defining H(i). */

/*  This file is a slight modification of LAPACK-3.0's DLAHRD */
/*  incorporating improvements proposed by Quintana-Orti and Van de */
/*  Gejin. Note that the entries of A(1:K,2:NB) differ from those */
/*  returned by the original LAPACK routine. This function is */
/*  not backward compatible with LAPACK3.0. */

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

/*     Quick return if possible */

    /* Parameter adjustments */
    --tau;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    y_dim1 = *ldy;
    y_offset = 1 + y_dim1;
    y -= y_offset;

    /* Function Body */
    if (*n <= 1) {
	return 0;
    }

    i__1 = *nb;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (i__ > 1) {

/*           Update A(K+1:N,I) */

/*           Update I-th column of A - Y * V' */

	    i__2 = *n - *k;
	    i__3 = i__ - 1;
	    dgemv_("NO TRANSPOSE", &i__2, &i__3, &c_b4, &y[*k + 1 + y_dim1],
		    ldy, &a[*k + i__ - 1 + a_dim1], lda, &c_b5, &a[*k + 1 +
		    i__ * a_dim1], &c__1);

/*           Apply I - V * T' * V' to this column (call it b) from the */
/*           left, using the last column of T as workspace */

/*           Let  V = ( V1 )   and   b = ( b1 )   (first I-1 rows) */
/*                    ( V2 )             ( b2 ) */

/*           where V1 is unit lower triangular */

/*           w := V1' * b1 */

	    i__2 = i__ - 1;
	    dcopy_(&i__2, &a[*k + 1 + i__ * a_dim1], &c__1, &t[*nb * t_dim1 +
		    1], &c__1);
	    i__2 = i__ - 1;
	    dtrmv_("Lower", "Transpose", "UNIT", &i__2, &a[*k + 1 + a_dim1],
		    lda, &t[*nb * t_dim1 + 1], &c__1);

/*           w := w + V2'*b2 */

	    i__2 = *n - *k - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("Transpose", &i__2, &i__3, &c_b5, &a[*k + i__ + a_dim1],
		    lda, &a[*k + i__ + i__ * a_dim1], &c__1, &c_b5, &t[*nb *
		    t_dim1 + 1], &c__1);

/*           w := T'*w */

	    i__2 = i__ - 1;
	    dtrmv_("Upper", "Transpose", "NON-UNIT", &i__2, &t[t_offset], ldt,
		     &t[*nb * t_dim1 + 1], &c__1);

/*           b2 := b2 - V2*w */

	    i__2 = *n - *k - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("NO TRANSPOSE", &i__2, &i__3, &c_b4, &a[*k + i__ + a_dim1],
		     lda, &t[*nb * t_dim1 + 1], &c__1, &c_b5, &a[*k + i__ +
		    i__ * a_dim1], &c__1);

/*           b1 := b1 - V1*w */

	    i__2 = i__ - 1;
	    dtrmv_("Lower", "NO TRANSPOSE", "UNIT", &i__2, &a[*k + 1 + a_dim1]
, lda, &t[*nb * t_dim1 + 1], &c__1);
	    i__2 = i__ - 1;
	    daxpy_(&i__2, &c_b4, &t[*nb * t_dim1 + 1], &c__1, &a[*k + 1 + i__
		    * a_dim1], &c__1);

	    a[*k + i__ - 1 + (i__ - 1) * a_dim1] = ei;
	}

/*        Generate the elementary reflector H(I) to annihilate */
/*        A(K+I+1:N,I) */

	i__2 = *n - *k - i__ + 1;
/* Computing MIN */
	i__3 = *k + i__ + 1;
	dlarfg_(&i__2, &a[*k + i__ + i__ * a_dim1], &a[std::min(i__3, *n)+ i__ *
		a_dim1], &c__1, &tau[i__]);
	ei = a[*k + i__ + i__ * a_dim1];
	a[*k + i__ + i__ * a_dim1] = 1.;

/*        Compute  Y(K+1:N,I) */

	i__2 = *n - *k;
	i__3 = *n - *k - i__ + 1;
	dgemv_("NO TRANSPOSE", &i__2, &i__3, &c_b5, &a[*k + 1 + (i__ + 1) *
		a_dim1], lda, &a[*k + i__ + i__ * a_dim1], &c__1, &c_b38, &y[*
		k + 1 + i__ * y_dim1], &c__1);
	i__2 = *n - *k - i__ + 1;
	i__3 = i__ - 1;
	dgemv_("Transpose", &i__2, &i__3, &c_b5, &a[*k + i__ + a_dim1], lda, &
		a[*k + i__ + i__ * a_dim1], &c__1, &c_b38, &t[i__ * t_dim1 +
		1], &c__1);
	i__2 = *n - *k;
	i__3 = i__ - 1;
	dgemv_("NO TRANSPOSE", &i__2, &i__3, &c_b4, &y[*k + 1 + y_dim1], ldy,
		&t[i__ * t_dim1 + 1], &c__1, &c_b5, &y[*k + 1 + i__ * y_dim1],
		 &c__1);
	i__2 = *n - *k;
	dscal_(&i__2, &tau[i__], &y[*k + 1 + i__ * y_dim1], &c__1);

/*        Compute T(1:I,I) */

	i__2 = i__ - 1;
	d__1 = -tau[i__];
	dscal_(&i__2, &d__1, &t[i__ * t_dim1 + 1], &c__1);
	i__2 = i__ - 1;
	dtrmv_("Upper", "No Transpose", "NON-UNIT", &i__2, &t[t_offset], ldt,
		&t[i__ * t_dim1 + 1], &c__1)
		;
	t[i__ + i__ * t_dim1] = tau[i__];

/* L10: */
    }
    a[*k + *nb + *nb * a_dim1] = ei;

/*     Compute Y(1:K,1:NB) */

    dlacpy_("ALL", k, nb, &a[(a_dim1 << 1) + 1], lda, &y[y_offset], ldy);
    dtrmm_("RIGHT", "Lower", "NO TRANSPOSE", "UNIT", k, nb, &c_b5, &a[*k + 1
	    + a_dim1], lda, &y[y_offset], ldy);
    if (*n > *k + *nb) {
	i__1 = *n - *k - *nb;
	dgemm_("NO TRANSPOSE", "NO TRANSPOSE", k, nb, &i__1, &c_b5, &a[(*nb +
		2) * a_dim1 + 1], lda, &a[*k + 1 + *nb + a_dim1], lda, &c_b5,
		&y[y_offset], ldy);
    }
    dtrmm_("RIGHT", "Upper", "NO TRANSPOSE", "NON-UNIT", k, nb, &c_b5, &t[
	    t_offset], ldt, &y[y_offset], ldy);

    return 0;

/*     End of DLAHR2 */

} /* dlahr2_ */

/* Subroutine */ int dlahrd_(integer *n, integer *k, integer *nb, double *
	a, integer *lda, double *tau, double *t, integer *ldt,
	double *y, integer *ldy)
{
	/* Table of constant values */
	static double c_b4 = -1.;
	static double c_b5 = 1.;
	static integer c__1 = 1;
	static double c_b38 = 0.;

    /* System generated locals */
    integer a_dim1, a_offset, t_dim1, t_offset, y_dim1, y_offset, i__1, i__2,
	    i__3;
    double d__1;

    /* Local variables */
    integer i__;
    double ei;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAHRD reduces the first NB columns of a real general n-by-(n-k+1) */
/*  matrix A so that elements below the k-th subdiagonal are zero. The */
/*  reduction is performed by an orthogonal similarity transformation */
/*  Q' * A * Q. The routine returns the matrices V and T which determine */
/*  Q as a block reflector I - V*T*V', and also the matrix Y = A * V * T. */

/*  This is an OBSOLETE auxiliary routine. */
/*  This routine will be 'deprecated' in a  future release. */
/*  Please use the new routine DLAHR2 instead. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A. */

/*  K       (input) INTEGER */
/*          The offset for the reduction. Elements below the k-th */
/*          subdiagonal in the first NB columns are reduced to zero. */

/*  NB      (input) INTEGER */
/*          The number of columns to be reduced. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N-K+1) */
/*          On entry, the n-by-(n-k+1) general matrix A. */
/*          On exit, the elements on and above the k-th subdiagonal in */
/*          the first NB columns are overwritten with the corresponding */
/*          elements of the reduced matrix; the elements below the k-th */
/*          subdiagonal, with the array TAU, represent the matrix Q as a */
/*          product of elementary reflectors. The other columns of A are */
/*          unchanged. See Further Details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  TAU     (output) DOUBLE PRECISION array, dimension (NB) */
/*          The scalar factors of the elementary reflectors. See Further */
/*          Details. */

/*  T       (output) DOUBLE PRECISION array, dimension (LDT,NB) */
/*          The upper triangular matrix T. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of the array T.  LDT >= NB. */

/*  Y       (output) DOUBLE PRECISION array, dimension (LDY,NB) */
/*          The n-by-nb matrix Y. */

/*  LDY     (input) INTEGER */
/*          The leading dimension of the array Y. LDY >= N. */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of nb elementary reflectors */

/*     Q = H(1) H(2) . . . H(nb). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a real scalar, and v is a real vector with */
/*  v(1:i+k-1) = 0, v(i+k) = 1; v(i+k+1:n) is stored on exit in */
/*  A(i+k+1:n,i), and tau in TAU(i). */

/*  The elements of the vectors v together form the (n-k+1)-by-nb matrix */
/*  V which is needed, with T and Y, to apply the transformation to the */
/*  unreduced part of the matrix, using an update of the form: */
/*  A := (I - V*T*V') * (A - Y*V'). */

/*  The contents of A on exit are illustrated by the following example */
/*  with n = 7, k = 3 and nb = 2: */

/*     ( a   h   a   a   a ) */
/*     ( a   h   a   a   a ) */
/*     ( a   h   a   a   a ) */
/*     ( h   h   a   a   a ) */
/*     ( v1  h   a   a   a ) */
/*     ( v1  v2  a   a   a ) */
/*     ( v1  v2  a   a   a ) */

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

/*     Quick return if possible */

    /* Parameter adjustments */
    --tau;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    y_dim1 = *ldy;
    y_offset = 1 + y_dim1;
    y -= y_offset;

    /* Function Body */
    if (*n <= 1) {
	return 0;
    }

    i__1 = *nb;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (i__ > 1) {

/*           Update A(1:n,i) */

/*           Compute i-th column of A - Y * V' */

	    i__2 = i__ - 1;
	    dgemv_("No transpose", n, &i__2, &c_b4, &y[y_offset], ldy, &a[*k
		    + i__ - 1 + a_dim1], lda, &c_b5, &a[i__ * a_dim1 + 1], &
		    c__1);

/*           Apply I - V * T' * V' to this column (call it b) from the */
/*           left, using the last column of T as workspace */

/*           Let  V = ( V1 )   and   b = ( b1 )   (first I-1 rows) */
/*                    ( V2 )             ( b2 ) */

/*           where V1 is unit lower triangular */

/*           w := V1' * b1 */

	    i__2 = i__ - 1;
	    dcopy_(&i__2, &a[*k + 1 + i__ * a_dim1], &c__1, &t[*nb * t_dim1 +
		    1], &c__1);
	    i__2 = i__ - 1;
	    dtrmv_("Lower", "Transpose", "Unit", &i__2, &a[*k + 1 + a_dim1],
		    lda, &t[*nb * t_dim1 + 1], &c__1);

/*           w := w + V2'*b2 */

	    i__2 = *n - *k - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("Transpose", &i__2, &i__3, &c_b5, &a[*k + i__ + a_dim1],
		    lda, &a[*k + i__ + i__ * a_dim1], &c__1, &c_b5, &t[*nb *
		    t_dim1 + 1], &c__1);

/*           w := T'*w */

	    i__2 = i__ - 1;
	    dtrmv_("Upper", "Transpose", "Non-unit", &i__2, &t[t_offset], ldt,
		     &t[*nb * t_dim1 + 1], &c__1);

/*           b2 := b2 - V2*w */

	    i__2 = *n - *k - i__ + 1;
	    i__3 = i__ - 1;
	    dgemv_("No transpose", &i__2, &i__3, &c_b4, &a[*k + i__ + a_dim1],
		     lda, &t[*nb * t_dim1 + 1], &c__1, &c_b5, &a[*k + i__ +
		    i__ * a_dim1], &c__1);

/*           b1 := b1 - V1*w */

	    i__2 = i__ - 1;
	    dtrmv_("Lower", "No transpose", "Unit", &i__2, &a[*k + 1 + a_dim1]
, lda, &t[*nb * t_dim1 + 1], &c__1);
	    i__2 = i__ - 1;
	    daxpy_(&i__2, &c_b4, &t[*nb * t_dim1 + 1], &c__1, &a[*k + 1 + i__
		    * a_dim1], &c__1);

	    a[*k + i__ - 1 + (i__ - 1) * a_dim1] = ei;
	}

/*        Generate the elementary reflector H(i) to annihilate */
/*        A(k+i+1:n,i) */

	i__2 = *n - *k - i__ + 1;
/* Computing MIN */
	i__3 = *k + i__ + 1;
	dlarfg_(&i__2, &a[*k + i__ + i__ * a_dim1], &a[std::min(i__3, *n)+ i__ *
		a_dim1], &c__1, &tau[i__]);
	ei = a[*k + i__ + i__ * a_dim1];
	a[*k + i__ + i__ * a_dim1] = 1.;

/*        Compute  Y(1:n,i) */

	i__2 = *n - *k - i__ + 1;
	dgemv_("No transpose", n, &i__2, &c_b5, &a[(i__ + 1) * a_dim1 + 1],
		lda, &a[*k + i__ + i__ * a_dim1], &c__1, &c_b38, &y[i__ *
		y_dim1 + 1], &c__1);
	i__2 = *n - *k - i__ + 1;
	i__3 = i__ - 1;
	dgemv_("Transpose", &i__2, &i__3, &c_b5, &a[*k + i__ + a_dim1], lda, &
		a[*k + i__ + i__ * a_dim1], &c__1, &c_b38, &t[i__ * t_dim1 +
		1], &c__1);
	i__2 = i__ - 1;
	dgemv_("No transpose", n, &i__2, &c_b4, &y[y_offset], ldy, &t[i__ *
		t_dim1 + 1], &c__1, &c_b5, &y[i__ * y_dim1 + 1], &c__1);
	dscal_(n, &tau[i__], &y[i__ * y_dim1 + 1], &c__1);

/*        Compute T(1:i,i) */

	i__2 = i__ - 1;
	d__1 = -tau[i__];
	dscal_(&i__2, &d__1, &t[i__ * t_dim1 + 1], &c__1);
	i__2 = i__ - 1;
	dtrmv_("Upper", "No transpose", "Non-unit", &i__2, &t[t_offset], ldt,
		&t[i__ * t_dim1 + 1], &c__1)
		;
	t[i__ + i__ * t_dim1] = tau[i__];

/* L10: */
    }
    a[*k + *nb + *nb * a_dim1] = ei;

    return 0;

/*     End of DLAHRD */

} /* dlahrd_ */

/* Subroutine */ int dlaic1_(integer *job, integer *j, double *x,
	double *sest, double *w, double *gamma, double *
	sestpr, double *s, double *c__)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b5 = 1.;

    /* System generated locals */
    double d__1, d__2, d__3, d__4;

    /* Builtin functions
    double sqrt(double), d_sign(double *, double *); */

    /* Local variables */
    double b, t, s1, s2, eps, tmp;
    double sine, test, zeta1, zeta2, alpha, norma;
    double absgam, absalp, cosine, absest;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAIC1 applies one step of incremental condition estimation in */
/*  its simplest version: */

/*  Let x, twonorm(x) = 1, be an approximate singular vector of an j-by-j */
/*  lower triangular matrix L, such that */
/*           twonorm(L*x) = sest */
/*  Then DLAIC1 computes sestpr, s, c such that */
/*  the vector */
/*                  [ s*x ] */
/*           xhat = [  c  ] */
/*  is an approximate singular vector of */
/*                  [ L     0  ] */
/*           Lhat = [ w' gamma ] */
/*  in the sense that */
/*           twonorm(Lhat*xhat) = sestpr. */

/*  Depending on JOB, an estimate for the largest or smallest singular */
/*  value is computed. */

/*  Note that [s c]' and sestpr**2 is an eigenpair of the system */

/*      diag(sest*sest, 0) + [alpha  gamma] * [ alpha ] */
/*                                            [ gamma ] */

/*  where  alpha =  x'*w. */

/*  Arguments */
/*  ========= */

/*  JOB     (input) INTEGER */
/*          = 1: an estimate for the largest singular value is computed. */
/*          = 2: an estimate for the smallest singular value is computed. */

/*  J       (input) INTEGER */
/*          Length of X and W */

/*  X       (input) DOUBLE PRECISION array, dimension (J) */
/*          The j-vector x. */

/*  SEST    (input) DOUBLE PRECISION */
/*          Estimated singular value of j by j matrix L */

/*  W       (input) DOUBLE PRECISION array, dimension (J) */
/*          The j-vector w. */

/*  GAMMA   (input) DOUBLE PRECISION */
/*          The diagonal element gamma. */

/*  SESTPR  (output) DOUBLE PRECISION */
/*          Estimated singular value of (j+1) by (j+1) matrix Lhat. */

/*  S       (output) DOUBLE PRECISION */
/*          Sine needed in forming xhat. */

/*  C       (output) DOUBLE PRECISION */
/*          Cosine needed in forming xhat. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --w;
    --x;

    /* Function Body */
    eps = dlamch_("Epsilon");
    alpha = ddot_(j, &x[1], &c__1, &w[1], &c__1);

    absalp = abs(alpha);
    absgam = abs(*gamma);
    absest = abs(*sest);

    if (*job == 1) {

/*        Estimating largest singular value */

/*        special cases */

	if (*sest == 0.) {
	    s1 = std::max(absgam,absalp);
	    if (s1 == 0.) {
		*s = 0.;
		*c__ = 1.;
		*sestpr = 0.;
	    } else {
		*s = alpha / s1;
		*c__ = *gamma / s1;
		tmp = sqrt(*s * *s + *c__ * *c__);
		*s /= tmp;
		*c__ /= tmp;
		*sestpr = s1 * tmp;
	    }
	    return 0;
	} else if (absgam <= eps * absest) {
	    *s = 1.;
	    *c__ = 0.;
	    tmp = std::max(absest,absalp);
	    s1 = absest / tmp;
	    s2 = absalp / tmp;
	    *sestpr = tmp * sqrt(s1 * s1 + s2 * s2);
	    return 0;
	} else if (absalp <= eps * absest) {
	    s1 = absgam;
	    s2 = absest;
	    if (s1 <= s2) {
		*s = 1.;
		*c__ = 0.;
		*sestpr = s2;
	    } else {
		*s = 0.;
		*c__ = 1.;
		*sestpr = s1;
	    }
	    return 0;
	} else if (absest <= eps * absalp || absest <= eps * absgam) {
	    s1 = absgam;
	    s2 = absalp;
	    if (s1 <= s2) {
		tmp = s1 / s2;
		*s = sqrt(tmp * tmp + 1.);
		*sestpr = s2 * *s;
		*c__ = *gamma / s2 / *s;
		*s = d_sign(&c_b5, &alpha) / *s;
	    } else {
		tmp = s2 / s1;
		*c__ = sqrt(tmp * tmp + 1.);
		*sestpr = s1 * *c__;
		*s = alpha / s1 / *c__;
		*c__ = d_sign(&c_b5, gamma) / *c__;
	    }
	    return 0;
	} else {

/*           normal case */

	    zeta1 = alpha / absest;
	    zeta2 = *gamma / absest;

	    b = (1. - zeta1 * zeta1 - zeta2 * zeta2) * .5;
	    *c__ = zeta1 * zeta1;
	    if (b > 0.) {
		t = *c__ / (b + sqrt(b * b + *c__));
	    } else {
		t = sqrt(b * b + *c__) - b;
	    }

	    sine = -zeta1 / t;
	    cosine = -zeta2 / (t + 1.);
	    tmp = sqrt(sine * sine + cosine * cosine);
	    *s = sine / tmp;
	    *c__ = cosine / tmp;
	    *sestpr = sqrt(t + 1.) * absest;
	    return 0;
	}

    } else if (*job == 2) {

/*        Estimating smallest singular value */

/*        special cases */

	if (*sest == 0.) {
	    *sestpr = 0.;
	    if (std::max(absgam,absalp) == 0.) {
		sine = 1.;
		cosine = 0.;
	    } else {
		sine = -(*gamma);
		cosine = alpha;
	    }
/* Computing MAX */
	    d__1 = abs(sine), d__2 = abs(cosine);
	    s1 = std::max(d__1,d__2);
	    *s = sine / s1;
	    *c__ = cosine / s1;
	    tmp = sqrt(*s * *s + *c__ * *c__);
	    *s /= tmp;
	    *c__ /= tmp;
	    return 0;
	} else if (absgam <= eps * absest) {
	    *s = 0.;
	    *c__ = 1.;
	    *sestpr = absgam;
	    return 0;
	} else if (absalp <= eps * absest) {
	    s1 = absgam;
	    s2 = absest;
	    if (s1 <= s2) {
		*s = 0.;
		*c__ = 1.;
		*sestpr = s1;
	    } else {
		*s = 1.;
		*c__ = 0.;
		*sestpr = s2;
	    }
	    return 0;
	} else if (absest <= eps * absalp || absest <= eps * absgam) {
	    s1 = absgam;
	    s2 = absalp;
	    if (s1 <= s2) {
		tmp = s1 / s2;
		*c__ = sqrt(tmp * tmp + 1.);
		*sestpr = absest * (tmp / *c__);
		*s = -(*gamma / s2) / *c__;
		*c__ = d_sign(&c_b5, &alpha) / *c__;
	    } else {
		tmp = s2 / s1;
		*s = sqrt(tmp * tmp + 1.);
		*sestpr = absest / *s;
		*c__ = alpha / s1 / *s;
		*s = -d_sign(&c_b5, gamma) / *s;
	    }
	    return 0;
	} else {

/*           normal case */

	    zeta1 = alpha / absest;
	    zeta2 = *gamma / absest;

/* Computing MAX */
	    d__3 = zeta1 * zeta1 + 1. + (d__1 = zeta1 * zeta2, abs(d__1)),
		    d__4 = (d__2 = zeta1 * zeta2, abs(d__2)) + zeta2 * zeta2;
	    norma = std::max(d__3,d__4);

/*           See if root is closer to zero or to ONE */

	    test = (zeta1 - zeta2) * 2. * (zeta1 + zeta2) + 1.;
	    if (test >= 0.) {

/*              root is close to zero, compute directly */

		b = (zeta1 * zeta1 + zeta2 * zeta2 + 1.) * .5;
		*c__ = zeta2 * zeta2;
		t = *c__ / (b + sqrt((d__1 = b * b - *c__, abs(d__1))));
		sine = zeta1 / (1. - t);
		cosine = -zeta2 / t;
		*sestpr = sqrt(t + eps * 4. * eps * norma) * absest;
	    } else {

/*              root is closer to ONE, shift by that amount */

		b = (zeta2 * zeta2 + zeta1 * zeta1 - 1.) * .5;
		*c__ = zeta1 * zeta1;
		if (b >= 0.) {
		    t = -(*c__) / (b + sqrt(b * b + *c__));
		} else {
		    t = b - sqrt(b * b + *c__);
		}
		sine = -zeta1 / t;
		cosine = -zeta2 / (t + 1.);
		*sestpr = sqrt(t + 1. + eps * 4. * eps * norma) * absest;
	    }
	    tmp = sqrt(sine * sine + cosine * cosine);
	    *s = sine / tmp;
	    *c__ = cosine / tmp;
	    return 0;

	}
    }
    return 0;

/*     End of DLAIC1 */

} /* dlaic1_ */

/* Subroutine */ bool dlaisnan_(double *din1, double *din2)
{
    /* System generated locals */
    bool ret_val;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  This routine is not for general use.  It exists solely to avoid */
/*  over-optimization in DISNAN. */

/*  DLAISNAN checks for NaNs by comparing its two arguments for */
/*  inequality.  NaN is the only floating-point value where NaN != NaN */
/*  returns .TRUE.  To check for NaNs, pass the same variable as both */
/*  arguments. */

/*  Strictly speaking, Fortran does not allow aliasing of function */
/*  arguments. So a compiler must assume that the two arguments are */
/*  not the same variable, and the test will not be optimized away. */
/*  Interprocedural or whole-program optimization may delete this */
/*  test.  The ISNAN functions will be replaced by the correct */
/*  Fortran 03 intrinsic once the intrinsic is widely available. */

/*  Arguments */
/*  ========= */

/*  DIN1     (input) DOUBLE PRECISION */
/*  DIN2     (input) DOUBLE PRECISION */
/*          Two numbers to compare for inequality. */

/*  ===================================================================== */

/*  .. Executable Statements .. */
    ret_val = *din1 != *din2;
    return ret_val;
} /* dlaisnan_ */

/* Subroutine */ int dlaln2_(bool *ltrans, integer *na, integer *nw,
	double *smin, double *ca, double *a, integer *lda,
	double *d1, double *d2, double *b, integer *ldb,
	double *wr, double *wi, double *x, integer *ldx,
	double *scale, double *xnorm, integer *info)
{
    /* Initialized data */

    static bool zswap[4] = { false,false,true,true };
    static bool rswap[4] = { false,true,false,true };
    static integer ipivot[16]	/* was [4][4] */ = { 1,2,3,4,2,1,4,3,3,4,1,2,
	    4,3,2,1 };

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, x_dim1, x_offset;
    double d__1, d__2, d__3, d__4, d__5, d__6;
    static double equiv_0[4], equiv_1[4];

    /* Local variables */
    integer j;
#define ci (equiv_0)
#define cr (equiv_1)
    double bi1, bi2, br1, br2, xi1, xi2, xr1, xr2, ci21, ci22, cr21, cr22,
	     li21, csi, ui11, lr21, ui12, ui22;
#define civ (equiv_0)
    double csr, ur11, ur12, ur22;
#define crv (equiv_1)
    double bbnd, cmax, ui11r, ui12s, temp, ur11r, ur12s, u22abs;
    integer icmax;
    double bnorm, cnorm, smini;
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

/*  DLALN2 solves a system of the form  (ca A - w D ) X = s B */
/*  or (ca A' - w D) X = s B   with possible scaling ("s") and */
/*  perturbation of A.  (A' means A-transpose.) */

/*  A is an NA x NA real matrix, ca is a real scalar, D is an NA x NA */
/*  real diagonal matrix, w is a real or complex value, and X and B are */
/*  NA x 1 matrices -- real if w is real, complex if w is complex.  NA */
/*  may be 1 or 2. */

/*  If w is complex, X and B are represented as NA x 2 matrices, */
/*  the first column of each being the real part and the second */
/*  being the imaginary part. */

/*  "s" is a scaling factor (.LE. 1), computed by DLALN2, which is */
/*  so chosen that X can be computed without overflow.  X is further */
/*  scaled if necessary to assure that norm(ca A - w D)*norm(X) is less */
/*  than overflow. */

/*  If both singular values of (ca A - w D) are less than SMIN, */
/*  SMIN*identity will be used instead of (ca A - w D).  If only one */
/*  singular value is less than SMIN, one element of (ca A - w D) will be */
/*  perturbed enough to make the smallest singular value roughly SMIN. */
/*  If both singular values are at least SMIN, (ca A - w D) will not be */
/*  perturbed.  In any case, the perturbation will be at most some small */
/*  multiple of max( SMIN, ulp*norm(ca A - w D) ).  The singular values */
/*  are computed by infinity-norm approximations, and thus will only be */
/*  correct to a factor of 2 or so. */

/*  Note: all input quantities are assumed to be smaller than overflow */
/*  by a reasonable factor.  (See BIGNUM.) */

/*  Arguments */
/*  ========== */

/*  LTRANS  (input) LOGICAL */
/*          =.TRUE.:  A-transpose will be used. */
/*          =.FALSE.: A will be used (not transposed.) */

/*  NA      (input) INTEGER */
/*          The size of the matrix A.  It may (only) be 1 or 2. */

/*  NW      (input) INTEGER */
/*          1 if "w" is real, 2 if "w" is complex.  It may only be 1 */
/*          or 2. */

/*  SMIN    (input) DOUBLE PRECISION */
/*          The desired lower bound on the singular values of A.  This */
/*          should be a safe distance away from underflow or overflow, */
/*          say, between (underflow/machine precision) and  (machine */
/*          precision * overflow ).  (See BIGNUM and ULP.) */

/*  CA      (input) DOUBLE PRECISION */
/*          The coefficient c, which A is multiplied by. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,NA) */
/*          The NA x NA matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of A.  It must be at least NA. */

/*  D1      (input) DOUBLE PRECISION */
/*          The 1,1 element in the diagonal matrix D. */

/*  D2      (input) DOUBLE PRECISION */
/*          The 2,2 element in the diagonal matrix D.  Not used if NW=1. */

/*  B       (input) DOUBLE PRECISION array, dimension (LDB,NW) */
/*          The NA x NW matrix B (right-hand side).  If NW=2 ("w" is */
/*          complex), column 1 contains the real part of B and column 2 */
/*          contains the imaginary part. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of B.  It must be at least NA. */

/*  WR      (input) DOUBLE PRECISION */
/*          The real part of the scalar "w". */

/*  WI      (input) DOUBLE PRECISION */
/*          The imaginary part of the scalar "w".  Not used if NW=1. */

/*  X       (output) DOUBLE PRECISION array, dimension (LDX,NW) */
/*          The NA x NW matrix X (unknowns), as computed by DLALN2. */
/*          If NW=2 ("w" is complex), on exit, column 1 will contain */
/*          the real part of X and column 2 will contain the imaginary */
/*          part. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of X.  It must be at least NA. */

/*  SCALE   (output) DOUBLE PRECISION */
/*          The scale factor that B must be multiplied by to insure */
/*          that overflow does not occur when computing X.  Thus, */
/*          (ca A - w D) X  will be SCALE*B, not B (ignoring */
/*          perturbations of A.)  It will be at most 1. */

/*  XNORM   (output) DOUBLE PRECISION */
/*          The infinity-norm of X, when X is regarded as an NA x NW */
/*          real matrix. */

/*  INFO    (output) INTEGER */
/*          An error flag.  It will be set to zero if no error occurs, */
/*          a negative number if an argument is in error, or a positive */
/*          number if  ca A - w D  had to be perturbed. */
/*          The possible values are: */
/*          = 0: No error occurred, and (ca A - w D) did not have to be */
/*                 perturbed. */
/*          = 1: (ca A - w D) had to be perturbed to make its smallest */
/*               (or only) singular value greater than SMIN. */
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
/*     .. Equivalences .. */
/*     .. */
/*     .. Data statements .. */
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

    /* Function Body */
/*     .. */
/*     .. Executable Statements .. */

/*     Compute BIGNUM */

    smlnum = 2. * dlamch_("Safe minimum");
    bignum = 1. / smlnum;
    smini = std::max(*smin,smlnum);

/*     Don't check for input errors */

    *info = 0;

/*     Standard Initializations */

    *scale = 1.;

    if (*na == 1) {

/*        1 x 1  (i.e., scalar) system   C X = B */

	if (*nw == 1) {

/*           Real 1x1 system. */

/*           C = ca A - w D */

	    csr = *ca * a[a_dim1 + 1] - *wr * *d1;
	    cnorm = abs(csr);

/*           If | C | < SMINI, use C = SMINI */

	    if (cnorm < smini) {
		csr = smini;
		cnorm = smini;
		*info = 1;
	    }

/*           Check scaling for  X = B / C */

	    bnorm = (d__1 = b[b_dim1 + 1], abs(d__1));
	    if (cnorm < 1. && bnorm > 1.) {
		if (bnorm > bignum * cnorm) {
		    *scale = 1. / bnorm;
		}
	    }

/*           Compute X */

	    x[x_dim1 + 1] = b[b_dim1 + 1] * *scale / csr;
	    *xnorm = (d__1 = x[x_dim1 + 1], abs(d__1));
	} else {

/*           Complex 1x1 system (w is complex) */

/*           C = ca A - w D */

	    csr = *ca * a[a_dim1 + 1] - *wr * *d1;
	    csi = -(*wi) * *d1;
	    cnorm = abs(csr) + abs(csi);

/*           If | C | < SMINI, use C = SMINI */

	    if (cnorm < smini) {
		csr = smini;
		csi = 0.;
		cnorm = smini;
		*info = 1;
	    }

/*           Check scaling for  X = B / C */

	    bnorm = (d__1 = b[b_dim1 + 1], abs(d__1)) + (d__2 = b[(b_dim1 <<
		    1) + 1], abs(d__2));
	    if (cnorm < 1. && bnorm > 1.) {
		if (bnorm > bignum * cnorm) {
		    *scale = 1. / bnorm;
		}
	    }

/*           Compute X */

	    d__1 = *scale * b[b_dim1 + 1];
	    d__2 = *scale * b[(b_dim1 << 1) + 1];
	    dladiv_(&d__1, &d__2, &csr, &csi, &x[x_dim1 + 1], &x[(x_dim1 << 1)
		     + 1]);
	    *xnorm = (d__1 = x[x_dim1 + 1], abs(d__1)) + (d__2 = x[(x_dim1 <<
		    1) + 1], abs(d__2));
	}

    } else {

/*        2x2 System */

/*        Compute the real part of  C = ca A - w D  (or  ca A' - w D ) */

	cr[0] = *ca * a[a_dim1 + 1] - *wr * *d1;
	cr[3] = *ca * a[(a_dim1 << 1) + 2] - *wr * *d2;
	if (*ltrans) {
	    cr[2] = *ca * a[a_dim1 + 2];
	    cr[1] = *ca * a[(a_dim1 << 1) + 1];
	} else {
	    cr[1] = *ca * a[a_dim1 + 2];
	    cr[2] = *ca * a[(a_dim1 << 1) + 1];
	}

	if (*nw == 1) {

/*           Real 2x2 system  (w is real) */

/*           Find the largest element in C */

	    cmax = 0.;
	    icmax = 0;

	    for (j = 1; j <= 4; ++j) {
		if ((d__1 = crv[j - 1], abs(d__1)) > cmax) {
		    cmax = (d__1 = crv[j - 1], abs(d__1));
		    icmax = j;
		}
/* L10: */
	    }

/*           If norm(C) < SMINI, use SMINI*identity. */

	    if (cmax < smini) {
/* Computing MAX */
		d__3 = (d__1 = b[b_dim1 + 1], abs(d__1)), d__4 = (d__2 = b[
			b_dim1 + 2], abs(d__2));
		bnorm = std::max(d__3,d__4);
		if (smini < 1. && bnorm > 1.) {
		    if (bnorm > bignum * smini) {
			*scale = 1. / bnorm;
		    }
		}
		temp = *scale / smini;
		x[x_dim1 + 1] = temp * b[b_dim1 + 1];
		x[x_dim1 + 2] = temp * b[b_dim1 + 2];
		*xnorm = temp * bnorm;
		*info = 1;
		return 0;
	    }

/*           Gaussian elimination with complete pivoting. */

	    ur11 = crv[icmax - 1];
	    cr21 = crv[ipivot[(icmax << 2) - 3] - 1];
	    ur12 = crv[ipivot[(icmax << 2) - 2] - 1];
	    cr22 = crv[ipivot[(icmax << 2) - 1] - 1];
	    ur11r = 1. / ur11;
	    lr21 = ur11r * cr21;
	    ur22 = cr22 - ur12 * lr21;

/*           If smaller pivot < SMINI, use SMINI */

	    if (abs(ur22) < smini) {
		ur22 = smini;
		*info = 1;
	    }
	    if (rswap[icmax - 1]) {
		br1 = b[b_dim1 + 2];
		br2 = b[b_dim1 + 1];
	    } else {
		br1 = b[b_dim1 + 1];
		br2 = b[b_dim1 + 2];
	    }
	    br2 -= lr21 * br1;
/* Computing MAX */
	    d__2 = (d__1 = br1 * (ur22 * ur11r), abs(d__1)), d__3 = abs(br2);
	    bbnd = std::max(d__2,d__3);
	    if (bbnd > 1. && abs(ur22) < 1.) {
		if (bbnd >= bignum * abs(ur22)) {
		    *scale = 1. / bbnd;
		}
	    }

	    xr2 = br2 * *scale / ur22;
	    xr1 = *scale * br1 * ur11r - xr2 * (ur11r * ur12);
	    if (zswap[icmax - 1]) {
		x[x_dim1 + 1] = xr2;
		x[x_dim1 + 2] = xr1;
	    } else {
		x[x_dim1 + 1] = xr1;
		x[x_dim1 + 2] = xr2;
	    }
/* Computing MAX */
	    d__1 = abs(xr1), d__2 = abs(xr2);
	    *xnorm = std::max(d__1,d__2);

/*           Further scaling if  norm(A) norm(X) > overflow */

	    if (*xnorm > 1. && cmax > 1.) {
		if (*xnorm > bignum / cmax) {
		    temp = cmax / bignum;
		    x[x_dim1 + 1] = temp * x[x_dim1 + 1];
		    x[x_dim1 + 2] = temp * x[x_dim1 + 2];
		    *xnorm = temp * *xnorm;
		    *scale = temp * *scale;
		}
	    }
	} else {

/*           Complex 2x2 system  (w is complex) */

/*           Find the largest element in C */

	    ci[0] = -(*wi) * *d1;
	    ci[1] = 0.;
	    ci[2] = 0.;
	    ci[3] = -(*wi) * *d2;
	    cmax = 0.;
	    icmax = 0;

	    for (j = 1; j <= 4; ++j) {
		if ((d__1 = crv[j - 1], abs(d__1)) + (d__2 = civ[j - 1], abs(
			d__2)) > cmax) {
		    cmax = (d__1 = crv[j - 1], abs(d__1)) + (d__2 = civ[j - 1]
			    , abs(d__2));
		    icmax = j;
		}
/* L20: */
	    }

/*           If norm(C) < SMINI, use SMINI*identity. */

	    if (cmax < smini) {
/* Computing MAX */
		d__5 = (d__1 = b[b_dim1 + 1], abs(d__1)) + (d__2 = b[(b_dim1
			<< 1) + 1], abs(d__2)), d__6 = (d__3 = b[b_dim1 + 2],
			abs(d__3)) + (d__4 = b[(b_dim1 << 1) + 2], abs(d__4));
		bnorm = std::max(d__5,d__6);
		if (smini < 1. && bnorm > 1.) {
		    if (bnorm > bignum * smini) {
			*scale = 1. / bnorm;
		    }
		}
		temp = *scale / smini;
		x[x_dim1 + 1] = temp * b[b_dim1 + 1];
		x[x_dim1 + 2] = temp * b[b_dim1 + 2];
		x[(x_dim1 << 1) + 1] = temp * b[(b_dim1 << 1) + 1];
		x[(x_dim1 << 1) + 2] = temp * b[(b_dim1 << 1) + 2];
		*xnorm = temp * bnorm;
		*info = 1;
		return 0;
	    }

/*           Gaussian elimination with complete pivoting. */

	    ur11 = crv[icmax - 1];
	    ui11 = civ[icmax - 1];
	    cr21 = crv[ipivot[(icmax << 2) - 3] - 1];
	    ci21 = civ[ipivot[(icmax << 2) - 3] - 1];
	    ur12 = crv[ipivot[(icmax << 2) - 2] - 1];
	    ui12 = civ[ipivot[(icmax << 2) - 2] - 1];
	    cr22 = crv[ipivot[(icmax << 2) - 1] - 1];
	    ci22 = civ[ipivot[(icmax << 2) - 1] - 1];
	    if (icmax == 1 || icmax == 4) {

/*              Code when off-diagonals of pivoted C are real */

		if (abs(ur11) > abs(ui11)) {
		    temp = ui11 / ur11;
/* Computing 2nd power */
		    d__1 = temp;
		    ur11r = 1. / (ur11 * (d__1 * d__1 + 1.));
		    ui11r = -temp * ur11r;
		} else {
		    temp = ur11 / ui11;
/* Computing 2nd power */
		    d__1 = temp;
		    ui11r = -1. / (ui11 * (d__1 * d__1 + 1.));
		    ur11r = -temp * ui11r;
		}
		lr21 = cr21 * ur11r;
		li21 = cr21 * ui11r;
		ur12s = ur12 * ur11r;
		ui12s = ur12 * ui11r;
		ur22 = cr22 - ur12 * lr21;
		ui22 = ci22 - ur12 * li21;
	    } else {

/*              Code when diagonals of pivoted C are real */

		ur11r = 1. / ur11;
		ui11r = 0.;
		lr21 = cr21 * ur11r;
		li21 = ci21 * ur11r;
		ur12s = ur12 * ur11r;
		ui12s = ui12 * ur11r;
		ur22 = cr22 - ur12 * lr21 + ui12 * li21;
		ui22 = -ur12 * li21 - ui12 * lr21;
	    }
	    u22abs = abs(ur22) + abs(ui22);

/*           If smaller pivot < SMINI, use SMINI */

	    if (u22abs < smini) {
		ur22 = smini;
		ui22 = 0.;
		*info = 1;
	    }
	    if (rswap[icmax - 1]) {
		br2 = b[b_dim1 + 1];
		br1 = b[b_dim1 + 2];
		bi2 = b[(b_dim1 << 1) + 1];
		bi1 = b[(b_dim1 << 1) + 2];
	    } else {
		br1 = b[b_dim1 + 1];
		br2 = b[b_dim1 + 2];
		bi1 = b[(b_dim1 << 1) + 1];
		bi2 = b[(b_dim1 << 1) + 2];
	    }
	    br2 = br2 - lr21 * br1 + li21 * bi1;
	    bi2 = bi2 - li21 * br1 - lr21 * bi1;
/* Computing MAX */
	    d__1 = (abs(br1) + abs(bi1)) * (u22abs * (abs(ur11r) + abs(ui11r))
		    ), d__2 = abs(br2) + abs(bi2);
	    bbnd = std::max(d__1,d__2);
	    if (bbnd > 1. && u22abs < 1.) {
		if (bbnd >= bignum * u22abs) {
		    *scale = 1. / bbnd;
		    br1 = *scale * br1;
		    bi1 = *scale * bi1;
		    br2 = *scale * br2;
		    bi2 = *scale * bi2;
		}
	    }

	    dladiv_(&br2, &bi2, &ur22, &ui22, &xr2, &xi2);
	    xr1 = ur11r * br1 - ui11r * bi1 - ur12s * xr2 + ui12s * xi2;
	    xi1 = ui11r * br1 + ur11r * bi1 - ui12s * xr2 - ur12s * xi2;
	    if (zswap[icmax - 1]) {
		x[x_dim1 + 1] = xr2;
		x[x_dim1 + 2] = xr1;
		x[(x_dim1 << 1) + 1] = xi2;
		x[(x_dim1 << 1) + 2] = xi1;
	    } else {
		x[x_dim1 + 1] = xr1;
		x[x_dim1 + 2] = xr2;
		x[(x_dim1 << 1) + 1] = xi1;
		x[(x_dim1 << 1) + 2] = xi2;
	    }
/* Computing MAX */
	    d__1 = abs(xr1) + abs(xi1), d__2 = abs(xr2) + abs(xi2);
	    *xnorm = std::max(d__1,d__2);

/*           Further scaling if  norm(A) norm(X) > overflow */

	    if (*xnorm > 1. && cmax > 1.) {
		if (*xnorm > bignum / cmax) {
		    temp = cmax / bignum;
		    x[x_dim1 + 1] = temp * x[x_dim1 + 1];
		    x[x_dim1 + 2] = temp * x[x_dim1 + 2];
		    x[(x_dim1 << 1) + 1] = temp * x[(x_dim1 << 1) + 1];
		    x[(x_dim1 << 1) + 2] = temp * x[(x_dim1 << 1) + 2];
		    *xnorm = temp * *xnorm;
		    *scale = temp * *scale;
		}
	    }
	}
    }

    return 0;

/*     End of DLALN2 */

} /* dlaln2_ */

#undef crv
#undef civ
#undef cr
#undef ci

/* Subroutine */ int dlals0_(integer *icompq, integer *nl, integer *nr,
	integer *sqre, integer *nrhs, double *b, integer *ldb, double
	*bx, integer *ldbx, integer *perm, integer *givptr, integer *givcol,
	integer *ldgcol, double *givnum, integer *ldgnum, double *
	poles, double *difl, double *difr, double *z__, integer *
	k, double *c__, double *s, double *work, integer *info)
{
	/* Table of constant values */
	static double c_b5 = -1.;
	static integer c__1 = 1;
	static double c_b11 = 1.;
	static double c_b13 = 0.;
	static integer c__0 = 0;

    /* System generated locals */
    integer givcol_dim1, givcol_offset, b_dim1, b_offset, bx_dim1, bx_offset,
	    difr_dim1, difr_offset, givnum_dim1, givnum_offset, poles_dim1,
	    poles_offset, i__1, i__2;
    double d__1;

    /* Local variables */
    integer i__, j, m, n;
    double dj;
    integer nlp1;
    double temp;
    double diflj, difrj, dsigj;
     double dsigjp;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLALS0 applies back the multiplying factors of either the left or the */
/*  right singular vector matrix of a diagonal matrix appended by a row */
/*  to the right hand side matrix B in solving the least squares problem */
/*  using the divide-and-conquer SVD approach. */

/*  For the left singular vector matrix, three types of orthogonal */
/*  matrices are involved: */

/*  (1L) Givens rotations: the number of such rotations is GIVPTR; the */
/*       pairs of columns/rows they were applied to are stored in GIVCOL; */
/*       and the C- and S-values of these rotations are stored in GIVNUM. */

/*  (2L) Permutation. The (NL+1)-st row of B is to be moved to the first */
/*       row, and for J=2:N, PERM(J)-th row of B is to be moved to the */
/*       J-th row. */

/*  (3L) The left singular vector matrix of the remaining matrix. */

/*  For the right singular vector matrix, four types of orthogonal */
/*  matrices are involved: */

/*  (1R) The right singular vector matrix of the remaining matrix. */

/*  (2R) If SQRE = 1, one extra Givens rotation to generate the right */
/*       null space. */

/*  (3R) The inverse transformation of (2L). */

/*  (4R) The inverse transformation of (1L). */

/*  Arguments */
/*  ========= */

/*  ICOMPQ (input) INTEGER */
/*         Specifies whether singular vectors are to be computed in */
/*         factored form: */
/*         = 0: Left singular vector matrix. */
/*         = 1: Right singular vector matrix. */

/*  NL     (input) INTEGER */
/*         The row dimension of the upper block. NL >= 1. */

/*  NR     (input) INTEGER */
/*         The row dimension of the lower block. NR >= 1. */

/*  SQRE   (input) INTEGER */
/*         = 0: the lower block is an NR-by-NR square matrix. */
/*         = 1: the lower block is an NR-by-(NR+1) rectangular matrix. */

/*         The bidiagonal matrix has row dimension N = NL + NR + 1, */
/*         and column dimension M = N + SQRE. */

/*  NRHS   (input) INTEGER */
/*         The number of columns of B and BX. NRHS must be at least 1. */

/*  B      (input/output) DOUBLE PRECISION array, dimension ( LDB, NRHS ) */
/*         On input, B contains the right hand sides of the least */
/*         squares problem in rows 1 through M. On output, B contains */
/*         the solution X in rows 1 through N. */

/*  LDB    (input) INTEGER */
/*         The leading dimension of B. LDB must be at least */
/*         max(1,MAX( M, N ) ). */

/*  BX     (workspace) DOUBLE PRECISION array, dimension ( LDBX, NRHS ) */

/*  LDBX   (input) INTEGER */
/*         The leading dimension of BX. */

/*  PERM   (input) INTEGER array, dimension ( N ) */
/*         The permutations (from deflation and sorting) applied */
/*         to the two blocks. */

/*  GIVPTR (input) INTEGER */
/*         The number of Givens rotations which took place in this */
/*         subproblem. */

/*  GIVCOL (input) INTEGER array, dimension ( LDGCOL, 2 ) */
/*         Each pair of numbers indicates a pair of rows/columns */
/*         involved in a Givens rotation. */

/*  LDGCOL (input) INTEGER */
/*         The leading dimension of GIVCOL, must be at least N. */

/*  GIVNUM (input) DOUBLE PRECISION array, dimension ( LDGNUM, 2 ) */
/*         Each number indicates the C or S value used in the */
/*         corresponding Givens rotation. */

/*  LDGNUM (input) INTEGER */
/*         The leading dimension of arrays DIFR, POLES and */
/*         GIVNUM, must be at least K. */

/*  POLES  (input) DOUBLE PRECISION array, dimension ( LDGNUM, 2 ) */
/*         On entry, POLES(1:K, 1) contains the new singular */
/*         values obtained from solving the secular equation, and */
/*         POLES(1:K, 2) is an array containing the poles in the secular */
/*         equation. */

/*  DIFL   (input) DOUBLE PRECISION array, dimension ( K ). */
/*         On entry, DIFL(I) is the distance between I-th updated */
/*         (undeflated) singular value and the I-th (undeflated) old */
/*         singular value. */

/*  DIFR   (input) DOUBLE PRECISION array, dimension ( LDGNUM, 2 ). */
/*         On entry, DIFR(I, 1) contains the distances between I-th */
/*         updated (undeflated) singular value and the I+1-th */
/*         (undeflated) old singular value. And DIFR(I, 2) is the */
/*         normalizing factor for the I-th right singular vector. */

/*  Z      (input) DOUBLE PRECISION array, dimension ( K ) */
/*         Contain the components of the deflation-adjusted updating row */
/*         vector. */

/*  K      (input) INTEGER */
/*         Contains the dimension of the non-deflated matrix, */
/*         This is the order of the related secular equation. 1 <= K <=N. */

/*  C      (input) DOUBLE PRECISION */
/*         C contains garbage if SQRE =0 and the C-value of a Givens */
/*         rotation related to the right null space if SQRE = 1. */

/*  S      (input) DOUBLE PRECISION */
/*         S contains garbage if SQRE =0 and the S-value of a Givens */
/*         rotation related to the right null space if SQRE = 1. */

/*  WORK   (workspace) DOUBLE PRECISION array, dimension ( K ) */

/*  INFO   (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

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

/*     Test the input parameters. */

    /* Parameter adjustments */
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    bx_dim1 = *ldbx;
    bx_offset = 1 + bx_dim1;
    bx -= bx_offset;
    --perm;
    givcol_dim1 = *ldgcol;
    givcol_offset = 1 + givcol_dim1;
    givcol -= givcol_offset;
    difr_dim1 = *ldgnum;
    difr_offset = 1 + difr_dim1;
    difr -= difr_offset;
    poles_dim1 = *ldgnum;
    poles_offset = 1 + poles_dim1;
    poles -= poles_offset;
    givnum_dim1 = *ldgnum;
    givnum_offset = 1 + givnum_dim1;
    givnum -= givnum_offset;
    --difl;
    --z__;
    --work;

    /* Function Body */
    *info = 0;

    if (*icompq < 0 || *icompq > 1) {
	*info = -1;
    } else if (*nl < 1) {
	*info = -2;
    } else if (*nr < 1) {
	*info = -3;
    } else if (*sqre < 0 || *sqre > 1) {
	*info = -4;
    }

    n = *nl + *nr + 1;

    if (*nrhs < 1) {
	*info = -5;
    } else if (*ldb < n) {
	*info = -7;
    } else if (*ldbx < n) {
	*info = -9;
    } else if (*givptr < 0) {
	*info = -11;
    } else if (*ldgcol < n) {
	*info = -13;
    } else if (*ldgnum < n) {
	*info = -15;
    } else if (*k < 1) {
	*info = -20;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLALS0", &i__1);
	return 0;
    }

    m = n + *sqre;
    nlp1 = *nl + 1;

    if (*icompq == 0) {

/*        Apply back orthogonal transformations from the left. */

/*        Step (1L): apply back the Givens rotations performed. */

	i__1 = *givptr;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    drot_(nrhs, &b[givcol[i__ + (givcol_dim1 << 1)] + b_dim1], ldb, &
		    b[givcol[i__ + givcol_dim1] + b_dim1], ldb, &givnum[i__ +
		    (givnum_dim1 << 1)], &givnum[i__ + givnum_dim1]);
/* L10: */
	}

/*        Step (2L): permute rows of B. */

	dcopy_(nrhs, &b[nlp1 + b_dim1], ldb, &bx[bx_dim1 + 1], ldbx);
	i__1 = n;
	for (i__ = 2; i__ <= i__1; ++i__) {
	    dcopy_(nrhs, &b[perm[i__] + b_dim1], ldb, &bx[i__ + bx_dim1],
		    ldbx);
/* L20: */
	}

/*        Step (3L): apply the inverse of the left singular vector */
/*        matrix to BX. */

	if (*k == 1) {
	    dcopy_(nrhs, &bx[bx_offset], ldbx, &b[b_offset], ldb);
	    if (z__[1] < 0.) {
		dscal_(nrhs, &c_b5, &b[b_offset], ldb);
	    }
	} else {
	    i__1 = *k;
	    for (j = 1; j <= i__1; ++j) {
		diflj = difl[j];
		dj = poles[j + poles_dim1];
		dsigj = -poles[j + (poles_dim1 << 1)];
		if (j < *k) {
		    difrj = -difr[j + difr_dim1];
		    dsigjp = -poles[j + 1 + (poles_dim1 << 1)];
		}
		if (z__[j] == 0. || poles[j + (poles_dim1 << 1)] == 0.) {
		    work[j] = 0.;
		} else {
		    work[j] = -poles[j + (poles_dim1 << 1)] * z__[j] / diflj /
			     (poles[j + (poles_dim1 << 1)] + dj);
		}
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    if (z__[i__] == 0. || poles[i__ + (poles_dim1 << 1)] ==
			    0.) {
			work[i__] = 0.;
		    } else {
			work[i__] = poles[i__ + (poles_dim1 << 1)] * z__[i__]
				/ (dlamc3_(&poles[i__ + (poles_dim1 << 1)], &
				dsigj) - diflj) / (poles[i__ + (poles_dim1 <<
				1)] + dj);
		    }
/* L30: */
		}
		i__2 = *k;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    if (z__[i__] == 0. || poles[i__ + (poles_dim1 << 1)] ==
			    0.) {
			work[i__] = 0.;
		    } else {
			work[i__] = poles[i__ + (poles_dim1 << 1)] * z__[i__]
				/ (dlamc3_(&poles[i__ + (poles_dim1 << 1)], &
				dsigjp) + difrj) / (poles[i__ + (poles_dim1 <<
				 1)] + dj);
		    }
/* L40: */
		}
		work[1] = -1.;
		temp = dnrm2_(k, &work[1], &c__1);
		dgemv_("T", k, nrhs, &c_b11, &bx[bx_offset], ldbx, &work[1], &
			c__1, &c_b13, &b[j + b_dim1], ldb);
		dlascl_("G", &c__0, &c__0, &temp, &c_b11, &c__1, nrhs, &b[j +
			b_dim1], ldb, info);
/* L50: */
	    }
	}

/*        Move the deflated rows of BX to B also. */

	if (*k < std::max(m,n)) {
	    i__1 = n - *k;
	    dlacpy_("A", &i__1, nrhs, &bx[*k + 1 + bx_dim1], ldbx, &b[*k + 1
		    + b_dim1], ldb);
	}
    } else {

/*        Apply back the right orthogonal transformations. */

/*        Step (1R): apply back the new right singular vector matrix */
/*        to B. */

	if (*k == 1) {
	    dcopy_(nrhs, &b[b_offset], ldb, &bx[bx_offset], ldbx);
	} else {
	    i__1 = *k;
	    for (j = 1; j <= i__1; ++j) {
		dsigj = poles[j + (poles_dim1 << 1)];
		if (z__[j] == 0.) {
		    work[j] = 0.;
		} else {
		    work[j] = -z__[j] / difl[j] / (dsigj + poles[j +
			    poles_dim1]) / difr[j + (difr_dim1 << 1)];
		}
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    if (z__[j] == 0.) {
			work[i__] = 0.;
		    } else {
			d__1 = -poles[i__ + 1 + (poles_dim1 << 1)];
			work[i__] = z__[j] / (dlamc3_(&dsigj, &d__1) - difr[
				i__ + difr_dim1]) / (dsigj + poles[i__ +
				poles_dim1]) / difr[i__ + (difr_dim1 << 1)];
		    }
/* L60: */
		}
		i__2 = *k;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    if (z__[j] == 0.) {
			work[i__] = 0.;
		    } else {
			d__1 = -poles[i__ + (poles_dim1 << 1)];
			work[i__] = z__[j] / (dlamc3_(&dsigj, &d__1) - difl[
				i__]) / (dsigj + poles[i__ + poles_dim1]) /
				difr[i__ + (difr_dim1 << 1)];
		    }
/* L70: */
		}
		dgemv_("T", k, nrhs, &c_b11, &b[b_offset], ldb, &work[1], &
			c__1, &c_b13, &bx[j + bx_dim1], ldbx);
/* L80: */
	    }
	}

/*        Step (2R): if SQRE = 1, apply back the rotation that is */
/*        related to the right null space of the subproblem. */

	if (*sqre == 1) {
	    dcopy_(nrhs, &b[m + b_dim1], ldb, &bx[m + bx_dim1], ldbx);
	    drot_(nrhs, &bx[bx_dim1 + 1], ldbx, &bx[m + bx_dim1], ldbx, c__,
		    s);
	}
	if (*k < std::max(m,n)) {
	    i__1 = n - *k;
	    dlacpy_("A", &i__1, nrhs, &b[*k + 1 + b_dim1], ldb, &bx[*k + 1 +
		    bx_dim1], ldbx);
	}

/*        Step (3R): permute rows of B. */

	dcopy_(nrhs, &bx[bx_dim1 + 1], ldbx, &b[nlp1 + b_dim1], ldb);
	if (*sqre == 1) {
	    dcopy_(nrhs, &bx[m + bx_dim1], ldbx, &b[m + b_dim1], ldb);
	}
	i__1 = n;
	for (i__ = 2; i__ <= i__1; ++i__) {
	    dcopy_(nrhs, &bx[i__ + bx_dim1], ldbx, &b[perm[i__] + b_dim1],
		    ldb);
/* L90: */
	}

/*        Step (4R): apply back the Givens rotations performed. */

	for (i__ = *givptr; i__ >= 1; --i__) {
	    d__1 = -givnum[i__ + givnum_dim1];
	    drot_(nrhs, &b[givcol[i__ + (givcol_dim1 << 1)] + b_dim1], ldb, &
		    b[givcol[i__ + givcol_dim1] + b_dim1], ldb, &givnum[i__ +
		    (givnum_dim1 << 1)], &d__1);
/* L100: */
	}
    }

    return 0;

/*     End of DLALS0 */

} /* dlals0_ */

/* Subroutine */ int dlalsa_(integer *icompq, integer *smlsiz, integer *n,
	integer *nrhs, double *b, integer *ldb, double *bx, integer *
	ldbx, double *u, integer *ldu, double *vt, integer *k,
	double *difl, double *difr, double *z__, double *
	poles, integer *givptr, integer *givcol, integer *ldgcol, integer *
	perm, double *givnum, double *c__, double *s, double *
	work, integer *iwork, integer *info)
{
	/* Table of constant values */
	static double c_b7 = 1.;
	static double c_b8 = 0.;
	static integer c__2 = 2;

    /* System generated locals */
    integer givcol_dim1, givcol_offset, perm_dim1, perm_offset, b_dim1,
	    b_offset, bx_dim1, bx_offset, difl_dim1, difl_offset, difr_dim1,
	    difr_offset, givnum_dim1, givnum_offset, poles_dim1, poles_offset,
	     u_dim1, u_offset, vt_dim1, vt_offset, z_dim1, z_offset, i__1,
	    i__2;

    /* Local variables */
    integer i__, j, i1, ic, lf, nd, ll, nl, nr, im1, nlf, nrf, lvl, ndb1,
	    nlp1, lvl2, nrp1, nlvl, sqre;
     integer inode, ndiml, ndimr;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLALSA is an itermediate step in solving the least squares problem */
/*  by computing the SVD of the coefficient matrix in compact form (The */
/*  singular vectors are computed as products of simple orthorgonal */
/*  matrices.). */

/*  If ICOMPQ = 0, DLALSA applies the inverse of the left singular vector */
/*  matrix of an upper bidiagonal matrix to the right hand side; and if */
/*  ICOMPQ = 1, DLALSA applies the right singular vector matrix to the */
/*  right hand side. The singular vector matrices were generated in */
/*  compact form by DLALSA. */

/*  Arguments */
/*  ========= */


/*  ICOMPQ (input) INTEGER */
/*         Specifies whether the left or the right singular vector */
/*         matrix is involved. */
/*         = 0: Left singular vector matrix */
/*         = 1: Right singular vector matrix */

/*  SMLSIZ (input) INTEGER */
/*         The maximum size of the subproblems at the bottom of the */
/*         computation tree. */

/*  N      (input) INTEGER */
/*         The row and column dimensions of the upper bidiagonal matrix. */

/*  NRHS   (input) INTEGER */
/*         The number of columns of B and BX. NRHS must be at least 1. */

/*  B      (input/output) DOUBLE PRECISION array, dimension ( LDB, NRHS ) */
/*         On input, B contains the right hand sides of the least */
/*         squares problem in rows 1 through M. */
/*         On output, B contains the solution X in rows 1 through N. */

/*  LDB    (input) INTEGER */
/*         The leading dimension of B in the calling subprogram. */
/*         LDB must be at least max(1,MAX( M, N ) ). */

/*  BX     (output) DOUBLE PRECISION array, dimension ( LDBX, NRHS ) */
/*         On exit, the result of applying the left or right singular */
/*         vector matrix to B. */

/*  LDBX   (input) INTEGER */
/*         The leading dimension of BX. */

/*  U      (input) DOUBLE PRECISION array, dimension ( LDU, SMLSIZ ). */
/*         On entry, U contains the left singular vector matrices of all */
/*         subproblems at the bottom level. */

/*  LDU    (input) INTEGER, LDU = > N. */
/*         The leading dimension of arrays U, VT, DIFL, DIFR, */
/*         POLES, GIVNUM, and Z. */

/*  VT     (input) DOUBLE PRECISION array, dimension ( LDU, SMLSIZ+1 ). */
/*         On entry, VT' contains the right singular vector matrices of */
/*         all subproblems at the bottom level. */

/*  K      (input) INTEGER array, dimension ( N ). */

/*  DIFL   (input) DOUBLE PRECISION array, dimension ( LDU, NLVL ). */
/*         where NLVL = INT(log_2 (N/(SMLSIZ+1))) + 1. */

/*  DIFR   (input) DOUBLE PRECISION array, dimension ( LDU, 2 * NLVL ). */
/*         On entry, DIFL(*, I) and DIFR(*, 2 * I -1) record */
/*         distances between singular values on the I-th level and */
/*         singular values on the (I -1)-th level, and DIFR(*, 2 * I) */
/*         record the normalizing factors of the right singular vectors */
/*         matrices of subproblems on I-th level. */

/*  Z      (input) DOUBLE PRECISION array, dimension ( LDU, NLVL ). */
/*         On entry, Z(1, I) contains the components of the deflation- */
/*         adjusted updating row vector for subproblems on the I-th */
/*         level. */

/*  POLES  (input) DOUBLE PRECISION array, dimension ( LDU, 2 * NLVL ). */
/*         On entry, POLES(*, 2 * I -1: 2 * I) contains the new and old */
/*         singular values involved in the secular equations on the I-th */
/*         level. */

/*  GIVPTR (input) INTEGER array, dimension ( N ). */
/*         On entry, GIVPTR( I ) records the number of Givens */
/*         rotations performed on the I-th problem on the computation */
/*         tree. */

/*  GIVCOL (input) INTEGER array, dimension ( LDGCOL, 2 * NLVL ). */
/*         On entry, for each I, GIVCOL(*, 2 * I - 1: 2 * I) records the */
/*         locations of Givens rotations performed on the I-th level on */
/*         the computation tree. */

/*  LDGCOL (input) INTEGER, LDGCOL = > N. */
/*         The leading dimension of arrays GIVCOL and PERM. */

/*  PERM   (input) INTEGER array, dimension ( LDGCOL, NLVL ). */
/*         On entry, PERM(*, I) records permutations done on the I-th */
/*         level of the computation tree. */

/*  GIVNUM (input) DOUBLE PRECISION array, dimension ( LDU, 2 * NLVL ). */
/*         On entry, GIVNUM(*, 2 *I -1 : 2 * I) records the C- and S- */
/*         values of Givens rotations performed on the I-th level on the */
/*         computation tree. */

/*  C      (input) DOUBLE PRECISION array, dimension ( N ). */
/*         On entry, if the I-th subproblem is not square, */
/*         C( I ) contains the C-value of a Givens rotation related to */
/*         the right null space of the I-th subproblem. */

/*  S      (input) DOUBLE PRECISION array, dimension ( N ). */
/*         On entry, if the I-th subproblem is not square, */
/*         S( I ) contains the S-value of a Givens rotation related to */
/*         the right null space of the I-th subproblem. */

/*  WORK   (workspace) DOUBLE PRECISION array. */
/*         The dimension must be at least N. */

/*  IWORK  (workspace) INTEGER array. */
/*         The dimension must be at least 3 * N */

/*  INFO   (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

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
/*     .. Executable Statements .. */

/*     Test the input parameters. */

    /* Parameter adjustments */
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    bx_dim1 = *ldbx;
    bx_offset = 1 + bx_dim1;
    bx -= bx_offset;
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
    } else if (*n < *smlsiz) {
	*info = -3;
    } else if (*nrhs < 1) {
	*info = -4;
    } else if (*ldb < *n) {
	*info = -6;
    } else if (*ldbx < *n) {
	*info = -8;
    } else if (*ldu < *n) {
	*info = -10;
    } else if (*ldgcol < *n) {
	*info = -19;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLALSA", &i__1);
	return 0;
    }

/*     Book-keeping and  setting up the computation tree. */

    inode = 1;
    ndiml = inode + *n;
    ndimr = ndiml + *n;

    dlasdt_(n, &nlvl, &nd, &iwork[inode], &iwork[ndiml], &iwork[ndimr],
	    smlsiz);

/*     The following code applies back the left singular vector factors. */
/*     For applying back the right singular vector factors, go to 50. */

    if (*icompq == 1) {
	goto L50;
    }

/*     The nodes on the bottom level of the tree were solved */
/*     by DLASDQ. The corresponding left and right singular vector */
/*     matrices are in explicit form. First apply back the left */
/*     singular vector matrices. */

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
	nr = iwork[ndimr + i1];
	nlf = ic - nl;
	nrf = ic + 1;
	dgemm_("T", "N", &nl, nrhs, &nl, &c_b7, &u[nlf + u_dim1], ldu, &b[nlf
		+ b_dim1], ldb, &c_b8, &bx[nlf + bx_dim1], ldbx);
	dgemm_("T", "N", &nr, nrhs, &nr, &c_b7, &u[nrf + u_dim1], ldu, &b[nrf
		+ b_dim1], ldb, &c_b8, &bx[nrf + bx_dim1], ldbx);
/* L10: */
    }

/*     Next copy the rows of B that correspond to unchanged rows */
/*     in the bidiagonal matrix to BX. */

    i__1 = nd;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ic = iwork[inode + i__ - 1];
	dcopy_(nrhs, &b[ic + b_dim1], ldb, &bx[ic + bx_dim1], ldbx);
/* L20: */
    }

/*     Finally go through the left singular vector matrices of all */
/*     the other subproblems bottom-up on the tree. */

    j = pow_ii(&c__2, &nlvl);
    sqre = 0;

    for (lvl = nlvl; lvl >= 1; --lvl) {
	lvl2 = (lvl << 1) - 1;

/*        find the first node LF and last node LL on */
/*        the current level LVL */

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
	    --j;
	    dlals0_(icompq, &nl, &nr, &sqre, nrhs, &bx[nlf + bx_dim1], ldbx, &
		    b[nlf + b_dim1], ldb, &perm[nlf + lvl * perm_dim1], &
		    givptr[j], &givcol[nlf + lvl2 * givcol_dim1], ldgcol, &
		    givnum[nlf + lvl2 * givnum_dim1], ldu, &poles[nlf + lvl2 *
		     poles_dim1], &difl[nlf + lvl * difl_dim1], &difr[nlf +
		    lvl2 * difr_dim1], &z__[nlf + lvl * z_dim1], &k[j], &c__[
		    j], &s[j], &work[1], info);
/* L30: */
	}
/* L40: */
    }
    goto L90;

/*     ICOMPQ = 1: applying back the right singular vector factors. */

L50:

/*     First now go through the right singular vector matrices of all */
/*     the tree nodes top-down. */

    j = 0;
    i__1 = nlvl;
    for (lvl = 1; lvl <= i__1; ++lvl) {
	lvl2 = (lvl << 1) - 1;

/*        Find the first node LF and last node LL on */
/*        the current level LVL. */

	if (lvl == 1) {
	    lf = 1;
	    ll = 1;
	} else {
	    i__2 = lvl - 1;
	    lf = pow_ii(&c__2, &i__2);
	    ll = (lf << 1) - 1;
	}
	i__2 = lf;
	for (i__ = ll; i__ >= i__2; --i__) {
	    im1 = i__ - 1;
	    ic = iwork[inode + im1];
	    nl = iwork[ndiml + im1];
	    nr = iwork[ndimr + im1];
	    nlf = ic - nl;
	    nrf = ic + 1;
	    if (i__ == ll) {
		sqre = 0;
	    } else {
		sqre = 1;
	    }
	    ++j;
	    dlals0_(icompq, &nl, &nr, &sqre, nrhs, &b[nlf + b_dim1], ldb, &bx[
		    nlf + bx_dim1], ldbx, &perm[nlf + lvl * perm_dim1], &
		    givptr[j], &givcol[nlf + lvl2 * givcol_dim1], ldgcol, &
		    givnum[nlf + lvl2 * givnum_dim1], ldu, &poles[nlf + lvl2 *
		     poles_dim1], &difl[nlf + lvl * difl_dim1], &difr[nlf +
		    lvl2 * difr_dim1], &z__[nlf + lvl * z_dim1], &k[j], &c__[
		    j], &s[j], &work[1], info);
/* L60: */
	}
/* L70: */
    }

/*     The nodes on the bottom level of the tree were solved */
/*     by DLASDQ. The corresponding right singular vector */
/*     matrices are in explicit form. Apply them back. */

    ndb1 = (nd + 1) / 2;
    i__1 = nd;
    for (i__ = ndb1; i__ <= i__1; ++i__) {
	i1 = i__ - 1;
	ic = iwork[inode + i1];
	nl = iwork[ndiml + i1];
	nr = iwork[ndimr + i1];
	nlp1 = nl + 1;
	if (i__ == nd) {
	    nrp1 = nr;
	} else {
	    nrp1 = nr + 1;
	}
	nlf = ic - nl;
	nrf = ic + 1;
	dgemm_("T", "N", &nlp1, nrhs, &nlp1, &c_b7, &vt[nlf + vt_dim1], ldu, &
		b[nlf + b_dim1], ldb, &c_b8, &bx[nlf + bx_dim1], ldbx);
	dgemm_("T", "N", &nrp1, nrhs, &nrp1, &c_b7, &vt[nrf + vt_dim1], ldu, &
		b[nrf + b_dim1], ldb, &c_b8, &bx[nrf + bx_dim1], ldbx);
/* L80: */
    }

L90:

    return 0;

/*     End of DLALSA */

} /* dlalsa_ */

/* Subroutine */ int dlalsd_(const char *uplo, integer *smlsiz, integer *n, integer
	*nrhs, double *d__, double *e, double *b, integer *ldb,
	double *rcond, integer *rank, double *work, integer *iwork,
	integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b6 = 0.;
	static integer c__0 = 0;
	static double c_b11 = 1.;

    /* System generated locals */
    integer b_dim1, b_offset, i__1, i__2;
    double d__1;

    /* Local variables */
    integer c__, i__, j, k;
    double r__;
    integer s, u, z__;
    double cs;
    integer bx;
    double sn;
    integer st, vt, nm1, st1;
    double eps;
    integer iwk;
    double tol;
    integer difl, difr;
    double rcnd;
    integer perm, nsub;
     integer nlvl, sqre, bxst;
    integer poles, sizei, nsize, nwork, icmpq1, icmpq2;
    integer givcol;
    double orgnrm;
    integer givnum, givptr, smlszp;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLALSD uses the singular value decomposition of A to solve the least */
/*  squares problem of finding X to minimize the Euclidean norm of each */
/*  column of A*X-B, where A is N-by-N upper bidiagonal, and X and B */
/*  are N-by-NRHS. The solution X overwrites B. */

/*  The singular values of A smaller than RCOND times the largest */
/*  singular value are treated as zero in solving the least squares */
/*  problem; in this case a minimum norm solution is returned. */
/*  The actual singular values are returned in D in ascending order. */

/*  This code makes very mild assumptions about floating point */
/*  arithmetic. It will work on machines with a guard digit in */
/*  add/subtract, or on those binary machines without guard digits */
/*  which subtract like the Cray XMP, Cray YMP, Cray C 90, or Cray 2. */
/*  It could conceivably fail on hexadecimal or decimal machines */
/*  without guard digits, but we know of none. */

/*  Arguments */
/*  ========= */

/*  UPLO   (input) CHARACTER*1 */
/*         = 'U': D and E define an upper bidiagonal matrix. */
/*         = 'L': D and E define a  lower bidiagonal matrix. */

/*  SMLSIZ (input) INTEGER */
/*         The maximum size of the subproblems at the bottom of the */
/*         computation tree. */

/*  N      (input) INTEGER */
/*         The dimension of the  bidiagonal matrix.  N >= 0. */

/*  NRHS   (input) INTEGER */
/*         The number of columns of B. NRHS must be at least 1. */

/*  D      (input/output) DOUBLE PRECISION array, dimension (N) */
/*         On entry D contains the main diagonal of the bidiagonal */
/*         matrix. On exit, if INFO = 0, D contains its singular values. */

/*  E      (input/output) DOUBLE PRECISION array, dimension (N-1) */
/*         Contains the super-diagonal entries of the bidiagonal matrix. */
/*         On exit, E has been destroyed. */

/*  B      (input/output) DOUBLE PRECISION array, dimension (LDB,NRHS) */
/*         On input, B contains the right hand sides of the least */
/*         squares problem. On output, B contains the solution X. */

/*  LDB    (input) INTEGER */
/*         The leading dimension of B in the calling subprogram. */
/*         LDB must be at least max(1,N). */

/*  RCOND  (input) DOUBLE PRECISION */
/*         The singular values of A less than or equal to RCOND times */
/*         the largest singular value are treated as zero in solving */
/*         the least squares problem. If RCOND is negative, */
/*         machine precision is used instead. */
/*         For example, if diag(S)*X=B were the least squares problem, */
/*         where diag(S) is a diagonal matrix of singular values, the */
/*         solution would be X(i) = B(i) / S(i) if S(i) is greater than */
/*         RCOND*max(S), and X(i) = 0 if S(i) is less than or equal to */
/*         RCOND*max(S). */

/*  RANK   (output) INTEGER */
/*         The number of singular values of A greater than RCOND times */
/*         the largest singular value. */

/*  WORK   (workspace) DOUBLE PRECISION array, dimension at least */
/*         (9*N + 2*N*SMLSIZ + 8*N*NLVL + N*NRHS + (SMLSIZ+1)**2), */
/*         where NLVL = max(0, INT(log_2 (N/(SMLSIZ+1))) + 1). */

/*  IWORK  (workspace) INTEGER array, dimension at least */
/*         (3*N*NLVL + 11*N) */

/*  INFO   (output) INTEGER */
/*         = 0:  successful exit. */
/*         < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*         > 0:  The algorithm failed to compute an singular value while */
/*               working on the submatrix lying in rows and columns */
/*               INFO/(N+1) through MOD(INFO,N+1). */

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
    b_dim1 = *ldb;
    b_offset = 1 + b_dim1;
    b -= b_offset;
    --work;
    --iwork;

    /* Function Body */
    *info = 0;

    if (*n < 0) {
	*info = -3;
    } else if (*nrhs < 1) {
	*info = -4;
    } else if (*ldb < 1 || *ldb < *n) {
	*info = -8;
    }
    if (*info != 0) {
	i__1 = -(*info);
	xerbla_("DLALSD", &i__1);
	return 0;
    }

    eps = dlamch_("Epsilon");

/*     Set up the tolerance. */

    if (*rcond <= 0. || *rcond >= 1.) {
	rcnd = eps;
    } else {
	rcnd = *rcond;
    }

    *rank = 0;

/*     Quick return if possible. */

    if (*n == 0) {
	return 0;
    } else if (*n == 1) {
	if (d__[1] == 0.) {
	    dlaset_("A", &c__1, nrhs, &c_b6, &c_b6, &b[b_offset], ldb);
	} else {
	    *rank = 1;
	    dlascl_("G", &c__0, &c__0, &d__[1], &c_b11, &c__1, nrhs, &b[
		    b_offset], ldb, info);
	    d__[1] = abs(d__[1]);
	}
	return 0;
    }

/*     Rotate the matrix if it is lower bidiagonal. */

    if (*(unsigned char *)uplo == 'L') {
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    dlartg_(&d__[i__], &e[i__], &cs, &sn, &r__);
	    d__[i__] = r__;
	    e[i__] = sn * d__[i__ + 1];
	    d__[i__ + 1] = cs * d__[i__ + 1];
	    if (*nrhs == 1) {
		drot_(&c__1, &b[i__ + b_dim1], &c__1, &b[i__ + 1 + b_dim1], &
			c__1, &cs, &sn);
	    } else {
		work[(i__ << 1) - 1] = cs;
		work[i__ * 2] = sn;
	    }
/* L10: */
	}
	if (*nrhs > 1) {
	    i__1 = *nrhs;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		i__2 = *n - 1;
		for (j = 1; j <= i__2; ++j) {
		    cs = work[(j << 1) - 1];
		    sn = work[j * 2];
		    drot_(&c__1, &b[j + i__ * b_dim1], &c__1, &b[j + 1 + i__ *
			     b_dim1], &c__1, &cs, &sn);
/* L20: */
		}
/* L30: */
	    }
	}
    }

/*     Scale. */

    nm1 = *n - 1;
    orgnrm = dlanst_("M", n, &d__[1], &e[1]);
    if (orgnrm == 0.) {
	dlaset_("A", n, nrhs, &c_b6, &c_b6, &b[b_offset], ldb);
	return 0;
    }

    dlascl_("G", &c__0, &c__0, &orgnrm, &c_b11, n, &c__1, &d__[1], n, info);
    dlascl_("G", &c__0, &c__0, &orgnrm, &c_b11, &nm1, &c__1, &e[1], &nm1,
	    info);

/*     If N is smaller than the minimum divide size SMLSIZ, then solve */
/*     the problem with another solver. */

    if (*n <= *smlsiz) {
	nwork = *n * *n + 1;
	dlaset_("A", n, n, &c_b6, &c_b11, &work[1], n);
	dlasdq_("U", &c__0, n, n, &c__0, nrhs, &d__[1], &e[1], &work[1], n, &
		work[1], n, &b[b_offset], ldb, &work[nwork], info);
	if (*info != 0) {
	    return 0;
	}
	tol = rcnd * (d__1 = d__[idamax_(n, &d__[1], &c__1)], abs(d__1));
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (d__[i__] <= tol) {
		dlaset_("A", &c__1, nrhs, &c_b6, &c_b6, &b[i__ + b_dim1], ldb);
	    } else {
		dlascl_("G", &c__0, &c__0, &d__[i__], &c_b11, &c__1, nrhs, &b[
			i__ + b_dim1], ldb, info);
		++(*rank);
	    }
/* L40: */
	}
	dgemm_("T", "N", n, nrhs, n, &c_b11, &work[1], n, &b[b_offset], ldb, &
		c_b6, &work[nwork], n);
	dlacpy_("A", n, nrhs, &work[nwork], n, &b[b_offset], ldb);

/*        Unscale. */

	dlascl_("G", &c__0, &c__0, &c_b11, &orgnrm, n, &c__1, &d__[1], n,
		info);
	dlasrt_("D", n, &d__[1], info);
	dlascl_("G", &c__0, &c__0, &orgnrm, &c_b11, n, nrhs, &b[b_offset],
		ldb, info);

	return 0;
    }

/*     Book-keeping and setting up some constants. */

    nlvl = (integer) (log((double) (*n) / (double) (*smlsiz + 1)) /
	    log(2.)) + 1;

    smlszp = *smlsiz + 1;

    u = 1;
    vt = *smlsiz * *n + 1;
    difl = vt + smlszp * *n;
    difr = difl + nlvl * *n;
    z__ = difr + (nlvl * *n << 1);
    c__ = z__ + nlvl * *n;
    s = c__ + *n;
    poles = s + *n;
    givnum = poles + (nlvl << 1) * *n;
    bx = givnum + (nlvl << 1) * *n;
    nwork = bx + *n * *nrhs;

    sizei = *n + 1;
    k = sizei + *n;
    givptr = k + *n;
    perm = givptr + *n;
    givcol = perm + nlvl * *n;
    iwk = givcol + (nlvl * *n << 1);

    st = 1;
    sqre = 0;
    icmpq1 = 1;
    icmpq2 = 0;
    nsub = 0;

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = d__[i__], abs(d__1)) < eps) {
	    d__[i__] = d_sign(&eps, &d__[i__]);
	}
/* L50: */
    }

    i__1 = nm1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if ((d__1 = e[i__], abs(d__1)) < eps || i__ == nm1) {
	    ++nsub;
	    iwork[nsub] = st;

/*           Subproblem found. First determine its size and then */
/*           apply divide and conquer on it. */

	    if (i__ < nm1) {

/*              A subproblem with E(I) small for I < NM1. */

		nsize = i__ - st + 1;
		iwork[sizei + nsub - 1] = nsize;
	    } else if ((d__1 = e[i__], abs(d__1)) >= eps) {

/*              A subproblem with E(NM1) not too small but I = NM1. */

		nsize = *n - st + 1;
		iwork[sizei + nsub - 1] = nsize;
	    } else {

/*              A subproblem with E(NM1) small. This implies an */
/*              1-by-1 subproblem at D(N), which is not solved */
/*              explicitly. */

		nsize = i__ - st + 1;
		iwork[sizei + nsub - 1] = nsize;
		++nsub;
		iwork[nsub] = *n;
		iwork[sizei + nsub - 1] = 1;
		dcopy_(nrhs, &b[*n + b_dim1], ldb, &work[bx + nm1], n);
	    }
	    st1 = st - 1;
	    if (nsize == 1) {

/*              This is a 1-by-1 subproblem and is not solved */
/*              explicitly. */

		dcopy_(nrhs, &b[st + b_dim1], ldb, &work[bx + st1], n);
	    } else if (nsize <= *smlsiz) {

/*              This is a small subproblem and is solved by DLASDQ. */

		dlaset_("A", &nsize, &nsize, &c_b6, &c_b11, &work[vt + st1],
			n);
		dlasdq_("U", &c__0, &nsize, &nsize, &c__0, nrhs, &d__[st], &e[
			st], &work[vt + st1], n, &work[nwork], n, &b[st +
			b_dim1], ldb, &work[nwork], info);
		if (*info != 0) {
		    return 0;
		}
		dlacpy_("A", &nsize, nrhs, &b[st + b_dim1], ldb, &work[bx +
			st1], n);
	    } else {

/*              A large problem. Solve it using divide and conquer. */

		dlasda_(&icmpq1, smlsiz, &nsize, &sqre, &d__[st], &e[st], &
			work[u + st1], n, &work[vt + st1], &iwork[k + st1], &
			work[difl + st1], &work[difr + st1], &work[z__ + st1],
			 &work[poles + st1], &iwork[givptr + st1], &iwork[
			givcol + st1], n, &iwork[perm + st1], &work[givnum +
			st1], &work[c__ + st1], &work[s + st1], &work[nwork],
			&iwork[iwk], info);
		if (*info != 0) {
		    return 0;
		}
		bxst = bx + st1;
		dlalsa_(&icmpq2, smlsiz, &nsize, nrhs, &b[st + b_dim1], ldb, &
			work[bxst], n, &work[u + st1], n, &work[vt + st1], &
			iwork[k + st1], &work[difl + st1], &work[difr + st1],
			&work[z__ + st1], &work[poles + st1], &iwork[givptr +
			st1], &iwork[givcol + st1], n, &iwork[perm + st1], &
			work[givnum + st1], &work[c__ + st1], &work[s + st1],
			&work[nwork], &iwork[iwk], info);
		if (*info != 0) {
		    return 0;
		}
	    }
	    st = i__ + 1;
	}
/* L60: */
    }

/*     Apply the singular values and treat the tiny ones as zero. */

    tol = rcnd * (d__1 = d__[idamax_(n, &d__[1], &c__1)], abs(d__1));

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {

/*        Some of the elements in D can be negative because 1-by-1 */
/*        subproblems were not solved explicitly. */

	if ((d__1 = d__[i__], abs(d__1)) <= tol) {
	    dlaset_("A", &c__1, nrhs, &c_b6, &c_b6, &work[bx + i__ - 1], n);
	} else {
	    ++(*rank);
	    dlascl_("G", &c__0, &c__0, &d__[i__], &c_b11, &c__1, nrhs, &work[
		    bx + i__ - 1], n, info);
	}
	d__[i__] = (d__1 = d__[i__], abs(d__1));
/* L70: */
    }

/*     Now apply back the right singular vectors. */

    icmpq2 = 1;
    i__1 = nsub;
    for (i__ = 1; i__ <= i__1; ++i__) {
	st = iwork[i__];
	st1 = st - 1;
	nsize = iwork[sizei + i__ - 1];
	bxst = bx + st1;
	if (nsize == 1) {
	    dcopy_(nrhs, &work[bxst], n, &b[st + b_dim1], ldb);
	} else if (nsize <= *smlsiz) {
	    dgemm_("T", "N", &nsize, nrhs, &nsize, &c_b11, &work[vt + st1], n,
		     &work[bxst], n, &c_b6, &b[st + b_dim1], ldb);
	} else {
	    dlalsa_(&icmpq2, smlsiz, &nsize, nrhs, &work[bxst], n, &b[st +
		    b_dim1], ldb, &work[u + st1], n, &work[vt + st1], &iwork[
		    k + st1], &work[difl + st1], &work[difr + st1], &work[z__
		    + st1], &work[poles + st1], &iwork[givptr + st1], &iwork[
		    givcol + st1], n, &iwork[perm + st1], &work[givnum + st1],
		     &work[c__ + st1], &work[s + st1], &work[nwork], &iwork[
		    iwk], info);
	    if (*info != 0) {
		return 0;
	    }
	}
/* L80: */
    }

/*     Unscale and sort the singular values. */

    dlascl_("G", &c__0, &c__0, &c_b11, &orgnrm, n, &c__1, &d__[1], n, info);
    dlasrt_("D", n, &d__[1], info);
    dlascl_("G", &c__0, &c__0, &orgnrm, &c_b11, n, nrhs, &b[b_offset], ldb,
	    info);

    return 0;

/*     End of DLALSD */

} /* dlalsd_ */

double dlamch_(const char *cmach)
{
	/* Table of constant values 
	static double c_b32 = 0.;*/

    /* Initialized data */
    static bool first = true;

    /* System generated locals */
    integer i__1;
    double ret_val;

    /* Local variables */
    static double t;
    integer it;
    static double rnd, eps, base;
    integer beta;
    static double emin, prec, emax;
    integer imin, imax;
    bool lrnd;
    static double rmin, rmax;
    double rmach;

    double small;
    static double sfmin;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAMCH determines double precision machine parameters. */

/*  Arguments */
/*  ========= */

/*  CMACH   (input) CHARACTER*1 */
/*          Specifies the value to be returned by DLAMCH: */
/*          = 'E' or 'e',   DLAMCH := eps */
/*          = 'S' or 's ,   DLAMCH := sfmin */
/*          = 'B' or 'b',   DLAMCH := base */
/*          = 'P' or 'p',   DLAMCH := eps*base */
/*          = 'N' or 'n',   DLAMCH := t */
/*          = 'R' or 'r',   DLAMCH := rnd */
/*          = 'M' or 'm',   DLAMCH := emin */
/*          = 'U' or 'u',   DLAMCH := rmin */
/*          = 'L' or 'l',   DLAMCH := emax */
/*          = 'O' or 'o',   DLAMCH := rmax */

/*          where */

/*          eps   = relative machine precision */
/*          sfmin = safe minimum, such that 1/sfmin does not overflow */
/*          base  = base of the machine */
/*          prec  = eps*base */
/*          t     = number of (base) digits in the mantissa */
/*          rnd   = 1.0 when rounding occurs in addition, 0.0 otherwise */
/*          emin  = minimum exponent before (gradual) underflow */
/*          rmin  = underflow threshold - base**(emin-1) */
/*          emax  = largest exponent before overflow */
/*          rmax  = overflow threshold  - (base**emax)*(1-eps) */

/* ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Save statement .. */
/*     .. */
/*     .. Data statements .. */
/*     .. */
/*     .. Executable Statements .. */

    if (first) {
	dlamc2_(&beta, &it, &lrnd, &eps, &imin, &rmin, &imax, &rmax);
	base = (double) beta;
	t = (double) it;
	if (lrnd) {
	    rnd = 1.;
	    i__1 = 1 - it;
	    eps = pow_di(&base, &i__1) / 2;
	} else {
	    rnd = 0.;
	    i__1 = 1 - it;
	    eps = pow_di(&base, &i__1);
	}
	prec = eps * base;
	emin = (double) imin;
	emax = (double) imax;
	sfmin = rmin;
	small = 1. / rmax;
	if (small >= sfmin) {

/*           Use SMALL plus a bit, to avoid the possibility of rounding */
/*           causing overflow when computing  1/sfmin. */

	    sfmin = small * (eps + 1.);
	}
    }

    if (lsame_(cmach, "E")) {
	rmach = eps;
    } else if (lsame_(cmach, "S")) {
	rmach = sfmin;
    } else if (lsame_(cmach, "B")) {
	rmach = base;
    } else if (lsame_(cmach, "P")) {
	rmach = prec;
    } else if (lsame_(cmach, "N")) {
	rmach = t;
    } else if (lsame_(cmach, "R")) {
	rmach = rnd;
    } else if (lsame_(cmach, "M")) {
	rmach = emin;
    } else if (lsame_(cmach, "U")) {
	rmach = rmin;
    } else if (lsame_(cmach, "L")) {
	rmach = emax;
    } else if (lsame_(cmach, "O")) {
	rmach = rmax;
    }

    ret_val = rmach;
    first = false;
    return ret_val;

/*     End of DLAMCH */

} /* dlamch_ */


/* *********************************************************************** */

/* Subroutine */ int dlamc1_(integer *beta, integer *t, bool *rnd, bool
	*ieee1)
{
    /* Initialized data */

    static bool first = true;

    /* System generated locals */
    double d__1, d__2;

    /* Local variables */
    double a, b, c__, f, t1, t2;
    static integer lt;
    double one, qtr;
    static bool lrnd;
    static integer lbeta;
    double savec;

    static bool lieee1;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAMC1 determines the machine parameters given by BETA, T, RND, and */
/*  IEEE1. */

/*  Arguments */
/*  ========= */

/*  BETA    (output) INTEGER */
/*          The base of the machine. */

/*  T       (output) INTEGER */
/*          The number of ( BETA ) digits in the mantissa. */

/*  RND     (output) LOGICAL */
/*          Specifies whether proper rounding  ( RND = .TRUE. )  or */
/*          chopping  ( RND = .FALSE. )  occurs in addition. This may not */
/*          be a reliable guide to the way in which the machine performs */
/*          its arithmetic. */

/*  IEEE1   (output) LOGICAL */
/*          Specifies whether rounding appears to be done in the IEEE */
/*          'round to nearest' style. */

/*  Further Details */
/*  =============== */

/*  The routine is based on the routine  ENVRON  by Malcolm and */
/*  incorporates suggestions by Gentleman and Marovich. See */

/*     Malcolm M. A. (1972) Algorithms to reveal properties of */
/*        floating-point arithmetic. Comms. of the ACM, 15, 949-951. */

/*     Gentleman W. M. and Marovich S. B. (1974) More on algorithms */
/*        that reveal properties of floating point arithmetic units. */
/*        Comms. of the ACM, 17, 276-277. */

/* ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Save statement .. */
/*     .. */
/*     .. Data statements .. */
/*     .. */
/*     .. Executable Statements .. */

    if (first) {
	one = 1.;

/*        LBETA,  LIEEE1,  LT and  LRND  are the  local values  of  BETA, */
/*        IEEE1, T and RND. */

/*        Throughout this routine  we use the function  DLAMC3  to ensure */
/*        that relevant values are  stored and not held in registers,  or */
/*        are not affected by optimizers. */

/*        Compute  a = 2.0**m  with the  smallest positive integer m such */
/*        that */

/*           fl( a + 1.0 ) = a. */

	a = 1.;
	c__ = 1.;

/* +       WHILE( C.EQ.ONE )LOOP */
L10:
	if (c__ == one) {
	    a *= 2;
	    c__ = dlamc3_(&a, &one);
	    d__1 = -a;
	    c__ = dlamc3_(&c__, &d__1);
	    goto L10;
	}
/* +       END WHILE */

/*        Now compute  b = 2.0**m  with the smallest positive integer m */
/*        such that */

/*           fl( a + b ) .gt. a. */

	b = 1.;
	c__ = dlamc3_(&a, &b);

/* +       WHILE( C.EQ.A )LOOP */
L20:
	if (c__ == a) {
	    b *= 2;
	    c__ = dlamc3_(&a, &b);
	    goto L20;
	}
/* +       END WHILE */

/*        Now compute the base.  a and c  are neighbouring floating point */
/*        numbers  in the  interval  ( beta**t, beta**( t + 1 ) )  and so */
/*        their difference is beta. Adding 0.25 to c is to ensure that it */
/*        is truncated to beta and not ( beta - 1 ). */

	qtr = one / 4;
	savec = c__;
	d__1 = -a;
	c__ = dlamc3_(&c__, &d__1);
	lbeta = (integer) (c__ + qtr);

/*        Now determine whether rounding or chopping occurs,  by adding a */
/*        bit  less  than  beta/2  and a  bit  more  than  beta/2  to  a. */

	b = (double) lbeta;
	d__1 = b / 2;
	d__2 = -b / 100;
	f = dlamc3_(&d__1, &d__2);
	c__ = dlamc3_(&f, &a);
	if (c__ == a) {
	    lrnd = true;
	} else {
	    lrnd = false;
	}
	d__1 = b / 2;
	d__2 = b / 100;
	f = dlamc3_(&d__1, &d__2);
	c__ = dlamc3_(&f, &a);
	if (lrnd && c__ == a) {
	    lrnd = false;
	}

/*        Try and decide whether rounding is done in the  IEEE  'round to */
/*        nearest' style. B/2 is half a unit in the last place of the two */
/*        numbers A and SAVEC. Furthermore, A is even, i.e. has last  bit */
/*        zero, and SAVEC is odd. Thus adding B/2 to A should not  change */
/*        A, but adding B/2 to SAVEC should change SAVEC. */

	d__1 = b / 2;
	t1 = dlamc3_(&d__1, &a);
	d__1 = b / 2;
	t2 = dlamc3_(&d__1, &savec);
	lieee1 = t1 == a && t2 > savec && lrnd;

/*        Now find  the  mantissa, t.  It should  be the  integer part of */
/*        log to the base beta of a,  however it is safer to determine  t */
/*        by powering.  So we find t as the smallest positive integer for */
/*        which */

/*           fl( beta**t + 1.0 ) = 1.0. */

	lt = 0;
	a = 1.;
	c__ = 1.;

/* +       WHILE( C.EQ.ONE )LOOP */
L30:
	if (c__ == one) {
	    ++lt;
	    a *= lbeta;
	    c__ = dlamc3_(&a, &one);
	    d__1 = -a;
	    c__ = dlamc3_(&c__, &d__1);
	    goto L30;
	}
/* +       END WHILE */

    }

    *beta = lbeta;
    *t = lt;
    *rnd = lrnd;
    *ieee1 = lieee1;
    first = false;
    return 0;

/*     End of DLAMC1 */

} /* dlamc1_ */


/* *********************************************************************** */

/* Subroutine */ int dlamc2_(integer *beta, integer *t, bool *rnd,
	double *eps, integer *emin, double *rmin, integer *emax,
	double *rmax)
{
    /* Initialized data */

    static bool first = true;
    static bool iwarn = false;

    /* System generated locals */
    integer i__1;
    double d__1, d__2, d__3, d__4, d__5;

    /* Local variables */
    double a, b, c__;
    integer i__;
    static integer lt;
    double one, two;
    bool ieee;
    double half;
    bool lrnd;
    static double leps;
    double zero;
    static integer lbeta;
    double rbase;
    static integer lemin, lemax;
    integer gnmin;
    double small;
    integer gpmin;
    double third;
    static double lrmin, lrmax;
    double sixth;
    bool lieee1;
    integer ngnmin, ngpmin;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAMC2 determines the machine parameters specified in its argument */
/*  list. */

/*  Arguments */
/*  ========= */

/*  BETA    (output) INTEGER */
/*          The base of the machine. */

/*  T       (output) INTEGER */
/*          The number of ( BETA ) digits in the mantissa. */

/*  RND     (output) LOGICAL */
/*          Specifies whether proper rounding  ( RND = .TRUE. )  or */
/*          chopping  ( RND = .FALSE. )  occurs in addition. This may not */
/*          be a reliable guide to the way in which the machine performs */
/*          its arithmetic. */

/*  EPS     (output) DOUBLE PRECISION */
/*          The smallest positive number such that */

/*             fl( 1.0 - EPS ) .LT. 1.0, */

/*          where fl denotes the computed value. */

/*  EMIN    (output) INTEGER */
/*          The minimum exponent before (gradual) underflow occurs. */

/*  RMIN    (output) DOUBLE PRECISION */
/*          The smallest normalized number for the machine, given by */
/*          BASE**( EMIN - 1 ), where  BASE  is the floating point value */
/*          of BETA. */

/*  EMAX    (output) INTEGER */
/*          The maximum exponent before overflow occurs. */

/*  RMAX    (output) DOUBLE PRECISION */
/*          The largest positive number for the machine, given by */
/*          BASE**EMAX * ( 1 - EPS ), where  BASE  is the floating point */
/*          value of BETA. */

/*  Further Details */
/*  =============== */

/*  The computation of  EPS  is based on a routine PARANOIA by */
/*  W. Kahan of the University of California at Berkeley. */

/* ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Save statement .. */
/*     .. */
/*     .. Data statements .. */
/*     .. */
/*     .. Executable Statements .. */

    if (first) {
	zero = 0.;
	one = 1.;
	two = 2.;

/*        LBETA, LT, LRND, LEPS, LEMIN and LRMIN  are the local values of */
/*        BETA, T, RND, EPS, EMIN and RMIN. */

/*        Throughout this routine  we use the function  DLAMC3  to ensure */
/*        that relevant values are stored  and not held in registers,  or */
/*        are not affected by optimizers. */

/*        DLAMC1 returns the parameters  LBETA, LT, LRND and LIEEE1. */

	dlamc1_(&lbeta, &lt, &lrnd, &lieee1);

/*        Start to find EPS. */

	b = (double) lbeta;
	i__1 = -lt;
	a = pow_di(&b, &i__1);
	leps = a;

/*        Try some tricks to see whether or not this is the correct  EPS. */

	b = two / 3;
	half = one / 2;
	d__1 = -half;
	sixth = dlamc3_(&b, &d__1);
	third = dlamc3_(&sixth, &sixth);
	d__1 = -half;
	b = dlamc3_(&third, &d__1);
	b = dlamc3_(&b, &sixth);
	b = abs(b);
	if (b < leps) {
	    b = leps;
	}

	leps = 1.;

/* +       WHILE( ( LEPS.GT.B ).AND.( B.GT.ZERO ) )LOOP */
L10:
	if (leps > b && b > zero) {
	    leps = b;
	    d__1 = half * leps;
/* Computing 5th power */
	    d__3 = two, d__4 = d__3, d__3 *= d__3;
/* Computing 2nd power */
	    d__5 = leps;
	    d__2 = d__4 * (d__3 * d__3) * (d__5 * d__5);
	    c__ = dlamc3_(&d__1, &d__2);
	    d__1 = -c__;
	    c__ = dlamc3_(&half, &d__1);
	    b = dlamc3_(&half, &c__);
	    d__1 = -b;
	    c__ = dlamc3_(&half, &d__1);
	    b = dlamc3_(&half, &c__);
	    goto L10;
	}
/* +       END WHILE */

	if (a < leps) {
	    leps = a;
	}

/*        Computation of EPS complete. */

/*        Now find  EMIN.  Let A = + or - 1, and + or - (1 + BASE**(-3)). */
/*        Keep dividing  A by BETA until (gradual) underflow occurs. This */
/*        is detected when we cannot recover the previous A. */

	rbase = one / lbeta;
	small = one;
	for (i__ = 1; i__ <= 3; ++i__) {
	    d__1 = small * rbase;
	    small = dlamc3_(&d__1, &zero);
/* L20: */
	}
	a = dlamc3_(&one, &small);
	dlamc4_(&ngpmin, &one, &lbeta);
	d__1 = -one;
	dlamc4_(&ngnmin, &d__1, &lbeta);
	dlamc4_(&gpmin, &a, &lbeta);
	d__1 = -a;
	dlamc4_(&gnmin, &d__1, &lbeta);
	ieee = false;

	if (ngpmin == ngnmin && gpmin == gnmin) {
	    if (ngpmin == gpmin) {
		lemin = ngpmin;
/*            ( Non twos-complement machines, no gradual underflow; */
/*              e.g.,  VAX ) */
	    } else if (gpmin - ngpmin == 3) {
		lemin = ngpmin - 1 + lt;
		ieee = true;
/*            ( Non twos-complement machines, with gradual underflow; */
/*              e.g., IEEE standard followers ) */
	    } else {
		lemin = std::min(ngpmin,gpmin);
/*            ( A guess; no known machine ) */
		iwarn = true;
	    }

	} else if (ngpmin == gpmin && ngnmin == gnmin) {
	    if ((i__1 = ngpmin - ngnmin, abs(i__1)) == 1) {
		lemin = std::max(ngpmin,ngnmin);
/*            ( Twos-complement machines, no gradual underflow; */
/*              e.g., CYBER 205 ) */
	    } else {
		lemin = std::min(ngpmin,ngnmin);
/*            ( A guess; no known machine ) */
		iwarn = true;
	    }

	} else if ((i__1 = ngpmin - ngnmin, abs(i__1)) == 1 && gpmin == gnmin)
		 {
	    if (gpmin - std::min(ngpmin,ngnmin) == 3) {
		lemin = std::max(ngpmin,ngnmin) - 1 + lt;
/*            ( Twos-complement machines with gradual underflow; */
/*              no known machine ) */
	    } else {
		lemin = std::min(ngpmin,ngnmin);
/*            ( A guess; no known machine ) */
		iwarn = true;
	    }

	} else {
/* Computing MIN */
	    i__1 = std::min(ngpmin,ngnmin), i__1 = std::min(i__1,gpmin);
	    lemin = std::min(i__1,gnmin);
/*         ( A guess; no known machine ) */
	    iwarn = true;
	}
	first = false;
/* ** */
/* Comment out this if block if EMIN is ok */
	if (iwarn) {
	    first = true;
		Melder_warning (U"DLAMC2 WARNING. The value EMIN may be incorrect:- ", lemin);
	}
/* ** */

/*        Assume IEEE arithmetic if we found denormalised  numbers above, */
/*        or if arithmetic seems to round in the  IEEE style,  determined */
/*        in routine DLAMC1. A true IEEE machine should have both  things */
/*        true; however, faulty machines may have one or the other. */

	ieee = ieee || lieee1;

/*        Compute  RMIN by successive division by  BETA. We could compute */
/*        RMIN as BASE**( EMIN - 1 ),  but some machines underflow during */
/*        this computation. */

	lrmin = 1.;
	i__1 = 1 - lemin;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    d__1 = lrmin * rbase;
	    lrmin = dlamc3_(&d__1, &zero);
/* L30: */
	}

/*        Finally, call DLAMC5 to compute EMAX and RMAX. */

	dlamc5_(&lbeta, &lt, &lemin, &ieee, &lemax, &lrmax);
    }

    *beta = lbeta;
    *t = lt;
    *rnd = lrnd;
    *eps = leps;
    *emin = lemin;
    *rmin = lrmin;
    *emax = lemax;
    *rmax = lrmax;

    return 0;


/*     End of DLAMC2 */

} /* dlamc2_ */


/* *********************************************************************** */

double dlamc3_(double *a, double *b)
{
    /*
		FIX by Paul Boersma 20200418:
		Some optimizers can optimize away the whole call to this function if ret_val is not declared volatile.
		In case this function is optimized away,
		the floating-point epsilon may be estimated (by dlamch) not as 2.2e-16
		but as 2048 times lower on i386 gcc (using an 80-bit register rather than a 64-bit memory position),
		so that an iterative procedure used in our discriminant analysis did not converge on 32-bit Windows
		and on 32-bit i386 Linux.
	*/
    volatile double ret_val;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAMC3  is intended to force  A  and  B  to be stored prior to doing */
/*  the addition of  A  and  B ,  for use in situations where optimizers */
/*  might hold one of these in a register. */

/*  Arguments */
/*  ========= */

/*  A       (input) DOUBLE PRECISION */
/*  B       (input) DOUBLE PRECISION */
/*          The values A and B. */

/* ===================================================================== */

/*     .. Executable Statements .. */

    ret_val = *a + *b;

    return ret_val;

/*     End of DLAMC3 */

} /* dlamc3_ */


/* *********************************************************************** */

/* Subroutine */ int dlamc4_(integer *emin, double *start, integer *base)
{
    /* System generated locals */
    integer i__1;
    double d__1;

    /* Local variables */
    double a;
    integer i__;
    double b1, b2, c1, c2, d1, d2, one, zero, rbase;



/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAMC4 is a service routine for DLAMC2. */

/*  Arguments */
/*  ========= */

/*  EMIN    (output) INTEGER */
/*          The minimum exponent before (gradual) underflow, computed by */
/*          setting A = START and dividing by BASE until the previous A */
/*          can not be recovered. */

/*  START   (input) DOUBLE PRECISION */
/*          The starting point for determining EMIN. */

/*  BASE    (input) INTEGER */
/*          The base of the machine. */

/* ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    a = *start;
    one = 1.;
    rbase = one / *base;
    zero = 0.;
    *emin = 1;
    d__1 = a * rbase;
    b1 = dlamc3_(&d__1, &zero);
    c1 = a;
    c2 = a;
    d1 = a;
    d2 = a;
/* +    WHILE( ( C1.EQ.A ).AND.( C2.EQ.A ).AND. */
/*    $       ( D1.EQ.A ).AND.( D2.EQ.A )      )LOOP */
L10:
    if (c1 == a && c2 == a && d1 == a && d2 == a) {
	--(*emin);
	a = b1;
	d__1 = a / *base;
	b1 = dlamc3_(&d__1, &zero);
	d__1 = b1 * *base;
	c1 = dlamc3_(&d__1, &zero);
	d1 = zero;
	i__1 = *base;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    d1 += b1;
/* L20: */
	}
	d__1 = a * rbase;
	b2 = dlamc3_(&d__1, &zero);
	d__1 = b2 / rbase;
	c2 = dlamc3_(&d__1, &zero);
	d2 = zero;
	i__1 = *base;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    d2 += b2;
/* L30: */
	}
	goto L10;
    }
/* +    END WHILE */

    return 0;

/*     End of DLAMC4 */

} /* dlamc4_ */


/* *********************************************************************** */

/* Subroutine */ int dlamc5_(integer *beta, integer *p, integer *emin,
	bool *ieee, integer *emax, double *rmax)
{
	/* Table of constant values */
	static double c_b32 = 0.;

    /* System generated locals */
    integer i__1;
    double d__1;

    /* Local variables */
    integer i__;
    double y, z__;
    integer try__, lexp;
    double oldy;
    integer uexp, nbits;

    double recbas;
    integer exbits, expsum;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAMC5 attempts to compute RMAX, the largest machine floating-point */
/*  number, without overflow.  It assumes that EMAX + abs(EMIN) sum */
/*  approximately to a power of 2.  It will fail on machines where this */
/*  assumption does not hold, for example, the Cyber 205 (EMIN = -28625, */
/*  EMAX = 28718).  It will also fail if the value supplied for EMIN is */
/*  too large (i.e. too close to zero), probably with overflow. */

/*  Arguments */
/*  ========= */

/*  BETA    (input) INTEGER */
/*          The base of floating-point arithmetic. */

/*  P       (input) INTEGER */
/*          The number of base BETA digits in the mantissa of a */
/*          floating-point value. */

/*  EMIN    (input) INTEGER */
/*          The minimum exponent before (gradual) underflow. */

/*  IEEE    (input) LOGICAL */
/*          A logical flag specifying whether or not the arithmetic */
/*          system is thought to comply with the IEEE standard. */

/*  EMAX    (output) INTEGER */
/*          The largest exponent before overflow */

/*  RMAX    (output) DOUBLE PRECISION */
/*          The largest machine floating-point number. */

/* ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     First compute LEXP and UEXP, two powers of 2 that bound */
/*     abs(EMIN). We then assume that EMAX + abs(EMIN) will sum */
/*     approximately to the bound that is closest to abs(EMIN). */
/*     (EMAX is the exponent of the required number RMAX). */

    lexp = 1;
    exbits = 1;
L10:
    try__ = lexp << 1;
    if (try__ <= -(*emin)) {
	lexp = try__;
	++exbits;
	goto L10;
    }
    if (lexp == -(*emin)) {
	uexp = lexp;
    } else {
	uexp = try__;
	++exbits;
    }

/*     Now -LEXP is less than or equal to EMIN, and -UEXP is greater */
/*     than or equal to EMIN. EXBITS is the number of bits needed to */
/*     store the exponent. */

    if (uexp + *emin > -lexp - *emin) {
	expsum = lexp << 1;
    } else {
	expsum = uexp << 1;
    }

/*     EXPSUM is the exponent range, approximately equal to */
/*     EMAX - EMIN + 1 . */

    *emax = expsum + *emin - 1;
    nbits = exbits + 1 + *p;

/*     NBITS is the total number of bits needed to store a */
/*     floating-point number. */

    if (nbits % 2 == 1 && *beta == 2) {

/*        Either there are an odd number of bits used to store a */
/*        floating-point number, which is unlikely, or some bits are */
/*        not used in the representation of numbers, which is possible, */
/*        (e.g. Cray machines) or the mantissa has an implicit bit, */
/*        (e.g. IEEE machines, Dec Vax machines), which is perhaps the */
/*        most likely. We have to assume the last alternative. */
/*        If this is true, then we need to reduce EMAX by one because */
/*        there must be some way of representing zero in an implicit-bit */
/*        system. On machines like Cray, we are reducing EMAX by one */
/*        unnecessarily. */

	--(*emax);
    }

    if (*ieee) {

/*        Assume we are on an IEEE machine which reserves one exponent */
/*        for infinity and NaN. */

	--(*emax);
    }

/*     Now create RMAX, the largest machine number, which should */
/*     be equal to (1.0 - BETA**(-P)) * BETA**EMAX . */

/*     First compute 1.0 - BETA**(-P), being careful that the */
/*     result is less than 1.0 . */

    recbas = 1. / *beta;
    z__ = *beta - 1.;
    y = 0.;
    i__1 = *p;
    for (i__ = 1; i__ <= i__1; ++i__) {
	z__ *= recbas;
	if (y < 1.) {
	    oldy = y;
	}
	y = dlamc3_(&y, &z__);
/* L20: */
    }
    if (y >= 1.) {
	y = oldy;
    }

/*     Now multiply by BETA**EMAX to get RMAX. */

    i__1 = *emax;
    for (i__ = 1; i__ <= i__1; ++i__) {
	d__1 = y * *beta;
	y = dlamc3_(&d__1, &c_b32);
/* L30: */
    }

    *rmax = y;
    return 0;

/*     End of DLAMC5 */

} /* dlamc5_ */

/* Subroutine */ int dlamrg_(integer *n1, integer *n2, double *a, integer
	*dtrd1, integer *dtrd2, integer *index)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    integer i__, ind1, ind2, n1sv, n2sv;


/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAMRG will create a permutation list which will merge the elements */
/*  of A (which is composed of two independently sorted sets) into a */
/*  single set which is sorted in ascending order. */

/*  Arguments */
/*  ========= */

/*  N1     (input) INTEGER */
/*  N2     (input) INTEGER */
/*         These arguements contain the respective lengths of the two */
/*         sorted lists to be merged. */

/*  A      (input) DOUBLE PRECISION array, dimension (N1+N2) */
/*         The first N1 elements of A contain a list of numbers which */
/*         are sorted in either ascending or descending order.  Likewise */
/*         for the final N2 elements. */

/*  DTRD1  (input) INTEGER */
/*  DTRD2  (input) INTEGER */
/*         These are the strides to be taken through the array A. */
/*         Allowable strides are 1 and -1.  They indicate whether a */
/*         subset of A is sorted in ascending (DTRDx = 1) or descending */
/*         (DTRDx = -1) order. */

/*  INDEX  (output) INTEGER array, dimension (N1+N2) */
/*         On exit this array will contain a permutation such that */
/*         if B( I ) = A( INDEX( I ) ) for I=1,N1+N2, then B will be */
/*         sorted in ascending order. */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    --index;
    --a;

    /* Function Body */
    n1sv = *n1;
    n2sv = *n2;
    if (*dtrd1 > 0) {
	ind1 = 1;
    } else {
	ind1 = *n1;
    }
    if (*dtrd2 > 0) {
	ind2 = *n1 + 1;
    } else {
	ind2 = *n1 + *n2;
    }
    i__ = 1;
/*     while ( (N1SV > 0) & (N2SV > 0) ) */
L10:
    if (n1sv > 0 && n2sv > 0) {
	if (a[ind1] <= a[ind2]) {
	    index[i__] = ind1;
	    ++i__;
	    ind1 += *dtrd1;
	    --n1sv;
	} else {
	    index[i__] = ind2;
	    ++i__;
	    ind2 += *dtrd2;
	    --n2sv;
	}
	goto L10;
    }
/*     end while */
    if (n1sv == 0) {
	i__1 = n2sv;
	for (n1sv = 1; n1sv <= i__1; ++n1sv) {
	    index[i__] = ind2;
	    ++i__;
	    ind2 += *dtrd2;
/* L20: */
	}
    } else {
/*     N2SV .EQ. 0 */
	i__1 = n1sv;
	for (n2sv = 1; n2sv <= i__1; ++n2sv) {
	    index[i__] = ind1;
	    ++i__;
	    ind1 += *dtrd1;
/* L30: */
	}
    }

    return 0;

/*     End of DLAMRG */

} /* dlamrg_ */

/* Subroutine */ integer dlaneg_(integer *n, double *d__, double *lld, double *
	sigma, double *pivmin, integer *r__)
{
    /* System generated locals */
    integer ret_val, i__1, i__2, i__3, i__4;

    /* Local variables */
    integer j;
    double p, t;
    integer bj;
    double tmp;
    integer neg1, neg2;
    double bsav, gamma, dplus;

    integer negcnt;
    bool sawnan;
    double dminus;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLANEG computes the Sturm count, the number of negative pivots */
/*  encountered while factoring tridiagonal T - sigma I = L D L^T. */
/*  This implementation works directly on the factors without forming */
/*  the tridiagonal matrix T.  The Sturm count is also the number of */
/*  eigenvalues of T less than sigma. */

/*  This routine is called from DLARRB. */

/*  The current routine does not use the PIVMIN parameter but rather */
/*  requires IEEE-754 propagation of Infinities and NaNs.  This */
/*  routine also has no input range restrictions but does require */
/*  default exception handling such that x/0 produces Inf when x is */
/*  non-zero, and Inf/Inf produces NaN.  For more information, see: */

/*    Marques, Riedy, and Voemel, "Benefits of IEEE-754 Features in */
/*    Modern Symmetric Tridiagonal Eigensolvers," SIAM Journal on */
/*    Scientific Computing, v28, n5, 2006.  DOI 10.1137/050641624 */
/*    (Tech report version in LAWN 172 with the same title.) */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The N diagonal elements of the diagonal matrix D. */

/*  LLD     (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (N-1) elements L(i)*L(i)*D(i). */

/*  SIGMA   (input) DOUBLE PRECISION */
/*          Shift amount in T - sigma I = L D L^T. */

/*  PIVMIN  (input) DOUBLE PRECISION */
/*          The minimum pivot in the Sturm sequence.  May be used */
/*          when zero pivots are encountered on non-IEEE-754 */
/*          architectures. */

/*  R       (input) INTEGER */
/*          The twist index for the twisted factorization that is used */
/*          for the negcount. */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Osni Marques, LBNL/NERSC, USA */
/*     Christof Voemel, University of California, Berkeley, USA */
/*     Jason Riedy, University of California, Berkeley, USA */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     Some architectures propagate Infinities and NaNs very slowly, so */
/*     the code computes counts in BLKLEN chunks.  Then a NaN can */
/*     propagate at most BLKLEN columns before being detected.  This is */
/*     not a general tuning parameter; it needs only to be just large */
/*     enough that the overhead is tiny in common cases. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */
    /* Parameter adjustments */
    --lld;
    --d__;

    /* Function Body */
    negcnt = 0;
/*     I) upper part: L D L^T - SIGMA I = L+ D+ L+^T */
    t = -(*sigma);
    i__1 = *r__ - 1;
    for (bj = 1; bj <= i__1; bj += 128) {
	neg1 = 0;
	bsav = t;
/* Computing MIN */
	i__3 = bj + 127, i__4 = *r__ - 1;
	i__2 = std::min(i__3,i__4);
	for (j = bj; j <= i__2; ++j) {
	    dplus = d__[j] + t;
	    if (dplus < 0.) {
		++neg1;
	    }
	    tmp = t / dplus;
	    t = tmp * lld[j] - *sigma;
/* L21: */
	}
	sawnan = disnan_(&t);
/*     Run a slower version of the above loop if a NaN is detected. */
/*     A NaN should occur only with a zero pivot after an infinite */
/*     pivot.  In that case, substituting 1 for T/DPLUS is the */
/*     correct limit. */
	if (sawnan) {
	    neg1 = 0;
	    t = bsav;
/* Computing MIN */
	    i__3 = bj + 127, i__4 = *r__ - 1;
	    i__2 = std::min(i__3,i__4);
	    for (j = bj; j <= i__2; ++j) {
		dplus = d__[j] + t;
		if (dplus < 0.) {
		    ++neg1;
		}
		tmp = t / dplus;
		if (disnan_(&tmp)) {
		    tmp = 1.;
		}
		t = tmp * lld[j] - *sigma;
/* L22: */
	    }
	}
	negcnt += neg1;
/* L210: */
    }

/*     II) lower part: L D L^T - SIGMA I = U- D- U-^T */
    p = d__[*n] - *sigma;
    i__1 = *r__;
    for (bj = *n - 1; bj >= i__1; bj += -128) {
	neg2 = 0;
	bsav = p;
/* Computing MAX */
	i__3 = bj - 127;
	i__2 = std::max(i__3,*r__);
	for (j = bj; j >= i__2; --j) {
	    dminus = lld[j] + p;
	    if (dminus < 0.) {
		++neg2;
	    }
	    tmp = p / dminus;
	    p = tmp * d__[j] - *sigma;
/* L23: */
	}
	sawnan = disnan_(&p);
/*     As above, run a slower version that substitutes 1 for Inf/Inf. */

	if (sawnan) {
	    neg2 = 0;
	    p = bsav;
/* Computing MAX */
	    i__3 = bj - 127;
	    i__2 = std::max(i__3,*r__);
	    for (j = bj; j >= i__2; --j) {
		dminus = lld[j] + p;
		if (dminus < 0.) {
		    ++neg2;
		}
		tmp = p / dminus;
		if (disnan_(&tmp)) {
		    tmp = 1.;
		}
		p = tmp * d__[j] - *sigma;
/* L24: */
	    }
	}
	negcnt += neg2;
/* L230: */
    }

/*     III) Twist index */
/*       T was shifted by SIGMA initially. */
    gamma = t + *sigma + p;
    if (gamma < 0.) {
	++negcnt;
    }
    ret_val = negcnt;
    return ret_val;
} /* dlaneg_ */

/* Subroutine */ double dlangb_(const char *norm, integer *n, integer *kl, integer *ku,
	double *ab, integer *ldab, double *work)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer ab_dim1, ab_offset, i__1, i__2, i__3, i__4, i__5, i__6;
    double ret_val, d__1, d__2, d__3;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    integer i__, j, k, l;
    double sum, scale;
    double value;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLANGB  returns the value of the one norm,  or the Frobenius norm, or */
/*  the  infinity norm,  or the element of  largest absolute value  of an */
/*  n by n band matrix  A,  with kl sub-diagonals and ku super-diagonals. */

/*  Description */
/*  =========== */

/*  DLANGB returns the value */

/*     DLANGB = ( max(abs(A(i,j))), NORM = 'M' or 'm' */
/*              ( */
/*              ( norm1(A),         NORM = '1', 'O' or 'o' */
/*              ( */
/*              ( normI(A),         NORM = 'I' or 'i' */
/*              ( */
/*              ( normF(A),         NORM = 'F', 'f', 'E' or 'e' */

/*  where  norm1  denotes the  one norm of a matrix (maximum column sum), */
/*  normI  denotes the  infinity norm  of a matrix  (maximum row sum) and */
/*  normF  denotes the  Frobenius norm of a matrix (square root of sum of */
/*  squares).  Note that  max(abs(A(i,j)))  is not a consistent matrix norm. */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies the value to be returned in DLANGB as described */
/*          above. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0.  When N = 0, DLANGB is */
/*          set to zero. */

/*  KL      (input) INTEGER */
/*          The number of sub-diagonals of the matrix A.  KL >= 0. */

/*  KU      (input) INTEGER */
/*          The number of super-diagonals of the matrix A.  KU >= 0. */

/*  AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          The band matrix A, stored in rows 1 to KL+KU+1.  The j-th */
/*          column of A is stored in the j-th column of the array AB as */
/*          follows: */
/*          AB(ku+1+i-j,j) = A(i,j) for max(1,j-ku)<=i<=min(n,j+kl). */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KL+KU+1. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (MAX(1,LWORK)), */
/*          where LWORK >= N when NORM = 'I'; otherwise, WORK is not */
/*          referenced. */

/* ===================================================================== */


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
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    --work;

    /* Function Body */
    if (*n == 0) {
	value = 0.;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	value = 0.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	    i__2 = *ku + 2 - j;
/* Computing MIN */
	    i__4 = *n + *ku + 1 - j, i__5 = *kl + *ku + 1;
	    i__3 = std::min(i__4,i__5);
	    for (i__ = std::max(i__2,1_integer); i__ <= i__3; ++i__) {
/* Computing MAX */
		d__2 = value, d__3 = (d__1 = ab[i__ + j * ab_dim1], abs(d__1))
			;
		value = std::max(d__2,d__3);
/* L10: */
	    }
/* L20: */
	}
    } else if (lsame_(norm, "O") || *(unsigned char *)
	    norm == '1') {

/*        Find norm1(A). */

	value = 0.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = 0.;
/* Computing MAX */
	    i__3 = *ku + 2 - j;
/* Computing MIN */
	    i__4 = *n + *ku + 1 - j, i__5 = *kl + *ku + 1;
	    i__2 = std::min(i__4,i__5);
	    for (i__ = std::max(i__3,1_integer); i__ <= i__2; ++i__) {
		sum += (d__1 = ab[i__ + j * ab_dim1], abs(d__1));
/* L30: */
	    }
	    value = std::max(value,sum);
/* L40: */
	}
    } else if (lsame_(norm, "I")) {

/*        Find normI(A). */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    work[i__] = 0.;
/* L50: */
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    k = *ku + 1 - j;
/* Computing MAX */
	    i__2 = 1, i__3 = j - *ku;
/* Computing MIN */
	    i__5 = *n, i__6 = j + *kl;
	    i__4 = std::min(i__5,i__6);
	    for (i__ = std::max(i__2,i__3); i__ <= i__4; ++i__) {
		work[i__] += (d__1 = ab[k + i__ + j * ab_dim1], abs(d__1));
/* L60: */
	    }
/* L70: */
	}
	value = 0.;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	    d__1 = value, d__2 = work[i__];
	    value = std::max(d__1,d__2);
/* L80: */
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.;
	sum = 1.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	    i__4 = 1, i__2 = j - *ku;
	    l = std::max(i__4,i__2);
	    k = *ku + 1 - j + l;
/* Computing MIN */
	    i__2 = *n, i__3 = j + *kl;
	    i__4 = std::min(i__2,i__3) - l + 1;
	    dlassq_(&i__4, &ab[k + j * ab_dim1], &c__1, &scale, &sum);
/* L90: */
	}
	value = scale * sqrt(sum);
    }

    ret_val = value;
    return ret_val;

/*     End of DLANGB */

} /* dlangb_ */

/* Subroutine */ double dlange_(const char *norm, integer *m, integer *n, double *a, integer
	*lda, double *work)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    double ret_val, d__1, d__2, d__3;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    integer i__, j;
    double sum, scale;
    double value;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLANGE  returns the value of the one norm,  or the Frobenius norm, or */
/*  the  infinity norm,  or the  element of  largest absolute value  of a */
/*  real matrix A. */

/*  Description */
/*  =========== */

/*  DLANGE returns the value */

/*     DLANGE = ( max(abs(A(i,j))), NORM = 'M' or 'm' */
/*              ( */
/*              ( norm1(A),         NORM = '1', 'O' or 'o' */
/*              ( */
/*              ( normI(A),         NORM = 'I' or 'i' */
/*              ( */
/*              ( normF(A),         NORM = 'F', 'f', 'E' or 'e' */

/*  where  norm1  denotes the  one norm of a matrix (maximum column sum), */
/*  normI  denotes the  infinity norm  of a matrix  (maximum row sum) and */
/*  normF  denotes the  Frobenius norm of a matrix (square root of sum of */
/*  squares).  Note that  max(abs(A(i,j)))  is not a consistent matrix norm. */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies the value to be returned in DLANGE as described */
/*          above. */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0.  When M = 0, */
/*          DLANGE is set to zero. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0.  When N = 0, */
/*          DLANGE is set to zero. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The m by n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(M,1). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (MAX(1,LWORK)), */
/*          where LWORK >= M when NORM = 'I'; otherwise, WORK is not */
/*          referenced. */

/* ===================================================================== */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --work;

    /* Function Body */
    if (std::min(*m,*n) == 0) {
	value = 0.;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	value = 0.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
		d__2 = value, d__3 = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		value = std::max(d__2,d__3);
/* L10: */
	    }
/* L20: */
	}
    } else if (lsame_(norm, "O") || *(unsigned char *)
	    norm == '1') {

/*        Find norm1(A). */

	value = 0.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = 0.;
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		sum += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L30: */
	    }
	    value = std::max(value,sum);
/* L40: */
	}
    } else if (lsame_(norm, "I")) {

/*        Find normI(A). */

	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    work[i__] = 0.;
/* L50: */
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		work[i__] += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L60: */
	    }
/* L70: */
	}
	value = 0.;
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	    d__1 = value, d__2 = work[i__];
	    value = std::max(d__1,d__2);
/* L80: */
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.;
	sum = 1.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    dlassq_(m, &a[j * a_dim1 + 1], &c__1, &scale, &sum);
/* L90: */
	}
	value = scale * sqrt(sum);
    }

    ret_val = value;
    return ret_val;

/*     End of DLANGE */

} /* dlange_ */

/* Subroutine */ double dlangt_(const char *norm, integer *n, double *dl, double *d__,
	double *du)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer i__1;
    double ret_val, d__1, d__2, d__3, d__4, d__5;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    integer i__;
    double sum, scale;
    double anorm;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLANGT  returns the value of the one norm,  or the Frobenius norm, or */
/*  the  infinity norm,  or the  element of  largest absolute value  of a */
/*  real tridiagonal matrix A. */

/*  Description */
/*  =========== */

/*  DLANGT returns the value */

/*     DLANGT = ( max(abs(A(i,j))), NORM = 'M' or 'm' */
/*              ( */
/*              ( norm1(A),         NORM = '1', 'O' or 'o' */
/*              ( */
/*              ( normI(A),         NORM = 'I' or 'i' */
/*              ( */
/*              ( normF(A),         NORM = 'F', 'f', 'E' or 'e' */

/*  where  norm1  denotes the  one norm of a matrix (maximum column sum), */
/*  normI  denotes the  infinity norm  of a matrix  (maximum row sum) and */
/*  normF  denotes the  Frobenius norm of a matrix (square root of sum of */
/*  squares).  Note that  max(abs(A(i,j)))  is not a consistent matrix norm. */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies the value to be returned in DLANGT as described */
/*          above. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0.  When N = 0, DLANGT is */
/*          set to zero. */

/*  DL      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) sub-diagonal elements of A. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal elements of A. */

/*  DU      (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) super-diagonal elements of A. */

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
    --du;
    --d__;
    --dl;

    /* Function Body */
    if (*n <= 0) {
	anorm = 0.;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	anorm = (d__1 = d__[*n], abs(d__1));
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	    d__2 = anorm, d__3 = (d__1 = dl[i__], abs(d__1));
	    anorm = std::max(d__2,d__3);
/* Computing MAX */
	    d__2 = anorm, d__3 = (d__1 = d__[i__], abs(d__1));
	    anorm = std::max(d__2,d__3);
/* Computing MAX */
	    d__2 = anorm, d__3 = (d__1 = du[i__], abs(d__1));
	    anorm = std::max(d__2,d__3);
/* L10: */
	}
    } else if (lsame_(norm, "O") || *(unsigned char *)
	    norm == '1') {

/*        Find norm1(A). */

	if (*n == 1) {
	    anorm = abs(d__[1]);
	} else {
/* Computing MAX */
	    d__3 = abs(d__[1]) + abs(dl[1]), d__4 = (d__1 = d__[*n], abs(d__1)
		    ) + (d__2 = du[*n - 1], abs(d__2));
	    anorm = std::max(d__3,d__4);
	    i__1 = *n - 1;
	    for (i__ = 2; i__ <= i__1; ++i__) {
/* Computing MAX */
		d__4 = anorm, d__5 = (d__1 = d__[i__], abs(d__1)) + (d__2 =
			dl[i__], abs(d__2)) + (d__3 = du[i__ - 1], abs(d__3));
		anorm = std::max(d__4,d__5);
/* L20: */
	    }
	}
    } else if (lsame_(norm, "I")) {

/*        Find normI(A). */

	if (*n == 1) {
	    anorm = abs(d__[1]);
	} else {
/* Computing MAX */
	    d__3 = abs(d__[1]) + abs(du[1]), d__4 = (d__1 = d__[*n], abs(d__1)
		    ) + (d__2 = dl[*n - 1], abs(d__2));
	    anorm = std::max(d__3,d__4);
	    i__1 = *n - 1;
	    for (i__ = 2; i__ <= i__1; ++i__) {
/* Computing MAX */
		d__4 = anorm, d__5 = (d__1 = d__[i__], abs(d__1)) + (d__2 =
			du[i__], abs(d__2)) + (d__3 = dl[i__ - 1], abs(d__3));
		anorm = std::max(d__4,d__5);
/* L30: */
	    }
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.;
	sum = 1.;
	dlassq_(n, &d__[1], &c__1, &scale, &sum);
	if (*n > 1) {
	    i__1 = *n - 1;
	    dlassq_(&i__1, &dl[1], &c__1, &scale, &sum);
	    i__1 = *n - 1;
	    dlassq_(&i__1, &du[1], &c__1, &scale, &sum);
	}
	anorm = scale * sqrt(sum);
    }

    ret_val = anorm;
    return ret_val;

/*     End of DLANGT */

} /* dlangt_ */

/* Subroutine */ double dlanhs_(const char *norm, integer *n, double *a, integer *lda,
	double *work)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;
    double ret_val, d__1, d__2, d__3;

    /* Local variables */
    integer i__, j;
    double sum, scale;
    double value;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLANHS  returns the value of the one norm,  or the Frobenius norm, or */
/*  the  infinity norm,  or the  element of  largest absolute value  of a */
/*  Hessenberg matrix A. */

/*  Description */
/*  =========== */

/*  DLANHS returns the value */

/*     DLANHS = ( max(abs(A(i,j))), NORM = 'M' or 'm' */
/*              ( */
/*              ( norm1(A),         NORM = '1', 'O' or 'o' */
/*              ( */
/*              ( normI(A),         NORM = 'I' or 'i' */
/*              ( */
/*              ( normF(A),         NORM = 'F', 'f', 'E' or 'e' */

/*  where  norm1  denotes the  one norm of a matrix (maximum column sum), */
/*  normI  denotes the  infinity norm  of a matrix  (maximum row sum) and */
/*  normF  denotes the  Frobenius norm of a matrix (square root of sum of */
/*  squares).  Note that  max(abs(A(i,j)))  is not a consistent matrix norm. */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies the value to be returned in DLANHS as described */
/*          above. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0.  When N = 0, DLANHS is */
/*          set to zero. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The n by n upper Hessenberg matrix A; the part of A below the */
/*          first sub-diagonal is not referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(N,1). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (MAX(1,LWORK)), */
/*          where LWORK >= N when NORM = 'I'; otherwise, WORK is not */
/*          referenced. */

/* ===================================================================== */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --work;

    /* Function Body */
    if (*n == 0) {
	value = 0.;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	value = 0.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	    i__3 = *n, i__4 = j + 1;
	    i__2 = std::min(i__3,i__4);
	    for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
		d__2 = value, d__3 = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		value = std::max(d__2,d__3);
/* L10: */
	    }
/* L20: */
	}
    } else if (lsame_(norm, "O") || *(unsigned char *)
	    norm == '1') {

/*        Find norm1(A). */

	value = 0.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    sum = 0.;
/* Computing MIN */
	    i__3 = *n, i__4 = j + 1;
	    i__2 = std::min(i__3,i__4);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		sum += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L30: */
	    }
	    value = std::max(value,sum);
/* L40: */
	}
    } else if (lsame_(norm, "I")) {

/*        Find normI(A). */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    work[i__] = 0.;
/* L50: */
	}
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	    i__3 = *n, i__4 = j + 1;
	    i__2 = std::min(i__3,i__4);
	    for (i__ = 1; i__ <= i__2; ++i__) {
		work[i__] += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L60: */
	    }
/* L70: */
	}
	value = 0.;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	    d__1 = value, d__2 = work[i__];
	    value = std::max(d__1,d__2);
/* L80: */
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.;
	sum = 1.;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
	    i__3 = *n, i__4 = j + 1;
	    i__2 = std::min(i__3,i__4);
	    dlassq_(&i__2, &a[j * a_dim1 + 1], &c__1, &scale, &sum);
/* L90: */
	}
	value = scale * sqrt(sum);
    }

    ret_val = value;
    return ret_val;

/*     End of DLANHS */

} /* dlanhs_ */

/* Subroutine */ double dlansb_(const char *norm, const char *uplo, integer *n, integer *k, double
	*ab, integer *ldab, double *work)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer ab_dim1, ab_offset, i__1, i__2, i__3, i__4;
    double ret_val, d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, l;
    double sum, absa, scale;
    double value;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLANSB  returns the value of the one norm,  or the Frobenius norm, or */
/*  the  infinity norm,  or the element of  largest absolute value  of an */
/*  n by n symmetric band matrix A,  with k super-diagonals. */

/*  Description */
/*  =========== */

/*  DLANSB returns the value */

/*     DLANSB = ( max(abs(A(i,j))), NORM = 'M' or 'm' */
/*              ( */
/*              ( norm1(A),         NORM = '1', 'O' or 'o' */
/*              ( */
/*              ( normI(A),         NORM = 'I' or 'i' */
/*              ( */
/*              ( normF(A),         NORM = 'F', 'f', 'E' or 'e' */

/*  where  norm1  denotes the  one norm of a matrix (maximum column sum), */
/*  normI  denotes the  infinity norm  of a matrix  (maximum row sum) and */
/*  normF  denotes the  Frobenius norm of a matrix (square root of sum of */
/*  squares).  Note that  max(abs(A(i,j)))  is not a consistent matrix norm. */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies the value to be returned in DLANSB as described */
/*          above. */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          band matrix A is supplied. */
/*          = 'U':  Upper triangular part is supplied */
/*          = 'L':  Lower triangular part is supplied */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0.  When N = 0, DLANSB is */
/*          set to zero. */

/*  K       (input) INTEGER */
/*          The number of super-diagonals or sub-diagonals of the */
/*          band matrix A.  K >= 0. */

/*  AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          The upper or lower triangle of the symmetric band matrix A, */
/*          stored in the first K+1 rows of AB.  The j-th column of A is */
/*          stored in the j-th column of the array AB as follows: */
/*          if UPLO = 'U', AB(k+1+i-j,j) = A(i,j) for max(1,j-k)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)   = A(i,j) for j<=i<=min(n,j+k). */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= K+1. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (MAX(1,LWORK)), */
/*          where LWORK >= N when NORM = 'I' or '1' or 'O'; otherwise, */
/*          WORK is not referenced. */

/* ===================================================================== */

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
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    --work;

    /* Function Body */
    if (*n == 0) {
	value = 0.;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	value = 0.;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
		i__2 = *k + 2 - j;
		i__3 = *k + 1;
		for (i__ = std::max(i__2,1_integer); i__ <= i__3; ++i__) {
/* Computing MAX */
		    d__2 = value, d__3 = (d__1 = ab[i__ + j * ab_dim1], abs(
			    d__1));
		    value = std::max(d__2,d__3);
/* L10: */
		}
/* L20: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		i__2 = *n + 1 - j, i__4 = *k + 1;
		i__3 = std::min(i__2,i__4);
		for (i__ = 1; i__ <= i__3; ++i__) {
/* Computing MAX */
		    d__2 = value, d__3 = (d__1 = ab[i__ + j * ab_dim1], abs(
			    d__1));
		    value = std::max(d__2,d__3);
/* L30: */
		}
/* L40: */
	    }
	}
    } else if (lsame_(norm, "I") || lsame_(norm, "O") || *(unsigned char *)norm == '1') {

/*        Find normI(A) ( = norm1(A), since A is symmetric). */

	value = 0.;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		sum = 0.;
		l = *k + 1 - j;
/* Computing MAX */
		i__3 = 1, i__2 = j - *k;
		i__4 = j - 1;
		for (i__ = std::max(i__3,i__2); i__ <= i__4; ++i__) {
		    absa = (d__1 = ab[l + i__ + j * ab_dim1], abs(d__1));
		    sum += absa;
		    work[i__] += absa;
/* L50: */
		}
		work[j] = sum + (d__1 = ab[*k + 1 + j * ab_dim1], abs(d__1));
/* L60: */
	    }
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
		d__1 = value, d__2 = work[i__];
		value = std::max(d__1,d__2);
/* L70: */
	    }
	} else {
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		work[i__] = 0.;
/* L80: */
	    }
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		sum = work[j] + (d__1 = ab[j * ab_dim1 + 1], abs(d__1));
		l = 1 - j;
/* Computing MIN */
		i__3 = *n, i__2 = j + *k;
		i__4 = std::min(i__3,i__2);
		for (i__ = j + 1; i__ <= i__4; ++i__) {
		    absa = (d__1 = ab[l + i__ + j * ab_dim1], abs(d__1));
		    sum += absa;
		    work[i__] += absa;
/* L90: */
		}
		value = std::max(value,sum);
/* L100: */
	    }
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.;
	sum = 1.;
	if (*k > 0) {
	    if (lsame_(uplo, "U")) {
		i__1 = *n;
		for (j = 2; j <= i__1; ++j) {
/* Computing MIN */
		    i__3 = j - 1;
		    i__4 = std::min(i__3,*k);
/* Computing MAX */
		    i__2 = *k + 2 - j;
		    dlassq_(&i__4, &ab[std::max(i__2, 1_integer)+ j * ab_dim1], &c__1, &
			    scale, &sum);
/* L110: */
		}
		l = *k + 1;
	    } else {
		i__1 = *n - 1;
		for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		    i__3 = *n - j;
		    i__4 = std::min(i__3,*k);
		    dlassq_(&i__4, &ab[j * ab_dim1 + 2], &c__1, &scale, &sum);
/* L120: */
		}
		l = 1;
	    }
	    sum *= 2;
	} else {
	    l = 1;
	}
	dlassq_(n, &ab[l + ab_dim1], ldab, &scale, &sum);
	value = scale * sqrt(sum);
    }

    ret_val = value;
    return ret_val;

/*     End of DLANSB */

} /* dlansb_ */

double dlansf_(const char *norm, char *transr, char *uplo, integer *n, double *a, double *work)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer i__1, i__2;
    double ret_val, d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, k, l;
    double s;
    integer n1;
    double aa;
    integer lda, ifm, noe, ilu;
    double scale;
    double value;


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

/*  DLANSF returns the value of the one norm, or the Frobenius norm, or */
/*  the infinity norm, or the element of largest absolute value of a */
/*  real symmetric matrix A in RFP format. */

/*  Description */
/*  =========== */

/*  DLANSF returns the value */

/*     DLANSF = ( max(abs(A(i,j))), NORM = 'M' or 'm' */
/*              ( */
/*              ( norm1(A),         NORM = '1', 'O' or 'o' */
/*              ( */
/*              ( normI(A),         NORM = 'I' or 'i' */
/*              ( */
/*              ( normF(A),         NORM = 'F', 'f', 'E' or 'e' */

/*  where  norm1  denotes the  one norm of a matrix (maximum column sum), */
/*  normI  denotes the  infinity norm  of a matrix  (maximum row sum) and */
/*  normF  denotes the  Frobenius norm of a matrix (square root of sum of */
/*  squares).  Note that  max(abs(A(i,j)))  is not a  matrix norm. */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER */
/*          Specifies the value to be returned in DLANSF as described */
/*          above. */

/*  TRANSR  (input) CHARACTER */
/*          Specifies whether the RFP format of A is normal or */
/*          transposed format. */
/*          = 'N':  RFP format is Normal; */
/*          = 'T':  RFP format is Transpose. */

/*  UPLO    (input) CHARACTER */
/*           On entry, UPLO specifies whether the RFP matrix A came from */
/*           an upper or lower triangular matrix as follows: */
/*           = 'U': RFP A came from an upper triangular matrix; */
/*           = 'L': RFP A came from a lower triangular matrix. */

/*  N       (input) INTEGER */
/*          The order of the matrix A. N >= 0. When N = 0, DLANSF is */
/*          set to zero. */

/*  A       (input) DOUBLE PRECISION array, dimension ( N*(N+1)/2 ); */
/*          On entry, the upper (if UPLO = 'U') or lower (if UPLO = 'L') */
/*          part of the symmetric matrix A stored in RFP format. See the */
/*          "Notes" below for more details. */
/*          Unchanged on exit. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (MAX(1,LWORK)), */
/*          where LWORK >= N when NORM = 'I' or '1' or 'O'; otherwise, */
/*          WORK is not referenced. */

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

    if (*n == 0) {
	ret_val = 0.;
	return ret_val;
    }

/*     set noe = 1 if n is odd. if n is even set noe=0 */

    noe = 1;
    if (*n % 2 == 0) {
	noe = 0;
    }

/*     set ifm = 0 when form='T or 't' and 1 otherwise */

    ifm = 1;
    if (lsame_(transr, "T")) {
	ifm = 0;
    }

/*     set ilu = 0 when uplo='U or 'u' and 1 otherwise */

    ilu = 1;
    if (lsame_(uplo, "U")) {
	ilu = 0;
    }

/*     set lda = (n+1)/2 when ifm = 0 */
/*     set lda = n when ifm = 1 and noe = 1 */
/*     set lda = n+1 when ifm = 1 and noe = 0 */

    if (ifm == 1) {
	if (noe == 1) {
	    lda = *n;
	} else {
/*           noe=0 */
	    lda = *n + 1;
	}
    } else {
/*        ifm=0 */
	lda = (*n + 1) / 2;
    }

    if (lsame_(norm, "M")) {

/*       Find max(abs(A(i,j))). */

	k = (*n + 1) / 2;
	value = 0.;
	if (noe == 1) {
/*           n is odd */
	    if (ifm == 1) {
/*           A is n by k */
		i__1 = k - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = *n - 1;
		    for (i__ = 0; i__ <= i__2; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = a[i__ + j * lda], abs(d__1));
			value = std::max(d__2,d__3);
		    }
		}
	    } else {
/*              xpose case; A is k by n */
		i__1 = *n - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = k - 1;
		    for (i__ = 0; i__ <= i__2; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = a[i__ + j * lda], abs(d__1));
			value = std::max(d__2,d__3);
		    }
		}
	    }
	} else {
/*           n is even */
	    if (ifm == 1) {
/*              A is n+1 by k */
		i__1 = k - 1;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = *n;
		    for (i__ = 0; i__ <= i__2; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = a[i__ + j * lda], abs(d__1));
			value = std::max(d__2,d__3);
		    }
		}
	    } else {
/*              xpose case; A is k by n+1 */
		i__1 = *n;
		for (j = 0; j <= i__1; ++j) {
		    i__2 = k - 1;
		    for (i__ = 0; i__ <= i__2; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = a[i__ + j * lda], abs(d__1));
			value = std::max(d__2,d__3);
		    }
		}
	    }
	}
    } else if (lsame_(norm, "I") || lsame_(norm, "O") || *(unsigned char *)norm == '1') {

/*        Find normI(A) ( = norm1(A), since A is symmetric). */

	if (ifm == 1) {
	    k = *n / 2;
	    if (noe == 1) {
/*              n is odd */
		if (ilu == 0) {
		    i__1 = k - 1;
		    for (i__ = 0; i__ <= i__1; ++i__) {
			work[i__] = 0.;
		    }
		    i__1 = k;
		    for (j = 0; j <= i__1; ++j) {
			s = 0.;
			i__2 = k + j - 1;
			for (i__ = 0; i__ <= i__2; ++i__) {
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       -> A(i,j+k) */
			    s += aa;
			    work[i__] += aa;
			}
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    -> A(j+k,j+k) */
			work[j + k] = s + aa;
			if (i__ == k + k) {
			    goto L10;
			}
			++i__;
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    -> A(j,j) */
			work[j] += aa;
			s = 0.;
			i__2 = k - 1;
			for (l = j + 1; l <= i__2; ++l) {
			    ++i__;
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       -> A(l,j) */
			    s += aa;
			    work[l] += aa;
			}
			work[j] += s;
		    }
L10:
		    i__ = idamax_(n, work, &c__1);
		    value = work[i__ - 1];
		} else {
/*                 ilu = 1 */
		    ++k;
/*                 k=(n+1)/2 for n odd and ilu=1 */
		    i__1 = *n - 1;
		    for (i__ = k; i__ <= i__1; ++i__) {
			work[i__] = 0.;
		    }
		    for (j = k - 1; j >= 0; --j) {
			s = 0.;
			i__1 = j - 2;
			for (i__ = 0; i__ <= i__1; ++i__) {
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       -> A(j+k,i+k) */
			    s += aa;
			    work[i__ + k] += aa;
			}
			if (j > 0) {
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       -> A(j+k,j+k) */
			    s += aa;
			    work[i__ + k] += s;
/*                       i=j */
			    ++i__;
			}
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    -> A(j,j) */
			work[j] = aa;
			s = 0.;
			i__1 = *n - 1;
			for (l = j + 1; l <= i__1; ++l) {
			    ++i__;
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       -> A(l,j) */
			    s += aa;
			    work[l] += aa;
			}
			work[j] += s;
		    }
		    i__ = idamax_(n, work, &c__1);
		    value = work[i__ - 1];
		}
	    } else {
/*              n is even */
		if (ilu == 0) {
		    i__1 = k - 1;
		    for (i__ = 0; i__ <= i__1; ++i__) {
			work[i__] = 0.;
		    }
		    i__1 = k - 1;
		    for (j = 0; j <= i__1; ++j) {
			s = 0.;
			i__2 = k + j - 1;
			for (i__ = 0; i__ <= i__2; ++i__) {
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       -> A(i,j+k) */
			    s += aa;
			    work[i__] += aa;
			}
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    -> A(j+k,j+k) */
			work[j + k] = s + aa;
			++i__;
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    -> A(j,j) */
			work[j] += aa;
			s = 0.;
			i__2 = k - 1;
			for (l = j + 1; l <= i__2; ++l) {
			    ++i__;
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       -> A(l,j) */
			    s += aa;
			    work[l] += aa;
			}
			work[j] += s;
		    }
		    i__ = idamax_(n, work, &c__1);
		    value = work[i__ - 1];
		} else {
/*                 ilu = 1 */
		    i__1 = *n - 1;
		    for (i__ = k; i__ <= i__1; ++i__) {
			work[i__] = 0.;
		    }
		    for (j = k - 1; j >= 0; --j) {
			s = 0.;
			i__1 = j - 1;
			for (i__ = 0; i__ <= i__1; ++i__) {
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       -> A(j+k,i+k) */
			    s += aa;
			    work[i__ + k] += aa;
			}
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    -> A(j+k,j+k) */
			s += aa;
			work[i__ + k] += s;
/*                    i=j */
			++i__;
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    -> A(j,j) */
			work[j] = aa;
			s = 0.;
			i__1 = *n - 1;
			for (l = j + 1; l <= i__1; ++l) {
			    ++i__;
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       -> A(l,j) */
			    s += aa;
			    work[l] += aa;
			}
			work[j] += s;
		    }
		    i__ = idamax_(n, work, &c__1);
		    value = work[i__ - 1];
		}
	    }
	} else {
/*           ifm=0 */
	    k = *n / 2;
	    if (noe == 1) {
/*              n is odd */
		if (ilu == 0) {
		    n1 = k;
/*                 n/2 */
		    ++k;
/*                 k is the row size and lda */
		    i__1 = *n - 1;
		    for (i__ = n1; i__ <= i__1; ++i__) {
			work[i__] = 0.;
		    }
		    i__1 = n1 - 1;
		    for (j = 0; j <= i__1; ++j) {
			s = 0.;
			i__2 = k - 1;
			for (i__ = 0; i__ <= i__2; ++i__) {
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       A(j,n1+i) */
			    work[i__ + n1] += aa;
			    s += aa;
			}
			work[j] = s;
		    }
/*                 j=n1=k-1 is special */
		    s = (d__1 = a[j * lda], abs(d__1));
/*                 A(k-1,k-1) */
		    i__1 = k - 1;
		    for (i__ = 1; i__ <= i__1; ++i__) {
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    A(k-1,i+n1) */
			work[i__ + n1] += aa;
			s += aa;
		    }
		    work[j] += s;
		    i__1 = *n - 1;
		    for (j = k; j <= i__1; ++j) {
			s = 0.;
			i__2 = j - k - 1;
			for (i__ = 0; i__ <= i__2; ++i__) {
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       A(i,j-k) */
			    work[i__] += aa;
			    s += aa;
			}
/*                    i=j-k */
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    A(j-k,j-k) */
			s += aa;
			work[j - k] += s;
			++i__;
			s = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    A(j,j) */
			i__2 = *n - 1;
			for (l = j + 1; l <= i__2; ++l) {
			    ++i__;
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       A(j,l) */
			    work[l] += aa;
			    s += aa;
			}
			work[j] += s;
		    }
		    i__ = idamax_(n, work, &c__1);
		    value = work[i__ - 1];
		} else {
/*                 ilu=1 */
		    ++k;
/*                 k=(n+1)/2 for n odd and ilu=1 */
		    i__1 = *n - 1;
		    for (i__ = k; i__ <= i__1; ++i__) {
			work[i__] = 0.;
		    }
		    i__1 = k - 2;
		    for (j = 0; j <= i__1; ++j) {
/*                    process */
			s = 0.;
			i__2 = j - 1;
			for (i__ = 0; i__ <= i__2; ++i__) {
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       A(j,i) */
			    work[i__] += aa;
			    s += aa;
			}
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    i=j so process of A(j,j) */
			s += aa;
			work[j] = s;
/*                    is initialised here */
			++i__;
/*                    i=j process A(j+k,j+k) */
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
			s = aa;
			i__2 = *n - 1;
			for (l = k + j + 1; l <= i__2; ++l) {
			    ++i__;
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       A(l,k+j) */
			    s += aa;
			    work[l] += aa;
			}
			work[k + j] += s;
		    }
/*                 j=k-1 is special :process col A(k-1,0:k-1) */
		    s = 0.;
		    i__1 = k - 2;
		    for (i__ = 0; i__ <= i__1; ++i__) {
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    A(k,i) */
			work[i__] += aa;
			s += aa;
		    }
/*                 i=k-1 */
		    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                 A(k-1,k-1) */
		    s += aa;
		    work[i__] = s;
/*                 done with col j=k+1 */
		    i__1 = *n - 1;
		    for (j = k; j <= i__1; ++j) {
/*                    process col j of A = A(j,0:k-1) */
			s = 0.;
			i__2 = k - 1;
			for (i__ = 0; i__ <= i__2; ++i__) {
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       A(j,i) */
			    work[i__] += aa;
			    s += aa;
			}
			work[j] += s;
		    }
		    i__ = idamax_(n, work, &c__1);
		    value = work[i__ - 1];
		}
	    } else {
/*              n is even */
		if (ilu == 0) {
		    i__1 = *n - 1;
		    for (i__ = k; i__ <= i__1; ++i__) {
			work[i__] = 0.;
		    }
		    i__1 = k - 1;
		    for (j = 0; j <= i__1; ++j) {
			s = 0.;
			i__2 = k - 1;
			for (i__ = 0; i__ <= i__2; ++i__) {
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       A(j,i+k) */
			    work[i__ + k] += aa;
			    s += aa;
			}
			work[j] = s;
		    }
/*                 j=k */
		    aa = (d__1 = a[j * lda], abs(d__1));
/*                 A(k,k) */
		    s = aa;
		    i__1 = k - 1;
		    for (i__ = 1; i__ <= i__1; ++i__) {
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    A(k,k+i) */
			work[i__ + k] += aa;
			s += aa;
		    }
		    work[j] += s;
		    i__1 = *n - 1;
		    for (j = k + 1; j <= i__1; ++j) {
			s = 0.;
			i__2 = j - 2 - k;
			for (i__ = 0; i__ <= i__2; ++i__) {
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       A(i,j-k-1) */
			    work[i__] += aa;
			    s += aa;
			}
/*                     i=j-1-k */
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    A(j-k-1,j-k-1) */
			s += aa;
			work[j - k - 1] += s;
			++i__;
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    A(j,j) */
			s = aa;
			i__2 = *n - 1;
			for (l = j + 1; l <= i__2; ++l) {
			    ++i__;
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       A(j,l) */
			    work[l] += aa;
			    s += aa;
			}
			work[j] += s;
		    }
/*                 j=n */
		    s = 0.;
		    i__1 = k - 2;
		    for (i__ = 0; i__ <= i__1; ++i__) {
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    A(i,k-1) */
			work[i__] += aa;
			s += aa;
		    }
/*                 i=k-1 */
		    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                 A(k-1,k-1) */
		    s += aa;
		    work[i__] += s;
		    i__ = idamax_(n, work, &c__1);
		    value = work[i__ - 1];
		} else {
/*                 ilu=1 */
		    i__1 = *n - 1;
		    for (i__ = k; i__ <= i__1; ++i__) {
			work[i__] = 0.;
		    }
/*                 j=0 is special :process col A(k:n-1,k) */
		    s = abs(a[0]);
/*                 A(k,k) */
		    i__1 = k - 1;
		    for (i__ = 1; i__ <= i__1; ++i__) {
			aa = (d__1 = a[i__], abs(d__1));
/*                    A(k+i,k) */
			work[i__ + k] += aa;
			s += aa;
		    }
		    work[k] += s;
		    i__1 = k - 1;
		    for (j = 1; j <= i__1; ++j) {
/*                    process */
			s = 0.;
			i__2 = j - 2;
			for (i__ = 0; i__ <= i__2; ++i__) {
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       A(j-1,i) */
			    work[i__] += aa;
			    s += aa;
			}
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    i=j-1 so process of A(j-1,j-1) */
			s += aa;
			work[j - 1] = s;
/*                    is initialised here */
			++i__;
/*                    i=j process A(j+k,j+k) */
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
			s = aa;
			i__2 = *n - 1;
			for (l = k + j + 1; l <= i__2; ++l) {
			    ++i__;
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       A(l,k+j) */
			    s += aa;
			    work[l] += aa;
			}
			work[k + j] += s;
		    }
/*                 j=k is special :process col A(k,0:k-1) */
		    s = 0.;
		    i__1 = k - 2;
		    for (i__ = 0; i__ <= i__1; ++i__) {
			aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                    A(k,i) */
			work[i__] += aa;
			s += aa;
		    }
/*                 i=k-1 */
		    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                 A(k-1,k-1) */
		    s += aa;
		    work[i__] = s;
/*                 done with col j=k+1 */
		    i__1 = *n;
		    for (j = k + 1; j <= i__1; ++j) {
/*                    process col j-1 of A = A(j-1,0:k-1) */
			s = 0.;
			i__2 = k - 1;
			for (i__ = 0; i__ <= i__2; ++i__) {
			    aa = (d__1 = a[i__ + j * lda], abs(d__1));
/*                       A(j-1,i) */
			    work[i__] += aa;
			    s += aa;
			}
			work[j - 1] += s;
		    }
		    i__ = idamax_(n, work, &c__1);
		    value = work[i__ - 1];
		}
	    }
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*       Find normF(A). */

	k = (*n + 1) / 2;
	scale = 0.;
	s = 1.;
	if (noe == 1) {
/*           n is odd */
	    if (ifm == 1) {
/*              A is normal */
		if (ilu == 0) {
/*                 A is upper */
		    i__1 = k - 3;
		    for (j = 0; j <= i__1; ++j) {
			i__2 = k - j - 2;
			dlassq_(&i__2, &a[k + j + 1 + j * lda], &c__1, &scale,
				 &s);
/*                    L at A(k,0) */
		    }
		    i__1 = k - 1;
		    for (j = 0; j <= i__1; ++j) {
			i__2 = k + j - 1;
			dlassq_(&i__2, &a[j * lda], &c__1, &scale, &s);
/*                    trap U at A(0,0) */
		    }
		    s += s;
/*                 double s for the off diagonal elements */
		    i__1 = k - 1;
		    i__2 = lda + 1;
		    dlassq_(&i__1, &a[k], &i__2, &scale, &s);
/*                 tri L at A(k,0) */
		    i__1 = lda + 1;
		    dlassq_(&k, &a[k - 1], &i__1, &scale, &s);
/*                 tri U at A(k-1,0) */
		} else {
/*                 ilu=1 & A is lower */
		    i__1 = k - 1;
		    for (j = 0; j <= i__1; ++j) {
			i__2 = *n - j - 1;
			dlassq_(&i__2, &a[j + 1 + j * lda], &c__1, &scale, &s)
				;
/*                    trap L at A(0,0) */
		    }
		    i__1 = k - 2;
		    for (j = 0; j <= i__1; ++j) {
			dlassq_(&j, &a[(j + 1) * lda], &c__1, &scale, &s);
/*                    U at A(0,1) */
		    }
		    s += s;
/*                 double s for the off diagonal elements */
		    i__1 = lda + 1;
		    dlassq_(&k, a, &i__1, &scale, &s);
/*                 tri L at A(0,0) */
		    i__1 = k - 1;
		    i__2 = lda + 1;
		    dlassq_(&i__1, &a[lda], &i__2, &scale, &s);
/*                 tri U at A(0,1) */
		}
	    } else {
/*              A is xpose */
		if (ilu == 0) {
/*                 A' is upper */
		    i__1 = k - 2;
		    for (j = 1; j <= i__1; ++j) {
			dlassq_(&j, &a[(k + j) * lda], &c__1, &scale, &s);
/*                    U at A(0,k) */
		    }
		    i__1 = k - 2;
		    for (j = 0; j <= i__1; ++j) {
			dlassq_(&k, &a[j * lda], &c__1, &scale, &s);
/*                    k by k-1 rect. at A(0,0) */
		    }
		    i__1 = k - 2;
		    for (j = 0; j <= i__1; ++j) {
			i__2 = k - j - 1;
			dlassq_(&i__2, &a[j + 1 + (j + k - 1) * lda], &c__1, &
				scale, &s);
/*                    L at A(0,k-1) */
		    }
		    s += s;
/*                 double s for the off diagonal elements */
		    i__1 = k - 1;
		    i__2 = lda + 1;
		    dlassq_(&i__1, &a[k * lda], &i__2, &scale, &s);
/*                 tri U at A(0,k) */
		    i__1 = lda + 1;
		    dlassq_(&k, &a[(k - 1) * lda], &i__1, &scale, &s);
/*                 tri L at A(0,k-1) */
		} else {
/*                 A' is lower */
		    i__1 = k - 1;
		    for (j = 1; j <= i__1; ++j) {
			dlassq_(&j, &a[j * lda], &c__1, &scale, &s);
/*                    U at A(0,0) */
		    }
		    i__1 = *n - 1;
		    for (j = k; j <= i__1; ++j) {
			dlassq_(&k, &a[j * lda], &c__1, &scale, &s);
/*                    k by k-1 rect. at A(0,k) */
		    }
		    i__1 = k - 3;
		    for (j = 0; j <= i__1; ++j) {
			i__2 = k - j - 2;
			dlassq_(&i__2, &a[j + 2 + j * lda], &c__1, &scale, &s)
				;
/*                    L at A(1,0) */
		    }
		    s += s;
/*                 double s for the off diagonal elements */
		    i__1 = lda + 1;
		    dlassq_(&k, a, &i__1, &scale, &s);
/*                 tri U at A(0,0) */
		    i__1 = k - 1;
		    i__2 = lda + 1;
		    dlassq_(&i__1, &a[1], &i__2, &scale, &s);
/*                 tri L at A(1,0) */
		}
	    }
	} else {
/*           n is even */
	    if (ifm == 1) {
/*              A is normal */
		if (ilu == 0) {
/*                 A is upper */
		    i__1 = k - 2;
		    for (j = 0; j <= i__1; ++j) {
			i__2 = k - j - 1;
			dlassq_(&i__2, &a[k + j + 2 + j * lda], &c__1, &scale,
				 &s);
/*                    L at A(k+1,0) */
		    }
		    i__1 = k - 1;
		    for (j = 0; j <= i__1; ++j) {
			i__2 = k + j;
			dlassq_(&i__2, &a[j * lda], &c__1, &scale, &s);
/*                    trap U at A(0,0) */
		    }
		    s += s;
/*                 double s for the off diagonal elements */
		    i__1 = lda + 1;
		    dlassq_(&k, &a[k + 1], &i__1, &scale, &s);
/*                 tri L at A(k+1,0) */
		    i__1 = lda + 1;
		    dlassq_(&k, &a[k], &i__1, &scale, &s);
/*                 tri U at A(k,0) */
		} else {
/*                 ilu=1 & A is lower */
		    i__1 = k - 1;
		    for (j = 0; j <= i__1; ++j) {
			i__2 = *n - j - 1;
			dlassq_(&i__2, &a[j + 2 + j * lda], &c__1, &scale, &s)
				;
/*                    trap L at A(1,0) */
		    }
		    i__1 = k - 1;
		    for (j = 1; j <= i__1; ++j) {
			dlassq_(&j, &a[j * lda], &c__1, &scale, &s);
/*                    U at A(0,0) */
		    }
		    s += s;
/*                 double s for the off diagonal elements */
		    i__1 = lda + 1;
		    dlassq_(&k, &a[1], &i__1, &scale, &s);
/*                 tri L at A(1,0) */
		    i__1 = lda + 1;
		    dlassq_(&k, a, &i__1, &scale, &s);
/*                 tri U at A(0,0) */
		}
	    } else {
/*              A is xpose */
		if (ilu == 0) {
/*                 A' is upper */
		    i__1 = k - 1;
		    for (j = 1; j <= i__1; ++j) {
			dlassq_(&j, &a[(k + 1 + j) * lda], &c__1, &scale, &s);
/*                    U at A(0,k+1) */
		    }
		    i__1 = k - 1;
		    for (j = 0; j <= i__1; ++j) {
			dlassq_(&k, &a[j * lda], &c__1, &scale, &s);
/*                    k by k rect. at A(0,0) */
		    }
		    i__1 = k - 2;
		    for (j = 0; j <= i__1; ++j) {
			i__2 = k - j - 1;
			dlassq_(&i__2, &a[j + 1 + (j + k) * lda], &c__1, &
				scale, &s);
/*                    L at A(0,k) */
		    }
		    s += s;
/*                 double s for the off diagonal elements */
		    i__1 = lda + 1;
		    dlassq_(&k, &a[(k + 1) * lda], &i__1, &scale, &s);
/*                 tri U at A(0,k+1) */
		    i__1 = lda + 1;
		    dlassq_(&k, &a[k * lda], &i__1, &scale, &s);
/*                 tri L at A(0,k) */
		} else {
/*                 A' is lower */
		    i__1 = k - 1;
		    for (j = 1; j <= i__1; ++j) {
			dlassq_(&j, &a[(j + 1) * lda], &c__1, &scale, &s);
/*                    U at A(0,1) */
		    }
		    i__1 = *n;
		    for (j = k + 1; j <= i__1; ++j) {
			dlassq_(&k, &a[j * lda], &c__1, &scale, &s);
/*                    k by k rect. at A(0,k+1) */
		    }
		    i__1 = k - 2;
		    for (j = 0; j <= i__1; ++j) {
			i__2 = k - j - 1;
			dlassq_(&i__2, &a[j + 1 + j * lda], &c__1, &scale, &s)
				;
/*                    L at A(0,0) */
		    }
		    s += s;
/*                 double s for the off diagonal elements */
		    i__1 = lda + 1;
		    dlassq_(&k, &a[lda], &i__1, &scale, &s);
/*                 tri L at A(0,1) */
		    i__1 = lda + 1;
		    dlassq_(&k, a, &i__1, &scale, &s);
/*                 tri U at A(0,0) */
		}
	    }
	}
	value = scale * sqrt(s);
    }

    ret_val = value;
    return ret_val;

/*     End of DLANSF */

} /* dlansf_ */

/* Subroutine */ double dlansp_(const char *norm, const char *uplo, integer *n, double *ap,
	double *work)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer i__1, i__2;
    double ret_val, d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, k;
    double sum, absa, scale;
    double value;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLANSP  returns the value of the one norm,  or the Frobenius norm, or */
/*  the  infinity norm,  or the  element of  largest absolute value  of a */
/*  real symmetric matrix A,  supplied in packed form. */

/*  Description */
/*  =========== */

/*  DLANSP returns the value */

/*     DLANSP = ( max(abs(A(i,j))), NORM = 'M' or 'm' */
/*              ( */
/*              ( norm1(A),         NORM = '1', 'O' or 'o' */
/*              ( */
/*              ( normI(A),         NORM = 'I' or 'i' */
/*              ( */
/*              ( normF(A),         NORM = 'F', 'f', 'E' or 'e' */

/*  where  norm1  denotes the  one norm of a matrix (maximum column sum), */
/*  normI  denotes the  infinity norm  of a matrix  (maximum row sum) and */
/*  normF  denotes the  Frobenius norm of a matrix (square root of sum of */
/*  squares).  Note that  max(abs(A(i,j)))  is not a consistent matrix norm. */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies the value to be returned in DLANSP as described */
/*          above. */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is supplied. */
/*          = 'U':  Upper triangular part of A is supplied */
/*          = 'L':  Lower triangular part of A is supplied */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0.  When N = 0, DLANSP is */
/*          set to zero. */

/*  AP      (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The upper or lower triangle of the symmetric matrix A, packed */
/*          columnwise in a linear array.  The j-th column of A is stored */
/*          in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2n-j)/2) = A(i,j) for j<=i<=n. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (MAX(1,LWORK)), */
/*          where LWORK >= N when NORM = 'I' or '1' or 'O'; otherwise, */
/*          WORK is not referenced. */

/* ===================================================================== */

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
    --ap;

    /* Function Body */
    if (*n == 0) {
	value = 0.;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	value = 0.;
	if (lsame_(uplo, "U")) {
	    k = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = k + j - 1;
		for (i__ = k; i__ <= i__2; ++i__) {
/* Computing MAX */
		    d__2 = value, d__3 = (d__1 = ap[i__], abs(d__1));
		    value = std::max(d__2,d__3);
/* L10: */
		}
		k += j;
/* L20: */
	    }
	} else {
	    k = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = k + *n - j;
		for (i__ = k; i__ <= i__2; ++i__) {
/* Computing MAX */
		    d__2 = value, d__3 = (d__1 = ap[i__], abs(d__1));
		    value = std::max(d__2,d__3);
/* L30: */
		}
		k = k + *n - j + 1;
/* L40: */
	    }
	}
    } else if (lsame_(norm, "I") || lsame_(norm, "O") || *(unsigned char *)norm == '1') {

/*        Find normI(A) ( = norm1(A), since A is symmetric). */

	value = 0.;
	k = 1;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		sum = 0.;
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    absa = (d__1 = ap[k], abs(d__1));
		    sum += absa;
		    work[i__] += absa;
		    ++k;
/* L50: */
		}
		work[j] = sum + (d__1 = ap[k], abs(d__1));
		++k;
/* L60: */
	    }
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
		d__1 = value, d__2 = work[i__];
		value = std::max(d__1,d__2);
/* L70: */
	    }
	} else {
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		work[i__] = 0.;
/* L80: */
	    }
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		sum = work[j] + (d__1 = ap[k], abs(d__1));
		++k;
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    absa = (d__1 = ap[k], abs(d__1));
		    sum += absa;
		    work[i__] += absa;
		    ++k;
/* L90: */
		}
		value = std::max(value,sum);
/* L100: */
	    }
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.;
	sum = 1.;
	k = 2;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 2; j <= i__1; ++j) {
		i__2 = j - 1;
		dlassq_(&i__2, &ap[k], &c__1, &scale, &sum);
		k += j;
/* L110: */
	    }
	} else {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j;
		dlassq_(&i__2, &ap[k], &c__1, &scale, &sum);
		k = k + *n - j + 1;
/* L120: */
	    }
	}
	sum *= 2;
	k = 1;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (ap[k] != 0.) {
		absa = (d__1 = ap[k], abs(d__1));
		if (scale < absa) {
/* Computing 2nd power */
		    d__1 = scale / absa;
		    sum = sum * (d__1 * d__1) + 1.;
		    scale = absa;
		} else {
/* Computing 2nd power */
		    d__1 = absa / scale;
		    sum += d__1 * d__1;
		}
	    }
	    if (lsame_(uplo, "U")) {
		k = k + i__ + 1;
	    } else {
		k = k + *n - i__ + 1;
	    }
/* L130: */
	}
	value = scale * sqrt(sum);
    }

    ret_val = value;
    return ret_val;

/*     End of DLANSP */

} /* dlansp_ */

/* Subroutine */ double dlanst_(const char *norm, integer *n, double *d__, double *e)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer i__1;
    double ret_val, d__1, d__2, d__3, d__4, d__5;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    integer i__;
    double sum, scale;
    double anorm;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLANST  returns the value of the one norm,  or the Frobenius norm, or */
/*  the  infinity norm,  or the  element of  largest absolute value  of a */
/*  real symmetric tridiagonal matrix A. */

/*  Description */
/*  =========== */

/*  DLANST returns the value */

/*     DLANST = ( max(abs(A(i,j))), NORM = 'M' or 'm' */
/*              ( */
/*              ( norm1(A),         NORM = '1', 'O' or 'o' */
/*              ( */
/*              ( normI(A),         NORM = 'I' or 'i' */
/*              ( */
/*              ( normF(A),         NORM = 'F', 'f', 'E' or 'e' */

/*  where  norm1  denotes the  one norm of a matrix (maximum column sum), */
/*  normI  denotes the  infinity norm  of a matrix  (maximum row sum) and */
/*  normF  denotes the  Frobenius norm of a matrix (square root of sum of */
/*  squares).  Note that  max(abs(A(i,j)))  is not a consistent matrix norm. */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies the value to be returned in DLANST as described */
/*          above. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0.  When N = 0, DLANST is */
/*          set to zero. */

/*  D       (input) DOUBLE PRECISION array, dimension (N) */
/*          The diagonal elements of A. */

/*  E       (input) DOUBLE PRECISION array, dimension (N-1) */
/*          The (n-1) sub-diagonal or super-diagonal elements of A. */

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
    --e;
    --d__;

    /* Function Body */
    if (*n <= 0) {
	anorm = 0.;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	anorm = (d__1 = d__[*n], abs(d__1));
	i__1 = *n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	    d__2 = anorm, d__3 = (d__1 = d__[i__], abs(d__1));
	    anorm = std::max(d__2,d__3);
/* Computing MAX */
	    d__2 = anorm, d__3 = (d__1 = e[i__], abs(d__1));
	    anorm = std::max(d__2,d__3);
/* L10: */
	}
    } else if (lsame_(norm, "O") || *(unsigned char *)
	    norm == '1' || lsame_(norm, "I")) {

/*        Find norm1(A). */

	if (*n == 1) {
	    anorm = abs(d__[1]);
	} else {
/* Computing MAX */
	    d__3 = abs(d__[1]) + abs(e[1]), d__4 = (d__1 = e[*n - 1], abs(
		    d__1)) + (d__2 = d__[*n], abs(d__2));
	    anorm = std::max(d__3,d__4);
	    i__1 = *n - 1;
	    for (i__ = 2; i__ <= i__1; ++i__) {
/* Computing MAX */
		d__4 = anorm, d__5 = (d__1 = d__[i__], abs(d__1)) + (d__2 = e[
			i__], abs(d__2)) + (d__3 = e[i__ - 1], abs(d__3));
		anorm = std::max(d__4,d__5);
/* L20: */
	    }
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.;
	sum = 1.;
	if (*n > 1) {
	    i__1 = *n - 1;
	    dlassq_(&i__1, &e[1], &c__1, &scale, &sum);
	    sum *= 2;
	}
	dlassq_(n, &d__[1], &c__1, &scale, &sum);
	anorm = scale * sqrt(sum);
    }

    ret_val = anorm;
    return ret_val;

/*     End of DLANST */

} /* dlanst_ */

/* Subroutine */ double dlansy_(const char *norm, const char *uplo, integer *n, double *a, integer
	*lda, double *work)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    double ret_val, d__1, d__2, d__3;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    integer i__, j;
    double sum, absa, scale;
    double value;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLANSY  returns the value of the one norm,  or the Frobenius norm, or */
/*  the  infinity norm,  or the  element of  largest absolute value  of a */
/*  real symmetric matrix A. */

/*  Description */
/*  =========== */

/*  DLANSY returns the value */

/*     DLANSY = ( max(abs(A(i,j))), NORM = 'M' or 'm' */
/*              ( */
/*              ( norm1(A),         NORM = '1', 'O' or 'o' */
/*              ( */
/*              ( normI(A),         NORM = 'I' or 'i' */
/*              ( */
/*              ( normF(A),         NORM = 'F', 'f', 'E' or 'e' */

/*  where  norm1  denotes the  one norm of a matrix (maximum column sum), */
/*  normI  denotes the  infinity norm  of a matrix  (maximum row sum) and */
/*  normF  denotes the  Frobenius norm of a matrix (square root of sum of */
/*  squares).  Note that  max(abs(A(i,j)))  is not a consistent matrix norm. */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies the value to be returned in DLANSY as described */
/*          above. */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is to be referenced. */
/*          = 'U':  Upper triangular part of A is referenced */
/*          = 'L':  Lower triangular part of A is referenced */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0.  When N = 0, DLANSY is */
/*          set to zero. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The symmetric matrix A.  If UPLO = 'U', the leading n by n */
/*          upper triangular part of A contains the upper triangular part */
/*          of the matrix A, and the strictly lower triangular part of A */
/*          is not referenced.  If UPLO = 'L', the leading n by n lower */
/*          triangular part of A contains the lower triangular part of */
/*          the matrix A, and the strictly upper triangular part of A is */
/*          not referenced. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(N,1). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (MAX(1,LWORK)), */
/*          where LWORK >= N when NORM = 'I' or '1' or 'O'; otherwise, */
/*          WORK is not referenced. */

/* ===================================================================== */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --work;

    /* Function Body */
    if (*n == 0) {
	value = 0.;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	value = 0.;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = j;
		for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
		    d__2 = value, d__3 = (d__1 = a[i__ + j * a_dim1], abs(
			    d__1));
		    value = std::max(d__2,d__3);
/* L10: */
		}
/* L20: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n;
		for (i__ = j; i__ <= i__2; ++i__) {
/* Computing MAX */
		    d__2 = value, d__3 = (d__1 = a[i__ + j * a_dim1], abs(
			    d__1));
		    value = std::max(d__2,d__3);
/* L30: */
		}
/* L40: */
	    }
	}
    } else if (lsame_(norm, "I") || lsame_(norm, "O") || *(unsigned char *)norm == '1') {

/*        Find normI(A) ( = norm1(A), since A is symmetric). */

	value = 0.;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		sum = 0.;
		i__2 = j - 1;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    absa = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		    sum += absa;
		    work[i__] += absa;
/* L50: */
		}
		work[j] = sum + (d__1 = a[j + j * a_dim1], abs(d__1));
/* L60: */
	    }
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
		d__1 = value, d__2 = work[i__];
		value = std::max(d__1,d__2);
/* L70: */
	    }
	} else {
	    i__1 = *n;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		work[i__] = 0.;
/* L80: */
	    }
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		sum = work[j] + (d__1 = a[j + j * a_dim1], abs(d__1));
		i__2 = *n;
		for (i__ = j + 1; i__ <= i__2; ++i__) {
		    absa = (d__1 = a[i__ + j * a_dim1], abs(d__1));
		    sum += absa;
		    work[i__] += absa;
/* L90: */
		}
		value = std::max(value,sum);
/* L100: */
	    }
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	scale = 0.;
	sum = 1.;
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 2; j <= i__1; ++j) {
		i__2 = j - 1;
		dlassq_(&i__2, &a[j * a_dim1 + 1], &c__1, &scale, &sum);
/* L110: */
	    }
	} else {
	    i__1 = *n - 1;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *n - j;
		dlassq_(&i__2, &a[j + 1 + j * a_dim1], &c__1, &scale, &sum);
/* L120: */
	    }
	}
	sum *= 2;
	i__1 = *lda + 1;
	dlassq_(n, &a[a_offset], &i__1, &scale, &sum);
	value = scale * sqrt(sum);
    }

    ret_val = value;
    return ret_val;

/*     End of DLANSY */

} /* dlansy_ */

/* Subroutine */ double dlantb_(const char *norm, const char *uplo, const char *diag, integer *n, integer *k,
	 double *ab, integer *ldab, double *work)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer ab_dim1, ab_offset, i__1, i__2, i__3, i__4, i__5;
    double ret_val, d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, l;
    double sum, scale;
    bool udiag;
    double value;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLANTB  returns the value of the one norm,  or the Frobenius norm, or */
/*  the  infinity norm,  or the element of  largest absolute value  of an */
/*  n by n triangular band matrix A,  with ( k + 1 ) diagonals. */

/*  Description */
/*  =========== */

/*  DLANTB returns the value */

/*     DLANTB = ( max(abs(A(i,j))), NORM = 'M' or 'm' */
/*              ( */
/*              ( norm1(A),         NORM = '1', 'O' or 'o' */
/*              ( */
/*              ( normI(A),         NORM = 'I' or 'i' */
/*              ( */
/*              ( normF(A),         NORM = 'F', 'f', 'E' or 'e' */

/*  where  norm1  denotes the  one norm of a matrix (maximum column sum), */
/*  normI  denotes the  infinity norm  of a matrix  (maximum row sum) and */
/*  normF  denotes the  Frobenius norm of a matrix (square root of sum of */
/*  squares).  Note that  max(abs(A(i,j)))  is not a consistent matrix norm. */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies the value to be returned in DLANTB as described */
/*          above. */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the matrix A is upper or lower triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  DIAG    (input) CHARACTER*1 */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0.  When N = 0, DLANTB is */
/*          set to zero. */

/*  K       (input) INTEGER */
/*          The number of super-diagonals of the matrix A if UPLO = 'U', */
/*          or the number of sub-diagonals of the matrix A if UPLO = 'L'. */
/*          K >= 0. */

/*  AB      (input) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          The upper or lower triangular band matrix A, stored in the */
/*          first k+1 rows of AB.  The j-th column of A is stored */
/*          in the j-th column of the array AB as follows: */
/*          if UPLO = 'U', AB(k+1+i-j,j) = A(i,j) for max(1,j-k)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)   = A(i,j) for j<=i<=min(n,j+k). */
/*          Note that when DIAG = 'U', the elements of the array AB */
/*          corresponding to the diagonal elements of the matrix A are */
/*          not referenced, but are assumed to be one. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= K+1. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (MAX(1,LWORK)), */
/*          where LWORK >= N when NORM = 'I'; otherwise, WORK is not */
/*          referenced. */

/* ===================================================================== */

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
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    --work;

    /* Function Body */
    if (*n == 0) {
	value = 0.;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	if (lsame_(diag, "U")) {
	    value = 1.;
	    if (lsame_(uplo, "U")) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
		    i__2 = *k + 2 - j;
		    i__3 = *k;
		    for (i__ = std::max(i__2,1_integer); i__ <= i__3; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = ab[i__ + j * ab_dim1],
				abs(d__1));
			value = std::max(d__2,d__3);
/* L10: */
		    }
/* L20: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		    i__2 = *n + 1 - j, i__4 = *k + 1;
		    i__3 = std::min(i__2,i__4);
		    for (i__ = 2; i__ <= i__3; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = ab[i__ + j * ab_dim1],
				abs(d__1));
			value = std::max(d__2,d__3);
/* L30: */
		    }
/* L40: */
		}
	    }
	} else {
	    value = 0.;
	    if (lsame_(uplo, "U")) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
		    i__3 = *k + 2 - j;
		    i__2 = *k + 1;
		    for (i__ = std::max(i__3,1_integer); i__ <= i__2; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = ab[i__ + j * ab_dim1],
				abs(d__1));
			value = std::max(d__2,d__3);
/* L50: */
		    }
/* L60: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		    i__3 = *n + 1 - j, i__4 = *k + 1;
		    i__2 = std::min(i__3,i__4);
		    for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = ab[i__ + j * ab_dim1],
				abs(d__1));
			value = std::max(d__2,d__3);
/* L70: */
		    }
/* L80: */
		}
	    }
	}
    } else if (lsame_(norm, "O") || *(unsigned char *)
	    norm == '1') {

/*        Find norm1(A). */

	value = 0.;
	udiag = lsame_(diag, "U");
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (udiag) {
		    sum = 1.;
/* Computing MAX */
		    i__2 = *k + 2 - j;
		    i__3 = *k;
		    for (i__ = std::max(i__2,1_integer); i__ <= i__3; ++i__) {
			sum += (d__1 = ab[i__ + j * ab_dim1], abs(d__1));
/* L90: */
		    }
		} else {
		    sum = 0.;
/* Computing MAX */
		    i__3 = *k + 2 - j;
		    i__2 = *k + 1;
		    for (i__ = std::max(i__3,1_integer); i__ <= i__2; ++i__) {
			sum += (d__1 = ab[i__ + j * ab_dim1], abs(d__1));
/* L100: */
		    }
		}
		value = std::max(value,sum);
/* L110: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (udiag) {
		    sum = 1.;
/* Computing MIN */
		    i__3 = *n + 1 - j, i__4 = *k + 1;
		    i__2 = std::min(i__3,i__4);
		    for (i__ = 2; i__ <= i__2; ++i__) {
			sum += (d__1 = ab[i__ + j * ab_dim1], abs(d__1));
/* L120: */
		    }
		} else {
		    sum = 0.;
/* Computing MIN */
		    i__3 = *n + 1 - j, i__4 = *k + 1;
		    i__2 = std::min(i__3,i__4);
		    for (i__ = 1; i__ <= i__2; ++i__) {
			sum += (d__1 = ab[i__ + j * ab_dim1], abs(d__1));
/* L130: */
		    }
		}
		value = std::max(value,sum);
/* L140: */
	    }
	}
    } else if (lsame_(norm, "I")) {

/*        Find normI(A). */

	value = 0.;
	if (lsame_(uplo, "U")) {
	    if (lsame_(diag, "U")) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] = 1.;
/* L150: */
		}
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    l = *k + 1 - j;
/* Computing MAX */
		    i__2 = 1, i__3 = j - *k;
		    i__4 = j - 1;
		    for (i__ = std::max(i__2,i__3); i__ <= i__4; ++i__) {
			work[i__] += (d__1 = ab[l + i__ + j * ab_dim1], abs(
				d__1));
/* L160: */
		    }
/* L170: */
		}
	    } else {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] = 0.;
/* L180: */
		}
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    l = *k + 1 - j;
/* Computing MAX */
		    i__4 = 1, i__2 = j - *k;
		    i__3 = j;
		    for (i__ = std::max(i__4,i__2); i__ <= i__3; ++i__) {
			work[i__] += (d__1 = ab[l + i__ + j * ab_dim1], abs(
				d__1));
/* L190: */
		    }
/* L200: */
		}
	    }
	} else {
	    if (lsame_(diag, "U")) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] = 1.;
/* L210: */
		}
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    l = 1 - j;
/* Computing MIN */
		    i__4 = *n, i__2 = j + *k;
		    i__3 = std::min(i__4,i__2);
		    for (i__ = j + 1; i__ <= i__3; ++i__) {
			work[i__] += (d__1 = ab[l + i__ + j * ab_dim1], abs(
				d__1));
/* L220: */
		    }
/* L230: */
		}
	    } else {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] = 0.;
/* L240: */
		}
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    l = 1 - j;
/* Computing MIN */
		    i__4 = *n, i__2 = j + *k;
		    i__3 = std::min(i__4,i__2);
		    for (i__ = j; i__ <= i__3; ++i__) {
			work[i__] += (d__1 = ab[l + i__ + j * ab_dim1], abs(
				d__1));
/* L250: */
		    }
/* L260: */
		}
	    }
	}
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	    d__1 = value, d__2 = work[i__];
	    value = std::max(d__1,d__2);
/* L270: */
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	if (lsame_(uplo, "U")) {
	    if (lsame_(diag, "U")) {
		scale = 1.;
		sum = (double) (*n);
		if (*k > 0) {
		    i__1 = *n;
		    for (j = 2; j <= i__1; ++j) {
/* Computing MIN */
			i__4 = j - 1;
			i__3 = std::min(i__4,*k);
/* Computing MAX */
			i__2 = *k + 2 - j;
			dlassq_(&i__3, &ab[std::max(i__2,1_integer)+ j * ab_dim1], &c__1,
				&scale, &sum);
/* L280: */
		    }
		}
	    } else {
		scale = 0.;
		sum = 1.;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		    i__4 = j, i__2 = *k + 1;
		    i__3 = std::min(i__4,i__2);
/* Computing MAX */
		    i__5 = *k + 2 - j;
		    dlassq_(&i__3, &ab[std::max(i__5,1_integer)+ j * ab_dim1], &c__1, &
			    scale, &sum);
/* L290: */
		}
	    }
	} else {
	    if (lsame_(diag, "U")) {
		scale = 1.;
		sum = (double) (*n);
		if (*k > 0) {
		    i__1 = *n - 1;
		    for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
			i__4 = *n - j;
			i__3 = std::min(i__4,*k);
			dlassq_(&i__3, &ab[j * ab_dim1 + 2], &c__1, &scale, &
				sum);
/* L300: */
		    }
		}
	    } else {
		scale = 0.;
		sum = 1.;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		    i__4 = *n - j + 1, i__2 = *k + 1;
		    i__3 = std::min(i__4,i__2);
		    dlassq_(&i__3, &ab[j * ab_dim1 + 1], &c__1, &scale, &sum);
/* L310: */
		}
	    }
	}
	value = scale * sqrt(sum);
    }

    ret_val = value;
    return ret_val;

/*     End of DLANTB */

} /* dlantb_ */

/* Subroutine */ double dlantp_(const char *norm, const char *uplo, const char *diag, integer *n, double
	*ap, double *work)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer i__1, i__2;
    double ret_val, d__1, d__2, d__3;

    /* Local variables */
    integer i__, j, k;
    double sum, scale;
    bool udiag;
    double value;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLANTP  returns the value of the one norm,  or the Frobenius norm, or */
/*  the  infinity norm,  or the  element of  largest absolute value  of a */
/*  triangular matrix A, supplied in packed form. */

/*  Description */
/*  =========== */

/*  DLANTP returns the value */

/*     DLANTP = ( max(abs(A(i,j))), NORM = 'M' or 'm' */
/*              ( */
/*              ( norm1(A),         NORM = '1', 'O' or 'o' */
/*              ( */
/*              ( normI(A),         NORM = 'I' or 'i' */
/*              ( */
/*              ( normF(A),         NORM = 'F', 'f', 'E' or 'e' */

/*  where  norm1  denotes the  one norm of a matrix (maximum column sum), */
/*  normI  denotes the  infinity norm  of a matrix  (maximum row sum) and */
/*  normF  denotes the  Frobenius norm of a matrix (square root of sum of */
/*  squares).  Note that  max(abs(A(i,j)))  is not a consistent matrix norm. */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies the value to be returned in DLANTP as described */
/*          above. */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the matrix A is upper or lower triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  DIAG    (input) CHARACTER*1 */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0.  When N = 0, DLANTP is */
/*          set to zero. */

/*  AP      (input) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          The upper or lower triangular matrix A, packed columnwise in */
/*          a linear array.  The j-th column of A is stored in the array */
/*          AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2n-j)/2) = A(i,j) for j<=i<=n. */
/*          Note that when DIAG = 'U', the elements of the array AP */
/*          corresponding to the diagonal elements of the matrix A are */
/*          not referenced, but are assumed to be one. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (MAX(1,LWORK)), */
/*          where LWORK >= N when NORM = 'I'; otherwise, WORK is not */
/*          referenced. */

/* ===================================================================== */

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
    --ap;

    /* Function Body */
    if (*n == 0) {
	value = 0.;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	k = 1;
	if (lsame_(diag, "U")) {
	    value = 1.;
	    if (lsame_(uplo, "U")) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = k + j - 2;
		    for (i__ = k; i__ <= i__2; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = ap[i__], abs(d__1));
			value = std::max(d__2,d__3);
/* L10: */
		    }
		    k += j;
/* L20: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = k + *n - j;
		    for (i__ = k + 1; i__ <= i__2; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = ap[i__], abs(d__1));
			value = std::max(d__2,d__3);
/* L30: */
		    }
		    k = k + *n - j + 1;
/* L40: */
		}
	    }
	} else {
	    value = 0.;
	    if (lsame_(uplo, "U")) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = k + j - 1;
		    for (i__ = k; i__ <= i__2; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = ap[i__], abs(d__1));
			value = std::max(d__2,d__3);
/* L50: */
		    }
		    k += j;
/* L60: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = k + *n - j;
		    for (i__ = k; i__ <= i__2; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = ap[i__], abs(d__1));
			value = std::max(d__2,d__3);
/* L70: */
		    }
		    k = k + *n - j + 1;
/* L80: */
		}
	    }
	}
    } else if (lsame_(norm, "O") || *(unsigned char *)
	    norm == '1') {

/*        Find norm1(A). */

	value = 0.;
	k = 1;
	udiag = lsame_(diag, "U");
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (udiag) {
		    sum = 1.;
		    i__2 = k + j - 2;
		    for (i__ = k; i__ <= i__2; ++i__) {
			sum += (d__1 = ap[i__], abs(d__1));
/* L90: */
		    }
		} else {
		    sum = 0.;
		    i__2 = k + j - 1;
		    for (i__ = k; i__ <= i__2; ++i__) {
			sum += (d__1 = ap[i__], abs(d__1));
/* L100: */
		    }
		}
		k += j;
		value = std::max(value,sum);
/* L110: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (udiag) {
		    sum = 1.;
		    i__2 = k + *n - j;
		    for (i__ = k + 1; i__ <= i__2; ++i__) {
			sum += (d__1 = ap[i__], abs(d__1));
/* L120: */
		    }
		} else {
		    sum = 0.;
		    i__2 = k + *n - j;
		    for (i__ = k; i__ <= i__2; ++i__) {
			sum += (d__1 = ap[i__], abs(d__1));
/* L130: */
		    }
		}
		k = k + *n - j + 1;
		value = std::max(value,sum);
/* L140: */
	    }
	}
    } else if (lsame_(norm, "I")) {

/*        Find normI(A). */

	k = 1;
	if (lsame_(uplo, "U")) {
	    if (lsame_(diag, "U")) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] = 1.;
/* L150: */
		}
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = j - 1;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			work[i__] += (d__1 = ap[k], abs(d__1));
			++k;
/* L160: */
		    }
		    ++k;
/* L170: */
		}
	    } else {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] = 0.;
/* L180: */
		}
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = j;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			work[i__] += (d__1 = ap[k], abs(d__1));
			++k;
/* L190: */
		    }
/* L200: */
		}
	    }
	} else {
	    if (lsame_(diag, "U")) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] = 1.;
/* L210: */
		}
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    ++k;
		    i__2 = *n;
		    for (i__ = j + 1; i__ <= i__2; ++i__) {
			work[i__] += (d__1 = ap[k], abs(d__1));
			++k;
/* L220: */
		    }
/* L230: */
		}
	    } else {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] = 0.;
/* L240: */
		}
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *n;
		    for (i__ = j; i__ <= i__2; ++i__) {
			work[i__] += (d__1 = ap[k], abs(d__1));
			++k;
/* L250: */
		    }
/* L260: */
		}
	    }
	}
	value = 0.;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	    d__1 = value, d__2 = work[i__];
	    value = std::max(d__1,d__2);
/* L270: */
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	if (lsame_(uplo, "U")) {
	    if (lsame_(diag, "U")) {
		scale = 1.;
		sum = (double) (*n);
		k = 2;
		i__1 = *n;
		for (j = 2; j <= i__1; ++j) {
		    i__2 = j - 1;
		    dlassq_(&i__2, &ap[k], &c__1, &scale, &sum);
		    k += j;
/* L280: */
		}
	    } else {
		scale = 0.;
		sum = 1.;
		k = 1;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    dlassq_(&j, &ap[k], &c__1, &scale, &sum);
		    k += j;
/* L290: */
		}
	    }
	} else {
	    if (lsame_(diag, "U")) {
		scale = 1.;
		sum = (double) (*n);
		k = 2;
		i__1 = *n - 1;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *n - j;
		    dlassq_(&i__2, &ap[k], &c__1, &scale, &sum);
		    k = k + *n - j + 1;
/* L300: */
		}
	    } else {
		scale = 0.;
		sum = 1.;
		k = 1;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *n - j + 1;
		    dlassq_(&i__2, &ap[k], &c__1, &scale, &sum);
		    k = k + *n - j + 1;
/* L310: */
		}
	    }
	}
	value = scale * sqrt(sum);
    }

    ret_val = value;
    return ret_val;

/*     End of DLANTP */

} /* dlantp_ */

/* Subroutine */ double dlantr_(const char *norm, const char *uplo, const char *diag, integer *m, integer *n,
	 double *a, integer *lda, double *work)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3, i__4;
    double ret_val, d__1, d__2, d__3;

    /* Local variables */
    integer i__, j;
    double sum, scale;
    bool udiag;
    double value;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLANTR  returns the value of the one norm,  or the Frobenius norm, or */
/*  the  infinity norm,  or the  element of  largest absolute value  of a */
/*  trapezoidal or triangular matrix A. */

/*  Description */
/*  =========== */

/*  DLANTR returns the value */

/*     DLANTR = ( max(abs(A(i,j))), NORM = 'M' or 'm' */
/*              ( */
/*              ( norm1(A),         NORM = '1', 'O' or 'o' */
/*              ( */
/*              ( normI(A),         NORM = 'I' or 'i' */
/*              ( */
/*              ( normF(A),         NORM = 'F', 'f', 'E' or 'e' */

/*  where  norm1  denotes the  one norm of a matrix (maximum column sum), */
/*  normI  denotes the  infinity norm  of a matrix  (maximum row sum) and */
/*  normF  denotes the  Frobenius norm of a matrix (square root of sum of */
/*  squares).  Note that  max(abs(A(i,j)))  is not a consistent matrix norm. */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies the value to be returned in DLANTR as described */
/*          above. */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the matrix A is upper or lower trapezoidal. */
/*          = 'U':  Upper trapezoidal */
/*          = 'L':  Lower trapezoidal */
/*          Note that A is triangular instead of trapezoidal if M = N. */

/*  DIAG    (input) CHARACTER*1 */
/*          Specifies whether or not the matrix A has unit diagonal. */
/*          = 'N':  Non-unit diagonal */
/*          = 'U':  Unit diagonal */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0, and if */
/*          UPLO = 'U', M <= N.  When M = 0, DLANTR is set to zero. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0, and if */
/*          UPLO = 'L', N <= M.  When N = 0, DLANTR is set to zero. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The trapezoidal matrix A (A is triangular if M = N). */
/*          If UPLO = 'U', the leading m by n upper trapezoidal part of */
/*          the array A contains the upper trapezoidal matrix, and the */
/*          strictly lower triangular part of A is not referenced. */
/*          If UPLO = 'L', the leading m by n lower trapezoidal part of */
/*          the array A contains the lower trapezoidal matrix, and the */
/*          strictly upper triangular part of A is not referenced.  Note */
/*          that when DIAG = 'U', the diagonal elements of A are not */
/*          referenced and are assumed to be one. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(M,1). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (MAX(1,LWORK)), */
/*          where LWORK >= M when NORM = 'I'; otherwise, WORK is not */
/*          referenced. */

/* ===================================================================== */

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
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --work;

    /* Function Body */
    if (std::min(*m,*n) == 0) {
	value = 0.;
    } else if (lsame_(norm, "M")) {

/*        Find max(abs(A(i,j))). */

	if (lsame_(diag, "U")) {
	    value = 1.;
	    if (lsame_(uplo, "U")) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		    i__3 = *m, i__4 = j - 1;
		    i__2 = std::min(i__3,i__4);
		    for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = a[i__ + j * a_dim1], abs(
				d__1));
			value = std::max(d__2,d__3);
/* L10: */
		    }
/* L20: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m;
		    for (i__ = j + 1; i__ <= i__2; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = a[i__ + j * a_dim1], abs(
				d__1));
			value = std::max(d__2,d__3);
/* L30: */
		    }
/* L40: */
		}
	    }
	} else {
	    value = 0.;
	    if (lsame_(uplo, "U")) {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = std::min(*m,j);
		    for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = a[i__ + j * a_dim1], abs(
				d__1));
			value = std::max(d__2,d__3);
/* L50: */
		    }
/* L60: */
		}
	    } else {
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m;
		    for (i__ = j; i__ <= i__2; ++i__) {
/* Computing MAX */
			d__2 = value, d__3 = (d__1 = a[i__ + j * a_dim1], abs(
				d__1));
			value = std::max(d__2,d__3);
/* L70: */
		    }
/* L80: */
		}
	    }
	}
    } else if (lsame_(norm, "O") || *(unsigned char *)
	    norm == '1') {

/*        Find norm1(A). */

	value = 0.;
	udiag = lsame_(diag, "U");
	if (lsame_(uplo, "U")) {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (udiag && j <= *m) {
		    sum = 1.;
		    i__2 = j - 1;
		    for (i__ = 1; i__ <= i__2; ++i__) {
			sum += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L90: */
		    }
		} else {
		    sum = 0.;
		    i__2 = std::min(*m,j);
		    for (i__ = 1; i__ <= i__2; ++i__) {
			sum += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L100: */
		    }
		}
		value = std::max(value,sum);
/* L110: */
	    }
	} else {
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (udiag) {
		    sum = 1.;
		    i__2 = *m;
		    for (i__ = j + 1; i__ <= i__2; ++i__) {
			sum += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L120: */
		    }
		} else {
		    sum = 0.;
		    i__2 = *m;
		    for (i__ = j; i__ <= i__2; ++i__) {
			sum += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L130: */
		    }
		}
		value = std::max(value,sum);
/* L140: */
	    }
	}
    } else if (lsame_(norm, "I")) {

/*        Find normI(A). */

	if (lsame_(uplo, "U")) {
	    if (lsame_(diag, "U")) {
		i__1 = *m;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] = 1.;
/* L150: */
		}
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
/* Computing MIN */
		    i__3 = *m, i__4 = j - 1;
		    i__2 = std::min(i__3,i__4);
		    for (i__ = 1; i__ <= i__2; ++i__) {
			work[i__] += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L160: */
		    }
/* L170: */
		}
	    } else {
		i__1 = *m;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] = 0.;
/* L180: */
		}
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = std::min(*m,j);
		    for (i__ = 1; i__ <= i__2; ++i__) {
			work[i__] += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L190: */
		    }
/* L200: */
		}
	    }
	} else {
	    if (lsame_(diag, "U")) {
		i__1 = *n;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] = 1.;
/* L210: */
		}
		i__1 = *m;
		for (i__ = *n + 1; i__ <= i__1; ++i__) {
		    work[i__] = 0.;
/* L220: */
		}
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m;
		    for (i__ = j + 1; i__ <= i__2; ++i__) {
			work[i__] += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L230: */
		    }
/* L240: */
		}
	    } else {
		i__1 = *m;
		for (i__ = 1; i__ <= i__1; ++i__) {
		    work[i__] = 0.;
/* L250: */
		}
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m;
		    for (i__ = j; i__ <= i__2; ++i__) {
			work[i__] += (d__1 = a[i__ + j * a_dim1], abs(d__1));
/* L260: */
		    }
/* L270: */
		}
	    }
	}
	value = 0.;
	i__1 = *m;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing MAX */
	    d__1 = value, d__2 = work[i__];
	    value = std::max(d__1,d__2);
/* L280: */
	}
    } else if (lsame_(norm, "F") || lsame_(norm, "E")) {

/*        Find normF(A). */

	if (lsame_(uplo, "U")) {
	    if (lsame_(diag, "U")) {
		scale = 1.;
		sum = (double) std::min(*m,*n);
		i__1 = *n;
		for (j = 2; j <= i__1; ++j) {
/* Computing MIN */
		    i__3 = *m, i__4 = j - 1;
		    i__2 = std::min(i__3,i__4);
		    dlassq_(&i__2, &a[j * a_dim1 + 1], &c__1, &scale, &sum);
/* L290: */
		}
	    } else {
		scale = 0.;
		sum = 1.;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = std::min(*m,j);
		    dlassq_(&i__2, &a[j * a_dim1 + 1], &c__1, &scale, &sum);
/* L300: */
		}
	    }
	} else {
	    if (lsame_(diag, "U")) {
		scale = 1.;
		sum = (double) std::min(*m,*n);
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m - j;
/* Computing MIN */
		    i__3 = *m, i__4 = j + 1;
		    dlassq_(&i__2, &a[std::min(i__3, i__4)+ j * a_dim1], &c__1, &
			    scale, &sum);
/* L310: */
		}
	    } else {
		scale = 0.;
		sum = 1.;
		i__1 = *n;
		for (j = 1; j <= i__1; ++j) {
		    i__2 = *m - j + 1;
		    dlassq_(&i__2, &a[j + j * a_dim1], &c__1, &scale, &sum);
/* L320: */
		}
	    }
	}
	value = scale * sqrt(sum);
    }

    ret_val = value;
    return ret_val;

/*     End of DLANTR */

} /* dlantr_ */

/* Subroutine */ int dlanv2_(double *a, double *b, double *c__,
	double *d__, double *rt1r, double *rt1i, double *rt2r,
	 double *rt2i, double *cs, double *sn)
{
	/* Table of constant values */
	static double c_b4 = 1.;

    /* System generated locals */
    double d__1, d__2;

    /* Builtin functions
    double d_sign(double *, double *), sqrt(double); */

    /* Local variables */
    double p, z__, aa, bb, cc, dd, cs1, sn1, sab, sac, eps, tau, temp,
	    scale, bcmax, bcmis, sigma;



/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLANV2 computes the Schur factorization of a real 2-by-2 nonsymmetric */
/*  matrix in standard form: */

/*       [ A  B ] = [ CS -SN ] [ AA  BB ] [ CS  SN ] */
/*       [ C  D ]   [ SN  CS ] [ CC  DD ] [-SN  CS ] */

/*  where either */
/*  1) CC = 0 so that AA and DD are real eigenvalues of the matrix, or */
/*  2) AA = DD and BB*CC < 0, so that AA + or - sqrt(BB*CC) are complex */
/*  conjugate eigenvalues. */

/*  Arguments */
/*  ========= */

/*  A       (input/output) DOUBLE PRECISION */
/*  B       (input/output) DOUBLE PRECISION */
/*  C       (input/output) DOUBLE PRECISION */
/*  D       (input/output) DOUBLE PRECISION */
/*          On entry, the elements of the input matrix. */
/*          On exit, they are overwritten by the elements of the */
/*          standardised Schur form. */

/*  RT1R    (output) DOUBLE PRECISION */
/*  RT1I    (output) DOUBLE PRECISION */
/*  RT2R    (output) DOUBLE PRECISION */
/*  RT2I    (output) DOUBLE PRECISION */
/*          The real and imaginary parts of the eigenvalues. If the */
/*          eigenvalues are a complex conjugate pair, RT1I > 0. */

/*  CS      (output) DOUBLE PRECISION */
/*  SN      (output) DOUBLE PRECISION */
/*          Parameters of the rotation matrix. */

/*  Further Details */
/*  =============== */

/*  Modified by V. Sima, Research Institute for Informatics, Bucharest, */
/*  Romania, to reduce the risk of cancellation errors, */
/*  when computing real eigenvalues, and to ensure, if possible, that */
/*  abs(RT1R) >= abs(RT2R). */

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

    eps = dlamch_("P");
    if (*c__ == 0.) {
	*cs = 1.;
	*sn = 0.;
	goto L10;

    } else if (*b == 0.) {

/*        Swap rows and columns */

	*cs = 0.;
	*sn = 1.;
	temp = *d__;
	*d__ = *a;
	*a = temp;
	*b = -(*c__);
	*c__ = 0.;
	goto L10;
    } else if (*a - *d__ == 0. && d_sign(&c_b4, b) != d_sign(&c_b4, c__)) {
	*cs = 1.;
	*sn = 0.;
	goto L10;
    } else {

	temp = *a - *d__;
	p = temp * .5;
/* Computing MAX */
	d__1 = abs(*b), d__2 = abs(*c__);
	bcmax = std::max(d__1,d__2);
/* Computing MIN */
	d__1 = abs(*b), d__2 = abs(*c__);
	bcmis = std::min(d__1,d__2) * d_sign(&c_b4, b) * d_sign(&c_b4, c__);
/* Computing MAX */
	d__1 = abs(p);
	scale = std::max(d__1,bcmax);
	z__ = p / scale * p + bcmax / scale * bcmis;

/*        If Z is of the order of the machine accuracy, postpone the */
/*        decision on the nature of eigenvalues */

	if (z__ >= eps * 4.) {

/*           Real eigenvalues. Compute A and D. */

	    d__1 = sqrt(scale) * sqrt(z__);
	    z__ = p + d_sign(&d__1, &p);
	    *a = *d__ + z__;
	    *d__ -= bcmax / z__ * bcmis;

/*           Compute B and the rotation matrix */

	    tau = dlapy2_(c__, &z__);
	    *cs = z__ / tau;
	    *sn = *c__ / tau;
	    *b -= *c__;
	    *c__ = 0.;
	} else {

/*           Complex eigenvalues, or real (almost) equal eigenvalues. */
/*           Make diagonal elements equal. */

	    sigma = *b + *c__;
	    tau = dlapy2_(&sigma, &temp);
	    *cs = sqrt((abs(sigma) / tau + 1.) * .5);
	    *sn = -(p / (tau * *cs)) * d_sign(&c_b4, &sigma);

/*           Compute [ AA  BB ] = [ A  B ] [ CS -SN ] */
/*                   [ CC  DD ]   [ C  D ] [ SN  CS ] */

	    aa = *a * *cs + *b * *sn;
	    bb = -(*a) * *sn + *b * *cs;
	    cc = *c__ * *cs + *d__ * *sn;
	    dd = -(*c__) * *sn + *d__ * *cs;

/*           Compute [ A  B ] = [ CS  SN ] [ AA  BB ] */
/*                   [ C  D ]   [-SN  CS ] [ CC  DD ] */

	    *a = aa * *cs + cc * *sn;
	    *b = bb * *cs + dd * *sn;
	    *c__ = -aa * *sn + cc * *cs;
	    *d__ = -bb * *sn + dd * *cs;

	    temp = (*a + *d__) * .5;
	    *a = temp;
	    *d__ = temp;

	    if (*c__ != 0.) {
		if (*b != 0.) {
		    if (d_sign(&c_b4, b) == d_sign(&c_b4, c__)) {

/*                    Real eigenvalues: reduce to upper triangular form */

			sab = sqrt((abs(*b)));
			sac = sqrt((abs(*c__)));
			d__1 = sab * sac;
			p = d_sign(&d__1, c__);
			tau = 1. / sqrt((d__1 = *b + *c__, abs(d__1)));
			*a = temp + p;
			*d__ = temp - p;
			*b -= *c__;
			*c__ = 0.;
			cs1 = sab * tau;
			sn1 = sac * tau;
			temp = *cs * cs1 - *sn * sn1;
			*sn = *cs * sn1 + *sn * cs1;
			*cs = temp;
		    }
		} else {
		    *b = -(*c__);
		    *c__ = 0.;
		    temp = *cs;
		    *cs = -(*sn);
		    *sn = temp;
		}
	    }
	}

    }

L10:

/*     Store eigenvalues in (RT1R,RT1I) and (RT2R,RT2I). */

    *rt1r = *a;
    *rt2r = *d__;
    if (*c__ == 0.) {
	*rt1i = 0.;
	*rt2i = 0.;
    } else {
	*rt1i = sqrt((abs(*b))) * sqrt((abs(*c__)));
	*rt2i = -(*rt1i);
    }
    return 0;

/*     End of DLANV2 */

} /* dlanv2_ */

/* Subroutine */ int dlapll_(integer *n, double *x, integer *incx,
	double *y, integer *incy, double *ssmin)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    double c__, a11, a12, a22, tau;
    double ssmax;

/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  Given two column vectors X and Y, let */

/*                       A = ( X Y ). */

/*  The subroutine first computes the QR factorization of A = Q*R, */
/*  and then computes the SVD of the 2-by-2 upper triangular matrix R. */
/*  The smaller singular value of R is returned in SSMIN, which is used */
/*  as the measurement of the linear dependency of the vectors X and Y. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The length of the vectors X and Y. */

/*  X       (input/output) DOUBLE PRECISION array, */
/*                         dimension (1+(N-1)*INCX) */
/*          On entry, X contains the N-vector X. */
/*          On exit, X is overwritten. */

/*  INCX    (input) INTEGER */
/*          The increment between successive elements of X. INCX > 0. */

/*  Y       (input/output) DOUBLE PRECISION array, */
/*                         dimension (1+(N-1)*INCY) */
/*          On entry, Y contains the N-vector Y. */
/*          On exit, Y is overwritten. */

/*  INCY    (input) INTEGER */
/*          The increment between successive elements of Y. INCY > 0. */

/*  SSMIN   (output) DOUBLE PRECISION */
/*          The smallest singular value of the N-by-2 matrix A = ( X Y ). */

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
    --y;
    --x;

    /* Function Body */
    if (*n <= 1) {
	*ssmin = 0.;
	return 0;
    }

/*     Compute the QR factorization of the N-by-2 matrix ( X Y ) */

    dlarfg_(n, &x[1], &x[*incx + 1], incx, &tau);
    a11 = x[1];
    x[1] = 1.;

    c__ = -tau * ddot_(n, &x[1], incx, &y[1], incy);
    daxpy_(n, &c__, &x[1], incx, &y[1], incy);

    i__1 = *n - 1;
    dlarfg_(&i__1, &y[*incy + 1], &y[(*incy << 1) + 1], incy, &tau);

    a12 = y[1];
    a22 = y[*incy + 1];

/*     Compute the SVD of 2-by-2 Upper triangular matrix. */

    dlas2_(&a11, &a12, &a22, ssmin, &ssmax);

    return 0;

/*     End of DLAPLL */

} /* dlapll_ */

/* Subroutine */ int dlapmt_(bool *forwrd, integer *m, integer *n,
	double *x, integer *ldx, integer *k)
{
    /* System generated locals */
    integer x_dim1, x_offset, i__1, i__2;

    /* Local variables */
    integer i__, j, ii, in;
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

/*  DLAPMT rearranges the columns of the M by N matrix X as specified */
/*  by the permutation K(1),K(2),...,K(N) of the integers 1,...,N. */
/*  If FORWRD = .TRUE.,  forward permutation: */

/*       X(*,K(J)) is moved X(*,J) for J = 1,2,...,N. */

/*  If FORWRD = .FALSE., backward permutation: */

/*       X(*,J) is moved to X(*,K(J)) for J = 1,2,...,N. */

/*  Arguments */
/*  ========= */

/*  FORWRD  (input) LOGICAL */
/*          = .TRUE., forward permutation */
/*          = .FALSE., backward permutation */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix X. M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix X. N >= 0. */

/*  X       (input/output) DOUBLE PRECISION array, dimension (LDX,N) */
/*          On entry, the M by N matrix X. */
/*          On exit, X contains the permuted matrix X. */

/*  LDX     (input) INTEGER */
/*          The leading dimension of the array X, LDX >= MAX(1,M). */

/*  K       (input/output) INTEGER array, dimension (N) */
/*          On entry, K contains the permutation vector. K is used as */
/*          internal workspace, but reset to its original value on */
/*          output. */

/*  ===================================================================== */

/*     .. Local Scalars .. */
/*     .. */
/*     .. Executable Statements .. */

    /* Parameter adjustments */
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    --k;

    /* Function Body */
    if (*n <= 1) {
	return 0;
    }

    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	k[i__] = -k[i__];
/* L10: */
    }

    if (*forwrd) {

/*        Forward permutation */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {

	    if (k[i__] > 0) {
		goto L40;
	    }

	    j = i__;
	    k[j] = -k[j];
	    in = k[j];

L20:
	    if (k[in] > 0) {
		goto L40;
	    }

	    i__2 = *m;
	    for (ii = 1; ii <= i__2; ++ii) {
		temp = x[ii + j * x_dim1];
		x[ii + j * x_dim1] = x[ii + in * x_dim1];
		x[ii + in * x_dim1] = temp;
/* L30: */
	    }

	    k[in] = -k[in];
	    j = in;
	    in = k[in];
	    goto L20;

L40:

/* L50: */
	    ;
	}

    } else {

/*        Backward permutation */

	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {

	    if (k[i__] > 0) {
		goto L80;
	    }

	    k[i__] = -k[i__];
	    j = k[i__];
L60:
	    if (j == i__) {
		goto L80;
	    }

	    i__2 = *m;
	    for (ii = 1; ii <= i__2; ++ii) {
		temp = x[ii + i__ * x_dim1];
		x[ii + i__ * x_dim1] = x[ii + j * x_dim1];
		x[ii + j * x_dim1] = temp;
/* L70: */
	    }

	    k[j] = -k[j];
	    j = k[j];
	    goto L60;

L80:

/* L90: */
	    ;
	}

    }

    return 0;

/*     End of DLAPMT */

} /* dlapmt_ */

/* Subroutine */ double dlapy2_(double *x, double *y)
{
    /* System generated locals */
    double ret_val, d__1;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    double w, z__, xabs, yabs;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAPY2 returns sqrt(x**2+y**2), taking care not to cause unnecessary */
/*  overflow. */

/*  Arguments */
/*  ========= */

/*  X       (input) DOUBLE PRECISION */
/*  Y       (input) DOUBLE PRECISION */
/*          X and Y specify the values x and y. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    xabs = abs(*x);
    yabs = abs(*y);
    w = std::max(xabs,yabs);
    z__ = std::min(xabs,yabs);
    if (z__ == 0.) {
	ret_val = w;
    } else {
/* Computing 2nd power */
	d__1 = z__ / w;
	ret_val = w * sqrt(d__1 * d__1 + 1.);
    }
    return ret_val;

/*     End of DLAPY2 */

} /* dlapy2_ */

/* Subroutine */ double dlapy3_(double *x, double *y, double *z__)
{
    /* System generated locals */
    double ret_val, d__1, d__2, d__3;

    /* Builtin functions
    double sqrt(double); */

    /* Local variables */
    double w, xabs, yabs, zabs;


/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAPY3 returns sqrt(x**2+y**2+z**2), taking care not to cause */
/*  unnecessary overflow. */

/*  Arguments */
/*  ========= */

/*  X       (input) DOUBLE PRECISION */
/*  Y       (input) DOUBLE PRECISION */
/*  Z       (input) DOUBLE PRECISION */
/*          X, Y and Z specify the values x, y and z. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */

    xabs = abs(*x);
    yabs = abs(*y);
    zabs = abs(*z__);
/* Computing MAX */
    d__1 = std::max(xabs,yabs);
    w = std::max(d__1,zabs);
    if (w == 0.) {
/*     W can be zero for max(0_integer,nan,0) */
/*     adding all three entries together will make sure */
/*     NaN will not disappear. */
	ret_val = xabs + yabs + zabs;
    } else {
/* Computing 2nd power */
	d__1 = xabs / w;
/* Computing 2nd power */
	d__2 = yabs / w;
/* Computing 2nd power */
	d__3 = zabs / w;
	ret_val = w * sqrt(d__1 * d__1 + d__2 * d__2 + d__3 * d__3);
    }
    return ret_val;

/*     End of DLAPY3 */

} /* dlapy3_ */

/* Subroutine */ int dlaqgb_(integer *m, integer *n, integer *kl, integer *ku,
	double *ab, integer *ldab, double *r__, double *c__,
	double *rowcnd, double *colcnd, double *amax, char *equed)
{
    /* System generated locals */
    integer ab_dim1, ab_offset, i__1, i__2, i__3, i__4, i__5, i__6;

    /* Local variables */
    integer i__, j;
    double cj, large, small;



/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAQGB equilibrates a general M by N band matrix A with KL */
/*  subdiagonals and KU superdiagonals using the row and scaling factors */
/*  in the vectors R and C. */

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
/*          On entry, the matrix A in band storage, in rows 1 to KL+KU+1. */
/*          The j-th column of A is stored in the j-th column of the */
/*          array AB as follows: */
/*          AB(ku+1+i-j,j) = A(i,j) for max(1,j-ku)<=i<=min(m,j+kl) */

/*          On exit, the equilibrated matrix, in the same storage format */
/*          as A.  See EQUED for the form of the equilibrated matrix. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDA >= KL+KU+1. */

/*  R       (input) DOUBLE PRECISION array, dimension (M) */
/*          The row scale factors for A. */

/*  C       (input) DOUBLE PRECISION array, dimension (N) */
/*          The column scale factors for A. */

/*  ROWCND  (input) DOUBLE PRECISION */
/*          Ratio of the smallest R(i) to the largest R(i). */

/*  COLCND  (input) DOUBLE PRECISION */
/*          Ratio of the smallest C(i) to the largest C(i). */

/*  AMAX    (input) DOUBLE PRECISION */
/*          Absolute value of largest matrix entry. */

/*  EQUED   (output) CHARACTER*1 */
/*          Specifies the form of equilibration that was done. */
/*          = 'N':  No equilibration */
/*          = 'R':  Row equilibration, i.e., A has been premultiplied by */
/*                  diag(R). */
/*          = 'C':  Column equilibration, i.e., A has been postmultiplied */
/*                  by diag(C). */
/*          = 'B':  Both row and column equilibration, i.e., A has been */
/*                  replaced by diag(R) * A * diag(C). */

/*  Internal Parameters */
/*  =================== */

/*  THRESH is a threshold value used to decide if row or column scaling */
/*  should be done based on the ratio of the row or column scaling */
/*  factors.  If ROWCND < THRESH, row scaling is done, and if */
/*  COLCND < THRESH, column scaling is done. */

/*  LARGE and SMALL are threshold values used to decide if row scaling */
/*  should be done based on the absolute size of the largest matrix */
/*  element.  If AMAX > LARGE or AMAX < SMALL, row scaling is done. */

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

/*     Quick return if possible */

    /* Parameter adjustments */
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    --r__;
    --c__;

    /* Function Body */
    if (*m <= 0 || *n <= 0) {
	*(unsigned char *)equed = 'N';
	return 0;
    }

/*     Initialize LARGE and SMALL. */

    small = dlamch_("Safe minimum") / dlamch_("Precision");
    large = 1. / small;

    if (*rowcnd >= .1 && *amax >= small && *amax <= large) {

/*        No row scaling */

	if (*colcnd >= .1) {

/*           No column scaling */

	    *(unsigned char *)equed = 'N';
	} else {

/*           Column scaling */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		cj = c__[j];
/* Computing MAX */
		i__2 = 1, i__3 = j - *ku;
/* Computing MIN */
		i__5 = *m, i__6 = j + *kl;
		i__4 = std::min(i__5,i__6);
		for (i__ = std::max(i__2,i__3); i__ <= i__4; ++i__) {
		    ab[*ku + 1 + i__ - j + j * ab_dim1] = cj * ab[*ku + 1 +
			    i__ - j + j * ab_dim1];
/* L10: */
		}
/* L20: */
	    }
	    *(unsigned char *)equed = 'C';
	}
    } else if (*colcnd >= .1) {

/*        Row scaling, no column scaling */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	    i__4 = 1, i__2 = j - *ku;
/* Computing MIN */
	    i__5 = *m, i__6 = j + *kl;
	    i__3 = std::min(i__5,i__6);
	    for (i__ = std::max(i__4,i__2); i__ <= i__3; ++i__) {
		ab[*ku + 1 + i__ - j + j * ab_dim1] = r__[i__] * ab[*ku + 1 +
			i__ - j + j * ab_dim1];
/* L30: */
	    }
/* L40: */
	}
	*(unsigned char *)equed = 'R';
    } else {

/*        Row and column scaling */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    cj = c__[j];
/* Computing MAX */
	    i__3 = 1, i__4 = j - *ku;
/* Computing MIN */
	    i__5 = *m, i__6 = j + *kl;
	    i__2 = std::min(i__5,i__6);
	    for (i__ = std::max(i__3,i__4); i__ <= i__2; ++i__) {
		ab[*ku + 1 + i__ - j + j * ab_dim1] = cj * r__[i__] * ab[*ku
			+ 1 + i__ - j + j * ab_dim1];
/* L50: */
	    }
/* L60: */
	}
	*(unsigned char *)equed = 'B';
    }

    return 0;

/*     End of DLAQGB */

} /* dlaqgb_ */

/* Subroutine */ int dlaqge_(integer *m, integer *n, double *a, integer *
	lda, double *r__, double *c__, double *rowcnd, double
	*colcnd, double *amax, char *equed)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    integer i__, j;
    double cj, large, small;



/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAQGE equilibrates a general M by N matrix A using the row and */
/*  column scaling factors in the vectors R and C. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M by N matrix A. */
/*          On exit, the equilibrated matrix.  See EQUED for the form of */
/*          the equilibrated matrix. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(M,1). */

/*  R       (input) DOUBLE PRECISION array, dimension (M) */
/*          The row scale factors for A. */

/*  C       (input) DOUBLE PRECISION array, dimension (N) */
/*          The column scale factors for A. */

/*  ROWCND  (input) DOUBLE PRECISION */
/*          Ratio of the smallest R(i) to the largest R(i). */

/*  COLCND  (input) DOUBLE PRECISION */
/*          Ratio of the smallest C(i) to the largest C(i). */

/*  AMAX    (input) DOUBLE PRECISION */
/*          Absolute value of largest matrix entry. */

/*  EQUED   (output) CHARACTER*1 */
/*          Specifies the form of equilibration that was done. */
/*          = 'N':  No equilibration */
/*          = 'R':  Row equilibration, i.e., A has been premultiplied by */
/*                  diag(R). */
/*          = 'C':  Column equilibration, i.e., A has been postmultiplied */
/*                  by diag(C). */
/*          = 'B':  Both row and column equilibration, i.e., A has been */
/*                  replaced by diag(R) * A * diag(C). */

/*  Internal Parameters */
/*  =================== */

/*  THRESH is a threshold value used to decide if row or column scaling */
/*  should be done based on the ratio of the row or column scaling */
/*  factors.  If ROWCND < THRESH, row scaling is done, and if */
/*  COLCND < THRESH, column scaling is done. */

/*  LARGE and SMALL are threshold values used to decide if row scaling */
/*  should be done based on the absolute size of the largest matrix */
/*  element.  If AMAX > LARGE or AMAX < SMALL, row scaling is done. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick return if possible */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --r__;
    --c__;

    /* Function Body */
    if (*m <= 0 || *n <= 0) {
	*(unsigned char *)equed = 'N';
	return 0;
    }

/*     Initialize LARGE and SMALL. */

    small = dlamch_("Safe minimum") / dlamch_("Precision");
    large = 1. / small;

    if (*rowcnd >= .1 && *amax >= small && *amax <= large) {

/*        No row scaling */

	if (*colcnd >= .1) {

/*           No column scaling */

	    *(unsigned char *)equed = 'N';
	} else {

/*           Column scaling */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		cj = c__[j];
		i__2 = *m;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    a[i__ + j * a_dim1] = cj * a[i__ + j * a_dim1];
/* L10: */
		}
/* L20: */
	    }
	    *(unsigned char *)equed = 'C';
	}
    } else if (*colcnd >= .1) {

/*        Row scaling, no column scaling */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] = r__[i__] * a[i__ + j * a_dim1];
/* L30: */
	    }
/* L40: */
	}
	*(unsigned char *)equed = 'R';
    } else {

/*        Row and column scaling */

	i__1 = *n;
	for (j = 1; j <= i__1; ++j) {
	    cj = c__[j];
	    i__2 = *m;
	    for (i__ = 1; i__ <= i__2; ++i__) {
		a[i__ + j * a_dim1] = cj * r__[i__] * a[i__ + j * a_dim1];
/* L50: */
	    }
/* L60: */
	}
	*(unsigned char *)equed = 'B';
    }

    return 0;

/*     End of DLAQGE */

} /* dlaqge_ */

/* Subroutine */ int dlaqp2_(integer *m, integer *n, integer *offset,
	double *a, integer *lda, integer *jpvt, double *tau,
	double *vn1, double *vn2, double *work)
{
	/* Table of constant values */
	static integer c__1 = 1;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    double d__1, d__2;

    /* Local variables */
    integer i__, j, mn;
    double aii;
    integer pvt;
    double temp;
    double temp2, tol3z;
    integer offpi, itemp;


/*  -- LAPACK auxiliary routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAQP2 computes a QR factorization with column pivoting of */
/*  the block A(OFFSET+1:M,1:N). */
/*  The block A(1:OFFSET,1:N) is accordingly pivoted, but not factorized. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A. M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A. N >= 0. */

/*  OFFSET  (input) INTEGER */
/*          The number of rows of the matrix A that must be pivoted */
/*          but no factorized. OFFSET >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, the upper triangle of block A(OFFSET+1:M,1:N) is */
/*          the triangular factor obtained; the elements in block */
/*          A(OFFSET+1:M,1:N) below the diagonal, together with the */
/*          array TAU, represent the orthogonal matrix Q as a product of */
/*          elementary reflectors. Block A(1:OFFSET,1:N) has been */
/*          accordingly pivoted, but no factorized. */

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

/*  VN1     (input/output) DOUBLE PRECISION array, dimension (N) */
/*          The vector with the partial column norms. */

/*  VN2     (input/output) DOUBLE PRECISION array, dimension (N) */
/*          The vector with the exact column norms. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N) */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*    G. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain */
/*    X. Sun, Computer Science Dept., Duke University, USA */

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

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --jpvt;
    --tau;
    --vn1;
    --vn2;
    --work;

    /* Function Body */
/* Computing MIN */
    i__1 = *m - *offset;
    mn = std::min(i__1,*n);
    tol3z = sqrt(dlamch_("Epsilon"));

/*     Compute factorization. */

    i__1 = mn;
    for (i__ = 1; i__ <= i__1; ++i__) {

	offpi = *offset + i__;

/*        Determine ith pivot column and swap if necessary. */

	i__2 = *n - i__ + 1;
	pvt = i__ - 1 + idamax_(&i__2, &vn1[i__], &c__1);

	if (pvt != i__) {
	    dswap_(m, &a[pvt * a_dim1 + 1], &c__1, &a[i__ * a_dim1 + 1], &
		    c__1);
	    itemp = jpvt[pvt];
	    jpvt[pvt] = jpvt[i__];
	    jpvt[i__] = itemp;
	    vn1[pvt] = vn1[i__];
	    vn2[pvt] = vn2[i__];
	}

/*        Generate elementary reflector H(i). */

	if (offpi < *m) {
	    i__2 = *m - offpi + 1;
	    dlarfp_(&i__2, &a[offpi + i__ * a_dim1], &a[offpi + 1 + i__ *
		    a_dim1], &c__1, &tau[i__]);
	} else {
	    dlarfp_(&c__1, &a[*m + i__ * a_dim1], &a[*m + i__ * a_dim1], &
		    c__1, &tau[i__]);
	}

	if (i__ <= *n) {

/*           Apply H(i)' to A(offset+i:m,i+1:n) from the left. */

	    aii = a[offpi + i__ * a_dim1];
	    a[offpi + i__ * a_dim1] = 1.;
	    i__2 = *m - offpi + 1;
	    i__3 = *n - i__;
	    dlarf_("Left", &i__2, &i__3, &a[offpi + i__ * a_dim1], &c__1, &
		    tau[i__], &a[offpi + (i__ + 1) * a_dim1], lda, &work[1]);
	    a[offpi + i__ * a_dim1] = aii;
	}

/*        Update partial column norms. */

	i__2 = *n;
	for (j = i__ + 1; j <= i__2; ++j) {
	    if (vn1[j] != 0.) {

/*              NOTE: The following 4 lines follow from the analysis in */
/*              Lapack Working Note 176. */

/* Computing 2nd power */
		d__2 = (d__1 = a[offpi + j * a_dim1], abs(d__1)) / vn1[j];
		temp = 1. - d__2 * d__2;
		temp = std::max(temp,0.);
/* Computing 2nd power */
		d__1 = vn1[j] / vn2[j];
		temp2 = temp * (d__1 * d__1);
		if (temp2 <= tol3z) {
		    if (offpi < *m) {
			i__3 = *m - offpi;
			vn1[j] = dnrm2_(&i__3, &a[offpi + 1 + j * a_dim1], &
				c__1);
			vn2[j] = vn1[j];
		    } else {
			vn1[j] = 0.;
			vn2[j] = 0.;
		    }
		} else {
		    vn1[j] *= sqrt(temp);
		}
	    }
/* L10: */
	}

/* L20: */
    }

    return 0;

/*     End of DLAQP2 */

} /* dlaqp2_ */

/* Subroutine */ int dlaqps_(integer *m, integer *n, integer *offset, integer
	*nb, integer *kb, double *a, integer *lda, integer *jpvt,
	double *tau, double *vn1, double *vn2, double *auxv,
	double *f, integer *ldf)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static double c_b8 = -1.;
	static double c_b9 = 1.;
	static double c_b16 = 0.;

    /* System generated locals */
    integer a_dim1, a_offset, f_dim1, f_offset, i__1, i__2;
    double d__1, d__2;

    /* Local variables */
    integer j, k, rk;
    double akk;
    integer pvt;
    double temp;
    double temp2, tol3z;
    integer itemp;
    integer lsticc, lastrk;


/*  -- LAPACK auxiliary routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAQPS computes a step of QR factorization with column pivoting */
/*  of a real M-by-N matrix A by using Blas-3.  It tries to factorize */
/*  NB columns from A starting from the row OFFSET+1, and updates all */
/*  of the matrix with Blas-3 xGEMM. */

/*  In some cases, due to catastrophic cancellations, it cannot */
/*  factorize NB columns.  Hence, the actual number of factorized */
/*  columns is returned in KB. */

/*  Block A(1:OFFSET,1:N) is accordingly pivoted, but not factorized. */

/*  Arguments */
/*  ========= */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A. M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A. N >= 0 */

/*  OFFSET  (input) INTEGER */
/*          The number of rows of A that have been factorized in */
/*          previous steps. */

/*  NB      (input) INTEGER */
/*          The number of columns to factorize. */

/*  KB      (output) INTEGER */
/*          The number of columns actually factorized. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, block A(OFFSET+1:M,1:KB) is the triangular */
/*          factor obtained and block A(1:OFFSET,1:N) has been */
/*          accordingly pivoted, but no factorized. */
/*          The rest of the matrix, block A(OFFSET+1:M,KB+1:N) has */
/*          been updated. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,M). */

/*  JPVT    (input/output) INTEGER array, dimension (N) */
/*          JPVT(I) = K <==> Column K of the full matrix A has been */
/*          permuted into position I in AP. */

/*  TAU     (output) DOUBLE PRECISION array, dimension (KB) */
/*          The scalar factors of the elementary reflectors. */

/*  VN1     (input/output) DOUBLE PRECISION array, dimension (N) */
/*          The vector with the partial column norms. */

/*  VN2     (input/output) DOUBLE PRECISION array, dimension (N) */
/*          The vector with the exact column norms. */

/*  AUXV    (input/output) DOUBLE PRECISION array, dimension (NB) */
/*          Auxiliar vector. */

/*  F       (input/output) DOUBLE PRECISION array, dimension (LDF,NB) */
/*          Matrix F' = L*Y'*A. */

/*  LDF     (input) INTEGER */
/*          The leading dimension of the array F. LDF >= max(1,N). */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*    G. Quintana-Orti, Depto. de Informatica, Universidad Jaime I, Spain */
/*    X. Sun, Computer Science Dept., Duke University, USA */

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

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --jpvt;
    --tau;
    --vn1;
    --vn2;
    --auxv;
    f_dim1 = *ldf;
    f_offset = 1 + f_dim1;
    f -= f_offset;

    /* Function Body */
/* Computing MIN */
    i__1 = *m, i__2 = *n + *offset;
    lastrk = std::min(i__1,i__2);
    lsticc = 0;
    k = 0;
    tol3z = sqrt(dlamch_("Epsilon"));

/*     Beginning of while loop. */

L10:
    if (k < *nb && lsticc == 0) {
	++k;
	rk = *offset + k;

/*        Determine ith pivot column and swap if necessary */

	i__1 = *n - k + 1;
	pvt = k - 1 + idamax_(&i__1, &vn1[k], &c__1);
	if (pvt != k) {
	    dswap_(m, &a[pvt * a_dim1 + 1], &c__1, &a[k * a_dim1 + 1], &c__1);
	    i__1 = k - 1;
	    dswap_(&i__1, &f[pvt + f_dim1], ldf, &f[k + f_dim1], ldf);
	    itemp = jpvt[pvt];
	    jpvt[pvt] = jpvt[k];
	    jpvt[k] = itemp;
	    vn1[pvt] = vn1[k];
	    vn2[pvt] = vn2[k];
	}

/*        Apply previous Householder reflectors to column K: */
/*        A(RK:M,K) := A(RK:M,K) - A(RK:M,1:K-1)*F(K,1:K-1)'. */

	if (k > 1) {
	    i__1 = *m - rk + 1;
	    i__2 = k - 1;
	    dgemv_("No transpose", &i__1, &i__2, &c_b8, &a[rk + a_dim1], lda,
		    &f[k + f_dim1], ldf, &c_b9, &a[rk + k * a_dim1], &c__1);
	}

/*        Generate elementary reflector H(k). */

	if (rk < *m) {
	    i__1 = *m - rk + 1;
	    dlarfp_(&i__1, &a[rk + k * a_dim1], &a[rk + 1 + k * a_dim1], &
		    c__1, &tau[k]);
	} else {
	    dlarfp_(&c__1, &a[rk + k * a_dim1], &a[rk + k * a_dim1], &c__1, &
		    tau[k]);
	}

	akk = a[rk + k * a_dim1];
	a[rk + k * a_dim1] = 1.;

/*        Compute Kth column of F: */

/*        Compute  F(K+1:N,K) := tau(K)*A(RK:M,K+1:N)'*A(RK:M,K). */

	if (k < *n) {
	    i__1 = *m - rk + 1;
	    i__2 = *n - k;
	    dgemv_("Transpose", &i__1, &i__2, &tau[k], &a[rk + (k + 1) *
		    a_dim1], lda, &a[rk + k * a_dim1], &c__1, &c_b16, &f[k +
		    1 + k * f_dim1], &c__1);
	}

/*        Padding F(1:K,K) with zeros. */

	i__1 = k;
	for (j = 1; j <= i__1; ++j) {
	    f[j + k * f_dim1] = 0.;
/* L20: */
	}

/*        Incremental updating of F: */
/*        F(1:N,K) := F(1:N,K) - tau(K)*F(1:N,1:K-1)*A(RK:M,1:K-1)' */
/*                    *A(RK:M,K). */

	if (k > 1) {
	    i__1 = *m - rk + 1;
	    i__2 = k - 1;
	    d__1 = -tau[k];
	    dgemv_("Transpose", &i__1, &i__2, &d__1, &a[rk + a_dim1], lda, &a[
		    rk + k * a_dim1], &c__1, &c_b16, &auxv[1], &c__1);

	    i__1 = k - 1;
	    dgemv_("No transpose", n, &i__1, &c_b9, &f[f_dim1 + 1], ldf, &
		    auxv[1], &c__1, &c_b9, &f[k * f_dim1 + 1], &c__1);
	}

/*        Update the current row of A: */
/*        A(RK,K+1:N) := A(RK,K+1:N) - A(RK,1:K)*F(K+1:N,1:K)'. */

	if (k < *n) {
	    i__1 = *n - k;
	    dgemv_("No transpose", &i__1, &k, &c_b8, &f[k + 1 + f_dim1], ldf,
		    &a[rk + a_dim1], lda, &c_b9, &a[rk + (k + 1) * a_dim1],
		    lda);
	}

/*        Update partial column norms. */

	if (rk < lastrk) {
	    i__1 = *n;
	    for (j = k + 1; j <= i__1; ++j) {
		if (vn1[j] != 0.) {

/*                 NOTE: The following 4 lines follow from the analysis in */
/*                 Lapack Working Note 176. */

		    temp = (d__1 = a[rk + j * a_dim1], abs(d__1)) / vn1[j];
/* Computing MAX */
		    d__1 = 0., d__2 = (temp + 1.) * (1. - temp);
		    temp = std::max(d__1,d__2);
/* Computing 2nd power */
		    d__1 = vn1[j] / vn2[j];
		    temp2 = temp * (d__1 * d__1);
		    if (temp2 <= tol3z) {
			vn2[j] = (double) lsticc;
			lsticc = j;
		    } else {
			vn1[j] *= sqrt(temp);
		    }
		}
/* L30: */
	    }
	}

	a[rk + k * a_dim1] = akk;

/*        End of while loop. */

	goto L10;
    }
    *kb = k;
    rk = *offset + *kb;

/*     Apply the block reflector to the rest of the matrix: */
/*     A(OFFSET+KB+1:M,KB+1:N) := A(OFFSET+KB+1:M,KB+1:N) - */
/*                         A(OFFSET+KB+1:M,1:KB)*F(KB+1:N,1:KB)'. */

/* Computing MIN */
    i__1 = *n, i__2 = *m - *offset;
    if (*kb < std::min(i__1,i__2)) {
	i__1 = *m - rk;
	i__2 = *n - *kb;
	dgemm_("No transpose", "Transpose", &i__1, &i__2, kb, &c_b8, &a[rk +
		1 + a_dim1], lda, &f[*kb + 1 + f_dim1], ldf, &c_b9, &a[rk + 1
		+ (*kb + 1) * a_dim1], lda);
    }

/*     Recomputation of difficult columns. */

L40:
    if (lsticc > 0) {
	itemp = i_dnnt(&vn2[lsticc]);
	i__1 = *m - rk;
	vn1[lsticc] = dnrm2_(&i__1, &a[rk + 1 + lsticc * a_dim1], &c__1);

/*        NOTE: The computation of VN1( LSTICC ) relies on the fact that */
/*        SNRM2 does not fail on vectors with norm below the value of */
/*        SQRT(DLAMCH('S')) */

	vn2[lsticc] = vn1[lsticc];
	lsticc = itemp;
	goto L40;
    }

    return 0;

/*     End of DLAQPS */

} /* dlaqps_ */

/* Subroutine */ int dlaqr0_(bool *wantt, bool *wantz, integer *n,
	integer *ilo, integer *ihi, double *h__, integer *ldh, double
	*wr, double *wi, integer *iloz, integer *ihiz, double *z__,
	integer *ldz, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__13 = 13;
	static integer c__15 = 15;
	static integer c_n1 = -1;
	static integer c__12 = 12;
	static integer c__14 = 14;
	static integer c__16 = 16;
	static bool c_false = false;
	static integer c__1 = 1;
	static integer c__3 = 3;

    /* System generated locals */
    integer h_dim1, h_offset, z_dim1, z_offset, i__1, i__2, i__3, i__4, i__5;
    double d__1, d__2, d__3, d__4;

    /* Local variables */
    integer i__, k;
    double aa, bb, cc, dd;
    integer ld;
    double cs;
    integer nh, it, ks, kt;
    double sn;
    integer ku, kv, ls, ns;
    double ss;
    integer nw, inf, kdu, nho, nve, kwh, nsr, nwr, kwv, ndec, ndfl, kbot, nmin;
    double swap;
    integer ktop;
    double zdum[1]	/* was [1][1] */;
    integer kacc22, itmax, nsmax, nwmax, kwtop;
    integer nibble, nwupbd;
    char jbcmpz[3];
    bool sorted;
    integer lwkopt;


/*  -- LAPACK auxiliary routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*     Purpose */
/*     ======= */

/*     DLAQR0 computes the eigenvalues of a Hessenberg matrix H */
/*     and, optionally, the matrices T and Z from the Schur decomposition */
/*     H = Z T Z**T, where T is an upper quasi-triangular matrix (the */
/*     Schur form), and Z is the orthogonal matrix of Schur vectors. */

/*     Optionally Z may be postmultiplied into an input orthogonal */
/*     matrix Q so that this routine can give the Schur factorization */
/*     of a matrix A which has been reduced to the Hessenberg form H */
/*     by the orthogonal matrix Q:  A = Q*H*Q**T = (QZ)*T*(QZ)**T. */

/*     Arguments */
/*     ========= */

/*     WANTT   (input) LOGICAL */
/*          = .TRUE. : the full Schur form T is required; */
/*          = .FALSE.: only eigenvalues are required. */

/*     WANTZ   (input) LOGICAL */
/*          = .TRUE. : the matrix of Schur vectors Z is required; */
/*          = .FALSE.: Schur vectors are not required. */

/*     N     (input) INTEGER */
/*           The order of the matrix H.  N .GE. 0. */

/*     ILO   (input) INTEGER */
/*     IHI   (input) INTEGER */
/*           It is assumed that H is already upper triangular in rows */
/*           and columns 1:ILO-1 and IHI+1:N and, if ILO.GT.1, */
/*           H(ILO,ILO-1) is zero. ILO and IHI are normally set by a */
/*           previous call to DGEBAL, and then passed to DGEHRD when the */
/*           matrix output by DGEBAL is reduced to Hessenberg form. */
/*           Otherwise, ILO and IHI should be set to 1 and N, */
/*           respectively.  If N.GT.0, then 1.LE.ILO.LE.IHI.LE.N. */
/*           If N = 0, then ILO = 1 and IHI = 0. */

/*     H     (input/output) DOUBLE PRECISION array, dimension (LDH,N) */
/*           On entry, the upper Hessenberg matrix H. */
/*           On exit, if INFO = 0 and WANTT is .TRUE., then H contains */
/*           the upper quasi-triangular matrix T from the Schur */
/*           decomposition (the Schur form); 2-by-2 diagonal blocks */
/*           (corresponding to complex conjugate pairs of eigenvalues) */
/*           are returned in standard form, with H(i,i) = H(i+1,i+1) */
/*           and H(i+1,i)*H(i,i+1).LT.0. If INFO = 0 and WANTT is */
/*           .FALSE., then the contents of H are unspecified on exit. */
/*           (The output value of H when INFO.GT.0 is given under the */
/*           description of INFO below.) */

/*           This subroutine may explicitly set H(i,j) = 0 for i.GT.j and */
/*           j = 1, 2, ... ILO-1 or j = IHI+1, IHI+2, ... N. */

/*     LDH   (input) INTEGER */
/*           The leading dimension of the array H. LDH .GE. max(1,N). */

/*     WR    (output) DOUBLE PRECISION array, dimension (IHI) */
/*     WI    (output) DOUBLE PRECISION array, dimension (IHI) */
/*           The real and imaginary parts, respectively, of the computed */
/*           eigenvalues of H(ILO:IHI,ILO:IHI) are stored in WR(ILO:IHI) */
/*           and WI(ILO:IHI). If two eigenvalues are computed as a */
/*           complex conjugate pair, they are stored in consecutive */
/*           elements of WR and WI, say the i-th and (i+1)th, with */
/*           WI(i) .GT. 0 and WI(i+1) .LT. 0. If WANTT is .TRUE., then */
/*           the eigenvalues are stored in the same order as on the */
/*           diagonal of the Schur form returned in H, with */
/*           WR(i) = H(i,i) and, if H(i:i+1,i:i+1) is a 2-by-2 diagonal */
/*           block, WI(i) = sqrt(-H(i+1,i)*H(i,i+1)) and */
/*           WI(i+1) = -WI(i). */

/*     ILOZ     (input) INTEGER */
/*     IHIZ     (input) INTEGER */
/*           Specify the rows of Z to which transformations must be */
/*           applied if WANTZ is .TRUE.. */
/*           1 .LE. ILOZ .LE. ILO; IHI .LE. IHIZ .LE. N. */

/*     Z     (input/output) DOUBLE PRECISION array, dimension (LDZ,IHI) */
/*           If WANTZ is .FALSE., then Z is not referenced. */
/*           If WANTZ is .TRUE., then Z(ILO:IHI,ILOZ:IHIZ) is */
/*           replaced by Z(ILO:IHI,ILOZ:IHIZ)*U where U is the */
/*           orthogonal Schur factor of H(ILO:IHI,ILO:IHI). */
/*           (The output value of Z when INFO.GT.0 is given under */
/*           the description of INFO below.) */

/*     LDZ   (input) INTEGER */
/*           The leading dimension of the array Z.  if WANTZ is .TRUE. */
/*           then LDZ.GE.MAX(1,IHIZ).  Otherwize, LDZ.GE.1. */

/*     WORK  (workspace/output) DOUBLE PRECISION array, dimension LWORK */
/*           On exit, if LWORK = -1, WORK(1) returns an estimate of */
/*           the optimal value for LWORK. */

/*     LWORK (input) INTEGER */
/*           The dimension of the array WORK.  LWORK .GE. max(1,N) */
/*           is sufficient, but LWORK typically as large as 6*N may */
/*           be required for optimal performance.  A workspace query */
/*           to determine the optimal workspace size is recommended. */

/*           If LWORK = -1, then DLAQR0 does a workspace query. */
/*           In this case, DLAQR0 checks the input parameters and */
/*           estimates the optimal workspace size for the given */
/*           values of N, ILO and IHI.  The estimate is returned */
/*           in WORK(1).  No error message related to LWORK is */
/*           issued by XERBLA.  Neither H nor Z are accessed. */


/*     INFO  (output) INTEGER */
/*             =  0:  successful exit */
/*           .GT. 0:  if INFO = i, DLAQR0 failed to compute all of */
/*                the eigenvalues.  Elements 1:ilo-1 and i+1:n of WR */
/*                and WI contain those eigenvalues which have been */
/*                successfully computed.  (Failures are rare.) */

/*                If INFO .GT. 0 and WANT is .FALSE., then on exit, */
/*                the remaining unconverged eigenvalues are the eigen- */
/*                values of the upper Hessenberg matrix rows and */
/*                columns ILO through INFO of the final, output */
/*                value of H. */

/*                If INFO .GT. 0 and WANTT is .TRUE., then on exit */

/*           (*)  (initial value of H)*U  = U*(final value of H) */

/*                where U is an orthogonal matrix.  The final */
/*                value of H is upper Hessenberg and quasi-triangular */
/*                in rows and columns INFO+1 through IHI. */

/*                If INFO .GT. 0 and WANTZ is .TRUE., then on exit */

/*                  (final value of Z(ILO:IHI,ILOZ:IHIZ) */
/*                   =  (initial value of Z(ILO:IHI,ILOZ:IHIZ)*U */

/*                where U is the orthogonal matrix in (*) (regard- */
/*                less of the value of WANTT.) */

/*                If INFO .GT. 0 and WANTZ is .FALSE., then Z is not */
/*                accessed. */

/*     ================================================================ */
/*     Based on contributions by */
/*        Karen Braman and Ralph Byers, Department of Mathematics, */
/*        University of Kansas, USA */

/*     ================================================================ */
/*     References: */
/*       K. Braman, R. Byers and R. Mathias, The Multi-Shift QR */
/*       Algorithm Part I: Maintaining Well Focused Shifts, and Level 3 */
/*       Performance, SIAM Journal of Matrix Analysis, volume 23, pages */
/*       929--947, 2002. */

/*       K. Braman, R. Byers and R. Mathias, The Multi-Shift QR */
/*       Algorithm Part II: Aggressive Early Deflation, SIAM Journal */
/*       of Matrix Analysis, volume 23, pages 948--973, 2002. */

/*     ================================================================ */
/*     .. Parameters .. */

/*     ==== Matrices of order NTINY or smaller must be processed by */
/*     .    DLAHQR because of insufficient subdiagonal scratch space. */
/*     .    (This is a hard limit.) ==== */

/*     ==== Exceptional deflation windows:  try to cure rare */
/*     .    slow convergence by varying the size of the */
/*     .    deflation window after KEXNW iterations. ==== */

/*     ==== Exceptional shifts: try to cure rare slow convergence */
/*     .    with ad-hoc exceptional shifts every KEXSH iterations. */
/*     .    ==== */

/*     ==== The constants WILK1 and WILK2 are used to form the */
/*     .    exceptional shifts. ==== */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */
    /* Parameter adjustments */
    h_dim1 = *ldh;
    h_offset = 1 + h_dim1;
    h__ -= h_offset;
    --wr;
    --wi;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;

    /* Function Body */
    *info = 0;

/*     ==== Quick return for N = 0: nothing to do. ==== */

    if (*n == 0) {
	work[1] = 1.;
	return 0;
    }

    if (*n <= 11) {

/*        ==== Tiny matrices must use DLAHQR. ==== */

	lwkopt = 1;
	if (*lwork != -1) {
	    dlahqr_(wantt, wantz, n, ilo, ihi, &h__[h_offset], ldh, &wr[1], &
		    wi[1], iloz, ihiz, &z__[z_offset], ldz, info);
	}
    } else {

/*        ==== Use small bulge multi-shift QR with aggressive early */
/*        .    deflation on larger-than-tiny matrices. ==== */

/*        ==== Hope for the best. ==== */

	*info = 0;

/*        ==== Set up job flags for ILAENV. ==== */

	if (*wantt) {
	    * (unsigned char *) & jbcmpz [0] = 'S';
	} else {
	    * (unsigned char *) & jbcmpz [0] = 'E';
	}
	if (*wantz) {
	    * (unsigned char *) & jbcmpz [1] = 'V';
	} else {
	    * (unsigned char *) & jbcmpz [1] = 'N';
	}
	jbcmpz [2] = '\0';
/*        ==== NWR = recommended deflation window size.  At this */
/*        .    point,  N .GT. NTINY = 11, so there is enough */
/*        .    subdiagonal workspace for NWR.GE.2 as required. */
/*        .    (In fact, there is enough subdiagonal space for */
/*        .    NWR.GE.3.) ==== */

	nwr = ilaenv_(&c__13, "DLAQR0", jbcmpz, n, ilo, ihi, lwork);
	nwr = std::max(2_integer,nwr);
/* Computing MIN */
	i__1 = *ihi - *ilo + 1, i__2 = (*n - 1) / 3, i__1 = std::min(i__1,i__2);
	nwr = std::min(i__1,nwr);

/*        ==== NSR = recommended number of simultaneous shifts. */
/*        .    At this point N .GT. NTINY = 11, so there is at */
/*        .    enough subdiagonal workspace for NSR to be even */
/*        .    and greater than or equal to two as required. ==== */

	nsr = ilaenv_(&c__15, "DLAQR0", jbcmpz, n, ilo, ihi, lwork);
/* Computing MIN */
	i__1 = nsr, i__2 = (*n + 6) / 9, i__1 = std::min(i__1,i__2), i__2 = *ihi - *ilo;
	nsr = std::min(i__1,i__2);
/* Computing MAX */
	i__1 = 2, i__2 = nsr - nsr % 2;
	nsr = std::max(i__1,i__2);

/*        ==== Estimate optimal workspace ==== */

/*        ==== Workspace query call to DLAQR3 ==== */

	i__1 = nwr + 1;
	dlaqr3_(wantt, wantz, n, ilo, ihi, &i__1, &h__[h_offset], ldh, iloz,
		ihiz, &z__[z_offset], ldz, &ls, &ld, &wr[1], &wi[1], &h__[
		h_offset], ldh, n, &h__[h_offset], ldh, n, &h__[h_offset],
		ldh, &work[1], &c_n1);

/*        ==== Optimal workspace = MAX(DLAQR5, DLAQR3) ==== */

/* Computing MAX */
	i__1 = nsr * 3 / 2, i__2 = (integer) work[1];
	lwkopt = std::max(i__1,i__2);

/*        ==== Quick return in case of workspace query. ==== */

	if (*lwork == -1) {
	    work[1] = (double) lwkopt;
	    return 0;
	}

/*        ==== DLAHQR/DLAQR0 crossover point ==== */

	nmin = ilaenv_(&c__12, "DLAQR0", jbcmpz, n, ilo, ihi, lwork);
	nmin = std::max(11_integer,nmin);

/*        ==== Nibble crossover point ==== */

	nibble = ilaenv_(&c__14, "DLAQR0", jbcmpz, n, ilo, ihi, lwork);
	nibble = std::max(0_integer,nibble);

/*        ==== Accumulate reflections during ttswp?  Use block */
/*        .    2-by-2 structure during matrix-matrix multiply? ==== */

	kacc22 = ilaenv_(&c__16, "DLAQR0", jbcmpz, n, ilo, ihi, lwork);
	kacc22 = std::max(0_integer,kacc22);
	kacc22 = std::min(2_integer,kacc22);

/*        ==== NWMAX = the largest possible deflation window for */
/*        .    which there is sufficient workspace. ==== */

/* Computing MIN */
	i__1 = (*n - 1) / 3, i__2 = *lwork / 2;
	nwmax = std::min(i__1,i__2);
	nw = nwmax;

/*        ==== NSMAX = the Largest number of simultaneous shifts */
/*        .    for which there is sufficient workspace. ==== */

/* Computing MIN */
	i__1 = (*n + 6) / 9, i__2 = (*lwork << 1) / 3;
	nsmax = std::min(i__1,i__2);
	nsmax -= nsmax % 2;

/*        ==== NDFL: an iteration count restarted at deflation. ==== */

	ndfl = 1;

/*        ==== ITMAX = iteration limit ==== */

/* Computing MAX */
	i__1 = 10, i__2 = *ihi - *ilo + 1;
	itmax = std::max(i__1,i__2) * 30;

/*        ==== Last row and column in the active block ==== */

	kbot = *ihi;

/*        ==== Main Loop ==== */

	i__1 = itmax;
	for (it = 1; it <= i__1; ++it) {

/*           ==== Done when KBOT falls below ILO ==== */

	    if (kbot < *ilo) {
		goto L90;
	    }

/*           ==== Locate active block ==== */

	    i__2 = *ilo + 1;
	    for (k = kbot; k >= i__2; --k) {
		if (h__[k + (k - 1) * h_dim1] == 0.) {
		    goto L20;
		}
/* L10: */
	    }
	    k = *ilo;
L20:
	    ktop = k;

/*           ==== Select deflation window size: */
/*           .    Typical Case: */
/*           .      If possible and advisable, nibble the entire */
/*           .      active block.  If not, use size MIN(NWR,NWMAX) */
/*           .      or MIN(NWR+1,NWMAX) depending upon which has */
/*           .      the smaller corresponding subdiagonal entry */
/*           .      (a heuristic). */
/*           . */
/*           .    Exceptional Case: */
/*           .      If there have been no deflations in KEXNW or */
/*           .      more iterations, then vary the deflation window */
/*           .      size.   At first, because, larger windows are, */
/*           .      in general, more powerful than smaller ones, */
/*           .      rapidly increase the window to the maximum possible. */
/*           .      Then, gradually reduce the window size. ==== */

		nh = kbot - ktop + 1;
		nwupbd = std::min(nh,nwmax);
	    if (ndfl < 5) {
		nw = std::min(nwupbd,nwr);
		} else {
/* Computing MIN */
		i__2 =nwupbd, i__3 = nw << 1;
		nw = std::min(i__2,i__3);
		}
		if (nw < nwmax) {
		if (nw >= nh - 1) {
			nw = nh;
		} else {
			kwtop = kbot - nw + 1;
			if ((d__1 = h__[kwtop + (kwtop - 1) * h_dim1], abs(d__1))
				> (d__2 = h__[kwtop - 1 + (kwtop - 2) * h_dim1],
				abs(d__2))) {
			++nw;
				}
			}
		    }
		    if (ndfl < 5) {
			ndec = -1;
			} else if (ndec >= 0 || nw >= nwupbd) {
			++ndec;
			if (nw - ndec < 2) {
		    ndec = 0;
			}
			nw -= ndec;
			}

/*           ==== Aggressive early deflation: */
/*           .    split workspace under the subdiagonal into */
/*           .      - an nw-by-nw work array V in the lower */
/*           .        left-hand-corner, */
/*           .      - an NW-by-at-least-NW-but-more-is-better */
/*           .        (NW-by-NHO) horizontal work array along */
/*           .        the bottom edge, */
/*           .      - an at-least-NW-but-more-is-better (NHV-by-NW) */
/*           .        vertical work array along the left-hand-edge. */
/*           .        ==== */

	    kv = *n - nw + 1;
	    kt = nw + 1;
	    nho = *n - nw - 1 - kt + 1;
	    kwv = nw + 2;
	    nve = *n - nw - kwv + 1;

/*           ==== Aggressive early deflation ==== */

	    dlaqr3_(wantt, wantz, n, &ktop, &kbot, &nw, &h__[h_offset], ldh,
		    iloz, ihiz, &z__[z_offset], ldz, &ls, &ld, &wr[1], &wi[1],
		     &h__[kv + h_dim1], ldh, &nho, &h__[kv + kt * h_dim1],
		    ldh, &nve, &h__[kwv + h_dim1], ldh, &work[1], lwork);

/*           ==== Adjust KBOT accounting for new deflations. ==== */

	    kbot -= ld;

/*           ==== KS points to the shifts. ==== */

	    ks = kbot - ls + 1;

/*           ==== Skip an expensive QR sweep if there is a (partly */
/*           .    heuristic) reason to expect that many eigenvalues */
/*           .    will deflate without it.  Here, the QR sweep is */
/*           .    skipped if many eigenvalues have just been deflated */
/*           .    or if the remaining active block is small. */

	    if (ld == 0 || ld * 100 <= nw * nibble && kbot - ktop + 1 > std::min(
		    nmin,nwmax)) {

/*              ==== NS = nominal number of simultaneous shifts. */
/*              .    This may be lowered (slightly) if DLAQR3 */
/*              .    did not provide that many shifts. ==== */

/* Computing MIN */
/* Computing MAX */
		i__4 = 2, i__5 = kbot - ktop;
		i__2 = std::min(nsmax,nsr), i__3 = std::max(i__4,i__5);
		ns = std::min(i__2,i__3);
		ns -= ns % 2;

/*              ==== If there have been no deflations */
/*              .    in a multiple of KEXSH iterations, */
/*              .    then try exceptional shifts. */
/*              .    Otherwise use shifts provided by */
/*              .    DLAQR3 above or from the eigenvalues */
/*              .    of a trailing principal submatrix. ==== */

		if (ndfl % 6 == 0) {
		    ks = kbot - ns + 1;
/* Computing MAX */
		    i__3 = ks + 1, i__4 = ktop + 2;
		    i__2 = std::max(i__3,i__4);
		    for (i__ = kbot; i__ >= i__2; i__ += -2) {
			ss = (d__1 = h__[i__ + (i__ - 1) * h_dim1], abs(d__1))
				 + (d__2 = h__[i__ - 1 + (i__ - 2) * h_dim1],
				abs(d__2));
			aa = ss * .75 + h__[i__ + i__ * h_dim1];
			bb = ss;
			cc = ss * -.4375;
			dd = aa;
			dlanv2_(&aa, &bb, &cc, &dd, &wr[i__ - 1], &wi[i__ - 1]
, &wr[i__], &wi[i__], &cs, &sn);
/* L30: */
		    }
		    if (ks == ktop) {
			wr[ks + 1] = h__[ks + 1 + (ks + 1) * h_dim1];
			wi[ks + 1] = 0.;
			wr[ks] = wr[ks + 1];
			wi[ks] = wi[ks + 1];
		    }
		} else {

/*                 ==== Got NS/2 or fewer shifts? Use DLAQR4 or */
/*                 .    DLAHQR on a trailing principal submatrix to */
/*                 .    get more. (Since NS.LE.NSMAX.LE.(N+6)/9, */
/*                 .    there is enough space below the subdiagonal */
/*                 .    to fit an NS-by-NS scratch array.) ==== */

		    if (kbot - ks + 1 <= ns / 2) {
			ks = kbot - ns + 1;
			kt = *n - ns + 1;
			dlacpy_("A", &ns, &ns, &h__[ks + ks * h_dim1], ldh, &
				h__[kt + h_dim1], ldh);
			if (ns > nmin) {
			    dlaqr4_(&c_false, &c_false, &ns, &c__1, &ns, &h__[
				    kt + h_dim1], ldh, &wr[ks], &wi[ks], &
				    c__1, &c__1, zdum, &c__1, &work[1], lwork,
				     &inf);
			} else {
			    dlahqr_(&c_false, &c_false, &ns, &c__1, &ns, &h__[
				    kt + h_dim1], ldh, &wr[ks], &wi[ks], &
				    c__1, &c__1, zdum, &c__1, &inf);
			}
			ks += inf;

/*                    ==== In case of a rare QR failure use */
/*                    .    eigenvalues of the trailing 2-by-2 */
/*                    .    principal submatrix.  ==== */

			if (ks >= kbot) {
			    aa = h__[kbot - 1 + (kbot - 1) * h_dim1];
			    cc = h__[kbot + (kbot - 1) * h_dim1];
			    bb = h__[kbot - 1 + kbot * h_dim1];
			    dd = h__[kbot + kbot * h_dim1];
			    dlanv2_(&aa, &bb, &cc, &dd, &wr[kbot - 1], &wi[
				    kbot - 1], &wr[kbot], &wi[kbot], &cs, &sn)
				    ;
			    ks = kbot - 1;
			}
		    }

		    if (kbot - ks + 1 > ns) {

/*                    ==== Sort the shifts (Helps a little) */
/*                    .    Bubble sort keeps complex conjugate */
/*                    .    pairs together. ==== */

			sorted = false;
			i__2 = ks + 1;
			for (k = kbot; k >= i__2; --k) {
			    if (sorted) {
				goto L60;
			    }
			    sorted = true;
			    i__3 = k - 1;
			    for (i__ = ks; i__ <= i__3; ++i__) {
				if ((d__1 = wr[i__], abs(d__1)) + (d__2 = wi[
					i__], abs(d__2)) < (d__3 = wr[i__ + 1]
					, abs(d__3)) + (d__4 = wi[i__ + 1],
					abs(d__4))) {
				    sorted = false;

				    swap = wr[i__];
				    wr[i__] = wr[i__ + 1];
				    wr[i__ + 1] = swap;

				    swap = wi[i__];
				    wi[i__] = wi[i__ + 1];
				    wi[i__ + 1] = swap;
				}
/* L40: */
			    }
/* L50: */
			}
L60:
			;
		    }

/*                 ==== Shuffle shifts into pairs of real shifts */
/*                 .    and pairs of complex conjugate shifts */
/*                 .    assuming complex conjugate shifts are */
/*                 .    already adjacent to one another. (Yes, */
/*                 .    they are.)  ==== */

		    i__2 = ks + 2;
		    for (i__ = kbot; i__ >= i__2; i__ += -2) {
			if (wi[i__] != -wi[i__ - 1]) {

			    swap = wr[i__];
			    wr[i__] = wr[i__ - 1];
			    wr[i__ - 1] = wr[i__ - 2];
			    wr[i__ - 2] = swap;

			    swap = wi[i__];
			    wi[i__] = wi[i__ - 1];
			    wi[i__ - 1] = wi[i__ - 2];
			    wi[i__ - 2] = swap;
			}
/* L70: */
		    }
		}

/*              ==== If there are only two shifts and both are */
/*              .    real, then use only one.  ==== */

		if (kbot - ks + 1 == 2) {
		    if (wi[kbot] == 0.) {
			if ((d__1 = wr[kbot] - h__[kbot + kbot * h_dim1], abs(
				d__1)) < (d__2 = wr[kbot - 1] - h__[kbot +
				kbot * h_dim1], abs(d__2))) {
			    wr[kbot - 1] = wr[kbot];
			} else {
			    wr[kbot] = wr[kbot - 1];
			}
		    }
		}

/*              ==== Use up to NS of the the smallest magnatiude */
/*              .    shifts.  If there aren't NS shifts available, */
/*              .    then use them all, possibly dropping one to */
/*              .    make the number of shifts even. ==== */

/* Computing MIN */
		i__2 = ns, i__3 = kbot - ks + 1;
		ns = std::min(i__2,i__3);
		ns -= ns % 2;
		ks = kbot - ns + 1;

/*              ==== Small-bulge multi-shift QR sweep: */
/*              .    split workspace under the subdiagonal into */
/*              .    - a KDU-by-KDU work array U in the lower */
/*              .      left-hand-corner, */
/*              .    - a KDU-by-at-least-KDU-but-more-is-better */
/*              .      (KDU-by-NHo) horizontal work array WH along */
/*              .      the bottom edge, */
/*              .    - and an at-least-KDU-but-more-is-better-by-KDU */
/*              .      (NVE-by-KDU) vertical work WV arrow along */
/*              .      the left-hand-edge. ==== */

		kdu = ns * 3 - 3;
		ku = *n - kdu + 1;
		kwh = kdu + 1;
		nho = *n - kdu - 3 - (kdu + 1) + 1;
		kwv = kdu + 4;
		nve = *n - kdu - kwv + 1;

/*              ==== Small-bulge multi-shift QR sweep ==== */

		dlaqr5_(wantt, wantz, &kacc22, n, &ktop, &kbot, &ns, &wr[ks],
			&wi[ks], &h__[h_offset], ldh, iloz, ihiz, &z__[
			z_offset], ldz, &work[1], &c__3, &h__[ku + h_dim1],
			ldh, &nve, &h__[kwv + h_dim1], ldh, &nho, &h__[ku +
			kwh * h_dim1], ldh);
	    }

/*           ==== Note progress (or the lack of it). ==== */

	    if (ld > 0) {
		ndfl = 1;
	    } else {
		++ndfl;
	    }

/*           ==== End of main loop ==== */
/* L80: */
	}

/*        ==== Iteration limit exceeded.  Set INFO to show where */
/*        .    the problem occurred and exit. ==== */

	*info = kbot;
L90:
	;
    }

/*     ==== Return the optimal value of LWORK. ==== */

    work[1] = (double) lwkopt;

/*     ==== End of DLAQR0 ==== */

    return 0;
} /* dlaqr0_ */

/* Subroutine */ int dlaqr1_(integer *n, double *h__, integer *ldh,
	double *sr1, double *si1, double *sr2, double *si2,
	double *v)
{
    /* System generated locals */
    integer h_dim1, h_offset;
    double d__1, d__2, d__3;

    /* Local variables */
    double s, h21s, h31s;


/*  -- LAPACK auxiliary routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*       Given a 2-by-2 or 3-by-3 matrix H, DLAQR1 sets v to a */
/*       scalar multiple of the first column of the product */

/*       (*)  K = (H - (sr1 + i*si1)*I)*(H - (sr2 + i*si2)*I) */

/*       scaling to avoid overflows and most underflows. It */
/*       is assumed that either */

/*               1) sr1 = sr2 and si1 = -si2 */
/*           or */
/*               2) si1 = si2 = 0. */

/*       This is useful for starting double implicit shift bulges */
/*       in the QR algorithm. */


/*       N      (input) integer */
/*              Order of the matrix H. N must be either 2 or 3. */

/*       H      (input) DOUBLE PRECISION array of dimension (LDH,N) */
/*              The 2-by-2 or 3-by-3 matrix H in (*). */

/*       LDH    (input) integer */
/*              The leading dimension of H as declared in */
/*              the calling procedure.  LDH.GE.N */

/*       SR1    (input) DOUBLE PRECISION */
/*       SI1    The shifts in (*). */
/*       SR2 */
/*       SI2 */

/*       V      (output) DOUBLE PRECISION array of dimension N */
/*              A scalar multiple of the first column of the */
/*              matrix K in (*). */

/*     ================================================================ */
/*     Based on contributions by */
/*        Karen Braman and Ralph Byers, Department of Mathematics, */
/*        University of Kansas, USA */

/*     ================================================================ */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */
    /* Parameter adjustments */
    h_dim1 = *ldh;
    h_offset = 1 + h_dim1;
    h__ -= h_offset;
    --v;

    /* Function Body */
    if (*n == 2) {
	s = (d__1 = h__[h_dim1 + 1] - *sr2, abs(d__1)) + abs(*si2) + (d__2 =
		h__[h_dim1 + 2], abs(d__2));
	if (s == 0.) {
	    v[1] = 0.;
	    v[2] = 0.;
	} else {
	    h21s = h__[h_dim1 + 2] / s;
	    v[1] = h21s * h__[(h_dim1 << 1) + 1] + (h__[h_dim1 + 1] - *sr1) *
		    ((h__[h_dim1 + 1] - *sr2) / s) - *si1 * (*si2 / s);
	    v[2] = h21s * (h__[h_dim1 + 1] + h__[(h_dim1 << 1) + 2] - *sr1 - *
		    sr2);
	}
    } else {
	s = (d__1 = h__[h_dim1 + 1] - *sr2, abs(d__1)) + abs(*si2) + (d__2 =
		h__[h_dim1 + 2], abs(d__2)) + (d__3 = h__[h_dim1 + 3], abs(
		d__3));
	if (s == 0.) {
	    v[1] = 0.;
	    v[2] = 0.;
	    v[3] = 0.;
	} else {
	    h21s = h__[h_dim1 + 2] / s;
	    h31s = h__[h_dim1 + 3] / s;
	    v[1] = (h__[h_dim1 + 1] - *sr1) * ((h__[h_dim1 + 1] - *sr2) / s)
		    - *si1 * (*si2 / s) + h__[(h_dim1 << 1) + 1] * h21s + h__[
		    h_dim1 * 3 + 1] * h31s;
	    v[2] = h21s * (h__[h_dim1 + 1] + h__[(h_dim1 << 1) + 2] - *sr1 - *
		    sr2) + h__[h_dim1 * 3 + 2] * h31s;
	    v[3] = h31s * (h__[h_dim1 + 1] + h__[h_dim1 * 3 + 3] - *sr1 - *
		    sr2) + h21s * h__[(h_dim1 << 1) + 3];
	}
    }
    return 0;
} /* dlaqr1_ */

/* Subroutine */ int dlaqr2_(bool *wantt, bool *wantz, integer *n, integer *ktop, integer *kbot, integer *nw,
	double *h__, integer *ldh, integer *iloz, integer *ihiz, double *z__, integer *ldz,
	integer *ns, integer *nd, double *sr, double *si, double *v, integer *ldv, integer *nh,
	double *t, integer *ldt, integer *nv, double *wv, integer *ldwv, double *work, integer *lwork)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static double c_b12 = 0.;
	static double c_b13 = 1.;
	static bool c_true = true;

    /* System generated locals */
    integer h_dim1, h_offset, t_dim1, t_offset, v_dim1, v_offset, wv_dim1,
	    wv_offset, z_dim1, z_offset, i__1, i__2, i__3, i__4;
    double d__1, d__2, d__3, d__4, d__5, d__6;

    /* Local variables */
    integer i__, j, k;
    double s, aa, bb, cc, dd, cs, sn;
    integer jw;
    double evi, evk, foo;
    integer kln;
    double tau, ulp;
    integer lwk1, lwk2;
    double beta;
    integer kend, kcol, info, ifst, ilst, ltop, krow;
    bool bulge;
    integer infqr, kwtop;
    double safmin;
    double safmax;
    bool sorted;
    double smlnum;
    integer lwkopt;

/*  -- LAPACK auxiliary routine (version 3.2.1)                        -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd.. */
/*  -- April 2009                                                      -- */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*     This subroutine is identical to DLAQR3 except that it avoids */
/*     recursion by calling DLAHQR instead of DLAQR4. */


/*     ****************************************************************** */
/*     Aggressive early deflation: */

/*     This subroutine accepts as input an upper Hessenberg matrix */
/*     H and performs an orthogonal similarity transformation */
/*     designed to detect and deflate fully converged eigenvalues from */
/*     a trailing principal submatrix.  On output H has been over- */
/*     written by a new Hessenberg matrix that is a perturbation of */
/*     an orthogonal similarity transformation of H.  It is to be */
/*     hoped that the final version of H has many zero subdiagonal */
/*     entries. */

/*     ****************************************************************** */
/*     WANTT   (input) LOGICAL */
/*          If .TRUE., then the Hessenberg matrix H is fully updated */
/*          so that the quasi-triangular Schur factor may be */
/*          computed (in cooperation with the calling subroutine). */
/*          If .FALSE., then only enough of H is updated to preserve */
/*          the eigenvalues. */

/*     WANTZ   (input) LOGICAL */
/*          If .TRUE., then the orthogonal matrix Z is updated so */
/*          so that the orthogonal Schur factor may be computed */
/*          (in cooperation with the calling subroutine). */
/*          If .FALSE., then Z is not referenced. */

/*     N       (input) INTEGER */
/*          The order of the matrix H and (if WANTZ is .TRUE.) the */
/*          order of the orthogonal matrix Z. */

/*     KTOP    (input) INTEGER */
/*          It is assumed that either KTOP = 1 or H(KTOP,KTOP-1)=0. */
/*          KBOT and KTOP together determine an isolated block */
/*          along the diagonal of the Hessenberg matrix. */

/*     KBOT    (input) INTEGER */
/*          It is assumed without a check that either */
/*          KBOT = N or H(KBOT+1,KBOT)=0.  KBOT and KTOP together */
/*          determine an isolated block along the diagonal of the */
/*          Hessenberg matrix. */

/*     NW      (input) INTEGER */
/*          Deflation window size.  1 .LE. NW .LE. (KBOT-KTOP+1). */

/*     H       (input/output) DOUBLE PRECISION array, dimension (LDH,N) */
/*          On input the initial N-by-N section of H stores the */
/*          Hessenberg matrix undergoing aggressive early deflation. */
/*          On output H has been transformed by an orthogonal */
/*          similarity transformation, perturbed, and the returned */
/*          to Hessenberg form that (it is to be hoped) has some */
/*          zero subdiagonal entries. */

/*     LDH     (input) integer */
/*          Leading dimension of H just as declared in the calling */
/*          subroutine.  N .LE. LDH */

/*     ILOZ    (input) INTEGER */
/*     IHIZ    (input) INTEGER */
/*          Specify the rows of Z to which transformations must be */
/*          applied if WANTZ is .TRUE.. 1 .LE. ILOZ .LE. IHIZ .LE. N. */

/*     Z       (input/output) DOUBLE PRECISION array, dimension (LDZ,N) */
/*          IF WANTZ is .TRUE., then on output, the orthogonal */
/*          similarity transformation mentioned above has been */
/*          accumulated into Z(ILOZ:IHIZ,ILO:IHI) from the right. */
/*          If WANTZ is .FALSE., then Z is unreferenced. */

/*     LDZ     (input) integer */
/*          The leading dimension of Z just as declared in the */
/*          calling subroutine.  1 .LE. LDZ. */

/*     NS      (output) integer */
/*          The number of unconverged (ie approximate) eigenvalues */
/*          returned in SR and SI that may be used as shifts by the */
/*          calling subroutine. */

/*     ND      (output) integer */
/*          The number of converged eigenvalues uncovered by this */
/*          subroutine. */

/*     SR      (output) DOUBLE PRECISION array, dimension KBOT */
/*     SI      (output) DOUBLE PRECISION array, dimension KBOT */
/*          On output, the real and imaginary parts of approximate */
/*          eigenvalues that may be used for shifts are stored in */
/*          SR(KBOT-ND-NS+1) through SR(KBOT-ND) and */
/*          SI(KBOT-ND-NS+1) through SI(KBOT-ND), respectively. */
/*          The real and imaginary parts of converged eigenvalues */
/*          are stored in SR(KBOT-ND+1) through SR(KBOT) and */
/*          SI(KBOT-ND+1) through SI(KBOT), respectively. */

/*     V       (workspace) DOUBLE PRECISION array, dimension (LDV,NW) */
/*          An NW-by-NW work array. */

/*     LDV     (input) integer scalar */
/*          The leading dimension of V just as declared in the */
/*          calling subroutine.  NW .LE. LDV */

/*     NH      (input) integer scalar */
/*          The number of columns of T.  NH.GE.NW. */

/*     T       (workspace) DOUBLE PRECISION array, dimension (LDT,NW) */

/*     LDT     (input) integer */
/*          The leading dimension of T just as declared in the */
/*          calling subroutine.  NW .LE. LDT */

/*     NV      (input) integer */
/*          The number of rows of work array WV available for */
/*          workspace.  NV.GE.NW. */

/*     WV      (workspace) DOUBLE PRECISION array, dimension (LDWV,NW) */

/*     LDWV    (input) integer */
/*          The leading dimension of W just as declared in the */
/*          calling subroutine.  NW .LE. LDV */

/*     WORK    (workspace) DOUBLE PRECISION array, dimension LWORK. */
/*          On exit, WORK(1) is set to an estimate of the optimal value */
/*          of LWORK for the given values of N, NW, KTOP and KBOT. */

/*     LWORK   (input) integer */
/*          The dimension of the work array WORK.  LWORK = 2*NW */
/*          suffices, but greater efficiency may result from larger */
/*          values of LWORK. */

/*          If LWORK = -1, then a workspace query is assumed; DLAQR2 */
/*          only estimates the optimal workspace size for the given */
/*          values of N, NW, KTOP and KBOT.  The estimate is returned */
/*          in WORK(1).  No error message related to LWORK is issued */
/*          by XERBLA.  Neither H nor Z are accessed. */

/*     ================================================================ */
/*     Based on contributions by */
/*        Karen Braman and Ralph Byers, Department of Mathematics, */
/*        University of Kansas, USA */

/*     ================================================================ */
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

/*     ==== Estimate optimal workspace. ==== */

    /* Parameter adjustments */
    h_dim1 = *ldh;
    h_offset = 1 + h_dim1;
    h__ -= h_offset;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --sr;
    --si;
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    wv_dim1 = *ldwv;
    wv_offset = 1 + wv_dim1;
    wv -= wv_offset;
    --work;

    /* Function Body */
/* Computing MIN */
    i__1 = *nw, i__2 = *kbot - *ktop + 1;
    jw = std::min(i__1,i__2);
    if (jw <= 2) {
	lwkopt = 1;
    } else {

/*        ==== Workspace query call to DGEHRD ==== */

	i__1 = jw - 1;
	dgehrd_(&jw, &c__1, &i__1, &t[t_offset], ldt, &work[1], &work[1], &
		c_n1, &info);
	lwk1 = (integer) work[1];

/*        ==== Workspace query call to DORMHR ==== */

	i__1 = jw - 1;
	dormhr_("R", "N", &jw, &jw, &c__1, &i__1, &t[t_offset], ldt, &work[1],
		 &v[v_offset], ldv, &work[1], &c_n1, &info);
	lwk2 = (integer) work[1];

/*        ==== Optimal workspace ==== */

	lwkopt = jw + std::max(lwk1,lwk2);
    }

/*     ==== Quick return in case of workspace query. ==== */

    if (*lwork == -1) {
	work[1] = (double) lwkopt;
	return 0;
    }

/*     ==== Nothing to do ... */
/*     ... for an empty active block ... ==== */
    *ns = 0;
    *nd = 0;
	work[1] = 1.;
    if (*ktop > *kbot) {
	return 0;
    }
/*     ... nor for an empty deflation window. ==== */
    if (*nw < 1) {
	return 0;
    }

/*     ==== Machine constants ==== */

    safmin = dlamch_("SAFE MINIMUM");
    safmax = 1. / safmin;
    dlabad_(&safmin, &safmax);
    ulp = dlamch_("PRECISION");
    smlnum = safmin * ((double) (*n) / ulp);

/*     ==== Setup deflation window ==== */

/* Computing MIN */
    i__1 = *nw, i__2 = *kbot - *ktop + 1;
    jw = std::min(i__1,i__2);
    kwtop = *kbot - jw + 1;
    if (kwtop == *ktop) {
	s = 0.;
    } else {
	s = h__[kwtop + (kwtop - 1) * h_dim1];
    }

    if (*kbot == kwtop) {

/*        ==== 1-by-1 deflation window: not much to do ==== */

	sr[kwtop] = h__[kwtop + kwtop * h_dim1];
	si[kwtop] = 0.;
	*ns = 1;
	*nd = 0;
/* Computing MAX */
	d__2 = smlnum, d__3 = ulp * (d__1 = h__[kwtop + kwtop * h_dim1], abs(
		d__1));
	if (abs(s) <= std::max(d__2,d__3)) {
	    *ns = 0;
	    *nd = 1;
	    if (kwtop > *ktop) {
		h__[kwtop + (kwtop - 1) * h_dim1] = 0.;
	    }
	}
	work[1] = 1.;
	return 0;
    }

/*     ==== Convert to spike-triangular form.  (In case of a */
/*     .    rare QR failure, this routine continues to do */
/*     .    aggressive early deflation using that part of */
/*     .    the deflation window that converged using INFQR */
/*     .    here and there to keep track.) ==== */

    dlacpy_("U", &jw, &jw, &h__[kwtop + kwtop * h_dim1], ldh, &t[t_offset],
	    ldt);
    i__1 = jw - 1;
    i__2 = *ldh + 1;
    i__3 = *ldt + 1;
    dcopy_(&i__1, &h__[kwtop + 1 + kwtop * h_dim1], &i__2, &t[t_dim1 + 2], &
	    i__3);

    dlaset_("A", &jw, &jw, &c_b12, &c_b13, &v[v_offset], ldv);
    dlahqr_(&c_true, &c_true, &jw, &c__1, &jw, &t[t_offset], ldt, &sr[kwtop],
	    &si[kwtop], &c__1, &jw, &v[v_offset], ldv, &infqr);

/*     ==== DTREXC needs a clean margin near the diagonal ==== */

    i__1 = jw - 3;
    for (j = 1; j <= i__1; ++j) {
	t[j + 2 + j * t_dim1] = 0.;
	t[j + 3 + j * t_dim1] = 0.;
/* L10: */
    }
    if (jw > 2) {
	t[jw + (jw - 2) * t_dim1] = 0.;
    }

/*     ==== Deflation detection loop ==== */

    *ns = jw;
    ilst = infqr + 1;
L20:
    if (ilst <= *ns) {
	if (*ns == 1) {
	    bulge = false;
	} else {
	    bulge = t[*ns + (*ns - 1) * t_dim1] != 0.;
	}

/*        ==== Small spike tip test for deflation ==== */

	if (! bulge) {

/*           ==== Real eigenvalue ==== */

	    foo = (d__1 = t[*ns + *ns * t_dim1], abs(d__1));
	    if (foo == 0.) {
		foo = abs(s);
	    }
/* Computing MAX */
	    d__2 = smlnum, d__3 = ulp * foo;
	    if ((d__1 = s * v[*ns * v_dim1 + 1], abs(d__1)) <= std::max(d__2,d__3))
		     {

/*              ==== Deflatable ==== */

		--(*ns);
	    } else {

/*              ==== Undeflatable.   Move it up out of the way. */
/*              .    (DTREXC can not fail in this case.) ==== */

		ifst = *ns;
		dtrexc_("V", &jw, &t[t_offset], ldt, &v[v_offset], ldv, &ifst,
			 &ilst, &work[1], &info);
		++ilst;
	    }
	} else {

/*           ==== Complex conjugate pair ==== */

	    foo = (d__3 = t[*ns + *ns * t_dim1], abs(d__3)) + sqrt((d__1 = t[*
		    ns + (*ns - 1) * t_dim1], abs(d__1))) * sqrt((d__2 = t[*
		    ns - 1 + *ns * t_dim1], abs(d__2)));
	    if (foo == 0.) {
		foo = abs(s);
	    }
/* Computing MAX */
	    d__3 = (d__1 = s * v[*ns * v_dim1 + 1], abs(d__1)), d__4 = (d__2 =
		     s * v[(*ns - 1) * v_dim1 + 1], abs(d__2));
/* Computing MAX */
	    d__5 = smlnum, d__6 = ulp * foo;
	    if (std::max(d__3,d__4) <= std::max(d__5,d__6)) {

/*              ==== Deflatable ==== */

		*ns += -2;
	    } else {

/*              ==== Undflatable. Move them up out of the way. */
/*              .    Fortunately, DTREXC does the right thing with */
/*              .    ILST in case of a rare exchange failure. ==== */

		ifst = *ns;
		dtrexc_("V", &jw, &t[t_offset], ldt, &v[v_offset], ldv, &ifst,
			 &ilst, &work[1], &info);
		ilst += 2;
	    }
	}

/*        ==== End deflation detection loop ==== */

	goto L20;
    }

/*        ==== Return to Hessenberg form ==== */

    if (*ns == 0) {
	s = 0.;
    }

    if (*ns < jw) {

/*        ==== sorting diagonal blocks of T improves accuracy for */
/*        .    graded matrices.  Bubble sort deals well with */
/*        .    exchange failures. ==== */

	sorted = false;
	i__ = *ns + 1;
L30:
	if (sorted) {
	    goto L50;
	}
	sorted = true;

	kend = i__ - 1;
	i__ = infqr + 1;
	if (i__ == *ns) {
	    k = i__ + 1;
	} else if (t[i__ + 1 + i__ * t_dim1] == 0.) {
	    k = i__ + 1;
	} else {
	    k = i__ + 2;
	}
L40:
	if (k <= kend) {
	    if (k == i__ + 1) {
		evi = (d__1 = t[i__ + i__ * t_dim1], abs(d__1));
	    } else {
		evi = (d__3 = t[i__ + i__ * t_dim1], abs(d__3)) + sqrt((d__1 =
			 t[i__ + 1 + i__ * t_dim1], abs(d__1))) * sqrt((d__2 =
			 t[i__ + (i__ + 1) * t_dim1], abs(d__2)));
	    }

	    if (k == kend) {
		evk = (d__1 = t[k + k * t_dim1], abs(d__1));
	    } else if (t[k + 1 + k * t_dim1] == 0.) {
		evk = (d__1 = t[k + k * t_dim1], abs(d__1));
	    } else {
		evk = (d__3 = t[k + k * t_dim1], abs(d__3)) + sqrt((d__1 = t[
			k + 1 + k * t_dim1], abs(d__1))) * sqrt((d__2 = t[k +
			(k + 1) * t_dim1], abs(d__2)));
	    }

	    if (evi >= evk) {
		i__ = k;
	    } else {
		sorted = false;
		ifst = i__;
		ilst = k;
		dtrexc_("V", &jw, &t[t_offset], ldt, &v[v_offset], ldv, &ifst,
			 &ilst, &work[1], &info);
		if (info == 0) {
		    i__ = ilst;
		} else {
		    i__ = k;
		}
	    }
	    if (i__ == kend) {
		k = i__ + 1;
	    } else if (t[i__ + 1 + i__ * t_dim1] == 0.) {
		k = i__ + 1;
	    } else {
		k = i__ + 2;
	    }
	    goto L40;
	}
	goto L30;
L50:
	;
    }

/*     ==== Restore shift/eigenvalue array from T ==== */

    i__ = jw;
L60:
    if (i__ >= infqr + 1) {
	if (i__ == infqr + 1) {
	    sr[kwtop + i__ - 1] = t[i__ + i__ * t_dim1];
	    si[kwtop + i__ - 1] = 0.;
	    --i__;
	} else if (t[i__ + (i__ - 1) * t_dim1] == 0.) {
	    sr[kwtop + i__ - 1] = t[i__ + i__ * t_dim1];
	    si[kwtop + i__ - 1] = 0.;
	    --i__;
	} else {
	    aa = t[i__ - 1 + (i__ - 1) * t_dim1];
	    cc = t[i__ + (i__ - 1) * t_dim1];
	    bb = t[i__ - 1 + i__ * t_dim1];
	    dd = t[i__ + i__ * t_dim1];
	    dlanv2_(&aa, &bb, &cc, &dd, &sr[kwtop + i__ - 2], &si[kwtop + i__
		    - 2], &sr[kwtop + i__ - 1], &si[kwtop + i__ - 1], &cs, &
		    sn);
	    i__ += -2;
	}
	goto L60;
    }

    if (*ns < jw || s == 0.) {
	if (*ns > 1 && s != 0.) {

/*           ==== Reflect spike back into lower triangle ==== */

	    dcopy_(ns, &v[v_offset], ldv, &work[1], &c__1);
	    beta = work[1];
	    dlarfg_(ns, &beta, &work[2], &c__1, &tau);
	    work[1] = 1.;

	    i__1 = jw - 2;
	    i__2 = jw - 2;
	    dlaset_("L", &i__1, &i__2, &c_b12, &c_b12, &t[t_dim1 + 3], ldt);

	    dlarf_("L", ns, &jw, &work[1], &c__1, &tau, &t[t_offset], ldt, &
		    work[jw + 1]);
	    dlarf_("R", ns, ns, &work[1], &c__1, &tau, &t[t_offset], ldt, &
		    work[jw + 1]);
	    dlarf_("R", &jw, ns, &work[1], &c__1, &tau, &v[v_offset], ldv, &
		    work[jw + 1]);

	    i__1 = *lwork - jw;
	    dgehrd_(&jw, &c__1, ns, &t[t_offset], ldt, &work[1], &work[jw + 1]
, &i__1, &info);
	}

/*        ==== Copy updated reduced window into place ==== */

	if (kwtop > 1) {
	    h__[kwtop + (kwtop - 1) * h_dim1] = s * v[v_dim1 + 1];
	}
	dlacpy_("U", &jw, &jw, &t[t_offset], ldt, &h__[kwtop + kwtop * h_dim1]
, ldh);
	i__1 = jw - 1;
	i__2 = *ldt + 1;
	i__3 = *ldh + 1;
	dcopy_(&i__1, &t[t_dim1 + 2], &i__2, &h__[kwtop + 1 + kwtop * h_dim1],
		 &i__3);

/*        ==== Accumulate orthogonal matrix in order update */
/*        .    H and Z, if requested.  ==== */

	if (*ns > 1 && s != 0.) {
	    i__1 = *lwork - jw;
	    dormhr_("R", "N", &jw, ns, &c__1, ns, &t[t_offset], ldt, &work[1],
		     &v[v_offset], ldv, &work[jw + 1], &i__1, &info);
	}

/*        ==== Update vertical slab in H ==== */

	if (*wantt) {
	    ltop = 1;
	} else {
	    ltop = *ktop;
	}
	i__1 = kwtop - 1;
	i__2 = *nv;
	for (krow = ltop; i__2 < 0 ? krow >= i__1 : krow <= i__1; krow +=
		i__2) {
/* Computing MIN */
	    i__3 = *nv, i__4 = kwtop - krow;
	    kln = std::min(i__3,i__4);
	    dgemm_("N", "N", &kln, &jw, &jw, &c_b13, &h__[krow + kwtop *
		    h_dim1], ldh, &v[v_offset], ldv, &c_b12, &wv[wv_offset],
		    ldwv);
	    dlacpy_("A", &kln, &jw, &wv[wv_offset], ldwv, &h__[krow + kwtop *
		    h_dim1], ldh);
/* L70: */
	}

/*        ==== Update horizontal slab in H ==== */

	if (*wantt) {
	    i__2 = *n;
	    i__1 = *nh;
	    for (kcol = *kbot + 1; i__1 < 0 ? kcol >= i__2 : kcol <= i__2;
		    kcol += i__1) {
/* Computing MIN */
		i__3 = *nh, i__4 = *n - kcol + 1;
		kln = std::min(i__3,i__4);
		dgemm_("C", "N", &jw, &kln, &jw, &c_b13, &v[v_offset], ldv, &
			h__[kwtop + kcol * h_dim1], ldh, &c_b12, &t[t_offset],
			 ldt);
		dlacpy_("A", &jw, &kln, &t[t_offset], ldt, &h__[kwtop + kcol *
			 h_dim1], ldh);
/* L80: */
	    }
	}

/*        ==== Update vertical slab in Z ==== */

	if (*wantz) {
	    i__1 = *ihiz;
	    i__2 = *nv;
	    for (krow = *iloz; i__2 < 0 ? krow >= i__1 : krow <= i__1; krow +=
		     i__2) {
/* Computing MIN */
		i__3 = *nv, i__4 = *ihiz - krow + 1;
		kln = std::min(i__3,i__4);
		dgemm_("N", "N", &kln, &jw, &jw, &c_b13, &z__[krow + kwtop *
			z_dim1], ldz, &v[v_offset], ldv, &c_b12, &wv[
			wv_offset], ldwv);
		dlacpy_("A", &kln, &jw, &wv[wv_offset], ldwv, &z__[krow +
			kwtop * z_dim1], ldz);
/* L90: */
	    }
	}
    }

/*     ==== Return the number of deflations ... ==== */

    *nd = jw - *ns;

/*     ==== ... and the number of shifts. (Subtracting */
/*     .    INFQR from the spike length takes care */
/*     .    of the case of a rare QR failure while */
/*     .    calculating eigenvalues of the deflation */
/*     .    window.)  ==== */

    *ns -= infqr;

/*      ==== Return optimal workspace. ==== */

    work[1] = (double) lwkopt;

/*     ==== End of DLAQR2 ==== */

    return 0;
} /* dlaqr2_ */

/* Subroutine */ int dlaqr3_(bool *wantt, bool *wantz, integer *n, integer *ktop, integer *kbot, integer *nw,
	double *h__, integer *ldh, integer *iloz, integer *ihiz, double *z__, integer *ldz,
	integer *ns, integer *nd, double *sr, double *si, double *v, integer *ldv, integer *nh,
	double *t, integer *ldt, integer *nv, double *wv, integer *ldwv, double *work, integer *lwork)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static integer c_n1 = -1;
	static bool c_true = true;
	static double c_b17 = 0.;
	static double c_b18 = 1.;
	static integer c__12 = 12;

    /* System generated locals */
    integer h_dim1, h_offset, t_dim1, t_offset, v_dim1, v_offset, wv_dim1,
	    wv_offset, z_dim1, z_offset, i__1, i__2, i__3, i__4;
    double d__1, d__2, d__3, d__4, d__5, d__6;

    /* Local variables */
    integer i__, j, k;
    double s, aa, bb, cc, dd, cs, sn;
    integer jw;
    double evi, evk, foo;
    integer kln;
    double tau, ulp;
    integer lwk1, lwk2, lwk3;
    double beta;
    integer kend, kcol, info, nmin, ifst, ilst, ltop, krow;
    bool bulge;
    integer infqr, kwtop;
    double safmin;
    double safmax;
	bool sorted;
    double smlnum;
    integer lwkopt;


/*  -- LAPACK auxiliary routine (version 3.2.1)                        -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd.. */
/*  -- April 2009                                                                            -- */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*     ****************************************************************** */
/*     Aggressive early deflation: */

/*     This subroutine accepts as input an upper Hessenberg matrix */
/*     H and performs an orthogonal similarity transformation */
/*     designed to detect and deflate fully converged eigenvalues from */
/*     a trailing principal submatrix.  On output H has been over- */
/*     written by a new Hessenberg matrix that is a perturbation of */
/*     an orthogonal similarity transformation of H.  It is to be */
/*     hoped that the final version of H has many zero subdiagonal */
/*     entries. */

/*     ****************************************************************** */
/*     WANTT   (input) LOGICAL */
/*          If .TRUE., then the Hessenberg matrix H is fully updated */
/*          so that the quasi-triangular Schur factor may be */
/*          computed (in cooperation with the calling subroutine). */
/*          If .FALSE., then only enough of H is updated to preserve */
/*          the eigenvalues. */

/*     WANTZ   (input) LOGICAL */
/*          If .TRUE., then the orthogonal matrix Z is updated so */
/*          so that the orthogonal Schur factor may be computed */
/*          (in cooperation with the calling subroutine). */
/*          If .FALSE., then Z is not referenced. */

/*     N       (input) INTEGER */
/*          The order of the matrix H and (if WANTZ is .TRUE.) the */
/*          order of the orthogonal matrix Z. */

/*     KTOP    (input) INTEGER */
/*          It is assumed that either KTOP = 1 or H(KTOP,KTOP-1)=0. */
/*          KBOT and KTOP together determine an isolated block */
/*          along the diagonal of the Hessenberg matrix. */

/*     KBOT    (input) INTEGER */
/*          It is assumed without a check that either */
/*          KBOT = N or H(KBOT+1,KBOT)=0.  KBOT and KTOP together */
/*          determine an isolated block along the diagonal of the */
/*          Hessenberg matrix. */

/*     NW      (input) INTEGER */
/*          Deflation window size.  1 .LE. NW .LE. (KBOT-KTOP+1). */

/*     H       (input/output) DOUBLE PRECISION array, dimension (LDH,N) */
/*          On input the initial N-by-N section of H stores the */
/*          Hessenberg matrix undergoing aggressive early deflation. */
/*          On output H has been transformed by an orthogonal */
/*          similarity transformation, perturbed, and the returned */
/*          to Hessenberg form that (it is to be hoped) has some */
/*          zero subdiagonal entries. */

/*     LDH     (input) integer */
/*          Leading dimension of H just as declared in the calling */
/*          subroutine.  N .LE. LDH */

/*     ILOZ    (input) INTEGER */
/*     IHIZ    (input) INTEGER */
/*          Specify the rows of Z to which transformations must be */
/*          applied if WANTZ is .TRUE.. 1 .LE. ILOZ .LE. IHIZ .LE. N. */

/*     Z       (input/output) DOUBLE PRECISION array, dimension (LDZ,N) */
/*          IF WANTZ is .TRUE., then on output, the orthogonal */
/*          similarity transformation mentioned above has been */
/*          accumulated into Z(ILOZ:IHIZ,ILO:IHI) from the right. */
/*          If WANTZ is .FALSE., then Z is unreferenced. */

/*     LDZ     (input) integer */
/*          The leading dimension of Z just as declared in the */
/*          calling subroutine.  1 .LE. LDZ. */

/*     NS      (output) integer */
/*          The number of unconverged (ie approximate) eigenvalues */
/*          returned in SR and SI that may be used as shifts by the */
/*          calling subroutine. */

/*     ND      (output) integer */
/*          The number of converged eigenvalues uncovered by this */
/*          subroutine. */

/*     SR      (output) DOUBLE PRECISION array, dimension KBOT */
/*     SI      (output) DOUBLE PRECISION array, dimension KBOT */
/*          On output, the real and imaginary parts of approximate */
/*          eigenvalues that may be used for shifts are stored in */
/*          SR(KBOT-ND-NS+1) through SR(KBOT-ND) and */
/*          SI(KBOT-ND-NS+1) through SI(KBOT-ND), respectively. */
/*          The real and imaginary parts of converged eigenvalues */
/*          are stored in SR(KBOT-ND+1) through SR(KBOT) and */
/*          SI(KBOT-ND+1) through SI(KBOT), respectively. */

/*     V       (workspace) DOUBLE PRECISION array, dimension (LDV,NW) */
/*          An NW-by-NW work array. */

/*     LDV     (input) integer scalar */
/*          The leading dimension of V just as declared in the */
/*          calling subroutine.  NW .LE. LDV */

/*     NH      (input) integer scalar */
/*          The number of columns of T.  NH.GE.NW. */

/*     T       (workspace) DOUBLE PRECISION array, dimension (LDT,NW) */

/*     LDT     (input) integer */
/*          The leading dimension of T just as declared in the */
/*          calling subroutine.  NW .LE. LDT */

/*     NV      (input) integer */
/*          The number of rows of work array WV available for */
/*          workspace.  NV.GE.NW. */

/*     WV      (workspace) DOUBLE PRECISION array, dimension (LDWV,NW) */

/*     LDWV    (input) integer */
/*          The leading dimension of W just as declared in the */
/*          calling subroutine.  NW .LE. LDV */

/*     WORK    (workspace) DOUBLE PRECISION array, dimension LWORK. */
/*          On exit, WORK(1) is set to an estimate of the optimal value */
/*          of LWORK for the given values of N, NW, KTOP and KBOT. */

/*     LWORK   (input) integer */
/*          The dimension of the work array WORK.  LWORK = 2*NW */
/*          suffices, but greater efficiency may result from larger */
/*          values of LWORK. */

/*          If LWORK = -1, then a workspace query is assumed; DLAQR3 */
/*          only estimates the optimal workspace size for the given */
/*          values of N, NW, KTOP and KBOT.  The estimate is returned */
/*          in WORK(1).  No error message related to LWORK is issued */
/*          by XERBLA.  Neither H nor Z are accessed. */

/*     ================================================================ */
/*     Based on contributions by */
/*        Karen Braman and Ralph Byers, Department of Mathematics, */
/*        University of Kansas, USA */

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

/*     ==== Estimate optimal workspace. ==== */

    /* Parameter adjustments */
    h_dim1 = *ldh;
    h_offset = 1 + h_dim1;
    h__ -= h_offset;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --sr;
    --si;
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    wv_dim1 = *ldwv;
    wv_offset = 1 + wv_dim1;
    wv -= wv_offset;
    --work;

    /* Function Body */
/* Computing MIN */
    i__1 = *nw, i__2 = *kbot - *ktop + 1;
    jw = std::min(i__1,i__2);
    if (jw <= 2) {
	lwkopt = 1;
    } else {

/*        ==== Workspace query call to DGEHRD ==== */

	i__1 = jw - 1;
	dgehrd_(&jw, &c__1, &i__1, &t[t_offset], ldt, &work[1], &work[1], &
		c_n1, &info);
	lwk1 = (integer) work[1];

/*        ==== Workspace query call to DORMHR ==== */

	i__1 = jw - 1;
	dormhr_("R", "N", &jw, &jw, &c__1, &i__1, &t[t_offset], ldt, &work[1],
		 &v[v_offset], ldv, &work[1], &c_n1, &info);
	lwk2 = (integer) work[1];

/*        ==== Workspace query call to DLAQR4 ==== */

	dlaqr4_(&c_true, &c_true, &jw, &c__1, &jw, &t[t_offset], ldt, &sr[1],
		&si[1], &c__1, &jw, &v[v_offset], ldv, &work[1], &c_n1, &
		infqr);
	lwk3 = (integer) work[1];

/*        ==== Optimal workspace ==== */

/* Computing MAX */
	i__1 = jw + std::max(lwk1,lwk2);
	lwkopt = std::max(i__1,lwk3);
    }

/*     ==== Quick return in case of workspace query. ==== */

    if (*lwork == -1) {
	work[1] = (double) lwkopt;
	return 0;
    }

/*     ==== Nothing to do ... */
/*     ... for an empty active block ... ==== */
    *ns = 0;
    *nd = 0;
	work[1] = 1.;
    if (*ktop > *kbot) {
	return 0;
    }
/*     ... nor for an empty deflation window. ==== */
    if (*nw < 1) {
	return 0;
    }

/*     ==== Machine constants ==== */

    safmin = dlamch_("SAFE MINIMUM");
    safmax = 1. / safmin;
    dlabad_(&safmin, &safmax);
    ulp = dlamch_("PRECISION");
    smlnum = safmin * ((double) (*n) / ulp);

/*     ==== Setup deflation window ==== */

/* Computing MIN */
    i__1 = *nw, i__2 = *kbot - *ktop + 1;
    jw = std::min(i__1,i__2);
    kwtop = *kbot - jw + 1;
    if (kwtop == *ktop) {
	s = 0.;
    } else {
	s = h__[kwtop + (kwtop - 1) * h_dim1];
    }

    if (*kbot == kwtop) {

/*        ==== 1-by-1 deflation window: not much to do ==== */

	sr[kwtop] = h__[kwtop + kwtop * h_dim1];
	si[kwtop] = 0.;
	*ns = 1;
	*nd = 0;
/* Computing MAX */
	d__2 = smlnum, d__3 = ulp * (d__1 = h__[kwtop + kwtop * h_dim1], abs(
		d__1));
	if (abs(s) <= std::max(d__2,d__3)) {
	    *ns = 0;
	    *nd = 1;
	    if (kwtop > *ktop) {
		h__[kwtop + (kwtop - 1) * h_dim1] = 0.;
	    }
	}
	work[1] = 1.;
	return 0;
    }

/*     ==== Convert to spike-triangular form.  (In case of a */
/*     .    rare QR failure, this routine continues to do */
/*     .    aggressive early deflation using that part of */
/*     .    the deflation window that converged using INFQR */
/*     .    here and there to keep track.) ==== */

    dlacpy_("U", &jw, &jw, &h__[kwtop + kwtop * h_dim1], ldh, &t[t_offset],
	    ldt);
    i__1 = jw - 1;
    i__2 = *ldh + 1;
    i__3 = *ldt + 1;
    dcopy_(&i__1, &h__[kwtop + 1 + kwtop * h_dim1], &i__2, &t[t_dim1 + 2], &
	    i__3);

    dlaset_("A", &jw, &jw, &c_b17, &c_b18, &v[v_offset], ldv);
    nmin = ilaenv_(&c__12, "DLAQR3", "SV", &jw, &c__1, &jw, lwork);
    if (jw > nmin) {
	dlaqr4_(&c_true, &c_true, &jw, &c__1, &jw, &t[t_offset], ldt, &sr[
		kwtop], &si[kwtop], &c__1, &jw, &v[v_offset], ldv, &work[1],
		lwork, &infqr);
    } else {
	dlahqr_(&c_true, &c_true, &jw, &c__1, &jw, &t[t_offset], ldt, &sr[
		kwtop], &si[kwtop], &c__1, &jw, &v[v_offset], ldv, &infqr);
    }

/*     ==== DTREXC needs a clean margin near the diagonal ==== */

    i__1 = jw - 3;
    for (j = 1; j <= i__1; ++j) {
	t[j + 2 + j * t_dim1] = 0.;
	t[j + 3 + j * t_dim1] = 0.;
/* L10: */
    }
    if (jw > 2) {
	t[jw + (jw - 2) * t_dim1] = 0.;
    }

/*     ==== Deflation detection loop ==== */

    *ns = jw;
    ilst = infqr + 1;
L20:
    if (ilst <= *ns) {
	if (*ns == 1) {
	    bulge = false;
	} else {
	    bulge = t[*ns + (*ns - 1) * t_dim1] != 0.;
	}

/*        ==== Small spike tip test for deflation ==== */

	if (! bulge) {

/*           ==== Real eigenvalue ==== */

	    foo = (d__1 = t[*ns + *ns * t_dim1], abs(d__1));
	    if (foo == 0.) {
		foo = abs(s);
	    }
/* Computing MAX */
	    d__2 = smlnum, d__3 = ulp * foo;
	    if ((d__1 = s * v[*ns * v_dim1 + 1], abs(d__1)) <= std::max(d__2,d__3))
		     {

/*              ==== Deflatable ==== */

		--(*ns);
	    } else {

/*              ==== Undeflatable.   Move it up out of the way. */
/*              .    (DTREXC can not fail in this case.) ==== */

		ifst = *ns;
		dtrexc_("V", &jw, &t[t_offset], ldt, &v[v_offset], ldv, &ifst,
			 &ilst, &work[1], &info);
		++ilst;
	    }
	} else {

/*           ==== Complex conjugate pair ==== */

	    foo = (d__3 = t[*ns + *ns * t_dim1], abs(d__3)) + sqrt((d__1 = t[*
		    ns + (*ns - 1) * t_dim1], abs(d__1))) * sqrt((d__2 = t[*
		    ns - 1 + *ns * t_dim1], abs(d__2)));
	    if (foo == 0.) {
		foo = abs(s);
	    }
/* Computing MAX */
	    d__3 = (d__1 = s * v[*ns * v_dim1 + 1], abs(d__1)), d__4 = (d__2 =
		     s * v[(*ns - 1) * v_dim1 + 1], abs(d__2));
/* Computing MAX */
	    d__5 = smlnum, d__6 = ulp * foo;
	    if (std::max(d__3,d__4) <= std::max(d__5,d__6)) {

/*              ==== Deflatable ==== */

		*ns += -2;
	    } else {

/*              ==== Undeflatable. Move them up out of the way. */
/*              .    Fortunately, DTREXC does the right thing with */
/*              .    ILST in case of a rare exchange failure. ==== */

		ifst = *ns;
		dtrexc_("V", &jw, &t[t_offset], ldt, &v[v_offset], ldv, &ifst,
			 &ilst, &work[1], &info);
		ilst += 2;
	    }
	}

/*        ==== End deflation detection loop ==== */

	goto L20;
    }

/*        ==== Return to Hessenberg form ==== */

    if (*ns == 0) {
	s = 0.;
    }

    if (*ns < jw) {

/*        ==== sorting diagonal blocks of T improves accuracy for */
/*        .    graded matrices.  Bubble sort deals well with */
/*        .    exchange failures. ==== */

	sorted = false;
	i__ = *ns + 1;
L30:
	if (sorted) {
	    goto L50;
	}
	sorted = true;

	kend = i__ - 1;
	i__ = infqr + 1;
	if (i__ == *ns) {
	    k = i__ + 1;
	} else if (t[i__ + 1 + i__ * t_dim1] == 0.) {
	    k = i__ + 1;
	} else {
	    k = i__ + 2;
	}
L40:
	if (k <= kend) {
	    if (k == i__ + 1) {
		evi = (d__1 = t[i__ + i__ * t_dim1], abs(d__1));
	    } else {
		evi = (d__3 = t[i__ + i__ * t_dim1], abs(d__3)) + sqrt((d__1 =
			 t[i__ + 1 + i__ * t_dim1], abs(d__1))) * sqrt((d__2 =
			 t[i__ + (i__ + 1) * t_dim1], abs(d__2)));
	    }

	    if (k == kend) {
		evk = (d__1 = t[k + k * t_dim1], abs(d__1));
	    } else if (t[k + 1 + k * t_dim1] == 0.) {
		evk = (d__1 = t[k + k * t_dim1], abs(d__1));
	    } else {
		evk = (d__3 = t[k + k * t_dim1], abs(d__3)) + sqrt((d__1 = t[
			k + 1 + k * t_dim1], abs(d__1))) * sqrt((d__2 = t[k +
			(k + 1) * t_dim1], abs(d__2)));
	    }

	    if (evi >= evk) {
		i__ = k;
	    } else {
		sorted = false;
		ifst = i__;
		ilst = k;
		dtrexc_("V", &jw, &t[t_offset], ldt, &v[v_offset], ldv, &ifst,
			 &ilst, &work[1], &info);
		if (info == 0) {
		    i__ = ilst;
		} else {
		    i__ = k;
		}
	    }
	    if (i__ == kend) {
		k = i__ + 1;
	    } else if (t[i__ + 1 + i__ * t_dim1] == 0.) {
		k = i__ + 1;
	    } else {
		k = i__ + 2;
	    }
	    goto L40;
	}
	goto L30;
L50:
	;
    }

/*     ==== Restore shift/eigenvalue array from T ==== */

    i__ = jw;
L60:
    if (i__ >= infqr + 1) {
	if (i__ == infqr + 1) {
	    sr[kwtop + i__ - 1] = t[i__ + i__ * t_dim1];
	    si[kwtop + i__ - 1] = 0.;
	    --i__;
	} else if (t[i__ + (i__ - 1) * t_dim1] == 0.) {
	    sr[kwtop + i__ - 1] = t[i__ + i__ * t_dim1];
	    si[kwtop + i__ - 1] = 0.;
	    --i__;
	} else {
	    aa = t[i__ - 1 + (i__ - 1) * t_dim1];
	    cc = t[i__ + (i__ - 1) * t_dim1];
	    bb = t[i__ - 1 + i__ * t_dim1];
	    dd = t[i__ + i__ * t_dim1];
	    dlanv2_(&aa, &bb, &cc, &dd, &sr[kwtop + i__ - 2], &si[kwtop + i__
		    - 2], &sr[kwtop + i__ - 1], &si[kwtop + i__ - 1], &cs, &
		    sn);
	    i__ += -2;
	}
	goto L60;
    }

    if (*ns < jw || s == 0.) {
	if (*ns > 1 && s != 0.) {

/*           ==== Reflect spike back into lower triangle ==== */

	    dcopy_(ns, &v[v_offset], ldv, &work[1], &c__1);
	    beta = work[1];
	    dlarfg_(ns, &beta, &work[2], &c__1, &tau);
	    work[1] = 1.;

	    i__1 = jw - 2;
	    i__2 = jw - 2;
	    dlaset_("L", &i__1, &i__2, &c_b17, &c_b17, &t[t_dim1 + 3], ldt);

	    dlarf_("L", ns, &jw, &work[1], &c__1, &tau, &t[t_offset], ldt, &
		    work[jw + 1]);
	    dlarf_("R", ns, ns, &work[1], &c__1, &tau, &t[t_offset], ldt, &
		    work[jw + 1]);
	    dlarf_("R", &jw, ns, &work[1], &c__1, &tau, &v[v_offset], ldv, &
		    work[jw + 1]);

	    i__1 = *lwork - jw;
	    dgehrd_(&jw, &c__1, ns, &t[t_offset], ldt, &work[1], &work[jw + 1]
, &i__1, &info);
	}

/*        ==== Copy updated reduced window into place ==== */

	if (kwtop > 1) {
	    h__[kwtop + (kwtop - 1) * h_dim1] = s * v[v_dim1 + 1];
	}
	dlacpy_("U", &jw, &jw, &t[t_offset], ldt, &h__[kwtop + kwtop * h_dim1]
, ldh);
	i__1 = jw - 1;
	i__2 = *ldt + 1;
	i__3 = *ldh + 1;
	dcopy_(&i__1, &t[t_dim1 + 2], &i__2, &h__[kwtop + 1 + kwtop * h_dim1],
		 &i__3);

/*        ==== Accumulate orthogonal matrix in order update */
/*        .    H and Z, if requested. ====  */

	if (*ns > 1 && s != 0.) {
	    i__1 = *lwork - jw;
	    dormhr_("R", "N", &jw, ns, &c__1, ns, &t[t_offset], ldt, &work[1],
		     &v[v_offset], ldv, &work[jw + 1], &i__1, &info);
	}

/*        ==== Update vertical slab in H ==== */

	if (*wantt) {
	    ltop = 1;
	} else {
	    ltop = *ktop;
	}
	i__1 = kwtop - 1;
	i__2 = *nv;
	for (krow = ltop; i__2 < 0 ? krow >= i__1 : krow <= i__1; krow +=
		i__2) {
/* Computing MIN */
	    i__3 = *nv, i__4 = kwtop - krow;
	    kln = std::min(i__3,i__4);
	    dgemm_("N", "N", &kln, &jw, &jw, &c_b18, &h__[krow + kwtop *
		    h_dim1], ldh, &v[v_offset], ldv, &c_b17, &wv[wv_offset],
		    ldwv);
	    dlacpy_("A", &kln, &jw, &wv[wv_offset], ldwv, &h__[krow + kwtop *
		    h_dim1], ldh);
/* L70: */
	}

/*        ==== Update horizontal slab in H ==== */

	if (*wantt) {
	    i__2 = *n;
	    i__1 = *nh;
	    for (kcol = *kbot + 1; i__1 < 0 ? kcol >= i__2 : kcol <= i__2;
		    kcol += i__1) {
/* Computing MIN */
		i__3 = *nh, i__4 = *n - kcol + 1;
		kln = std::min(i__3,i__4);
		dgemm_("C", "N", &jw, &kln, &jw, &c_b18, &v[v_offset], ldv, &
			h__[kwtop + kcol * h_dim1], ldh, &c_b17, &t[t_offset],
			 ldt);
		dlacpy_("A", &jw, &kln, &t[t_offset], ldt, &h__[kwtop + kcol *
			 h_dim1], ldh);
/* L80: */
	    }
	}

/*        ==== Update vertical slab in Z ==== */

	if (*wantz) {
	    i__1 = *ihiz;
	    i__2 = *nv;
	    for (krow = *iloz; i__2 < 0 ? krow >= i__1 : krow <= i__1; krow +=
		     i__2) {
/* Computing MIN */
		i__3 = *nv, i__4 = *ihiz - krow + 1;
		kln = std::min(i__3,i__4);
		dgemm_("N", "N", &kln, &jw, &jw, &c_b18, &z__[krow + kwtop *
			z_dim1], ldz, &v[v_offset], ldv, &c_b17, &wv[
			wv_offset], ldwv);
		dlacpy_("A", &kln, &jw, &wv[wv_offset], ldwv, &z__[krow +
			kwtop * z_dim1], ldz);
/* L90: */
	    }
	}
    }

/*     ==== Return the number of deflations ... ==== */

    *nd = jw - *ns;

/*     ==== ... and the number of shifts. (Subtracting */
/*     .    INFQR from the spike length takes care */
/*     .    of the case of a rare QR failure while */
/*     .    calculating eigenvalues of the deflation */
/*     .    window.)  ==== */

    *ns -= infqr;

/*      ==== Return optimal workspace. ==== */

    work[1] = (double) lwkopt;

/*     ==== End of DLAQR3 ==== */

    return 0;
} /* dlaqr3_ */

/* Subroutine */ int dlaqr4_(bool *wantt, bool *wantz, integer *n, integer *ilo, integer *ihi, double *h__,
	integer *ldh, double *wr, double *wi, integer *iloz, integer *ihiz, double *z__,
	integer *ldz, double *work, integer *lwork, integer *info)
{
	/* Table of constant values */
	static integer c__13 = 13;
	static integer c__15 = 15;
	static integer c_n1 = -1;
	static integer c__12 = 12;
	static integer c__14 = 14;
	static integer c__16 = 16;
	static bool c_false = false;
	static integer c__1 = 1;
	static integer c__3 = 3;

    /* System generated locals */
    integer h_dim1, h_offset, z_dim1, z_offset, i__1, i__2, i__3, i__4, i__5;
    double d__1, d__2, d__3, d__4;

    /* Local variables */
    integer i__, k, ld, nh, it, ks, kt, ku, kv, ls, ns;
    double aa, bb, cc, dd;
    double cs, sn, ss, swap;
    integer nw, inf, kdu, nho, nve, kwh, nsr, nwr, kwv, ndec, ndfl, kbot, nmin;
    integer ktop, kacc22;
    double zdum[1]	/* was [1][1] */;
    bool sorted;
    integer itmax, nsmax, nwmax, kwtop, lwkopt, nibble, nwupbd;
    char jbcmpz[3];


/*  -- LAPACK auxiliary routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*     This subroutine implements one level of recursion for DLAQR0. */
/*     It is a complete implementation of the small bulge multi-shift */
/*     QR algorithm.  It may be called by DLAQR0 and, for large enough */
/*     deflation window size, it may be called by DLAQR3.  This */
/*     subroutine is identical to DLAQR0 except that it calls DLAQR2 */
/*     instead of DLAQR3. */

/*     Purpose */
/*     ======= */

/*     DLAQR4 computes the eigenvalues of a Hessenberg matrix H */
/*     and, optionally, the matrices T and Z from the Schur decomposition */
/*     H = Z T Z**T, where T is an upper quasi-triangular matrix (the */
/*     Schur form), and Z is the orthogonal matrix of Schur vectors. */

/*     Optionally Z may be postmultiplied into an input orthogonal */
/*     matrix Q so that this routine can give the Schur factorization */
/*     of a matrix A which has been reduced to the Hessenberg form H */
/*     by the orthogonal matrix Q:  A = Q*H*Q**T = (QZ)*T*(QZ)**T. */

/*     Arguments */
/*     ========= */

/*     WANTT   (input) LOGICAL */
/*          = .TRUE. : the full Schur form T is required; */
/*          = .FALSE.: only eigenvalues are required. */

/*     WANTZ   (input) LOGICAL */
/*          = .TRUE. : the matrix of Schur vectors Z is required; */
/*          = .FALSE.: Schur vectors are not required. */

/*     N     (input) INTEGER */
/*           The order of the matrix H.  N .GE. 0. */

/*     ILO   (input) INTEGER */
/*     IHI   (input) INTEGER */
/*           It is assumed that H is already upper triangular in rows */
/*           and columns 1:ILO-1 and IHI+1:N and, if ILO.GT.1, */
/*           H(ILO,ILO-1) is zero. ILO and IHI are normally set by a */
/*           previous call to DGEBAL, and then passed to DGEHRD when the */
/*           matrix output by DGEBAL is reduced to Hessenberg form. */
/*           Otherwise, ILO and IHI should be set to 1 and N, */
/*           respectively.  If N.GT.0, then 1.LE.ILO.LE.IHI.LE.N. */
/*           If N = 0, then ILO = 1 and IHI = 0. */

/*     H     (input/output) DOUBLE PRECISION array, dimension (LDH,N) */
/*           On entry, the upper Hessenberg matrix H. */
/*           On exit, if INFO = 0 and WANTT is .TRUE., then H contains */
/*           the upper quasi-triangular matrix T from the Schur */
/*           decomposition (the Schur form); 2-by-2 diagonal blocks */
/*           (corresponding to complex conjugate pairs of eigenvalues) */
/*           are returned in standard form, with H(i,i) = H(i+1,i+1) */
/*           and H(i+1,i)*H(i,i+1).LT.0. If INFO = 0 and WANTT is */
/*           .FALSE., then the contents of H are unspecified on exit. */
/*           (The output value of H when INFO.GT.0 is given under the */
/*           description of INFO below.) */

/*           This subroutine may explicitly set H(i,j) = 0 for i.GT.j and */
/*           j = 1, 2, ... ILO-1 or j = IHI+1, IHI+2, ... N. */

/*     LDH   (input) INTEGER */
/*           The leading dimension of the array H. LDH .GE. max(1,N). */

/*     WR    (output) DOUBLE PRECISION array, dimension (IHI) */
/*     WI    (output) DOUBLE PRECISION array, dimension (IHI) */
/*           The real and imaginary parts, respectively, of the computed */
/*           eigenvalues of H(ILO:IHI,ILO:IHI) are stored in WR(ILO:IHI) */
/*           and WI(ILO:IHI). If two eigenvalues are computed as a */
/*           complex conjugate pair, they are stored in consecutive */
/*           elements of WR and WI, say the i-th and (i+1)th, with */
/*           WI(i) .GT. 0 and WI(i+1) .LT. 0. If WANTT is .TRUE., then */
/*           the eigenvalues are stored in the same order as on the */
/*           diagonal of the Schur form returned in H, with */
/*           WR(i) = H(i,i) and, if H(i:i+1,i:i+1) is a 2-by-2 diagonal */
/*           block, WI(i) = sqrt(-H(i+1,i)*H(i,i+1)) and */
/*           WI(i+1) = -WI(i). */

/*     ILOZ     (input) INTEGER */
/*     IHIZ     (input) INTEGER */
/*           Specify the rows of Z to which transformations must be */
/*           applied if WANTZ is .TRUE.. */
/*           1 .LE. ILOZ .LE. ILO; IHI .LE. IHIZ .LE. N. */

/*     Z     (input/output) DOUBLE PRECISION array, dimension (LDZ,IHI) */
/*           If WANTZ is .FALSE., then Z is not referenced. */
/*           If WANTZ is .TRUE., then Z(ILO:IHI,ILOZ:IHIZ) is */
/*           replaced by Z(ILO:IHI,ILOZ:IHIZ)*U where U is the */
/*           orthogonal Schur factor of H(ILO:IHI,ILO:IHI). */
/*           (The output value of Z when INFO.GT.0 is given under */
/*           the description of INFO below.) */

/*     LDZ   (input) INTEGER */
/*           The leading dimension of the array Z.  if WANTZ is .TRUE. */
/*           then LDZ.GE.MAX(1,IHIZ).  Otherwize, LDZ.GE.1. */

/*     WORK  (workspace/output) DOUBLE PRECISION array, dimension LWORK */
/*           On exit, if LWORK = -1, WORK(1) returns an estimate of */
/*           the optimal value for LWORK. */

/*     LWORK (input) INTEGER */
/*           The dimension of the array WORK.  LWORK .GE. max(1,N) */
/*           is sufficient, but LWORK typically as large as 6*N may */
/*           be required for optimal performance.  A workspace query */
/*           to determine the optimal workspace size is recommended. */

/*           If LWORK = -1, then DLAQR4 does a workspace query. */
/*           In this case, DLAQR4 checks the input parameters and */
/*           estimates the optimal workspace size for the given */
/*           values of N, ILO and IHI.  The estimate is returned */
/*           in WORK(1).  No error message related to LWORK is */
/*           issued by XERBLA.  Neither H nor Z are accessed. */


/*     INFO  (output) INTEGER */
/*             =  0:  successful exit */
/*           .GT. 0:  if INFO = i, DLAQR4 failed to compute all of */
/*                the eigenvalues.  Elements 1:ilo-1 and i+1:n of WR */
/*                and WI contain those eigenvalues which have been */
/*                successfully computed.  (Failures are rare.) */

/*                If INFO .GT. 0 and WANT is .FALSE., then on exit, */
/*                the remaining unconverged eigenvalues are the eigen- */
/*                values of the upper Hessenberg matrix rows and */
/*                columns ILO through INFO of the final, output */
/*                value of H. */

/*                If INFO .GT. 0 and WANTT is .TRUE., then on exit */

/*           (*)  (initial value of H)*U  = U*(final value of H) */

/*                where U is an orthogonal matrix.  The final */
/*                value of H is upper Hessenberg and quasi-triangular */
/*                in rows and columns INFO+1 through IHI. */

/*                If INFO .GT. 0 and WANTZ is .TRUE., then on exit */

/*                  (final value of Z(ILO:IHI,ILOZ:IHIZ) */
/*                   =  (initial value of Z(ILO:IHI,ILOZ:IHIZ)*U */

/*                where U is the orthogonal matrix in (*) (regard- */
/*                less of the value of WANTT.) */

/*                If INFO .GT. 0 and WANTZ is .FALSE., then Z is not */
/*                accessed. */

/*     ================================================================ */
/*     Based on contributions by */
/*        Karen Braman and Ralph Byers, Department of Mathematics, */
/*        University of Kansas, USA */

/*     ================================================================ */
/*     References: */
/*       K. Braman, R. Byers and R. Mathias, The Multi-Shift QR */
/*       Algorithm Part I: Maintaining Well Focused Shifts, and Level 3 */
/*       Performance, SIAM Journal of Matrix Analysis, volume 23, pages */
/*       929--947, 2002. */

/*       K. Braman, R. Byers and R. Mathias, The Multi-Shift QR */
/*       Algorithm Part II: Aggressive Early Deflation, SIAM Journal */
/*       of Matrix Analysis, volume 23, pages 948--973, 2002. */

/*     ================================================================ */
/*     .. Parameters .. */

/*     ==== Matrices of order NTINY or smaller must be processed by */
/*     .    DLAHQR because of insufficient subdiagonal scratch space. */
/*     .    (This is a hard limit.) ==== */

/*     ==== Exceptional deflation windows:  try to cure rare */
/*     .    slow convergence by varying the size of the */
/*     .    deflation window after KEXNW iterations. ==== */

/*     ==== Exceptional shifts: try to cure rare slow convergence */
/*     .    with ad-hoc exceptional shifts every KEXSH iterations. */
/*     .    ==== */

/*     ==== The constants WILK1 and WILK2 are used to form the */
/*     .    exceptional shifts. ==== */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */
    /* Parameter adjustments */
    h_dim1 = *ldh;
    h_offset = 1 + h_dim1;
    h__ -= h_offset;
    --wr;
    --wi;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --work;

    /* Function Body */
    *info = 0;

/*     ==== Quick return for N = 0: nothing to do. ==== */

    if (*n == 0) {
	work[1] = 1.;
	return 0;
    }

    if (*n <= 11) {
/*        ==== Tiny matrices must use DLAHQR. ==== */

	lwkopt = 1;
	if (*lwork != -1) {
	    dlahqr_(wantt, wantz, n, ilo, ihi, &h__[h_offset], ldh, &wr[1], &
		    wi[1], iloz, ihiz, &z__[z_offset], ldz, info);
	}
    } else {

/*        ==== Use small bulge multi-shift QR with aggressive early */
/*        .    deflation on larger-than-tiny matrices. ==== */

/*        ==== Hope for the best. ==== */

	*info = 0;

/*        ==== Set up job flags for ILAENV. ==== */

	if (*wantt) {
	    * (unsigned char *) & jbcmpz [0] = 'S';
	} else {
	    * (unsigned char *) & jbcmpz [0] = 'E';
	}
	if (*wantz) {
	    * (unsigned char *) & jbcmpz [1] = 'V';
	} else {
	    * (unsigned char *) & jbcmpz [1] = 'N';
	}
	jbcmpz [2] = '\0';
/*        ==== NWR = recommended deflation window size.  At this */
/*        .    point,  N .GT. NTINY = 11, so there is enough */
/*        .    subdiagonal workspace for NWR.GE.2 as required. */
/*        .    (In fact, there is enough subdiagonal space for */
/*        .    NWR.GE.3.) ==== */

	nwr = ilaenv_(&c__13, "DLAQR4", jbcmpz, n, ilo, ihi, lwork);
	nwr = std::max(2_integer,nwr);
/* Computing MIN */
	i__1 = *ihi - *ilo + 1, i__2 = (*n - 1) / 3, i__1 = std::min(i__1,i__2);
	nwr = std::min(i__1,nwr);

/*        ==== NSR = recommended number of simultaneous shifts. */
/*        .    At this point N .GT. NTINY = 11, so there is at */
/*        .    enough subdiagonal workspace for NSR to be even */
/*        .    and greater than or equal to two as required. ==== */

	nsr = ilaenv_(&c__15, "DLAQR4", jbcmpz, n, ilo, ihi, lwork);
/* Computing MIN */
	i__1 = nsr, i__2 = (*n + 6) / 9, i__1 = std::min(i__1,i__2), i__2 = *ihi -
		*ilo;
	nsr = std::min(i__1,i__2);
/* Computing MAX */
	i__1 = 2, i__2 = nsr - nsr % 2;
	nsr = std::max(i__1,i__2);

/*        ==== Estimate optimal workspace ==== */

/*        ==== Workspace query call to DLAQR2 ==== */

	i__1 = nwr + 1;
	dlaqr2_(wantt, wantz, n, ilo, ihi, &i__1, &h__[h_offset], ldh, iloz,
		ihiz, &z__[z_offset], ldz, &ls, &ld, &wr[1], &wi[1], &h__[
		h_offset], ldh, n, &h__[h_offset], ldh, n, &h__[h_offset],
		ldh, &work[1], &c_n1);

/*        ==== Optimal workspace = MAX(DLAQR5, DLAQR2) ==== */

/* Computing MAX */
	i__1 = nsr * 3 / 2, i__2 = (integer) work[1];
	lwkopt = std::max(i__1,i__2);

/*        ==== Quick return in case of workspace query. ==== */

	if (*lwork == -1) {
	    work[1] = (double) lwkopt;
	    return 0;
	}

/*        ==== DLAHQR/DLAQR0 crossover point ==== */

	nmin = ilaenv_(&c__12, "DLAQR4", jbcmpz, n, ilo, ihi, lwork);
	nmin = std::max(11_integer,nmin);

/*        ==== Nibble crossover point ==== */

	nibble = ilaenv_(&c__14, "DLAQR4", jbcmpz, n, ilo, ihi, lwork);
	nibble = std::max(0_integer,nibble);

/*        ==== Accumulate reflections during ttswp?  Use block */
/*        .    2-by-2 structure during matrix-matrix multiply? ==== */

	kacc22 = ilaenv_(&c__16, "DLAQR4", jbcmpz, n, ilo, ihi, lwork);
	kacc22 = std::max(0_integer,kacc22);
	kacc22 = std::min(2_integer,kacc22);

/*        ==== NWMAX = the largest possible deflation window for */
/*        .    which there is sufficient workspace. ==== */

/* Computing MIN */
	i__1 = (*n - 1) / 3, i__2 = *lwork / 2;
	nwmax = std::min(i__1,i__2);
	nw = nwmax;

/*        ==== NSMAX = the Largest number of simultaneous shifts */
/*        .    for which there is sufficient workspace. ==== */

/* Computing MIN */
	i__1 = (*n + 6) / 9, i__2 = (*lwork << 1) / 3;
	nsmax = std::min(i__1,i__2);
	nsmax -= nsmax % 2;

/*        ==== NDFL: an iteration count restarted at deflation. ==== */

	ndfl = 1;

/*        ==== ITMAX = iteration limit ==== */

/* Computing MAX */
	i__1 = 10, i__2 = *ihi - *ilo + 1;
	itmax = std::max(i__1,i__2) * 30;

/*        ==== Last row and column in the active block ==== */

	kbot = *ihi;

/*        ==== Main Loop ==== */

	i__1 = itmax;
	for (it = 1; it <= i__1; ++it) {

/*           ==== Done when KBOT falls below ILO ==== */

	    if (kbot < *ilo) {
		goto L90;
	    }

/*           ==== Locate active block ==== */

	    i__2 = *ilo + 1;
	    for (k = kbot; k >= i__2; --k) {
		if (h__[k + (k - 1) * h_dim1] == 0.) {
		    goto L20;
		}
/* L10: */
	    }
	    k = *ilo;
L20:
	    ktop = k;

/*           ==== Select deflation window size: */
/*           .    Typical Case: */
/*           .      If possible and advisable, nibble the entire */
/*           .      active block.  If not, use size MIN(NWR,NWMAX) */
/*           .      or MIN(NWR+1,NWMAX) depending upon which has */
/*           .      the smaller corresponding subdiagonal entry */
/*           .      (a heuristic). */
/*           . */
/*           .    Exceptional Case: */
/*           .      If there have been no deflations in KEXNW or */
/*           .      more iterations, then vary the deflation window */
/*           .      size.   At first, because, larger windows are, */
/*           .      in general, more powerful than smaller ones, */
/*           .      rapidly increase the window to the maximum possible. */
/*           .      Then, gradually reduce the window size. ==== */

	    nh = kbot - ktop + 1;
		nwupbd = std::min(nh,nwmax);
	    if (ndfl < 5) {
			nw = std::min(nwupbd,nwr);
		} else {
/* Computing MIN */
		i__2 = nwupbd, i__3 = nw << 1;
		nw = std::min(i__2,i__3);
		}
		if (nw < nwmax) {
		if (nw >= nh - 1) {
			nw = nh;
		} else {
			kwtop = kbot - nw + 1;
			if ((d__1 = h__[kwtop + (kwtop - 1) * h_dim1], abs(d__1))
				> (d__2 = h__[kwtop - 1 + (kwtop - 2) * h_dim1],
				abs(d__2))) {
			++nw;
			}
		}
		}
	    if (ndfl < 5) {
		ndec = -1;
	    } else if (ndec >= 0 || nw >= nwupbd) {
		++ndec;
		if (nw - ndec < 2) {
		    ndec = 0;
		}
		nw -= ndec;
		}

/*           ==== Aggressive early deflation: */
/*           .    split workspace under the subdiagonal into */
/*           .      - an nw-by-nw work array V in the lower */
/*           .        left-hand-corner, */
/*           .      - an NW-by-at-least-NW-but-more-is-better */
/*           .        (NW-by-NHO) horizontal work array along */
/*           .        the bottom edge, */
/*           .      - an at-least-NW-but-more-is-better (NHV-by-NW) */
/*           .        vertical work array along the left-hand-edge. */
/*           .        ==== */

	    kv = *n - nw + 1;
	    kt = nw + 1;
	    nho = *n - nw - 1 - kt + 1;
	    kwv = nw + 2;
	    nve = *n - nw - kwv + 1;

/*           ==== Aggressive early deflation ==== */

	    dlaqr2_(wantt, wantz, n, &ktop, &kbot, &nw, &h__[h_offset], ldh,
		    iloz, ihiz, &z__[z_offset], ldz, &ls, &ld, &wr[1], &wi[1],
		     &h__[kv + h_dim1], ldh, &nho, &h__[kv + kt * h_dim1],
		    ldh, &nve, &h__[kwv + h_dim1], ldh, &work[1], lwork);

/*           ==== Adjust KBOT accounting for new deflations. ==== */

	    kbot -= ld;

/*           ==== KS points to the shifts. ==== */

	    ks = kbot - ls + 1;

/*           ==== Skip an expensive QR sweep if there is a (partly */
/*           .    heuristic) reason to expect that many eigenvalues */
/*           .    will deflate without it.  Here, the QR sweep is */
/*           .    skipped if many eigenvalues have just been deflated */
/*           .    or if the remaining active block is small. */

	    if (ld == 0 || ld * 100 <= nw * nibble && kbot - ktop + 1 > std::min(
		    nmin,nwmax)) {

/*              ==== NS = nominal number of simultaneous shifts. */
/*              .    This may be lowered (slightly) if DLAQR2 */
/*              .    did not provide that many shifts. ==== */

/* Computing MIN */
/* Computing MAX */
		i__4 = 2, i__5 = kbot - ktop;
		i__2 = std::min(nsmax,nsr), i__3 = std::max(i__4,i__5);
		ns = std::min(i__2,i__3);
		ns -= ns % 2;

/*              ==== If there have been no deflations */
/*              .    in a multiple of KEXSH iterations, */
/*              .    then try exceptional shifts. */
/*              .    Otherwise use shifts provided by */
/*              .    DLAQR2 above or from the eigenvalues */
/*              .    of a trailing principal submatrix. ==== */

		if (ndfl % 6 == 0) {
		    ks = kbot - ns + 1;
/* Computing MAX */
		    i__3 = ks + 1, i__4 = ktop + 2;
		    i__2 = std::max(i__3,i__4);
		    for (i__ = kbot; i__ >= i__2; i__ += -2) {
			ss = (d__1 = h__[i__ + (i__ - 1) * h_dim1], abs(d__1))
				 + (d__2 = h__[i__ - 1 + (i__ - 2) * h_dim1],
				abs(d__2));
			aa = ss * .75 + h__[i__ + i__ * h_dim1];
			bb = ss;
			cc = ss * -.4375;
			dd = aa;
			dlanv2_(&aa, &bb, &cc, &dd, &wr[i__ - 1], &wi[i__ - 1]
, &wr[i__], &wi[i__], &cs, &sn);
/* L30: */
		    }
		    if (ks == ktop) {
			wr[ks + 1] = h__[ks + 1 + (ks + 1) * h_dim1];
			wi[ks + 1] = 0.;
			wr[ks] = wr[ks + 1];
			wi[ks] = wi[ks + 1];
		    }
		} else {

/*                 ==== Got NS/2 or fewer shifts? Use DLAHQR */
/*                 .    on a trailing principal submatrix to */
/*                 .    get more. (Since NS.LE.NSMAX.LE.(N+6)/9, */
/*                 .    there is enough space below the subdiagonal */
/*                 .    to fit an NS-by-NS scratch array.) ==== */

		    if (kbot - ks + 1 <= ns / 2) {
			ks = kbot - ns + 1;
			kt = *n - ns + 1;
			dlacpy_("A", &ns, &ns, &h__[ks + ks * h_dim1], ldh, &
				h__[kt + h_dim1], ldh);
			dlahqr_(&c_false, &c_false, &ns, &c__1, &ns, &h__[kt
				+ h_dim1], ldh, &wr[ks], &wi[ks], &c__1, &
				c__1, zdum, &c__1, &inf);
			ks += inf;

/*                    ==== In case of a rare QR failure use */
/*                    .    eigenvalues of the trailing 2-by-2 */
/*                    .    principal submatrix.  ==== */

			if (ks >= kbot) {
			    aa = h__[kbot - 1 + (kbot - 1) * h_dim1];
			    cc = h__[kbot + (kbot - 1) * h_dim1];
			    bb = h__[kbot - 1 + kbot * h_dim1];
			    dd = h__[kbot + kbot * h_dim1];
			    dlanv2_(&aa, &bb, &cc, &dd, &wr[kbot - 1], &wi[
				    kbot - 1], &wr[kbot], &wi[kbot], &cs, &sn)
				    ;
			    ks = kbot - 1;
			}
		    }

		    if (kbot - ks + 1 > ns) {

/*                    ==== Sort the shifts (Helps a little) */
/*                    .    Bubble sort keeps complex conjugate */
/*                    .    pairs together. ==== */

			sorted = false;
			i__2 = ks + 1;
			for (k = kbot; k >= i__2; --k) {
			    if (sorted) {
				goto L60;
			    }
			    sorted = true;
			    i__3 = k - 1;
			    for (i__ = ks; i__ <= i__3; ++i__) {
				if ((d__1 = wr[i__], abs(d__1)) + (d__2 = wi[
					i__], abs(d__2)) < (d__3 = wr[i__ + 1]
					, abs(d__3)) + (d__4 = wi[i__ + 1],
					abs(d__4))) {
				    sorted = false;

				    swap = wr[i__];
				    wr[i__] = wr[i__ + 1];
				    wr[i__ + 1] = swap;

				    swap = wi[i__];
				    wi[i__] = wi[i__ + 1];
				    wi[i__ + 1] = swap;
				}
/* L40: */
			    }
/* L50: */
			}
L60:
			;
		    }

/*                 ==== Shuffle shifts into pairs of real shifts */
/*                 .    and pairs of complex conjugate shifts */
/*                 .    assuming complex conjugate shifts are */
/*                 .    already adjacent to one another. (Yes, */
/*                 .    they are.)  ==== */

		    i__2 = ks + 2;
		    for (i__ = kbot; i__ >= i__2; i__ += -2) {
			if (wi[i__] != -wi[i__ - 1]) {

			    swap = wr[i__];
			    wr[i__] = wr[i__ - 1];
			    wr[i__ - 1] = wr[i__ - 2];
			    wr[i__ - 2] = swap;

			    swap = wi[i__];
			    wi[i__] = wi[i__ - 1];
			    wi[i__ - 1] = wi[i__ - 2];
			    wi[i__ - 2] = swap;
			}
/* L70: */
		    }
		}

/*              ==== If there are only two shifts and both are */
/*              .    real, then use only one.  ==== */

		if (kbot - ks + 1 == 2) {
		    if (wi[kbot] == 0.) {
			if ((d__1 = wr[kbot] - h__[kbot + kbot * h_dim1], abs(
				d__1)) < (d__2 = wr[kbot - 1] - h__[kbot +
				kbot * h_dim1], abs(d__2))) {
			    wr[kbot - 1] = wr[kbot];
			} else {
			    wr[kbot] = wr[kbot - 1];
			}
		    }
		}

/*              ==== Use up to NS of the the smallest magnatiude */
/*              .    shifts.  If there aren't NS shifts available, */
/*              .    then use them all, possibly dropping one to */
/*              .    make the number of shifts even. ==== */

/* Computing MIN */
		i__2 = ns, i__3 = kbot - ks + 1;
		ns = std::min(i__2,i__3);
		ns -= ns % 2;
		ks = kbot - ns + 1;

/*              ==== Small-bulge multi-shift QR sweep: */
/*              .    split workspace under the subdiagonal into */
/*              .    - a KDU-by-KDU work array U in the lower */
/*              .      left-hand-corner, */
/*              .    - a KDU-by-at-least-KDU-but-more-is-better */
/*              .      (KDU-by-NHo) horizontal work array WH along */
/*              .      the bottom edge, */
/*              .    - and an at-least-KDU-but-more-is-better-by-KDU */
/*              .      (NVE-by-KDU) vertical work WV arrow along */
/*              .      the left-hand-edge. ==== */

		kdu = ns * 3 - 3;
		ku = *n - kdu + 1;
		kwh = kdu + 1;
		nho = *n - kdu - 3 - (kdu + 1) + 1;
		kwv = kdu + 4;
		nve = *n - kdu - kwv + 1;

/*              ==== Small-bulge multi-shift QR sweep ==== */

		dlaqr5_(wantt, wantz, &kacc22, n, &ktop, &kbot, &ns, &wr[ks],
			&wi[ks], &h__[h_offset], ldh, iloz, ihiz, &z__[
			z_offset], ldz, &work[1], &c__3, &h__[ku + h_dim1],
			ldh, &nve, &h__[kwv + h_dim1], ldh, &nho, &h__[ku +
			kwh * h_dim1], ldh);
	    }

/*           ==== Note progress (or the lack of it). ==== */

	    if (ld > 0) {
		ndfl = 1;
	    } else {
		++ndfl;
	    }

/*           ==== End of main loop ==== */
/* L80: */
	}

/*        ==== Iteration limit exceeded.  Set INFO to show where */
/*        .    the problem occurred and exit. ==== */

	*info = kbot;
L90:
	;
    }

/*     ==== Return the optimal value of LWORK. ==== */

    work[1] = (double) lwkopt;

/*     ==== End of DLAQR4 ==== */

    return 0;
} /* dlaqr4_ */

/* Subroutine */ int dlaqr5_(bool *wantt, bool *wantz, integer *kacc22,
	integer *n, integer *ktop, integer *kbot, integer *nshfts, double
	*sr, double *si, double *h__, integer *ldh, integer *iloz,
	integer *ihiz, double *z__, integer *ldz, double *v, integer *
	ldv, double *u, integer *ldu, integer *nv, double *wv,
	integer *ldwv, integer *nh, double *wh, integer *ldwh)
{
	/* Table of constant values */
	static double c_b7 = 0.;
	static double c_b8 = 1.;
	static integer c__3 = 3;
	static integer c__1 = 1;
	static integer c__2 = 2;

    /* System generated locals */
    integer h_dim1, h_offset, u_dim1, u_offset, v_dim1, v_offset, wh_dim1,
	    wh_offset, wv_dim1, wv_offset, z_dim1, z_offset, i__1, i__2, i__3,
	     i__4, i__5, i__6, i__7;
    double d__1, d__2, d__3, d__4, d__5;

    /* Local variables */
    integer i__, j, k, m, i2, j2, i4, j4, k1;
    double h11, h12, h21, h22;
    integer m22, ns, nu;
    double vt[3], scl;
    integer kdu, kms;
    double ulp;
    integer knz, kzs;
    double tst1, tst2, beta;
    bool blk22, bmp22, accum;
    integer mend, jcol, jlen, jbot, mbot, jtop, jrow, mtop;
    integer ndcol, incol, krcol, nbmps;
    double swap, alpha, safmin, safmax, refsum, smlnum;
    integer mstart;


/*  -- LAPACK auxiliary routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*     This auxiliary subroutine called by DLAQR0 performs a */
/*     single small-bulge multi-shift QR sweep. */

/*      WANTT  (input) logical scalar */
/*             WANTT = .true. if the quasi-triangular Schur factor */
/*             is being computed.  WANTT is set to .false. otherwise. */

/*      WANTZ  (input) logical scalar */
/*             WANTZ = .true. if the orthogonal Schur factor is being */
/*             computed.  WANTZ is set to .false. otherwise. */

/*      KACC22 (input) integer with value 0, 1, or 2. */
/*             Specifies the computation mode of far-from-diagonal */
/*             orthogonal updates. */
/*        = 0: DLAQR5 does not accumulate reflections and does not */
/*             use matrix-matrix multiply to update far-from-diagonal */
/*             matrix entries. */
/*        = 1: DLAQR5 accumulates reflections and uses matrix-matrix */
/*             multiply to update the far-from-diagonal matrix entries. */
/*        = 2: DLAQR5 accumulates reflections, uses matrix-matrix */
/*             multiply to update the far-from-diagonal matrix entries, */
/*             and takes advantage of 2-by-2 block structure during */
/*             matrix multiplies. */

/*      N      (input) integer scalar */
/*             N is the order of the Hessenberg matrix H upon which this */
/*             subroutine operates. */

/*      KTOP   (input) integer scalar */
/*      KBOT   (input) integer scalar */
/*             These are the first and last rows and columns of an */
/*             isolated diagonal block upon which the QR sweep is to be */
/*             applied. It is assumed without a check that */
/*                       either KTOP = 1  or   H(KTOP,KTOP-1) = 0 */
/*             and */
/*                       either KBOT = N  or   H(KBOT+1,KBOT) = 0. */

/*      NSHFTS (input) integer scalar */
/*             NSHFTS gives the number of simultaneous shifts.  NSHFTS */
/*             must be positive and even. */

/*      SR     (input/output) DOUBLE PRECISION array of size (NSHFTS) */
/*      SI     (input/output) DOUBLE PRECISION array of size (NSHFTS) */
/*             SR contains the real parts and SI contains the imaginary */
/*             parts of the NSHFTS shifts of origin that define the */
/*             multi-shift QR sweep.  On output SR and SI may be */
/*             reordered. */

/*      H      (input/output) DOUBLE PRECISION array of size (LDH,N) */
/*             On input H contains a Hessenberg matrix.  On output a */
/*             multi-shift QR sweep with shifts SR(J)+i*SI(J) is applied */
/*             to the isolated diagonal block in rows and columns KTOP */
/*             through KBOT. */

/*      LDH    (input) integer scalar */
/*             LDH is the leading dimension of H just as declared in the */
/*             calling procedure.  LDH.GE.MAX(1,N). */

/*      ILOZ   (input) INTEGER */
/*      IHIZ   (input) INTEGER */
/*             Specify the rows of Z to which transformations must be */
/*             applied if WANTZ is .TRUE.. 1 .LE. ILOZ .LE. IHIZ .LE. N */

/*      Z      (input/output) DOUBLE PRECISION array of size (LDZ,IHI) */
/*             If WANTZ = .TRUE., then the QR Sweep orthogonal */
/*             similarity transformation is accumulated into */
/*             Z(ILOZ:IHIZ,ILO:IHI) from the right. */
/*             If WANTZ = .FALSE., then Z is unreferenced. */

/*      LDZ    (input) integer scalar */
/*             LDA is the leading dimension of Z just as declared in */
/*             the calling procedure. LDZ.GE.N. */

/*      V      (workspace) DOUBLE PRECISION array of size (LDV,NSHFTS/2) */

/*      LDV    (input) integer scalar */
/*             LDV is the leading dimension of V as declared in the */
/*             calling procedure.  LDV.GE.3. */

/*      U      (workspace) DOUBLE PRECISION array of size */
/*             (LDU,3*NSHFTS-3) */

/*      LDU    (input) integer scalar */
/*             LDU is the leading dimension of U just as declared in the */
/*             in the calling subroutine.  LDU.GE.3*NSHFTS-3. */

/*      NH     (input) integer scalar */
/*             NH is the number of columns in array WH available for */
/*             workspace. NH.GE.1. */

/*      WH     (workspace) DOUBLE PRECISION array of size (LDWH,NH) */

/*      LDWH   (input) integer scalar */
/*             Leading dimension of WH just as declared in the */
/*             calling procedure.  LDWH.GE.3*NSHFTS-3. */

/*      NV     (input) integer scalar */
/*             NV is the number of rows in WV agailable for workspace. */
/*             NV.GE.1. */

/*      WV     (workspace) DOUBLE PRECISION array of size */
/*             (LDWV,3*NSHFTS-3) */

/*      LDWV   (input) integer scalar */
/*             LDWV is the leading dimension of WV as declared in the */
/*             in the calling subroutine.  LDWV.GE.NV. */

/*     ================================================================ */
/*     Based on contributions by */
/*        Karen Braman and Ralph Byers, Department of Mathematics, */
/*        University of Kansas, USA */

/*     ================================================================ */
/*     Reference: */

/*     K. Braman, R. Byers and R. Mathias, The Multi-Shift QR */
/*     Algorithm Part I: Maintaining Well Focused Shifts, and */
/*     Level 3 Performance, SIAM Journal of Matrix Analysis, */
/*     volume 23, pages 929--947, 2002. */

/*     ================================================================ */
/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */

/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Executable Statements .. */

/*     ==== If there are no shifts, then there is nothing to do. ==== */

    /* Parameter adjustments */
    --sr;
    --si;
    h_dim1 = *ldh;
    h_offset = 1 + h_dim1;
    h__ -= h_offset;
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    wv_dim1 = *ldwv;
    wv_offset = 1 + wv_dim1;
    wv -= wv_offset;
    wh_dim1 = *ldwh;
    wh_offset = 1 + wh_dim1;
    wh -= wh_offset;

    /* Function Body */
    if (*nshfts < 2) {
	return 0;
    }

/*     ==== If the active block is empty or 1-by-1, then there */
/*     .    is nothing to do. ==== */

    if (*ktop >= *kbot) {
	return 0;
    }

/*     ==== Shuffle shifts into pairs of real shifts and pairs */
/*     .    of complex conjugate shifts assuming complex */
/*     .    conjugate shifts are already adjacent to one */
/*     .    another. ==== */

    i__1 = *nshfts - 2;
    for (i__ = 1; i__ <= i__1; i__ += 2) {
	if (si[i__] != -si[i__ + 1]) {

	    swap = sr[i__];
	    sr[i__] = sr[i__ + 1];
	    sr[i__ + 1] = sr[i__ + 2];
	    sr[i__ + 2] = swap;

	    swap = si[i__];
	    si[i__] = si[i__ + 1];
	    si[i__ + 1] = si[i__ + 2];
	    si[i__ + 2] = swap;
	}
/* L10: */
    }

/*     ==== NSHFTS is supposed to be even, but if it is odd, */
/*     .    then simply reduce it by one.  The shuffle above */
/*     .    ensures that the dropped shift is real and that */
/*     .    the remaining shifts are paired. ==== */

    ns = *nshfts - *nshfts % 2;

/*     ==== Machine constants for deflation ==== */

    safmin = dlamch_("SAFE MINIMUM");
    safmax = 1. / safmin;
    dlabad_(&safmin, &safmax);
    ulp = dlamch_("PRECISION");
    smlnum = safmin * ((double) (*n) / ulp);

/*     ==== Use accumulated reflections to update far-from-diagonal */
/*     .    entries ? ==== */

    accum = *kacc22 == 1 || *kacc22 == 2;

/*     ==== If so, exploit the 2-by-2 block structure? ==== */

    blk22 = ns > 2 && *kacc22 == 2;

/*     ==== clear trash ==== */

    if (*ktop + 2 <= *kbot) {
	h__[*ktop + 2 + *ktop * h_dim1] = 0.;
    }

/*     ==== NBMPS = number of 2-shift bulges in the chain ==== */

    nbmps = ns / 2;

/*     ==== KDU = width of slab ==== */

    kdu = nbmps * 6 - 3;

/*     ==== Create and chase chains of NBMPS bulges ==== */

    i__1 = *kbot - 2;
    i__2 = nbmps * 3 - 2;
    for (incol = (1 - nbmps) * 3 + *ktop - 1; i__2 < 0 ? incol >= i__1 :
	    incol <= i__1; incol += i__2) {
	ndcol = incol + kdu;
	if (accum) {
	    dlaset_("ALL", &kdu, &kdu, &c_b7, &c_b8, &u[u_offset], ldu);
	}

/*        ==== Near-the-diagonal bulge chase.  The following loop */
/*        .    performs the near-the-diagonal part of a small bulge */
/*        .    multi-shift QR sweep.  Each 6*NBMPS-2 column diagonal */
/*        .    chunk extends from column INCOL to column NDCOL */
/*        .    (including both column INCOL and column NDCOL). The */
/*        .    following loop chases a 3*NBMPS column long chain of */
/*        .    NBMPS bulges 3*NBMPS-2 columns to the right.  (INCOL */
/*        .    may be less than KTOP and and NDCOL may be greater than */
/*        .    KBOT indicating phantom columns from which to chase */
/*        .    bulges before they are actually introduced or to which */
/*        .    to chase bulges beyond column KBOT.)  ==== */

/* Computing MIN */
	i__4 = incol + nbmps * 3 - 3, i__5 = *kbot - 2;
	i__3 = std::min(i__4,i__5);
	for (krcol = incol; krcol <= i__3; ++krcol) {

/*           ==== Bulges number MTOP to MBOT are active double implicit */
/*           .    shift bulges.  There may or may not also be small */
/*           .    2-by-2 bulge, if there is room.  The inactive bulges */
/*           .    (if any) must wait until the active bulges have moved */
/*           .    down the diagonal to make room.  The phantom matrix */
/*           .    paradigm described above helps keep track.  ==== */

/* Computing MAX */
	    i__4 = 1, i__5 = (*ktop - 1 - krcol + 2) / 3 + 1;
	    mtop = std::max(i__4,i__5);
/* Computing MIN */
	    i__4 = nbmps, i__5 = (*kbot - krcol) / 3;
	    mbot = std::min(i__4,i__5);
	    m22 = mbot + 1;
	    bmp22 = mbot < nbmps && krcol + (m22 - 1) * 3 == *kbot - 2;

/*           ==== Generate reflections to chase the chain right */
/*           .    one column.  (The minimum value of K is KTOP-1.) ==== */

	    i__4 = mbot;
	    for (m = mtop; m <= i__4; ++m) {
		k = krcol + (m - 1) * 3;
		if (k == *ktop - 1) {
		    dlaqr1_(&c__3, &h__[*ktop + *ktop * h_dim1], ldh, &sr[(m
			    << 1) - 1], &si[(m << 1) - 1], &sr[m * 2], &si[m *
			     2], &v[m * v_dim1 + 1]);
		    alpha = v[m * v_dim1 + 1];
		    dlarfg_(&c__3, &alpha, &v[m * v_dim1 + 2], &c__1, &v[m *
			    v_dim1 + 1]);
		} else {
		    beta = h__[k + 1 + k * h_dim1];
		    v[m * v_dim1 + 2] = h__[k + 2 + k * h_dim1];
		    v[m * v_dim1 + 3] = h__[k + 3 + k * h_dim1];
		    dlarfg_(&c__3, &beta, &v[m * v_dim1 + 2], &c__1, &v[m *
			    v_dim1 + 1]);

/*                 ==== A Bulge may collapse because of vigilant */
/*                 .    deflation or destructive underflow.  In the */
/*                 .    underflow case, try the two-small-subdiagonals */
/*                 .    trick to try to reinflate the bulge.  ==== */

		    if (h__[k + 3 + k * h_dim1] != 0. || h__[k + 3 + (k + 1) *
			     h_dim1] != 0. || h__[k + 3 + (k + 2) * h_dim1] ==
			     0.) {

/*                    ==== Typical case: not collapsed (yet). ==== */

			h__[k + 1 + k * h_dim1] = beta;
			h__[k + 2 + k * h_dim1] = 0.;
			h__[k + 3 + k * h_dim1] = 0.;
		    } else {

/*                    ==== Atypical case: collapsed.  Attempt to */
/*                    .    reintroduce ignoring H(K+1,K) and H(K+2,K). */
/*                    .    If the fill resulting from the new */
/*                    .    reflector is too large, then abandon it. */
/*                    .    Otherwise, use the new one. ==== */

			dlaqr1_(&c__3, &h__[k + 1 + (k + 1) * h_dim1], ldh, &
				sr[(m << 1) - 1], &si[(m << 1) - 1], &sr[m *
				2], &si[m * 2], vt);
			alpha = vt[0];
			dlarfg_(&c__3, &alpha, &vt[1], &c__1, vt);
			refsum = vt[0] * (h__[k + 1 + k * h_dim1] + vt[1] *
				h__[k + 2 + k * h_dim1]);

			if ((d__1 = h__[k + 2 + k * h_dim1] - refsum * vt[1],
				abs(d__1)) + (d__2 = refsum * vt[2], abs(d__2)
				) > ulp * ((d__3 = h__[k + k * h_dim1], abs(
				d__3)) + (d__4 = h__[k + 1 + (k + 1) * h_dim1]
				, abs(d__4)) + (d__5 = h__[k + 2 + (k + 2) *
				h_dim1], abs(d__5)))) {

/*                       ==== Starting a new bulge here would */
/*                       .    create non-negligible fill.  Use */
/*                       .    the old one with trepidation. ==== */

			    h__[k + 1 + k * h_dim1] = beta;
			    h__[k + 2 + k * h_dim1] = 0.;
			    h__[k + 3 + k * h_dim1] = 0.;
			} else {

/*                       ==== Stating a new bulge here would */
/*                       .    create only negligible fill. */
/*                       .    Replace the old reflector with */
/*                       .    the new one. ==== */

			    h__[k + 1 + k * h_dim1] -= refsum;
			    h__[k + 2 + k * h_dim1] = 0.;
			    h__[k + 3 + k * h_dim1] = 0.;
			    v[m * v_dim1 + 1] = vt[0];
			    v[m * v_dim1 + 2] = vt[1];
			    v[m * v_dim1 + 3] = vt[2];
			}
		    }
		}
/* L20: */
	    }

/*           ==== Generate a 2-by-2 reflection, if needed. ==== */

	    k = krcol + (m22 - 1) * 3;
	    if (bmp22) {
		if (k == *ktop - 1) {
		    dlaqr1_(&c__2, &h__[k + 1 + (k + 1) * h_dim1], ldh, &sr[(
			    m22 << 1) - 1], &si[(m22 << 1) - 1], &sr[m22 * 2],
			     &si[m22 * 2], &v[m22 * v_dim1 + 1]);
		    beta = v[m22 * v_dim1 + 1];
		    dlarfg_(&c__2, &beta, &v[m22 * v_dim1 + 2], &c__1, &v[m22
			    * v_dim1 + 1]);
		} else {
		    beta = h__[k + 1 + k * h_dim1];
		    v[m22 * v_dim1 + 2] = h__[k + 2 + k * h_dim1];
		    dlarfg_(&c__2, &beta, &v[m22 * v_dim1 + 2], &c__1, &v[m22
			    * v_dim1 + 1]);
		    h__[k + 1 + k * h_dim1] = beta;
		    h__[k + 2 + k * h_dim1] = 0.;
		}
	    }

/*           ==== Multiply H by reflections from the left ==== */

	    if (accum) {
		jbot = std::min(ndcol,*kbot);
	    } else if (*wantt) {
		jbot = *n;
	    } else {
		jbot = *kbot;
	    }
	    i__4 = jbot;
	    for (j = std::max(*ktop,krcol); j <= i__4; ++j) {
/* Computing MIN */
		i__5 = mbot, i__6 = (j - krcol + 2) / 3;
		mend = std::min(i__5,i__6);
		i__5 = mend;
		for (m = mtop; m <= i__5; ++m) {
		    k = krcol + (m - 1) * 3;
		    refsum = v[m * v_dim1 + 1] * (h__[k + 1 + j * h_dim1] + v[
			    m * v_dim1 + 2] * h__[k + 2 + j * h_dim1] + v[m *
			    v_dim1 + 3] * h__[k + 3 + j * h_dim1]);
		    h__[k + 1 + j * h_dim1] -= refsum;
		    h__[k + 2 + j * h_dim1] -= refsum * v[m * v_dim1 + 2];
		    h__[k + 3 + j * h_dim1] -= refsum * v[m * v_dim1 + 3];
/* L30: */
		}
/* L40: */
	    }
	    if (bmp22) {
		k = krcol + (m22 - 1) * 3;
/* Computing MAX */
		i__4 = k + 1;
		i__5 = jbot;
		for (j = std::max(i__4,*ktop); j <= i__5; ++j) {
		    refsum = v[m22 * v_dim1 + 1] * (h__[k + 1 + j * h_dim1] +
			    v[m22 * v_dim1 + 2] * h__[k + 2 + j * h_dim1]);
		    h__[k + 1 + j * h_dim1] -= refsum;
		    h__[k + 2 + j * h_dim1] -= refsum * v[m22 * v_dim1 + 2];
/* L50: */
		}
	    }

/*           ==== Multiply H by reflections from the right. */
/*           .    Delay filling in the last row until the */
/*           .    vigilant deflation check is complete. ==== */

	    if (accum) {
		jtop = std::max(*ktop,incol);
	    } else if (*wantt) {
		jtop = 1;
	    } else {
		jtop = *ktop;
	    }
	    i__5 = mbot;
	    for (m = mtop; m <= i__5; ++m) {
		if (v[m * v_dim1 + 1] != 0.) {
		    k = krcol + (m - 1) * 3;
/* Computing MIN */
		    i__6 = *kbot, i__7 = k + 3;
		    i__4 = std::min(i__6,i__7);
		    for (j = jtop; j <= i__4; ++j) {
			refsum = v[m * v_dim1 + 1] * (h__[j + (k + 1) *
				h_dim1] + v[m * v_dim1 + 2] * h__[j + (k + 2)
				* h_dim1] + v[m * v_dim1 + 3] * h__[j + (k +
				3) * h_dim1]);
			h__[j + (k + 1) * h_dim1] -= refsum;
			h__[j + (k + 2) * h_dim1] -= refsum * v[m * v_dim1 +
				2];
			h__[j + (k + 3) * h_dim1] -= refsum * v[m * v_dim1 +
				3];
/* L60: */
		    }

		    if (accum) {

/*                    ==== Accumulate U. (If necessary, update Z later */
/*                    .    with with an efficient matrix-matrix */
/*                    .    multiply.) ==== */

			kms = k - incol;
/* Computing MAX */
			i__4 = 1, i__6 = *ktop - incol;
			i__7 = kdu;
			for (j = std::max(i__4,i__6); j <= i__7; ++j) {
			    refsum = v[m * v_dim1 + 1] * (u[j + (kms + 1) *
				    u_dim1] + v[m * v_dim1 + 2] * u[j + (kms
				    + 2) * u_dim1] + v[m * v_dim1 + 3] * u[j
				    + (kms + 3) * u_dim1]);
			    u[j + (kms + 1) * u_dim1] -= refsum;
			    u[j + (kms + 2) * u_dim1] -= refsum * v[m *
				    v_dim1 + 2];
			    u[j + (kms + 3) * u_dim1] -= refsum * v[m *
				    v_dim1 + 3];
/* L70: */
			}
		    } else if (*wantz) {

/*                    ==== U is not accumulated, so update Z */
/*                    .    now by multiplying by reflections */
/*                    .    from the right. ==== */

			i__7 = *ihiz;
			for (j = *iloz; j <= i__7; ++j) {
			    refsum = v[m * v_dim1 + 1] * (z__[j + (k + 1) *
				    z_dim1] + v[m * v_dim1 + 2] * z__[j + (k
				    + 2) * z_dim1] + v[m * v_dim1 + 3] * z__[
				    j + (k + 3) * z_dim1]);
			    z__[j + (k + 1) * z_dim1] -= refsum;
			    z__[j + (k + 2) * z_dim1] -= refsum * v[m *
				    v_dim1 + 2];
			    z__[j + (k + 3) * z_dim1] -= refsum * v[m *
				    v_dim1 + 3];
/* L80: */
			}
		    }
		}
/* L90: */
	    }

/*           ==== Special case: 2-by-2 reflection (if needed) ==== */

	    k = krcol + (m22 - 1) * 3;
	    if (bmp22 && v[m22 * v_dim1 + 1] != 0.) {
/* Computing MIN */
		i__7 = *kbot, i__4 = k + 3;
		i__5 = std::min(i__7,i__4);
		for (j = jtop; j <= i__5; ++j) {
		    refsum = v[m22 * v_dim1 + 1] * (h__[j + (k + 1) * h_dim1]
			    + v[m22 * v_dim1 + 2] * h__[j + (k + 2) * h_dim1])
			    ;
		    h__[j + (k + 1) * h_dim1] -= refsum;
		    h__[j + (k + 2) * h_dim1] -= refsum * v[m22 * v_dim1 + 2];
/* L100: */
		}

		if (accum) {
		    kms = k - incol;
/* Computing MAX */
		    i__5 = 1, i__7 = *ktop - incol;
		    i__4 = kdu;
		    for (j = std::max(i__5,i__7); j <= i__4; ++j) {
			refsum = v[m22 * v_dim1 + 1] * (u[j + (kms + 1) *
				u_dim1] + v[m22 * v_dim1 + 2] * u[j + (kms +
				2) * u_dim1]);
			u[j + (kms + 1) * u_dim1] -= refsum;
			u[j + (kms + 2) * u_dim1] -= refsum * v[m22 * v_dim1
				+ 2];
/* L110: */
		    }
		} else if (*wantz) {
		    i__4 = *ihiz;
		    for (j = *iloz; j <= i__4; ++j) {
			refsum = v[m22 * v_dim1 + 1] * (z__[j + (k + 1) *
				z_dim1] + v[m22 * v_dim1 + 2] * z__[j + (k +
				2) * z_dim1]);
			z__[j + (k + 1) * z_dim1] -= refsum;
			z__[j + (k + 2) * z_dim1] -= refsum * v[m22 * v_dim1
				+ 2];
/* L120: */
		    }
		}
	    }

/*           ==== Vigilant deflation check ==== */

	    mstart = mtop;
	    if (krcol + (mstart - 1) * 3 < *ktop) {
		++mstart;
	    }
	    mend = mbot;
	    if (bmp22) {
		++mend;
	    }
	    if (krcol == *kbot - 2) {
		++mend;
	    }
	    i__4 = mend;
	    for (m = mstart; m <= i__4; ++m) {
/* Computing MIN */
		i__5 = *kbot - 1, i__7 = krcol + (m - 1) * 3;
		k = std::min(i__5,i__7);

/*              ==== The following convergence test requires that */
/*              .    the tradition small-compared-to-nearby-diagonals */
/*              .    criterion and the Ahues & Tisseur (LAWN 122, 1997) */
/*              .    criteria both be satisfied.  The latter improves */
/*              .    accuracy in some examples. Falling back on an */
/*              .    alternate convergence criterion when TST1 or TST2 */
/*              .    is zero (as done here) is traditional but probably */
/*              .    unnecessary. ==== */

		if (h__[k + 1 + k * h_dim1] != 0.) {
		    tst1 = (d__1 = h__[k + k * h_dim1], abs(d__1)) + (d__2 =
			    h__[k + 1 + (k + 1) * h_dim1], abs(d__2));
		    if (tst1 == 0.) {
			if (k >= *ktop + 1) {
			    tst1 += (d__1 = h__[k + (k - 1) * h_dim1], abs(
				    d__1));
			}
			if (k >= *ktop + 2) {
			    tst1 += (d__1 = h__[k + (k - 2) * h_dim1], abs(
				    d__1));
			}
			if (k >= *ktop + 3) {
			    tst1 += (d__1 = h__[k + (k - 3) * h_dim1], abs(
				    d__1));
			}
			if (k <= *kbot - 2) {
			    tst1 += (d__1 = h__[k + 2 + (k + 1) * h_dim1],
				    abs(d__1));
			}
			if (k <= *kbot - 3) {
			    tst1 += (d__1 = h__[k + 3 + (k + 1) * h_dim1],
				    abs(d__1));
			}
			if (k <= *kbot - 4) {
			    tst1 += (d__1 = h__[k + 4 + (k + 1) * h_dim1],
				    abs(d__1));
			}
		    }
/* Computing MAX */
		    d__2 = smlnum, d__3 = ulp * tst1;
		    if ((d__1 = h__[k + 1 + k * h_dim1], abs(d__1)) <= std::max(
			    d__2,d__3)) {
/* Computing MAX */
			d__3 = (d__1 = h__[k + 1 + k * h_dim1], abs(d__1)),
				d__4 = (d__2 = h__[k + (k + 1) * h_dim1], abs(
				d__2));
			h12 = std::max(d__3,d__4);
/* Computing MIN */
			d__3 = (d__1 = h__[k + 1 + k * h_dim1], abs(d__1)),
				d__4 = (d__2 = h__[k + (k + 1) * h_dim1], abs(
				d__2));
			h21 = std::min(d__3,d__4);
/* Computing MAX */
			d__3 = (d__1 = h__[k + 1 + (k + 1) * h_dim1], abs(
				d__1)), d__4 = (d__2 = h__[k + k * h_dim1] -
				h__[k + 1 + (k + 1) * h_dim1], abs(d__2));
			h11 = std::max(d__3,d__4);
/* Computing MIN */
			d__3 = (d__1 = h__[k + 1 + (k + 1) * h_dim1], abs(
				d__1)), d__4 = (d__2 = h__[k + k * h_dim1] -
				h__[k + 1 + (k + 1) * h_dim1], abs(d__2));
			h22 = std::min(d__3,d__4);
			scl = h11 + h12;
			tst2 = h22 * (h11 / scl);

/* Computing MAX */
			d__1 = smlnum, d__2 = ulp * tst2;
			if (tst2 == 0. || h21 * (h12 / scl) <= std::max(d__1,d__2))
				 {
			    h__[k + 1 + k * h_dim1] = 0.;
			}
		    }
		}
/* L130: */
	    }

/*           ==== Fill in the last row of each bulge. ==== */

/* Computing MIN */
	    i__4 = nbmps, i__5 = (*kbot - krcol - 1) / 3;
	    mend = std::min(i__4,i__5);
	    i__4 = mend;
	    for (m = mtop; m <= i__4; ++m) {
		k = krcol + (m - 1) * 3;
		refsum = v[m * v_dim1 + 1] * v[m * v_dim1 + 3] * h__[k + 4 + (
			k + 3) * h_dim1];
		h__[k + 4 + (k + 1) * h_dim1] = -refsum;
		h__[k + 4 + (k + 2) * h_dim1] = -refsum * v[m * v_dim1 + 2];
		h__[k + 4 + (k + 3) * h_dim1] -= refsum * v[m * v_dim1 + 3];
/* L140: */
	    }

/*           ==== End of near-the-diagonal bulge chase. ==== */

/* L150: */
	}

/*        ==== Use U (if accumulated) to update far-from-diagonal */
/*        .    entries in H.  If required, use U to update Z as */
/*        .    well. ==== */

	if (accum) {
	    if (*wantt) {
		jtop = 1;
		jbot = *n;
	    } else {
		jtop = *ktop;
		jbot = *kbot;
	    }
	    if (! blk22 || incol < *ktop || ndcol > *kbot || ns <= 2) {

/*              ==== Updates not exploiting the 2-by-2 block */
/*              .    structure of U.  K1 and NU keep track of */
/*              .    the location and size of U in the special */
/*              .    cases of introducing bulges and chasing */
/*              .    bulges off the bottom.  In these special */
/*              .    cases and in case the number of shifts */
/*              .    is NS = 2, there is no 2-by-2 block */
/*              .    structure to exploit.  ==== */

/* Computing MAX */
		i__3 = 1, i__4 = *ktop - incol;
		k1 = std::max(i__3,i__4);
/* Computing MAX */
		i__3 = 0, i__4 = ndcol - *kbot;
		nu = kdu - std::max(i__3,i__4) - k1 + 1;

/*              ==== Horizontal Multiply ==== */

		i__3 = jbot;
		i__4 = *nh;
		for (jcol = std::min(ndcol,*kbot) + 1; i__4 < 0 ? jcol >= i__3 :
			jcol <= i__3; jcol += i__4) {
/* Computing MIN */
		    i__5 = *nh, i__7 = jbot - jcol + 1;
		    jlen = std::min(i__5,i__7);
		    dgemm_("C", "N", &nu, &jlen, &nu, &c_b8, &u[k1 + k1 *
			    u_dim1], ldu, &h__[incol + k1 + jcol * h_dim1],
			    ldh, &c_b7, &wh[wh_offset], ldwh);
		    dlacpy_("ALL", &nu, &jlen, &wh[wh_offset], ldwh, &h__[
			    incol + k1 + jcol * h_dim1], ldh);
/* L160: */
		}

/*              ==== Vertical multiply ==== */

		i__4 = std::max(*ktop,incol) - 1;
		i__3 = *nv;
		for (jrow = jtop; i__3 < 0 ? jrow >= i__4 : jrow <= i__4;
			jrow += i__3) {
/* Computing MIN */
		    i__5 = *nv, i__7 = std::max(*ktop,incol) - jrow;
		    jlen = std::min(i__5,i__7);
		    dgemm_("N", "N", &jlen, &nu, &nu, &c_b8, &h__[jrow + (
			    incol + k1) * h_dim1], ldh, &u[k1 + k1 * u_dim1],
			    ldu, &c_b7, &wv[wv_offset], ldwv);
		    dlacpy_("ALL", &jlen, &nu, &wv[wv_offset], ldwv, &h__[
			    jrow + (incol + k1) * h_dim1], ldh);
/* L170: */
		}

/*              ==== Z multiply (also vertical) ==== */

		if (*wantz) {
		    i__3 = *ihiz;
		    i__4 = *nv;
		    for (jrow = *iloz; i__4 < 0 ? jrow >= i__3 : jrow <= i__3;
			     jrow += i__4) {
/* Computing MIN */
			i__5 = *nv, i__7 = *ihiz - jrow + 1;
			jlen = std::min(i__5,i__7);
			dgemm_("N", "N", &jlen, &nu, &nu, &c_b8, &z__[jrow + (
				incol + k1) * z_dim1], ldz, &u[k1 + k1 *
				u_dim1], ldu, &c_b7, &wv[wv_offset], ldwv);
			dlacpy_("ALL", &jlen, &nu, &wv[wv_offset], ldwv, &z__[
				jrow + (incol + k1) * z_dim1], ldz)
				;
/* L180: */
		    }
		}
	    } else {

/*              ==== Updates exploiting U's 2-by-2 block structure. */
/*              .    (I2, I4, J2, J4 are the last rows and columns */
/*              .    of the blocks.) ==== */

		i2 = (kdu + 1) / 2;
		i4 = kdu;
		j2 = i4 - i2;
		j4 = kdu;

/*              ==== KZS and KNZ deal with the band of zeros */
/*              .    along the diagonal of one of the triangular */
/*              .    blocks. ==== */

		kzs = j4 - j2 - (ns + 1);
		knz = ns + 1;

/*              ==== Horizontal multiply ==== */

		i__4 = jbot;
		i__3 = *nh;
		for (jcol = std::min(ndcol,*kbot) + 1; i__3 < 0 ? jcol >= i__4 :
			jcol <= i__4; jcol += i__3) {
/* Computing MIN */
		    i__5 = *nh, i__7 = jbot - jcol + 1;
		    jlen = std::min(i__5,i__7);

/*                 ==== Copy bottom of H to top+KZS of scratch ==== */
/*                  (The first KZS rows get multiplied by zero.) ==== */

		    dlacpy_("ALL", &knz, &jlen, &h__[incol + 1 + j2 + jcol *
			    h_dim1], ldh, &wh[kzs + 1 + wh_dim1], ldwh);

/*                 ==== Multiply by U21' ==== */

		    dlaset_("ALL", &kzs, &jlen, &c_b7, &c_b7, &wh[wh_offset],
			    ldwh);
		    dtrmm_("L", "U", "C", "N", &knz, &jlen, &c_b8, &u[j2 + 1
			    + (kzs + 1) * u_dim1], ldu, &wh[kzs + 1 + wh_dim1]
, ldwh);

/*                 ==== Multiply top of H by U11' ==== */

		    dgemm_("C", "N", &i2, &jlen, &j2, &c_b8, &u[u_offset],
			    ldu, &h__[incol + 1 + jcol * h_dim1], ldh, &c_b8,
			    &wh[wh_offset], ldwh);

/*                 ==== Copy top of H to bottom of WH ==== */

		    dlacpy_("ALL", &j2, &jlen, &h__[incol + 1 + jcol * h_dim1]
, ldh, &wh[i2 + 1 + wh_dim1], ldwh);

/*                 ==== Multiply by U21' ==== */

		    dtrmm_("L", "L", "C", "N", &j2, &jlen, &c_b8, &u[(i2 + 1)
			    * u_dim1 + 1], ldu, &wh[i2 + 1 + wh_dim1], ldwh);

/*                 ==== Multiply by U22 ==== */

		    i__5 = i4 - i2;
		    i__7 = j4 - j2;
		    dgemm_("C", "N", &i__5, &jlen, &i__7, &c_b8, &u[j2 + 1 + (
			    i2 + 1) * u_dim1], ldu, &h__[incol + 1 + j2 +
			    jcol * h_dim1], ldh, &c_b8, &wh[i2 + 1 + wh_dim1],
			     ldwh);

/*                 ==== Copy it back ==== */

		    dlacpy_("ALL", &kdu, &jlen, &wh[wh_offset], ldwh, &h__[
			    incol + 1 + jcol * h_dim1], ldh);
/* L190: */
		}

/*              ==== Vertical multiply ==== */

		i__3 = std::max(incol,*ktop) - 1;
		i__4 = *nv;
		for (jrow = jtop; i__4 < 0 ? jrow >= i__3 : jrow <= i__3;
			jrow += i__4) {
/* Computing MIN */
		    i__5 = *nv, i__7 = std::max(incol,*ktop) - jrow;
		    jlen = std::min(i__5,i__7);

/*                 ==== Copy right of H to scratch (the first KZS */
/*                 .    columns get multiplied by zero) ==== */

		    dlacpy_("ALL", &jlen, &knz, &h__[jrow + (incol + 1 + j2) *
			     h_dim1], ldh, &wv[(kzs + 1) * wv_dim1 + 1], ldwv);

/*                 ==== Multiply by U21 ==== */

		    dlaset_("ALL", &jlen, &kzs, &c_b7, &c_b7, &wv[wv_offset],
			    ldwv);
		    dtrmm_("R", "U", "N", "N", &jlen, &knz, &c_b8, &u[j2 + 1
			    + (kzs + 1) * u_dim1], ldu, &wv[(kzs + 1) *
			    wv_dim1 + 1], ldwv);

/*                 ==== Multiply by U11 ==== */

		    dgemm_("N", "N", &jlen, &i2, &j2, &c_b8, &h__[jrow + (
			    incol + 1) * h_dim1], ldh, &u[u_offset], ldu, &
			    c_b8, &wv[wv_offset], ldwv);

/*                 ==== Copy left of H to right of scratch ==== */

		    dlacpy_("ALL", &jlen, &j2, &h__[jrow + (incol + 1) *
			    h_dim1], ldh, &wv[(i2 + 1) * wv_dim1 + 1], ldwv);

/*                 ==== Multiply by U21 ==== */

		    i__5 = i4 - i2;
		    dtrmm_("R", "L", "N", "N", &jlen, &i__5, &c_b8, &u[(i2 +
			    1) * u_dim1 + 1], ldu, &wv[(i2 + 1) * wv_dim1 + 1]
, ldwv);

/*                 ==== Multiply by U22 ==== */

		    i__5 = i4 - i2;
		    i__7 = j4 - j2;
		    dgemm_("N", "N", &jlen, &i__5, &i__7, &c_b8, &h__[jrow + (
			    incol + 1 + j2) * h_dim1], ldh, &u[j2 + 1 + (i2 +
			    1) * u_dim1], ldu, &c_b8, &wv[(i2 + 1) * wv_dim1
			    + 1], ldwv);

/*                 ==== Copy it back ==== */

		    dlacpy_("ALL", &jlen, &kdu, &wv[wv_offset], ldwv, &h__[
			    jrow + (incol + 1) * h_dim1], ldh);
/* L200: */
		}

/*              ==== Multiply Z (also vertical) ==== */

		if (*wantz) {
		    i__4 = *ihiz;
		    i__3 = *nv;
		    for (jrow = *iloz; i__3 < 0 ? jrow >= i__4 : jrow <= i__4;
			     jrow += i__3) {
/* Computing MIN */
			i__5 = *nv, i__7 = *ihiz - jrow + 1;
			jlen = std::min(i__5,i__7);

/*                    ==== Copy right of Z to left of scratch (first */
/*                    .     KZS columns get multiplied by zero) ==== */

			dlacpy_("ALL", &jlen, &knz, &z__[jrow + (incol + 1 +
				j2) * z_dim1], ldz, &wv[(kzs + 1) * wv_dim1 +
				1], ldwv);

/*                    ==== Multiply by U12 ==== */

			dlaset_("ALL", &jlen, &kzs, &c_b7, &c_b7, &wv[
				wv_offset], ldwv);
			dtrmm_("R", "U", "N", "N", &jlen, &knz, &c_b8, &u[j2
				+ 1 + (kzs + 1) * u_dim1], ldu, &wv[(kzs + 1)
				* wv_dim1 + 1], ldwv);

/*                    ==== Multiply by U11 ==== */

			dgemm_("N", "N", &jlen, &i2, &j2, &c_b8, &z__[jrow + (
				incol + 1) * z_dim1], ldz, &u[u_offset], ldu,
				&c_b8, &wv[wv_offset], ldwv);

/*                    ==== Copy left of Z to right of scratch ==== */

			dlacpy_("ALL", &jlen, &j2, &z__[jrow + (incol + 1) *
				z_dim1], ldz, &wv[(i2 + 1) * wv_dim1 + 1],
				ldwv);

/*                    ==== Multiply by U21 ==== */

			i__5 = i4 - i2;
			dtrmm_("R", "L", "N", "N", &jlen, &i__5, &c_b8, &u[(
				i2 + 1) * u_dim1 + 1], ldu, &wv[(i2 + 1) *
				wv_dim1 + 1], ldwv);

/*                    ==== Multiply by U22 ==== */

			i__5 = i4 - i2;
			i__7 = j4 - j2;
			dgemm_("N", "N", &jlen, &i__5, &i__7, &c_b8, &z__[
				jrow + (incol + 1 + j2) * z_dim1], ldz, &u[j2
				+ 1 + (i2 + 1) * u_dim1], ldu, &c_b8, &wv[(i2
				+ 1) * wv_dim1 + 1], ldwv);

/*                    ==== Copy the result back to Z ==== */

			dlacpy_("ALL", &jlen, &kdu, &wv[wv_offset], ldwv, &
				z__[jrow + (incol + 1) * z_dim1], ldz);
/* L210: */
		    }
		}
	    }
	}
/* L220: */
    }

/*     ==== End of DLAQR5 ==== */

    return 0;
} /* dlaqr5_ */

/* Subroutine */ int dlaqsb_(const char *uplo, integer *n, integer *kd, double *
	ab, integer *ldab, double *s, double *scond, double *amax,
	 const char *equed)
{
    /* System generated locals */
    integer ab_dim1, ab_offset, i__1, i__2, i__3, i__4;

    /* Local variables */
    integer i__, j;
    double cj, large;

    double small;



/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAQSB equilibrates a symmetric band matrix A using the scaling */
/*  factors in the vector S. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is stored. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  KD      (input) INTEGER */
/*          The number of super-diagonals of the matrix A if UPLO = 'U', */
/*          or the number of sub-diagonals if UPLO = 'L'.  KD >= 0. */

/*  AB      (input/output) DOUBLE PRECISION array, dimension (LDAB,N) */
/*          On entry, the upper or lower triangle of the symmetric band */
/*          matrix A, stored in the first KD+1 rows of the array.  The */
/*          j-th column of A is stored in the j-th column of the array AB */
/*          as follows: */
/*          if UPLO = 'U', AB(kd+1+i-j,j) = A(i,j) for max(1,j-kd)<=i<=j; */
/*          if UPLO = 'L', AB(1+i-j,j)    = A(i,j) for j<=i<=min(n,j+kd). */

/*          On exit, if INFO = 0, the triangular factor U or L from the */
/*          Cholesky factorization A = U'*U or A = L*L' of the band */
/*          matrix A, in the same storage format as A. */

/*  LDAB    (input) INTEGER */
/*          The leading dimension of the array AB.  LDAB >= KD+1. */

/*  S       (input) DOUBLE PRECISION array, dimension (N) */
/*          The scale factors for A. */

/*  SCOND   (input) DOUBLE PRECISION */
/*          Ratio of the smallest S(i) to the largest S(i). */

/*  AMAX    (input) DOUBLE PRECISION */
/*          Absolute value of largest matrix entry. */

/*  EQUED   (output) CHARACTER*1 */
/*          Specifies whether or not equilibration was done. */
/*          = 'N':  No equilibration. */
/*          = 'Y':  Equilibration was done, i.e., A has been replaced by */
/*                  diag(S) * A * diag(S). */

/*  Internal Parameters */
/*  =================== */

/*  THRESH is a threshold value used to decide if scaling should be done */
/*  based on the ratio of the scaling factors.  If SCOND < THRESH, */
/*  scaling is done. */

/*  LARGE and SMALL are threshold values used to decide if scaling should */
/*  be done based on the absolute size of the largest matrix element. */
/*  If AMAX > LARGE or AMAX < SMALL, scaling is done. */

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

/*     Quick return if possible */

    /* Parameter adjustments */
    ab_dim1 = *ldab;
    ab_offset = 1 + ab_dim1;
    ab -= ab_offset;
    --s;

    /* Function Body */
    if (*n <= 0) {
	*(unsigned char *)equed = 'N';
	return 0;
    }

/*     Initialize LARGE and SMALL. */

    small = dlamch_("Safe minimum") / dlamch_("Precision");
    large = 1. / small;

    if (*scond >= .1 && *amax >= small && *amax <= large) {

/*        No equilibration */

	*(unsigned char *)equed = 'N';
    } else {

/*        Replace A by diag(S) * A * diag(S). */

	if (lsame_(uplo, "U")) {

/*           Upper triangle of A is stored in band format. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		cj = s[j];
/* Computing MAX */
		i__2 = 1, i__3 = j - *kd;
		i__4 = j;
		for (i__ = std::max(i__2,i__3); i__ <= i__4; ++i__) {
		    ab[*kd + 1 + i__ - j + j * ab_dim1] = cj * s[i__] * ab[*
			    kd + 1 + i__ - j + j * ab_dim1];
/* L10: */
		}
/* L20: */
	    }
	} else {

/*           Lower triangle of A is stored. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		cj = s[j];
/* Computing MIN */
		i__2 = *n, i__3 = j + *kd;
		i__4 = std::min(i__2,i__3);
		for (i__ = j; i__ <= i__4; ++i__) {
		    ab[i__ + 1 - j + j * ab_dim1] = cj * s[i__] * ab[i__ + 1
			    - j + j * ab_dim1];
/* L30: */
		}
/* L40: */
	    }
	}
	*(unsigned char *)equed = 'Y';
    }

    return 0;

/*     End of DLAQSB */

} /* dlaqsb_ */

/* Subroutine */ int dlaqsp_(const char *uplo, integer *n, double *ap,
	double *s, double *scond, double *amax, char *equed)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Local variables */
    integer i__, j, jc;
    double cj, large;

    double small;



/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAQSP equilibrates a symmetric matrix A using the scaling factors */
/*  in the vector S. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is stored. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  AP      (input/output) DOUBLE PRECISION array, dimension (N*(N+1)/2) */
/*          On entry, the upper or lower triangle of the symmetric matrix */
/*          A, packed columnwise in a linear array.  The j-th column of A */
/*          is stored in the array AP as follows: */
/*          if UPLO = 'U', AP(i + (j-1)*j/2) = A(i,j) for 1<=i<=j; */
/*          if UPLO = 'L', AP(i + (j-1)*(2n-j)/2) = A(i,j) for j<=i<=n. */

/*          On exit, the equilibrated matrix:  diag(S) * A * diag(S), in */
/*          the same storage format as A. */

/*  S       (input) DOUBLE PRECISION array, dimension (N) */
/*          The scale factors for A. */

/*  SCOND   (input) DOUBLE PRECISION */
/*          Ratio of the smallest S(i) to the largest S(i). */

/*  AMAX    (input) DOUBLE PRECISION */
/*          Absolute value of largest matrix entry. */

/*  EQUED   (output) CHARACTER*1 */
/*          Specifies whether or not equilibration was done. */
/*          = 'N':  No equilibration. */
/*          = 'Y':  Equilibration was done, i.e., A has been replaced by */
/*                  diag(S) * A * diag(S). */

/*  Internal Parameters */
/*  =================== */

/*  THRESH is a threshold value used to decide if scaling should be done */
/*  based on the ratio of the scaling factors.  If SCOND < THRESH, */
/*  scaling is done. */

/*  LARGE and SMALL are threshold values used to decide if scaling should */
/*  be done based on the absolute size of the largest matrix element. */
/*  If AMAX > LARGE or AMAX < SMALL, scaling is done. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick return if possible */

    /* Parameter adjustments */
    --s;
    --ap;

    /* Function Body */
    if (*n <= 0) {
	*(unsigned char *)equed = 'N';
	return 0;
    }

/*     Initialize LARGE and SMALL. */

    small = dlamch_("Safe minimum") / dlamch_("Precision");
    large = 1. / small;

    if (*scond >= .1 && *amax >= small && *amax <= large) {

/*        No equilibration */

	*(unsigned char *)equed = 'N';
    } else {

/*        Replace A by diag(S) * A * diag(S). */

	if (lsame_(uplo, "U")) {

/*           Upper triangle of A is stored. */

	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		cj = s[j];
		i__2 = j;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    ap[jc + i__ - 1] = cj * s[i__] * ap[jc + i__ - 1];
/* L10: */
		}
		jc += j;
/* L20: */
	    }
	} else {

/*           Lower triangle of A is stored. */

	    jc = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		cj = s[j];
		i__2 = *n;
		for (i__ = j; i__ <= i__2; ++i__) {
		    ap[jc + i__ - j] = cj * s[i__] * ap[jc + i__ - j];
/* L30: */
		}
		jc = jc + *n - j + 1;
/* L40: */
	    }
	}
	*(unsigned char *)equed = 'Y';
    }

    return 0;

/*     End of DLAQSP */

} /* dlaqsp_ */

/* Subroutine */ int dlaqsy_(const char *uplo, integer *n, double *a, integer *
	lda, double *s, double *scond, double *amax, char *equed)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;

    /* Local variables */
    integer i__, j;
    double cj, large;

    double small;



/*  -- LAPACK auxiliary routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLAQSY equilibrates a symmetric matrix A using the scaling factors */
/*  in the vector S. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is stored. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the leading */
/*          n by n upper triangular part of A contains the upper */
/*          triangular part of the matrix A, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading n by n lower triangular part of A contains the lower */
/*          triangular part of the matrix A, and the strictly upper */
/*          triangular part of A is not referenced. */

/*          On exit, if EQUED = 'Y', the equilibrated matrix: */
/*          diag(S) * A * diag(S). */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(N,1). */

/*  S       (input) DOUBLE PRECISION array, dimension (N) */
/*          The scale factors for A. */

/*  SCOND   (input) DOUBLE PRECISION */
/*          Ratio of the smallest S(i) to the largest S(i). */

/*  AMAX    (input) DOUBLE PRECISION */
/*          Absolute value of largest matrix entry. */

/*  EQUED   (output) CHARACTER*1 */
/*          Specifies whether or not equilibration was done. */
/*          = 'N':  No equilibration. */
/*          = 'Y':  Equilibration was done, i.e., A has been replaced by */
/*                  diag(S) * A * diag(S). */

/*  Internal Parameters */
/*  =================== */

/*  THRESH is a threshold value used to decide if scaling should be done */
/*  based on the ratio of the scaling factors.  If SCOND < THRESH, */
/*  scaling is done. */

/*  LARGE and SMALL are threshold values used to decide if scaling should */
/*  be done based on the absolute size of the largest matrix element. */
/*  If AMAX > LARGE or AMAX < SMALL, scaling is done. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Executable Statements .. */

/*     Quick return if possible */

    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --s;

    /* Function Body */
    if (*n <= 0) {
	*(unsigned char *)equed = 'N';
	return 0;
    }

/*     Initialize LARGE and SMALL. */

    small = dlamch_("Safe minimum") / dlamch_("Precision");
    large = 1. / small;

    if (*scond >= .1 && *amax >= small && *amax <= large) {

/*        No equilibration */

	*(unsigned char *)equed = 'N';
    } else {

/*        Replace A by diag(S) * A * diag(S). */

	if (lsame_(uplo, "U")) {

/*           Upper triangle of A is stored. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		cj = s[j];
		i__2 = j;
		for (i__ = 1; i__ <= i__2; ++i__) {
		    a[i__ + j * a_dim1] = cj * s[i__] * a[i__ + j * a_dim1];
/* L10: */
		}
/* L20: */
	    }
	} else {

/*           Lower triangle of A is stored. */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		cj = s[j];
		i__2 = *n;
		for (i__ = j; i__ <= i__2; ++i__) {
		    a[i__ + j * a_dim1] = cj * s[i__] * a[i__ + j * a_dim1];
/* L30: */
		}
/* L40: */
	    }
	}
	*(unsigned char *)equed = 'Y';
    }

    return 0;

/*     End of DLAQSY */

} /* dlaqsy_ */

/* Subroutine */ int dlaqtr_(bool *ltran, bool *lreal, integer *n,
	double *t, integer *ldt, double *b, double *w, double
	*scale, double *x, double *work, integer *info)
{
	/* Table of constant values */
	static integer c__1 = 1;
	static bool c_false = false;
	static integer c__2 = 2;
	static double c_b21 = 1.;
	static double c_b25 = 0.;
	static bool c_true = true;

    /* System generated locals */
    integer t_dim1, t_offset, i__1, i__2;
    double d__1, d__2, d__3, d__4, d__5, d__6;

    /* Local variables */
    double d__[4]	/* was [2][2] */;
    integer i__, j, k;
    double v[4]	/* was [2][2] */, z__;
    integer j1, j2, n1, n2;
    double si, xj, sr, rec, eps, tjj, tmp;
    integer ierr;
    double smin, xmax;
    integer jnext;
    double sminw, xnorm;
    double scaloc;
    double bignum;
    bool notran;
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

/*  DLAQTR solves the real quasi-triangular system */

/*               op(T)*p = scale*c,               if LREAL = .TRUE. */

/*  or the complex quasi-triangular systems */

/*             op(T + iB)*(p+iq) = scale*(c+id),  if LREAL = .FALSE. */

/*  in real arithmetic, where T is upper quasi-triangular. */
/*  If LREAL = .FALSE., then the first diagonal block of T must be */
/*  1 by 1, B is the specially structured matrix */

/*                 B = [ b(1) b(2) ... b(n) ] */
/*                     [       w            ] */
/*                     [           w        ] */
/*                     [              .     ] */
/*                     [                 w  ] */

/*  op(A) = A or A', A' denotes the conjugate transpose of */
/*  matrix A. */

/*  On input, X = [ c ].  On output, X = [ p ]. */
/*                [ d ]                  [ q ] */

/*  This subroutine is designed for the condition number estimation */
/*  in routine DTRSNA. */

/*  Arguments */
/*  ========= */

/*  LTRAN   (input) LOGICAL */
/*          On entry, LTRAN specifies the option of conjugate transpose: */
/*             = .FALSE.,    op(T+i*B) = T+i*B, */
/*             = .TRUE.,     op(T+i*B) = (T+i*B)'. */

/*  LREAL   (input) LOGICAL */
/*          On entry, LREAL specifies the input matrix structure: */
/*             = .FALSE.,    the input is complex */
/*             = .TRUE.,     the input is real */

/*  N       (input) INTEGER */
/*          On entry, N specifies the order of T+i*B. N >= 0. */

/*  T       (input) DOUBLE PRECISION array, dimension (LDT,N) */
/*          On entry, T contains a matrix in Schur canonical form. */
/*          If LREAL = .FALSE., then the first diagonal block of T mu */
/*          be 1 by 1. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of the matrix T. LDT >= max(1,N). */

/*  B       (input) DOUBLE PRECISION array, dimension (N) */
/*          On entry, B contains the elements to form the matrix */
/*          B as described above. */
/*          If LREAL = .TRUE., B is not referenced. */

/*  W       (input) DOUBLE PRECISION */
/*          On entry, W is the diagonal element of the matrix B. */
/*          If LREAL = .TRUE., W is not referenced. */

/*  SCALE   (output) DOUBLE PRECISION */
/*          On exit, SCALE is the scale factor. */

/*  X       (input/output) DOUBLE PRECISION array, dimension (2*N) */
/*          On entry, X contains the right hand side of the system. */
/*          On exit, X is overwritten by the solution. */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          On exit, INFO is set to */
/*             0: successful exit. */
/*               1: the some diagonal 1 by 1 block has been perturbed by */
/*                  a small number SMIN to keep nonsingularity. */
/*               2: the some diagonal 2 by 2 block has been perturbed by */
/*                  a small number in DLALN2 to keep nonsingularity. */
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
/*     .. Executable Statements .. */

/*     Do not test the input parameters for errors */

    /* Parameter adjustments */
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    --b;
    --x;
    --work;

    /* Function Body */
    notran = ! (*ltran);
    *info = 0;

/*     Quick return if possible */

    if (*n == 0) {
	return 0;
    }

/*     Set constants to control overflow */

    eps = dlamch_("P");
    smlnum = dlamch_("S") / eps;
    bignum = 1. / smlnum;

    xnorm = dlange_("M", n, n, &t[t_offset], ldt, d__);
    if (! (*lreal)) {
/* Computing MAX */
	d__1 = xnorm, d__2 = abs(*w), d__1 = std::max(d__1,d__2), d__2 = dlange_(
		"M", n, &c__1, &b[1], n, d__);
	xnorm = std::max(d__1,d__2);
    }
/* Computing MAX */
    d__1 = smlnum, d__2 = eps * xnorm;
    smin = std::max(d__1,d__2);

/*     Compute 1-norm of each column of strictly upper triangular */
/*     part of T to control overflow in triangular solver. */

    work[1] = 0.;
    i__1 = *n;
    for (j = 2; j <= i__1; ++j) {
	i__2 = j - 1;
	work[j] = dasum_(&i__2, &t[j * t_dim1 + 1], &c__1);
/* L10: */
    }

    if (! (*lreal)) {
	i__1 = *n;
	for (i__ = 2; i__ <= i__1; ++i__) {
	    work[i__] += (d__1 = b[i__], abs(d__1));
/* L20: */
	}
    }

    n2 = *n << 1;
    n1 = *n;
    if (! (*lreal)) {
	n1 = n2;
    }
    k = idamax_(&n1, &x[1], &c__1);
    xmax = (d__1 = x[k], abs(d__1));
    *scale = 1.;

    if (xmax > bignum) {
	*scale = bignum / xmax;
	dscal_(&n1, scale, &x[1], &c__1);
	xmax = bignum;
    }

    if (*lreal) {

	if (notran) {

/*           Solve T*p = scale*c */

	    jnext = *n;
	    for (j = *n; j >= 1; --j) {
		if (j > jnext) {
		    goto L30;
		}
		j1 = j;
		j2 = j;
		jnext = j - 1;
		if (j > 1) {
		    if (t[j + (j - 1) * t_dim1] != 0.) {
			j1 = j - 1;
			jnext = j - 2;
		    }
		}

		if (j1 == j2) {

/*                 Meet 1 by 1 diagonal block */

/*                 Scale to avoid overflow when computing */
/*                     x(j) = b(j)/T(j,j) */

		    xj = (d__1 = x[j1], abs(d__1));
		    tjj = (d__1 = t[j1 + j1 * t_dim1], abs(d__1));
		    tmp = t[j1 + j1 * t_dim1];
		    if (tjj < smin) {
			tmp = smin;
			tjj = smin;
			*info = 1;
		    }

		    if (xj == 0.) {
			goto L30;
		    }

		    if (tjj < 1.) {
			if (xj > bignum * tjj) {
			    rec = 1. / xj;
			    dscal_(n, &rec, &x[1], &c__1);
			    *scale *= rec;
			    xmax *= rec;
			}
		    }
		    x[j1] /= tmp;
		    xj = (d__1 = x[j1], abs(d__1));

/*                 Scale x if necessary to avoid overflow when adding a */
/*                 multiple of column j1 of T. */

		    if (xj > 1.) {
			rec = 1. / xj;
			if (work[j1] > (bignum - xmax) * rec) {
			    dscal_(n, &rec, &x[1], &c__1);
			    *scale *= rec;
			}
		    }
		    if (j1 > 1) {
			i__1 = j1 - 1;
			d__1 = -x[j1];
			daxpy_(&i__1, &d__1, &t[j1 * t_dim1 + 1], &c__1, &x[1]
, &c__1);
			i__1 = j1 - 1;
			k = idamax_(&i__1, &x[1], &c__1);
			xmax = (d__1 = x[k], abs(d__1));
		    }

		} else {

/*                 Meet 2 by 2 diagonal block */

/*                 Call 2 by 2 linear system solve, to take */
/*                 care of possible overflow by scaling factor. */

		    d__[0] = x[j1];
		    d__[1] = x[j2];
		    dlaln2_(&c_false, &c__2, &c__1, &smin, &c_b21, &t[j1 + j1
			    * t_dim1], ldt, &c_b21, &c_b21, d__, &c__2, &
			    c_b25, &c_b25, v, &c__2, &scaloc, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 2;
		    }

		    if (scaloc != 1.) {
			dscal_(n, &scaloc, &x[1], &c__1);
			*scale *= scaloc;
		    }
		    x[j1] = v[0];
		    x[j2] = v[1];

/*                 Scale V(1,1) (= X(J1)) and/or V(2,1) (=X(J2)) */
/*                 to avoid overflow in updating right-hand side. */

/* Computing MAX */
		    d__1 = abs(v[0]), d__2 = abs(v[1]);
		    xj = std::max(d__1,d__2);
		    if (xj > 1.) {
			rec = 1. / xj;
/* Computing MAX */
			d__1 = work[j1], d__2 = work[j2];
			if (std::max(d__1,d__2) > (bignum - xmax) * rec) {
			    dscal_(n, &rec, &x[1], &c__1);
			    *scale *= rec;
			}
		    }

/*                 Update right-hand side */

		    if (j1 > 1) {
			i__1 = j1 - 1;
			d__1 = -x[j1];
			daxpy_(&i__1, &d__1, &t[j1 * t_dim1 + 1], &c__1, &x[1]
, &c__1);
			i__1 = j1 - 1;
			d__1 = -x[j2];
			daxpy_(&i__1, &d__1, &t[j2 * t_dim1 + 1], &c__1, &x[1]
, &c__1);
			i__1 = j1 - 1;
			k = idamax_(&i__1, &x[1], &c__1);
			xmax = (d__1 = x[k], abs(d__1));
		    }

		}

L30:
		;
	    }

	} else {

/*           Solve T'*p = scale*c */

	    jnext = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < jnext) {
		    goto L40;
		}
		j1 = j;
		j2 = j;
		jnext = j + 1;
		if (j < *n) {
		    if (t[j + 1 + j * t_dim1] != 0.) {
			j2 = j + 1;
			jnext = j + 2;
		    }
		}

		if (j1 == j2) {

/*                 1 by 1 diagonal block */

/*                 Scale if necessary to avoid overflow in forming the */
/*                 right-hand side element by inner product. */

		    xj = (d__1 = x[j1], abs(d__1));
		    if (xmax > 1.) {
			rec = 1. / xmax;
			if (work[j1] > (bignum - xj) * rec) {
			    dscal_(n, &rec, &x[1], &c__1);
			    *scale *= rec;
			    xmax *= rec;
			}
		    }

		    i__2 = j1 - 1;
		    x[j1] -= ddot_(&i__2, &t[j1 * t_dim1 + 1], &c__1, &x[1], &
			    c__1);

		    xj = (d__1 = x[j1], abs(d__1));
		    tjj = (d__1 = t[j1 + j1 * t_dim1], abs(d__1));
		    tmp = t[j1 + j1 * t_dim1];
		    if (tjj < smin) {
			tmp = smin;
			tjj = smin;
			*info = 1;
		    }

		    if (tjj < 1.) {
			if (xj > bignum * tjj) {
			    rec = 1. / xj;
			    dscal_(n, &rec, &x[1], &c__1);
			    *scale *= rec;
			    xmax *= rec;
			}
		    }
		    x[j1] /= tmp;
/* Computing MAX */
		    d__2 = xmax, d__3 = (d__1 = x[j1], abs(d__1));
		    xmax = std::max(d__2,d__3);

		} else {

/*                 2 by 2 diagonal block */

/*                 Scale if necessary to avoid overflow in forming the */
/*                 right-hand side elements by inner product. */

/* Computing MAX */
		    d__3 = (d__1 = x[j1], abs(d__1)), d__4 = (d__2 = x[j2],
			    abs(d__2));
		    xj = std::max(d__3,d__4);
		    if (xmax > 1.) {
			rec = 1. / xmax;
/* Computing MAX */
			d__1 = work[j2], d__2 = work[j1];
			if (std::max(d__1,d__2) > (bignum - xj) * rec) {
			    dscal_(n, &rec, &x[1], &c__1);
			    *scale *= rec;
			    xmax *= rec;
			}
		    }

		    i__2 = j1 - 1;
		    d__[0] = x[j1] - ddot_(&i__2, &t[j1 * t_dim1 + 1], &c__1,
			    &x[1], &c__1);
		    i__2 = j1 - 1;
		    d__[1] = x[j2] - ddot_(&i__2, &t[j2 * t_dim1 + 1], &c__1,
			    &x[1], &c__1);

		    dlaln2_(&c_true, &c__2, &c__1, &smin, &c_b21, &t[j1 + j1 *
			     t_dim1], ldt, &c_b21, &c_b21, d__, &c__2, &c_b25,
			     &c_b25, v, &c__2, &scaloc, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 2;
		    }

		    if (scaloc != 1.) {
			dscal_(n, &scaloc, &x[1], &c__1);
			*scale *= scaloc;
		    }
		    x[j1] = v[0];
		    x[j2] = v[1];
/* Computing MAX */
		    d__3 = (d__1 = x[j1], abs(d__1)), d__4 = (d__2 = x[j2],
			    abs(d__2)), d__3 = std::max(d__3,d__4);
		    xmax = std::max(d__3,xmax);

		}
L40:
		;
	    }
	}

    } else {

/* Computing MAX */
	d__1 = eps * abs(*w);
	sminw = std::max(d__1,smin);
	if (notran) {

/*           Solve (T + iB)*(p+iq) = c+id */

	    jnext = *n;
	    for (j = *n; j >= 1; --j) {
		if (j > jnext) {
		    goto L70;
		}
		j1 = j;
		j2 = j;
		jnext = j - 1;
		if (j > 1) {
		    if (t[j + (j - 1) * t_dim1] != 0.) {
			j1 = j - 1;
			jnext = j - 2;
		    }
		}

		if (j1 == j2) {

/*                 1 by 1 diagonal block */

/*                 Scale if necessary to avoid overflow in division */

		    z__ = *w;
		    if (j1 == 1) {
			z__ = b[1];
		    }
		    xj = (d__1 = x[j1], abs(d__1)) + (d__2 = x[*n + j1], abs(
			    d__2));
		    tjj = (d__1 = t[j1 + j1 * t_dim1], abs(d__1)) + abs(z__);
		    tmp = t[j1 + j1 * t_dim1];
		    if (tjj < sminw) {
			tmp = sminw;
			tjj = sminw;
			*info = 1;
		    }

		    if (xj == 0.) {
			goto L70;
		    }

		    if (tjj < 1.) {
			if (xj > bignum * tjj) {
			    rec = 1. / xj;
			    dscal_(&n2, &rec, &x[1], &c__1);
			    *scale *= rec;
			    xmax *= rec;
			}
		    }
		    dladiv_(&x[j1], &x[*n + j1], &tmp, &z__, &sr, &si);
		    x[j1] = sr;
		    x[*n + j1] = si;
		    xj = (d__1 = x[j1], abs(d__1)) + (d__2 = x[*n + j1], abs(
			    d__2));

/*                 Scale x if necessary to avoid overflow when adding a */
/*                 multiple of column j1 of T. */

		    if (xj > 1.) {
			rec = 1. / xj;
			if (work[j1] > (bignum - xmax) * rec) {
			    dscal_(&n2, &rec, &x[1], &c__1);
			    *scale *= rec;
			}
		    }

		    if (j1 > 1) {
			i__1 = j1 - 1;
			d__1 = -x[j1];
			daxpy_(&i__1, &d__1, &t[j1 * t_dim1 + 1], &c__1, &x[1]
, &c__1);
			i__1 = j1 - 1;
			d__1 = -x[*n + j1];
			daxpy_(&i__1, &d__1, &t[j1 * t_dim1 + 1], &c__1, &x[*
				n + 1], &c__1);

			x[1] += b[j1] * x[*n + j1];
			x[*n + 1] -= b[j1] * x[j1];

			xmax = 0.;
			i__1 = j1 - 1;
			for (k = 1; k <= i__1; ++k) {
/* Computing MAX */
			    d__3 = xmax, d__4 = (d__1 = x[k], abs(d__1)) + (
				    d__2 = x[k + *n], abs(d__2));
			    xmax = std::max(d__3,d__4);
/* L50: */
			}
		    }

		} else {

/*                 Meet 2 by 2 diagonal block */

		    d__[0] = x[j1];
		    d__[1] = x[j2];
		    d__[2] = x[*n + j1];
		    d__[3] = x[*n + j2];
		    d__1 = -(*w);
		    dlaln2_(&c_false, &c__2, &c__2, &sminw, &c_b21, &t[j1 +
			    j1 * t_dim1], ldt, &c_b21, &c_b21, d__, &c__2, &
			    c_b25, &d__1, v, &c__2, &scaloc, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 2;
		    }

		    if (scaloc != 1.) {
			i__1 = *n << 1;
			dscal_(&i__1, &scaloc, &x[1], &c__1);
			*scale = scaloc * *scale;
		    }
		    x[j1] = v[0];
		    x[j2] = v[1];
		    x[*n + j1] = v[2];
		    x[*n + j2] = v[3];

/*                 Scale X(J1), .... to avoid overflow in */
/*                 updating right hand side. */

/* Computing MAX */
		    d__1 = abs(v[0]) + abs(v[2]), d__2 = abs(v[1]) + abs(v[3])
			    ;
		    xj = std::max(d__1,d__2);
		    if (xj > 1.) {
			rec = 1. / xj;
/* Computing MAX */
			d__1 = work[j1], d__2 = work[j2];
			if (std::max(d__1,d__2) > (bignum - xmax) * rec) {
			    dscal_(&n2, &rec, &x[1], &c__1);
			    *scale *= rec;
			}
		    }

/*                 Update the right-hand side. */

		    if (j1 > 1) {
			i__1 = j1 - 1;
			d__1 = -x[j1];
			daxpy_(&i__1, &d__1, &t[j1 * t_dim1 + 1], &c__1, &x[1]
, &c__1);
			i__1 = j1 - 1;
			d__1 = -x[j2];
			daxpy_(&i__1, &d__1, &t[j2 * t_dim1 + 1], &c__1, &x[1]
, &c__1);

			i__1 = j1 - 1;
			d__1 = -x[*n + j1];
			daxpy_(&i__1, &d__1, &t[j1 * t_dim1 + 1], &c__1, &x[*
				n + 1], &c__1);
			i__1 = j1 - 1;
			d__1 = -x[*n + j2];
			daxpy_(&i__1, &d__1, &t[j2 * t_dim1 + 1], &c__1, &x[*
				n + 1], &c__1);

			x[1] = x[1] + b[j1] * x[*n + j1] + b[j2] * x[*n + j2];
			x[*n + 1] = x[*n + 1] - b[j1] * x[j1] - b[j2] * x[j2];

			xmax = 0.;
			i__1 = j1 - 1;
			for (k = 1; k <= i__1; ++k) {
/* Computing MAX */
			    d__3 = (d__1 = x[k], abs(d__1)) + (d__2 = x[k + *
				    n], abs(d__2));
			    xmax = std::max(d__3,xmax);
/* L60: */
			}
		    }

		}
L70:
		;
	    }

	} else {

/*           Solve (T + iB)'*(p+iq) = c+id */

	    jnext = 1;
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
		if (j < jnext) {
		    goto L80;
		}
		j1 = j;
		j2 = j;
		jnext = j + 1;
		if (j < *n) {
		    if (t[j + 1 + j * t_dim1] != 0.) {
			j2 = j + 1;
			jnext = j + 2;
		    }
		}

		if (j1 == j2) {

/*                 1 by 1 diagonal block */

/*                 Scale if necessary to avoid overflow in forming the */
/*                 right-hand side element by inner product. */

		    xj = (d__1 = x[j1], abs(d__1)) + (d__2 = x[j1 + *n], abs(
			    d__2));
		    if (xmax > 1.) {
			rec = 1. / xmax;
			if (work[j1] > (bignum - xj) * rec) {
			    dscal_(&n2, &rec, &x[1], &c__1);
			    *scale *= rec;
			    xmax *= rec;
			}
		    }

		    i__2 = j1 - 1;
		    x[j1] -= ddot_(&i__2, &t[j1 * t_dim1 + 1], &c__1, &x[1], &
			    c__1);
		    i__2 = j1 - 1;
		    x[*n + j1] -= ddot_(&i__2, &t[j1 * t_dim1 + 1], &c__1, &x[
			    *n + 1], &c__1);
		    if (j1 > 1) {
			x[j1] -= b[j1] * x[*n + 1];
			x[*n + j1] += b[j1] * x[1];
		    }
		    xj = (d__1 = x[j1], abs(d__1)) + (d__2 = x[j1 + *n], abs(
			    d__2));

		    z__ = *w;
		    if (j1 == 1) {
			z__ = b[1];
		    }

/*                 Scale if necessary to avoid overflow in */
/*                 complex division */

		    tjj = (d__1 = t[j1 + j1 * t_dim1], abs(d__1)) + abs(z__);
		    tmp = t[j1 + j1 * t_dim1];
		    if (tjj < sminw) {
			tmp = sminw;
			tjj = sminw;
			*info = 1;
		    }

		    if (tjj < 1.) {
			if (xj > bignum * tjj) {
			    rec = 1. / xj;
			    dscal_(&n2, &rec, &x[1], &c__1);
			    *scale *= rec;
			    xmax *= rec;
			}
		    }
		    d__1 = -z__;
		    dladiv_(&x[j1], &x[*n + j1], &tmp, &d__1, &sr, &si);
		    x[j1] = sr;
		    x[j1 + *n] = si;
/* Computing MAX */
		    d__3 = (d__1 = x[j1], abs(d__1)) + (d__2 = x[j1 + *n],
			    abs(d__2));
		    xmax = std::max(d__3,xmax);

		} else {

/*                 2 by 2 diagonal block */

/*                 Scale if necessary to avoid overflow in forming the */
/*                 right-hand side element by inner product. */

/* Computing MAX */
		    d__5 = (d__1 = x[j1], abs(d__1)) + (d__2 = x[*n + j1],
			    abs(d__2)), d__6 = (d__3 = x[j2], abs(d__3)) + (
			    d__4 = x[*n + j2], abs(d__4));
		    xj = std::max(d__5,d__6);
		    if (xmax > 1.) {
			rec = 1. / xmax;
/* Computing MAX */
			d__1 = work[j1], d__2 = work[j2];
			if (std::max(d__1,d__2) > (bignum - xj) / xmax) {
			    dscal_(&n2, &rec, &x[1], &c__1);
			    *scale *= rec;
			    xmax *= rec;
			}
		    }

		    i__2 = j1 - 1;
		    d__[0] = x[j1] - ddot_(&i__2, &t[j1 * t_dim1 + 1], &c__1,
			    &x[1], &c__1);
		    i__2 = j1 - 1;
		    d__[1] = x[j2] - ddot_(&i__2, &t[j2 * t_dim1 + 1], &c__1,
			    &x[1], &c__1);
		    i__2 = j1 - 1;
		    d__[2] = x[*n + j1] - ddot_(&i__2, &t[j1 * t_dim1 + 1], &
			    c__1, &x[*n + 1], &c__1);
		    i__2 = j1 - 1;
		    d__[3] = x[*n + j2] - ddot_(&i__2, &t[j2 * t_dim1 + 1], &
			    c__1, &x[*n + 1], &c__1);
		    d__[0] -= b[j1] * x[*n + 1];
		    d__[1] -= b[j2] * x[*n + 1];
		    d__[2] += b[j1] * x[1];
		    d__[3] += b[j2] * x[1];

		    dlaln2_(&c_true, &c__2, &c__2, &sminw, &c_b21, &t[j1 + j1
			    * t_dim1], ldt, &c_b21, &c_b21, d__, &c__2, &
			    c_b25, w, v, &c__2, &scaloc, &xnorm, &ierr);
		    if (ierr != 0) {
			*info = 2;
		    }

		    if (scaloc != 1.) {
			dscal_(&n2, &scaloc, &x[1], &c__1);
			*scale = scaloc * *scale;
		    }
		    x[j1] = v[0];
		    x[j2] = v[1];
		    x[*n + j1] = v[2];
		    x[*n + j2] = v[3];
/* Computing MAX */
		    d__5 = (d__1 = x[j1], abs(d__1)) + (d__2 = x[*n + j1],
			    abs(d__2)), d__6 = (d__3 = x[j2], abs(d__3)) + (
			    d__4 = x[*n + j2], abs(d__4)), d__5 = std::max(d__5,
			    d__6);
		    xmax = std::max(d__5,xmax);

		}

L80:
		;
	    }

	}

    }

    return 0;

/*     End of DLAQTR */

} /* dlaqtr_ */
