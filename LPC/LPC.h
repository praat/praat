#ifndef _LPC_h_
#define _LPC_h_
/* LPC.h
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
 djmw 20030526 GPL header
 djmw 20110306 Latest modification.
*/

#ifndef _Matrix_h_
	#include "Matrix.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

#include "LPC_def.h"
#define LPC_methods Sampled_methods
oo_CLASS_CREATE (LPC, Sampled);

/*
	From Sampled:
	xmin, xmax : range of time (s)
	x1 : position of first frame (s)
	dx : step size (s)
	nx : number of frames
*/	

int LPC_init (LPC me, double tmin, double tmax, long nt, double dt, double t1,
	int predictionOrder, double samplingPeriod);

Any LPC_create (double tmin, double tmax, long nt, double dt, double t1,
	int predictionOrder, double samplingPeriod);

void LPC_drawGain (LPC me, Graphics g, double t1, double t2,
	double gmin, double gmax, int garnish);
	
void LPC_drawPoles (LPC me, Graphics g, double time, int garnish);

Matrix LPC_to_Matrix (LPC me);

/******************* Frames ************************************************/

int LPC_Frame_init (LPC_Frame me, int nCoefficients);

#ifdef __cplusplus
	}
#endif

#endif /* _LPC_h_ */
