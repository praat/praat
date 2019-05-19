#ifndef _Spectrogram_h_
#define _Spectrogram_h_
/* Spectrogram.h
 *
 * Copyright (C) 1992-2007,2011,2012,2015-2019 David Weenink & Paul Boersma
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

#include "Matrix.h"
#include "Graphics.h"

Thing_define (Spectrogram, Matrix) {
	void v_info ()
		override;
	int v_domainQuantity ()
		override { return MelderQuantity_TIME_SECONDS; }
};

/* Attributes:
	xmin			// Start time (seconds).
	xmax		// End time (seconds).
	nx			// Number of time slices.
	dx			// The time between two subsequent time slices.
	x1			// The centre of the first time slice.
	ymin			// Minimum frequency (Hz).
	ymax		// Maximum frequency (Hz).
	dy			// Frequency step (Hz).
	y1			// Centre of first frequency band (Hz).
	z [iy] [ix]		// Power spectrum density.
*/

autoSpectrogram Spectrogram_create (double tmin, double tmax, integer nt, double dt, double t1,
					double fmin, double fmax, integer nf, double df, double f1);
/*
	Function:
		Create the spectrogram data structure.
	Preconditions:
		nt > 0;
		nf > 0;
		dt > 0.0;
		df > 0.0;
	Postconditions:
		result -> xmin == tmin;		result -> ymin == fmin;
		result -> xmax == tmax;		result -> ymax == fmax;
		result -> nx == nt;			result -> ny == nf;
		result -> dx == dt;			result -> dy == df;
		result -> x1 == t1;			result -> y1 == f1;
		result -> z [1..nf] [1..nt] == 0.0;
*/

void Spectrogram_paintInside (Spectrogram me, Graphics g,
	double tmin, double tmax, double fmin, double fmax, double maximum, int autoscaling,
	double dynamicRange, double preemphasis, double dynamicCompression);
void Spectrogram_paint (Spectrogram me, Graphics g,
	double tmin, double tmax, double fmin, double fmax, double maximum, int autoscaling,
	double dynamicRange, double preemphasis, double dynamicCompression,
	bool garnish);
/*
	Function:
		Draw me to a Graphics.
		If tmax <= tmin, draw all time samples.
		If fmax <= fmin, draw all frequency bands.
	Arguments:
		dynamicRange (dB): the difference between the maximum intensity and the lowest visible intensity.
		preemphasis (dB/octave): high-pass filtering.
		dynamicCompression (0-1):
			the amount by which weaker frames are enhanced in the direction of the strongest frame;
			0 = no compression, 1 = complete compression (all frames shown equally strong).
		garnish:
			a boolean that determines if a box, ticks, numbers, and text are written in the margins.
*/

autoSpectrogram Matrix_to_Spectrogram (Matrix me);
/*
	Create a Spectrogram from a Matrix,
	with deep copy of all its attributes, except class information and methods.
*/

autoMatrix Spectrogram_to_Matrix (Spectrogram me);
/*
	Create a Matrix from a Spectrogram,
	with deep copy of all its attributes, except class information and methods.
*/

/* End of file Spectrogram.h */
#endif
