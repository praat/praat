#ifndef _xxNum_h_
#define _xxNum_h_
/* xxNum.h
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

/*
	Called in inline functions.
*/

double NUMinner_ (numvec x, numvec y);
void NUM_sum_mean (numvec x, double *p_sum, double *p_mean) noexcept;
void NUM_sum_mean_sumsq_variance_stdev (numvec x, double *p_sum, double *p_mean, double *p_sumsq, double *p_variance, double *p_stdev) noexcept;
void NUM_sum_mean_sumsq_variance_stdev (nummat x, integer columnNumber, double *p_sum, double *p_mean, double *p_sumsq, double *p_variance, double *p_stdev) noexcept;

/*
	From here on in alphabetical order.
*/

double NUMcenterOfGravity (numvec x) noexcept;

inline static double NUMinner (numvec x, numvec y) {
	integer n = x.size;
	if (y.size != n) return undefined;
	if (n <= 8) {
		if (n <= 2) return n <= 0 ? 0.0 : n == 1 ? x [1] * y [1] : (double) ((longdouble) x [1] * (longdouble) y [1] + (longdouble) x [2] * (longdouble) y [2]);
		if (n <= 4) return n == 3 ?
			(double) ((longdouble) x [1] * (longdouble) y [1] + (longdouble) x [2] * (longdouble) y [2] + (longdouble) x [3] * (longdouble) y [3]) :
			(double) (((longdouble) x [1] * (longdouble) y [1] + (longdouble) x [2] * (longdouble) y [2]) + ((longdouble) x [3] * (longdouble) y [3] + (longdouble) x [4] * (longdouble) y [4]));
		if (n <= 6) return n == 5 ?
			(double) (((longdouble) x [1] * (longdouble) y [1] + (longdouble) x [2] * (longdouble) y [2] + (longdouble) x [3] * (longdouble) y [3]) + ((longdouble) x [4] * (longdouble) y [4] + (longdouble) x [5] * (longdouble) y [5])) :
			(double) (((longdouble) x [1] * (longdouble) y [1] + (longdouble) x [2] * (longdouble) y [2] + (longdouble) x [3] * (longdouble) y [3]) + ((longdouble) x [4] * (longdouble) y [4] + (longdouble) x [5] * (longdouble) y [5] + (longdouble) x [6] * (longdouble) y [6]));
		return n == 7 ?
			(double) ((((longdouble) x [1] * (longdouble) y [1] + (longdouble) x [2] * (longdouble) y [2]) + ((longdouble) x [3] * (longdouble) y [3] + (longdouble) x [4] * (longdouble) y [4])) + ((longdouble) x [5] * (longdouble) y [5] + (longdouble) x [6] * (longdouble) y [6] + (longdouble) x [7] * (longdouble) y [7])) :
			(double) ((((longdouble) x [1] * (longdouble) y [1] + (longdouble) x [2] * (longdouble) y [2]) + ((longdouble) x [3] * (longdouble) y [3] + (longdouble) x [4] * (longdouble) y [4])) + (((longdouble) x [5] * (longdouble) y [5] + (longdouble) x [6] * (longdouble) y [6]) + ((longdouble) x [7] * (longdouble) y [7] + (longdouble) x [8] * (longdouble) y [8])));
	}
	return NUMinner_ (x, y);
}

inline static integer NUMlength (conststring32 str) {
	return str32len (str);
}

inline static double NUMlog2 (double x) {
	return log (x) * NUMlog2e;
}

inline static double NUMmean (numvec x) noexcept {
	integer n = x.size;
	if (n <= 8) {
		if (n <= 2) return n <= 0 ? undefined : n == 1 ? x [1] : (double) (0.5 * ((longdouble) x [1] + (longdouble) x [2]));
		if (n <= 4) return n == 3 ?
			(double) ((1.0 / (longdouble) 3.0) * ((longdouble) x [1] + (longdouble) x [2] + (longdouble) x [3])) :
			(double) (0.25 * (((longdouble) x [1] + (longdouble) x [2]) + ((longdouble) x [3] + (longdouble) x [4])));
		if (n <= 6) return n == 5 ?
			(double) ((1.0 / (longdouble) 5.0) * (((longdouble) x [1] + (longdouble) x [2] + (longdouble) x [3]) + ((longdouble) x [4] + (longdouble) x [5]))) :
			(double) ((1.0 / (longdouble) 6.0) * (((longdouble) x [1] + (longdouble) x [2] + (longdouble) x [3]) + ((longdouble) x [4] + (longdouble) x [5] + (longdouble) x [6])));
		return n == 7 ?
			(double) ((1.0 / (longdouble) 7.0) * ((((longdouble) x [1] + (longdouble) x [2]) + ((longdouble) x [3] + (longdouble) x [4])) + ((longdouble) x [5] + (longdouble) x [6] + (longdouble) x [7]))) :
			(double) (0.125 * ((((longdouble) x [1] + (longdouble) x [2]) + ((longdouble) x [3] + (longdouble) x [4])) + (((longdouble) x [5] + (longdouble) x [6]) + ((longdouble) x [7] + (longdouble) x [8]))));
	}
	double mean;
	NUM_sum_mean (x, nullptr, & mean);
	return mean;
}

double NUMnorm (numvec x, double power) noexcept;

inline static double NUMnorm (nummat x, double power) noexcept {
	return NUMnorm (as_numvec (x), power);
}

integer NUMnumberOfTokens (conststring32 str);

/*
	Return zero for non-positive base.
*/
double NUMpow (double base, double exponent) {
	return base <= 0.0 ? 0.0 : pow (base, exponent);
}

inline static double NUMsqrt (double x) {
	#if defined (_WIN32)
		if (x < 0.0) return undefined;
	#endif
	return sqrt (x);
}

double NUMstdev (numvec x) noexcept;

inline static double NUMsum (numvec x) noexcept {
	integer n = x.size;
	if (n <= 8) {
		if (n <= 2) return n <= 0 ? 0.0 : n == 1 ? x [1] : x [1] + x [2];
		if (n <= 4) return n == 3 ?
			(double) ((longdouble) x [1] + (longdouble) x [2] + (longdouble) x [3]) :
			(double) (((longdouble) x [1] + (longdouble) x [2]) + ((longdouble) x [3] + (longdouble) x [4]));
		if (n <= 6) return n == 5 ?
			(double) (((longdouble) x [1] + (longdouble) x [2] + (longdouble) x [3]) + ((longdouble) x [4] + (longdouble) x [5])) :
			(double) (((longdouble) x [1] + (longdouble) x [2] + (longdouble) x [3]) + ((longdouble) x [4] + (longdouble) x [5] + (longdouble) x [6]));
		return n == 7 ?
			(double) ((((longdouble) x [1] + (longdouble) x [2]) + ((longdouble) x [3] + (longdouble) x [4])) + ((longdouble) x [5] + (longdouble) x [6] + (longdouble) x [7])) :
			(double) ((((longdouble) x [1] + (longdouble) x [2]) + ((longdouble) x [3] + (longdouble) x [4])) + (((longdouble) x [5] + (longdouble) x [6]) + ((longdouble) x [7] + (longdouble) x [8])));
	}
	double sum;
	NUM_sum_mean (x, & sum, nullptr);
	return sum;
}

double NUMsumsq (numvec x) noexcept;

double NUMvariance (numvec x) noexcept;

/* End of file xxNum.h */
#endif
