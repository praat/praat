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

inline bool startsWith (conststring8 string, conststring8 startCharacter) {
	return strncmp (string, startCharacter, 1) == 0;
}

integer getLeadingDimension (constMATVU const& m, integer argumentNumber, conststring32 name) {
	if (m.nrow == 0 || m.ncol == 0)
		return 0;
	if (! (m.rowStride == 1 || m.colStride == 1))
		Melder_throw (name, U": Argument ", argumentNumber, U" has both row and column strides larger than 1. This is not allowed for LAPACK.");
	return m.rowStride;
}

int NUMlapack_dgeev_ (const char *jobvl, const char *jobvr, integer *n, double *a, integer *lda, double *wr, double *wi, double *vl, integer *ldvl, double *vr, integer *ldvr, double *work, integer *lwork, integer *info) {
	return dgeev_ (jobvl, jobvr, n, a, lda, wr, wi,	vl, ldvl, vr, ldvr,
	work, lwork, info);
}



integer NUMlapack_dgesvd_query (conststring8 jobu, conststring8 jobvt, MATVU const& inout_a, VEC const& inout_singularValues, MATVU const& inout_u, MATVU const& inout_vt) {
	conststring32 myName = U"NUMlapack_dgesvd_query";
	integer lda = getLeadingDimension (inout_a, 2, myName);
	integer ldu = getLeadingDimension (inout_u, 5, myName);
	integer ldvt = getLeadingDimension (inout_vt, 6, myName);
	integer nrow = inout_a.nrow, ncol = inout_a.ncol;
	double *p_vt = ldvt > 0 ? & inout_vt [1] [1] : nullptr;
	double *p_u = ldu > 0 ? & inout_u [1] [1] : nullptr;
	Melder_assert (inout_singularValues.size >= std::min (nrow, ncol));
	Melder_assert (! (ldu == 0 && ldvt == 0));
	Melder_assert (ldu > 0 || startsWith (jobu, "O") || startsWith (jobu, "N"));
	Melder_assert (ldvt > 0 || startsWith (jobu, "O") || startsWith (jobu, "N"));

	double wt;
	integer lwork = -1, info;
	dgesvd_ (jobu, jobvt, & nrow, & ncol, & inout_a [1] [1], & lda, & inout_singularValues [1], p_u, & ldu, p_vt, & ldvt, & wt, & lwork, & info);
	Melder_require (info == 0,
		myName, U" returns error ", info);
	return (integer) wt;
}

void NUMlapack_dgesvd (conststring8 jobu, conststring8  jobvt, MATVU const& inout_a, VEC const& inout_singularValues, MATVU const& inout_u, MATVU const& inout_vt, VEC const& work) {
	conststring32 myName = U"NUMlapack_dgesvd";
	integer lda = getLeadingDimension (inout_a, 2, myName);
	integer ldu = getLeadingDimension (inout_u, 5, myName);
	integer ldvt = getLeadingDimension (inout_vt, 6, myName);
	integer nrow = inout_a.nrow, ncol = inout_a.ncol;
	double wt;
	integer lwork = -1, info;
	dgesvd_ (jobu,  jobvt, & nrow, & ncol, & inout_a [1] [1], & lda, & inout_singularValues [1], & inout_vt [1] [1], & ldu, nullptr, & ldvt, & wt, & lwork, & info);
	Melder_require (info == 0,
		myName, U" return error ", info);
}

int NUMlapack_dgesvd_ (const char *jobu, const char *jobvt, integer *m, integer *n, double *a, integer *lda, double *s, double *u, integer *ldu, double *vt, integer *ldvt, double *work, integer *lwork, integer *info) {
	return dgesvd_ (jobu, jobvt, m, n, a, lda, s, u, ldu, vt, ldvt, work,
	lwork, info);
}

int NUMlapack_dggsvd_ (const char *jobu, const char *jobv, const char *jobq, integer *m, integer *n, integer *p, integer *k, integer *l, double *a, integer *lda, double *b, integer *ldb, double *alpha, double *beta, double *u, integer *ldu, double *v, integer *ldv, double *q, integer *ldq, double *work, integer *iwork, integer *info) {
	return dggsvd_ (jobu, jobv, jobq, m, n, p, k, l, a, lda, b, ldb, alpha, beta, u, ldu, v, ldv, q, ldq, work, iwork, info);
}

int NUMlapack_dhseqr_ (const char *job, const char *compz, integer *n, integer *ilo, integer *ihi, double *h, integer *ldh, double *wr, double *wi, double *z, integer *ldz, double *work, integer *lwork, integer *info) {
	return dhseqr_ (job, compz, n, ilo, ihi, h, ldh, wr, wi, z, ldz, work, lwork, info);
}

int NUMlapack_dpotf2_ (const char *uplo, integer *n, double *a, integer *lda, integer *info) {
	return dpotf2_ (uplo, n, a, lda, info);
}

int NUMlapack_dsyev_ (const char *jobz, const char *uplo, integer *n, double *a,	integer *lda, double *w, double *work, integer *lwork, integer *info) {
	return dsyev_ (jobz, uplo, n, a, lda, w, work, lwork, info);
}

int NUMlapack_dtrtri_ (const char *uplo, const char *diag, integer *n, double *
	a, integer *lda, integer *info) {
	return dtrtri_ (uplo, diag, n, a, lda, info);
}

int NUMlapack_dtrti2_ (const char *uplo, const char *diag, integer *n, double *a, integer *lda, integer *info) {
	return dtrti2_ (uplo, diag, n, a, lda, info);
}
/*End of file NUMlapack.cpp */
