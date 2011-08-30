#ifndef _Sampled2_h_
#define _Sampled2_h_
/* Sampled2.h
 *
 * Copyright (C) 1993-2011 David Weenink & Paul Boersma
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

#include "Sampled.h"

#include "Sampled2_def.h"
oo_CLASS_CREATE (Sampled2, Sampled);

void Sampled2_init (I, double xmin, double xmax, long nx, double dx, double x1,
	double ymin, double ymax, long ny, double dy, double y1);

long Sampled2_getWindowSamplesX (I, double xmin, double xmax, long *ixmin, long *ixmax);
/*
	Function:
		return the number of samples with x values in [xmin, xmax].
		Put the first of these samples in ixmin.
		Put the last of these samples in ixmax.
	Postconditions:
		*ixmin >= 1;
		*ixmax <= my nx;
		if (result != 0) *ixmin <= *ixmax; else *ixmin > *ixmax;
		if (result != 0) result == *ixmax - *ixmin + 1;
*/

double Sampled2_columnToX (I, double column);   /* Return my x1 + (column - 1) * my dx.	 */

double Sampled2_rowToY (I, double row);   /* Return my y1 + (row - 1) * my dy. */

double Sampled2_xToColumn (I, double x);   /* Return (x - xmin) / my dx + 1. */

long Sampled2_xToLowColumn (I, double x);   /* Return floor (Matrix_xToColumn (me, x)). */

long Sampled2_xToHighColumn (I, double x);   /* Return ceil (Matrix_xToColumn (me, x)). */

long Sampled2_xToNearestColumn (I, double x);   /* Return floor (Matrix_xToColumn (me, x) + 0.5). */

double Sampled2_yToRow (I, double y);   /* Return (y - ymin) / my dy + 1. */

long Sampled2_yToLowRow (I, double y);   /* Return floor (Matrix_yToRow (me, y)). */

long Sampled2_yToHighRow (I, double x);   /* Return ceil (Matrix_yToRow (me, y)). */

long Sampled2_yToNearestRow (I, double y);   /* Return floor (Matrix_yToRow (me, y) + 0.5). */

long Sampled2_getWindowSamplesY (I, double ymin, double ymax, long *iymin, long *iymax);

/**** a little bit dangerous ******/

long Sampled2_getWindowExtrema_d (I, double **z, long ixmin, long ixmax,
	long iymin, long iymax, double *minimum, double *maximum);
	
long Sampled2_getWindowExtrema_f (I, float **z, long ixmin, long ixmax,
	long iymin, long iymax, double *minimum, double *maximum);
/*
	Function:
		compute the minimum and maximum values of z over all samples inside [ixmin, ixmax] * [iymin, iymax].
	Arguments:
		if ixmin = 0, start at first column; if ixmax = 0, end at last column (same for iymin and iymax).
	Return value:
		the number of samples inside the window.
	Postconditions:
		if result == 0, *minimum and *maximum are not changed;
*/

#endif /* _Sampled2_h_ */
