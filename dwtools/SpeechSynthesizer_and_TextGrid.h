#ifndef _SpeechSynthesizer_and_TextGrid_h_
#define _SpeechSynthesizer_and_TextGrid_h_
/* SpeechSynthesizer_and_TextGrid.h
 *
 * Copyright (C) 2011-2012 David Weenink
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
	djmw 20111214
*/

#include "SpeechSynthesizer.h"
#include "TextGrid_extensions.h"
#include "EditDistanceTable.h"

Sound SpeechSynthesizer_and_TextInterval_to_Sound (SpeechSynthesizer me, TextInterval thee, TextGrid *tg);

Sound SpeechSynthesizer_and_TextGrid_to_Sound (SpeechSynthesizer me, TextGrid thee, long itier, long iiint, TextGrid *tg);

TextGrid SpeechSynthesizer_and_Sound_and_TextInterval_align (SpeechSynthesizer me, Sound thee, TextInterval him,
	double silenceThreshold, double minSilenceDuration, double minSoundingDuration);
TextGrid SpeechSynthesizer_and_Sound_and_IntervalTier_align (SpeechSynthesizer me, Sound thee, IntervalTier him,
	long istart, long iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration);
TextGrid SpeechSynthesizer_and_Sound_and_TextGrid_align (SpeechSynthesizer me, Sound thee, TextGrid him,
	long tierNumber, long istart, long iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration);
TextGrid SpeechSynthesizer_and_Sound_and_TextGrid_align2 (SpeechSynthesizer me, Sound thee, TextGrid him, long tierNumber, long istart, long iend, double silenceThreshold, double minSilenceDuration, double minSoundingDuration, double trimDuration);

Table IntervalTiers__to_Table_textAlignmentment (IntervalTier target, IntervalTier source, EditCostsTable costs);
Table TextGrids_to_Table_textAlignmentment (TextGrid target, long ttier, TextGrid source, long stier, EditCostsTable costs);

/* For testing purposes only */
TextGrid TextGrid_and_IntervalTier_patch (TextGrid me, IntervalTier thee, const wchar_t *patchLabel, double precision);
TextGrid TextGrid_and_IntervalTier_cutPartsMatchingLabel (TextGrid me, IntervalTier thee, const wchar_t *label, double precision);

#endif // _SpeechSynthesizer_and_TextGrid_h_
