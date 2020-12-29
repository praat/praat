/* Vector.cpp
 *
 * Copyright (C) 1992-2009,2011,2012,2014-2018,2020 Paul Boersma
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

#include "enums_getText.h"
#include "Vector_enums.h"
#include "enums_getValue.h"
#include "Vector_enums.h"

Thing_implement (Vector, Matrix, 2);

/*
	Vector::v_getVector () returns the value in channel `rowNumber`,
	or if `rowNumber` == 0, then the average value of all the channels.
	The value is assumed to be zero in columns outside the domain.
*/
double structVector :: v_getVector (integer rowNumber, integer columnNumber) {
	if (columnNumber < 1 || columnNumber > our nx)
		return 0.0;
	if (our ny == 1)
		return our z [1] [columnNumber];   // optimization
	if (rowNumber == 0) {
		if (our ny == 2)
			return 0.5 * (our z [1] [columnNumber] + our z [2] [columnNumber]);   // optimization
		longdouble sum = 0.0;
		for (integer channel = 1; channel <= our ny; channel ++)
			sum += our z [channel] [columnNumber];
		return double (sum / our ny);
	}
	Melder_assert (rowNumber > 0 && rowNumber <= our ny);
	return our z [rowNumber] [columnNumber];
}

/*
	Vector::v_getFunction1 () returns a channel or the average of all the channels.
	There is linear interpolation between columns.
	The value is assumed to be zero in columns outside the domain.
*/
double structVector :: v_getFunction1 (integer rowNumber, double x) {
	const double columnNumber_real = (x - our x1) / our dx + 1.0;
	const integer leftColumnNumber = Melder_ifloor (columnNumber_real);
	const double columnNumber_phase = columnNumber_real - leftColumnNumber;
	double leftValue;
	if (leftColumnNumber < 1 || leftColumnNumber > our nx) {
		leftValue = 0.0;   // outside the definition region, Formula is expected to return zero
	} else if (our ny == 1) {
		leftValue = z [1] [leftColumnNumber];   // optimization
	} else if (rowNumber == 0) {
		if (our ny == 2) {
			leftValue = 0.5 * (our z [1] [leftColumnNumber] + our z [2] [leftColumnNumber]);   // optimization
		} else {
			longdouble sum = 0.0;
			for (integer channel = 1; channel <= ny; channel ++)
				sum += our z [channel] [leftColumnNumber];
			leftValue = double (sum / our ny);
		}
	} else {
		Melder_assert (rowNumber > 0 && rowNumber <= our ny);
		leftValue = our z [rowNumber] [leftColumnNumber];
	}
	const integer rightColumnNumber = leftColumnNumber + 1;
	double rightValue;
	if (rightColumnNumber < 1 || rightColumnNumber > our nx) {
		rightValue = 0.0;   // outside the definition region, Formula is expected to return zero
	} else if (our ny == 1) {
		rightValue = z [1] [rightColumnNumber];   // optimization
	} else if (rowNumber == 0) {
		if (our ny == 2) {
			rightValue = 0.5 * (our z [1] [rightColumnNumber] + our z [2] [rightColumnNumber]);   // optimization
		} else {
			longdouble sum = 0.0;
			for (integer channel = 1; channel <= our ny; channel ++)
				sum += our z [channel] [rightColumnNumber];
			rightValue = double (sum / our ny);
		}
	} else {
		Melder_assert (rowNumber > 0 && rowNumber <= our ny);
		rightValue = z [rowNumber] [rightColumnNumber];
	}
	return (1.0 - columnNumber_phase) * leftValue + columnNumber_phase * rightValue;
}

double structVector :: v_getValueAtSample (integer isamp, integer ilevel, int unit) {
/*
	Preconditions:
		1 <= isamp <= my nx
		0 <= ilevel <= my ny
*/
	double value;
	if (ilevel > Vector_CHANNEL_AVERAGE) {
		value = our z [ilevel] [isamp];
	} else if (our ny == 1) {
		value = our z [1] [isamp];   // optimization
	} else if (our ny == 2) {
		value = 0.5 * (our z [1] [isamp] + our z [2] [isamp]);   // optimization
	} else {
		longdouble sum = 0.0;
		for (integer channel = 1; channel <= our ny; channel ++)
			sum += our z [channel] [isamp];
		value = double (sum / our ny);
	}
	return isdefined (value) ? our v_convertStandardToSpecialUnit (value, ilevel, unit) : undefined;
}

/***** Get content. *****/

integer kVector_valueInterpolation_to_interpolationDepth (kVector_valueInterpolation valueInterpolationType) {
	switch (valueInterpolationType) {
		case kVector_valueInterpolation :: UNDEFINED:
			return NUM_VALUE_INTERPOLATE_LINEAR;
		case kVector_valueInterpolation :: NEAREST:
			return NUM_VALUE_INTERPOLATE_NEAREST;
		case kVector_valueInterpolation :: LINEAR:
			return NUM_VALUE_INTERPOLATE_LINEAR;
		case kVector_valueInterpolation :: CUBIC:
			return NUM_VALUE_INTERPOLATE_CUBIC;
		case kVector_valueInterpolation :: SINC70:
			return NUM_VALUE_INTERPOLATE_SINC70;
		case kVector_valueInterpolation :: SINC700:
			return NUM_VALUE_INTERPOLATE_SINC700;
	}
	return 0;   // never reached
}

integer kVector_peakInterpolation_to_interpolationDepth (kVector_peakInterpolation peakInterpolationType) {
	switch (peakInterpolationType) {
		case kVector_peakInterpolation :: UNDEFINED:
			return NUM_PEAK_INTERPOLATE_PARABOLIC;
		case kVector_peakInterpolation :: NONE:
			return NUM_PEAK_INTERPOLATE_NONE;
		case kVector_peakInterpolation :: PARABOLIC:
			return NUM_PEAK_INTERPOLATE_PARABOLIC;
		case kVector_peakInterpolation :: CUBIC:
			return NUM_PEAK_INTERPOLATE_CUBIC;
		case kVector_peakInterpolation :: SINC70:
			return NUM_PEAK_INTERPOLATE_SINC70;
		case kVector_peakInterpolation :: SINC700:
			return NUM_PEAK_INTERPOLATE_SINC700;
	}
	return 0;   // never reached
}

/*
	Vector_getValueAtX () returns the interpolated value in channel `ilevel`,
	or if `ilevel` == 0, then the average of all the interpolated channels.
*/
double Vector_getValueAtX (Vector me, double x, integer ilevel, kVector_valueInterpolation valueInterpolationType) {
	const double leftEdge = my x1 - 0.5 * my dx, rightEdge = leftEdge + my nx * my dx;
	if (x <  leftEdge || x > rightEdge)
		return undefined;
	const integer interpolationDepth = kVector_valueInterpolation_to_interpolationDepth (valueInterpolationType);
	if (ilevel > Vector_CHANNEL_AVERAGE) {
		Melder_assert (ilevel <= my ny);
		const double index_real = (x - my x1) / my dx + 1.0;
		return NUM_interpolate_sinc (my z.row (ilevel), index_real, interpolationDepth);
	}
	longdouble sum = 0.0;
	for (integer ichan = 1; ichan <= my ny; ichan ++) {
		const double index_real = (x - my x1) / my dx + 1.0;
		sum += NUM_interpolate_sinc (my z.row (ichan), index_real, interpolationDepth);
	}
	return double (sum / my ny);
}

/***** Get shape. *****/

void Vector_getMinimumAndX (Vector me, double xmin, double xmax, integer channelNumber, kVector_peakInterpolation peakInterpolationType,
	double *out_minimum, double *out_xOfMinimum)
{
	Melder_assert (channelNumber >= 1 && channelNumber <= my ny);
	constVEC y = my z.row (channelNumber);
	double minimum, x;
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	integer imin, imax;
	if (! Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
		/*
			No samples between xmin and xmax.
			Try to return the lesser of the values at these two points.
		*/
		kVector_valueInterpolation valueInterpolationType = ( peakInterpolationType > kVector_peakInterpolation :: NONE ?
				kVector_valueInterpolation :: LINEAR : kVector_valueInterpolation :: NEAREST );
		const double yleft = Vector_getValueAtX (me, xmin, channelNumber, valueInterpolationType);
		const double yright = Vector_getValueAtX (me, xmax, channelNumber, valueInterpolationType);
		minimum = std::min (yleft, yright);
		x = ( yleft == yright ? (xmin + xmax) / 2.0 : yleft < yright ? xmin : xmax );
	} else {
		minimum = y [imin];
		x = imin;
		if (y [imax] < minimum) {
			minimum = y [imax];
			x = imax;
		}
		if (imin == 1)
			imin ++;
		if (imax == my nx)
			imax --;
		for (integer i = imin; i <= imax; i ++) {
			if (y [i] < y [i - 1] && y [i] <= y [i + 1]) {
				double i_real;
				const double localMinimum = NUMimproveMinimum (y, i, kVector_peakInterpolation_to_interpolationDepth (peakInterpolationType), & i_real);
				if (localMinimum < minimum) {
					minimum = localMinimum;
					x = i_real;
				}
			}
		}
		x = my x1 + (x - 1) * my dx;   // convert sample to x
		Melder_clip (xmin, & x, xmax);
	}
	if (out_minimum)
		*out_minimum = minimum;
	if (out_xOfMinimum)
		*out_xOfMinimum = x;
}

void Vector_getMinimumAndXAndChannel (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType,
	double *out_minimum, double *out_xOfMinimum, integer *out_channelOfMinimum)
{
	double minimum, xOfMinimum;
	integer channelOfMinimum = 1;
	Vector_getMinimumAndX (me, xmin, xmax, 1, peakInterpolationType, & minimum, & xOfMinimum);
	for (integer channel = 2; channel <= my ny; channel ++) {
		double minimumOfChannel, xOfMinimumOfChannel;
		Vector_getMinimumAndX (me, xmin, xmax, channel, peakInterpolationType, & minimumOfChannel, & xOfMinimumOfChannel);
		if (minimumOfChannel < minimum) {
			minimum = minimumOfChannel;
			xOfMinimum = xOfMinimumOfChannel;
			channelOfMinimum = channel;
		}
	}
	if (out_minimum)
		*out_minimum = minimum;
	if (out_xOfMinimum)
		*out_xOfMinimum = xOfMinimum;
	if (out_channelOfMinimum)
		*out_channelOfMinimum = channelOfMinimum;
}

double Vector_getMinimum (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType) {
	double minimum;
	Vector_getMinimumAndXAndChannel (me, xmin, xmax, peakInterpolationType, & minimum, nullptr, nullptr);
	return minimum;
}

double Vector_getXOfMinimum (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType) {
	double xOfMinimum;
	Vector_getMinimumAndXAndChannel (me, xmin, xmax, peakInterpolationType, nullptr, & xOfMinimum, nullptr);
	return xOfMinimum;
}

integer Vector_getChannelOfMinimum (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType) {
	integer channelOfMinimum;
	Vector_getMinimumAndXAndChannel (me, xmin, xmax, peakInterpolationType, nullptr, nullptr, & channelOfMinimum);
	return channelOfMinimum;
}

void Vector_getMaximumAndX (Vector me, double xmin, double xmax, integer channelNumber, kVector_peakInterpolation peakInterpolationType,
	double *out_maximum, double *out_xOfMaximum)
{
	Melder_assert (channelNumber >= 1 && channelNumber <= my ny);
	constVEC y = my z.row (channelNumber);
	double maximum, x;
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	integer imin, imax, i;
	if (! Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax)) {
		/*
			No samples between xmin and xmax.
			Try to return the greater of the values at these two points.
		*/
		kVector_valueInterpolation valueInterpolationType = ( peakInterpolationType > kVector_peakInterpolation :: NONE ?
				kVector_valueInterpolation :: LINEAR : kVector_valueInterpolation :: NEAREST );
		const double yleft = Vector_getValueAtX (me, xmin, channelNumber, valueInterpolationType);
		const double yright = Vector_getValueAtX (me, xmax, channelNumber, valueInterpolationType);
		maximum = std::max (yleft, yright);
		x = ( yleft == yright ? (xmin + xmax) / 2.0 : yleft > yright ? xmin : xmax );
	} else {
		maximum = y [imin];
		x = imin;
		if (y [imax] > maximum) {
			maximum = y [imax];
			x = imax;
		}
		if (imin == 1)
			imin ++;
		if (imax == my nx)
			imax --;
		for (i = imin; i <= imax; i ++) {
			if (y [i] > y [i - 1] && y [i] >= y [i + 1]) {
				double i_real;
				const double localMaximum = NUMimproveMaximum (y, i, kVector_peakInterpolation_to_interpolationDepth (peakInterpolationType), & i_real);
				if (localMaximum > maximum) {
					maximum = localMaximum;
					x = i_real;
				}
			}
		}
		x = my x1 + (x - 1) * my dx;   // convert sample to x
		Melder_clip (xmin, & x, xmax);
	}
	if (out_maximum)
		*out_maximum = maximum;
	if (out_xOfMaximum)
		*out_xOfMaximum = x;
}

void Vector_getMaximumAndXAndChannel (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType,
	double *out_maximum, double *out_xOfMaximum, integer *out_channelOfMaximum)
{
	double maximum, xOfMaximum;
	integer channelOfMaximum = 1;
	Vector_getMaximumAndX (me, xmin, xmax, 1, peakInterpolationType, & maximum, & xOfMaximum);
	for (integer channel = 2; channel <= my ny; channel ++) {
		double maximumOfChannel, xOfMaximumOfChannel;
		Vector_getMaximumAndX (me, xmin, xmax, channel, peakInterpolationType, & maximumOfChannel, & xOfMaximumOfChannel);
		if (maximumOfChannel > maximum) {
			maximum = maximumOfChannel;
			xOfMaximum = xOfMaximumOfChannel;
			channelOfMaximum = channel;
		}
	}
	if (out_maximum)
		*out_maximum = maximum;
	if (out_xOfMaximum)
		*out_xOfMaximum = xOfMaximum;
	if (out_channelOfMaximum)
		*out_channelOfMaximum = channelOfMaximum;
}

double Vector_getMaximum (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType) {
	double maximum;
	Vector_getMaximumAndXAndChannel (me, xmin, xmax, peakInterpolationType, & maximum, nullptr, nullptr);
	return maximum;
}

double Vector_getXOfMaximum (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType) {
	double xOfMaximum;
	Vector_getMaximumAndXAndChannel (me, xmin, xmax, peakInterpolationType, nullptr, & xOfMaximum, nullptr);
	return xOfMaximum;
}

integer Vector_getChannelOfMaximum (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType) {
	integer channelOfMaximum;
	Vector_getMaximumAndXAndChannel (me, xmin, xmax, peakInterpolationType, nullptr, nullptr, & channelOfMaximum);
	return channelOfMaximum;
}

double Vector_getAbsoluteExtremum (Vector me, double xmin, double xmax, kVector_peakInterpolation peakInterpolationType) {
	const double minimum = Vector_getMinimum (me, xmin, xmax, peakInterpolationType);
	const double maximum = Vector_getMaximum (me, xmin, xmax, peakInterpolationType);
	return std::max (fabs (minimum), fabs (maximum));
}

/***** Get statistics. *****/

double Vector_getMean (Vector me, double xmin, double xmax, integer channel) {
	return Sampled_getMean (me, xmin, xmax, channel, 0, true);
}

double Vector_getStandardDeviation (Vector me, double xmin, double xmax, integer ilevel) {
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	integer imin, imax, n = Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax);
	if (n < 2)
		return undefined;
	if (ilevel == Vector_CHANNEL_AVERAGE) {
		longdouble sum2 = 0.0;
		for (integer channel = 1; channel <= my ny; channel ++) {
			const double mean = Vector_getMean (me, xmin, xmax, channel);
			for (integer i = imin; i <= imax; i ++) {
				const double diff = my z [channel] [i] - mean;
				sum2 += diff * diff;
			}
		}
		return sqrt (double (sum2 / (n * my ny - my ny)));   // The number of constraints equals the number of channels,
				// because from every channel its own mean was subtracted.
				// Corollary: a two-channel mono sound will have the same stdev as the corresponding one-channel sound.
	}
	const double mean = Vector_getMean (me, xmin, xmax, ilevel);
	longdouble sum2 = 0.0;
	for (integer i = imin; i <= imax; i ++) {
		const double diff = my z [ilevel] [i] - mean;
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
		centre_VEC_inout (my channel (ichan));
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

void Vector_draw (Vector me, Graphics graphics, double *pxmin, double *pxmax, double *pymin, double *pymax,
	double defaultDy, conststring32 method)
{
	const bool xreversed = ( *pxmin > *pxmax ), yreversed = ( *pymin > *pymax );
	if (xreversed)
		std::swap (*pxmin, *pxmax);
	if (yreversed)
		std::swap (*pymin, *pymax);
	Function_bidirectionalAutowindow (me, pxmin, pxmax);
	/*
		Domain expressed in sample numbers.
	*/
	integer ixmin, ixmax;
	const integer n = Matrix_getWindowSamplesX (me, *pxmin, *pxmax, & ixmin, & ixmax);
	if (n < 1)
		return;
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
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, ( xreversed ? *pxmax : *pxmin ), ( xreversed ? *pxmin : *pxmax ),
			( yreversed ? *pymax : *pymin ), ( yreversed ? *pymin : *pymax ));
	if (str32str (method, U"bars") || str32str (method, U"Bars")) {
		for (integer ix = ixmin; ix <= ixmax; ix ++) {
			const double x = Sampled_indexToX (me, ix);
			double y = my z [1] [ix];
			double left = x - 0.5 * my dx, right = x + 0.5 * my dx;
			Melder_clipRight (& y, *pymax);
			Melder_clipLeft (*pxmin, & left);
			Melder_clipRight (& right, *pxmax);
			if (y > *pymin) {
				Graphics_line (graphics, left, y, right, y);
				Graphics_line (graphics, left, y, left, *pymin);
				Graphics_line (graphics, right, y, right, *pymin);
			}
		}
	} else if (str32str (method, U"poles") || str32str (method, U"Poles")) {
		for (integer ix = ixmin; ix <= ixmax; ix ++) {
			const double x = Sampled_indexToX (me, ix);
			Graphics_line (graphics, x, 0.0, x, my z [1] [ix]);
		}
	} else if (str32str (method, U"speckles") || str32str (method, U"Speckles")) {
		for (integer ix = ixmin; ix <= ixmax; ix ++) {
			const double x = Sampled_indexToX (me, ix);
			Graphics_speckle (graphics, x, my z [1] [ix]);
		}
	} else {
		/*
			The default: draw as a curve.
		*/
		Graphics_function (graphics, & my z [1] [0], ixmin, ixmax,
				Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax));
	}
	Graphics_unsetInner (graphics);
}
	
/* End of file Vector.cpp */
