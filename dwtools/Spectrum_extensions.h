#ifndef _Spectrum_extensions_h_
#define _Spectrum_extensions_h_
/* Spectrum_extensions.h
 *
 * Copyright (C) 1993-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20010114
 djmw 20020813 GPL header
 djmw 20121022 Latest modification
*/

#include "Spectrum.h"
#include "Sound.h"
#include "Graphics.h"

Spectrum Spectrum_resample (Spectrum me, long numberOfFrequencies);

Spectrum Spectrum_compressFrequencyDomain (Spectrum me, double fmax, long interpolationDepth, int freqscale, int method);
Spectrum Spectrum_shiftFrequencies (Spectrum me, double shiftBy, double newMaximumFrequency, long interpolationDepth);
// Shift will be plusminus dx/2

Matrix Spectrum_unwrap (Spectrum me);
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
	double phase_min, double phase_max, int unwrap, int garnish);

Spectrum Spectra_multiply (Spectrum me, Spectrum thee);
void Spectrum_conjugate (Spectrum me);

#endif /* _Spectrum_extensions_h_ */
