/* MFCC.cpp
 *
 * Mel Frequency Cepstral Coefficients class.
 *
 * Copyright (C) 1993-2011 David Weenink
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
 * djmw 20020813 GPL header
 * djmw 20110304 Thing_new
*/

#include "MFCC.h"
#include "NUM2.h"

class_methods (MFCC, CC)
class_methods_end

MFCC MFCC_create (double tmin, double tmax, long nt, double dt, double t1,
	long maximumNumberOfCoefficients, double fmin_mel, double fmax_mel)
{
	try {
		autoMFCC me = Thing_new (MFCC);
		CC_init (me.peek(), tmin, tmax, nt, dt, t1, maximumNumberOfCoefficients, fmin_mel, fmax_mel);
		return me.transfer();
	} catch (MelderError) { Melder_throw ("MFCC not created."); }
}

void MFCC_lifter (MFCC me, long lifter)
{
	try {
		Melder_assert (lifter > 0);
		autoNUMvector<double> c (1, my maximumNumberOfCoefficients);	
		for (long i=1; i <= my maximumNumberOfCoefficients; i++)
		{
			c[i] = (1 + lifter / 2 * sin (NUMpi * i / lifter));
		}
	
		for (long frame=1; frame <= my nx; frame++)
		{
			CC_Frame cf = (CC_Frame) & my frame[frame];
			for (long i=1; i <= cf -> numberOfCoefficients; i++)
			{
				cf -> c[i] *= c[i];
			}
		}
	} catch (MelderError) { Melder_throw (me, ": not lifted."); }
}

/* End of file MFCC.cpp */
