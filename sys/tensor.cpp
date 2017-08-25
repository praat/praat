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
#include "NUM2.h"   /* for NUMsort2 */

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
	
	How would you sum the four numbers a, b, c and d?
	
	The naive way would be to write in C:
(1)		sum = a + b + c + d;
	which means the same as
(2)		sum = ((a + b) + c) + d;
	In machine-code-like C, using the "registers" r1 and r2, you could implement this as
(3)		r1 = a;
		r2 = b;
		r1 += r2;
		r2 = c;
		r1 += r2;
		r2 = d;
		r1 += r2;
		sum = r1;
	Both formulation (2) and (3) lead to identical true machine code,
	at least with clang or gcc, and with optimization option -O3.
	
	An alternative way to add the four numbers is to proceed in a *pairwise* manner,
	dividing the four numbers into two groups, then summing these separately,
	then summing the two results. In short C this would be:
(4)		sum = (a + b) + (c + d);
	In machine-code-like C, using the "registers" r1, r2 and r3, you can implement this as
(5)		r1 = a;
		r2 = b;
		r1 += r2;
		r2 = c;   // r1 cannot be reused (its value will be needed three lines on), but r2 can
		r3 = d;
		r2 += r3;
		r1 += r2;
		sum = r1;
	The number of loads, stores and additions is identical to those in the naive case;
	if anything, the pairwise formulation could be a bit faster than the naive formulation,
	because d can be loaded into r3 at the same time when c is loaded into r2
	(the naive method can be improved by using r3 and r4, but this can be done to the
	pairwise method as well, and the difference between the two methods then shifts from
	residing in the parallelism of loads to residing in the parallelism of additions).
	More importantly than execution speed, the floating-point rounding errors
	are much smaller with the pairwise method than with the naive method.
	
	Using the `nextTerm` macro to retrieve the values of a, b, c and d from memory,
	the register part of code (5) is expressed in the following macro:
*/
#define SUM_4_TERMS_INTO_R1_USING_R2_R3(nextTerm) \
	r1 = nextTerm;  r2 = nextTerm;  r1 += r2; \
	r2 = nextTerm;  r3 = nextTerm;  r2 += r3; \
	r1 += r2;

/*
	One might think that the above formulation for adding four terms costs three clock cycles
	(assuming that the fetches do not cost anything),
	because the three additions have to wait for each other.
	In that case, the following formulation would work better:
*/
#define SUM_4_TERMS_INTO_R1_USING_R2_R3_R4(nextTerm) \
	r1 = nextTerm;  r2 = nextTerm;  r1 += r2; \
	r3 = nextTerm;  r4 = nextTerm;  r3 += r4; \
	r1 += r3;
/*
	This formulation requires only two clock cycles, because the first addition to r1
	and the addition to r3 can be performed in parallel. However, both compilers
	that I am using in 2017 (Clang on the Mac, and gcc on Windows and Linux) seem to figure
	this out by themselves and seem to cause the processor to use only two clock cycles anyway.
	At least, that is what one could conclude from the speed on all three platforms,
	which is 0.30 nanoseconds per addition, which is just over two-thirds
	of the clock period of the 2.3 GHz processor (which is 43.5 nanoseconds).
	
	So "optimizations" like this one do not really seem to be needed in 2017.
*/

/*
	To sum 8 terms instead of 4, we repeat the addition, now summing the next four terms,
	but they cannot be accumulated in r1, because the value of r1 (which contains the sum of
	the first four terms) will have to survive the second addition.
	Registers r2, r3 and r4, though, can be reused, and we accumulate the four terms in r2:
*/
#define SUM_4_TERMS_INTO_R2_USING_R3_R4(nextTerm) \
	r2 = nextTerm;  r3 = nextTerm;  r2 += r3; \
	r3 = nextTerm;  r4 = nextTerm;  r3 += r4; \
	r2 += r3;

/*
	Now that r1 and r2 both contain four terms, we can add them into r1:
*/
#define SUM_8_TERMS_INTO_R1_USING_R2_R3_R4(nextTerm) \
	SUM_4_TERMS_INTO_R1_USING_R2_R3 (nextTerm) \
	SUM_4_TERMS_INTO_R2_USING_R3_R4 (nextTerm) \
	r1 += r2;

/*
	This procedure can continue recursively.
	To sum 16 terms, we need three more macros:
*/

#define SUM_4_TERMS_INTO_R3_USING_R4_R5(nextTerm) \
	r3 = nextTerm;  r4 = nextTerm;  r3 += r4; \
	r4 = nextTerm;  r5 = nextTerm;  r4 += r5; \
	r3 += r4;

#define SUM_8_TERMS_INTO_R2_USING_R3_R4_R5(nextTerm) \
	SUM_4_TERMS_INTO_R2_USING_R3_R4 (nextTerm) \
	SUM_4_TERMS_INTO_R3_USING_R4_R5 (nextTerm) \
	r2 += r3;

#define SUM_16_TERMS_INTO_R1_USING_R2_R3_R4_R5(nextTerm) \
	SUM_8_TERMS_INTO_R1_USING_R2_R3_R4 (nextTerm) \
	SUM_8_TERMS_INTO_R2_USING_R3_R4_R5 (nextTerm) \
	r1 += r2;

/*
	To sum 32 terms, we need four more macros:
*/

#define SUM_4_TERMS_INTO_R4_USING_R5_R6(nextTerm) \
	r4 = nextTerm;  r5 = nextTerm;  r4 += r5; \
	r5 = nextTerm;  r6 = nextTerm;  r5 += r6; \
	r4 += r5;

#define SUM_8_TERMS_INTO_R3_USING_R4_R5_R6(nextTerm) \
	SUM_4_TERMS_INTO_R3_USING_R4_R5 (nextTerm) \
	SUM_4_TERMS_INTO_R4_USING_R5_R6 (nextTerm) \
	r3 += r4;

#define SUM_16_TERMS_INTO_R2_USING_R3_R4_R5_R6(nextTerm) \
	SUM_8_TERMS_INTO_R2_USING_R3_R4_R5 (nextTerm) \
	SUM_8_TERMS_INTO_R3_USING_R4_R5_R6 (nextTerm) \
	r2 += r3;

#define SUM_32_TERMS_INTO_R1_USING_R2_R3_R4_R5_R6(nextTerm) \
	SUM_16_TERMS_INTO_R1_USING_R2_R3_R4_R5 (nextTerm) \
	SUM_16_TERMS_INTO_R2_USING_R3_R4_R5_R6 (nextTerm) \
	r1 += r2;

/*
	To sum 64 terms, we need five more macros:
*/

#define SUM_4_TERMS_INTO_R5_USING_R6_R7(nextTerm) \
	r5 = nextTerm;  r6 = nextTerm;  r5 += r6; \
	r6 = nextTerm;  r7 = nextTerm;  r6 += r7; \
	r5 += r6;

#define SUM_8_TERMS_INTO_R4_USING_R5_R6_R7(nextTerm) \
	SUM_4_TERMS_INTO_R4_USING_R5_R6 (nextTerm) \
	SUM_4_TERMS_INTO_R5_USING_R6_R7 (nextTerm) \
	r4 += r5;

#define SUM_16_TERMS_INTO_R3_USING_R4_R5_R6_R7(nextTerm) \
	SUM_8_TERMS_INTO_R3_USING_R4_R5_R6 (nextTerm) \
	SUM_8_TERMS_INTO_R4_USING_R5_R6_R7 (nextTerm) \
	r3 += r4;

#define SUM_32_TERMS_INTO_R2_USING_R3_R4_R5_R6_R7(nextTerm) \
	SUM_16_TERMS_INTO_R2_USING_R3_R4_R5_R6 (nextTerm) \
	SUM_16_TERMS_INTO_R3_USING_R4_R5_R6_R7 (nextTerm) \
	r2 += r3;

#define SUM_64_TERMS_INTO_R1_USING_R2_R3_R4_R5_R6_R7(nextTerm) \
	SUM_32_TERMS_INTO_R1_USING_R2_R3_R4_R5_R6 (nextTerm) \
	SUM_32_TERMS_INTO_R2_USING_R3_R4_R5_R6_R7 (nextTerm) \
	r1 += r2;

/*
	On a processor far, far away,
	which has more than 64 registers, perfect prefetching,
	and perfectly parallel operations on independent registers,
	the following alternative may add the 64 terms within 6 clock cycles
	(six is the number of times we add something to r1;
	each of these additions has to wait for the result of the previous addition).
	In 2017, however, this formulation still runs slower by a factor of 2 to 10,
	depending on the processor and the compiler.
*/
#define SUM_64_TERMS_INTO_R1_USING_R2_TRHOUGH_R64(nextTerm) \
	r1 = nextTerm, r2 = nextTerm, r3 = nextTerm, r4 = nextTerm, r5 = nextTerm, r6 = nextTerm, r7 = nextTerm; real80 r8 = nextTerm; \
	real80 r9 = nextTerm, r10 = nextTerm, r11 = nextTerm, r12 = nextTerm, r13 = nextTerm, r14 = nextTerm, r15 = nextTerm, r16 = nextTerm; \
	real80 r17 = nextTerm, r18 = nextTerm, r19 = nextTerm, r20 = nextTerm, r21 = nextTerm, r22 = nextTerm, r23 = nextTerm, r24 = nextTerm; \
	real80 r25 = nextTerm, r26 = nextTerm, r27 = nextTerm, r28 = nextTerm, r29 = nextTerm, r30 = nextTerm, r31 = nextTerm, r32 = nextTerm; \
	real80 r33 = nextTerm, r34 = nextTerm, r35 = nextTerm, r36 = nextTerm, r37 = nextTerm, r38 = nextTerm, r39 = nextTerm, r40 = nextTerm; \
	real80 r41 = nextTerm, r42 = nextTerm, r43 = nextTerm, r44 = nextTerm, r45 = nextTerm, r46 = nextTerm, r47 = nextTerm, r48 = nextTerm; \
	real80 r49 = nextTerm, r50 = nextTerm, r51 = nextTerm, r52 = nextTerm, r53 = nextTerm, r54 = nextTerm, r55 = nextTerm, r56 = nextTerm; \
	real80 r57 = nextTerm, r58 = nextTerm, r59 = nextTerm, r60 = nextTerm, r61 = nextTerm, r62 = nextTerm, r63 = nextTerm, r64 = nextTerm; \
	r1 += r2, r3 += r4, r5 += r6, r7 += r8, r9 += r10, r11 += r12, r13 += r14, r15 += r16; \
	r17 += r18, r19 += r20, r21 += r22, r23 += r24, r25 += r26, r27 += r28, r29 += r30, r31 += r32; \
	r33 += r34, r35 += r36, r37 += r38, r39 += r40, r41 += r42, r43 += r44, r45 += r46, r47 += r48; \
	r49 += r50, r51 += r52, r53 += r54, r55 += r56, r57 += r58, r59 += r60, r61 += r62, r63 += r64; \
	r1 += r3, r5 += r7, r9 += r11, r13 += r15, r17 += r19, r21 += r23, r25 += r27, r29 += r31; \
	r33 += r35, r37 += r39, r41 += r43, r45 += r47, r49 += r51, r53 += r55, r57 += r59, r61 += r63; \
	r1 += r5, r9 += r13, r17 += r21, r25 += r29, r33 += r37, r41 += r45, r49 += r53, r57 += r61; \
	r1 += r9, r17 += r25, r33 += r41, r49 += r57; \
	r1 += r17, r33 += r49; \
	r1 += r33;

/*
	To show that the above idea is not totally crazy, here is an intermediate alternative,
	which runs approximately as fast (in 2017) as the formulation that uses r2 through r7.
*/
#define SUM_64_TERMS_INTO_R1_USING_R2_TRHOUGH_R13(nextTerm) \
	r1 = nextTerm, r2 = nextTerm, r3 = nextTerm, r4 = nextTerm, r5 = nextTerm, r6 = nextTerm, r7 = nextTerm; real80 r8 = nextTerm; \
	real80 r9 = ((r1 + r2) + (r3 + r4)) + ((r5 + r6) + (r7 + r8)); \
	r1 = nextTerm, r2 = nextTerm, r3 = nextTerm, r4 = nextTerm, r5 = nextTerm, r6 = nextTerm, r7 = nextTerm; r8 = nextTerm; \
	real80 r10 = ((r1 + r2) + (r3 + r4)) + ((r5 + r6) + (r7 + r8)); \
	r9 += r10; \
	r1 = nextTerm, r2 = nextTerm, r3 = nextTerm, r4 = nextTerm, r5 = nextTerm, r6 = nextTerm, r7 = nextTerm; r8 = nextTerm; \
	real80 r11 = ((r1 + r2) + (r3 + r4)) + ((r5 + r6) + (r7 + r8)); \
	r1 = nextTerm, r2 = nextTerm, r3 = nextTerm, r4 = nextTerm, r5 = nextTerm, r6 = nextTerm, r7 = nextTerm; r8 = nextTerm; \
	real80 r12 = ((r1 + r2) + (r3 + r4)) + ((r5 + r6) + (r7 + r8)); \
	r11 += r12; \
	r9 += r11; \
	r1 = nextTerm, r2 = nextTerm, r3 = nextTerm, r4 = nextTerm, r5 = nextTerm, r6 = nextTerm, r7 = nextTerm; r8 = nextTerm; \
	r10 = ((r1 + r2) + (r3 + r4)) + ((r5 + r6) + (r7 + r8)); \
	r1 = nextTerm, r2 = nextTerm, r3 = nextTerm, r4 = nextTerm, r5 = nextTerm, r6 = nextTerm, r7 = nextTerm; r8 = nextTerm; \
	r11 = ((r1 + r2) + (r3 + r4)) + ((r5 + r6) + (r7 + r8)); \
	r10 += r11; \
	r1 = nextTerm, r2 = nextTerm, r3 = nextTerm, r4 = nextTerm, r5 = nextTerm, r6 = nextTerm, r7 = nextTerm; r8 = nextTerm; \
	r12 = ((r1 + r2) + (r3 + r4)) + ((r5 + r6) + (r7 + r8)); \
	r1 = nextTerm, r2 = nextTerm, r3 = nextTerm, r4 = nextTerm, r5 = nextTerm, r6 = nextTerm, r7 = nextTerm; r8 = nextTerm; \
	real80 r13 = ((r1 + r2) + (r3 + r4)) + ((r5 + r6) + (r7 + r8)); \
	r12 += r13; \
	r10 += r12; \
	r9 += r10; \
	r1 = r9;

/*
	A generalization about the timing of the additions in all the above macros
	is that r(n+1) is added to r(n) precisely when r(n+1) contains the same
	number of terms as r(n). This criterion for collapsing the partial sums
	is also used below, where the terms are not accumulated in registers but on
	a small stack.
*/

#define tensor_SUM(sum,number,term) \
	{/* scope */ \
		integer n = number; \
		constexpr integer baseCasePower = 6; \
		constexpr integer baseCaseSize = 1 << baseCasePower; \
		integer numberOfBaseCases = n >> baseCasePower; \
		sum = 0.0; \
		if (n & 1) { \
			real80 r1 = term; \
			sum += r1; \
		} \
		if (n & 2) { \
			real80 r1 = term, r2 = term; \
			r1 += r2; \
			sum += r1; \
		} \
		if (n & 4) { \
			real80 r1, r2, r3; \
			SUM_4_TERMS_INTO_R1_USING_R2_R3 (term) \
			sum += r1; \
		} \
		if (n & 8) { \
			real80 r1, r2, r3, r4; \
			SUM_8_TERMS_INTO_R1_USING_R2_R3_R4 (term) \
			sum += r1; \
		} \
		if (n & 16) { \
			real80 r1, r2, r3, r4, r5; \
			SUM_16_TERMS_INTO_R1_USING_R2_R3_R4_R5 (term) \
			sum += r1; \
		} \
		if (n & 32) { \
			real80 r1, r2, r3, r4, r5, r6; \
			SUM_32_TERMS_INTO_R1_USING_R2_R3_R4_R5_R6 (term) \
			sum += r1; \
		} \
		if (numberOfBaseCases != 0) { \
			/*                                                                                  */ \
			/*  The value of numbersOfTerms [0] stays at 0, to denote the bottom of the stack.  */ \
			/*  The maximum value of numbersOfTerms [1] should be 2^62,                         */ \
			/*  because x.size can be at most 2^63-1 (if sizeof integer is 64).                 */ \
			/*  The maximum value of numbersOfTerms [2] should then be 2^61.                    */ \
			/*  The maximum value of numbersOfTerms [3] should be 2^60.                         */ \
			/*  ...                                                                             */ \
			/*  The maximum value of numbersOfTerms [57] should be 2^6,                         */ \
			/*  which is the granularity with which base case sums are put on the stack.        */ \
			/*  The maximum value of numbersOfTerms [58] should also be 2^6,                    */ \
			/*  because this can be the situation just before collapsing the top of the stack.  */ \
			/*  However, if the whole stack is filled up like this, the actual number of        */ \
			/*  terms is already 2^63. Therefore, we need one element less.                     */ \
			/*  So the highest index of numbersOfTerms [] should be 57.                         */ \
			/*                                                                                  */ \
			constexpr integer highestIndex = 63 - baseCasePower; \
			integer numbersOfTerms [1 + highestIndex]; \
			real80 partialSums [1 + highestIndex]; \
			numbersOfTerms [0] = 0; \
			integer stackPointer = 0; \
			for (integer ipart = 1; ipart <= numberOfBaseCases; ipart ++) { \
				/*                                                                              */ \
				/*  Compute the sum of the next 64 data points.                                 */ \
				/*                                                                              */ \
				real80 r1, r2, r3, r4, r5, r6, r7; \
				/*                                                                              */ \
				/* There is a choice between three formulation of the algorithm.                */ \
				/* In 2017, the first and third are two to ten times faster than the second,    */ \
				/* depending on the compiler and the processor.                                 */ \
				/* In the future this may change.                                               */ \
				/*                                                                              */ \
				SUM_64_TERMS_INTO_R1_USING_R2_R3_R4_R5_R6_R7 (term) \
				/*SUM_64_TERMS_INTO_R1_USING_R2_TRHOUGH_R64 (term)*/ \
				/*SUM_64_TERMS_INTO_R1_USING_R2_TRHOUGH_R13 (term)*/ \
				/*                                                                              */ \
				/*  Put this sum on top of the stack.                                           */ \
				/*                                                                              */ \
				partialSums [++ stackPointer] = r1; \
				numbersOfTerms [stackPointer] = baseCaseSize; \
				/*                                                                              */ \
				/*  The collapse criterion:                                                     */ \
				/*                                                                              */ \
				while (numbersOfTerms [stackPointer] == numbersOfTerms [stackPointer - 1]) { \
					partialSums [stackPointer - 1] += partialSums [stackPointer]; \
					numbersOfTerms [-- stackPointer] *= 2; \
				} \
			} \
			/*                                                                                  */ \
			/*  Add all the elements of the stack, starting at the top.                         */ \
			/*                                                                                  */ \
			while (stackPointer > 0) { \
				sum += partialSums [stackPointer --]; \
			} \
		} \
	}

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
	}
	real80 sum;
	real *y = x.at;
	tensor_SUM (sum, x.size, (++ y, (real80) *y))
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
	real80 sumsq;
	real *y = x.at;
	tensor_SUM (sumsq, x.size, (++ y, real80 (*y - mean) * real80 (*y - mean)))
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
	real80 sum;
	real *y = x.at;
	if (power == 2.0) {
		tensor_SUM (sum, x.size, (++ y, (real80) *y * (real80) *y))
		return sqrt ((real) sum);
	} else if (power == 1.0) {
		tensor_SUM (sum, x.size, (++ y, (real80) fabs (*y)))
		return (real) sum;
	} else {
		tensor_SUM (sum, x.size, (++ y, powl ((real80) fabs (*y), power)))
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
	real80 sum;
	real *xx = x.at, *yy = y.at;
	tensor_SUM (sum, x.size, (++ xx, ++ yy, (real80) *xx * (real80) *yy))
	return (real) sum;
}

static real _inner_stride_scalar (numvec x, numvec y, integer stride) {
	if (x.size != y.size) return undefined;
	real80 sum;
	real *xx = x.at, *yy = y.at - (stride - 1);
	tensor_SUM (sum, x.size, (++ xx, yy += stride, (real80) *xx * (real80) *yy))
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
