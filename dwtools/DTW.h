#ifndef _DTW_h_
#define _DTW_h_
/* DTW.h
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

#ifndef _Spectrogram_h_
	#include "Spectrogram.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif

#include "DTW_def.h"
#define DTW_methods Matrix_methods
oo_CLASS_CREATE (DTW, Matrix)

Any DTW_create (double tminp, double tmaxp, long ntp, double dtp, double t1p,
	double tminc, double tmaxc, long ntc, double dtc, double t1c); 

void DTW_findPath (DTW me, int matchStart, int matchEnd, int slope);
/*
	Function:
		Calculate the minimum path (through a distance matrix).
		 
	Possible constraints:
	
	matchStart:	path starts at position (1,1) (bottom-left)
	matchEnd:	path ends at (I,J) (top-right)
	slope:	1	no constraints
			2	1/3 < slope < 3
			3	1/2 < slope < 2
			4	2/3 < slope < 3/2
					
	For algorithm and DP-equations see the article (& especially table I) in:
	Sakoe, H. & Chiba, S. (1978), Dynamic Programming Algorithm Optimization
		for Spoken Word	recognition, IEEE Trans. on ASSP, vol 26, 43-49.
*/

double DTW_getPathY (DTW me, double t, int lowest);
/*
	Get the time Y-time that corresponds to time t (along X).
	When the path goes "vertical", you can query the lowest/highest y-time.
	lowest = 0 :get highest.
	When t is outside the x-domain of the DTW-object, the value of t 
	will be returned. 
*/


void DTW_paintDistances (DTW me, Any g, double xmin, double xmax, double ymin,
	double ymax, double minimum, double maximum, int garnish);

void DTW_drawPath (DTW me, Any g, double xmin, double xmax, double ymin,
	double ymax, int garnish);
	
void DTW_drawDistancesAlongPath (DTW me, Any g, double xmin, double xmax,
	double dmin, double dmax, int garnish);
	
Matrix DTW_distancesToMatrix (DTW me);
	
DTW Spectrograms_to_DTW (Spectrogram me, Spectrogram thee, int matchStart,
	int matchEnd, int slope);
 
#endif /* _DTW_h_ */
