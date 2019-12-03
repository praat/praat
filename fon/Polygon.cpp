/* Polygon.cpp
 *
 * Copyright (C) 1992-2012,2014-2018 Paul Boersma
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

Thing_implement (Polygon, Daata, 1);

void structPolygon :: v_info () {
	our structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of points: ", our numberOfPoints);
	MelderInfo_writeLine (U"Perimeter: ", Melder_single (Polygon_perimeter (this)));
}
  
void structPolygon :: v_writeText (MelderFile file) {
	texputi32 (file, our numberOfPoints, U"numberOfPoints");
	for (integer i = 1; i <= our numberOfPoints; i ++) {
		texputr64 (file, our x [i], U"x [", Melder_integer (i), U"]");
		texputr64 (file, our y [i], U"y [", Melder_integer (i), U"]");
	}
}

void structPolygon :: v_readText (MelderReadText text, int /*formatVersion*/) {
	our numberOfPoints = texgeti32 (text);
	if (our numberOfPoints < 1)
		Melder_throw (U"Cannot read a Polygon with only ", our numberOfPoints, U" points.");
	our x = newVECraw (our numberOfPoints);
	our y = newVECraw (our numberOfPoints);
	for (integer i = 1; i <= our numberOfPoints; i ++) {
		our x [i] = texgetr64 (text);
		our y [i] = texgetr64 (text);
	}
}

autoPolygon Polygon_create (integer numberOfPoints) {
	try {
		autoPolygon me = Thing_new (Polygon);
		my numberOfPoints = numberOfPoints;
		my x = newVECzero (numberOfPoints);
		my y = newVECzero (numberOfPoints);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Polygon not created.");
	}
}

void Polygon_randomize (Polygon me) {
	for (integer i = 1; i <= my numberOfPoints; i ++) {
		integer j = NUMrandomInteger (i, my numberOfPoints);
		double xdum = my x [i];
		double ydum = my y [i];
		my x [i] = my x [j];
		my y [i] = my y [j];
		my x [j] = xdum;
		my y [j] = ydum;
	}
}

double Polygon_perimeter (Polygon me) {
	if (my numberOfPoints < 1) return 0.0;
	double dx = my x [1] - my x [my numberOfPoints], dy = my y [1] - my y [my numberOfPoints];
	double result = sqrt (dx * dx + dy * dy);
	for (integer i = 1; i <= my numberOfPoints - 1; i ++) {
		dx = my x [i] - my x [i + 1];
		dy = my y [i] - my y [i + 1];
		result += sqrt (dx * dx + dy * dy);
	}
	return result;
}

static void computeDistanceTable (Polygon me, INTMAT const& table) {
	for (integer i = 1; i <= my numberOfPoints - 1; i ++)
		for (integer j = i + 1; j <= my numberOfPoints; j ++) {
			double dx = my x [i] - my x [j], dy = my y [i] - my y [j];
			table [i] [j] = table [j] [i] =
				Melder_ifloor (sqrt (dx * dx + dy * dy));   // round to zero
		}
}

static integer computeTotalDistance (constINTMAT const& distance, integer path [], integer numberOfCities) {
	integer result = 0;
	for (integer i = 1; i <= numberOfCities; i ++)
		result += distance [path [i - 1]] [path [i]];
	return result;
}

static void shuffle (integer path [], integer numberOfCities) {
	for (integer i = 1; i <= numberOfCities; i ++) {
		integer j = NUMrandomInteger (i, numberOfCities);
		std::swap (path [i], path [j]);
	}
	path [0] = path [numberOfCities];
}
  
static bool tryExchange (constINTMAT const& distance, integer *path, integer numberOfCities, integer *totalDistance) {
	bool result = false;
	integer b1 = path [0];
	integer b2nr = 1;
	while (b2nr < numberOfCities - 1) {
		integer b2 = path [b2nr];
		integer distance_b1_b2 = distance [b1] [b2];
		integer d2nr = b2nr + 2;
		integer d1 = path [d2nr - 1];
		bool cont = true;
		while (d2nr <= numberOfCities && cont) {
			integer d2 = path [d2nr];
			integer gain = distance_b1_b2 + distance [d1] [d2] - distance [b1] [d1] - distance [b2] [d2];
			if (gain > 0) {
				integer below = b2nr, above = d2nr - 1;
				cont = false;
				do {
					integer help = path [below];
					path [below ++] = path [above];
					path [above --] = help;
				} while (below < above);
				*totalDistance -= gain;
			}
			d1 = d2;
			d2nr ++;
		}
		if (cont) { b1 = b2; b2nr ++; } else result = true;
	}
	return result;
}

static bool tryAdoption (constINTMAT const& distance, integer *path, integer numberOfCities, integer *totalDistance)
{
	integer *help = NUMvector <integer> (0, numberOfCities);
	bool result = false;

	/* Compute maximum distance between two successive cities. */

	integer city1 = path [0], city2 = path [1];
	integer maximumDistance = distance [city1] [city2];
	for (integer i = 2; i <= numberOfCities; i ++) {
		city1 = city2;
		city2 = path [i];
		if (distance [city1] [city2] > maximumDistance)
			maximumDistance = distance [city1] [city2];
	}
	integer maximumGainLeft = maximumDistance;
	for (integer i = 1; i <= numberOfCities; i ++) {
		bool cont = true;
		integer b1, b2, distance_b1_b2, d1nr = 3, cc, e1nrMax = 6;
		integer numberOfCitiesMinus1 = numberOfCities - 1;
		for (integer j = 0; j <= numberOfCitiesMinus1; j ++) path [j] = path [j + 1];
		path [numberOfCities] = path [0];
		b1 = path [0];
		b2 = path [1];
		distance_b1_b2 = distance [b1] [b2];
		cc = path [2];
		while (d1nr < numberOfCitiesMinus1 && cont) {
			integer d1 = path [d1nr];
			integer gain1 = distance_b1_b2 + distance [d1] [cc] - distance [d1] [b2];
			if (gain1 + maximumGainLeft > 0) {
				integer e1nr = d1nr + 1;
				integer dn = path [d1nr];
				if (e1nrMax > numberOfCitiesMinus1) e1nrMax = numberOfCitiesMinus1;
				while (e1nr < e1nrMax && cont) {
					integer e1 = path [e1nr];
					integer gain = gain1 + distance [dn] [e1] - distance [dn] [b1] - distance [cc] [e1];
					if (gain > 0) {
						integer nAdoption = e1nr - d1nr;
						integer dnnr = e1nr - 1;
						cont = false;
						*totalDistance -= gain;
						for (integer j = 0; j <= dnnr - 1; j ++) help [j] = path [j + 1];
						for (integer j = 1; j <= nAdoption; j ++) path [j] = help [dnnr - j];
						for (integer j = 0; j <= d1nr - 2; j ++) path [nAdoption + j + 1] = help [j];
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
	NUMvector_free (help, 0);
	return result;
}

void Polygon_salesperson (Polygon me, integer numberOfIterations) {
	try {
		integer numberOfShortest = 1, totalDistance, shortestDistance = 0;

		integer numberOfCities = my numberOfPoints;
		if (numberOfCities < 1)
			Melder_throw (U"No points.");
		autoINTMAT distance = newINTMATzero (numberOfCities, numberOfCities);
		computeDistanceTable (me, distance.get());
		autoNUMvector <integer> path (0_integer, numberOfCities);
		for (integer i = 1; i <= numberOfCities; i ++)
			path [i] = i;
		path [0] = numberOfCities;   // close path
		autoNUMvector <integer> shortestPath (NUMvector_copy (path.peek(), 0, numberOfCities), 0);
		for (integer iteration = 1; iteration <= numberOfIterations; iteration ++) {
			if (iteration > 1) shuffle (path.peek(), numberOfCities);
			totalDistance = computeTotalDistance (distance.get(), path.peek(), numberOfCities);
			if (iteration == 1) shortestDistance = totalDistance;
			do {
				do {
				} while (tryExchange (distance.get(), path.peek(), numberOfCities, & totalDistance));
			} while (tryAdoption (distance.get(), path.peek(), numberOfCities, & totalDistance));
			if (totalDistance < shortestDistance) {   // new shortest path
				numberOfShortest = 1;
				for (int i = 0; i <= numberOfCities; i ++) shortestPath [i] = path [i];
				shortestDistance = totalDistance;
			}
			else if (totalDistance == shortestDistance)   // shortest path confirmed
				numberOfShortest ++;
		}
		if (numberOfIterations > 1)
			Melder_casual (U"Polygon_salesperson:"
				U" found ", numberOfShortest,
				U" times the same shortest path.");

		/* Change me: I will follow the shortest path found. */

		autoPolygon help = Data_copy (me);
		for (integer i = 1; i <= numberOfCities; i ++) {
			my x [i] = help -> x [shortestPath [i]];
			my y [i] = help -> y [shortestPath [i]];
		}
	} catch (MelderError) {
		Melder_throw (me, U": shortest path not found.");
	}
}

static void setWindow (Polygon me, Graphics graphics,
	double xmin, double xmax, double ymin, double ymax)
{
	Melder_assert (me);
	if (xmax == xmin) {   // autoscaling along x axis
		xmax = xmin = my x [1];
		for (integer i = 2; i <= my numberOfPoints; i ++) {
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
		for (integer i = 2; i <= my numberOfPoints; i ++) {
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

void Polygon_draw (Polygon me, Graphics g, double xmin, double xmax, double ymin, double ymax) {
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	Graphics_polyline (g, my numberOfPoints, & my x [1], & my y [1]);
	Graphics_unsetInner (g);
}

void Polygon_drawClosed (Polygon me, Graphics g, double xmin, double xmax, double ymin, double ymax) {
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	Graphics_polyline_closed (g, my numberOfPoints, & my x [1], & my y [1]);
	Graphics_unsetInner (g);
}

void Polygon_paint (Polygon me, Graphics g, MelderColour colour, double xmin, double xmax, double ymin, double ymax) {
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	Graphics_setColour (g, colour);
	Graphics_fillArea (g, my numberOfPoints, & my x [1], & my y [1]);
	Graphics_unsetInner (g);
}

void Polygon_drawCircles (Polygon me, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double diameter_mm)
{
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	for (integer i = 1; i <= my numberOfPoints; i ++)
		Graphics_circle_mm (g, my x [i], my y [i], diameter_mm);
	Graphics_unsetInner (g);
}

void Polygon_paintCircles (Polygon me, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double diameter)
{
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	for (integer i = 1; i <= my numberOfPoints; i ++)
		Graphics_fillCircle_mm (g, my x [i], my y [i], diameter);
	Graphics_unsetInner (g);
}

void Polygons_drawConnection (Polygon me, Polygon thee, Graphics g,
	double xmin, double xmax, double ymin, double ymax, int hasArrow, double relativeLength)
{
	double w2 = 0.5 * (1 - relativeLength), w1 = 1 - w2;
	integer n = my numberOfPoints;
	if (thy numberOfPoints < n) n = thy numberOfPoints;
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	for (integer i = 1; i <= n; i ++) {
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
