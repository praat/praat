#ifndef _Polynomial_h_
#define _Polynomial_h_
/* Polynomial.h
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

#include "FunctionSeries.h"
#include "Graphics.h"

Thing_define (Polynomial, FunctionSeries) {
	// overridden methods:
	public:
		virtual double v_evaluate (double x);
		virtual dcomplex v_evaluate_z (dcomplex z);
		virtual void v_evaluateTerms (double x, VEC terms);
		virtual void v_getExtrema (double x1, double x2, double *xmin, double *ymin, double *xmax, double *ymax);
		//virtual integer v_getDegree ();   David, is het OK dat deze niet overschreven wordt? Ja
};

autoPolynomial Polynomial_create (double xmin, double xmax, integer degree);

autoPolynomial Polynomial_createFromString (double xmin, double xmax, conststring32 s);

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
void Polynomial_initFromProductOfSecondOrderTerms (Polynomial me, constVEC a);
autoPolynomial Polynomial_createFromProductOfSecondOrderTermsString (double xmin, double xmax, conststring32 s);

void Polynomial_initFromRealRoots (Polynomial me, constVEC roots);
autoPolynomial Polynomial_createFromRealRootsString (double xmin, double xmax, conststring32 s);

double Polynomial_getArea (Polynomial me, double xmin, double xmax);

autoPolynomial Polynomial_getDerivative (Polynomial me);

autoPolynomial Polynomial_getPrimitive (Polynomial me, double constant);

void Polynomial_draw (Polynomial me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool garnish);

double Polynomial_evaluate (Polynomial me, double x);

void Polynomial_evaluateWithDerivative (Polynomial me, double x, double *fx, double *dfx);

autoVEC Polynomial_evaluateDerivatives (Polynomial me, double x, long numberOfDerivatives);
/* result[1] is function value at x, result[2..numberOfDerivatives+1] are derivatives at x. */

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

void Polynomials_divide (Polynomial me, Polynomial thee, autoPolynomial *out_q, autoPolynomial *out_r);

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

double Polynomial_findOneSimpleRealRoot_nr (Polynomial me, double xmin, double xmax);
double Polynomial_findOneSimpleRealRoot_ridders (Polynomial me, double xmin, double xmax);
/* Preconditions: there must be exactly one root in the [xmin, xmax] interval;
 * Root will be found by newton-raphson with bisecting
 */

#endif /* _Polynomial_h_ */
