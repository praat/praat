#ifndef _CC_h_
#define _CC_h_
/* CC.h
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
 djmw 20010219 Cepstral Coefficients (abstract) class.
 djmw 20020402 GPL header
 djmw 20030612 Include CC_def.h
 djmw 20110306 Latest modification.
*/

#include "Graphics.h"
#include "Matrix.h"

#include "CC_def.h"
oo_CLASS_CREATE (CC, Sampled);

void CC_init (I, double tmin, double tmax, long nt, double dt, double t1,
	long maximumNumberOfCoefficients, double fmin, double fmax);

void CC_getNumberOfCoefficients_extrema (I, long startframe,
	long endframe, long *min, long *max);

long CC_getMinimumNumberOfCoefficients (I, long startframe, long endframe);

long CC_getMaximumNumberOfCoefficients (I, long startframe, long endframe);

void CC_paint (I, Graphics g, double xmin, double xmax, long cmin,
	long cmax, double minimum, double maximum, int garnish);

void CC_drawC0 (I, Graphics g, double xmin, double xmax, double ymin,
	double ymax, int garnish);

Matrix CC_to_Matrix (I);

double CC_getValue (I, double t, long index);

/******************* Frames ************************************************/

void CC_Frame_init (CC_Frame me, long numberOfCoefficients);

#endif /* _CC_h_ */
