#ifndef _Intensity_extensions_h_
#define _Intensity_extensions_h_
/* Intensity_extensions.h
 *
 * Copyright (C) 2006-2011 David Weenink
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
 djmw 20061204 Initial version
 djmw 20110307 Latewst modification
*/

#ifndef _Sound_h_
	#include "Sound.h"
#endif
#ifndef _Intensity_h_
	#include "Intensity.h"
#endif
#ifndef _TextGrid_h_
	#include "TextGrid.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

TextGrid Intensity_to_TextGrid_detectSilences (Intensity me, double silenceThreshold_dB, 
	double minSilenceDuration, double minSoundingDuration, wchar_t *silenceLabel, wchar_t *soundingLabel);
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

#ifdef __cplusplus
	}
#endif

#endif /* _Intensity_extensions_h_ */
