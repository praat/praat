/* FunctionSeries.cpp
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

#include "FunctionSeries.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "FunctionSeries_def.h"
#include "oo_COPY.h"
#include "FunctionSeries_def.h"
#include "oo_EQUAL.h"
#include "FunctionSeries_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FunctionSeries_def.h"
#include "oo_WRITE_TEXT.h"
#include "FunctionSeries_def.h"
#include "oo_WRITE_BINARY.h"
#include "FunctionSeries_def.h"
#include "oo_READ_TEXT.h"
#include "FunctionSeries_def.h"
#include "oo_READ_BINARY.h"
#include "FunctionSeries_def.h"
#include "oo_DESCRIPTION.h"
#include "FunctionSeries_def.h"

Thing_implement (FunctionSeries, Function, 0);

double structFunctionSeries :: v_evaluate (double /* x */) {
	return undefined;
}

dcomplex structFunctionSeries :: v_evaluate_z (dcomplex /* z */) {
	return { undefined, undefined };
}

void structFunctionSeries :: v_evaluateTerms (double /* x */, VEC terms) {
	Melder_assert (terms.size == numberOfCoefficients);
	terms  <<=  undefined;
}

integer structFunctionSeries :: v_getDegree () {
	return numberOfCoefficients - 1;
}

void structFunctionSeries :: v_getExtrema (double x1, double x2, double *out_xmin, double *out_ymin, double *out_xmax, double *out_ymax) { // David, geen aparte naam hier nodig: ???
	const integer numberOfPoints = 1000;

	// Melder_warning (L"defaultGetExtrema: extrema calculated by sampling the interval");

	const double dx = (x2 - x1) / (numberOfPoints - 1);
	double x = x1;
	double xmn = x, xmx = xmn, ymn = v_evaluate (x), ymx = ymn; // xmin, xmax .. would shadow  member
	for (integer i = 2; i <= numberOfPoints; i ++) {
		x += dx;
		const double y = v_evaluate (x);
		if (y > ymx) {
			ymx = y;
			xmx = x;
		} else if (y < ymn) {
			ymn = y;
			xmn = x;
		}
	}
	if (out_xmin)
		*out_xmin = xmn;
	if (out_xmax)
		*out_xmax = xmx;
	if (out_ymin)
		*out_ymin = ymn;
	if (out_ymax)
		*out_ymax = ymx;
}

void FunctionSeries_init (FunctionSeries me, double xmin, double xmax, integer numberOfCoefficients) {
	my coefficients = zero_VEC (numberOfCoefficients);
	my numberOfCoefficients = numberOfCoefficients;
	my _capacity = numberOfCoefficients;
	my xmin = xmin;
	my xmax = xmax;
}

autoFunctionSeries FunctionSeries_create (double xmin, double xmax, integer numberOfCoefficients) {
	try {
		autoFunctionSeries me = Thing_new (FunctionSeries);
		FunctionSeries_init (me.get(), xmin, xmax, numberOfCoefficients);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FunctionSeries not created.");
	}
}

void FunctionSeries_initFromString (FunctionSeries me, double xmin, double xmax, conststring32 s, bool allowTrailingZeros) {
	autoVEC numbers = newVECfromString (s);
	integer numberOfCoefficients = numbers.size;
	if (! allowTrailingZeros)
		while (numbers [numberOfCoefficients] == 0.0 && numberOfCoefficients > 1)
			numberOfCoefficients --;
	FunctionSeries_init (me, xmin, xmax, numberOfCoefficients);
	my coefficients.part (1, numberOfCoefficients) <<= numbers.part (1, numberOfCoefficients);
}

integer FunctionSeries_getDegree (FunctionSeries me) {
	return my v_getDegree ();
}

void FunctionSeries_setDomain (FunctionSeries me, double xmin, double xmax) {
	my xmin = xmin;
	my xmax = xmax;
}

double FunctionSeries_evaluate (FunctionSeries me, double x) {
	return my v_evaluate (x);
}

dcomplex FunctionSeries_evaluate_z (FunctionSeries me, dcomplex z) {
	return my v_evaluate_z (z);
}

void FunctionSeries_evaluateTerms (FunctionSeries me, double x, VEC terms) {
	my v_evaluateTerms (x, terms);
}

void FunctionSeries_getExtrema (FunctionSeries me, double x1, double x2, double *xmin, double *ymin, double *xmax, double *ymax) {
	if (x2 <= x1) {
		x1 = my xmin;
		x2 = my xmax;
	}
	my v_getExtrema (x1, x2, xmin, ymin, xmax, ymax);
}

double FunctionSeries_getMinimum (FunctionSeries me, double x1, double x2) {
	double ymin;
	FunctionSeries_getExtrema (me, x1, x2, nullptr, & ymin, nullptr, nullptr);
	return ymin;
}

double FunctionSeries_getXOfMinimum (FunctionSeries me, double x1, double x2) {
	double xmin;
	FunctionSeries_getExtrema (me, x1, x2, & xmin, nullptr, nullptr, nullptr);
	return xmin;
}

double FunctionSeries_getMaximum (FunctionSeries me, double x1, double x2) {
	double ymax;
	FunctionSeries_getExtrema (me, x1, x2, nullptr, nullptr, nullptr, & ymax);
	return ymax;
}

double FunctionSeries_getXOfMaximum (FunctionSeries me, double x1, double x2) {
	double xmax;
	FunctionSeries_getExtrema (me, x1, x2, nullptr, nullptr, & xmax, nullptr);
	return xmax;
}

static void Graphics_polyline_clipTopBottom (Graphics g, VEC x, VEC y, double ymin, double ymax) {
	Melder_assert (x.size == y.size);
	integer index = 1;

	if (x.size < 2)
		return;
	double x1 = x [1], y1 = y [1];
	double xb = x1, yb = y1;

	for (integer i = 2; i < x.size; i ++) {
		const double x2 = x [i], y2 = y [i];

		if (! ((y1 > ymax && y2 > ymax) || (y1 < ymin && y2 < ymin))) {
			const double dxy = (x2 - x1) / (y1 - y2);
			const double xcros_max = x1 - (ymax - y1) * dxy;
			const double xcros_min = x1 - (ymin - y1) * dxy;
			if (y1 > ymax && y2 < ymax) {
				/*
					Line enters from above: start new segment. Save start values.
				*/
				xb = x [i - 1];
				yb = y [i - 1];
				index = i - 1;
				y [i - 1] = ymax;
				x [i - 1] = xcros_max;
			}
			if (y1 > ymin && y2 < ymin) {
				/*
					Line leaves at bottom: draw segment. Save end values and restore them
					Origin of arrays for Graphics_polyline are at element 0 !!!
				*/
				const double xe = x [i], ye = y [i];
				y [i] = ymin;
				x [i] = xcros_min;

				Graphics_polyline (g, i - index + 1,
						x.asArgumentToFunctionThatExpectsZeroBasedArray() + index,
						y.asArgumentToFunctionThatExpectsZeroBasedArray() + index);

				x [index] = xb;
				y [index] = yb;
				x [i] = xe;
				y [i] = ye;
			}
			if (y1 < ymin && y2 > ymin) {
				// Line enters from below: start new segment. Save start values

				xb = x [i - 1];
				yb = y [i - 1];
				index = i - 1;
				y [i - 1] = ymin;
				x [i - 1] = xcros_min;
			}
			if (y1 < ymax && y2 > ymax) {
				// Line leaves at top: draw segment. Save and restore

				const double xe = x [i], ye = y [i];
				y [i] = ymax;
				x [i] =  xcros_max;

				Graphics_polyline (g, i - index + 1,
						x.asArgumentToFunctionThatExpectsZeroBasedArray() + index,
						y.asArgumentToFunctionThatExpectsZeroBasedArray() + index);

				x [index] = xb;
				y [index] = yb;
				x [i] = xe;
				y [i] = ye;
			}
		} else {
			index = i;
		}
		y1 = y2;
		x1 = x2;
	}
	if (index < x.size - 1) {
		Graphics_polyline (g, x.size - index,
				x.asArgumentToFunctionThatExpectsZeroBasedArray() + index,
				y.asArgumentToFunctionThatExpectsZeroBasedArray() + index);
		x [index] = xb;
		y [index] = yb;
	}
}

void FunctionSeries_draw (FunctionSeries me, Graphics g, double xmin, double xmax, double ymin, double ymax, int extrapolate, bool garnish) {
	integer numberOfPoints = 1000;

	autoVEC x = raw_VEC (numberOfPoints);
	autoVEC y = raw_VEC (numberOfPoints);

	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	double fxmin = xmin, fxmax = xmax;
	if (! extrapolate) {
		if (xmax < my xmin || xmin > my xmax)
			return;
		if (xmin < my xmin)
			fxmin = my xmin;
		if (xmax > my xmax)
			fxmax = my xmax;
	}

	if (ymax <= ymin) {
		double x1, x2;
		FunctionSeries_getExtrema (me, fxmin, fxmax, & x1, & ymin, & x2, & ymax);
	}

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	/*
		Draw only the parts within [fxmin, fxmax] X [ymin, ymax].
	*/
	const double dx = (fxmax - fxmin) / (numberOfPoints - 1);
	for (integer i = 1; i <= numberOfPoints; i ++) {
		x [i] = fxmin + (i - 1.0) * dx;
		y [i] = FunctionSeries_evaluate (me, x [i]);
	}
	Graphics_polyline_clipTopBottom (g, x.get(), y.get(), ymin, ymax);
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void FunctionSeries_drawBasisFunction (FunctionSeries me, Graphics g, integer index, double xmin, double xmax, double ymin, double ymax, int extrapolate, bool garnish) {
	if (index < 1 || index > my numberOfCoefficients)
		return;
	autoFunctionSeries thee = Data_copy (me);

	thy coefficients.get () <<= 0.0;
	thy coefficients [index] = 1.0;
	thy numberOfCoefficients = index;
	FunctionSeries_draw (thee.get(), g, xmin, xmax, ymin, ymax, extrapolate, garnish);
}

void FunctionSeries_setCoefficient (FunctionSeries me, integer index, double value) {
	Melder_require (index > 0 && index <= my numberOfCoefficients,
		U"Index out of range [1, ", my numberOfCoefficients, U"].");
	Melder_require (value == 0.0 && index < my numberOfCoefficients,
		U"You should not remove the highest degree term.");
	my coefficients [index] = value;
}

/* end of file FunctionSeries.cpp */
