/* CCs_to_DTW.c
 *
 *	Dynamic Time Warp of two CCs.
 *
 * Copyright (C) 1993-2017 David Weenink
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

/*
 djmw 2001
 djmw 20020315 GPL header
 djmw 20080122 float -> double
 */

#include "CCs_to_DTW.h"

static void regression (CC me, integer frame, double r[], integer nr) {

	// sum(i^2;i=-n..n) = 2n^3/3 + n^2 +n/3 = n (n (2n/3 + 1) + 1/3);

	integer nrd2 = nr / 2;
	double sumsq = nrd2 * (nrd2 * (nr / 3.0 + 1.0) + 1.0 / 3.0);

	if (frame <= nrd2 || frame >= my nx - nrd2) {
		return;
	}

	for (integer i = 0; i <= my maximumNumberOfCoefficients; i ++) {
		r [i] = 0.0;
	}

	integer nmin = CC_getMinimumNumberOfCoefficients (me, frame - nrd2, frame + nrd2);

	for (integer i = 0; i <= nmin; i ++) {
		double ri = 0;
		for (integer j = -nrd2; j <= nrd2; j ++) {
			CC_Frame cf = & my frame[frame + j];
			double c = i == 0 ? cf -> c0 : cf -> c [i];
			ri += c * j;
		}
		r [i] = ri / sumsq / my dx;
	}
}

autoDTW CCs_to_DTW (CC me, CC thee, double wc, double wle, double wr, double wer, double dtr) {
	try {
		integer nr = Melder_ifloor (dtr / my dx);
		
		Melder_require (my maximumNumberOfCoefficients == thy maximumNumberOfCoefficients,
			U"CC orders should be equal.");
		Melder_require (! (wr != 0.0 && nr < 2), 
			U"Time window for regression is too small.");

		if (nr % 2 == 0) {
			nr ++;
		}
		if (wr != 0.0) {
			Melder_casual (nr, U" frames used for regression coefficients.");
		}

		autoDTW him = DTW_create (my xmin, my xmax, my nx, my dx, my x1, thy xmin, thy xmax, thy nx, thy dx, thy x1);
		autoNUMvector <double> ri ((integer) 0, my maximumNumberOfCoefficients);
		autoNUMvector <double> rj ((integer) 0, my maximumNumberOfCoefficients);

		/* Calculate distance matrix. */

		autoMelderProgress progess (U"CCs_to_DTW");
		for (integer i = 1; i <= my nx; i ++) {
			CC_Frame fi = & my frame [i];

			regression (me, i, ri.peek(), nr);

			for (integer j = 1; j <= thy nx; j ++) {
				CC_Frame fj = & thy frame [j];
				real80 dist = 0.0, distr = 0.0;

				/* Cepstral distance. */

				if (wc != 0.0) {
					for (integer k = 1; k <= fj -> numberOfCoefficients; k ++) {
						double d = fi -> c [k] - fj -> c [k];
						dist += d * d;
					}
					dist *= wc;
				}

				/* Log energy distance. */

				if (wle != 0.0) {
					double d = fi -> c0 - fj -> c0;
					dist += wle * d * d;
				}

				/* Regression distance. */

				if (wr != 0.0) {
					regression (thee, j, rj.peek(), nr);
					for (integer k = 1; k <= fj -> numberOfCoefficients; k ++) {
						double d = ri [k] - rj [k];
						distr += d * d;
					}
					dist += wr * distr;
				}

				/* Regression on c[0]: log(energy) */

				if (wer != 0.0) {
					if (wr == 0.0) {
						regression (thee, j, rj.peek(), nr);
					}
					double d = ri [0] - rj [0];
					dist += wer * d * d;
				}

				dist /= wc + wle + wr + wer;
				his z [i] [j] = sqrt ((real) dist);   // prototype along y-direction
			}

			if (i % 10 == 1) {
				Melder_progress (0.999 * i / my nx, U"Calculate distances: frame ", i, U" from ", my nx, U".");
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"DTW not created from CCs.");
	}
}

/* End of file CCs_to_DTW.cpp */
