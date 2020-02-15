/* complex.cpp
 *
 * Copyright (C) 1992-2005,2011,2016-2018,2020 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
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

#include "melder.h"

dcomplex dcomplex_sqrt (dcomplex z) {
	dcomplex result;
	double x, y, w, r;
	if (z.real() == 0 && z.imag() == 0) {
		result. real (0.0);
		result. imag (0.0);
		return result;
	}
	x = fabs (z.real());
	y = fabs (z.imag());
	if (x >= y) {
		r = y / x;
		w = sqrt (x) * sqrt (0.5 * (1.0 + sqrt (1.0 + r * r)));
	} else {
		r = x / y;
		w = sqrt (y) * sqrt (0.5 * (r + sqrt (1.0 + r * r)));
	}
	if (z.real() >= 0.0) {
		result. real (w);
		result. imag (z.imag() / (2.0 * w));
	} else {
		result. imag ((z.imag() >= 0) ? w : -w);
		result. real (z.imag() / (2.0 * result.imag()));
	}
	return result;
}

/* End of file complex.cpp */
