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
static longdouble NUMsumOfSquaredDifferences_longdouble (constVECVU const& vec, double mean) {
	if (vec.stride == 1) {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, vec.size,
			const double *p = & vec [1],
			longdouble (*p - mean) * longdouble (*p - mean),
			p += 1
		)
		return sum;
	} else {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, vec.size,
			const double *p = & vec [1],
			longdouble (*p - mean) * longdouble (*p - mean),
			p += vec.stride
		)
		return sum;
	}
}
static longdouble NUMsum2_longdouble (constVECVU const& vec) {
	if (vec.stride == 1) {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, vec.size,
			const double *p = & vec [1],
			longdouble (*p) * longdouble (*p),
			p += 1
		)
		return sum;
	} else {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, vec.size,
			const double *p = & vec [1],
			longdouble (*p) * longdouble (*p),
			p += vec.stride
		)
		return sum;
	}
}
static longdouble NUMsum2_longdouble (constMATVU const& mat) {
	if (mat.nrow <= mat.ncol) {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, mat.nrow,
			integer irow = 1,
			NUMsum2_longdouble (mat [irow]),
			irow += 1
		)
		return sum;
	} else {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, mat.ncol,
			integer icol = 1,
			NUMsum2_longdouble (mat.column (icol)),
			icol += 1
		)
		return sum;
	}
}
static longdouble NUMsumAbs_longdouble (constVECVU const& vec) {
	if (vec.stride == 1) {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, vec.size,
			const double *p = & vec [1],
			longdouble (fabs (*p)),
			p += 1
		)
		return sum;
	} else {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, vec.size,
			const double *p = & vec [1],
			longdouble (fabs (*p)),
			p += vec.stride
		)
		return sum;
	}
}
static longdouble NUMsumAbs_longdouble (constMATVU const& mat) {
	if (mat.nrow <= mat.ncol) {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, mat.nrow,
			integer irow = 1,
			NUMsumAbs_longdouble (mat [irow]),
			irow += 1
		)
		return sum;
	} else {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, mat.ncol,
			integer icol = 1,
			NUMsumAbs_longdouble (mat.column (icol)),
			icol += 1
		)
		return sum;
	}
}
static longdouble NUMsumPower_longdouble (constVECVU const& vec, longdouble power) {
	if (vec.stride == 1) {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, vec.size,
			const double *p = & vec [1],
			powl (longdouble (fabs (*p)), power),
			p += 1
		)
		return sum;
	} else {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, vec.size,
			const double *p = & vec [1],
			powl (longdouble (fabs (*p)), power),
			p += vec.stride
		)
		return sum;
	}
}
static longdouble NUMsumPower_longdouble (constMATVU const& mat, longdouble power) {
	if (mat.nrow <= mat.ncol) {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, mat.nrow,
			integer irow = 1,
			NUMsumPower_longdouble (mat [irow], power),
			irow += 1
		)
		return sum;
	} else {
		PAIRWISE_SUM (
			longdouble, sum,
			integer, mat.ncol,
			integer icol = 1,
			NUMsumPower_longdouble (mat.column (icol), power),
			icol += 1
		)
		return sum;
	}
}

struct MelderMeanSumsq_longdouble {
	longdouble mean, sumsq;
};

static MelderMeanSumsq_longdouble NUMmeanSumsq (constVECVU const& vec) noexcept {
	MelderMeanSumsq_longdouble result;
	result.mean = NUMsum_longdouble (vec) / vec.size;
	double mean = double (result.mean);
	if (vec.stride == 1) {
		PAIRWISE_SUM (longdouble, sumsq, integer, vec.size,
			const double *p = & vec [1],
			longdouble (*p - mean) * longdouble (*p - mean),
			p += 1
		)
		result.sumsq = sumsq;
	} else {
		PAIRWISE_SUM (longdouble, sumsq, integer, vec.size,
			const double *p = & vec [1],
			longdouble (*p - mean) * longdouble (*p - mean),
			p += vec.stride
		)
		result.sumsq = sumsq;
	}
	return result;
}
static MelderMeanSumsq_longdouble NUMmeanSumsq (constMATVU const& mat) noexcept {
	MelderMeanSumsq_longdouble result;
	result.mean = NUMsum_longdouble (mat) / (mat.nrow * mat.ncol);
	double mean = double (result.mean);
	if (mat.nrow <= mat.ncol) {
		PAIRWISE_SUM (longdouble, sumsq, integer, mat.nrow,
			integer irow = 1,
			NUMsumOfSquaredDifferences_longdouble (mat [irow], mean),
			irow += 1
		)
		result.sumsq = sumsq;
	} else {
		PAIRWISE_SUM (longdouble, sumsq, integer, mat.ncol,
			integer icol = 1,
			NUMsumOfSquaredDifferences_longdouble (mat.column (icol), mean),
			icol += 1
		)
		result.sumsq = sumsq;
	}
	return result;
}

/*
	Global functions in alphabetic order.
*/

double NUMcenterOfGravity (constVECVU const& x) noexcept {
	longdouble weightedSumOfIndexes = 0.0, sumOfWeights = 0.0;
	for (integer i = 1; i <= x.size; i ++) {
		weightedSumOfIndexes += i * x [i];
		sumOfWeights += x [i];
	}
	return double (weightedSumOfIndexes / sumOfWeights);
}

double NUMinner (constVECVU const& x, constVECVU const& y) noexcept {
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

double NUMmean (constVECVU const& vec) {
		//Melder_require (vec.size >= 1,
		//	U"mean(vector#): the size of the vector should be greater than 0.");
		longdouble sum = NUMsum_longdouble (vec);
		return double (sum / vec.size);
}
double NUMmean (constMATVU const& mat) noexcept {
	longdouble sum = NUMsum_longdouble (mat);
	return double (sum / (mat.nrow * mat.ncol));
}

MelderGaussianStats NUMmeanStdev (constVECVU const& vec) noexcept {
	MelderMeanSumsq_longdouble stats = NUMmeanSumsq (vec);
	const longdouble variance = stats.sumsq / (vec.size - 1);
	MelderGaussianStats result;
	result.mean = double (stats.mean);
	result.stdev = sqrt (double (variance));
	return result;
}


double NUMnorm (constVECVU const& vec, double power) noexcept {
	if (power < 0.0) return undefined;
	if (power == 2.0) {
		double sum2 = double (NUMsum2_longdouble (vec));
		return sqrt (sum2);
	} else if (power == 1.0) {
		double sumAbs = double (NUMsumAbs_longdouble (vec));
		return sumAbs;
	} else {
		longdouble sumPower = NUMsumPower_longdouble (vec, power);
		return double (powl (sumPower, longdouble (1.0) / power));
	}
}
double NUMnorm (constMATVU const& mat, double power) noexcept {
	if (power < 0.0) return undefined;
	if (power == 2.0) {
		double sum2 = double (NUMsum2_longdouble (mat));
		return sqrt (sum2);
	} else if (power == 1.0) {
		double sumAbs = double (NUMsumAbs_longdouble (mat));
		return sumAbs;
	} else {
		longdouble sumPower = NUMsumPower_longdouble (mat, power);
		return double (powl (sumPower, longdouble (1.0) / power));
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

double NUMstdev (constVECVU const& vec) noexcept {
	MelderMeanSumsq_longdouble stats = NUMmeanSumsq (vec);
	const longdouble variance = stats.sumsq / (vec.size - 1);
	double stdev = sqrt (double (variance));
	return stdev;
}
double NUMstdev (constMATVU const& mat) noexcept {
	MelderMeanSumsq_longdouble stats = NUMmeanSumsq (mat);
	const longdouble variance = stats.sumsq / (mat.nrow * mat.ncol - 1);
	double stdev = sqrt (double (variance));
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

double NUMsum2 (constVECVU const& vec) {
	return double (NUMsum2_longdouble (vec));
}
double NUMsum2 (constMATVU const& mat) {
	return double (NUMsum2_longdouble (mat));
}

double NUMsumOfSquaredDifferences (constVECVU const& vec, double mean) {
	return double (NUMsumOfSquaredDifferences_longdouble (vec, mean));
}

double NUMsumsq (const constVECVU& vec) noexcept {
	MelderMeanSumsq_longdouble stats = NUMmeanSumsq (vec);
	return double (stats.sumsq);
}
double NUMsumsq (const constMATVU& mat) noexcept {
	MelderMeanSumsq_longdouble stats = NUMmeanSumsq (mat);
	return double (stats.sumsq);
}

double NUMvariance (const constVECVU& vec) noexcept {
	MelderMeanSumsq_longdouble stats = NUMmeanSumsq (vec);
	const longdouble variance = stats.sumsq / (vec.size - 1);
	return double (variance);
}
double NUMvariance (const constMATVU& mat) noexcept {
	MelderMeanSumsq_longdouble stats = NUMmeanSumsq (mat);
	const longdouble variance = stats.sumsq / (mat.nrow * mat.ncol - 1);
	return double (variance);
}

/* End of file NUM.cpp */
