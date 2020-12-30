#ifndef _LineSpectralFrequencies_h_
#define _LineSpectralFrequencies_h_
/* LineSpectralFrequencies.h
 *
 * Copyright (C) 2016-2020 David Weenink
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

#include "LPC.h"
#include "Matrix.h"
#include "Graphics.h"

#include "LineSpectralFrequencies_def.h"

/*
	From Sampled:
	xmin, xmax : range of time (s)
	x1 : position of first frame (s)
	dx : step size (s)
	nx : number of frames
*/

void LineSpectralFrequencies_init (LineSpectralFrequencies me, double tmin, double tmax, integer nt, double dt, double t1, integer numberOfFrequencies, double maximumFrequency);

autoLineSpectralFrequencies LineSpectralFrequencies_create (double tmin, double tmax, integer nt, double dt, double t1, integer numberOfFrequencies, double maximumFrequency);

void LineSpectralFrequencies_drawFrequencies (LineSpectralFrequencies me, Graphics g, double fromTime, double toTime, double fmin, double fmax, bool garnish);

autoMatrix LineSpectralFrequencies_downto_Matrix (LineSpectralFrequencies me);

/******************* Frames ************************************************/

void LineSpectralFrequencies_Frame_init (LineSpectralFrequencies_Frame me, integer numberOfFrequencies);

static inline autoVEC LineSpectralFrequencies_listFrequenciesInFrame (LineSpectralFrequencies me, integer frameNumber) {
	my checkIndex (frameNumber);
	autoVEC result = zero_VEC (my maximumNumberOfFrequencies);
	result.all() <<= my d_frames [frameNumber]. frequencies.get();
	return result;
}

static inline autoMAT LineSpectralFrequencies_listAllFrequencies (LineSpectralFrequencies me) {
	autoMAT result = zero_MAT (my maximumNumberOfFrequencies, my nx);
	for (integer iframe = 1; iframe <= my nx; iframe ++) {
		LineSpectralFrequencies_Frame lsf = & my d_frames [iframe];
		result.column (iframe) . part (1, lsf ->numberOfFrequencies) <<= lsf -> frequencies.get();
	}
	return result;
}


#endif /* _LineSpectralFrequencies_h_ */
