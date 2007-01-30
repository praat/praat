/* DTW.c
 *
 * Copyright (C) 1993-2006 David Weenink
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
*/

#include "DTW.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "DTW_def.h"
#include "oo_COPY.h"
#include "DTW_def.h"
#include "oo_EQUAL.h"
#include "DTW_def.h"
#include "oo_WRITE_ASCII.h"
#include "DTW_def.h"
#include "oo_WRITE_BINARY.h"
#include "DTW_def.h"
#include "oo_READ_ASCII.h"
#include "DTW_def.h"
#include "oo_READ_BINARY.h"
#include "DTW_def.h"
#include "oo_DESCRIPTION.h"
#include "DTW_def.h"

#define DTW_BIG 1e38
/*
	Two 'slope lines, lh and ll, start in the lower left corner, the upper/lower has the maximum/minimum allowed slope.
	Two other lines, ru and rl, end in the upper-right corner. The upper/lower line have minimum/maximum slope.
	1. the four lines with the two intersections determine a diamond
		For vertical line at x we return the upper and lower y of the two intersections 
	1. When we don't have a diamond, we return an error and the vertical distance between the slopelines in ylow.
*/

static int get_ylimitsFromConstraints (long nsteps_xory, long nsteps_xandy, long nx, long ny, long x, long *ylow, long *yhigh)
{
	long xextension, yextension, nfitsx, restx, nfitsy, resty;
	long i, xp[3], yl[3],yh[3];

	if (nsteps_xandy == 0)
	{
		*ylow = 1; *yhigh = ny + 1;
		return 1;
	}
	else if (nsteps_xory < 2)
	{
		*ylow = *yhigh = x;
		return 1;
	}
	xextension = nsteps_xory + nsteps_xandy - 1;
	yextension = nsteps_xandy + 1 - 1;
	xp[1] = x;
	xp[2] = nx - x + 1;
	for (i = 1; i <= 2; i++)
	{
		nfitsx = (xp[i] - 1) / xextension;
		nfitsy = (xp[i] - 1) / yextension;
		resty = (xp[i] - 1) % yextension;
		resty += nsteps_xory;
		yh[i] = nfitsy * xextension + resty;
		restx =  (xp[i] - 1) % xextension;
		restx = restx > 0 && restx > nsteps_xory ? restx - nsteps_xory + 1 : 1;
		yl[i] = nfitsx * yextension + restx;
	}
	yh[0] = yh[2];
	yh[2] = ny - yl[2] + 1;
	yl[2] = ny - yh[0] - 1;
	/*
		When no intersection return the 'distance'
	*/
	if (((*ylow = yl[1] - yh[2]) > 0) || ((*ylow = yl[2]- yh[1]) > 0)) return 0;
	
	*ylow = yl[1] > yl[2] ? yl[1] : yl[2];
	*yhigh = yh[1] < yh[2] ? yh[1] : yh[2];
	
	return 1;
}

static void get_adjustmentwindow_parameters (DTW me, double adjustment_window_duration, int adjustment_window_includes_end,
	long *r, double *adjustment_window_slope)
{
	double durationx = (my xmax - my xmin); 
	double durationy = (my ymax - my ymin);
	double slope = durationy / durationx;
	
	*adjustment_window_slope = slope;
	
	/* If the adjustment_window includes the endpoint we set a slope on the adjustment range r */
	*adjustment_window_slope = adjustment_window_includes_end ? slope : 1;
	 
	*r = ceil (fabs (adjustment_window_duration) / my dx); /* r may be 0 */
}

static void get_ylimitsFromAdjustmentwindow (DTW me, double adjustment_window_duration, int adjustment_window_includes_end,
	long x, long *ylow, long *yhigh)
{
	double dtx, ty, adjustment_window_slope;
	long yscaled, r;
		
	get_adjustmentwindow_parameters (me, adjustment_window_duration, adjustment_window_includes_end, &r, &adjustment_window_slope);
	
	dtx = my x1 + (x - 1) * my dx - my xmin;
	ty = my ymin + adjustment_window_slope * dtx;
	yscaled = (ty - my y1) / my dy;
	
	*yhigh = yscaled + r;
	if (*yhigh > my ny) *yhigh = my ny;
	*ylow = yscaled - r;
	if (*ylow < 1) *ylow = 1;
	/* Next will occur when adjustment_window + durationY < durationX and ! adjustment_window_includes_end */
	if (*ylow > my ny) *ylow = my ny + 1;
}

static void info (I)
{
	iam (DTW);
	
	classData -> info (me);
	MelderInfo_writeLine5 ("Domain prototype:", Melder_double (my ymin), " to ", 
		Melder_double (my ymax), " (s).");
	MelderInfo_writeLine5 ("Domain candidate:", Melder_double (my xmin), " to ", 
		Melder_double (my xmax), " (s).");
	MelderInfo_writeLine2 ("Number of frames prototype: ", Melder_integer (my ny));
	MelderInfo_writeLine2 ("Number of frames candidate: ", Melder_integer (my nx));
	MelderInfo_writeLine2 ("Path length (frames): ", Melder_integer (my pathLength));
	MelderInfo_writeLine2 ("Global warped distance: ", Melder_double (my weightedDistance));
	if (my nx == my ny)
	{
		double dd = 0; long i;
		for (i=1; i <= my nx; i++)
		{
			dd += my z[i][i];
		}
		MelderInfo_writeLine2 ("Distance along diagonal: ", Melder_double (dd / my nx));
	}
}

class_methods (DTW, Matrix)
	class_method_local (DTW, destroy)
	class_method_local (DTW, equal)
	class_method_local (DTW, copy)
	class_method (info)
	class_method_local (DTW, readAscii)
	class_method_local (DTW, readBinary)
	class_method_local (DTW, writeAscii)
	class_method_local (DTW, writeBinary)
	class_method_local (DTW, description)
class_methods_end

Any DTW_create (double tminp, double tmaxp, long ntp, double dtp, double t1p,
	double tminc, double tmaxc, long ntc, double dtc, double t1c)
{
	DTW me = new (DTW);
	if (me == NULL || ! Matrix_init (me, tminc, tmaxc, ntc, dtc, t1c,
			tminp, tmaxp, ntp, dtp, t1p) ||
		((my path = NUMstructvector (DTW_State, 1, ntc + ntp - 1)) == NULL))
			forget (me);
	return me;
}

DTW DTW_swapAxes (DTW me)
{
	DTW thee;
	long x, y, i;
	
	thee = DTW_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
	if (thee == NULL) return NULL;

	for (x = 1; x <= my nx; x++)
	{
		for (y = 1; y <= my ny; y++)
		{
			thy z[x][y] = my z[y][x];
		}
	}
	thy pathLength = my pathLength;
	for (i = 1; i <= my pathLength; i++)
	{
		thy path[i].x = my path[i].y;
		thy path[i].y = my path[i].x;
	}
	return thee;
}

static int DTW_checkAdjustmentWindow (DTW me, char *proc, double adjustment_window_duration, int adjustment_window_includes_end)
{
	double durationx = (my xmax - my xmin), durationMin = durationx; 
	double durationy = (my ymax - my ymin), durationMax = durationy;

	if (durationx > durationy)
	{
		durationMin = durationy; durationMax = durationx;
	}
	
	if (! adjustment_window_includes_end && durationMin + adjustment_window_duration < durationMax)
	{
		return Melder_error ("%s: There is a conflict between the chosen parameters.\n "
			"We cannot \'Match end positions\' because the sum of the durations of the shortest \n"
			"object and the adjustment window is shorter than the duration of the longest object and you have not \n"
			"chosen to include the end point in the adjustment window.\n"
			"Suggestions:\n"
			"1. Include end point in adjustment window, or,\n"
			"2. Uncheck \'Match end positions\'.", proc);
	}
	return 1;
}

static int DTW_checkSlopeConstraintParameters (DTW me, char *proc, long nsteps_xory, long nsteps_xandy)
{
	double durationx = (my xmax - my xmin), durationMin = durationx; 
	double durationy = (my ymax - my ymin), durationMax = durationy;
	double slope = durationy / durationx;
 
	if (durationx > durationy)
	{
		durationMin = durationy; durationMax = durationx;
	}
		
	if (nsteps_xory > 0 && nsteps_xandy > 0)
	{
		/* Where do we end on y with minimum and maximum slope possible? */
		long ylow, yhigh;
		
		if (! get_ylimitsFromConstraints (nsteps_xory, nsteps_xandy, my nx, my ny, 1, &ylow, &yhigh) && ylow > 0)
		{
			double slope_min = slope < 1 ? 1 / slope : slope;
			return Melder_error ("%s: The slope constraints cannot be satisfied.\n"
				"Hint:\n"
				"To follow the the shortest path diagonal, the quotient \'Non-diagonal steps\'/\'Diagonal steps\' is %.4g.\n"
				"",
				proc, slope_min);
		}
	}
	return 1;
}

static int get_ylimits_x (DTW me, int choice, double adjustment_window_duration, int adjustment_window_includes_end, 
	long nsteps_xory, long nsteps_xandy, long x, long *ylow, long *yhigh)
{
	if (choice == DTW_SAKOECHIBA)
	{
		get_ylimitsFromAdjustmentwindow (me, adjustment_window_duration, adjustment_window_includes_end, x, ylow, yhigh);
	}
	else if (choice == DTW_SLOPES)
	{
		return get_ylimitsFromConstraints (nsteps_xory, nsteps_xandy, my nx, my ny, x, ylow, yhigh);
	}
	return 1;
}

static int slope_constraints (long x, long y, long nsteps_x, long nsteps_y, long nsteps_xandy, long **psi)
{
	long i, xm = 0, ym = 0; /* mx and my as variables gives compilation errors */
	long minsteps_xory = nsteps_x < nsteps_y ? nsteps_x : nsteps_y;
	
	/* No constraints ?*/
	if (nsteps_xandy <= 0) return 1;

	
	/* In lower left corner? */
	if (x < nsteps_x && y < nsteps_y) return 1;
	
	for (i = 1; i <= minsteps_xory + nsteps_xandy; i++)
	{	
		/* X? */
	    if (x > 1 && psi[y][x] == DTW_X)
		{  
			xm++;
			if (xm >= nsteps_x) return 0;
			x--; 
		}
		/* Y?  */
		else if (y > 1 && psi[y][x] == DTW_Y)
		{
			ym++;
			if (ym >= nsteps_y) return 0;
			y--;
		}
		/* XANDY? */
		else if (x > 1 && y > 1 && psi[y][x] == DTW_XANDY)
		{
			x--; y--;
		}
		else break;
	}
	return 1;
}

#define DTW_REACHABLE(x,y) (psi[y][x] != DTW_UNREACHABLE && psi[y][x] != DTW_FORBIDDEN)
/* Improvement: Working with unsigned longs -> psi[y[[x] & 1 */

/* Does not chack validity of parameters! */
static int _DTW_pathFinder (DTW me, int choice, double adjustment_window_duration, int adjustment_window_includes_end, 
	long nsteps_xory, long nsteps_xandy, double costs_x, double costs_y, double costs_xandy)
{
	float **delta = NULL, minimum;
	long xmargin = my dx, ymargin = my dy, x, y, ylow, yhigh, xpos, ypos, **psi = NULL;
	long pathIndex, numberOfCells = 0, nodirection_assigned = 0;
	long nsteps_x = nsteps_xory, nsteps_y = nsteps_xory;
	int xstart_flexible = 1, xend_flexible = 1, ystart_flexible = 1, yend_flexible = 1;
	int no_slope_constraints = choice != DTW_SLOPES || nsteps_xandy == 0;
			 
	/* Always allow HOR+VER = DIAG */
	
	if (((delta = NUMfmatrix_copy (my z, 1, my ny, 1, my nx)) == NULL) ||
		((psi = NUMlmatrix (1, my ny, 1, my nx)) == NULL)) goto end;
	/*
		1. Forward pass.
	*/
	
	Melder_progress (0.0, "Find path");
	
	/* 
		initialize the psi and delta matrices.
		Treat first column and row as special.
	*/
	
	for (x = 1; x <= my nx; x++)
	{
		if (! get_ylimits_x (me, choice, adjustment_window_duration, adjustment_window_includes_end, 
			nsteps_xory, nsteps_xandy, x, &ylow, &yhigh)) goto end;
		/*Melder_casual ("x, ylow yhigh: %5d %5d %5d", x, ylow, yhigh);*/
		for (y = my ny; y > yhigh; y--) psi[y][x] = DTW_FORBIDDEN;
		for (y = ylow - 1; y >= 1; y--) psi[y][x] = DTW_FORBIDDEN;
		for (y = 1; y <= my ny; y++) delta[y][x] = DTW_BIG;
		
		if (x == 1) /* First column is special */
		{
			psi[1][x] = DTW_START;
			delta[1][x] = my z[1][x];
			for (y = 2; y <= ylow; y++)
			{
				if (ystart_flexible && ! no_slope_constraints) /* If y<= ylow then path may start at any (y,1) */
				{
					psi[y][x] = DTW_START;
					delta[y][x] = my z[y][x];
				}
				else /* Path may only start at (1,1) */
				{
					psi[y][x] = DTW_Y;
					delta[y][x] += delta[y-1][x] + costs_y * my z[y][x];
				}
			}
		}
		if (psi[1][x] != DTW_FORBIDDEN && x > 1) /* First part of first row is special */
		{
			if (xstart_flexible && ! no_slope_constraints) /* If x reachabe then path may start at any (1,x) */
			{
				psi[1][x] = DTW_START;
				delta[1][x] = my z[1][y];
			}
			else /* Can only be reached from X direction */
			{
				psi[1][x] = DTW_X;
				delta[1][x] += delta[1][x-1] + costs_x * my z[1][x];
			}
		}
	}
		
	for (x = 2; x <= my nx; x++)
	{
		if (! get_ylimits_x (me, choice, adjustment_window_duration, adjustment_window_includes_end, 
			nsteps_xory, nsteps_xandy, x, &ylow, &yhigh)) return Melder_error ("Er");

		if (ylow < 2) ylow = 2;
		if (yhigh > my ny) yhigh = my ny;
		for (y = ylow; y <= yhigh; y++)
		{
			long direction; float g;
			
			numberOfCells++;
			minimum = DTW_BIG;
			direction = DTW_UNREACHABLE;

			if (DTW_REACHABLE(x-1,y-1))
			{
				minimum = delta[y-1][x-1] + costs_xandy * my z[y][x];
				direction = DTW_XANDY;
			}
			if (DTW_REACHABLE(x-1,y) && 
				(no_slope_constraints || slope_constraints (x - 1, y, nsteps_x - 1, nsteps_y, nsteps_xandy, psi)) &&
				(g = delta[y][x-1] + costs_x * my z[y][x]) < minimum)
			{
				minimum = g;
				direction = DTW_X;
			}
			if (DTW_REACHABLE(x,y-1) && 
				(no_slope_constraints || slope_constraints (x, y - 1, nsteps_x, nsteps_y - 1, nsteps_xandy, psi)) &&
				(g = delta[y-1][x] + costs_y * my z[y][x]) < minimum)
			{
				minimum = g;
				direction = DTW_Y;
			}
		
			if (direction == DTW_UNREACHABLE) /* Point near the borders */
			{
				nodirection_assigned++;
				if (Melder_debug != 0)
				{
					if (nodirection_assigned == 1) MelderInfo_open ();
					MelderInfo_writeLine4 (Melder_integer (x), " ",  Melder_integer (y), " (x,y) unreachable.");
				}
			}
			delta[y][x] = minimum;
			psi[y][x] = direction;
		}
		if ((x % 10) == 2 && ! Melder_progress (0.999 * x / my nx,
			"Calculate time warp: frame %ld from %ld", x, my nx)) goto end;
	}

	/*
		2. Backward pass.

		Find minimum at end of path and trace back.
	*/
	if (Melder_debug != 0 && nodirection_assigned > 0)
	{
		MelderInfo_writeLine4 (Melder_integer (nodirection_assigned), " cells from ", Melder_integer (numberOfCells), " had no valid direction assigned.");
		MelderInfo_close ();
		
	}
	minimum = delta[ypos = my ny][xpos = my nx];
	if (choice == DTW_SLOPES)
	{
		xmargin = xend_flexible ? nsteps_xory : 0;
		ymargin = yend_flexible ? nsteps_xory : 0;
	}
	/* Find minimum in last column; start from top */
	for (y = my ny; y > my ny - ymargin; y--)
	{
		if (psi[y][my nx] == DTW_FORBIDDEN) break;
		if (delta[y][my nx] < minimum) minimum = delta[ypos = y][my nx];
	}
	/* Search back along x */
	for (x = my nx; x > my nx - xmargin; x--)
	{
		if (psi[my ny][x] == DTW_FORBIDDEN) break;
		if (delta[my ny][x] < minimum) minimum = delta[my ny][xpos=x];
	}
	
	pathIndex = my nx + my ny - 1; /* At maximum path length */
	my weightedDistance = minimum / ( my nx + my ny);
	my path[pathIndex].y = y = ypos;
	my path[pathIndex].x = x = xpos;
	
	/*
		Fill path backwards.
	*/
	 
	while (x > 1)
	{
		if (psi[y][x] == DTW_XANDY)
		{
			x--;
			y--;
		}
		else if (psi[y][x] == DTW_X)
		{
			x--;
		}
		else if (psi[y][x] == DTW_Y)
		{
			y--;
		}
		else if (psi[y][x] == DTW_START)
		{
			break;
		}
		else
		{
			Melder_error("DTW_pathfinder: Path stops at (x, y) = (%d, %d).", x, y);
			goto end;
		}
		
		Melder_assert (pathIndex > 1 && y > 0);
		my path[--pathIndex].x = x;
		my path[pathIndex].y = y;
	}
	
	/* 
		If the real path length is shorter than the maximum path length 
		then shift the path to start at 1.
	*/
	my pathLength = my nx + my ny - pathIndex;
	if (pathIndex > 1)
	{
		for (x = 1; x <= my pathLength; x++)
		{
			my path[x] = my path[pathIndex++];
		}
	}

end:

	Melder_progress (1.0, NULL);
	NUMfmatrix_free (delta, 1, 1);
	NUMlmatrix_free (psi, 1, 1);
	return ! Melder_hasError();
}

int DTW_pathFinder_band (DTW me, double adjustment_window_duration, int adjustment_window_includes_end, 
	 double costs_x, double costs_y, double costs_xandy)
{
	return DTW_checkAdjustmentWindow (me, "DTW_pathFinder_band", adjustment_window_duration, adjustment_window_includes_end) &&
		_DTW_pathFinder (me, DTW_SAKOECHIBA, adjustment_window_duration, adjustment_window_includes_end,
			1, 0, costs_x, costs_y, costs_xandy);
}

int DTW_pathFinder_slopes (DTW me, long nsteps_xory, long nsteps_xandy, double costs_x, double costs_y, double costs_xandy)
{
	return DTW_checkSlopeConstraintParameters (me, "DTW_pathFinder_slopes", nsteps_xory, nsteps_xandy) &&
		_DTW_pathFinder (me, DTW_SLOPES, 0.0, 1, nsteps_xory, nsteps_xandy, costs_x, costs_y, costs_xandy);
}

void DTW_findPath (DTW me, int matchStart, int matchEnd, int slope)
{
	long pathIndex = my nx + my ny - 1; /* At maximum path length */
	long i, j, ipos = 0, **psi = NULL;
	float **delta = NULL, minimum;
	float slopeConstraint[5] = { DTW_BIG, DTW_BIG, 3, 2, 1.5 } ;
	float relDuration = (my ymax - my ymin) / (my xmax - my xmin);

	if (slope < 1 || slope > 4) Melder_error ("DTW_findPath: Invalid slope constraint.");
	
	if (relDuration < 1)
	{
		relDuration = 1 / relDuration;
	}
	 
	if (relDuration > slopeConstraint[slope])
	{
		Melder_warning ("DTW_findPath: There is a conflict between the chosen slope constraint and the relative  duration.\n "
		"The duration ratio of the longest and the shortest object is %.17g. This implies that the largest slope in the \n"
		"constraint must have a value greater or equal to this ratio.", relDuration);
	} 
	if (((delta = NUMfmatrix_copy (my z, 1, my ny, 1, my nx)) == NULL) ||
		((psi = NUMlmatrix (1, my ny, 1, my nx)) == NULL)) goto end;
	
	/*
		Forward pass.
	*/
	Melder_progress (0.0, "Find path");
	if (matchStart)
	{
		for (i = 2; i <= my ny; i++)
		{
			delta[i][1] = DTW_BIG;
			psi[i][1] = DTW_START;
		}
	}
	for (j = 2; j <= my nx; j++)
	{
		/*
			Given state (i2,j2) which can come from state (i1,j1)
			i2 = (i1 or i1+1), j2 = (j1 or j1-1)
			The matchStart gives slopes:
			1. j2 < i1 when i1 < my ny; 
			2. (i2,j2) cannot come from (i2-1,j2) when i1 < my ny
				(horizontal slope) 
		*/
		
		delta[1][j] = delta[1][j-1] + my z[1][j];
		psi[1][j] =  DTW_X;
		for (i = 2; i <= my ny; i++)
		{
			long direction = DTW_XANDY;
			float g;
			/* move along the diagonal */			
			minimum = delta[i-1][j-1] + 2 * my z[i][j];
			
			switch (slope) {
			case 1: /* no restriction */
s1:			{
				if ((g = delta[i][j-1] + my z[i][j]) < minimum)
				{
					minimum = g;
					direction = DTW_X;
				}
				if ((g = delta[i-1][j] + my z[i][j]) < minimum)
				{
					minimum = g;
					direction = DTW_Y;
				} 
			}
			break;
			
			/*
				P = 1/2
			*/
			
			case 2: /* P = 1/2 */
/*s2:*/		{
				if (i < 4 || j < 4) goto s1;
				
				if (psi[i][j-1] == DTW_X &&
					psi[i][j-2] == DTW_XANDY &&
					(g = delta[i-1][j-3] + 2 * my z[i][j-2] + my z[i][j-1] + 
						my z[i][j]) < minimum)
				{
					minimum = g;
					direction = DTW_X;
				}
				
				if (psi[i][j-1] == DTW_XANDY &&
					(g = delta[i-1][j-2] + 2 * my z[i][j-1] + my z[i][j]) <
						 minimum)
				{
					minimum = g;
					direction = DTW_X;
				}
				
				if (psi[i-1][j] == DTW_XANDY &&
					(g = delta[i-2][j-1] + 2 * my z[i-1][j] + my z[i][j]) <
					 minimum)
				{
					minimum = g;
					direction = DTW_Y;
				}
				
				if (psi[i-1][j] == DTW_Y &&
					psi[i-2][j] == DTW_XANDY &&
					(g = delta[i-3][j-1] + 2 * my z[i-2][j] + my z[i-1][j] + 
						my z[i][j]) < minimum)
				{
					minimum = g;
					direction = DTW_Y;
				}
			}
			break;
			
			/*
				P = 1
			*/
			
			case 3:
s3:			{
				if (i < 3 || j < 3) goto s1;
				
				if (psi[i][j-1] == DTW_XANDY &&
					(g = delta[i-1][j-2] + 2 * my z[i][j-1] + my z[i][j]) <
					 minimum)
				{
					minimum = g;
					direction = DTW_X;
				}
				
				if (psi[i-1][j] == DTW_XANDY && 
					(g = delta[i-2][j-1] + 2 * my z[i-1][j] + my z[i][j]) <
					 minimum)
				{
					minimum = g;
					direction = DTW_Y;
				}
			}
			break;
			
			/*
				P = 2
			*/
			
			case 4:
/*s4:*/		{
				if (j > 3 && i > 2)
				{
					if (psi[i][j-1] == DTW_XANDY &&
						psi[i-1][j-2] == DTW_XANDY &&
						(g = delta[i-2][j-3] + 2 * my z[i-1][j-2] + 
							2 * my z[i][j-1] + my z[i][j]) < minimum)
					{
						minimum = g;
						direction = DTW_X;
					}
					
					if (psi[i-1][j] == DTW_XANDY && 
						psi[i-2][j-1] == DTW_XANDY &&
						(g = delta[i-3][j-2] + 2 * my z[i-2][j-1] + 
							2 * my z[i-1][j] + my z[i][j]) < minimum)
					{
						minimum = g;
						direction = DTW_Y;
					}
				}
				else goto s3;
			}
			break;
			default:
			break;
			}
			
			psi[i][j] = direction;
			delta[i][j] = minimum;
		}
		if ((j % 10) == 2 && ! Melder_progress (0.999 * j / my nx,
			"Calculate time warp: frame %ld from %ld", j, my nx)) goto end;
	}
	
	/*
		Find minimum at end of path and trace back.
	*/
	
	minimum = delta[ipos = my ny][my nx];
	if (! matchEnd)
	{
		for (i = my ny - 1; i > 0; i--)
		{
			if (delta[i][my nx] < minimum) minimum = delta[ipos = i][my nx];
		}
	}
	
	my weightedDistance = minimum / ( my nx + my ny);
	my path[pathIndex].y = ipos;
	my path[pathIndex].x = j = my nx;
	
	/*
		Fill path backwards.
	*/
	 
	while (j > 1)
	{
		if (psi[ipos][j] == DTW_XANDY)
		{
			j--;
			ipos--;
		}
		else if (psi[ipos][j] == DTW_X)
		{
			j--;
		}
		else if (psi[ipos][j] == DTW_Y)
		{
			ipos--;
		}
		else
		{
			Melder_error("DTW_findPath: illegal path");
			goto end;
		}
		
		Melder_assert (pathIndex > 1 && ipos > 0);
		my path[--pathIndex].x = j;
		my path[pathIndex].y = ipos;
	}
	
	my pathLength = my nx + my ny - pathIndex;
	if (pathIndex > 1)
	{
		for (j = 1; j <= my pathLength; j++)
		{
			my path[j] = my path[pathIndex++];
		}
	}
	
end:

	Melder_progress (1.0, NULL);
	NUMfmatrix_free (delta, 1, 1);
	NUMlmatrix_free (psi, 1, 1);
}

double DTW_getPathY (DTW me, double tx)
{
	long ix, iy, i, ileft, iright, ibottom, itop, nxx, nyy;
	double ty0, ty, boxx, boxy;

	/* Assume linear behaviour outside domains 
	   Other option: scale with x_domain/y_domain
	*/
	
	if (tx < my xmin) return my ymin - (my xmin -tx);
	if (tx > my xmax) return my ymax + (tx - my xmax);
	
	/* Find column in DTW matrix */
	
	ix = (tx - my x1) / my dx + 1;
	if (ix < 1) ix = 1;
	if (ix > my nx) ix = my nx;
	
	/* Find index in the path and the row number (iy) (*/
	
	i = ix + my path[1].x - 1;
	while (i <= my pathLength && my path[i].x != ix) { i++;	}
	if (i > my pathLength) return NUMundefined; 
	iy = my path[i].y; /* row */
	
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

	/* Horizontal path? Find left and right positions. */
		
	ileft = i - 1;
	while (ileft >= 1 && my path[ileft].y == iy) { ileft--; }
	ileft++;
	if (ileft == 1 && ix > 1 && my path[ileft].y > 1) ileft++;
	
	iright = i + 1;
	while (iright <= my pathLength && my path[iright].y == iy) { iright++; }
	iright--;
	if (iright == my pathLength && ix < my nx && my path[iright].y < my ny) iright--;
	
	nxx = iright - ileft + 1;
	
	/* Vertical path? Only if not already horizontal. */
	
	ibottom = i;
	itop = i;
	
	if (nxx == 1)
	{
		ibottom--;
		while (ibottom >= 1 && my path[ibottom].x == ix) { ibottom--; }
		ibottom++;
		
		itop++;
		while (itop <= 1 && my path[itop].x == ix) { itop--; }
		itop++;
	}
		
	nyy = itop - ibottom + 1;
	
	boxx = nxx * my dx;
	boxy = nyy * my dy;
	
	/* Corrections at extreme left and right if path[1].x=1 && path[1].y>1 */
	
	if (ix == my nx)
	{
		boxx = my xmax -(my x1 + (ix - 1) * my dx - my dx / 2);
		boxy = my ymax -(my y1 + (iy - 1) * my dy - my dy / 2);
		ty = my ymax - (boxy - (boxx - (my xmax - tx)) * boxy / boxx); 
	}
	else if (ix == 1)
	{
		boxx = my x1 + my dx / 2 - my xmin;
		boxy = my y1 + (itop - 1) * my dy + my dy / 2 - my ymin;
		ty = (tx - my xmin) * boxy / boxx + my ymin;
	}
	else
	{
		/* Diagonal interpolation in a box with lower left (0,0) and upper right (nxx*dx, nyy*dy). */
		ty0 = (tx - (my x1 + (my path[ileft].x - 1) * my dx - my dx / 2)) * boxy / boxx;
		ty =  my y1 + (my path[ibottom].y - 1) * my dy - my dy / 2 + ty0;
	}
	return ty;
}

long DTW_getMaximumConsecutiveSteps (DTW me, int direction)
{
	long i, nglobal = 1, nlocal = 1;
	
	for (i = 2; i <= my pathLength; i++)
	{
		int localdirection;
		
		if (my path[i].y == my path[i-1].y)
		{
			localdirection = DTW_X;
		}
		else if (my path[i].x == my path[i-1].x)
		{
			localdirection = DTW_Y;
		}
		else
		{
			localdirection = DTW_XANDY;
		}
		
		if (localdirection == direction)
		{
			nlocal += 1;
		}
		if (direction != localdirection || i == my pathLength)
		{
			if (nlocal > nglobal) nglobal = nlocal;
			nlocal = 1;
		}
	}
	return nglobal;
}

void DTW_paintDistances (DTW me, Any g, double xmin, double xmax, double ymin,
	double ymax, double minimum, double maximum, int garnish)
{
	(void) garnish;
	Matrix_paintCells (me, g, xmin, xmax, ymin, ymax, minimum, maximum);
}

static Polygon _DTW_to_Polygon (DTW me, int choice, double adjustment_window_duration, int adjustment_window_includes_end, 
	long nsteps_xory, long nsteps_xandy)
{
	long x, ylow, yhigh;
	long numberOfPoints, iylow, iyhigh;
	double x1, y1, x2, y2;
	Polygon thee = NULL;
	
	numberOfPoints = 4 * my nx + 1;
	thee = Polygon_create (numberOfPoints);
	if (thee == NULL) return NULL;

	iyhigh = 0; iylow = numberOfPoints;
	for (x = 1; x <= my nx; x++)
	{
		if (! get_ylimits_x (me, choice, adjustment_window_duration, adjustment_window_includes_end, 
			nsteps_xory, nsteps_xandy, x, &ylow, &yhigh))
		{
			(void) Melder_error ("");
			goto end;
		}
	
		if (ylow > my ny) break;
		x1 = my x1 + (x - 1) * my dx - my dx / 2;
		x2 = x1 + my dx;
		y1 = my y1 + (yhigh - 1) * my dy + my dy / 2;
		y2 = my y1 +  (ylow - 1) * my dy - my dy / 2;
		
		thy x[++iyhigh] = x1;
		thy y[iyhigh] = y1;
		thy x[++iyhigh] = x2;
		thy y[iyhigh] = y1;

		thy x[--iylow] = x1;
		thy y[iylow] = y2;
		thy x[--iylow] = x2;
		thy y[iylow] = y2;
	}
	
	/* Connect end to begin */
	
	thy x[numberOfPoints] = thy x[1];
	thy y[numberOfPoints] = thy y[1];
		
end:

	if (Melder_hasError ()) forget (thee);
	return thee;
}

Polygon DTW_to_Polygon_band (DTW me, double adjustment_window_duration, int adjustment_window_includes_end)
{
	return DTW_checkAdjustmentWindow (me, "DTW_to_Polygon_band", adjustment_window_duration, adjustment_window_includes_end) ?
		_DTW_to_Polygon (me, DTW_SAKOECHIBA, adjustment_window_duration, adjustment_window_includes_end, 1, 0) : NULL;
}

Polygon DTW_to_Polygon_slopes (DTW me, long nsteps_xory, long nsteps_xandy)
{
	return DTW_checkSlopeConstraintParameters (me, "DTW_to_Polygon_slopes", nsteps_xory, nsteps_xandy) ? 
		_DTW_to_Polygon (me, DTW_SLOPES, 0.0, 1, nsteps_xory, nsteps_xandy) : NULL;
}

void DTW_drawPath (DTW me, Any g, double xmin, double xmax, double ymin,
	double ymax, int garnish)
{
	long i, ixmin, ixmax, iymin, iymax, ipmin = 1, ipmax;
	double x1, x2, y1, y2;
	
	if (xmin >= xmax)
	{
		xmin = my xmin; xmax = my xmax;
	}
	if (ymin >= ymax)
	{
		ymin = my ymin; ymax = my ymax;
	}
	
	if (! Matrix_getWindowSamplesY (me, ymin, ymax, &iymin, &iymax) ||
		! Matrix_getWindowSamplesX (me, xmin, xmax, &ixmin, &ixmax)) return;
	
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	
	while (ipmin < my pathLength && my path[ipmin].x < ixmin) ipmin++;
	
	ipmax = ipmin;
	
	while (ipmax < my pathLength && my path[ipmax].x <= ixmax) ipmax++;
	
	x1 = Matrix_columnToX (me, my path[ipmin].x);
	y1 = Matrix_rowToY (me, my path[ipmin].y);
	
	for (i = ipmin + 1; i <= ipmax; i++)
	{
		x2 = Matrix_columnToX (me, my path[i].x);
		y2 = Matrix_rowToY (me, my path[i].y);
		Graphics_line (g, x1, y1, x2, y2);
		x1 = x2;
		y1 = y2;
	}
	
	Graphics_unsetInner (g);
	if (garnish)
	{
		Graphics_drawInnerBox(g);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

Matrix DTW_distancesToMatrix (DTW me)
{
	Matrix thee;
	
	if ((thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
		my ymin, my ymax, my ny, my dy, my y1)) == NULL) return thee;

	NUMfmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
	return thee;
}

/* nog aanpassen, dl = sqrt (dx^2 + dy^2) */
void DTW_drawDistancesAlongPath (DTW me, Any g, double xmin, double xmax, 
	double dmin, double dmax, int garnish)
{
	long i, ixmax, ixmin;
	float *d = NULL;
	
	if (! (d = NUMfvector (1, my nx))) return;
	
	if (xmin >= xmax)
	{
		xmin = my xmin; xmax = my xmax;
	}
	if(	! Matrix_getWindowSamplesX (me, xmin, xmax, &ixmin, &ixmax)) return;
	
	i = 1;
	while (i < my pathLength && my path[i].x < ixmin) i++;
	ixmin = i;
	
	while (i <= my pathLength && my path[i].x < ixmax) i++;
	ixmax = i;
	
	if ((d = NUMfvector (ixmin, ixmax)) == NULL) return;
	
	for (i = ixmin; i <= ixmax; i++)
	{
		d[i] = my z[my path[i].y][i];
	}
	
	if (dmin >= dmax)
	{
		NUMfvector_extrema (d, ixmin, ixmax, &dmin, &dmax);
	}
	else
	{
		for (i = ixmin; i <= ixmax; i++)
		{
			if (d[i] > dmax)
			{
				d[i] = dmax;
			}
			else if (d[i] < dmin)
			{
				d[i] = dmin;
			}
		}
	}
	
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, dmin, dmax);
	Graphics_function (g, d, ixmin, ixmax, xmin, xmax);
	Graphics_unsetInner (g);
	
	if (garnish)
	{
		Graphics_drawInnerBox(g);
		Graphics_textLeft (g, 1, "distance");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
	
	NUMfvector_free (d, ixmin);
}

/*
	metric = 1...n (sum (a_i^n))^(1/n)
*/
DTW Matrices_to_DTW (I, thou, int matchStart, int matchEnd, int slope, int metric)
{
	iam (Matrix); thouart (Matrix);
	DTW him = NULL;
	long i, j, k;

	if (thy ny != my ny) return Melder_errorp ("Matrices_to_DTW: Columns must have the same dimensions.");

	him = DTW_create (my xmin, my xmax, my nx, my dx, my x1, thy xmin, thy xmax, thy nx, thy dx, thy x1);
	if (him == NULL) return NULL;
	
	for (i = 1; i <= my nx; i++)
	{
		for (j = 1; j <= thy nx; j++)
		{	/*
				First divide distance by maximum to prevent overflow when metric
				is a large number.
				d = (x^n)^(1/n) may overflow if x>1 & n >>1 even if d would not overflow!
			*/
			double dmax = 0, d = 0, dtmp;
			for (k = 1; k <= my ny; k++)
			{
				dtmp = fabs (my z[k][i] - thy z[k][j]);
				if (dtmp > dmax) dmax = dtmp;
			}
			if (dmax > 0)
			{
				for (k = 1; k <= my ny; k++)
				{
					dtmp = fabs (my z[k][i] - thy z[k][j]) / dmax;
					d +=  pow (dtmp, metric);
				}
			}
			d = dmax * pow (d, 1.0 / metric);
			his z[i][j] = d / my ny; /* == d * dy / ymax */
		}
		if ((i % 10) == 1 && ! Melder_progress (0.999 * i / my nx,
			 "Calculate distances: column %ld from %ld", i, my nx)) goto end;
	}
	DTW_findPath (him, matchStart, matchEnd, slope);
end:
	Melder_progress (1.0, NULL);
	if (Melder_hasError ()) forget (him);
	return him;
}

DTW Spectrograms_to_DTW (Spectrogram me, Spectrogram thee, int matchStart,
	int matchEnd, int slope, int metric)
{
	Matrix m1 = NULL, m2 = NULL;
	DTW him = NULL;
	long i, j;
	
	if (my xmin != thy xmin || my ymax != thy ymax || my ny != thy ny)
	{
		 return Melder_errorp("Spectrograms_to_DTW: #frequencies and/or "
		 	"frequency ranges do not match.");
	}
	if (((m1 = Spectrogram_to_Matrix (me)) == NULL) ||
		((m2 = Spectrogram_to_Matrix (thee)) == NULL) ||
		((him = DTW_create (my xmin, my xmax, my nx, my dx, my x1,
			thy xmin, thy xmax, thy nx, thy dx, thy x1)) == NULL)) goto end;
			
	/*
		Take log10 for dB's (4e-10 scaling not necessary)
	*/
	
	for (i = 1; i <= my ny; i++)
	{
		for (j = 1; j <= my nx; j++)
		{
			m1 -> z[i][j] = 10 * log10 (m1 -> z[i][j]);
		}
	}
	for (i = 1; i <= thy ny; i++)
	{
		for (j = 1; j <= thy nx; j++)
		{
			m2 -> z[i][j] = 10 * log10 (m2 -> z[i][j]);
		}
	}
	
	him = Matrices_to_DTW (m1, m2, matchStart, matchEnd, slope, metric);
	
end:
	forget (m1);
	forget (m2);
	if (Melder_hasError ()) forget (him);
	return him;
}

#define FREQUENCY(frame)  ((frame) -> candidate [1]. frequency)
#define NOT_VOICED(f)  ((f) <= 0.0 || (f) >= my ceiling)   /* This includes NUMundefined! */

static int Pitch_findFirstAndLastVoicedFrame (Pitch me, long *first, long *last)
{
	*first = 1;
	while (*first <= my nx && ! Pitch_isVoiced_i (me, *first)) (*first)++;
	*last = my nx;
	while (*last >= *first && ! Pitch_isVoiced_i (me, *last)) (*last)--;
	return *first <= my nx && *last >= 1;
}

DTW Pitches_to_DTW_sgc (Pitch me, Pitch thee, double vuv_costs, double time_weight, int matchStart, int matchEnd, int slope);
DTW Pitches_to_DTW_sgc (Pitch me, Pitch thee, double vuv_costs, double time_weight, int matchStart, int matchEnd, int slope) // vuv_costs=24, time_weight=10 ?
{
	DTW him = NULL;
	long i, j, myfirst, mylast, thyfirst, thylast;
	double *pitchx = NULL;
	int unit = Pitch_UNIT_SEMITONES_100;
 
 	if (vuv_costs < 0) return Melder_errorp ("Voiced-unvoiced costs may not be negative.");
 	if (time_weight < 0) return Melder_errorp ("Time costs weight may not be negative.");

	if (! Pitch_findFirstAndLastVoicedFrame (me, &myfirst, &mylast) ||
		! Pitch_findFirstAndLastVoicedFrame (thee, &thyfirst, &thylast)) return Melder_errorp 
			("No voiced frames.");
	/*
		We do not want the silences before the first voiced frame and after the last voiced frame 
		to determine the distances.
		We create paths from (1,1)...(thyfirst,myfirst) and (thylast,mylast)...(thy nx,my nx)
		by making the other cell's distances very large.
	*/
 	him = DTW_create (my xmin, my xmax, my nx, my dx, my x1, thy xmin, thy xmax, thy nx, thy dx, thy x1);
 	if (him == NULL) return NULL;
 	pitchx =  NUMdvector (1, thy nx);
 	if (pitchx == NULL) goto end;
 	for (j = 1; j <= thy nx; j++)
 	{
 		pitchx[j] = Sampled_getValueAtSample (thee, j, Pitch_LEVEL_FREQUENCY, unit);
 	}
 	
 	for (i = 1; i <= my nx; i++)
 	{
 		double pitchy = Sampled_getValueAtSample (me, i, Pitch_LEVEL_FREQUENCY, unit);
 		double t1 = my x1 + (i - 1) * my dx;
 		for (j = 1; j <= thy nx; j++)
 		{
 			double t2 = thy x1 + (j - 1) * thy dx;
 			double dist_f = 0; // based on pitch difference
 			double dist_t = fabs (t1 -t2);
 			if (pitchy == NUMundefined)
 			{
 				if (pitchx[j] != NUMundefined)
 				{
 					dist_f = vuv_costs;
 				}
 			}
 			else if (pitchx[j] == NUMundefined)
 			{
 				dist_f = vuv_costs;
 			}
 			else
 			{
 				dist_f = fabs(pitchy - pitchx[j]);
 			}
 			his z[i][j] = sqrt (dist_f * dist_f + time_weight * dist_t * dist_t);
 		}
 	}
 	DTW_findPath (him, matchStart, matchEnd, slope);
end:
	NUMdvector_free (pitchx, 1);
 	if (Melder_hasError ()) forget (him);
 	return him;
}

DTW Pitches_to_DTW (Pitch me, Pitch thee, double vuv_costs, double time_weight, int matchStart, int matchEnd, int slope) // vuv_costs=24, time_weight=10 ?
{
	DTW him = NULL;
	long i, j;
	double *pitchx = NULL;
	int unit = Pitch_UNIT_SEMITONES_100;
 
 	if (vuv_costs < 0) return Melder_errorp ("Voiced-unvoiced costs may not be negative.");
 	if (time_weight < 0) return Melder_errorp ("Time costs weight may not be negative.");

 	him = DTW_create (my xmin, my xmax, my nx, my dx, my x1, thy xmin, thy xmax, thy nx, thy dx, thy x1);
 	if (him == NULL) return NULL;
 	pitchx =  NUMdvector (1, thy nx);
 	if (pitchx == NULL) goto end;
 	for (j = 1; j <= thy nx; j++)
 	{
 		pitchx[j] = Sampled_getValueAtSample (thee, j, Pitch_LEVEL_FREQUENCY, unit);
 	}
 	
 	for (i = 1; i <= my nx; i++)
 	{
 		double pitchy = Sampled_getValueAtSample (me, i, Pitch_LEVEL_FREQUENCY, unit);
 		double t1 = my x1 + (i - 1) * my dx;
 		for (j = 1; j <= thy nx; j++)
 		{
 			double t2 = thy x1 + (j - 1) * thy dx;
 			double dist_f = 0; // based on pitch difference
 			double dist_t = fabs (t1 -t2);
 			if (pitchy == NUMundefined)
 			{
 				if (pitchx[j] != NUMundefined)
 				{
 					dist_f = vuv_costs;
 				}
 			}
 			else if (pitchx[j] == NUMundefined)
 			{
 				dist_f = vuv_costs;
 			}
 			else
 			{
 				dist_f = fabs(pitchy - pitchx[j]);
 			}
 			his z[i][j] = sqrt (dist_f * dist_f + time_weight * dist_t * dist_t);
 		}
 	}
 	DTW_findPath (him, matchStart, matchEnd, slope);
end:
	NUMdvector_free (pitchx, 1);
 	if (Melder_hasError ()) forget (him);
 	return him;
}

DurationTier DTW_to_DurationTier (DTW me)
{



}

/* End of file DTW.c */
