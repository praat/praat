#ifndef _SpeechSynthesizer_h_
#define _SpeechSynthesizer_h_
/* SpeechSynthesizer.h
 *
 * Copyright (C) 2011-2013,2015-2023 David Weenink, 2015,2023,2024 Paul Boersma
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

#include "Sound.h"
#include "TextGrid.h"   // also for Table

#define SpeechSynthesizer_PHONEMECODINGS_IPA 2
#define SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM 1
#define SpeechSynthesizer_INPUT_TEXTONLY 1
#define SpeechSynthesizer_INPUT_PHONEMESONLY 2
#define SpeechSynthesizer_INPUT_TAGGEDTEXT 3

#include "SpeechSynthesizer_def.h"

autoSpeechSynthesizer SpeechSynthesizer_create (conststring32 languageName, conststring32 voiceName);

void SpeechSynthesizer_setTextInputSettings (SpeechSynthesizer me, int inputTextFormat, int inputPhonemeCoding);

void SpeechSynthesizer_setEstimateSpeechRateFromSpeech (SpeechSynthesizer me, bool estimate);

void SpeechSynthesizer_setSpeechOutputSettings (SpeechSynthesizer me, double samplingFrequency, double wordgap, double pitchAdjustment, double pitchRange, double wordsPerMinute, int outputPhonemeCodes);

autoSound SpeechSynthesizer_to_Sound (SpeechSynthesizer me, conststring32 text, autoTextGrid *tg, autoTable *events);

void SpeechSynthesizer_playText (SpeechSynthesizer me, conststring32 text);

autostring32 SpeechSynthesizer_getPhonemesFromText (SpeechSynthesizer me, conststring32 text, bool separateBySpaces);

/*
	For general use.
*/
extern STRVEC theSpeechSynthesizerLanguageNames;
extern STRVEC theSpeechSynthesizerVoiceNames;

/*
	For developers mainly.
*/
extern Table theSpeechSynthesizerLanguagePropertiesTable;
extern Table theSpeechSynthesizerVoicePropertiesTable;

/*
	Create the eSpeak-Praat-FileInMemorySet and the four other objects mentioned above.
*/
void classSpeechSynthesizer_initClass ();

/*
	Try to change the language name, the voice name and/or the phoneme set name if any of these are not in the list.
	This also translates ISO 639 language codes (or family/language combinations),
	and fixes capitalization of the first letter.
	Examples of language names:
		en -> English (Great Britain)         // obsolete
		gmw/nl -> Dutch                       // ISO 639 family/language code (if identical to the eSpeak-internal code)
		nl -> Dutch                           // ISO 639 language code
	Examples of voice names:
		f1 -> Female1                         // obsolete
		female1 -> Female1                    // capitalization
	The following "centric" ones are obsolete nowadays in eSpeak, but will still map in case any ancient Praat scripts use them:
		Default -> English (Great Britain)    // obsolete
		default -> Male1                      // obsolete
*/
void SpeechSynthesizer_checkAndRepairLanguageAndVoiceNames (SpeechSynthesizer me);

/* End of file SpeechSynthesizer.h */
#endif
