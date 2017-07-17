#ifndef _Eigen_h_
#define _Eigen_h_
/* Eigen.h
 *
 * Copyright (C) 1993-2012, 2015 David Weenink
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

#include "Collection.h"
#include "Graphics.h"
#include "Strings_.h"

#include "Eigen_def.h"

autoEigen Eigen_create (long numberOfEigenvalues, long dimension);

void Eigen_init (Eigen me, long numberOfEigenvalues, long dimension);

void Eigen_initFromSymmetricMatrix (Eigen me, double **a, long n);

void Eigen_initFromSquareRoot (Eigen me, double **a, long numberOfRows, long numberOfColumns);
/*
	Calculate eigenstructure for symmetric matrix A'A (e.g. covariance matrix),
	when only A is given.
	Precondition: numberOfRows > 1
	Method: SVD.
*/

void Eigen_initFromSquareRootPair (Eigen me, double **a, long numberOfRows, long numberOfColumns, double **b, long numberOfRows_b);
/*
	Calculate eigenstructure for A'Ax - lambda B'Bx = 0
	Preconditions: numberOfRows >= numberOfColumns &&
		numberOfRows_b >= numberOfColumns
	Method: Generalized SVD.
*/

long Eigen_getNumberOfEigenvectors (Eigen me);

long Eigen_getDimensionOfComponents (Eigen me);

double Eigen_getCumulativeContributionOfComponents (Eigen me, long from, long to);

long Eigen_getDimensionOfFraction (Eigen me, double fraction);

double Eigen_getEigenvectorElement (Eigen me, long ivec, long element);

double Eigen_getSumOfEigenvalues (Eigen me, long from, long to);


void Eigen_sort (Eigen me);
/*
	Sort eigenvalues and corresponding eigenvectors in decreasing order.
*/

void Eigen_invertEigenvector (Eigen me, long ivec);

void Eigen_drawEigenvalues (Eigen me, Graphics g, long first, long last, double ymin, double ymax,
	bool fractionOfTotal, bool cumulative, double size_mm, const char32 *mark, bool garnish);

void Eigen_drawEigenvector (Eigen me, Graphics g, long ivec, long first, long last, double minimum, double maximum, bool weigh,
	double size_mm, const char32 *mark, bool connect, char32 **rowLabels, bool garnish);
/*
	Draw eigenvector. When rowLabels != nullptr, draw row text labels on bottom axis.
*/

/**
	Adapt the sign of each eigenvector except the first
	in such a way that it correlates positively with the first eigenvector.
*/
void Eigens_alignEigenvectors (OrderedOf<structEigen>* me);

double Eigens_getAngleBetweenEigenplanes_degrees (Eigen me, Eigen thee);
/*
	Get angle between the eigenplanes, spanned by the first two eigenvectors, .
*/

void Eigen_and_matrix_into_matrix_principalComponents (Eigen me, double **from, long numberOfRows, long from_colbegin, double **to, long numberOfDimensionsToKeep, long to_colbegin);
/*
 * Preconditions:
 * 
 * 	from[numberOfRows, from_colbegin - 1 + my dimension] exists
 * 	to [numberOfRows, to_colbegin - 1 + numberOfDimensionsToKeep] exists
 * 
 * Project the vectors in matrix 'from' along the 'numberOfDimensionsToKeep' eigenvectors into the matrix 'to'.
 */


#endif /* _Eigen_h_ */

