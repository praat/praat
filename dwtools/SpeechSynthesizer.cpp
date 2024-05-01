/* SpeechSynthesizer.cpp
 *
 * Copyright (C) 2011-2023 David Weenink, 2012,2013,2015-2024 Paul Boersma
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

#include "espeak_ng_version.h"
#include "espeak_ng.h"
#include "espeakdata_FileInMemory.h"

#include "SpeechSynthesizer.h"
#include "Strings_extensions.h"
#include "speak_lib.h"
#include "synthdata.h"
#include "encoding.h"
#include "dictionary.h"
#include "string.h"
#include "translate.h"
#include "voice.h"

#include "oo_DESTROY.h"
#include "SpeechSynthesizer_def.h"
#include "oo_COPY.h"
#include "SpeechSynthesizer_def.h"
#include "oo_EQUAL.h"
#include "SpeechSynthesizer_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SpeechSynthesizer_def.h"
#include "oo_WRITE_TEXT.h"
#include "SpeechSynthesizer_def.h"
#include "oo_WRITE_BINARY.h"
#include "SpeechSynthesizer_def.h"
#include "oo_READ_TEXT.h"
#include "SpeechSynthesizer_def.h"
#include "oo_READ_BINARY.h"
#include "SpeechSynthesizer_def.h"
#include "oo_DESCRIPTION.h"
#include "SpeechSynthesizer_def.h"

#define espeak_SAMPLINGFREQUENCY 22050.h""

#include "UnicodeData.h"

extern int option_phoneme_events;   // BUG: external declaration outside header file (ppgb 20210307)

Thing_implement (EspeakVoice, Daata, 1);


static autostring8 ipa_to_kirshenbaum (conststring32 text) {
	const struct ipaksymbol {
		char32 ipa; 
		char32 kirshenbaum;
	} ipaksymbols [] = {
		{UNICODE_LATIN_SMALL_LETTER_A, U'a'},					// a
		{UNICODE_LATIN_SMALL_LETTER_B, U'b'},					// b
		{UNICODE_LATIN_SMALL_LETTER_C, U'c'},					// c
		{UNICODE_LATIN_SMALL_LETTER_D, U'd'},					// d
		{UNICODE_LATIN_SMALL_LETTER_E, U'e'},					// e
		{UNICODE_LATIN_SMALL_LETTER_F, U'f'},					// f
		{UNICODE_LATIN_SMALL_LETTER_G, U'g'},					// g
		{UNICODE_LATIN_SMALL_LETTER_SCRIPT_G, U'g'},			// ɡ
		{UNICODE_LATIN_SMALL_LETTER_H, U'h'},					// h
		{UNICODE_LATIN_SMALL_LETTER_I, U'i'},					// i
		{UNICODE_LATIN_SMALL_LETTER_J, U'j'},					// j
		{UNICODE_LATIN_SMALL_LETTER_K, U'k'},					// k
		{UNICODE_LATIN_SMALL_LETTER_L, U'l'},					// l
		{UNICODE_LATIN_SMALL_LETTER_M, U'm'},					// m
		{UNICODE_LATIN_SMALL_LETTER_N, U'n'},					// n
		{UNICODE_LATIN_SMALL_LETTER_O, U'o'},					// o
		{UNICODE_LATIN_SMALL_LETTER_P, U'p'},					// p
		{UNICODE_LATIN_SMALL_LETTER_Q, U'q'},					// q
		{UNICODE_LATIN_SMALL_LETTER_TURNED_R, U'r'},			// ɹ
		{UNICODE_LATIN_SMALL_LETTER_S, U's'},					// s
		{UNICODE_LATIN_SMALL_LETTER_T, U't'},					// t
		{UNICODE_LATIN_SMALL_LETTER_U, U'u'},					// u
		{UNICODE_LATIN_SMALL_LETTER_V, U'v'},					// v
		{UNICODE_LATIN_SMALL_LETTER_W, U'w'},					// w
		{UNICODE_LATIN_SMALL_LETTER_X, U'x'},					// x
		{UNICODE_LATIN_SMALL_LETTER_Y, U'y'},					// y
		{UNICODE_LATIN_SMALL_LETTER_Z, U'z'},					// z
		{UNICODE_LATIN_SMALL_LETTER_ALPHA, U'A'},				// ɑ
		{UNICODE_GREEK_SMALL_LETTER_BETA, U'B'},				// β
		{UNICODE_LATIN_SMALL_LETTER_C_WITH_CEDILLA, U'C'},		// ç
		{UNICODE_LATIN_SMALL_LETTER_ETH, U'D'},					// ð
		{UNICODE_LATIN_SMALL_LETTER_OPEN_E, U'E'},				// ɛ
		// F unused
		{UNICODE_LATIN_LETTER_SMALL_CAPITAL_G, U'G'},			// ɢ
		{UNICODE_LATIN_SMALL_LETTER_H_WITH_STROKE, U'H'},		// ħ
		{UNICODE_LATIN_LETTER_SMALL_CAPITAL_I, U'I'},			// ɪ
		{UNICODE_LATIN_SMALL_LETTER_IOTA, U'I'},				// ɩ
		{UNICODE_LATIN_SMALL_LETTER_DOTLESS_J_WITH_STROKE, U'J'},// ɟ
		// K unused
		{UNICODE_LATIN_SMALL_LETTER_L_WITH_MIDDLE_TILDE, U'L'},	// ɫ
		{UNICODE_LATIN_LETTER_SMALL_CAPITAL_L, U'L'},			// ʟ
		{UNICODE_LATIN_SMALL_LETTER_L_WITH_BELT, U'L'},			// ɬ
		{UNICODE_LATIN_SMALL_LETTER_M_WITH_HOOK, U'M'},			// ɱ
		{UNICODE_LATIN_SMALL_LETTER_ENG, U'N'},					// ŋ
		{UNICODE_LATIN_SMALL_LETTER_OPEN_O, U'O'},				// ɔ
		{UNICODE_GREEK_CAPITAL_LETTER_PHI, U'P'},				// Φ
		{UNICODE_LATIN_SMALL_LETTER_GAMMA, U'Q'},				// ɣ
		{UNICODE_LATIN_SMALL_LETTER_SCHWA_WITH_HOOK, U'R'},		// ɚ
		{UNICODE_LATIN_LETTER_SMALL_CAPITAL_R, U'R'},			// ʀ
		{UNICODE_LATIN_SMALL_LETTER_ESH, U'S'},					// ʃ
		{UNICODE_GREEK_SMALL_LETTER_THETA, U'T'},				// θ
		{UNICODE_LATIN_SMALL_LETTER_UPSILON, U'U'},				// ʊ
		{UNICODE_LATIN_SMALL_LETTER_CLOSED_OMEGA, U'U'},		// ɷ
		{UNICODE_LATIN_SMALL_LETTER_TURNED_V, U'V'},			// ʌ
		{UNICODE_LATIN_SMALL_LIGATURE_OE, U'W'},				// œ
		{UNICODE_GREEK_SMALL_LETTER_CHI, U'X'},					// χ
		{UNICODE_LATIN_SMALL_LETTER_O_WITH_STROKE, U'Y'},		// ø
		{UNICODE_LATIN_SMALL_LETTER_EZH, U'Z'},					// ʒ
		{UNICODE_LATIN_LETTER_GLOTTAL_STOP, U'?'},				// ʔ
		{UNICODE_LATIN_SMALL_LETTER_SCHWA, U'@'},				// ə
		{UNICODE_LATIN_SMALL_LETTER_AE, U'&'},					// æ
		{UNICODE_LATIN_SMALL_LETTER_R_WITH_FISHHOOK, U'*'}		// ɾ	
	};
	const struct ipaksymbol2 {
		char32 ipa; 
		char32 kirshenbaum[3];
	} ipaksymbols2 [] = {
		// alveoalars
		{UNICODE_LATIN_LETTER_SMALL_CAPITAL_B, U"b"},			// ʙ unsupported: b<trl>
		{UNICODE_LATIN_SMALL_LETTER_B_WITH_HOOK, U"b"},			// ɓ unsupported: b`
		{UNICODE_LATIN_SMALL_LETTER_D_WITH_HOOK, U"d"},			// ɗ unsupported: d`
		{UNICODE_LATIN_SMALL_LETTER_TURNED_T, U"t"},			// ʇ unsupported: t!
		{UNICODE_LATIN_SMALL_LETTER_L_WITH_BELT, U"s"},			// ɬ unsupported: s<lat>
		{UNICODE_LATIN_SMALL_LETTER_LEZH, U"z"},				// ɮ unsupported: z<lat>
		{UNICODE_LATIN_LETTER_SMALL_CAPITAL_R, U"r"},			// ʀ unsupported: r<trl>
		{UNICODE_LATIN_SMALL_LETTER_R_WITH_FISHHOOK, U"*"},		// ɾ 
		{UNICODE_LATIN_SMALL_LETTER_TURNED_R_WITH_LONG_LEG, U"*"},// ɺ unsupported: *<lat>
		{UNICODE_LATIN_LETTER_STRETCHED_C, U"c"},		// ʗ unsupported: c!
		{UNICODE_LATIN_LETTER_INVERTED_GLOTTAL_STOP, U"l"},		// ʖ unsupported: l!
/*		{, U""},		//  
		{, U""},		//  
		{, U""},		//  
		{, U""},		//  
		{, U""},		//  
		{, U""},		//  
*/		
	};
	autostring8 result;
	return result;
	
}





autoEspeakVoice EspeakVoice_create () {
	try {
		autoEspeakVoice me = Thing_new (EspeakVoice);
		my numberOfFormants = 9;   // equals N_PEAKS
		my numberOfKlattParameters = 8;
		my klattv = zero_INTVEC (my numberOfKlattParameters);
		my freq = zero_INTVEC (my numberOfFormants);
		my height = zero_INTVEC (my numberOfFormants);   // 100% = 256
		my width = zero_INTVEC (my numberOfFormants);   // 100% = 256
		my freqadd = zero_INTVEC (my numberOfFormants);   // Hz

		// copies without temporary adjustments from embedded commands
		my freq2 = zero_INTVEC (my numberOfFormants);   // 100% = 256
		my height2 = zero_INTVEC (my numberOfFormants);   // 100% = 256

		my breath = zero_INTVEC (my numberOfFormants);   // amount of breath for each formant. breath [0] indicates whether any are set.
		my breathw = zero_INTVEC (my numberOfFormants);   // width of each breath formant
		my numberOfToneAdjusts = 1000;   // equals N_TONE_ADJUST in voice.h
		my tone_adjust = newvectorzero<unsigned char> (my numberOfToneAdjusts);
		EspeakVoice_setDefaults (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"EspeakVoice not created.");
	}
}

void EspeakVoice_setDefaults (EspeakVoice me) {
	(void) me;
}

void EspeakVoice_initFromEspeakVoice (EspeakVoice me, voice_t *voicet) {
	my v_name = Melder_dup (Melder_peek8to32 (voicet -> v_name));

	my phoneme_tab_ix = voicet -> phoneme_tab_ix;
	my pitch_base = voicet -> pitch_base;
	my pitch_range = voicet -> pitch_range;

	my speedf1 = voicet -> speedf1;
	my speedf2 = voicet -> speedf2;
	my speedf3 = voicet -> speedf3;

	my speed_percent = voicet -> speed_percent;
	my flutter = voicet -> flutter;
	my roughness = voicet -> roughness;
	my echo_delay = voicet -> echo_delay;
	my echo_amp = voicet -> echo_amp;
	my n_harmonic_peaks = voicet -> n_harmonic_peaks;
	my peak_shape = voicet -> peak_shape;
	my voicing = voicet -> voicing;
	my formant_factor = voicet -> formant_factor;
	my consonant_amp = voicet -> consonant_amp;
	my consonant_ampv = voicet -> consonant_ampv;
	my samplerate = voicet -> samplerate;
	my numberOfKlattParameters = 8;
	for (integer i = 1; i <= my numberOfKlattParameters; i ++)
		my klattv [i] = voicet -> klattv [i - 1];
	for (integer i = 1; i <= my numberOfFormants; i ++) {
		my freq [i] = voicet -> freq [i - 1];
		my height [i] = voicet -> height [i - 1];
		my width [i] = voicet -> width [i - 1];
		my freqadd [i] = voicet -> freqadd [i - 1];
		my freq2 [i] = voicet -> freq2 [i - 1];
		my height2 [i] = voicet -> height2 [i - 1];
		my breath [i] = voicet -> breath [i - 1];
		my breathw [i] = voicet -> breathw [i - 1];
	}
	my numberOfToneAdjusts = 1000;
	for (integer i = 1; i <= my numberOfToneAdjusts; i ++)
		my tone_adjust [i] = voicet -> tone_adjust [i - 1];
}

void EspeakVoice_into_voice (EspeakVoice me, voice_t *voicet) {   // BUG unused (ppgb 20210307)

	if (my v_name)
		strncpy (voicet -> v_name, Melder_peek32to8 (my v_name.get()), 40);
	if (my language_name)
		strncpy (voicet -> language_name, Melder_peek32to8 (my language_name.get()), 20);
	voicet -> phoneme_tab_ix = my phoneme_tab_ix;
	voicet -> pitch_base = my pitch_base;
	voicet -> pitch_range = my pitch_range;

	voicet -> speedf1 = my speedf1;
	voicet -> speedf2 = my speedf2;
	voicet -> speedf3 = my speedf3;

	voicet -> speed_percent = my speed_percent;
	voicet -> flutter = my flutter;
	voicet -> roughness = my roughness;
	voicet -> echo_delay = my echo_delay;
	voicet -> echo_amp = my echo_amp;
	voicet -> n_harmonic_peaks = my n_harmonic_peaks;
	voicet -> peak_shape = my peak_shape;
	voicet -> voicing = my voicing;
	voicet -> formant_factor = my formant_factor;
	voicet -> consonant_amp = my consonant_amp;
	voicet -> consonant_ampv = my consonant_ampv;
	voicet -> samplerate = my samplerate;
	for (integer i = 1; i <= my numberOfKlattParameters; i ++)
		voicet -> klattv [i - 1] = my klattv [i];
	for (integer i = 1; i <= my numberOfFormants; i ++) {
		voicet -> freq [i - 1] = my freq [i];
		voicet -> height [i - 1] = my height [i];
		voicet -> width [i - 1] = my width [i];
		voicet -> freqadd [i - 1] = my freqadd [i];
		voicet -> freq2 [i - 1] = my freq2 [i];
		voicet -> height2 [i - 1] = my height2 [i];
		voicet -> breath [i - 1] = my breath [i];
		voicet -> breathw [i - 1] = my breathw [i];
	}
	for (integer i = 1; i <= my numberOfToneAdjusts; i ++)
		voicet -> tone_adjust [i - 1] = voicet -> tone_adjust [i];
}

Thing_implement (SpeechSynthesizer, Daata, 1);

void structSpeechSynthesizer :: v1_info () {
	SpeechSynthesizer_Parent :: v1_info ();
	MelderInfo_writeLine (U"Synthesizer version: espeak-ng ", our d_synthesizerVersion.get());
	MelderInfo_writeLine (U"Language: ", our d_languageName.get());
	MelderInfo_writeLine (U"Voice: ", our d_voiceName.get());
	MelderInfo_writeLine (U"Phoneme set: ", our d_phonemeSet.get());
	MelderInfo_writeLine (U"Input text format: ", (our d_inputTextFormat == SpeechSynthesizer_INPUT_TEXTONLY ? U"text only" :
		d_inputTextFormat == SpeechSynthesizer_INPUT_PHONEMESONLY ? U"phonemes only" : U"tagged text"));
	MelderInfo_writeLine (U"Input phoneme coding: ", (our d_inputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM ? U"Kirshenbaum" : U"???"));
	MelderInfo_writeLine (U"Sampling frequency: ", our d_samplingFrequency, U" Hz");
	MelderInfo_writeLine (U"Word gap: ", our d_wordGap, U" s");
	MelderInfo_writeLine (U"Pitch multiplier: ", our d_pitchAdjustment, U" (0.5-2.0)");
	MelderInfo_writeLine (U"Pitch range multiplier: ", our d_pitchRange, U" (0.0-2.0)");
	MelderInfo_writeLine (U"Speaking rate: ", our d_wordsPerMinute, U" words per minute",
		our d_estimateSpeechRate ? U" (but estimated from speech if possible)" : U" (fixed)");
	MelderInfo_writeLine (U"Output phoneme coding: ",
		our d_inputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM ? U"Kirshenbaum" :
		our d_inputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_IPA ? U"IPA" : U"???"
	);
}

static int synthCallback (short *wav, int numsamples, espeak_EVENT *events)
{
	char phoneme_name [9];
	if (wav == nullptr)
		return 1;	
	/*
		It is essential that the SpeechSynthesizer is identified here by the user_data,
		because the espeakEVENT_LIST_TERMINATED event may still be accompanied by
		a piece of audio data!!
	*/
	SpeechSynthesizer me = (SpeechSynthesizer) (events -> user_data);
	while (events -> type != espeakEVENT_LIST_TERMINATED) {
		if (events -> type == espeakEVENT_SAMPLERATE) {
			my d_internalSamplingFrequency = events -> id.number;
		} else {
			/*
				my events = Table "time type type-t t-pos length a-pos sample id uniq";
									1    2    3      4      5     6     7     8   9
			*/
			Table_appendRow (my d_events.get());
			const integer irow = my d_events -> rows.size;
			const double time = events -> audio_position * 0.001;
			Table_setNumericValue (my d_events.get(), irow, 1, time);
			Table_setNumericValue (my d_events.get(), irow, 2, events -> type);
			// Column 3 will be filled afterwards
			Table_setNumericValue (my d_events.get(), irow, 4, events -> text_position);
			Table_setNumericValue (my d_events.get(), irow, 5, events -> length);
			Table_setNumericValue (my d_events.get(), irow, 6, events -> audio_position);
			Table_setNumericValue (my d_events.get(), irow, 7, events -> sample);
			if (events -> type == espeakEVENT_MARK || events -> type == espeakEVENT_PLAY) {
				Table_setStringValue (my d_events.get(), irow, 8, Melder_peek8to32 (events -> id.name));
			} else {
				// Ugly hack because id.string is not 0-terminated if 8 chars long!
				memcpy (phoneme_name, events -> id.string, 8);
				//phoneme_name [8] = 0;
				phoneme_name [4] = 0;   // ppgb UGLY HACK IN ORDER TO MAKE FEWER MISTAKES (20231022)
				//TRACE
				trace (U"phoneme name <<", Melder_peek8to32 (phoneme_name), U">>");
				Table_setStringValue (my d_events.get(), irow, 8, Melder_peek8to32 (phoneme_name));
			}
			Table_setNumericValue (my d_events.get(), irow, 9, events -> unique_identifier);
		}
		events++;
	}
	if (me) {
		my d_wav.resize (my d_numberOfSamples + numsamples);
		for (integer i = 1; i <= numsamples; i++)
			my d_wav [my d_numberOfSamples + i] = wav [i - 1];
		my d_numberOfSamples += numsamples;
	}
	return 0;
}

static conststring32 SpeechSynthesizer_getLanguageCode (SpeechSynthesizer me) {
	try {
		const integer irow = Table_searchColumn (espeakdata_languages_propertiesTable.get(), 2, my d_languageName.get());
		Melder_require (irow != 0,
			U"Cannot find language \"", my d_languageName.get(), U"\".");
		return Table_getStringValue_a (espeakdata_languages_propertiesTable.get(), irow, 1);
	} catch (MelderError) {
		Melder_throw (me, U": Cannot find language code.");
	}
}

static conststring32 SpeechSynthesizer_getPhonemeCode (SpeechSynthesizer me) {
	try {
		const integer irow = Table_searchColumn (espeakdata_languages_propertiesTable.get(), 2, my d_phonemeSet.get());
		Melder_require (irow != 0,
			U"Cannot find phoneme set \"", my d_phonemeSet.get(), U"\".");
		return Table_getStringValue_a (espeakdata_languages_propertiesTable.get(), irow, 1);
	} catch (MelderError) {
		Melder_throw (me, U": Cannot find phoneme code.");
	}
}

static conststring32 SpeechSynthesizer_getVoiceCode (SpeechSynthesizer me) {
	try {
		const integer irow = Table_searchColumn (espeakdata_voices_propertiesTable.get(), 2, my d_voiceName.get());
		Melder_require (irow != 0,
			U": Cannot find voice variant \"", my d_voiceName.get(), U"\".");
		return Table_getStringValue_a (espeakdata_voices_propertiesTable.get(), irow, 1);
	} catch (MelderError) {
		Melder_throw (me, U": Cannot find voice code.");
	}
}

autoSpeechSynthesizer SpeechSynthesizer_create (conststring32 languageName, conststring32 voiceName) {
	try {
		autoSpeechSynthesizer me = Thing_new (SpeechSynthesizer);
		my d_synthesizerVersion = Melder_dup (ESPEAK_NG_VERSION);
		my d_languageName = Melder_dup (languageName);
		(void) SpeechSynthesizer_getLanguageCode (me.get());  // existence check
		my d_voiceName = Melder_dup (voiceName);
		(void) SpeechSynthesizer_getVoiceCode (me.get());  // existence check
		my d_phonemeSet = Melder_dup (languageName);
		SpeechSynthesizer_setTextInputSettings (me.get(), SpeechSynthesizer_INPUT_TAGGEDTEXT, SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM);
		SpeechSynthesizer_setSpeechOutputSettings (me.get(), 44100.0, 0.01, 1.0, 1.0, 175.0, SpeechSynthesizer_PHONEMECODINGS_IPA);
		SpeechSynthesizer_setEstimateSpeechRateFromSpeech (me.get(), true);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SpeechSynthesizer not created.");
	}
}

void SpeechSynthesizer_setTextInputSettings (SpeechSynthesizer me, int inputTextFormat, int inputPhonemeCoding) {
	my d_inputTextFormat = inputTextFormat;
	my d_inputPhonemeCoding = inputPhonemeCoding;
}

void SpeechSynthesizer_setEstimateSpeechRateFromSpeech (SpeechSynthesizer me, bool estimate) {
	my d_estimateSpeechRate = estimate;
}

void SpeechSynthesizer_setSpeechOutputSettings (SpeechSynthesizer me, double samplingFrequency, double wordGap, double pitchAdjustment, double pitchRange, double wordsPerMinute, int outputPhonemeCoding) {
	my d_samplingFrequency = samplingFrequency;
	my d_wordGap = wordGap;
	my d_pitchAdjustment = Melder_clipped (0.5, pitchAdjustment, 2.0);
	my d_pitchRange = Melder_clipped (0.0, pitchRange, 2.0);
	if (wordsPerMinute <= 0.0)
		wordsPerMinute = 175.0;   // SMELL: looks like an arbitrary default
	my d_wordsPerMinute = Melder_clipped (80.0, wordsPerMinute, 450.0);
	my d_outputPhonemeCoding = outputPhonemeCoding;
}

void SpeechSynthesizer_playText (SpeechSynthesizer me, conststring32 text) {
	autoSound thee = SpeechSynthesizer_to_Sound (me, text, nullptr, nullptr);
	Sound_play (thee.get(), nullptr, nullptr);
}

static autoSound buffer_to_Sound (constINTVEC const& wav, double samplingFrequency)
{
	try {
		const double dx = 1.0 / samplingFrequency;
		const double xmax = wav.size * dx;
		autoSound thee = Sound_create (1, 0.0, xmax, wav.size, dx, dx / 2.0);
		for (integer i = 1; i <= wav.size; i++)
			thy z [1] [i] = wav [i] / 32768.0;
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from synthesizer data.");
	}
}

static void IntervalTier_addBoundaryUnsorted (IntervalTier me, integer iinterval, double time, conststring32 newLabel, bool isNewleftLabel) {
	//TRACE
	trace (U"size before: ", my intervals.size);
	for (integer i = 1; i <= my intervals.size; i ++)
		trace (U"phoneme ", i, U" ", my intervals.at [i] -> xmin, U" ", my intervals.at [i] -> xmax,
				U" ", Melder_length (my intervals.at [i] -> text.get()), U" ", my intervals.at [i] -> text.get());

	trace (U"isNewLeftLabel: ", isNewleftLabel);
	trace (U"iinterval: ", iinterval);
	Melder_require (time > my xmin && time < my xmax,
		U"The time (", time, U" seconds) is not inside the domain of the interval tier (", my xmin, U" .. ", my xmax, U" seconds).");
	/*
		Find interval to split
	*/
	if (iinterval <= 0)
		iinterval = IntervalTier_timeToLowIndex (me, time);
	/*
		Modify end time of left label
	*/
	const TextInterval ti = my intervals.at [iinterval];
	trace (U"end time ", ti -> xmax, U" overridden by ", time);
	ti -> xmax = time;
	if (isNewleftLabel) {
		trace (U"label <<", ti -> text.get(), U">> overridden by ", newLabel);
		TextInterval_setText (ti, newLabel);
	}
	autoTextInterval ti_new = TextInterval_create (time, my xmax, (! isNewleftLabel ? newLabel : U"" ));
	my intervals. addItem_unsorted_move (ti_new.move());

	trace (U"size after: ", my intervals.size);
	for (integer i = 1; i <= my intervals.size; i ++)
		trace (U"phoneme ", i, U" ", my intervals.at [i] -> xmin, U" ", my intervals.at [i] -> xmax,
				U" ", Melder_length (my intervals.at [i] -> text.get()), U" ", my intervals.at [i] -> text.get());
}

static void Table_setEventTypeString (Table me) {
	try {
		for (integer i = 1; i <= my rows.size; i ++) {
			const int type = Table_getNumericValue_a (me, i, 2);
			conststring32 label = U"0";
			if (type == espeakEVENT_WORD)
				label = U"word";
			else if (type == espeakEVENT_SENTENCE)
				label = U"sent";
			else if (type == espeakEVENT_MARK)
				label = U"mark";
			else if (type == espeakEVENT_PLAY)
				label = U"play";
			else if (type == espeakEVENT_END)
				label = U"s-end";
			else if (type == espeakEVENT_MSG_TERMINATED)
				label = U"msg_term";
			else if (type == espeakEVENT_PHONEME)
				label = U"phoneme";
			Table_setStringValue (me, i, 3, label);
		}
	} catch (MelderError) {
		Melder_throw (U"Event types not set.");
	}
}

static void MelderString_trimWhiteSpaceAtEnd (MelderString *me) {
	while (my length > 1 && (my string [my length - 1] == U' ' || my string [my length - 1] == U'\t'
		|| my string [my length - 1] == U'\r' || my string [my length - 1] == U'\n'))
			my string [-- my length] = U'\0';
}

static void IntervalTier_mergeSpecialIntervals (IntervalTier me) {
	integer intervalIndex = my intervals.size;
	TextInterval right = my intervals.at [intervalIndex];
	const integer labelLength_right = TextInterval_labelLength (right);
	bool isEmptyInterval_right = labelLength_right == 0 || (labelLength_right == 1 && Melder_equ (right -> text.get(), U"\001"));
	while (intervalIndex > 1) {
		const TextInterval left = my intervals.at [intervalIndex - 1];
		const integer labelLength_left = TextInterval_labelLength (left);
		const bool isEmptyInterval_left = labelLength_left == 0 || (labelLength_left == 1 && Melder_equ (left -> text.get(), U"\001"));
		if (isEmptyInterval_right && isEmptyInterval_left) {
			/*
				Remove right interval and empty left interval
			*/
			left -> xmax = right -> xmax;
			TextInterval_setText (left, U""); 
			my intervals. removeItem (intervalIndex);
		}
		right = left;
		isEmptyInterval_right = isEmptyInterval_left;
		intervalIndex --;
	}
}

#if 0   // BUG unused (ppgb 20210307)
/* insert boundary at time t and merge/delete intervals after this time */
static void IntervalTier_insertBoundaryAndMergeIntervalsAfter (IntervalTier me, double t) {
	if (t <= my xmin || t >= my xmax)
		return;
	
	const integer intervalNumber = IntervalTier_timeToLowIndex (me, t);
	while (my intervals.size > intervalNumber + 1)
		my intervals. removeItem (my intervals.size);

	// there can be maximally one interval left to the right of intervalNumber
	const TextInterval ti = my intervals.at [intervalNumber];
	if (ti -> xmin == t) {   // if t happens to be on a boundary: remove the next interval if it exists
		if (my intervals.size > intervalNumber)
			my intervals. removeItem (my intervals .size);
		ti -> xmax = my xmax;
		TextInterval_setText (ti, U"");
	} else {
		ti -> xmax = t;
		TextInterval last = my intervals.at [my intervals.size];
		last -> xmin = t;
		last -> xmax = my xmax;
		TextInterval_setText (last, U"");
	}
}
#endif

static bool almost_equal (double t1, double t2) {
	// the "=" sign is essential for a difference of zero if t1 == 0
	return fabs (t1 - t2) <= 1e-12 * fabs (t1);
}

static void IntervalTier_insertEmptyIntervalsFromOtherTier (IntervalTier to, IntervalTier from) {
	for (integer iint = 1; iint <= from -> intervals.size; iint ++) {
		const TextInterval tifrom = from -> intervals.at [iint];
		if (TextInterval_labelLength (tifrom) == 0) {   // found empty interval
			const double t_left = tifrom -> xmin, t_right = tifrom -> xmax;
			integer intervalIndex_to = IntervalTier_timeToLowIndex (to, t_left);
			if (intervalIndex_to > 0) {   // insert to the right of intervalIndex_to
				const TextInterval tito = to -> intervals.at [intervalIndex_to];
				if (! almost_equal (tito -> xmin, t_left)) {   // not on the start boundary of the interval, it cannot be at xmax
					autoTextInterval newInterval = TextInterval_create (t_left, tito -> xmax, U"");
					tito -> xmax = t_left;
					to -> intervals. addItem_move (newInterval.move());
				}
			}
			intervalIndex_to = IntervalTier_timeToHighIndex (to, t_right);
			const TextInterval tito = to -> intervals.at [intervalIndex_to];
			if (intervalIndex_to > 0) {
				if (! almost_equal (t_right, tito -> xmax)) {   // insert to the left of intervalIndex_to
					autoTextInterval newInterval = TextInterval_create (tito -> xmin, t_right, U"");
					tito -> xmin = t_right;
					to -> intervals. addItem_move (newInterval.move());
				}
			}
		}
	}
}

static void IntervalTier_removeVeryShortIntervals (IntervalTier me) {
	integer iint = 1;
	while (iint <= my intervals.size) {
		const TextInterval ti = my intervals.at [iint];
		if (almost_equal (ti -> xmin, ti -> xmax))
			my intervals.removeItem (iint);
		else
			iint ++;
	}
}

static autoTextGrid Table_to_TextGrid (Table me, conststring32 text, double xmin, double xmax) {
	//Table_createWithColumnNames (0, L"time type type-t t-pos length a-pos sample id uniq");
	try {
		const integer textLength = Melder_length (text);
		const integer numberOfRows = my rows.size;
		const integer timeColumnIndex = Table_columnNameToNumber_e (me, U"time");
		const integer typeColumnIndex = Table_columnNameToNumber_e (me, U"type");
		const integer tposColumnIndex = Table_columnNameToNumber_e (me, U"t-pos");
		const integer   idColumnIndex = Table_columnNameToNumber_e (me, U"id");
		autoTextGrid thee = TextGrid_create (xmin, xmax, U"sentence clause word phoneme", U"");

		TextGrid_setIntervalText (thee.get(), 1, 1, text);

		integer p1c = 1, p1w = 1;
		double time_phon_p = xmin;
		bool wordEnd = false;
		autoMelderString mark;

		const IntervalTier clauses = (IntervalTier) thy tiers->at [2];
		const IntervalTier words = (IntervalTier) thy tiers->at [3];
		const IntervalTier phonemes = (IntervalTier) thy tiers->at [4];
		for (integer irow = 1; irow <= numberOfRows; irow ++) {
			/*const*/ double time = Table_getNumericValue_a (me, irow, timeColumnIndex);
			const int type = Table_getNumericValue_a (me, irow, typeColumnIndex);
			const integer pos = Table_getNumericValue_a (me, irow, tposColumnIndex);
			integer length;
			if (type == espeakEVENT_SENTENCE) {
				/*
					Insert only a new boundary, no text
					(text will be inserted at an "end sentence" event).
				*/
				if (time > xmin && time < xmax)
					IntervalTier_addBoundaryUnsorted (clauses, clauses -> intervals.size, time, U"", true);
				p1c = pos;
			} else if (type == espeakEVENT_END) {
				/*
					End of clause: insert new boundary, and fill left interval with text.
				*/
				length = pos - p1c + 1;
				MelderString_ncopy (&mark, text + p1c - 1, length);
				MelderString_trimWhiteSpaceAtEnd (& mark);
				if (time > xmin && time < xmax)
					IntervalTier_addBoundaryUnsorted (clauses, clauses -> intervals.size, time, mark.string, true);
				else
					TextGrid_setIntervalText (thee.get(), 2, clauses -> intervals.size, mark.string);
				p1c = pos;
				/*
					End of clause always signals "end of a word".
				*/
				if (pos <= textLength) {
					length = pos - p1w + 1;
					MelderString_ncopy (&mark, text + p1w - 1, length);
					MelderString_trimWhiteSpaceAtEnd (& mark);
					if (time > xmin && time < xmax)
						IntervalTier_addBoundaryUnsorted (words, words -> intervals.size, time, mark.string, true);
					else
						TextGrid_setIntervalText (thee.get(), 3, words -> intervals.size, mark.string);
					/*
						Now the next word event should not trigger setting the left interval text.
					*/
					wordEnd = false;
				}
			} else if (type == espeakEVENT_WORD) {
				if (pos < p1w)
					continue;
				if (time > xmin && time < xmax) {
					length = pos - p1w;
					if (pos == textLength)
						length++;
					MelderString_ncopy (&mark, text + p1w - 1, length);
					MelderString_trimWhiteSpaceAtEnd (& mark);
					IntervalTier_addBoundaryUnsorted (words, words -> intervals.size, time, ( wordEnd ? mark.string : U"" ), true);
				}
				wordEnd = true;
				p1w = pos;
			} else if (type == espeakEVENT_PHONEME) {
				const conststring32 phoneme = Table_getStringValue_a (me, irow, idColumnIndex);
				//TRACE
				trace (U"found in table the phoneme <<", phoneme, U">>, to be inserted at ", time, U" (usually after ", time_phon_p, U")");
				const TextInterval lastPhonemeInterval = phonemes -> intervals.at [phonemes -> intervals.size];
				if (false) {
					if (time > time_phon_p) {
						/*
							Insert new boundary and label interval with the phoneme.
							TODO: Translate the phoneme to the correct notation
						*/
						if (time > lastPhonemeInterval -> xmin && time < lastPhonemeInterval -> xmax) {
							trace (U"add boundary (1) for phoneme <<", phoneme, U">> at ", time);
							IntervalTier_addBoundaryUnsorted (phonemes, phonemes -> intervals.size, time, phoneme, false);
						}
					} else {
						/*
							Just in case the phoneme starts at xmin, we only need to set interval text.
						*/
						trace (U"overriding <<", lastPhonemeInterval -> text.get(), U">> with <<", phoneme, U">>");
						TextGrid_setIntervalText (thee.get(), 4, phonemes -> intervals.size, phoneme);
					}
				} else {
					if (time > time_phon_p) {
						/*
							Insert new boundary and label interval with the phoneme.
							TODO: Translate the phoneme to the correct notation
						*/
						if (time > lastPhonemeInterval -> xmin && time < lastPhonemeInterval -> xmax) {
							trace (U"add boundary (1) for phoneme <<", phoneme, U">> at ", time);
							IntervalTier_addBoundaryUnsorted (phonemes, phonemes -> intervals.size, time, phoneme, false);
						}
					} else if (! Melder_equ (phoneme, U"")) {
						/*
							Just in case the phoneme starts at xmin, we only need to set interval text.
						*/
						trace (U"overriding <<", lastPhonemeInterval -> text.get(), U">> with <<", phoneme, U">>");
						TextGrid_setIntervalText (thee.get(), 4, phonemes -> intervals.size, phoneme);
					} else {
						//TRACE
						trace (U"stupid repair, houtjes-touwtjes");
						time = 0.001 * lastPhonemeInterval -> xmax + 0.999 * lastPhonemeInterval -> xmin;
						trace (U"add boundary (2) for phoneme <<", phoneme, U">> at ", time);
						IntervalTier_addBoundaryUnsorted (phonemes, phonemes -> intervals.size, time, phoneme, false);
					}
				}
				time_phon_p = time;
			}
		}
		clauses -> intervals. sort ();
		words -> intervals. sort ();
		phonemes -> intervals. sort ();
		
		IntervalTier_mergeSpecialIntervals (phonemes); // Merge neighbouring empty U"" and U"\001" intervals
		
		IntervalTier_removeVeryShortIntervals (words);
		IntervalTier_removeVeryShortIntervals (clauses);
		/*
			Use empty intervals in phoneme tier for more precision in the word tier.
		*/
		IntervalTier_insertEmptyIntervalsFromOtherTier (words, phonemes);
		IntervalTier_mergeSpecialIntervals (words); // Merge neighbouring empty U"" and U"\001" intervals

		return thee;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not created from Table with events.");
	}
}

static void SpeechSynthesizer_generateSynthesisData (SpeechSynthesizer me, conststring32 text) {
	try {
		int synth_flags = 0;
		espeak_ng_InitializePath (nullptr); // PATH_ESPEAK_DATA
		espeak_ng_ERROR_CONTEXT context = { 0 };
		espeak_ng_STATUS status = espeak_ng_Initialize (& context);
		Melder_require (status == ENS_OK,
			U"Internal espeak error. ", status);
		if (my d_inputTextFormat == SpeechSynthesizer_INPUT_TAGGEDTEXT)
			synth_flags |= espeakSSML;
		if (my d_inputTextFormat != SpeechSynthesizer_INPUT_TEXTONLY)
			synth_flags |= espeakPHONEMES;
		option_phoneme_events = espeakINITIALIZE_PHONEME_EVENTS; // extern int option_phoneme_events;
		if (my d_outputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_IPA)
			option_phoneme_events |= espeakINITIALIZE_PHONEME_IPA;

		espeak_ng_SetParameter (espeakRATE, my d_wordsPerMinute, 0);
		/*
			pitchAdjustment_0_99 = a * log10 (my d_pitchAdjustment) + b,
			where 0.5 <= my d_pitchAdjustment <= 2
			pitchRange_0_99 = my d_pitchRange * 49.5,
			where 0 <= my d_pitchRange <= 2
		*/
		const int pitchAdjustment_0_99 = (int) ((49.5 / NUMlog10_2) * log10 (my d_pitchAdjustment) + 49.5);   // rounded towards zero
		espeak_ng_SetParameter (espeakPITCH, pitchAdjustment_0_99, 0);
		const int pitchRange_0_99 = (int) (my d_pitchRange * 49.5);   // rounded towards zero
		espeak_ng_SetParameter (espeakRANGE, pitchRange_0_99, 0);
		const conststring32 languageCode = SpeechSynthesizer_getLanguageCode (me);
		const conststring32 voiceCode = SpeechSynthesizer_getVoiceCode (me);
		
		espeak_ng_SetVoiceByName (Melder_peek32to8 (Melder_cat (languageCode, U"+", voiceCode)));
		const int wordGap_10ms = my d_wordGap * 100;   // espeak word gap is in units of 10 ms
		espeak_ng_SetParameter (espeakWORDGAP, wordGap_10ms, 0);
		espeak_ng_SetParameter (espeakCAPITALS, 0, 0);
		espeak_ng_SetParameter (espeakPUNCTUATION, espeakPUNCT_NONE, 0);

		status = espeak_ng_InitializeOutput (ENOUTPUT_MODE_SYNCHRONOUS, 2048, nullptr);
		espeak_SetSynthCallback (synthCallback);
		if (! Melder_equ (my d_phonemeSet.get(), my d_languageName.get())) {
			const conststring32 phonemeCode = SpeechSynthesizer_getPhonemeCode (me);
			const int index_phon_table_list = LookupPhonemeTable (Melder_peek32to8 (phonemeCode));
			if (index_phon_table_list > 0) {
				voice -> phoneme_tab_ix = index_phon_table_list;
				(void) DoVoiceChange(voice);
			}
		}

		const conststring32 columnNames [] =
				{ U"time", U"type", U"type-t", U"t-pos", U"length", U"a-pos", U"sample", U"id", U"uniq" };
		my d_events = Table_createWithColumnNames (0, ARRAY_TO_STRVEC (columnNames));
		unsigned int unique_identifier = 0;
		#ifdef _WIN32
			conststringW textW = Melder_peek32toW (text);
			synth_flags |= espeakCHARS_WCHAR;
			espeak_ng_Synthesize (textW, wcslen (textW) + 1, 0, POS_CHARACTER, 0, synth_flags, & unique_identifier, me);
		#else
			conststring8 textUTF8 = Melder_peek32to8 (text);
			synth_flags |= espeakCHARS_UTF8;
			espeak_ng_Synthesize (textUTF8, Melder_length_utf8 (text, false) + 1, 0, POS_CHARACTER, 0, synth_flags, & unique_identifier, me);
		#endif

		espeak_ng_Terminate ();
	} catch (MelderError) {
		espeak_Terminate ();
		Melder_throw (U"SpeechSynthesizer: synthesis data not generated.");
	}	
}

autostring32 SpeechSynthesizer_getPhonemesFromText (SpeechSynthesizer me, conststring32 text, bool separateBySpaces) {
	try {
		SpeechSynthesizer_generateSynthesisData (me, text);
		const double dt = 1.0 / my d_internalSamplingFrequency;
		const double tmin = 0.0, tmax = my d_wav.size * dt;
		double xmin = Table_getNumericValue_a (my d_events.get(), 1, 1);
		Melder_clipLeft (tmin, & xmin);
		double xmax = Table_getNumericValue_a (my d_events.get(), my d_events -> rows.size, 1);
		Melder_clipRight (& xmax, tmax);
		autoTextGrid tg = Table_to_TextGrid (my d_events.get(), text, xmin, xmax);
		/*
			Get phonemes from the phoneme tier
		*/
		Melder_assert (tg -> tiers -> size == 4);
		IntervalTier phonemeTier = static_cast<IntervalTier> (tg -> tiers -> at [4]);
		const integer numberOfIntervals = phonemeTier -> intervals.size;
		Melder_require (numberOfIntervals > 0,
			U"There are no phonemes in the tier.");
		autoMelderString phonemes;
		const conststring32 phonemeSeparator = ( separateBySpaces ? U" " : U"" );
		const conststring32 wordSeparator = ( separateBySpaces ? U"  " : U" " );
		for (integer iint = 1; iint <= numberOfIntervals; iint ++) {
			TextInterval interval = phonemeTier -> intervals.at [iint];
			const conststring32 phonemeLabel = interval -> text.get();
			const bool isEmptyLabel = Melder_equ (phonemeLabel, U"");
			if (isEmptyLabel) {
				if (iint > 1 && iint < numberOfIntervals)
					MelderString_append (& phonemes, wordSeparator);
			} else
				MelderString_append (& phonemes, phonemeLabel, (iint < numberOfIntervals ? phonemeSeparator : U"") );
		}
		return Melder_dup (phonemes.string);   // TODO: implement MelderString_move()
	} catch (MelderError) {
		Melder_throw (U"Phonemes not generated.");
	}
}

autoSound SpeechSynthesizer_to_Sound (SpeechSynthesizer me, conststring32 text, autoTextGrid *tg, autoTable *events) {
	try {
		SpeechSynthesizer_generateSynthesisData (me, text);
		autoSound thee = buffer_to_Sound (my d_wav.get(), my d_internalSamplingFrequency);

		if (my d_samplingFrequency != my d_internalSamplingFrequency)
			thee = Sound_resample (thee.get(), my d_samplingFrequency, 50);
		my d_numberOfSamples = 0; // re-use the wav-buffer
		if (tg) {
			double xmin = Table_getNumericValue_a (my d_events.get(), 1, 1);
			if (xmin > thy xmin)
				xmin = thy xmin;
			double xmax = Table_getNumericValue_a (my d_events.get(), my d_events -> rows.size, 1);
			if (xmax < thy xmax)
				xmax = thy xmax;
			autoTextGrid tg1 = Table_to_TextGrid (my d_events.get(), text, xmin, xmax);
			*tg = TextGrid_extractPart (tg1.get(), thy xmin, thy xmax, 0);
		}
		if (events) {
			Table_setEventTypeString (my d_events.get());
			*events = my d_events.move();
		}
		my d_events. reset();
		return thee;
	} catch (MelderError) {
		espeak_Terminate ();
		Melder_throw (U"SpeechSynthesizer: text not converted to Sound.");
	}
}

/* End of file SpeechSynthesizer.cpp */
