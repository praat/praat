/* complex.cpp
 *
 * Copyright (C) 1992-2011,2017 Paul Boersma
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

#include <math.h>
#include "complex.h"

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

/* End of file complex.cpp */
