#ifndef _MelFilter_and_MFCC_h_
#define _MelFilter_and_MFCC_h_
/* MelFilter_and_MFCC.h
 *
 * Copyright (C) 1993-2013 David Weenink
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
 djmw 2001
 djmw 20020813 GPL header
 djmw 20110307 Latest modification
*/

#include "FilterBank.h"
#include "MFCC.h"

/*
Three slightly "different" definitions of MFCC
1. Davis & Mermelstein
	MFCC[i] = SUM (j=1..N, f[j] * cos (i(j-1/2)pi/N)), i=1..N-1
2. Vergin & O'Shaughnessy
	MFCC[i] = SUM (j=0..N-1, f[j] * cos (i(j+1/2)pi/N)), i = 0..N-1
3. HTK-book
	MFCC[i] = sqrt(2/n) SUM (j=1..N, f[j] * cos (i(j-1/2)pi/N))
    i = 0..N-1

The f[j]'s are the filterbank outputs in dB.
We follow the definition of Davis and Mermelstein:
    	MFCC[i] = SUM (j=1..N, f[j] * cos (i(j-1/2)pi/N)), i=1..N-1,
however, we also calculate the zero coefficient as
    	MFCC[0] = SUM (j=1..N, f[j]) / N.
The pure cosine transform MFCC[0] would be the sum instead of the average.
This average is more convenient in the inverse transform (from MFCC to
MelFilter).
*/


MFCC MelFilter_to_MFCC (MelFilter me, long numberOfCoefficients);
/*
Calculates the Cosine Transform of the filterbank values.
*/

MelFilter MFCC_to_MelFilter (MFCC me, long firstCoefficient, long lastCoefficient);
/*
Calculates the Inverse CT of cepstral coefficients.
*/

#endif /* MelFilter_and_MFCC.h */
