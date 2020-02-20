#ifndef _Spline_h_
#define _Spline_h_
/* Spline.h
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

#define Spline_MAXIMUM_DEGREE 20

#include "Graphics.h"
#include "Polynomial.h"
#include "Spline_def.h"

void Spline_init (Spline me, double xmin, double xmax, integer degree, integer numberOfCoefficients, integer numberOfKnots);

integer Spline_getOrder (Spline me);

void Spline_drawKnots (Spline me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool garnish);

autoSpline Spline_scaleX (Spline me, double xmin, double xmax);
/* scale domain and knots to new domain */

Thing_define (MSpline, Spline) {
	// overridden methods:
	public:
		virtual double v_evaluate (double x);
		virtual void v_evaluateTerms (double x, VEC terms);
};

autoMSpline MSpline_create (double xmin, double xmax, integer degree, integer numberOfInteriorKnots);

autoMSpline MSpline_createFromStrings (double xmin, double xmax, integer degree, conststring32 coef, conststring32 interiorKnots);

Thing_define (ISpline, Spline) {
	// overridden methods:
	public:
		virtual double v_evaluate (double x);
		virtual void v_evaluateTerms (double x, VEC terms);
		virtual integer v_getOrder ();
};

autoISpline ISpline_create (double xmin, double xmax, integer degree, integer numberOfInteriorKnots);

autoISpline ISpline_createFromStrings (double xmin, double xmax, integer degree, conststring32 coef, conststring32 interiorKnots);

#endif /* _Spline_h_ */
