/* MAT.cpp
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

#define USE_CBLAS_GEMM  0
#define USE_GSL_GEMM  0
#ifdef macintosh
	#define USE_APPLE_GEMM  1
#else
	#define USE_APPLE_GEMM  0
#endif

#include "melder.h"
#include "../dwsys/NUM2.h"
#if USE_CBLAS_GEMM
	#include "../dwsys/NUMcblas.h"
#endif
#if USE_GSL_GEMM
	#include "../external/gsl/gsl_blas.h"
#endif
#if USE_APPLE_GEMM
	#include <Accelerate/Accelerate.h>
#endif

void MATcentreEachColumn_inplace (MAT x, double centres []) {
	autoVEC columnBuffer = VECraw (x.nrow);
	for (integer icol = 1; icol <= x.ncol; icol ++) {
		for (integer irow = 1; irow <= x.nrow; irow ++)
			columnBuffer [irow] = x [irow] [icol];
		double columnMean;
		VECcentre_inplace (columnBuffer.get(), & columnMean);
		for (integer irow = 1; irow <= x.nrow; irow ++)
			x [irow] [icol] = columnBuffer [irow];
		if (centres)
			centres [icol] = columnMean;
	}
}

void MATcentreEachRow_inplace (MAT x) {
	for (integer irow = 1; irow <= x.nrow; irow ++)
		VECcentre_inplace (x.row (irow));
}

void MATdoubleCentre_inplace (MAT x) {
	MATcentreEachRow_inplace (x);
	MATcentreEachColumn_inplace (x);
}

void MATmul_preallocated_ (const MAT& target, const constMAT& x, const constMAT& y) {
	for (integer irow = 1; irow <= target.nrow; irow ++) {
		for (integer icol = 1; icol <= target.ncol; icol ++) {
			PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
				const double *px = & x [irow] [1];
				const double *py = & y [1] [icol],
				(longdouble) *px * (longdouble) *py,
				(px += 1, py += y.ncol)
			)
			target [irow] [icol] = double (sum);
		}
	}
}

void MATmul_fast_preallocated_ (const MAT& target, const constMAT& x, const constMAT& y) {
	#if 1
	/*
		This version is 10,0.76,0.32,0.34 ns per multiply-add for size = 1,10,100,1000.

		The trick is to have the inner loop run along two final indices.
		Note that the multiplication factor within the inner loop is constant,
		so it will be moved out of the loop by the compiler.
	*/
	for (integer irow = 1; irow <= target.nrow; irow ++) {
		for (integer icol = 1; icol <= target.ncol; icol ++) {
			PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
				const double *px = & x [irow] [1];
				const double *py = & y [1] [icol],
				(longdouble) *px * (longdouble) *py,
				(px += 1, py += y.ncol)
			)
			target [irow] [icol] = double (sum);
		}
	}
}

void MATmul_fast_preallocated_ (const MAT& target, const constMAT& x, const constMAT& y) {
	#if USE_CBLAS_GEMM
		/*
			This version is 49,0.75,0.32,0.33 ns per multiply-add for size = 1,10,100,1000.
		*/
		double alpha = 1.0, beta = 0.0;
		NUMblas_dgemm ("N", "N", & target.nrow, & target.ncol, & x.ncol, & alpha,
				(double *) & x [1] [1], & x.nrow, (double *) & y [1] [1], & y.nrow, & beta, & target [1] [1], & target.nrow);
	#elif USE_GSL_GEMM
		/*
			This version is 34,0.72,0.31,0.41(NoTrans;1.34Trans) ns per multiply-add for size = 1,10,100,1000.
		*/
		gsl_matrix gslx { (size_t) x.nrow, (size_t) x.ncol, (size_t) x.nrow, (double *) & x [1] [1], nullptr, false };
		gsl_matrix gsly { (size_t) y.nrow, (size_t) y.ncol, (size_t) y.nrow, (double *) & y [1] [1], nullptr, false };
		gsl_matrix gsltarget { (size_t) target.nrow, (size_t) target.ncol, (size_t) target.nrow, (double *) & target [1] [1], nullptr, false };
		gsl_blas_dgemm (CblasTrans, CblasTrans, 1.0, & gslx, & gsly, 0.0, & gsltarget);
	#elif USE_APPLE_GEMM
		cblas_dgemm (CblasRowMajor, CblasNoTrans, CblasNoTrans, target.nrow, target.ncol, x.ncol,
				1.0, & x [1] [1], x.nrow, & y [1] [1], y.nrow, 0.0, & target [1] [1], target.nrow);   // 24,0.71,0.31,0.45
	#elif 1
		/*
			This version is 10,0.76,0.32,0.34 ns per multiply-add for size = 1,10,100,1000.

			The trick is to have the inner loop run along two final indices.
			Note that the multiplication factor within the inner loop is constant,
			so it will be moved out of the loop by the compiler.
		*/
		for (integer irow = 1; irow <= target.nrow; irow ++) {
			for (integer icol = 1; icol <= target.ncol; icol ++)
				target [irow] [icol] = 0.0;
			for (integer i = 1; i <= x.ncol; i ++)
				for (integer icol = 1; icol <= target.ncol; icol ++)
					target [irow] [icol] += x [irow] [i] * y [i] [icol];
		}
	#else
		/*
			This version is 20,0.80,0.32,0.33 ns per multiply-add for size = 1,10,100,1000.
		*/
		double *ptarget = & asvector (target) [1];
		const double *px = & asvector (x) [1], *py = & asvector (y) [1];
		for (integer irow = 0; irow < target.nrow; irow ++) {
			for (integer icol = 0; icol < target.ncol; icol ++)
				ptarget [irow * target.ncol + icol] = 0.0;
			for (integer i = 0; i < x.ncol; i ++)
				for (integer icol = 0; icol < target.ncol; icol ++)
					ptarget [irow * target.ncol + icol] += px [irow * x.ncol + i] * py [i * y.ncol + icol];
		}
	#endif
}

void MATmul_nt_preallocated_ (const MAT& target, const constMAT& x, const constMAT& y) {
	for (integer irow = 1; irow <= target.nrow; irow ++) {
		for (integer icol = 1; icol <= target.ncol; icol ++) {
			PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
				const double *px = & x [irow] [1];
				const double *py = & y [icol] [1],
				(longdouble) *px * (longdouble) *py,
				(px += 1, py += 1)
			)
			target [irow] [icol] = double (sum);
		}
	}
}

void MATmul_tn_preallocated_ (const MAT& target, const constMAT& x, const constMAT& y) {
	for (integer irow = 1; irow <= target.nrow; irow ++) {
		for (integer icol = 1; icol <= target.ncol; icol ++) {
			PAIRWISE_SUM (longdouble, sum, integer, x.nrow,
				const double *px = & x [1] [irow];
				const double *py = & y [1] [icol],
				(longdouble) *px * (longdouble) *py,
				(px += x.ncol, py += y.ncol)
			)
			target [irow] [icol] = double (sum);
		}
	}
}

void MATmul_tn_fast_preallocated_ (const MAT& target, const constMAT& x, const constMAT& y) {
	for (integer irow = 1; irow <= target.nrow; irow ++) {
		for (integer icol = 1; icol <= target.ncol; icol ++)
			target [irow] [icol] = 0.0;
		for (integer i = 1; i <= x.nrow; i ++)
			for (integer icol = 1; icol <= target.ncol; icol ++)
				target [irow] [icol] += x [i] [irow] * y [i] [icol];
	}
}

void MATmul_tt_preallocated_ (const MAT& target, const constMAT& x, const constMAT& y) {
	for (integer irow = 1; irow <= target.nrow; irow ++) {
		for (integer icol = 1; icol <= target.ncol; icol ++) {
			PAIRWISE_SUM (longdouble, sum, integer, x.nrow,
				const double *px = & x [1] [irow];
				const double *py = & y [icol] [1],
				(longdouble) *px * (longdouble) *py,
				(px += x.ncol, py += 1)
			)
			target [irow] [icol] = double (sum);
		}
	}
}

void MATmul_tt_fast_preallocated_ (const MAT& target, const constMAT& x, const constMAT& y) {
	integer n = x.nrow;
	if (n > 100) {
		autoMAT xt = MATtranspose (x), yt = MATtranspose (y);
		MATmul_fast_preallocated (target, xt.get(), yt.get());
		return;
	}
	MATmul_tt_preallocated_ (target, x, y);
}

autoMAT MATouter (constVEC x, constVEC y) {
	autoMAT result = MATraw (x.size, y.size);
	for (integer irow = 1; irow <= x.size; irow ++)
		for (integer icol = 1; icol <= y.size; icol ++)
			result [irow] [icol] = x [irow] * y [icol];
	return result;
}

autoMAT MATpeaks (constVEC x, bool includeEdges, int interpolate, bool sortByHeight) {
	if (x.size < 2) includeEdges = false;
	integer numberOfPeaks = 0;
	for (integer i = 2; i < x.size; i ++)
		if (x [i] > x [i - 1] && x [i] >= x [i + 1])
			numberOfPeaks ++;
	if (includeEdges) {
		if (x [1] > x [2]) numberOfPeaks ++;
		if (x [x.size] > x [x.size - 1]) numberOfPeaks ++;
	}
	autoMAT result = MATraw (2, numberOfPeaks);
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

/* End of file MAT.cpp */
