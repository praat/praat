/* Polygon_extensions.c
 *
 * Copyright (C) 1993-2002 David Weenink
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
 djmw 20020813 GPL header
*/

#include "Polygon_extensions.h"
#include "Matrix.h"
#include "NUM2.h"

void Polygon_translate (I, double xt, double yt)
{
	iam (Polygon);
	long i;
	
	for (i=1; i <= my numberOfPoints; i++)
	{
		my x[i] += xt;
		my y[i] += yt;
	}
}

/* rotate w.r.t. (xc,yc) */
void Polygon_rotate (I, double alpha, double xc, double yc)
{
	iam (Polygon);
	long i;
	double cosa = cos (alpha), sina = sin (alpha);
	
	Polygon_translate (me, -xc, -yc);
	for (i=1; i <= my numberOfPoints; i++)
	{
		double x = my x[i];
		my x[i] = cosa * my x[i] - sina * my y[i];
		my y[i] = sina * x       + cosa * my y[i];
	}
	Polygon_translate (me, xc, yc);
}
	
void Polygon_scale (I, double xs, double ys)
{
	iam (Polygon);
	long i;
	
	for (i=1; i <= my numberOfPoints; i++)
	{
		my x[i] *= xs;
		my y[i] *= ys;
	}
}

void Polygon_reverseX (I)
{
	iam (Polygon);
	long i;
	
	for (i=1; i <= my numberOfPoints; i++)
	{
		my x[i] = -my x[i];
	}
	
}

void Polygon_reverseY (I)
{
	iam (Polygon);
	long i;
	for (i=1; i <= my numberOfPoints; i++)
	{
		my y[i] = -my y[i];
	}
	
}

void Polygon_Categories_draw (I, thou, Any graphics, double xmin, double xmax,
	double ymin, double ymax, int garnish)
{
    iam (Polygon); thouart (Categories);
    double min, max, tmp;
	long i;

    if (my numberOfPoints != thy size) return;
	
    if (xmax == xmin)
    {
		NUMfvector_extrema (my x, 1, my numberOfPoints, & min, & max);
		tmp = max - min == 0 ? 0.5 : 0.0;
		xmin = min - tmp; xmax = max + tmp;
    }
	
    if (ymax == ymin)
    {
		NUMfvector_extrema (my y, 1, my numberOfPoints, & min, & max);
		tmp = max - min == 0 ? 0.5 : 0.0;
		ymin = min - tmp; ymax = max + tmp;
    }
	    
    Graphics_setInner (graphics);
    Graphics_setWindow (graphics, xmin, xmax, ymin, ymax);
    Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_HALF);
	
    for (i=1; i <= my numberOfPoints; i++)
	{
		OrderedOfString_drawItem (thee, graphics, i, my x[i], my y[i]);
	}
    Graphics_unsetInner (graphics);
    if (garnish)
    {
		Graphics_drawInnerBox (graphics);
		Graphics_marksLeft (graphics, 2, 1, 1, 0);
		if (ymin * ymax < 0.0)
		{
			Graphics_markLeft (graphics, 0.0, 1, 1, 1, NULL);
		}
		Graphics_marksBottom (graphics, 2, 1, 1, 0);
		if (xmin * xmax < 0.0)
		{
			Graphics_markBottom (graphics, 0.0, 1, 1, 1, NULL);
		}
	}
}

static void setWindow (Polygon me, Any graphics,
	double xmin, double xmax, double ymin, double ymax)
{
	Melder_assert (me);
	
	if (xmax <= xmin)   /* Autoscaling along x axis. */
	{
		long i;
		xmax = xmin = my x [1];
		for (i = 2; i <= my numberOfPoints; i ++)
		{
			if (my x [i] < xmin) xmin = my x [i];
			if (my x [i] > xmax) xmax = my x [i];
		}
		if (xmin == xmax)
		{
			xmin -= 1.0;
			xmax += 1.0;
		}
	}
	if (ymax <= ymin)   /* Autoscaling along y axis. */
	{
		long i;
		
		ymax = ymin = my y [1];
		for (i = 2; i <= my numberOfPoints; i ++)
		{
			if (my y [i] < ymin) ymin = my y [i];
			if (my y [i] > ymax) ymax = my y [i];
		}
		if (ymin == ymax)
		{
			ymin -= 1.0;
			ymax += 1.0;
		}
	}
	Graphics_setWindow (graphics, xmin, xmax, ymin, ymax);
}

void Polygon_drawMarks (I, Graphics g, double xmin, double xmax,
	double ymin, double ymax, double size_mm, const wchar_t *mark)
{
	iam (Polygon);
	long i;
	
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	for (i=1; i <= my numberOfPoints; i++)
	{
		Graphics_mark (g, my x[i], my y[i], size_mm, mark);
	}
	Graphics_unsetInner (g);
}

#define AREA2(x1,y1,x2,y2,x3,y3) ((x2-x1)*(y3-y1)-(x3-x1)*(y2-y1))

/*
	Draw a line from (x1,y1) to (x2,y2), only inside (or) outside the polygon.

static void Polygon_drawLine (I, float x1, float y1, float x2, float y2, 
	int outside)
{
	iam (Polygon);
	long i, nint = 0;

	for (i = 1; i <= my numberOfPoints; i++)
	{

	}
}
*/
/* End of file Polygon_extensions.c */
