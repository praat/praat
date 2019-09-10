#ifndef _CC_h_
#define _CC_h_
/* CC.h
 *
 * Copyright (C) 1993-2019 David Weenink
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

/*
 djmw 20010219 Cepstral Coefficients (abstract) class.
 djmw 20020402 GPL header
 djmw 20030612 Include CC_def.h
 djmw 20110306 Latest modification.
*/

#include "Graphics.h"
#include "Matrix.h"

#include "CC_def.h"

void CC_init (CC me, double tmin, double tmax, integer nt, double dt, double t1, integer maximumNumberOfCoefficients, double fmin, double fmax);

void CC_getNumberOfCoefficients_extrema (CC me, integer startframe, integer endframe, integer *min, integer *max);

integer CC_getMaximumNumberOfCoefficientsUsed (CC me);

integer CC_getMinimumNumberOfCoefficients (CC me, integer startframe, integer endframe);

integer CC_getMaximumNumberOfCoefficients (CC me, integer startframe, integer endframe);

integer CC_getNumberOfCoefficients (CC me, integer iframe);

double CC_getValueInFrame (CC me, integer iframe, integer index);

double CC_getC0ValueInFrame (CC me, integer iframe);

void CC_paint (CC me, Graphics g, double xmin, double xmax, integer cmin, integer cmax, double minimum, double maximum, bool garnish);

void CC_drawC0 (CC me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool garnish);

autoMatrix CC_to_Matrix (CC me);

double CC_getValue (CC me, double t, integer index);

/******************* Frames ************************************************/

void CC_Frame_init (CC_Frame me, integer numberOfCoefficients);

#endif /* _CC_h_ */
