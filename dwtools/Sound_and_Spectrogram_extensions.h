#ifndef _Sound_and_Spectrogram_extensions_h_
#define _Sound_and_Spectrogram_extensions_h_
/* Sound_and_Spectrogram_extensions.h
 *
 * Copyright (C) 2014-2015 David Weenink
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
 djmw 20140914
*/

#include "Spectrogram_extensions.h"
#include "Pitch.h"
#include "Sound.h"

autoBarkSpectrogram Sound_to_BarkSpectrogram (Sound me, double analysisWidth, double dt,
	double f1_bark, double fmax_bark, double df_bark);
/*
	Filtering with filters on a Bark scale as defined by
		Andrew Sekey & Brian Hanson (1984), "Improved 1-Bark bandwidth
		"auditory filter", Jasa 75, 1902-1904.
	Although not explicitely stated the filter function is defined in the
	power domain.
	10 log F(z) = 15.8 + 7.5(z + 0.5) - 17.5 * sqrt(1 + (z + 0.5)^2)
*/

autoMelSpectrogram Sound_to_MelSpectrogram (Sound me, double analysisWidth, double dt,
	double f1_mel, double fmax_mel, double df_mel);

autoSpectrogram Sound_to_Spectrogram_pitchDependent (Sound me, double analysisWidth,
	double dt, double f1_hz, double fmax_hz, double df_hz, double relative_bw,
	double minimumPitch, double maximumPitch);

autoSpectrogram Sound_Pitch_to_Spectrogram (Sound me, Pitch thee,
	double analysisWidth, double dt, double f1_hz, double fmax_hz,
	double df_hz, double relative_bw);

autoSound BandFilterSpectrograms_crossCorrelate (BandFilterSpectrogram me, BandFilterSpectrogram thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain);

autoSound BandFilterSpectrograms_convolve (BandFilterSpectrogram me, BandFilterSpectrogram thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain);


#endif /* _Sound_and_Spectrogram_extensions_h_ */
