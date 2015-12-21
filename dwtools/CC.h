#ifndef _CC_h_
#define _CC_h_
/* CC.h
 *
 * Copyright (C) 1993-2014 David Weenink
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
oo_CLASS_CREATE (CC);

void CC_init (CC me, double tmin, double tmax, long nt, double dt, double t1, long maximumNumberOfCoefficients, double fmin, double fmax);

void CC_getNumberOfCoefficients_extrema (CC me, long startframe, long endframe, long *min, long *max);

long CC_getMinimumNumberOfCoefficients (CC me, long startframe, long endframe);

long CC_getMaximumNumberOfCoefficients (CC me, long startframe, long endframe);

long CC_getNumberOfCoefficients (CC me, long iframe);

double CC_getValueInFrame (CC me, long iframe, long index);

double CC_getC0ValueInFrame (CC me, long iframe);

void CC_paint (CC me, Graphics g, double xmin, double xmax, long cmin, long cmax, double minimum, double maximum, int garnish);

void CC_drawC0 (CC me, Graphics g, double xmin, double xmax, double ymin, double ymax, int garnish);

autoMatrix CC_to_Matrix (CC me);

double CC_getValue (CC me, double t, long index);

/******************* Frames ************************************************/

void CC_Frame_init (CC_Frame me, long numberOfCoefficients);

#endif /* _CC_h_ */
