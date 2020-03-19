#pragma once
/* MAT_numerics.h
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

#include "melder.h"

void MAT_getEigenSystemFromSymmetricMatrix (constMAT a, autoMAT *out_eigenvectors, autoVEC *out_eigenvalues, bool sortAscending);
/*
	Calculates the eigenvectors and eigenvalues of a symmetric matrix;
	Input:
		a, a symmetric matrix of size nrow x nrow (a.ncol == a.nrow)
		   only the upper-half of the matrix is used in the calculation
		sortAscending if true eigenvalues (and corresponding eigenvectors) are sorted ascending
	Output:
		if (out_eigenvalues) eigenvalues sorted
		if (out_eigenvectors) eigenvectors corresponding to the eigenvalues, stored as row-wise vectors.
*/

void MAT_getEigenSystemFromSymmetricMatrix_preallocated (MAT eigenvectors, VEC eigenvalues, constMATVU const& a, bool sortAscending);
/*
	Input:
		a, a symmetric a.ncol x a.ncol matrix
		   only the upper-half of the matrix is used in the calculation
		eigenvalues, a vector of size ncol
		eigenvectors, a matrix a.ncol x a.ncol
		sortAscending if true eigenvalues (and corresponding eigenvectors) will be sorted ascending
	Output:
		eigenvectors, stored row-wise
		eigenvalues, eigenvalues sorted according to sortAscending
*/

void MAT_getEigenSystemFromGeneralSquareMatrix (constMAT const& inout_a, autoCOMPVEC *out_eigenvalues, automatrix<dcomplex> *out_eigenvectors);
/* no standard sorting with complex numbers.
	Compute eigenvalues of general nxn matrix with right eigenvectors.
	There is no standard sorting with complex numbers.
	Input:
	Output:
		inout_a has been overwriten
		out_eigenvalues
			the eigenvalues.
			Complex conjugate pairs of eigenvalues appear consecutively
            with the eigenvalue having the positive imaginary part first.
		out_eigenvectors
			the eigenvectors (stored row-wise!)
*/

void MAT_asPrincipalComponents_preallocated (MATVU pc, constMATVU const& m, integer numberOfComponents);
autoMAT MAT_asPrincipalComponents (constMATVU m, integer numberOfComponents);

void MATpseudoInverse (MATVU const& target, constMATVU const& mat, double tolerance);
autoMAT newMATpseudoInverse (constMATVU const& mat, double tolerance);
/*
	Determines the pseudo-inverse Y^-1 of Y[1..nrow][1..ncol] via s.v.d.
	Alternative notation for pseudo-inverse: (Y'.Y)^-1.Y'
	Returns a [1..ncol][1..nrow] matrix
*/

/* End of file MAT_numerics.h */
