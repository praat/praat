/* Vector.c
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
 * pb 2002/07/16 GPL
 * pb 2004/10/24 Sampled statistics
 */

#include "Vector.h"

static double getVector (I, long icol) {
	iam (Vector);
	if (icol < 1 || icol > my nx) return 0.0;
	return my z [1] [icol];
}

static double getFunction1 (I, double x) {
	iam (Vector);
	double rcol = (x - my x1) / my dx + 1.0;
	long icol = floor (rcol);
	double dcol = rcol - icol;
	double z1 = icol < 1 || icol > my nx ? 0.0 : my z [1] [icol];
	double z2 = icol < 0 || icol >= my nx ? 0.0 : my z [1] [icol + 1];
	return (1.0 - dcol) * z1 + dcol * z2;
}

static double getValueAtSample (I, long isamp, long which, int units) {
	iam (Vector);
	(void) which;
	(void) units;
	return my z [1] [isamp];
}

class_methods (Vector, Matrix)
	class_method (getVector)
	class_method (getFunction1)
	us -> getMatrix = NULL;
	us -> getFunction2 = NULL;
	class_method (getValueAtSample)
class_methods_end

/***** Get content. *****/

double Vector_getValueAtX (I, double x, int interpolation) { iam (Vector);
	double leftEdge = my x1 - 0.5 * my dx, rightEdge = leftEdge + my nx * my dx;
	if (x <  leftEdge || x > rightEdge) return NUMundefined;
	return NUM_interpolate_sinc_f (my z [1], my nx, Sampled_xToIndex (me, x),
		interpolation == 3 ? 70 : interpolation == 4 ? 700 : interpolation);
}

/***** Get shape. *****/

void Vector_getMinimumAndX (I, double xmin, double xmax, int interpolation,
	double *return_minimum, double *return_xOfMinimum)
{
	iam (Vector);
	long imin, imax, i, n = my nx;
	float *y = my z [1];
	double minimum, x;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	if (! Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
		/*
		 * No samples between xmin and xmax.
		 * Try to return the lesser of the values at these two points.
		 */
		double yleft = Vector_getValueAtX (me, xmin, interpolation > 0 ? 1 : 0);
		double yright = Vector_getValueAtX (me, xmax, interpolation > 0 ? 1 : 0);
		minimum = yleft < yright ? yleft : yright;
		x = yleft == yright ? (xmin + xmax) / 2 : yleft < yright ? xmin : xmax;
	} else {
		minimum = y [imin], x = imin;
		if (y [imax] < minimum) minimum = y [imax], x = imax;
		if (imin == 1) imin ++;
		if (imax == my nx) imax --;
		for (i = imin; i <= imax; i ++) {
			if (y [i] < y [i - 1] && y [i] <= y [i + 1]) {
				double i_real, localMinimum = NUMimproveMinimum_f (y, n, i, interpolation, & i_real);
				if (localMinimum < minimum) minimum = localMinimum, x = i_real;
			}
		}
		x = my x1 + (x - 1) * my dx;   /* Convert sample to x. */
		if (x < xmin) x = xmin; else if (x > xmax) x = xmax;
	}
	if (return_minimum) *return_minimum = minimum;
	if (return_xOfMinimum) *return_xOfMinimum = x;
}
double Vector_getMinimum (I, double xmin, double xmax, int interpolation) { iam (Vector);
	double minimum;
	Vector_getMinimumAndX (me, xmin, xmax, interpolation, & minimum, NULL);
	return minimum;
}
double Vector_getXOfMinimum (I, double xmin, double xmax, int interpolation) { iam (Vector);
	double xOfMinimum;
	Vector_getMinimumAndX (me, xmin, xmax, interpolation, NULL, & xOfMinimum);
	return xOfMinimum;
}
void Vector_getMaximumAndX (I, double xmin, double xmax, int interpolation,
	double *return_maximum, double *return_xOfMaximum)
{
	iam (Vector);
	long imin, imax, i, n = my nx;
	float *y = my z [1];
	double maximum, x;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	if (! Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
		/*
		 * No samples between xmin and xmax.
		 * Try to return the greater of the values at these two points.
		 */
		double yleft = Vector_getValueAtX (me, xmin, interpolation > 0 ? 1 : 0);
		double yright = Vector_getValueAtX (me, xmax, interpolation > 0 ? 1 : 0);
		maximum = yleft > yright ? yleft : yright;
		x = yleft == yright ? (xmin + xmax) / 2 : yleft > yright ? xmin : xmax;
	} else {
		maximum = y [imin], x = imin;
		if (y [imax] > maximum) maximum = y [imax], x = imax;
		if (imin == 1) imin ++;
		if (imax == my nx) imax --;
		for (i = imin; i <= imax; i ++) {
			if (y [i] > y [i - 1] && y [i] >= y [i + 1]) {
				double i_real, localMaximum = NUMimproveMaximum_f (y, n, i, interpolation, & i_real);
				if (localMaximum > maximum) maximum = localMaximum, x = i_real;
			}
		}
		x = my x1 + (x - 1) * my dx;   /* Convert sample to x. */
		if (x < xmin) x = xmin; else if (x > xmax) x = xmax;
	}
	if (return_maximum) *return_maximum = maximum;
	if (return_xOfMaximum) *return_xOfMaximum = x;
}
double Vector_getMaximum (I, double xmin, double xmax, int interpolation) { iam (Vector);
	double maximum;
	Vector_getMaximumAndX (me, xmin, xmax, interpolation, & maximum, NULL);
	return maximum;
}
double Vector_getXOfMaximum (I, double xmin, double xmax, int interpolation) { iam (Vector);
	double xOfMaximum;
	Vector_getMaximumAndX (me, xmin, xmax, interpolation, NULL, & xOfMaximum);
	return xOfMaximum;
}
double Vector_getAbsoluteExtremum (I, double xmin, double xmax, int interpolation) { iam (Vector);
	double minimum = fabs (Vector_getMinimum (me, xmin, xmax, interpolation));
	double maximum = fabs (Vector_getMaximum (me, xmin, xmax, interpolation));
	return minimum > maximum ? minimum : maximum;
}

/***** Get statistics. *****/

double Vector_getMean (I, double xmin, double xmax) {
	iam (Vector);
	return Sampled_getMean (me, xmin, xmax, 0, 0, TRUE);
}

double Vector_getStandardDeviation (I, double xmin, double xmax) { iam (Vector);
	long imin, imax, i, n;
	double mean, sum2 = 0.0;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	n = Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax);
	if (n < 2) return NUMundefined;
	mean = Vector_getMean (me, xmin, xmax);
	for (i = imin; i <= imax; i ++) sum2 += (my z [1] [i] - mean) * (my z [1] [i] - mean);
	return sqrt (sum2 / (n - 1));
}

double Vector_getRootMeanSquare (I, double xmin, double xmax) { iam (Vector);
	long imin, imax, i, n;
	double sum2 = 0.0;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	n = Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax);
	if (n < 1) return NUMundefined;
	for (i = imin; i <= imax; i ++) sum2 += my z [1] [i] * my z [1] [i];
	return sqrt (sum2 / n);
}

double Vector_getEnergy (I, double xmin, double xmax) { iam (Vector);
	long imin, imax, i, n;
	double sum2 = 0.0;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	n = Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax);
	if (n < 1) return NUMundefined;
	for (i = imin; i <= imax; i ++) sum2 += my z [1] [i] * my z [1] [i];
	return sum2 * my dx;
}

double Vector_getPower (I, double xmin, double xmax) { iam (Vector);
	long imin, imax, i, n;
	double sum2 = 0.0;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	n = Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax);
	if (n < 1) return NUMundefined;
	for (i = imin; i <= imax; i ++) sum2 += my z [1] [i] * my z [1] [i];
	return sum2 / n;
}

/***** Modify. *****/

void Vector_addScalar (I, double scalar) { iam (Vector);
	float scalar_f = scalar, *amp = my z [1];
	long i, n = my nx;
	for (i = 1; i <= n; i ++) amp [i] += scalar_f;
}

void Vector_subtractMean (I) { iam (Vector);
	double mean = Vector_getMean (me, 0, 0);
	Vector_addScalar (me, - mean);
}

void Vector_multiplyByScalar (I, double scalar) { iam (Vector);
	float scalar_f = scalar, *amp = my z [1];
	long i, n = my nx;
	for (i = 1; i <= n; i ++) amp [i] *= scalar_f;
}

void Vector_scale (I, double scale) { iam (Vector);
	float *amp = my z [1];
	double extremum = 0.0;
	long i, n = my nx;
	for (i = 1; i <= n; i ++) if (fabs (amp [i]) > extremum) extremum = fabs (amp [i]);
	if (extremum != 0.0) {
		scale /= extremum;
		for (i = 1; i <= n; i ++) amp [i] *= scale;
	}
}

/***** Graphics. *****/

void Vector_draw (I, Graphics g, double *pxmin, double *pxmax, double *pymin, double *pymax,
	double defaultDy, const char *method)
{
	iam (Vector);
	long ixmin, ixmax, ix;
	/*
	 * Automatic domain.
	 */
	if (*pxmin == *pxmax) {
		*pxmin = my xmin;
		*pxmax = my xmax;
	}
	/*
	 * Domain expressed in sample numbers.
	 */
	Matrix_getWindowSamplesX (me, *pxmin, *pxmax, & ixmin, & ixmax);
	/*
	 * Automatic vertical range.
	 */
	if (*pymin == *pymax) {
		Matrix_getWindowExtrema (me, ixmin, ixmax, 1, 1, pymin, pymax);
		if (*pymin == *pymax) {
			*pymin -= defaultDy;
			*pymax += defaultDy;
		}
	}
	/*
	 * Set coordinates for drawing.
	 */
	Graphics_setInner (g);
	Graphics_setWindow (g, *pxmin, *pxmax, *pymin, *pymax);
	if (strstr (method, "bars")) {
		for (ix = ixmin; ix <= ixmax; ix ++) {
			double x = Sampled_indexToX (me, ix);
			double y = my z [1] [ix];
			double left = x - 0.5 * my dx, right = x + 0.5 * my dx;
			if (y > *pymax) y = *pymax;
			if (left < *pxmin) left = *pxmin;
			if (right > *pxmax) right = *pxmax;
			Graphics_line (g, left, y, right, y);
			Graphics_line (g, left, y, left, *pymin);
			Graphics_line (g, right, y, right, *pymin);
		}
	} else if (strstr (method, "poles")) {
		for (ix = ixmin; ix <= ixmax; ix ++) {
			double x = Sampled_indexToX (me, ix);
			Graphics_line (g, x, 0, x, my z [1] [ix]);
		}
	} else if (strstr (method, "speckles")) {
		for (ix = ixmin; ix <= ixmax; ix ++) {
			double x = Sampled_indexToX (me, ix);
			Graphics_fillCircle_mm (g, x, my z [1] [ix], 1.0);
		}
	} else {
		/*
		 * The default default: draw as a curve.
		 */
		Graphics_function (g, my z [1], ixmin, ixmax,
			Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax));
	}
	Graphics_unsetInner (g);
}
	
/* End of file Vector.c */
