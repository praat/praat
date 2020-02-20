#ifndef _Eigen_h_
#define _Eigen_h_
/* Eigen.h
 *
 * Copyright (C) 1993-2020 David Weenink
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

#include "Eigen_def.h"

autoEigen Eigen_create (integer numberOfEigenvalues, integer dimension);

void Eigen_init (Eigen me, integer numberOfEigenvalues, integer dimension);

void Eigen_initFromSymmetricMatrix (Eigen me, constMATVU const& a);

void Eigen_initFromSquareRoot (Eigen me, constMATVU const& a);
/*
	Calculate eigenstructure for symmetric matrix A'A (e.g. covariance matrix),
	when only A is given.
	Precondition: numberOfRows > 1
	Method: SVD.
*/

void Eigen_initFromSquareRootPair (Eigen me, constMAT a, constMAT b);
/*
	Calculate eigenstructure for A'Ax - lambda B'Bx = 0
	Preconditions: numberOfRows >= numberOfColumns &&
		numberOfRows_b >= numberOfColumns
	Method: Generalized SVD.
*/

integer Eigen_getNumberOfEigenvectors (Eigen me);

integer Eigen_getDimensionOfComponents (Eigen me);

double Eigen_getCumulativeContributionOfComponents (Eigen me, integer from, integer to);

integer Eigen_getDimensionOfFraction (Eigen me, double fraction);

double Eigen_getEigenvectorElement (Eigen me, integer ivec, integer element);

double Eigen_getSumOfEigenvalues (Eigen me, integer from, integer to);


void Eigen_sort (Eigen me);
/*
	Sort eigenvalues and corresponding eigenvectors in decreasing order.
*/

void Eigen_invertEigenvector (Eigen me, integer ivec);

void Eigen_drawEigenvalues (Eigen me, Graphics g, integer first, integer last, double ymin, double ymax,
	bool fractionOfTotal, bool cumulative, double size_mm, conststring32 mark, bool garnish);

void Eigen_drawEigenvector (Eigen me, Graphics g, integer ivec, integer first, integer last, double minimum, double maximum, bool weigh,
	double size_mm, conststring32 mark, bool connect, char32 **rowLabels, bool garnish);
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

#endif /* _Eigen_h_ */

