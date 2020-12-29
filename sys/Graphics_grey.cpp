/* Graphics_grey.cpp
 *
 * Copyright (C) 1992-2008,2011,2012,2015-2020 Paul Boersma
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

#include <stdlib.h>
#include "melder.h"
#include "Graphics.h"

//#define MAXGREYSIDE  12
#define MAXGREYSIDE  1000
#define MAXGREYPATH  (2 * MAXGREYSIDE * (MAXGREYSIDE - 1) + 2)
#define MAXGREYEDGECONTOURS  (2 * (MAXGREYSIDE - 1))
#define MAXGREYCLOSEDCONTOURS  (MAXGREYPATH / 4 + 1)
#define MAXGREYEDGEPOINTS  (4 * MAXGREYSIDE)

typedef struct {
	integer numberOfPoints;
	integer beginRow, beginCol, beginOri;
	integer endRow, endCol, endOri;
	integer lowerGrey, upperGrey;
	autoVEC x, y;
} structEdgeContour, *EdgeContour;

static EdgeContour EdgeContour_create (integer numberOfPoints) {
	EdgeContour result = Melder_calloc (structEdgeContour, 1);
	result -> numberOfPoints = numberOfPoints;
	result -> x = zero_VEC (numberOfPoints);
	result -> y = zero_VEC (numberOfPoints);
	return result;   // LEAK
}
static void EdgeContour_delete (EdgeContour e) {
	e -> x. reset();
	Melder_free (e);
}

typedef struct {
	integer numberOfPoints;
	int grey, drawn;
	double xmin, xmax, ymin, ymax;
	autoVEC x, y;
} structClosedContour, *ClosedContour;

static ClosedContour ClosedContour_create (integer numberOfPoints) {
	ClosedContour result = Melder_calloc (structClosedContour, 1);
	result -> numberOfPoints = numberOfPoints;
	result -> x = zero_VEC (numberOfPoints);
	result -> y = zero_VEC (numberOfPoints);
	return result;   // LEAK
}
static void ClosedContour_delete (ClosedContour c) {
	c -> x. reset();
	Melder_free (c);
}

typedef struct {
	integer ori, iContour, start, usedAsEntry, grey;
	double val;
} structEdgePoint, *EdgePoint;

static Graphics theGraphics;
static integer numberOfEdgeContours;
static EdgeContour *edgeContours;
static integer numberOfEdgePoints;
static structEdgePoint *edgePoints;
static integer numberOfClosedContours;
static ClosedContour *closedContours;

static integer numberOfPoints;
static integer row1, row2, col1, col2;
static integer iBorder, numberOfBorders;
static autoBOOLMAT right, below;
static constMATVU data;
static double *border;
static autoVEC x, y;
static double dx, dy, xoff, yoff;

static int empty (integer row, integer col, integer ori)
{
	if (ori == 3) { row ++; ori = 1; }
	if (ori == 2) { col ++; ori = 4; }
	if (ori == 1)
		return (data [row] [col] < border [iBorder]) !=
				 (data [row] [col + 1] < border [iBorder]) &&
				 ! right [row - row1 + 1] [col - col1 + 1];
	else /* ori == 4 */
		return (data [row] [col] < border [iBorder]) !=
				 (data [row + 1] [col] < border [iBorder]) &&
				 ! below [row - row1 + 1] [col - col1 + 1];
}
static integer note (integer row, integer col, integer ori)
{
	++ numberOfPoints;
	Melder_assert (numberOfPoints <= MAXGREYPATH);
	if (ori == 3) { row ++; ori = 1; }
	if (ori == 2) { col ++; ori = 4; }
	if (ori == 1)
	{
		right [row - row1 + 1] [col - col1 + 1] = true;
		x [numberOfPoints] = xoff + (col + (border [iBorder] - data [row] [col]) /
			 (data [row] [col + 1] - data [row] [col])) * dx;
		y [numberOfPoints] = yoff + row * dy;
	}
	else /* ori == 4 */
	{
		below [row - row1 + 1] [col - col1 + 1] = true;
		x [numberOfPoints] = xoff + col * dx;
		y [numberOfPoints] = yoff + (row + (border [iBorder] - data [row] [col]) /
			 (data [row + 1] [col] - data [row] [col])) * dy;
	}
	return 1;
}

static void fillGrey (integer numberOfPoints, constVECVU const& x, constVECVU const& y, int igrey)
/* "igrey" is in between 1 and numberOfBorders + 1. */
{
	Graphics_setGrey (theGraphics, 1.0 - (igrey - 1.0) / numberOfBorders);
	Graphics_fillArea (theGraphics, numberOfPoints, & x [1], & y [1]);
}

static void makeEdgeContour (integer row0, integer col0, integer ori0) {
	numberOfPoints = 0;
	integer row = row0, col = col0, ori = ori0;
	note (row0, col0, ori0);

	bool edge = false;
	do {
		bool clockwise = ! (ori & 1);
		do {   /* Preference for contours perpendicular to x == y. */
			ori = (clockwise ? ori : ori + 2) % 4 + 1;
		} while (! empty (row, col, ori));
		switch (ori) {
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
		note (row, col, ori);
	}
	while (! edge);
	Melder_assert (numberOfEdgeContours < MAXGREYEDGECONTOURS * numberOfBorders);

	EdgeContour e = edgeContours [++ numberOfEdgeContours] = EdgeContour_create (numberOfPoints);
	e -> beginRow = row0;
	e -> beginCol = col0;
	e -> beginOri = ori0;
	e -> endRow = row;
	e -> endCol = col;
	e -> endOri = ori;

	bool up = false;
	switch (ori0) {
		case 1: up = data [row0] [col0 + 1] > data [row0] [col0]; break;
		case 2: up = data [row0 + 1] [col0 + 1] > data [row0] [col0 + 1]; break;
		case 3: up = data [row0 + 1] [col0] > data [row0 + 1] [col0 + 1]; break;
		case 4: up = data [row0] [col0] > data [row0 + 1] [col0]; break;
	}
	if (up) { e -> lowerGrey = iBorder; e -> upperGrey = iBorder + 1; }
	else { e -> lowerGrey = iBorder + 1; e -> upperGrey = iBorder; }
	for (integer iPoint = 1; iPoint <= numberOfPoints; iPoint ++) {
		e -> x [iPoint] = x [iPoint];
		e -> y [iPoint] = y [iPoint];
	}
}

static void makeClosedContour (integer row0, integer col0, integer ori0) {
	double x1, y1;
	ClosedContour c;

	numberOfPoints = 0;
	integer row = row0, col = col0, ori = ori0;
	do {
		bool clockwise = ! (ori & 1);
		do {   /* Preference for contours perpendicular to x == y. */
			ori = (clockwise ? ori : ori + 2) % 4 + 1;
		} while (! empty (row, col, ori));
		switch (ori) {
			case 1: row --; break;
			case 2: col ++; break;
			case 3: row ++; break;
			case 4: col --; break;
		}
		ori = (ori + 1) % 4 + 1;
		note (row, col, ori);
	}
	while (row != row0 || col != col0 || ori != ori0);
	Melder_assert (numberOfClosedContours < MAXGREYCLOSEDCONTOURS * numberOfBorders);
	c = closedContours [++ numberOfClosedContours] = ClosedContour_create (numberOfPoints);

	/* Find a point just inside or outside the contour. */
	/* Find out whether it is above or below the contour. */

	x1 = x [numberOfPoints];
	y1 = y [numberOfPoints];
	bool up = false;
	if (ori == 3) { row ++; ori = 1; }
	else if (ori == 2) { col ++; ori = 4; }
	if (ori == 1) {
		if (x1 > xoff + (col + 0.5) * dx)
			x1 -= 0.01 * dx;
		else
			x1 += 0.01 * dx;
		up = data [row] [col] + ((x1 - xoff) / dx - col) *
			  (data [row] [col + 1] - data [row] [col]) > border [iBorder];
	} else { /* ori == 4 */
		if (y1 > yoff + (row + 0.5) * dy)
			y1 -= 0.01 * dy;
		else
			y1 += 0.01 * dy;
		up = data [row] [col] + ((y1 - yoff) / dy - row) *
			  (data [row + 1] [col] - data [row] [col]) > border [iBorder];
	}

	/* Find out whether the point is inside or outside the contour. */

	if (! NUMrotationsPointInPolygon (x1, y1, numberOfPoints,
			x.asArgumentToFunctionThatExpectsOneBasedArray(), y.asArgumentToFunctionThatExpectsOneBasedArray()))
		up = ! up;

	double xmin = 1e308, xmax = -1e308, ymin = 1e308, ymax = -1e308;
	c -> grey = up ? iBorder + 1 : iBorder;
	for (int i = 1; i <= numberOfPoints; i ++) {
		c -> x [i] = x [i];
		c -> y [i] = y [i];
		if (x [i] < xmin) xmin = x [i];
		if (x [i] > xmax) xmax = x [i];
		if (y [i] < ymin) ymin = y [i];
		if (y [i] > ymax) ymax = y [i];
	}
	c -> xmin = xmin;
	c -> xmax = xmax;
	c -> ymin = ymin;
	c -> ymax = ymax;
}

static void smallGrey () {
	numberOfEdgeContours = 0;
	numberOfClosedContours = 0;
	for (iBorder = 1; iBorder <= numberOfBorders; iBorder ++) {
		for (integer row = 1; row <= MAXGREYSIDE; row ++)
			for (integer col = 1; col <= MAXGREYSIDE; col ++)
				right [row] [col] = below [row] [col] = false;

		/* Find all the edge contours of this border value. */

		for (integer col = col1; col < col2; col ++)
			if (empty (row1, col, 1))
				makeEdgeContour (row1, col, 1);
		for (integer row = row1; row < row2; row ++)
			if (empty (row, col2 - 1, 2))
				makeEdgeContour (row, col2 - 1, 2);
		for (integer col = col2 - 1; col >= col1; col --)
			if (empty (row2 - 1, col, 3))
				makeEdgeContour (row2 - 1, col, 3);
		for (integer row = row2 - 1; row >= row1; row --)
			if (empty (row, col1, 4))
				makeEdgeContour (row, col1, 4);

		/* Find all the closed contours of this border value. */

		for (integer row = row1 + 1; row < row2; row ++)
			for (integer col = col1; col < col2; col ++)
				if (empty (row, col, 1)) makeClosedContour (row, col, 1);
		for (integer col = col1 + 1; col < col2; col ++)
			for (integer row = row1; row < row2; row ++)
				if (empty (row, col, 4)) makeClosedContour (row, col, 4);
	}
	numberOfEdgePoints = 2 * numberOfEdgeContours + 4;
	Melder_assert (numberOfEdgePoints <= MAXGREYEDGEPOINTS * numberOfBorders);

	/* Make a list of all points on the edge. */

		/* The edge points include the four corner points. */

	for (int i = 1; i <= 4; i ++) {
		EdgePoint p = & edgePoints [i];
		p -> ori = i;
		p -> val = 0;
		p -> iContour = 0;
		p -> start = 0;
		p -> usedAsEntry = 0;
		p -> grey = -1;
	}

		/* The edge points include the first points of the edge contours. */

	for (int i = 1; i <= numberOfEdgeContours; i ++) {
		EdgeContour c = edgeContours [i];
		EdgePoint p = & edgePoints [i + i + 3];
		switch (p -> ori = c -> beginOri) {
			case 1: p -> val = c -> x [1] - xoff - col1 * dx; break;
			case 2: p -> val = c -> y [1] - yoff - row1 * dy; break;
			case 3: p -> val = xoff + col2 * dx - c -> x [1]; break;
			case 4: p -> val = yoff + row2 * dy - c -> y [1]; break;
		}
		p -> iContour = i;
		p -> start = 1;
		p -> usedAsEntry = 0;
		p -> grey = c -> lowerGrey;
	}

		/* The edge points include the last points of the edge contours. */

	for (int i = 1; i <= numberOfEdgeContours; i ++) {
		EdgeContour c = edgeContours [i];
		EdgePoint p = & edgePoints [i + i + 4];
		switch (p -> ori = c -> endOri) {
			case 1: p -> val = c -> x [c -> numberOfPoints] - xoff - col1 * dx; break;
			case 2: p -> val = c -> y [c -> numberOfPoints] - yoff - row1 * dy; break;
			case 3: p -> val = xoff + col2 * dx - c -> x [c -> numberOfPoints]; break;
			case 4: p -> val = yoff + row2 * dy - c -> y [c -> numberOfPoints]; break;
		}
		p -> iContour = i;
		p -> start = 0;
		p -> usedAsEntry = 0;
		p -> grey = c -> upperGrey;
	}

	/* Sort the list of edge points with keys Ori and Val. */
	for (int i = 1; i < numberOfEdgePoints; i ++) {
		structEdgePoint p;
		int min = i, j;
		for (j = i + 1; j <= numberOfEdgePoints; j ++)
			if (edgePoints [min]. ori > edgePoints [j]. ori ||
				(edgePoints [min]. ori == edgePoints [j]. ori && edgePoints [min]. val > edgePoints [j]. val))
				min = j;
		p = edgePoints [i];
		edgePoints [i] = edgePoints [min];
		edgePoints [min] = p;
	}

	{
		for (int edge0 = 1; edge0 <= numberOfEdgePoints; edge0 ++)
		if (edgePoints [edge0].grey > -1 && ! edgePoints [edge0].usedAsEntry) {
			int iPoint = 0;
			int edge1 = edge0;
			int darkness;
			do {
				/*
					Follow one edge contour.
				*/
				EdgePoint p = & edgePoints [edge1];
				integer iContour = p -> iContour;
				EdgeContour c = edgeContours [iContour];
				Melder_assert (iContour > 0);
				darkness = p -> grey;
				p -> usedAsEntry = 1;
				if (p -> start) {
					for (int i = 1; i <= c -> numberOfPoints; i ++) {
						Melder_assert (iPoint < MAXGREYPATH);
						x [++ iPoint] = c -> x [i];
						y [iPoint] = c -> y [i];
					}
					for (int i = edge1 + 1; i <= numberOfEdgePoints; i ++)
						if (edgePoints [i].iContour == iContour)
							edge1 = i;
				} else {
					int edge1dummy = edge1;
					for (integer i = c -> numberOfPoints; i >= 1; i --) {
						Melder_assert (iPoint < MAXGREYPATH);
						x [++ iPoint] = c -> x [i];
						y [iPoint] = c -> y [i];
					}
					for (int i = 1; i <= edge1dummy - 1; i ++)
						if (edgePoints [i].iContour == iContour)
							edge1 = i;
				}
				edge1 = edge1 % numberOfEdgePoints + 1;

				/*
					Round some corners.
				*/
				while (edgePoints [edge1].grey == -1) {
					++ iPoint;
					Melder_assert (iPoint <= MAXGREYPATH);
					switch (edgePoints [edge1].ori) {
						case 1: x [iPoint] = xoff + col1 * dx; y [iPoint] = yoff + row1 * dy; break;
						case 2: x [iPoint] = xoff + col2 * dx; y [iPoint] = yoff + row1 * dy; break;
						case 3: x [iPoint] = xoff + col2 * dx; y [iPoint] = yoff + row2 * dy; break;
						case 4: x [iPoint] = xoff + col1 * dx; y [iPoint] = yoff + row2 * dy; break;
					}
					edge1 = edge1 % numberOfEdgePoints + 1;
				}
			}
			while (edge1 != edge0);
			fillGrey (iPoint, x.all(), y.all(), darkness);
		}
	}
	if (numberOfEdgeContours == 0) {
		int i = 1;
		while (i <= numberOfBorders && border [i] < data [row1] [col1])
			i ++;
		x [1] = x [4] = xoff + col1 * dx;
		x [2] = x [3] = xoff + col2 * dx;
		y [1] = y [2] = yoff + row1 * dy;
		y [3] = y [4] = yoff + row2 * dy;
		fillGrey (4, x.all(), y.all(), i);
	}

	/*
		Iterate over all the closed contours.
		Those that are not enclosed by any other contour, are filled first.
	*/
	{
		bool found;
		do {
			found = false;
			for (integer i = 1; i <= numberOfClosedContours; i ++) {
				ClosedContour ci = closedContours [i];
				if (! ci -> drawn) {
					bool enclosed = false;
					integer j = 1;
					while (j <= numberOfClosedContours && ! enclosed) {
						ClosedContour cj = closedContours [j];
						if (! cj -> drawn && j != i &&
							 ci -> xmin > cj -> xmin && ci -> xmax < cj -> xmax && 
							 ci -> ymin > cj -> ymin && ci -> ymax < cj -> ymax)
							enclosed = NUMrotationsPointInPolygon (ci -> x [1], ci -> y [1],
									cj -> numberOfPoints,
									cj -> x.asArgumentToFunctionThatExpectsOneBasedArray(),
									cj -> y.asArgumentToFunctionThatExpectsOneBasedArray());
						j ++;
					}
					if (! enclosed) {
						found = true;
						fillGrey (ci -> numberOfPoints, ci -> x.all(), ci -> y.all(), ci -> grey);
						ci -> drawn = 1;
					}
				}
			}
		} while (found);
	}
	Graphics_setGrey (theGraphics, 0.0);
	for (int i = 1; i <= numberOfEdgeContours; i ++)
		EdgeContour_delete (edgeContours [i]);
	for (int i = 1; i <= numberOfClosedContours; i ++)
		ClosedContour_delete (closedContours [i]);
}

void Graphics_grey (Graphics me, constMATVU const& z,
	double x1WC, double x2WC, double y1WC, double y2WC,
	int _numberOfBorders, double borders [])
{
	if (z.nrow <= 1 || z.ncol <= 1)
		return;

	/* Static variables. */

	theGraphics = me;
	numberOfBorders = _numberOfBorders;
	data = z;
	border = borders;
	dx = (x2WC - x1WC) / (z.ncol - 1);
	dy = (y2WC - y1WC) / (z.nrow - 1);
	xoff = x1WC - dx;
	yoff = y1WC - dy;
	if (NUMisEmpty (right.get())) {
		right = zero_BOOLMAT (MAXGREYSIDE, MAXGREYSIDE);   // BUG memory
		below = zero_BOOLMAT (MAXGREYSIDE, MAXGREYSIDE);
		x = zero_VEC (MAXGREYPATH);
		y = zero_VEC (MAXGREYPATH);
		edgeContours = Melder_calloc (EdgeContour, MAXGREYEDGECONTOURS * numberOfBorders) - 1;
		closedContours = Melder_calloc (ClosedContour, MAXGREYCLOSEDCONTOURS * numberOfBorders) - 1;
		edgePoints = Melder_calloc (structEdgePoint, MAXGREYEDGEPOINTS * numberOfBorders);
	}

	/* The matrix is subdivided into matrices with side MAXGREYSIDE, so that:
	 * 1. All the paths will fit into our memory (we have to remember them all).
	 * 2. The path for filling fits into the PostScript path, which may be max. 1500 points long.
	 */
	for (row1 = 1; row1 < z.nrow; row1 += MAXGREYSIDE - 1) {
		row2 = row1 + (MAXGREYSIDE - 1);
		if (row2 > z.nrow) row2 = z.nrow;
		for (col1 = 1; col1 < z.ncol; col1 += MAXGREYSIDE - 1) {
			col2 = col1 + (MAXGREYSIDE - 1);
			if (col2 > z.ncol) col2 = z.ncol;
			smallGrey ();
		}
	}
}

/* End of file Graphics_grey.cpp */
