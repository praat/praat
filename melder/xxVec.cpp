/* xxVec.cpp
 *
 * Copyright (C) 2017,2018 Paul Boersma
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

#include "melder.h"
#include "../dwsys/NUM2.h"   /* for NUMsort2 */
#include "PAIRWISE_SUM.h"

autonumvec copy_numvec (numvec x) {
	autonumvec result (x.size, kTensorInitializationType::RAW);
	for (integer i = 1; i <= x.size; i ++) {
		result [i] = x [i];
	}
	return result;
}

autonummat copy_nummat (nummat x) {
	autonummat result (x.nrow, x.ncol, kTensorInitializationType::RAW);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++) {
			result [irow] [icol] = x [irow] [icol];
		}
	}
	return result;
}

autonummat outer_nummat (numvec x, numvec y) {
	autonummat result (x.size, y.size, kTensorInitializationType::RAW);
	for (integer irow = 1; irow <= x.size; irow ++) {
		for (integer icol = 1; icol <= y.size; icol ++) {
			result [irow] [icol] = x [irow] * y [icol];
		}
	}
	return result;
}

autonummat peaks_nummat (numvec x, bool includeEdges, int interpolate, bool sortByHeight) {
	if (x.size < 2) {
		includeEdges = false;
	}
	integer numberOfPeaks = 0;
	for (integer i = 2; i < x.size; i ++) {
		if (x [i] > x [i - 1] && x [i] >= x [i + 1]) {
			numberOfPeaks ++;
		}
	}
	if (includeEdges) {
		if (x [1] > x [2]) numberOfPeaks ++;
		if (x [x.size] > x [x.size - 1]) numberOfPeaks ++;
	}
	autonummat result (2, numberOfPeaks, kTensorInitializationType::RAW);
	integer peakNumber = 0;
	if (includeEdges && x [1] > x [2]) {
		result [1] [++ peakNumber] = 1;
		result [2] [peakNumber] = x [1];
	}
	for (integer i = 2; i < x.size; i ++) {
		if (x [i] > x [i - 1] && x [i] >= x [i + 1]) {
			++ peakNumber;
			if (interpolate != 0) {   // this is not a boolean; there could follow more options
				/*
					Parabolic interpolation.
				*/
				double dy = 0.5 * (x [i + 1] - x [i - 1]);
				double d2y = (x [i] - x [i - 1]) + (x [i] - x [i + 1]);
				Melder_assert (d2y > 0.0);
				result [1] [peakNumber] = (double) i + dy / d2y;
				result [2] [peakNumber] = x [i] + 0.5 * dy * (dy / d2y);
			} else {
				/*
					Don't interpolate: choose the nearest index.
				*/
				result [1] [peakNumber] = i;
				result [2] [peakNumber] = x [i];
			}
		}
	}
	if (includeEdges && x [x.size] > x [x.size - 1]) {
		result [1] [++ peakNumber] = x.size;
		result [2] [peakNumber] = x [x.size];
	}
	Melder_assert (peakNumber == numberOfPeaks);
	if (sortByHeight) {
		for (integer i = 1; i <= numberOfPeaks; i ++)
			result [2] [i] *= -1.0;
		NUMsort2 (result.ncol, result [2], result [1]);
		for (integer i = 1; i <= numberOfPeaks; i ++)
			result [2] [i] *= -1.0;
	}
	return result;
}

inline static double NUMinner_stride_ (numvec x, numvec y, integer stride) {
	if (x.size != y.size) return undefined;
	PAIRWISE_SUM (longdouble, sum, integer, x.size,
		double *xx = & x [0];
		double *yy = & y [1 - stride],
		(++ xx, yy += stride),   // this goes way beyond the confines of y
		(longdouble) *xx * (longdouble) *yy)
	return (double) sum;
}

inline static void mul_inplace (numvec target, numvec vec, nummat mat) {
	for (integer j = 1; j <= mat.ncol; j ++) {
		if ((false)) {
			target [j] = 0.0;
			for (integer i = 1; i <= mat.nrow; i ++) {
				target [j] += vec [i] * mat [i] [j];
			}
		} else {
			target [j] = NUMinner_stride_ (vec, { & mat [1] [j] - 1, mat.nrow }, mat.ncol);
		}
	}
}

inline static void mul_inplace (numvec target, nummat mat, numvec vec) {
	for (integer i = 1; i <= mat.nrow; i ++) {
		if ((false)) {
			target [i] = 0.0;
			for (integer j = 1; j <= vec.size; j ++) {
				target [i] += mat [i] [j] * vec [j];
			}
		} else {
			target [i] = NUMinner ({ & mat [i] [1] - 1, mat.ncol }, vec);
		}
	}
}

autonumvec mul_numvec (numvec vec, nummat mat) {
	if (mat.nrow != vec.size) return autonumvec { };
	autonumvec result { mat.ncol, kTensorInitializationType::RAW };
	mul_inplace (result.get(), vec, mat);
	return result;
}

autonumvec mul_numvec (nummat mat, numvec vec) {
	if (vec.size != mat.ncol) return autonumvec { };
	autonumvec result { mat.nrow, kTensorInitializationType::RAW };
	mul_inplace (result.get(), mat, vec);
	return result;
}

void numvec_sort (numvec x) {
	NUMsort_d (x.size, x.at);
}

/* End of file xxVec.cpp */
