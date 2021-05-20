/* AnalyticSound.cpp
 *
 * Copyright (C) 2021 David Weenink
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

autoAnalyticSound AnalyticSound_create (double xmin, double xmax, integer nx, double dx, double x1) {
	try {
		autoAnalyticSound me = Thing_new (AnalyticSound);
		Matrix_init (me.get(), xmin, xmax, nx, dx, x1, 1.0, 2.0, 2, 1.0, 1.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"AnalyticSound not created.");
	}
}

autoAnalyticSound Sound_to_AnalyticSound (Sound me) {
	try {
		autoAnalyticSound thee = AnalyticSound_create (my xmin, my xmax, my nx, my dx, my x1);
		autoSpectrum spectrum = Sound_to_Spectrum (me, true);
		Spectrum_shiftPhaseBy90Degrees (spectrum.get());
		autoSound sound90 = Spectrum_to_Sound (spectrum.get());
		thy z.row (1)  <<=  my z.row (1);
		thy z.row (2)  <<=  sound90 -> z.row (1).part (1, my nx);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not create AnalyticSound.");
	}
}

autoIntensity AnalyticSound_to_Intensity (AnalyticSound me) {
	try {
		autoIntensity thee = Intensity_create (my xmin, my xmax, my nx, my dx, my x1);
		for (integer i = 1; i <= my nx; i ++) {
			double power_rehearingThreshold = (sqr ( my z [1] [i]) + sqr (my z [2] [i])) / 4.0e-10;
			thy z [1][i] = ( power_rehearingThreshold > 1e-30 ? 10.0 * log10 (power_rehearingThreshold) : -300.0 );
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not create Intensity.");
	}
}
/* End of file AnalyticSound.h */
