#ifndef _LegendreSeries_h_
#define _LegendreSeries_h_
/* LegendreSeries.h
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

Thing_define (LegendreSeries, FunctionSeries) {
	// overridden methods:
	public:
		virtual double v_evaluate (double x);
		virtual void v_evaluateTerms (double x, VEC terms);
		virtual void v_getExtrema (double x1, double x2, double *xmin, double *ymin, double *xmax, double *ymax);
};

autoLegendreSeries LegendreSeries_create (double xmin, double xmax, integer numberOfPolynomials);

autoLegendreSeries LegendreSeries_createFromString (double xmin, double xmax, conststring32 s);

autoLegendreSeries LegendreSeries_getDerivative (LegendreSeries me);

autoPolynomial LegendreSeries_to_Polynomial (LegendreSeries me);

#endif /* _LegendreSeries_h_ */
