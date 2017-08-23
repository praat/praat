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

void numvec :: _initAt (integer givenSize, bool zero) {
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

void nummat :: _initAt (integer givenNrow, integer givenNcol, bool zero) {
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

/*
	Recursive ("pairwise") addition preserves precision and enhances speed.
*/

#define tensor_ADD_1_TERM \
	real80 r1 = tensor_NEXT_TERM;   /* 1 */

#define tensor_ADD_2_TERMS \
	tensor_ADD_1_TERM \
	real80 r2 = tensor_NEXT_TERM;   /* 2 */ \
	r1 += r2;   /* 2 terms */

#define tensor_ADD_4_TERMS \
	tensor_ADD_2_TERMS \
	r2 = tensor_NEXT_TERM;   /* 3 */ \
	real80 r3 = tensor_NEXT_TERM;   /* 4 */ \
	r2 += r3;   /* 2 terms */ \
	r1 += r2;   /* 4 terms */

#define tensor_ADD_8_TERMS \
	tensor_ADD_4_TERMS \
	r2 = tensor_NEXT_TERM;   /* 5 */ \
	r3 = tensor_NEXT_TERM;   /* 6 */ \
	r2 += r3;   /* 2 terms */ \
	r3 = tensor_NEXT_TERM;   /* 7 */ \
	real80 r4 = tensor_NEXT_TERM;   /* 8 */ \
	r3 += r4;   /* 2 terms */ \
	r2 += r3;   /* 4 terms */ \
	r1 += r2;   /* 8 terms */

#define tensor_ADD_16_TERMS \
	tensor_ADD_8_TERMS \
	r2 = tensor_NEXT_TERM;   /* 9 */ \
	r3 = tensor_NEXT_TERM;   /* 10 */ \
	r2 += r3;   /* 2 terms */ \
	r3 = tensor_NEXT_TERM;   /* 11 */ \
	r4 = tensor_NEXT_TERM;   /* 12 */ \
	r3 += r4;   /* 2 terms */ \
	r2 += r3;   /* 4 terms */ \
	r3 = tensor_NEXT_TERM;   /* 13 */ \
	r4 = tensor_NEXT_TERM;   /* 14 */ \
	r3 += r4;   /* 2 terms */ \
	r4 = tensor_NEXT_TERM;   /* 15 */ \
	real80 r5 = tensor_NEXT_TERM;   /* 16 */ \
	r4 += r5;   /* 2 terms */ \
	r3 += r4;   /* 4 terms */ \
	r2 += r3;   /* 8 terms */ \
	r1 += r2;   /* 16 terms */

#define tensor_ADD_32_TERMS \
	tensor_ADD_16_TERMS \
	r2 = tensor_NEXT_TERM;   /* 17 */ \
	r3 = tensor_NEXT_TERM;   /* 18 */ \
	r2 += r3;   /* 2 terms */ \
	r3 = tensor_NEXT_TERM;   /* 19 */ \
	r4 = tensor_NEXT_TERM;   /* 20 */ \
	r3 += r4;   /* 2 terms */ \
	r2 += r3;   /* 4 terms */ \
	r3 = tensor_NEXT_TERM;   /* 21 */ \
	r4 = tensor_NEXT_TERM;   /* 22 */ \
	r3 += r4;   /* 2 terms */ \
	r4 = tensor_NEXT_TERM;   /* 23 */ \
	r5 = tensor_NEXT_TERM;   /* 24 */ \
	r4 += r5;   /* 2 terms */ \
	r3 += r4;   /* 4 terms */ \
	r2 += r3;   /* 8 terms */ \
	r3 = tensor_NEXT_TERM;   /* 25 */ \
	r4 = tensor_NEXT_TERM;   /* 26 */ \
	r3 += r4;   /* 2 terms */ \
	r4 = tensor_NEXT_TERM;   /* 27 */ \
	r5 = tensor_NEXT_TERM;   /* 28 */ \
	r4 += r5;   /* 2 terms */ \
	r3 += r4;   /* 4 terms */ \
	r4 = tensor_NEXT_TERM;   /* 29 */ \
	r5 = tensor_NEXT_TERM;   /* 30 */ \
	r4 += r5;   /* 2 terms */ \
	r5 = tensor_NEXT_TERM;   /* 31 */ \
	real80 r6 = tensor_NEXT_TERM;   /* 32 */ \
	r5 += r6;   /* 2 terms */ \
	r4 += r5;   /* 4 terms */ \
	r3 += r4;   /* 8 terms */ \
	r2 += r3;   /* 16 terms */ \
	r1 += r2;   /* 32 terms */

#define tensor_ADD_64_TERMS \
	tensor_ADD_32_TERMS \
	r2 = tensor_NEXT_TERM;   /* 33 */ \
	r3 = tensor_NEXT_TERM;   /* 34 */ \
	r2 += r3;   /* 2 terms */ \
	r3 = tensor_NEXT_TERM;   /* 35 */ \
	r4 = tensor_NEXT_TERM;   /* 36 */ \
	r3 += r4;   /* 2 terms */ \
	r2 += r3;   /* 4 terms */ \
	r3 = tensor_NEXT_TERM;   /* 37 */ \
	r4 = tensor_NEXT_TERM;   /* 38 */ \
	r3 += r4;   /* 2 terms */ \
	r4 = tensor_NEXT_TERM;   /* 39 */ \
	r5 = tensor_NEXT_TERM;   /* 40 */ \
	r4 += r5;   /* 2 terms */ \
	r3 += r4;   /* 4 terms */ \
	r2 += r3;   /* 8 terms */ \
	r3 = tensor_NEXT_TERM;   /* 41 */ \
	r4 = tensor_NEXT_TERM;   /* 42 */ \
	r3 += r4;   /* 2 terms */ \
	r4 = tensor_NEXT_TERM;   /* 43 */ \
	r5 = tensor_NEXT_TERM;   /* 44 */ \
	r4 += r5;   /* 2 terms */ \
	r3 += r4;   /* 4 terms */ \
	r4 = tensor_NEXT_TERM;   /* 45 */ \
	r5 = tensor_NEXT_TERM;   /* 46 */ \
	r4 += r5;   /* 2 terms */ \
	r5 = tensor_NEXT_TERM;   /* 47 */ \
	r6 = tensor_NEXT_TERM;   /* 48 */ \
	r5 += r6;   /* 2 terms */ \
	r4 += r5;   /* 4 terms */ \
	r3 += r4;   /* 8 terms */ \
	r2 += r3;   /* 16 terms */ \
	r3 = tensor_NEXT_TERM;   /* 49 */ \
	r4 = tensor_NEXT_TERM;   /* 50 */ \
	r3 += r4;   /* 2 terms */ \
	r4 = tensor_NEXT_TERM;   /* 51 */ \
	r5 = tensor_NEXT_TERM;   /* 52 */ \
	r4 += r5;   /* 2 terms */ \
	r3 += r4;   /* 4 terms */ \
	r4 = tensor_NEXT_TERM;   /* 53 */ \
	r5 = tensor_NEXT_TERM;   /* 54 */ \
	r4 += r5;   /* 2 terms */ \
	r5 = tensor_NEXT_TERM;   /* 55 */ \
	r6 = tensor_NEXT_TERM;   /* 56 */ \
	r5 += r6;   /* 2 terms */ \
	r4 += r5;   /* 4 terms */ \
	r3 += r4;   /* 8 terms */ \
	r4 = tensor_NEXT_TERM;   /* 57 */ \
	r5 = tensor_NEXT_TERM;   /* 58 */ \
	r4 += r5;   /* 2 terms */ \
	r5 = tensor_NEXT_TERM;   /* 59 */ \
	r6 = tensor_NEXT_TERM;   /* 60 */ \
	r5 += r6;   /* 2 terms */ \
	r4 += r5;   /* 4 terms */ \
	r5 = tensor_NEXT_TERM;   /* 61 */ \
	r6 = tensor_NEXT_TERM;   /* 62 */ \
	r5 += r6;   /* 2 terms */ \
	r6 = tensor_NEXT_TERM;   /* 63 */ \
	real80 r7 = tensor_NEXT_TERM;   /* 64 */ \
	r6 += r7;   /* 2 terms */ \
	r5 += r6;   /* 4 terms */ \
	r4 += r5;   /* 8 terms */ \
	r3 += r4;   /* 16 terms */ \
	r2 += r3;   /* 32 terms */ \
	r1 += r2;   /* 64 terms */

void sum_mean_scalar (numvec x, real *p_sum, real *p_mean) noexcept {
	if (x.size <= 4) {
		switch (x.size) {
			case 0: {
				if (p_sum) *p_sum = 0.0;
				if (p_mean) *p_mean = undefined;
			} break; case 1: {
				if (p_sum) *p_sum = x [1];
				if (p_mean) *p_mean = x [1];
			} break; case 2: {
				real80 sum = (real80) x [1] + (real80) x [2];
				if (p_sum) *p_sum = (real) sum;
				if (p_mean) *p_mean = real (0.5 * sum);
			} break; case 3: {
				real80 sum = (real80) x [1] + (real80) x [2] + (real80) x [3];
				if (p_sum) *p_sum = (real) sum;
				if (p_mean) *p_mean = real (sum / 3.0);
			} break; case 4: {
				real80 sum = ((real80) x [1] + (real80) x [2]) + ((real80) x [3] + (real80) x [4]);
				if (p_sum) *p_sum = (real) sum;
				if (p_mean) *p_mean = real (0.25 * sum);
			} break; default: {
				if (p_sum) *p_sum = undefined;
				if (p_mean) *p_mean = undefined;
			}
		}
		return;
	}
	if (Melder_debug != 0) {
		if (Melder_debug == 48) {
			/*
				Naive implementation in real64.
			*/
			real sum = 0.0;   // -> sum in R (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				sum += x [i];   // sum before in R, x [i] in R -> sum after in R
			}
			if (p_sum) *p_sum = sum;
			real mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
			if (p_mean) *p_mean = mean;
			return;
		}
		if (Melder_debug == 49) {
			/*
				Naive implementation in real80.
			*/
			real80 sum = 0.0;   // -> sum in R (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				sum += (real80) x [i];   // sum before in R, x [i] in R -> sum after in R
			}
			if (p_sum) *p_sum = (real) sum;
			real80 mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
			if (p_mean) *p_mean = (real) mean;
			return;
		}
		if (Melder_debug == 50) {
			/*
				First-element offset corrects for large DC components.
			*/
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
			return;
		}
		if (Melder_debug == 51) {
			real80 sum = 0.0;   // -> sum in R (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				sum += x [i];   // sum before in R, x [i] in R -> sum after in R
			}
			if (p_sum) *p_sum = (real) sum;
			real80 mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
			if (p_mean) {
				real80 sum2 = 0.0;
				for (integer i = 1; i <= x.size; i ++) {
					sum2 += (real80) x [i] - mean;
				}
				*p_mean = real (mean + sum2 / x.size);
			}
			return;
		}
		if (Melder_debug == 52) {
			/*
				Pairwise algorithm with base case 8. For an explanation see the base case 64 case.
			*/
			constexpr integer baseCasePower = 3;
			constexpr integer baseCaseSize = 1 << baseCasePower;
			integer numberOfBaseCases = x.size / baseCaseSize, remainder = x.size % baseCaseSize;
			real80 sum = 0.0;
			real *y = x.at;
			#define tensor_NEXT_TERM  (++ y, (real80) *y)
			if (remainder & 1)  { tensor_ADD_1_TERM   sum += r1; }
			if (remainder & 2)  { tensor_ADD_2_TERMS  sum += r1; }
			if (remainder & 4)  { tensor_ADD_4_TERMS  sum += r1; }
			if (numberOfBaseCases != 0) {
				constexpr integer highestIndex = 63 - baseCasePower;
				integer numbersOfTerms [1 + highestIndex];
				real80 partialSums [1 + highestIndex];
				numbersOfTerms [0] = 0;
				integer stackPointer = 0;
				for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
					tensor_ADD_8_TERMS
					partialSums [++ stackPointer] = r1;
					numbersOfTerms [stackPointer] = baseCaseSize;
					while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
						numbersOfTerms [-- stackPointer] *= 2;
						partialSums [stackPointer] += partialSums [stackPointer + 1];
					}
				}
				for (integer i = stackPointer; i > 0; i --) {
					sum += partialSums [i];
				}
			}
			#undef tensor_NEXT_TERM
			if (p_sum) *p_sum = (real) sum;
			if (p_mean) {
				real80 mean = sum / x.size;
				*p_mean = (real) mean;
			}
			return;
		}
		if (Melder_debug == 53) {
			/*
				Pairwise algorithm with base case 16. For an explanation see the base case 64 case.
			*/
			constexpr integer baseCasePower = 4;
			constexpr integer baseCaseSize = 1 << baseCasePower;
			integer numberOfBaseCases = x.size / baseCaseSize, remainder = x.size % baseCaseSize;
			real80 sum = 0.0;
			real *y = x.at;
			#define tensor_NEXT_TERM  (++ y, (real80) *y)
			if (remainder & 1)  { tensor_ADD_1_TERM   sum += r1; }
			if (remainder & 2)  { tensor_ADD_2_TERMS  sum += r1; }
			if (remainder & 4)  { tensor_ADD_4_TERMS  sum += r1; }
			if (remainder & 8)  { tensor_ADD_8_TERMS  sum += r1; }
			if (numberOfBaseCases != 0) {
				constexpr integer highestIndex = 63 - baseCasePower;
				integer numbersOfTerms [1 + highestIndex];
				real80 partialSums [1 + highestIndex];
				numbersOfTerms [0] = 0;
				integer stackPointer = 0;
				for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
					tensor_ADD_16_TERMS
					partialSums [++ stackPointer] = r1;
					numbersOfTerms [stackPointer] = baseCaseSize;
					while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
						numbersOfTerms [-- stackPointer] *= 2;
						partialSums [stackPointer] += partialSums [stackPointer + 1];
					}
				}
				for (integer i = stackPointer; i > 0; i --) {
					sum += partialSums [i];
				}
			}
			#undef tensor_NEXT_TERM
			if (p_sum) *p_sum = (real) sum;
			if (p_mean) {
				real80 mean = sum / x.size;
				*p_mean = (real) mean;
			}
			return;
		}
		if (Melder_debug == 54) {
			/*
				Pairwise algorithm with base case 32. For an explanation see the base case 64 case.
			*/
			constexpr integer baseCasePower = 5;
			constexpr integer baseCaseSize = 1 << baseCasePower;
			integer numberOfBaseCases = x.size / baseCaseSize, remainder = x.size % baseCaseSize;
			real80 sum = 0.0;
			real *y = x.at;
			#define tensor_NEXT_TERM  (++ y, (real80) *y)
			if (remainder & 1)  { tensor_ADD_1_TERM    sum += r1; }
			if (remainder & 2)  { tensor_ADD_2_TERMS   sum += r1; }
			if (remainder & 4)  { tensor_ADD_4_TERMS   sum += r1; }
			if (remainder & 8)  { tensor_ADD_8_TERMS   sum += r1; }
			if (remainder & 16) { tensor_ADD_16_TERMS  sum += r1; }
			if (numberOfBaseCases != 0) {
				constexpr integer highestIndex = 63 - baseCasePower;
				integer numbersOfTerms [1 + highestIndex];
				real80 partialSums [1 + highestIndex];
				numbersOfTerms [0] = 0;
				integer stackPointer = 0;
				for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
					tensor_ADD_32_TERMS
					partialSums [++ stackPointer] = r1;
					numbersOfTerms [stackPointer] = baseCaseSize;
					while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
						numbersOfTerms [-- stackPointer] *= 2;
						partialSums [stackPointer] += partialSums [stackPointer + 1];
					}
				}
				for (integer i = stackPointer; i > 0; i --) {
					sum += partialSums [i];
				}
			}
			#undef tensor_NEXT_TERM
			if (p_sum) *p_sum = (real) sum;
			if (p_mean) {
				real80 mean = sum / x.size;
				*p_mean = (real) mean;
			}
			return;
		}
	}
	/*
		Our standard: pairwise algorithm with base case size 64 (if baseCasePower is 6).

		If you want to change the base case size, do the following three things:
		1. Change the `constexpr integer baseCasePower = 6` assignment (e.g. to 7).
		2. Change the number of bit tests for remainder (e.g. up to `remainder & 64`).
		3. Change the `tensor_ADD_64_TERMS` statement (e.g. to tensor_ADD_128_TERMS).
	*/
	constexpr integer baseCasePower = 6;
	constexpr integer baseCaseSize = 1 << baseCasePower;
	integer numberOfBaseCases = x.size / baseCaseSize, remainder = x.size % baseCaseSize;
	real80 sum = 0.0;
	real *y = x.at;
	#define tensor_NEXT_TERM  (++ y, (real80) *y)
	if (remainder & 1)  { tensor_ADD_1_TERM    sum += r1; }
	if (remainder & 2)  { tensor_ADD_2_TERMS   sum += r1; }
	if (remainder & 4)  { tensor_ADD_4_TERMS   sum += r1; }
	if (remainder & 8)  { tensor_ADD_8_TERMS   sum += r1; }
	if (remainder & 16) { tensor_ADD_16_TERMS  sum += r1; }
	if (remainder & 32) { tensor_ADD_32_TERMS  sum += r1; }
	if (numberOfBaseCases != 0) {
		/*
			The value of numbersOfTerms [0] stays at 0, to denote the bottom of the stack.
			The maximum value of numbersOfTerms [1] should be 2^62, because x.size can be at most 2^63-1 (if sizeof integer is 64).
			The maximum value of numbersOfTerms [2] should then be 2^61.
			The maximum value of numbersOfTerms [3] should be 2^60.
			...
			The maximum value of numbersOfTerms [57] should be 2^6, which is the granularity with which base case sums are put on the stack.
			The maximum value of numbersOfTerms [58] should also be 2^6, because this can be the situation just before collapsing the top of the stack.
			However, if the whole stack is filled up like this, the actual number of terms is already 2^63. Therefore, we need one element less.
			So the highest index of numbersOfTerms [] should be 57.
		*/
		constexpr integer highestIndex = 63 - baseCasePower;
		integer numbersOfTerms [1 + highestIndex];
		real80 partialSums [1 + highestIndex];
		numbersOfTerms [0] = 0;   // the constant zero at the bottom of the stack
		integer stackPointer = 0;
		for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
			/*
				Compute the sum of the next 64 data points.
				Put this sum on top of the stack.
			*/
			tensor_ADD_64_TERMS
			partialSums [++ stackPointer] = r1;
			numbersOfTerms [stackPointer] = baseCaseSize;
			while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
				numbersOfTerms [-- stackPointer] *= 2;
				partialSums [stackPointer] += partialSums [stackPointer + 1];
			}
		}
		/*
			Add all the elements of the stack, starting at the top (small sums to big sums).
		*/
		for (integer i = stackPointer; i > 0; i --) {
			sum += partialSums [i];
		}
	}
	#undef tensor_NEXT_TERM
	if (p_sum) *p_sum = (real) sum;
	if (p_mean) {
		real80 mean = sum / x.size;   // it helps a bit to perform this division while still in real80
		*p_mean = (real) mean;
	}
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
	if (Melder_debug != 0) {
		if (Melder_debug == 48) {
			/*
				Naive implementation in real64.
			*/
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
			return;
		}
		if (Melder_debug == 49) {
			/*
				Naive implementation in real80.
			*/
			real80 sum = 0.0;   // -> sum in R (invariant)
			for (integer i = 1; i <= x.size; i ++) {
				sum += (real80) x [i];   // sum before in R, x [i] in R -> sum after in R
			}
			if (p_sum) *p_sum = (real) sum;
			real80 mean = sum / x.size;   // sum in R, x.size != 0 -> mean in R
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
			return;
		}
		if (Melder_debug == 50) {
			/*
				First-element offset corrects for large DC components.
			*/
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
			return;
		}
		if (Melder_debug == 51) {
			real sum, mean;
			sum_mean_scalar (x, & sum, & mean);
			if (p_sum) *p_sum = sum;
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
			return;
		}
		if (Melder_debug == 52) {
			real mean;
			sum_mean_scalar (x, p_sum, & mean);
			if (p_mean) *p_mean = mean;
			if (! p_sumsq && ! p_variance && ! p_stdev) {
				return;
			}
			constexpr integer baseCasePower = 3;
			constexpr integer baseCaseSize = 1 << baseCasePower;
			integer numberOfBaseCases = x.size / baseCaseSize, remainder = x.size % baseCaseSize;
			real80 sumsq = 0.0;
			real *y = x.at;
			#define tensor_NEXT_TERM  (++ y, real80 (*y - mean) * real80 (*y - mean))
			if (remainder & 1)  { tensor_ADD_1_TERM   sumsq += r1; }
			if (remainder & 2)  { tensor_ADD_2_TERMS  sumsq += r1; }
			if (remainder & 4)  { tensor_ADD_4_TERMS  sumsq += r1; }
			if (numberOfBaseCases != 0) {
				constexpr integer highestIndex = 63 - baseCasePower;
				integer numbersOfTerms [1 + highestIndex];
				real80 partialSums [1 + highestIndex];
				numbersOfTerms [0] = 0;
				integer stackPointer = 0;
				for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
					tensor_ADD_8_TERMS
					partialSums [++ stackPointer] = r1;
					numbersOfTerms [stackPointer] = baseCaseSize;
					while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
						numbersOfTerms [-- stackPointer] *= 2;
						partialSums [stackPointer] += partialSums [stackPointer + 1];
					}
				}
				for (integer i = stackPointer; i > 0; i --) {
					sumsq += partialSums [i];
				}
			}
			#undef tensor_NEXT_TERM
			real variance = (real) sumsq / (x.size - 1);
			if (p_sumsq) *p_sumsq = (real) sumsq;
			if (p_variance) *p_variance = variance;
			if (p_stdev) *p_stdev = sqrt (variance);
			return;
		}
		if (Melder_debug == 53) {
			real mean;
			sum_mean_scalar (x, p_sum, & mean);
			if (p_mean) *p_mean = mean;
			if (! p_sumsq && ! p_variance && ! p_stdev) {
				return;
			}
			constexpr integer baseCasePower = 4;
			constexpr integer baseCaseSize = 1 << baseCasePower;
			integer numberOfBaseCases = x.size / baseCaseSize, remainder = x.size % baseCaseSize;
			real80 sumsq = 0.0;
			real *y = x.at;
			#define tensor_NEXT_TERM  (++ y, real80 (*y - mean) * real80 (*y - mean))
			if (remainder & 1)  { tensor_ADD_1_TERM   sumsq += r1; }
			if (remainder & 2)  { tensor_ADD_2_TERMS  sumsq += r1; }
			if (remainder & 4)  { tensor_ADD_4_TERMS  sumsq += r1; }
			if (remainder & 8)  { tensor_ADD_8_TERMS  sumsq += r1; }
			if (numberOfBaseCases != 0) {
				constexpr integer highestIndex = 63 - baseCasePower;
				integer numbersOfTerms [1 + highestIndex];
				real80 partialSums [1 + highestIndex];
				numbersOfTerms [0] = 0;
				integer stackPointer = 0;
				for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
					tensor_ADD_16_TERMS
					partialSums [++ stackPointer] = r1;
					numbersOfTerms [stackPointer] = baseCaseSize;
					while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
						numbersOfTerms [-- stackPointer] *= 2;
						partialSums [stackPointer] += partialSums [stackPointer + 1];
					}
				}
				for (integer i = stackPointer; i > 0; i --) {
					sumsq += partialSums [i];
				}
			}
			#undef tensor_NEXT_TERM
			real variance = (real) sumsq / (x.size - 1);
			if (p_sumsq) *p_sumsq = (real) sumsq;
			if (p_variance) *p_variance = variance;
			if (p_stdev) *p_stdev = sqrt (variance);
			return;
		}
		if (Melder_debug == 54) {
			real mean;
			sum_mean_scalar (x, p_sum, & mean);
			if (p_mean) *p_mean = mean;
			if (! p_sumsq && ! p_variance && ! p_stdev) {
				return;
			}
			constexpr integer baseCasePower = 5;
			constexpr integer baseCaseSize = 1 << baseCasePower;
			integer numberOfBaseCases = x.size / baseCaseSize, remainder = x.size % baseCaseSize;
			real80 sumsq = 0.0;
			real *y = x.at;
			#define tensor_NEXT_TERM  (++ y, real80 (*y - mean) * real80 (*y - mean))
			if (remainder & 1)  { tensor_ADD_1_TERM    sumsq += r1; }
			if (remainder & 2)  { tensor_ADD_2_TERMS   sumsq += r1; }
			if (remainder & 4)  { tensor_ADD_4_TERMS   sumsq += r1; }
			if (remainder & 8)  { tensor_ADD_8_TERMS   sumsq += r1; }
			if (remainder & 16) { tensor_ADD_16_TERMS  sumsq += r1; }
			if (numberOfBaseCases != 0) {
				constexpr integer highestIndex = 63 - baseCasePower;
				integer numbersOfTerms [1 + highestIndex];
				real80 partialSums [1 + highestIndex];
				numbersOfTerms [0] = 0;
				integer stackPointer = 0;
				for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
					tensor_ADD_32_TERMS
					partialSums [++ stackPointer] = r1;
					numbersOfTerms [stackPointer] = baseCaseSize;
					while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
						numbersOfTerms [-- stackPointer] *= 2;
						partialSums [stackPointer] += partialSums [stackPointer + 1];
					}
				}
				for (integer i = stackPointer; i > 0; i --) {
					sumsq += partialSums [i];
				}
			}
			#undef tensor_NEXT_TERM
			real variance = (real) sumsq / (x.size - 1);
			if (p_sumsq) *p_sumsq = (real) sumsq;
			if (p_variance) *p_variance = variance;
			if (p_stdev) *p_stdev = sqrt (variance);
			return;
		}
	}
	/*
		Our standard: pairwise algorithm with base case 64.
	*/
	real mean;
	sum_mean_scalar (x, p_sum, & mean);   // compute the sum only if the user asks for it, but the mean always, because we need it here
	if (p_mean) *p_mean = mean;
	if (! p_sumsq && ! p_variance && ! p_stdev) {
		return;
	}
	constexpr integer baseCasePower = 6;
	constexpr integer baseCaseSize = 1 << baseCasePower;
	integer numberOfBaseCases = x.size / baseCaseSize, remainder = x.size % baseCaseSize;
	real80 sumsq = 0.0;
	real *y = x.at;
	#define tensor_NEXT_TERM  (++ y, real80 (*y - mean) * real80 (*y - mean))
	if (remainder & 1)  { tensor_ADD_1_TERM    sumsq += r1; }
	if (remainder & 2)  { tensor_ADD_2_TERMS   sumsq += r1; }
	if (remainder & 4)  { tensor_ADD_4_TERMS   sumsq += r1; }
	if (remainder & 8)  { tensor_ADD_8_TERMS   sumsq += r1; }
	if (remainder & 16) { tensor_ADD_16_TERMS  sumsq += r1; }
	if (remainder & 32) { tensor_ADD_32_TERMS  sumsq += r1; }
	if (numberOfBaseCases != 0) {
		constexpr integer highestIndex = 63 - baseCasePower;
		integer numbersOfTerms [1 + highestIndex];
		real80 partialSums [1 + highestIndex];
		numbersOfTerms [0] = 0;
		integer stackPointer = 0;
		for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
			tensor_ADD_64_TERMS
			partialSums [++ stackPointer] = r1;
			numbersOfTerms [stackPointer] = baseCaseSize;
			while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
				numbersOfTerms [-- stackPointer] *= 2;
				partialSums [stackPointer] += partialSums [stackPointer + 1];
			}
		}
		for (integer i = stackPointer; i > 0; i --) {
			sumsq += partialSums [i];
		}
	}
	#undef tensor_NEXT_TERM
	real variance = (real) sumsq / (x.size - 1);
	if (p_sumsq) *p_sumsq = (real) sumsq;
	if (p_variance) *p_variance = variance;
	if (p_stdev) *p_stdev = sqrt (variance);
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
	for (integer i = 1; i <= x.size; i ++) {
		weightedSumOfIndexes += i * x [i];
		sumOfWeights += x [i];
	}
	return weightedSumOfIndexes / sumOfWeights;
}

real norm_scalar (numvec x, real power) noexcept {
	if (power < 0.0) return undefined;
	constexpr integer baseCasePower = 6;
	constexpr integer baseCaseSize = 1 << baseCasePower;
	integer numberOfBaseCases = x.size / baseCaseSize, remainder = x.size % baseCaseSize;
	real80 sum = 0.0;
	real *y = x.at;
	if (power == 2.0) {
		#define tensor_NEXT_TERM  (++ y, (real80) *y * (real80) *y)
		if (remainder & 1)  { tensor_ADD_1_TERM    sum += r1; }
		if (remainder & 2)  { tensor_ADD_2_TERMS   sum += r1; }
		if (remainder & 4)  { tensor_ADD_4_TERMS   sum += r1; }
		if (remainder & 8)  { tensor_ADD_8_TERMS   sum += r1; }
		if (remainder & 16) { tensor_ADD_16_TERMS  sum += r1; }
		if (remainder & 32) { tensor_ADD_32_TERMS  sum += r1; }
		if (numberOfBaseCases != 0) {
			constexpr integer highestIndex = 63 - baseCasePower;
			integer numbersOfTerms [1 + highestIndex];
			real80 partialSums [1 + highestIndex];
			numbersOfTerms [0] = 0;
			integer stackPointer = 0;
			for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
				tensor_ADD_64_TERMS
				partialSums [++ stackPointer] = r1;
				numbersOfTerms [stackPointer] = baseCaseSize;
				while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
					numbersOfTerms [-- stackPointer] *= 2;
					partialSums [stackPointer] += partialSums [stackPointer + 1];
				}
			}
			for (integer i = stackPointer; i > 0; i --) {
				sum += partialSums [i];
			}
		}
		#undef tensor_NEXT_TERM
		return sqrt ((real) sum);
	} else if (power == 1.0) {
		#define tensor_NEXT_TERM  (++ y, (real80) fabs (*y))
		if (remainder & 1)  { tensor_ADD_1_TERM    sum += r1; }
		if (remainder & 2)  { tensor_ADD_2_TERMS   sum += r1; }
		if (remainder & 4)  { tensor_ADD_4_TERMS   sum += r1; }
		if (remainder & 8)  { tensor_ADD_8_TERMS   sum += r1; }
		if (remainder & 16) { tensor_ADD_16_TERMS  sum += r1; }
		if (remainder & 32) { tensor_ADD_16_TERMS  sum += r1; }
		if (numberOfBaseCases != 0) {
			constexpr integer highestIndex = 63 - baseCasePower;
			integer numbersOfTerms [1 + highestIndex];
			real80 partialSums [1 + highestIndex];
			numbersOfTerms [0] = 0;
			integer stackPointer = 0;
			for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
				tensor_ADD_64_TERMS
				partialSums [++ stackPointer] = r1;
				numbersOfTerms [stackPointer] = baseCaseSize;
				while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
					numbersOfTerms [-- stackPointer] *= 2;
					partialSums [stackPointer] += partialSums [stackPointer + 1];
				}
			}
			for (integer i = stackPointer; i > 0; i --) {
				sum += partialSums [i];
			}
		}
		#undef tensor_NEXT_TERM
		return (real) sum;
	} else {
		#define tensor_NEXT_TERM  (++ y, powl ((real80) fabs (*y), power))
		if (remainder & 1)  { tensor_ADD_1_TERM    sum += r1; }
		if (remainder & 2)  { tensor_ADD_2_TERMS   sum += r1; }
		if (remainder & 4)  { tensor_ADD_4_TERMS   sum += r1; }
		if (remainder & 8)  { tensor_ADD_8_TERMS   sum += r1; }
		if (remainder & 16) { tensor_ADD_16_TERMS  sum += r1; }
		if (remainder & 32) { tensor_ADD_16_TERMS  sum += r1; }
		if (numberOfBaseCases != 0) {
			constexpr integer highestIndex = 63 - baseCasePower;
			integer numbersOfTerms [1 + highestIndex];
			real80 partialSums [1 + highestIndex];
			numbersOfTerms [0] = 0;
			integer stackPointer = 0;
			for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
				tensor_ADD_64_TERMS
				partialSums [++ stackPointer] = r1;
				numbersOfTerms [stackPointer] = baseCaseSize;
				while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
					numbersOfTerms [-- stackPointer] *= 2;
					partialSums [stackPointer] += partialSums [stackPointer + 1];
				}
			}
			for (integer i = stackPointer; i > 0; i --) {
				sum += partialSums [i];
			}
		}
		#undef tensor_NEXT_TERM
		return (real) powl (sum, (real80) 1.0 / power);
	}
}

autonumvec copy_numvec (numvec x) {
	autonumvec result (x.size, false);
	for (integer i = 1; i <= x.size; i ++) {
		result [i] = x [i];
	}
	return result;
}

autonummat copy_nummat (nummat x) {
	autonummat result (x.nrow, x.ncol, false);
	for (integer irow = 1; irow <= x.nrow; irow ++) {
		for (integer icol = 1; icol <= x.ncol; icol ++) {
			result [irow] [icol] = x [irow] [icol];
		}
	}
	return result;
}

autonummat outer_nummat (numvec x, numvec y) {
	autonummat result (x.size, y.size, false);
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
	autonummat result (2, numberOfPeaks, false);
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
				real dy = 0.5 * (x [i + 1] - x [i - 1]);
				real d2y = (x [i] - x [i - 1]) + (x [i] - x [i + 1]);
				Melder_assert (d2y > 0.0);
				result [1] [peakNumber] = (real) i + dy / d2y;
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

real _inner_scalar (numvec x, numvec y) {
	if (x.size != y.size) return undefined;
	constexpr integer baseCasePower = 6;
	constexpr integer baseCaseSize = 1 << baseCasePower;
	integer numberOfBaseCases = x.size / baseCaseSize, remainder = x.size % baseCaseSize;
	real80 sum = 0.0;
	real *xx = x.at, *yy = y.at;
	#define tensor_NEXT_TERM  (++ xx, ++ yy, ((real80) *xx * (real80) *yy))
	if (remainder & 1)  { tensor_ADD_1_TERM    sum += r1; }
	if (remainder & 2)  { tensor_ADD_2_TERMS   sum += r1; }
	if (remainder & 4)  { tensor_ADD_4_TERMS   sum += r1; }
	if (remainder & 8)  { tensor_ADD_8_TERMS   sum += r1; }
	if (remainder & 16) { tensor_ADD_16_TERMS  sum += r1; }
	if (remainder & 32) { tensor_ADD_32_TERMS  sum += r1; }
	if (numberOfBaseCases != 0) {
		constexpr integer highestIndex = 63 - baseCasePower;
		integer numbersOfTerms [1 + highestIndex];
		real80 partialSums [1 + highestIndex];
		numbersOfTerms [0] = 0;
		integer stackPointer = 0;
		for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
			tensor_ADD_64_TERMS
			partialSums [++ stackPointer] = r1;
			numbersOfTerms [stackPointer] = baseCaseSize;
			while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
				numbersOfTerms [-- stackPointer] *= 2;
				partialSums [stackPointer] += partialSums [stackPointer + 1];
			}
		}
		for (integer i = stackPointer; i > 0; i --) {
			sum += partialSums [i];
		}
	}
	#undef tensor_NEXT_TERM
	return (real) sum;
}

static real _inner_stride_scalar (numvec x, numvec y, integer stride) {
	if (x.size != y.size) return undefined;
	constexpr integer baseCasePower = 6;
	constexpr integer baseCaseSize = 1 << baseCasePower;
	integer numberOfBaseCases = x.size / baseCaseSize, remainder = x.size % baseCaseSize;
	real80 sum = 0.0;
	real *xx = x.at, *yy = y.at - stride + 1;
	#define tensor_NEXT_TERM  (++ xx, yy += stride, (real80) *xx * (real80) *yy)
	if (remainder & 1)  { tensor_ADD_1_TERM    sum += r1; }
	if (remainder & 2)  { tensor_ADD_2_TERMS   sum += r1; }
	if (remainder & 4)  { tensor_ADD_4_TERMS   sum += r1; }
	if (remainder & 8)  { tensor_ADD_8_TERMS   sum += r1; }
	if (remainder & 16) { tensor_ADD_16_TERMS  sum += r1; }
	if (remainder & 32) { tensor_ADD_32_TERMS  sum += r1; }
	if (numberOfBaseCases != 0) {
		constexpr integer highestIndex = 63 - baseCasePower;
		integer numbersOfTerms [1 + highestIndex];
		real80 partialSums [1 + highestIndex];
		numbersOfTerms [0] = 0;
		integer stackPointer = 0;
		for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) {
			tensor_ADD_64_TERMS
			partialSums [++ stackPointer] = r1;
			numbersOfTerms [stackPointer] = baseCaseSize;
			while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) {
				numbersOfTerms [-- stackPointer] *= 2;
				partialSums [stackPointer] += partialSums [stackPointer + 1];
			}
		}
		for (integer i = stackPointer; i > 0; i --) {
			sum += partialSums [i];
		}
	}
	#undef tensor_NEXT_TERM
	return (real) sum;
}

inline static void mul_inline (numvec target, numvec vec, nummat mat) {
	for (integer j = 1; j <= mat.ncol; j ++) {
		if ((false)) {
			target [j] = 0.0;
			for (integer i = 1; i <= mat.nrow; i ++) {
				target [j] += vec [i] * mat [i] [j];
			}
		} else {
			target [j] = _inner_stride_scalar (vec, { & mat [1] [j] - 1, mat.nrow }, mat.ncol);
		}
	}
}

inline static void mul_inline (numvec target, nummat mat, numvec vec) {
	for (integer i = 1; i <= mat.nrow; i ++) {
		if ((false)) {
			target [i] = 0.0;
			for (integer j = 1; j <= vec.size; j ++) {
				target [i] += mat [i] [j] * vec [j];
			}
		} else {
			target [i] = inner_scalar ({ & mat [i] [1] - 1, mat.ncol }, vec);
		}
	}
}

autonumvec mul_numvec (numvec vec, nummat mat) {
	if (mat.nrow != vec.size) return autonumvec { nullptr, 0 };
	autonumvec result { mat.ncol, false };
	mul_inline (result.get(), vec, mat);
	return result;
}

autonumvec mul_numvec (nummat mat, numvec vec) {
	if (vec.size != mat.ncol) return autonumvec { nullptr, 0 };
	autonumvec result { mat.nrow, false };
	mul_inline (result.get(), mat, vec);
	return result;
}

/* End of file tensor.cpp */
