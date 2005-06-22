/* Sampled.h
 *
 * Copyright (C) 1992-2005 Paul Boersma
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
 * pb 2005/06/16
 */

#ifndef _Sampled_h_
#define _Sampled_h_

/* Sampled inherits from Function */
#ifndef _Function_h_
	#include "Function.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif

#define Sampled_members Function_members \
	long nx; \
	double dx, x1;
#define Sampled_methods Function_methods \
	double (*getValueAtSample) (I, long isamp, long ilevel, int unit);
class_create (Sampled, Function)

/* A Sampled is a Function that is sampled at nx points [1..nx], */
/* which are spaced apart by a constant distance dx. */
/* The first sample point is at x1, the second at x1 + dx, */
/* and the last at x1 + (nx - 1) * dx. */

double Sampled_indexToX (I, long i);

double Sampled_xToIndex (I, double x);

long Sampled_xToLowIndex (I, double x);

long Sampled_xToHighIndex (I, double x);

long Sampled_xToNearestIndex (I, double x);

long Sampled_getWindowSamples (I, double xmin, double xmax, long *ixmin, long *ixmax);

int Sampled_init (I, double xmin, double xmax, long nx, double dx, double x1);

int Sampled_shortTermAnalysis (I, double windowDuration, double timeStep,
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

double Sampled_getValueAtSample (I, long isamp, long ilevel, int unit);
double Sampled_getValueAtX (I, double x, long ilevel, int unit, int interpolate);
long Sampled_countDefinedSamples (I, long ilevel, int unit);
double * Sampled_getSortedValues (I, long ilevel, int unit, long *numberOfValues);

double Sampled_getQuantile
	(I, double xmin, double xmax, double quantile, long ilevel, int unit);
double Sampled_getMean
	(I, double xmin, double xmax, long ilevel, int unit, int interpolate);
double Sampled_getMean_standardUnit
	(I, double xmin, double xmax, long ilevel, int averagingUnit, int interpolate);
double Sampled_getIntegral
	(I, double xmin, double xmax, long ilevel, int unit, int interpolate);
double Sampled_getIntegral_standardUnit
	(I, double xmin, double xmax, long ilevel, int averagingUnit, int interpolate);
double Sampled_getStandardDeviation
	(I, double xmin, double xmax, long ilevel, int unit, int interpolate);
double Sampled_getStandardDeviation_standardUnit
	(I, double xmin, double xmax, long ilevel, int averagingUnit, int interpolate);

void Sampled_getMinimumAndX (I, double xmin, double xmax, long ilevel, int unit, int interpolate,
	double *return_minimum, double *return_xOfMinimum);
double Sampled_getMinimum (I, double xmin, double xmax, long ilevel, int unit, int interpolate);
double Sampled_getXOfMinimum (I, double xmin, double xmax, long ilevel, int unit, int interpolate);
void Sampled_getMaximumAndX (I, double xmin, double xmax, long ilevel, int unit, int interpolate,
	double *return_maximum, double *return_xOfMaximum);
double Sampled_getMaximum (I, double xmin, double xmax, long ilevel, int unit, int interpolate);
double Sampled_getXOfMaximum (I, double xmin, double xmax, long ilevel, int unit, int interpolate);

void Sampled_drawInside
	(I, Graphics g, double xmin, double xmax, double ymin, double ymax, double speckle_mm, long ilevel, int unit);

#endif
/* End of file Sampled.h */
