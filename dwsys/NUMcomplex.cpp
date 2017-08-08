/* NUMcomplex.cpp
 *
 * Copyright (C) 2017 David Weenink
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

#include <cmath>
#include <complex>
#include "NUMcomplex.h"

// The following code was translated from fortran into c++ by David weenink.
// The fortran code is from the article of Eric Kostlan and Dmitry Gokhman, A program for calculating the incomplete 
//		gamma function. Technical report, Dept. of Mathematics, Univ. of California, Berkeley, 1987.

static double norm1 (std::complex<double> *x) {
	return fabs (x -> real()) + fabs (imag(*x));
}

static void term (std::complex<double> *alpha, std::complex<double> *x, long i, std::complex<double> *p, std::complex<double> *q) {
// Calculate p*q = (-1)^i (1-x^(alpha+i))/(alpha+i)i! 
	std::complex<double> zero = 0.0;
	double tol = 3e-7, xlim = 39.0, di = i;

	if (i == 0) {
		*q = 1.0;
	}
	std::complex<double> alphai = *alpha + di;
	if (*x == zero) {
		*p = 1.0 / alphai;
		if (i != 0) {
			*q /= -di;
		}
		return;
	}
	std::complex<double> cdlx = log (*x);
	// If (1-x**alphai) = -x**alphai,
	// then change the inductive scheme to avoid overflow.
	if ((alphai * cdlx).real() > xlim && i != 0) {
			*p *= (alphai - 1.0) / alphai;
			*q *= - *x / di;
		return;
	}
	if (norm1 (& alphai) > tol) {
		*p = (1.0 - std::pow (*x, alphai)) / alphai;
	} else {
		*p = -cdlx * (1.0 + cdlx * alphai * 0.5);
	}
	if (i == 0) {
		*q /= - di;
	}
}

static void cdhs (std::complex<double> *alpha, std::complex<double> *x, std::complex<double> *result) {
	std::complex<double> zero (0.0,0.0);
	std::complex<double> q0 = 1.0, q1 = 1.0, p0 = *x, p1 = *x + 1.0 - *alpha, r0;
	double tol1 = 1e10, tol2 = 1e-10, error = 1e-18;
	for (long i = 1; i <= 100000; i++) {
		double di = i;
		if (p0 != zero && q0 != zero && q1 != zero) {
			r0 = p0 / q0;
			*result = p1 / q1;
			std::complex<double> r0mr1 = r0 - *result;
			if (norm1 (& r0mr1) < norm1 (result) * error) {
				return;
			}
			// renormalize to avoid underflow or overflow
			if (norm1 (& p0) > tol1 || norm1 (& p0) < tol2 || norm1 (& q0) > tol1 || norm1 (& q0) < tol2) {
				std::complex<double> factor = p0 * q0;
				p0 /= factor;
				q0 /= factor;
				p1 /= factor;
				q1 /= factor;
			}
			p0 = *x * p1 + di * p0; // y[k+2] = x * y[k+1] + (k+2)/2 * y[k] with k even
			q0 = *x * q1 + di * q0;
			p1 = p0 + (di + 1.0 - *alpha) * p1; // y[k+2] = y[k+1] + ((k+3)/2 - alpha) * y[k] with k odd
			q1 = q0 + (di + 1.0 - *alpha) * q1;
		}
	}
	// We should not come here at all!
	*result = 0.5 * (r0 + *result);
}

static void cdh (std::complex<double> *alpha, std::complex<double> *x, std::complex<double> *result) {
	std::complex<double> one (1.0, 0.0);
	long n = (long) (*alpha - *x).real();
	if (n > 0) {
		std::complex<double> cn = n + 1;
		std::complex<double> alpha1 = *alpha - cn;
		std::complex<double> term = one / *x;
		std::complex<double> sum = term;
		for (long i = 1; i <= n; i++) {
			cn = n - i + 1;
			term *= (alpha1 + cn) / *x;
			sum += term;
		}
		cdhs (&alpha1, x, result);
		sum += term * alpha1 / *result;
		*result = one / sum;
	} else {
		cdhs (alpha, x, result);
	}
}

// Gamma[alpha,x] = integral{x, infty, t^(alpha-1)exp(-t)dt}, Gamma[alpha]= Gamma[alpha,0]
void NUMincompleteGammaFunction (double alpha_re, double alpha_im, double x_re, double x_im, double *result_re, double *result_im) {
	std::complex<double> alpha (alpha_re, alpha_im), x (x_re, x_im), result;
	double xlim = 1.0;
	long ibuf = 34;
	std::complex<double> re = 0.36787944117144232, one = 1.0, p, q, r;
	if (norm1 (& x) < xlim || x.real() < 0.0 && fabs (imag (x)) < xlim) {
		cdh (& alpha, & one, & r);
		result = re / r;
		long ilim = (long) (x / re).real();
		for (long i = 0; i <= ibuf - ilim; i++) {
			term (& alpha, & x, i, & p, & q);
			result += p * q;
		}
	} else {
		cdh (& alpha, & x, & r);
		result = exp (-x + alpha * log (x)) / r;
	}
	if (result_re) {
		*result_re = result.real();
	}
	if (result_im) {
		*result_im = result.imag();
	}
}

// End of translated fortran code

/* 
* We have to scale the amplitude "a" of the gammatone such that the response is 0 dB at the peak (w=w0);
* 
* To calculate the spectrum of the finite gammatone 
* 		g(t) = t^(n-1)*exp(-b*t)cos(w0*t+phi) for 0 < t <= T
* 		g(t) = 0 for t > T
* we can write
* 		g(t)= t^(n-1)*exp(-b*t)(exp(I*(w0*t+phi))+exp(-I*(w0*t+phi)))/2
* 			= (gp(t)+gm(t))/2, where
* 		gp(t) = t^(n-1)*exp(-b*t + I*(w0*t+phi))
* 		gm(t) = t^(n-1)*exp(-b*t - I*(w0*t+phi))
* 
* Laplace[g(t)]= Laplace[g(t)]-Laplace[g(t+T)],where
* 		g(t+T) = g2(t) = (t+T)^(n-1)*exp(-b*(t+T))cos(w0*(t+T)+phi) for t>0
* 
*		g2(t) = (t+T)^(n-1)*(exp(-b*(t+T)+I(w0*(t+T)+phi) + exp(-b*(t+T)-I(w0*(t+T)+phi)))/2
* 			  = (exp(-b*T+I*w0*T+I*phi)(t+T)^(n-1)exp(-b*t+I*w0*t) + exp(-b*T-I*w0*T-I*phi)(t+T)^(n-1)exp(-b*t-I*w0*t))/2
* 			  = (gpT(t)+gmT(t))/2, where
* 		gpT(t)= exp(-b*T+I*w0*T+I*phi) (t+T)^(n-1) exp(-b*t+I*w0*t)
* 		gmT(t)= exp(-b*T-I*w0*T-I*phi) (t+T)^(n-1) exp(-b*t-I*w0*t)
* 
* Fp(w)  = Laplace[gp(t), s=Iw] = exp( I*phi) (b+I(w-w0))^-n Gamma[n]
* Fm(w)  = Laplace[gm(t), s=Iw] = exp(-I*phi) (b+I(w+w0))^-n Gamma[n]
* FpT(w) = Laplace[gpT(t),s=Iw] = exp( I*phi) exp(I*w*T) (b+I(w-w0))^-n Gamma[n, b*T+I(w-w0)*T]
* FmT(w) = Laplace[gmT(t),s=Iw] = exp(-I*phi) exp(I*w*T) (b+I(w+w0))^-n Gamma[n, b*T+I(w+w0)*T]
* F[g(t)] = (Fp(w) + Fm(w) - FpT(w) - FmT(w)) / 2
* 
* At resonance w=w0:
* 	F(w0) = (Fp(w0) + Fm(w0) - FpT(w0) - FmT(w0)) / 2 =
*		  = (exp(I*phi) (b)^-n Gamma[n] + exp(-I*phi) (b+I*2*w0)^-n Gamma[n] -
* 			exp(I*phi) exp(I*w0*T) (b)^-n Gamma[n, b*T] -
* 			exp(-I*phi) exp(I*w0*T) (b+I*2*w0)^-n Gamma[n, b*T+I*2*w0)]) / 2
* 		  = b^-n(exp(I*phi) Gamma[n] + exp(-I*phi) (1+I*2*w0/b)^-n Gamma[n] -
* 			exp(I*phi) exp(I*w0*T) Gamma[n, b*T0] - exp(-I*phi) exp(I*w0*T) (1+I*2*w0/b)^-n Gamma[n, b*T+I*2*w0*T)]) / 2
* 
* (x+I*y)^-n = (r*exp(I theta))^-n, where r = sqrt(x^2+y^2) and theta = ArcTan (y/x)
* (1+I*a)^-n = (1+a^2)^(-n/2) exp(-I*n*theta)
*/
void gammaToneFilterResponseAtResonance (double centre_frequency, double bandwidth, long gamma, double initialPhase, double t0, double *response_re, double *response_im) {
	
	double b = NUM2pi * bandwidth, w0 = NUM2pi * centre_frequency, theta = atan (2.0 * centre_frequency / bandwidth);
	double gamma_n = exp (NUMlnGamma (gamma)), bpow = pow (b, -gamma);
	std::complex<double> expiphi (cos (initialPhase), sin(initialPhase)), expmiphi = conj (expiphi);
	std::complex<double> expnitheta (cos (gamma * theta), - sin(gamma * theta));
	std::complex<double> expiw0t0 (cos (w0 * t0), sin (w0 * t0));
	std::complex<double> peak = expnitheta * pow (1.0 + 4.0 * (w0 / b) * (w0 / b), - 0.5 * gamma);
	double result1_re, result1_im, result2_re, result2_im;
	NUMincompleteGammaFunction (gamma, 0.0, b * t0, 0.0,           & result1_re, & result1_im);
	NUMincompleteGammaFunction (gamma, 0.0, b * t0, 2.0 * w0 * t0, & result2_re, & result2_im);
	std::complex<double> result1 (result1_re, result1_im), result2 (result2_re, result2_im);
	std::complex<double> filterResponseAtResonance = 0.5 * bpow * ((expiphi + expmiphi * peak) * gamma_n -
		expiw0t0 * (expiphi * result1 + expmiphi * peak * result2));
	if (response_re) {
		*response_re = filterResponseAtResonance.real ();
	}
	if (response_im) {
		*response_im = filterResponseAtResonance.imag ();
	}
}


/* End of file NUMcomplex.cpp */
