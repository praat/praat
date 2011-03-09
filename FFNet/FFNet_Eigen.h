#ifndef _FFNet_Eigen_h_
#define _FFNet_Eigen_h_
/* FFNet_Eigen.h
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
 djmw 19940726
 djmw 20110307 Latest modification.
*/

#include "FFNet.h"
#include "Eigen.h"

#ifdef __cplusplus
	extern "C" {
#endif

void FFNet_Eigen_drawIntersection (FFNet me, Eigen eigen, Graphics g, long pcx, long pcy,
    double xmin, double xmax, double ymin, double ymax);
/* Draw intersections of hyperplanes of units in layer 1 with eigenplane */
/* formed by pcx and pcy. */
/* pcx (pcy) > 0	: negative values left (bottom), positive right (top) */
/* pcx (pcy) < 0	: negative values right (top). */

void FFNet_Eigen_drawDecisionPlaneInEigenspace (FFNet me, thou, Graphics g, long unit, 
	long layer,	long pcx, long pcy, double xmin, double xmax, double ymin, double ymax);

#ifdef __cplusplus
	}
#endif

#endif /* _FFNet_Eigen_h_ */
