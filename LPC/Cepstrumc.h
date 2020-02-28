#ifndef _Cepstrumc_h_
#define _Cepstrumc_h_
/* Cepstrumc.h
 *
 * Copyright (C) 1994-2019 David Weenink
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
 djmw 19950822
 djmw 20020812 GPL header
 djmw 20110306 Latest modification.
*/

#include "Sampled.h"
#include "Matrix.h"
#include "Graphics.h"
#include "DTW.h"

#include "Cepstrumc_def.h"

void Cepstrumc_init (Cepstrumc me, double tmin, double tmax, integer nt, double dt, double t1,
	integer nCoefficients, double samplingFrequency);
	
autoCepstrumc Cepstrumc_create (double tmin, double tmax, integer nt, double dt, double t1,
	integer nCoefficients, double samplingFrequency);

/******************* Frames ************************************************/

void Cepstrumc_Frame_init (Cepstrumc_Frame me, integer nCoefficients);
	
autoDTW Cepstrumc_to_DTW (Cepstrumc me, Cepstrumc thee, double wc, double wle,
	double wr, double wer, double dtr, int matchStart, int matchEnd, int constraint);
/*
	1. Calculate distances between Cepstra:
		Distance between frame i (from me) and j (from thee) is
		wc * d1 + wle * d2 + wr * d3 + wer * d4,
			where wc, wle, wr & wer are weights and
			d1 = Sum (k=1; k=nCoefficients; (c[i,k]-c[j,k])^2)
			d2 = (c[0,k]-c[0,k])^2
			d3 = Sum (k=1; k=nCoefficients; (r[i,k]-r[j,k])^2), with
				r[i,k] the regression coefficient of the cepstral coefficients
				from the frames within a time span of 'dtr' seconds.
				c[i,j] is jth cepstral coefficient in frame i.
			d4 = regression on energy (c[0])
	2. Find optimum path through the distance matrix (see DTW).
	
	PRECONDITIONS:
	
	at least one of wc, wle, wr, wer != 0
*/

autoMatrix Cepstrumc_to_Matrix (Cepstrumc me);

#endif /* _Cepstrumc_h_ */
