/* SpeechSynthesizer_def.h
 *
 * Copyright (C) 2011-2012, 2015-2016 David Weenink
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

#define ooSTRUCT EspeakVoice
oo_DEFINE_CLASS (EspeakVoice, Daata)
	oo_STRING (v_name) // maximum 39 characters + 1 0-byte
	oo_STRING (language_name) // maximum 19 characters + 1 0-byte

	oo_INT32 (phoneme_tab_ix)  // phoneme table number
	oo_INT32 (pitch_base)    // Hz
	oo_INT32 (pitch_range)   // Hz

	oo_INT32 (speedf1)
	oo_INT32 (speedf2)
	oo_INT32 (speedf3)

	oo_INT32 (speed_percent)      // adjust the WPM speed by this percentage
	oo_INT32 (flutter)
	oo_INT32 (roughness)
	oo_INT32 (echo_delay)
	oo_INT32 (echo_amp)
	oo_INT32 (n_harmonic_peaks)  // highest formant which is formed from adding harmonics
	oo_INT32 (peak_shape)        // alternative shape for formant peaks (0=standard 1=squarer)
	oo_INT32 (voicing)           // 100% = 64, level of formant-synthesized sound
	oo_INT32 (formant_factor)      // adjust nominal formant frequencies by this  because of the voice's pitch (256ths)
	oo_INT32 (consonant_amp)     // amplitude of unvoiced consonants
	oo_INT32 (consonant_ampv)    // amplitude of the noise component of voiced consonants
	oo_INT32 (samplerate)			// sampling frequency as integer Hz
	oo_INT32 (numberOfKlattParameters) // default 8 
	oo_INT_VECTOR (klattv, numberOfKlattParameters)

	// parameters used by Wavegen
	oo_INT32 (numberOfFormants) // 9
	oo_INT_VECTOR (freq, numberOfFormants)		// (short) 100% = 256
	oo_INT_VECTOR (height, numberOfFormants)	// (short) 100% = 256
	oo_INT_VECTOR (width, numberOfFormants)		// (short) 100% = 256
	oo_INT_VECTOR (freqadd, numberOfFormants)	// (short) Hz

	// copies without temporary adjustments from embedded commands
	oo_INT_VECTOR (freq2, numberOfFormants)		// (short) 100% = 256
	oo_INT_VECTOR (height2, numberOfFormants)	// (short) 100% = 256
	oo_INT_VECTOR (width2, numberOfFormants)	// (short) 100% = 256

	oo_INT_VECTOR (breath, numberOfFormants)	// (int64) amount of breath for each formant. breath[0] indicates whether any are set.
	oo_INT_VECTOR (breathw, numberOfFormants)	// width of each breath formant
	oo_INT32 (numberOfToneAdjusts)
	oo_UBYTE_VECTOR_FROM (tone_adjust, 1, numberOfToneAdjusts)

oo_END_CLASS (EspeakVoice)
#undef ooSTRUCT


#define ooSTRUCT SpeechSynthesizer
oo_DEFINE_CLASS (SpeechSynthesizer, Daata)

	oo_FROM (1)
		oo_STRING (d_synthesizerVersion)
	oo_ENDFROM

	// sythesizers language /voice
	oo_STRING (d_languageName)
	oo_STRING (d_voiceName)

	oo_FROM (1)
		oo_STRING (d_phonemeSet)
	oo_ENDFROM
	#if oo_READING
		oo_VERSION_UNTIL (1)
			d_phonemeSet = Melder_dup (d_languageName.get());
			d_synthesizerVersion = Melder_dup (ESPEAK_NG_VERSION);
			oo_INTEGER (d_wordsPerMinute)
		oo_VERSION_ELSE
			oo_DOUBLE (d_wordsPerMinute)
		oo_VERSION_END
	#else
		oo_DOUBLE (d_wordsPerMinute)
	#endif
	// text-only, phonemes-only, mixed
	oo_INT (d_inputTextFormat)
	// 1/: output phonemes in espeak/ notation
	oo_INT (d_inputPhonemeCoding)
	// speech output
	oo_DOUBLE (d_samplingFrequency)
	oo_DOUBLE (d_wordgap)
	oo_DOUBLE (d_pitchAdjustment)
	oo_DOUBLE (d_pitchRange)
	// 1/2: output phonemes in espeak/IPA notation
	oo_INT (d_outputPhonemeCoding)

	#if oo_READING_TEXT
		oo_VERSION_UNTIL (1)
			oo_INT (d_estimateSpeechRate)   // this used to be oo_BOOL, which was written in text as 0 or 1, which is inappropriate for boolean text
		oo_VERSION_ELSE
			oo_QUESTION (d_estimateSpeechRate)
		oo_VERSION_END
	#else
		oo_QUESTION (d_estimateSpeechRate)
	#endif

	#if ! oo_READING && ! oo_WRITING
		// Filled by the callback
		oo_OBJECT (Table, 0, d_events)
		oo_DOUBLE (d_internalSamplingFrequency)
		oo_INTEGER (d_numberOfSamples)
		oo_INTEGER (d_wavCapacity)
		oo_INT_VECTOR (d_wav, d_wavCapacity)
	#endif

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (SpeechSynthesizer)
#undef ooSTRUCT

/* End of file SpeechSynthesizer_def.h */
