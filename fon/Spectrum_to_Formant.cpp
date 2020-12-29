/* Spectrum_to_Formant.cpp
 *
 * Copyright (C) 1992-2011,2015-2017,2019,2020 Paul Boersma
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
		integer nfreq = my nx;
		autoVEC power = zero_VEC (nfreq);
		constexpr double fakeDuration = 1.0;
		constexpr double fakeTimeStep = 1.0;
		autoFormant thee = Formant_create (0.0, fakeDuration, 1, fakeTimeStep, 0.5 * fakeDuration, maxnFormants);
		for (integer i = 1; i <= nfreq; i ++)
			power [i] = my z [1] [i] * my z [1] [i] + my z [2] [i] * my z [2] [i];
		for (integer i = 2; i < nfreq; i ++)
			if (power [i] > power [i - 1] && power [i] >= power [i + 1]) {
				const double firstDerivative = power [i + 1] - power [i - 1];
				const double secondDerivative = 2.0 * power [i] - power [i - 1] - power [i + 1];
				const Formant_Formant formant = thy frames [1]. formant. append();
				formant -> frequency = my dx * (i - 1 + 0.5 * firstDerivative / secondDerivative);
				const double min3dB = 0.5 * (power [i] + 0.125 * firstDerivative * firstDerivative / secondDerivative);
				/*
					Search to the left.
				*/
				integer j = i - 1;
				while (power [j] > min3dB && j > 1)
					j --;
				if (power [j] > min3dB)
					formant -> bandwidth = formant -> frequency;
				else
					formant -> bandwidth =
							formant -> frequency -
							my dx * (j - 1 + (min3dB - power [j]) / (power [j + 1] - power [j]));
				/*
					Search to the right.
				*/
				j = i + 1;
				while (power [j] > min3dB && j < nfreq)
					j ++;
				if (power [j] > min3dB)
					formant -> bandwidth += my xmax - formant -> frequency;
				else
					formant -> bandwidth +=
							my dx * (j - 1 - (min3dB - power [j]) / (power [j - 1] - power [j])) -
							formant -> frequency;
				if (thy frames [1]. formant.size == maxnFormants)
					break;
			}
		thy frames [1]. numberOfFormants = thy frames [1]. formant.size;   // maintain invariant
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Formant.");
	}
}

/* End of file Spectrum_to_Formant.cpp */
