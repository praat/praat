/* Sound_to_MFCC.c
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
 djmw 20010410
 djmw 20020813 GPL header
*/

#include "Sound_to_MFCC.h"
#include "Sound_and_FilterBank.h"

MFCC Sound_to_MFCC (Sound me, long numberOfCoefficients, double analysisWidth,
	double dt, double f1_mel, double fmax_mel, double df_mel)
{
	MelFilter mf;
	MFCC mfcc = NULL;
	
	mf = Sound_to_MelFilter (me, analysisWidth, dt, f1_mel, fmax_mel, df_mel);
	if (mf != NULL)
	{
		mfcc = MelFilter_to_MFCC (mf, numberOfCoefficients);
		forget (mf);
	}
	return mfcc;
}

/* End of file Sound_to_MFCC.c */
