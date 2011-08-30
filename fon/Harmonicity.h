#ifndef _Harmonicity_h_
#define _Harmonicity_h_
/* Harmonicity.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "Vector.h"

Thing_define (Harmonicity, Vector) {
	// overridden methods:
	protected:
		virtual void v_info ();
		virtual int v_domainQuantity () { return MelderQuantity_TIME_SECONDS; }
};

/* Attributes:
		xmin				// Start time (seconds).
		xmax > xmin		// End time (seconds).
		nx >= 1			// Number of time slices.
		dx > 0.0			// Time step (seconds).
		x1				// Centre of first time slice (seconds).
		ymin, ymax, ny, dy, y1 = 1
		z [1] [1..nt]
			// The harmonic strength, a real number between -150 dB and +150 dB:
			// -150 dB means not periodic at all, +150 dB means perfectly periodic.
			// Normal values for speech are between -10 dB for [s] and +30 dB for [u].
			// -200 means silence.
*/

Harmonicity Harmonicity_create (double tmin, double tmax, long nt, double dt, double t1);

void Harmonicity_draw (Harmonicity me, double tmin, double tmax, double min, double max);
/*
	draw a harmonicity contour into the current Graphics.
	If tmax <= tmin, draw whole time domain.
	If max <= min, scale to extrema.
*/

double Harmonicity_getMean (Harmonicity me, double tmin, double tmax);
double Harmonicity_getStandardDeviation (Harmonicity me, double tmin, double tmax);
double Harmonicity_getQuantile (Harmonicity me, double quantile);

Matrix Harmonicity_to_Matrix (Harmonicity me);
Harmonicity Matrix_to_Harmonicity (Matrix me);

/* End of file Harmonicity.h */
#endif
