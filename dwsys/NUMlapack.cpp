/* NUMlapack.cpp
 *
 * Copyright (C) 2020 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "clapack.h"

#include "NUMlapack.h"

integer getLeadingDimension (constMATVU const& m) {
	return m.rowStride > 1 ? m.rowStride : m.colStride; 
}

int NUMlapack_dgeev_ (char *jobvl, char *jobvr, integer *n, double *a, integer *lda, double *wr, double *wi, double *vl, integer *ldvl, double *vr, integer *ldvr, double *work, integer *lwork, integer *info) {
	return dgeev_ (jobvl, jobvr, n, a, lda, wr, wi,	vl, ldvl, vr, ldvr,
	work, lwork, info);
}



integer NUMlapack_dgesvd_query (char jobu, char jobvt, MATVU const& inout_a, VEC const& inout_singularValues, MATVU const& inout_u, MATVU const& inout_vt) {
	
	integer lwork = -1, info, lda = getLeadingDimension (inout_a);
	double wt;
	integer ldu = inout_a.rowStride, ldvt = inout_vt.rowStride;
	integer nrow = inout_a.nrow, ncol = inout_a.ncol;
	dgesvd_ (& jobu, & jobvt, & nrow, & ncol, & inout_a [1] [1], & lda, & inout_singularValues [1], & inout_vt [1] [1], & ldu, nullptr, & ldvt, & wt, & lwork, & info);

	return info == 0 ? (integer) (wt+0.1) : - info;
}

int NUMlapack_dgesvd_ (char *jobu, char *jobvt, integer *m, integer *n, double *a, integer *lda, double *s, double *u, integer *ldu, double *vt, integer *ldvt, double *work, integer *lwork, integer *info) {
	return dgesvd_ (jobu, jobvt, m, n, a, lda, s, u, ldu, vt, ldvt, work,
	lwork, info);
}

int NUMlapack_dggsvd_ (char *jobu, char *jobv, char *jobq, integer *m, integer *n, integer *p, integer *k, integer *l, double *a, integer *lda, double *b, integer *ldb, double *alpha, double *beta, double *u, integer *ldu, double *v, integer *ldv, double *q, integer *ldq, double *work, integer *iwork, integer *info) {
	return dggsvd_ (jobu, jobv, jobq, m, n, p, k, l, a, lda, b, ldb, alpha, beta, u, ldu, v, ldv, q, ldq, work, iwork, info);
}

int NUMlapack_dhseqr_ (char *job, char *compz, integer *n, integer *ilo, integer *ihi, double *h, integer *ldh, double *wr, double *wi, double *z, integer *ldz, double *work, integer *lwork, integer *info) {
	return dhseqr_ (job, compz, n, ilo, ihi, h, ldh, wr, wi, z, ldz, work, lwork, info);
}

int NUMlapack_dpotf2_ (char *uplo, integer *n, double *a, integer *lda, integer *info) {
	return dpotf2_ (uplo, n, a, lda, info);
}

int NUMlapack_dsyev_ (char *jobz, char *uplo, integer *n, double *a,	integer *lda, double *w, double *work, integer *lwork, integer *info) {
	return dsyev_ (jobz, uplo, n, a, lda, w, work, lwork, info);
}

int NUMlapack_dtrtri_ (char *uplo, char *diag, integer *n, double *
	a, integer *lda, integer *info) {
	return dtrtri_ (uplo, diag, n, a, lda, info);
}

int NUMlapack_dtrti2_ (char *uplo, char *diag, integer *n, double *a, integer *lda, integer *info) {
	return dtrti2_ (uplo, diag, n, a, lda, info);
}
/*End of file NUMlapack.cpp */
