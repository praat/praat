#ifndef _Vector_h_
#define _Vector_h_
/* Vector.h
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 * pb 2004/05/05
 */

/* Vector inherits from Matrix */
/* A Vector is a Matrix with a single row. */
#ifndef _Matrix_h_
	#include "Matrix.h"
#endif

#define Vector_members  Matrix_members
#define Vector_methods  Matrix_methods
class_create (Vector, Matrix)

double Vector_getValueAtX (I, double x, int interpolationDepth);

void Vector_getMinimumAndX (I, double xmin, double xmax, int interpolation,
	double *return_minimum, double *return_xOfMinimum);
void Vector_getMaximumAndX (I, double xmin, double xmax, int interpolation,
	double *return_minimum, double *return_xOfMinimum);
double Vector_getMinimum (I, double xmin, double xmax, int interpolation);
double Vector_getMaximum (I, double xmin, double xmax, int interpolation);
double Vector_getAbsoluteExtremum (I, double xmin, double xmax, int interpolation);
double Vector_getXOfMinimum (I, double xmin, double xmax, int interpolation);
double Vector_getXOfMaximum (I, double xmin, double xmax, int interpolation);

double Vector_getMean (I, double xmin, double xmax);
double Vector_getRootMeanSquare (I, double xmin, double xmax);
double Vector_getStandardDeviation (I, double xmin, double xmax);
double Vector_getEnergy (I, double xmin, double xmax);
double Vector_getPower (I, double xmin, double xmax);

void Vector_addScalar (I, double scalar);
void Vector_subtractMean (I);
void Vector_multiplyByScalar (I, double scalar);
void Vector_scale (I, double scale);

void Vector_draw (I, Graphics g, double *pxmin, double *pxmax, double *pymin, double *pymax,
	double defaultDy, const char *method);
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
