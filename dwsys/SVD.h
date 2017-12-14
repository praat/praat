/* SVD.h
 *
 * Copyright (C) 1994-2017 David Weenink
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

/*
 djmw 20020423 GPL header
 djmw 20120808 Latest modification.
*/
#ifndef _SVD_h_
#define _SVD_h_

#include "NUM2.h"
#include "Data.h"

#include "SVD_def.h"

void SVD_init (SVD me, integer numberOfRows, integer numberOfColumns);

autoSVD SVD_create (integer numberOfRows, integer numberOfColumns);
/*
	my tolerance = eps * MAX (numberOfRows, numberOfColumns)
	where eps is the floating point precision, approximately 2.2e-16
*/

autoSVD SVD_create_d (double **m, integer numberOfRows, integer numberOfColumns);

autoSVD SVD_create_f (float **m, integer numberOfRows, integer numberOfColumns);
/*
	Copy matrix into svd->u and calculate U D V'
*/

void SVD_svd_d (SVD me, double **m);

void SVD_svd_f (SVD me, float **m);
/*
	Perform SVD analysis on matrix M, i.e., decompose M as M = UDV'.
	Watch out: dataType contains V, not V' !!
*/

void SVD_compute (SVD me);

void SVD_solve (SVD me, double b[], double x[]);
/* Solve Ax = b */

void SVD_solve2 (SVD me, double b[], double x[], double fractionOfSumOfSingularValues);

void SVD_sort (SVD me);
/*
	Sort singular values (and corresponding column vectors of U and V) in decreasing order.
*/

void SVD_setTolerance (SVD me, double tolerance);

double SVD_getTolerance (SVD me);

double SVD_getConditionNumber (SVD me);

double SVD_getSumOfSingularValuesAsFractionOfTotal (SVD me, integer from, integer to);

integer SVD_getMinimumNumberOfSingularValues (SVD me, double fractionOfSumOfSingularValues);

double SVD_getSumOfSingularValues (SVD me, integer from, integer to);

integer SVD_zeroSmallSingularValues (SVD me, double tolerance);
/*
	Zero singular values smaller than maximum_singular_value * tolerance
	If tolerance == 0 then then my tolerance will be used.
	Return the number of s.v.'s zeroed.
*/

void SVD_synthesize (SVD me, integer sv_from, integer sv_to, double **m);
/*
	Synthesize matrix as U D(sv_from:sv_to) V'.
	(The synthesized matrix is an approximation of the svd'ed matrix with
	only a selected number of sv's).
	Matrix m is [numberOfRows x numberOfColumns] and must be allocated
	by caller!
*/

void SVD_getSquared (SVD me, double **m, bool inverse);
// compute V D^2 V' or V D^-2 V'

integer SVD_getRank (SVD me);

autoGSVD GSVD_create (integer numberOfColumns);

autoGSVD GSVD_create_d (double **m1, integer numberOfRows1, integer numberOfColumns, double **m2, integer numberOfRows2);

void GSVD_setTolerance (GSVD me, double tolerance);

double GSVD_getTolerance (GSVD me);

#endif /* _SVD_h_ */
