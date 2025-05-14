#ifndef _Sampled_h_
#define _Sampled_h_
/* Sampled.h
 *
 * Copyright (C) 1992-2005,2007,2011,2013-2020,2024,2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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

/* Sampled inherits from Function */
#include "Function.h"
#include "Graphics.h"

#include "Sampled_def.h"

/* A Sampled is a Function that is sampled at nx points [1..nx], */
/* which are spaced apart by a constant distance dx. */
/* The first sample point is at x1, the second at x1 + dx, */
/* and the last at x1 + (nx - 1) * dx. */

template <typename T> static inline double Sampled_indexToX (constSampled me, T index) { return my x1 + (index - (T) 1) * my dx; }
static inline double Sampled_xToIndex (constSampled me, double x) { return (x - my x1) / my dx + 1.0; }
static inline integer Sampled_xToLowIndex     (constSampled me, double x) { return Melder_ifloor   ((x - my x1) / my dx + 1.0); }
static inline integer Sampled_xToHighIndex    (constSampled me, double x) { return Melder_iceiling ((x - my x1) / my dx + 1.0); }
static inline integer Sampled_xToNearestIndex (constSampled me, double x) { return Melder_iround   ((x - my x1) / my dx + 1.0); }

static inline autoVEC Sampled_listAllXValues (constSampled me) {
	autoVEC result = raw_VEC (my nx);
	for (integer i = 1; i <= my nx; i ++)
		result [i] = my x1 + (i - 1) * my dx;
	return result;
}

integer Sampled_getWindowSamples (constSampled me, double xmin, double xmax, integer *ixmin, integer *ixmax);

void Sampled_init (mutableSampled me, double xmin, double xmax, integer nx, double dx, double x1);

void Sampled_shortTermAnalysis (constSampled me, double windowDuration, double timeStep,
	integer *numberOfFrames, double *firstTime);
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

double Sampled_getValueAtSample (constSampled me, integer sampleNumber, integer levelNumber, int unit);
autoVEC Sampled_listValuesOfAllSamples (constSampled me, integer levelNumber, int unit);
double Sampled_getValueAtX (constSampled me, double x, integer levelNumber, int unit, bool interpolate);
autoVEC Sampled_listValuesAtXes (constSampled me, constVECVU const& xes, integer levelNumber, int unit, bool interpolate);

integer Sampled_countDefinedSamples
	(constSampled me, double xmin, double xmax, integer levelNumber, int unit);
autoVEC Sampled_getSortedValues
	(constSampled me, double xmin, double xmax, integer levelNumber, int unit);

double Sampled_getQuantile
	(constSampled me, double xmin, double xmax, double quantile, integer levelNumber, int unit);
double Sampled_getMean
	(constSampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate);
double Sampled_getMeanOfSquared
	(constSampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate);
double Sampled_getMean_standardUnit
	(constSampled me, double xmin, double xmax, integer levelNumber, int averagingUnit, bool interpolate);
double Sampled_getMeanOfSquared_standardUnit
	(constSampled me, double xmin, double xmax, integer levelNumber, int averagingUnit, bool interpolate);
double Sampled_getIntegral
	(constSampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate);
double Sampled_getIntegralOfSquared
	(constSampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate);
double Sampled_getIntegral_standardUnit
	(constSampled me, double xmin, double xmax, integer levelNumber, int averagingUnit, bool interpolate);
double Sampled_getIntegralOfSquared_standardUnit
	(constSampled me, double xmin, double xmax, integer levelNumber, int averagingUnit, bool interpolate);
double Sampled_getStandardDeviation
	(constSampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate);
double Sampled_getStandardDeviation_standardUnit
	(constSampled me, double xmin, double xmax, integer levelNumber, int averagingUnit, bool interpolate);

void Sampled_getMinimumAndX
	(constSampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate,
	 double *return_minimum, double *return_xOfMinimum);
double Sampled_getMinimum
	(constSampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate);
double Sampled_getXOfMinimum
	(constSampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate);
void Sampled_getMaximumAndX
	(constSampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate,
	 double *return_maximum, double *return_xOfMaximum);
double Sampled_getMaximum
	(constSampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate);
double Sampled_getXOfMaximum
	(constSampled me, double xmin, double xmax, integer levelNumber, int unit, bool interpolate);

void Sampled_drawInside
	(constSampled me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool speckle, integer levelNumber, int unit);

/* End of file Sampled.h */
#endif
