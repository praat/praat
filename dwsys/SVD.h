#ifndef _SVD_h_
#define _SVD_h_
/* SVD.h
 *
 * Copyright (C) 1994-2020 David Weenink
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

#include "NUM2.h"
#include "Data.h"

#include "SVD_def.h"

void SVD_init (SVD me, integer numberOfRows, integer numberOfColumns);

autoSVD SVD_create (integer numberOfRows, integer numberOfColumns);
/*
	my tolerance = eps * MAX (numberOfRows, numberOfColumns)
	where eps is the floating point precision, approximately 2.2e-16
*/

autoSVD SVD_createFromGeneralMatrix (constMATVU const& m);

void SVD_update (SVD me, constMATVU const& m);
/*
	Perform SVD analysis on matrix M, i.e., decompose M as M = UDV'.
	Watch out: dataType contains V, not V' !!
*/

void SVD_compute (SVD me);

/* Solve Ax = b */
void SVD_solve_preallocated (SVD me, constVECVU const& b, VECVU const& result);
autoVEC SVD_solve (SVD me, constVECVU const& b);

/* Solve A*X = B */
void SVD_solve_preallocated (SVD me, constMATVU const& b, MATVU const& result);
autoMAT SVD_solve (SVD me, constMATVU const& b);

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

autoMAT SVD_synthesize (SVD me, integer sv_from, integer sv_to);
/*
	Synthesize matrix as U D(sv_from:sv_to) V'.
	(The synthesized matrix is an approximation of the svd'ed matrix if
	only a selected number of sv's is used).
	Matrix m is [numberOfRows x numberOfColumns] and must be allocated
	by caller!
*/

autoMAT SVD_getSquared (SVD me, bool inverse);
void SVD_getSquared_preallocated (SVD me, bool inverse, MAT const& m);
// compute V D^2 V' or V D^-2 V'

integer SVD_getRank (SVD me);

autoGSVD GSVD_create (integer numberOfColumns);

autoGSVD GSVD_create (constMATVU const& m1, constMATVU const& m2);

void GSVD_setTolerance (GSVD me, double tolerance);

double GSVD_getTolerance (GSVD me);

#endif /* _SVD_h_ */
