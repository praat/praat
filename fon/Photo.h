#ifndef _Photo_h_
#define _Photo_h_
/* Photo.h
 *
 * Copyright (C) 2013 Paul Boersma
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

#include "Matrix.h"

#include "Photo_def.h"
oo_CLASS_CREATE (Photo, SampledXY);


Photo Photo_create
	(double xmin, double xmax, long nx, double dx, double x1,
	 double ymin, double ymax, long ny, double dy, double y1);
/*
	Function:
		return a new opaque black Photo, or NULL if out of memory.
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
		result -> d_red -> z [1..ny] [1..nx] == 0.0;
		result -> d_green -> z [1..ny] [1..nx] == 0.0;
		result -> d_blue -> z [1..ny] [1..nx] == 0.0;
		result -> d_transparency -> z [1..ny] [1..nx] == 0.0;
*/

Photo Photo_createSimple (long numberOfRows, long numberOfColumns);
/*
	Function:
		return a new opaque black Photo, or NULL if out of memory.
	Preconditions:
		numberOfRows >= 1;  numberOfColumns >= 1;
	Postconditions:
		result -> xmin == 0.5;
		result -> xmax == numberOfColumns + 0.5;
		result -> ymin == 0.5;
		result -> ymax == numberOfRows + 0.5;
		result -> nx == numberOfColumns;
		result -> ny == numberOfRows;
		result -> dx == 1;
		result -> dy == 1;
		result -> x1 == 1;
		result -> y1 == 1;
		result -> d_red -> z [1..ny] [1..nx] == 0.0;
		result -> d_green -> z [1..ny] [1..nx] == 0.0;
		result -> d_blue -> z [1..ny] [1..nx] == 0.0;
		result -> d_transparency -> z [1..ny] [1..nx] == 0.0;
*/

Photo Photo_readFromImageFile (MelderFile file);

/* End of file Photo.h */
#endif
