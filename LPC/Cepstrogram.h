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

void Cepstrogram_paint (Cepstrogram me, Graphics g, double tmin, double tmax, double qmin, double qmax, double dBminimum, double dBmaximum, int garnish);

Cepstrogram Cepstrogram_smooth (Cepstrogram me, double timeAveragingWindow, double quefrencyAveragingWindow);

Cepstrogram Sound_to_Cepstrogram (Sound me, double analysisWidth, double dt, double maximumFrequency, double preEmphasisFrequency);

Cepstrogram Sound_to_Cepstrogram_hillenbrand (Sound me, double analysisWidth, double dt);

Table Cepstrogram_to_Table_cpp (Cepstrogram me, double pitchFloor, double pitchCeiling, int interpolation, double qstartFit, double qendFit, int method);
Cepstrum Cepstrogram_to_Cepstrum_slice (Cepstrogram me, double time);

double Cepstrogram_getCPPS (Cepstrogram me, double timeAveragingWindow, double quefrencyAveragingWindow, double pitchFloor, double pitchCeiling, int interpolation, double qstartFit, double qendFit, int fitMethod);


Matrix Cepstrogram_to_Matrix (Cepstrogram me);
Cepstrogram Matrix_to_Cepstrogram (Matrix me);
#endif /* _Cepstrogram_h_ */
