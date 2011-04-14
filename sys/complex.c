/* complex.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 1999/06/19
 * pb 2002/03/07 GPL
 * pb 2011/04/06 C++
 */

#include <math.h>
#include "complex.h"

fcomplex fcomplex_add (fcomplex a, fcomplex b) {
	fcomplex result;
	result.re = a.re + b.re;
	result.im = a.im + b.im;
	return result;
}

dcomplex dcomplex_add (dcomplex a, dcomplex b) {
	dcomplex result;
	result.re = a.re + b.re;
	result.im = a.im + b.im;
	return result;
}

fcomplex fcomplex_sub (fcomplex a, fcomplex b) {
	fcomplex result;
	result.re = a.re - b.re;
	result.im = a.im - b.im;
	return result;
}

dcomplex dcomplex_sub (dcomplex a, dcomplex b) {
	dcomplex result;
	result.re = a.re - b.re;
	result.im = a.im - b.im;
	return result;
}

fcomplex fcomplex_mul (fcomplex a, fcomplex b) {
	fcomplex result;
	result.re = a.re * b.re - a.im * b.im;
	result.im = a.im * b.re + a.re * b.im;
	return result;
}

dcomplex dcomplex_mul (dcomplex a, dcomplex b) {
	dcomplex result;
	result.re = a.re * b.re - a.im * b.im;
	result.im = a.im * b.re + a.re * b.im;
	return result;
}

fcomplex fcomplex_create (float re, float im) {
	fcomplex result;
	result.re = re;
	result.im = im;
	return result;
}

dcomplex dcomplex_create (double re, double im) {
	dcomplex result;
	result.re = re;
	result.im = im;
	return result;
}

fcomplex fcomplex_conjugate (fcomplex z) {
	fcomplex result;
	result.re = z.re;
	result.im = - z.im;
	return result;
}

dcomplex dcomplex_conjugate (dcomplex z) {
	dcomplex result;
	result.re = z.re;
	result.im = - z.im;
	return result;
}

fcomplex fcomplex_div (fcomplex a, fcomplex b) {
	fcomplex result;
	float r, den;
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

dcomplex dcomplex_div (dcomplex a, dcomplex b) {
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

float fcomplex_abs (fcomplex z) {
	float x, y, temp;
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

double dcomplex_abs (dcomplex z) {
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

fcomplex fcomplex_sqrt (fcomplex z) {
	fcomplex result;
	float x, y, w, r;
	if (z.re == 0 && z.im == 0) {
		result.re = 0;
		result.im = 0;
		return result;
	}
	x = fabs (z.re);
	y = fabs (z.im);
	if (x >= y) {
		r = y / x;
		w = sqrt (x) * sqrt (0.5 * (1.0 + sqrt (1.0 + r * r)));
	} else {
		r = x / y;
		w = sqrt (y) * sqrt (0.5 * (r + sqrt (1.0 + r * r)));
	}
	if (z.re >= 0.0) {
		result.re = w;
		result.im = z.im / (2.0 * w);
	} else {
		result.im = (z.im >= 0) ? w : -w;
		result.re = z.im / (2.0 * result.im);
	}
	return result;
}

dcomplex dcomplex_sqrt (dcomplex z) {
	dcomplex result;
	double x, y, w, r;
	if (z.re == 0 && z.im == 0) {
		result.re = 0;
		result.im = 0;
		return result;
	}
	x = fabs (z.re);
	y = fabs (z.im);
	if (x >= y) {
		r = y / x;
		w = sqrt (x) * sqrt (0.5 * (1.0 + sqrt (1.0 + r * r)));
	} else {
		r = x / y;
		w = sqrt (y) * sqrt (0.5 * (r + sqrt (1.0 + r * r)));
	}
	if (z.re >= 0.0) {
		result.re = w;
		result.im = z.im / (2.0 * w);
	} else {
		result.im = (z.im >= 0) ? w : -w;
		result.re = z.im / (2.0 * result.im);
	}
	return result;
}

fcomplex fcomplex_rmul (float x, fcomplex a) {
	fcomplex result;
	result.re = x * a.re;
	result.im = x * a.im;
	return result;
}

dcomplex dcomplex_rmul (double x, dcomplex a) {
	dcomplex result;
	result.re = x * a.re;
	result.im = x * a.im;
	return result;
}

fcomplex fcomplex_exp (fcomplex z) {
	fcomplex result;
	double size = exp (z.re);
	result.re = size * cos (z.im);
	result.im = size * sin (z.im);
	return result;
}

dcomplex dcomplex_exp (dcomplex z) {
	dcomplex result;
	double size = exp (z.re);
	result.re = size * cos (z.im);
	result.im = size * sin (z.im);
	return result;
}

/* End of file complex.cpp */
