/* NUMrandom.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/03/04 GPL
 * pb 2006/12/18 improved comment
 * pb 2011/03/29 C++
 */

/*
	Based on:
	Donald E. Knuth (1998): The art of computer programming. Third edition. Vol. 2: seminumerical algorithms.
		Boston: Addison-Wesley.
*/

#include "NUM.h"
//#include "melder.h"
#include <time.h>

/*
	The random sequence is based on lagged Fibonacci on real numbers in [0; 1) (Knuth:27,29):

		X [n] = (X [n - 100] + X [n - 37]) mod 1.0

	The routine NUMrandomRestart relies on a 52-bit mantissa, so we will get 10^15 different random numbers.
	The period is infinitely larger than that.

	To compute x mod 1, Knuth uses the formula

		x - (int) x

	However, we use the following, which is faster:

		if (x > 1.0) x -= 1.0;
*/
#define LONG_LAG  100
#define SHORT_LAG  37
#define LAG_DIFF  (LONG_LAG - SHORT_LAG)
/*
	Each of the 4 billion possible seeds comes with its own stream of random numbers.
	These streams must be guaranteed not to overlap within the first 2^70 draws (Knuth:187).
	That's 30 thousand years if one random number takes 1 nanosecond.
*/
#define STREAM_SEPARATION  70
/*
	The following value has to be at least 100, i.e. equal to the long lag.
	If it is 100, then the entire random sequence will be output by NUMrandomFraction.
	In that case, however, the resulting numbers will not be completely random (Knuth:72), i.e.:

		X [n] - X [n - 37] = X [n - 63] - X [n - 163]   (modulo 1.0)

	The following Praat script will show this:

		echo Birthday spacings test:
		for i to 200
		   a'i' = randomUniform (0,1)
		endfor
		for i from 164 to 200
		   xn = a'i'
		   j = i - 37
		   xn37 = a'j'
		   j = i - 63
		   xn63 = a'j'
		   j = i - 163
		   xn163 = a'j'
		   diff1 = xn - xn37
		   diff2 = xn63 - xn163
		   printline 'diff1' 'diff2'
		endfor

	We see that diff1 and diff2 are equal modulo 1.
	This is why Knuth advises (p.35,72,188) to throw away 909 numbers after generating 100,
	i.e. the following value will be 1009.
*/
#define QUALITY  1009

/*
	A test for the integrity of NUMrandomRestart and NUMrandomFraction (Knuth:188,603):

		NUMrandomRestart (310952);
		for (i = 1; i <= 1009 * 2009 - 100 + 1; i ++)
			x = NUMrandomFraction ();
		Melder_information (Melder_double (x));

	With QUALITY set to 100, this should give 0.27452626307394156768 (Knuth:603).
	With QUALITY set to 1009, it should give 0.73203216867254750078.
*/

static double randomArray [LONG_LAG];
static int randomInited = 0;
static long randomArrayPointer1, randomArrayPointer2, iquality;

void NUMrandomRestart (unsigned long seed) {
	/*
		Based on Knuth, p. 187,602.

		Knuth had:
			int s = seed;
		This is incorrect (even if an int is 32 bit), since a negative value causes a loop in the test
			if (s != 0)
				s >>= 1;
		because the >> operator on negative integers adds a sign bit to the left.
	 */
	long t, j;
	double u [2 * LONG_LAG - 1], ul [2 * LONG_LAG - 1];
	double ulp = 1.0 / (1L << 30) / (1L << 22), ss;
	ss = 2.0 * ulp * (seed + 2);   /* QUESTION: does this work if seed exceeds 2^32 - 3? See Knuth p. 187. */
	for (j = 0; j < LONG_LAG; j ++) {
		u [j] = ss;
		ul [j] = 0.0;
		ss += ss;
		if (ss >= 1.0) ss -= 1.0 - 2 * ulp;
	}
	for (; j < 2 * LONG_LAG - 1; j ++)
		u [j] = ul [j] = 0.0;
	u [1] += ulp;
	ul [1] = ulp;
	t = STREAM_SEPARATION - 1;
	while (t > 0) {
		for (j = LONG_LAG - 1; j > 0; j --) {
			ul [j + j] = ul [j];
			u [j + j] = u [j];
		}
		for (j = 2 * LONG_LAG - 2; j > LAG_DIFF; j -= 2) {
			ul [2 * LONG_LAG - 1 - j] = 0.0;
			u [2 * LONG_LAG - 1 - j] = u [j] - ul [j];
		}
		for (j = 2 * LONG_LAG - 2; j >= LONG_LAG; j --) if (ul [j] != 0) {
			ul [j - LAG_DIFF] = ulp - ul [j - LAG_DIFF];
			u [j - LAG_DIFF] += u [j];
			if (u [j - LAG_DIFF] >= 1.0) u [j - LAG_DIFF] -= 1.0;
			ul [j - LONG_LAG] = ulp - ul [j - LONG_LAG];
			u [j - LONG_LAG] += u [j];
			if (u [j - LONG_LAG] >= 1.0) u [j - LONG_LAG] -= 1.0;
		}
		if ((seed & 1) != 0) {
			for (j = LONG_LAG; j > 0; j --) {
				ul [j] = ul [j - 1];
				u [j] = u [j - 1];
			}
			ul [0] = ul [LONG_LAG];
			u [0] = u [LONG_LAG];
			if (ul [LONG_LAG] != 0) {
				ul [SHORT_LAG] = ulp - ul [SHORT_LAG];
				u [SHORT_LAG] += u [LONG_LAG];
				if (u [SHORT_LAG] >= 1.0) u [SHORT_LAG] -= 1.0;
			}
		}
		if (seed != 0) {
			seed >>= 1;
		} else {
			t --;
		}
	}
	for (j = 0; j < SHORT_LAG; j ++)
		randomArray [j + LAG_DIFF] = u [j];
	for (; j < LONG_LAG; j ++)
		randomArray [j - SHORT_LAG] = u [j];
	randomArrayPointer1 = 0;
	randomArrayPointer2 = LAG_DIFF;
	iquality = 0;
	randomInited = 1;
}

double NUMrandomFraction (void) {
	/*
		Knuth uses a long random array of length QUALITY to copy values from randomArray.
		We save 8 kilobytes by using randomArray as a cyclic array (10% speed loss).
	*/
	long p1, p2;
	double newValue;
	if (! randomInited) NUMrandomRestart (time (NULL));
	p1 = randomArrayPointer1, p2 = randomArrayPointer2;
	if (p1 >= LONG_LAG) p1 = 0;
	if (p2 >= LONG_LAG) p2 = 0;
	newValue = randomArray [p1] + randomArray [p2];
	if (newValue >= 1.0) newValue -= 1.0;
	randomArray [p1] = newValue;
	p1 ++;
	p2 ++;
	if (++ iquality == LONG_LAG) {
		for (; iquality < QUALITY; iquality ++) {
			double newValue2;
			/*
				Possible future minor speed improvement:
					the cyclic array is walked down instead of up.
					The following tests will then be for 0.
			*/
			if (p1 >= LONG_LAG) p1 = 0;
			if (p2 >= LONG_LAG) p2 = 0;
			newValue2 = randomArray [p1] + randomArray [p2];
			if (newValue2 >= 1.0) newValue2 -= 1.0;
			randomArray [p1] = newValue2;
			p1 ++;
			p2 ++;
		}
		iquality = 0;
	}
	randomArrayPointer1 = p1;
	randomArrayPointer2 = p2;
	return newValue;
}

double NUMrandomUniform (double lowest, double highest) {
	return lowest + (highest - lowest) * NUMrandomFraction ();
}

long NUMrandomInteger (long lowest, long highest) {
	return lowest + (long) ((highest - lowest + 1) * NUMrandomFraction ());
}

#define repeat  do
#define until(cond)  while (! (cond))
double NUMrandomGauss (double mean, double standardDeviation) {
	/*
		Knuth, p. 122.
	*/
	static int secondAvailable = 0;
	static double y;
	double s, x;
	if (secondAvailable) {
		secondAvailable = FALSE;
		return mean + standardDeviation * y;
	} else {
		repeat {
			x = 2.0 * NUMrandomFraction () - 1.0;   /* Inside the square [-1; 1] x [-1; 1]. */
			y = 2.0 * NUMrandomFraction () - 1.0;
			s = x * x + y * y;
		} until (s < 1.0);   /* Inside the unit circle. */
		if (s == 0.0) {
			x = y = 0.0;
		} else {
			double factor = sqrt (-2.0 * log (s) / s);
			x *= factor, y *= factor;
		}
		secondAvailable = TRUE;
		return mean + standardDeviation * x;
	}
}

double NUMrandomPoisson (double mean) {
	/*
		The Poisson distribution is

			P(k) = mean^k * exp (- mean) / k!

		We have to find a function, with known primitive,
		that is always (a bit) greater than P (k).
		This function is based on the Lorentzian distribution,
		with a maximum of P(mean)/0.9 at k=mean:

			f (k) = mean^mean * exp (- mean) / mean! / (0.9 * (1 + (k - mean)^2 / (2 * mean)))

		The tangent is computed as the deviate

			tangent = tan (pi * unif (0, 1))

		This must equal the square root of (k - mean)^2 / (2 * mean),
		so that a trial value for k is given by

			k = floor (mean + tangent * sqrt (2 * mean))

		The probability that this is a good value is proportionate to the ratio of the Poisson
		distribution and the encapsulating function:

			probability = P (k) / f (k) = 0.9 * (1 + tangent^2) * mean ^ (k - mean) * mean! / k!

		The last two factors can be calculated as

			exp ((k - mean) * ln (mean) + lnGamma (mean + 1) - lnGamma (k + 1))
	*/
	static double previousMean = -1.0;   /* This routine may well be called repeatedly with the same mean. Optimize. */
	if (mean < 8.0) {
		static double expMean;
		double product = 1.0;
		long result = -1;
		if (mean != previousMean) {
			previousMean = mean;
			expMean = exp (- mean);
		}
		repeat {
			product *= NUMrandomFraction ();
			result ++;
		} until (product <= expMean);
		return result;
	} else {
		static double sqrtTwoMean, lnMean, lnMeanFactorial;
		double result, probability, tangent;
		if (mean != previousMean) {
			previousMean = mean;
			sqrtTwoMean = sqrt (2.0 * mean);
			lnMean = log (mean);
			lnMeanFactorial = NUMlnGamma (mean + 1.0);
		}
		repeat {
			repeat {
				tangent = tan (NUMpi * NUMrandomFraction ());
				result = mean + tangent * sqrtTwoMean;
			} until (result >= 0.0);
			result = floor (result);
			probability = 0.9 * (1.0 + tangent * tangent) * exp ((result - mean) * lnMean + lnMeanFactorial - NUMlnGamma (result + 1.0));
		} until (NUMrandomFraction () <= probability);
		return result;
	}
}

/* End of file NUMrandom.cpp */
