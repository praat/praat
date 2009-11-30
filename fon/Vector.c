/* Vector.c
 *
 * Copyright (C) 1992-2009 Paul Boersma
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
 * pb 2005/06/16 units
 * pb 2006/10/28 Vector_draw case-insensitive
 * pb 2007/01/27 multiple channels
 * pb 2007/01/28 new getVector and getFunction1
 * pb 2008/01/19 double
 * pb 2009/11/23 support for drawing with reversed axes
 */

#include "Vector.h"

//
// Vector::getVector () returns a channel or the average of all the channels.
//
static double getVector (I, long irow, long icol) {
	iam (Vector);
	if (icol < 1 || icol > my nx) return 0.0;
	if (my ny == 1) return my z [1] [icol];   // Optimization.
	if (irow == 0) {
		if (my ny == 2) return 0.5 * (my z [1] [icol] + my z [2] [icol]);   // Optimization.
		double sum = 0.0;
		for (long channel = 1; channel <= my ny; channel ++) {
			sum += my z [channel] [icol];
		}
		return sum / my ny;
	}
	Melder_assert (irow > 0 && irow <= my ny);
	return my z [irow] [icol];
}

//
// Vector::getFunction1 () returns a channel or the average of all the channels.
//
static double getFunction1 (I, long irow, double x) {
	iam (Vector);
	double rcol = (x - my x1) / my dx + 1.0;
	long icol = floor (rcol);
	double dcol = rcol - icol;
	double z1;
	if (icol < 1 || icol > my nx) {
		z1 = 0.0;   // Outside the definition region, Formulas is expected to return zero.
	} else if (my ny == 1) {
		z1 = my z [1] [icol];   // Optimization.
	} else if (irow == 0) {
		if (my ny == 2) {
			z1 = 0.5 * (my z [1] [icol] + my z [2] [icol]);   // Optimization.
		} else {
			double sum = 0.0;
			for (long channel = 1; channel <= my ny; channel ++) {
				sum += my z [channel] [icol];
			}
			z1 = sum / my ny;
		}
	} else {
		Melder_assert (irow > 0 && irow <= my ny);
		z1 = my z [irow] [icol];
	}
	double z2;
	if (icol < 0 || icol >= my nx) {
		z2 = 0.0;   // Outside the definition region, Formulas is expected to return zero.
	} else if (my ny == 1) {
		z2 = my z [1] [icol + 1];   // Optimization.
	} else if (irow == 0) {
		if (my ny == 2) {
			z2 = 0.5 * (my z [1] [icol + 1] + my z [2] [icol + 1]);   // Optimization.
		} else {
			double sum = 0.0;
			for (long channel = 1; channel <= my ny; channel ++) {
				sum += my z [channel] [icol + 1];
			}
			z2 = sum / my ny;
		}
	} else {
		Melder_assert (irow > 0 && irow <= my ny);
		z2 = my z [irow] [icol + 1];
	}
	return (1.0 - dcol) * z1 + dcol * z2;
}

static double getValueAtSample (I, long isamp, long ilevel, int unit) {
// Preconditions:
//    1 <= isamp <= my nx
//    0 <= ilevel <= my ny
	iam (Vector);
	double value;
	if (ilevel > Vector_CHANNEL_AVERAGE) {
		value = my z [ilevel] [isamp];
	} else if (my ny == 1) {
		value = my z [1] [isamp];   // Optimization.
	} else if (my ny == 2) {
		value = 0.5 * (my z [1] [isamp] + my z [2] [isamp]);   // Optimization.
	} else {
		double sum = 0.0;
		for (long channel = 1; channel <= my ny; channel ++) {
			sum += my z [channel] [isamp];
		}
		value = sum / my ny;
	}
	return NUMdefined (value) ? our convertStandardToSpecialUnit (me, value, ilevel, unit) : NUMundefined;
}

class_methods (Vector, Matrix)
	class_method (getVector)
	class_method (getFunction1)
	us -> getMatrix = NULL;
	us -> getFunction2 = NULL;
	class_method (getValueAtSample)
class_methods_end

/***** Get content. *****/

//
// Vector_getValueAtX () returns the average of all the interpolated channels.
//
double Vector_getValueAtX (I, double x, long ilevel, int interpolation) {
	iam (Vector);
	double leftEdge = my x1 - 0.5 * my dx, rightEdge = leftEdge + my nx * my dx;
	if (x <  leftEdge || x > rightEdge) return NUMundefined;
	if (ilevel > Vector_CHANNEL_AVERAGE) {
		Melder_assert (ilevel <= my ny);
		return NUM_interpolate_sinc (my z [ilevel], my nx, Sampled_xToIndex (me, x),
			interpolation == Vector_VALUE_INTERPOLATION_SINC70 ? NUM_VALUE_INTERPOLATE_SINC70 :
			interpolation == Vector_VALUE_INTERPOLATION_SINC700 ? NUM_VALUE_INTERPOLATE_SINC700 :
			interpolation);
	}
	double sum = 0.0;
	for (long channel = 1; channel <= my ny; channel ++) {
		sum += NUM_interpolate_sinc (my z [channel], my nx, Sampled_xToIndex (me, x),
			interpolation == Vector_VALUE_INTERPOLATION_SINC70 ? NUM_VALUE_INTERPOLATE_SINC70 :
			interpolation == Vector_VALUE_INTERPOLATION_SINC700 ? NUM_VALUE_INTERPOLATE_SINC700 :
			interpolation);
	}
	return sum / my ny;
}

/***** Get shape. *****/

void Vector_getMinimumAndX (I, double xmin, double xmax, long channel, int interpolation,
	double *return_minimum, double *return_xOfMinimum)
{
	iam (Vector);
	long imin, imax, n = my nx;
	Melder_assert (channel >= 1 && channel <= my ny);
	double *y = my z [channel];
	double minimum, x;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	if (! Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
		/*
		 * No samples between xmin and xmax.
		 * Try to return the lesser of the values at these two points.
		 */
		double yleft = Vector_getValueAtX (me, xmin, channel,
			interpolation > Vector_VALUE_INTERPOLATION_NEAREST ? Vector_VALUE_INTERPOLATION_LINEAR : Vector_VALUE_INTERPOLATION_NEAREST);
		double yright = Vector_getValueAtX (me, xmax, channel,
			interpolation > Vector_VALUE_INTERPOLATION_NEAREST ? Vector_VALUE_INTERPOLATION_LINEAR : Vector_VALUE_INTERPOLATION_NEAREST);
		minimum = yleft < yright ? yleft : yright;
		x = yleft == yright ? (xmin + xmax) / 2 : yleft < yright ? xmin : xmax;
	} else {
		minimum = y [imin], x = imin;
		if (y [imax] < minimum) minimum = y [imax], x = imax;
		if (imin == 1) imin ++;
		if (imax == my nx) imax --;
		for (long i = imin; i <= imax; i ++) {
			if (y [i] < y [i - 1] && y [i] <= y [i + 1]) {
				double i_real, localMinimum = NUMimproveMinimum (y, n, i, interpolation, & i_real);
				if (localMinimum < minimum) minimum = localMinimum, x = i_real;
			}
		}
		x = my x1 + (x - 1) * my dx;   /* Convert sample to x. */
		if (x < xmin) x = xmin; else if (x > xmax) x = xmax;
	}
	if (return_minimum) *return_minimum = minimum;
	if (return_xOfMinimum) *return_xOfMinimum = x;
}

void Vector_getMinimumAndXAndChannel (I, double xmin, double xmax, int interpolation,
	double *return_minimum, double *return_xOfMinimum, long *return_channelOfMinimum)
{
	iam (Vector);
	double minimum, xOfMinimum;
	long channelOfMinimum = 1;
	Vector_getMinimumAndX (me, xmin, xmax, 1, interpolation, & minimum, & xOfMinimum);
	for (long channel = 2; channel <= my ny; channel ++) {
		double minimumOfChannel, xOfMinimumOfChannel;
		Vector_getMinimumAndX (me, xmin, xmax, channel, interpolation, & minimumOfChannel, & xOfMinimumOfChannel);
		if (minimumOfChannel < minimum) {
			minimum = minimumOfChannel;
			xOfMinimum = xOfMinimumOfChannel;
			channelOfMinimum = channel;
		}
	}
	if (return_minimum) *return_minimum = minimum;
	if (return_xOfMinimum) *return_xOfMinimum = xOfMinimum;
	if (return_channelOfMinimum) *return_channelOfMinimum = channelOfMinimum;
}

double Vector_getMinimum (I, double xmin, double xmax, int interpolation) {
	iam (Vector);
	double minimum;
	Vector_getMinimumAndXAndChannel (me, xmin, xmax, interpolation, & minimum, NULL, NULL);
	return minimum;
}

double Vector_getXOfMinimum (I, double xmin, double xmax, int interpolation) {
	iam (Vector);
	double xOfMinimum;
	Vector_getMinimumAndXAndChannel (me, xmin, xmax, interpolation, NULL, & xOfMinimum, NULL);
	return xOfMinimum;
}

long Vector_getChannelOfMinimum (I, double xmin, double xmax, int interpolation) {
	iam (Vector);
	long channelOfMinimum;
	Vector_getMinimumAndXAndChannel (me, xmin, xmax, interpolation, NULL, NULL, & channelOfMinimum);
	return channelOfMinimum;
}

void Vector_getMaximumAndX (I, double xmin, double xmax, long channel, int interpolation,
	double *return_maximum, double *return_xOfMaximum)
{
	iam (Vector);
	long imin, imax, i, n = my nx;
	Melder_assert (channel >= 1 && channel <= my ny);
	double *y = my z [channel];
	double maximum, x;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	if (! Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
		/*
		 * No samples between xmin and xmax.
		 * Try to return the greater of the values at these two points.
		 */
		double yleft = Vector_getValueAtX (me, xmin, channel,
			interpolation > Vector_VALUE_INTERPOLATION_NEAREST ? Vector_VALUE_INTERPOLATION_LINEAR : Vector_VALUE_INTERPOLATION_NEAREST);
		double yright = Vector_getValueAtX (me, xmax, channel,
			interpolation > Vector_VALUE_INTERPOLATION_NEAREST ? Vector_VALUE_INTERPOLATION_LINEAR : Vector_VALUE_INTERPOLATION_NEAREST);
		maximum = yleft > yright ? yleft : yright;
		x = yleft == yright ? (xmin + xmax) / 2 : yleft > yright ? xmin : xmax;
	} else {
		maximum = y [imin], x = imin;
		if (y [imax] > maximum) maximum = y [imax], x = imax;
		if (imin == 1) imin ++;
		if (imax == my nx) imax --;
		for (i = imin; i <= imax; i ++) {
			if (y [i] > y [i - 1] && y [i] >= y [i + 1]) {
				double i_real, localMaximum = NUMimproveMaximum (y, n, i, interpolation, & i_real);
				if (localMaximum > maximum) maximum = localMaximum, x = i_real;
			}
		}
		x = my x1 + (x - 1) * my dx;   /* Convert sample to x. */
		if (x < xmin) x = xmin; else if (x > xmax) x = xmax;
	}
	if (return_maximum) *return_maximum = maximum;
	if (return_xOfMaximum) *return_xOfMaximum = x;
}

void Vector_getMaximumAndXAndChannel (I, double xmin, double xmax, int interpolation,
	double *return_maximum, double *return_xOfMaximum, long *return_channelOfMaximum)
{
	iam (Vector);
	double maximum, xOfMaximum;
	long channelOfMaximum = 1;
	Vector_getMaximumAndX (me, xmin, xmax, 1, interpolation, & maximum, & xOfMaximum);
	for (long channel = 2; channel <= my ny; channel ++) {
		double maximumOfChannel, xOfMaximumOfChannel;
		Vector_getMaximumAndX (me, xmin, xmax, channel, interpolation, & maximumOfChannel, & xOfMaximumOfChannel);
		if (maximumOfChannel > maximum) {
			maximum = maximumOfChannel;
			xOfMaximum = xOfMaximumOfChannel;
			channelOfMaximum = channel;
		}
	}
	if (return_maximum) *return_maximum = maximum;
	if (return_xOfMaximum) *return_xOfMaximum = xOfMaximum;
	if (return_channelOfMaximum) *return_channelOfMaximum = channelOfMaximum;
}

double Vector_getMaximum (I, double xmin, double xmax, int interpolation) {
	iam (Vector);
	double maximum;
	Vector_getMaximumAndXAndChannel (me, xmin, xmax, interpolation, & maximum, NULL, NULL);
	return maximum;
}

double Vector_getXOfMaximum (I, double xmin, double xmax, int interpolation) {
	iam (Vector);
	double xOfMaximum;
	Vector_getMaximumAndXAndChannel (me, xmin, xmax, interpolation, NULL, & xOfMaximum, NULL);
	return xOfMaximum;
}

long Vector_getChannelOfMaximum (I, double xmin, double xmax, int interpolation) {
	iam (Vector);
	long channelOfMaximum;
	Vector_getMaximumAndXAndChannel (me, xmin, xmax, interpolation, NULL, NULL, & channelOfMaximum);
	return channelOfMaximum;
}

double Vector_getAbsoluteExtremum (I, double xmin, double xmax, int interpolation) { iam (Vector);
	double minimum = fabs (Vector_getMinimum (me, xmin, xmax, interpolation));
	double maximum = fabs (Vector_getMaximum (me, xmin, xmax, interpolation));
	return minimum > maximum ? minimum : maximum;
}

/***** Get statistics. *****/

double Vector_getMean (I, double xmin, double xmax, long channel) {
	iam (Vector);
	return Sampled_getMean (me, xmin, xmax, channel, 0, TRUE);
}

double Vector_getStandardDeviation (I, double xmin, double xmax, long ilevel) {
	iam (Vector);
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	long imin, imax, n = Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax);
	if (n < 2) return NUMundefined;
	if (ilevel == Vector_CHANNEL_AVERAGE) {
		double sum2 = 0.0;
		for (long channel = 1; channel <= my ny; channel ++) {
			double mean = Vector_getMean (me, xmin, xmax, channel);
			for (long i = imin; i <= imax; i ++) {
				double diff = my z [channel] [i] - mean;
				sum2 += diff * diff;
			}
		}
		return sqrt (sum2 / (n * my ny - my ny));   // The number of constraints equals the number of channels,
				// because from every channel its own mean was subtracted.
				// Corollary: a two-channel mono sound will have the same stdev as the corresponding one-channel sound.
	}
	double mean = Vector_getMean (me, xmin, xmax, ilevel);
	double sum2 = 0.0;
	for (long i = imin; i <= imax; i ++) {
		double diff = my z [ilevel] [i] - mean;
		sum2 += diff * diff;
	}
	return sqrt (sum2 / (n - 1));
}

/***** Modify. *****/

void Vector_addScalar (I, double scalar) {
	iam (Vector);
	for (long channel = 1; channel <= my ny; channel ++) {
		for (long i = 1; i <= my nx; i ++) {
			my z [channel] [i] += scalar;
		}
	}
}

void Vector_subtractMean (I) {
	iam (Vector);
	for (long channel = 1; channel <= my ny; channel ++) {
		double sum = 0.0;
		for (long i = 1; i <= my nx; i ++) {
			sum += my z [channel] [i];
		}
		double mean = sum / my nx;
		for (long i = 1; i <= my nx; i ++) {
			my z [channel] [i] -= mean;
		}
	}
}

void Vector_multiplyByScalar (I, double scalar) {
	iam (Vector);
	for (long channel = 1; channel <= my ny; channel ++) {
		for (long i = 1; i <= my nx; i ++) {
			my z [channel] [i] *= scalar;
		}
	}
}

void Vector_scale (I, double scale) {
	iam (Vector);
	double extremum = 0.0;
	for (long channel = 1; channel <= my ny; channel ++) {
		for (long i = 1; i <= my nx; i ++) {
			if (fabs (my z [channel] [i]) > extremum) extremum = fabs (my z [channel] [i]);
		}
	}
	if (extremum != 0.0) {
		Vector_multiplyByScalar (me, scale / extremum);
	}
}

/***** Graphics. *****/

void Vector_draw (I, Graphics g, double *pxmin, double *pxmax, double *pymin, double *pymax,
	double defaultDy, const wchar_t *method)
{
	iam (Vector);
	bool xreversed = *pxmin > *pxmax, yreversed = *pymin > *pymax;
	if (xreversed) { double temp = *pxmin; *pxmin = *pxmax; *pxmax = temp; }
	if (yreversed) { double temp = *pymin; *pymin = *pymax; *pymax = temp; }
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
	Graphics_setWindow (g, xreversed ? *pxmax : *pxmin, xreversed ? *pxmin : *pxmax, yreversed ? *pymax : *pymin, yreversed ? *pymin : *pymax);
	if (wcsstr (method, L"bars") || wcsstr (method, L"Bars")) {
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
	} else if (wcsstr (method, L"poles") || wcsstr (method, L"Poles")) {
		for (ix = ixmin; ix <= ixmax; ix ++) {
			double x = Sampled_indexToX (me, ix);
			Graphics_line (g, x, 0, x, my z [1] [ix]);
		}
	} else if (wcsstr (method, L"speckles") || wcsstr (method, L"Speckles")) {
		for (ix = ixmin; ix <= ixmax; ix ++) {
			double x = Sampled_indexToX (me, ix);
			Graphics_fillCircle_mm (g, x, my z [1] [ix], 1.0);
		}
	} else {
		/*
		 * The default: draw as a curve.
		 */
		Graphics_function (g, my z [1], ixmin, ixmax,
			Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax));
	}
	Graphics_unsetInner (g);
}
	
/* End of file Vector.c */
