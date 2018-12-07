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

double NUMcenterOfGravity (constVEC const& x) noexcept {
	longdouble weightedSumOfIndexes = 0.0, sumOfWeights = 0.0;
	for (integer i = 1; i <= x.size; i ++) {
		weightedSumOfIndexes += i * x [i];
		sumOfWeights += x [i];
	}
	return double (weightedSumOfIndexes / sumOfWeights);
}

double NUMinner_ (constVECVU const& x, constVECVU const& y) noexcept {
	if (x.stride == 1) {
		if (y.stride == 1) {
			PAIRWISE_SUM (longdouble, sum, integer, x.size,
				const double *px = & x [1];
				const double *py = & y [1],
				longdouble (*px) * longdouble (*py),
				(px += 1, py += 1)
			)
			return double (sum);
		} else {
			PAIRWISE_SUM (longdouble, sum, integer, x.size,
				const double *px = & x [1];
				const double *py = & y [1],
				longdouble (*px) * longdouble (*py),
				(px += 1, py += y.stride)
			)
			return double (sum);
		}
	} else if (y.stride == 1) {
		PAIRWISE_SUM (longdouble, sum, integer, x.size,
			const double *px = & x [1];
			const double *py = & y [1],
			longdouble (*px) * longdouble (*py),
			(px += x.stride, py += 1)
		)
		return double (sum);
	} else {
		PAIRWISE_SUM (longdouble, sum, integer, x.size,
			const double *px = & x [1];
			const double *py = & y [1],
			longdouble (*px) * longdouble (*py),
			(px += x.stride, py += y.stride)
		)
		return double (sum);
	}
}

double NUMnorm (constVEC const& x, double power) noexcept {
	if (power < 0.0) return undefined;
	if (power == 2.0) {
		PAIRWISE_SUM (longdouble, sum, integer, x.size,
			const double *px = & x [1],
			longdouble (*px) * longdouble (*px),
			px += 1
		)
		return sqrt (double (sum));
	} else if (power == 1.0) {
		PAIRWISE_SUM (longdouble, sum, integer, x.size,
			const double *px = & x [1],
			longdouble (fabs (*px)),
			px += 1
		)
		return double (sum);
	} else {
		PAIRWISE_SUM (longdouble, sum, integer, x.size,
			const double *px = & x [1],
			powl (longdouble (fabs (*px)), power),
			px += 1
		)
		return double (powl (sum, longdouble (1.0) / power));
	}
}

integer NUMnumberOfTokens (conststring32 string) noexcept {
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

double NUMstdev (constVECVU const& x) noexcept {
	double stdev;
	NUM_sum_mean_sumsq_variance_stdev (x, nullptr, nullptr, nullptr, nullptr, & stdev);
	return stdev;
}

void NUM_sum_mean (constVECVU const& x, double *out_sum, double *out_mean) noexcept {
	if (x.size <= 4) {
		switch (x.size) {
			case 0: {
				if (out_sum) *out_sum = 0.0;
				if (out_mean) *out_mean = undefined;
			} break; case 1: {
				if (out_sum) *out_sum = x [1];
				if (out_mean) *out_mean = x [1];
			} break; case 2: {
				const longdouble sum = longdouble (x [1]) + longdouble (x [2]);
				if (out_sum) *out_sum = double (sum);
				if (out_mean) *out_mean = double (0.5 * sum);
			} break; case 3: {
				const longdouble sum = longdouble (x [1]) + longdouble (x [2]) + longdouble (x [3]);
				if (out_sum) *out_sum = double (sum);
				if (out_mean) *out_mean = double ((1.0 / longdouble (3.0)) * sum);
			} break; case 4: {
				const longdouble sum = (longdouble (x [1]) + longdouble (x [2])) + (longdouble (x [3]) + longdouble (x [4]));
				if (out_sum) *out_sum = double (sum);
				if (out_mean) *out_mean = double (0.25 * sum);
			} break; default: {
				if (out_sum) *out_sum = undefined;
				if (out_mean) *out_mean = undefined;
			}
		}
		return;
	}
	if (Melder_debug == 0 || Melder_debug < 48 || Melder_debug > 51) {
		if (x.stride == 1) {
			PAIRWISE_SUM (
				longdouble, sum,
				integer, x.size,
				const double *px = & x [1],
				longdouble (*px),
				px += 1
			)
			if (out_sum) *out_sum = double (sum);
			if (out_mean) *out_mean = double (sum / x.size);   // it helps a bit to perform the division while still in longdouble
		} else {
			PAIRWISE_SUM (
				longdouble, sum,
				integer, x.size,
				const double *px = & x [1],
				longdouble (*px),
				px += x.stride
			)
			if (out_sum) *out_sum = double (sum);
			if (out_mean) *out_mean = double (sum / x.size);   // it helps a bit to perform the division while still in longdouble
		}
	} else if (Melder_debug == 48) {
		SEQUENTIAL_SUM (double, sum, integer, x.size, const double *px = & x [1], *px, px += x.stride)
		if (out_sum) *out_sum = double (sum);
		if (out_mean) *out_mean = double (sum / x.size);
	} else if (Melder_debug == 49) {
		SEQUENTIAL_SUM (longdouble, sum, integer, x.size, const double *px = & x [1], *px, px += x.stride)
		if (out_sum) *out_sum = double (sum);
		if (out_mean) *out_mean = double (sum / x.size);
	} else if (Melder_debug == 50) {
		KAHAN_SUM (longdouble, sum, integer, x.size, const double *px = & x [1], *px, px += x.stride)
		if (out_sum) *out_sum = double (sum);
		if (out_mean) *out_mean = double (sum / x.size);
	} else if (Melder_debug == 51) {
		TWO_LOOP_SUM (longdouble, sum, integer, x.size, const double *px = & x [1], *px, px += x.stride)
		if (out_sum) *out_sum = double (sum);
		if (out_mean) *out_mean = double (sum / x.size);
	}
}

void NUM_sum_mean_sumsq_variance_stdev (constVECVU const& x,
	double *out_sum, double *out_mean, double *out_sumsq, double *out_variance, double *out_stdev) noexcept
{
	if (x.size < 2) {
		if (x.size <= 0) {
			if (out_sum) *out_sum = 0.0;
			if (out_mean) *out_mean = undefined;
			if (out_sumsq) *out_sumsq = undefined;
		} else {
			if (out_sum) *out_sum = x [1];
			if (out_mean) *out_mean = x [1];
			if (out_sumsq) *out_sumsq = 0.0;
		}
		if (out_variance) *out_variance = undefined;
		if (out_stdev) *out_stdev = undefined;
		return;
	}
	if (Melder_debug != 0) {
		if (Melder_debug == 48) {
			/*
				Naive implementation in double.
			*/
			double sum = 0.0;   // -> sum in R (invariant)   [R is the set of real numbers]
			for (integer i = 1; i <= x.size; i ++)
				sum += x [i];   // sum before in R, x [i] in R -> sum after in R
			if (out_sum) *out_sum = sum;
			const double mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
			if (out_mean) *out_mean = mean;
			if (! out_sumsq && ! out_variance && ! out_stdev) return;
			double sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				const double residual = x [i] - mean;   // x [i] in R, mean in R -> residual in R
				const double squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
				sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
			}
			if (out_sumsq) *out_sumsq = sumOfSquaredResiduals;
			const integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
			const double meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
			if (out_variance) *out_variance = double (meanSquaredResidual);
			if (out_stdev) {
				const double rootMeanSquaredResidual = sqrt (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
				*out_stdev = rootMeanSquaredResidual;
			}
			return;
		}
		if (Melder_debug == 49) {
			/*
				Naive implementation in longdouble.
			*/
			longdouble sum = 0.0;   // -> sum in R (invariant)
			for (integer i = 1; i <= x.size; i ++)
				sum += longdouble (x [i]);   // sum before in R, x [i] in R -> sum after in R
			if (out_sum) *out_sum = double (sum);
			const longdouble mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
			if (out_mean) *out_mean = double (mean);
			if (! out_sumsq && ! out_variance && ! out_stdev) return;
			longdouble sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				const longdouble residual = longdouble (x [i]) - mean;   // x [i] in R, mean in R -> residual in R
				const longdouble squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
				sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
			}
			if (out_sumsq) *out_sumsq = double (sumOfSquaredResiduals);
			const integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
			const longdouble meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
			if (out_variance) *out_variance = (double) meanSquaredResidual;
			if (out_stdev) {
				const longdouble rootMeanSquaredResidual = sqrtl (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
				*out_stdev = double (rootMeanSquaredResidual);
			}
			return;
		}
		if (Melder_debug == 50) {
			double mean;
			NUM_sum_mean (x, out_sum, & mean);
			if (out_mean) *out_mean = mean;
			if (! out_sumsq && ! out_variance && ! out_stdev)
				return;
			KAHAN_SUM (longdouble, sumsq, integer, x.size,
					const double *px = & x [1],
					longdouble (*px - mean) * longdouble (*px - mean),
					px += x.stride)
			const double variance = double (sumsq / (x.size - 1));
			if (out_sumsq) *out_sumsq = (double) sumsq;
			if (out_variance) *out_variance = variance;
			if (out_stdev) *out_stdev = sqrt (variance);
			return;
		}
		if (Melder_debug == 51) {
			double sum, mean;
			NUM_sum_mean (x, & sum, & mean);
			if (out_sum) *out_sum = sum;
			if (out_mean) *out_mean = mean;
			if (! out_sumsq && ! out_variance && ! out_stdev) return;
			double sumOfSquaredResiduals = 0.0;   // -> sumOfSquares >= 0.0 (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				const double residual = x [i] - mean;   // x [i] in R, mean in R -> residual in R
				const double squaredResidual = residual * residual;   // residual in R -> squaredResidual >= 0.0
				sumOfSquaredResiduals += squaredResidual;   // sumOfSquaredResiduals before >= 0.0, squaredResidual >= 0.0 -> sumOfSquaredResiduals after >= 0.0
			}
			if (out_sumsq) *out_sumsq = sumOfSquaredResiduals;
			const integer degreesOfFreedom = x.size - 1;   // x.size >= 2 -> degreesOfFreedom >= 1 -> degreesOfFreedom > 0
			const double meanSquaredResidual = sumOfSquaredResiduals / degreesOfFreedom;   // sumOfSquaredResiduals >= 0.0, degreesOfFreedom > 0 -> meanSquaredResidual >= 0.0
			if (out_variance) *out_variance = (double) meanSquaredResidual;
			if (out_stdev) {
				const double rootMeanSquaredResidual = sqrt (meanSquaredResidual);   // meanSquaredResidual >= 0.0 -> rootMeanSquaredResidual >= 0.0 (in particular, not NaN)
				*out_stdev = rootMeanSquaredResidual;
			}
			return;
		}
	}
	/*
		Our standard: pairwise algorithm with base case 64.
	*/
	double sum, mean;
	NUM_sum_mean (x, & sum, & mean);
	if (out_sum) *out_sum = sum;
	if (out_mean) *out_mean = mean;
	if (! out_sumsq && ! out_variance && ! out_stdev) return;
	if (x.stride == 1) {
		PAIRWISE_SUM (longdouble, sumsq, integer, x.size,
			const double *px = & x [1],
			longdouble (*px - mean) * longdouble (*px - mean),
			px += 1
		)
		const longdouble variance = sumsq / (x.size - 1);
		if (out_sumsq) *out_sumsq = double (sumsq);
		if (out_variance) *out_variance = double (variance);
		if (out_stdev) *out_stdev = sqrt (double (variance));
	} else {
		PAIRWISE_SUM (longdouble, sumsq, integer, x.size,
			const double *px = & x [1],
			longdouble (*px - mean) * longdouble (*px - mean),
			px += x.stride
		)
		const longdouble variance = sumsq / (x.size - 1);
		if (out_sumsq) *out_sumsq = double (sumsq);
		if (out_variance) *out_variance = double (variance);
		if (out_stdev) *out_stdev = sqrt (double (variance));
	}
}

double NUMsumsq (const constVECVU& x) noexcept {
	double sumsq;
	NUM_sum_mean_sumsq_variance_stdev (x, nullptr, nullptr, & sumsq, nullptr, nullptr);
	return sumsq;
}

double NUMvariance (const constVECVU& x) noexcept {
	double variance;
	NUM_sum_mean_sumsq_variance_stdev (x, nullptr, nullptr, nullptr, & variance, nullptr);
	return variance;
}

/* End of file NUM.cpp */
