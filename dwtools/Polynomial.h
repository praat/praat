#ifndef _Polynomial_h_
#define _Polynomial_h_
/* Polynomial.h
 *
 * Copyright (C) 1993-2017 David Weenink
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

#define FITTER_PARAMETER_FREE 0
#define FITTER_PARAMETER_FIXED 1

#include "SimpleVector.h"
#include "Function.h"
#include "TableOfReal.h"
#include "Graphics.h"
#include "Minimizers.h"
#include "Spectrum.h"
#include "RealTier.h"
#include "SSCP.h"

#define Spline_MAXIMUM_DEGREE 20

#include "Polynomial_def.h"

void FunctionTerms_init (FunctionTerms me, double xmin, double xmax, integer numberOfCoefficients);

void FunctionTerms_initFromString (FunctionTerms me, double xmin, double xmax, const char32 *s, bool allowTrailingZeros);

autoFunctionTerms FunctionTerms_create (double xmin, double xmax, integer numberOfCoefficients);

void FunctionTerms_setDomain (FunctionTerms me, double xmin, double xmax);

void FunctionTerms_setCoefficient (FunctionTerms me, integer index, double value);

double FunctionTerms_evaluate (FunctionTerms me, double x);

void FunctionTerms_evaluate_z (FunctionTerms me, dcomplex *z, dcomplex *p);

void FunctionTerms_evaluateTerms (FunctionTerms me, double x, double terms[]);

void FunctionTerms_getExtrema (FunctionTerms me, double x1, double x2, double *xmin, double *ymin, double *xmax, double *ymax);

integer FunctionTerms_getDegree (FunctionTerms me);

double FunctionTerms_getMinimum (FunctionTerms me, double x1, double x2);

double FunctionTerms_getXOfMinimum (FunctionTerms me, double x1, double x2);

double FunctionTerms_getMaximum (FunctionTerms me, double x1, double x2);

double FunctionTerms_getXOfMaximum (FunctionTerms me, double x1, double x2);
/*
	Returns minimum and maximum function values (ymin, ymax) in
	interval [x1, x2] and their x-values (xmin, xmax).
	Precondition: [x1, x2] is a (sub)domain
		my xmin <= x1 < x2 <= my xmax
*/

void FunctionTerms_draw (FunctionTerms me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	int extrapolate, int garnish);
/*
	Extrapolate only for functions whose domain is extendable and that can be extrapolated.
	Polynomials can be extrapolated.
	LegendreSeries and ChebyshevSeries cannot be extrapolated.
*/
void FunctionTerms_drawBasisFunction (FunctionTerms me, Graphics g, integer index, double xmin, double xmax,
	double ymin, double ymax, int extrapolate, int garnish);

Thing_define (Polynomial, FunctionTerms) {
	// overridden methods:
	public:
		virtual double v_evaluate (double x);
		virtual dcomplex v_evaluate_z (dcomplex z);
		virtual void v_evaluateTerms (double x, double terms[]);
		virtual void v_getExtrema (double x1, double x2, double *xmin, double *ymin, double *xmax, double *ymax);
		//virtual integer v_getDegree ();   David, is het OK dat deze niet overschreven wordt? Ja
};

autoPolynomial Polynomial_create (double xmin, double xmax, integer degree);

autoPolynomial Polynomial_createFromString (double xmin, double xmax, const char32 *s);

void Polynomial_scaleCoefficients_monic (Polynomial me);
/* Make coefficent of leading term 1.0 */

autoPolynomial Polynomial_scaleX (Polynomial me, double xmin, double xmax);
/* x' = (x-location) / scale */

dcomplex Polynomial_evaluate_z (Polynomial me, dcomplex z);
/* Evaluate at complex z = x + iy */


/* Product (i=1; numberOfTerms; (1 + a*x + x^2)
 * Precondition : my numberOfCoeffcients >= 3+2*numberOfOmegas
 * 	Polynomial is uses as a "buffer". We define it one and reuse it 
 */
void Polynomial_initFromProductOfSecondOrderTerms (Polynomial me, double *a, integer numberOfTerms);
autoPolynomial Polynomial_createFromProductOfSecondOrderTermsString (double xmin, double xmax, const char32 *s);

void Polynomial_initFromRealRoots (Polynomial me, double *roots, integer numberOfRoots);
autoPolynomial Polynomial_createFromRealRootsString (double xmin, double xmax, const char32 *s);

double Polynomial_getArea (Polynomial me, double xmin, double xmax);

autoPolynomial Polynomial_getDerivative (Polynomial me);

autoPolynomial Polynomial_getPrimitive (Polynomial me, double constant);

void Polynomial_draw (Polynomial me, Graphics g, double xmin, double xmax, double ymin, double ymax, int garnish);

double Polynomial_evaluate (Polynomial me, double x);

void Polynomial_evaluateWithDerivative (Polynomial me, double x, double *fx, double *dfx);

void Polynomial_evaluateDerivatives (Polynomial me, double x, double *derivatives /*[0.. numberOfDerivatives]*/, integer numberOfDerivatives);
/* derivatives[0] = Polynomial_evaluate (me, x); */

void Polynomial_evaluateTerms (Polynomial me, double x, double terms[]);

autoPolynomial Polynomials_multiply (Polynomial me, Polynomial thee);

void Polynomial_multiply_firstOrderFactor (Polynomial me, double factor);
/* P(x) * (x-factor)
 * Postcondition: my numberOfCoefficients = old_numberOfCoefficients + 1 
 */

void Polynomial_multiply_secondOrderFactor (Polynomial me, double factor);
/* P(x) * (x^2 - a)
 * Postcondition: my numberOfCoefficients = old_numberOfCoefficients + 2
 */

void Polynomials_divide (Polynomial me, Polynomial thee, autoPolynomial *q, autoPolynomial *r);

void Polynomial_divide_firstOrderFactor (Polynomial me, double factor, double *p_remainder); // P(x)/(x-a)
/* Functions: calculate coefficients of new polynomial P(x)/(x-a)
 * if p_remainder != nullptr it will contain 
 *		remainder after dividing by monomial factor x-a.
 * 		`undefined` if my numberOfCoefficients == 1 (error condition)
 * Postcondition: my numberOfCoefficients reduced by 1 
*/

void Polynomial_divide_secondOrderFactor (Polynomial me, double factor);
/* P(x) / (x^2 - a)
 * Postcondition: my numberOfCoefficients = old_numberOfCoefficients - 2
 */

Thing_define (LegendreSeries, FunctionTerms) {
	// overridden methods:
	public:
		virtual double v_evaluate (double x);
		virtual void v_evaluateTerms (double x, double terms[]);
		virtual void v_getExtrema (double x1, double x2, double *xmin, double *ymin, double *xmax, double *ymax);
};

autoLegendreSeries LegendreSeries_create (double xmin, double xmax, integer numberOfPolynomials);

autoLegendreSeries LegendreSeries_createFromString (double xmin, double xmax, const char32 *s);

autoLegendreSeries LegendreSeries_getDerivative (LegendreSeries me);

autoPolynomial LegendreSeries_to_Polynomial (LegendreSeries me);

Thing_define (Roots, ComplexVector) {
};

autoRoots Roots_create (integer numberOfRoots);

void Roots_fixIntoUnitCircle (Roots me);

void Roots_sort (Roots me);
/* Sort to size of real part a+bi, a-bi*/

dcomplex Roots_evaluate_z (Roots me, dcomplex z);

autoRoots Polynomial_to_Roots_ev (Polynomial me);

integer Roots_getNumberOfRoots (Roots me);

void Roots_draw (Roots me, Graphics g, double rmin, double rmax, double imin, double imax,
	const char32 *symbol, int fontSize, int garnish);

dcomplex Roots_getRoot (Roots me, integer index);

void Roots_setRoot (Roots me, integer index, double re, double im);

autoSpectrum Roots_to_Spectrum (Roots me, double nyquistFrequency, integer numberOfFrequencies, double radius);

autoRoots Polynomial_to_Roots (Polynomial me);
/* Find roots of polynomial and polish them */

double Polynomial_findOneSimpleRealRoot_nr (Polynomial me, double xmin, double xmax);
double Polynomial_findOneSimpleRealRoot_ridders (Polynomial me, double xmin, double xmax);
/* Preconditions: there must be exactly one root in the [xmin, xmax] interval;
 * Root will be found by newton-raphson with bisecting
 */

void Roots_Polynomial_polish (Roots me, Polynomial thee);

autoPolynomial Roots_to_Polynomial (Roots me, bool rootsAreReal);

autoPolynomial TableOfReal_to_Polynomial (TableOfReal me, integer degree, integer xcol, integer ycol, integer scol);

autoLegendreSeries TableOfReal_to_LegendreSeries (TableOfReal me, integer numberOfPolynomials, integer xcol, integer ycol, integer scol);

autoSpectrum Polynomial_to_Spectrum (Polynomial me, double nyquistFrequency, integer numberOfFrequencies, double radius);

/*
	A ChebyshevSeries p(x) on a domain [xmin,xmax] is defined as the
	following linear combination of Chebyshev polynomials T[k](x') of
	degree k-1 and domain [-1, 1]:
		p(x) = sum (k=1..numberOfCoefficients, c[k]*T[k](x')) - c[1] / 2, where
		x' = (2 * x - xmin - xmax) / (xmax - xmin)
	This is equivalent to:
		p(x) = c[1] /2 + sum (k=2..numberOfCoefficients, c[k]*T[k](x'))
*/
Thing_define (ChebyshevSeries, FunctionTerms) {
	// overridden methods:
	public:
		virtual double v_evaluate (double x);
		virtual void v_evaluateTerms (double x, double terms[]);
		virtual void v_getExtrema (double x1, double x2, double *xmin, double *ymin, double *xmax, double *ymax);
};

autoChebyshevSeries ChebyshevSeries_create (double xmin, double xmax, integer numberOfPolynomials);

autoChebyshevSeries ChebyshevSeries_createFromString (double xmin, double xmax, const char32 *s);

autoPolynomial ChebyshevSeries_to_Polynomial (ChebyshevSeries me);

void Spline_init (Spline me, double xmin, double xmax, integer degree, integer numberOfCoefficients, integer numberOfKnots);

integer Spline_getOrder (Spline me);

void Spline_drawKnots (Spline me, Graphics g, double xmin, double xmax, double ymin, double ymax, int garnish);

autoSpline Spline_scaleX (Spline me, double xmin, double xmax);
/* scale domain and knots to new domain */

Thing_define (MSpline, Spline) {
	// overridden methods:
	public:
		virtual double v_evaluate (double x);
		virtual void v_evaluateTerms (double x, double terms[]);
};

autoMSpline MSpline_create (double xmin, double xmax, integer degree, integer numberOfInteriorKnots);

autoMSpline MSpline_createFromStrings (double xmin, double xmax, integer degree, const char32 *coef, const char32 *interiorKnots);

Thing_define (ISpline, Spline) {
	// overridden methods:
	public:
		virtual double v_evaluate (double x);
		virtual void v_evaluateTerms (double x, double terms[]);
		virtual integer v_getOrder ();
};

autoISpline ISpline_create (double xmin, double xmax, integer degree, integer numberOfInteriorKnots);

autoISpline ISpline_createFromStrings (double xmin, double xmax, integer degree, const char32 *coef, const char32 *interiorKnots);

/****************** fit **********************************************/

void FunctionTerms_RealTier_fit (FunctionTerms me, RealTier thee, int freezeCoefficients[], double tol, int ic, autoCovariance *c);

autoPolynomial RealTier_to_Polynomial (RealTier me, integer degree, double tol, int ic, autoCovariance *cvm);

autoLegendreSeries RealTier_to_LegendreSeries (RealTier me, integer degree, double tol, int ic, autoCovariance *cvm);

autoChebyshevSeries RealTier_to_ChebyshevSeries (RealTier me, integer degree, double tol, int ic, autoCovariance *cvm);

#endif /* _Polynomial_h_ */
