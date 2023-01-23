/* AnalyticSound.cpp
 *
 * Copyright (C) 2021-2022 David Weenink
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

#include "AnalyticSound.h"
#include "Sound_and_Spectrum.h"
#include "Spectrum_extensions.h"

Thing_implement (AnalyticSound, Sound, 0);

double structAnalyticSound :: v_getValueAtSample (integer isamp, integer which, int /* unit */) const {
	/*
		Preconditions: 
			1 <= isample <= nx 
			0 <= which <= ny
	*/
	double value;
	if (which == 0) 
		value = sqrt (sqr (z [1] [isamp]) + sqr (z [2] [isamp]));
	else
		value = z [which] [isamp];
	return value;
}

autoAnalyticSound AnalyticSound_create (double xmin, double xmax, integer nx, double dx, double x1) {
	try {
		autoAnalyticSound me = Thing_new (AnalyticSound);
		Matrix_init (me.get(), xmin, xmax, nx, dx, x1, 1.0, 2.0, 2, 1.0, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"AnalyticSound not created.");
	}
}

static void Spectrum_into_AnalyticSound (Spectrum me, AnalyticSound thee) {
	try {
		autoSound him = Spectrum_to_Sound (me);
		const integer nx = std::min (his nx, thy nx);
		thy z.row (1).part (1, nx)  <<=  his z.row (1).part (1, nx);
		Spectrum_shiftPhaseBy90Degrees (me);
		him = Spectrum_to_Sound (me);
		thy z.row (2).part (1, nx)  <<=  his z.row (1).part (1, nx);
		Spectrum_unshiftPhaseBy90Degrees (me); // restore original
	} catch (MelderError) {
		Melder_throw (me, U": cannot ", thee);
	}
}

void Sound_into_AnalyticSound (Sound me, AnalyticSound thee) {
	try {
		const integer nx = std::min (my nx, thy nx);
		/*
			Calculate the Hilbert transform
			20220208: the DFT can be very slow for prime numbers, wait for FFTW?
		*/
		autoSpectrum spectrum = Sound_to_Spectrum (me, false);
		Spectrum_shiftPhaseBy90Degrees (spectrum.get());
		autoSound hilbert = Spectrum_to_Sound (spectrum.get());
		thy z.row (1).part (1, nx)  <<=  my z.row (1).part (1, nx);
		thy z.row (2).part (1, nx)  <<=  hilbert -> z.row (1).part (1, nx);
	} catch (MelderError) {
		Melder_throw (me, U": could not convert to AnalyticSound ", thee);
	}
}

autoAnalyticSound Sound_to_AnalyticSound (Sound me) {
	try {
		autoAnalyticSound thee = AnalyticSound_create (my xmin, my xmax, my nx, my dx, my x1);
		autoSpectrum spectrum = Sound_to_Spectrum (me, false);
		Sound_into_AnalyticSound (me, thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not create AnalyticSound.");
	}
}

autoSound AnalyticSound_to_Sound (AnalyticSound me) {
	try {
		auto thee = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1);
		thy z.row (1)  <<=  my z.row (1);
		thy z.row (2)  <<=  my z.row (2);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not create AnalyticSound.");
	}
}

autoIntensity AnalyticSound_to_Intensity (AnalyticSound me) {
	try {
		autoIntensity thee = Intensity_create (my xmin, my xmax, my nx, my dx, my x1);
		for (integer i = 1; i <= my nx; i ++) {
			const double power_rehearingThreshold = std:: max (sqr (my v_getValueAtSample (i, 0, 0)) / 4.0e-10 , 1e-30);
			thy z [1] [i] = 10.0 * log10 (power_rehearingThreshold);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not create Intensity.");
	}
}
/* End of file AnalyticSound.h */
