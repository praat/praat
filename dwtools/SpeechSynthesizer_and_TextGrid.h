#ifndef _SpeechSynthesizer_and_TextGrid_h_
#define _SpeechSynthesizer_and_TextGrid_h_
/* SpeechSynthesizer_and_TextGrid.h
 *
 * Copyright (C) 2011-2012 David Weenink
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
	djmw 20111214
*/

#include "SpeechSynthesizer.h"
#include "TextGrid_extensions.h"
#include "EditDistanceTable.h"

autoSound SpeechSynthesizer_and_TextInterval_to_Sound (SpeechSynthesizer me, TextInterval thee, autoTextGrid *tg);

autoSound SpeechSynthesizer_and_TextGrid_to_Sound (SpeechSynthesizer me, TextGrid thee, long itier, long iiint, autoTextGrid *tg);

autoTextGrid SpeechSynthesizer_and_Sound_and_TextInterval_align (SpeechSynthesizer me, Sound thee, TextInterval him,
	double silenceThreshold, double minSilenceDuration, double minSoundingDuration);
autoTextGrid SpeechSynthesizer_and_Sound_and_IntervalTier_align (SpeechSynthesizer me, Sound thee, IntervalTier him,
	long istart, long iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration);
autoTextGrid SpeechSynthesizer_and_Sound_and_TextGrid_align (SpeechSynthesizer me, Sound thee, TextGrid him,
	long tierNumber, long istart, long iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration);
autoTextGrid SpeechSynthesizer_and_Sound_and_TextGrid_align2 (SpeechSynthesizer me, Sound thee, TextGrid him, long tierNumber, long istart, long iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration, double trimDuration);

autoTable IntervalTiers__to_Table_textAlignmentment (IntervalTier target, IntervalTier source, EditCostsTable costs);
autoTable TextGrids_to_Table_textAlignmentment (TextGrid target, long ttier, TextGrid source, long stier, EditCostsTable costs);

/* For testing purposes only */
autoTextGrid TextGrid_and_IntervalTier_patch (TextGrid me, IntervalTier thee, const char32 *patchLabel, double precision);
autoTextGrid TextGrid_and_IntervalTier_cutPartsMatchingLabel (TextGrid me, IntervalTier thee, const char32 *label, double precision);

#endif // _SpeechSynthesizer_and_TextGrid_h_
