/* DTW.c
 *
 * Copyright (C) 1993-2002 David Weenink
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

static void info (I)
{
	iam (DTW);
	Melder_info ("DTW info\nName: %s\n"
		"Domain prototype: from %.8g to %.8g\n"
		"Domain candidate: from %.8g to %.8g\n"
		"Number of frames prototype: %ld\n"
		"Number of frames candidate: %ld\n"
		"Path length %ld (frames)\n"
		"Global warped distance: %.8g\n",
		Thing_getName (me), my ymin, my ymax, my xmin, my xmax, my ny, my nx,
		my pathLength, my weightedDistance);
	if (my nx == my ny)
	{
		double dd = 0; long i;
		for (i=1; i <= my nx; i++) dd += my z[i][i];
		Melder_info ("\n\nDistance along diagonal: %.8g\n", dd / my nx);
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
 
	         
#define DTW_DIAGONAL 0
#define DTW_HORIZONTAL 1
#define DTW_VERTICAL -1
#define DTW_BIG 1e38
#define DTW_NODIRECTION -2

void DTW_findPath (DTW me, int matchStart, int matchEnd, int slope)
{
	long pathIndex = my nx + my ny - 1, i, j, ipos = 0, **psi = NULL;
	float **delta = NULL, minimum;
	float slopeConstraint[5] = { DTW_BIG, DTW_BIG, 3, 2, 1.5 } ;
	float relDuration = (my ymax - my ymin) / (my xmax - my xmin);

	Melder_assert (slope > 0 && slope < 5);
	
	if (relDuration < 1)
	{
		relDuration = 1 / relDuration;
	}
	 
	if (relDuration > slopeConstraint[slope])
	{
		Melder_warning ("DTW_findPath: The relative duration and the "
			"maximum allowed slope are in conflict.");
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
			psi[i][1] = DTW_NODIRECTION;
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
		psi[1][j] =  DTW_HORIZONTAL;
		for (i = 2; i <= my ny; i++)
		{
			long direction = DTW_DIAGONAL;
			float g;
			/* move along the diagonal */			
			minimum = delta[i-1][j-1] + 2 * my z[i][j];
			
			switch (slope) {
			case 1:
s1:			{
				if ((g = delta[i][j-1] + my z[i][j]) < minimum)
				{
					minimum = g;
					direction = DTW_HORIZONTAL;
				}
				if ((g = delta[i-1][j] + my z[i][j]) < minimum)
				{
					minimum = g;
					direction = DTW_VERTICAL;
				} 
			}
			break;
			
			/*
				P = 1/2
			*/
			
			case 2: /* P = 1/2 */
/*s2:*/		{
				if (i < 4 || j < 4) goto s1;
				
				if (psi[i][j-1] == DTW_HORIZONTAL &&
					psi[i][j-2] == DTW_DIAGONAL &&
					(g = delta[i-1][j-3] + 2 * my z[i][j-2] + my z[i][j-1] + 
						my z[i][j]) < minimum)
				{
					minimum = g;
					direction = DTW_HORIZONTAL;
				}
				
				if (psi[i][j-1] == DTW_DIAGONAL &&
					(g = delta[i-1][j-2] + 2 * my z[i][j-1] + my z[i][j]) <
						 minimum)
				{
					minimum = g;
					direction = DTW_HORIZONTAL;
				}
				
				if (psi[i-1][j] == DTW_DIAGONAL &&
					(g = delta[i-2][j-1] + 2 * my z[i-1][j] + my z[i][j]) <
					 minimum)
				{
					minimum = g;
					direction = DTW_VERTICAL;
				}
				
				if (psi[i-1][j] == DTW_VERTICAL &&
					psi[i-2][j] == DTW_DIAGONAL &&
					(g = delta[i-3][j-1] + 2 * my z[i-2][j] + my z[i-1][j] + 
						my z[i][j]) < minimum)
				{
					minimum = g;
					direction = DTW_VERTICAL;
				}
			}
			break;
			
			/*
				P = 1
			*/
			
			case 3:
s3:			{
				if (i < 3 || j < 3) goto s1;
				
				if (psi[i][j-1] == DTW_DIAGONAL &&
					(g = delta[i-1][j-2] + 2 * my z[i][j-1] + my z[i][j]) <
					 minimum)
				{
					minimum = g;
					direction = DTW_HORIZONTAL;
				}
				
				if (psi[i-1][j] == DTW_DIAGONAL && 
					(g = delta[i-2][j-1] + 2 * my z[i-1][j] + my z[i][j]) <
					 minimum)
				{
					minimum = g;
					direction = DTW_VERTICAL;
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
					if (psi[i][j-1] == DTW_DIAGONAL &&
						psi[i-1][j-2] == DTW_DIAGONAL &&
						(g = delta[i-2][j-3] + 2 * my z[i-1][j-2] + 
							2 * my z[i][j-1] + my z[i][j]) < minimum)
					{
						minimum = g;
						direction = DTW_HORIZONTAL;
					}
					
					if (psi[i-1][j] == DTW_DIAGONAL && 
						psi[i-2][j-1] == DTW_DIAGONAL &&
						(g = delta[i-3][j-2] + 2 * my z[i-2][j-1] + 
							2 * my z[i-1][j] + my z[i][j]) < minimum)
					{
						minimum = g;
						direction = DTW_VERTICAL;
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
		if (psi[ipos][j] == DTW_DIAGONAL)
		{
			j--;
			ipos--;
		}
		else if (psi[ipos][j] == DTW_HORIZONTAL)
		{
			j--;
		}
		else if (psi[ipos][j] == DTW_VERTICAL)
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

double DTW_getPathY (DTW me, double t, int lowest)
{
	long col, i, ifrom = 0, ito, row;

	if (t < my xmin || t > my xmax) return t;
	
	col = Matrix_xToNearestColumn (me, t);

	/*
		Enforce that when xmin <= t <= xmax,
		the output will be in [ymin, ymax]
	*/

	if (col > my nx) col = my nx;
	if (col < 1) col = 1;

	for (i=1; i <= my pathLength; i++)
	{
		if (my path[i].x == col)
		{
			if (ifrom == 0) ifrom = i;
			ito = i;
		}
		else if (ifrom != 0) break;
	}
	if (ifrom == 0) return t;
	row = my path[lowest ? ifrom : ito].y;
	return Matrix_rowToY (me, row);
}

void DTW_paintDistances (DTW me, Any g, double xmin, double xmax, double ymin,
	double ymax, double minimum, double maximum, int garnish)
{
	(void) garnish;
	Matrix_paintCells (me, g, xmin, xmax, ymin, ymax, minimum, maximum);
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

DTW Spectrograms_to_DTW (Spectrogram me, Spectrogram thee, int matchStart,
	int matchEnd, int slope)
{
	Matrix m1 = NULL, m2 = NULL;
	DTW him = NULL;
	long i, j, k;
	
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
	
	Melder_progress (0.0, "");
	
	for (i = 1; i <= my nx; i++)
	{
		for (j = 1; j <= thy nx; j++)
		{
			double d = 0;
			for (k = 1; k <= my ny; k++)
			{
				d += fabs (m1 -> z[k][i] - m2 -> z[k][j]);
			}
			his z[i][j] = d / my ny; /* == d * dy / ymax */
		}
		if ((i % 10) == 1 && ! Melder_progress (0.999 * i / my nx,
			 "Calculate distances: column %ld from %ld", i, my nx)) goto end;
	}
	Melder_progress (1.0, NULL);
	DTW_findPath (him, matchStart, matchEnd, slope);
	
end:

	Melder_progress (1.0, NULL);
	forget (m1); forget (m2);
	if (Melder_hasError ()) forget (him);
	return him;
}
 
/* End of file DTW.c */
