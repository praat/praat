/* NUM.cpp
 *
 * Copyright (C) 1992-2008,2011,2012,2014,2015,2017 Paul Boersma
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
 * pb 2005/07/08 NUMpow
 * pb 2006/08/02 NUMinvSigmoid
 * pb 2007/01/27 use #defines for value interpolation
 * pb 2007/08/20 built a "weird value" check into NUMviterbi (bug report by Adam Jacks)
 * pb 2008/01/19 double
 * pb 2008/09/21 NUMshift
 * pb 2008/09/22 NUMscale
 * pb 2011/03/29 C++
 */

#include "melder.h"
#include "NUM2.h"
#define SIGN(x,s) ((s) < 0 ? -fabs (x) : fabs(x))
double NUMpow (double base, double exponent) { return base <= 0.0 ? 0.0 : pow (base, exponent); }
/*
	GSL is more accurate than the other routines, but makes
	"Sound: To Intensity..." 10 times slower...
*/
#include "gsl_errno.h"
#include "gsl_sf_bessel.h"
#include "gsl_sf_gamma.h"
#include "gsl_sf_erf.h"

void NUMshift (double *x, double xfrom, double xto) {
	if (*x == xfrom) *x = xto; else *x += xto - xfrom;
}

void NUMscale (double *x, double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	if (*x == xminfrom) *x = xminto;
	else if (*x == xmaxfrom) *x = xmaxto;
	else *x = xminto + (xmaxto - xminto) * ((*x - xminfrom) / (xmaxfrom - xminfrom));
}

void NUMinit () {
	gsl_set_error_handler_off ();
	NUMrandom_init ();
}

void NUMfbtoa (double formant, double bandwidth, double dt, double *a1, double *a2) {
	*a1 = 2 * exp (- NUMpi * bandwidth * dt) * cos (2 * NUMpi * formant * dt);
	*a2 = exp (- 2 * NUMpi * bandwidth * dt);
}

void NUMfilterSecondOrderSection_a (double x [], long n, double a1, double a2) {
	x [2] += a1 * x [1];
	for (long i = 3; i <= n; i ++)
		x [i] += a1 * x [i - 1] - a2 * x [i - 2];
}

void NUMfilterSecondOrderSection_fb (double x [], long n, double dt, double formant, double bandwidth) {
	double a1, a2;
	NUMfbtoa (formant, bandwidth, dt, & a1, & a2);
	NUMfilterSecondOrderSection_a (x, n, a1, a2);
}

double NUMftopreemphasis (double f, double dt) {
	return exp (- 2.0 * NUMpi * f * dt);
}

void NUMpreemphasize_a (double x [], long n, double preemphasis) {
	for (long i = n; i >= 2; i --)
		x [i] -= preemphasis * x [i - 1];
}

void NUMdeemphasize_a (double x [], long n, double preemphasis) {
	long i;
	for (i = 2; i <= n; i ++)
		x [i] += preemphasis * x [i - 1];
}

void NUMpreemphasize_f (double x [], long n, double dt, double frequency) {
	NUMpreemphasize_a (x, n, NUMftopreemphasis (frequency, dt));
}

void NUMdeemphasize_f (double x [], long n, double dt, double frequency) {
	NUMdeemphasize_a (x, n, NUMftopreemphasis (frequency, dt));
}

void NUMautoscale (double x [], long n, double scale) {
	double maximum = 0.0;
	for (long i = 1; i <= n; i ++)
		if (fabs (x [i]) > maximum) maximum = fabs (x [i]);
	if (maximum > 0.0) {
		double factor = scale / maximum;
		for (long i = 1; i <= n; i ++)
			x [i] *= factor;
	}
}

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

double NUMbesselI (long n, double x) {
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

double NUMbesselK_f (long n, double x) {
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
	for (long i = 1; i < n; i ++) {
		besselK = besselK_min2 + twoByX * i * besselK_min1;
		besselK_min2 = besselK_min1;
		besselK_min1 = besselK;
	}
	Melder_assert (isdefined (besselK));
	return besselK;
}

double NUMbesselK (long n, double x) {
	gsl_sf_result result;
	int status = gsl_sf_bessel_Kn_e (n, x, & result);
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
	return 1 - 0.5 * NUMerfcc (NUMsqrt1_2 * z);
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

double NUMcombinations (long n, long k) {
	double result = 1.0;
	long i;
	if (k > n / 2) k = n - k;
	for (i = 1; i <= k; i ++) result *= n - i + 1;
	for (i = 2; i <= k; i ++) result /= i;
	return result;
}

#define NUM_interpolate_simple_cases \
	if (nx < 1) return undefined; \
	if (x > nx) return y [nx]; \
	if (x < 1) return y [1]; \
	if (x == midleft) return y [midleft]; \
	/* 1 < x < nx && x not integer: interpolate. */ \
	if (maxDepth > midright - 1) maxDepth = midright - 1; \
	if (maxDepth > nx - midleft) maxDepth = nx - midleft; \
	if (maxDepth <= NUM_VALUE_INTERPOLATE_NEAREST) return y [(long) floor (x + 0.5)]; \
	if (maxDepth == NUM_VALUE_INTERPOLATE_LINEAR) return y [midleft] + (x - midleft) * (y [midright] - y [midleft]); \
	if (maxDepth == NUM_VALUE_INTERPOLATE_CUBIC) { \
		double yl = y [midleft], yr = y [midright]; \
		double dyl = 0.5 * (yr - y [midleft - 1]), dyr = 0.5 * (y [midright + 1] - yl); \
		double fil = x - midleft, fir = midright - x; \
		return yl * fir + yr * fil - fil * fir * (0.5 * (dyr - dyl) + (fil - 0.5) * (dyl + dyr - 2 * (yr - yl))); \
	}

#if defined (__POWERPC__)
double NUM_interpolate_sinc (double y [], long nx, double x, long maxDepth) {
	long ix, midleft = floor (x), midright = midleft + 1, left, right;
	double result = 0.0, a, halfsina, aa, daa, cosaa, sinaa, cosdaa, sindaa;
	NUM_interpolate_simple_cases
	left = midright - maxDepth, right = midleft + maxDepth;
	a = NUMpi * (x - midleft);
	halfsina = 0.5 * sin (a);
	aa = a / (x - left + 1); cosaa = cos (aa); sinaa = sin (aa);
	daa = NUMpi / (x - left + 1); cosdaa = cos (daa); sindaa = sin (daa);
	for (ix = midleft; ix >= left; ix --) {
		double d = halfsina / a * (1.0 + cosaa), help;
		result += y [ix] * d;
		a += NUMpi;
		help = cosaa * cosdaa - sinaa * sindaa;
		sinaa = cosaa * sindaa + sinaa * cosdaa;
		cosaa = help;
		halfsina = - halfsina;
	}
	a = NUMpi * (midright - x);
	halfsina = 0.5 * sin (a);
	aa = a / (right - x + 1); cosaa = cos (aa); sinaa = sin (aa);
	daa = NUMpi / (right - x + 1); cosdaa = cos (daa); sindaa = sin (daa);
	for (ix = midright; ix <= right; ix ++) {
		double d = halfsina / a * (1.0 + cosaa), help;
		result += y [ix] * d;
		a += NUMpi;
		help = cosaa * cosdaa - sinaa * sindaa;
		sinaa = cosaa * sindaa + sinaa * cosdaa;
		cosaa = help;
		halfsina = - halfsina;
	}
	return result;
}
#else
double NUM_interpolate_sinc (double y [], long nx, double x, long maxDepth) {
	long ix, midleft = (long) floor (x), midright = midleft + 1, left, right;
	double result = 0.0, a, halfsina, aa, daa;
	NUM_interpolate_simple_cases
	left = midright - maxDepth, right = midleft + maxDepth;
	a = NUMpi * (x - midleft);
	halfsina = 0.5 * sin (a);
	aa = a / (x - left + 1);
	daa = NUMpi / (x - left + 1);
	for (ix = midleft; ix >= left; ix --) {
		double d = halfsina / a * (1.0 + cos (aa));
		result += y [ix] * d;
		a += NUMpi;
		aa += daa;
		halfsina = - halfsina;
	}
	a = NUMpi * (midright - x);
	halfsina = 0.5 * sin (a);
	aa = a / (right - x + 1);
	daa = NUMpi / (right - x + 1); \
	for (ix = midright; ix <= right; ix ++) {
		double d = halfsina / a * (1.0 + cos (aa));
		result += y [ix] * d;
		a += NUMpi;
		aa += daa;
		halfsina = - halfsina;
	}
	return result;
}
#endif

/********** Improving extrema **********/
#pragma mark Improving extrema

struct improve_params {
	int depth;
	double *y;
	long ixmax;
	int isMaximum;
};

static double improve_evaluate (double x, void *closure) {
	struct improve_params *me = (struct improve_params *) closure;
	double y = NUM_interpolate_sinc (my y, my ixmax, x, my depth);
	return my isMaximum ? - y : y;
}

double NUMimproveExtremum (double *y, long nx, long ixmid, int interpolation, double *ixmid_real, int isMaximum) {
	struct improve_params params;
	double result;
	if (ixmid <= 1) { *ixmid_real = 1; return y [1]; }
	if (ixmid >= nx) { *ixmid_real = nx; return y [nx]; }
	if (interpolation <= NUM_PEAK_INTERPOLATE_NONE) { *ixmid_real = ixmid; return y [ixmid]; }
	if (interpolation == NUM_PEAK_INTERPOLATE_PARABOLIC) {
		double dy = 0.5 * (y [ixmid + 1] - y [ixmid - 1]);
		double d2y = 2 * y [ixmid] - y [ixmid - 1] - y [ixmid + 1];
		*ixmid_real = ixmid + dy / d2y;
		return y [ixmid] + 0.5 * dy * dy / d2y;
	}
	/* Sinc interpolation. */
	params. y = y;
	params. depth = interpolation == NUM_PEAK_INTERPOLATE_SINC70 ? 70 : 700;
	params. ixmax = nx;
	params. isMaximum = isMaximum;
	/*return isMaximum ?
		- NUM_minimize (ixmid - 1, ixmid, ixmid + 1, improve_evaluate, & params, 1e-10, 1e-11, ixmid_real) :
		NUM_minimize (ixmid - 1, ixmid, ixmid + 1, improve_evaluate, & params, 1e-10, 1e-11, ixmid_real);*/
	*ixmid_real = NUMminimize_brent (improve_evaluate, ixmid - 1, ixmid + 1, & params, 1e-10, & result);
	return isMaximum ? - result : result;
}

double NUMimproveMaximum (double *y, long nx, long ixmid, int interpolation, double *ixmid_real)
	{ return NUMimproveExtremum (y, nx, ixmid, interpolation, ixmid_real, 1); }
double NUMimproveMinimum (double *y, long nx, long ixmid, int interpolation, double *ixmid_real)
	{ return NUMimproveExtremum (y, nx, ixmid, interpolation, ixmid_real, 0); }

/********** Viterbi **********/

void NUM_viterbi (
	long numberOfFrames, long maxnCandidates,
	long (*getNumberOfCandidates) (long iframe, void *closure),
	double (*getLocalCost) (long iframe, long icand, void *closure),
	double (*getTransitionCost) (long iframe, long icand1, long icand2, void *closure),
	void (*putResult) (long iframe, long place, void *closure),
	void *closure)
{
	autoNUMmatrix <double> delta (1, numberOfFrames, 1, maxnCandidates);
	autoNUMmatrix <long> psi (1, numberOfFrames, 1, maxnCandidates);
	autoNUMvector <long> numberOfCandidates (1, numberOfFrames);
	for (long iframe = 1; iframe <= numberOfFrames; iframe ++) {
		numberOfCandidates [iframe] = getNumberOfCandidates (iframe, closure);
		for (long icand = 1; icand <= numberOfCandidates [iframe]; icand ++)
			delta [iframe] [icand] = - getLocalCost (iframe, icand, closure);
	}
	for (long iframe = 2; iframe <= numberOfFrames; iframe ++) {
		for (long icand2 = 1; icand2 <= numberOfCandidates [iframe]; icand2 ++) {
			double maximum = -1e308;
			long place = 0;
			for (long icand1 = 1; icand1 <= numberOfCandidates [iframe - 1]; icand1 ++) {
				double value = delta [iframe - 1] [icand1] + delta [iframe] [icand2]
					- getTransitionCost (iframe, icand1, icand2, closure);
				if (value > maximum) { maximum = value; place = icand1; }
			}
			if (place == 0)
				Melder_throw (U"Viterbi algorithm cannot compute a track because of weird values.");
			delta [iframe] [icand2] = maximum;
			psi [iframe] [icand2] = place;
		}
	}
	/*
	 * Find the end of the most probable path.
	 */
	long place;
	double maximum = delta [numberOfFrames] [place = 1];
	for (long icand = 2; icand <= numberOfCandidates [numberOfFrames]; icand ++)
		if (delta [numberOfFrames] [icand] > maximum)
			maximum = delta [numberOfFrames] [place = icand];
	/*
	 * Backtrack.
	 */
	for (long iframe = numberOfFrames; iframe >= 1; iframe --) {
		putResult (iframe, place, closure);
		place = psi [iframe] [place];
	}
}

/******************/

struct parm2 {
	int ntrack;
	long ncomb;
	long **indices;
	double (*getLocalCost) (long iframe, long icand, int itrack, void *closure);
	double (*getTransitionCost) (long iframe, long icand1, long icand2, int itrack, void *closure);
	void (*putResult) (long iframe, long place, int itrack, void *closure);
	void *closure;
};

static long getNumberOfCandidates_n (long iframe, void *closure) {
	struct parm2 *me = (struct parm2 *) closure;
	(void) iframe;
	return my ncomb;
}
static double getLocalCost_n (long iframe, long jcand, void *closure) {
	struct parm2 *me = (struct parm2 *) closure;
	double localCost = 0.0;
	for (int itrack = 1; itrack <= my ntrack; itrack ++)
		localCost += my getLocalCost (iframe, my indices [jcand] [itrack], itrack, my closure);
	return localCost;
}
static double getTransitionCost_n (long iframe, long jcand1, long jcand2, void *closure) {
	struct parm2 *me = (struct parm2 *) closure;
	double transitionCost = 0.0;
	for (int itrack = 1; itrack <= my ntrack; itrack ++)
		transitionCost += my getTransitionCost (iframe,
			my indices [jcand1] [itrack], my indices [jcand2] [itrack], itrack, my closure);
	return transitionCost;
}
static void putResult_n (long iframe, long jplace, void *closure) {
	struct parm2 *me = (struct parm2 *) closure;
	for (int itrack = 1; itrack <= my ntrack; itrack ++)
		my putResult (iframe, my indices [jplace] [itrack], itrack, my closure);
}

void NUM_viterbi_multi (
	long nframe, long ncand, int ntrack,
	double (*getLocalCost) (long iframe, long icand, int itrack, void *closure),
	double (*getTransitionCost) (long iframe, long icand1, long icand2, int itrack, void *closure),
	void (*putResult) (long iframe, long place, int itrack, void *closure),
	void *closure)
{
	struct parm2 parm;
	parm.indices = nullptr;

	if (ntrack > ncand) Melder_throw (U"(NUM_viterbi_multi:) "
		U"Number of tracks (", ntrack, U") should not exceed number of candidates (", ncand, U").");
	double ncomb = NUMcombinations (ncand, ntrack);
	if (ncomb > 10000000) Melder_throw (U"(NUM_viterbi_multi:) "
		U"Unrealistically high number of combinations (", ncomb, U").");
	parm. ntrack = ntrack;
	parm. ncomb = ncomb;

	/*
	 * For ncand == 5 and ntrack == 3, parm.indices is going to contain:
	 *   1 2 3
	 *   1 2 4
	 *   1 2 5
	 *   1 3 4
	 *   1 3 5
	 *   1 4 5
	 *   2 3 4
	 *   2 3 5
	 *   2 4 5
	 *   3 4 5
	 */
	autoNUMmatrix <long> indices (1, parm. ncomb, 1, ntrack);
	parm.indices = indices.peek();
	autoNUMvector <long> icand (1, ntrack);
	for (int itrack = 1; itrack <= ntrack; itrack ++)
		icand [itrack] = itrack;   // start out with "1 2 3"
	long jcomb = 0;
	for (;;) {
		jcomb ++;
		for (int itrack = 1; itrack <= ntrack; itrack ++)
			parm. indices [jcomb] [itrack] = icand [itrack];
		int itrack = ntrack;
		for (; itrack >= 1; itrack --) {
			if (++ icand [itrack] <= ncand - (ntrack - itrack)) {
				for (int jtrack = itrack + 1; jtrack <= ntrack; jtrack ++)
					icand [jtrack] = icand [itrack] + jtrack - itrack;
				break;
			}
		}
		if (itrack == 0) break;
	}
	Melder_assert (jcomb == ncomb);
	parm. getLocalCost = getLocalCost;
	parm. getTransitionCost = getTransitionCost;
	parm. putResult = putResult;
	parm. closure = closure;
	NUM_viterbi (nframe, ncomb, getNumberOfCandidates_n, getLocalCost_n, getTransitionCost_n, putResult_n, & parm);
}

int NUMrotationsPointInPolygon (double x0, double y0, long n, double x [], double y []) {
	long nup = 0, i;
	int upold = y [n] > y0, upnew;
	for (i = 1; i <= n; i ++) if ((upnew = y [i] > y0) != upold) {
		long j = i == 1 ? n : i - 1;
		if (x0 < x [i] + (x [j] - x [i]) * (y0 - y [i]) / (y [j] - y [i])) {
			if (upnew) nup ++; else nup --;
		}
		upold = upnew;
	}
	return nup;
}

/* End of file NUM.cpp */
