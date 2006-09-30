/* Polygon.c
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 */

#include "Polygon.h"

#include "oo_DESTROY.h"
#include "Polygon_def.h"
#include "oo_COPY.h"
#include "Polygon_def.h"
#include "oo_EQUAL.h"
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
	MelderInfo_writeLine2 ("Number of points: ", Melder_integer (my numberOfPoints));
	MelderInfo_writeLine2 ("Perimeter: ", Melder_single (Polygon_perimeter (me)));
}
  
static int writeAscii (I, FILE *f) {
	iam (Polygon);
	long i;
	ascputi4 (my numberOfPoints, f, "numberOfPoints");
	for (i = 1; i <= my numberOfPoints; i ++) {
		ascputr4 (my x [i], f, "x [%ld]", i);
		ascputr4 (my y [i], f, "y [%ld]", i);
	}
	return 1;
}

static int readAscii (I, FILE *f) {
	iam (Polygon);
	long i;
	my numberOfPoints = ascgeti4 (f);
	if (my numberOfPoints < 1)
		return Melder_error ("(Polygon::readAscii:) Cannot read 'number of points' < 1.");
	if (! (my x = NUMfvector (1, my numberOfPoints)) || ! (my y = NUMfvector (1, my numberOfPoints)))
		return 0;
	for (i = 1; i <= my numberOfPoints; i ++) {
		my x [i] = ascgetr4 (f);
		my y [i] = ascgetr4 (f);
	}
	return 1;
}

class_methods (Polygon, Data)
	class_method_local (Polygon, destroy)
	class_method (info)
	class_method_local (Polygon, description)
	class_method_local (Polygon, copy)
	class_method_local (Polygon, equal)
	class_method (writeAscii)
	class_method (readAscii)
	class_method_local (Polygon, writeBinary)
	class_method_local (Polygon, readBinary)
class_methods_end

Polygon Polygon_create (long numberOfPoints) {
	Polygon me = new (Polygon);
	if (! me) return NULL;
	my numberOfPoints = numberOfPoints;
	if (! (my x = NUMfvector (1, numberOfPoints)) || ! (my y = NUMfvector (1, numberOfPoints)))
		{ forget (me); return Melder_errorp ("Polygon not created."); }
	return me;
}

void Polygon_randomize (I) {
	iam (Polygon);
	long i, j;
	float xdum, ydum;
	for (i = 1; i <= my numberOfPoints; i ++) {
		j = NUMrandomInteger (i, my numberOfPoints);
		xdum = my x [i];
		ydum = my y [i];
		my x [i] = my x [j];
		my y [i] = my y [j];
		my x [j] = xdum;
		my y [j] = ydum;
	}
}

double Polygon_perimeter (I) {
	iam (Polygon);
	double dx, dy;
	double result = sqrt (( dx = my x [1] - my x [my numberOfPoints], dx * dx ) +
					( dy = my y [1] - my y [my numberOfPoints], dy * dy ));
	long i;
	for (i = 1; i <= my numberOfPoints - 1; i ++)
		result += sqrt (( dx = my x [i] - my x [i + 1], dx * dx ) + ( dy = my y [i] - my y [i + 1], dy * dy ));
	return result;
}

static void computeDistanceTable (Polygon me, int **table) {
	int i, j;
	for (i = 1; i <= my numberOfPoints - 1; i ++)
		for (j = i + 1; j <= my numberOfPoints; j ++) {
			float dx, dy;
			table [i] [j] = table [j] [i] =
				sqrt (( dx = my x [i] - my x [j], dx * dx ) + ( dy = my y [i] - my y [j], dy * dy ));
					/* Round to zero. */
		}
}

static long computeTotalDistance (int **distance, int path [], int numberOfCities) {
	int i;
	long result = 0;
	for (i = 1; i <= numberOfCities; i ++)
		result += distance [path [i - 1]] [path [i]];
	return result;
}

static void shuffle (int path [], int numberOfCities) {
	int i;
	for (i = 1; i <= numberOfCities; i ++) {
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
	int numberOfCities, i, *path, *shortestPath, **distance;
	long numberOfShortest = 1, totalDistance, shortestDistance, iteration;
	Polygon help;

	numberOfCities = my numberOfPoints;
	if (! (distance = NUMimatrix (1, numberOfCities, 1, numberOfCities))) return;
	computeDistanceTable (me, distance);
	if (! (path = NUMivector (0, numberOfCities))) return;
	for (i = 1; i <= numberOfCities; i ++)
		path [i] = i;
	path [0] = numberOfCities;   /* Close path. */
	if (! (shortestPath = NUMivector_copy (path, 0, numberOfCities))) return;
	for (iteration = 1; iteration <= numberOfIterations; iteration ++)
	{
		if (iteration > 1) shuffle (path, numberOfCities);
		totalDistance = computeTotalDistance (distance, path, numberOfCities);
		if (iteration == 1) shortestDistance = totalDistance;
		do
		{
			do {} while (tryExchange (distance, path, numberOfCities, & totalDistance));
		}
			while (tryAdoption (distance, path, numberOfCities, & totalDistance));
		if (totalDistance < shortestDistance)   /* New shortest path. */
		{
			numberOfShortest = 1;
			for (i = 0; i <= numberOfCities; i ++) shortestPath [i] = path [i];
			shortestDistance = totalDistance;
		}
		else if (totalDistance == shortestDistance)   /* Shortest path confirmed. */
			numberOfShortest ++;
	}
	if (numberOfIterations > 1)
		Melder_casual ("Polygon_salesperson: "
			"found %ld times the same shortest path.", numberOfShortest);

	/* Change me: I will follow the shortest path found. */

	help = Data_copy (me);
	for (i = 1; i <= numberOfCities; i ++)
	{
		my x [i] = help -> x [shortestPath [i]];
		my y [i] = help -> y [shortestPath [i]];
	}
	forget (help);

	NUMimatrix_free (distance, 1, 1);
	NUMivector_free (path, 0);
	NUMivector_free (shortestPath, 0);
}

static void setWindow (Polygon me, Any graphics,
	double xmin, double xmax, double ymin, double ymax)
{
	Melder_assert (me);
	if (xmax == xmin)   /* Autoscaling along x axis. */
	{
		long i;
		xmax = xmin = my x [1];
		for (i = 2; i <= my numberOfPoints; i ++) {
			if (my x [i] < xmin) xmin = my x [i];
			if (my x [i] > xmax) xmax = my x [i];
		}
		if (xmin == xmax) { xmin -= 1.0; xmax += 1.0; }
	}
	if (ymax == ymin)   /* Autoscaling along y axis. */
	{
		long i;
		ymax = ymin = my y [1];
		for (i = 2; i <= my numberOfPoints; i ++)
		{
			if (my y [i] < ymin) ymin = my y [i];
			if (my y [i] > ymax) ymax = my y [i];
		}
		if (ymin == ymax) { ymin -= 1.0; ymax += 1.0; }
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

void Polygon_paint (I, Graphics g, double realColour, double xmin, double xmax, double ymin, double ymax) {
	iam (Polygon);
	int integerColour = floor (realColour);
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	if (integerColour)
		Graphics_setColour (g, integerColour);
	else
		Graphics_setGrey (g, realColour);
	Graphics_fillArea (g, my numberOfPoints, & my x [1], & my y [1]);
	if (! integerColour) Graphics_setGrey (g, 0);
	Graphics_unsetInner (g);
}

void Polygon_drawCircles (I, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double diameter_mm)
{
	iam (Polygon);
	long i;
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	for (i = 1; i <= my numberOfPoints; i ++)
		Graphics_circle_mm (g, my x [i], my y [i], diameter_mm);
	Graphics_unsetInner (g);
}

void Polygon_paintCircles (I, Graphics g,
	double xmin, double xmax, double ymin, double ymax, double diameter)
{
	iam (Polygon);
	long i;
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	for (i = 1; i <= my numberOfPoints; i ++)
		Graphics_fillCircle_mm (g, my x [i], my y [i], diameter);
	Graphics_unsetInner (g);
}

void Polygons_drawConnection (I, thou, Graphics g,
	double xmin, double xmax, double ymin, double ymax, int hasArrow, double relativeLength)
{
	iam (Polygon); thouart (Polygon);
	double w2 = 0.5 * (1 - relativeLength), w1 = 1 - w2;
	long n = my numberOfPoints, i;
	if (thy numberOfPoints < n) n = thy numberOfPoints;
	Graphics_setInner (g);
	setWindow (me, g, xmin, xmax, ymin, ymax);
	for (i = 1; i <= n; i ++) {
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

/* End of file Polygon.c */
