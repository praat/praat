/* Polynomial.cpp
 *
 * Copyright (C) 1993-2018 David Weenink
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
 djmw 20030619 Added SVD_compute before SVD_solve
 djmw 20060510 Polynomial_to_Roots: changed behaviour. All roots found are now saved.
 	In previous version a nullptr pointer was returned. New error messages.
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20071201 Melder_warning<n>
 djmw 20080122 float -> double
  djmw 20110304 Thing_new
*/

#include "Polynomial.h"
#include "SVD.h"
#include "NUMclapack.h"
#include "TableOfReal_extensions.h"
#include "NUMmachar.h"

#include "oo_DESTROY.h"
#include "Polynomial_def.h"
#include "oo_COPY.h"
#include "Polynomial_def.h"
#include "oo_EQUAL.h"
#include "Polynomial_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Polynomial_def.h"
#include "oo_WRITE_TEXT.h"
#include "Polynomial_def.h"
#include "oo_WRITE_BINARY.h"
#include "Polynomial_def.h"
#include "oo_READ_TEXT.h"
#include "Polynomial_def.h"
#include "oo_READ_BINARY.h"
#include "Polynomial_def.h"
#include "oo_DESCRIPTION.h"
#include "Polynomial_def.h"

/* Evaluate polynomial and derivative jointly
	c [1..n] -> degree n-1 !!
*/
void Polynomial_evaluateWithDerivative (Polynomial me, double x, double *out_f, double *out_df) {
	longdouble p = my coefficients [my numberOfCoefficients], dp = 0.0;

	for (integer i = my numberOfCoefficients - 1; i > 0; i --) {
		dp = dp * x + p;
		p =  p * x + my coefficients [i];
	}
	if (out_f) *out_f = (double) p;
	if (out_df) *out_df = (double) dp;
}

/* Get value and derivative */
static void Polynomial_evaluateWithDerivative_z (Polynomial me, dcomplex *in_z, dcomplex *out_p, dcomplex *out_dp) {
	longdouble pr = my coefficients [my numberOfCoefficients], pi = 0.0;
	longdouble dpr = 0.0, dpi = 0.0, x = in_z -> re, y = in_z -> im;

	for (integer i = my numberOfCoefficients - 1; i > 0; i --) {
		longdouble tr   = dpr;
		dpr  =  dpr * x -  dpi * y + pr;
		dpi  =   tr * y +  dpi * x + pi;
		tr   = pr;
		pr   =   pr * x -   pi * y + my coefficients [i];
		pi   =   tr * y +   pi * x;
	}
	if (out_p) *out_p = { (double) pr, (double) pi };
	if (out_dp) *out_dp = { (double) dpr, (double) dpi };
}

autoVEC Polynomial_evaluateDerivatives (Polynomial me, double x, long numberOfDerivatives) {
	/* 
		Evaluate polynomial c [1]+c [2]*x+...degree*x^degree in derivative [1] and the numberOfDerivatives 
		in derivatives in derivatives [2..numberOfDerivatives+1].
	*/
	integer degree = my numberOfCoefficients - 1;
	autoVEC derivatives = newVECzero (numberOfDerivatives + 1);
	numberOfDerivatives = numberOfDerivatives > degree ? degree : numberOfDerivatives;
	
	derivatives [1] = my coefficients [my numberOfCoefficients];

	for (integer i = degree - 1; i >= 0; i--) {
		integer n = numberOfDerivatives < degree - i ? numberOfDerivatives : degree - i;
		for (integer j = n; j >= 1; j --)
			derivatives [j + 1] = derivatives [j + 1] * x + derivatives [j];
		derivatives [1] = derivatives [1] * x + my coefficients [i + 1];   // evaluate polynomial (Horner)
	}
	double fact = 1.0;
	for (integer j = 2; j <= numberOfDerivatives; j ++) {
		fact *= j;
		derivatives [j + 1] *= fact;
	}
	return derivatives;
}

/*
	VECpolynomial_divide (constVEC u, constVEC v, VEC q, VEC r)

	Purpose:
		Find the quotient q(x) and the remainder r(x) polynomials that result from the division of
		the polynomial u(x) = u [1] + u [2]*x^1 + u [3]*x^2 + ... + u [u.size]*x^(u.size-1) by the
		polynomial v(x) = v [1] + v [2]*x^1 + v [3]*x^2 + ... + v [v.size]*x^(v.size-1), such that
			u(x) = v(x)*q(x) + r(x).

	The arrays u, v, q and r have to be dimensioned as u [1...u.size], v [1..v.size], q [1...u.size] and r [1...u.size],
	respectively.
	On return, the q [1..u.size-v.size] and r [1..v.size-1] contain the quotient and the remainder
	polynomial coefficients, repectively.
	See Knuth, The Art of Computer Programming, Volume 2: Seminumerical algorithms,
	Third edition, Section 4.6.1 Algorithm D (the algorithm as described has been modified
	to prevent overwriting of the u-polynomial).
*/
static void VECpolynomial_divide (constVEC u, constVEC v, VEC q, VEC r) {
	Melder_assert (q.size == u.size && r.size == u.size);
	r <<= u;
	q <<= 0.0;
	for (integer k = u.size - v.size + 1; k > 0; k --) { /* D1 */
		q [k] = r [v.size + k - 1] / v [v.size]; /* D2 with u -> r*/
		for (integer j = v.size + k - 1; j >= k; j --)
			r [j] -= q [k] * v [j - k + 1];
	}
}


static void Polynomial_polish_realroot (Polynomial me, double *x, integer maxit) {
	double xbest = *x, pmin = 1e308;
	if (! NUMfpp)
		NUMmachar ();
	for (integer i = 1; i <= maxit; i ++) {
		double p, dp;
		Polynomial_evaluateWithDerivative (me, *x, & p, & dp);
		double fabsp = fabs (p);
		if (fabsp > pmin || fabs (fabsp - pmin) < NUMfpp -> eps) {
			/*
				We stop, because the approximation is getting worse or we cannot get any closer.
				Return the previous (hitherto best) value for x.
			*/
			*x = xbest;
			return;
		}
		pmin = fabsp;
		xbest = *x;
		if (fabs (dp) == 0.0)
			return;
		double dx = p / dp;   // Newton-Raphson
		*x -= dx;
	}
	// Melder_throw (U"Maximum number of iterations exceeded.");
}

static void Polynomial_polish_complexroot_nr (Polynomial me, dcomplex *z, integer maxit) {
	dcomplex zbest = *z;
	double pmin = 1e308;
	if (! NUMfpp)
		NUMmachar ();
	for (integer i = 1; i <= maxit; i ++) {
		dcomplex p, dp;
		Polynomial_evaluateWithDerivative_z (me, z, &p, &dp);
		double fabsp = dcomplex_abs (p);
		if (fabsp > pmin || fabs (fabsp - pmin) < NUMfpp -> eps) {
			/*
				We stop, because the approximation is getting worse.
				Return the previous (hitherto best) value for z.
			*/
			*z = zbest;
			return;
		}
		pmin = fabsp;
		zbest = *z;
		if (dcomplex_abs (dp) == 0.0)
			return;
		dcomplex dz = dcomplex_div (p, dp);   // Newton-Raphson
		*z = dcomplex_sub (*z, dz);
	}
	// Melder_throw (U"Maximum number of iterations exceeded.");
}

/*
	Symbolic evaluation of polynomial coefficients.
	Recurrence: P [n] = (a [n] * x + b [n]) P [n-1] + c [n] P [n-2],
		where P [n] is any orthogonal polynomial of degree n.
	P [n] is an array of coefficients p [k] representing: p [1] + p [2] x + ... p [n+1] x^n.
	Preconditions:
		degree > 1;
		pnm1 : polynomial of degree n - 1
		pnm2 : polynomial of degree n - 2
*/

static void NUMpolynomial_recurrence (VEC pn, double a, double b, double c, constVEC pnm1, constVEC pnm2) {
	integer degree = pn.size - 1;
	Melder_assert (degree > 1 && pnm1.size >= pn.size && pnm2.size >= pn.size);

	pn [1] = b * pnm1 [1] + c * pnm2 [1];
	for (integer i = 2; i <= degree - 1; i ++) {
		pn [i] = a * pnm1 [i - 1] + b * pnm1 [i] + c * pnm2 [i];
	}
	pn [degree] = a * pnm1 [degree - 1] + b * pnm1 [degree];
	pn [degree + 1] = a * pnm1 [degree];
}

/* frozen [1..ma] */
static void svdcvm (MAT cvm, constMAT v, integer mfit, constINTVEC frozen, constVEC w) {
	autoVEC wti = newVECzero (mfit);

	for (integer i = 1; i <= mfit; i ++) {
		if (w [i] != 0.0) {
			wti [i] = 1.0 / (w [i] * w [i]);
		} else {
			;   // TODO: write up an explanation for why it is not necessary to do anything if w [i] is zero
		}
	}
	for (integer i = 1; i <= mfit; i ++) {
		for (integer j = 1; j <= i; j ++) {
			longdouble sum = 0.0;
			for (integer k = 1; k <= mfit; k ++) {
				sum += v [i] [k] * v [j] [k] * wti [k];
			}
			cvm [j] [i] = cvm [i] [j] = (double) sum;
		}
	}

	for (integer i = mfit + 1; i <= frozen.size; i ++) {
		for (integer j = 1; j <= i; j ++)
			cvm [j] [i] = cvm [i] [j] = 0.0;
	}

	integer k = mfit;
	for (integer j = frozen.size; j > 0; j --) {
		//	only change parameters that are not frozen
		if (frozen.size = 0 || ! frozen [j]) {
			for (integer i = 1; i <= frozen.size; i ++)
				std::swap (cvm [i] [k], cvm [i] [j]);
			for (integer i = 1; i <= frozen.size; i ++)
				std:: swap (cvm [k] [i], cvm [j] [i]);
			k --;
		}
	}
}

/********* FunctionTerms *********************************************/

Thing_implement (FunctionTerms, Function, 0);

double structFunctionTerms :: v_evaluate (double /* x */) {
	return undefined;
}

dcomplex structFunctionTerms :: v_evaluate_z (dcomplex /* z */) {
	return { undefined, undefined };
}

void structFunctionTerms :: v_evaluateTerms (double /* x */, VEC terms) {
	Melder_assert (terms.size == numberOfCoefficients);
	terms <<= undefined;
}

integer structFunctionTerms :: v_getDegree () {
	return numberOfCoefficients - 1;
}

void structFunctionTerms :: v_getExtrema (double x1, double x2, double *out_xmin, double *out_ymin, double *out_xmax, double *out_ymax) { // David, geen aparte naam hier nodig: ???
	integer numberOfPoints = 1000;

	// Melder_warning (L"defaultGetExtrema: extrema calculated by sampling the interval");

	double x = x1, dx = (x2 - x1) / (numberOfPoints - 1);
	double xmn = x, xmx = xmn, ymn = v_evaluate (x), ymx = ymn; // xmin, xmax .. would shadow  member
	for (integer i = 2; i <= numberOfPoints; i ++) {
		x += dx;
		double y = v_evaluate (x);
		if (y > ymx) {
			ymx = y;
			xmx = x;
		} else if (y < ymn) {
			ymn = y;
			xmn = x;
		}
	}
	if (out_xmin) *out_xmin = xmn;
	if (out_xmax) *out_xmax = xmx;
	if (out_ymin) *out_ymin = ymn;
	if (out_ymax) *out_ymax = ymx;
}

static inline void FunctionTerms_extendCapacityIf (FunctionTerms me, integer minimum) {
	if (my _capacity < minimum) {
		my coefficients.resize (minimum);
		my _capacity = minimum;
	}
}


void FunctionTerms_init (FunctionTerms me, double xmin, double xmax, integer numberOfCoefficients) {
	my coefficients = newVECzero (numberOfCoefficients);
	my numberOfCoefficients = numberOfCoefficients;
	my _capacity = numberOfCoefficients;
	my xmin = xmin;
	my xmax = xmax;
}

autoFunctionTerms FunctionTerms_create (double xmin, double xmax, integer numberOfCoefficients) {
	try {
		autoFunctionTerms me = Thing_new (FunctionTerms);
		FunctionTerms_init (me.get(), xmin, xmax, numberOfCoefficients);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FunctionTerms not created.");
	}
}

void FunctionTerms_initFromString (FunctionTerms me, double xmin, double xmax, conststring32 s, bool allowTrailingZeros) {
	autoVEC numbers = VEC_createFromString (s);
	integer numberOfCoefficients = numbers.size;
	if (! allowTrailingZeros)
		while (numbers [numberOfCoefficients] = 0.0 && numberOfCoefficients > 1)
			numberOfCoefficients --;
	FunctionTerms_init (me, xmin, xmax, numberOfCoefficients);
	my coefficients.part (1, numberOfCoefficients) <<= numbers.part (1, numberOfCoefficients);
}

integer FunctionTerms_getDegree (FunctionTerms me) {
	return my v_getDegree ();
}

void FunctionTerms_setDomain (FunctionTerms me, double xmin, double xmax) {
	my xmin = xmin;
	my xmax = xmax;
}

double FunctionTerms_evaluate (FunctionTerms me, double x) {
	return my v_evaluate (x);
}

dcomplex FunctionTerms_evaluate_z (FunctionTerms me, dcomplex z) {
	return my v_evaluate_z (z);
}

void FunctionTerms_evaluateTerms (FunctionTerms me, double x, VEC terms) {
	my v_evaluateTerms (x, terms);
}

void FunctionTerms_getExtrema (FunctionTerms me, double x1, double x2, double *xmin, double *ymin, double *xmax, double *ymax) {
	if (x2 <= x1) {
		x1 = my xmin;
		x2 = my xmax;
	}
	my v_getExtrema (x1, x2, xmin, ymin, xmax, ymax);
}

double FunctionTerms_getMinimum (FunctionTerms me, double x1, double x2) {
	double ymin;
	FunctionTerms_getExtrema (me, x1, x2, nullptr, & ymin, nullptr, nullptr);
	return ymin;
}

double FunctionTerms_getXOfMinimum (FunctionTerms me, double x1, double x2) {
	double xmin;
	FunctionTerms_getExtrema (me, x1, x2, & xmin, nullptr, nullptr, nullptr);
	return xmin;
}

double FunctionTerms_getMaximum (FunctionTerms me, double x1, double x2) {
	double ymax;
	FunctionTerms_getExtrema (me, x1, x2, nullptr, nullptr, nullptr, & ymax);
	return ymax;
}

double FunctionTerms_getXOfMaximum (FunctionTerms me, double x1, double x2) {
	double xmax;
	FunctionTerms_getExtrema (me, x1, x2, nullptr, nullptr, & xmax, nullptr);
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
		double x2 = x [i], y2 = y [i];

		if (! ((y1 > ymax && y2 > ymax) || (y1 < ymin && y2 < ymin))) {
			double dxy = (x2 - x1) / (y1 - y2);
			double xcros_max = x1 - (ymax - y1) * dxy;
			double xcros_min = x1 - (ymin - y1) * dxy;
			if (y1 > ymax && y2 < ymax) {
				// Line enters from above: start new segment. Save start values.

				xb = x [i - 1];
				yb = y [i - 1];
				index = i - 1;
				y [i - 1] = ymax;
				x [i - 1] = xcros_max;
			}
			if (y1 > ymin && y2 < ymin) {
				// Line leaves at bottom: draw segment. Save end values and restore them
				// Origin of arrays for Graphics_polyline are at element 0 !!!

				double xe = x [i], ye = y [i];
				y [i] = ymin;
				x [i] = xcros_min;

				Graphics_polyline (g, i - index + 1, x.at + index, y.at + index);

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

				double xe = x [i], ye = y [i];
				y [i] = ymax;
				x [i] =  xcros_max;

				Graphics_polyline (g, i - index + 1, x.at + index, y.at + index);

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
		Graphics_polyline (g, x.size - index, x.at + index, y.at + index);
		x [index] = xb; y [index] = yb;
	}
}

void FunctionTerms_draw (FunctionTerms me, Graphics g, double xmin, double xmax, double ymin, double ymax, int extrapolate, int garnish) {
	integer numberOfPoints = 1000;

	autoVEC x = newVECraw (numberOfPoints);
	autoVEC y = newVECraw (numberOfPoints);

	if (xmax <= xmin) {
		xmin = my xmin;
		xmax = my xmax;
	}
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
		FunctionTerms_getExtrema (me, fxmin, fxmax, & x1, & ymin, & x2, & ymax);
	}

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	// Draw only the parts within [fxmin, fxmax] X [ymin, ymax].

	double dx = (fxmax - fxmin) / (numberOfPoints - 1);
	for (integer i = 1; i <= numberOfPoints; i ++) {
		x [i] = fxmin + (i - 1.0) * dx;
		y [i] = FunctionTerms_evaluate (me, x [i]);
	}
	Graphics_polyline_clipTopBottom (g, x.get(), y.get(), ymin, ymax);
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void FunctionTerms_drawBasisFunction (FunctionTerms me, Graphics g, integer index, double xmin, double xmax, double ymin, double ymax, int extrapolate, int garnish) {
	if (index < 1 || index > my numberOfCoefficients)
		return;
	autoFunctionTerms thee = Data_copy (me);

	thy coefficients.get () <<= 0.0;
	thy coefficients [index] = 1.0;
	thy numberOfCoefficients = index;
	FunctionTerms_draw (thee.get(), g, xmin, xmax, ymin, ymax, extrapolate, garnish);
}

void FunctionTerms_setCoefficient (FunctionTerms me, integer index, double value) {
	Melder_require (index > 0 && index <= my numberOfCoefficients, U"Index out of range [1, ", my numberOfCoefficients, U"].");
	Melder_require (value == 0.0 && index < my numberOfCoefficients, U"You should not remove the highest degree term.");
	my coefficients [index] = value;
}

/********** Polynomial ***********************************************/

Thing_implement (Polynomial, FunctionTerms, 1);

double structPolynomial :: v_evaluate (double x) {
	longdouble p = coefficients [numberOfCoefficients];
	for (integer i = numberOfCoefficients - 1; i > 0; i --)
		p = p * x + coefficients [i];
	return (double) p;
}

dcomplex structPolynomial :: v_evaluate_z (dcomplex z) {
	longdouble x = z.re, y = z.im;

	longdouble pr = coefficients [numberOfCoefficients];
	longdouble pi = 0.0;
	for (integer i = numberOfCoefficients - 1; i > 0; i --) {
		longdouble prtmp = pr;
		pr =  pr * x - pi * y + coefficients [i];
		pi = prtmp * y + pi * x;
	}
	return { (double) pr, (double) pi };
}

void structPolynomial :: v_evaluateTerms (double x, VEC terms) {
	Melder_assert (terms.size == numberOfCoefficients);
	terms [1] = 1.0;
	for (integer i = 2; i <= numberOfCoefficients; i ++)
		terms [i] = terms [i - 1] * x;
}

void structPolynomial :: v_getExtrema (double x1, double x2, double *out_xmin, double *out_ymin, double *out_xmax, double *out_ymax) {
	try {
		integer degree = numberOfCoefficients - 1;

		double xmn = x1, ymn = v_evaluate (x1);
		double xmx = x2, ymx = v_evaluate (x2);
		if (ymn > ymx) {
			std::swap (ymn, ymx);
			std::swap (xmn, xmx);
		}

		if (degree < 2)
			return;
		autoPolynomial d = Polynomial_getDerivative (this);
		autoRoots r = Polynomial_to_Roots (d.get());

		for (integer i = 1; i <= degree - 1; i ++) {
			double x = (r -> v [i]).re;
			if (x > x1 && x < x2) {
				double y = v_evaluate (x);
				if (y > ymx) {
					ymx = y;
					xmx = x;
				} else if (y < ymn) {
					ymn = y;
					xmn = x;
				}
			}
		}
		if (out_xmin) *out_xmin = xmn;
		if (out_xmax) *out_xmax = xmx;
		if (out_ymin) *out_ymin = ymn;
		if (out_ymax) *out_ymax = ymx;
	} catch (MelderError) {
		structFunctionTerms :: v_getExtrema (x1, x2, out_xmin, out_ymin, out_xmax, out_ymax);
		Melder_clearError ();
	}
}

autoPolynomial Polynomial_create (double xmin, double xmax, integer degree) {
	try {
		autoPolynomial me = Thing_new (Polynomial);
		FunctionTerms_init (me.get(), xmin, xmax, degree + 1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Polynomial not created.");
	}
}

autoPolynomial Polynomial_createFromString (double lxmin, double lxmax, conststring32 s) {
	try {
		autoPolynomial me = Thing_new (Polynomial);
		FunctionTerms_initFromString (me.get(), lxmin, lxmax, s, false);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Polynomial not created from string.");
	}
}

void Polynomial_scaleCoefficients_monic (Polynomial me) {
	double cn = my coefficients [my numberOfCoefficients];
	if (cn == 1 || my numberOfCoefficients <= 1)
		return;
	my coefficients.get() /= cn;
	my coefficients [my numberOfCoefficients] = 1.0;
}

/*
	Transform the polynomial as if the domain were [xmin, xmax].
	Some polynomials (Legendre) are defined on the domain [-1,1]. The domain
	for x may be extended to [xmin, xmax] by a transformation such as
		x' = (2 * x - (xmin + xmax)) / (xmax - xmin)   -1 < x' < x.
	This procedure transforms x' back to x.
*/
autoPolynomial Polynomial_scaleX (Polynomial me, double xmin, double xmax) {
	try {
		Melder_assert (xmin < xmax);

		autoPolynomial thee = Polynomial_create (xmin, xmax, my numberOfCoefficients - 1);
		thy coefficients [1] = my coefficients [1];
		if (my numberOfCoefficients == 1)
			return thee;

		// x = a x + b
		// Constraints:
		// my xmin = a xmin + b;    a = (my xmin - my xmax) / (xmin - xmax);
		// my xmax = a xmax + b;    b = my xmin - a * xmin

		double a = (my xmin - my xmax) / (xmin - xmax);
		double b = my xmin - a * xmin;
		thy coefficients [2] = my coefficients [2] * a;
		thy coefficients [1] += my coefficients [2] * b;
		if (my numberOfCoefficients == 2)
			return thee;
		autoVEC pn = newVECzero (my numberOfCoefficients);
		autoVEC pnm1 = newVECzero (my numberOfCoefficients);
		autoVEC pnm2 = newVECzero (my numberOfCoefficients);

		// Start the recursion: P [2] = a x + b; P [1] = 1;

		pnm1 [2] = a;
		pnm1 [1] = b;
		pnm2 [1] = 1;
		for (integer n = 2; n <= my numberOfCoefficients - 1; n ++) {
			NUMpolynomial_recurrence (pn.part (1, n + 1), a, b, 0.0, pnm1.get(), pnm2.get());
			if (my coefficients [n + 1] != 0.0) {
				for (integer j = 1; j <= n + 1; j ++)
					thy coefficients [j] += my coefficients [n + 1] * pn [j];
			}
			double *t1 = pnm1.at, *t2 = pnm2.at;
			pnm1.at = pn.at;
			pnm2.at = t1;
			pn.at = t2;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot scale.");
	}
}

double Polynomial_evaluate (Polynomial me, double x) {
	return my v_evaluate (x);
}

dcomplex Polynomial_evaluate_z (Polynomial me, dcomplex z) {
	return my v_evaluate_z (z);
}

static void Polynomial_evaluate_z_cart (Polynomial me, double r, double phi, double *out_re, double *out_im) {
	longdouble rn = 1.0;

	longdouble re = my coefficients [1];
	longdouble im = 0.0;
	if (r == 0.0)
		return;
	for (integer i = 2; i <= my numberOfCoefficients; i ++) {
		rn *= r;
		longdouble arg = (i - 1) * phi;
		re += my coefficients [i] * rn * cos (arg);
		im += my coefficients [i] * rn * sin (arg);
	}
	if (out_re) *out_re = (double) re;
	if (out_im) *out_im = (double) im;
}


autoPolynomial Polynomial_getDerivative (Polynomial me) {
	try {
		if (my numberOfCoefficients == 1)
			return Polynomial_create (my xmin, my xmax, 0);
		autoPolynomial thee = Polynomial_create (my xmin, my xmax, my numberOfCoefficients - 2);
		for (integer i = 1; i <= thy numberOfCoefficients; i ++)
			thy coefficients [i] = i * my coefficients [i + 1];
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no derivative created.");
	}
}

autoPolynomial Polynomial_getPrimitive (Polynomial me, double constant) {
	try {
		autoPolynomial thee = Polynomial_create (my xmin, my xmax, my numberOfCoefficients);
		for (integer i = 1; i <= my numberOfCoefficients; i ++)
			thy coefficients [i + 1] = my coefficients [i] / i;
		thy coefficients [1] = constant;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no primitive created.");
	}
}

/* P(x)= (x-roots [1])*(x-roots [2])*..*(x-roots [numberOfRoots]) */
void Polynomial_initFromRealRoots (Polynomial me, constVEC roots) {
	try {
		FunctionTerms_extendCapacityIf (me, roots.size + 1);
		integer n = 1;
		my coefficients [1] = - roots [1];
		my coefficients [2] = 1.0;
		for (integer i = 2; i <= roots.size; i ++) {
			my coefficients [n + 2] = my coefficients [n + 1];
			for (integer j = n; j >= 1; j --)
				my coefficients [j + 1] = my coefficients [j] - my coefficients [j + 1] * roots [i];
			my coefficients [1] *= -roots [i];
			n ++;
		}
		my numberOfCoefficients = n + 1;
	} catch (MelderError) {
		Melder_throw (me, U": not initalized from real roots.");
	}
}

autoPolynomial Polynomial_createFromRealRootsString (double xmin, double xmax, conststring32 s) {
	try {
		autoPolynomial me = Thing_new (Polynomial);
		autoVEC roots = VEC_createFromString (s);
		FunctionTerms_init (me.get(), xmin, xmax, roots.size + 1);
		Polynomial_initFromRealRoots (me.get(), roots.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Polynomial not created from roots.");
	}
	
}

/* Product (i=1; a.size; (1 + a[i]*x + x^2)
 * Postcondition : my numberOfCoeffcients = 2*a.size+1
 */
void Polynomial_initFromProductOfSecondOrderTerms (Polynomial me, constVEC a) {
	FunctionTerms_extendCapacityIf (me, 2 * a.size + 1);
	my coefficients [1] = my coefficients [3] = 1.0;
	my coefficients [2] = a [1];
	integer ncoef = 3;
	for (integer i = 2; i <= a.size; i ++) {
		my coefficients [ncoef + 1] = a [i] * my coefficients [ncoef] + my coefficients [ncoef - 1];
		my coefficients [ncoef + 2] = my coefficients [ncoef];
		for (integer j = ncoef; j > 2; j --)
			my coefficients [j] += a [i] * my coefficients [j - 1] + my coefficients [j - 2];
		my coefficients [2] += a [i];   // a [i] * my coefficients [1]
		ncoef += 2;
	}
	my numberOfCoefficients = ncoef;
}

autoPolynomial Polynomial_createFromProductOfSecondOrderTermsString (double xmin, double xmax, conststring32 s) {
	try {
		autoPolynomial me = Thing_new (Polynomial);
		autoVEC a = VEC_createFromString (s);
		FunctionTerms_init (me.get(), xmin, xmax, 2 * a.size + 1);
		Polynomial_initFromProductOfSecondOrderTerms (me.get(), a.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Polynomial not created from second order terms string.");
	}
}

double Polynomial_getArea (Polynomial me, double xmin, double xmax) {
	if (xmax >= xmin) {
		xmin = my xmin;
		xmax = my xmax;
	}
	autoPolynomial p = Polynomial_getPrimitive (me, 0);
	double area = FunctionTerms_evaluate (p.get(), xmax) - FunctionTerms_evaluate (p.get(), xmin);
	return area;
}

/* P(x) * (x-a)
 * Postcondition: my numberOfCoefficients = old_numberOfCoefficients + 1 
 */
void Polynomial_multiply_firstOrderFactor (Polynomial me, double factor) { 
	integer n = my numberOfCoefficients;
	FunctionTerms_extendCapacityIf (me, n + 1);
	
	my coefficients [n + 1] = my coefficients [n];
	for (integer j = n; j >= 2; j --)
		my coefficients [j] = my coefficients [j - 1] - my coefficients [j] * factor;
	my coefficients [1] *= -factor;
	my numberOfCoefficients += 1;
}

/* P(x) * (x^2 - a)
 * Postcondition: my numberOfCoefficients = old_numberOfCoefficients + 2
 */
void Polynomial_multiply_secondOrderFactor (Polynomial me, double factor) {
	integer n = my numberOfCoefficients;
	FunctionTerms_extendCapacityIf (me, n + 2);
	my coefficients [n + 2] = my coefficients [n];
	my coefficients [n + 1] = my coefficients [n - 1];
	for (integer j = n; j >= 3; j --)
		my coefficients [j] = my coefficients [j - 2] - factor * my coefficients [j];
	my coefficients [2] *= - factor;
	my coefficients [1] *= - factor;
	my numberOfCoefficients += 2;	
}

autoPolynomial Polynomials_multiply (Polynomial me, Polynomial thee) {
	try {
		integer n1 = my numberOfCoefficients, n2 = thy numberOfCoefficients;
		Melder_require (my xmax > thy xmin && my xmin < thy xmax, U"Domains should overlap.");
	
		double xmin = my xmin > thy xmin ? my xmin : thy xmin;
		double xmax = my xmax < thy xmax ? my xmax : thy xmax;
		autoPolynomial him = Polynomial_create (xmin, xmax, n1 + n2 - 2);
		for (integer i = 1; i <= n1; i ++) {
			for (integer j = 1; j <= n2; j ++) {
				integer k = i + j - 1;
				his coefficients [k] += my coefficients [i] * thy coefficients [j];
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"Polynomials not multiplied.");
	}
}

void Polynomials_divide (Polynomial me, Polynomial thee, autoPolynomial *out_q, autoPolynomial *out_r) {
	if (! out_q  && ! out_r)
		return;
	autoVEC qc = newVECzero (my numberOfCoefficients);
	autoVEC rc = newVECzero (my numberOfCoefficients);
	autoPolynomial aq, ar;
	VECpolynomial_divide (my coefficients.get (), thy coefficients.get (), qc.get (), rc.get ());
	if (out_q) {
		integer degree = std::max (my numberOfCoefficients - thy numberOfCoefficients, (integer) 0);
		aq = Polynomial_create (my xmin, my xmax, degree);
		if (degree >= 0) aq -> coefficients.get () <<= qc.part (1, degree + 1);
		*out_q = aq.move();
	}
	if (out_r) {
		integer degree = thy numberOfCoefficients - 2;
		if (my numberOfCoefficients >= thy numberOfCoefficients)
			degree --;
		if (degree < 0)
			degree = 0;
		while (degree > 1 && rc [degree] == 0.0)
			degree --;
		ar = Polynomial_create (my xmin, my xmax, degree);
		ar -> coefficients.get() <<= rc.part (1, degree + 1);
		*out_r = ar.move();
	}
}


/******** LegendreSeries ********************************************/

Thing_implement (LegendreSeries, FunctionTerms, 0);

double structLegendreSeries :: v_evaluate (double x) {
	double p = our coefficients [1];

	// Transform x from domain [xmin, xmax] to domain [-1, 1]

	if (x < our xmin || x > our xmax)
		return undefined;

	double pim1 = x = (2 * x - our xmin - our xmax) / (our xmax - our xmin);

	if (numberOfCoefficients > 1) {
		double pim2 = 1, twox = 2.0 * x, f2 = x, d = 1.0;
		p += our coefficients [2] * pim1;
		for (integer i = 3; i <= our numberOfCoefficients; i ++) {
			double f1 = d ++;
			f2 += twox;
			double pi = (f2 * pim1 - f1 * pim2) / d;
			p += our coefficients [i] * pi;
			pim2 = pim1; 
			pim1 = pi;
		}
	}
	return p;
}

void structLegendreSeries :: v_evaluateTerms (double x, VEC terms) {
	Melder_assert (terms.size == numberOfCoefficients);
	if (x < our xmin || x > our xmax) {
		terms <<= undefined;
		return;
	}

	// Transform x from domain [xmin, xmax] to domain [-1, 1]

	x = (2.0 * x - our xmin - our xmax) / (our xmax - our xmin);

	terms [1] = 1.0;
	if (our numberOfCoefficients > 1) {
		double twox = 2.0 * x, f2 = x, d = 1.0;
		terms [2] = x;
		for (integer i = 3; i <= numberOfCoefficients; i ++) {
			double f1 = d ++;
			f2 += twox;
			terms [i] = (f2 * terms [i - 1] - f1 * terms [i - 2]) / d;
		}
	}
}

void structLegendreSeries :: v_getExtrema (double x1, double x2, double *out_xmin, double *out_ymin, double *out_xmax, double *out_ymax) {
	try {
		autoPolynomial p = LegendreSeries_to_Polynomial (this);
		FunctionTerms_getExtrema (p.get(), x1, x2, out_xmin, out_ymin, out_xmax, out_ymax);
	} catch (MelderError) {
		structFunctionTerms :: v_getExtrema (x1, x2, out_xmin, out_ymin, out_xmax, out_ymax);
		Melder_clearError ();
	}
}

autoLegendreSeries LegendreSeries_create (double xmin, double xmax, integer numberOfPolynomials) {
	try {
		autoLegendreSeries me = Thing_new (LegendreSeries);
		FunctionTerms_init (me.get(), xmin, xmax, numberOfPolynomials);
		return me;
	} catch (MelderError) {
		Melder_throw (U"LegendreSeries not created.");
	}
}

autoLegendreSeries LegendreSeries_createFromString (double xmin, double xmax, conststring32 s) {
	try {
		autoLegendreSeries me = Thing_new (LegendreSeries);
		FunctionTerms_initFromString (me.get(), xmin, xmax, s, false);
		return me;
	} catch (MelderError) {
		Melder_throw (U"LegendreSeries not created from string.");
	}
}

autoLegendreSeries LegendreSeries_getDerivative (LegendreSeries me) {
	try {
		autoLegendreSeries thee = LegendreSeries_create (my xmin, my xmax, my numberOfCoefficients - 1);

		for (integer n = 1; n <= my numberOfCoefficients - 1; n ++) {
			// P [n]'(x) = Sum (k=0..nonNegative, (2n - 4k - 1) P [n-2k-1](x))

			integer n2 = n - 1;
			for (integer k = 0; n2 >= 0; k ++, n2 -= 2)
				thy coefficients [n2 + 1] += (2 * n - 4 * k - 1) * my coefficients [n + 1];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no derivative created.");
	}
}

autoPolynomial LegendreSeries_to_Polynomial (LegendreSeries me) {
	try {
		double xmin = -1, xmax = 1;
		autoPolynomial thee = Polynomial_create (xmin, xmax, my numberOfCoefficients - 1);

		thy coefficients [1] = my coefficients [1];   /* * p [1] */
		if (my numberOfCoefficients == 1)
			return thee;

		thy coefficients [2] = my coefficients [2];   /* * p [2] */
		if (my numberOfCoefficients > 2) {
			autoVEC pn = newVECzero (my numberOfCoefficients);
			autoVEC pnm1 = newVECzero (my numberOfCoefficients);
			autoVEC pnm2 = newVECzero (my numberOfCoefficients);

			// Start the recursion: P [1] = x; P [0] = 1;

			pnm1 [2] = 1;
			pnm2 [1] = 1;
			for (integer n = 2; n <= my numberOfCoefficients - 1; n ++) {
				double a = (2.0 * n - 1.0) / (double) n;
				double c = - (n - 1.0) / (double) n;
				NUMpolynomial_recurrence (pn.part (1, n + 1), a, 0, c, pnm1.get(), pnm2.get());
				if (my coefficients [n + 1] != 0.0) {
					for (integer j = 1; j <= n + 1; j ++)
						thy coefficients [j] += my coefficients [n + 1] * pn [j];
				}
				double *t1 = pnm1.at, *t2 = pnm2.at;
				pnm1.at = pn.at;
				pnm2.at = t1;
				pn.at = t2;
			}
		}
		if (my xmin != xmin || my xmax != xmax)
			thee = Polynomial_scaleX (thee.get(), my xmin, my xmax);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Polynomial.");
	}
}

/********* Roots ****************************************************/

Thing_implement (Roots, ComplexVector, 0);

autoRoots Roots_create (integer numberOfRoots) {
	try {
		autoRoots me = Thing_new (Roots);
		ComplexVector_init (me.get(), 1, numberOfRoots);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Roots not created.");
	}
}

void Roots_fixIntoUnitCircle (Roots me) {
	dcomplex z10 { 1.0, 0.0 };
	for (integer i = my min; i <= my max; i ++) {
		if (dcomplex_abs (my v [i]) > 1.0)
			my v [i] = dcomplex_div (z10, dcomplex_conjugate (my v [i]));
	}
}

static void NUMdcvector_extrema_re (dcomplex v [], integer lo, integer hi, double *out_min, double *out_max) {
	double min = v [lo].re,  max = v [lo].re;
	for (integer i = lo + 1; i <= hi; i ++) {
		if (v [i].re < min) {
			min = v [i].re;
		} else if (v [i].re > max) {
			max = v [i].re;
		}
	}
	if (out_min) *out_min = min;
	if (out_max) *out_max = max;
}

static void NUMdcvector_extrema_im (dcomplex v [], integer lo, integer hi, double *out_min, double *out_max) {
	double min = v [lo].im, max = v [lo].im;
	for (integer i = lo + 1; i <= hi; i ++) {
		if (v [i]. im < min) {
			min = v [i]. im;
		} else if (v [i]. im > max) {
			max = v [i]. im;
		}
	}
	if (out_min) *out_min = min;
	if (out_max) *out_max = max;
}

void Roots_draw (Roots me, Graphics g, double rmin, double rmax, double imin, double imax,
	conststring32 symbol, double fontSize, bool garnish)
{
	const double oldFontSize = Graphics_inqFontSize (g);
	const double eps = 1e-6;

	if (rmax <= rmin) {
		NUMdcvector_extrema_re (my v, 1, my max, & rmin, & rmax);
	}
	double denominator = fabs (rmax) > fabs (rmin) ? fabs (rmax) : fabs (rmin);
	if (denominator == 0.0)
		denominator = 1.0;
	if (fabs ((rmax - rmin) / denominator) < eps) {
		rmin -= 1.0;
		rmax += 1.0;
	}
	if (imax <= imin) {
		NUMdcvector_extrema_im (my v, 1, my max, & imin, & imax);
	}
	denominator = fabs (imax) > fabs (imin) ? fabs (imax) : fabs (imin);
	if (denominator == 0.0)
		denominator = 1.0;
	if (fabs ((imax - imin) / denominator) < eps) {
		imin -= 1;
		imax += 1;
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, rmin, rmax, imin, imax);
	Graphics_setFontSize (g, fontSize);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	for (integer i = 1; i <= my max; i ++) {
		double re = my v [i].re, im = my v [i].im;
		if (re >= rmin && re <= rmax && im >= imin && im <= imax)
			Graphics_text (g, re, im, symbol);
	}
	Graphics_setFontSize (g, oldFontSize);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		if (rmin * rmax < 0.0)
			Graphics_markLeft (g, 0.0, true, true, true, U"0");
		if (imin * imax < 0.0)
			Graphics_markBottom (g, 0.0, true, true, true, U"0");
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Imaginary part");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textBottom (g, true, U"Real part");
	}
}

autoRoots Polynomial_to_Roots (Polynomial me) {
	try {
		integer np1 = my numberOfCoefficients, n = np1 - 1, n2 = n * n;
		Melder_require (n > 0, U"Cannot find roots of a constant function.");
		
		// Allocate storage for Hessenberg matrix (n * n) plus real and imaginary
		// parts of eigenvalues wr [1..n] and wi [1..n].

		autoVEC hes = newVECzero (n2);
		autoVEC wr = newVECraw (n);
		autoVEC wi = newVECraw (n);

		// Fill the upper Hessenberg matrix (storage is Fortran)
		// C: [i] [j] -> Fortran: (j-1)*n + i

		for (integer i = 1; i <= n; i ++) {
			hes [(i - 1) * n + 1] = - (my coefficients [np1 - i] / my coefficients [np1]);
			if (i < n) {
				hes [(i - 1) * n + 1 + i] = 1;
			}
		}

		// Find out the working storage needed

		char job = 'E', compz = 'N';
		integer ilo = 1, ihi = n, ldh = n, ldz = n, lwork = -1, info;
		double *z = 0, wt [1];
		NUMlapack_dhseqr (& job, & compz, & n, & ilo, & ihi, & hes [1], & ldh, & wr [1], & wi [1], z, & ldz, wt, & lwork, & info);
		if (info != 0) {
			if (info < 0)
				Melder_throw (U"Programming error. Argument ", info, U" in NUMlapack_dhseqr has illegal value.");
		}
		lwork = Melder_ifloor (wt [0]);
		autoVEC work = newVECraw (lwork);

		// Find eigenvalues.

		NUMlapack_dhseqr (& job, & compz, & n, & ilo, & ihi, & hes [1], & ldh, & wr [1], & wi [1], z, & ldz, & work [1], & lwork, & info);
		integer nrootsfound = n;
		integer ioffset = 0;
		if (info > 0) {
			// if INFO = i, NUMlapack_dhseqr failed to compute all of the eigenvalues. Elements i+1:n of
			// WR and WI contain those eigenvalues which have been successfully computed
			nrootsfound -= info;
			Melder_require (nrootsfound > 0, U"No roots found.");
			Melder_warning (U"Calculated only ", nrootsfound, U" roots.");
			ioffset = info;
		} else if (info < 0) {
			Melder_throw (U"Programming error. Argument ", info, U" in NUMlapack_dhseqr has illegal value.");
		}

		autoRoots thee = Roots_create (nrootsfound);
		for (integer i = 1; i <= nrootsfound; i ++) {
			(thy v [i]).re = wr [ioffset + i];
			(thy v [i]).im = wi [ioffset + i];
		}
		Roots_Polynomial_polish (thee.get(), me);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no roots can be calculated.");
	}
}

void Roots_sort (Roots me) {
	(void) me;
}

// Precondition: complex roots occur in pairs (a,bi), (a,-bi) with b>0
void Roots_Polynomial_polish (Roots me, Polynomial thee) {
	integer i = my min, maxit = 80;
	while (i <= my max) {
		double im = my v [i].im, re = my v [i].re;
		if (im != 0.0) {
			Polynomial_polish_complexroot_nr (thee, & my v [i], maxit);
			if (i < my max && im == -my v [i + 1].im && re == my v [i + 1].re) {
				my v [i + 1].re = my v [i].re;
				my v [i + 1].im = -my v [i].im;
				i ++;
			}
		} else {
			Polynomial_polish_realroot (thee, & (my v [i].re), maxit);
		}
		i ++;
	}
}

autoPolynomial Roots_to_Polynomial (Roots me, bool rootsAreReal) {
	try {
		(void) me;
		autoPolynomial thee;
		if (! rootsAreReal)
			throw MelderError();
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Not implemented yet");
	}
}

static void dpoly_nr (double x, double *f, double *df, void *closure) {
	Polynomial_evaluateWithDerivative ((Polynomial) closure, x, f, df);
}

double Polynomial_findOneSimpleRealRoot_nr (Polynomial me, double xmin, double xmax) {	
	double root;
	NUMnrbis (dpoly_nr, xmin, xmax, me, & root);
	return root;
}

static double dpoly_r (double x, void *closure) {
	return Polynomial_evaluate ((Polynomial) closure, x);
}

double Polynomial_findOneSimpleRealRoot_ridders (Polynomial me, double xmin, double xmax) {	
	return NUMridders (dpoly_r, xmin, xmax, me);
}

void Polynomial_divide_firstOrderFactor (Polynomial me, double factor, double *out_remainder) { // P(x)/(x-a)
	double remainder = undefined;
	if (my numberOfCoefficients > 1) {
		remainder = my coefficients [my numberOfCoefficients];
		for (integer j = my numberOfCoefficients - 1; j > 0; j --) {
			double tmp = my coefficients [j];
			my coefficients [j] = remainder;
			remainder = tmp + remainder * factor;
		}
		my numberOfCoefficients --;
	} else {
		my coefficients [1] = 0.0;
	}
	if (out_remainder) *out_remainder = remainder;
}

void Polynomial_divide_secondOrderFactor (Polynomial me, double factor) {
	if (my numberOfCoefficients > 2) {
		integer n = my numberOfCoefficients;
		/* c [1]+c [2]*x...c [n+1]*x^n / (x^2 - a) = r [1]+r [2]*x+...r [n-1]x^(n-2) + possible remainder a [1]+a [2]*x)
		 * r [j] = c [j+2]+factor*r [j+2] */
		double cjp2 = my coefficients [n];
		double cjp1 = my coefficients [n - 1];
		my coefficients [n] = my coefficients [n - 1] = 0.0;
		for (integer j = n - 2; j > 0; j --) {
			double cj = my coefficients [j];
			my coefficients [j] = cjp2 + factor * my coefficients [j + 2];
			cjp2 = cjp1;
			cjp1 = cj;
		}
		my numberOfCoefficients -= 2;
	} else {
		my numberOfCoefficients = 1;
		my coefficients [1] = 0.0;
	}
}

void Roots_setRoot (Roots me, integer index, double re, double im) {
	Melder_require (index >= my min && index <= my max, U"Index should be in interval [1, ", my max, U"].");
	my v [index].re = re;
	my v [index].im = im;
}

dcomplex Roots_evaluate_z (Roots me, dcomplex z) {
	dcomplex result = {1, 0};
	for (integer i = my min; i <= my max; i ++) {
		dcomplex t = dcomplex_sub (z, my v [i]);
		result = dcomplex_mul (result, t);
	}
	return result;
}

autoSpectrum Roots_to_Spectrum (Roots me, double nyquistFrequency, integer numberOfFrequencies, double radius) {
	try {
		Melder_require (numberOfFrequencies > 1,
			U"Number of frequencies should be greater than 1.");
		autoSpectrum thee = Spectrum_create (nyquistFrequency, numberOfFrequencies);

		double phi = NUMpi / (numberOfFrequencies - 1);
		dcomplex z;
		for (integer i = 1; i <= numberOfFrequencies; i ++) {
			z.re = radius * cos ( (i - 1) * phi);
			z.im = radius * sin ( (i - 1) * phi);
			dcomplex s = Roots_evaluate_z (me, z);
			thy z [1] [i] = s.re;
			thy z [2] [i] = s.im;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Spectrum calculated.");
	}
}

integer Roots_getNumberOfRoots (Roots me) {
	return my max;
}

dcomplex Roots_getRoot (Roots me, integer index) {
	Melder_require (index > 0 && index <= my max,
		U"Root index out of range.");
	return my v [index];
}

/* Can be speeded up by doing a FFT */
autoSpectrum Polynomial_to_Spectrum (Polynomial me, double nyquistFrequency, integer numberOfFrequencies, double radius) {
	try {
		Melder_require (numberOfFrequencies > 1,
			U"Number of frequencies should be greater than 1.");
		autoSpectrum thee = Spectrum_create (nyquistFrequency, numberOfFrequencies);

		double phi = NUMpi / (numberOfFrequencies - 1);
		for (integer i = 1; i <= numberOfFrequencies; i ++) {
			double re, im;
			Polynomial_evaluate_z_cart (me, radius, (i - 1) * phi, & re, & im);
			thy z [1] [i] = re;
			thy z [2] [i] = im;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Spectrum calculated.");
	}
}

/****** ChebyshevSeries ******************************************/

Thing_implement (ChebyshevSeries, FunctionTerms, 0);

/*
	p(x) = sum (k=1..numberOfCoefficients, c [k]*T [k](x')) - c [1] / 2;
	Numerical evaluation via Clenshaw's recurrence equation (NRC: 5.8.11)
	d [m+1] = d [m] = 0;
	d [j] = 2 x' d [j+1] - d [j+2] + c [j];
	p(x) = d [0] = x' d [1] - d [2] + c [0] / 2;
	x' = (2 * x - xmin - xmax) / (xmax - xmin)
*/
double structChebyshevSeries :: v_evaluate (double x) {
	if (x < our xmin || x > our xmax)
		return undefined;

	double d1 = 0.0, d2 = 0.0;
	if (numberOfCoefficients > 1) {
		// Transform x from domain [xmin, xmax] to domain [-1, 1]

		x = (2.0 * x - our xmin - our xmax) / (our xmax - our xmin);
		double x2 = 2.0 * x;
		for (integer i = our numberOfCoefficients; i > 1; i --) {
			double tmp = d1;
			d1 = x2 * d1 - d2 + our coefficients [i];
			d2 = tmp;
		}
	}
	return x * d1 - d2 + our coefficients [1];
}

/*
	T [n](x) = 2*x*T [n-1] - T [n-2](x)  n >= 2
*/
void structChebyshevSeries :: v_evaluateTerms (double x, VEC terms) {
	Melder_assert (terms.size == numberOfCoefficients);
	if (x < our xmin || x > our xmax) {
		terms <<= undefined;
		return;
	}
	terms [1] = 1.0;
	if (numberOfCoefficients > 1) {
		// Transform x from domain [xmin, xmax] to domain [-1, 1]

		terms [2] = x = (2.0 * x - xmin - xmax) / (xmax - xmin);

		for (integer i = 3; i <= numberOfCoefficients; i ++)
			terms [i] = 2.0 * x * terms [i - 1] - terms [i - 2];
	}
}

void structChebyshevSeries :: v_getExtrema (double x1, double x2, double *out_xmin, double *out_ymin, double *out_xmax, double *out_ymax) {
	try {
		autoPolynomial p = ChebyshevSeries_to_Polynomial (this);
		FunctionTerms_getExtrema (p.get(), x1, x2, out_xmin, out_ymin, out_xmax, out_ymax);
	} catch (MelderError) {
		Melder_throw (this, U"Extrema cannot be calculated");
	}
}

autoChebyshevSeries ChebyshevSeries_create (double lxmin, double lxmax, integer numberOfPolynomials) {
	try {
		autoChebyshevSeries me = Thing_new (ChebyshevSeries);
		FunctionTerms_init (me.get(), lxmin, lxmax, numberOfPolynomials);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ChebyshevSeries not created.");
	}
}

autoChebyshevSeries ChebyshevSeries_createFromString (double lxmin, double lxmax, conststring32 s) {
	try {
		autoChebyshevSeries me = Thing_new (ChebyshevSeries);
		FunctionTerms_initFromString (me.get(), lxmin, lxmax, s, false);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ChebyshevSeries not created from string.");
	};
}

autoPolynomial ChebyshevSeries_to_Polynomial (ChebyshevSeries me) {
	try {
		double xmin = -1.0, xmax = 1.0;

		autoPolynomial thee = Polynomial_create (xmin, xmax, my numberOfCoefficients - 1);

		thy coefficients [1] = my coefficients [1] /* * p [1] */;
		if (my numberOfCoefficients == 1)
			return thee;

		thy coefficients [2] = my coefficients [2];
		if (my numberOfCoefficients > 2) {
			autoVEC pn = newVECzero (my numberOfCoefficients);
			autoVEC pnm1 = newVECzero (my numberOfCoefficients);
			autoVEC pnm2 = newVECzero (my numberOfCoefficients);

			// Start the recursion: T [2] = x; T [1] = 1;

			pnm1 [2] = 1.0;
			pnm2 [1] = 1.0;
			double a = 2.0, b = 0.0, c = -1.0;
			for (integer n = 2; n <= my numberOfCoefficients - 1; n ++) {
				NUMpolynomial_recurrence (pn.part (1, n + 1), a, b, c, pnm1.get (), pnm2.get());
				if (my coefficients [n + 1] != 0.0) {
					for (integer j = 1; j <= n + 1; j ++)
						thy coefficients [j] += my coefficients [n + 1] * pn [j];
				}
				double *t1 = pnm1.at, *t2 = pnm2.at;  // circular swap
				pnm1.at = pn.at;
				pnm2.at = t1;
				pn.at = t2;
			}
		}
		if (my xmin != xmin || my xmax != xmax)
			thee = Polynomial_scaleX (thee.get(), my xmin, my xmax);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U"; not converted to Polynomial.");
	};
}


void FunctionTerms_RealTier_fit (FunctionTerms me, RealTier thee, INTVEC freeze, double tol, int ic, autoCovariance *c) {
	try {
		integer numberOfData = thy points.size;
		integer numberOfParameters = my numberOfCoefficients;
		integer numberOfFreeParameters = numberOfParameters;
		Melder_require (numberOfData > 1, U"The number of data point should be larger than 1.");

		autoFunctionTerms frozen = Data_copy (me);
		autoVEC terms = newVECzero (my numberOfCoefficients);
		autoVEC p = newVECzero (numberOfParameters);
		autoVEC y_residual = newVECraw (numberOfData);
		autoCovariance ac;
		if (ic)
			ac = Covariance_create (numberOfParameters);

		integer k = 1;
		for (integer j = 1; j <= my numberOfCoefficients; j ++) {
			if (freeze.size > 0 && freeze [j])
				numberOfFreeParameters--;
			else {
				p [k ++] = my coefficients [j];
				frozen -> coefficients [j] = 0.0;
			}
		}
		
		Melder_require (numberOfFreeParameters > 0, U"No free parameters left.");

		autoSVD svd = SVD_create (numberOfData, numberOfFreeParameters);

		double sigma = RealTier_getStandardDeviation_points (thee, my xmin, my xmax);
		
		Melder_require (isdefined (sigma), U"Not enough data points in fit interval.");

		for (integer i = 1; i <= numberOfData; i ++) {
			// Only 'residual variance' must be explained by the model
			// Evaluate only with the frozen parameters

			RealPoint point = thy points.at [i];
			double x = point -> number;
			double y = point -> value;
			double y_frozen = ( numberOfFreeParameters == numberOfParameters ? 0.0 :
				FunctionTerms_evaluate (frozen.get(), x));

			y_residual [i] = (y - y_frozen) / sigma;

			// Data matrix

			FunctionTerms_evaluateTerms (me, x, terms.get ());
			k = 0;
			for (integer j = 1; j <= my numberOfCoefficients; j ++) {
				if (freeze.size == 0 || ! freeze [j]) 
					svd -> u [i] [++ k] = terms [j] / sigma;
			}
		}

		// SVD and evaluation of the singular values

		if (tol > 0.0)
			SVD_setTolerance (svd.get(), tol);

		SVD_compute (svd.get());
		autoVEC result = SVD_solve (svd.get(), y_residual.get());

		// Put fitted values at correct position
		k = 1;
		for (integer j = 1; j <= my numberOfCoefficients; j ++) {
			if (freeze.size == 0 || ! freeze [j])
				my coefficients [j] = result [k ++];
		}
		if (ic)
			svdcvm (ac -> data.get(), svd -> v.get(), numberOfFreeParameters, freeze, svd -> d.get());
		if (c) *c = ac.move();
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": no fit.");
	}
}


autoPolynomial RealTier_to_Polynomial (RealTier me, integer degree, double tol, int ic, autoCovariance *cvm) {
	try {
		autoPolynomial thee = Polynomial_create (my xmin, my xmax, degree);
		autoINTVEC nul;
		FunctionTerms_RealTier_fit (thee.get(), me, nul.get(), tol, ic, cvm);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Polynomial fitted.");
	}
}

autoLegendreSeries RealTier_to_LegendreSeries (RealTier me, integer degree, double tol, int ic, autoCovariance *cvm) {
	try {
		autoLegendreSeries thee = LegendreSeries_create (my xmin, my xmax, degree);
		autoINTVEC nul;
		FunctionTerms_RealTier_fit (thee.get(), me, nul.get(), tol, ic, cvm);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LegendreSeries fitted.");
	}
}

autoChebyshevSeries RealTier_to_ChebyshevSeries (RealTier me, integer degree, double tol, int ic, autoCovariance *cvm) {
	try {
		autoChebyshevSeries thee = ChebyshevSeries_create (my xmin, my xmax, degree);
		autoINTVEC nul;
		FunctionTerms_RealTier_fit (thee.get(), me, nul.get(), tol, ic, cvm);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U":no ChebyshevSeries fitted.");
	};
}

/******* Splines *************************************************/

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
	integer numberOfSplines = points.size + order - 2;
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

	integer index_b = index - order + 1;
	index_b = std::max (index_b, (integer) 1);
	if (x < points [index_b])
		return 0.0;

	integer index_e = index_b + std::min (index, order);
	index_e = std::min (points.size, index_e);
	if (x > points [index_e])
		return 0.0;

	// Calculate M [i](x|1,t) according to eq.2.

	for (integer k = 1; k <= order; k ++) {
		integer k1 = index - order + k, k2 = k1 + 1;
		m [k] = 0.0;
		if (k1 > 0 && k2 <= points.size && x >= points [k1] && x < points [k2])
			m [k] = 1.0 / (points [k2] - points [k1]);
	}

	// Iterate to get M [i](x|k,t)

	for (integer k = 2; k <= order; k ++) {
		for (integer j = 1; j <= order - k + 1; j ++) {
			integer k1 = index - order + j, k2 = k1 + k;
			if (k2 > 1 && k1 < 1)
				k1 = 1;
			else if (k2 > points.size && k1 < points.size)
				k2 = points.size;
			if (k1 > 0 && k2 <= points.size) {
				double p1 = points [k1], p2 = points [k2];
				m [j] = k * ((x - p1) * m [j] + (p2 - x) * m [j + 1]) /
					((k - 1) * (p2 - p1));
			}
		}
	}
	return m [1];
}

static double NUMispline2 (constVEC points, integer order, integer index, double x) {
	Melder_assert (points.size > 2 && order > 0 && index > 0);

	integer index_b = index - order + 1;
	index_b = std::max (index_b, (integer) 1);

	if (x < points [index_b])
		return 0.0;

	integer index_e = index_b + std::min (index, order);
	index_e = std::min (points.size, index_e);

	if (x > points [index_e])
		return 1.0;
	integer j;
	for (j = index_e - 1; j >= index_b; j--)
		if (x > points [j]) break;

	// Equation 5 in Ramsay's article contains some errors!!!
	// 1. the interval selection must be 'j-k <= i <= j' instead of
	//	'j-k+1 <= i <= j'
	// 2. the summation index m starts at 'i+1' instead of 'i'

	double y = 0.0;
	for (integer m = index + 1; m <= j + order; m ++) {
		integer km = m - order, kmp = km + order + 1;
		km = std::max (km, (integer) 1);
		kmp = std::min (kmp, points.size);
		y += (points [kmp] - points [km]) * NUMmspline2 (points, order + 1, m, x);
	}
	return y /= (order + 1);
}

Thing_implement (Spline, FunctionTerms, 0);

double structSpline :: v_evaluate (double /* x */) {
	return 0.0;
}

integer structSpline :: v_getDegree () {
	return degree;
}

integer structSpline :: v_getOrder () {
	return degree + 1;
}

/* Precondition: FunctionTerms part inited + degree */
static void Spline_initKnotsFromString (Spline me, integer degree, conststring32 interiorKnots_string) {

	Melder_require (degree <= Spline_MAXIMUM_DEGREE, U"Degree should be <= ", Spline_MAXIMUM_DEGREE, U".");
	
	autoVEC interiorKnots = VEC_createFromString (interiorKnots_string);

	VECsort_inplace (interiorKnots.get());
	Melder_require (interiorKnots [1] > my xmin && interiorKnots [interiorKnots.size] <= my xmax,
		U"Knots should be inside domain.");

	my degree = degree;
	integer order = Spline_getOrder (me); /* depends on spline type !! */
	integer n = interiorKnots.size + order;
	Melder_require (my numberOfCoefficients == n, U"Number of coefficients should equal ", n, U".");

	my numberOfKnots = interiorKnots.size + 2;
	my knots = newVECzero (my numberOfKnots);
	my knots.part (2, interiorKnots.size + 1) <<= interiorKnots;
	my knots [1] = my xmin;
	my knots [my numberOfKnots] = my xmax;
}

void Spline_init (Spline me, double xmin, double xmax, integer degree, integer numberOfCoefficients, integer numberOfKnots) {

	Melder_require (degree <= Spline_MAXIMUM_DEGREE, U"Degree should be <= ", Spline_MAXIMUM_DEGREE, U".");
	
	FunctionTerms_init (me, xmin, xmax, numberOfCoefficients);
	my knots = newVECzero (numberOfKnots);
	my degree = degree;
	my numberOfKnots = numberOfKnots;
	my knots [1] = xmin;
	my knots [numberOfKnots] = xmax;
}

void Spline_drawKnots (Spline me, Graphics g, double xmin, double xmax, double ymin, double ymax, int garnish) {
	integer order = Spline_getOrder (me);

	if (xmax <= xmin) {
		xmin = my xmin;
		xmax = my xmax;
	}
	if (xmax < my xmin || xmin > my xmax)
		return;

	if (ymax <= ymin) {
		double x1, x2;
		FunctionTerms_getExtrema (me, xmin, xmax, & x1, & ymin, & x2, & ymax);
	}

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
		integer numberOfKnots = ! garnish ? 0 : my numberOfKnots + 2 * (order - 1);
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

		// x = a x + b
		// Constraints:
		// my xmin = a xmin + b;    a = (my xmin - my xmax) / (xmin - xmax);
		// my xmax = a xmax + b;    b = my xmin - a * xmin

		double a = (xmin - xmax) / (my xmin - my xmax);
		double b = xmin - a * my xmin;
		for (integer i = 1; i <= my numberOfKnots; i ++) {
			thy knots [i] = a * my knots [i] + b;
		}
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
	for (integer i = 1; i <= numberOfCoefficients; i ++) {
		if (coefficients [i] != 0.0)
			result += coefficients [i] * NUMmspline2 (knots.get(), degree + 1, i, x);
	}
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
		integer numberOfCoefficients = numberOfInteriorKnots + degree + 1;
		integer numberOfKnots = numberOfCoefficients + degree + 1;
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
		FunctionTerms_initFromString (me.get(), xmin, xmax, coef, true);
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
	for (integer i = 1; i <= numberOfCoefficients; i ++) {
		if (coefficients [i] != 0.0)
			result += coefficients [i] * NUMispline2 (knots.get(), degree, i, x);
	}
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
		integer numberOfCoefficients = numberOfInteriorKnots + degree;
		integer numberOfKnots = numberOfCoefficients + degree;
		Spline_init (me.get(), xmin, xmax, degree, numberOfCoefficients, numberOfKnots);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ISpline not created.");
	}
}

autoISpline ISpline_createFromStrings (double xmin, double xmax, integer degree, conststring32 coef, conststring32 interiorKnots) {
	try {
		if (degree > Spline_MAXIMUM_DEGREE)
			Melder_throw (U"Degree should be <= 20.");
		autoISpline me = Thing_new (ISpline);
		FunctionTerms_initFromString (me.get(), xmin, xmax, coef, true);
		Spline_initKnotsFromString (me.get(), degree, interiorKnots);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ISpline not created from strings.");
	};
}

/*

#define RationalFunction_members Function_members \
	Polynomial num, denum;
#define RationalFunction_methods Function_methods
class_create (RationalFunction, Function)

RationalFunction RationalFunction_create (double xmin, double xmax,
	integer degree_num, integer degree_denum)
{
	RationalFunction me = new (RationalFunction);
	if (! me || ! (my num = Polynomial_create (xmin, xmax, degree_num)) ||
		!  (my denum = Polynomial_create (xmin, xmax, degree_denum))) forget (me);
	return me;
}

RationalFunction RationalFunction_createFromString (I, double xmin, double xmax,
	char *num, char *denum)
{
	RationalFunction me = new (RationalFunction); integer i;

	if (! (my num = Polynomial_createFromString (xmin, xmax, num)) ||
		! (my denum = Polynomial_createFromString (xmin, xmax, denum))) forget (me);
	if (my denum -> v [1] != 1 && my denum -> v [1] != 0)
	{
		double q0 = my denum -> v [1];
		for (i=1; 1 <= my num ->numberOfCoefficients; i ++) my num -> v [i] /= q0;
		for (i=1; 1 <= my denum ->numberOfCoefficients; i ++) my denum -> v [i] /= q0;
	}
	return me;
}

// divide out common roots
RationalFunction RationalFunction_simplify (RationalFunction me)
{
	Roots num = nullptr, denum = nullptr; RationalFunction thee = nullptr;
	if (! (num = Polynomial_to_Roots (my num)) ||
		! (denum = Polynomial_to_Roots (my denum))) goto end;
}

*/

/* end of file Polynomial.cpp */
