#ifndef _PAIRWISE_SUM_h_
#define _PAIRWISE_SUM_h_
/* PAIRWISE_SUM.h
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

		PAIRWISE_SUM (
			AccumulatorType,    // the type of the accumulator: long double, double, float
			sumVariableName,    // the variable name of the accumulator, e.g. "sum"
			CounterType,        // the type of the term counters: long, int, int64_t, intptr_t
			sizeExpression,     // the total number of terms to sum
			incrementStatement, // the statement that increments the loop pointer(s)
			termExpression      // the expression that computes each of the terms
		)

	We explain this in detail below, pedagocically starting with an analogous macro
	for the usual "sequential" addition.

	## 1. SEQUENTIAL ADDITION: A SINGLE ACCUMULATOR

	The simplest use of addition of multiple values is if you have a single array of
	floating-point data to sum. If this array is x [1..size],
	then the usual straightforward algorithm for adding the terms is

		long double sum = 0.0;
		for (long i = 1; i <= size; i ++)
			sum += x [i];
		printf ("%.17g", (double) sum);
		
	This is *sequential addition*: a single *accumulator*, namely the variable `sum`,
	accumulates every element sequentially. Note that even though the array `x` will
	typically of type `double`, the accumulator is of type `long double` in this example:
	using 80 bits for summing is much better than using 64 bits, and
	on modern computers it is usually equally fast.

	The formulation above, which uses array indexing, is equivalent to a formulation
	in terms of a "looping pointer":

		double *xx = x;   // the looping pointer
		long double sum = 0.0;
		for (long i = 1; i <= size; i ++)
			sum += * ++ xx;
		printf ("%.17g", (double) sum);

	The looping-pointer formulation consists of pre-incrementing the pointer and then
	dereferencing it. This has been idiomatic C from the 1980s on, but modern compilers
	produce equally efficient code if you separate the two steps:

		double *xx = x;   // the looping pointer
		long double sum = 0.0;
		for (long i = 1; i <= size; i ++) {
			xx += 1;   // first increment...
			sum += *xx;   // ... then dereference
		}
		printf ("%.17g", (double) sum);

	In this little algorithm we can discern the following six ingredients:
	- AccumulatorType: long double
	- sumVariableName: "sum"
	- CounterType: long
	- sizeExpression: size
	- incrementStatement: xx += 1
	- termExpression: *xx

	The algorithm can therefore be replaced with
	
		double *xx = x;   // the looping pointer
		SEQUENTIAL_SUM (long double, sum, long, size, xx += 1, *xx)
		printf ("%.17g", (double) sum);

	where the SEQUENTIAL_SUM macro is defined as:
*/
#define SEQUENTIAL_SUM(AccumulatorType,sumVariableName,CounterType,sizeExpression,incrementStatement,termExpression) \
	AccumulatorType sumVariableName = 0.0; \
	CounterType _n = sizeExpression;   /* to make sure that the size expression is evaluated only once */ \
	for (CounterType _i = 1; _i <= _n; _i ++) { \
		incrementStatement; \
		sumVariableName += termExpression; \
	}

/*
	## 2. SEQUENTIAL ADDITION IN MACHINE CODE

	How would you sum the four numbers a, b, c and d,
	if all your computer can do is adding two numbers at a time?
	
	You could write the sum in one stroke in C, without parentheses:

		sum = a + b + c + d;

	Evaluation of pluses in C proceeds from left to right, so this formulation means the same as

		sum = ((a + b) + c) + d;

	In machine-like C, using the "registers" r1, r2, r3 and r4, you could implement this as

		r1 = a;
		r2 = b;
		r1 += r2;
		r3 = c;
		r1 += r3;
		r4 = d;
		r1 += r4;
		sum = r1;

	All these three formulations lead to identical true machine code,
	at least with Clang or GCC, and with optimization option -O3.


	## 3. SPEED OF SEQUENTIAL ADDITION
	
	If your processor has good prefetching, then the four loads from RAM (moving a, b, c, and d
	into the registers r1, r2, r3 and r4) will take up no time at all.
	The three additions, however, will require three clock cycles in total:
	as all additions are performed on r1, each addition will have to wait
	until the previous addition has finished. Adding up 64 numbers will require 63 cycles.


	## 4. PRECISION OF SEQUENTIAL ADDITION
	
	The worst case floating-point rounding error for sequential addition is N * epsilon,
	where N is the number of numbers to add, and epsilon is the precision of one number.
	The expected random-walk rounding error is epsilon * sqrt (N).


	## 5. HOW TO USE PAIRWISE ADDITION

	Pairwise addition uses the exact same macro arguments as sequential addition,
	although it works several times faster and is several times more precise:
	
		double *xx = x;   // the looping pointer
		PAIRWISE_SUM (long double, sum, long, size, xx += 1, *xx)
		printf ("%.17g", (double) sum);

	The macro starts by declaring a variable of type `AccumulatorType` and name `sumVariableName`.
	Thus, the expansion of the macro effectively starts with the declaration `long double sum;`
	and the variable `sum` will be available after the macro ends,
	so that you can use it in your subsequent code, as is done here by using `(double) sum`.
	This variable is used to accumulate the resulting sum and should for best results
	therefore be capable of accumulating partial sums with little rounding error.
	For this reason, you will usually want AccumulatorType to be `long double`.
	If long doubles are not available on your platform, or if they are slow on your platform,
	you can use `double` instead, or even `float`.

	The size of the array to sum comes in in the argument `sizeExpression`, and in the expansion
	of the macro this number and all macro-internal counters are of type `CounterType`.
	In the example above I used `long` for simplicity, but you should note that on 64-bit Windows
	a `long` is only 32 bits, whereas the natural type for array indexing on a 64-bit machine
	would be `int64_t`; I therefore tend to use a CounterType of `intptr_t` myself,
	because this is 32 bits on 32-bit machines and 64 bits on 64-bit machines.
	If your `size` is an `int` (always 32 bits nowadays), you would use a CounterType of `int`;
	it is not beneficial to use a wider CounterType than the width of your `size`.

	The fifth argument of the macro is the formula you use for incrementing the loop pointer(s),
	as in `xx += 1` above. The macro uses this formula to prepare for the retrieval of
	every term in the summation.
	
	The sixth argument is an expression that should evaluate to the value of a term,
	given the current (incremented) value of the loop pointer(s), as in `*xx` above.

	More complicated use cases than a single array also exist. For instance, if you have to compute
	the inner product of the two arrays x [1..n] and y [1..n], you can do
	
		double *xx = x, *yy = y;   // two looping pointers
		PAIRWISE_SUM (long double, long, n, inner, (++ xx, ++yy), (long double) *xx * (long double) *yy)
		printf ("%.17g", (double) inner);

	Note for the fifth argument: you can see here that you can do the two increments simultaneously
	by using parentheses and a comma; fortunately, the C macro preprocessor understands enough
	about parentheses to see that you mean the fifth argument to be a single argument.

	Note for the sixth argument: as 64-bit multiplication loses a lot of the precision of its
	two 64-bit operands, it is advisable to convert both operands to `long double`
	*before* the multiplication, as is done here. This usually costs no extra computation
	time (it can actually be faster). If you do
	
		double *xx = x, *yy = y;   // two looping pointers
		NUM_SUM (long double, inner, long, n, (++ xx, ++yy), *xx * *yy)
		printf ("%.17g", (double) inner);

	instead, the conversion to `long double` is done (by the macro) *after* the multiplication,
	which is less precise.

	Other use cases include matrix multiplication, array multiplication with strides
	(`(xx += xstride, yy += ystride)`, `(long double) *xx * (long double) *yy`),
	and computing norms.


	## 6. PAIRWISE ADDITION: MULTIPLE ACCUMULATORS

	An alternative way to add the four numbers, next to sequential addition,
	is to proceed in a *pairwise* manner, dividing the four numbers into two groups,
	then summing these separately, then summing the two results. In short C this would be:

		sum = (a + b) + (c + d);

	This pairwise addition continues recursively. For instance, you add 8 numbers this way:

		sum = ((a + b) + (c + d)) + ((e + f) + (g + h));


	## 7. PAIRWISE ADDITION IN MACHINE CODE

	In machine-like C, using the "registers" r1, r2, r3 and r4, you can implement this as

		r1 = a;
		r2 = b;
		r1 += r2;
		r3 = c;
		r4 = d;
		r3 += r4;
		r1 += r3;
		sum = r1;

	The number of loads, stores and additions is identical to those in the sequential case.
	

	## 8. SPEED OF PAIRWISE ADDITION
	
	Pairwise addition can be faster than sequential addition,
	because the addition of r2 to r1 can be performed in parallel to the addition of r4 to r3.
	The three additions in section 6 could therefore be performed in two clock cycles.
	Indeed, both compilers that I am using in 2017 (Clang on the Mac, and GCC on Windows and Linux)
	take only 0.30 nanoseconds per addition, which is just over two-thirds of the clock period
	of the 2.3 GHz processor (which is 43.5 nanoseconds) of my 2014 Macbook Pro.
	On a processor far, far away, which has more than 64 registers, perfect prefetching,
	and perfectly parallel operations on independent registers, 64 terms may be added
	within 6 clock cycles: six is the number of times we need to add something to r1,
	and each of these additions has to wait for the result of the previous addition.
	Theoretically, then, but not yet in practice, the execution time has fallen from order N
	to order log(N), at least for low N (and for high N it will be of order N,
	but with a much lower multiplication factor than sequential addition has).


	## 9. PRECISION OF PAIRWISE ADDITION
	
	The worst-case rounding error is only epsilon * log (N),
	and the expected random-walk rounding error is only epsilon * sqrt (log (N)).
	Both are much better than in the sequential condition case.


	## 10. IMPLEMENTATION OF PAIRWISE ADDITION: LOW POWERS OF 2
	
	We have to implement pairwise addition with a macro,
	because the fifth and sixth arguments to PAIRWISE_SUM() have to be formulas for getting
	the next element to add. We use fixed formulas for adding 2, 4, 8, 16 or 32 terms,
	and a stack for the next 57 higher powers of 2, up to 2^62,
	so that our addition will work for N up to 2^63 - 1.
	The fixed formulas for the low powers of 2 are recursively defined macros:
*/

#define _num_SUM_1_TERM(AccumulatorType,accumulator,incrementStatement,termExpression) \
	incrementStatement; \
	accumulator = termExpression;

#define _num_SUM_2_TERMS(AccumulatorType,accumulator,incrementStatement,termExpression) \
	_num_SUM_1_TERM (AccumulatorType, accumulator, incrementStatement, termExpression); \
	{ \
		AccumulatorType _r2; \
		_num_SUM_1_TERM (AccumulatorType, _r2, incrementStatement, termExpression) \
		accumulator += _r2; \
	}

#define _num_SUM_4_TERMS(AccumulatorType,accumulator,incrementStatement,termExpression) \
	_num_SUM_2_TERMS (AccumulatorType, accumulator, incrementStatement, termExpression) \
	{ \
		AccumulatorType _r3; \
		_num_SUM_2_TERMS (AccumulatorType, _r3, incrementStatement, termExpression) \
		accumulator += _r3; \
	}

#define _num_SUM_8_TERMS(AccumulatorType,accumulator,incrementStatement,termExpression) \
	_num_SUM_4_TERMS (AccumulatorType, accumulator, incrementStatement, termExpression) \
	{ \
		AccumulatorType _r4; \
		_num_SUM_4_TERMS (AccumulatorType, _r4, incrementStatement, termExpression) \
		accumulator += _r4; \
	}

#define _num_SUM_16_TERMS(AccumulatorType,accumulator,incrementStatement,termExpression) \
	_num_SUM_8_TERMS (AccumulatorType, accumulator, incrementStatement, termExpression) \
	{ \
		AccumulatorType _r5; \
		_num_SUM_8_TERMS (AccumulatorType, _r5, incrementStatement, termExpression) \
		accumulator += _r5; \
	}

#define _num_SUM_32_TERMS(AccumulatorType,accumulator,incrementStatement,termExpression) \
	_num_SUM_16_TERMS (AccumulatorType, accumulator, incrementStatement, termExpression) \
	{ \
		AccumulatorType _r6; \
		_num_SUM_16_TERMS (AccumulatorType, _r6, incrementStatement, termExpression) \
		accumulator += _r6; \
	}
/*
	(The difference between the variable names "_r2", "_r3" and so on is not strictly needed,
	but making them the same would lead the compiler to issue warnings about shadowed variables.)
*/

/*
	## 11. IMPLEMENTATION OF PAIRWISE ADDITION: HIGH POWERS OF 2
	
	Higher powers of 2 than 32 go on a stack. We sum 64 values at each stroke,
	and this requires a fixed formula for these 64 terms:
*/
#define _num_SUM_64_TERMS(AccumulatorType,accumulator,incrementStatement,termExpression) \
	_num_SUM_32_TERMS (AccumulatorType, accumulator, incrementStatement, termExpression) \
	{ \
		AccumulatorType _r7; \
		_num_SUM_32_TERMS (AccumulatorType, _r7, incrementStatement, termExpression) \
		accumulator += _r7; \
	}

/*
	A generalization about the timing of the additions in all the above macros
	is that r(i+1) is added to r(i) precisely when r(i+1) contains the same
	number of terms as r(i). This criterion for collapsing the partial sums
	is also used in the stack logic below.
*/

#define PAIRWISE_SUM(AccumulatorType,sumVariableName,CounterType,sizeExpression,incrementStatement,termExpression) \
	AccumulatorType sumVariableName = 0.0; \
	{/* scope */ \
		CounterType _n = sizeExpression; \
		const int _baseCasePower = 6; \
		const int _baseCaseSize = 1 << _baseCasePower; \
		CounterType _numberOfBaseCases = _n >> _baseCasePower; \
		if (_n & 1) { \
			AccumulatorType _partialSum; \
			_num_SUM_1_TERM (AccumulatorType, _partialSum, incrementStatement, termExpression) \
			sumVariableName += _partialSum; \
		} \
		if (_n & 2) { \
			AccumulatorType _partialSum; \
			_num_SUM_2_TERMS (AccumulatorType, _partialSum, incrementStatement, termExpression) \
			sumVariableName += _partialSum; \
		} \
		if (_n & 4) { \
			AccumulatorType _partialSum; \
			_num_SUM_4_TERMS (AccumulatorType, _partialSum, incrementStatement, termExpression) \
			sumVariableName += _partialSum; \
		} \
		if (_n & 8) { \
			AccumulatorType _partialSum; \
			_num_SUM_8_TERMS (AccumulatorType, _partialSum, incrementStatement, termExpression) \
			sumVariableName += _partialSum; \
		} \
		if (_n & 16) { \
			AccumulatorType _partialSum; \
			_num_SUM_16_TERMS (AccumulatorType, _partialSum, incrementStatement, termExpression) \
			sumVariableName += _partialSum; \
		} \
		if (_n & 32) { \
			AccumulatorType _partialSum; \
			_num_SUM_32_TERMS (AccumulatorType, _partialSum, incrementStatement, termExpression) \
			sumVariableName += _partialSum; \
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
				_num_SUM_64_TERMS (AccumulatorType, _partialSum, incrementStatement, termExpression) \
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
				sumVariableName += _partialSumStack [_stackPointer --]; \
			} \
		} \
	}
/*
	Note that we don't do the usual trick with `do {...} while (0)` that would allow you to add
	a semicolon after the `PAIRWISE_SUM()` call. This is to prevent the suggestion that the macro
	constitutes a single statement. The macro contains a sequence of two things: the definition
	of `sum` and a long block that usually changes the value of `sum`. Hence, the macro cannot
	be used as a single statement and e.g. has to be bracketed if used in an `else` clause.
	You are therefore advised to call `PAIRWISE_SUM()` without appending the misleading semicolon.
*/

/* End of file NUM_SUM.h */
#endif
