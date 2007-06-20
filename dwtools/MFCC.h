#ifndef _MFCC_h_
#define _MFCC_h_
/* MFCC.h
 *
 * Mel Frequency Cepstral Coefficients class.
 *
 * Copyright (C) 1993-2002 David Weenink
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
 djmw 20010501
 djmw 20020813 GPL header
*/

#ifndef _CC_h_
	#include "CC.h"
#endif

#define MFCC_members CC_members
#define MFCC_methods CC_methods
class_create (MFCC, CC);

/*
	Interpretatation:
	Mel frequency cepstral coefficients as a function of time.
	c0 represents the average filter output (dB's).
*/

MFCC MFCC_create (double tmin, double tmax, long nt, double dt, double t1,
	long maximumNumberOfCoefficients, double fmin_mel, double fmax_mel);

int MFCC_lifter (MFCC me, long lifter);
/*
	Lifter the cepstral coefficients:
	
	c[i] *= (1 + lifter / 2 * sin (NUMpi * i / lifter))

*/

#endif /* _MFCC_h_ */
