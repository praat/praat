#ifndef _Eigen_h_
#define _Eigen_h_
/* Eigen.h
 *
 * Copyright (C) 1993-2012, 2015 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020813 GPL header
 djmw 20110306 Latest modification
*/

#include "Collection.h"
#include "Graphics.h"
#include "Strings_.h"

#include "Eigen_def.h"

autoEigen Eigen_create (long numberOfEigenvalues, long dimension);

void Eigen_init (Eigen me, long numberOfEigenvalues, long dimension);

void Eigen_initFromSymmetricMatrix_f (Eigen me, float **a, long n);

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
	int fractionOfTotal, int cumulative, double size_mm, const char32 *mark, int garnish);

void Eigen_drawEigenvector (Eigen me, Graphics g, long ivec, long first, long last, double minimum, double maximum, int weigh, 
	double size_mm, const char32 *mark, int connect, char32 **rowLabels, int garnish);
/*
	Draw eigenvector. When rowLabels != nullptr, draw row text labels on bottom axis.
*/

void Eigens_alignEigenvectors (Collection me);
/*
	Correlate all eigenvectors with the eigenvectors of the first Eigen.
	If r < 0 then mirror the eigenvectors of
*/

double Eigens_getAngleBetweenEigenplanes_degrees (Eigen me, Eigen thee);
/*
	Get angle between the eigenplanes, spanned by the first two eigenvectors, .
*/

#endif /* _Eigen_h_ */

