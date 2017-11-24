#ifndef _MFCC_h_
#define _MFCC_h_
/* MFCC.h
 *
 * Mel Frequency Cepstral Coefficients class.
 *
 * Copyright (C) 1993-2017 David Weenink
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
 djmw 20010501
 djmw 20020813 GPL header
 djmw 20120504 Latest modification.
*/

#include "CC.h"
#include "Sound.h"
#include "TableOfReal.h"

Thing_define (MFCC, CC) {
	void v_info ()
		override;
};

/*
	Three slightly "different" definitions of MFCC
	1. Davis & Mermelstein
		MFCC[i] = SUM (j=1..N, f[j] * cos (i(j-1/2)pi/N)), i = 1..N-1
	2. Vergin & O'Shaughnessy
		MFCC[i] = SUM (j=0..N-1, f[j] * cos (i(j+1/2)pi/N)), i = 0..N-1
	3. HTK-book
		MFCC[i] = sqrt(2/n) SUM (j=1..N, f[j] * cos (i(j-1/2)pi/N)), i = 0..N-1

	The f[j]'s are the MelSpectrogram values converted to dB.
	We follow the definition of Davis and Mermelstein:
    	MFCC[i] = SUM (j=1..N, f[j] * cos (i(j-1/2)pi/N)), i=1..N-1,
*/

/*
	Interpretation:
	Mel frequency cepstral coefficient vectors as a function of time.
	c0 represents the sum of the dB filter outputs.
*/

autoMFCC MFCC_create (double tmin, double tmax, integer nt, double dt, double t1, integer maximumNumberOfCoefficients, double fmin_mel, double fmax_mel);

void MFCC_lifter (MFCC me, integer lifter);
/*
	Lifter the cepstral coefficients:
	c[i] *= (1 + lifter / 2 * sin (NUMpi * i / lifter))
*/

autoTableOfReal MFCC_to_TableOfReal (MFCC me, bool includeC0);

autoSound MFCC_to_Sound (MFCC me);

autoSound MFCCs_crossCorrelate (MFCC me, MFCC thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain);

autoSound MFCCs_convolve (MFCC me, MFCC thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain);

autoMatrix MFCC_to_Matrix_features (MFCC me, double windowLength, bool includeEnergy);

#endif /* _MFCC_h_ */
