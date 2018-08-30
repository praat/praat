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

bool MAT_isSymmetric (constMAT x);
/*
	Returns true if the matrix is symmetric else false.
*/

void MAT_getEigenSystemFromSymmetricMatrix (constMAT a, autoMAT *out_eigenvectors, autoVEC *out_eigenvalues);
/*
	Calculates the eigenvectors and eigenvalues of a symmetric matrix;
	Input:
		a, a symmetric matrix of size nrow x nrow (a.ncol == a.nrow)
		   only the upper-half of the matrix is used in the calculation
	Output:
		if(out_eigenvalues) eigenvalues sorted from large to small
		if(out_eigenvectors) eigenvectors corresponding to the eigenvalues, stored as row-wise vectors.
*/

void MAT_getEigenSystemFromSymmetricMatrix_inline (MAT inout_a, bool wantEigenvectors, VEC inout_eigenvalues);
/*
	Input:
		inout_a, a symmetric a.ncol x a.ncol matrix
		   only the upper-half of the matrix is used in the calculation
		inout_eigenvalues, a vector of size ncol
		wantEigenvectors, true if you want eigenvectors calculated
	Output:
		inout_a, if (wantEigenvectors) eigenvectors, stored row-wise
		inout_eigenvalues, eigenvalues sorted from large to small
*/

void MAT_getPrincipalComponentsOfSymmetricMatrix_inline (constMAT a, integer nComponents, MAT inout_pc);
/*
	Input:
		a, a symmetric nrow x nrow matrix
		   only the upper-half of the matrix is used in the calculation
		nComponents, the number of components to determine (1 <= nComponents <= a.nrow)
		inout_pc, a a.nrow x nComponents matrix
	Output:
		inout_pc, the principal components stored column-wise
*/

/* End of file MAT_numerics.h */
