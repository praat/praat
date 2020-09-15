#ifndef _Vector_h_
#define _Vector_h_
/* Vector.h
 *
 * Copyright (C) 1992-2005,2007,2011,2012,2015-2018,2020 Paul Boersma
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

/* Vector inherits from Matrix */
/* A Vector is a horizontal Matrix. */
/* The rows are 'channels'. There will often be only one channel, but e.g. a stereo sound has two. */
#include "Matrix.h"

Thing_define (Vector, Matrix) {
	bool v_hasGetVector ()
		override { return true; }
	double v_getVector (integer irow, integer icol)
		override;
	bool v_hasGetFunction1 ()
		override { return true; }
	double v_getFunction1 (integer irow, double x)
		override;
	bool v_hasGetMatrix ()
		override { return false; }
	bool v_hasGetFunction2 ()
		override { return false; }
	double v_getValueAtSample (integer isamp, integer ilevel, int unit)
		override;

	VEC channel (integer channelNumber) { return z.row (channelNumber); }
};

#include "Vector_enums.h"
integer kVector_valueInterpolation_to_interpolationDepth (kVector_valueInterpolation valueInterpolationType);
integer kVector_peakInterpolation_to_interpolationDepth (kVector_peakInterpolation peakInterpolationType);

#define Vector_CHANNEL_AVERAGE  0
#define Vector_CHANNEL_1  1
#define Vector_CHANNEL_2  2
double Vector_getValueAtX (Vector me, double x, integer channel, kVector_valueInterpolation valueInterpolationType);

void Vector_getMinimumAndX (Vector me, double xmin, double xmax, integer channel, kVector_peakInterpolation peakInterpolationType,
	double *return_minimum, double *return_xOfMinimum);
void Vector_getMinimumAndXAndChannel (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType,
	double *return_minimum, double *return_xOfMinimum, integer *return_channelOfMinimum);
void Vector_getMaximumAndX (Vector me, double xmin, double xmax, integer channel, kVector_peakInterpolation peakInterpolationType,
	double *return_maximum, double *return_xOfMaximum);
void Vector_getMaximumAndXAndChannel (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType,
	double *return_maximum, double *return_xOfMaximum, integer *return_channelOfMaximum);
double Vector_getMinimum (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType);
double Vector_getMaximum (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType);
double Vector_getAbsoluteExtremum (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType);
double Vector_getXOfMinimum (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType);
double Vector_getXOfMaximum (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType);
integer Vector_getChannelOfMinimum (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType);
integer Vector_getChannelOfMaximum (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType);

double Vector_getMean (Vector me, double xmin, double xmax, integer channel);
double Vector_getStandardDeviation (Vector me, double xmin, double xmax, integer channel);

void Vector_addScalar (Vector me, double scalar);
void Vector_subtractMean (Vector me);
void Vector_multiplyByScalar (Vector me, double scalar);
void Vector_scale (Vector me, double scale);

void Vector_draw (Vector me, Graphics g, double *pxmin, double *pxmax, double *pymin, double *pymax,
	double defaultDy, conststring32 method);
/*
	If *pxmin equals *pxmax, then autowindowing from my xmin to my xmax.
	If *pymin equals *pymax, then autoscaling from minimum to maximum;
	if minimum then equals maximum, defaultDy will be subtracted from *pymin and added to *pymax;
	it must be a positive real number (e.g. 0.5 Pa for Sound, 1.0 dB for Ltas).
	method can be "curve", "bars", "poles", or "speckles"; it must not be null;
	if anything else is specified, a curve is drawn.
*/

/* End of file Vector.h */
#endif
