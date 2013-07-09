#ifndef _Cepstrogram_h_
#define _Cepstrogram_h_
/* Cepstrogram.h
 *
 * Copyright (C) 2012-2013 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20121017
 djmw 20130502 Latest modification.
*/

/*
	The Cepstrogram is a matrix of REAL numbers.
	It is the inverse-fourier-transformed logarithm of the spectrum
	of a (sound) signal.
*/

#include "Cepstrum.h"
#include "Sound.h"
#include "Table.h"

Thing_define (Cepstrogram, Matrix) {
};
Thing_define (PowerCepstrogram, Cepstrogram) {
};
/*
	xmin, xmax : time domain
	nx, dx, x1	: sampling in the time domain
	ymin		// Lowest quefrency.
	ymax		// Highest quefrency.
	ny			// Number of quefrencies.
	dy			// Quefrency step.
	y1			// First quefrency.

*/

Cepstrogram Cepstrogram_create (double tmin, double tmax, long nt, double dt, double t1,
	double qmin, double qmax, long nq, double dq, double q1);
PowerCepstrogram PowerCepstrogram_create (double tmin, double tmax, long nt, double dt, double t1,
	double qmin, double qmax, long nq, double dq, double q1);

void PowerCepstrogram_paint (PowerCepstrogram me, Graphics g, double tmin, double tmax, double qmin, double qmax, double dBminimum, double dBmaximum, int garnish);

PowerCepstrogram PowerCepstrogram_smooth (PowerCepstrogram me, double timeAveragingWindow, double quefrencyAveragingWindow);

PowerCepstrogram Sound_to_PowerCepstrogram (Sound me, double analysisWidth, double dt, double maximumFrequency, double preEmphasisFrequency);

PowerCepstrogram Sound_to_PowerCepstrogram_hillenbrand (Sound me, double analysisWidth, double dt);

Table PowerCepstrogram_to_Table_hillenbrand (PowerCepstrogram me, double pitchFloor, double pitchCeiling);
Table PowerCepstrogram_to_Table_cpp (PowerCepstrogram me, double pitchFloor, double pitchCeiling, double deltaF0, int interpolation, double qstartFit, double qendFit, int lineType, int method);
PowerCepstrum PowerCepstrogram_to_PowerCepstrum_slice (PowerCepstrogram me, double time);

PowerCepstrogram PowerCepstrogram_subtractTilt (PowerCepstrogram me, double qstartFit, double qendFit, int lineType, int fitMethod);
void PowerCepstrogram_subtractTilt_inline (PowerCepstrogram me, double qstartFit, double qendFit, int lineType, int fitMethod);

double PowerCepstrogram_getCPPS_hillenbrand (PowerCepstrogram me, bool subtractTiltBeforeSmoothing, double timeAveragingWindow, double quefrencyAveragingWindow, double pitchFloor, double pitchCeiling);
double PowerCepstrogram_getCPPS (PowerCepstrogram me, bool subtractTiltBeforeSmoothing, double timeAveragingWindow, double quefrencyAveragingWindow,
	double pitchFloor, double pitchCeiling, double deltaF0, int interpolation, double qstartFit, double qendFit, int lineType, int fitMethod);

Matrix PowerCepstrogram_to_Matrix (PowerCepstrogram me);
PowerCepstrogram Matrix_to_Cepstrogram (Matrix me);
#endif /* _Cepstrogram_h_ */
