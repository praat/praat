/* MFCC.c
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
 djmw 20020813 GPL header
*/

#include "MFCC.h"
#include "NUM2.h"

class_methods (MFCC, CC)
class_methods_end

MFCC MFCC_create (double tmin, double tmax, long nt, double dt, double t1,
	long maximumNumberOfCoefficients, double fmin_mel, double fmax_mel)
{
	MFCC me = new (MFCC);
	if (me == NULL || ! CC_init (me, tmin, tmax, nt, dt, t1,
		maximumNumberOfCoefficients, fmin_mel, fmax_mel)) forget (me);
	return me;
}

int MFCC_lifter (MFCC me, long lifter)
{
	long i, frame;
	double *c;

	Melder_assert (lifter > 0);
	
	if ((c = NUMdvector (1, my maximumNumberOfCoefficients)) == NULL) return 0;
	
	for (i=1; i <= my maximumNumberOfCoefficients; i++)
	{
		c[i] = (1 + lifter / 2 * sin (NUMpi * i / lifter));
	}
	
	for (frame=1; frame <= my nx; frame++)
	{
		CC_Frame cf = & my frame[frame];
		for (i=1; i <= cf -> numberOfCoefficients; i++)
		{
			cf -> c[i] *= c[i];
		}
	}
	
	NUMdvector_free (c, 1);
	return Melder_hasError ();
}

/* End of file MFCC.c */
