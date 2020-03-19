/* NUMcomplex.cpp
 *
 * Copyright (C) 2017-2020 David Weenink
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
#include "NUMcomplex.h"

/*
	The code to calculate the complex incomplete gamma function was translated from fortran code into c++ by David Weenink.
	The fortran code is from the following article:
	Eric Kostlan & Dmitry Gokhman, A program for calculating the incomplete gamma function.
	Technical report, Dept. of Mathematics, Univ. of California, Berkeley, 1987.
	
	Their algorithm calcutes the complex incomplete gamma function Γ(α,x) by using the following formula:
	(1)	Γ(α,x)= exp(-x)x^α / h(α,x),
	where h(α,x) is a continued fraction:
	(2)	h(α,x)=x+(1-α)
		         -----
		         1+1
		           -------
				   x+(2-α)
				     ------
				     1+2
				       ------
				       x+(3-α)
				         -------
				         1+...
	Efficient calculation of h(α,x) is possible because consecutive terms of a continued fraction can be computed by means of
	a two term linear recursion relation.
	If for the sequences {p[k]} and {q[k]}, k=0,1,2,... the following difference equations hold
	(3)	y[k+2]=x*y[k+1]+((k+2)/2)* y[k] for k even
		      =y[k+1]((k+3)/2-α)*y[k]  for k is odd
	and initial conditions p[0]=x, p[1]=x+1-α, q[0]=1, q[1]=1, then p[k]/q[k] is the k-th term
	in the continued fraction for h(α,x).
	
	The value of h(α+1,x) can be reduced to h(α,x) 
	(4) x/h(α+1,x) = α/h(α,x)+1 by using the following relation between gamma functions:
		Γ(α+1,x)=αΓ(α,x)+x^α exp(-x).
	If x is near the negative real axis then x can be moved to some new value y:
	(5)	Γ(α,y)-Γ(α,x)=sum(n=0,Infinity,(-1)^n(x^(α+n)-y^(α+n))/(n!(α+n)),
	where in the code below a valye of y=1 is used to facilitate the computation.
	
	Numerical considerations:
	1. In formula (5) Kostlan & Gokhman use y=1 to facilitate the computation
	2. If (α,x) is near (−n ,0), where n is a non-negative integer, but x is not equal to zero, 
	the n-th term in (5) can turn out to be 0.0/0.0. They replace this term with -log(x)-(α+n)log(x)²/2. 
	Essentially they are helping the computer to calculate (1-x^(α+n))/(α+n).
	
	Problems:
	Kostlan & Gokhman notice three intrinsic problems that any algorithm for calculating Γ(α,x) must face:
	1. For x=0 and α a nonpositive integer Γ(α,x)=∞.
	2. If α is not an integer Γ(α,x) is a multi-valued function with a branch point at x=0
	3. As we approach ∞ along certain directions computation of Γ(α,x) is limited  because of the inability to compute x^α accurately.

*/

static double norm1 (const dcomplex *x) {
	return fabs (real (*x)) + fabs (imag (*x));
}

static void xShiftTerm (const dcomplex *alpha, const dcomplex *x, integer i, dcomplex *p, dcomplex *q) {
// Calculate p*q = (-1)^i (1-x^(alpha+i))/(alpha+i)i! 
	constexpr dcomplex zero = 0.0;
	constexpr double tol = 3e-7, xlim = 39.0;
	const double di = i;

	if (i == 0)
		*q = 1.0;
	const dcomplex alphai = *alpha + di;
	if (*x == zero) {
		*p = 1.0 / alphai;
		if (i != 0)
			*q /= -di;
		return;
	}
	const dcomplex cdlx = log (*x);
	/*
		If (1-x**alphai) = -x**alphai,
		then change the inductive scheme to avoid overflow.
	*/
	if (real (alphai * cdlx) > xlim && i != 0) {
			*p *= (alphai - 1.0) / alphai;
			*q *= - *x / di;
		return;
	}
	if (norm1 (& alphai) > tol)
		*p = (1.0 - std::pow (*x, alphai)) / alphai;
	else
		*p = -cdlx * (1.0 + cdlx * alphai * 0.5);
	if (i == 0)
		*q /= - di;
}

static void continuedFractionExpansion (const dcomplex *alpha, const dcomplex *x, dcomplex *result) {
	constexpr double tol1 = 1e10, tol2 = 1e-10, error = 1e-18;
	constexpr dcomplex zero (0.0,0.0);
	dcomplex q0 = 1.0, q1 = 1.0, p0 = *x;
	dcomplex p1 = *x + 1.0 - *alpha, r0;
	for (integer i = 1; i <= 100000; i++) {
		const double di = i;
		if (p0 != zero && q0 != zero && q1 != zero) {
			r0 = p0 / q0;
			*result = p1 / q1;
			const dcomplex r0mr1 = r0 - *result;
			if (norm1 (& r0mr1) < norm1 (result) * error)
				return;
			/*
				Renormalize to avoid underflow or overflow
			*/
			if (norm1 (& p0) > tol1 || norm1 (& p0) < tol2 || norm1 (& q0) > tol1 || norm1 (& q0) < tol2) {
				const dcomplex factor = p0 * q0;
				p0 /= factor;
				q0 /= factor;
				p1 /= factor;
				q1 /= factor;
			}
			p0 = *x * p1 + di * p0; // y[k+2] = x * y[k+1] + (k+2)/2 * y[k] with k even
			q0 = *x * q1 + di * q0;
			p1 = p0 + (di + 1.0 - *alpha) * p1; // y[k+2] = y[k+1] + ((k+3)/2 - alpha) * y[k] with k odd
			q1 = q0 + (di + 1.0 - *alpha) * q1;
		} else {
			// We should not come here at all!
			*result = 0.5 * (r0 + *result);
			return;
		}
	}
	// We should not come here at all!
	*result = 0.5 * (r0 + *result);
}

static void shiftAlphaByOne (const dcomplex *alpha, const dcomplex *x, dcomplex *result) {
	constexpr dcomplex one (1.0, 0.0);
	const integer n = (integer) (real (*alpha) - real (*x));
	if (n > 0) {
		dcomplex cn = n + 1;
		dcomplex term = one / *x;
		dcomplex sum = term;
		const dcomplex alpha1 = *alpha - cn;
		for (integer i = 1; i <= n; i ++) {
			cn = n - i + 1;
			term *= (alpha1 + cn) / *x;
			sum += term;
		}
		continuedFractionExpansion (& alpha1, x, result);
		sum += term * alpha1 / *result;
		*result = one / sum;
	} else {
		continuedFractionExpansion (alpha, x, result);
	}
}

// Gamma[alpha,x] = integral{x, infty, t^(alpha-1)exp(-t)dt}, Gamma[alpha]= Gamma[alpha,0]
dcomplex NUMincompleteGammaFunction (const dcomplex alpha, const dcomplex x) {
	dcomplex result;
	constexpr double xlim = 1.0;
	constexpr integer ibuf = 34;
	constexpr dcomplex re = 0.36787944117144232, one = 1.0;
	dcomplex p, q, r;
	if (norm1 (& x) < xlim || (real (x) < 0.0 && fabs (imag (x)) < xlim)) {
		shiftAlphaByOne (& alpha, & one, & r);
		result = re / r;
		const integer ilim = (integer) real (x / re);
		for (integer i = 0; i <= ibuf - ilim; i++) {
			xShiftTerm (& alpha, & x, i, & p, & q);
			result += p * q;
		}
	} else {
		shiftAlphaByOne (& alpha, & x, & r);
		result = exp (-x + alpha * log (x)) / r;
	}
	return { real (result), imag (result) };
}

// End of translated fortran code

/* 
* We have to scale the amplitude "a" of the gammatone such that the response is 0 dB at the peak (w=w0);
* 
* To calculate the spectrum of the truncated gammatone 
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
dcomplex gammaToneFilterResponseAtCentreFrequency (double centre_frequency, double bandwidth, double gamma, double initialPhase, double truncationTime) {
	const double b = NUM2pi * bandwidth, w0 = NUM2pi * centre_frequency, theta = atan (2.0 * centre_frequency / bandwidth);
	const double gamma_n = exp (NUMlnGamma (gamma)), bpow = pow (b, -gamma);
	const dcomplex expiphi (cos (initialPhase), sin (initialPhase)), expmiphi = conj (expiphi);
	const dcomplex expnitheta (cos (gamma * theta), - sin (gamma * theta));
	const dcomplex expiw0T (cos (w0 * truncationTime), sin (w0 * truncationTime));
	const dcomplex peak = expnitheta * pow (1.0 + 4.0 * (w0 / b) * (w0 / b), - 0.5 * gamma);
	const dcomplex r1 = NUMincompleteGammaFunction (dcomplex {gamma, 0.0},dcomplex { b * truncationTime, 0.0});
	const dcomplex r2 = NUMincompleteGammaFunction (dcomplex {gamma, 0.0}, dcomplex {b * truncationTime, 2.0 * w0 * truncationTime});
	const dcomplex response = 0.5 * bpow * ((expiphi + expmiphi * peak) * gamma_n -
		expiw0T * (expiphi * r1 + expmiphi * peak * r2));
	return response;
}

/* End of file NUMcomplex.cpp */
