#ifndef _FFNet_Eigen_h_
#define _FFNet_Eigen_h_
/* FFNet_Eigen.h
 *
 * Copyright (C) 1994-2017 David Weenink
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
 djmw 19940726
 djmw 20110307 Latest modification.
*/

#include "FFNet.h"
#include "Eigen.h"

void FFNet_Eigen_drawIntersection (FFNet me, Eigen eigen, Graphics g, integer pcx, integer pcy,
    double xmin, double xmax, double ymin, double ymax);
/* Draw intersections of hyperplanes of units in layer 1 with eigenplane */
/* formed by pcx and pcy. */
/* pcx (pcy) > 0	: negative values left (bottom), positive right (top) */
/* pcx (pcy) < 0	: negative values right (top). */

void FFNet_Eigen_drawDecisionPlaneInEigenspace (FFNet me, Eigen thee, Graphics g, integer unit,
	integer layer,	integer pcx, integer pcy, double xmin, double xmax, double ymin, double ymax);

#endif /* _FFNet_Eigen_h_ */
