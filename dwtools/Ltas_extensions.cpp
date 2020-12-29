/* Ltas_extensions.cpp
 *
 * Copyright (C) 2012-2019 David Weenink
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

#include "Ltas_extensions.h"
#include "NUM2.h"

void Ltas_fitTrendLine (Ltas me, double fmin, double fmax, bool lnf, int method, double *a, double *b) {
	try {
		if (fmax <= fmin) {
			fmin = my xmin;
			fmax = my xmax;
		}
		integer ifmin, ifmax;
		const integer numberOfSamples = Sampled_getWindowSamples (me, fmin, fmax, & ifmin, & ifmax);
		Melder_require (numberOfSamples > 1,
			U"There should be at least two data points to fit a line.");
		autoVEC x = raw_VEC (numberOfSamples);
		autoVEC y = raw_VEC (numberOfSamples);
		for (integer i = ifmin; i <= ifmax; i ++) {
			const integer ixy = i - ifmin + 1;
			x [ixy] = my x1 + (i - 1) * my dx;
			if (lnf)
				x [ixy] = log10 (x [ixy]); // For Ltas always x1 > 0
			y [ixy] = my z [1] [i];
		}
		NUMlineFit (x.get(), y.get(), a, b, method);
	} catch (MelderError) {
		Melder_throw (U"Tilt line not determined.");
	}
}

/* End of file Ltas_extensions.cpp */
