#ifndef _SpeechSynthesizer_h_
#define _SpeechSynthesizer_h_
/* SpeechSynthesizer.h
 *
 * Copyright (C) 2011-2013, 2015-2017 David Weenink, 2015 Paul Boersma
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

#include "Sound.h"
#include "TextGrid.h"
#include "espeak_ng.h"
#include "FileInMemoryManager.h"
#include "speech.h"
#include "synthesize.h"
#include "espeakdata_FileInMemory.h"

#define SpeechSynthesizer_PHONEMECODINGS_IPA 2
#define SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM 1
#define SpeechSynthesizer_INPUT_TEXTONLY 1
#define SpeechSynthesizer_INPUT_PHONEMESONLY 2
#define SpeechSynthesizer_INPUT_TAGGEDTEXT 3

#include "SpeechSynthesizer_def.h"

autoEspeakVoice EspeakVoice_create ();

void EspeakVoice_setDefaults (EspeakVoice me);

void EspeakVoice_initFromEspeakVoice (EspeakVoice me, voice_t *voice);


void SpeechSynthesizer_initEspeak ();

autoSpeechSynthesizer SpeechSynthesizer_create (conststring32 languageName, conststring32 voiceName);

conststring32 SpeechSynthesizer_getLanguageCode (SpeechSynthesizer me);

conststring32 SpeechSynthesizer_getVoiceCode (SpeechSynthesizer me);

void SpeechSynthesizer_changeLanguageNameToCurrent (SpeechSynthesizer me);

void SpeechSynthesizer_setTextInputSettings (SpeechSynthesizer me, int inputTextFormat, int inputPhonemeCoding);

void SpeechSynthesizer_setEstimateSpeechRateFromSpeech (SpeechSynthesizer me, bool estimate);

void SpeechSynthesizer_setSpeechOutputSettings (SpeechSynthesizer me, double samplingFrequency, double wordgap, double pitchAdjustment, double pitchRange, double wordsPerMinute, int outputPhonemeCodes);

autoSound SpeechSynthesizer_to_Sound (SpeechSynthesizer me, conststring32 text, autoTextGrid *tg, autoTable *events);

void SpeechSynthesizer_playText (SpeechSynthesizer me, conststring32 text);

/* End of file SpeechSynthesizer.h */
#endif
