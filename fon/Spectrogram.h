#ifndef _Spectrogram_h_
#define _Spectrogram_h_
/* Spectrogram.h
 *
 * Copyright (C) 1992-2011 David Weenink & Paul Boersma
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
 * pb 2011/07/11
 */

#include "Matrix.h"
#include "Graphics.h"

#ifdef __cplusplus
	extern "C" {
#endif

Thing_declare1cpp (Spectrogram);
struct structSpectrogram : public structMatrix {
};
#define Spectrogram__methods(klas)  Matrix__methods(klas)
Thing_declare2cpp (Spectrogram, Matrix);

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

Spectrogram Spectrogram_create (double tmin, double tmax, long nt, double dt, double t1,
					double fmin, double fmax, long nf, double df, double f1);
/*
	Function:
		Create the spectrogram data structure.
	Preconditions:
		nt > 0;
		nf > 0;
		dt > 0.0;
		df > 0.0;
	Postconditions:
		result -> xmin = tmin;		result -> ymin = fmin;
		result -> xmax = tmax;		result -> ymax = fmax;
		result -> nx = nt;			result -> ny = nf;
		result -> dx = dt;			result -> dy = df;
		result -> x1 = t1;			result -> y1 = f1;
		result -> z [1..nf] [1..nt] = 0.0;
*/

void Spectrogram_paintInside (I, Graphics g,
	double tmin, double tmax, double fmin, double fmax, double maximum, int autoscaling,
	double dynamicRange, double preemphasis, double dynamicCompression);
void Spectrogram_paint (I, Graphics g,
	double tmin, double tmax, double fmin, double fmax, double maximum, int autoscaling,
	double dynamicRange, double preemphasis, double dynamicCompression,
	int garnish);
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

Spectrogram Matrix_to_Spectrogram (I);
/*
	Create a Spectrogram from a Matrix,
	with deep copy of all its attributes, except class information and methods.
	Return NULL if out of memory.  
*/

Matrix Spectrogram_to_Matrix (I);
/*
	Create a Matrix from a Spectrogram,
	with deep copy of all its attributes, except class information and methods.
	Return NULL if out of memory.  
*/

#ifdef __cplusplus
	}
#endif

/* End of file Spectrogram.h */
#endif
