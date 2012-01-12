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
#include "Table.h"
#include "TextGrid.h"
#include "speak_lib.h"

Thing_define (SpeechSynthesizer, Data) {
	public:
		wchar_t *d_espeakdataDir;
		double d_samplingFrequency;
		double d_internalSamplingFrequency;
		double d_wordgap;
		long d_pitchAdjustment;
		long d_wordsPerMinute;
		wchar_t *d_voiceName;
		wchar_t *d_punctuations;
		bool d_createEventPerPhoneme;
		bool d_interpretSSML; // Elements within < > are treated as SSML elements (if not recognised ignored)
		bool d_interpretPhonemeCodes;  // Text within [[ ]] is treated as phonemes codes (Hirshenbaum encoding)
		bool d_ipa; // output phonems in IPA notation
		// Filled by the call back
		Table events;
		long d_numberOfSamples, d_wavCapacity;
		short *d_wav;
	// overridden methods:
		void v_destroy ();
		void v_info ();
};

SpeechSynthesizer SpeechSynthesizer_create (const wchar_t *voiceName, const wchar_t *espeakdataDir,
	double samplingFrequency, double wordgap,
	long pitchAdjustment, long wordsPerMinute, bool interpretSSML, bool interpretPhonemeCodes);

void SpeechSynthesizer_setSamplingFrequency (SpeechSynthesizer me, double samplingFrequency);
void SpeechSynthesizer_setSpeakingRate (SpeechSynthesizer me, double wordsPerMinute);
void SpeechSynthesizer_setWordGap (SpeechSynthesizer me, double wordgap);

void SpeechSynthesizer_setDefaults (SpeechSynthesizer me, double samplingFrequency, double wordgap,
	long pitchAdjustment, long wordsPerMinute, bool interpretSSML, bool interpretPhonemeCodes);

void SpeechSynthesizer_setDefaultDataDir (SpeechSynthesizer me, const wchar_t *espeakdataDir);

Sound SpeechSynthesizer_to_Sound (SpeechSynthesizer me, const wchar_t *text);

void SpeechSynthesizer_playText (SpeechSynthesizer me, const wchar_t *text);

Sound SpeechSynthesizer_to_Sound_special (SpeechSynthesizer me, const wchar_t *text, double wordgap,
	long pitchAdjustment, long wordsPerMinute, bool interpretSSML, bool interpretPhonemeCodes,
	bool ipa, TextGrid *tg, Table *events);

Table Table_create_fromEspeakLanguageCodes (const wchar_t *language);

/* End of file SpeechSynthesizer.h */
#endif
