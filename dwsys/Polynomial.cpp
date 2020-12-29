/* Polynomial.cpp
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
 djmw 20030619 Added SVD_compute before SVD_solve
 djmw 20060510 Polynomial_to_Roots: changed behaviour. All roots found are now saved.
 	In previous version a nullptr pointer was returned. New error messages.
 djmw 20071012 Added: oo_CAN_WRITE_AS_ENCODING.h
 djmw 20071201 Melder_warning<n>
 djmw 20080122 float -> double
  djmw 20110304 Thing_new
*/

#include "Roots.h"
#include "Polynomial.h"
#include "SVD.h"
#include "NUMmachar.h"

/* Evaluate polynomial and derivative jointly
	c [1..n] -> degree n-1 !!
*/
void Polynomial_evaluateWithDerivative (Polynomial me, double x, double *out_f, double *out_df) {
	longdouble p = my coefficients [my numberOfCoefficients], dp = 0.0;

	for (integer i = my numberOfCoefficients - 1; i > 0; i --) {
		dp = dp * x + p;
		p =  p * x + my coefficients [i];
	}
	if (out_f)
		*out_f = (double) p;
	if (out_df)
		*out_df = (double) dp;
}

autoVEC Polynomial_evaluateDerivatives (Polynomial me, double x, long numberOfDerivatives) {
	/* 
		Evaluate polynomial c [1]+c [2]*x+...degree*x^degree in derivative [1] and the numberOfDerivatives 
		in derivatives in derivatives [2..numberOfDerivatives+1].
	*/
	const integer degree = my numberOfCoefficients - 1;
	autoVEC derivatives = zero_VEC (numberOfDerivatives + 1);
	numberOfDerivatives = numberOfDerivatives > degree ? degree : numberOfDerivatives;
	
	derivatives [1] = my coefficients [my numberOfCoefficients];

	for (integer i = degree - 1; i >= 0; i--) {
		const integer n = numberOfDerivatives < degree - i ? numberOfDerivatives : degree - i;
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
	r  <<=  u;
	q  <<=  0.0;
	for (integer k = u.size - v.size + 1; k > 0; k --) { /* D1 */
		q [k] = r [v.size + k - 1] / v [v.size]; /* D2 with u -> r*/
		for (integer j = v.size + k - 1; j >= k; j --)
			r [j] -= q [k] * v [j - k + 1];
	}
}


Thing_implement (Polynomial, FunctionSeries, 1);


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
double structPolynomial :: v_evaluate (double x) {
	longdouble p = coefficients [numberOfCoefficients];
	for (integer i = numberOfCoefficients - 1; i > 0; i --)
		p = p * x + coefficients [i];
	return (double) p;
}

dcomplex structPolynomial :: v_evaluate_z (dcomplex z) {
	longdouble x = z.real(), y = z.imag();

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
		const integer degree = numberOfCoefficients - 1;

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
			const double x = r -> roots [i].real();
			if (x > x1 && x < x2) {
				const double y = v_evaluate (x);
				if (y > ymx) {
					ymx = y;
					xmx = x;
				} else if (y < ymn) {
					ymn = y;
					xmn = x;
				}
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
	} catch (MelderError) {
		structFunctionSeries :: v_getExtrema (x1, x2, out_xmin, out_ymin, out_xmax, out_ymax);
		Melder_clearError ();
	}
}

autoPolynomial Polynomial_create (double xmin, double xmax, integer degree) {
	try {
		autoPolynomial me = Thing_new (Polynomial);
		FunctionSeries_init (me.get(), xmin, xmax, degree + 1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Polynomial not created.");
	}
}

autoPolynomial Polynomial_createFromString (double lxmin, double lxmax, conststring32 s) {
	try {
		autoPolynomial me = Thing_new (Polynomial);
		FunctionSeries_initFromString (me.get(), lxmin, lxmax, s, false);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Polynomial not created from string.");
	}
}

void Polynomial_scaleCoefficients_monic (Polynomial me) {
	const double cn = my coefficients [my numberOfCoefficients];
	if (cn == 1 || my numberOfCoefficients <= 1)
		return;
	my coefficients.get()  /=  cn;
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
		/*
			x = a x + b
			Constraints:
			my xmin = a xmin + b;    a = (my xmin - my xmax) / (xmin - xmax);
			my xmax = a xmax + b;    b = my xmin - a * xmin
		*/
		const double a = (my xmin - my xmax) / (xmin - xmax);
		const double b = my xmin - a * xmin;
		thy coefficients [2] = my coefficients [2] * a;
		thy coefficients [1] += my coefficients [2] * b;
		if (my numberOfCoefficients == 2)
			return thee;
		autoVEC pn = zero_VEC (my numberOfCoefficients);
		autoVEC pnm1 = zero_VEC (my numberOfCoefficients);
		autoVEC pnm2 = zero_VEC (my numberOfCoefficients);

		// Start the recursion: P [2] = a x + b; P [1] = 1;

		pnm1 [2] = a;
		pnm1 [1] = b;
		pnm2 [1] = 1.0;
		for (integer n = 2; n <= my numberOfCoefficients - 1; n ++) {
			NUMpolynomial_recurrence (pn.part (1, n + 1), a, b, 0.0, pnm1.get(), pnm2.get());
			if (my coefficients [n + 1] != 0.0)
				for (integer j = 1; j <= n + 1; j ++)
					thy coefficients [j] += my coefficients [n + 1] * pn [j];
			autoVEC pn_old = std::move (pn);
			pn = std::move (pnm2);
			pnm2 = std::move (pnm1);
			pnm1 = std::move (pn_old);
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
		my extendCapacity (roots.size + 1);
		integer n = 1;
		my coefficients [1] = - roots [1];
		my coefficients [2] = 1.0;
		for (integer iroot = 2; iroot <= roots.size; iroot ++) {
			my coefficients [n + 2] = my coefficients [n + 1];
			for (integer j = n; j >= 1; j --)
				my coefficients [j + 1] = my coefficients [j] - my coefficients [j + 1] * roots [iroot];
			my coefficients [1] *= -roots [iroot];
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
		autoVEC roots = newVECfromString (s);
		FunctionSeries_init (me.get(), xmin, xmax, roots.size + 1);
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
	my extendCapacity (2 * a.size + 1);
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
		autoVEC a = newVECfromString (s);
		FunctionSeries_init (me.get(), xmin, xmax, 2 * a.size + 1);
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
	double area = FunctionSeries_evaluate (p.get(), xmax) - FunctionSeries_evaluate (p.get(), xmin);
	return area;
}

/* P(x) * (x-a)
 * Postcondition: my numberOfCoefficients = old_numberOfCoefficients + 1 
 */
void Polynomial_multiply_firstOrderFactor (Polynomial me, double factor) { 
	const integer n = my numberOfCoefficients;
	my extendCapacity (n + 1);
	
	my coefficients [n + 1] = my coefficients [n];
	for (integer j = n; j >= 2; j --)
		my coefficients [j] = my coefficients [j - 1] - my coefficients [j] * factor;
	my coefficients [1] *= -factor;
	my numberOfCoefficients += 1;
}

/*
	P(x) * (x^2 - a)
	Postcondition: my numberOfCoefficients = old_numberOfCoefficients + 2
 */
void Polynomial_multiply_secondOrderFactor (Polynomial me, double factor) {
	const integer n = my numberOfCoefficients;
	my extendCapacity (n + 2);
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
		const integer n1 = my numberOfCoefficients, n2 = thy numberOfCoefficients;
		Melder_require (my xmax > thy xmin && my xmin < thy xmax,
			U"Domains should overlap.");
		const double xmin = my xmin > thy xmin ? my xmin : thy xmin;
		const double xmax = my xmax < thy xmax ? my xmax : thy xmax;
		autoPolynomial him = Polynomial_create (xmin, xmax, n1 + n2 - 2);
		for (integer i = 1; i <= n1; i ++)
			for (integer j = 1; j <= n2; j ++)
				his coefficients [i + j - 1] += my coefficients [i] * thy coefficients [j];
		return him;
	} catch (MelderError) {
		Melder_throw (U"Polynomials not multiplied.");
	}
}

void Polynomials_divide (Polynomial me, Polynomial thee, autoPolynomial *out_q, autoPolynomial *out_r) {
	if (! out_q  && ! out_r)
		return;
	autoVEC qc = zero_VEC (my numberOfCoefficients);
	autoVEC rc = zero_VEC (my numberOfCoefficients);
	autoPolynomial aq, ar;
	VECpolynomial_divide (my coefficients.get (), thy coefficients.get (), qc.get (), rc.get ());
	if (out_q) {
		const integer degree = std::max (my numberOfCoefficients - thy numberOfCoefficients, 0_integer);
		aq = Polynomial_create (my xmin, my xmax, degree);
		if (degree >= 0)
			aq -> coefficients.get () <<= qc.part (1, degree + 1);
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
		ar -> coefficients.all()  <<=  rc.part (1, degree + 1);
		*out_r = ar.move();
	}
}

static double dpoly_nr (double x, double *df, void *closure) {
	double f;
	Polynomial_evaluateWithDerivative ((Polynomial) closure, x, & f, df);
	return f;
}

double Polynomial_findOneSimpleRealRoot_nr (Polynomial me, double xmin, double xmax) {	
	double root = NUMnrbis (dpoly_nr, xmin, xmax, me);
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
			const double tmp = my coefficients [j];
			my coefficients [j] = remainder;
			remainder = tmp + remainder * factor;
		}
		my numberOfCoefficients --;
	} else {
		my coefficients [1] = 0.0;
	}
	if (out_remainder)
		*out_remainder = remainder;
}

void Polynomial_divide_secondOrderFactor (Polynomial me, double factor) {
	if (my numberOfCoefficients > 2) {
		integer n = my numberOfCoefficients;
		/*
			c [1]+c [2]*x...c [n+1]*x^n / (x^2 - a) = r [1]+r [2]*x+...r [n-1]x^(n-2) + possible remainder a [1]+a [2]*x)
			r [j] = c [j+2]+factor*r [j+2]
		*/
		double cjp2 = my coefficients [n];
		double cjp1 = my coefficients [n - 1];
		my coefficients [n] = my coefficients [n - 1] = 0.0;
		for (integer j = n - 2; j > 0; j --) {
			const double cj = my coefficients [j];
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
	Melder_require (index >= 1 && index <= my numberOfRoots,
		U"Index should be in interval [1, ", my numberOfRoots, U"].");
	my roots [index]. real (re);
	my roots [index]. imag (im);
}

dcomplex Roots_evaluate_z (Roots me, dcomplex z) {
	dcomplex result = {1, 0};
	for (integer i = 1; i <= my numberOfRoots; i ++) {
		dcomplex t = dcomplex_sub (z, my roots [i]);
		result = dcomplex_mul (result, t);
	}
	return result;
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
