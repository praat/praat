#ifndef _NUM_SUM_h_
#define _NUM_SUM_h_
/* NUM_SUM.h
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

/*
	# PAIRWISE ADDITION FOR C AND C++

	Recursive (pairwise) addition enhances precision and execution speed.
	The macro you will call is

		NUM_SUM (AccumulatorType, sumVariableName, termFormula, CounterType, numberOfTerms)


	## 1. USAGE

	The simplest use of the macro is if you have a single array of floating-point data to sum.
	If this array is x [1..n], then the usual straightforward algorithm for adding the terms is

		long double sum = 0.0;
		for (long i = 1; i <= n; i ++)
			sum += x [i];
		printf ("%ld", (double) sum);

	With the pairwise addition algorithm you would instead do
	
		double *xx = x;   // the looping pointer
		NUM_SUM (long double, sum, * ++ xx, intptr_t, n)
		printf ("%ld", (double) sum);

	AccumulatorType would usually be `long double`,
	because using 80 bits for summing is much better than using 64 bits (and usually equally fast).
	For a cheap version, use `double` instead, or even `float`.

	CounterType would usually be `intptr_t`,
	because we would like to sum arrays whose sizes are expressed
	in 32 bits on 32-bit machines and in 64 bits on 64-bit machines.
	For a cheap version, use `long` instead (but note that this is 32 bits on 64-bit Windows),
	or even `int`; `int64_t` also works fine on 64-bit machines.

	More complicated use cases also exist. For instance, if you have to compute
	the inner product of the two arrays x [1..n] and y [1..n], you can do
	
		double *xx = x, *yy = y;   // two looping pointers
		NUM_SUM (long double, inner, (++ xx, ++yy, (long double) *xx * (long double) *yy), intptr_t, n)
		printf ("%ld", (double) inner);


	## 2. HOW IT WORKS

	How would you sum the four numbers a, b, c and d?
	
	The sequential algorithm is what you would get when writing the sum
	in one stroke in C, without parentheses:
(1)		sum = a + b + c + d;
	which means the same as
(2)		sum = ((a + b) + c) + d;
	In machine-like C, using the "registers" r1, r2, r3 and r4, you could implement this as
(3)		r1 = a;
		r2 = b;
		r1 += r2;
		r3 = c;
		r1 += r3;
		r4 = d;
		r1 += r4;
		sum = r1;
	Both formulations (2) and (3) lead to identical true machine code,
	at least with clang or gcc, and with optimization option -O3.
	
	SPEED OF SEQUENTIAL ADDITION. If your processor has good prefetching,
	then the four loads from RAM will take up no time at all,
	but the three additions will require three clock cycles:
	as all additions are performed on r1, each addition will have to wait
	until the previous addition has finished. Adding up 64 numbers would require 63 cycles.
	
	PRECISION OF SEQUENTIAL ADDITION. The worst case floating-point rounding error is N * epsilon,
	where N is the number of numbers to add, and epsilon is the precision of one number.
	The expected random-walk rounding error is epsilon * sqrt (N).
	
	An alternative way to add the four numbers is to proceed in a *pairwise* manner,
	dividing the four numbers into two groups, then summing these separately,
	then summing the two results. In short C this would be:
(4)		sum = (a + b) + (c + d);
	In machine-like C, using the "registers" r1, r2, r3 and r4, you can implement this as
(5)		r1 = a;
		r2 = b;
		r1 += r2;
		r3 = c;
		r4 = d;
		r3 += r4;
		r1 += r3;
		sum = r1;
	The number of loads, stores and additions is identical to those in the sequential case.
	
	SPEED OF PAIRWISE ADDITION. Pairwise addition can be faster than sequential addition,
	because the addition of r2 to r1 can be performed in parallel to the addition of r4 to r3.
	The three additions in (5) could therefore be performed in two clock cycles.
	Indeed, both compilers that I am using in 2017 (Clang on the Mac, and gcc on Windows and Linux)
	take only 0.30 nanoseconds per addition, which is just over two-thirds of the clock period
	of the 2.3 GHz processor (which is 43.5 nanoseconds) of my 2014 Macbook Pro.
	On a processor far, far away, which has more than 64 registers, perfect prefetching,
	and perfectly parallel operations on independent registers, 64 terms may be added
	within 6 clock cycles: six is the number of times we need to add something to r1,
	and each of these additions has to wait for the result of the previous addition.
	Theoretically, then, but not yet in practice, the execution time has fallen from order N
	to order log(N), at least for low N (and for high N it will still be of order N,
	but with a much lower multiplication factor).

	PRECISION OF PAIRWISE ADDITION. The worst-case rounding error is only epsilon * log (N),
	and the expected random-walk rounding error is only epsilon * sqrt (log (N)).


	## 3. IMPLEMENTATION
	
	LOW POWERS OF 2. We have to implement pairwise addition with a macro,
	because the third argument to NUM_SUM() has to be a formula for getting
	the next element to add. We use fixed formulas for adding 2, 4, 8, 16 or 32 terms,
	and a stack for the next 57 higher powers of 2, up to 2^62,
	so that our addition will work for N up to 2^63 - 1.
	The fixed formulas for the low powers of 2 are recursively defined macros:
*/

#define _num_SUM_2_TERMS(AccumulatorType,accumulator,nextTerm) \
	accumulator = nextTerm; \
	{ AccumulatorType _r2 = nextTerm;  accumulator += _r2; }

#define _num_SUM_4_TERMS(AccumulatorType,accumulator,nextTerm) \
	_num_SUM_2_TERMS (AccumulatorType, accumulator, nextTerm) \
	{ AccumulatorType _r3;  _num_SUM_2_TERMS (AccumulatorType, _r3, nextTerm)  accumulator += _r3; }

#define _num_SUM_8_TERMS(AccumulatorType,accumulator,nextTerm) \
	_num_SUM_4_TERMS (AccumulatorType, accumulator, nextTerm) \
	{ AccumulatorType _r4;  _num_SUM_4_TERMS (AccumulatorType, _r4, nextTerm)  accumulator += _r4; }

#define _num_SUM_16_TERMS(AccumulatorType,accumulator,nextTerm) \
	_num_SUM_8_TERMS (AccumulatorType, accumulator, nextTerm) \
	{ AccumulatorType _r5;  _num_SUM_8_TERMS (AccumulatorType, _r5, nextTerm)  accumulator += _r5; }

#define _num_SUM_32_TERMS(AccumulatorType,accumulator,nextTerm) \
	_num_SUM_16_TERMS (AccumulatorType, accumulator, nextTerm) \
	{ AccumulatorType _r6;  _num_SUM_16_TERMS (AccumulatorType, _r6, nextTerm)  accumulator += _r6; }
/*
	(The difference between the variable names "_r2", "_r3" and so on is not strictly needed,
	but making them the same would lead the compiler to issue warnings about shadowed variables.)
*/

/*
	HIGH POWERS OF 2. Higher powers of 2 than 32 go on a stack. We sum 64 values at each stroke,
	and this requires a fixed formula for these 64 terms:
*/
#define _num_SUM_64_TERMS(AccumulatorType,accumulator,nextTerm) \
	_num_SUM_32_TERMS (AccumulatorType, accumulator, nextTerm) \
	{ AccumulatorType _r7;  _num_SUM_32_TERMS (AccumulatorType, _r7, nextTerm)  accumulator += _r7; }

/*
	A generalization about the timing of the additions in all the above macros
	is that r(i+1) is added to r(i) precisely when r(i+1) contains the same
	number of terms as r(i). This criterion for collapsing the partial sums
	is also used in the stack logic below.
*/

#define NUM_SUM(AccumulatorType,sum,term,CounterType,number) \
	AccumulatorType sum = 0.0; \
	{/* scope */ \
		CounterType _n = number; \
		const int _baseCasePower = 6; \
		const int _baseCaseSize = 1 << _baseCasePower; \
		CounterType _numberOfBaseCases = _n >> _baseCasePower; \
		if (_n & 1) { \
			AccumulatorType _partialSum = term; \
			sum += _partialSum; \
		} \
		if (_n & 2) { \
			AccumulatorType _partialSum; \
			_num_SUM_2_TERMS (AccumulatorType, _partialSum, term) \
			sum += _partialSum; \
		} \
		if (_n & 4) { \
			AccumulatorType _partialSum; \
			_num_SUM_4_TERMS (AccumulatorType, _partialSum, term) \
			sum += _partialSum; \
		} \
		if (_n & 8) { \
			AccumulatorType _partialSum; \
			_num_SUM_8_TERMS (AccumulatorType, _partialSum, term) \
			sum += _partialSum; \
		} \
		if (_n & 16) { \
			AccumulatorType _partialSum; \
			_num_SUM_16_TERMS (AccumulatorType, _partialSum, term) \
			sum += _partialSum; \
		} \
		if (_n & 32) { \
			AccumulatorType _partialSum; \
			_num_SUM_32_TERMS (AccumulatorType, _partialSum, term) \
			sum += _partialSum; \
		} \
		if (_numberOfBaseCases != 0) { \
			/*                                                                                  */ \
			/*  The value of numbersOfTerms [0] stays at 0, to denote the bottom of the stack.  */ \
			/*  The maximum value of numbersOfTerms [1] should be 2^62,                         */ \
			/*  because n can be at most 2^63-1 (assuming CounterType is 64 bits).              */ \
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
			const int _highestIndex = 8 * sizeof (CounterType) - 1 - _baseCasePower; \
			CounterType _numbersOfTerms [1 + _highestIndex]; \
			AccumulatorType _partialSumStack [1 + _highestIndex]; \
			_numbersOfTerms [0] = 0; \
			int _stackPointer = 0; \
			for (CounterType _ipart = 1; _ipart <= _numberOfBaseCases; _ipart ++) { \
				/*                                                                              */ \
				/*  Compute the sum of the next 64 data points.                                 */ \
				/*                                                                              */ \
				AccumulatorType _partialSum; \
				_num_SUM_64_TERMS (AccumulatorType, _partialSum, term) \
				/*                                                                              */ \
				/*  Put this sum on top of the stack.                                           */ \
				/*                                                                              */ \
				_partialSumStack [++ _stackPointer] = _partialSum; \
				_numbersOfTerms [_stackPointer] = _baseCaseSize; \
				/*                                                                              */ \
				/*  The collapse criterion:                                                     */ \
				/*                                                                              */ \
				while (_numbersOfTerms [_stackPointer] == _numbersOfTerms [_stackPointer - 1]) { \
					_partialSumStack [_stackPointer - 1] += _partialSumStack [_stackPointer]; \
					_numbersOfTerms [-- _stackPointer] *= 2; \
				} \
			} \
			/*                                                                                  */ \
			/*  Add all the elements of the stack, starting at the top.                         */ \
			/*                                                                                  */ \
			while (_stackPointer > 0) { \
				sum += _partialSumStack [_stackPointer --]; \
			} \
		} \
	}
/*
	Note that we don't do the usual trick with `do {...} while (0)` that would allow you to add
	a semicolon after the `NUM_SUM()` call. This is to prevent the suggestion that the macro
	constitutes a single statement. The macro contains a sequence of two things: the definition
	of `sum` and a long block that usually changes the value of `sum`. Hence, the macro cannot
	be used as a single statement and e.g. has to be bracketed if used in an `else` clause.
	You are therefore advised to call `NUM_SUM()` without appending a semicolon.
*/

/* End of file NUM_SUM.h */
#endif
