/* Vector.cpp
 *
 * Copyright (C) 1992-2009,2011,2012,2014-2018 Paul Boersma
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

#include "Vector.h"

//
// Vector::getVector () returns a channel or the average of all the channels.
//
double structVector :: v_getVector (integer irow, integer icol) {
	if (icol < 1 || icol > nx) return 0.0;
	if (ny == 1) return z [1] [icol];   // optimization
	if (irow == 0) {
		if (ny == 2) return 0.5 * (z [1] [icol] + z [2] [icol]);   // optimization
		longdouble sum = 0.0;
		for (integer channel = 1; channel <= ny; channel ++)
			sum += z [channel] [icol];
		return double (sum / ny);
	}
	Melder_assert (irow > 0 && irow <= ny);
	return z [irow] [icol];
}

//
// Vector::getFunction1 () returns a channel or the average of all the channels.
//
double structVector :: v_getFunction1 (integer irow, double x) {
	double rcol = (x - x1) / dx + 1.0;
	integer icol = Melder_ifloor (rcol);
	double dcol = rcol - icol;
	double z1;
	if (icol < 1 || icol > nx) {
		z1 = 0.0;   // outside the definition region, Formula is expected to return zero
	} else if (ny == 1) {
		z1 = z [1] [icol];   // optimization
	} else if (irow == 0) {
		if (ny == 2) {
			z1 = 0.5 * (z [1] [icol] + z [2] [icol]);   // optimization
		} else {
			longdouble sum = 0.0;
			for (integer channel = 1; channel <= ny; channel ++)
				sum += z [channel] [icol];
			z1 = double (sum / ny);
		}
	} else {
		Melder_assert (irow > 0 && irow <= ny);
		z1 = z [irow] [icol];
	}
	double z2;
	if (icol < 0 || icol >= nx) {
		z2 = 0.0;   // outside the definition region, Formula is expected to return zero
	} else if (ny == 1) {
		z2 = z [1] [icol + 1];   // optimization
	} else if (irow == 0) {
		if (ny == 2) {
			z2 = 0.5 * (z [1] [icol + 1] + z [2] [icol + 1]);   // optimization
		} else {
			longdouble sum = 0.0;
			for (integer channel = 1; channel <= ny; channel ++)
				sum += z [channel] [icol + 1];
			z2 = double (sum / ny);
		}
	} else {
		Melder_assert (irow > 0 && irow <= ny);
		z2 = z [irow] [icol + 1];
	}
	return (1.0 - dcol) * z1 + dcol * z2;
}

double structVector :: v_getValueAtSample (integer isamp, integer ilevel, int unit) {
// Preconditions:
//    1 <= isamp <= my nx
//    0 <= ilevel <= my ny
	double value;
	if (ilevel > Vector_CHANNEL_AVERAGE) {
		value = z [ilevel] [isamp];
	} else if (ny == 1) {
		value = z [1] [isamp];   // optimization
	} else if (ny == 2) {
		value = 0.5 * (z [1] [isamp] + z [2] [isamp]);   // optimization
	} else {
		longdouble sum = 0.0;
		for (integer channel = 1; channel <= ny; channel ++)
			sum += z [channel] [isamp];
		value = double (sum / ny);
	}
	return isdefined (value) ? v_convertStandardToSpecialUnit (value, ilevel, unit) : undefined;
}

Thing_implement (Vector, Matrix, 2);

/***** Get content. *****/

//
// Vector_getValueAtX () returns the average of all the interpolated channels.
//
double Vector_getValueAtX (Vector me, double x, integer ilevel, int interpolation) {
	double leftEdge = my x1 - 0.5 * my dx, rightEdge = leftEdge + my nx * my dx;
	if (x <  leftEdge || x > rightEdge) return undefined;
	if (ilevel > Vector_CHANNEL_AVERAGE) {
		Melder_assert (ilevel <= my ny);
		return NUM_interpolate_sinc (my z.row (ilevel), Sampled_xToIndex (me, x),
			interpolation == Vector_VALUE_INTERPOLATION_SINC70 ? NUM_VALUE_INTERPOLATE_SINC70 :
			interpolation == Vector_VALUE_INTERPOLATION_SINC700 ? NUM_VALUE_INTERPOLATE_SINC700 :
			interpolation);
	}
	longdouble sum = 0.0;
	for (integer ichan = 1; ichan <= my ny; ichan ++) {
		sum += NUM_interpolate_sinc (my z.row (ichan), Sampled_xToIndex (me, x),
			interpolation == Vector_VALUE_INTERPOLATION_SINC70 ? NUM_VALUE_INTERPOLATE_SINC70 :
			interpolation == Vector_VALUE_INTERPOLATION_SINC700 ? NUM_VALUE_INTERPOLATE_SINC700 :
			interpolation);
	}
	return double (sum / my ny);
}

/***** Get shape. *****/

void Vector_getMinimumAndX (Vector me, double xmin, double xmax, integer channelNumber, int interpolation,
	double *out_minimum, double *out_xOfMinimum)
{
	Melder_assert (channelNumber >= 1 && channelNumber <= my ny);
	constVEC y = my z.row (channelNumber);
	double minimum, x;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	integer imin, imax;
	if (! Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
		/*
			No samples between xmin and xmax.
			Try to return the lesser of the values at these two points.
		*/
		double yleft = Vector_getValueAtX (me, xmin, channelNumber,
			interpolation > Vector_VALUE_INTERPOLATION_NEAREST ? Vector_VALUE_INTERPOLATION_LINEAR : Vector_VALUE_INTERPOLATION_NEAREST);
		double yright = Vector_getValueAtX (me, xmax, channelNumber,
			interpolation > Vector_VALUE_INTERPOLATION_NEAREST ? Vector_VALUE_INTERPOLATION_LINEAR : Vector_VALUE_INTERPOLATION_NEAREST);
		minimum = yleft < yright ? yleft : yright;
		x = yleft == yright ? (xmin + xmax) / 2 : yleft < yright ? xmin : xmax;
	} else {
		minimum = y [imin];
		x = imin;
		if (y [imax] < minimum) {
			minimum = y [imax];
			x = imax;
		}
		if (imin == 1) imin ++;
		if (imax == my nx) imax --;
		for (integer i = imin; i <= imax; i ++) {
			if (y [i] < y [i - 1] && y [i] <= y [i + 1]) {
				double i_real, localMinimum = NUMimproveMinimum (y, i, interpolation, & i_real);
				if (localMinimum < minimum) {
					minimum = localMinimum;
					x = i_real;
				}
			}
		}
		x = my x1 + (x - 1) * my dx;   // convert sample to x
		if (x < xmin) x = xmin; else if (x > xmax) x = xmax;
	}
	if (out_minimum) *out_minimum = minimum;
	if (out_xOfMinimum) *out_xOfMinimum = x;
}

void Vector_getMinimumAndXAndChannel (Vector me, double xmin, double xmax, int interpolation,
	double *out_minimum, double *out_xOfMinimum, integer *out_channelOfMinimum)
{
	double minimum, xOfMinimum;
	integer channelOfMinimum = 1;
	Vector_getMinimumAndX (me, xmin, xmax, 1, interpolation, & minimum, & xOfMinimum);
	for (integer channel = 2; channel <= my ny; channel ++) {
		double minimumOfChannel, xOfMinimumOfChannel;
		Vector_getMinimumAndX (me, xmin, xmax, channel, interpolation, & minimumOfChannel, & xOfMinimumOfChannel);
		if (minimumOfChannel < minimum) {
			minimum = minimumOfChannel;
			xOfMinimum = xOfMinimumOfChannel;
			channelOfMinimum = channel;
		}
	}
	if (out_minimum) *out_minimum = minimum;
	if (out_xOfMinimum) *out_xOfMinimum = xOfMinimum;
	if (out_channelOfMinimum) *out_channelOfMinimum = channelOfMinimum;
}

double Vector_getMinimum (Vector me, double xmin, double xmax, int interpolation) {
	double minimum;
	Vector_getMinimumAndXAndChannel (me, xmin, xmax, interpolation, & minimum, nullptr, nullptr);
	return minimum;
}

double Vector_getXOfMinimum (Vector me, double xmin, double xmax, int interpolation) {
	double xOfMinimum;
	Vector_getMinimumAndXAndChannel (me, xmin, xmax, interpolation, nullptr, & xOfMinimum, nullptr);
	return xOfMinimum;
}

integer Vector_getChannelOfMinimum (Vector me, double xmin, double xmax, int interpolation) {
	integer channelOfMinimum;
	Vector_getMinimumAndXAndChannel (me, xmin, xmax, interpolation, nullptr, nullptr, & channelOfMinimum);
	return channelOfMinimum;
}

void Vector_getMaximumAndX (Vector me, double xmin, double xmax, integer channelNumber, int interpolation,
	double *out_maximum, double *out_xOfMaximum)
{
	Melder_assert (channelNumber >= 1 && channelNumber <= my ny);
	constVEC y = my z.row (channelNumber);
	double maximum, x;
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	integer imin, imax, i;
	if (! Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
		/*
			No samples between xmin and xmax.
			Try to return the greater of the values at these two points.
		*/
		double yleft = Vector_getValueAtX (me, xmin, channelNumber,
			interpolation > Vector_VALUE_INTERPOLATION_NEAREST ? Vector_VALUE_INTERPOLATION_LINEAR : Vector_VALUE_INTERPOLATION_NEAREST);
		double yright = Vector_getValueAtX (me, xmax, channelNumber,
			interpolation > Vector_VALUE_INTERPOLATION_NEAREST ? Vector_VALUE_INTERPOLATION_LINEAR : Vector_VALUE_INTERPOLATION_NEAREST);
		maximum = yleft > yright ? yleft : yright;
		x = yleft == yright ? (xmin + xmax) / 2 : yleft > yright ? xmin : xmax;
	} else {
		maximum = y [imin];
		x = imin;
		if (y [imax] > maximum) {
			maximum = y [imax];
			x = imax;
		}
		if (imin == 1) imin ++;
		if (imax == my nx) imax --;
		for (i = imin; i <= imax; i ++) {
			if (y [i] > y [i - 1] && y [i] >= y [i + 1]) {
				double i_real, localMaximum = NUMimproveMaximum (y, i, interpolation, & i_real);
				if (localMaximum > maximum) {
					maximum = localMaximum;
					x = i_real;
				}
			}
		}
		x = my x1 + (x - 1) * my dx;   // convert sample to x
		if (x < xmin) x = xmin; else if (x > xmax) x = xmax;
	}
	if (out_maximum) *out_maximum = maximum;
	if (out_xOfMaximum) *out_xOfMaximum = x;
}

void Vector_getMaximumAndXAndChannel (Vector me, double xmin, double xmax, int interpolation,
	double *out_maximum, double *out_xOfMaximum, integer *out_channelOfMaximum)
{
	double maximum, xOfMaximum;
	integer channelOfMaximum = 1;
	Vector_getMaximumAndX (me, xmin, xmax, 1, interpolation, & maximum, & xOfMaximum);
	for (integer channel = 2; channel <= my ny; channel ++) {
		double maximumOfChannel, xOfMaximumOfChannel;
		Vector_getMaximumAndX (me, xmin, xmax, channel, interpolation, & maximumOfChannel, & xOfMaximumOfChannel);
		if (maximumOfChannel > maximum) {
			maximum = maximumOfChannel;
			xOfMaximum = xOfMaximumOfChannel;
			channelOfMaximum = channel;
		}
	}
	if (out_maximum) *out_maximum = maximum;
	if (out_xOfMaximum) *out_xOfMaximum = xOfMaximum;
	if (out_channelOfMaximum) *out_channelOfMaximum = channelOfMaximum;
}

double Vector_getMaximum (Vector me, double xmin, double xmax, int interpolation) {
	double maximum;
	Vector_getMaximumAndXAndChannel (me, xmin, xmax, interpolation, & maximum, nullptr, nullptr);
	return maximum;
}

double Vector_getXOfMaximum (Vector me, double xmin, double xmax, int interpolation) {
	double xOfMaximum;
	Vector_getMaximumAndXAndChannel (me, xmin, xmax, interpolation, nullptr, & xOfMaximum, nullptr);
	return xOfMaximum;
}

integer Vector_getChannelOfMaximum (Vector me, double xmin, double xmax, int interpolation) {
	integer channelOfMaximum;
	Vector_getMaximumAndXAndChannel (me, xmin, xmax, interpolation, nullptr, nullptr, & channelOfMaximum);
	return channelOfMaximum;
}

double Vector_getAbsoluteExtremum (Vector me, double xmin, double xmax, int interpolation) {
	double minimum = fabs (Vector_getMinimum (me, xmin, xmax, interpolation));
	double maximum = fabs (Vector_getMaximum (me, xmin, xmax, interpolation));
	return minimum > maximum ? minimum : maximum;
}

/***** Get statistics. *****/

double Vector_getMean (Vector me, double xmin, double xmax, integer channel) {
	return Sampled_getMean (me, xmin, xmax, channel, 0, true);
}

double Vector_getStandardDeviation (Vector me, double xmin, double xmax, integer ilevel) {
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	integer imin, imax, n = Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax);
	if (n < 2) return undefined;
	if (ilevel == Vector_CHANNEL_AVERAGE) {
		longdouble sum2 = 0.0;
		for (integer channel = 1; channel <= my ny; channel ++) {
			double mean = Vector_getMean (me, xmin, xmax, channel);
			for (integer i = imin; i <= imax; i ++) {
				double diff = my z [channel] [i] - mean;
				sum2 += diff * diff;
			}
		}
		return sqrt (double (sum2 / (n * my ny - my ny)));   // The number of constraints equals the number of channels,
				// because from every channel its own mean was subtracted.
				// Corollary: a two-channel mono sound will have the same stdev as the corresponding one-channel sound.
	}
	double mean = Vector_getMean (me, xmin, xmax, ilevel);
	longdouble sum2 = 0.0;
	for (integer i = imin; i <= imax; i ++) {
		double diff = my z [ilevel] [i] - mean;
		sum2 += diff * diff;
	}
	return sqrt (double (sum2 / (n - 1)));
}

/***** Modify. *****/

void Vector_addScalar (Vector me, double scalar) {
	for (integer ichan = 1; ichan <= my ny; ichan ++)
		my channel (ichan)  +=  scalar;
}

void Vector_subtractMean (Vector me) {
	for (integer ichan = 1; ichan <= my ny; ichan ++)
		VECcentre_inplace (my channel (ichan));
}

void Vector_multiplyByScalar (Vector me, double scalar) {
	for (integer ichan = 1; ichan <= my ny; ichan ++)
		my channel (ichan)  *=  scalar;
}

void Vector_scale (Vector me, double scale) {
	double extremum = NUMextremum (my z.get());
	if (extremum != 0.0)
		Vector_multiplyByScalar (me, scale / extremum);
}

/***** Graphics. *****/

void Vector_draw (Vector me, Graphics g, double *pxmin, double *pxmax, double *pymin, double *pymax,
	double defaultDy, conststring32 method)
{
	bool xreversed = *pxmin > *pxmax, yreversed = *pymin > *pymax;
	if (xreversed) { double temp = *pxmin; *pxmin = *pxmax; *pxmax = temp; }
	if (yreversed) { double temp = *pymin; *pymin = *pymax; *pymax = temp; }
	/*
		Automatic domain.
	*/
	if (*pxmin == *pxmax) {
		*pxmin = my xmin;
		*pxmax = my xmax;
	}
	/*
		Domain expressed in sample numbers.
	*/
	integer ixmin, ixmax;
	integer n = Matrix_getWindowSamplesX (me, *pxmin, *pxmax, & ixmin, & ixmax);
	if (n < 1) return;
	/*
		Automatic vertical range.
	*/
	if (*pymin == *pymax) {
		Matrix_getWindowExtrema (me, ixmin, ixmax, 1, 1, pymin, pymax);
		if (*pymin == *pymax) {
			*pymin -= defaultDy;
			*pymax += defaultDy;
		}
	}
	/*
		Set coordinates for drawing.
	*/
	Graphics_setInner (g);
	Graphics_setWindow (g, xreversed ? *pxmax : *pxmin, xreversed ? *pxmin : *pxmax, yreversed ? *pymax : *pymin, yreversed ? *pymin : *pymax);
	if (str32str (method, U"bars") || str32str (method, U"Bars")) {
		for (integer ix = ixmin; ix <= ixmax; ix ++) {
			double x = Sampled_indexToX (me, ix);
			double y = my z [1] [ix];
			double left = x - 0.5 * my dx, right = x + 0.5 * my dx;
			if (y > *pymax) y = *pymax;
			if (left < *pxmin) left = *pxmin;
			if (right > *pxmax) right = *pxmax;
			if (y > *pymin) {
				Graphics_line (g, left, y, right, y);
				Graphics_line (g, left, y, left, *pymin);
				Graphics_line (g, right, y, right, *pymin);
			}
		}
	} else if (str32str (method, U"poles") || str32str (method, U"Poles")) {
		for (integer ix = ixmin; ix <= ixmax; ix ++) {
			double x = Sampled_indexToX (me, ix);
			Graphics_line (g, x, 0.0, x, my z [1] [ix]);
		}
	} else if (str32str (method, U"speckles") || str32str (method, U"Speckles")) {
		for (integer ix = ixmin; ix <= ixmax; ix ++) {
			double x = Sampled_indexToX (me, ix);
			Graphics_speckle (g, x, my z [1] [ix]);
		}
	} else {
		/*
		 * The default: draw as a curve.
		 */
		Graphics_function (g, & my z [1] [0], ixmin, ixmax,
			Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax));
	}
	Graphics_unsetInner (g);
}
	
/* End of file Vector.cpp */
