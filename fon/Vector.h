#ifndef _Vector_h_
#define _Vector_h_
/* Vector.h
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

/*
 * pb 2011/03/03
 */

/* Vector inherits from Matrix */
/* A Vector is horizontal Matrix. */
/* The rows are 'channels'. There will often be only one channel, but e.g. a stereo sound has two. */
#include "Matrix.h"

#ifdef __cplusplus
	extern "C" {
#endif

Thing_declare1cpp (Vector);

#define Vector_CHANNEL_AVERAGE  0
#define Vector_CHANNEL_1  1
#define Vector_CHANNEL_2  2
#define Vector_VALUE_INTERPOLATION_NEAREST  0
#define Vector_VALUE_INTERPOLATION_LINEAR  1
#define Vector_VALUE_INTERPOLATION_CUBIC  2
#define Vector_VALUE_INTERPOLATION_SINC70  3
#define Vector_VALUE_INTERPOLATION_SINC700  4
double Vector_getValueAtX (I, double x, long channel, int interpolation);

void Vector_getMinimumAndX (I, double xmin, double xmax, long channel, int interpolation,
	double *return_minimum, double *return_xOfMinimum);
void Vector_getMinimumAndXAndChannel (I, double xmin, double xmax, int interpolation,
	double *return_minimum, double *return_xOfMinimum, long *return_channelOfMinimum);
void Vector_getMaximumAndX (I, double xmin, double xmax, long channel, int interpolation,
	double *return_maximum, double *return_xOfMaximum);
void Vector_getMaximumAndXAndChannel (I, double xmin, double xmax, int interpolation,
	double *return_maximum, double *return_xOfMaximum, long *return_channelOfMaximum);
double Vector_getMinimum (I, double xmin, double xmax, int interpolation);
double Vector_getMaximum (I, double xmin, double xmax, int interpolation);
double Vector_getAbsoluteExtremum (I, double xmin, double xmax, int interpolation);
double Vector_getXOfMinimum (I, double xmin, double xmax, int interpolation);
double Vector_getXOfMaximum (I, double xmin, double xmax, int interpolation);
long Vector_getChannelOfMinimum (I, double xmin, double xmax, int interpolation);
long Vector_getChannelOfMaximum (I, double xmin, double xmax, int interpolation);

double Vector_getMean (I, double xmin, double xmax, long channel);
double Vector_getStandardDeviation (I, double xmin, double xmax, long channel);

void Vector_addScalar (I, double scalar);
void Vector_subtractMean (I);
void Vector_multiplyByScalar (I, double scalar);
void Vector_scale (I, double scale);

void Vector_draw (I, Graphics g, double *pxmin, double *pxmax, double *pymin, double *pymax,
	double defaultDy, const wchar_t *method);
/*
	If *pxmin equals *pxmax, then autowindowing from my xmin to my xmax.
	If *pymin equals *pymax, then autoscaling from minimum to maximum;
	if minimum then equals maximum, defaultDy will be subtracted from *pymin and added to *pymax;
	it must be a positive real number (e.g. 0.5 Pa for Sound, 1.0 dB for Ltas).
	method can be "curve", "bars", "poles", or "speckles"; it must not be NULL;
	if anything else is specified, a curve is drawn.
*/

#ifdef __cplusplus
	}

	struct structVector : public structMatrix {
	};
	#define Vector__methods(klas)  Matrix__methods(klas)
	Thing_declare2cpp (Vector, Matrix);

#endif

/* End of file Vector.h */
#endif
