/* SVD.h
 *
 * Copyright (C) 1994-2002 David Weenink
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
 djmw 20020423 GPL header
 djmw 20070620 Latest modification.
*/
#ifndef _SVD_h_
#define _SVD_h_

#include "NUM2.h"
#ifndef _Data_h_
	#include "Data.h"
#endif

#define SVD_members Data_members		\
	double tolerance;	\
	long numberOfRows, numberOfColumns;	\
	double **u; /* column vectors */ \
	double **v; /* row vectors */ \
	double *d; /* decreasing singular values */
#define SVD_methods Data_methods
class_create (SVD, Data);

#define GSVD_members Data_members		\
	double tolerance; \
	long numberOfColumns; \
	double **q, **r; \
	double *d1, *d2;
#define GSVD_methods Data_methods
class_create (GSVD, Data);

int SVD_init (I, long numberOfRows, long numberOfColumns);

SVD SVD_create (long numberOfRows, long numberOfColumns);
/*
	my tolerance = eps * MAX (numberOfRows, numberOfColumns) 
	where eps is the floating point precision, approximately 2.2e-16
*/

SVD SVD_create_d (double **m, long numberOfRows, long numberOfColumns);
SVD SVD_create_f (float **m, long numberOfRows, long numberOfColumns);
/*
	Copy matrix into svd->u and calculate U D V'
*/

int SVD_svd_d (I, double **m);
int SVD_svd_f (I, float **m);
/*
	Perform SVD analysis on matrix M, i.e., decompose M as M = UDV'.
	Watch out: dataType contains V, not V' !!
*/

int SVD_compute (I);

int SVD_solve (I, double b[], double x[]);
/* Solve Ax = b */

int SVD_sort (I);
/*
	Sort singular values (and corresponding column vectors of U and V) in decreasing order.
*/

void SVD_setTolerance (I, double tolerance);
double SVD_getTolerance (I);

long SVD_zeroSmallSingularValues (I, double tolerance);
/*
	Zero singular values smaller than maximum_singular_value * tolerance
	If tolerance == 0 then then my tolerance will be used.
	Return the number of s.v.'s zeroed.
*/

int SVD_synthesize (I, long sv_from, long sv_to, double **m);
/*
	Synthesize matrix as U D(sv_from:sv_to) V'.
	(The synthesized matrix is an approximation of the svd'ed matrix with 
	only a selected number of sv's). 
	Matrix m is [numberOfRows x numberOfColumns] and must be allocated
	by caller!
*/

long SVD_getRank (I);

GSVD GSVD_create (long numberOfColumns);

GSVD GSVD_create_d (double **m1, long numberOfRows1, long numberOfColumns,
	double **m2, long numberOfRows2);

void GSVD_setTolerance (GSVD me, double tolerance);

double GSVD_getTolerance (GSVD me);

#endif /* _SVD_h_ */
