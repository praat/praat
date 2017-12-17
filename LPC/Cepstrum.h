#ifndef _Cepstrum_h_
#define _Cepstrum_h_
/* Cepstrum.h
 *
 * Copyright (C) 1994-2013, 2015-2016 David Weenink
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

/*
 djmw 20010111
 djmw 20020812 GPL header
 djmw 20121117 Latest modification.
*/

/*
	The Cepstrum is a sequence of real numbers.
	It is the spectrum of the power spectrum of a (sound) signal.
*/

#include "Matrix.h"

Thing_define (Cepstrum, Matrix) {
	double v_getValueAtSample (integer isamp, integer which, int units)
		override;
};

/*
	The Cepstrum is a sequence of real numbers.
	It is the power spectrum of the power spectrum of a (sound) signal.
*/

Thing_define (PowerCepstrum, Cepstrum) {
	double v_getValueAtSample (integer isamp, integer which, int units)
		override;
};

/*
	xmin		// Lowest quefrency.
	xmax		// Highest quefrency.
	nx			// Number of quefrencies.
	dx			// Quefrency step.
	x1			// First quefrency.

	ymin = ymax = dy = y1 = 1
	ny = 1
*/

autoCepstrum Cepstrum_create (double qmax, integer nq);

autoPowerCepstrum PowerCepstrum_create (double qmax, integer nq);
/* Preconditions:
		nq >= 2;
	Postconditions:
		my xmin = 0;				my ymin = 1;
		my xmax = qmax;				my ymax = 1;
		my nx = nq;					my ny = 1;
		my dx = qmax / (nq -1);			my dy = 1;
		my x1 = 0;			my y1 = 1;
		my z [1..ny] [1..nx] = 0.0;
*/

void PowerCepstrum_draw (PowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, int garnish);

void Cepstrum_drawLinear (Cepstrum me, Graphics g, double qmin, double qmax, double minimum, double maximum, int garnish);

void PowerCepstrum_drawTiltLine (PowerCepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, double qstart, double qend, int lineType, int method);
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

void PowerCepstrum_getMaximumAndQuefrency (PowerCepstrum me, double pitchFloor, double pitchCeiling, int interpolation, double *maximum, double *quefrency);

// The standard of Hillenbrand with fitting options
double PowerCepstrum_getPeakProminence_hillenbrand (PowerCepstrum me, double pitchFloor, double pitchCeiling, double *qpeak);

double PowerCepstrum_getRNR (PowerCepstrum me, double pitchFloor, double pitchCeiling, double f0fractionalWidth);
double PowerCepstrum_getPeakProminence (PowerCepstrum me, double pitchFloor, double pitchCeiling, int interpolation, double qstartFit, double qendFit, int lineType, int fitMethod, double *qpeak);
void PowerCepstrum_fitTiltLine (PowerCepstrum me, double qmin, double qmax, double *slope, double *intercept, int lineType, int method);
autoPowerCepstrum PowerCepstrum_subtractTilt (PowerCepstrum me, double qstartFit, double qendFit, int lineType, int fitMethod);
void PowerCepstrum_subtractTilt_inplace (PowerCepstrum me, double qstartFit, double qendFit, int lineType, int fitMethod);

void PowerCepstrum_smooth_inplace (PowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations);
autoPowerCepstrum PowerCepstrum_smooth (PowerCepstrum me, double quefrencyAveragingWindow, integer numberOfIterations);

autoMatrix PowerCepstrum_to_Matrix (PowerCepstrum me);
autoPowerCepstrum Matrix_to_PowerCepstrum (Matrix me);
autoPowerCepstrum Matrix_to_PowerCepstrum_row (Matrix me, integer row);
autoPowerCepstrum Matrix_to_PowerCepstrum_column (Matrix me, integer col);
autoPowerCepstrum Cepstrum_downto_PowerCepstrum (Cepstrum me);

#endif /* _Cepstrum_h_ */
