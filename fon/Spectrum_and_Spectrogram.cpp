/* Spectrum_and_Spectrogram.cpp
 *
 * Copyright (C) 1992-2011,2014,2015,2017 David Weenink & Paul Boersma
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

/*
 * pb 1998/04/18
 * pb 2002/07/16 GPL
 * pb 2011/06/10 C++
 */

#include "Spectrum_and_Spectrogram.h"

autoSpectrum Spectrogram_to_Spectrum (Spectrogram me, double tim) {
	try {
		autoSpectrum thee = Spectrum_create (my ymax, my ny);
		/* Override stupid Spectrum values. */
		thy xmin = my ymin;
		thy xmax = my ymax;
		thy x1 = my y1;   // centre of first band, instead of 0 (makes it unFFTable)
		thy dx = my dy;   // frequency step
		integer itime = Sampled_xToNearestIndex (me, tim);
		if (itime < 1 ) itime = 1;
		if (itime > my nx) itime = my nx;
		for (integer ifreq = 1; ifreq <= my ny; ifreq ++) {
			double value = my z [ifreq] [itime];
			if (value < 0.0)
				Melder_throw (U"Negative values in spectrogram.");
			thy z [1] [ifreq] = sqrt (value);
			thy z [2] [ifreq] = 0.0;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": spectral slice not extracted.");
	}
}

autoSpectrogram Spectrum_to_Spectrogram (Spectrum me) {
	try {
		autoSpectrogram thee = Spectrogram_create (0, 1, 1, 1, 0.5, my xmin, my xmax, my nx, my dx, my x1);
		for (integer i = 1; i <= my nx; i ++)
			thy z [i] [1] = my z [1] [i] * my z [1] [i] + my z [2] [i] * my z [2] [i];
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Spectrogram.");
	}
}

/* End of file Spectrum_and_Spectrogram.cpp */
