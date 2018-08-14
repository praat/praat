/* NUM.cpp
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

double NUMcenterOfGravity (VEC x) noexcept {
	double weightedSumOfIndexes = 0.0, sumOfWeights = 0.0;
	for (integer i = 1; i <= x.size; i ++) {
		weightedSumOfIndexes += i * x [i];
		sumOfWeights += x [i];
	}
	return weightedSumOfIndexes / sumOfWeights;
}

double NUMinner_ (VEC x, VEC y) {
	if (x.size != y.size) return undefined;
	PAIRWISE_SUM (longdouble, sum, integer, x.size,
		double *xx = x.at;
		double *yy = y.at,
		(++ xx, ++ yy),
		(longdouble) *xx * (longdouble) *yy);
	return (double) sum;
}

double NUMnorm (VEC x, double power) noexcept {
	if (power < 0.0) return undefined;
	if (power == 2.0) {
		PAIRWISE_SUM (longdouble, sum, integer, x.size, double *y = x.at, ++ y, (longdouble) *y * (longdouble) *y)
		return sqrt ((double) sum);
	} else if (power == 1.0) {
		PAIRWISE_SUM (longdouble, sum, integer, x.size, double *y = x.at, ++ y, (longdouble) fabs (*y))
		return (double) sum;
	} else {
		PAIRWISE_SUM (longdouble, sum, integer, x.size, double *y = x.at, ++ y, powl ((longdouble) fabs (*y), power))
		return (double) powl (sum, (longdouble) 1.0 / power);
	}
}

integer NUMnumberOfTokens (conststring32 string) {
	integer numberOfTokens = 0;
	const char32 *p = & string [0];
	for (;;) {
		Melder_skipHorizontalOrVerticalSpace (& p);
		if (*p == U'\0')
			break;
		numberOfTokens ++;
		p ++;   // step over first nonspace
		p = Melder_findEndOfInk (p);
	}
	return numberOfTokens;
}

double NUMstdev (VEC x) noexcept {
	double stdev;
	NUM_sum_mean_sumsq_variance_stdev (x, nullptr, nullptr, nullptr, nullptr, & stdev);
	return stdev;
}

void NUM_sum_mean (VEC x, double *p_sum, double *p_mean) noexcept {
	if (x.size <= 4) {
		switch (x.size) {
			case 0: {
				if (p_sum) *p_sum = 0.0;
				if (p_mean) *p_mean = undefined;
			} break; case 1: {
				if (p_sum) *p_sum = x [1];
				if (p_mean) *p_mean = x [1];
			} break; case 2: {
				longdouble sum = (longdouble) x [1] + (longdouble) x [2];
				if (p_sum) *p_sum = (double) sum;
				if (p_mean) *p_mean = double (0.5 * sum);
			} break; case 3: {
				longdouble sum = (longdouble) x [1] + (longdouble) x [2] + (longdouble) x [3];
				if (p_sum) *p_sum = (double) sum;
				if (p_mean) *p_mean = double ((1.0 / (longdouble) 3.0) * sum);
			} break; case 4: {
				longdouble sum = ((longdouble) x [1] + (longdouble) x [2]) + ((longdouble) x [3] + (longdouble) x [4]);
				if (p_sum) *p_sum = (double) sum;
				if (p_mean) *p_mean = double (0.25 * sum);
			} break; default: {
				if (p_sum) *p_sum = undefined;
				if (p_mean) *p_mean = undefined;
			}
		}
		return;
	}
	if (Melder_debug == 0 || Melder_debug < 48 || Melder_debug > 51) {
		PAIRWISE_SUM (longdouble, sum, integer, x.size, double *xx = x.at, ++ xx, (longdouble) *xx)
		if (p_sum) *p_sum = (double) sum;
		if (p_mean) *p_mean = double (sum / x.size);   // it helps a bit to perform the division while still in longdouble
	} else if (Melder_debug == 48) {
		SEQUENTIAL_SUM (double, sum, integer, x.size, double *xx = x.at, ++ xx, *xx)
		if (p_sum) *p_sum = (double) sum;
		if (p_mean) *p_mean = double (sum / x.size);
	} else if (Melder_debug == 49) {
		SEQUENTIAL_SUM (longdouble, sum, integer, x.size, double *xx = x.at, ++ xx, *xx)
		if (p_sum) *p_sum = (double) sum;
		if (p_mean) *p_mean = double (sum / x.size);
	} else if (Melder_debug == 50) {
		KAHAN_SUM (longdouble, sum, integer, x.size, double *xx = x.at, ++ xx, *xx)
		if (p_sum) *p_sum = (double) sum;
		if (p_mean) *p_mean = double (sum / x.size);
	} else if (Melder_debug == 51) {
		TWO_LOOP_SUM (longdouble, sum, integer, x.size, double *xx = x.at, ++ xx, *xx)
		if (p_sum) *p_sum = (double) sum;
		if (p_mean) *p_mean = double (sum / x.size);
	}
}

void NUM_sum_mean_sumsq_variance_stdev (VEC x, double *p_sum, double *p_mean, double *p_sumsq, double *p_variance, double *p_stdev) noexcept {
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
	if (Melder_debug != 0) {
		if (Melder_debug == 48) {
			/*
				Naive implementation in double.
			*/
			double sum = 0.0;   // -> sum in R (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				sum += x [i];   // sum before in R, x [i] in R -> sum after in R
			}
			if (p_sum) *p_sum = sum;
			double mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
			if (p_mean) *p_mean = mean;
			if (! p_sumsq && ! p_variance && ! p_stdev) return;
			double sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				double residual = x [i] - mean;   // x [i] in R, mean in R -> residual in R
				double squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
				sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
			}
			if (p_sumsq) *p_sumsq = sumOfSquaredResiduals;
			integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
			double meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
			if (p_variance) *p_variance = (double) meanSquaredResidual;
			if (p_stdev) {
				double rootMeanSquaredResidual = sqrt (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
				*p_stdev = rootMeanSquaredResidual;
			}
			return;
		}
		if (Melder_debug == 49) {
			/*
				Naive implementation in longdouble.
			*/
			longdouble sum = 0.0;   // -> sum in R (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				sum += (longdouble) x [i];   // sum before in R, x [i] in R -> sum after in R
			}
			if (p_sum) *p_sum = (double) sum;
			longdouble mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
			if (p_mean) *p_mean = (double) mean;
			if (! p_sumsq && ! p_variance && ! p_stdev) return;
			longdouble sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				longdouble residual = (longdouble) x [i] - mean;   // x [i] in R, mean in R -> residual in R
				longdouble squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
				sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
			}
			if (p_sumsq) *p_sumsq = (double) sumOfSquaredResiduals;
			integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
			longdouble meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
			if (p_variance) *p_variance = (double) meanSquaredResidual;
			if (p_stdev) {
				longdouble rootMeanSquaredResidual = sqrtl (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
				*p_stdev = (double) rootMeanSquaredResidual;
			}
			return;
		}
		if (Melder_debug == 50) {
			double mean;
			NUM_sum_mean (x, p_sum, & mean);
			if (p_mean) *p_mean = mean;
			if (! p_sumsq && ! p_variance && ! p_stdev) {
				return;
			}
			double *y;
			KAHAN_SUM (longdouble, sumsq, integer, x.size, y = x.at, ++ y, longdouble (*y - mean) * longdouble (*y - mean))
			double variance = double (sumsq / (x.size - 1));
			if (p_sumsq) *p_sumsq = (double) sumsq;
			if (p_variance) *p_variance = variance;
			if (p_stdev) *p_stdev = sqrt (variance);
			return;
		}
		if (Melder_debug == 51) {
			double sum, mean;
			NUM_sum_mean (x, & sum, & mean);
			if (p_sum) *p_sum = sum;
			if (p_mean) *p_mean = mean;
			if (! p_sumsq && ! p_variance && ! p_stdev) return;
			double sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				double residual = x [i] - mean;   // x [i] in R, mean in R -> residual in R
				double squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
				sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
			}
			if (p_sumsq) *p_sumsq = sumOfSquaredResiduals;
			integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
			double meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
			if (p_variance) *p_variance = (double) meanSquaredResidual;
			if (p_stdev) {
				double rootMeanSquaredResidual = sqrt (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
				*p_stdev = rootMeanSquaredResidual;
			}
			return;
		}
	}
	/*
		Our standard: pairwise algorithm with base case 64.
	*/
	PAIRWISE_SUM (longdouble, sum, integer, x.size, double *xx = x.at, ++ xx, (longdouble) *xx)
	double mean = double (sum / x.size);   // rounded to double, because this guarantees that x[i] - mean will be zero for constant x[1..size]
	if (p_sum) *p_sum = (double) sum;
	if (p_mean) *p_mean = (double) mean;
	if (! p_sumsq && ! p_variance && ! p_stdev) return;
	PAIRWISE_SUM (longdouble, sumsq, integer, x.size, double *xx = x.at, ++ xx, longdouble (*xx - mean) * longdouble (*xx - mean))
	longdouble variance = sumsq / (x.size - 1);
	if (p_sumsq) *p_sumsq = (double) sumsq;
	if (p_variance) *p_variance = (double) variance;
	if (p_stdev) *p_stdev = sqrt ((double) variance);
}

void NUM_sum_mean_sumsq_variance_stdev (MAT x, integer columnNumber, double *p_sum, double *p_mean, double *p_sumsq, double *p_variance, double *p_stdev) noexcept {
	if (x.nrow < 2) {
		if (x.nrow <= 0) {
			if (p_sum) *p_sum = 0.0;
			if (p_mean) *p_mean = undefined;
			if (p_sumsq) *p_sumsq = undefined;
		} else {
			if (p_sum) *p_sum = x [1] [columnNumber];
			if (p_mean) *p_mean = x [1] [columnNumber];
			if (p_sumsq) *p_sumsq = 0.0;
		}
		if (p_variance) *p_variance = undefined;
		if (p_stdev) *p_stdev = undefined;
		return;
	}
	PAIRWISE_SUM (longdouble, sum, integer, x.nrow, double *xx = & x.at [1] [columnNumber] - x.ncol, xx += x.ncol, (longdouble) *xx)
	double mean = double (sum / x.nrow);   // rounded to double, because this guarantees that x[i] - mean will be zero for constant x[1..size]
	if (p_sum) *p_sum = (double) sum;
	if (p_mean) *p_mean = (double) mean;
	if (! p_sumsq && ! p_variance && ! p_stdev) return;
	PAIRWISE_SUM (longdouble, sumsq, integer, x.nrow, double *xx = & x.at [1] [columnNumber] - x.ncol, xx += x.ncol, longdouble (*xx - mean) * longdouble (*xx - mean))
	longdouble variance = sumsq / (x.nrow - 1);
	if (p_sumsq) *p_sumsq = (double) sumsq;
	if (p_variance) *p_variance = (double) variance;
	if (p_stdev) *p_stdev = sqrt ((double) variance);
}

double NUMsumsq (VEC x) noexcept {
	double sumsq;
	NUM_sum_mean_sumsq_variance_stdev (x, nullptr, nullptr, & sumsq, nullptr, nullptr);
	return sumsq;
}

double NUMvariance (VEC x) noexcept {
	double variance;
	NUM_sum_mean_sumsq_variance_stdev (x, nullptr, nullptr, nullptr, & variance, nullptr);
	return variance;
}

/* End of file NUM.cpp */
