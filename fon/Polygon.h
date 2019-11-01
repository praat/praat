#ifndef _Polygon_h_
#define _Polygon_h_
/* Polygon.h
 *
 * Copyright (C) 1992-2005,2007-2009,2011,2015-2017,2019 Paul Boersma
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

#include "Data.h"
#include "Graphics.h"

#include "Polygon_def.h"

autoPolygon Polygon_create (integer numberOfPoints);
/*
	Function:
		create a new instance of Polygon.
	Return value:
		the newly created object.
	Precondition:
		numberOfPoints >= 1;
	Failure:
		Out of memory.
	Postconditions:
		result -> numberOfPoints == numberOfPoints;
		result -> x [1..numberOfPoints] == 0.0;
		result -> y [1..numberOfPoints] == 0.0;
*/

void Polygon_randomize (Polygon me);   /* Randomize the order of the points. */

double Polygon_perimeter (Polygon me);   /* Return the length of the closed path through all points. */

void Polygon_salesperson (Polygon me, integer numberOfIterations);
/*
	Function:
		change the order of the points in such a way that it defines the shortest closed path.
	Preconditions:
		!! me;
		numberOfIterations >= 1;
	Postconditions:
		my numberOfPoints == my old numberOfPoints;
		Polygon_perimeter (me) <= old Polygon_perimeter (me);
*/

/*** Drawing routines. ***/

void Polygon_draw (Polygon me, Graphics g, double xmin, double xmax, double ymin, double ymax);
void Polygon_drawClosed (Polygon me, Graphics g, double xmin, double xmax, double ymin, double ymax);

void Polygon_paint (Polygon me, Graphics g, MelderColour colour, double xmin, double xmax, double ymin, double ymax);

void Polygon_drawCircles (Polygon me, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double diameter_mm);

void Polygon_paintCircles (Polygon me, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double diameter_mm);

void Polygons_drawConnection (Polygon me, Polygon thee, Graphics g,
	double xmin, double xmax, double ymin, double ymax, int hasArrow, double relativeLength);

/* End of file Polygon.h */
#endif
