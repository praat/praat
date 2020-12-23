#ifndef _complex_h_
#define _complex_h_
/* complex.h
 *
 * Copyright (C) 1992-2005,2011,2016-2018,2020 Paul Boersma
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

//struct dcomplex { double re, im; };
#include <complex>
using dcomplex = std::complex <double>;

inline dcomplex dcomplex_add (dcomplex a, dcomplex b) {
	dcomplex result;
	result. real (a.real() + b.real());
	result. imag (a.imag() + b.imag());
	return result;
}

inline dcomplex dcomplex_sub (dcomplex a, dcomplex b) {
	dcomplex result;
	result. real (a.real() - b.real());
	result. imag (a.imag() - b.imag());
	return result;
}

inline dcomplex dcomplex_mul (dcomplex a, dcomplex b) {
	dcomplex result;
	result. real (a.real() * b.real() - a.imag() * b.imag());
	result. imag (a.imag() * b.real() + a.real() * b.imag());
	return result;
}

inline dcomplex dcomplex_conjugate (dcomplex z) {
	dcomplex result;
	result. real (z.real());
	result. imag (- z.imag());
	return result;
}

inline dcomplex dcomplex_div (dcomplex a, dcomplex b) {
	dcomplex result;
	if (fabs (b.real()) >= fabs (b.imag())) {
		const double r = b.imag() / b.real();
		const double den = b.real() + r * b.imag();
		result. real ((a.real() + r * a.imag()) / den);
		result. imag ((a.imag() - r * a.real()) / den);
	} else {
		const double r = b.real() / b.imag();
		const double den = b.imag() + r * b.real();
		result. real ((a.real() * r + a.imag()) / den);
		result. imag ((a.imag() * r - a.real()) / den);
	}
	return result;
}

inline double dcomplex_abs (dcomplex z) {
	const double x = fabs (z.real());
	const double y = fabs (z.imag());
	if (x == 0.0) return y;
	if (y == 0.0) return x;
	if (x > y) {
		const double temp = y / x;
		return x * sqrt (1.0 + temp * temp);
	} else {
		const double temp = x / y;
		return y * sqrt (1.0 + temp * temp);
	}
}

inline dcomplex dcomplex_rmul (double x, dcomplex a) {
	dcomplex result;
	result. real (x * a.real());
	result. imag (x * a.imag());
	return result;
}

inline dcomplex dcomplex_exp (dcomplex z) {
	dcomplex result;
	double size = exp (z.real());
	result. real (size * cos (z.imag()));
	result. imag (size * sin (z.imag()));
	return result;
}

dcomplex dcomplex_sqrt (dcomplex z);

/* End of file complex.h */
#endif
