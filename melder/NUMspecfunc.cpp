/* NUMspecfunc.cpp
 *
 * Copyright (C) 1992-2008,2011,2012,2014,2015,2017,2018 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * pb 2002/03/07 GPL
 * pb 2003/06/19 ridders3 replaced with ridders
 * pb 2003/07/09 gsl
 * pb 2003/08/27 NUMfisherQ: underflow and iteration excess should not return undefined
 * pb 2006/08/02 NUMinvSigmoid
 * pb 2008/01/19 double
 * pb 2008/09/21 NUMshift
 * pb 2008/09/22 NUMscale
 * pb 2011/03/29 C++
 */

#include "melder.h"
#include "../dwsys/NUM2.h"   // NUMridders

#include "../external/gsl/gsl_errno.h"
#include "../external/gsl/gsl_sf_bessel.h"
#include "../external/gsl/gsl_sf_gamma.h"
#include "../external/gsl/gsl_sf_erf.h"

double NUMlnGamma (double x) {
	gsl_sf_result result;
	int status = gsl_sf_lngamma_e (x, & result);
	return ( status == GSL_SUCCESS ? result. val : undefined );
}

double NUMbeta (double z, double w) {
	if (z <= 0.0 || w <= 0.0) return undefined;
	return exp (NUMlnGamma (z) + NUMlnGamma (w) - NUMlnGamma (z + w));
}

double NUMincompleteBeta (double a, double b, double x) {
	gsl_sf_result result;
	int status = gsl_sf_beta_inc_e (a, b, x, & result);
	if (status != GSL_SUCCESS && status != GSL_EUNDRFLW && status != GSL_EMAXITER) {
		Melder_fatal (U"NUMincompleteBeta status ", status);
		return undefined;
	}
	return result. val;
}

double NUMbinomialP (double p, double k, double n) {
	double binomialQ;
	if (p < 0.0 || p > 1.0 || n <= 0.0 || k < 0.0 || k > n) return undefined;
	if (k == n) return 1.0;
	binomialQ = NUMincompleteBeta (k + 1, n - k, p);
	if (isundef (binomialQ)) return undefined;
	return 1.0 - binomialQ;
}

double NUMbinomialQ (double p, double k, double n) {
	if (p < 0.0 || p > 1.0 || n <= 0.0 || k < 0.0 || k > n) return undefined;
	if (k == 0.0) return 1.0;
	return NUMincompleteBeta (k, n - k + 1, p);
}

struct binomial { double p, k, n; };

static double binomialP (double p, void *binomial_void) {
	struct binomial *binomial = (struct binomial *) binomial_void;
	return NUMbinomialP (p, binomial -> k, binomial -> n) - binomial -> p;
}

static double binomialQ (double p, void *binomial_void) {
	struct binomial *binomial = (struct binomial *) binomial_void;
	return NUMbinomialQ (p, binomial -> k, binomial -> n) - binomial -> p;
}

double NUMinvBinomialP (double p, double k, double n) {
	static struct binomial binomial;
	if (p < 0 || p > 1 || n <= 0 || k < 0 || k > n) return undefined;
	if (k == n) return 1.0;
	binomial. p = p;
	binomial. k = k;
	binomial. n = n;
	return NUMridders (binomialP, 0.0, 1.0, & binomial);
}

double NUMinvBinomialQ (double p, double k, double n) {
	static struct binomial binomial;
	if (p < 0 || p > 1 || n <= 0 || k < 0 || k > n) return undefined;
	if (k == 0) return 0.0;
	binomial. p = p;
	binomial. k = k;
	binomial. n = n;
	return NUMridders (binomialQ, 0.0, 1.0, & binomial);
}

/* Modified Bessel function I0. Abramowicz & Stegun, p. 378.*/
double NUMbessel_i0_f (double x) {
	if (x < 0.0) return NUMbessel_i0_f (- x);
	if (x < 3.75) {
		/* Formula 9.8.1. Accuracy 1.6e-7. */
		double t = x / 3.75;
		t *= t;
		return 1.0 + t * (3.5156229 + t * (3.0899424 + t * (1.2067492
			+ t * (0.2659732 + t * (0.0360768 + t * 0.0045813)))));
	}
	/*
		otherwise: x >= 3.75
	*/
	/* Formula 9.8.2. Accuracy of the polynomial factor 1.9e-7. */
	double t = 3.75 / x;   /* <= 1.0 */
	return exp (x) / sqrt (x) * (0.39894228 + t * (0.01328592
		+ t * (0.00225319 + t * (-0.00157565 + t * (0.00916281
		+ t * (-0.02057706 + t * (0.02635537 + t * (-0.01647633
		+ t * 0.00392377))))))));
}

/* Modified Bessel function I1. Abramowicz & Stegun, p. 378. */
double NUMbessel_i1_f (double x) {
	if (x < 0.0) return - NUMbessel_i1_f (- x);
	if (x < 3.75) {
		/* Formula 9.8.3. Accuracy of the polynomial factor 8e-9. */
		double t = x / 3.75;
		t *= t;
		return x * (0.5 + t * (0.87890594 + t * (0.51498869 + t * (0.15084934
			+ t * (0.02658733 + t * (0.00301532 + t * 0.00032411))))));
	}
	/*
		otherwise: x >= 3.75
	*/
	/* Formula 9.8.4. Accuracy of the polynomial factor 2.2e-7. */
	double t = 3.75 / x;   /* <= 1.0 */
	return exp (x) / sqrt (x) * (0.39894228 + t * (-0.03988024
		+ t * (-0.00362018 + t * (0.00163801 + t * (-0.01031555
		+ t * (0.02282967 + t * (-0.02895312 + t * (0.01787654
		+ t * (-0.00420059)))))))));
}

double NUMbesselI (integer n, double x) {
	gsl_sf_result result;
	int status = gsl_sf_bessel_In_e (n, x, & result);
	return ( status == GSL_SUCCESS ? result. val : undefined );
}

/* Modified Bessel function K0. Abramowicz & Stegun, p. 379. */
double NUMbessel_k0_f (double x) {
	if (x <= 0.0) return undefined;
	if (x <= 2.0) {
		/* Formula 9.8.5. Accuracy 1e-8. */
		double x2 = 0.5 * x, t = x2 * x2;
		return - log (x2) * NUMbessel_i0_f (x) + (-0.57721566 + t * (0.42278420
			+ t * (0.23069756 + t * (0.03488590 + t * (0.00262698
			+ t * (0.00010750 + t * 0.00000740))))));
	}
	/*
		otherwise: 2 < x < positive infinity
	*/
	/* Formula 9.8.6. Accuracy of the polynomial factor 1.9e-7. */
	double t = 2.0 / x;   /* < 1.0 */
	return exp (- x) / sqrt (x) * (1.25331414 + t * (-0.07832358
		+ t * (0.02189568 + t * (-0.01062446 + t * (0.00587872
		+ t * (-0.00251540 + t * 0.00053208))))));
}

/* Modified Bessel function K1. Abramowicz & Stegun, p. 379. */
double NUMbessel_k1_f (double x) {
	if (x <= 0.0) return undefined;
	if (x <= 2.0) {
		/* Formula 9.8.7. Accuracy  of the polynomial factor 8e-9. */
		double x2 = 0.5 * x, t = x2 * x2;
		return log (x2) * NUMbessel_i1_f (x) + (1.0 / x) * (1.0 + t * (0.15443144
			+ t * (-0.67278579 + t * (-0.18156897 + t * (-0.01919402
			+ t * (-0.00110404 + t * (-0.00004686)))))));
	}
	/*
		otherwise: 2 < x < positive infinity
	*/
	/* Formula 9.8.8. Accuracy of the polynomial factor 2.2e-7. */
	double t = 2.0 / x;   /* < 1.0 */
	return exp (- x) / sqrt (x) * (1.25331414 + t * (0.23498619
			 + t * (-0.03655620 + t * (0.01504268 + t * (-0.00780353
			 + t * (0.00325614 + t * (-0.00068245)))))));
}

double NUMbesselK_f (integer n, double x) {
	double besselK = undefined;
	Melder_assert (n >= 0 && x > 0);
	double besselK_min2 = NUMbessel_k0_f (x);
	if (n == 0) return besselK_min2;
	double besselK_min1 = NUMbessel_k1_f (x);
	if (n == 1) return besselK_min1;
	Melder_assert (n >= 2);
	double twoByX = 2.0 / x;
	/*
		Recursion formula.
	*/
	for (integer i = 1; i < n; i ++) {
		besselK = besselK_min2 + twoByX * i * besselK_min1;
		besselK_min2 = besselK_min1;
		besselK_min1 = besselK;
	}
	Melder_assert (isdefined (besselK));
	return besselK;
}

double NUMbesselK (integer n, double x) {
	gsl_sf_result result;
	int status = gsl_sf_bessel_Kn_e ((int) n, x, & result);
	return ( status == GSL_SUCCESS ? result. val : undefined );
}

double NUMsigmoid (double x)
	{ return x > 0.0 ? 1.0 / (1.0 + exp (- x)) : 1.0 - 1.0 / (1.0 + exp (x)); }

double NUMinvSigmoid (double x)
	{ return x <= 0.0 || x >= 1.0 ? undefined : log (x / (1.0 - x)); }

double NUMerfcc (double x) {
	gsl_sf_result result;
	int status = gsl_sf_erfc_e (x, & result);
	return status == GSL_SUCCESS ? result. val : undefined;
}

double NUMgaussP (double z) {
	return 1.0 - 0.5 * NUMerfcc (NUMsqrt1_2 * z);
}

double NUMgaussQ (double z) {
	return 0.5 * NUMerfcc (NUMsqrt1_2 * z);
}

double NUMincompleteGammaP (double a, double x) {
	gsl_sf_result result;
	int status = gsl_sf_gamma_inc_P_e (a, x, & result);
	return status == GSL_SUCCESS ? result. val : undefined;
}

double NUMincompleteGammaQ (double a, double x) {
	gsl_sf_result result;
	int status = gsl_sf_gamma_inc_Q_e (a, x, & result);
	return status == GSL_SUCCESS ? result. val : undefined;
}

double NUMchiSquareP (double chiSquare, double degreesOfFreedom) {
	if (chiSquare < 0 || degreesOfFreedom <= 0) return undefined;
	return NUMincompleteGammaP (0.5 * degreesOfFreedom, 0.5 * chiSquare);
}

double NUMchiSquareQ (double chiSquare, double degreesOfFreedom) {
	if (chiSquare < 0 || degreesOfFreedom <= 0) return undefined;
	return NUMincompleteGammaQ (0.5 * degreesOfFreedom, 0.5 * chiSquare);
}

double NUMcombinations (integer n, integer k) {
	longdouble result = 1.0;
	if (k > n / 2) k = n - k;
	for (integer i = 1; i <= k; i ++) result *= n - i + 1;
	for (integer i = 2; i <= k; i ++) result /= i;
	return (double) result;
}

/* End of file NUMspecfunc.cpp */
