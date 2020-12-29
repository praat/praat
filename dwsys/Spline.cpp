/* Spline.cpp
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

#include "Spline.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "Spline_def.h"
#include "oo_COPY.h"
#include "Spline_def.h"
#include "oo_EQUAL.h"
#include "Spline_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Spline_def.h"
#include "oo_WRITE_TEXT.h"
#include "Spline_def.h"
#include "oo_WRITE_BINARY.h"
#include "Spline_def.h"
#include "oo_READ_TEXT.h"
#include "Spline_def.h"
#include "oo_READ_BINARY.h"
#include "Spline_def.h"
#include "oo_DESCRIPTION.h"
#include "Spline_def.h"

/*
	Functions for calculating an mspline and an ispline. These functions should replace
	the functions in NUM2.c. Before we can do that we first have to adapt the spline-
	dependencies in MDS.c.

	Formally nKnots == order + numberOfInteriorKnots + order.
	We forget about the multiple knots at start and end.
	Our point-sequece xmin < interiorKont [1] < ... < interiorKnot [n] < xmax.
	nKnots is now numberOfinteriorKnots + 2.
*/
static double NUMmspline2 (constVEC points, integer order, integer index, double x) {
	const integer numberOfSplines = points.size + order - 2;
	double m [Spline_MAXIMUM_DEGREE + 2];

	Melder_assert (points.size > 2 && order > 0 && index > 0);

	if (index > numberOfSplines)
		return undefined;

	/*
		Find the range/interval where x is located.
		M-splines of order k have degree k-1.
		M-splines are zero outside interval [knot [i], knot [i+order]).
		First and last 'order' knots are equal, i.e.,
		knot [1] = ... = knot [order] && knot [nKnots-order+1] = ... knot [nKnots].
	*/

	const integer index_b = Melder_clippedLeft (1_integer, index - order + 1);
	if (x < points [index_b])
		return 0.0;

	const integer index_e = Melder_clippedRight (index_b + std::min (index, order), points.size);
	if (x > points [index_e])
		return 0.0;
	/*
		Calculate M [i](x|1,t) according to eq.2.
	*/
	for (integer k = 1; k <= order; k ++) {
		const integer k1 = index - order + k, k2 = k1 + 1;
		m [k] = 0.0;
		if (k1 > 0 && k2 <= points.size && x >= points [k1] && x < points [k2])
			m [k] = 1.0 / (points [k2] - points [k1]);
	}
	/*
		Iterate to get M [i](x|k,t)
	*/
	for (integer k = 2; k <= order; k ++) {
		for (integer j = 1; j <= order - k + 1; j ++) {
			integer k1 = index - order + j, k2 = k1 + k;
			if (k2 > 1 && k1 < 1)
				k1 = 1;
			else if (k2 > points.size && k1 < points.size)
				k2 = points.size;
			if (k1 > 0 && k2 <= points.size) {
				const double p1 = points [k1], p2 = points [k2];
				m [j] = k * ((x - p1) * m [j] + (p2 - x) * m [j + 1]) /
					((k - 1) * (p2 - p1));
			}
		}
	}
	return m [1];
}

static double NUMispline2 (constVEC points, integer order, integer index, double x) {
	Melder_assert (points.size > 2 && order > 0 && index > 0);

	const integer index_b = Melder_clippedLeft (1_integer, index - order + 1);
	if (x < points [index_b])
		return 0.0;

	const integer index_e = Melder_clippedRight (index_b + std::min (index, order), points.size);
	if (x > points [index_e])
		return 1.0;

	integer j;
	for (j = index_e - 1; j >= index_b; j--)
		if (x > points [j])
			break;
	/*
		Equation 5 in Ramsay's article contains some errors!!!
		1. the interval selection must be 'j-k <= i <= j' instead of
			'j-k+1 <= i <= j'
		2. the summation index m starts at 'i+1' instead of 'i'
	*/
	longdouble y = 0.0;
	for (integer m = index + 1; m <= j + order; m ++) {
		integer km = m - order, kmp = km + order + 1;
		Melder_clipLeft (1_integer, & km);
		Melder_clipRight (& kmp, points.size);
		y += (points [kmp] - points [km]) * NUMmspline2 (points, order + 1, m, x);
	}
	return double (y / (order + 1));
}

Thing_implement (Spline, FunctionSeries, 0);

double structSpline :: v_evaluate (double /* x */) {
	return 0.0;
}

integer structSpline :: v_getDegree () {
	return degree;
}

integer structSpline :: v_getOrder () {
	return degree + 1;
}

/* Precondition: FunctionSeries part inited + degree */
static void Spline_initKnotsFromString (Spline me, integer degree, conststring32 interiorKnots_string) {
	Melder_require (degree <= Spline_MAXIMUM_DEGREE,
		U"Degree should be <= ", Spline_MAXIMUM_DEGREE, U".");
	
	autoVEC interiorKnots = newVECfromString (interiorKnots_string);

	sort_VEC_inout (interiorKnots.get());
	Melder_require (interiorKnots [1] > my xmin && interiorKnots [interiorKnots.size] <= my xmax,
		U"Knots should be inside domain.");

	my degree = degree;
	const integer order = Spline_getOrder (me); /* depends on spline type !! */
	const integer n = interiorKnots.size + order;
	Melder_require (my numberOfCoefficients == n,
		U"Number of coefficients should equal ", n, U".");

	my numberOfKnots = interiorKnots.size + 2;
	my knots = zero_VEC (my numberOfKnots);
	my knots.part (2, interiorKnots.size + 1) <<= interiorKnots.all();
	my knots [1] = my xmin;
	my knots [my numberOfKnots] = my xmax;
}

void Spline_init (Spline me, double xmin, double xmax, integer degree, integer numberOfCoefficients, integer numberOfKnots) {
	Melder_require (degree <= Spline_MAXIMUM_DEGREE,
		U"Degree should be <= ", Spline_MAXIMUM_DEGREE, U".");
	FunctionSeries_init (me, xmin, xmax, numberOfCoefficients);
	my knots = zero_VEC (numberOfKnots);
	my degree = degree;
	my numberOfKnots = numberOfKnots;
	my knots [1] = xmin;
	my knots [numberOfKnots] = xmax;
}

void Spline_drawKnots (Spline me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool garnish) {
	const integer order = Spline_getOrder (me);
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (xmax < my xmin || xmin > my xmax)
		return;

	if (ymax <= ymin)
		FunctionSeries_getExtrema (me, xmin, xmax, nullptr, & ymin, nullptr, & ymax);

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	if (my knots [1] >= xmin && my knots [1] <= xmax) {
		Graphics_markTop (g, my knots [1], false, true, true,
				! garnish ? U"" :
				order == 1 ? U"t__1_" :
				order == 2 ? U"{t__1_, t__2_}" :
				Melder_cat (U"{t__1_..t__", order, U"_}")
			);
	}
	for (integer i = 2; i <= my numberOfKnots - 1; i ++) {
		if (my knots [i] >= xmin && my knots [i] <= xmax) {
			Graphics_markTop (g, my knots [i], false, true, true,
				! garnish ? U"" :
					Melder_cat (U"t__", i + order - 1, U"_")
				);
		}
	}
	if (my knots [my numberOfKnots] >= xmin && my knots [my numberOfKnots] <= xmax) {
		const integer numberOfKnots = ! garnish ? 0 : my numberOfKnots + 2 * (order - 1);
		Graphics_markTop (g, my knots [my numberOfKnots], false, true, true,
				! garnish ? U"" :
				order == 1 ? Melder_cat (U"t__", numberOfKnots, U"_") :
				order == 2 ? Melder_cat (U"{t__", numberOfKnots - 1, U"_, t__", numberOfKnots, U"_}") :
				Melder_cat (U"{t__", numberOfKnots - order + 1, U"_..t__", numberOfKnots, U"_}")
			);
	}
}

integer Spline_getOrder (Spline me) {
	return my v_getOrder ();
}

autoSpline Spline_scaleX (Spline me, double xmin, double xmax) {
	try {
		Melder_assert (xmin < xmax);

		autoSpline thee = Data_copy (me);

		thy xmin = xmin;
		thy xmax = xmax;
		/*
			x = a x + b
			Constraints:
			my xmin = a xmin + b;    a = (my xmin - my xmax) / (xmin - xmax);
			my xmax = a xmax + b;    b = my xmin - a * xmin
		*/
		const double a = (xmin - xmax) / (my xmin - my xmax);
		const double b = xmin - a * my xmin;
		for (integer i = 1; i <= my numberOfKnots; i ++)
			thy knots [i] = a * my knots [i] + b;
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Scaled Spline not created.");
	}
}

/********* MSplines ************************************************/

double structMSpline :: v_evaluate (double x) {
	if (x < our xmin || x > our xmax)
		return 0.0;
	double result = 0.0;
	for (integer i = 1; i <= numberOfCoefficients; i ++)
		if (coefficients [i] != 0.0)
			result += coefficients [i] * NUMmspline2 (knots.get(), degree + 1, i, x);
	return result;
}

void structMSpline :: v_evaluateTerms (double x, VEC terms) {
	Melder_assert (terms.size == numberOfCoefficients);
	if (x < our xmin || x > our xmax)
		return;
	for (integer i = 1; i <= numberOfCoefficients; i ++)
		terms [i] = NUMmspline2 (knots.get(), degree + 1, i, x);
}

Thing_implement (MSpline, Spline, 0);

autoMSpline MSpline_create (double xmin, double xmax, integer degree, integer numberOfInteriorKnots) {
	try {
		autoMSpline me = Thing_new (MSpline);
		const integer numberOfCoefficients = numberOfInteriorKnots + degree + 1;
		const integer numberOfKnots = numberOfCoefficients + degree + 1;
		Spline_init (me.get(), xmin, xmax, degree, numberOfCoefficients, numberOfKnots);
		return me;
	} catch (MelderError) {
		Melder_throw (U"MSpline not created.");
	}
}

autoMSpline MSpline_createFromStrings (double xmin, double xmax, integer degree, conststring32 coef, conststring32 interiorKnots) {
	try {
		Melder_require (degree <= Spline_MAXIMUM_DEGREE,
			U"Degree should be <= ", Spline_MAXIMUM_DEGREE, U".");
		autoMSpline me = Thing_new (MSpline);
		FunctionSeries_initFromString (me.get(), xmin, xmax, coef, true);
		Spline_initKnotsFromString (me.get(), degree, interiorKnots);
		return me;
	} catch (MelderError) {
		Melder_throw (U"MSpline not created from strings.");
	}
}

/******** ISplines ************************************************/

double structISpline :: v_evaluate (double x) {
	if (x < our xmin || x > our xmax)
		return 0.0;
	double result = 0.0;
	for (integer i = 1; i <= numberOfCoefficients; i ++)
		if (coefficients [i] != 0.0)
			result += coefficients [i] * NUMispline2 (knots.get(), degree, i, x);
	return result;
}

void structISpline :: v_evaluateTerms (double x, VEC terms) {
	Melder_assert (terms.size == numberOfCoefficients);
	for (integer i = 1; i <= numberOfCoefficients; i ++)
		terms [i] = NUMispline2 (knots.get(), degree, i, x);
}

integer structISpline :: v_getOrder () {
	return degree;
}

Thing_implement (ISpline, Spline, 0);

autoISpline ISpline_create (double xmin, double xmax, integer degree, integer numberOfInteriorKnots) {
	try {
		autoISpline me = Thing_new (ISpline);
		const integer numberOfCoefficients = numberOfInteriorKnots + degree;
		const integer numberOfKnots = numberOfCoefficients + degree;
		Spline_init (me.get(), xmin, xmax, degree, numberOfCoefficients, numberOfKnots);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ISpline not created.");
	}
}

autoISpline ISpline_createFromStrings (double xmin, double xmax, integer degree, conststring32 coef, conststring32 interiorKnots) {
	try {
		Melder_require (degree <= Spline_MAXIMUM_DEGREE,
			U"Degree should should not exceed ", Spline_MAXIMUM_DEGREE);
		autoISpline me = Thing_new (ISpline);
		FunctionSeries_initFromString (me.get(), xmin, xmax, coef, true);
		Spline_initKnotsFromString (me.get(), degree, interiorKnots);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ISpline not created from strings.");
	};
}

/* end of file Spline.cpp */
