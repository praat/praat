#ifndef _CCs_to_DTW_h_
#define _CCs_to_DTW_h_
/* CCs_to_DTW.h
 *
 *	Dynamic Time Warp of two CCs.
 *
 * Copyright (C) 1993-2018 David Weenink
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
 djmw 20020315 GPL header
 */


#include "CC.h"
#include "DTW.h"


autoDTW CCs_to_DTW (CC me, CC thee, double coefficientWeight, double logEnergyWeight, double coefficientRegressionWeight, double logEnergyRegressionWeight, double regressionWindowLength);
/*
	1. Calculate distances between CCs:
		Distance between frame i (from me) and j (from thee) is
		coefficientWeight * d1 + logEnergyWeight * d2 + coefficientRegressionWeight * d3 + logEnergyRegressionWeight * d4,
		where
			d1 = Sum (k=1; k=nCoefficients; (c[i,k]-c[j,k])^2)
			d2 = (c[0,k]-c[0,k])^2
			d3 = Sum (k=1; k=nCoefficients; (r[i,k]-r[j,k])^2), with
				r[i,k] the regression coefficient of the cepstral coefficients
				from the frames within a time span of 'dtr' seconds.
				c[i,j] is jth cepstral coefficient in frame i.
			d4 = regression on energy (c[0])
	2. Find optimum path through the distance matrix (see DTW).

	PRECONDITIONS:

	at least one of the four weights != 0
*/

#endif /* _CCs_to_DTW_h_ */
