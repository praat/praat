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

void sum_mean_sumsq_variance_stdev_scalar (numvec x, real *p_sum, real *p_mean, real *p_sumsq, real *p_variance, real *p_stdev) noexcept {
	if (x.size < 2) {
		if (x.size <= 0) {
			if (p_sum) *p_sum = 0.0;
			if (p_mean) *p_mean = undefined;
			if (p_sumsq) *p_sumsq = undefined;
		} else {
			if (p_sum) *p_sum = x [1];
			if (p_mean) *p_mean = x [1];
			if (p_sumsq) *p_sumsq = 0.0;
		}
		if (p_variance) *p_variance = undefined;
		if (p_stdev) *p_stdev = undefined;
		return;
	}
	if (Melder_debug == 48) {
		real sum = 0.0;   // -> sum in R (invariant)
		for (integer i = 1; i <= x.size; i ++) {
			sum += x [i];   // sum before in R, x [i] in R -> sum after in R
		}
		if (p_sum) *p_sum = sum;
		real mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
		if (p_mean) *p_mean = mean;
		if (! p_sumsq && ! p_variance && ! p_stdev) return;
		real sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
		for (integer i = 1; i <= x.size; i ++) {
			real residual = x [i] - mean;   // x [i] in R, mean in R -> residual in R
			real squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
			sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
		}
		if (p_sumsq) *p_sumsq = sumOfSquaredResiduals;
		integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
		real meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
		if (p_variance) *p_variance = (real) meanSquaredResidual;
		if (p_stdev) {
			real rootMeanSquaredResidual = sqrt (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
			*p_stdev = rootMeanSquaredResidual;
		}
	} else if (Melder_debug == 49) {
		#define REAL  real
		if (! p_sumsq && ! p_variance && ! p_stdev) {
			//real offset = x [1];
			const real offset = 0.0;
			long terms [65];
			REAL suma [65];
			terms [1] = 0;
			int top = 2;
			long n2 = x.size / 2;
			for (long i = 1; i <= n2; i ++) {
				/*
					Compute the sum of the next two data points.
					Put this sum on top of the stack.
				*/
				suma [top] = (REAL) ((x [2*i-1] - offset) + (x [2*i] - offset));
				terms [top] = 2;
				while (terms [top] == terms [top - 1]) {
					top --;
					terms [top] *= 2;
					suma [top] += suma [top + 1];
				}
				top ++;
			}
			top --;
			if (x.size & 1) {
				/*
					x.size is odd. Put the last point on the stack.
				*/
				top ++;
				suma [top] = (REAL) (x [x.size] - offset);
			}
			REAL sum = suma [top];
			/*
				x.size is not a power of 2; the stack contains more than one element.
				Add all the elements.
			*/
			for (long i = top - 1; i >= 2; i --) {
				sum += suma [i];
			}
			REAL mean = offset + sum / x.size;
			if (p_sum) {
				sum += offset * x.size;
				*p_sum = (real) sum;
			}
			if (p_mean) *p_mean = (real) mean;
			return;
		}
		int64 terms [65];
		REAL suma [65], sa [65];
		terms [1] = 0;
		int top = 2;
		long n2 = x.size / 2;
		for (long i = 1; i <= n2; i ++) {
			suma [top] = x [2*i-1] + x [2*i];
			REAL diff = x [2*i] - x [2*i-1];
			sa [top] = diff * diff / 2.0;
			terms [top] = 2;
			while (terms [top] == terms [top - 1]) {
				top --;
				terms [top] *= 2;
				diff = suma [top] - suma [top + 1];
				sa [top] += sa [top + 1] + diff * diff / terms [top];
				suma [top] += suma [top + 1];
			}
			top ++;
		}
		top --;
		if (x.size & 1) {
			top ++;
			terms [top] = 1;
			suma [top] = x [x.size];
			sa [top] = 0.0;
		}
		long t = terms [top];
		REAL sum = suma [top];
		REAL sumOfSquaredResiduals = sa [top];
		for (long i = top - 1; i >= 2; i --) {
			REAL diff = terms [i] * sum / t - suma [i];
			sumOfSquaredResiduals += sa [i] + t * diff * diff / ((REAL) terms [i] * (REAL) (terms [i] + t));
			sum += suma [i];
			t += terms [i];
		}
		REAL mean = sum / x.size;
		REAL variance = sumOfSquaredResiduals / (x.size - 1);
		if (p_sum) *p_sum = (real) sum;
		if (p_mean) *p_mean = (real) mean;
		if (p_sumsq) *p_sumsq = (real) sumOfSquaredResiduals;
		if (p_variance) *p_variance = (real) variance;
		if (p_stdev) *p_stdev = (real) sqrtl (variance);
		#undef REAL
	} else if (Melder_debug == 50) {
		#define REAL  real80
		#define SQRT  sqrtl
		REAL sum = 0.0;
		REAL correction = 0.0;
		for (integer i = 1; i <= x.size; i ++) {
			REAL value = (REAL) x [i] - correction;
			REAL newSum = sum + value;
			correction = (newSum - sum) - value;
			sum = newSum;
		}
		if (p_sum) *p_sum = (real) sum;
		REAL mean = sum / x.size;
		if (p_mean) *p_mean = (real) mean;
		if (! p_sumsq && ! p_variance && ! p_stdev) return;
		REAL sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
		for (integer i = 1; i <= x.size; i ++) {
			REAL residual = (REAL) x [i] - mean;   // x [i] in R, mean in R -> residual in R
			REAL squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
			sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
		}
		if (p_sumsq) *p_sumsq = (real) sumOfSquaredResiduals;
		integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
		REAL meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
		if (p_variance) *p_variance = (real) meanSquaredResidual;
		if (p_stdev) {
			REAL rootMeanSquaredResidual = SQRT (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
			*p_stdev = (real) rootMeanSquaredResidual;
		}
		#undef REAL
		#undef SQRT
	} else {
		real80 offset = (real80) x [1];   // x.size != 0 -> offset in R
		real80 sumOfDifferences = 0.0;   // sumOfDifferences in R (invariant)
		for (integer i = 2; i <= x.size; i ++) {
			sumOfDifferences += (real80) x [i] - offset;   // sumOfDifferences before in R, x [i] in R, offset in R -> sumOfDifferences after in R
		}
		if (p_sum) {
			real80 sum = sumOfDifferences + offset * x.size;
			*p_sum = (real) sum;
		}
		real80 mean = offset + sumOfDifferences / x.size;   // offset in R, sumOfDifferences in R, x.size != 0 -> mean in R
		if (p_mean) *p_mean = (real) mean;
		if (! p_sumsq && ! p_variance && ! p_stdev) return;
		real80 sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
		for (integer i = 1; i <= x.size; i ++) {
			real80 residual = (real80) x [i] - mean;   // x [i] in R, mean in R -> residual in R
			real80 squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
			sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
		}
		if (p_sumsq) *p_sumsq = (real) sumOfSquaredResiduals;
		integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
		real80 meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
		if (p_variance) *p_variance = (real) meanSquaredResidual;
		if (p_stdev) {
			real80 rootMeanSquaredResidual = sqrtl (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
			*p_stdev = (real) rootMeanSquaredResidual;
		}
	}
}

real sumsq_scalar (numvec x) noexcept {
	real sumsq;
	sum_mean_sumsq_variance_stdev_scalar (x, nullptr, nullptr, & sumsq, nullptr, nullptr);
	return sumsq;
}

real variance_scalar (numvec x) noexcept {
	real variance;
	sum_mean_sumsq_variance_stdev_scalar (x, nullptr, nullptr, nullptr, & variance, nullptr);
	return variance;
}

real stdev_scalar (numvec x) noexcept {
	real stdev;
	sum_mean_sumsq_variance_stdev_scalar (x, nullptr, nullptr, nullptr, nullptr, & stdev);
	return stdev;
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
