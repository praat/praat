#ifndef _FunctionSeries_h_
#define _FunctionSeries_h_
/* FunctionSeries.h
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

#include "Function.h"
#include "Graphics.h"

#include "FunctionSeries_def.h"

void FunctionSeries_init (FunctionSeries me, double xmin, double xmax, integer numberOfCoefficients);

void FunctionSeries_initFromString (FunctionSeries me, double xmin, double xmax, conststring32 s, bool allowTrailingZeros);

autoFunctionSeries FunctionSeries_create (double xmin, double xmax, integer numberOfCoefficients);

void FunctionSeries_setDomain (FunctionSeries me, double xmin, double xmax);

void FunctionSeries_setCoefficient (FunctionSeries me, integer index, double value);

double FunctionSeries_evaluate (FunctionSeries me, double x);

void FunctionSeries_evaluate_z (FunctionSeries me, dcomplex *z, dcomplex *p);

void FunctionSeries_evaluateTerms (FunctionSeries me, double x, VEC terms);

void FunctionSeries_getExtrema (FunctionSeries me, double x1, double x2, double *xmin, double *ymin, double *xmax, double *ymax);

integer FunctionSeries_getDegree (FunctionSeries me);

double FunctionSeries_getMinimum (FunctionSeries me, double x1, double x2);

double FunctionSeries_getXOfMinimum (FunctionSeries me, double x1, double x2);

double FunctionSeries_getMaximum (FunctionSeries me, double x1, double x2);

double FunctionSeries_getXOfMaximum (FunctionSeries me, double x1, double x2);
/*
	Returns minimum and maximum function values (ymin, ymax) in
	interval [x1, x2] and their x-values (xmin, xmax).
	Precondition: [x1, x2] is a (sub)domain
		my xmin <= x1 < x2 <= my xmax
*/

void FunctionSeries_draw (FunctionSeries me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	int extrapolate, bool garnish);
/*
	Extrapolate only for functions whose domain is extendable and that can be extrapolated.
	Polynomials can be extrapolated.
	LegendreSeries and ChebyshevSeries cannot be extrapolated.
*/
void FunctionSeries_drawBasisFunction (FunctionSeries me, Graphics g, integer index, double xmin, double xmax,
	double ymin, double ymax, int extrapolate, bool garnish);

#endif /* _FunctionSeries_h_ */
