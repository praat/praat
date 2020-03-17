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

static inline bool isRowMajor (constMATVU const& m) {
	return m.firstCell && m.colStride == 1 && m.rowStride >= 1;
}
static inline bool isColMajor (constMATVU const& m) {
	return m.firstCell && m.rowStride == 1 && m.colStride >= 1;
}

void VEClayout_asColMajor (VEC const& result, constMATVU const& in) {
	Melder_assert (result.size  == in.nrow * in.ncol);
	for (integer irow = 1; irow <= in.nrow; irow ++)
		for (integer icol = 1; icol <= in.ncol; icol ++)
			result [(icol - 1) * in.nrow + irow] = in [irow] [icol];
}

void MATlayout_asRowMajor (MATVU result, constVEC in) {
	Melder_assert (in.size  == result.nrow * result.ncol);
	for (integer irow = 1; irow <= result.nrow; irow ++)
		for (integer icol = 1; icol <= result.ncol; icol ++)
			result [irow] [icol] = in [(icol - 1) * result.nrow + irow];
}

integer getLeadingDimension (constMATVU const& m) {
	Melder_assert (! (m.rowStride > 1 && m.colStride > 1));
	/*
		A column major layout has columnStride > 1 and a rowStride == 1;
		a row major layout has colStride == 1 and a rowStride > 1.
		An empty MATVU has rowStride = 0 and colStride = 1.
	*/
	return ( m.rowStride == 1 ? m.colStride : m.rowStride );
}

int NUMlapack_dgeev_ (const char *jobvl, const char *jobvr, integer *n, double *a, integer *lda, double *wr, double *wi, double *vl, integer *ldvl, double *vr, integer *ldvr, double *work, integer *lwork, integer *info) {
	return dgeev_ (jobvl, jobvr, n, a, lda, wr, wi,	vl, ldvl, vr, ldvr,
	work, lwork, info);
}

integer NUMlapack_dgesvd_query (constMATVU const& a, constMATVU const& u, constVEC const& singularValues, constMATVU const& vt) {
	Melder_assert (a.nrow >= a.ncol);
	Melder_assert (a.nrow == u.nrow && a.ncol == u.ncol);
	Melder_assert (singularValues.size == a.ncol);
	Melder_assert (a.ncol == vt.nrow);
	Melder_assert (vt.nrow == vt.ncol);
	
	constMATVU a_f = a.transpose ();
	constMATVU u_f = vt.transpose (); // yes, we swap u and vt !
	constMATVU vt_f = u.transpose ();
	integer nrow = a_f.nrow, ncol = a_f.ncol;
	integer lda = getLeadingDimension (a_f);
	integer ldu = getLeadingDimension (u_f);
	integer ldvt = getLeadingDimension (vt_f);
	conststring8 jobu = "Small", jobvt = "Small";
	double workSize;
	integer lwork = -1, info;
	dgesvd_ (jobu, jobvt, & nrow, & ncol, const_cast<double *> (& a_f [1] [1]), & lda, const_cast<double *> (& singularValues [1]), const_cast<double *> (& u_f [1] [1]), & ldu, const_cast<double *> (& vt_f [1] [1]), & ldvt, & workSize, & lwork, & info);
	Melder_require (info == 0,
		U"NUMlapack_dgesvd_query returns error ", info, U".");
	return (integer) workSize;
}

void NUMlapack_dgesvd (constMATVU const& inout_a, MATVU const& inout_u, VEC const& inout_singularValues, MATVU const& inout_vt, VEC const& work) {
	Melder_assert (inout_a.nrow >= inout_a.ncol);
	Melder_assert (inout_a.nrow == inout_u.nrow && inout_a.ncol == inout_u.ncol);
	Melder_assert (inout_singularValues.size == inout_a.ncol);
	Melder_assert (inout_a.ncol == inout_vt.nrow);
	Melder_assert (inout_vt.nrow == inout_vt.ncol);
	/*
		Compute svd(A) = U D Vt.
		The svd routine dgesvd_ from CLAPACK uses (fortran) column major storage, while	C uses row major storage.
		To solve the problem above we have to transpose the matrix A, calculate the
		solution and transpose the U and Vt matrices of the solution.
		However, if we solve the transposed problem svd(A') = V D U', we have less work to do:
		We may call the dgesvd_ algorithm with reverted row/column dimensions, and we switch the U and V'
		output arguments.
	*/
	constMATVU a_f = inout_a.transpose ();
	MATVU u_f = inout_vt.transpose (); // swap u and vt !
	MATVU vt_f = inout_u.transpose ();
	integer nrow = a_f.nrow, ncol = a_f.ncol;
	integer lda = getLeadingDimension (a_f);
	integer ldu = getLeadingDimension (u_f);
	integer ldvt = getLeadingDimension (vt_f);
	conststring8 jobu = "Small", jobvt = "Small";
	integer lwork = work.size, info;
	dgesvd_ (jobu, jobvt, & nrow, & ncol, const_cast<double *> (& a_f [1] [1]), & lda, & inout_singularValues [1], & u_f [1] [1], & ldu, & vt_f [1] [1], & ldvt, & work [1], & lwork, & info);
	Melder_require (info == 0,
		U"NUMlapack_dgesvd returns error ", info);
}

int NUMlapack_dgesvd_ (const char *jobu, const char *jobvt, integer *m, integer *n, double *a, integer *lda, double *s, double *u, integer *ldu, double *vt, integer *ldvt, double *work, integer *lwork, integer *info) {
	return dgesvd_ (jobu, jobvt, m, n, a, lda, s, u, ldu, vt, ldvt, work,
	lwork, info);
}

int NUMlapack_dggsvd_ (const char *jobu, const char *jobv, const char *jobq, integer *m, integer *n, integer *p, integer *k, integer *l, double *a, integer *lda, double *b, integer *ldb, double *alpha, double *beta, double *u, integer *ldu, double *v, integer *ldv, double *q, integer *ldq, double *work, integer *iwork, integer *info) {
	return dggsvd_ (jobu, jobv, jobq, m, n, p, k, l, a, lda, b, ldb, alpha, beta, u, ldu, v, ldv, q, ldq, work, iwork, info);
}

static integer dhseqr_getColMajorWorkspaceSize (constMATVU const& upperHessenberg, constMATVU const& z) {
	// TODO only needed if matrices have row major layout!!
	integer nrow = upperHessenberg.nrow;
	integer size = 2 * nrow; // extra for the wr and wi arrays
	if (! isColMajor (upperHessenberg))
		size += nrow * nrow;
	size += z.nrow * z.ncol;
	return size;
}

integer NUMlapack_dhseqr_query (constMATVU const& upperHessenberg, constCOMPVECVU const& eigenvalues, constMATVU const& z) {
	Melder_assert (upperHessenberg.nrow == upperHessenberg.ncol);
	Melder_assert (eigenvalues.size == upperHessenberg.nrow);
	integer nrow = upperHessenberg.nrow;
	integer ldh = getLeadingDimension (upperHessenberg);
	integer ldz = getLeadingDimension (z);
	Melder_assert (ldz == 0 || z.nrow == z.ncol && z.nrow == upperHessenberg.nrow);
	conststring8 job = ( ldz == 0 ? "Eigenvalues" : "S" );
	conststring8 compz = ( ldz == 0 ? "NoSchurvectors" : "IsUnitMatrixBeforeSchurVectorsReturned" );
	ldz = ldh; // even if not used must be of correct size
	integer ilo = 1, ihi = nrow, lwork = -1, info;
	double workSize, wr, wi;
	dhseqr_ (job, compz, & nrow, & ilo, & ihi, const_cast<double *> (& upperHessenberg [1] [1]), & ldh, & wr, & wi, const_cast<double *> (& z [1] [1]), & ldz, & workSize, & lwork, & info);
	Melder_require (info == 0,
		U"NUMlapack_dhseqr_query returns error ", info, U".");
	integer workspace = (integer) workSize;
	integer extraWorkspace = dhseqr_getColMajorWorkspaceSize (upperHessenberg, z);
	return workspace + extraWorkspace;
}

integer NUMlapack_dhseqr (constMATVU const& inout_upperHessenberg, COMPVECVU const& inout_eigenvalues, MATVU const& inout_z, VEC const& work) {
	Melder_assert (inout_upperHessenberg.nrow == inout_upperHessenberg.ncol);
	Melder_assert (inout_eigenvalues.size == inout_upperHessenberg.nrow);
	integer workSize = NUMlapack_dhseqr_query (inout_upperHessenberg, inout_eigenvalues, inout_z);
	Melder_assert (work.size >= workSize);
	integer nrow = inout_upperHessenberg.nrow;
	integer ldh = nrow, ldz = inout_z.nrow;
	Melder_assert (ldz == 0 || nrow == inout_z.ncol && inout_z.nrow == nrow);
	conststring8 job = ( ldz == 0 ? "Eigenvalues" : "S" );
	conststring8 compz = ( ldz == 0 ? "NoSchurvectorsNeeded" : "IsUnitMatrixBeforeSchurVectorsReturned" );
	ldz = ldh; // even if not used must be of correct size
	integer endIndex = 0;
	VEC wr = work.part (endIndex + 1, endIndex + nrow);
	endIndex += nrow;
	VEC wi = work.part (endIndex + 1, endIndex + nrow);
	endIndex += nrow;
	/*
		Change to fortran layout if necessary
	*/
	double *p_uh = const_cast<double *> (& inout_upperHessenberg [1] [1]);
	if (! isColMajor (inout_upperHessenberg)) {
		VEC uh_CM = work.part (endIndex + 1, endIndex + nrow * nrow);
		endIndex += nrow * nrow;
		VEClayout_asColMajor (uh_CM, inout_upperHessenberg);
		p_uh = & uh_CM [1];
	}
	double *p_z = & inout_z [1] [1];
	if (inout_z.nrow > 0) {
		VEC z_CM (& work [endIndex + 1], inout_z.nrow * inout_z.nrow, false);
		VEClayout_asColMajor (z_CM, inout_z);
		p_z = & work [endIndex + 1];
		endIndex += inout_z.nrow * inout_z.nrow;
	}
	integer ilo = 1, ihi = nrow, lwork = work.size - endIndex, info = 0;
	dhseqr_ (job, compz, & nrow, & ilo, & ihi, p_uh, & ldh, & wr [1], & wi [1], p_z, & ldz, & work [endIndex + 1], & lwork, & info);
	integer numberOfEigenvaluesFound = nrow, ioffset = 0;
	if (info > 0) {
		/*
			if INFO = i, NUMlapack_dhseqr failed to compute all of the eigenvalues. Elements i+1:n of
			WR and WI contain those eigenvalues which have been successfully computed
		*/
		numberOfEigenvaluesFound -= info;
		Melder_require (numberOfEigenvaluesFound > 0,
			U"No eigenvalues found.");
		ioffset = info;
	} else if (info < 0) {
		Melder_throw (U"NUMlapack_dhseqr returns error ", info, U".");
	}

	for (integer i = 1; i <= numberOfEigenvaluesFound; i ++) {
		inout_eigenvalues [i]. real (wr [ioffset + i]);
		inout_eigenvalues [i]. imag (wi [ioffset + i]);
	}
	/*
		Restore z to row major if necessary
	*/
	if (inout_z.nrow > 0)
		MATlayout_asRowMajor (inout_z, constVEC (p_z, nrow, false));
	return numberOfEigenvaluesFound;
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
