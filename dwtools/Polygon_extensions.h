#ifndef _Polygon_extensions_h_
#define _Polygon_extensions_h_
/* Polygon_extensions.h
 *
 * Copyright (C) 1993-2011 David Weenink
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
 djmw 1996
 djmw 20020813 GPL header
 djmw 20110307 Latest modification
*/

#include "Polygon.h"
#include "Categories.h"
#include "Graphics.h"
#include "Sound.h"

#define Polygon_INSIDE -1
#define Polygon_OUTSIDE 0
#define Polygon_BOUNDARY 1
#define Polygon_EDGE 3
#define Polygon_VERTEX 5

#ifdef __cplusplus
	extern "C" {
#endif

Polygon Polygon_createSimple (wchar_t *xystring);
Polygon Polygon_createFromRandomVertices (long numberOfVertices, double xmin, double xmax, double ymin, double ymax);
Polygon Polygon_simplify (Polygon me);
void Polygon_translate (I, double xt, double yt);
void Polygon_rotate (I, double alpha, double xc, double yc);
void Polygon_scale (I, double xs, double ys);
void Polygon_reverseX (I);
void Polygon_reverseY (I);
Polygon Polygon_circularPermutation (Polygon me, long nshift);

// Is point (x,y) Inside, Outside, Boundary (Edge or Vertex) ?
int Polygon_getLocationOfPoint (Polygon me, double x0, double y0, double eps);

void Polygon_Categories_draw (I, Any categories, Any graphics, double xmin, double xmax,
	double ymin, double ymax, int garnish);
/* reverse axis when min > max */

void Polygon_drawMarks (I, Graphics g, double xmin, double xmax,
	double ymin, double ymax, double size_mm, const wchar_t *mark);

Polygon Sound_to_Polygon (Sound me, int channel, double tmin, double tmax, double ymin, double ymax, double level);
/*
	Post-conditions:

	If there are ns = i2 - i1 + 1 samples of the sound in the interval (tmin, tmax), the first at index i1
	and the last at index i2, the Polygon will have np = 2 + ns + 2 + 1 points.
	i1 = Sampled_xToHighIndex (me, tmin)
	i2 = Sampled_xToLowIndex (me, tmax)
	thy x[1] = tmin
	thy y[1] = clip(level)
	thy x[2] = tmin
	thy y[2] = interpolated amplitude in 'channel' at tmin (or, averaged over all channels if channel = 0)
	thy x[3] = my x1 + (i1 - 1) * my dx
	thy y[3] = clip(my z[channel][i1]) (or, averaged over all channels if channel == 0)
	...
	thy x[ns+2] = my x1 +(i2 - 1) * my dx
	thy y[ns+2] = clip(my z[channel][i2]) (or, averaged over all channels if channel == 0)
	thy x[ns+3] = tmax
	thy y[ns+3] = clip(interpolated amplitude in 'channel' at tmax (or, averaged over all channels if channel == 0))
	thy x[ns+4] = tmax
	thy y[ns+4] = clip(level)
	thy x[ns+5] = thy x[1] (close the path)
	thy y[ns+5] = thy y[1]

	where clip(y) = y < ymin ? ymin : y > ymax ? ymax ; y;
*/

Polygon Sounds_to_Polygon_enclosed (Sound me, Sound thee, int channel, double tmin, double tmax, double ymin, double ymax);
/* Area enclosed by the sounds */

Polygon Polygons_union (Polygon me, Polygon thee);

#ifdef __cplusplus
	}
#endif

#endif /* _Polygon_extensions_h_ */
