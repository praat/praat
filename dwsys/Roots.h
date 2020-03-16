#ifndef _Roots_h_
#define _Roots_h_
/* Roots.h
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

/*
 djmw 20020813 GPL header
 djmw 20110306 Latest modification.
*/

#include "Data.h"
#include "Graphics.h"
#include "Polynomial.h"

#include "Roots_def.h"


autoRoots Roots_create (integer numberOfRoots);

void Roots_fixIntoUnitCircle (Roots me);

void Roots_sort (Roots me);
/* Sort to size of real part a+bi, a-bi*/

dcomplex Roots_evaluate_z (Roots me, dcomplex z);

autoRoots Polynomial_to_Roots_ev (Polynomial me);

integer Roots_getNumberOfRoots (Roots me);

void Roots_draw (Roots me, Graphics g, double rmin, double rmax, double imin, double imax,
	conststring32 symbol, double fontSize, bool garnish);

dcomplex Roots_getRoot (Roots me, integer index);

void Roots_setRoot (Roots me, integer index, double re, double im);

autoRoots Polynomial_to_Roots (Polynomial me);
/* Find roots of polynomial and polish them */

void Roots_Polynomial_polish (Roots me, Polynomial thee);

autoPolynomial Roots_to_Polynomial (Roots me, bool rootsAreReal);

/*
	workspace.size >= n * n + 3 * n =
		n * n		; for hessenberg matrix
		+ 2 * n 	; for real and imaginary parts
		+ n			; for dhseqr_
*/
void Polynomial_into_Roots (Polynomial me, Roots r, VEC const& workspace);
#endif /* _Roots_h_ */
