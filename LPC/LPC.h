#ifndef _LPC_h_
#define _LPC_h_
/* LPC.h
 *
 * Copyright (C) 1994-2012, 2015-2017 David Weenink
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

#include "Matrix.h"
#include "Graphics.h"

#include "LPC_def.h"

/*
	From Sampled:
	xmin, xmax : range of time (s)
	x1 : position of first frame (s)
	dx : step size (s)
	nx : number of frames
*/

void LPC_init (LPC me, double tmin, double tmax, integer nt, double dt, double t1, int predictionOrder, double samplingPeriod);

autoLPC LPC_create (double tmin, double tmax, integer nt, double dt, double t1, int predictionOrder, double samplingPeriod);

void LPC_drawGain (LPC me, Graphics g, double t1, double t2, double gmin, double gmax, bool garnish);

void LPC_drawPoles (LPC me, Graphics g, double time, int garnish);

autoMatrix LPC_downto_Matrix_lpc (LPC me);

autoMatrix LPC_downto_Matrix_rc (LPC me);

autoMatrix LPC_downto_Matrix_area (LPC me);

/******************* Frames ************************************************/

void LPC_Frame_init (LPC_Frame me, int nCoefficients);

#endif /* _LPC_h_ */
