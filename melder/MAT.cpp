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

#include "melder.h"
#include "../dwsys/NUM2.h"
//#include "../dwsys/NUMcblas.h"
//#include "../external/gsl/gsl_blas.h"

#ifdef macintosh
	#include <Accelerate/Accelerate.h>
	#import <MetalPerformanceShaders/MetalPerformanceShaders.h>
#endif

void MATcentreEachColumn_inplace (MAT const& x) noexcept {
	for (integer icol = 1; icol <= x.ncol; icol ++) {
		double const columnMean = NUMcolumnMean (x, icol);
		for (integer irow = 1; irow <= x.nrow; irow ++)
			x [irow] [icol] -= columnMean;
	}
}

void MATcentreEachRow_inplace (MAT const& x) noexcept {
	for (integer irow = 1; irow <= x.nrow; irow ++)
		VECcentre_inplace (x.row (irow));
}

void MATdoubleCentre_inplace (MAT const& x) noexcept {
	MATcentreEachRow_inplace (x);
	MATcentreEachColumn_inplace (x);
}

void MATmtm_preallocated (MAT const& target, constMAT const& x) noexcept {
	Melder_assert (target.nrow == x.ncol);
	Melder_assert (target.ncol == x.ncol);
	#if 0
	for (integer irow = 1; irow <= target.nrow; irow ++) {
		for (integer icol = irow; icol <= target.ncol; icol ++) {
			longdouble t = 0.0;
			for (integer k = 1; k <= x.nrow; k ++)
				t += x [k] [irow] * x [k] [icol];
			target [irow] [icol] = target [icol] [irow] = double (t);
		}
	}
	#elif 0
	for (integer irow = 1; irow <= target.nrow; irow ++) {
		for (integer icol = irow; icol <= target.ncol; icol ++) {
			PAIRWISE_SUM (longdouble, sum, integer, x.nrow,
				double const *px1 = & x [1] [irow];
				double const *px2 = & x [1] [icol],
				longdouble (*px1) * longdouble (*px2),
				(px1 += x.ncol, px2 += x.ncol)
			)
			target [irow] [icol] = target [icol] [irow] = double (sum);
		}
	}
	#else
	for (integer irow = 1; irow <= target.nrow; irow ++)
		for (integer icol = irow; icol <= target.ncol; icol ++)
			target [irow] [icol] = 0.0;
	for (integer k = 1; k <= x.nrow; k ++)
		for (integer irow = 1; irow <= target.nrow; irow ++)
			for (integer icol = irow; icol <= target.ncol; icol ++)
				target [irow] [icol] += x [k] [irow] * x [k] [icol];
	for (integer irow = 2; irow <= target.nrow; irow ++)
		for (integer icol = 1; icol < irow; icol ++)
			target [irow] [icol] = target [icol] [irow];
	#endif
}

void MATVUmul_ (MATVU const& target, constMATVU const& x, constMATVU const& y) noexcept {
	/*
		Precise matrix multiplication, using pairwise summation.
	*/
	for (integer irow = 1; irow <= target.nrow; irow ++) {
		for (integer icol = 1; icol <= target.ncol; icol ++) {
			PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
				double const *px = & x [irow] [1];
				double const *py = & y [1] [icol],
				longdouble (*px) * longdouble (*py),
				(px += x.colStride, py += y.rowStride)
			)
			target [irow] [icol] = double (sum);
		}
	}
}

inline constVEC VECrow_nocheck (constMAT const& mat, integer rowNumber) {
	return constVEC (mat.at [rowNumber], mat.ncol);
}
inline void MATmul_fast_preallocated_ (MAT const& target, constMAT const& x, constMAT const& y) noexcept {
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
	#elif 0
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
	#elif 0
		/*
			This version is 20,0.80,0.32,0.33 ns per multiply-add for size = 1,10,100,1000.
		*/
		double *ptarget = & asvector (target) [1];
		double const *px = & asvector (x) [1], *py = & asvector (y) [1];
		for (integer irow = 0; irow < target.nrow; irow ++) {
			for (integer icol = 0; icol < target.ncol; icol ++)
				ptarget [irow * target.ncol + icol] = 0.0;
			for (integer i = 0; i < x.ncol; i ++)
				for (integer icol = 0; icol < target.ncol; icol ++)
					ptarget [irow * target.ncol + icol] += px [irow * x.ncol + i] * py [i * y.ncol + icol];
		}
	#elif 0
		/*
			Naive slow implementation, via stored row pointers.
			This version is 7.5,0.69,0.87,1.87 ns per multiply-add for size = 1,10,100,1000.
		*/
		for (integer irow = 1; irow <= target.nrow; irow ++) {
			for (integer icol = 1; icol <= target.ncol; icol ++) {
				target [irow] [icol] = 0.0;
				for (integer i = 1; i <= x.ncol; i ++)
					target [irow] [icol] += x [irow] [i] * y [i] [icol];
			}
		}
	#elif 0
		/*
			Naive slow implementation, via computed row pointers.
			This version is not slower than the version with stored pointers,
			although the inner loop contains the multiplication i * y.ncol,
			which the compiler cannot get rid of
			(some compilers may replace it with a y.ncol stride addition).
			It anything, this version is slightly faster than the one with stored pointers:
			the speed is 9.1,0.63,0.83,1.83 ns per multiply-add for size = 1,10,100,1000.
		*/
		double *ptarget = & asvector (target) [1];
		double const *px = & asvector (x) [1], *py = & asvector (y) [1];
		for (integer irow = 0; irow < target.nrow; irow ++) {
			for (integer icol = 0; icol < target.ncol; icol ++) {
				ptarget [irow * target.ncol + icol] = 0.0;
				for (integer i = 0; i < x.ncol; i ++)
					ptarget [irow * target.ncol + icol] += px [irow * x.ncol + i] * py [i * y.ncol + icol];
			}
		}
	#elif 0
		/*
			Another attempt to slow down the computation,
			namely by making matrix indexing compute a vector, with size information and all.
			This version is 8.4,1.00,0.95,2.38 ns per multiply-add for size = 1,10,100,1000.
			That is really somewhat slower, but is that because of the size computation
			or because of the range check?
		*/
		for (integer irow = 1; irow <= target.nrow; irow ++) {
			for (integer icol = 1; icol <= target.ncol; icol ++) {
				target [irow] [icol] = 0.0;
				for (integer i = 1; i <= x.ncol; i ++)
					target [irow] [icol] += x.row (irow) [i] * y.row (i) [icol];
			}
		}
	#elif 0
		/*
			Here we get rid of the row number check, but we still compute
			a whole vector with size information. Programmingwise, this would be our ideal.
			This version is 7.5,0.70,0.87,2.04 ns per multiply-add for size = 1,10,100,1000.
			It seems to be slightly slower for large matrices than the first stored-row-pointer version.
		*/
		for (integer irow = 1; irow <= target.nrow; irow ++) {
			for (integer icol = 1; icol <= target.ncol; icol ++) {
				target [irow] [icol] = 0.0;
				for (integer i = 1; i <= x.ncol; i ++)
					target [irow] [icol] += VECrow_nocheck (x, irow) [i] * VECrow_nocheck (y, i) [icol];
			}
		}
	#else
		/*
			The smart version, with whole-vector computation. Still programmatically ideal.
			This version is 14.6,0.66,0.31,0.36 ns per multiply-add for size = 1,10,100,1000.
		*/
		for (integer irow = 1; irow <= target.nrow; irow ++) {
			for (integer icol = 1; icol <= target.ncol; icol ++)
				target [irow] [icol] = 0.0;
			for (integer i = 1; i <= x.ncol; i ++)
				for (integer icol = 1; icol <= target.ncol; icol ++)
					target [irow] [icol] += VECrow_nocheck (x, irow) [i] * VECrow_nocheck (y, i) [icol];
		}
	#endif
}

static inline void MATVUmul_rough_naiveReferenceImplementation (MATVU const& target, constMATVU const& x, constMATVU const& y) noexcept {
	/*
		If x.colStride == size and y.colStride == 1,
		this version is 0.073, 1.32, 1.17, 0.58 Gflops for size = 1,10,100,1000.
	*/
	for (integer irow = 1; irow <= target.nrow; irow ++) {
		for (integer icol = 1; icol <= target.ncol; icol ++) {
			target [irow] [icol] = 0.0;
			for (integer i = 1; i <= x.ncol; i ++)
				target [irow] [icol] += x [irow] [i] * y [i] [icol];
		}
	}
}
void MATVUmul_fast_ (MATVU const& target, constMATVU const& x, constMATVU const& y) noexcept {
	#ifdef macintoshXXX
		static bool gpuInited = false;
		id<MTLDevice> gpuDevice;
		if (! gpuInited) {
			gpuDevice = MTLCreateSystemDefaultDevice ();
			Melder_casual (U"GPU device", Melder_pointer (gpuDevice));
			gpuInited = true;
		}
	#endif
	if ((false)) {
		MATVUmul_rough_naiveReferenceImplementation (target, x, y);
	} else if (y.colStride == 1) {
		/*
			This case is appropriate for the multiplication of full matrices
				X.Y
			or
				X'.Y

			The speed for X.Y is 0.063, 1.38, 3.16, 3.02 Gflops for size = 1,10,100,1000.
			The speed for X'.Y is 0.063, 1.37, 3.13, 2.73 Gflops for size = 1,10,100,1000.

			The trick is to have the inner loop run along two fastest indices;
			for target as well as y, this fastest index is the last index.
			Note that the multiplication factor within the inner loop is constant,
			so we move it out of the loop (by hand, in case the compiler doesn't do it).
		*/
		for (integer irow = 1; irow <= target.nrow; irow ++) {
			VECVU const targetrow = target [irow];
			for (integer icol = 1; icol <= target.ncol; icol ++)
				targetrow [icol] = 0.0;
			for (integer i = 1; i <= x.ncol; i ++) {
				double const xcell = x [irow] [i];
				constVECVU const yrow = y [i];
				for (integer icol = 1; icol <= target.ncol; icol ++)
					targetrow [icol] += xcell * yrow [icol];
			}
		}
	} else if (y.rowStride == 1) {
		if (x.colStride == 1) {
			/*
				This case will be appropriate for the multiplication of full matrices
					X.Y'
				The speed is 0.063, 1.17, 1.68, 1.70 Gflops for size = 1,10,100,1000.
			*/
			MATVUmul_ (target, x, y);
		} else {
			/*
				This case will be appropriate for the multiplication of full matrices
					X'.Y'
				However, this will work only once an automatrix has row and column strides;
				until that time we cannot really modify the structure of `target`.

				So we will make this fast by making the target matrix column-major.
				The speed will be 0.065, 1.27, 1.45, 1.21 Gflops for size = 1,10,100,1000.

				For the moment, the target has to stay row-major.
				The speed is 0.064, 1.25, 1.40, 0.43 Gflops for size = 1,10,100,1000.

				The trick is to have the inner loop run along two fastest indices;
				for both target (in future) and y, this fastest index is the first index.
			*/
			//target.rowStride = 1;
			//target.colStride = target.nrow;
			for (integer icol = 1; icol <= target.ncol; icol ++) {
				VECVU const targetcolumn = target.column (icol);
				for (integer irow = 1; irow <= target.nrow; irow ++)
					targetcolumn [irow] = 0.0;
				for (integer i = 1; i <= x.ncol; i ++) {
					constVECVU const ycol = y.column (i);
					for (integer irow = 1; irow <= target.nrow; irow ++)
						targetcolumn [irow] += x [irow] [i] * ycol [irow];
				}
			}
		}
	} else {
		/*
			A rare case: the strides of y are both greater than 1.
			We do not bother to optimize these cases yet.
		*/
		MATVUmul_rough_naiveReferenceImplementation (target, x, y);
	}
}

void MATouter_preallocated (const MAT& target, const constVEC& x, const constVEC& y) {

	for (integer irow = 1; irow <= x.size; irow ++)
		for (integer icol = 1; icol <= y.size; icol ++)
			target [irow] [icol] = x [irow] * y [icol];
}
autoMAT MATouter (const constVEC& x, const constVEC& y) {
	autoMAT result = MATraw (x.size, y.size);
	MATouter_preallocated (result.get(), x, y);
	return result;
}

autoMAT MATpeaks (constVEC const& x, bool includeEdges, int interpolate, bool sortByHeight) {
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
