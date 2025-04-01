#ifndef _PowerCepstrum_h_
#define _PowerCepstrum_h_
/* PowerCepstrum.h
 *
 * Copyright (C) 2012-2025 David Weenink
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
#include "Table.h"
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
	double v_getValueAtSample (integer isamp, integer which, int units) const
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

void PowerCepstrum_draw (constPowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, bool garnish);

integer PowerCepstrum_getPointRange (constPowerCepstrum me, double qmin, double qmax, integer& imin, integer& imax);
void PowerCepstrum_getPoints (constPowerCepstrum me, integer imin, integer imax, VEC x, VEC y);


void PowerCepstrum_drawTrendLine (constPowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, double qstart, double qend, kCepstrum_trendType lineType, kCepstrum_trendFit method);
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

void PowerCepstrum_getMaximumAndQuefrency_pitch (constPowerCepstrum me, double pitchFloor, double pitchCeiling, kVector_peakInterpolation peakInterpolationType, double& maximum, double& quefrency); // deprecated, too many options for interpolation type.

void PowerCepstrum_getMaximumAndQuefrency_q (constPowerCepstrum me, double fromQuefrency, double toQuefrency, 
	kCepstrum_peakInterpolation peakInterpolationType, double& peakdB, double& quefrency);

// The standard of Hillenbrand with fitting options
double PowerCepstrum_getPeakProminence_hillenbrand (constPowerCepstrum me, double pitchFloor, double pitchCeiling, double& qpeak);

double PowerCepstrum_getRNR (constPowerCepstrum me, double pitchFloor, double pitchCeiling, double f0fractionalWidth);

autoTable PowerCepstrum_tabulateRhamonics (constPowerCepstrum me, double pitchFloor, double pitchCeiling, kVector_peakInterpolation peakInterpolationType);
/*
	column 1: quefrency
	column 2: power at quefrency
*/

double PowerCepstrum_getPeakProminence (constPowerCepstrum me, double pitchFloor, double pitchCeiling, kVector_peakInterpolation peakInterpolationType, double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod, double& qpeak);

void PowerCepstrum_fitTrendLine (constPowerCepstrum me, double qmin, double qmax, double *out_slope, double *out_intercept, kCepstrum_trendType lineType, kCepstrum_trendFit method);

double PowerCepstrum_getTrendLineValue (constPowerCepstrum me, double quefrency, double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod);

autoPowerCepstrum PowerCepstrum_subtractTrend (constPowerCepstrum me, double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod);

void PowerCepstrum_subtractTrend_inplace (mutablePowerCepstrum me, double qstartFit, double qendFit, kCepstrum_trendType lineType, kCepstrum_trendFit fitMethod);

void PowerCepstrum_smooth_inplace (mutablePowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations);

autoPowerCepstrum PowerCepstrum_smooth (constPowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations);

autoMatrix PowerCepstrum_to_Matrix (constPowerCepstrum me);

autoPowerCepstrum Matrix_to_PowerCepstrum (constMatrix me);

autoPowerCepstrum Matrix_to_PowerCepstrum_row (constMatrix me, integer row);

autoPowerCepstrum Matrix_to_PowerCepstrum_column (constMatrix me, integer col);

autoPowerCepstrum Cepstrum_downto_PowerCepstrum (constCepstrum me);

#endif /* _PowerCepstrum_h_ */
