#ifndef _Sound_and_Spectrum_dft_h_
#define _Sound_and_Spectrum_dft_h_
/* Sound_and_Spectrum_dft.h
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

#include "Sound.h"
#include "Spectrum.h"

/*
	Speed up the DFT calculations by using the FFT on a resampled signal that has 
	exactly a power of 2 samples.
	(after a suggestion by Ton Wempe).
*/
autoSpectrum Sound_to_Spectrum_resampled (Sound me, integer interpolationDepth);

autoSound Spectrum_to_Sound_resampled (Spectrum me, integer interpolationDepth);

#endif /* _Sound_and_Spectrum_dft_h_ */
