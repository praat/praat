/* Sound_and_Spectrum.h
 *
 * Copyright (C) 1992-2004 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2004/11/22
 */

#ifndef _Sound_h_
	#include "Sound.h"
#endif
#ifndef _Spectrum_h_
	#include "Spectrum.h"
#endif

Spectrum Sound_to_Spectrum_at (Sound me, double tim, double windowDuration, int windowType);

Spectrum Sound_to_Spectrum (Sound me, int fast);
Sound Spectrum_to_Sound (Spectrum me);

Spectrum Spectrum_lpcSmoothing (Spectrum me, int numberOfPeaks, double preemphasisFrequency);

Sound Sound_filter_passHannBand (Sound me, double fmin, double fmax, double smooth);
Sound Sound_filter_stopHannBand (Sound me, double fmin, double fmax, double smooth);
Sound Sound_filter_formula (Sound me, const char *formula);

/* End of file Sound_and_Spectrum.h */
