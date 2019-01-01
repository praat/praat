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

void MATcentreEachColumn_inplace (MATVU const& x) noexcept {
	for (integer icol = 1; icol <= x.ncol; icol ++) {
		const double columnMean = NUMmean (x.column (icol));
		for (integer irow = 1; irow <= x.nrow; irow ++)
			x [irow] [icol] -= columnMean;
	}
}

void MATcentreEachRow_inplace (MATVU const& x) noexcept {
	for (integer irow = 1; irow <= x.nrow; irow ++)
		VECcentre_inplace (x [irow]);
}

void MATdoubleCentre_inplace (MATVU const& x) noexcept {
	MATcentreEachRow_inplace (x);
	MATcentreEachColumn_inplace (x);
}

void MATmtm_preallocated (MATVU const& target, constMATVU const& x) noexcept {
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
				const double *px1 = & x [1] [irow];
				const double *px2 = & x [1] [icol],
				longdouble (*px1) * longdouble (*px2),
				(px1 += x.rowStride, px2 += x.rowStride)
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
	if (x.colStride == 1) {
		if (y.rowStride == 1) {
			/*
				Appropriate for Target := X.Y',
				if X and Y are packed row-major matrices.
				The speed is 0.142, 0.716, 1.32, 1.64, 2.33, 2.22, 2.08, 2.31, 2.18, 1.89, 1.77, 1.53 Gflop/s
				for size =       1,     3,   10,   20,   50,  100,  200,  500, 1000, 2000, 3000, 5000.
			*/
			for (integer irow = 1; irow <= target.nrow; irow ++) {
				for (integer icol = 1; icol <= target.ncol; icol ++) {
					PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
						const double *px = & x [irow] [1];
						const double *py = & y [1] [icol],
						longdouble (*px) * longdouble (*py),
						(px += 1, py += 1)
					)
					target [irow] [icol] = double (sum);
				}
			}
		} else {
			/*
				Appropriate for Target := X.Y,
				if X and Y are packed row-major matrices.
				The speed is 0.143, 0.684, 1.20, 1.64, 2.24, 2.04, 1.44, 1.22, 0.56, 0.114 Gflop/s
				for size =       1,     3,   10,   20,   50,  100,  200,  500, 1000,  2000.
			*/
			for (integer irow = 1; irow <= target.nrow; irow ++) {
				for (integer icol = 1; icol <= target.ncol; icol ++) {
					PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
						const double *px = & x [irow] [1];
						const double *py = & y [1] [icol],
						longdouble (*px) * longdouble (*py),
						(px += 1, py += y.rowStride)
					)
					target [irow] [icol] = double (sum);
				}
			}
		}
	} else if (y.rowStride == 1) {
		/*
			Appropriate for Target := X'.Y',
			if X and Y are packed row-major matrices.
			The speed is 0.136, 0.666, 1.22, 1.65, 2.36, 1.96, 1.62, 1.24, 0.69, 0.118 Gflop/s
			for size =       1,     3,   10,   20,   50,  100,  200,  500, 1000,  2000.
		*/
		for (integer irow = 1; irow <= target.nrow; irow ++) {
			for (integer icol = 1; icol <= target.ncol; icol ++) {
				PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
					const double *px = & x [irow] [1];
					const double *py = & y [1] [icol],
					longdouble (*px) * longdouble (*py),
					(px += x.colStride, py += 1)
				)
				target [irow] [icol] = double (sum);
			}
		}
	} else {
		/*
			Appropriate for Target := X'.Y,
			if X and Y are packed row-major matrices.
			The speed is 0.143, 0.572, 1.10, 1.43, 1.71, 1.70, 1.29, 0.71, 0.067 Gflop/s
			for size =       1,     3,   10,   20,   50,  100,  200,  500,  1000.
		*/
		for (integer irow = 1; irow <= target.nrow; irow ++) {
			for (integer icol = 1; icol <= target.ncol; icol ++) {
				PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
					const double *px = & x [irow] [1];
					const double *py = & y [1] [icol],
					longdouble (*px) * longdouble (*py),
					(px += x.colStride, py += y.rowStride)
				)
				target [irow] [icol] = double (sum);
			}
		}
	}
}

void MATVUmul_forceAllocation_ (MATVU const& target, constMATVU x, constMATVU y) {
	/*
		As seen above, the only multiplication that stays fast for large sizes,
		if X and Y are packed row-major matrices, is X.Y';
		this is because both x.colStride and y.rowStride are 1 in that case.
		It may therefore be useful to convert any matrix X that has
		a column stride unequal to 1, and any matrix Y that has a row stride
		unequal to 1, to matrices that do have these desirable properties.

		For the X.Y case, where X and Y are packed row-major matrices,
		the speed is 0.084, 0.124, 0.91, 1.56, 2.26, 2.18, 2.12, 2.25, 2.23, 1.85, 1.78, 1.57 Gflop/s
		for size =       1,     3,   10,   20,   50,  100,  200,  500, 1000, 2000, 3000, 5000.

		For the X.Y' case, where X and Y are packed row-major matrices, there is no allocation, and
		the speed is 0.084, 0.610, 1.26, 1.69, 2.32, 2.20, 2.12, 2.28, 2.24, 1.91, 1.76, 1.53 Gflop/s
		for size =       1,     3,   10,   20,   50,  100,  200,  500, 1000, 2000, 3000, 5000.

		For the X'.Y case, where X and Y are packed row-major matrices,
		the speed is 0.082, 0.068, 0.73, 1.42, 2.20, 2.14, 2.09, 2.27, 2.21, 1.84, 1.77, 1.53 Gflop/s
		for size =       1,     3,   10,   20,   50,  100,  200,  500, 1000, 2000, 3000, 5000.

		For the X'.Y' case, where X and Y are packed row-major matrices,
		the speed is 0.082, 0.117, 0.90, 1.57, 2.25, 2.19, 2.12, 2.23, 2.09, 1.92, 1.69, 1.48 Gflop/s
		for size =       1,     3,   10,   20,   50,  100,  200,  500, 1000, 2000, 3000, 5000.
	*/
	autoMAT tmpX, tmpY;   // the scope shall extend to the end of the function
	if (x.colStride != 1) {
		tmpX = newMATcopy (x);
		x = tmpX.all();
		Melder_assert (x.colStride == 1);
	}
	if (y.rowStride != 1) {
		tmpY = newMATtranspose (y);
		y = tmpY.transpose();
		Melder_assert (y.rowStride == 1);
	}
	for (integer irow = 1; irow <= target.nrow; irow ++) {
		for (integer icol = 1; icol <= target.ncol; icol ++) {
			PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
				const double *px = & x [irow] [1];
				const double *py = & y [1] [icol],
				longdouble (*px) * longdouble (*py),
				(px += 1, py += 1)
			)
			target [irow] [icol] = double (sum);
		}
	}
}

void MATVUmul_allowAllocation_ (MATVU const& target, constMATVU x, constMATVU y) {
	/*
		The faster of MATVUmul_ and MATVUmul_forceAllocation.
		Allocation takes place only for larger matrices, e.g. from size 47 on
		(100,000 flops).

		For the X.Y case, where X and Y are packed row-major matrices,
		the speed is 0.087, 0.574, 1.18, 1.61, 2.25, 2.14, 2.11, 2.23, 2.23, 1.88, 1.74, 1.53 Gflop/s
		for size =       1,     3,   10,   20,   50,  100,  200,  500, 1000, 2000, 3000, 5000.

		For the X.Y' case, where X and Y are packed row-major matrices, there is never allocation, and
		the speed is 0.088, 0.577, 1.28, 1.67, 2.27, 2.18, 2.12, 2.28, 2.20, 1.96, 1.78, 1.57 Gflop/s
		for size =       1,     3,   10,   20,   50,  100,  200,  500, 1000, 2000, 3000, 5000.

		For the X'.Y case, where X and Y are packed row-major matrices,
		the speed is 0.084, 0.547, 1.12, 1.44, 2.20, 2.15, 2.04, 2.25, 2.18, 1.92, 1.74, 1.48 Gflop/s
		for size =       1,     3,   10,   20,   50,  100,  200,  500, 1000, 2000, 3000, 5000.

		For the X'.Y' case, where X and Y are packed row-major matrices,
		the speed is 0.084, 0.553, 1.18, 1.63, 2.31, 2.12, 2.12, 2.25, 2.16, 1.90, 1.79, 1.50 Gflop/s
		for size =       1,     3,   10,   20,   50,  100,  200,  500, 1000, 2000, 3000, 5000.
	*/
	if (x.colStride == 1) {
		if (y.rowStride == 1) {
			for (integer irow = 1; irow <= target.nrow; irow ++) {
				for (integer icol = 1; icol <= target.ncol; icol ++) {
					PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
						const double *px = & x [irow] [1];
						const double *py = & y [1] [icol],
						longdouble (*px) * longdouble (*py),
						(px += 1, py += 1)
					)
					target [irow] [icol] = double (sum);
				}
			}
		} else {
			if (double (target.nrow) * double (target.ncol) * double (x.ncol) > 1e5) {
				autoMAT tmpY = newMATtranspose (y);
				y = tmpY.transpose();
				Melder_assert (y.rowStride == 1);
				for (integer irow = 1; irow <= target.nrow; irow ++) {
					for (integer icol = 1; icol <= target.ncol; icol ++) {
						PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
							const double *px = & x [irow] [1];
							const double *py = & y [1] [icol],
							longdouble (*px) * longdouble (*py),
							(px += 1, py += 1)
						)
						target [irow] [icol] = double (sum);
					}
				}
			} else {
				for (integer irow = 1; irow <= target.nrow; irow ++) {
					for (integer icol = 1; icol <= target.ncol; icol ++) {
						PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
							const double *px = & x [irow] [1];
							const double *py = & y [1] [icol],
							longdouble (*px) * longdouble (*py),
							(px += 1, py += y.rowStride)
						)
						target [irow] [icol] = double (sum);
					}
				}
			}
		}
	} else if (y.rowStride == 1) {
		if (double (target.nrow) * double (target.ncol) * double (x.ncol) > 1e5) {
			autoMAT tmpX = newMATcopy (x);
			x = tmpX.all();
			Melder_assert (x.colStride == 1);
			for (integer irow = 1; irow <= target.nrow; irow ++) {
				for (integer icol = 1; icol <= target.ncol; icol ++) {
					PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
						const double *px = & x [irow] [1];
						const double *py = & y [1] [icol],
						longdouble (*px) * longdouble (*py),
						(px += 1, py += 1)
					)
					target [irow] [icol] = double (sum);
				}
			}
		} else {
			for (integer irow = 1; irow <= target.nrow; irow ++) {
				for (integer icol = 1; icol <= target.ncol; icol ++) {
					PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
						const double *px = & x [irow] [1];
						const double *py = & y [1] [icol],
						longdouble (*px) * longdouble (*py),
						(px += x.colStride, py += 1)
					)
					target [irow] [icol] = double (sum);
				}
			}
		}
	} else {
		if (double (target.nrow) * double (target.ncol) * double (x.ncol) > 1e5) {
			autoMAT tmpX = newMATcopy (x);
			x = tmpX.all();
			Melder_assert (x.colStride == 1);
			autoMAT tmpY = newMATtranspose (y);
			y = tmpY.transpose();
			Melder_assert (y.rowStride == 1);
			for (integer irow = 1; irow <= target.nrow; irow ++) {
				for (integer icol = 1; icol <= target.ncol; icol ++) {
					PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
						const double *px = & x [irow] [1];
						const double *py = & y [1] [icol],
						longdouble (*px) * longdouble (*py),
						(px += 1, py += 1)
					)
					target [irow] [icol] = double (sum);
				}
			}
		} else {
			for (integer irow = 1; irow <= target.nrow; irow ++) {
				for (integer icol = 1; icol <= target.ncol; icol ++) {
					PAIRWISE_SUM (longdouble, sum, integer, x.ncol,
						const double *px = & x [irow] [1];
						const double *py = & y [1] [icol],
						longdouble (*px) * longdouble (*py),
						(px += x.colStride, py += y.rowStride)
					)
					target [irow] [icol] = double (sum);
				}
			}
		}
	}
}

static inline void MATVUmul_rough_naiveReferenceImplementation (MATVU const& target, constMATVU const& x, constMATVU const& y) noexcept {
	/*
		If x.colStride == size and y.colStride == 1,
		this version is 0.073, 1.32, 1.17, 0.58 Gflop/s for size = 1,10,100,1000.
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
	#ifdef macintosh
		if (@available (macOS 10.13, *) &&false) {
			/*
				The speed is 0.000'003, 0.003, 0.59,  8.4,    77 Gflop/s
				for size =           1,    10,  100, 1000, 10000.
			*/
			static bool gpuInited = false;
			static id <MTLDevice> gpuDevice;
			static id <MTLCommandQueue> gpuQueue;
			if (! gpuInited) {
				gpuDevice = MTLCreateSystemDefaultDevice ();
				Melder_casual (U"GPU device", Melder_pointer (gpuDevice));
				gpuInited = true;
				gpuQueue = [gpuDevice newCommandQueue];
			}
			MPSMatrixMultiplication *matrixMultiplication = [[MPSMatrixMultiplication alloc]
				initWithDevice: gpuDevice
				resultRows: integer_to_uinteger (target.nrow)
				resultColumns: integer_to_uinteger (target.ncol)
				interiorColumns: integer_to_uinteger (x.ncol)
			];
			Melder_assert (matrixMultiplication != nil);
			automatrix <float> x32 = newmatrixraw <float> (x.nrow, x.ncol);
			for (integer irow = 1; irow <= x.nrow; irow ++)
				for (integer icol = 1; icol <= x.ncol; icol ++)
					x32 [irow] [icol] = float (x [irow] [icol]);
			automatrix <float> y32 = newmatrixraw <float> (y.nrow, y.ncol);
			for (integer irow = 1; irow <= y.nrow; irow ++)
				for (integer icol = 1; icol <= y.ncol; icol ++)
					y32 [irow] [icol] = float (y [irow] [icol]);
			automatrix <float> target32 = newmatrixraw <float> (target.nrow, target.ncol);
			id <MTLBuffer> bufferX = [gpuDevice
					newBufferWithBytes: & x32 [1] [1]
					length: integer_to_uinteger (x32.nrow) * integer_to_uinteger (x32.ncol) * sizeof (float)
					options: MTLResourceStorageModeShared];
			id <MTLBuffer> bufferY = [gpuDevice
					newBufferWithBytes: & y32 [1] [1]
					length: integer_to_uinteger (y32.nrow) * integer_to_uinteger (y32.ncol) * sizeof (float)
					options: MTLResourceStorageModeShared];
			id <MTLBuffer> bufferTarget = [gpuDevice
					newBufferWithBytes: & target32 [1] [1]
					length: integer_to_uinteger (target32.nrow) * integer_to_uinteger (target32.ncol) * sizeof (float)
					options: MTLResourceStorageModeShared];
			MPSMatrixDescriptor *descriptorX =
				[MPSMatrixDescriptor matrixDescriptorWithRows: integer_to_uinteger (x.nrow)
					columns: integer_to_uinteger (x.ncol)
					rowBytes: integer_to_uinteger (x.ncol) * sizeof (float)
					dataType: MPSDataTypeFloat32];
			MPSMatrixDescriptor *descriptorY =
				[MPSMatrixDescriptor matrixDescriptorWithRows: integer_to_uinteger (y.nrow)
					columns: integer_to_uinteger (y.ncol)
					rowBytes: integer_to_uinteger (y.ncol) * sizeof (float)
					dataType: MPSDataTypeFloat32];
			MPSMatrixDescriptor *descriptorTarget =
				[MPSMatrixDescriptor matrixDescriptorWithRows: integer_to_uinteger (target.nrow)
					columns: integer_to_uinteger (target.ncol)
					rowBytes: integer_to_uinteger (target.ncol) * sizeof (float)
					dataType: MPSDataTypeFloat32];
			MPSMatrix *mpsX = [[MPSMatrix alloc] initWithBuffer: bufferX descriptor: descriptorX];
			Melder_assert (mpsX != nil);
			MPSMatrix *mpsY = [[MPSMatrix alloc] initWithBuffer: bufferY descriptor: descriptorY];
			Melder_assert (mpsY != nil);
			MPSMatrix *mpsTarget = [[MPSMatrix alloc] initWithBuffer: bufferTarget descriptor: descriptorTarget];
			Melder_assert (mpsTarget != nil);
			NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
      		id <MTLCommandBuffer> commandBuffer = [gpuQueue commandBuffer];   // autoreleased
      		[matrixMultiplication encodeToCommandBuffer: commandBuffer
      			leftMatrix: mpsX
      			rightMatrix: mpsY
      			resultMatrix: mpsTarget];
			[commandBuffer commit];
			[commandBuffer waitUntilCompleted];
			NSError *error = [commandBuffer error];
			if (error)
				Melder_casual (U"Error during execution: ",
					Melder_peek8to32 ([[error localizedDescription] UTF8String]),
					Melder_peek8to32 ([[error localizedFailureReason] UTF8String]),
					Melder_peek8to32 ([[error localizedRecoverySuggestion] UTF8String])
				);
			[error release];
			float *rawPointer = (float *) [bufferTarget contents];
			//float *rawPointer = (float *) [[bufferTarget data] contents];
			for (integer irow = 1; irow <= target.nrow; irow ++)
				for (integer icol = 1; icol <= target.ncol; icol ++) {
					double value = double (*rawPointer ++);
					//Melder_casual (value);
					target [irow] [icol] = value;
				}
			[bufferX release];
			[bufferY release];
			[bufferTarget release];
			[matrixMultiplication release];
			[mpsX release];
			[mpsY release];
			[mpsTarget release];
			//[descriptorX release];   // apparently the MPSMatrix objects have become owners?
			//[descriptorY release];
			//[descriptorTarget release];
			[pool release];   // this releases `commandBuffer`
			return;
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

			The speed for X.Y is 0.053, 1.37, 3.14, 2.99, 2.38, 2.06, 1.70 Gflop/s
			for size =               1,   10,  100, 1000, 2000, 3000, 5000.
			The speed for X'.Y is 0.063, 1.37, 3.11, 2.72 Gflop/s for size = 1,10,100,1000.

			The trick is to have the inner loop run along two fastest indices;
			for target as well as y, this fastest index is the last index.
			Note that the multiplication factor within the inner loop is constant,
			so we move it out of the loop (by hand, in case the compiler doesn't do it).
		*/
		#if 1
		for (integer irow = 1; irow <= target.nrow; irow ++) {
			VECVU const targetrow = target [irow];
			for (integer icol = 1; icol <= target.ncol; icol ++)
				targetrow [icol] = 0.0;
			for (integer i = 1; i <= x.ncol; i ++) {
				const double xcell = x [irow] [i];
				constVECVU const yrow = y [i];
				for (integer icol = 1; icol <= target.ncol; icol ++)
					targetrow [icol] += xcell * yrow [icol];
			}
		}
		#elif 0
			/*
				Using 64-bit BLAS from Apple's Accelerate framework.
				The speed for X.Y is 0.037, 1.51, 3.40, 2.40, 1.82, 1.64, 1.04 Gflop/s
				for size =               1,   10,  100, 1000, 2000, 3000, 5000.
				This is not really faster than our own simple implementation
				(perhaps 32-bit BLAS is faster, because it can use more flops per cycle).
			*/
			cblas_dgemm (CblasRowMajor, CblasNoTrans, CblasNoTrans,
				target.nrow, target.ncol, x.ncol,
				1.0,
				& x [1] [1], x.rowStride,
				& y [1] [1], y.rowStride,
				0.0,
				& target [1] [1], target.rowStride
			);
		#else
		/*
			An implementation that is notationally ideal.
			Does the compiler manage to move the constant parts
			of the expression outside the loop?

			The speed for X.Y is 0.056, 1.08, 2.99, 2.87 Gflop/s for size = 1,10,100,1000.
		*/
		for (integer irow = 1; irow <= target.nrow; irow ++) {
			for (integer icol = 1; icol <= target.ncol; icol ++)
				target [irow] [icol] = 0.0;
			for (integer i = 1; i <= x.ncol; i ++)
				for (integer icol = 1; icol <= target.ncol; icol ++)
					target [irow] [icol] += x [irow] [i] * y [i] [icol];
		}
		#endif
	} else if (y.rowStride == 1) {
		if (x.colStride == 1) {
			/*
				This case will be appropriate for the multiplication of full matrices
					X.Y'
				The speed is 0.064, 1.18, 1.67, 1.69 Gflop/s for size = 1,10,100,1000.
			*/
			MATVUmul_ (target, x, y);
		} else {
			/*
				This case will be appropriate for the multiplication of full matrices
					X'.Y'

				So we will make this fast by making the target matrix column-major.
				The speed will be 0.065, 1.27, 1.45, 1.21 Gflop/s for size = 1,10,100,1000.
				However, this will work only once an automatrix has row and column strides;
				until that time we cannot really modify the structure of `target`.

				For the moment, the target has to stay row-major.
				The speed is 0.064, 1.21, 1.41, 0.43 Gflop/s for size = 1,10,100,1000.

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
					constVECVU const ycolumn = y.column (i);
					for (integer irow = 1; irow <= target.nrow; irow ++)
						targetcolumn [irow] += x [irow] [i] * ycolumn [irow];
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

void MATouter_preallocated (MATVU const& target, constVECVU const& x, constVECVU const& y) {
	for (integer irow = 1; irow <= x.size; irow ++)
		for (integer icol = 1; icol <= y.size; icol ++)
			target [irow] [icol] = x [irow] * y [icol];
}
autoMAT newMATouter (constVECVU const& x, constVECVU const& y) {
	autoMAT result = newMATraw (x.size, y.size);
	MATouter_preallocated (result.get(), x, y);
	return result;
}

autoMAT newMATpeaks (constVECVU const& x, bool includeEdges, int interpolate, bool sortByHeight) {
	if (x.size < 2) includeEdges = false;
	integer numberOfPeaks = 0;
	for (integer i = 2; i < x.size; i ++)
		if (x [i] > x [i - 1] && x [i] >= x [i + 1])
			numberOfPeaks ++;
	if (includeEdges) {
		if (x [1] > x [2]) numberOfPeaks ++;
		if (x [x.size] > x [x.size - 1]) numberOfPeaks ++;
	}
	autoMAT result = newMATraw (2, numberOfPeaks);
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
				const double dy = 0.5 * (x [i + 1] - x [i - 1]);
				const double d2y = (x [i] - x [i - 1]) + (x [i] - x [i + 1]);
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
		NUMsortTogether (result.row (2), result.row (1));

		for (integer i = 1; i <= numberOfPeaks; i ++)
			result [2] [i] *= -1.0;
	}
	return result;
}

/* End of file MAT.cpp */
