#ifndef _Polynomial_h_
#define _Polynomial_h_
/* Polynomial.h
 *
 * Copyright (C) 1993-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
oo_CLASS_CREATE (FunctionTerms, Function);

void FunctionTerms_init (I, double xmin, double xmax, long numberOfCoefficients);

void FunctionTerms_initFromString (I, double xmin, double xmax, const wchar_t *s, int allowTrailingZeros);

FunctionTerms FunctionTerms_create (double xmin, double xmax, long numberOfCoefficients);

void FunctionTerms_setDomain (I, double xmin, double xmax);

void FunctionTerms_setCoefficient (I, long index, double value);

double FunctionTerms_evaluate (I, double x);

void FunctionTerms_evaluate_z (I, dcomplex *z, dcomplex *p);

void FunctionTerms_evaluateTerms (I, double x, double terms[]);

void FunctionTerms_getExtrema (I, double x1, double x2, double *xmin, double *ymin,
	double *xmax, double *ymax);

long FunctionTerms_getDegree (I);

double FunctionTerms_getMinimum (I, double x1, double x2);

double FunctionTerms_getXOfMinimum (I, double x1, double x2);

double FunctionTerms_getMaximum (I, double x1, double x2);

double FunctionTerms_getXOfMaximum (I, double x1, double x2);
/*
	Returns minimum and maximum function values (ymin, ymax) in
	interval [x1, x2] and their x-values (xmin, xmax).
	Precondition: [x1, x2] is a (sub)domain
		my xmin <= x1 < x2 <= my xmax
*/

void FunctionTerms_draw (I, Graphics g, double xmin, double xmax, double ymin, double ymax,
	int extrapolate, int garnish);
/*
	Extrapolate only for functions whose domain is extendable and that can be extrapolated.
	Polynomials can be extrapolated.
	LegendreSeries and ChebyshevSeries cannot be extrapolated.
*/
void FunctionTerms_drawBasisFunction (I, Graphics g, long index, double xmin, double xmax,
	double ymin, double ymax, int extrapolate, int garnish);

Thing_define (Polynomial, FunctionTerms) {
	// overridden methods:
	public:
		virtual double v_evaluate (double x);
		virtual void v_evaluate_z (dcomplex *z, dcomplex *p);
		virtual void v_evaluateTerms (double x, double terms[]);
		virtual void v_getExtrema (double x1, double x2, double *xmin, double *ymin, double *xmax, double *ymax);
		//virtual long v_getDegree ();   David, is het OK dat deze niet overschreven wordt? Ja
};

Polynomial Polynomial_create (double xmin, double xmax, long degree);

Polynomial Polynomial_createFromString (double xmin, double xmax, const wchar_t *s);

void Polynomial_scaleCoefficients_monic (Polynomial me);
/* Make coefficent of leading term 1.0 */

Polynomial Polynomial_scaleX (Polynomial me, double xmin, double xmax);
/* x' = (x-location) / scale */

void Polynomial_evaluate_z (Polynomial me, dcomplex *z, dcomplex *p);
/* Evaluate at complex z = x + iy */


double Polynomial_getArea (Polynomial me, double xmin, double xmax);

Polynomial Polynomial_getDerivative (Polynomial me);

Polynomial Polynomial_getPrimitive (Polynomial me);

void Polynomial_draw (I, Graphics g, double xmin, double xmax, double ymin, double ymax, int garnish);

double Polynomial_evaluate (I, double x);

void Polynomial_evaluateTerms (I, double x, double terms[]);

Polynomial Polynomials_multiply (Polynomial me, Polynomial thee);

void Polynomials_divide (Polynomial me, Polynomial thee, Polynomial *q, Polynomial *r);

Thing_define (LegendreSeries, FunctionTerms) {
	// overridden methods:
	public:
		virtual double v_evaluate (double x);
		virtual void v_evaluateTerms (double x, double terms[]);
		virtual void v_getExtrema (double x1, double x2, double *xmin, double *ymin, double *xmax, double *ymax);
};

LegendreSeries LegendreSeries_create (double xmin, double xmax, long numberOfPolynomials);

LegendreSeries LegendreSeries_createFromString (double xmin, double xmax, const wchar_t *s);

LegendreSeries LegendreSeries_getDerivative (LegendreSeries me);

Polynomial LegendreSeries_to_Polynomial (LegendreSeries me);

Thing_define (Roots, ComplexVector) {
};

Roots Roots_create (long numberOfRoots);

void Roots_fixIntoUnitCircle (Roots me);

void Roots_sort (Roots me);
/* Sort to size of real part a+bi, a-bi*/

dcomplex Roots_evaluate_z (Roots me, dcomplex z);

Roots Polynomial_to_Roots_ev (Polynomial me);

long Roots_getNumberOfRoots (Roots me);

void Roots_draw (Roots me, Graphics g, double rmin, double rmax, double imin, double imax,
	const wchar_t *symbol, int fontSize, int garnish);

dcomplex Roots_getRoot (Roots me, long index);
void Roots_setRoot (Roots me, long index, double re, double im);

Spectrum Roots_to_Spectrum (Roots me, double nyquistFrequency,
	long numberOfFrequencies, double radius);

Roots Polynomial_to_Roots (Polynomial me);
/* Find roots of polynomial and polish them */

void Roots_and_Polynomial_polish (Roots me, Polynomial thee);

Polynomial Roots_to_Polynomial (Roots me);

Polynomial TableOfReal_to_Polynomial (I, long degree, long xcol,
	long ycol, long scol);

LegendreSeries TableOfReal_to_LegendreSeries (I, long numberOfPolynomials,
	long xcol, long ycol, long scol);

Spectrum Polynomial_to_Spectrum (Polynomial me, double nyquistFrequency,
	long numberOfFrequencies, double radius);

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

ChebyshevSeries ChebyshevSeries_create (double xmin, double xmax, long numberOfPolynomials);

ChebyshevSeries ChebyshevSeries_createFromString (double xmin, double xmax, const wchar_t *s);

Polynomial ChebyshevSeries_to_Polynomial (ChebyshevSeries me);

oo_CLASS_CREATE (Spline, FunctionTerms);

void Spline_init (I, double xmin, double xmax, long degree, long numberOfCoefficients, long numberOfKnots);

long Spline_getOrder (I);

void Spline_drawKnots (I, Graphics g, double xmin, double xmax, double ymin, double ymax, int garnish);

Spline Spline_scaleX (I, double xmin, double xmax);
/* scale domain and knots to new domain */

Thing_define (MSpline, Spline) {
	// overridden methods:
	public:
		virtual double v_evaluate (double x);
		virtual void v_evaluateTerms (double x, double terms[]);
};

MSpline MSpline_create (double xmin, double xmax, long degree, long numberOfInteriorKnots);

MSpline MSpline_createFromStrings (double xmin, double xmax, long degree, const wchar_t *coef, const wchar_t *interiorKnots);

Thing_define (ISpline, Spline) {
	// overridden methods:
	public:
		virtual double v_evaluate (double x);
		virtual void v_evaluateTerms (double x, double terms[]);
		virtual long v_getOrder ();
};

ISpline ISpline_create (double xmin, double xmax, long degree, long numberOfInteriorKnots);
ISpline ISpline_createFromStrings (double xmin, double xmax, long degree, const wchar_t *coef, const wchar_t *interiorKnots);

/****************** fit **********************************************/

void FunctionTerms_and_RealTier_fit (I, thou, int *freezeCoefficients, double tol, int ic, Covariance *c);

Polynomial RealTier_to_Polynomial (I, long degree, double tol, int ic, Covariance *cvm);

LegendreSeries RealTier_to_LegendreSeries (I, long degree, double tol, int ic, Covariance *cvm);

ChebyshevSeries RealTier_to_ChebyshevSeries (I, long degree, double tol, int ic, Covariance *cvm);

#endif /* _Polynomial_h_ */
