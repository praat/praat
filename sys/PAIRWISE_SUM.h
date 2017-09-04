#ifndef _PAIRWISE_SUM_h_
#define _PAIRWISE_SUM_h_
/* PAIRWISE_SUM.h
 *
 * Copyright (C) 2017 Paul Boersma <paul.boersma@uva.nl>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * (This is the two-clause BSD license, which is approximately identical to CC-BY.)
 */

/*
	# PAIRWISE SUMMATION FOR C AND C++

	The pairwise summation algorithm as implemented here is approximately 1.5 times
	faster than using the usual naive sequential summation loop,
	and is also hundreds of times more precise than sequential summation.
	Using pairwise summation instead of sequential summation will improve all your
	code for sums, means, inner products, norms, matrix multiplications,
	and CPU-based neural networks.
	
	This is not necessarily a small thing in terms of CO2.

	The macro you will call is

		PAIRWISE_SUM (
			AccumulatorType,     // the type of the accumulator: long double, double, float;
			sumVariableName,     // the variable name of the accumulator, e.g. "sum";
			CounterType,         // the type of the term counters: long, int, int64_t, intptr_t;
			sizeExpression,      // the total number of terms to sum;
			initializeStatement, // the statement that declares and initializes the loop pointer(s),
			                     // which is/are typically a pointer or pointers to the data;
			incrementStatement,  // the statement that increments the loop pointer(s)
			                     // before the next piece(s) of data is/are fetched;
			termExpression       // the expression that computes a "term" (thing to sum)
			                     // from a piece or pieces of data.
		)

	We explain this in detail below, pedagocically starting with an analogous macro
	for the more usual "sequential" summation algorithm.

	## 1. SEQUENTIAL SUMMATION: A SINGLE ACCUMULATOR

	The simplest use of summation of multiple values is if you have a single array of
	floating-point data to sum. If this array is x [1..size],
	then the usual straightforward algorithm for adding the terms is

		long double sum = 0.0;
		for (long i = 1; i <= size; i ++)
			sum += x [i];
		printf ("%.17g", (double) sum);
		
	This is *sequential summation*: a single *accumulator*, namely the variable `sum`,
	accumulates every element sequentially. Note that even though the array `x` will
	typically of type `double`, the accumulator is of type `long double` in this example:
	using 80 bits for summing is much better than using 64 bits (1000 times more precise),
	and on modern computers it is usually equally fast.

	The formulation above, which uses array indexing, is equivalent to a formulation
	in terms of a "looping pointer":

		long double sum = 0.0;
		double *xx = x;   // the looping pointer
		for (long i = 1; i <= size; i ++)
			sum += * ++ xx;
		printf ("%.17g", (double) sum);

	The looping-pointer formulation consists of defining (declaring and initializing)
	the pointer before the loop, and then inside the loop
	first pre-incrementing the pointer and then dereferencing it.
	The "*++xx" formulation has been idiomatic C from the 1980s on,
	but modern compilers produce equally efficient code if you separate the two steps:

		long double sum = 0.0;
		double *xx = x;   // decalere and initialize
		for (long i = 1; i <= size; i ++) {
			xx += 1;   // first increment...
			sum += *xx;   // ... then dereference
		}
		printf ("%.17g", (double) sum);

	In this little algorithm we can discern the following seven ingredients:
	- AccumulatorType: long double
	- sumVariableName: "sum"
	- CounterType: long
	- sizeExpression: "size"
	- initializeStatement: "double *xx = x"
	- incrementStatement: "xx += 1"
	- termExpression: "*xx"

	The algorithm can therefore be replaced with
	
		SEQUENTIAL_SUM (long double, sum, long, size, double *xx = x, xx += 1, *xx)
		printf ("%.17g", (double) sum);

	where the SEQUENTIAL_SUM macro is defined as:
*/
#define SEQUENTIAL_SUM(AccumulatorType, sumVariableName, CounterType, sizeExpression, \
	initializeStatement, incrementStatement, termExpression) \
\
	AccumulatorType sumVariableName = 0.0; \
	{/* scope */ \
		initializeStatement; \
		CounterType _n = sizeExpression;   /* to ensure that the size expression is evaluated only once */ \
		for (CounterType _i = 1; _i <= _n; _i ++) { \
			incrementStatement; \
			sumVariableName += termExpression; \
		} \
	}

/*
	## 2. SEQUENTIAL SUMMATION IN MACHINE CODE

	How would you sum the four numbers a, b, c and d,
	if all that your computer can do is adding two numbers at a time?
	
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


	## 3. SPEED OF SEQUENTIAL SUMMATION
	
	If your processor has good prefetching, then the four loads from RAM (moving a, b, c,
	and d into the registers r1, r2, r3 and r4) will take up no time at all.
	The three additions, however, will require three clock cycles in total:
	as all additions are performed on r1, each addition will have to wait
	until the previous addition has finished. Adding up 64 numbers will require 63 cycles.


	## 4. PRECISION OF SEQUENTIAL SUMMATION
	
	The worst case floating-point rounding error for sequential summation is N * epsilon,
	where N is the number of numbers to add, and epsilon is the precision of one number.
	The expected random-walk rounding error is epsilon * sqrt (N).


	## 5. PAIRWISE SUMMATION: MULTIPLE ACCUMULATORS

	An alternative way to add the four numbers, next to sequential summation,
	is to proceed in a *pairwise* manner, dividing the four numbers into two groups,
	then summing these separately, then summing the two results. In short C this would be:

		sum = (a + b) + (c + d);

	This pairwise summation continues recursively. For instance, you add 8 numbers this way:

		sum = ((a + b) + (c + d)) + ((e + f) + (g + h));


	## 6. PAIRWISE SUMMATION IN MACHINE CODE

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
	

	## 7. SPEED OF PAIRWISE SUMMATION
	
	Pairwise summation can be faster than sequential summation,
	because the addition of r2 to r1 can be performed in parallel to the addition of r4 to r3.
	The three additions in section 5 could therefore be performed in two clock cycles.
	Indeed, both compilers that I am using in 2017 (Clang on the Mac, and GCC on Windows and Linux)
	take only 0.30 nanoseconds per addition, which is just over two-thirds of the clock period
	of the 2.3 GHz processor (which is 43.5 nanoseconds) of my 2014 Macbook Pro.
	On a processor far, far away, which has more than 64 registers, perfect prefetching,
	and perfectly parallel operations on independent registers, 64 terms may be added
	within 6 clock cycles: six is the number of times we need to add something to r1,
	and each of these additions has to wait for the result of the previous addition.
	Theoretically, then, but not yet in practice, the execution time has fallen from order N
	to order log(N), at least for low N (and for high N it will be of order N,
	but with a much lower multiplication factor than sequential summation has).


	## 8. PRECISION OF PAIRWISE SUMMATION
	
	The worst-case rounding error is only epsilon * log (N),
	and the expected random-walk rounding error is only epsilon * sqrt (log (N)).
	Both are much better than in the sequential summation case.


	## 9. HOW TO USE PAIRWISE SUMMATION

	For pairwise summation we use the exact same macro arguments as for sequential summation:
	
		PAIRWISE_SUM (long double, sum, long, size, double *xx = x, xx += 1, *xx)
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

	The fifth argument of the macro declares and initializes the loop pointer,
	as in `double *xx = x` above. This pointer starts out pointing just below the elements of the array.

	The sixth argument of the macro is the formula you use for incrementing the loop pointer(s),
	as in `xx += 1` above. The macro uses this formula to prepare for the retrieval of
	every term in the summation.
	
	The seventh argument is an expression that should evaluate to the value of a term,
	given the current (incremented) value of the loop pointer(s), as in `*xx` above.

	More complicated use cases than a single array also exist. For instance, if you have to compute
	the inner product of the two arrays x [1..n] and y [1..n], you can do
	
		PAIRWISE_SUM (long double, inner, long, n,
			double *xx = x;   // the semicolon ensures that this line and the next form a single argument
			double *yy = y,
			(++ xx, ++ yy),
			(long double) *xx * (long double) *yy)
		printf ("%.17g", (double) inner);

	Note for the sixth argument: you can see here that you can do the two increments simultaneously
	by using parentheses and a comma; fortunately, the C macro preprocessor understands enough
	about parentheses to see that you mean the fifth argument to be a single argument.

	Note for the seventh argument: as 64-bit multiplication loses a lot of the precision of its
	two 64-bit operands, it is advisable to convert both operands to `long double`
	*before* the multiplication, as is done here. This usually costs no extra computation
	time (it can actually be faster). If you do
	
		PAIRWISE_SUM (long double, inner, long, n, double *xx = x; double *yy = y, (++ xx, ++ yy), *xx * *yy)
		printf ("%.17g", (double) inner);

	instead, the conversion to `long double` is done (by the macro) *after* the multiplication,
	which is less precise.

	Other use cases include array multiplication with strides...

		PAIRWISE_SUM (long double, inner, long, n,
			double *xx = & x [1 - xstride];   // note the funny semicolon again
			double *yy = & y [1 - ystride],
			(xx += xstride, yy += ystride),
			(long double) *xx * (long double) *yy)
		printf ("%.17g", (double) inner);

	... and small-lag convolution...

	for (long i = 1; i <= xSize - kernelSize + 1; i ++) {
		PAIRWISE_SUM (long double, conv, long, kernelSize,
			double *xx = & x [i - 1];
			double *filter = & kernel [kernelSize + 1],
			(xx += 1, filter -= 1),
			(long double) *xx * (long double) *filter)
		result [i] = conv;
	}

	... and matrix multiplication, and computing norms.


	## 10. IMPLEMENTATION OF PAIRWISE SUMMATION: LOW POWERS OF 2
	
	We have to implement pairwise summation with a macro,
	because the sixth and seventh arguments to PAIRWISE_SUM() have to be formulas for getting
	the next element to add. We use fixed formulas for adding 2, 4, 8, 16 or 32 terms,
	and a stack for the next 57 higher powers of 2, up to 2^62,
	so that our summation will work for N up to 2^63 - 1.
	The fixed formulas for the low powers of 2 are recursively defined macros:
*/

#define PAIRWISE_SUM_1_TERM(AccumulatorType, accumulator, incrementStatement, termExpression) \
	incrementStatement; \
	AccumulatorType accumulator = termExpression;

#define PAIRWISE_SUM_2_TERMS(AccumulatorType, accumulator, incrementStatement, termExpression) \
	PAIRWISE_SUM_1_TERM (AccumulatorType, accumulator, incrementStatement, termExpression) \
	{ \
		PAIRWISE_SUM_1_TERM (AccumulatorType, _r2, incrementStatement, termExpression) \
		accumulator += _r2; \
	}

#define PAIRWISE_SUM_4_TERMS(AccumulatorType, accumulator, incrementStatement, termExpression) \
	PAIRWISE_SUM_2_TERMS (AccumulatorType, accumulator, incrementStatement, termExpression) \
	{ \
		PAIRWISE_SUM_2_TERMS (AccumulatorType, _r3, incrementStatement, termExpression) \
		accumulator += _r3; \
	}

#define PAIRWISE_SUM_8_TERMS(AccumulatorType, accumulator, incrementStatement, termExpression) \
	PAIRWISE_SUM_4_TERMS (AccumulatorType, accumulator, incrementStatement, termExpression) \
	{ \
		PAIRWISE_SUM_4_TERMS (AccumulatorType, _r4, incrementStatement, termExpression) \
		accumulator += _r4; \
	}

#define PAIRWISE_SUM_16_TERMS(AccumulatorType, accumulator, incrementStatement, termExpression) \
	PAIRWISE_SUM_8_TERMS (AccumulatorType, accumulator, incrementStatement, termExpression) \
	{ \
		PAIRWISE_SUM_8_TERMS (AccumulatorType, _r5, incrementStatement, termExpression) \
		accumulator += _r5; \
	}

#define PAIRWISE_SUM_32_TERMS(AccumulatorType, accumulator, incrementStatement, termExpression) \
	PAIRWISE_SUM_16_TERMS (AccumulatorType, accumulator, incrementStatement, termExpression) \
	{ \
		PAIRWISE_SUM_16_TERMS (AccumulatorType, _r6, incrementStatement, termExpression) \
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
#define PAIRWISE_SUM_64_TERMS(AccumulatorType, accumulator, incrementStatement, termExpression) \
	PAIRWISE_SUM_32_TERMS (AccumulatorType, accumulator, incrementStatement, termExpression) \
	{ \
		PAIRWISE_SUM_32_TERMS (AccumulatorType, _r7, incrementStatement, termExpression) \
		accumulator += _r7; \
	}
#define PAIRWISE_SUM_128_TERMS(AccumulatorType, accumulator, incrementStatement, termExpression) \
	PAIRWISE_SUM_64_TERMS (AccumulatorType, accumulator, incrementStatement, termExpression) \
	{ \
		PAIRWISE_SUM_64_TERMS (AccumulatorType, _r8, incrementStatement, termExpression) \
		accumulator += _r8; \
	}
#define PAIRWISE_SUM_256_TERMS(AccumulatorType, accumulator, incrementStatement, termExpression) \
	PAIRWISE_SUM_128_TERMS (AccumulatorType, accumulator, incrementStatement, termExpression) \
	{ \
		PAIRWISE_SUM_128_TERMS (AccumulatorType, _r9, incrementStatement, termExpression) \
		accumulator += _r9; \
	}

/*
	A generalization about the timing of the summations in all the above macros
	is that r(i+1) is added to r(i) precisely when r(i+1) contains the same
	number of terms as r(i). This criterion for collapsing the partial sums
	is also used in the stack logic below.
*/

#define PAIRWISE_SUM(AccumulatorType, sumVariableName, CounterType, sizeExpression, \
	initializeStatement, incrementStatement, termExpression) \
\
	AccumulatorType sumVariableName = 0.0; \
	{/* scope */ \
		initializeStatement; \
		CounterType _n = sizeExpression; \
		if (_n & 1) { \
			PAIRWISE_SUM_1_TERM (AccumulatorType, _partialSum, incrementStatement, termExpression) \
			sumVariableName += _partialSum; \
		} \
		if (_n & 2) { \
			PAIRWISE_SUM_2_TERMS (AccumulatorType, _partialSum, incrementStatement, termExpression) \
			sumVariableName += _partialSum; \
		} \
		if (_n & 4) { \
			PAIRWISE_SUM_4_TERMS (AccumulatorType, _partialSum, incrementStatement, termExpression) \
			sumVariableName += _partialSum; \
		} \
		if (_n & 8) { \
			PAIRWISE_SUM_8_TERMS (AccumulatorType, _partialSum, incrementStatement, termExpression) \
			sumVariableName += _partialSum; \
		} \
		if (_n & 16) { \
			PAIRWISE_SUM_16_TERMS (AccumulatorType, _partialSum, incrementStatement, termExpression) \
			sumVariableName += _partialSum; \
		} \
		if (_n & 32) { \
			PAIRWISE_SUM_32_TERMS (AccumulatorType, _partialSum, incrementStatement, termExpression) \
			sumVariableName += _partialSum; \
		} \
		const int _baseCasePower = 6;   /* because the base case is 64 = 2^6 terms */ \
		CounterType _numberOfBaseCases = _n >> _baseCasePower; \
		if (_numberOfBaseCases != 0) { \
			/*                                                                                  */ \
			/*  The value of _powers [0] stays at 0, to denote the bottom of the stack.         */ \
			/*  The maximum value of _powers [1] should be 62, which denotes that 2^62 terms    */ \
			/*  have been accumulated into _partialSumStack [1]. This the maximum,              */ \
			/*  because _n can be at most 2^63-1 (assuming CounterType is 64 bits).             */ \
			/*  The maximum value of _powers [2] should then be 61.                             */ \
			/*  The maximum value of _powers [3] should be 60.                                  */ \
			/*  ...                                                                             */ \
			/*  The maximum value of _powers [57] should be 6. It ends there, because           */ \
			/*  2^6 is the granularity with which base case sums are put on the stack.          */ \
			/*  The maximum value of _powers [58] should also be 6,                             */ \
			/*  because this can be the situation just before collapsing the top of the stack.  */ \
			/*  However, if the whole stack is filled up like this, the actual number of        */ \
			/*  terms is already 2^63 (2^62 + 2^61 + 2^60 + ... 2^6 + 2^6). Therefore, we       */ \
			/*  need one element less, so the highest index of _powers [] should be 57.         */ \
			/*  For 32-bit counters, this highest index is 25.                                  */ \
			/*                                                                                  */ \
			const int _numberOfBitsInCounterType = 8 * sizeof (CounterType);   /* 64 or 32 */ \
			const int _highestIndex = _numberOfBitsInCounterType - 1 - _baseCasePower; \
			AccumulatorType _partialSumStack [1 + _highestIndex];   /* 8 bytes too many, but better code */ \
			unsigned char _powers [1 + _highestIndex]; \
			_powers [0] = 0; \
			int _stackPointer = 0; \
			for (CounterType _ipart = 1; _ipart <= _numberOfBaseCases; _ipart ++) { \
				/*                                                                              */ \
				/*  Compute the sum of the next 64 data points.                                 */ \
				/*                                                                              */ \
				PAIRWISE_SUM_64_TERMS (AccumulatorType, _partialSum, incrementStatement, termExpression) \
				/*                                                                              */ \
				/*  Put this sum on top of the stack.                                           */ \
				/*                                                                              */ \
				_partialSumStack [++ _stackPointer] = _partialSum; \
				_powers [_stackPointer] = _baseCasePower; \
				/*                                                                              */ \
				/*  The collapse criterion:                                                     */ \
				/*                                                                              */ \
				while (_powers [_stackPointer] == _powers [_stackPointer - 1]) { \
					_partialSumStack [_stackPointer - 1] += _partialSumStack [_stackPointer]; \
					_powers [-- _stackPointer] += 1; \
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
	constitutes a single statement. The macro contains a sequence of two things: the definitions
	of `sum` and the pointer(s), and a long block that usually changes the value of `sum`. Hence,
	the macro cannot be used as a single statement
	and e.g. has to be bracketed if used in an `else` clause.
	You are therefore advised to call `PAIRWISE_SUM()` without appending the misleading semicolon.
*/

/*
	## 12. SOME LESS GOOD SUMMATION ALGORITHMS
	
	The summation algorithm (at least for computing the mean) in the statistics software R
	is two-loop summation. This is fairly precise, but very slow:
*/
#define TWO_LOOP_SUM(AccumulatorType, sumVariableName, CounterType, sizeExpression, \
	initializeStatement, incrementStatement, termExpression) \
\
	AccumulatorType sumVariableName = 0.0; \
	{/* scope */ \
		CounterType _n = sizeExpression; \
		{/* scope */ \
			initializeStatement; \
			for (CounterType _i = 1; _i <= _n; _i ++) { \
				incrementStatement; \
				sumVariableName += termExpression; \
			} \
		} \
		AccumulatorType _mean = sumVariableName / _n; \
		{/* scope */ \
			sumVariableName = 0.0; \
			initializeStatement; \
			for (CounterType _i = 1; _i <= _n; _i ++) { \
				incrementStatement; \
				sumVariableName += (termExpression) - _mean; \
			} \
			sumVariableName += _mean * _n; \
		} \
	}
/*
	Another one is the Kahan algorithm. Its precision is comparable to that of pairwise summation,
	but it is extremely slow:
*/
#define KAHAN_SUM(AccumulatorType, sumVariableName, CounterType, sizeExpression, \
	initializeStatement, incrementStatement, termExpression) \
\
	AccumulatorType sumVariableName = 0.0; \
	{/* scope */ \
		initializeStatement; \
		CounterType _n = sizeExpression; \
		AccumulatorType _correction = 0.0; \
		for (CounterType _i = 1; _i <= _n; _i ++) { \
			incrementStatement; \
			AccumulatorType _correctedTerm = (termExpression) - _correction; \
			AccumulatorType _newSum = sumVariableName + _correctedTerm; \
			_correction = (_newSum - sumVariableName) - _correctedTerm; \
			sumVariableName = _newSum; \
		} \
	}

/* End of file PAIRWISE_SUM.h */
#endif
