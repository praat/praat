/* MAT_numerics.cpp
 *
 * Copyright (C) 2018-2019 David Weenink
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

#include "NUMclapack.h"
#include "MAT_numerics.h"
#include "SVD.h"
#include "PAIRWISE_SUM.h"

void MAT_getEigenSystemFromSymmetricMatrix_preallocated (MAT eigenvectors, VEC eigenvalues, constMATVU const& m, bool sortAscending) {
	Melder_assert (m.nrow == m.ncol);
	Melder_assert (eigenvalues.size == m.ncol);
	Melder_assert (eigenvectors.nrow == eigenvectors.ncol);
	Melder_assert (m.nrow == eigenvectors.nrow);

	char jobz = 'V', uplo = 'U';
	integer workSize = -1, info, ncol = m.ncol;
	double wt [1];
	
	eigenvectors <<= m;
	// 0. No need to transpose a because it is a symmetric matrix
	// 1. Query for the size of the work array
	
	(void) NUMlapack_dsyev (& jobz, & uplo, & ncol, & eigenvectors [1] [1], & ncol, eigenvalues.begin(), wt, & workSize, & info);
	Melder_require (info == 0, U"dsyev initialization code = ", info, U").");
	
	workSize = Melder_iceiling (wt [0]);
	autoVEC work = newVECraw (workSize);

	// 2. Calculate the eigenvalues and eigenvectors (row-wise)
	
	(void) NUMlapack_dsyev (& jobz, & uplo, & ncol, & eigenvectors [1] [1], & ncol, eigenvalues.begin(), work.begin(), & workSize, & info);
	Melder_require (info == 0, U"dsyev code = ", info, U").");

	// 3. Eigenvalues are returned in ascending order

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
	autoVEC eigenvalues = newVECraw (a.nrow);
	autoMAT eigenvectors = newMATraw (a.nrow, a.ncol);	
	
	MAT_getEigenSystemFromSymmetricMatrix_preallocated (eigenvectors.get(), eigenvalues.get(), a, sortAscending);
	
	if (out_eigenvectors)
		*out_eigenvectors = eigenvectors.move ();
	if (out_eigenvalues)
		*out_eigenvalues = eigenvalues.move ();
}

void MAT_eigenvectors_decompress (constMAT eigenvectors, constVEC eigenvalues_re, constVEC eigenvalues_im, autoMAT *out_eigenvectors_reim) {
	const integer n = eigenvalues_re.size;	
	Melder_assert (eigenvalues_im.size == n);
	Melder_assert (eigenvectors.nrow == n && eigenvectors.ncol == eigenvectors.nrow);
		
	autoMAT eigenvectors_reim = newMATzero (n, 2 * n);
	integer pair_index = 0;
	for (integer ivec = 1; ivec <= eigenvalues_re.size; ivec ++) {
		// eigenvalues of a real matrix are either real or occur in complex conjugate pairs
		
		if (eigenvalues_im [ivec] == 0.0) { // real eigenvalue
			for (integer j = 1; j <= n; j ++)
				eigenvectors_reim [j] [2 * ivec - 1] = eigenvectors [ivec] [j];
		} else if (ivec > 1 && eigenvalues_re [ivec] == eigenvalues_re [ivec - 1] && 
			eigenvalues_im [ivec] == -eigenvalues_im [ivec - 1] && ivec - pair_index > 1) {
			for (integer j = 1; j <= n; j ++) {
				eigenvectors_reim [j] [2 * (ivec - 1) - 1]= eigenvectors [ivec - 1] [j];
				eigenvectors_reim [j] [2 * (ivec - 1)] = eigenvectors [ivec] [j];
				eigenvectors_reim [j] [2 * ivec - 1]  = eigenvectors [ivec - 1] [j];
				eigenvectors_reim [j] [2 * ivec] = eigenvectors [ivec] [j] != 0.0 ? -eigenvectors [ivec] [j] : 0.0; // avoid -0
			}
			pair_index = ivec; // guard for multiple equal complex conjugate pairs
		}
	}
	if (out_eigenvectors_reim)
		*out_eigenvectors_reim = eigenvectors_reim.move();
}

void MAT_getEigenSystemFromGeneralMatrix (constMAT a, autoMAT *out_lefteigenvectors, autoMAT *out_righteigenvectors, autoVEC *out_eigenvalues_re, autoVEC *out_eigenvalues_im) {
	Melder_assert (a.nrow == a.ncol);
	integer n = a.nrow, info, workSize = -1;
	char jobvl = out_lefteigenvectors ? 'V' : 'N';
	integer left_nvecs = out_lefteigenvectors ? n : 1;   // 1x1 if not wanted
	char jobvr = out_righteigenvectors ? 'V' : 'N';
	integer right_nvecs = out_righteigenvectors ? n : 1;
	double wt;
	
	autoMAT data = newMATtranspose (a);   // lapack is fortran storage
	autoVEC eigenvalues_re = newVECraw (n);
	autoVEC eigenvalues_im = newVECraw (n);
	autoMAT righteigenvectors = newMATraw (right_nvecs, right_nvecs); // 1x1 if not needed
	autoMAT lefteigenvectors = newMATraw (left_nvecs, left_nvecs); // 1x1 if not needed
	
	(void) NUMlapack_dgeev (& jobvl, & jobvr, & n, & data [1] [1], & n,	eigenvalues_re.begin(), eigenvalues_im.begin(),	& lefteigenvectors [1] [1],
		& n, & righteigenvectors [1] [1], & n, & wt, & workSize, & info);
	Melder_require (info == 0, U"dgeev initialization code = ", info, U").");
	
	workSize = Melder_iceiling (wt);
	autoVEC work = newVECraw (workSize);
	
	(void) NUMlapack_dgeev (& jobvl, & jobvr, & n, & data [1] [1], & n, eigenvalues_re.begin(), eigenvalues_im.begin(),	& lefteigenvectors [1] [1],
		& n, & righteigenvectors [1] [1], & n, & work [1], & workSize, & info);
	Melder_require (info == 0, U"dgeev code = ", info, U").");
	
	if (out_righteigenvectors)
		*out_righteigenvectors = righteigenvectors.move();
	if (out_lefteigenvectors)
		*out_lefteigenvectors = lefteigenvectors.move();
	if (out_eigenvalues_re)
		*out_eigenvalues_re = eigenvalues_re.move();
	if (out_eigenvalues_im)
		*out_eigenvalues_im = eigenvalues_im.move();
}

void MAT_asPrincipalComponents_preallocated (MATVU result, constMATVU const& m, integer numberOfComponents) {
	Melder_assert (numberOfComponents  > 0 && numberOfComponents <= m.ncol);
	Melder_assert (result.nrow == m.nrow && result.ncol == numberOfComponents);
	autoSVD svd = SVD_createFromGeneralMatrix (m);
	MATmul (result, m, svd -> v.verticalBand (1, result.ncol));
}

autoMAT MAT_asPrincipalComponents (constMATVU m, integer numberOfComponents) {
	Melder_assert (numberOfComponents  > 0 && numberOfComponents <= m.ncol);
	autoMAT result = newMATraw (m.nrow, numberOfComponents);
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
	autoMAT result = newMATraw (mat.ncol, mat.nrow);
	MATpseudoInverse (result.all(), mat, tolerance);
	return result;
}

/* End of file MAT_numerics.cpp */
