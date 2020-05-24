#ifndef _clapack_h_
#define _clapack_h_

#include "melder.h"

	/* double precision version of CLAPACK 3.2.1 interface */
	
void chla_transtype__(char *ret_val, integer ret_val_len, integer *trans);

int dbdsdc_(const char *uplo, const char *compq, integer *n, double *
	d__, double *e, double *u, integer *ldu, double *vt, 
	integer *ldvt, double *q, integer *iq, double *work, integer *
	iwork, integer *info);

int dbdsqr_(const char *uplo, integer *n, integer *ncvt, integer *
	nru, integer *ncc, double *d__, double *e, double *vt, 
	integer *ldvt, double *u, integer *ldu, double *c__, integer *
	ldc, double *work, integer *info);

int ddisna_(const char *job, integer *m, integer *n, double *d__, double *sep, integer *info);

int dgbbrd_(const char *vect, integer *m, integer *n, integer *ncc,
	integer *kl, integer *ku, double *ab, integer *ldab, double *
	d__, double *e, double *q, integer *ldq, double *pt, 
	integer *ldpt, double *c__, integer *ldc, double *work, 
	integer *info);

int dgbcon_(const char *norm, integer *n, integer *kl, integer *ku,
	double *ab, integer *ldab, integer *ipiv, double *anorm,
	double *rcond, double *work, integer *iwork, integer *info);

int dgbequ_(integer *m, integer *n, integer *kl, integer *ku,
	double *ab, integer *ldab, double *r__, double *c__,
	double *rowcnd, double *colcnd, double *amax, integer *
	info);

int dgbequb_(integer *m, integer *n, integer *kl, integer *
	ku, double *ab, integer *ldab, double *r__, double *c__,
	double *rowcnd, double *colcnd, double *amax, integer *info);

int dgbrfs_(const char *trans, integer *n, integer *kl, integer *
	ku, integer *nrhs, double *ab, integer *ldab, double *afb, 
	integer *ldafb, integer *ipiv, double *b, integer *ldb, 
	double *x, integer *ldx, double *ferr, double *berr, 
	double *work, integer *iwork, integer *info);

int dgbrfsx_(const char *trans, const char *equed, integer *n, integer *
	kl, integer *ku, integer *nrhs, double *ab, integer *ldab, 
	double *afb, integer *ldafb, integer *ipiv, double *r__, 
	double *c__, double *b, integer *ldb, double *x, integer *
	ldx, double *rcond, double *berr, integer *n_err_bnds__, 
	double *err_bnds_norm__, double *err_bnds_comp__, integer *
	nparams, double *params, double *work, integer *iwork, integer *info);

int dgbsv_(integer *n, integer *kl, integer *ku, integer *nrhs, double *ab, integer *ldab,
	integer *ipiv, double *b, integer *ldb, integer *info);

int dgbsvxx_(const char *fact, const char *trans, integer *n, integer *
	kl, integer *ku, integer *nrhs, double *ab, integer *ldab, 
	double *afb, integer *ldafb, integer *ipiv, char *equed, 
	double *r__, double *c__, double *b, integer *ldb, 
	double *x, integer *ldx, double *rcond, double *rpvgrw, 
	double *berr, integer *n_err_bnds__, double *err_bnds_norm__, 
	double *err_bnds_comp__, integer *nparams, double *params, 
	double *work, integer *iwork, integer *info);

int dgbsvx_(const char *fact, const char *trans, integer *n, integer *kl,
	integer *ku, integer *nrhs, double *ab, integer *ldab,
	double *afb, integer *ldafb, integer *ipiv, char *equed, 
	double *r__, double *c__, double *b, integer *ldb, 
	double *x, integer *ldx, double *rcond, double *ferr, 
	double *berr, double *work, integer *iwork, integer *info);

int dgbtf2_(integer *m, integer *n, integer *kl, integer *ku,
	double *ab, integer *ldab, integer *ipiv, integer *info);

int dgbtrf_(integer *m, integer *n, integer *kl, integer *ku,
	double *ab, integer *ldab, integer *ipiv, integer *info);

int dgbtrs_(const char *trans, integer *n, integer *kl, integer *
	ku, integer *nrhs, double *ab, integer *ldab, integer *ipiv, 
	double *b, integer *ldb, integer *info);

int dgebak_(const char *job, const char *side, integer *n, integer *ilo,
	integer *ihi, double *scale, integer *m, double *v, integer *
	ldv, integer *info);

int dgebal_(const char *job, integer *n, double *a, integer *
	lda, integer *ilo, integer *ihi, double *scale, integer *info);

int dgebd2_(integer *m, integer *n, double *a, integer *
	lda, double *d__, double *e, double *tauq, double *
	taup, double *work, integer *info);

int dgebrd_(integer *m, integer *n, double *a, integer *
	lda, double *d__, double *e, double *tauq, double *
	taup, double *work, integer *lwork, integer *info);

int dgecon_(const char *norm, integer *n, double *a, integer *
	lda, double *anorm, double *rcond, double *work, integer *
	iwork, integer *info);

int dgeequ_(integer *m, integer *n, double *a, integer *
	lda, double *r__, double *c__, double *rowcnd, double 
	*colcnd, double *amax, integer *info);

int dgeequb_(integer *m, integer *n, double *a, integer *lda, double *r__, double *c__,
	double *rowcnd, double *colcnd, double *amax, integer *info);

int dgees_(const char *jobvs, const char *sort, bool (*select)(const double *, const double *),
	integer *n, double *a, integer *lda, integer *sdim, double *wr, 
	double *wi, double *vs, integer *ldvs, double *work, 
	integer *lwork, bool *bwork, integer *info);

int dgeesx_(const char *jobvs, const char *sort, bool (*select)(const double *, const double *),
	const char *sense, integer *n, double *a, integer *lda, integer *sdim, 
	double *wr, double *wi, double *vs, integer *ldvs, 
	double *rconde, double *rcondv, double *work, integer *
	lwork, integer *iwork, integer *liwork, bool *bwork, integer *info);

int dgeev_(const char *jobvl, const char *jobvr, integer *n, double *a, integer *lda,
	double *wr,	double *wi, double *vl, integer *ldvl, double *vr, integer *ldvr, 
	double *work, integer *lwork, integer *info);

int dgeevx_(const char *balanc, const char *jobvl, const char *jobvr, const char *
	sense, integer *n, double *a, integer *lda, double *wr, 
	double *wi, double *vl, integer *ldvl, double *vr, 
	integer *ldvr, integer *ilo, integer *ihi, double *scale, 
	double *abnrm, double *rconde, double *rcondv, double  	
	*work, integer *lwork, integer *iwork, integer *info);

int dgegs_(const char *jobvsl, const char *jobvsr, integer *n,
	double *a, integer *lda, double *b, integer *ldb, double *
	alphar, double *alphai, double *beta, double *vsl, 
	integer *ldvsl, double *vsr, integer *ldvsr, double *work, 
	integer *lwork, integer *info);

int dgegv_(const char *jobvl, const char *jobvr, integer *n, double *
	a, integer *lda, double *b, integer *ldb, double *alphar, 
	double *alphai, double *beta, double *vl, integer *ldvl, 
	double *vr, integer *ldvr, double *work, integer *lwork, 
	integer *info);

int dgehd2_(integer *n, integer *ilo, integer *ihi,
	double *a, integer *lda, double *tau, double *work, 
	integer *info);

int dgehrd_(integer *n, integer *ilo, integer *ihi,
	double *a, integer *lda, double *tau, double *work, 
	integer *lwork, integer *info);

int dgelq2_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *info);

int dgelqf_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *lwork, integer *info);

int dgels_(const char *trans, integer *m, integer *n, integer *
	nrhs, double *a, integer *lda, double *b, integer *ldb, 
	double *work, integer *lwork, integer *info);

int dgelsd_(integer *m, integer *n, integer *nrhs,
	double *a, integer *lda, double *b, integer *ldb, double *
	s, double *rcond, integer *rank, double *work, integer *lwork,
	 integer *iwork, integer *info);

int dgelss_(integer *m, integer *n, integer *nrhs,
	double *a, integer *lda, double *b, integer *ldb, double *
	s, double *rcond, integer *rank, double *work, integer *lwork,
	 integer *info);

int dgelsx_(integer *m, integer *n, integer *nrhs,
	double *a, integer *lda, double *b, integer *ldb, integer *
	jpvt, double *rcond, integer *rank, double *work, integer *
	info);

int dgelsy_(integer *m, integer *n, integer *nrhs,
	double *a, integer *lda, double *b, integer *ldb, integer *
	jpvt, double *rcond, integer *rank, double *work, integer *
	lwork, integer *info);

int dgeql2_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *info);

int dgeqlf_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *lwork, integer *info);

int dgeqp3_(integer *m, integer *n, double *a, integer *
	lda, integer *jpvt, double *tau, double *work, integer *lwork,
	integer *info);

int dgeqpf_(integer *m, integer *n, double *a, integer *
	lda, integer *jpvt, double *tau, double *work, integer *info);

int dgeqr2_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *info);

int dgeqrf_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *lwork, integer *info);

int dgerfs_(const char *trans, integer *n, integer *nrhs,
	double *a, integer *lda, double *af, integer *ldaf, integer *
	ipiv, double *b, integer *ldb, double *x, integer *ldx, 
	double *ferr, double *berr, double *work, integer *iwork, 
	integer *info);

int dgerq2_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *info);

int dgerqf_(integer *m, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *lwork, integer *info);

int dgesc2_(integer *n, double *a, integer *lda,
	double *rhs, integer *ipiv, integer *jpiv, double *scale);

int dgesdd_(const char *jobz, integer *m, integer *n, double *
	a, integer *lda, double *s, double *u, integer *ldu, 
	double *vt, integer *ldvt, double *work, integer *lwork, 
	integer *iwork, integer *info);

int dgesv_(integer *n, integer *nrhs, double *a, integer
	*lda, integer *ipiv, double *b, integer *ldb, integer *info);

int dgesvd_(const char *jobu, const char *jobvt, integer *m, integer *n,
	double *a, integer *lda, double *s, double *u, integer *
	ldu, double *vt, integer *ldvt, double *work, integer *lwork, 
	integer *info);

int dgesvx_(const char *fact, const char *trans, integer *n, integer *
	nrhs, double *a, integer *lda, double *af, integer *ldaf, 
	integer *ipiv, char *equed, double *r__, double *c__, 
	double *b, integer *ldb, double *x, integer *ldx, double *
	rcond, double *ferr, double *berr, double *work, integer *
	iwork, integer *info);

int dgetc2_(integer *n, double *a, integer *lda, integer *ipiv, integer *jpiv, integer *info);

int dgetf2_(integer *m, integer *n, double *a, integer *lda, integer *ipiv, integer *info);

int dgetrf_(integer *m, integer *n, double *a, integer *lda, integer *ipiv, integer *info);

int dgetri_(integer *n, double *a, integer *lda, integer *ipiv, double *work, integer *lwork, integer *info);

int dgetrs_(const char *trans, integer *n, integer *nrhs,
	double *a, integer *lda, integer *ipiv, double *b, integer *
	ldb, integer *info);

int dggbak_(const char *job, const char *side, integer *n, integer *ilo,
	integer *ihi, double *lscale, double *rscale, integer *m, 
	double *v, integer *ldv, integer *info);

int dggbal_(const char *job, integer *n, double *a, integer *
	lda, double *b, integer *ldb, integer *ilo, integer *ihi, 
	double *lscale, double *rscale, double *work, integer *
	info);

int dgges_(const char *jobvsl, const char *jobvsr, const char *sort,
	bool (*selctg)(const double *, const double *, const double *),
	integer *n, double *a, integer *lda, double *b, 
	integer *ldb, integer *sdim, double *alphar, double *alphai, 
	double *beta, double *vsl, integer *ldvsl, double *vsr, 
	integer *ldvsr, double *work, integer *lwork, bool *bwork, 
	integer *info);

int dggesx_(const char *jobvsl, const char *jobvsr, const char *sort,
	bool (*selctg)(const double *, const double *, const double *),
	const char *sense, integer *n, double *a, integer *lda, 
	double *b, integer *ldb, integer *sdim, double *alphar, 
	double *alphai, double *beta, double *vsl, integer *ldvsl,
	 double *vsr, integer *ldvsr, double *rconde, double *
	rcondv, double *work, integer *lwork, integer *iwork, integer * 	
	liwork, bool *bwork, integer *info);

int dggev_(const char *jobvl, const char *jobvr, integer *n, double *
	a, integer *lda, double *b, integer *ldb, double *alphar, 
	double *alphai, double *beta, double *vl, integer *ldvl, 
	double *vr, integer *ldvr, double *work, integer *lwork, 
	integer *info);

int dggevx_(const char *balanc, const char *jobvl, const char *jobvr, const char *
	sense, integer *n, double *a, integer *lda, double *b, 
	integer *ldb, double *alphar, double *alphai, double *
	beta, double *vl, integer *ldvl, double *vr, integer *ldvr, 
	integer *ilo, integer *ihi, double *lscale, double *rscale, 
	double *abnrm, double *bbnrm, double *rconde, double *
	rcondv, double *work, integer *lwork, integer *iwork, bool * 	
	bwork, integer *info);

int dggglm_(integer *n, integer *m, integer *p, double *
	a, integer *lda, double *b, integer *ldb, double *d__, 
	double *x, double *y, double *work, integer *lwork, 
	integer *info);

int dgghrd_(const char *compq, const char *compz, integer *n, integer *
	ilo, integer *ihi, double *a, integer *lda, double *b, 
	integer *ldb, double *q, integer *ldq, double *z__, integer *
	ldz, integer *info);

int dgglse_(integer *m, integer *n, integer *p, double *
	a, integer *lda, double *b, integer *ldb, double *c__, 
	double *d__, double *x, double *work, integer *lwork, 
	integer *info);

int dggqrf_(integer *n, integer *m, integer *p, double *
	a, integer *lda, double *taua, double *b, integer *ldb, 
	double *taub, double *work, integer *lwork, integer *info);

int dggrqf_(integer *m, integer *p, integer *n, double *
	a, integer *lda, double *taua, double *b, integer *ldb, 
	double *taub, double *work, integer *lwork, integer *info);

int dggsvd_(const char *jobu, const char *jobv, const char *jobq, integer *m,
	integer *n, integer *p, integer *k, integer *l, double *a, 
	integer *lda, double *b, integer *ldb, double *alpha, 
	double *beta, double *u, integer *ldu, double *v, integer 
	*ldv, double *q, integer *ldq, double *work, integer *iwork, 
	integer *info);

int dggsvp_(const char *jobu, const char *jobv, const char *jobq, integer *m,
	integer *p, integer *n, double *a, integer *lda, double *b, 
	integer *ldb, double *tola, double *tolb, integer *k, integer 
	*l, double *u, integer *ldu, double *v, integer *ldv, 
	double *q, integer *ldq, integer *iwork, double *tau, 
	double *work, integer *info);

int dgtcon_(const char *norm, integer *n, double *dl,
	double *d__, double *du, double *du2, integer *ipiv, 
	double *anorm, double *rcond, double *work, integer *
	iwork, integer *info);

int dgtrfs_(const char *trans, integer *n, integer *nrhs,
	double *dl, double *d__, double *du, double *dlf, 
	double *df, double *duf, double *du2, integer *ipiv, 
	double *b, integer *ldb, double *x, integer *ldx, double *
	ferr, double *berr, double *work, integer *iwork, integer *
	info);

int dgtsv_(integer *n, integer *nrhs, double *dl,
	double *d__, double *du, double *b, integer *ldb, integer 
	*info);

int dgtsvx_(const char *fact, const char *trans, integer *n, integer *
	nrhs, double *dl, double *d__, double *du, double *
	dlf, double *df, double *duf, double *du2, integer *ipiv, 
	double *b, integer *ldb, double *x, integer *ldx, double *
	rcond, double *ferr, double *berr, double *work, integer *
	iwork, integer *info);

int dgttrf_(integer *n, double *dl, double *d__,
	double *du, double *du2, integer *ipiv, integer *info);

int dgttrs_(const char *trans, integer *n, integer *nrhs,
	double *dl, double *d__, double *du, double *du2,  	
	integer *ipiv, double *b, integer *ldb, integer *info);

int dgtts2_(integer *itrans, integer *n, integer *nrhs,
	double *dl, double *d__, double *du, double *du2, 
	integer *ipiv, double *b, integer *ldb);

int dhgeqz_(const char *job, const char *compq, const char *compz, integer *n,
	integer *ilo, integer *ihi, double *h__, integer *ldh, double 
	*t, integer *ldt, double *alphar, double *alphai, double *
	beta, double *q, integer *ldq, double *z__, integer *ldz, 
	double *work, integer *lwork, integer *info);

int dhsein_(const char *side, const char *eigsrc, const char *initv, bool *
	select, integer *n, double *h__, integer *ldh, double *wr, 
	double *wi, double *vl, integer *ldvl, double *vr, 
	integer *ldvr, integer *mm, integer *m, double *work, integer *
	ifaill, integer *ifailr, integer *info);

int dhseqr_(const char *job, const char *compz, integer *n, integer *ilo, integer *ihi,
	double *h__, integer *ldh, double *wr, double *wi, double *z__, integer *ldz,
	double *work, integer *lwork, integer *info);

bool disnan_(double *din);

double dla_gbrcond__(const char *trans, integer *n, integer *kl, integer *ku,
	double *ab, integer *ldab, double *afb, integer *ldafb, 
	integer *ipiv, integer *cmode, double *c__, integer *info, 
	double *work, integer *iwork, integer trans_len);

int dla_gbrfsx_extended__(integer *prec_type__, integer *
	trans_type__, integer *n, integer *kl, integer *ku, integer *nrhs, 
	double *ab, integer *ldab, double *afb, integer *ldafb, 
	integer *ipiv, bool *colequ, double *c__, double *b, 
	integer *ldb, double *y, integer *ldy, double *berr_out__, 
	integer *n_norms__, double *err_bnds_norm__, double *
	err_bnds_comp__, double *res, double *ayb, double *dy, 
	double *y_tail__, double *rcond, integer *ithresh, double 
	*rthresh, double *dz_ub__, bool *ignore_cwise__, integer *info);

double dla_gbrpvgrw__(integer *n, integer *kl, integer *ku, integer *ncols,
	double *ab, integer *ldab, double *afb, integer *ldafb);

double dla_porcond__(const char *uplo, integer *n, double *a, integer *lda, 
	double *af, integer *ldaf, integer *cmode, double *c__, 
	integer *info, double *work, integer *iwork, integer uplo_len);

int dla_wwaddw__(integer *n, double *x, double *y, double *w);

int dlabad_(double *small, double *large);

int dlabrd_(integer *m, integer *n, integer *nb, double *
	a, integer *lda, double *d__, double *e, double *tauq, 
	double *taup, double *x, integer *ldx, double *y, integer 
	*ldy);

int dlacn2_(integer *n, double *v, double *x,
	integer *isgn, double *est, integer *kase, integer *isave);

int dlacon_(integer *n, double *v, double *x,
	integer *isgn, double *est, integer *kase);

int dlacpy_(const char *uplo, integer *m, integer *n, double *
	a, integer *lda, double *b, integer *ldb);

int dladiv_(double *a, double *b, double *c__,
	double *d__, double *p, double *q);

int dlae2_(double *a, double *b, double *c__,
	double *rt1, double *rt2);

int dlaebz_(integer *ijob, integer *nitmax, integer *n,
	integer *mmax, integer *minp, integer *nbmin, double *abstol, 
	double *reltol, double *pivmin, double *d__, double *
	e, double *e2, integer *nval, double *ab, double *c__, 
	integer *mout, integer *nab, double *work, integer *iwork, 
	integer *info);

int dlaed0_(integer *icompq, integer *qsiz, integer *n,
	double *d__, double *e, double *q, integer *ldq, 
	double *qstore, integer *ldqs, double *work, integer *iwork, 
	integer *info);

int dlaed1_(integer *n, double *d__, double *q,
	integer *ldq, integer *indxq, double *rho, integer *cutpnt, 
	double *work, integer *iwork, integer *info);

int dlaed2_(integer *k, integer *n, integer *n1, double *
	d__, double *q, integer *ldq, integer *indxq, double *rho, 
	double *z__, double *dlamda, double *w, double *q2, 
	integer *indx, integer *indxc, integer *indxp, integer *coltyp, 
	integer *info);

int dlaed3_(integer *k, integer *n, integer *n1, double *
	d__, double *q, integer *ldq, double *rho, double *dlamda,
	double *q2, integer *indx, integer *ctot, double *w,
	double *s, integer *info);

int dlaed4_(integer *n, integer *i__, double *d__,
	double *z__, double *delta, double *rho, double *dlam,
	integer *info);

int dlaed5_(integer *i__, double *d__, double *z__,
	double *delta, double *rho, double *dlam);

int dlaed6_(integer *kniter, bool *orgati, double *
	rho, double *d__, double *z__, double *finit, double *
	tau, integer *info);

int dlaed7_(integer *icompq, integer *n, integer *qsiz,
	integer *tlvls, integer *curlvl, integer *curpbm, double *d__, 
	double *q, integer *ldq, integer *indxq, double *rho, integer 
	*cutpnt, double *qstore, integer *qptr, integer *prmptr, integer *
	perm, integer *givptr, integer *givcol, double *givnum, 
	double *work, integer *iwork, integer *info);

int dlaed8_(integer *icompq, integer *k, integer *n, integer
	*qsiz, double *d__, double *q, integer *ldq, integer *indxq, 
	double *rho, integer *cutpnt, double *z__, double *dlamda,
	double *q2, integer *ldq2, double *w, integer *perm, integer
	*givptr, integer *givcol, double *givnum, integer *indxp, integer 
	*indx, integer *info);

int dlaed9_(integer *k, integer *kstart, integer *kstop,
	integer *n, double *d__, double *q, integer *ldq, double *
	rho, double *dlamda, double *w, double *s, integer *lds, 
	integer *info);

int dlaeda_(integer *n, integer *tlvls, integer *curlvl,
	integer *curpbm, integer *prmptr, integer *perm, integer *givptr, 
	integer *givcol, double *givnum, double *q, integer *qptr, 
	double *z__, double *ztemp, integer *info);

int dlaein_(bool *rightv, bool *noinit, integer *n,
	double *h__, integer *ldh, double *wr, double *wi, 
	double *vr, double *vi, double *b, integer *ldb, 
	double *work, double *eps3, double *smlnum, double *
	bignum, integer *info);

int dlaev2_(double *a, double *b, double *c__,
	double *rt1, double *rt2, double *cs1, double *sn1);

int dlaexc_(bool *wantq, integer *n, double *t,
	integer *ldt, double *q, integer *ldq, integer *j1, integer *n1, 
	integer *n2, double *work, integer *info);

int dlag2_(double *a, integer *lda, double *b, integer *ldb, double *safmin, double *scale1, 
	double *scale2, double *wr1, double *wr2, double *wi);

int dlag2s_(integer *m, integer *n, double *a, integer *lda, float *sa, integer *ldsa, integer *info);

int dlags2_(bool *upper, double *a1, double *a2,
	double *a3, double *b1, double *b2, double *b3, 
	double *csu, double *snu, double *csv, double *snv, 
	double *csq, double *snq);

int dlagtf_(integer *n, double *a, double *lambda,
	double *b, double *c__, double *tol, double *d__, 
	integer *in, integer *info);

int dlagtm_(const char *trans, integer *n, integer *nrhs,
	double *alpha, double *dl, double *d__, double *du, 
	double *x, integer *ldx, double *beta, double *b, integer 
	*ldb);

int dlagts_(integer *job, integer *n, double *a,
	double *b, double *c__, double *d__, integer *in, 
	double *y, double *tol, integer *info);

int dlagv2_(double *a, integer *lda, double *b,
	integer *ldb, double *alphar, double *alphai, double *
	beta, double *csl, double *snl, double *csr, double *
	snr);

int dlahqr_(bool *wantt, bool *wantz, integer *n,
	integer *ilo, integer *ihi, double *h__, integer *ldh, double 
	*wr, double *wi, integer *iloz, integer *ihiz, double *z__, 
	integer *ldz, integer *info);

int dlahr2_(integer *n, integer *k, integer *nb, double *
	a, integer *lda, double *tau, double *t, integer *ldt, 
	double *y, integer *ldy);

int dlahrd_(integer *n, integer *k, integer *nb, double *
	a, integer *lda, double *tau, double *t, integer *ldt, 
	double *y, integer *ldy);

int dlaic1_(integer *job, integer *j, double *x,
	double *sest, double *w, double *gamma, double *
	sestpr, double *s, double *c__);

bool dlaisnan_(double *din1, double *din2);

int dlaln2_(bool *ltrans, integer *na, integer *nw,
	double *smin, double *ca, double *a, integer *lda, 
	double *d1, double *d2, double *b, integer *ldb, 
	double *wr, double *wi, double *x, integer *ldx, 
	double *scale, double *xnorm, integer *info);

int dlals0_(integer *icompq, integer *nl, integer *nr,
	integer *sqre, integer *nrhs, double *b, integer *ldb, double 
	*bx, integer *ldbx, integer *perm, integer *givptr, integer *givcol, 
	integer *ldgcol, double *givnum, integer *ldgnum, double *
	poles, double *difl, double *difr, double *z__, integer *
	k, double *c__, double *s, double *work, integer *info);

int dlalsa_(integer *icompq, integer *smlsiz, integer *n,
	integer *nrhs, double *b, integer *ldb, double *bx, integer *
	ldbx, double *u, integer *ldu, double *vt, integer *k, 
	double *difl, double *difr, double *z__, double *
	poles, integer *givptr, integer *givcol, integer *ldgcol, integer *
	perm, double *givnum, double *c__, double *s, double *
	work, integer *iwork, integer *info);

int dlalsd_(const char *uplo, integer *smlsiz, integer *n, integer
	*nrhs, double *d__, double *e, double *b, integer *ldb, 
	double *rcond, integer *rank, double *work, integer *iwork, 
	integer *info);

int dlamc1_ (integer *beta, integer *t, bool *rnd, bool *ieee1);

int dlamc2_ (integer *beta, integer *t, bool *rnd,
	double *eps, integer *emin, double *rmin, integer *emax, double *rmax);

double dlamc3_ (double *a, double *b);

int dlamc4_ (integer *emin, double *start, integer *base);

int dlamc5_ (integer *beta, integer *p, integer *emin,
	bool *ieee, integer *emax, double *rmax);

double dlamch_ (const char *cmach);

int dlamrg_(integer *n1, integer *n2, double *a, integer 
	*dtrd1, integer *dtrd2, integer *index);

integer dlaneg_(integer *n, double *d__, double *lld, double *
	sigma, double *pivmin, integer *r__);

double dlangb_(const char *norm, integer *n, integer *kl, integer *ku,
	double *ab, integer *ldab, double *work);

double dlange_(const char *norm, integer *m, integer *n, double *a, integer
	*lda, double *work);

double dlangt_(const char *norm, integer *n, double *dl, double *d__,
	double *du);

double dlanhs_ (const char *norm, integer *n, double *a, integer *lda,
	double *work);

double dlansb_(const char *norm, const char *uplo, integer *n, integer *k, double
	*ab, integer *ldab, double *work);

double dlansp_(const char *norm, const char *uplo, integer *n, double *ap,
	double *work);

double dlanst_(const char *norm, integer *n, double *d__, double *e);

double dlansy_(const char *norm, const char *uplo, integer *n, double *a, integer
	*lda, double *work);

double dlantb_(const char *norm, const char *uplo, const char *diag, integer *n, integer *k,
	 double *ab, integer *ldab, double *work);

double dlantp_(const char *norm, const char *uplo, const char *diag, integer *n, double *ap, double *work);

double dlantr_(const char *norm, const char *uplo, const char *diag, integer *m, integer *n,
	 double *a, integer *lda, double *work);

double dlansf_(const char *norm, char *transr, char *uplo, integer *n, double *a, double *work);

int dlanv2_(double *a, double *b, double *c__, 
	double *d__, double *rt1r, double *rt1i, double *rt2r,
	 double *rt2i, double *cs, double *sn);

int dlapll_(integer *n, double *x, integer *incx,
	double *y, integer *incy, double *ssmin);

int dlapmt_(bool *forwrd, integer *m, integer *n,
	double *x, integer *ldx, integer *k);

double dlapy2_(double *x, double *y);

double dlapy3_(double *x, double *y, double *z__);

int dlaqgb_(integer *m, integer *n, integer *kl, integer *ku,
	 double *ab, integer *ldab, double *r__, double *c__, 
	double *rowcnd, double *colcnd, double *amax, char *equed);

int dlaqge_(integer *m, integer *n, double *a, integer *
	lda, double *r__, double *c__, double *rowcnd, double 
	*colcnd, double *amax, char *equed);

int dlaqp2_(integer *m, integer *n, integer *offset,
	double *a, integer *lda, integer *jpvt, double *tau, 
	double *vn1, double *vn2, double *work);

int dlaqps_(integer *m, integer *n, integer *offset, integer
	*nb, integer *kb, double *a, integer *lda, integer *jpvt, 
	double *tau, double *vn1, double *vn2, double *auxv, 
	double *f, integer *ldf);

int dlaqr0_(bool *wantt, bool *wantz, integer *n, integer *ilo, integer *ihi, double *h__,
	integer *ldh, double *wr, double *wi, integer *iloz, integer *ihiz, double *z__,
	integer *ldz, double *work, integer *lwork, integer *info);

int dlaqr1_(integer *n, double *h__, integer *ldh, double *sr1, double *si1,
	double *sr2, double *si2, double *v);

int dlaqr2_(bool *wantt, bool *wantz, integer *n, integer *ktop, integer *kbot, integer *nw,
	double *h__, integer *ldh, integer *iloz, integer *ihiz, double *z__, integer *ldz,
	integer *ns, integer *nd, double *sr, double *si, double *v, integer *ldv, integer *nh, 
	double *t, integer *ldt, integer *nv, double *wv, integer *ldwv, double *work, integer *lwork);
int dlaqr3_(bool *wantt, bool *wantz, integer *n, integer *ktop, integer *kbot, integer *nw,
	double *h__, integer *ldh, integer *iloz, integer *ihiz, double *z__, integer *ldz,
	integer *ns, integer *nd, double *sr, double *si, double *v, integer *ldv, integer *nh,
	double *t, integer *ldt, integer *nv, double *wv, integer *ldwv, double *work, integer *lwork);

int dlaqr4_(bool *wantt, bool *wantz, integer *n, integer *ilo, integer *ihi, double *h__,
	integer *ldh, double *wr, double *wi, integer *iloz, integer *ihiz, double *z__,
	integer *ldz, double *work, integer *lwork, integer *info);

int dlaqr5_(bool *wantt, bool *wantz, integer *kacc22, integer *n, integer *ktop, integer *kbot,
	integer *nshfts, double *sr, double *si, double *h__, integer *ldh, integer *iloz,
	integer *ihiz, double *z__, integer *ldz, double *v, integer *ldv, double *u, integer *ldu, 
	integer *nv, double *wv, integer *ldwv, integer *nh, double *wh, integer *ldwh);

int dlaqsb_(const char *uplo, integer *n, integer *kd, double *ab, integer *ldab, double *s,
	double *scond, double *amax, const char *equed);

int dlaqsp_(const char *uplo, integer *n, double *ap, double *s, double *scond, double *amax, char *equed);

int dlaqsy_(const char *uplo, integer *n, double *a, integer *lda, double *s, double *scond,
	double *amax, char *equed);

int dlaqtr_(bool *ltran, bool *lfloat, integer *n, double *t, integer *ldt, double *b,
	double *w, double *scale, double *x, double *work, integer *info);

int dlar1v_(integer *n, integer *b1, integer *bn, double *lambda, double *d__, double *l,
	double *ld, double *lld, double *pivmin, double *gaptol, double *z__, bool *wantnc,
	integer *negcnt, double *ztz, double *mingma, integer *r__, integer *isuppz,
	double *nrminv, double *resid, double *rqcorr, double *work);

int dlar2v_(integer *n, double *x, double *y, double *z__, integer *incx, double *c__, 
	double *s, integer *incc);

int dlarf_(const char *side, integer *m, integer *n, double *v, integer *incv, double *tau, 
	double *c__, integer *ldc, double *work);

int dlarfb_(const char *side, const char *trans, const char *direct, const char *storev, integer *m,
	integer *n, integer *k, double *v, integer *ldv, double *t, integer *ldt, double *c__, 
	integer *ldc, double *work, integer *ldwork);

int dlarfg_(integer *n, double *alpha, double *x, integer *incx, double *tau);

int dlarfp_(integer *n, double *alpha, double *x, integer *incx, double *tau);

int dlarft_(const char *direct, const char *storev, integer *n, integer *k, double *v,
	integer *ldv, double *tau, double *t, integer *ldt);

int dlarfx_(const char *side, integer *m, integer *n, double *v, double *tau, double *c__, integer *ldc, double *work);

int dlargv_(integer *n, double *x, integer *incx, double *y, integer *incy, double *c__, integer *incc);

int dlarnv_(integer *idist, integer *iseed, integer *n, double *x);

int dlarra_(integer *n, double *d__, double *e, double *e2, double *spltol, double *tnrm,
	integer *nsplit, integer *isplit, integer *info);

int dlarrb_(integer *n, double *d__, double *lld, integer *ifirst, integer *ilast, double *rtol1,
	double *rtol2, integer *offset, double *w, double *wgap, double *werr, double *work,
	integer *iwork, double *pivmin, double *spdiam, integer *twist, integer *info);

int dlarrc_(const char *jobt, integer *n, double *vl, double *vu, double *d__, double *e, double *pivmin,
	integer *eigcnt, integer *lcnt, integer *rcnt, integer *info);

int dlarrd_(const char *range, const char *order, integer *n, double *vl, double *vu, integer *il,
	integer *iu, double *gers, double *reltol, double *d__, double *e, double *e2, double *pivmin,
	integer *nsplit, integer *isplit, integer *m, double *w, double *werr, double *wl, double *wu,
	integer *iblock, integer *indexw, double *work, integer *iwork, integer *info);

int dlarre_(const char *range, integer *n, double *vl, double *vu, integer *il, integer *iu,
	double *d__, double *e, double *e2, double *rtol1, double *rtol2, double *spltol, integer *nsplit,
	integer *isplit, integer *m, double *w, double *werr, double *wgap, integer *iblock, 
	integer *indexw, double *gers, double *pivmin, double *work, integer *iwork, integer *info);

int dlarrf_(integer *n, double *d__, double *l, double *ld, integer *clstrt, integer *clend, 
	double *w, double *wgap, double *werr, double *spdiam, double *clgapl, double *clgapr,
	double *pivmin, double *sigma, double *dplus, double *lplus, double *work, integer *info);

int dlarrj_(integer *n, double *d__, double *e2, integer *ifirst, integer *ilast, double *rtol,
	integer *offset, double *w, double *werr, double *work, integer *iwork,
	double *pivmin, double *spdiam, integer *info);

int dlarrk_(integer *n, integer *iw, double *gl, double *gu, double *d__, double *e2, 
	double *pivmin, double *reltol, double *w, double *werr, integer *info);

int dlarrr_(integer *n, double *d__, double *e, integer *info);

int dlarrv_(integer *n, double *vl, double *vu, 
	double *d__, double *l, double *pivmin, integer *isplit, 
	integer *m, integer *dol, integer *dou, double *minrgp, 
	double *rtol1, double *rtol2, double *w, double *werr,
	 double *wgap, integer *iblock, integer *indexw, double *gers,
	 double *z__, integer *ldz, integer *isuppz, double *work, 
	integer *iwork, integer *info);

int dlarscl2_(integer *m, integer *n, double *d__, double *x, integer *ldx);

int dlartg_(double *f, double *g, double *cs, double *sn, double *r__);

int dlartv_(integer *n, double *x, integer *incx, double *y, integer *incy, 
	double *c__, double *s, integer *incc);

int dlaruv_(integer *iseed, integer *n, double *x);

int dlarz_(const char *side, integer *m, integer *n, integer *l, 
	double *v, integer *incv, double *tau, double *c__, 
	integer *ldc, double *work);

int dlarzb_(const char *side, const char *trans, const char *direct, const char *storev,
	integer *m, integer *n, integer *k, integer *l, double *v, integer *ldv, double *t,
	integer *ldt, double *c__, integer *ldc, double *work, integer *ldwork);

int dlarzt_(const char *direct, const char *storev, integer *n, integer *
	k, double *v, integer *ldv, double *tau, double *t, integer *ldt);

int dlas2_(double *f, double *g, double *h__, double *ssmin, double *ssmax);

int dlascl_(const char *type__, integer *kl, integer *ku, double *cfrom, double *cto,
	integer *m, integer *n, double *a, integer *lda, integer *info);

int dlascl2_(integer *m, integer *n, double *d__, double *x, integer *ldx);

int dlasd0_(integer *n, integer *sqre, double *d__,
	double *e, double *u, integer *ldu, double *vt, integer *
	ldvt, integer *smlsiz, integer *iwork, double *work, integer *
	info);

int dlasd1_(integer *nl, integer *nr, integer *sqre,
	double *d__, double *alpha, double *beta, double *u, 
	integer *ldu, double *vt, integer *ldvt, integer *idxq, integer *
	iwork, double *work, integer *info);

int dlasd2_(integer *nl, integer *nr, integer *sqre, integer
	*k, double *d__, double *z__, double *alpha, double *
	beta, double *u, integer *ldu, double *vt, integer *ldvt, 
	double *dsigma, double *u2, integer *ldu2, double *vt2, 
	integer *ldvt2, integer *idxp, integer *idx, integer *idxc, integer *
	idxq, integer *coltyp, integer *info);

int dlasd3_(integer *nl, integer *nr, integer *sqre, integer
	*k, double *d__, double *q, integer *ldq, double *dsigma, 
	double *u, integer *ldu, double *u2, integer *ldu2, 
	double *vt, integer *ldvt, double *vt2, integer *ldvt2, 
	integer *idxc, integer *ctot, double *z__, integer *info);

int dlasd4_(integer *n, integer *i__, double *d__,
	double *z__, double *delta, double *rho, double *
	sigma, double *work, integer *info);

int dlasd5_(integer *i__, double *d__, double *z__,
	double *delta, double *rho, double *dsigma, double *
	work);

int dlasd6_(integer *icompq, integer *nl, integer *nr,
	integer *sqre, double *d__, double *vf, double *vl, 
	double *alpha, double *beta, integer *idxq, integer *perm, 
	integer *givptr, integer *givcol, integer *ldgcol, double *givnum,
	 integer *ldgnum, double *poles, double *difl, double *
	difr, double *z__, integer *k, double *c__, double *s, 
	double *work, integer *iwork, integer *info);

int dlasd7_(integer *icompq, integer *nl, integer *nr,
	integer *sqre, integer *k, double *d__, double *z__, 
	double *zw, double *vf, double *vfw, double *vl, 
	double *vlw, double *alpha, double *beta, double *
	dsigma, integer *idx, integer *idxp, integer *idxq, integer *perm, 
	integer *givptr, integer *givcol, integer *ldgcol, double *givnum,
	 integer *ldgnum, double *c__, double *s, integer *info);

int dlasd8_(integer *icompq, integer *k, double *d__,
	double *z__, double *vf, double *vl, double *difl, 
	double *difr, integer *lddifr, double *dsigma, double *
	work, integer *info);

int dlasda_(integer *icompq, integer *smlsiz, integer *n,
	integer *sqre, double *d__, double *e, double *u, integer 
	*ldu, double *vt, integer *k, double *difl, double *difr, 
	double *z__, double *poles, integer *givptr, integer *givcol, 
	integer *ldgcol, integer *perm, double *givnum, double *c__, 
	double *s, double *work, integer *iwork, integer *info);

int dlasdq_(const char *uplo, integer *sqre, integer *n, integer *
	ncvt, integer *nru, integer *ncc, double *d__, double *e, 
	double *vt, integer *ldvt, double *u, integer *ldu, 
	double *c__, integer *ldc, double *work, integer *info);

int dlasdt_(integer *n, integer *lvl, integer *nd, integer *
	inode, integer *ndiml, integer *ndimr, integer *msub);

int dlaset_(const char *uplo, integer *m, integer *n, double *
	alpha, double *beta, double *a, integer *lda);

int dlasq1_(integer *n, double *d__, double *e,
	double *work, integer *info);

int dlasq2_(integer *n, double *z__, integer *info);

int dlasq3_(integer *i0, integer *n0, double *z__, integer *pp, double *dmin__, double *sigma,
	double *desig, double *qmax, integer *nfail, integer *iter, integer *ndiv, bool *ieee,
	integer *ttype, double *dmin1, double *dmin2, double *dn, double *dn1, double *dn2,
	double *g, double *tau);

int dlasq4_(integer *i0, integer *n0, double *z__, 
	integer *pp, integer *n0in, double *dmin__, double *dmin1, 
	double *dmin2, double *dn, double *dn1, double *dn2, 
	double *tau, integer *ttype, double *g);

int dlasq5_(integer *i0, integer *n0, double *z__,
	integer *pp, double *tau, double *dmin__, double *dmin1, 
	double *dmin2, double *dn, double *dnm1, double *dnm2,
	 bool *ieee);

int dlasq6_(integer *i0, integer *n0, double *z__,
	integer *pp, double *dmin__, double *dmin1, double *dmin2,
	 double *dn, double *dnm1, double *dnm2);

int dlasr_(const char *side, const char *pivot, const char *direct, integer *m,
	 integer *n, double *c__, double *s, double *a, integer *
	lda);

int dlasrt_(const char *id, integer *n, double *d__, integer *
	info);

int dlassq_(integer *n, double *x, integer *incx,
	double *scale, double *sumsq);

int dlasv2_(double *f, double *g, double *h__,
	double *ssmin, double *ssmax, double *snr, double *
	csr, double *snl, double *csl);

int dlaswp_(integer *n, double *a, integer *lda, integer
	*k1, integer *k2, integer *ipiv, integer *incx);

int dlasy2_(bool *ltranl, bool *ltranr, integer *isgn,
	integer *n1, integer *n2, double *tl, integer *ldtl, double *
	tr, integer *ldtr, double *b, integer *ldb, double *scale, 
	double *x, integer *ldx, double *xnorm, integer *info);

int dlasyf_(const char *uplo, integer *n, integer *nb, integer *kb, double *a, integer *lda, 
	integer *ipiv, double *w, integer *ldw, integer *info);

int dlat2s_(const char *uplo, integer *n, double *a, integer *lda, float *sa, integer *ldsa, integer *info);

int dlatbs_(const char *uplo, const char *trans, const char *diag,const  char *normin, integer *n,
	integer *kd, double *ab, integer *ldab, double *x, double *scale, double *cnorm, integer *info);

int dlatdf_(integer *ijob, integer *n, double *z__,
	integer *ldz, double *rhs, double *rdsum, double *rdscal, 
	integer *ipiv, integer *jpiv);

int dlatps_(const char *uplo, const char *trans, const char *diag, const char *
	normin, integer *n, double *ap, double *x, double *scale, 
	double *cnorm, integer *info);

int dlatrd_(const char *uplo, integer *n, integer *nb, double *
	a, integer *lda, double *e, double *tau, double *w, 
	integer *ldw);

int dlatrs_(const char *uplo, const char *trans, const char *diag, const char *
	normin, integer *n, double *a, integer *lda, double *x, 
	double *scale, double *cnorm, integer *info);

int dlatrz_(integer *m, integer *n, integer *l, double *
	a, integer *lda, double *tau, double *work);

int dlatzm_(const char *side, integer *m, integer *n, double *
	v, integer *incv, double *tau, double *c1, double *c2, 
	integer *ldc, double *work);

int dlauu2_(const char *uplo, integer *n, double *a, integer *
	lda, integer *info);

int dlauum_(const char *uplo, integer *n, double *a, integer *
	lda, integer *info);

int dlazq3_(integer *i0, integer *n0, double *z__,
	integer *pp, double *dmin__, double *sigma, double *desig,
	 double *qmax, integer *nfail, integer *iter, integer *ndiv, 
	bool *ieee, integer *ttype, double *dmin1, double *dmin2, 
	double *dn, double *dn1, double *dn2, double *tau);

int dlazq4_(integer *i0, integer *n0, double *z__, integer *pp, integer *n0in, double *dmin__, 
	double *dmin1, double *dmin2, double *dn, double *dn1, double *dn2, double *tau, integer *ttype, double *g);

integer dmaxloc_(double *a, integer *dimm);

double dnrm2_(integer *n, double *x, integer *incx);

int dopgtr_(const char *uplo, integer *n, double *ap, double *tau, double *q, integer *ldq, double *work, integer *info);

int dopmtr_(const char *side, const char *uplo, const char *trans, integer *m, integer *n, 
	double *ap, double *tau, double *c__, integer *ldc, double *work, integer *info);

int dorg2l_(integer *m, integer *n, integer *k, double *a, integer *lda, double *tau, double *work, integer *info);

int dorg2r_(integer *m, integer *n, integer *k, double *a, integer *lda, double *tau, double *work, integer *info);

int dorgbr_(const char *vect, integer *m, integer *n, integer *k,
	double *a, integer *lda, double *tau, double *work, 
	integer *lwork, integer *info);

int dorghr_(integer *n, integer *ilo, integer *ihi,
	double *a, integer *lda, double *tau, double *work, 
	integer *lwork, integer *info);

int dorgl2_(integer *m, integer *n, integer *k, double *
	a, integer *lda, double *tau, double *work, integer *info);

int dorglq_(integer *m, integer *n, integer *k, double *
	a, integer *lda, double *tau, double *work, integer *lwork, 
	integer *info);

int dorgql_(integer *m, integer *n, integer *k, double *
	a, integer *lda, double *tau, double *work, integer *lwork, 
	integer *info);

int dorgqr_(integer *m, integer *n, integer *k, double *
	a, integer *lda, double *tau, double *work, integer *lwork, 
	integer *info);

int dorgr2_(integer *m, integer *n, integer *k, double *
	a, integer *lda, double *tau, double *work, integer *info);

int dorgrq_(integer *m, integer *n, integer *k, double *
	a, integer *lda, double *tau, double *work, integer *lwork, 
	integer *info);

int dorgtr_(const char *uplo, integer *n, double *a, integer *
	lda, double *tau, double *work, integer *lwork, integer *info);

int dorm2l_(const char *side, const char *trans, integer *m, integer *n,
	integer *k, double *a, integer *lda, double *tau, double *
	c__, integer *ldc, double *work, integer *info);

int dorm2r_(const char *side, const char *trans, integer *m, integer *n,
	integer *k, double *a, integer *lda, double *tau, double *
	c__, integer *ldc, double *work, integer *info);

int dormbr_(const char *vect, const char *side, const char *trans, integer *m,
	integer *n, integer *k, double *a, integer *lda, double *tau, 
	double *c__, integer *ldc, double *work, integer *lwork, 
	integer *info);

int dormhr_(const char *side, const char *trans, integer *m, integer *n,
	integer *ilo, integer *ihi, double *a, integer *lda, double *
	tau, double *c__, integer *ldc, double *work, integer *lwork, 
	integer *info);

int dorml2_(const char *side, const char *trans, integer *m, integer *n,
	integer *k, double *a, integer *lda, double *tau, double *
	c__, integer *ldc, double *work, integer *info);

int dormlq_(const char *side, const char *trans, integer *m, integer *n,
	integer *k, double *a, integer *lda, double *tau, double *
	c__, integer *ldc, double *work, integer *lwork, integer *info);

int dormql_(const char *side, const char *trans, integer *m, integer *n,
	integer *k, double *a, integer *lda, double *tau, double *
	c__, integer *ldc, double *work, integer *lwork, integer *info);

int dormqr_(const char *side, const char *trans, integer *m, integer *n,
	integer *k, double *a, integer *lda, double *tau, double *
	c__, integer *ldc, double *work, integer *lwork, integer *info);

int dormr2_(const char *side, const char *trans, integer *m, integer *n,
	integer *k, double *a, integer *lda, double *tau, double *
	c__, integer *ldc, double *work, integer *info);

int dormr3_(const char *side, const char *trans, integer *m, integer *n,
	integer *k, integer *l, double *a, integer *lda, double *tau, 
	double *c__, integer *ldc, double *work, integer *info);

int dormrq_(const char *side, const char *trans, integer *m, integer *n,
	integer *k, double *a, integer *lda, double *tau, double *
	c__, integer *ldc, double *work, integer *lwork, integer *info);

int dormrz_(const char *side, const char *trans, integer *m, integer *n,
	integer *k, integer *l, double *a, integer *lda, double *tau, 
	double *c__, integer *ldc, double *work, integer *lwork, 
	integer *info);

int dormtr_(const char *side, const char *uplo, const char *trans, integer *m,
	integer *n, double *a, integer *lda, double *tau, double *
	c__, integer *ldc, double *work, integer *lwork, integer *info);

int dpbcon_(const char *uplo, integer *n, integer *kd, double *
	ab, integer *ldab, double *anorm, double *rcond, double *
	work, integer *iwork, integer *info);

int dpbequ_(const char *uplo, integer *n, integer *kd, double *
	ab, integer *ldab, double *s, double *scond, double *amax,
	 integer *info);

int dpbrfs_(const char *uplo, integer *n, integer *kd, integer *
	nrhs, double *ab, integer *ldab, double *afb, integer *ldafb, 
	double *b, integer *ldb, double *x, integer *ldx, double *
	ferr, double *berr, double *work, integer *iwork, integer *
	info);

int dpbstf_(const char *uplo, integer *n, integer *kd, double *
	ab, integer *ldab, integer *info);

int dpbsv_(const char *uplo, integer *n, integer *kd, integer *
	nrhs, double *ab, integer *ldab, double *b, integer *ldb, 
	integer *info);

int dpbsvx_(const char *fact, const char *uplo, integer *n, integer *kd,
	integer *nrhs, double *ab, integer *ldab, double *afb, 
	integer *ldafb, char *equed, double *s, double *b, integer *
	ldb, double *x, integer *ldx, double *rcond, double *ferr,
	double *berr, double *work, integer *iwork, integer *info);

int dpbtf2_(const char *uplo, integer *n, integer *kd, double *
	ab, integer *ldab, integer *info);

int dpbtrf_(const char *uplo, integer *n, integer *kd, double *ab, integer *ldab, integer *info);

int dpbtrs_(const char *uplo, integer *n, integer *kd, integer *nrhs, double *ab, 
	integer *ldab, double *b, integer *ldb, integer *info);

int dpftrf_(const char *transr, const char *uplo, integer *n, double *a, integer *info);

int dpftri_(const char *transr, const char *uplo, integer *n, double *a, integer *info);

int dpftrs_(char *transr, char *uplo, integer *n, integer *nrhs, double *a, double *b, integer *ldb, integer *info);

int dpocon_(const char *uplo, integer *n, double *a, integer *lda, double *anorm, 
	double *rcond, double *work, integer *iwork, integer *info);

int dpoequ_(integer *n, double *a, integer *lda, double *s, double *scond, double *amax, integer *info);

int dpoequb_(integer *n, double *a, integer *lda, double *s, double *scond, double *amax, integer *info);

int dporfs_(const char *uplo, integer *n, integer *nrhs, double *a, integer *lda, 
	double *af, integer *ldaf, double *b, integer *ldb, double *x, integer *ldx,
	double *ferr, double *berr, double *work, integer *iwork, integer *info);

int dporfsx_(const char *uplo, const char *equed, integer *n, integer *nrhs, double *a, integer *lda,
	double *af, integer *ldaf, double *s, double *b, integer *ldb, double *x, integer *
	ldx, double *rcond, double *berr, integer *n_err_bnds__, double *err_bnds_norm__, double *err_bnds_comp__,
	integer *nparams, double *params, double *work, integer *iwork, integer *info);

int dposv_(const char *uplo, integer *n, integer *nrhs, double *a, integer *lda, double *b,
	integer *ldb, integer *info);

int dposvx_(const char *fact, const char *uplo, integer *n, integer *
	nrhs, double *a, integer *lda, double *af, integer *ldaf, 
	char *equed, double *s, double *b, integer *ldb, double *
	x, integer *ldx, double *rcond, double *ferr, double *
	berr, double *work, integer *iwork, integer *info);

int dposvxx_(const char *fact, const char *uplo, integer *n, integer *nrhs, double *a,
	integer *lda, double *af, integer *ldaf, char *equed, double *s, double *b, 
	integer *ldb, double *x, integer *ldx, double *rcond, double *rpvgrw, double *berr,
	integer *n_err_bnds__, double *err_bnds_norm__, double *err_bnds_comp__, 
	integer *nparams, double *params, double *work, integer *iwork, integer *info);

int dpotf2_(const char *uplo, integer *n, double *a, integer *lda, integer *info);

int dpotrf_(const char *uplo, integer *n, double *a, integer *lda, integer *info);

int dpotri_(const char *uplo, integer *n, double *a, integer *lda, integer *info);

int dpotrs_(const char *uplo, integer *n, integer *nrhs, double *a, integer *lda, double *b,
	integer *ldb, integer *	info);

 int dppcon_(const char *uplo, integer *n, double *ap, 
	double *anorm, double *rcond, double *work, integer *
	iwork, integer *info);

 int dppequ_(const char *uplo, integer *n, double *ap, 
	double *s, double *scond, double *amax, integer *info);

 int dpprfs_(const char *uplo, integer *n, integer *nrhs, 
	double *ap, double *afp, double *b, integer *ldb, 
	double *x, integer *ldx, double *ferr, double *berr, 
	double *work, integer *iwork, integer *info);

int dppsv_(const char *uplo, integer *n, integer *nrhs, double *ap, double *b, integer *ldb, integer *info);

int dppsvx_(const char *fact, const char *uplo, integer *n, integer *nrhs, double *ap, double *afp,
	char *equed, double *s, double *b, integer *ldb, double *x, integer *ldx, double *rcond,
	double *ferr, double *berr, double *work, integer *iwork, integer *info);

int dpptrf_(const char *uplo, integer *n, double *ap, integer *info);

int dpptri_(const char *uplo, integer *n, double *ap, integer *info);

int dpptrs_(const char *uplo, integer *n, integer *nrhs, double *ap, double *b, integer *ldb, integer *info);

int dpstf2_(const char *uplo, integer *n, double *a, integer *lda, integer *piv, integer *rank, 
	double *tol, double *work, integer *info);

int dpstrf_(const char *uplo, integer *n, double *a, integer *lda, integer *piv, integer *rank,
	double *tol, double *work, integer *info);

int dptcon_(integer *n, double *d__, double *e, double *anorm, double *rcond, double *work, integer *info);

int dpteqr_(const char *compz, integer *n, double *d__, double *e, double *z__, integer *ldz, 
	double *work, integer *info);

int dptrfs_(integer *n, integer *nrhs, double *d__, double *e, double *df, double *ef, double *b,
	integer *ldb, double *x, integer *ldx, double *ferr, double *berr, double *work, integer *info);

int dptsv_(integer *n, integer *nrhs, double *d__, double *e, double *b, integer *ldb, integer *info);

 int dptsvx_(const char *fact, integer *n, integer *nrhs, 
	double *d__, double *e, double *df, double *ef, 
	double *b, integer *ldb, double *x, integer *ldx, double *
	rcond, double *ferr, double *berr, double *work, integer *
	info);

 int dpttrf_(integer *n, double *d__, double *e, 
	integer *info);

 int dpttrs_(integer *n, integer *nrhs, double *d__, 
	double *e, double *b, integer *ldb, integer *info);

 int dptts2_(integer *n, integer *nrhs, double *d__, 
	double *e, double *b, integer *ldb);

 int drscl_(integer *n, double *sa, double *sx, 
	integer *incx);

 int dsbev_(const char *jobz, const char *uplo, integer *n, integer *kd, 
	double *ab, integer *ldab, double *w, double *z__, 
	integer *ldz, double *work, integer *info);

 int dsbevd_(const char *jobz, const char *uplo, integer *n, integer *kd, 
	double *ab, integer *ldab, double *w, double *z__, 
	integer *ldz, double *work, integer *lwork, integer *iwork, 
	integer *liwork, integer *info);

 int dsbevx_(const char *jobz, const char *range, const char *uplo, integer *n, 
	integer *kd, double *ab, integer *ldab, double *q, integer *
	ldq, double *vl, double *vu, integer *il, integer *iu, 
	double *abstol, integer *m, double *w, double *z__, 
	integer *ldz, double *work, integer *iwork, integer *ifail, 
	integer *info);

 int dsbgst_(const char *vect, const char *uplo, integer *n, integer *ka, 
	integer *kb, double *ab, integer *ldab, double *bb, integer *
	ldbb, double *x, integer *ldx, double *work, integer *info);

 int dsbgv_(const char *jobz, const char *uplo, integer *n, integer *ka, 
	integer *kb, double *ab, integer *ldab, double *bb, integer *
	ldbb, double *w, double *z__, integer *ldz, double *work, 
	integer *info);

 int dsbgvd_(const char *jobz, const char *uplo, integer *n, integer *ka, 
	integer *kb, double *ab, integer *ldab, double *bb, integer *
	ldbb, double *w, double *z__, integer *ldz, double *work, 
	integer *lwork, integer *iwork, integer *liwork, integer *info);

 int dsbgvx_(const char *jobz, const char *range, const char *uplo, integer *n, 
	integer *ka, integer *kb, double *ab, integer *ldab, double *
	bb, integer *ldbb, double *q, integer *ldq, double *vl, 
	double *vu, integer *il, integer *iu, double *abstol, integer 
	*m, double *w, double *z__, integer *ldz, double *work, 
	integer *iwork, integer *ifail, integer *info);

int dsbtrd_(const char *vect, const char *uplo, integer *n, integer *kd, double *ab, integer *ldab, 
	double *d__, double *e, double *q, integer *ldq, double *work, integer *info);

int dsgesv_(integer *n, integer *nrhs, double *a, integer *lda, integer *ipiv, double *b, integer *ldb, 
	double *x, integer *ldx, double *work, float *swork, integer *iter, integer *info);

int dspcon_(const char *uplo, integer *n, double *ap, integer *ipiv, double *anorm, double *rcond, 
	double *work, integer *iwork, integer *info);

 int dspev_(const char *jobz, const char *uplo, integer *n, double *
	ap, double *w, double *z__, integer *ldz, double *work, 
	integer *info);

 int dspevd_(const char *jobz, const char *uplo, integer *n, double *
	ap, double *w, double *z__, integer *ldz, double *work, 
	integer *lwork, integer *iwork, integer *liwork, integer *info);

 int dspevx_(const char *jobz, const char *range, const char *uplo, integer *n, 
	double *ap, double *vl, double *vu, integer *il, integer *
	iu, double *abstol, integer *m, double *w, double *z__, 
	integer *ldz, double *work, integer *iwork, integer *ifail, 
	integer *info);

 int dspgst_(integer *itype, const char *uplo, integer *n, 
	double *ap, double *bp, integer *info);

 int dspgv_(integer *itype, const char *jobz, const char *uplo, integer *
	n, double *ap, double *bp, double *w, double *z__, 
	integer *ldz, double *work, integer *info);

 int dspgvd_(integer *itype, const char *jobz, const char *uplo, integer *
	n, double *ap, double *bp, double *w, double *z__, 
	integer *ldz, double *work, integer *lwork, integer *iwork, 
	integer *liwork, integer *info);

int dspgvx_(integer *itype, const char *jobz, const char *range, const char *uplo, integer *n, double *ap,
	double *bp, double *vl, double *vu, integer *il, integer *iu, double *abstol, integer *m,
	double *w, double *z__, integer *ldz, double *work, integer *iwork, integer *ifail, integer *info);

int dsposv_(const char *uplo, integer *n, integer *nrhs, double *a, integer *lda, double *b, integer *ldb,
	double *x, integer *ldx, double *work, float *swork, integer *iter, integer *info);

int dsprfs_(const char *uplo, integer *n, integer *nrhs, double *ap, double *afp, integer *ipiv,
	double *b, integer *ldb, double *x, integer *ldx, double *ferr, 
	double *berr, double *work, integer *iwork, integer *info);

int dspsv_(const char *uplo, integer *n, integer *nrhs, double *ap, integer *ipiv, double *b, 
	integer *ldb, integer *info);

 int dspsvx_(const char *fact, const char *uplo, integer *n, integer *
	nrhs, double *ap, double *afp, integer *ipiv, double *b, 
	integer *ldb, double *x, integer *ldx, double *rcond, 
	double *ferr, double *berr, double *work, integer *iwork, 
	integer *info);

 int dsptrd_(const char *uplo, integer *n, double *ap, 
	double *d__, double *e, double *tau, integer *info);

 int dsptrf_(const char *uplo, integer *n, double *ap, integer *
	ipiv, integer *info);

 int dsptri_(const char *uplo, integer *n, double *ap, integer *
	ipiv, double *work, integer *info);

 int dsptrs_(const char *uplo, integer *n, integer *nrhs, 
	double *ap, integer *ipiv, double *b, integer *ldb, integer *
	info);

 int dstebz_(const char *range, const char *order, integer *n, double 
	*vl, double *vu, integer *il, integer *iu, double *abstol, 
	double *d__, double *e, integer *m, integer *nsplit, 
	double *w, integer *iblock, integer *isplit, double *work, 
	integer *iwork, integer *info);

 int dstedc_(const char *compz, integer *n, double *d__, 
	double *e, double *z__, integer *ldz, double *work, 
	integer *lwork, integer *iwork, integer *liwork, integer *info);

 int dstegr_(const char *jobz, const char *range, integer *n, double *
	d__, double *e, double *vl, double *vu, integer *il, 
	integer *iu, double *abstol, integer *m, double *w, 
	double *z__, integer *ldz, integer *isuppz, double *work, 
	integer *lwork, integer *iwork, integer *liwork, integer *info);

 int dstein_(integer *n, double *d__, double *e, 
	integer *m, double *w, integer *iblock, integer *isplit, 
	double *z__, integer *ldz, double *work, integer *iwork, 
	integer *ifail, integer *info);

 int dstemr_(const char *jobz, const char *range, integer *n, double *
	d__, double *e, double *vl, double *vu, integer *il, 
	integer *iu, integer *m, double *w, double *z__, integer *ldz,
	 integer *nzc, integer *isuppz, bool *tryrac, double *work, 
	integer *lwork, integer *iwork, integer *liwork, integer *info);

 int dsteqr_(const char *compz, integer *n, double *d__, 
	double *e, double *z__, integer *ldz, double *work, 
	integer *info);

 int dsterf_(integer *n, double *d__, double *e, 
	integer *info);

 int dstev_(const char *jobz, integer *n, double *d__, 
	double *e, double *z__, integer *ldz, double *work, 
	integer *info);

 int dstevd_(const char *jobz, integer *n, double *d__, 
	double *e, double *z__, integer *ldz, double *work, 
	integer *lwork, integer *iwork, integer *liwork, integer *info);

 int dstevr_(const char *jobz, const char *range, integer *n, double *
	d__, double *e, double *vl, double *vu, integer *il, 
	integer *iu, double *abstol, integer *m, double *w, 
	double *z__, integer *ldz, integer *isuppz, double *work, 
	integer *lwork, integer *iwork, integer *liwork, integer *info);

int dstevx_(const char *jobz, const char *range, integer *n, double *d__, double *e, 
	double *vl, double *vu, integer *il, integer *iu, double *abstol, integer *m, 
	double *w, double *z__, integer *ldz, double *work, integer *iwork, 
	integer *ifail, integer *info);

int dsycon_(const char *uplo, integer *n, double *a, integer *lda, integer *ipiv, double *anorm,
	double *rcond, double *work, integer *iwork, integer *info);

int dsyequb_(const char *uplo, integer *n, double *a, integer *lda, double *s, double *scond,
	double *amax, double *work, integer *info);

int dsyev_(const char *jobz, const char *uplo, integer *n, double *a,
	 integer *lda, double *w, double *work, integer *lwork, integer *info);

int dsyevd_(const char *jobz, const char *uplo, integer *n, double *a, integer *lda, 
	double *w, double *work, integer *lwork, integer *iwork, integer *liwork, integer *info);

int dsyevr_(const char *jobz, const char *range, const char *uplo, integer *n, double *a,
	integer *lda, double *vl, double *vu, integer *il, integer *iu, double *abstol, integer *m,
	double *w, double *z__, integer *ldz, integer *isuppz, double *work, 
	integer *lwork, integer *iwork, integer *liwork, integer *info);

int dsyevx_(const char *jobz, const char *range, const char *uplo, integer *n,
	double *a, integer *lda, double *vl, double *vu, integer *
	il, integer *iu, double *abstol, integer *m, double *w, 
	double *z__, integer *ldz, double *work, integer *lwork, 
	integer *iwork, integer *ifail, integer *info);

int dsygs2_(integer *itype, const char *uplo, integer *n,
	double *a, integer *lda, double *b, integer *ldb, integer *
	info);

int dsygst_(integer *itype, const char *uplo, integer *n,
	double *a, integer *lda, double *b, integer *ldb, integer *
	info);

int dsygv_(integer *itype, const char *jobz, const char *uplo, integer *
	n, double *a, integer *lda, double *b, integer *ldb, 
	double *w, double *work, integer *lwork, integer *info);

int dsygvd_(integer *itype, const char *jobz, const char *uplo, integer *n, double *a, 
	integer *lda, double *b, integer *ldb, double *w, double *work, integer *lwork, 
	integer *iwork, integer *liwork, integer *info);

int dsygvx_(integer *itype, const char *jobz, const char *range, const char *uplo,
	integer *n, double *a, integer *lda, double *b, integer *ldb, double *vl, double *vu,
	integer *il, integer *iu, double *abstol, integer *m, double *w, double *z__,
	integer *ldz, double *work, integer *lwork, integer *iwork, integer *ifail, integer *info);

int dsyrfs_(const char *uplo, integer *n, integer *nrhs, 
	double *a, integer *lda, double *af, integer *ldaf, integer *
	ipiv, double *b, integer *ldb, double *x, integer *ldx, 
	double *ferr, double *berr, double *work, integer *iwork, integer *info);

int dsyrfsx_(const char *uplo, const char *equed, integer *n, integer *nrhs, double *a, 
	integer *lda, double *af, integer *ldaf, integer *ipiv, double *s, double *b, integer *ldb,
	double *x, integer *ldx, double *rcond, double *berr, integer *n_err_bnds__, 
	double *err_bnds_norm__, double *err_bnds_comp__, integer *nparams, double *params, 
	double *work, integer *iwork, integer *info);

int dsysv_(const char *uplo, integer *n, integer *nrhs, double *a, integer *lda, integer *ipiv,
	double *b, integer *ldb, double *work, integer *lwork, integer *info);

int dsysvx_(const char *fact, const char *uplo, integer *n, integer *
	nrhs, double *a, integer *lda, double *af, integer *ldaf,
	integer *ipiv, double *b, integer *ldb, double *x, integer *
	ldx, double *rcond, double *ferr, double *berr,
	double *work, integer *lwork, integer *iwork, integer *info);

int dsysvxx_(const char *fact, const char *uplo, integer *n, integer *nrhs, double *a,
	integer *lda, double *af, integer *ldaf, integer *ipiv, char *equed, double *s, double *b,
	integer *ldb, double *x, integer *ldx, double *rcond, double *rpvgrw, double *berr,
	integer *n_err_bnds__, double *err_bnds_norm__, double *err_bnds_comp__, integer *nparams,
	double *params, double *work, integer *iwork, integer *info);

int dsytd2_(const char *uplo, integer *n, double *a, integer *lda, double *d__, double *e, double *tau, integer *info);

int dsytf2_(const char *uplo, integer *n, double *a, integer *lda, integer *ipiv, integer *info);

int dsytrd_(const char *uplo, integer *n, double *a, integer *lda, double *d__, double *e, double *tau, 
	double *work, integer *lwork, integer *info);

int dsytrf_(const char *uplo, integer *n, double *a, integer *lda, integer *ipiv, double *work, 
	integer *lwork, integer *info);

int dsytri_(const char *uplo, integer *n, double *a, integer *lda, integer *ipiv, double *work, integer *info);

int dsytrs_(const char *uplo, integer *n, integer *nrhs, double *a, integer *lda, integer *ipiv,
	double *b, integer *ldb, integer *info);

int dtbcon_(const char *norm, const char *uplo, const char *diag, integer *n, integer *kd,
	double *ab, integer *ldab, double *rcond, double *work, integer *iwork, integer *info);

int dtbrfs_(const char *uplo, const char *trans, const char *diag, integer *n, integer *kd,
	integer *nrhs, double *ab, integer *ldab, double *b, integer *ldb, double *x, integer *ldx,
	double *ferr, double *berr, double *work, integer *iwork, integer *info);

int dtbtrs_(const char *uplo, const char *trans, const char *diag, integer *n, integer *kd,
	integer *nrhs, double *ab, integer *ldab, double *b, integer *ldb, integer *info);

int dtfsm_(const char *transr, const char *side, const char *uplo, const char *trans,
	 const char *diag, integer *m, integer *n, double *alpha, double *a, double *b, integer *ldb);

int dtftri_(const char *transr, const char *uplo, const char *diag, integer *n, double *a, integer *info);

int dtfttp_(const char *transr, const char *uplo, integer *n, double *arf, double *ap, integer *info);

int dtfttr_(const char *transr, const char *uplo, integer *n, double *arf, double *a, integer *lda, integer *info);

int dtgevc_(const char *side, const char *howmny, bool *select, integer *n, double *s,
	integer *lds, double *p, integer *ldp, double *vl, integer *ldvl, double *vr, 
	integer *ldvr, integer *mm, integer *m, double *work, integer *info);

int dtgex2_(bool *wantq, bool *wantz, integer *n, 
	double *a, integer *lda, double *b, integer *ldb, double *
	q, integer *ldq, double *z__, integer *ldz, integer *j1, integer *
	n1, integer *n2, double *work, integer *lwork, integer *info);

int dtgexc_(bool *wantq, bool *wantz, integer *n, 
	double *a, integer *lda, double *b, integer *ldb, double *
	q, integer *ldq, double *z__, integer *ldz, integer *ifst, 
	integer *ilst, double *work, integer *lwork, integer *info);

int dtgsen_(integer *ijob, bool *wantq, bool *wantz,
	bool *select, integer *n, double *a, integer *lda, double *
	b, integer *ldb, double *alphar, double *alphai, double *
	beta, double *q, integer *ldq, double *z__, integer *ldz, 
	integer *m, double *pl, double *pr, double *dif, 
	double *work, integer *lwork, integer *iwork, integer *liwork, 
	integer *info);

int dtgsja_(const char *jobu, const char *jobv, const char *jobq, integer *m,
	integer *p, integer *n, integer *k, integer *l, double *a, 
	integer *lda, double *b, integer *ldb, double *tola, 
	double *tolb, double *alpha, double *beta, double *u, 
	integer *ldu, double *v, integer *ldv, double *q, integer *
	ldq, double *work, integer *ncycle, integer *info);

int dtgsna_(const char *job, const char *howmny, bool *select,
	integer *n, double *a, integer *lda, double *b, integer *ldb, 
	double *vl, integer *ldvl, double *vr, integer *ldvr, 
	double *s, double *dif, integer *mm, integer *m, double *
	work, integer *lwork, integer *iwork, integer *info);

int dtgsy2_(const char *trans, integer *ijob, integer *m, integer *
	n, double *a, integer *lda, double *b, integer *ldb, 
	double *c__, integer *ldc, double *d__, integer *ldd, 
	double *e, integer *lde, double *f, integer *ldf, double *
	scale, double *rdsum, double *rdscal, integer *iwork, integer 
	*pq, integer *info);

int dtgsyl_(const char *trans, integer *ijob, integer *m, integer *
	n, double *a, integer *lda, double *b, integer *ldb, 
	double *c__, integer *ldc, double *d__, integer *ldd, 
	double *e, integer *lde, double *f, integer *ldf, double *
	scale, double *dif, double *work, integer *lwork, integer *
	iwork, integer *info);

int dtpcon_(const char *norm, const char *uplo, const char *diag, integer *n,
	double *ap, double *rcond, double *work, integer *iwork, 
	integer *info);

int dtprfs_(const char *uplo, const char *trans, const char *diag, integer *n, 
	integer *nrhs, double *ap, double *b, integer *ldb, 
	double *x, integer *ldx, double *ferr, double *berr, 
	double *work, integer *iwork, integer *info);

int dtptri_(const char *uplo, const char *diag, integer *n, double *ap, integer *info);

int dtptrs_(const char *uplo, const char *trans, const char *diag, integer *n, 
	integer *nrhs, double *ap, double *b, integer *ldb, integer *info);

int dtpttf_(const char *transr, const char *uplo, integer *n, double *ap, double *arf, integer *info);

int dtpttr_(const char *uplo, integer *n, double *ap, double *a, integer *lda, integer *info);

int dtrcon_(const char *norm, const char *uplo, const char *diag, integer *n, 
	double *a, integer *lda, double *rcond, double *work, integer *iwork, integer *info);

int dtrevc_(const char *side, const char *howmny, bool *select, integer *n, double *t, integer *ldt,
	double *vl, integer *ldvl, double *vr, integer *ldvr, integer *mm, integer *m,
	double *work, integer *info);

int dtrexc_(const char *compq, integer *n, double *t, integer *ldt, double *q, integer *ldq,
	integer *ifst, integer *ilst, double *work, integer *info);

int dtrrfs_(const char *uplo, const char *trans, const char *diag, integer *n, integer *nrhs, 
	double *a, integer *lda, double *b, integer *ldb, double *x, integer *ldx, double *ferr,
	double *berr, double *work, integer *iwork, integer *info);

int dtrsen_(const char *job, const char *compq, bool *select, integer
	*n, double *t, integer *ldt, double *q, integer *ldq, 
	double *wr, double *wi, integer *m, double *s, double 
	*sep, double *work, integer *lwork, integer *iwork, integer *
	liwork, integer *info);

int dtrsna_(const char *job, const char *howmny, bool *select, integer *n, double *t, integer *ldt,
	double *vl, integer *ldvl, double *vr, integer *ldvr, double *s, double *sep, integer *mm,
	integer *m, double *work, integer *ldwork, integer *iwork, integer *info);

int dtrsyl_(const char *trana, const char *tranb, integer *isgn, integer *m, integer *n, double *a, 
	integer *lda, double *b, integer *ldb, double *c__, integer *ldc, double *scale, integer *info);

int dtrti2_(const char *uplo, const char *diag, integer *n, double *a, integer *lda, integer *info);

int dtrtri_(const char *uplo, const char *diag, integer *n, double *a, integer *lda, integer *info);

int dtrtrs_(const char *uplo, const char *trans, const char *diag, integer *n, integer *nrhs, double *a,
	integer *lda, double *b, integer *ldb, integer *info);

int dtrttf_(const char *transr, const char *uplo, integer *n, double *a, integer *lda, double *arf, integer *info);

int dtrttp_(const char *uplo, integer *n, double *a, integer *lda, double *ap, integer *info);

int dtzrqf_(integer *m, integer *n, double *a, integer *lda, double *tau, integer *info);

int dtzrzf_(integer *m, integer *n, double *a, integer *lda, double *tau, double *work, integer *lwork, integer *info);

integer ieeeck_(integer *ispec, float *zero, float *one);

integer iladlc_(integer *m, integer *n, double *a, integer *lda);

integer iladlr_(integer *m, integer *n, double *a, integer *lda);

integer ilaenv_(integer *ispec, const char *name__, const char *opts, integer *n1, 
	integer *n2, integer *n3, integer *n4);

integer ilaprec_(const char *prec);

integer ilatrans_(const char *trans);

int ilaver_(integer *vers_major__, integer *vers_minor__, integer *vers_patch__);

integer iparmq_ (integer *ispec, const char *name__, const char *opts, integer *n, integer
	*ilo, integer *ihi, integer *lwork);

	/* Some single precision routines needed in approximations */
	
int sgetrf_(integer *m, integer *n, float *a, integer *lda, integer *ipiv, integer *info);

int sgetrs_(const char *trans, integer *n, integer *nrhs, float *a, integer *lda, integer *ipiv, 
	float *b, integer *ldb, integer *info);

int slag2d_(integer *m, integer *n, float *sa, integer *ldsa, double *a, integer *lda, integer *info);

double slamch_(const char *cmach);

#endif /* _clapack_h_ */
