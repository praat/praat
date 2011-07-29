#ifndef _Sound_and_Spectrogram_h_
#define _Sound_and_Spectrogram_h_
/* Sound_and_Spectrogram.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "Sound.h"
#include "Spectrogram.h"

#include "Sound_and_Spectrogram_enums.h"

Spectrogram Sound_to_Spectrogram (Sound me, double effectiveAnalysisWidth, double fmax,
	double minimumTimeStep1, double minimumFreqStep1, enum kSound_to_Spectrogram_windowShape windowShape,
	double maximumTimeOversampling, double maximumFreqOversampling);

Sound Spectrogram_to_Sound (Spectrogram me, double fsamp);

/* End of Sound_and_Spectrogram.h */
#endif
