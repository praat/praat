#ifndef _SpeechSynthesizer_h_
#define _SpeechSynthesizer_h_
/* SpeechSynthesizer.h
 *
 * Copyright (C) 2011-2013 David Weenink
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
#include "../external/espeak/speech.h"
#include "../external/espeak/speak_lib.h"
#include "../external/espeak/phoneme.h"
#include "../external/espeak/synthesize.h"
#include "../external/espeak/voice.h"

#define SpeechSynthesizer_PHONEMECODINGS_IPA 2
#define SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM 1
#define SpeechSynthesizer_INPUT_TEXTONLY 1
#define SpeechSynthesizer_INPUT_PHONEMESONLY 2
#define SpeechSynthesizer_INPUT_TAGGEDTEXT 3

#include "SpeechSynthesizer_def.h"
oo_CLASS_CREATE (SpeechSynthesizerVoice, Data);
oo_CLASS_CREATE (SpeechSynthesizer, Data);

SpeechSynthesizerVoice SpeechSynthesizerVoice_create (long numberOfFormants);
void SpeechSynthesizerVoice_setDefaults (SpeechSynthesizerVoice me);
void SpeechSynthesizerVoice_initFromEspeakVoice (SpeechSynthesizerVoice me, voice_t *voice);


void SpeechSynthesizer_initSoundBuffer (SpeechSynthesizer me);
SpeechSynthesizer SpeechSynthesizer_create (const wchar_t *voiceLanguageName, const wchar_t *voiceVariantName);

const wchar_t *SpeechSynthesizer_getVoiceLanguageCodeFromName (SpeechSynthesizer me, const wchar_t *voiceLanguageName);
const wchar_t *SpeechSynthesizer_getVoiceVariantCodeFromName (SpeechSynthesizer me, const wchar_t *voiceVariantName);

void SpeechSynthesizer_setTextInputSettings (SpeechSynthesizer me, int inputTextFormat, int inputPhonemeCoding);
void SpeechSynthesizer_setSpeechOutputSettings (SpeechSynthesizer me, double samplingFrequency, double wordgap, long pitchAdjustment, long pitchRange, long wordsPerMinute, bool estimateWordsPerMinute, int outputPhonemeCodes);

Sound SpeechSynthesizer_to_Sound (SpeechSynthesizer me, const wchar_t *text, TextGrid *tg, Table *events);

void SpeechSynthesizer_playText (SpeechSynthesizer me, const wchar_t *text);


/* End of file SpeechSynthesizer.h */
#endif
