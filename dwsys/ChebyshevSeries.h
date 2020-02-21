#ifndef _ChebyshevSeries_h_
#define _ChebyshevSeries_h_
/*ChebyshevSeries.h
 *
 * Copyright (C) 2020 David Weenink
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

#include "FunctionSeries.h"
#include "Polynomial.h"

/*
	A ChebyshevSeries p(x) on a domain [xmin,xmax] is defined as the
	following linear combination of Chebyshev polynomials T[k](x') of
	degree k-1 and domain [-1, 1]:
		p(x) = sum (k=1..numberOfCoefficients, c[k]*T[k](x')) - c[1] / 2, where
		x' = (2 * x - xmin - xmax) / (xmax - xmin)
	This is equivalent to:
		p(x) = c[1] /2 + sum (k=2..numberOfCoefficients, c[k]*T[k](x'))
*/

Thing_define (ChebyshevSeries, FunctionSeries) {
	// overridden methods:
	public:
		virtual double v_evaluate (double x);
		virtual void v_evaluateTerms (double x, VEC terms);
		virtual void v_getExtrema (double x1, double x2, double *xmin, double *ymin, double *xmax, double *ymax);
};

autoChebyshevSeries ChebyshevSeries_create (double xmin, double xmax, integer numberOfPolynomials);

autoChebyshevSeries ChebyshevSeries_createFromString (double xmin, double xmax, conststring32 s);

autoPolynomial ChebyshevSeries_to_Polynomial (ChebyshevSeries me);


#endif /* _ChebyshevSeries_h_ */
