#ifndef _Spectrum_extensions_h_
#define _Spectrum_extensions_h_
/* Spectrum_extensions.h
 *
 * Copyright (C) 1993-2019 David Weenink
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

/*
 djmw 20010114
 djmw 20020813 GPL header
 djmw 20121022 Latest modification
*/

#include "Spectrum.h"
#include "Sound.h"
#include "Graphics.h"

autoSpectrum Spectrum_resample (Spectrum me, integer numberOfFrequencies);

autoSpectrum Spectrum_compressFrequencyDomain (Spectrum me, double fmax, integer interpolationDepth, int freqscale, int method);

autoSpectrum Spectrum_shiftFrequencies (Spectrum me, double shiftBy, double newMaximumFrequency, integer interpolationDepth);
// Shift will be plusminus dx/2

autoMatrix Spectrum_unwrap (Spectrum me);
/*
	Unwrap the phases of the spectrum according to an algorithm by
	Tribolet as published in:

	Tribolet, J.M. & Quatieri, T.F. (1979), Computation of the Complex
		Spectrum, in: Programs for Digital Signal Processing,
		Digital Signal Processing Commitee (eds), IEEE Press,
		chapter 7.1.

	First row of returned matrix contains the amplitudes-squared,
	second row contains the unwrapped phases.
*/

void Spectrum_drawPhases (Spectrum me, Graphics g, double fmin, double fmax,
	double phase_min, double phase_max, int unwrap, bool garnish);

autoSpectrum Spectra_multiply (Spectrum me, Spectrum thee);

void Spectrum_conjugate (Spectrum me);

void Spectrum_getMaximumInInterval (Spectrum me, double fromFrequencyHz, double toFrequencyHz, double *frequency, double *amplitude_dB);


#endif /* _Spectrum_extensions_h_ */
