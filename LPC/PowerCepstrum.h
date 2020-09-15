#ifndef _PowerCepstrum_h_
#define _PowerCepstrum_h_
/* PowerCepstrum.h
 *
 * Copyright (C) 2012-2020 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Cepstrum.h"
#include "Vector.h"

/*
	The PowerCepstrum is a sequence of real numbers.
	It is the power spectrum of the power spectrum of a (sound) signal.

	xmin		// Lowest quefrency.
	xmax		// Highest quefrency.
	nx			// Number of quefrencies.
	dx			// Quefrency step.
	x1			// First quefrency.

	ymin = ymax = dy = y1 = 1
	ny = 1
*/

Thing_define (PowerCepstrum, Cepstrum) {
	double v_getValueAtSample (integer isamp, integer which, int units)
		override;
};

autoPowerCepstrum PowerCepstrum_create (double qmax, integer nq);
/* Preconditions:
		nq >= 2;
	Postconditions:
		my xmin == 0;					my ymin == 1;
		my xmax == qmax;				my ymax == 1;
		my nx == nq;					my ny == 1;
		my dx == qmax / (nq -1);		my dy == 1;
		my x1 == 0.0;					my y1 == 1.0;
		my z [1..ny] [1..nx] == 0.0;
*/

void PowerCepstrum_draw (PowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, bool garnish);

void PowerCepstrum_drawTrendLine (PowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, double qstart, double qend, kCepstrumTrendType lineType, kCepstrumTrendFit method);
/*
	Function:
		Draw a Cepstrum
	Preconditions:
		maximum > minimum;
	Arguments:
		[qmin, qmax]: quefrencies; x domain of drawing;
		Autowindowing: if qmax <= qmin, x domain of drawing is
			[my xmin, my xmax].
		[minimum, maximum]: amplitude; y range of drawing.
*/

void PowerCepstrum_getMaximumAndQuefrency (PowerCepstrum me, double pitchFloor, double pitchCeiling, kVector_peakInterpolation peakInterpolationType, double *maximum, double *quefrency);

// The standard of Hillenbrand with fitting options
double PowerCepstrum_getPeakProminence_hillenbrand (PowerCepstrum me, double pitchFloor, double pitchCeiling, double *qpeak);

double PowerCepstrum_getRNR (PowerCepstrum me, double pitchFloor, double pitchCeiling, double f0fractionalWidth);

double PowerCepstrum_getPeakProminence (PowerCepstrum me, double pitchFloor, double pitchCeiling, kVector_peakInterpolation peakInterpolationType, double qstartFit, double qendFit, kCepstrumTrendType lineType, kCepstrumTrendFit fitMethod, double *qpeak);

void PowerCepstrum_fitTrendLine (PowerCepstrum me, double qmin, double qmax, double *out_slope, double *out_intercept, kCepstrumTrendType lineType, kCepstrumTrendFit method);

autoPowerCepstrum PowerCepstrum_subtractTrend (PowerCepstrum me, double qstartFit, double qendFit, kCepstrumTrendType  lineType, kCepstrumTrendFit fitMethod);

void PowerCepstrum_subtractTrend_inplace (PowerCepstrum me, double qstartFit, double qendFit, kCepstrumTrendType lineType, kCepstrumTrendFit fitMethod);

void PowerCepstrum_smooth_inplace (PowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations);

autoPowerCepstrum PowerCepstrum_smooth (PowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations);

autoMatrix PowerCepstrum_to_Matrix (PowerCepstrum me);

autoPowerCepstrum Matrix_to_PowerCepstrum (Matrix me);

autoPowerCepstrum Matrix_to_PowerCepstrum_row (Matrix me, integer row);

autoPowerCepstrum Matrix_to_PowerCepstrum_column (Matrix me, integer col);

autoPowerCepstrum Cepstrum_downto_PowerCepstrum (Cepstrum me);

#endif /* _PowerCepstrum_h_ */
