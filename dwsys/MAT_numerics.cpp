/* MAT_numerics.cpp
 *
 * Copyright (C) 2018 David Weenink
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

#include "melder.h"
#include "NUMclapack.h"
#include "MAT_numerics.h"

#define SWAP(x,y) { tmp = x; x = y; y = tmp; }

void MAT_getEigenSystemFromSymmetricMatrix_inline (MAT a, bool wantEigenvectors, VEC eigenvalues) {
	Melder_assert (a.nrow == a.ncol);
	Melder_assert (eigenvalues.size == a.ncol);

	char jobz = wantEigenvectors ? 'V' : 'N', uplo = 'U';
	integer workSize = -1, n = a.ncol, info;
	double wt[1], tmp;
	
	// 1. Query for the size of the work array
	
	(void) NUMlapack_dsyev (& jobz, & uplo, & n, & a [1] [1], & n, & eigenvalues [1], wt, & workSize, & info);
	Melder_require (info == 0, U"dsyev initialization code = ", info, U").");
	
	workSize = Melder_iceiling (wt [0]);
	autoVEC work (workSize, kTensorInitializationType::RAW);

	// 2. Calculate the eigenvalues / eigenvectors
	
	(void) NUMlapack_dsyev (& jobz, & uplo, & n, & a [1] [1], &n, & eigenvalues [1], & work [1], & workSize, & info);
	Melder_require (info == 0, U"dsyev code = ", info, U").");

	// 3. EIgenvalues are in ascending order, we want descending order.

	for (integer i = 1; i <= n / 2; i ++) {
		integer ilast = n - i + 1;
		SWAP (eigenvalues [i], eigenvalues [ilast])
		if (wantEigenvectors) {
			for (integer j = 1; j <= n; j ++) {
				SWAP (a [i] [j], a [ilast] [j])
			}
		}
	}
}

void MAT_getEigenSystemFromSymmetricMatrix (constMAT a, autoMAT *out_eigenvectors, autoVEC *out_eigenvalues) {
	Melder_assert (a.nrow == a.ncol);
	
	bool wantEigenvectors = out_eigenvectors != nullptr;
	
	autoVEC eigenvalues (a.nrow, kTensorInitializationType::RAW);	
	autoMAT eigenvectors (a.ncol, a.ncol, kTensorInitializationType::RAW);	
	MATcopy_inplace (eigenvectors.get(), a);
	
	MAT_getEigenSystemFromSymmetricMatrix_inline (eigenvectors.get(), wantEigenvectors, eigenvalues.get());
	
	if (out_eigenvectors) *out_eigenvectors = eigenvectors.move ();
	if (out_eigenvalues) *out_eigenvalues = eigenvalues.move ();
}

void MAT_getPrincipalComponentsOfSymmetricMatrix_inline (constMAT a, integer nComponents, MAT pc) {
	Melder_assert (a.nrow == a.ncol);
	Melder_assert (nComponents > 0 && nComponents <= a.ncol);
	Melder_assert (pc.nrow == a.nrow && pc.ncol == nComponents);
	
	autoMAT eigenvectors (a.nrow, a.nrow, kTensorInitializationType::RAW);
	MAT_getEigenSystemFromSymmetricMatrix (a, & eigenvectors, nullptr);

	for (integer i = 1; i <= a.nrow; i ++) {
		for (integer j = 1; j <= nComponents; j ++) {
			longdouble s = 0.0;
			for (integer k = 1; k <= a.nrow; k ++) {
				s += a [k] [i] * eigenvectors [k] [j];
			}
			pc [i] [j] = (double) s;
		}
	}
}

bool MAT_isSymmetric (constMAT x) {
	bool isSymmetric = false;
	if (x.nrow == x.ncol) {
		for (integer i = 1; i <= x.nrow; i ++)
			for (integer j = i + 1; j < x.ncol; j ++)
				if (x [i] [j] != x [j] [i])
					return false;
		isSymmetric = true;
	}
	return isSymmetric;
}

#undef SWAP

/* End of file MAT_numerics.cpp */
