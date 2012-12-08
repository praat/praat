/* SpeechSynthesizer_def.h
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

#define ooSTRUCT SpeechSynthesizerVoice
oo_DEFINE_CLASS (SpeechSynthesizerVoice, Data)
	oo_STRING (d_v_name)

	oo_LONG (d_phoneme_tab_ix)  // phoneme table number
	oo_LONG (d_pitch_base)    // Hz
	oo_LONG (d_pitch_range)   // Hz

	oo_LONG (d_speedf1)
	oo_LONG (d_speedf2)
	oo_LONG (d_speedf3)

	oo_DOUBLE (d_speed_percent)      // adjust the WPM speed by this percentage
	oo_DOUBLE (d_flutter)
	oo_DOUBLE (d_roughness)
	oo_DOUBLE (d_echo_delay)
	oo_DOUBLE (d_echo_amp)
	oo_LONG (d_n_harmonic_peaks)  // highest formant which is formed from adding harmonics
	oo_INT (d_peak_shape)        // alternative shape for formant peaks (0=standard 1=squarer)
	oo_DOUBLE (d_voicing)           // 100% = 64, level of formant-synthesized sound
	oo_DOUBLE (d_formant_factor)      // adjust nominal formant frequencies by this  because of the voice's pitch (256ths)
	oo_DOUBLE (d_consonant_amp)     // amplitude of unvoiced consonants
	oo_DOUBLE (d_consonant_ampv)    // amplitude of the noise component of voiced consonants
	oo_DOUBLE (d_samplingFrequency)
	oo_INT_VECTOR_FROM (d_klattv, 0, 7)

	// parameters used by Wavegen
	oo_LONG (d_numberOfFormants)
	oo_INT_VECTOR_FROM (d_freq, 0, d_numberOfFormants)		// 100% = 256
	oo_INT_VECTOR_FROM (d_height, 0, d_numberOfFormants)	// 100% = 256
	oo_INT_VECTOR_FROM (d_width, 0, d_numberOfFormants)		// 100% = 256
	oo_INT_VECTOR_FROM (d_freqadd, 0, d_numberOfFormants)	// Hz

	// copies without temporary adjustments from embedded commands
	oo_INT_VECTOR_FROM (d_freq2, 0, d_numberOfFormants)		// 100% = 256
	oo_INT_VECTOR_FROM (d_height2, 0, d_numberOfFormants)	// 100% = 256
	oo_INT_VECTOR_FROM (d_width2, 0, d_numberOfFormants)	// 100% = 256

	oo_INT_VECTOR_FROM (d_breath, 0, d_numberOfFormants)	// amount of breath for each formant. breath[0] indicates whether any are set.
	oo_INT_VECTOR_FROM (d_breathw, 0, d_numberOfFormants)	// width of each breath formant

oo_END_CLASS (SpeechSynthesizerVoice)
#undef ooSTRUCT


#define ooSTRUCT SpeechSynthesizer
oo_DEFINE_CLASS (SpeechSynthesizer, Data)

	// sythesizers language /voice
	oo_STRING (d_voiceLanguageName)
	oo_STRING (d_voiceVariantName)
	oo_LONG (d_wordsPerMinute)
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
	oo_BOOL (d_estimateWordsPerMinute)

	#if !oo_READING && !oo_WRITING
		// Filled by the call back
		oo_OBJECT(Table, 0, d_events)
		oo_DOUBLE (d_internalSamplingFrequency)
		oo_LONG (d_numberOfSamples)
		oo_LONG (d_wavCapacity)
		oo_INT_VECTOR (d_wav, d_wavCapacity)
	#endif
	#if oo_READING
		SpeechSynthesizer_initSoundBuffer (this);
	#endif
	#if oo_DECLARING
		protected:
		// overridden methods:
			virtual void v_info ();
	#endif

oo_END_CLASS (SpeechSynthesizer)
#undef ooSTRUCT

/* End of file SpeechSynthesizer_def.h */
