#ifndef _Polygon_h_
#define _Polygon_h_
/* Polygon.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 2011/06/18
 */

#include "Data.h"
#include "Graphics.h"

#ifdef __cplusplus
	extern "C" {
#endif

#define Polygon_members  Data_members \
	long numberOfPoints; \
	double *x, *y;
#define Polygon_methods  Data_methods
class_create (Polygon, Data);

Polygon Polygon_create (long numberOfPoints);
/*
	Function:
		create a new instance of Polygon.
	Return value:
		a pointer to the newly created object, or NULL in case of failure.
	Precondition:
		numberOfPoints >= 1;
	Failure:
		Out of memory.
	Postconditions:
		result -> numberOfPoints == numberOfPoints;
		result -> x [1..numberOfPoints] == 0.0;
		result -> y [1..numberOfPoints] == 0.0;
*/

void Polygon_randomize (I);   /* Randomize the order of the points. */

double Polygon_perimeter (I);   /* Return the length of the closed path through all points. */

void Polygon_salesperson (I, long numberOfIterations);
/*
	Function:
		change the order of the points in such a way that it defines the shortest closed path.
	Preconditions:
		me != NULL;
		numberOfIterations >= 1;
	Postconditions:
		my numberOfPoints == my old numberOfPoints;
		Polygon_perimeter (me) <= old Polygon_perimeter (me);
*/

/*** Drawing routines. ***/

void Polygon_draw (I, Graphics g, double xmin, double xmax, double ymin, double ymax);
void Polygon_drawClosed (I, Graphics g, double xmin, double xmax, double ymin, double ymax);

void Polygon_paint (I, Graphics g, Graphics_Colour colour, double xmin, double xmax, double ymin, double ymax);

void Polygon_drawCircles (I, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double diameter_mm);

void Polygon_paintCircles (I, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double diameter_mm);

void Polygons_drawConnection (I, thou, Graphics g,
	double xmin, double xmax, double ymin, double ymax, int hasArrow, double relativeLength);

#ifdef __cplusplus
	}
#endif

/* End of file Polygon.h */
#endif
