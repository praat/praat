#ifndef _Cepstrumc_h_
#define _Cepstrumc_h_
/* Cepstrumc.h
 *
 * Copyright (C) 1994-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
oo_CLASS_CREATE (Cepstrumc, Sampled);

void Cepstrumc_init (Cepstrumc me, double tmin, double tmax, long nt, double dt, double t1,
	int nCoefficients, double samplingFrequency);
	
Cepstrumc Cepstrumc_create (double tmin, double tmax, long nt, double dt, double t1,
	int nCoefficients, double samplingFrequency);

/******************* Frames ************************************************/

void Cepstrumc_Frame_init (Cepstrumc_Frame me, int nCoefficients);
	
DTW Cepstrumc_to_DTW ( Cepstrumc me, Cepstrumc thee, double wc, double wle,
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

Matrix Cepstrumc_to_Matrix (Cepstrumc me);

#endif /* _Cepstrumc_h_ */
