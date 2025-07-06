#ifndef _Roots_h_
#define _Roots_h_
/* Roots.h
 *
 * Copyright (C) 1993-2025 David Weenink
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
 djmw 20020813 GPL header
*/

#include "Data.h"
#include "Graphics.h"
#include "Polynomial.h"
#include "WorkvectorPool.h"

#include "Roots_def.h"


autoRoots Roots_create (integer numberOfRoots);

void Roots_fixIntoUnitCircle (mutableRoots me);

void Roots_sort (mutableRoots me);
/* Sort to size of real part a+bi, a-bi*/

dcomplex Roots_evaluate_z (constRoots me, dcomplex z);

autoRoots Polynomial_to_Roots_ev (constPolynomial me);

integer Roots_getNumberOfRoots (constRoots me);

void Roots_draw (constRoots me, Graphics g, double rmin, double rmax, double imin, double imax,
	conststring32 symbol, double fontSize, bool garnish);

dcomplex Roots_getRoot (constRoots me, integer index);

void Roots_setRoot (mutableRoots me, integer index, double re, double im);

autoRoots Polynomial_to_Roots (constPolynomial me);
/* Find roots of polynomial and polish them */

void Roots_Polynomial_polish (mutableRoots me, constPolynomial thee);

autoPolynomial Roots_to_Polynomial (constRoots me, bool rootsAreReal);

/*
	workspace size >= n * n + 2 * n + 11 * n=
		n * n		; for hessenberg matrix
		+ 2 * n 	; for real and imaginary parts
		+ 11 * n	; the maximum for dhseqr_
*/
void Polynomial_into_Roots (constPolynomial me, mutableRoots r, VEC workspace);

#endif /* _Roots_h_ */
