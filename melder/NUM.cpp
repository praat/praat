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

/*
	Local functions.
*/

static longdouble NUMsum_longdouble (constVECVU const& vec) {
	if (vec.stride == 1) {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, vec.size,
			const double *p = & vec [1],
			longdouble (*p),
			p += 1
		)
		return sum;
	} else {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, vec.size,
			const double *p = & vec [1],
			longdouble (*p),
			p += vec.stride
		)
		return sum;
	}
}

static longdouble NUMsum_longdouble (constMATVU const& mat) {
	if (mat.nrow <= mat.ncol) {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, mat.nrow,
			integer irow = 1,
			NUMsum_longdouble (mat [irow]),
			irow += 1
		)
		return sum;
	} else {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, mat.ncol,
			integer icol = 1,
			NUMsum_longdouble (mat.column (icol)),
			icol += 1
		)
		return sum;
	}
}

/*
	Global functions in alphabetic order.
*/

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

double NUMmean (constVECVU const& vec) noexcept {
	longdouble sum = NUMsum_longdouble (vec);
	return double (sum / vec.size);
}

double NUMmean (constMATVU const& mat) noexcept {
	longdouble sum = NUMsum_longdouble (mat);
	return double (sum / (mat.nrow * mat.ncol));
}

double NUMnorm (constVECVU const& x, double power) noexcept {
	if (power < 0.0) return undefined;
	if (power == 2.0) {
		PAIRWISE_SUM (longdouble, sum, integer, x.size,
			const double *px = & x [1],
			longdouble (*px) * longdouble (*px),
			px += x.stride
		)
		return sqrt (double (sum));
	} else if (power == 1.0) {
		PAIRWISE_SUM (longdouble, sum, integer, x.size,
			const double *px = & x [1],
			longdouble (fabs (*px)),
			px += x.stride
		)
		return double (sum);
	} else {
		PAIRWISE_SUM (longdouble, sum, integer, x.size,
			const double *px = & x [1],
			powl (longdouble (fabs (*px)), power),
			px += x.stride
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

double NUMsum (constVECVU const& vec) noexcept {
	longdouble sum = NUMsum_longdouble (vec);
	return double (sum);
}

double NUMsum (constMATVU const& mat) noexcept {
	longdouble sum = NUMsum_longdouble (mat);
	return double (sum);
}

static void NUM_sum_mean (constVECVU const& vec, double *out_sum, double *out_mean) noexcept {
	longdouble sum = NUMsum_longdouble (vec);
	if (out_sum) *out_sum = double (sum);
	if (out_mean) *out_mean = double (sum / vec.size);   // it helps a bit to perform the division while still in longdouble
}

void NUM_sum_mean_sumsq_variance_stdev (constVECVU const& x,
	double *out_sum, double *out_mean, double *out_sumsq, double *out_variance, double *out_stdev) noexcept
{
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
