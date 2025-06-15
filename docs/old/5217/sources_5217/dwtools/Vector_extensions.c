/*
*/
/* Vector_extensions.c
 * 
 * Copyright (C) 2006 David Weenink
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
 djmw 20060516 First
 */

#include "Vector_extensions.h"

/*
	Problems with not left to right: marks etc
*/

void Vector_draw_special (I, Graphics g, double *pxmin, double *pxmax, double *pymin, double *pymax,
	double defaultDy, const char *method, char *direction)
{
	iam (Vector);
	long ixmin, ixmax, ix;
	int vertical = direction != 0 && streq (direction, "v");
	
	/*
	 * Automatic domain.
	 */
	if (*pxmin == *pxmax) {
		*pxmin = my xmin;
		*pxmax = my xmax;
	}
	/*
	 * Domain expressed in sample numbers.
	 */
	Matrix_getWindowSamplesX (me, *pxmin, *pxmax, & ixmin, & ixmax);
	/*
	 * Automatic vertical range.
	 */
	if (*pymin == *pymax) {
		Matrix_getWindowExtrema (me, ixmin, ixmax, 1, 1, pymin, pymax);
		if (*pymin == *pymax) {
			*pymin -= defaultDy;
			*pymax += defaultDy;
		}
	}
	
	/*
	 * Set coordinates for drawing.
	 */
	Graphics_setInner (g);
	if (vertical)
	{
		double t = *pxmin;
		*pxmin = *pxmax; *pxmax = t;
		t = *pymin;
		*pymin = *pymax; *pymax = t;
	}
	
	Graphics_setWindow (g, *pxmin, *pxmax, *pymin, *pymax);
	
	if (strstr (method, "bars")) {
		for (ix = ixmin; ix <= ixmax; ix ++) {
			double x = Sampled_indexToX (me, ix);
			double y = my z [1] [ix];
			double left = x - 0.5 * my dx, right = x + 0.5 * my dx;
			if (vertical)
			{
				
			}
			if (y > *pymax) y = *pymax;
			if (left < *pxmin) left = *pxmin;
			if (right > *pxmax) right = *pxmax;
			Graphics_line (g, left, y, right, y);
			Graphics_line (g, left, y, left, *pymin);
			Graphics_line (g, right, y, right, *pymin);
		}
	} else if (strstr (method, "poles")) {
		for (ix = ixmin; ix <= ixmax; ix ++) {
			double x = Sampled_indexToX (me, ix);
			Graphics_line (g, x, 0, x, my z [1] [ix]);
		}
	} else if (strstr (method, "speckles")) {
		for (ix = ixmin; ix <= ixmax; ix ++) {
			double x = Sampled_indexToX (me, ix);
			Graphics_fillCircle_mm (g, x, my z [1] [ix], 1.0);
		}
	} else {
		/*
		 * The default default: draw as a curve.
		 */
		Graphics_function (g, my z [1], ixmin, ixmax,
			Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax));
	}
	Graphics_unsetInner (g);
}
