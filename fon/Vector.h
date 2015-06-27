#ifndef _Vector_h_
#define _Vector_h_
/* Vector.h
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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

/* Vector inherits from Matrix */
/* A Vector is a horizontal Matrix. */
/* The rows are 'channels'. There will often be only one channel, but e.g. a stereo sound has two. */
#include "Matrix.h"

Thing_define (Vector, Matrix) {
	bool v_hasGetVector ()
		override { return true; }
	double v_getVector (long irow, long icol)
		override;
	bool v_hasGetFunction1 ()
		override { return true; }
	double v_getFunction1 (long irow, double x)
		override;
	bool v_hasGetMatrix ()
		override { return false; }
	bool v_hasGetFunction2 ()
		override { return false; }
	double v_getValueAtSample (long isamp, long ilevel, int unit)
		override;
};

#define Vector_CHANNEL_AVERAGE  0
#define Vector_CHANNEL_1  1
#define Vector_CHANNEL_2  2
#define Vector_VALUE_INTERPOLATION_NEAREST  0
#define Vector_VALUE_INTERPOLATION_LINEAR  1
#define Vector_VALUE_INTERPOLATION_CUBIC  2
#define Vector_VALUE_INTERPOLATION_SINC70  3
#define Vector_VALUE_INTERPOLATION_SINC700  4
double Vector_getValueAtX (Vector me, double x, long channel, int interpolation);

void Vector_getMinimumAndX (Vector me, double xmin, double xmax, long channel, int interpolation,
	double *return_minimum, double *return_xOfMinimum);
void Vector_getMinimumAndXAndChannel (Vector me, double xmin, double xmax, int interpolation,
	double *return_minimum, double *return_xOfMinimum, long *return_channelOfMinimum);
void Vector_getMaximumAndX (Vector me, double xmin, double xmax, long channel, int interpolation,
	double *return_maximum, double *return_xOfMaximum);
void Vector_getMaximumAndXAndChannel (Vector me, double xmin, double xmax, int interpolation,
	double *return_maximum, double *return_xOfMaximum, long *return_channelOfMaximum);
double Vector_getMinimum (Vector me, double xmin, double xmax, int interpolation);
double Vector_getMaximum (Vector me, double xmin, double xmax, int interpolation);
double Vector_getAbsoluteExtremum (Vector me, double xmin, double xmax, int interpolation);
double Vector_getXOfMinimum (Vector me, double xmin, double xmax, int interpolation);
double Vector_getXOfMaximum (Vector me, double xmin, double xmax, int interpolation);
long Vector_getChannelOfMinimum (Vector me, double xmin, double xmax, int interpolation);
long Vector_getChannelOfMaximum (Vector me, double xmin, double xmax, int interpolation);

double Vector_getMean (Vector me, double xmin, double xmax, long channel);
double Vector_getStandardDeviation (Vector me, double xmin, double xmax, long channel);

void Vector_addScalar (Vector me, double scalar);
void Vector_subtractMean (Vector me);
void Vector_multiplyByScalar (Vector me, double scalar);
void Vector_scale (Vector me, double scale);

void Vector_draw (Vector me, Graphics g, double *pxmin, double *pxmax, double *pymin, double *pymax,
	double defaultDy, const char32 *method);
/*
	If *pxmin equals *pxmax, then autowindowing from my xmin to my xmax.
	If *pymin equals *pymax, then autoscaling from minimum to maximum;
	if minimum then equals maximum, defaultDy will be subtracted from *pymin and added to *pymax;
	it must be a positive real number (e.g. 0.5 Pa for Sound, 1.0 dB for Ltas).
	method can be "curve", "bars", "poles", or "speckles"; it must not be NULL;
	if anything else is specified, a curve is drawn.
*/

/* End of file Vector.h */
#endif
