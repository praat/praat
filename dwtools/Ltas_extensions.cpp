/* Ltas_extensions.cpp
 *
 * Copyright (C) 2012-2013 David Weenink
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

#include "Ltas_extensions.h"
#include "NUM2.h"

void Ltas_fitTiltLine (Ltas me, double fmin, double fmax, bool lnf, int method, double *a, double *b) {
	try {
		if (fmax <= fmin) {
			fmin = my xmin; fmax = my xmax;
		}
		long ifmin, ifmax, numberOfSamples = Sampled_getWindowSamples (me, fmin, fmax, &ifmin, &ifmax);
		if (numberOfSamples < 2) {
			Melder_throw ("There must be at least two data points to fit a line.");
		}
		autoNUMvector<double> x (1, numberOfSamples);
		autoNUMvector<double> y (1, numberOfSamples);
		for (long i = ifmin; i <= ifmax; i++) {
			long ixy = i - ifmin + 1;
			x[ixy] = my x1 + (i - 1) * my dx;
			if (lnf) {
				// For Ltas always x1 > 0
				x[ixy] = log10 (x[ixy]);
			}
			y[ixy] = my z[1][i];
		}
		NUMlineFit (x.peek(), y.peek(), numberOfSamples, a, b, method);
	} catch (MelderError) {
		Melder_throw ("Tilt line not determined.");
	}
}

/* End of file Ltas_extensions.cpp */
