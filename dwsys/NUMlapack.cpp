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

#include "NUMlapack.h"


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


/*End of file NUMlapack.cpp */
