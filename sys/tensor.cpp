/* tensor.cpp
 *
 * Copyright (C) 2017 Paul Boersma
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

#include "tensor.h"
#include "NUM2.h"

void numvec :: _initAt (long givenSize, bool zero) {
	Melder_assert (givenSize >= 0);
	try {
		our at = ( givenSize == 0 ? nullptr : NUMvector <double> (1, givenSize, zero) );
	} catch (MelderError) {
		Melder_throw (U"Numeric vector not created.");
	}
}

void numvec :: _freeAt () noexcept {
	if (our at) NUMvector_free (our at, 1);
}

void nummat :: _initAt (long givenNrow, long givenNcol, bool zero) {
	Melder_assert (givenNrow >= 0);
	Melder_assert (givenNcol >= 0);
	try {
		our at = ( givenNrow > 0 && givenNcol > 0 ? NUMmatrix <double> (1, givenNrow, 1, givenNcol, zero) : nullptr );
	} catch (MelderError) {
		Melder_throw (U"Numeric matrix not created.");
	}
}

void nummat :: _freeAt () noexcept {
	if (our at) NUMmatrix_free (our at, 1, 1);
}

void mean_variance_stdev_scalar (numvec x, real *p_mean, real *p_variance, real *p_stdev) noexcept {
	if (x.size < 2) {
		if (p_mean) *p_mean = x.size == 1 ? x [1] : undefined;
		if (p_stdev) *p_stdev = undefined;
		return;
	}
	if (Melder_debug == 48) {
		real sum = 0.0;   // -> sum in R (invariant)
		for (integer i = 1; i <= x.size; i ++) {
			sum += x [i];   // sum before in R, x [i] in R -> sum after in R
		}
		real mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
		if (p_mean) *p_mean = mean;
		if (! p_variance && ! p_stdev) return;
		real sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
		for (integer i = 1; i <= x.size; i ++) {
			real residual = x [i] - mean;   // x [i] in R, mean in R -> residual in R
			real squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
			sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
		}
		integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
		real meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
		if (p_variance) *p_variance = (real) meanSquaredResidual;
		if (p_stdev) {
			real rootMeanSquaredResidual = sqrt (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
			*p_stdev = rootMeanSquaredResidual;
		}
	} else if (Melder_debug == 49) {
		real mean, sumOfSquaredResiduals;
		NUMmeanAndVariance (x.at, x.size, & mean, & sumOfSquaredResiduals);
		real variance = sumOfSquaredResiduals / (x.size - 1);
		if (p_mean) *p_mean = mean;
		if (p_variance) *p_variance = variance;
		if (p_stdev) *p_stdev = sqrt (variance);
	} else {
		real80 sum = 0.0;   // -> sum in R (invariant)
		for (integer i = 1; i <= x.size; i ++) {
			sum += (real80) x [i];   // sum before in R, x [i] in R -> sum after in R
		}
		real80 mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
		if (p_mean) *p_mean = (real) mean;
		if (! p_variance && ! p_stdev) return;
		real80 sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
		for (integer i = 1; i <= x.size; i ++) {
			real80 residual = (real80) x [i] - mean;   // x [i] in R, mean in R -> residual in R
			real80 squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
			sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
		}
		integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
		real80 meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
		if (p_variance) *p_variance = (real) meanSquaredResidual;
		if (p_stdev) {
			real80 rootMeanSquaredResidual = sqrtl (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
			*p_stdev = (real) rootMeanSquaredResidual;
		}
	}
}

real stdev_scalar (numvec x) noexcept {
	if (x.size < 2) return undefined;   // -> from here on, x.size >= 2 -> x.size != 0
	if (Melder_debug == 48) {
		real sum = 0.0;   // -> sum in R (invariant)
		for (integer i = 1; i <= x.size; i ++) {
			sum += x [i];   // sum before in R, x [i] in R -> sum after in R
		}
		real mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
		real sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
		for (integer i = 1; i <= x.size; i ++) {
			real residual = x [i] - mean;   // x [i] in R, mean in R -> residual in R
			real squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
			sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
		}
		integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
		real meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
		real rootMeanSquaredResidual = sqrt (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
		return rootMeanSquaredResidual;
	} else if (Melder_debug == 49) {
		real mean, variance;
		NUMmeanAndVariance (x.at, x.size, & mean, & variance);
		return sqrt (variance / (x.size - 1));
	} else {
		real80 sum = 0.0;   // -> sum in R (invariant)
		for (integer i = 1; i <= x.size; i ++) {
			sum += (real80) x [i];   // sum before in R, x [i] in R -> sum after in R
		}
		real80 mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
		real80 sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
		for (integer i = 1; i <= x.size; i ++) {
			real80 residual = (real80) x [i] - mean;   // x [i] in R, mean in R -> residual in R
			real80 squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
			sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
		}
		integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
		real80 meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
		real80 rootMeanSquaredResidual = sqrtl (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
		return (real) rootMeanSquaredResidual;
	}
}

double center_scalar (numvec x) noexcept {
	double weightedSumOfIndexes = 0.0, sumOfWeights = 0.0;
	for (long i = 1; i <= x.size; i ++) {
		weightedSumOfIndexes += i * x [i];
		sumOfWeights += x [i];
	}
	return weightedSumOfIndexes / sumOfWeights;
}

autonumvec copy_numvec (numvec x) {
	autonumvec result (x.size, false);
	for (long i = 1; i <= x.size; i ++) {
		result [i] = x [i];
	}
	return result;
}

autonummat copy_nummat (nummat x) {
	autonummat result (x.nrow, x.ncol, false);
	for (long irow = 1; irow <= x.nrow; irow ++) {
		for (long icol = 1; icol <= x.ncol; icol ++) {
			result [irow] [icol] = x [irow] [icol];
		}
	}
	return result;
}

autonummat outer_nummat (numvec x, numvec y) {
	autonummat result (x.size, y.size, false);
	for (long irow = 1; irow <= x.size; irow ++) {
		for (long icol = 1; icol <= y.size; icol ++) {
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
	autonummat result (2, numberOfPeaks, false);
	integer peakNumber = 0;
	if (includeEdges && x [1] > x [2]) {
		result [1] [++ peakNumber] = 1;
		result [2] [peakNumber] = x [1];
	}
	for (integer i = 2; i < x.size; i ++) {
		if (x [i] > x [i - 1] && x [i] >= x [i + 1]) {
			++ peakNumber;
			if (interpolate) {
				real dy = 0.5 * (x [i + 1] - x [i - 1]);
				real d2y = (x [i] - x [i - 1]) + (x [i] - x [i + 1]);
				Melder_assert (d2y > 0.0);
				result [1] [peakNumber] = (real) i + dy / d2y;
				result [2] [peakNumber] = x [i] + 0.5 * dy * (dy / d2y);
			} else {
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

/* End of file tensor.cpp */
