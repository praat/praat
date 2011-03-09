#ifndef _Ltas_h_
#define _Ltas_h_
/* Ltas.h
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

/*
 * pb 2011/03/03
 */

#ifndef _Spectrum_h_
	#include "Spectrum.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif
#ifndef _Vector_h_
	#include "Vector.h"
#endif
#ifndef _PointProcess_h_
	#include "PointProcess.h"
#endif
#ifndef _Collection_h_
	#include "Collection.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

#define Ltas_members  Vector_members
#define Ltas_methods  Vector_methods
class_create (Ltas, Vector);

/*
	Attributes:
		xmin				// Minimum frequency (Hz).
		xmax > xmin		// Maximum frequency (Hz).
		nx >= 1			// Number of bands.
		dx > 0.0			// Band width (Hz).
		x1				// Centre of first band (Hz).
		ymin, ymax, ny, dy, y1 = 1
		z [1] [1..nx]		// The intensity per band, in db/Hz.
*/

Ltas Ltas_create (long nx, double dx);
/*
	Function:
		create an Ltas, or NULL if out of memory.
	Preconditions:
		nx >= 1;
		dx > 0.0;
	Postconditions:
		my xmin == 0;              my ymin == 1;
		my xmax == nx * dx;        my ymax == 1;
		my nx == nx;               my ny == 1;
		my dx == dx;               my dy == 1;
		my x1 == 0.5 * dx;         my y1 == 1;
		my z [1] [1..nx] == 1e-4; // straight tube, area 1 cm2.
 */

void Ltas_draw (Ltas me, Graphics g, double fmin, double fmax,
	double minimum, double maximum, int garnish, const wchar_t *method);

Matrix Ltas_to_Matrix (Ltas me);
Ltas Matrix_to_Ltas (Matrix me);

Ltas Ltases_merge (Collection ltases);
Ltas Ltases_average (Collection ltases);

Ltas Ltas_computeTrendLine (Ltas me, double fmin, double fmax);
Ltas Ltas_subtractTrendLine (Ltas me, double fmin, double fmax);

/* Direct computations. */

Ltas Spectrum_to_Ltas (Spectrum me, double bandwidth);
Ltas Spectrum_to_Ltas_1to1 (Spectrum me);
Ltas PointProcess_Sound_to_Ltas (PointProcess pulses, Sound sound,
	double maximumFrequency, double bandWidth,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor);
Ltas PointProcess_Sound_to_Ltas_harmonics (PointProcess pulses, Sound sound,
	long maximumHarmonic,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor);

/* Shortcuts. */

Ltas Sound_to_Ltas (Sound me, double bandwidth);
Ltas Sound_to_Ltas_pitchCorrected (Sound sound, double minimumPitch, double maximumPitch,
	double maximumFrequency, double bandWidth,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor);

double Ltas_getSlope (Ltas me, double f1min, double f1max, double f2min, double f2max, int averagingUnits);
double Ltas_getLocalPeakHeight (Ltas me, double environmentMin, double environmentMax, double peakMin, double peakMax, int averagingUnits);

#ifdef __cplusplus
	}
#endif

/* End of file Ltas.h */
#endif
