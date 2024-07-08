/* Roots_and_Formant.cpp
 *
 * Copyright (C) 1994-2024 David Weenink
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

#include "Formant.h"
#include "Roots.h"

void Roots_into_Formant_Frame (constRoots me, Formant_Frame thee, double samplingFrequency, double margin) {
	/*
		Determine the formants and bandwidths
	*/
	Melder_assert (my numberOfRoots == my roots.size); // check invariant
	thy formant.resize (0);
	const double nyquistFrequency = 0.5 * samplingFrequency;
	const double fLow = margin, fHigh = nyquistFrequency - margin;
	for (integer iroot = 1; iroot <= my numberOfRoots; iroot ++) {
		if (my roots [iroot].imag() < 0.0)
			continue;
		const double frequency = fabs (atan2 (my roots [iroot].imag(), my roots [iroot].real())) * nyquistFrequency / NUMpi;
		if (frequency >= fLow && frequency <= fHigh) {
			const double bandwidth = - log (norm (my roots [iroot])) * nyquistFrequency / NUMpi;
			Formant_Formant newff = thy formant . append ();
			newff -> frequency = frequency;
			newff -> bandwidth = bandwidth;
		}
	}
	thy numberOfFormants = thy formant.size; // maintain invariant
}

/* End  of file Roots_and_Formant.h */

