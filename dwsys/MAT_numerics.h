#pragma once
/* MAT_numerics.h
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

inline static void MATcopyTranspose_inplace (MAT x, constMAT source) {
	Melder_assert (x.nrow == source.ncol && x.ncol == source.nrow);
	for (integer irow = 1; irow <= source.nrow; irow ++)
		for (integer icol = 1; icol <= source.ncol; icol ++)
			x [icol] [irow] = source [irow] [icol];
}

inline static autoMAT MATtranspose (constMAT x) {
	autoMAT result = MATraw (x.nrow, x.ncol);
	for (integer irow = 1; irow <= x.nrow; irow ++)
		for (integer icol = 1; icol <= x.ncol; icol ++)
			result [icol] [irow] = x [irow] [icol];
	return result;
}

bool MAT_isSymmetric (constMAT x);
/*
	Returns true if the matrix is symmetric else false.
*/

void MAT_getEigenSystemFromSymmetricMatrix (constMAT a, autoMAT *out_eigenvectors, autoVEC *out_eigenvalues, bool sortAscending);
/*
	Calculates the eigenvectors and eigenvalues of a symmetric matrix;
	Input:
		a, a symmetric matrix of size nrow x nrow (a.ncol == a.nrow)
		   only the upper-half of the matrix is used in the calculation
		sortAscending if true eigenvalues (and corresponding eigenvectors) are sorted ascending
	Output:
		if(out_eigenvalues) eigenvalues sorted
		if(out_eigenvectors) eigenvectors corresponding to the eigenvalues, stored as row-wise vectors.
*/

void MAT_getEigenSystemFromSymmetricMatrix_inplace (MAT inout_a, bool wantEigenvectors, VEC inout_eigenvalues, bool sortAscending);
/*
	Input:
		inout_a, a symmetric a.ncol x a.ncol matrix
		   only the upper-half of the matrix is used in the calculation
		inout_eigenvalues, a vector of size ncol
		wantEigenvectors, true if you want eigenvectors calculated
		sortAscending if true eigenvalues (and corresponding eigenvectors) are sorted ascending
	Output:
		inout_a, if (wantEigenvectors) eigenvectors, stored row-wise
		inout_eigenvalues, eigenvalues sorted from large to small
*/

void MAT_getPrincipalComponentsOfSymmetricMatrix_inplace (constMAT a, integer nComponents, MAT inout_pc);
/*
	Input:
		a, a symmetric nrow x nrow matrix
		   only the upper-half of the matrix is used in the calculation
		nComponents, the number of components to determine (1 <= nComponents <= a.nrow)
		inout_pc, a a.nrow x nComponents matrix
	Output:
		inout_pc, the principal components stored column-wise
*/

void MAT_getEigenSystemFromGeneralMatrix (constMAT a, autoMAT *out_lefteigenvectors, autoMAT *out_righteigenvectors, autoVEC *out_eigenvalues_re, autoVEC *out_eigenvalues_im);
/* no standard sorting with complex numbers.
	Compute eigenvalues of general nxn matrix with optionally the left/right eigenvectors.
	There is no standard sorting with complex numbers.
	Input:
	Output:
		out_eigenvalues_re, out_eigenvalues_im
			the real and imaginary parts of the eigenvalues.
			Complex conjugate pairs of eigenvalues appear consecutively
            with the eigenvalue having the positive imaginary part first.
		out_righteigenvectors, out_lefteigenvectors:
			the left and right eigenvectors (stored row-wise!, compressed)
			if the j-th eigenvalue is real the eigenvector is real and in row j.
			if j and j+1 form a complex conjugate pair, the two eigenvectors are 
			complex conjugates, whose real part is in j and its imaginary part in j+1.
*/

void MAT_eigenvectors_decompress (constMAT eigenvectors, constVEC eigenvalues_re, constVEC eigenvalues_im, autoMAT *out_eigenvectors_reim);
/*
	Decompresses each eigenvector row into two consecutive columns (real and imaginary part)
*/

/* End of file MAT_numerics.h */
