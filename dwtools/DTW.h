#ifndef _DTW_h_
#define _DTW_h_
/* DTW.h
 *
 * Copyright (C) 1993-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20020813 GPL header
 djmw 20110306 Latest modification.
*/

#include "Spectrogram.h"
#include "Polygon_extensions.h"
#include "Pitch.h"
#include "DurationTier.h"
#include "Sound.h"

#include "DTW_def.h"

#define DTW_SAKOECHIBA 1
#define DTW_SLOPES 2

#define DTW_UNREACHABLE -1
#define DTW_FORBIDDEN -2
#define DTW_START 3
#define DTW_XANDY 2
#define DTW_X 4
#define DTW_Y 6

void DTW_Path_Query_init (DTW_Path_Query me, integer ny, integer nx);

/* Prototype on y-axis and test on x-axis */
autoDTW DTW_create (double tminp, double tmaxp, integer ntp, double dtp, double t1p,
	double tminc, double tmaxc, integer ntc, double dtc, double t1c);

void DTW_setWeights (DTW me, double wx, double wy, double wd);

autoDTW DTW_swapAxes (DTW me);

void DTW_findPath_bandAndSlope (DTW me, double sakoeChibaBand, int localSlope, autoMatrix *cumulativeDists);

void DTW_findPath (DTW me, bool matchStart, bool matchEnd, int slope); // deprecated
/* Obsolete
	Function:
		Calculate the minimum path (through a distance matrix).

	Possible constraints:

	matchStart:	path starts at position (1,1) (bottom-left)
	matchEnd:	path ends at (I,J) (top-right)
	slope:	1	no constraints
			2	1/3 < slope < 3
			3	1/2 < slope < 2
			4	2/3 < slope < 3/2

	For algorithm and DP-equations see the article (& especially table I) in:
	Sakoe, H. & Chiba, S. (1978), Dynamic Programming Algorithm Optimization
		for Spoken Word	recognition, IEEE Trans. on ASSP, vol 26, 43-49.
*/

void DTW_Path_recode (DTW me);

double DTW_getYTimeFromXTime (DTW me, double tx);

double DTW_getXTimeFromYTime (DTW me, double ty);

double DTW_getPathY (DTW me, double tx);

integer DTW_getMaximumConsecutiveSteps (DTW me, int direction);

void DTW_paintDistances (DTW me, Graphics g, double xmin, double xmax, double ymin,
	double ymax, double minimum, double maximum, bool garnish);

void DTW_drawPath (DTW me, Graphics g, double xmin, double xmax, double ymin,
	double ymax, bool garnish);

void DTW_drawWarpX (DTW me, Graphics g, double xmin, double xmax, double ymin, double ymax, double tx, bool garnish);
void DTW_drawWarpY (DTW me, Graphics g, double xmin, double xmax, double ymin, double ymax, double ty, bool garnish);

void DTW_pathRemoveRedundantNodes (DTW me);

void DTW_pathQueryRecode (DTW me);

void DTW_drawDistancesAlongPath (DTW me, Graphics g, double xmin, double xmax, double dmin, double dmax, bool garnish);

void DTW_Sounds_draw (DTW me, Sound yy, Sound xx, Graphics g, double xmin, double xmax,
	double ymin, double ymax, bool garnish);

void DTW_Sounds_drawWarpX (DTW me, Sound yy, Sound xx, Graphics g, double xmin, double xmax,
	double ymin, double ymax, double tx, bool garnish);

autoPolygon DTW_to_Polygon (DTW me, double band, int slope);

void DTW_Polygon_findPathInside (DTW me, Polygon thee, int localSlope, autoMatrix *cumulativeDists);

autoMatrix DTW_to_Matrix_distances (DTW me);

autoMatrix DTW_to_Matrix_cumulativeDistances (DTW me, double sakoeChibaBand, int slope);

autoMatrix DTW_Polygon_to_Matrix_cumulativeDistances (DTW me, Polygon thee, int localSlope);

autoDTW Matrices_to_DTW (Matrix me, Matrix thee, bool matchStart, bool matchEnd, int slope, double metric);

autoDTW Spectrograms_to_DTW (Spectrogram me, Spectrogram thee, bool matchStart, bool matchEnd, int slope, double metric);

autoDTW Pitches_to_DTW (Pitch me, Pitch thee, double vuv_costs, double time_weight, bool matchStart, bool matchEnd, int slope);

autoDurationTier DTW_to_DurationTier (DTW me);

void DTW_Matrix_replace (DTW me, Matrix thee);

#endif /* _DTW_h_ */
