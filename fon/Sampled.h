#ifndef _Sampled_h_
#define _Sampled_h_
/* Sampled.h
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

/* Sampled inherits from Function */
#include "Function.h"
#include "Graphics.h"

#include "Sampled_def.h"
oo_CLASS_CREATE (Sampled, Function);

/* A Sampled is a Function that is sampled at nx points [1..nx], */
/* which are spaced apart by a constant distance dx. */
/* The first sample point is at x1, the second at x1 + dx, */
/* and the last at x1 + (nx - 1) * dx. */

double Sampled_indexToX (Sampled me, long i);

double Sampled_xToIndex (Sampled me, double x);

long Sampled_xToLowIndex (Sampled me, double x);

long Sampled_xToHighIndex (Sampled me, double x);

long Sampled_xToNearestIndex (Sampled me, double x);

long Sampled_getWindowSamples (Sampled me, double xmin, double xmax, long *ixmin, long *ixmax);

void Sampled_init (Sampled me, double xmin, double xmax, long nx, double dx, double x1);

void Sampled_shortTermAnalysis (Sampled me, double windowDuration, double timeStep,
		long *numberOfFrames, double *firstTime);
/*
	Function:
		how to put as many analysis windows of length 'windowDuration' as possible into my duration,
		when they are equally spaced by 'timeStep'.
	Input arguments:
		windowDuration:
			the duration of the analysis window, in seconds.
		timeStep:
			the time step, in seconds.
	Output arguments:
		numberOfFrames:
			at least 1 (if no failure); equals floor ((nx * dx - windowDuration) / timeStep) + 1.
		firstTime:
			the centre of the first frame, in seconds.
	Return value:
		1 = OK, 0 = failure.
	Failures:
		Window longer than signal.
	Postconditions:
		the frames are divided symmetrically over my defined domain,
		which is [x1 - dx/2, x[nx] + dx/2], where x[nx] == x1 + (nx - 1) * dx.
		All analysis windows will fit into this domain.
	Usage:
		the resulting Sampled (analysis sequence, e.g., Pitch, Formant, Spectrogram, etc.)
		will have the following attributes:
			result -> xmin == my xmin;   // Copy logical domain.
			result -> xmax == my xmax;
			result -> nx == numberOfFrames;
			result -> dx == timeStep;
			result -> x1 == firstTime;
*/

double Sampled_getValueAtSample (Sampled me, long isamp, long ilevel, int unit);
double Sampled_getValueAtX (Sampled me, double x, long ilevel, int unit, int interpolate);
long Sampled_countDefinedSamples (Sampled me, long ilevel, int unit);
double * Sampled_getSortedValues (Sampled me, long ilevel, int unit, long *numberOfValues);

double Sampled_getQuantile
	(Sampled me, double xmin, double xmax, double quantile, long ilevel, int unit);
double Sampled_getMean
	(Sampled me, double xmin, double xmax, long ilevel, int unit, int interpolate);
double Sampled_getMean_standardUnit
	(Sampled me, double xmin, double xmax, long ilevel, int averagingUnit, int interpolate);
double Sampled_getIntegral
	(Sampled me, double xmin, double xmax, long ilevel, int unit, int interpolate);
double Sampled_getIntegral_standardUnit
	(Sampled me, double xmin, double xmax, long ilevel, int averagingUnit, int interpolate);
double Sampled_getStandardDeviation
	(Sampled me, double xmin, double xmax, long ilevel, int unit, int interpolate);
double Sampled_getStandardDeviation_standardUnit
	(Sampled me, double xmin, double xmax, long ilevel, int averagingUnit, int interpolate);

void Sampled_getMinimumAndX (Sampled me, double xmin, double xmax, long ilevel, int unit, int interpolate,
	double *return_minimum, double *return_xOfMinimum);
double Sampled_getMinimum (Sampled me, double xmin, double xmax, long ilevel, int unit, int interpolate);
double Sampled_getXOfMinimum (Sampled me, double xmin, double xmax, long ilevel, int unit, int interpolate);
void Sampled_getMaximumAndX (Sampled me, double xmin, double xmax, long ilevel, int unit, int interpolate,
	double *return_maximum, double *return_xOfMaximum);
double Sampled_getMaximum (Sampled me, double xmin, double xmax, long ilevel, int unit, int interpolate);
double Sampled_getXOfMaximum (Sampled me, double xmin, double xmax, long ilevel, int unit, int interpolate);

void Sampled_drawInside
	(Sampled me, Graphics g, double xmin, double xmax, double ymin, double ymax, double speckle_mm, long ilevel, int unit);

/* End of file Sampled.h */
#endif
