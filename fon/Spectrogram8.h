#ifndef _Spectrogram8_h_
#define _Spectrogram8_h_
/* Spectrogram8.h
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 1995/06/02
 * pb 2002/07/16 GPL
 */

/*
	class Spectrogram8 = Image;
*/
#ifndef _Image_h_
	#include "Image.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif

#define Spectrogram8_members  Image_members
#define Spectrogram8_methods  Image_methods
class_create (Spectrogram8, Image)

/* Attributes:
	xmin			// Starting time (seconds).
	xmax		// Total duration (seconds).
	nx			// Number of time slices.
	dx			// The time between two subsequent time slices.
	x1			// The centre of the first time slice.
	ymin			// Minimum frequency (Hertz).
	ymax		// Maximum frequency (Hertz).
	dy			// Frequency step (Hertz).
	y1			// Centre of first frequency band (Hertz).
	z [iy] [ix]		// Power spectrum density (dB/Hz * 2 + 40).
*/

Spectrogram8 Spectrogram8_create (double tmin, double tmax, long nt, double dt, double t1,
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
		result -> z [1..nf] [1..nt] = 0;
*/

void Spectrogram8_paintInside (I, Graphics g, double tmin, double tmax, double fmin, double fmax,
	double minimum, double maximum);
void Spectrogram8_paint (I, Graphics g,
	double tmin, double tmax, double fmin, double fmax,
	double minimum, double maximum,
	int garnish);
/*
	Function:
		Draw me to a Graphics.
		If tmax <= tmin, draw all time samples.
		If fmax <= fmin, draw all frequency bands.
	Arguments:
		garnish:
			a boolean that determines if a box, ticks, numbers, and text are written in the margins.
*/

/* End of file Spectrogram8.h */
#endif
