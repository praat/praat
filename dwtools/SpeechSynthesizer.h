#ifndef _SpeechSynthesizer_h_
#define _SpeechSynthesizer_h_
/* SpeechSynthesizer.h
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

#include "Sound.h"
#include "TextGrid.h"

#define SpeechSynthesizer_PHONEMECODINGS_IPA 2
#define SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM 1
#define SpeechSynthesizer_INPUT_TEXTONLY 1
#define SpeechSynthesizer_INPUT_PHONEMESONLY 2
#define SpeechSynthesizer_INPUT_TAGGEDTEXT 3

Thing_define (SpeechSynthesizer, Data) {
	public:
		// sythesizers language /voice
		long d_voice, d_voiceVariant;
		wchar_t *d_voiceName, *d_voiceVariantName;
		wchar_t *d_punctuations;
		// text input
		int d_inputTextFormat; // text-only, phonemes-only, mixed
		int d_inputPhonemeCoding; // 1/: output phonemes in espeak/ notation
		// speech output
		long d_numberOfSamples, d_wavCapacity;
		short *d_wav;
		double d_samplingFrequency;
		double d_internalSamplingFrequency;
		double d_wordgap;
		long d_pitchAdjustment, d_pitchRange;
		long d_wordsPerMinute;
		bool d_estimateWordsPerMinute;
		// TextGrid
		bool d_createEventPerPhoneme;
		int d_outputPhonemeCoding; // 1/2: output phonemes in espeak/IPA notation
		Table d_events; // Filled by the call back
	// overridden methods:
		void v_destroy ();
		void v_info ();
};

SpeechSynthesizer SpeechSynthesizer_create (long voice, long voiceVariant);

void SpeechSynthesizer_setTextInputSettings (SpeechSynthesizer me, int inputTextFormat, int inputPhonemeCoding);
void SpeechSynthesizer_setSpeechOutputSettings (SpeechSynthesizer me, double samplingFrequency, double wordgap, long pitchAdjustment, long pitchRange, long wordsPerMinute, bool estimateWordsPerMinute, int outputPhonemeCodes);

Sound SpeechSynthesizer_to_Sound (SpeechSynthesizer me, const wchar_t *text, TextGrid *tg, Table *events);

void SpeechSynthesizer_playText (SpeechSynthesizer me, const wchar_t *text);


/* End of file SpeechSynthesizer.h */
#endif
