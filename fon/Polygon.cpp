/* Polygon.cpp
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
 * pb 2002/07/16 GPL
 * pb 2004/01/27 paint with colours
 * pb 2004/06/15 allow reversed axes
 * pb 2007/06/21 tex
 * pb 2007/10/01 canWriteAsEncoding
 * pb 2008/01/19 double
 * pb 2009/12/14 RGB colours
 * pb 2011/06/06 C++
 * pb 2011/06/18 Polygon_drawClosed ()
 */

#include "Polygon.h"

#include "oo_DESTROY.h"
#include "Polygon_def.h"
#include "oo_COPY.h"
#include "Polygon_def.h"
#include "oo_EQUAL.h"
#include "Polygon_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Polygon_def.h"
#include "oo_WRITE_BINARY.h"
#include "Polygon_def.h"
#include "oo_READ_BINARY.h"
#include "Polygon_def.h"
#include "oo_DESCRIPTION.h"
#include "Polygon_def.h"

static void info (I) {
	iam (Polygon);
	classData -> info (me);
	MelderInfo_writeLine2 (L"Number of points: ", Melder_integer (my numberOfPoints));
	MelderInfo_writeLine2 (L"Perimeter: ", Melder_single (Polygon_perimeter (me)));
}
  
static int writeText (I, MelderFile file) {
	iam (Polygon);
	texputi4 (file, my numberOfPoints, L"numberOfPoints", 0,0,0,0,0);
	for (long i = 1; i <= my numberOfPoints; i ++) {
		texputr4 (file, my x [i], L"x [", Melder_integer (i), L"]", 0,0,0);
		texputr4 (file, my y [i], L"y [", Melder_integer (i), L"]", 0,0,0);
	}
	return 1;
}

static int readText (I, MelderReadText text) {
	iam (Polygon);
	my numberOfPoints = texgeti4 (text);
	if (my numberOfPoints < 1)
		Melder_throw ("Cannot read a Polygon with only ", my numberOfPoints, " points.");
	my x = NUMvector <double> (1, my numberOfPoints);
	my y = NUMvector <double> (1, my numberOfPoints);
	for (long i = 1; i <= my numberOfPoints; i ++) {
		my x [i] = texgetr4 (text);
		my y [i] = texgetr4 (text);
	}
	return 1;
}

class_methods (Polygon, Data) {
	us -> version = 1;
	class_method_local (Polygon, destroy)
	class_method (info)
	class_method_local (Polygon, description)
	class_method_local (Polygon, copy)
	class_method_local (Polygon, equal)
	class_method_local (Polygon, canWriteAsEncoding)
	class_method (writeText)
	class_method (readText)
	class_method_local (Polygon, writeBinary)
	class_method_local (Polygon, readBinary)
	class_methods_end
}

Polygon Polygon_create (long numberOfPoints) {
	try {
		autoPolygon me = Thing_new (Polygon);
		my numberOfPoints = numberOfPoints;
		my x = NUMvector <double> (1, numberOfPoints);
		my y = NUMvector <double> (1, numberOfPoints);
		return me.transfer();
	} catch (MelderError) {
		rethrowmzero ("Polygon not created.");
	}
}

void Polygon_randomize (I) {
	iam (Polygon);
	for (long i = 1; i <= my numberOfPoints; i ++) {
		long j = NUMrandomInteger (i, my numberOfPoints);
		double xdum = my x [i];
		double ydum = my y [i];
		my x [i] = my x [j];
		my y [i] = my y [j];
		my x [j] = xdum;
		my y [j] = ydum;
	}
}

double Polygon_perimeter (I) {
	iam (Polygon);
	if (my numberOfPoints < 1) return 0.0;
	double dx = my x [1] - my x [my numberOfPoints], dy = my y [1] - my y [my numberOfPoints];
	double result = sqrt (dx * dx + dy * dy);
	for (long i = 1; i <= my numberOfPoints - 1; i ++) {
		dx = my x [i] - my x [i + 1];
		dy = my y [i] - my y [i + 1];
		result += sqrt (dx * dx + dy * dy);
	}
	return result;
}

static void computeDistanceTable (Polygon me, int **table) {
	for (long i = 1; i <= my numberOfPoints - 1; i ++)
		for (long j = i + 1; j <= my numberOfPoints; j ++) {
			double dx = my x [i] - my x [j], dy = my y [i] - my y [j];
			table [i] [j] = table [j] [i] =
				sqrt (dx * dx + dy * dy);
					/* Round to zero. */
		}
}

static long computeTotalDistance (int **distance, int path [], int numberOfCities) {
	long result = 0;
	for (long i = 1; i <= numberOfCities; i ++)
		result += distance [path [i - 1]] [path [i]];
	return result;
}

static void shuffle (int path [], int numberOfCities) {
	for (long i = 1; i <= numberOfCities; i ++) {
		int j = NUMrandomInteger (i, numberOfCities);
		int help = path [i];
		path [i] = path [j];
		path [j] = help;
	}
	path [0] = path [numberOfCities];
}
  
static int tryExchange (int **distance, int *path, int numberOfCities, long *totalDistance) {
	int result = 0;
	int b1 = path [0];
	int b2nr = 1;
	while (b2nr < numberOfCities - 1) {
		int b2 = path [b2nr];
		int distance_b1_b2 = distance [b1] [b2];
		int d2nr = b2nr + 2;
		int d1 = path [d2nr - 1];
		int cont = 1;
		while (d2nr <= numberOfCities && cont) {
			int d2 = path [d2nr];
			int gain = distance_b1_b2 + distance [d1] [d2] - distance [b1] [d1] - distance [b2] [d2];
			if (gain > 0) {
				int below = b2nr, above = d2nr - 1;
				cont = 0;
				do {
					int help = path [below];
					path [below ++] = path [above];
					path [above --] = help;
				} while (below < above);
				*totalDistance -= gain;
			}
			d1 = d2;
			d2nr ++;
		}
		if (cont) { b1 = b2; b2nr ++; } else result = 1;
	}
	return result;
}

static int tryAdoption (int **distance, int *path, int numberOfCities, long *totalDistance)
{
	int *help = NUMivector (0, numberOfCities);
	int i, maximumGainLeft, result = 0;

	/* Compute maximum distance between two successive cities. */

	int city1 = path [0], city2 = path [1];
	int maximumDistance = distance [city1] [city2];
	for (i = 2; i <= numberOfCities; i ++) {
		city1 = city2;
		city2 = path [i];
		if (distance [city1] [city2] > maximumDistance)
			maximumDistance = distance [city1] [city2];
	}
	maximumGainLeft = maximumDistance;
	for (i = 1; i <= numberOfCities; i ++) {
		int cont = 1, b1, b2, distance_b1_b2, d1nr = 3, cc, e1nrMax = 6;
		int numberOfCitiesMinus1 = numberOfCities - 1, j;
		for (j = 0; j <= numberOfCitiesMinus1; j ++) path [j] = path [j + 1];
		path [numberOfCities] = path [0];
		b1 = path [0];
		b2 = path [1];
		distance_b1_b2 = distance [b1] [b2];
		cc = path [2];
		while (d1nr < numberOfCitiesMinus1 && cont) {
			int d1 = path [d1nr];
			int gain1 = distance_b1_b2 + distance [d1] [cc] - distance [d1] [b2];
			if (gain1 + maximumGainLeft > 0) {
				int e1nr = d1nr + 1;
				int dn = path [d1nr];
				if (e1nrMax > numberOfCitiesMinus1) e1nrMax = numberOfCitiesMinus1;
				while (e1nr < e1nrMax && cont) {
					int e1 = path [e1nr];
					int gain = gain1 + distance [dn] [e1] - distance [dn] [b1] - distance [cc] [e1];
					if (gain > 0) {
						int nAdoption = e1nr - d1nr;
						int dnnr = e1nr - 1;
						cont = 0;
						*totalDistance -= gain;
						for (j = 0; j <= dnnr - 1; j ++) help [j] = path [j + 1];
						for (j = 1; j <= nAdoption; j ++) path [j] = help [dnnr - j];
						for (j = 0; j <= d1nr - 2; j ++) path [nAdoption + j + 1] = help [j];
					}
					dn = e1;
					e1nr ++;
				}
			}
			e1nrMax ++;
			cc = d1;
			d1nr ++;
		}
		result |= ! cont;
	}
	NUMivector_free (help, 0);
	return result;
}

void Polygon_salesperson (I, long numberOfIterations)
{
	iam (Polygon);
	try {
		long numberOfShortest = 1, totalDistance, shortestDistance;

		int numberOfCities = my numberOfPoints;
		if (numberOfCities < 1)
			Melder_throw ("No points.");
		autoNUMmatrix <int> distance (1, numberOfCities, 1, numberOfCities);
		computeDistanceTable (me, distance.peek());
		autoNUMvector <int> path (0L, numberOfCities);
		for (int i = 1; i <= numberOfCities; i ++)
			path [i] = i;
		path [0] = numberOfCities;   /* Close path. */
		autoNUMvector <int> shortestPath (NUMvector_copy (path.peek(), 0, numberOfCities), 0);
		for (long iteration = 1; iteration <= numberOfIterations; iteration ++) {
			if (iteration > 1) shuffle (path.peek(), numberOfCities);
			totalDistance = computeTotalDistance (distance.peek(), path.peek(), numberOfCities);
			if (iteration == 1) shortestDistance = totalDistance;
			do {
				do {
				} while (tryExchange (distance.peek(), path.peek(), numberOfCities, & totalDistance));
			} while (tryAdoption (distance.peek(), path.peek(), numberOfCities, & totalDistance));
			if (totalDistance < shortestDistance) {   /* New shortest path. */
				numberOfShortest = 1;
				for (int i = 0; i <= numberOfCities; i ++) shortestPath [i] = path [i];
				shortestDistance = totalDistance;
			}
			else if (totalDistance == shortestDistance)   /* Shortest path confirmed. */
				numberOfShortest ++;
		}
		if (numberOfIterations > 1)
			Melder_casual ("Polygon_salesperson: "
				"found %ld times the same shortest path.", numberOfShortest);

		/* Change me: I will follow the shortest path found. */

		autoPolygon help = (Polygon) Data_copy (me);
		for (long i = 1; i <= numberOfCities; i ++) {
			my x [i] = help -> x [shortestPath [i]];
			my y [i] = help -> y [shortestPath [i]];
		}
	} catch (MelderError) {
		rethrowm (me, ": shortest path not found.");
	}
}

static void setWindow (Polygon me, Any graphics,
	double xmin, double xmax, double ymin, double ymax)
{
	Melder_assert (me);
	if (xmax == xmin) {   // autoscaling along x axis
		xmax = xmin = my x [1];
		for (long i = 2; i <= my numberOfPoints; i ++) {
			if (my x [i] < xmin)
				xmin = my x [i];
			if (my x [i] > xmax)
				xmax = my x [i];
		}
		if (xmin == xmax) {
			xmin -= 1.0;
			xmax += 1.0;
		}
	}
	if (ymax == ymin) {   // autoscaling along y axis
		ymax = ymin = my y [1];
		for (long i = 2; i <= my numberOfPoints; i ++) {
			if (my y [i] < ymin)
				ymin = my y [i];
			if (my y [i] > ymax)
				ymax = my y [i];
		}
		if (ymin == ymax) {
			ymin -= 1.0;
			ymax += 1.0;
		}
	}
	Graphics_setWindow (graphics, xmin, xmax, ymin, ymax);
}

void Polygon_draw (I, Graphics g, double xmin, double xmax, double ymin, double ymax) {
	iam (Polygon);
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	Graphics_polyline (g, my numberOfPoints, & my x [1], & my y [1]);
	Graphics_unsetInner (g);
}

void Polygon_drawClosed (I, Graphics g, double xmin, double xmax, double ymin, double ymax) {
	iam (Polygon);
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	Graphics_polyline_closed (g, my numberOfPoints, & my x [1], & my y [1]);
	Graphics_unsetInner (g);
}

void Polygon_paint (I, Graphics g, Graphics_Colour colour, double xmin, double xmax, double ymin, double ymax) {
	iam (Polygon);
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	Graphics_setColour (g, colour);
	Graphics_fillArea (g, my numberOfPoints, & my x [1], & my y [1]);
	Graphics_unsetInner (g);
}

void Polygon_drawCircles (I, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double diameter_mm)
{
	iam (Polygon);
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	for (long i = 1; i <= my numberOfPoints; i ++)
		Graphics_circle_mm (g, my x [i], my y [i], diameter_mm);
	Graphics_unsetInner (g);
}

void Polygon_paintCircles (I, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double diameter)
{
	iam (Polygon);
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	for (long i = 1; i <= my numberOfPoints; i ++)
		Graphics_fillCircle_mm (g, my x [i], my y [i], diameter);
	Graphics_unsetInner (g);
}

void Polygons_drawConnection (I, thou, Graphics g,
	double xmin, double xmax, double ymin, double ymax, int hasArrow, double relativeLength)
{
	iam (Polygon); thouart (Polygon);
	double w2 = 0.5 * (1 - relativeLength), w1 = 1 - w2;
	long n = my numberOfPoints;
	if (thy numberOfPoints < n) n = thy numberOfPoints;
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	for (long i = 1; i <= n; i ++) {
		double x1 = my x [i], x2 = thy x [i], y1 = my y [i], y2 = thy y [i];
		double dummy = w1 * x1 + w2 * x2;
		x2 = w1 * x2 + w2 * x1;
		x1 = dummy;
		dummy = w1 * y1 + w2 * y2;
		y2 = w1 * y2 + w2 * y1;
		y1 = dummy;
		if (hasArrow)
			Graphics_arrow (g, x1, y1, x2, y2);
		else
			Graphics_line (g, x1, y1, x2, y2);
	}
	Graphics_unsetInner (g);
}

/* End of file Polygon.cpp */
