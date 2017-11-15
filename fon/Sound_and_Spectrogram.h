#ifndef _Sound_and_Spectrogram_h_
#define _Sound_and_Spectrogram_h_
/* Sound_and_Spectrogram.h
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

#include "Sound.h"
#include "Spectrogram.h"

#include "Sound_and_Spectrogram_enums.h"

autoSpectrogram Sound_to_Spectrogram (Sound me, double effectiveAnalysisWidth, double fmax,
	double minimumTimeStep1, double minimumFreqStep1, kSound_to_Spectrogram_windowShape windowShape,
	double maximumTimeOversampling, double maximumFreqOversampling);

autoSound Spectrogram_to_Sound (Spectrogram me, double fsamp);

/* End of Sound_and_Spectrogram.h */
#endif
