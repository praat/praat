/* Excitation_to_Formant.cpp
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

#include "Excitation_to_Formant.h"

autoFormant Excitation_to_Formant (Excitation me, integer maximumNumberOfFormants) {
	try {
		integer numberOfFrequencies = my nx, nform = 0;
		double *p = & my z [1] [0];

		autoFormant thee = Formant_create (0.0, 1.0, 1, 1.0, 0.5, maximumNumberOfFormants);
		thy d_frames [1]. formant = NUMvector <structFormant_Formant> (1, maximumNumberOfFormants);
		for (integer i = 2; i < numberOfFrequencies; i ++)
			if (p [i] > p [i - 1] && p [i] >= p [i + 1]) {
				double firstDerivative = p [i+1] - p [i-1], secondDerivative = 2 * p [i] - p [i-1] - p [i+1];
				integer j;
				Formant_Formant formant = & thy d_frames [1]. formant [++ nform];
				formant -> frequency = Excitation_barkToHertz (
					my x1 + my dx * (i - 1 + 0.5 * firstDerivative / secondDerivative));
				double min3phon = p [i] + 0.125 * firstDerivative * firstDerivative / secondDerivative - 3.0;
				/* Search left. */
				j = i - 1; while (p [j] > min3phon && j > 1) j --;
				double left = Excitation_barkToHertz (
					p [j] > min3phon ? my xmin : my x1 + my dx * (j - 1 + (min3phon - p [j]) / (p [j + 1] - p [j])));
				 /* Search right. */
				j = i + 1; while (p [j] > min3phon && j < numberOfFrequencies) j ++;
				double right = Excitation_barkToHertz (
					p [j] > min3phon ? my xmax : my x1 + my dx * (j - 1 - (min3phon - p [j]) / (p [j - 1] - p [j])));
				formant -> bandwidth = right - left;
				if (nform == thy maxnFormants) break;
			}
		thy d_frames [1]. nFormants = nform;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Formant.");
	}
}

/* End of file Excitation_to_Formant.cpp */
