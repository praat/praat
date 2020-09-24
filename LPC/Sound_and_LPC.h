#ifndef _Sound_and_LPC_h_
#define _Sound_and_LPC_h_
/* Sound_and_LPC.h
 *
 * Copyright (C) 1994-2020 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 19971103
 djmw 20020812 GPL header
 djmw 20110307 Latest modification
*/


#include "LPC.h"
#include "Sound.h"

autoLPC Sound_to_LPC_autocorrelation (Sound me, int predictionOrder, double analysisWidth, double dt, double preEmphasisFrequency);

autoLPC Sound_to_LPC_covariance (Sound me, int predictionOrder, double analysisWidth, double dt, double preEmphasisFrequency);

autoLPC Sound_to_LPC_burg (Sound me, int predictionOrder, double analysisWidth, double dt, double preEmphasisFrequency);

autoLPC Sound_to_LPC_marple (Sound me, int predictionOrder, double analysisWidth, double dt, double preEmphasisFrequency, double tol1, double tol2);

void Sound_into_LPC (Sound me, LPC thee, double analysisWidth, double preEmphasisFrequency, kLPC_Analysis method, double tol1, double tol2);
/*
 * Function:
 *	Calculate linear prediction coefficients according to following model:
 *  Minimize E(m) = Sum(n=n0;n=n1; (x[n] + Sum(k=1;k=m; a[k]*x[n-k])))
 * Method:
 *  The minimization is carried out by solving the equations:
 *  Sum(i=1;i=m; a[i]*c[i][k]) = -c[0][k] for k=1,2,...,m
 *  where c[i][k] = Sum(n=n0;n=n1;x[n-i]*x[n-k])
 *  1. Covariance:
 *		n0=m; n1 = N-1;
 *      c[i][k] is symmetric, positive semi-definite matrix
 *  	Markel&Gray, LP of Speech, page 221;
 *  2. Autocorrelation
 *		signal is zero outside the interval;
 *      n0=-infinity; n1=infinity
 *      c[i][k] symmetric, positive definite Toeplitz matrix
 *  	Markel&Gray, LP of Speech, page 219;
 * Preconditions:
 *	predictionOrder > 0;
 *  preEmphasisFrequency >= 0;
 *
 * Burg method: see Numerical recipes Chapter 13.
 *
 * Marple method: see Marple, L. (1980), A new autoregressive spectrum analysis
 *		algorithm, IEEE Trans. on ASSP 28, 441-453.
 *	tol1 : stop iteration when E(m) / E(0) < tol1
 *	tol2 : stop iteration when (E(m)-E(m-1)) / E(m-1) < tol2,
 */

autoSound LPC_Sound_filter (LPC me, Sound thee, bool useGain);
/*
	E(z) = X(z)A(z),
	A(z) = 1 + Sum (k=1, k=m, a(k)z^-k);

	filter:
		given e & a, determine x;
		x(n) = e(n) - Sum (k=1, m, a(k)x(n-k))
	useGain determines whether the LPC-gain is used in the synthesis.
*/

void LPC_Sound_filterWithFilterAtTime_inplace (LPC me, Sound thee, integer channel, double time);

autoSound LPC_Sound_filterWithFilterAtTime (LPC me, Sound thee, integer channel, double time);

autoSound LPC_Sound_filterInverse (LPC me, Sound thee);
/*
	E(z) = X(z)A(z),
	A(z) = 1 + Sum (k=1, k=m, a(k)z^-k);

	filter inverse:
		given x & a, determine e;
		e(n) = x(n) + Sum (k=1, m, a(k)x(n-k))
*/

autoSound LPC_Sound_filterInverseWithFilterAtTime (LPC me, Sound thee, integer channel, double time);

void LPC_Sound_filterInverseWithFilterAtTime_inplace (LPC me, Sound thee, integer channel, double time);

#endif /* _Sound_and_LPC_h_ */
