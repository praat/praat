/* DTW.cpp
 *
 * Copyright (C) 1993-2013 David Weenink
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
 djmw 20020813 GPL header
 djmw 20050302 Path finder modified with sakoe-chiba band
 djmw 20050305 DTW_to_Polygon
 djmw 20050306 DTW_swapAxes
 djmw 20050530 Added Matrices_to_DTW
 djmw 20060909 DTW_getPathY linear behaviour outside domains.
 djmw 20061205 Pitches_to_DTW
 djmw 20061214 Changed info to Melder_writeLine<x> format.
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20071016 To Melder_error<n>
 djmw 20071022 Extra comments + possible bug correction in DTW_Path_recode.
 djmw 20071201 Melder_warning<n>.
 djmw 20071204 DTW_and_Sounds_draw.
 djmw 20080122 float -> double
 djmw 20081123 DTW_and_Sounds_checkDomains did not swap sounds correctly.
 djmw 20091009 Removed a bug in DTW_Path_recode that could cause two identical x and y times in succesion at the end.
 djmw 20100504 extra check in DTW_Path_makeIndex
 djmw 20110304 Thing_new
*/

#include "DTW.h"
#include "Sound_extensions.h"
#include "NUM2.h"
#include "NUMmachar.h"

#include "oo_DESTROY.h"
#include "DTW_def.h"
#include "oo_COPY.h"
#include "DTW_def.h"
#include "oo_EQUAL.h"
#include "DTW_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "DTW_def.h"
#include "oo_WRITE_TEXT.h"
#include "DTW_def.h"
#include "oo_WRITE_BINARY.h"
#include "DTW_def.h"
#include "oo_READ_TEXT.h"
#include "DTW_def.h"
#include "oo_READ_BINARY.h"
#include "DTW_def.h"
#include "oo_DESCRIPTION.h"
#include "DTW_def.h"

Thing_implement (DTW, Matrix, 2);

#define DTW_BIG 1e38

void structDTW :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Domain prototype:", Melder_double (ymin), L" to ",
	                       Melder_double (ymax), L" (s).");
	MelderInfo_writeLine (L"Domain candidate:", Melder_double (xmin), L" to ",
	                       Melder_double (xmax), L" (s).");
	MelderInfo_writeLine (L"Number of frames prototype: ", Melder_integer (ny));
	MelderInfo_writeLine (L"Number of frames candidate: ", Melder_integer (nx));
	MelderInfo_writeLine (L"Path length (frames): ", Melder_integer (pathLength));
	MelderInfo_writeLine (L"Global warped distance: ", Melder_double (weightedDistance));
	if (nx == ny) {
		double dd = 0;
		for (long i = 1; i <= nx; i++) {
			dd += z[i][i];
		}
		MelderInfo_writeLine (L"Distance along diagonal: ", Melder_double (dd / nx));
	}
}

static void DTW_drawWarpX_raw (DTW me, Graphics g, double xmin, double xmax, double ymin, double ymax, double tx, int garnish, int inset);
static void DTW_paintDistances_raw (DTW me, Graphics g, double xmin, double xmax, double ymin,
                                    double ymax, double minimum, double maximum, int garnish, int inset);
static void DTW_drawPath_raw (DTW me, Graphics g, double xmin, double xmax, double ymin,
                              double ymax, int garnish, int inset);
static double _DTW_and_Sounds_getPartY (Graphics g, double dtw_part_x);
static void DTW_findPath_special (DTW me, int matchStart, int matchEnd, int slope, Matrix *cummulativeDists);
/*
	Two 'slope lines, lh and ll, start in the lower left corner, the upper/lower has the maximum/minimum allowed slope.
	Two other lines, ru and rl, end in the upper-right corner. The upper/lower line have minimum/maximum slope.
	1. the four lines with the two intersections determine a diamond
		For vertical line at x we return the upper and lower y of the two intersections
	1. When we don't have a diamond, we return an error and the vertical distance between the slopelines in ylow.
*/


/*
   The actual path will be interpolated as follows:
   The distance matrix has cells of dimensions dx by dy.
   The optimal path connects these cells with one another in the following ways:

   In a diagonal ''path'' segment, i.e. when cells have no side in common,
   the interplated path runs from the lowerleft corner to the upperright corner.
   If a path segment is horizontal or vertical the path also runs from lowerleft to upperright.
   It is only when a horizontal and a vertical segment have a cell in common that we have to make
   some adjustments to the path in the common cell.
   This will be done as follows:
   Let nx and ny be the number of horizontal and vertical cells in this path segment.
   The path will always start at the lowerleft of the leftmost block and end
   at the upperright of the rightmost block.
   In the common cell we have to introduce an extra point in the path at (x1,y1)
   where the line from the horizontal segment and the line from the vertical segment intersect.
   Let the lowerleft and upperright point of the common cell be (0,0) and (dx,dy),
   then the coordinates (x,y) of the intersection can be calculated as follows:
   If we have two lines y = a1*x+b1 and y = a2*x+b2 they intersect at
      (x,y) = (b2-b1, a1*b2-a2*b1)/(a1-a2)
	For horizontal-vertical (hv) cells:
		a1 = dy/(nx*dx),   a2 = ny*dy/dx
		b1 = dy*(nx-1)/nx, b2 = 0
	Then:
		(x,y) = (dx, dy*ny ) * (nx-1)/(nx*ny-1)

	For vertical-horizontal (vh) cells:
		a1 = ny*dy/dx,   a2 = dy/(nx*dx)
		b1 = -(ny-1)*dy   b2 = 0
	Then:
		(x,y) = (nx*dx, dy) * (ny-1)/(nx*ny-1)
*/

/* DTW_getXTime (DTW me, (DTW_getYTime (DTW me, double tx)) == tx */
double DTW_getYTimeFromXTime (DTW me, double tx) {
	DTW_Path_Query thee = & my pathQuery;

	if (tx < my xmin) {
		return my ymin - (my xmin - tx);
	}
	if (tx > my xmax) {
		return my ymax + (tx - my xmax);
	}

	if (! NUMfpp) {
		NUMmachar ();
	}
	double eps = 3 * NUMfpp -> eps;

	/* Find in which column is tx */

	long ib, ie;
	long ix = (long) floor ( (tx - my x1) / my dx + 1.5);
	if (ix < 1) {
		ib = 1; ie = 2;
	} else if (ix > my nx) {
		ib = thy nxy - 1; ie = thy nxy;
	} else {
		ib = thy xindex[ix].ibegin; ie = thy xindex[ix].iend;
	}
	if (ie - ib > 1) {
		long it = ib + 1;
		while (tx - thy xytimes[it].x > eps) {
			it++;
		}
		ie = it; ib = it - 1;
	}
	double a = (thy xytimes[ib].y - thy xytimes[ie].y) / (thy xytimes[ib].x - thy xytimes[ie].x);
	double b = thy xytimes[ib].y - a * thy xytimes[ib].x;
	return a * tx + b;
}

double DTW_getXTimeFromYTime (DTW me, double ty) {
	DTW_Path_Query thee = & my pathQuery;

	if (ty < my ymin) {
		return my ymin - (my ymin - ty);
	}
	if (ty > my ymax) {
		return my ymax + (ty - my ymax);
	}

	if (! NUMfpp) {
		NUMmachar ();
	}
	double eps = 3 * NUMfpp -> eps;

	/* Find in which row is ty */

	long ib, ie;
	long iy = (long) floor ( (ty - my y1) / my dy + 1.5);
	if (iy < 1) {
		ib = 1; ie = 2;
	} else if (iy > my ny) {
		ib = thy nxy - 1; ie = thy nxy;
	} else {
		ib = thy yindex[iy].ibegin; ie = thy yindex[iy].iend;
	}
	if (ie - ib > 1) {
		long it = ib + 1;
		while (ty - thy xytimes[it].y > eps) {
			it++;
		}
		ie = it; ib = it - 1;
	}
	double a = (thy xytimes[ib].y - thy xytimes[ie].y) / (thy xytimes[ib].x - thy xytimes[ie].x);
	double b = thy xytimes[ib].y - a * thy xytimes[ib].x;
	return (ty - b) / a;
}

void DTW_Path_Query_init (DTW_Path_Query me, long ny, long nx) {
	Melder_assert (ny > 0 && nx > 0);
	my ny = ny;
	my nx = nx;
	my nxy = 2 * (ny > nx ? ny : nx) + 2; // maximum number of points
	my xytimes = NUMvector<structDTW_Path_xytime> (1, my nxy);
	my yindex = NUMvector<structDTW_Path_Index> (1, my ny);
	my xindex = NUMvector<structDTW_Path_Index> (1, my nx);
}

static void DTW_Path_makeIndex (DTW me, int xory) {
	DTW_Path_Query thee = & my pathQuery;
	DTW_Path_Index index;
	double x1, dx;
	long nx;

	if (! NUMfpp) {
		NUMmachar ();
	}
	double eps = 3 * NUMfpp -> eps;

	if (xory == DTW_X) {
		index = thy xindex;
		nx = my nx;
		x1 = my x1;
		dx = my dx;
	} else {
		index = thy yindex;
		nx = my ny;
		x1 = my y1;
		dx = my dy;
	}
	double xy_x2 = xory == DTW_X ? thy xytimes[3].x : thy xytimes[3].y;
	long i = 3;
	for (long j = 1; j <= nx; j++) {
		double xlow = x1 + (j - 1 - 0.5) * dx;
		double xhigh = xlow + dx;

		if (xlow - xy_x2 > -eps && i < thy nxy) { // i.e. xlow >= xy_x2
			i++;
			xy_x2 = xory == DTW_X ? thy xytimes[i].x : thy xytimes[i].y;
		}

		index[j].ibegin = i - 1;

		while (xhigh - xy_x2 > eps && i < thy nxy) { // i.e. xhigh > xy_x2
			i++;
			xy_x2 = xory == DTW_X ? thy xytimes[i].x : thy xytimes[i].y;
		}

		index[j].iend = i;
	}
}

/* Recode the path from a chain of cells to a piecewise linear path. */
void DTW_Path_recode (DTW me) {
	DTW_Path_Query thee = & my pathQuery;
	long nxy;		// current number of elements in recoded path
	long nx = 1;	// current number of successive horizontal cells in the cells chain
	long ny = 1;	// current number of successive vertical cells in the cells chain
	long nd = 0;	// current number of successive diagonal cells in the cells chain
	int isv = 0;	// previous segment in the original path was vertical
	int ish = 0;	// previous segment in the original path was horizontal
	long ixp = 0, iyp = 0; // previous cell

	/* Algorithm
		1: get all the points in the path
		2. get the x and y indices
	*/

	/* 1. Starting point always at origin */
	thy xytimes[1].x = my xmin;
	thy xytimes[1].y = my ymin;
	nx = my path[1].x;
	ixp = nx - 1;
	thy xytimes[2].x = my x1 + (nx - 1 - 0.5) * my dx;
	ny = my path[1].y;
	iyp = ny - 1;
	thy xytimes[2].y = my y1 + (ny - 1 - 0.5) * my dy;
	// implicit: my x1 - 0.5 * my dx > my xmin && my y1 - 0.5 * my dy > my ymin
	nxy = 2;
	for (long j = 1; j <= my pathLength; j++) {
		long index; // where are we in the new path?
		long ix = my path[j].x, iy = my path[j].y;
		double xright = my x1 + (ix - 1 + 0.5) * my dx;
		double x, y, f, ytop = my y1 + (iy - 1 + 0.5) * my dy;

		if (iy == iyp) { // horizontal path?
			ish = 1;
			if (isv) { // we came from vertical direction?
				// We came from a vertical direction so this is the second horizontal cell in a row.
				// The statement after this "if" updates nx to 2.
				nx = 1; isv = 0;
			}
			nx++;

			if (ny > 1 || nd > 1) {
				// Previous segment was diagonal or vertical: modify intersection
				// The vh intersection (x,y) = (nx*dx, dy) * (ny-1)/(nx*ny-1)
				// A diagonal segment has ny = 1.
				f = (ny - 1.0) / (nx * ny - 1);
				x = xright - nx * my dx + nx * my dx * f;
				y = ytop - my dy + my dy * f;
				index = nxy - 1;
				if (nx == 2) {
					index = nxy;
					nxy++;
				}
				thy xytimes[index].x = x;
				thy xytimes[index].y = y;
			}
			nd = 0;
		} else if (ix == ixp) { // vertical
			isv = 1;
			if (ish) {
				ny = 1; ish = 0;
			}
			ny++;

			if (nx > 1 || nd > 1) {
				// The hv intersection (x,y) = (dx, dy*ny ) * (nx-1)/(nx*ny-1)
				f = (nx - 1.0) / (nx * ny - 1);
				x = xright - my dx + my dx * f;
				y = ytop - ny * my dy + ny * my dy * f;
				index = nxy - 1;
				if (ny == 2) {
					index = nxy;
					nxy++;
				}
				thy xytimes[index].x = x;
				thy xytimes[index].y = y;
			}
			nd = 0;
		} else if (ix == (ixp + 1) && iy == (iyp + 1)) { // diagonal
			nd++;
			if (nd == 1) {
				nxy++;
			}
			nx = ny = 1;
		} else {
			Melder_throw ("The path goes back in time.");
		}
		// update
		thy xytimes[nxy].x = xright;
		thy xytimes[nxy].y = ytop;
		ixp = ix; iyp = iy;
	}

	if (my xmax > thy xytimes[nxy].x || my ymax > thy xytimes[nxy].y) {
		nxy++;
		thy xytimes[nxy].x = my xmax;
		thy xytimes[nxy].y = my ymax;
	}
	Melder_assert (nxy <= 2 * (my ny > my nx ? my ny : my nx) + 2);
	thy nxy = nxy;

	DTW_Path_makeIndex (me, DTW_X);
	DTW_Path_makeIndex (me, DTW_Y);
}

void DTW_pathRemoveRedundantNodes (DTW me) {
	long i = 1, skip = 0;

	for (long j = 2; j <= my pathLength; j++) {
		if ( (my path[j].y == my path[i].y) || my path[j].x == my path[i].x) {
			skip++;
		} else {
			/* if (j-1)^th was the last of a series: store it */
			if (skip > 0) {
				my path[++i] = my path[j - 1];
			}
			/* same check again */
			skip = 0;
			if ( (my path[j].y == my path[i].y) || my path[j].x == my path[i].x) {
				skip++;
			} else {
				my path[++i] = my path[j];
			}
		}
	}
	if (skip > 0) {
		my path[++i] = my path[my pathLength];
	}
	my pathLength = i;
}

/* Prototype must be on y-axis and test on x-axis */

DTW DTW_create (double tminp, double tmaxp, long ntp, double dtp, double t1p,
                double tminc, double tmaxc, long ntc, double dtc, double t1c) {
	try {
		autoDTW me = Thing_new (DTW);
		Matrix_init (me.peek(), tminc, tmaxc, ntc, dtc, t1c, tminp, tmaxp, ntp, dtp, t1p);
		my path = NUMvector<structDTW_Path> (1, ntc + ntp - 1);
		DTW_Path_Query_init (& my pathQuery, ntp, ntc);
		my wx = 1; my wy = 1; my wd = 2;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("DTW not created.");
	}
}

void DTW_setWeights (DTW me, double wx, double wy, double wd) {
	my wx = wx; my wy = wy; my wd = wd;
}

DTW DTW_swapAxes (DTW me) {
	try {
		autoDTW thee = DTW_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);

		for (long x = 1; x <= my nx; x++) {
			for (long y = 1; y <= my ny; y++) {
				thy z[x][y] = my z[y][x];
			}
		}
		thy pathLength = my pathLength;
		for (long i = 1; i <= my pathLength; i++) {
			thy path[i].x = my path[i].y;
			thy path[i].y = my path[i].x;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": axes not swapped.");
	}
}

double DTW_getPathY (DTW me, double tx) {
	// Assume linear behaviour outside domains
	//  Other option: scale with x_domain/y_domain

	if (tx < my xmin) {
		return my ymin - (my xmin - tx);
	}
	if (tx > my xmax) {
		return my ymax + (tx - my xmax);
	}

	// Find column in DTW matrix

	long ix = (tx - my x1) / my dx + 1;
	if (ix < 1) {
		ix = 1;
	}
	if (ix > my nx) {
		ix = my nx;
	}

	// Find index in the path and the row number (iy)

	long i = ix + my path[1].x - 1;
	while (i <= my pathLength && my path[i].x != ix) {
		i++;
	}
	if (i > my pathLength) {
		return NUMundefined;
	}
	long iy = my path[i].y; /* row */

	/*
		We like to have a "continuous" interpretation of time for the quantized x and y times.
		A time block is specified by lower left (x,y) and upper right (x+dx,y+dy).
		The path is interpreted as piecewise linear.
		Two special cases:
		1. the local path is horizontal, i.e. two or more path elements with the same path.y value.
			We calculate the y-time from the line that connects the lower-left position of the leftmost
			horizontal time block to the upper-right position of the rightmost time horizontal block.
			(lowest column number to highest column number)
			For the first column and the last column we need to do something special if
		2. the local path is vertical, i.e. two or path elements with the same path.x value.
			We calculate the y-time from the line that connects the lower-left position of the bottommost
			vertical time block to the upper-right position of the topmost time horizontal block.
			(lowest row number to highest row number)
	*/

	// Horizontal path? Find left and right positions.

	long ileft = i - 1;
	while (ileft >= 1 && my path[ileft].y == iy) {
		ileft--;
	}
	ileft++;
	if (ileft == 1 && ix > 1 && my path[ileft].y > 1) {
		ileft++;
	}

	long iright = i + 1;
	while (iright <= my pathLength && my path[iright].y == iy) {
		iright++;
	}
	iright--;
	if (iright == my pathLength && ix < my nx && my path[iright].y < my ny) {
		iright--;
	}

	long nxx = iright - ileft + 1;

	// Vertical path? Only if not already horizontal.

	long ibottom = i;
	long itop = i;

	if (nxx == 1) {
		ibottom--;
		while (ibottom >= 1 && my path[ibottom].x == ix) {
			ibottom--;
		}
		ibottom++;

		itop++;
		while (itop <= 1 && my path[itop].x == ix) {
			itop--;
		}
		itop++;
	}

	long nyy = itop - ibottom + 1;
	double boxx = nxx * my dx;
	double boxy = nyy * my dy;
	double ty;

	// Corrections at extreme left and right if path[1].x=1 && path[1].y>1

	if (ix == my nx) {
		boxx = my xmax - (my x1 + (ix - 1) * my dx - my dx / 2);
		boxy = my ymax - (my y1 + (iy - 1) * my dy - my dy / 2);
		ty = my ymax - (boxy - (boxx - (my xmax - tx)) * boxy / boxx);
	} else if (ix == 1) {
		boxx = my x1 + my dx / 2 - my xmin;
		boxy = my y1 + (itop - 1) * my dy + my dy / 2 - my ymin;
		ty = (tx - my xmin) * boxy / boxx + my ymin;
	} else {
		// Diagonal interpolation in a box with lower left (0,0) and upper right (nxx*dx, nyy*dy).
		double ty0 = (tx - (my x1 + (my path[ileft].x - 1) * my dx - my dx / 2)) * boxy / boxx;
		ty =  my y1 + (my path[ibottom].y - 1) * my dy - my dy / 2 + ty0;
	}
	return ty;
}

long DTW_getMaximumConsecutiveSteps (DTW me, int direction) {
	long nglobal = 1, nlocal = 1;

	for (long i = 2; i <= my pathLength; i++) {
		int localdirection;

		if (my path[i].y == my path[i - 1].y) {
			localdirection = DTW_X;
		} else if (my path[i].x == my path[i - 1].x) {
			localdirection = DTW_Y;
		} else {
			localdirection = DTW_XANDY;
		}

		if (localdirection == direction) {
			nlocal += 1;
		}
		if (direction != localdirection || i == my pathLength) {
			if (nlocal > nglobal) {
				nglobal = nlocal;
			}
			nlocal = 1;
		}
	}
	return nglobal;
}

static void DTW_paintDistances_raw (DTW me, Graphics g, double xmin, double xmax, double ymin,
                                    double ymax, double minimum, double maximum, int garnish, int inset) {
	long ixmin, ixmax, iymin, iymax;
	if (xmax <= xmin) {
		xmin = my xmin;
		xmax = my xmax;
	}
	if (ymax <= ymin) {
		ymin = my ymin;
		ymax = my ymax;
	}
	(void) Matrix_getWindowSamplesX (me, xmin - 0.49999 * my dx, xmax + 0.49999 * my dx,
	                                 & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin - 0.49999 * my dy, ymax + 0.49999 * my dy,
	                                 & iymin, & iymax);
	if (maximum <= minimum) {
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);
	}
	if (maximum <= minimum) {
		minimum -= 1.0;
		maximum += 1.0;
	}
	if (xmin >= xmax || ymin >= ymax) {
		return;
	}
	if (inset) {
		Graphics_setInner (g);
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_cellArray (g, my z,
	                    ixmin, ixmax, Matrix_columnToX (me, ixmin - 0.5), Matrix_columnToX (me, ixmax + 0.5),
	                    iymin, iymax, Matrix_rowToY (me, iymin - 0.5), Matrix_rowToY (me, iymax + 0.5),
	                    minimum, maximum);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	if (inset) {
		Graphics_unsetInner (g);
	}
	if (garnish) {
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

void DTW_paintDistances (DTW me, Graphics g, double xmin, double xmax, double ymin,
                         double ymax, double minimum, double maximum, int garnish) {
	DTW_paintDistances_raw (me, g, xmin, xmax, ymin, ymax, minimum, maximum, garnish, 1);
}

static void DTW_drawPath_raw (DTW me, Graphics g, double xmin, double xmax, double ymin,
                              double ymax, int garnish, int inset) {
	DTW_Path_Query thee = & my pathQuery;

	if (xmin >= xmax) {
		xmin = my xmin; xmax = my xmax;
	}
	if (ymin >= ymax) {
		ymin = my ymin; ymax = my ymax;
	}

	if (inset) {
		Graphics_setInner (g);
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	for (long i = 1; i < thy nxy; i++) {
		double x1, y1, x2, y2;
		if (NUMclipLineWithinRectangle (thy xytimes[i].x, thy xytimes[i].y,
            thy xytimes[i + 1].x, thy xytimes[i + 1].y,
            xmin, ymin, xmax, ymax, &x1, &y1, &x2, &y2)) {
			Graphics_line (g, x1, y1, x2, y2);
		}
	}

	if (inset) {
		Graphics_unsetInner (g);
	}
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

void DTW_drawPath (DTW me, Graphics g, double xmin, double xmax, double ymin,
                   double ymax, int garnish) {
	DTW_drawPath_raw (me, g, xmin, xmax, ymin, ymax, garnish, 1);
}

static void DTW_drawWarpX_raw (DTW me, Graphics g, double xmin, double xmax, double ymin, double ymax, double tx, int garnish, int inset) {
	double ty = DTW_getYTimeFromXTime (me, tx);
	int lineType = Graphics_inqLineType (g);

	if (xmin >= xmax) {
		xmin = my xmin; xmax = my xmax;
	}
	if (ymin >= ymax) {
		ymin = my ymin; ymax = my ymax;
	}

	if (inset) {
		Graphics_setInner (g);
	}
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	ty = DTW_getYTimeFromXTime (me, tx);
	Graphics_setLineType (g, Graphics_DOTTED);
	if (ty <= ymax) {
		Graphics_line (g, tx, ymin, tx, ty);
		Graphics_line (g, tx, ty, xmin, ty);
	} else {
		Graphics_line (g, tx, ymin, tx, ymax);
	}

	Graphics_setLineType (g, lineType);

	if (inset) {
		Graphics_unsetInner (g);
	}

	if (garnish) {
		Graphics_markBottom (g, tx, 1, 1, 0, NULL);
		if (ty <= ymax) {
			Graphics_markLeft (g, ty, 1, 1, 0, 0);
		}
	}
}

void DTW_drawWarpX (DTW me, Graphics g, double xmin, double xmax, double ymin, double ymax, double tx, int garnish) {
	DTW_drawWarpX_raw (me, g, xmin, xmax, ymin, ymax, tx, garnish, 1);
}

static void DTW_and_Sounds_checkDomains (DTW me, Sound *y, Sound *x, double *xmin, double *xmax, double *ymin, double *ymax) {
	if (my ymin == (*y) -> xmin && my ymax == (*y) -> xmax) {
		if (my xmin != (*x) -> xmin || my xmax != (*x) -> xmax) {
			Melder_throw ("The domains of the DTW and the sound('s) don't match");
		}
	} else if (my ymin == (*x) -> xmin && my ymax == (*x) -> xmax) {
		if (my xmin != (*y) -> xmin || my xmax != (*y) -> xmax) {
			Melder_throw ("The domains of the DTW and the sound('s) don't match");
		}
		Sound tmp = *y; *y = *x; *x = tmp; // swap x and y
	} else {
		Melder_throw ("The domains of the DTW and the sound('s) don't match");
	}

	if (*xmin >= *xmax) {
		*xmin = my xmin; *xmax = my xmax;
	}
	if (*ymin >= *ymax) {
		*ymin = my ymin; *ymax = my ymax;
	}
}

static void drawBox (Graphics g) {
	double x1WC, x2WC, y1WC, y2WC;
	double lineWidth = Graphics_inqLineWidth (g);
	Graphics_inqWindow (g, &x1WC, &x2WC, &y1WC, &y2WC);
	Graphics_setLineWidth (g, 2.0 * lineWidth);
	Graphics_rectangle (g, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineWidth (g, lineWidth);
}

/*
  In a picture with a DTW and a left and bottom Sound, we want "width" of the vertical sound
  and the "height" of the horizontal Sound t be equal.
  Given the horizontal fraction of the DTW-part, this routine returns the vertical part.
*/
static double _DTW_and_Sounds_getPartY (Graphics g, double dtw_part_x) {
	double x1NDC, x2NDC, y1NDC, y2NDC;
	Graphics_inqViewport (g, &x1NDC, &x2NDC, &y1NDC, &y2NDC);
	return 1 - ( (1 - dtw_part_x) * (x2NDC - x1NDC)) / (y2NDC - y1NDC);
}

void DTW_and_Sounds_draw (DTW me, Sound y, Sound x, Graphics g, double xmin, double xmax,
                          double ymin, double ymax, int garnish) {
	DTW_and_Sounds_checkDomains (me, &y, &x, &xmin, &xmax, &ymin, &ymax);

	Graphics_setInner (g);
	Graphics_Viewport ovp = g -> outerViewport; // save for unsetInner

	double dtw_part_x = 0.85;
	double dtw_part_y = _DTW_and_Sounds_getPartY (g, dtw_part_x);

	/* DTW */

	Graphics_Viewport vp = Graphics_insetViewport (g, 1 - dtw_part_x, 1, 1 - dtw_part_y, 1);
	DTW_paintDistances_raw (me, g, xmin, xmax, ymin, ymax, 0, 0, 0, 0);
	DTW_drawPath_raw (me, g, xmin, xmax, ymin, ymax, 0, 0);
	drawBox (g);
	Graphics_resetViewport (g, vp);

	/* Sound y */

	vp = Graphics_insetViewport (g, 0, 1 - dtw_part_x, 1 - dtw_part_y, 1);
	Sound_draw_btlr (y, g, ymin, ymax, -1, 1, FROM_BOTTOM_TO_TOP, 0);
	if (garnish) {
		drawBox (g);
	}
	Graphics_resetViewport (g, vp);

	/* Sound x */

	vp = Graphics_insetViewport (g, 1 - dtw_part_x, 1, 0, 1 - dtw_part_y);
	Sound_draw_btlr (x, g, xmin, xmax, -1, 1, FROM_LEFT_TO_RIGHT, 0);
	if (garnish) {
		drawBox (g);
	}
	Graphics_resetViewport (g, vp);


	/* Set window coordinates so that margins will work, i.e. extend time domains */

	double xmin3 = xmax - (xmax - xmin) / dtw_part_x;
	double ymin3 = ymax - (ymax - ymin) / dtw_part_y;
	Graphics_setWindow (g, xmin3, xmax, ymin3, ymax);

	g -> outerViewport = ovp; // restore from _setInner
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_markLeft (g, ymin, 1, 1, 0, NULL);
		Graphics_markLeft (g, ymax, 1, 1, 0, NULL);

		Graphics_markBottom (g, xmin, 1, 1, 0, NULL);
		Graphics_markBottom (g, xmax, 1, 1, 0, NULL);
	}
}

void DTW_and_Sounds_drawWarpX (DTW me, Sound yy, Sound xx, Graphics g, double xmin, double xmax,
                               double ymin, double ymax, double tx, int garnish) {
	Sound y = yy, x = xx;
	int lineType = Graphics_inqLineType (g);

	DTW_and_Sounds_checkDomains (me, &y, &x, &xmin, &xmax, &ymin, &ymax);

	Graphics_setInner (g);
	double dtw_part_x = 0.85;
	double dtw_part_y = _DTW_and_Sounds_getPartY (g, dtw_part_x);

	xmin = xmax - (xmax - xmin) / dtw_part_x;
	ymin = ymax - (ymax - ymin) / dtw_part_y;
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	double ty = DTW_getYTimeFromXTime (me, tx);
	Graphics_setLineType (g, Graphics_DOTTED);

	Graphics_line (g, tx, ymin, tx, ty);
	Graphics_line (g, tx, ty, xmin, ty);

	Graphics_setLineType (g, lineType);

	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_markBottom (g, tx, 1, 1, 0, NULL);
		Graphics_markLeft (g, ty, 1, 1, 0, NULL);
	}
}

Matrix DTW_to_Matrix_distances (DTW me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		NUMmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": distances not converted to Matrix.");
	}
}

/* nog aanpassen, dl = sqrt (dx^2 + dy^2) */
void DTW_drawDistancesAlongPath (DTW me, Graphics g, double xmin, double xmax, double dmin, double dmax, int garnish) {
	if (xmin >= xmax) {
		xmin = my xmin; xmax = my xmax;
	}
	long ixmax, ixmin;
	if (! Matrix_getWindowSamplesX (me, xmin, xmax, &ixmin, &ixmax)) {
		return;
	}

	long ii = 1;
	while (ii < my pathLength && my path[ii].x < ixmin) {
		ii++;
	}
	ixmin = ii;

	while (ii <= my pathLength && my path[ii].x < ixmax) {
		ii++;
	}
	ixmax = ii;

	autoNUMvector<double> d (ixmin, ixmax);

	for (long i = ixmin; i <= ixmax; i++) {
		d[i] = my z[my path[i].y][i];
	}

	if (dmin >= dmax) {
		NUMvector_extrema (d.peek(), ixmin, ixmax, &dmin, &dmax);
	} else {
		for (long i = ixmin; i <= ixmax; i++) {
			if (d[i] > dmax) {
				d[i] = dmax;
			} else if (d[i] < dmin) {
				d[i] = dmin;
			}
		}
	}

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, dmin, dmax);
	Graphics_function (g, d.peek(), ixmin, ixmax, xmin, xmax);
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textLeft (g, 1, L"distance");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

/*
	metric = 1...n (sum (a_i^n))^(1/n)
*/
DTW Matrices_to_DTW (I, thou, int matchStart, int matchEnd, int slope, int metric) {
	try {
		iam (Matrix); thouart (Matrix);

		if (thy ny != my ny) {
			Melder_throw (L"Columns must have the same dimensions.");
		}

		autoDTW him = DTW_create (my xmin, my xmax, my nx, my dx, my x1, thy xmin, thy xmax, thy nx, thy dx, thy x1);
		autoMelderProgress progess (L"Calculate distances");
		for (long i = 1; i <= my nx; i++) {
			for (long j = 1; j <= thy nx; j++) {
				/*
					First divide distance by maximum to prevent overflow when metric
					is a large number.
					d = (x^n)^(1/n) may overflow if x>1 & n >>1 even if d would not overflow!
				*/
				double dmax = 0, d = 0, dtmp;
				for (long k = 1; k <= my ny; k++) {
					dtmp = fabs (my z[k][i] - thy z[k][j]);
					if (dtmp > dmax) {
						dmax = dtmp;
					}
				}
				if (dmax > 0) {
					for (long k = 1; k <= my ny; k++) {
						dtmp = fabs (my z[k][i] - thy z[k][j]) / dmax;
						d +=  pow (dtmp, metric);
					}
				}
				d = dmax * pow (d, 1.0 / metric);
				his z[i][j] = d / my ny; /* == d * dy / ymax */
			}
			if ( (i % 10) == 1) {
				Melder_progress (0.999 * i / my nx, L"Calculate distances: column ",
				                  Melder_integer (i), L" from ", Melder_integer (my nx), L".");
			}
		}
		DTW_findPath (him.peek(), matchStart, matchEnd, slope);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("DTW not created from matrices.");
	}
}

DTW Spectrograms_to_DTW (Spectrogram me, Spectrogram thee, int matchStart,
                         int matchEnd, int slope, int metric) {
	try {
		if (my xmin != thy xmin || my ymax != thy ymax || my ny != thy ny) {
			Melder_throw (L"The number of frequencies and/or frequency ranges do not match.");
		}

		autoMatrix m1 = Spectrogram_to_Matrix (me);
		autoMatrix m2 = Spectrogram_to_Matrix (thee);

		// Take log10 for dB's (4e-10 scaling not necessary)

		for (long i = 1; i <= my ny; i++) {
			for (long j = 1; j <= my nx; j++) {
				m1 -> z[i][j] = 10 * log10 (m1 -> z[i][j]);
			}
		}
		for (long i = 1; i <= thy ny; i++) {
			for (long j = 1; j <= thy nx; j++) {
				m2 -> z[i][j] = 10 * log10 (m2 -> z[i][j]);
			}
		}

		autoDTW him = Matrices_to_DTW (m1.peek(), m2.peek(), matchStart, matchEnd, slope, metric);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("DTW not created from Spectrograms.");
	}
}

#define FREQUENCY(frame)  ((frame) -> candidate [1]. frequency)
#define NOT_VOICED(f)  ((f) <= 0.0 || (f) >= my ceiling)   /* This includes NUMundefined! */

static int Pitch_findFirstAndLastVoicedFrame (Pitch me, long *first, long *last) {
	*first = 1;
	while (*first <= my nx && ! Pitch_isVoiced_i (me, *first)) {
		(*first) ++;
	}
	*last = my nx;
	while (*last >= *first && ! Pitch_isVoiced_i (me, *last)) {
		(*last)--;
	}
	return *first <= my nx && *last >= 1;
}

DTW Pitches_to_DTW_sgc (Pitch me, Pitch thee, double vuv_costs, double time_weight, int matchStart, int matchEnd, int slope);
DTW Pitches_to_DTW_sgc (Pitch me, Pitch thee, double vuv_costs, double time_weight, int matchStart, int matchEnd, int slope) { // vuv_costs=24, time_weight=10 ?
	try {
		if (vuv_costs < 0) {
			Melder_throw ("Voiced-unvoiced costs may not be negative.");
		}
		if (time_weight < 0) {
			Melder_throw ("Time costs weight may not be negative.");
		}

		long myfirst, mylast, thyfirst, thylast;
		if (! Pitch_findFirstAndLastVoicedFrame (me, &myfirst, &mylast) ||
		        ! Pitch_findFirstAndLastVoicedFrame (thee, &thyfirst, &thylast)) {
			Melder_throw ("No voiced frames.");
		}
		/*
			We do not want the silences before the first voiced frame and after the last voiced frame
			to determine the distances.
			We create paths from (1,1)...(thyfirst,myfirst) and (thylast,mylast)...(thy nx,my nx)
			by making the other cell's distances very large.
		*/
		autoDTW him = DTW_create (my xmin, my xmax, my nx, my dx, my x1, thy xmin, thy xmax, thy nx, thy dx, thy x1);
		autoNUMvector<double> pitchx (1, thy nx);
		int unit = kPitch_unit_SEMITONES_100;
		for (long j = 1; j <= thy nx; j++) {
			pitchx[j] = Sampled_getValueAtSample (thee, j, Pitch_LEVEL_FREQUENCY, unit);
		}

		for (long i = 1; i <= my nx; i++) {
			double pitchy = Sampled_getValueAtSample (me, i, Pitch_LEVEL_FREQUENCY, unit);
			double t1 = my x1 + (i - 1) * my dx;
			for (long j = 1; j <= thy nx; j++) {
				double t2 = thy x1 + (j - 1) * thy dx;
				double dist_f = 0; // based on pitch difference
				double dist_t = fabs (t1 - t2);
				if (pitchy == NUMundefined) {
					if (pitchx[j] != NUMundefined) {
						dist_f = vuv_costs;
					}
				} else if (pitchx[j] == NUMundefined) {
					dist_f = vuv_costs;
				} else {
					dist_f = fabs (pitchy - pitchx[j]);
				}
				his z[i][j] = sqrt (dist_f * dist_f + time_weight * dist_t * dist_t);
			}
		}
		DTW_findPath (him.peek(), matchStart, matchEnd, slope);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("DTW not created from Pitches.");
	}
}

DTW Pitches_to_DTW (Pitch me, Pitch thee, double vuv_costs, double time_weight, int matchStart,
                    int matchEnd, int slope) { // vuv_costs=24, time_weight=10 ?
	try {
		if (vuv_costs < 0) {
			Melder_throw ("Voiced-unvoiced costs may not be negative.");
		}
		if (time_weight < 0) {
			Melder_throw ("Time costs weight may not be negative.");
		}

		autoDTW him = DTW_create (my xmin, my xmax, my nx, my dx, my x1, thy xmin, thy xmax, thy nx, thy dx, thy x1);
		autoNUMvector<double> pitchx (1, thy nx);
		int unit = kPitch_unit_SEMITONES_100;
		for (long j = 1; j <= thy nx; j++) {
			pitchx[j] = Sampled_getValueAtSample (thee, j, Pitch_LEVEL_FREQUENCY, unit);
		}

		for (long i = 1; i <= my nx; i++) {
			double pitchy = Sampled_getValueAtSample (me, i, Pitch_LEVEL_FREQUENCY, unit);
			double t1 = my x1 + (i - 1) * my dx;
			for (long j = 1; j <= thy nx; j++) {
				double t2 = thy x1 + (j - 1) * thy dx;
				double dist_f = 0; // based on pitch difference
				double dist_t = fabs (t1 - t2);
				if (pitchy == NUMundefined) {
					if (pitchx[j] != NUMundefined) {
						dist_f = vuv_costs;
					}
				} else if (pitchx[j] == NUMundefined) {
					dist_f = vuv_costs;
				} else {
					dist_f = fabs (pitchy - pitchx[j]);
				}
				his z[i][j] = sqrt (dist_f * dist_f + time_weight * dist_t * dist_t);
			}
		}

		DTW_findPath (him.peek(), matchStart, matchEnd, slope);
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("DTW not created from Pitches.");
	}
}

DurationTier DTW_to_DurationTier (DTW me) {
	(void) me;
	DurationTier thee = NULL;
	return thee;
}

void DTW_and_Matrix_replace (DTW me, Matrix thee) {
	try {
		if (my xmin != thy xmin || my xmax != thy xmax || my ymin != thy ymin || my ymax != thy ymax) {
			Melder_throw ("The X and Y domains of the matrix and the DTW must be equal.");
		}
		if (my nx != thy nx || my dx != thy dx || my ny != thy ny || my dy != thy dy) {
			Melder_throw ("The sampling of the matrix and the DTW must be equal.");
		}
		double minimum, maximum;
		Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
		if (minimum < 0) {
			Melder_throw ("Distances cannot be negative.");
		}
		NUMmatrix_copyElements<double> (thy z, my z, 1, my ny, 1, my nx);
	} catch (MelderError) {
		Melder_throw (me, ": distances not replaced.");
	}
}

/****************** new implementation ********/

void DTW_findPath (DTW me, int matchStart, int matchEnd, int slope) {
    DTW_findPath_special (me, matchStart, matchEnd, slope, 0);
}

Matrix DTW_to_Matrix_cummulativeDistances (DTW me, double sakoeChibaBand, int slope) {
    try {
        Matrix cummulativeDistances;
        DTW_findPath_bandAndSlope (me, sakoeChibaBand, slope, &cummulativeDistances);
        return cummulativeDistances;
    } catch (MelderError) {
        Melder_throw (me, ": cummulative costs matrix not created.");
    }
}

static void DTW_checkSlopeConstraints (DTW me, double band, int slope) {
    try {
        double slopes[5] = { DTW_BIG, DTW_BIG, 3, 2, 1.5 } ;
        double dtw_slope = (my ymax - my ymin - band) / (my xmax - my xmin - band);
        if (slope < 1 || slope > 4) {
            Melder_throw ("Invalid slope constraint.");
        }
        if (dtw_slope <= 0 && slope != 1) {
            Melder_throw ("Band too wide.");
        }
        if (dtw_slope < 1) {
            dtw_slope = 1 / dtw_slope;
        }
            if (dtw_slope > slopes[slope]) {
            Melder_warning (L"There is a conflict between the chosen slope constraint and the relative duration. "
				"The duration ratio of the longest and the shortest object is ", Melder_double (dtw_slope),
				L". This implies that the largest slope in the constraint must have a value greater or equal to this ratio.");
        }
    } catch (MelderError) {
        Melder_throw ("Slope constraints can't be met.");
    }
}

static void DTW_and_Polygon_setUnreachableParts (DTW me, Polygon thee, long **psi) {
    try {
        double eps = my dx / 100; // safely enough
        double dtw_slope = (my ymax - my ymin) / (my xmax - my xmin);

        double xmin, xmax, ymin, ymax;
        Polygon_getExtrema (thee, &xmin, &xmax, &ymin, &ymax);
        // if the Polygon and the DTW don't overlap everything is unreachable!
        if (xmax <= my xmin || xmin >= my xmax || ymax <= my ymin || ymin >= my ymax) {
            Melder_throw ("DTW and Polygon don't overlap.");
        }
        // find border "above" polygon
        for (long ix = 1; ix <= my nx; ix++) {
            double x = my x1 + (ix - 1) * my dx;
            long iystart = (dtw_slope * ix * (my dx / my dy) + 1.0);
            for (long iy = iystart + 1; iy <= my ny; iy++) {
                double y = my y1 + (iy - 1) * my dy;
                if (Polygon_getLocationOfPoint (thee, x, y, eps) == Polygon_OUTSIDE) {
                    for (long k = iy; k <= my ny; k++) {
                        psi[k][ix] =  DTW_UNREACHABLE;
                    }
                    break;
                }
            }
        }
        // find border "below" polygon
        for (long ix = 2; ix <= my nx; ix++) {
            double x = my x1 + (ix - 1) * my dx;
            long iystart = (dtw_slope * ix * (my dx / my dy)); // start 1 lower
            if (iystart > my ny) iystart = my ny;
            for (long iy = iystart - 1; iy >= 1; iy--) {
                double y = my y1 + (iy - 1) * my dy;
                if (Polygon_getLocationOfPoint (thee, x, y, eps) == Polygon_OUTSIDE) {
                    for (long k = iy; k >= 1; k--) {
                        psi[k][ix] = DTW_UNREACHABLE;
                    }
                    break;
                }
            }
        }
    } catch (MelderError) {
        Melder_throw (me, " can't set unreachable parts.");
    }

}

#define DTW_ISREACHABLE(y,x) ((psi[y][x] != DTW_UNREACHABLE) && (psi[y][x] != DTW_FORBIDDEN))
static void DTW_findPath_special (DTW me, int matchStart, int matchEnd, int slope, Matrix *cummulativeDists) {
    (void) matchStart;
    (void) matchEnd;
	try {
       autoPolygon thee = DTW_to_Polygon (me, 0.0, slope);
       DTW_and_Polygon_findPathInside (me, thee.peek(), slope, cummulativeDists);
	} catch (MelderError) {
		Melder_throw (me, ": cannot find path.");
	}
}

// Intersection of two straight lines y = a[i]*x+b[i], where a[2] = 1 / a[1]. Point (x1,y1) is on first line,
// point (x2,y2) is on second line.
static void getIntersectionPoint (double x1, double y1, double x2, double y2, double a, double *x3, double *y3) {
    *x3 = (y2 - y1 + a * x1 - x2 / a) / (a - 1 / a);
    *y3 = a * *x3 + y1 - a * x1;
}

Polygon DTW_to_Polygon (DTW me, double band, int slope) {
    try {
        DTW_checkSlopeConstraints (me, band, slope);
        double slopes[5] = { DTW_BIG, DTW_BIG, 3, 2, 1.5 } ;
        if (band <= 0) {
            if (slope == 1) {
                autoPolygon thee = Polygon_create (4);
                thy x[1] = my xmin; thy y[1] = my ymin;
                thy x[2] = my xmin; thy y[2] = my ymax;
                thy x[3] = my xmax; thy y[3] = my ymax;
                thy x[4] = my xmax; thy y[4] = my ymin;
                return thee.transfer();
            } else {
                autoPolygon thee = Polygon_create (4);
                thy x[1] = my xmin; thy y[1] = my ymin;
                thy x[3] = my xmax; thy y[3] = my ymax;
                double x, y;
                getIntersectionPoint (my xmin, my ymin, my xmax, my ymax, slopes[slope], &x, &y);
                if (x < my xmin) x = my xmin;
                if (x > my xmax) x = my xmax;
                if (y < my ymin) y = my ymin;
                if (y > my ymax) y = my ymax;
                thy x[2] = x;
                thy y[2] = y;
                getIntersectionPoint (my xmin, my ymin, my xmax, my ymax, 1 / slopes[slope], &x, &y);
                if (x < my xmin) x = my xmin;
                if (x > my xmax) x = my xmax;
                if (y < my ymin) y = my ymin;
                if (y > my ymax) y = my ymax;
                thy x[4] = x;
                thy y[4] = y;
                return thee.transfer();
            }
        } else {
            if (slope == 1) {
                autoPolygon thee = Polygon_create (6);
                thy x[1] = my xmin; thy y[1] = my ymin;
                thy x[2] = my xmin; thy y[2] = my ymin + band;
                thy x[3] = my xmax - band; thy y[3] = my ymax;
                thy x[4] = my xmax; thy y[4] = my ymax;
                thy x[5] = my xmax; thy y[5] = my ymax - band;
                thy x[6] = my xmin + band; thy y[6] = my ymin;
                return thee.transfer();
            } else {
                autoPolygon thee = Polygon_create (8);
                double x, y;
                thy x[1] = my xmin; thy y[1] = my ymin;
                thy x[2] = my xmin; thy y[2] = my ymin + band;
                getIntersectionPoint (my xmin, my ymin + band, my xmax - band, my ymax, slopes[slope], &x, &y);
                if (x < my xmin) x = my xmin;
                if (x > my xmax) x = my xmax;
                if (y < my ymin) y = my ymin;
                if (y > my ymax) y = my ymax;
                thy x[3] = x;
                thy y[3] = y;
                thy x[4] = my xmax - band; thy y[4] = my ymax;
                thy x[5] = my xmax; thy y[5]= my ymax;
                thy x[6] = my xmax; thy y[6] = my ymax - band;
                getIntersectionPoint (my xmin + band, my ymin, my xmax, my ymax - band, 1 / slopes[slope], &x, &y);
                if (x < my xmin) x = my xmin;
                if (x > my xmax) x = my xmax;
                if (y < my ymin) y = my ymin;
                if (y > my ymax) y = my ymax;
                thy x[7] = x;
                thy y[7] = y;
                thy x[8] = my xmin + band; thy y[8] = my ymin;
                return thee.transfer();
            }
        }
    } catch (MelderError) {
        Melder_throw (me, " no Polygon created.");
    }
}

Matrix DTW_and_Polygon_to_Matrix_cummulativeDistances (DTW me, Polygon thee, int localSlope) {
    try {
        Matrix cummulativeDistances;
        DTW_and_Polygon_findPathInside (me, thee, localSlope, &cummulativeDistances);
        return cummulativeDistances;
    } catch (MelderError) {
        Melder_throw (me, ": cummulative costs matrix not created from DTW and Polygon.");
    }
}

void DTW_findPath_bandAndSlope (DTW me, double sakoeChibaBand, int localSlope, Matrix *cummulativeDists) {
    try {
        autoPolygon thee = DTW_to_Polygon (me, sakoeChibaBand, localSlope);
        DTW_and_Polygon_findPathInside (me, thee.peek(), localSlope, cummulativeDists);
    } catch (MelderError) {
        Melder_throw (me, L" can't determine the path.");
    }
}

void DTW_and_Polygon_findPathInside (DTW me, Polygon thee, int localSlope, Matrix *cummulativeDists) {
    try {
        double slopes[5] = { DTW_BIG, DTW_BIG, 3, 2, 1.5 };
        long pathIndex = my nx + my ny - 1; /* Maximum path length */
        // if localSlope == 1 start of path is within 10% of minimum duration. Starts farther away
        long delta_xy = (my nx < my ny ? my nx : my ny) / 10; // if localSlope == 1 start within 10% of

        if (localSlope < 1 || localSlope > 4) {
            Melder_throw ("Local slope parameter is illegal.");
        }

        autoNUMmatrix<double> delta (-2, my ny, -2, my nx);
        autoNUMmatrix<long> psi (-2, my ny, -2, my nx);
        for (long i = 1; i <= my ny; i++) {
            for (long j = 1; j <= my nx; j++) {
                delta[i][j] = my z[i][j];
            }
        }
        // start by making the outside unreachable
        for (long k = -2; k <= 1; k++) {
           for (long j = -2; j <= my nx; j++) {
               // delta[k][j] = DTW_BIG;
                psi[k][j] = DTW_UNREACHABLE;
            }
            for (long i = 1; i <= my ny; i++) {
             //   delta[i][k] = DTW_BIG;
                psi[i][k] = DTW_UNREACHABLE;
            }
        }

        // Make begin part of first column reachable
        long rowto = delta_xy;
        if (localSlope != 1) rowto = slopes[localSlope] + 1.0;
        for (long iy = 2; iy <= rowto; iy++) {
            if (localSlope != 1) {
                delta[iy][1] = delta[iy - 1][1] + my z[iy][1];
                psi[iy][1] = DTW_Y;
            } else {
                psi[iy][1] = DTW_START; // will be adapted by DTW_and_Polygon_setUnreachableParts
            }
        }
        // Make begin part of first row reachable
        long colto = delta_xy;
        if (localSlope != 1) colto = slopes[localSlope] + 1.0;
        for (long ix = 2; ix <= colto; ix++) {
            if (localSlope != 1) {
                delta[1][ix] = delta[1][ix -1] + my z[1][ix];
                psi[1][ix] = DTW_X;
            } else {
                psi[1][ix] = DTW_START; // will be adapted by DTW_and_Polygon_setUnreachableParts
           }
        }

        // Now we can set the unreachable parts from the Polygon
        DTW_and_Polygon_setUnreachableParts (me, thee, psi.peek());

        // Forward pass.
        long numberOfIsolatedPoints = 0;
        autoMelderProgress progress (L"Find path");
        for (long j = 2; j <= my nx; j++) {
            for (long i = 2; i <= my ny; i++) {
                if (! DTW_ISREACHABLE (i, j)) continue;
                double g, gmin = DTW_BIG;
                long direction = 0;
                if (DTW_ISREACHABLE (i - 1, j - 1)) {
                    gmin = delta[i - 1][j - 1] + 2 * my z[i][j];
                    direction = DTW_XANDY;
                } else if (DTW_ISREACHABLE (i, j - 1)) {
                    gmin = delta[i][j - 1] + my z[i][j];
                    direction = DTW_X;
                } else if (DTW_ISREACHABLE (i - 1, j)) {
                    gmin = delta[i - 1][j] + my z[i][j];
                    direction = DTW_Y;
                } else {
                    numberOfIsolatedPoints++;
                    continue;
                }

                switch (localSlope) {
                case 1:  { // no restriction
                    if (DTW_ISREACHABLE (i, j - 1) && ((g = delta[i][j - 1] + my z[i][j]) < gmin)) {
                        gmin = g;
                        direction = DTW_X;
                    }
                    if (DTW_ISREACHABLE (i - 1, j) && ((g = delta[i - 1][j] + my z[i][j]) < gmin)) {
                        gmin = g;
                        direction = DTW_Y;
                    }
                }
                break;

                // P = 1/2

                case 2: { // P = 1/2
                    if (DTW_ISREACHABLE (i - 1, j - 3) && psi[i][j - 1] == DTW_X && psi[i][j - 2] == DTW_XANDY &&
                        (g = delta[i-1][j-3] + 2 * my z[i][j-2] + my z[i][j-1] + my z[i][j]) < gmin) {
                        gmin = g;
                        direction = DTW_X;
                    }
                    if (DTW_ISREACHABLE (i - 1, j - 2) && psi[i][j - 1] == DTW_XANDY &&
                        (g = delta[i - 1][j - 2] + 2 * my z[i][j - 1] + my z[i][j]) < gmin) {
                        gmin = g;
                        direction = DTW_X;
                    }
                    if (DTW_ISREACHABLE (i - 2, j - 1) && psi[i - 1][j] == DTW_XANDY &&
                        (g = delta[i - 2][j - 1] + 2 * my z[i - 1][j] + my z[i][j]) < gmin) {
                        gmin = g;
                        direction = DTW_Y;
                    }
                    if (DTW_ISREACHABLE (i - 3, j - 1) && psi[i - 1][j] == DTW_Y && psi[i - 2][j] == DTW_XANDY &&
                        (g = delta[i-3][j-1] + 2 * my z[i-2][j] + my z[i-1][j] + my z[i][j]) < gmin) {
                        gmin = g;
                        direction = DTW_Y;
                    }
                }
                break;

                // P = 1

                case 3: {
                    if (DTW_ISREACHABLE (i - 1, j - 2) && psi[i][j - 1] == DTW_XANDY &&
                        (g = delta[i - 1][j - 2] + 2 * my z[i][j - 1] + my z[i][j]) < gmin) {
                        gmin = g;
                        direction = DTW_X;
                    }
                    if (DTW_ISREACHABLE (i - 2, j - 1) && psi[i - 1][j] == DTW_XANDY &&
                        (g = delta[i - 2][j - 1] + 2 * my z[i - 1][j] + my z[i][j]) < gmin) {
                        gmin = g;
                        direction = DTW_Y;
                    }
                }
                break;

                // P = 2

                case 4: {
                    if (DTW_ISREACHABLE (i - 2, j - 3) && psi[i][j - 1] == DTW_XANDY && psi[i - 1][j - 2] == DTW_XANDY &&
                        (g = delta[i-2][j-3] + 2 * my z[i-1][j-2] + 2 * my z[i][j-1] + my z[i][j]) < gmin) {
                            gmin = g;
                            direction = DTW_X;
                    }
                    if (DTW_ISREACHABLE (i - 3, j - 2) && psi[i - 1][j] == DTW_XANDY && psi[i - 2][j - 1] == DTW_XANDY &&
                        (g = delta[i-3][j-2] + 2 * my z[i-2][j-1] + 2 * my z[i-1][j] + my z[i][j]) < gmin) {
                            gmin = g;
                            direction = DTW_Y;
                    }
                }
                break;
                default:
                break;
                }
                Melder_assert (direction != 0);
                psi[i][j] = direction;
                delta[i][j] = gmin;
            }
            if ((j % 10) == 2) {
                Melder_progress (0.999 * j / my nx, L"Calculate time warp: frame ",
                    Melder_integer (j), L" from ", Melder_integer (my nx), L".");
            }
        }

        // Find minimum at end of path and trace back.

        long iy = my ny;
        double minimum = delta[iy][my nx];
        for (long i = my ny - 1; i > 0; i--) {
            if (! DTW_ISREACHABLE (i, my nx)) {
                break; // we're in unreachable places
            } else if (delta[i][my nx] < minimum) {
                minimum = delta[iy = i][my nx];
            }
        }

        my weightedDistance = minimum / (my nx + my ny);
        my path[pathIndex].y = iy;
        long ix = my path[pathIndex].x = my nx;

        // Fill path backwards.

        while (ix > 1) {
            if (psi[iy][ix] == DTW_XANDY) {
                ix--;
                iy--;
            } else if (psi[iy][ix] == DTW_X) {
                ix--;
            } else if (psi[iy][ix] == DTW_Y) {
                iy--;
            } else if (psi[iy][ix] == DTW_START) {
                break;
            }
            if (pathIndex < 1 || iy < 1) break;
            //Melder_assert (pathIndex > 1 && iy > 0);
            my path[--pathIndex].x = ix;
            my path[pathIndex].y = iy;
        }

        my pathLength = my nx + my ny - pathIndex;
        if (pathIndex > 1) {
            for (long j = 1; j <= my pathLength; j++) {
                my path[j] = my path[pathIndex++];
            }
        }

        DTW_Path_recode (me);
        if (cummulativeDists != 0) {
            autoMatrix him = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
                my ymin, my ymax, my ny, my dy, my y1);
            for (long i = 1; i <= my ny; i++) {
                for (long j = 1; j <= my nx; j++) {
                    his z[i][j] = delta[i][j];
                }
            }
            *cummulativeDists = him.transfer();
        }
    } catch (MelderError) {
        Melder_throw (me, ": cannot find path.");
    }
}

/* End of file DTW.cpp */
