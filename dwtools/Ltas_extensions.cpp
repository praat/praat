/* Ltas_extensions.cpp
 *
 * Copyright (C) 2012-2017 David Weenink
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

void Ltas_fitTiltLine (Ltas me, double fmin, double fmax, bool lnf, int method, double *a, double *b) {
	try {
		if (fmax <= fmin) {
			fmin = my xmin; fmax = my xmax;
		}
		integer ifmin, ifmax, numberOfSamples = Sampled_getWindowSamples (me, fmin, fmax, & ifmin, & ifmax);
		Melder_require (numberOfSamples > 1, U"There should be at least two data points to fit a line.");
		autoNUMvector<double> x (1, numberOfSamples);
		autoNUMvector<double> y (1, numberOfSamples);
		for (integer i = ifmin; i <= ifmax; i ++) {
			integer ixy = i - ifmin + 1;
			x [ixy] = my x1 + (i - 1) * my dx;
			if (lnf) {
				// For Ltas always x1 > 0
				x [ixy] = log10 (x [ixy]);
			}
			y [ixy] = my z [1] [i];
		}
		NUMlineFit (x.peek(), y.peek(), numberOfSamples, a, b, method);
	} catch (MelderError) {
		Melder_throw (U"Tilt line not determined.");
	}
}

/* End of file Ltas_extensions.cpp */
