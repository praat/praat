#ifndef _complex_h_
#define _complex_h_
/* complex.h
 *
 * Copyright (C) 1992-2011,2017 Paul Boersma
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

#include <math.h>

struct dcomplex { double re, im; };

inline static dcomplex dcomplex_add (dcomplex a, dcomplex b) {
	dcomplex result;
	result.re = a.re + b.re;
	result.im = a.im + b.im;
	return result;
}

inline static dcomplex dcomplex_sub (dcomplex a, dcomplex b) {
	dcomplex result;
	result.re = a.re - b.re;
	result.im = a.im - b.im;
	return result;
}

inline static dcomplex dcomplex_mul (dcomplex a, dcomplex b) {
	dcomplex result;
	result.re = a.re * b.re - a.im * b.im;
	result.im = a.im * b.re + a.re * b.im;
	return result;
}

inline static dcomplex dcomplex_conjugate (dcomplex z) {
	dcomplex result;
	result.re = z.re;
	result.im = - z.im;
	return result;
}

inline static dcomplex dcomplex_div (dcomplex a, dcomplex b) {
	dcomplex result;
	double r, den;
	if (fabs (b.re) >= fabs (b.im)) {
		r = b.im / b.re;
		den = b.re + r * b.im;
		result.re = (a.re + r * a.im) / den;
		result.im = (a.im - r * a.re) / den;
	} else {
		r = b.re / b.im;
		den = b.im + r * b.re;
		result.re = (a.re * r + a.im) / den;
		result.im = (a.im * r - a.re) / den;
	}
	return result;
}

inline static double dcomplex_abs (dcomplex z) {
	double x, y, temp;
	x = fabs (z.re);
	y = fabs (z.im);
	if (x == 0.0) return y;
	if (y == 0.0) return x;
	if (x > y) {
		temp = y / x;
		return x * sqrt (1.0 + temp * temp);
	} else {
		temp = x / y;
		return y * sqrt (1.0 + temp * temp);
	}
}

inline static dcomplex dcomplex_rmul (double x, dcomplex a) {
	dcomplex result;
	result.re = x * a.re;
	result.im = x * a.im;
	return result;
}

inline static dcomplex dcomplex_exp (dcomplex z) {
	dcomplex result;
	double size = exp (z.re);
	result.re = size * cos (z.im);
	result.im = size * sin (z.im);
	return result;
}

dcomplex dcomplex_sqrt (dcomplex z);

/* End of file complex.h */
#endif
