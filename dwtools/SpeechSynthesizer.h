#ifndef _SpeechSynthesizer_h_
#define _SpeechSynthesizer_h_
/* SpeechSynthesizer.h
 *
 * Copyright (C) 2011-2013, 2015-2016 David Weenink, 2015 Paul Boersma
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

autoSpeechSynthesizerVoice SpeechSynthesizerVoice_create (long numberOfFormants);

void SpeechSynthesizerVoice_setDefaults (SpeechSynthesizerVoice me);

void SpeechSynthesizerVoice_initFromEspeakVoice (SpeechSynthesizerVoice me, voice_t *voice);


void SpeechSynthesizer_initEspeak ();

autoSpeechSynthesizer SpeechSynthesizer_create (const char32 *voiceLanguageName, const char32 *voiceVariantName);

const char32 *SpeechSynthesizer_getVoiceLanguageCodeFromName (SpeechSynthesizer me, const char32 *voiceLanguageName);

const char32 *SpeechSynthesizer_getVoiceVariantCodeFromName (SpeechSynthesizer me, const char32 *voiceVariantName);

void SpeechSynthesizer_changeLanguageNameToCurrent (SpeechSynthesizer me);

void SpeechSynthesizer_setTextInputSettings (SpeechSynthesizer me, int inputTextFormat, int inputPhonemeCoding);

void SpeechSynthesizer_setSpeechOutputSettings (SpeechSynthesizer me, double samplingFrequency, double wordgap, long pitchAdjustment, long pitchRange, long wordsPerMinute, bool estimateWordsPerMinute, int outputPhonemeCodes);

autoSound SpeechSynthesizer_to_Sound (SpeechSynthesizer me, const char32 *text, autoTextGrid *tg, autoTable *events);

void SpeechSynthesizer_playText (SpeechSynthesizer me, const char32 *text);


/* End of file SpeechSynthesizer.h */
#endif
