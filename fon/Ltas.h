#ifndef _Ltas_h_
#define _Ltas_h_
/* Ltas.h
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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

#include "Spectrum.h"
#include "Sound.h"
#include "PointProcess.h"
#include "Collection.h"

Thing_define (Ltas, Vector) {
	void v_info ()
		override;
	int v_domainQuantity ()
		override { return MelderQuantity_FREQUENCY_HERTZ; }
	double v_convertStandardToSpecialUnit (double value, integer level, int unit)
		override;
	double v_convertSpecialToStandardUnit (double value, integer level, int unit)
		override;
};

/*
	Attributes:
		xmin				// Minimum frequency (Hz).
		xmax > xmin		// Maximum frequency (Hz).
		nx >= 1			// Number of bands.
		dx > 0.0			// Band width (Hz).
		x1				// Centre of first band (Hz).
		ymin, ymax, dy, y1 = 1.0
		ny = 1
		z [1] [1..nx]		// The intensity per band, in dB/Hz.
*/

autoLtas Ltas_create (integer nx, double dx);
/*
	Function:
		create an Ltas.
	Preconditions:
		nx >= 1;
		dx > 0.0;
	Postconditions:
		my xmin == 0.0;              my ymin == 1;
		my xmax == nx * dx;        my ymax == 1;
		my nx == nx;               my ny == 1;
		my dx == dx;               my dy == 1;
		my x1 == 0.5 * dx;         my y1 == 1;
		my z [1] [1..nx] == 1e-4; // straight tube, area 1 cm2.
 */

void Ltas_draw (Ltas me, Graphics g, double fmin, double fmax,
	double minimum, double maximum, bool garnish, conststring32 method);

autoMatrix Ltas_to_Matrix (Ltas me);
autoLtas Matrix_to_Ltas (Matrix me);

Collection_define (LtasBag, CollectionOf, Ltas) {
};

autoLtas Ltases_merge (LtasBag ltases);
autoLtas Ltases_average (LtasBag ltases);

autoLtas Ltas_computeTrendLine (Ltas me, double fmin, double fmax);
autoLtas Ltas_subtractTrendLine (Ltas me, double fmin, double fmax);

/* Direct computations. */

autoLtas Spectrum_to_Ltas (Spectrum me, double bandwidth);
autoLtas Spectrum_to_Ltas_1to1 (Spectrum me);
autoLtas PointProcess_Sound_to_Ltas (PointProcess pulses, Sound sound,
	double maximumFrequency, double bandWidth,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor);
autoLtas PointProcess_Sound_to_Ltas_harmonics (PointProcess pulses, Sound sound,
	integer maximumHarmonic,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor);

/* Shortcuts. */

autoLtas Sound_to_Ltas (Sound me, double bandwidth);
autoLtas Sound_to_Ltas_pitchCorrected (Sound sound, double minimumPitch, double maximumPitch,
	double maximumFrequency, double bandWidth,
	double shortestPeriod, double longestPeriod, double maximumPeriodFactor);

double Ltas_getSlope (Ltas me, double f1min, double f1max, double f2min, double f2max, int averagingUnits);
double Ltas_getLocalPeakHeight (Ltas me, double environmentMin, double environmentMax, double peakMin, double peakMax, int averagingUnits);

/* End of file Ltas.h */
#endif
