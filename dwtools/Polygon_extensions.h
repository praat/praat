#ifndef _Polygon_extensions_h_
#define _Polygon_extensions_h_
/* Polygon_extensions.h
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
 djmw 1996
 djmw 20020813 GPL header
*/

#include "Polygon.h"
#include "Categories.h"
#include "Graphics.h"

void Polygon_translate (I, double xt, double yt);
void Polygon_rotate (I, double alpha, double xc, double yc);
void Polygon_scale (I, double xs, double ys);
void Polygon_reverseX (I);
void Polygon_reverseY (I);

void Polygon_Categories_draw (I, Any categories, Any graphics, double xmin, double xmax, 
	double ymin, double ymax, int garnish);
/* reverse axis when min > max */

void Polygon_drawMarks (I, Graphics g, double xmin, double xmax,
	double ymin, double ymax, double size_mm, const char *mark);

#endif /* _Polygon_extensions_h_ */
