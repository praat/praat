/* Graphics_altitude.cpp
 *
 * Copyright (C) 1992-2005,2008,2011,2015-2020 Paul Boersma
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

#include "Graphics.h"

static int empty (constMATVU const& data, constBOOLMAT const& right, constBOOLMAT const& below, integer row1, integer col1, double height, integer row, integer col, integer ori) {
	if (ori == 3) {
		row ++;
		ori = 1;
	} else if (ori == 2) {
		col ++;
		ori = 4;
	}
	if (ori == 1)
		return (data [row] [col] < height) != (data [row] [col + 1] < height) &&
			! right [row - row1 + 1] [col - col1 + 1];
	else /* ori == 4 */
		return (data [row] [col] < height) != (data [row + 1] [col] < height) &&
			! below [row - row1 + 1] [col - col1 + 1];
}

#define MAXALTSIDE  50
#define MAXALTPATH  (2 * MAXALTSIDE * (MAXALTSIDE - 1) + 2)

static integer numberOfPoints;
static integer row1, row2, col1, col2;
static autoBOOLMAT right, below;
static autoVEC x, y;
static bool closed;
static double dx, dy, xoff, yoff;

static int note (constMATVU const& z, double height, integer row, integer col, int ori, bool pleaseForget) {
	++ numberOfPoints;
	Melder_assert (numberOfPoints <= MAXALTPATH);
	if (ori == 3) {
		row ++;
		ori = 1;
	} else if (ori == 2) {
		col ++;
		ori = 4;
	}
	if (ori == 1) {
		if (pleaseForget)
			right [row - row1 + 1] [col - col1 + 1] = true;
		x [numberOfPoints] = xoff +
			(col + (height - z [row] [col]) / (z [row] [col + 1] - z [row] [col])) * dx;
		y [numberOfPoints] = yoff + row * dy;
	} else {   /* ori == 4 */
		if (pleaseForget)
			below [row - row1 + 1] [col - col1 + 1] = true;
		x [numberOfPoints] = xoff + col * dx;
		y [numberOfPoints] = yoff +
			(row + (height - z [row] [col]) / (z [row + 1] [col] - z [row] [col])) * dy;
	}
	return 1;
}

static void makeContour (Graphics graphics, constMATVU const& z, double height, integer row0, integer col0, int ori0) {
	integer row = row0, col = col0;
	bool edge = false;
	int ori = ori0;
	numberOfPoints = 0;
	note (z, height, row0, col0, ori0, false);
	do {
		bool const clockwise = ! (ori & 1);
		do {   /* Preference for contours perpendicular to x == y. */
			ori = ( clockwise ? ori : ori + 2 ) % 4 + 1;
		} while (! empty (z, right.get(), below.get(), row1, col1, height, row, col, ori));
		if (! closed) switch (ori) {
			case 1: edge = row == row1; break;
			case 2: edge = col == col2 - 1; break;
			case 3: edge = row == row2 - 1; break;
			case 4: edge = col == col1; break;
		}
		if (! edge) {
			switch (ori) {
				case 1: row --; break;
				case 2: col ++; break;
				case 3: row ++; break;
				case 4: col --; break;
			}
			ori = (ori + 1) % 4 + 1;
		}
		if (! note (z, height, row, col, ori, 1))
			return;
	}
	while (edge == 0 && (row != row0 || col != col0 || ori != ori0))
		;
	if (closed)
		note (z, height, row0, col0, ori0, true);   // close the contour
	Graphics_polyline (graphics, numberOfPoints, & x [1], & y [1]);
}

static void smallAlt (Graphics graphics, constMATVU const& z, double height) {
	for (integer row = 1; row <= MAXALTSIDE; row ++)
		for (integer col = 1; col <= MAXALTSIDE; col ++)
			right [row] [col] = below [row] [col] = false;

	/* Find all the edge contours of this border value. */

	closed = false;
	for (integer col = col1; col < col2; col ++)
		if (empty (z, right.get(), below.get(), row1, col1, height, row1, col, 1))
			makeContour (graphics, z, height, row1, col, 1);
	for (integer row = row1; row < row2; row ++)
		if (empty (z, right.get(), below.get(), row1, col1, height, row, col2 - 1, 2))
			makeContour (graphics, z, height, row, col2 - 1, 2);
	for (integer col = col2 - 1; col >= col1; col --)
		if (empty (z, right.get(), below.get(), row1, col1, height, row2 - 1, col, 3))
			makeContour (graphics, z, height, row2 - 1, col, 3);
	for (integer row = row2 - 1; row >= row1; row --)
		if (empty (z, right.get(), below.get(), row1, col1, height, row, col1, 4))
			makeContour (graphics, z, height, row, col1, 4);

	/* Find all the closed contours of this border value. */

	closed = true;
	for (integer row = row1 + 1; row < row2; row ++)
		for (integer col = col1; col < col2; col ++)
			if (empty (z, right.get(), below.get(), row1, col1, height, row, col, 1))
				makeContour (graphics, z, height, row, col, 1);
	for (integer col = col1 + 1; col < col2; col ++)
		for (integer row = row1; row < row2; row ++)
			if (empty (z, right.get(), below.get(), row1, col1, height, row, col, 4))
				makeContour (graphics, z, height, row, col, 4);
}

void Graphics_contour (Graphics me, constMATVU const& z,
	double x1WC, double x2WC, double y1WC, double y2WC,
	double height)
{
	if (z.nrow <= 1 || z.ncol <= 1)
		return;
	dx = (x2WC - x1WC) / (z.ncol - 1);
	dy = (y2WC - y1WC) / (z.nrow - 1);
	xoff = x1WC - dx;
	yoff = y1WC - dy;
	if (NUMisEmpty (right.get())) {   // static!
		right = zero_BOOLMAT (MAXALTSIDE, MAXALTSIDE);
		below = zero_BOOLMAT (MAXALTSIDE, MAXALTSIDE);
		x = zero_VEC (MAXALTPATH);
		y = zero_VEC (MAXALTPATH);
	}
	for (row1 = 1; row1 < z.nrow; row1 += MAXALTSIDE - 1) {
		for (col1 = 1; col1 < z.ncol; col1 += MAXALTSIDE - 1) {
			if ((row2 = row1 + (MAXALTSIDE - 1)) > z.nrow)
				row2 = z.nrow;
			if ((col2 = col1 + (MAXALTSIDE - 1)) > z.ncol)
				col2 = z.ncol;
			smallAlt (me, z, height);
		}
	}
}

void Graphics_altitude (Graphics me, constMATVU const& z,
	double x1WC, double x2WC, double y1WC, double y2WC,
	int numberOfBorders, double borders [])
{
	if (z.nrow <= 1 || z.ncol <= 1)
		return;
	dx = (x2WC - x1WC) / (z.ncol - 1);
	dy = (y2WC - y1WC) / (z.nrow - 1);
	xoff = x1WC - dx;
	yoff = y1WC - dy;
	if (NUMisEmpty (right.get())) {   // static!
		right = zero_BOOLMAT (MAXALTSIDE, MAXALTSIDE);
		below = zero_BOOLMAT (MAXALTSIDE, MAXALTSIDE);
		x = zero_VEC (MAXALTPATH);
		y = zero_VEC (MAXALTPATH);
	}
	for (row1 = 1; row1 < z.nrow; row1 += MAXALTSIDE - 1) {
		for (col1 = 1; col1 < z.ncol; col1 += MAXALTSIDE - 1) {
			if ((row2 = row1 + (MAXALTSIDE - 1)) > z.nrow)
				row2 = z.nrow;
			if ((col2 = col1 + (MAXALTSIDE - 1)) > z.ncol)
				col2 = z.ncol;
			for (int iborder = 1; iborder <= numberOfBorders; iborder ++)
				smallAlt (me, z, borders [iborder]);
		}
	}
}

/* End of file Graphics_altitude.cpp */
