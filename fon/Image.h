#ifndef _Image_h_
#define _Image_h_
/* Image.h
 *
 * Copyright (C) 1992-2011,2015,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Sampled.h"
#include "Graphics.h"

#include "Image_def.h"

/*
	An object of type Image represents a quantized function z (x, y)
	on the domain [xmin, xmax] * [ymin, ymax].
	The domain has been sampled in the x and y directions
	with constant sampling intervals (dx and dy) along each direction.
	The samples are thus z [iy] [ix] with ix = 1..nx and iy = 1..ny.
	These represent the function values z (x1 + (ix - 1) * dx, y1 + (iy - 1) * dy).
*/

//int Image_init
//	(Image me, double xmin, double xmax, integer nx, double dx, double x1,
//		       double ymin, double ymax, integer ny, double dy, double y1);

//Image Image_create
//	(double xmin, double xmax, integer nx, double dx, double x1,
//	 double ymin, double ymax, integer ny, double dy, double y1);
/*
	Function:
		return a new empty Image.
	Preconditions:
		xmin <= xmax;   ymin <= ymax;
		nx >= 1;  ny >= 1;
		dx > 0.0;   dy > 0.0;
	Postconditions:
		result -> xmin == xmin;
		result -> xmax == xmax;
		result -> ymin == ymin;
		result -> ymax == ymax;
		result -> nx == nx;
		result -> ny == ny;
		result -> dx == dx;
		result -> dy == dy;
		result -> x1 == x1;
		result -> y1 == y1;
		result -> z [1..ny] [1..nx] == 0;
*/

//integer Image_getWindowExtrema (Image me, integer ixmin, integer ixmax, integer iymin, integer iymax,
//	unsigned char *minimum, unsigned char *maximum);

//void Image_paint (Image me, Graphics g, double xmin, double xmax, double ymin, double ymax,
//	unsigned char minimum, unsigned char maximum);
/*
	Every sample is drawn as a grey rectangle.
	The larger the value of the sample, the darker the rectangle.
*/

/* End of file Image.h */
#endif
