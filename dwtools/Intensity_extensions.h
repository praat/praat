#ifndef _Intensity_extensions_h_
#define _Intensity_extensions_h_
/* Intensity_extensions.h
 *
 * Copyright (C) 2006-2017 David Weenink
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
 djmw 20061204 Initial version
 djmw 20110307 Latewst modification
*/

#include "Sound.h"
#include "Intensity.h"
#include "IntensityTier.h"
#include "TextGrid.h"

autoTextGrid Intensity_to_TextGrid_detectSilences (Intensity me, double silenceThreshold_dB, double minSilenceDuration,
	double minSoundingDuration, conststring32 silenceLabel, conststring32 soundingLabel);
/*
	Marks "silence" intervals in a sound as intervals in a TextGrid.
	silenceThreshold_dB: silence-to-speech and speech-to-silence threshold as dB's below maximum intensity
		We have speech if:
			local_intensity >= max_intensity - silenceThreshold.
		Silence if:
			local_intensity < max_intensity - silenceThreshold.
	If minSilenceDuration > 0 then only intervals with a duration > minSilenceDuration will be labelled
	as silences in the IntervalTier.
*/

autoIntensity IntensityTier_to_Intensity (IntensityTier me, double dt);

autoTextGrid IntensityTier_to_TextGrid_detectSilences (IntensityTier me, double dt,
	double silenceThreshold_dB, double minSilenceDuration, double minSoundingDuration,
	conststring32 silenceLabel, conststring32 soundingLabel
);

#endif /* _Intensity_extensions_h_ */
