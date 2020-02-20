/* Roots_to_Spectrum.cpp
 *
 * Copyright (C) 2020 David Weenink
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

#include "Roots_to_Spectrum.h"

autoSpectrum Roots_to_Spectrum (Roots me, double nyquistFrequency, integer numberOfFrequencies, double radius) {
	try {
		Melder_require (numberOfFrequencies > 1,
			U"Number of frequencies should be greater than 1.");
		autoSpectrum thee = Spectrum_create (nyquistFrequency, numberOfFrequencies);

		const double phi = NUMpi / (numberOfFrequencies - 1);
		dcomplex z;
		for (integer i = 1; i <= numberOfFrequencies; i ++) {
			z. real (radius * cos ((i - 1) * phi));
			z. imag (radius * sin ((i - 1) * phi));
			const dcomplex s = Roots_evaluate_z (me, z);
			thy z [1] [i] = s.real();
			thy z [2] [i] = s.imag();
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Spectrum calculated.");
	}
}

/* End of file Roots.cpp */
