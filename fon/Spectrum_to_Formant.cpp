/* Spectrum_to_Formant.cpp
 *
 * Copyright (C) 1992-2011,2015,2017 Paul Boersma
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

#include "Spectrum_to_Formant.h"

autoFormant Spectrum_to_Formant (Spectrum me, int maxnFormants) {
	try {
		integer nfreq = my nx, nform = 0;
		autoNUMvector <double> p (1, nfreq);   // power
		autoFormant thee = Formant_create (0, 1, 1, 1, 0.5, maxnFormants);
		thy d_frames [1]. formant = NUMvector <structFormant_Formant> (1, maxnFormants);
		for (integer i = 1; i <= nfreq; i ++)
			p [i] = my z [1] [i] * my z [1] [i] + my z [2] [i] * my z [2] [i];
		for (integer i = 2; i < nfreq; i ++)
			if (p [i] > p [i - 1] && p [i] >= p [i + 1]) {
				double firstDerivative = p [i+1] - p [i-1], secondDerivative = 2 * p [i] - p [i-1] - p [i+1];
				Formant_Formant formant = & thy d_frames [1]. formant [++ nform];
				formant -> frequency = my dx * (i - 1 + 0.5 * firstDerivative / secondDerivative);
				double min3dB = 0.5 * (p [i] + 0.125 * firstDerivative * firstDerivative / secondDerivative);
				/* Search left. */
				integer j = i - 1; while (p [j] > min3dB && j > 1) j --;
				if (p [j] > min3dB)
					formant -> bandwidth = formant -> frequency;
				else
					formant -> bandwidth = formant -> frequency - my dx * (j - 1 + (min3dB - p [j]) / (p [j + 1] - p [j]));
				 /* Search right. */
				j = i + 1; while (p [j] > min3dB && j < nfreq) j ++;
				if (p [j] > min3dB)
					formant -> bandwidth += my xmax - formant -> frequency;
				else
					formant -> bandwidth += my dx * (j - 1 - (min3dB - p [j]) / (p [j - 1] - p [j])) - formant -> frequency;
				if (nform == maxnFormants) break;
			}
		thy d_frames [1]. nFormants = nform;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Formant.");
	}
}

/* End of file Spectrum_to_Formant.cpp */
