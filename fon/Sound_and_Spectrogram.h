/* Sound_and_Spectrogram.h
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2007/08/13
 */

#ifndef _Sound_h_
	#include "Sound.h"
#endif
#ifndef _Spectrogram_h_
	#include "Spectrogram.h"
#endif

#define Sound_to_Spectrogram_WINDOW_SHAPE_SQUARE  0
#define Sound_to_Spectrogram_WINDOW_SHAPE_HAMMING  1
#define Sound_to_Spectrogram_WINDOW_SHAPE_BARTLETT  2
#define Sound_to_Spectrogram_WINDOW_SHAPE_WELCH  3
#define Sound_to_Spectrogram_WINDOW_SHAPE_HANNING  4
#define Sound_to_Spectrogram_WINDOW_SHAPE_GAUSSIAN  5

const wchar_t * Sound_to_Spectrogram_windowShapeText (int windowShape);

Spectrogram Sound_to_Spectrogram (Sound me, double effectiveAnalysisWidth, double fmax,
	double minimumTimeStep1, double minimumFreqStep1, int windowShape,
	double maximumTimeOversampling, double maximumFreqOversampling);

Sound Spectrogram_to_Sound (Spectrogram me, double fsamp);

/* End of Sound_and_Spectrogram.h */
