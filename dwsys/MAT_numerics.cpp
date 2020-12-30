/* MAT_numerics.cpp
 *
 * Copyright (C) 2018-2020 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "NUMlapack.h"
#include "MAT_numerics.h"
#include "SVD.h"
#include "PAIRWISE_SUM.h"

void MAT_getEigenSystemFromSymmetricMatrix_preallocated (MAT eigenvectors, VEC eigenvalues, constMATVU const& m, bool sortAscending) {
	Melder_assert (m.nrow == m.ncol);
	Melder_assert (eigenvalues.size == m.ncol);
	Melder_assert (eigenvectors.nrow == eigenvectors.ncol);
	Melder_assert (m.nrow == eigenvectors.nrow);

	integer lwork = -1, info, ncol = m.ncol;
	double wt;
	
	eigenvectors  <<=  m;
	/*
		0. No need to transpose a because it is a symmetric matrix
		1. Query for the size of the work array
	*/
	(void) NUMlapack_dsyev_ ("V", "U", ncol, & eigenvectors [1] [1], ncol, & eigenvalues [1], & wt, lwork, & info);

	Melder_require (info == 0,
		U"dsyev initialization code = ", info, U").");
	
	lwork = Melder_iceiling (wt);
	autoVEC work = raw_VEC (lwork);
	/*
		2. Calculate the eigenvalues and eigenvectors (row-wise)
	*/
	(void) NUMlapack_dsyev_ ("V", "U", ncol, & eigenvectors [1] [1], ncol, & eigenvalues [1], & work [1], lwork, & info);
	Melder_require (info == 0,
		U"dsyev code = ", info, U").");
	/*
		3. Eigenvalues are returned in ascending order
	*/
	if (! sortAscending) {
		for (integer i = 1; i <= m.ncol / 2; i ++) {
			const integer ilast = m.ncol - i + 1;
			std::swap (eigenvalues [i], eigenvalues [ilast]);
			for (integer j = 1; j <= m.ncol; j ++)
				std::swap (eigenvectors [i] [j], eigenvectors [ilast] [j]);
		}
	}
}

void MAT_getEigenSystemFromSymmetricMatrix (constMAT a, autoMAT *out_eigenvectors, autoVEC *out_eigenvalues, bool sortAscending) {
	Melder_assert (a.nrow == a.ncol);	
	autoVEC eigenvalues = raw_VEC (a.nrow);
	autoMAT eigenvectors = raw_MAT (a.nrow, a.ncol);	
	
	MAT_getEigenSystemFromSymmetricMatrix_preallocated (eigenvectors.get(), eigenvalues.get(), a, sortAscending);
	
	if (out_eigenvectors)
		*out_eigenvectors = eigenvectors.move ();
	if (out_eigenvalues)
		*out_eigenvalues = eigenvalues.move ();
}

void MAT_getEigenSystemFromGeneralSquareMatrix (constMAT const& data, autoCOMPVEC *out_eigenvalues, automatrix<dcomplex> *out_eigenvectors) {
	if (! (out_eigenvalues || out_eigenvectors))
		return;
	Melder_assert (data.nrow == data.ncol);
	autoMAT a = transpose_MAT (data);   // lapack needs column major layout
	autoVEC eigenvalues_re = raw_VEC (a.nrow);
	autoVEC eigenvalues_im = raw_VEC (a.nrow);
	autoMAT eigenvectors_right;
	double *p_evec_right = nullptr;
	if (out_eigenvectors) {
		eigenvectors_right = raw_MAT (a.nrow, a.nrow);
		p_evec_right = & eigenvectors_right [1] [1];
	}

	double wtmp[3];
	integer lwork = -1, info;
	const char *jobvr = ( out_eigenvectors ? "V" : "N" );
	NUMlapack_dgeev_ ("N", jobvr, a.nrow, & a [1] [1], a.nrow, & eigenvalues_re [1], & eigenvalues_im [1],
		nullptr, a.nrow, p_evec_right, a.nrow, & wtmp [1], lwork, & info);
	Melder_require (info == 0,
		U"NUMlapack_dgeev_ query returns error ", info, U".");
	
	lwork = Melder_iceiling (wtmp [1]);
	autoVEC work = raw_VEC (lwork);
	NUMlapack_dgeev_ ("N", jobvr, a.nrow, & a [1] [1], a.nrow, & eigenvalues_re [1], & eigenvalues_im [1],
		nullptr, a.nrow, p_evec_right, a.nrow, & work [1], lwork, & info);
	integer numberOfEigenvalues = a.nrow, istart = 0;
	if (info > 0)
		istart = info; // only evals [info+1: a.nrow], no eigenvectors
		
	if (out_eigenvalues) {
		numberOfEigenvalues -= istart;
		autoCOMPVEC eigenvalues = raw_COMPVEC (numberOfEigenvalues); // vec in column
		for (integer ival = 1; ival <= numberOfEigenvalues; ival ++) {
			eigenvalues [ival] .real (eigenvalues_re [istart + ival]);
			eigenvalues [ival] .imag (eigenvalues_im [istart + ival]);
		}
		*out_eigenvalues = eigenvalues.move();
	}
	if (out_eigenvectors && istart == 0) {
		automatrix<dcomplex> evecs = newmatrixzero<dcomplex> (numberOfEigenvalues, a.nrow);
		constvector<dcomplex> evals = (*out_eigenvalues).get();
		/*
			The following processing is based on the fact that the eigenvalues
			are either real or occur in pairs (a+ib, a-ib).
		*/
		integer ivec = 1;
		while (ivec <= numberOfEigenvalues) {
			for (integer icol = 1; icol <= a.nrow; icol ++)
				evecs [ivec] [icol] .real (eigenvectors_right [icol] [ivec]);
			// imaginary part is zero by initialisation
			if (evals [ivec].imag() != 0.0) {
				for (integer icol = 1; icol <= a.nrow; icol ++) {
					evecs [ivec    ] [icol] .imag ( eigenvectors_right [icol][ivec + 1]);
					evecs [ivec + 1] [icol] .real ( eigenvectors_right [icol][ivec    ]);
					evecs [ivec + 1] [icol] .imag (-eigenvectors_right [icol][ivec + 1]);
				}
				ivec ++;
			}
			ivec ++;
		}
		*out_eigenvectors = evecs.move();
	}
}

void MAT_asPrincipalComponents_preallocated (MATVU result, constMATVU const& m, integer numberOfComponents) {
	Melder_assert (numberOfComponents  > 0 && numberOfComponents <= m.ncol);
	Melder_assert (result.nrow == m.nrow && result.ncol == numberOfComponents);
	autoSVD svd = SVD_createFromGeneralMatrix (m);
	mul_MAT_out (result, m, svd -> v.verticalBand (1, result.ncol));
}

autoMAT MAT_asPrincipalComponents (constMATVU m, integer numberOfComponents) {
	Melder_assert (numberOfComponents  > 0 && numberOfComponents <= m.ncol);
	autoMAT result = raw_MAT (m.nrow, numberOfComponents);
	MAT_asPrincipalComponents_preallocated (result.get(), m, numberOfComponents);
	return result;
}

void MATpseudoInverse (MATVU const& target, constMATVU const& mat, double tolerance) {
	Melder_assert (target.nrow == mat.ncol && target.ncol == mat.nrow);
	autoSVD me = SVD_createFromGeneralMatrix (mat);
	(void) SVD_zeroSmallSingularValues (me.get(), tolerance);
	for (integer irow = 1; irow <= target.nrow; irow ++) {
		for (integer icol = 1; icol <= target.ncol; icol ++) {
			PAIRWISE_SUM (
				longdouble, sum,
				integer, mat.ncol,
				integer k = 1,
				my d [k] == 0.0 ? 0.0 : my v [irow] [k] * my u [icol] [k] / my d [k],
				k += 1
			)
			target [irow] [icol] = double (sum);
		}
	}
}

autoMAT newMATpseudoInverse (constMATVU const& mat, double tolerance) {
	autoMAT result = raw_MAT (mat.ncol, mat.nrow);
	MATpseudoInverse (result.all(), mat, tolerance);
	return result;
}

/* End of file MAT_numerics.cpp */
