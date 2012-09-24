#ifndef _ParamCurve_h_
#define _ParamCurve_h_
/* ParamCurve.h
 *
 * Copyright (C) 1992-2011,2012 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
	Parametrized curve (x (t), y (t)):
		two functions (x and y) of one variable (the parameter t).
	Sampled parametrized curve (x [i], y [j]):
		x [i] = x (tx [i]) = x (tx1 + (i - 1) * dtx);
		y [i] = y (ty [i]) = y (ty1 + (i - 1) * dty);
*/

#include "Sound.h"
#include "Graphics.h"

#include "ParamCurve_def.h"
oo_CLASS_CREATE (ParamCurve, Function);

void ParamCurve_init (ParamCurve me, Sound x, Sound y);

ParamCurve ParamCurve_create (Sound x, Sound y);
/*
	Return value:
		a newly created ParamCurve object,
		or NULL in case of failure.
	Failures:
		Out of memory.
		Domains do not overlap:
			x -> xmax <= y -> xmin || x -> xmin >= y -> xmax.
	Postconditions:
		(Result's domain is intersection of both domains:)
		result -> xmin = max (x -> xmin, y -> xmin);
		result -> xmax = min (x -> xmax, y -> xmax);
*/

void ParamCurve_draw (ParamCurve me, Graphics g, double t1, double t2, double dt,
	double x1, double x2, double y1, double y2, int garnish);
/*
	Function:
		draw the points of the curve between parameter values t1 and t2,
		in time steps dt starting at t1 and including t2,
		along the axes [x1, x2] x [y1, y2].
	Defaults:
		t2 <= t1: draw all (overlapping) points.
		dt <= 0.0: time step is the smaller of my x -> dx and my y -> dx.
		x2 <= x1: autoscaling along horizontal axis.
		y2 <= y1: autoscaling along vertical axis.
	Return value:
		1 if OK, 0 if out of memory.
*/

void ParamCurve_swapXY (ParamCurve me);
/*
	Reflect around y = x.
	Postconditions:
		x == old y;
		y == old x;
*/

/* End of file ParamCurve.h */
#endif

