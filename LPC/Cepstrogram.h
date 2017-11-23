#ifndef _Cepstrogram_h_
#define _Cepstrogram_h_
/* Cepstrogram.h
 *
 * Copyright (C) 2012-2017 David Weenink
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
 djmw 20121017
 djmw 20130502 Latest modification.
*/

/*
	The Cepstrogram is a matrix of real numbers.
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

autoCepstrogram Cepstrogram_create (double tmin, double tmax, integer nt, double dt, double t1, double qmin, double qmax, integer nq, double dq, double q1);

autoPowerCepstrogram PowerCepstrogram_create (double tmin, double tmax, integer nt, double dt, double t1, double qmin, double qmax, integer nq, double dq, double q1);

void PowerCepstrogram_paint (PowerCepstrogram me, Graphics g, double tmin, double tmax, double qmin, double qmax, double dBmaximum, int autoscaling, double dynamicRangedB, double dynamicCompression, int garnish);

autoPowerCepstrogram PowerCepstrogram_smooth (PowerCepstrogram me, double timeAveragingWindow, double quefrencyAveragingWindow);

autoPowerCepstrogram Sound_to_PowerCepstrogram (Sound me, double pitchFloor, double dt, double maximumFrequency, double preEmphasisFrequency);

autoPowerCepstrogram Sound_to_PowerCepstrogram_hillenbrand (Sound me, double analysisWidth, double dt);

autoTable PowerCepstrogram_to_Table_hillenbrand (PowerCepstrogram me, double pitchFloor, double pitchCeiling);

autoTable PowerCepstrogram_to_Table_cpp (PowerCepstrogram me, double pitchFloor, double pitchCeiling, double deltaF0, int interpolation, double qstartFit, double qendFit, int lineType, int method);

autoPowerCepstrum PowerCepstrogram_to_PowerCepstrum_slice (PowerCepstrogram me, double time);

autoPowerCepstrogram PowerCepstrogram_subtractTilt (PowerCepstrogram me, double qstartFit, double qendFit, int lineType, int fitMethod);

void PowerCepstrogram_subtractTilt_inplace (PowerCepstrogram me, double qstartFit, double qendFit, int lineType, int fitMethod);

double PowerCepstrogram_getCPPS_hillenbrand (PowerCepstrogram me, bool subtractTiltBeforeSmoothing, double timeAveragingWindow, double quefrencyAveragingWindow, double pitchFloor, double pitchCeiling);

double PowerCepstrogram_getCPPS (PowerCepstrogram me, bool subtractTiltBeforeSmoothing, double timeAveragingWindow, double quefrencyAveragingWindow, double pitchFloor, double pitchCeiling, double deltaF0, int interpolation, double qstartFit, double qendFit, int lineType, int fitMethod);

autoMatrix PowerCepstrogram_to_Matrix (PowerCepstrogram me);

autoPowerCepstrogram Matrix_to_PowerCepstrogram (Matrix me);

#endif /* _Cepstrogram_h_ */
