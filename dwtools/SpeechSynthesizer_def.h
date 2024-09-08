/* SpeechSynthesizer_def.h
 *
 * Copyright (C) 2011-2020 David Weenink, 2024 Paul Boersma
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

#define ooSTRUCT SpeechSynthesizer
oo_DEFINE_CLASS (SpeechSynthesizer, Daata)

	oo_FROM (1)
		oo_STRING (dummySynthesizerVersion)   // no longer used (was already overwritten at reading time)
	oo_ENDFROM

	/*
		Three language and voice settings.
	*/
	oo_STRING (d_languageName)
	oo_STRING (d_voiceName)
	oo_FROM (1)
		oo_STRING (d_phonemeSetName)
	oo_ENDFROM
	#if oo_READING
		oo_VERSION_UNTIL (1)
			d_phonemeSetName = Melder_dup (d_languageName.get());
		oo_VERSION_END
		SpeechSynthesizer_checkAndRepairLanguageAndVoiceNames (this);
	#endif

	#if oo_READING
		oo_VERSION_UNTIL (1)
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
	oo_DOUBLE (d_wordGap)
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

	#if ! oo_READING && ! oo_WRITING && ! oo_COMPARING
		// Filled by the callback
		oo_OBJECT (Table, 0, d_events)
		oo_DOUBLE (d_internalSamplingFrequency)
		oo_INTEGER (d_numberOfSamples)
		oo_INTEGER (d_wavCapacity)
		oo_INTVEC16 (d_wav, d_wavCapacity)
	#endif

	#if oo_DECLARING
		void v1_info ()
			override;
	#endif

oo_END_CLASS (SpeechSynthesizer)
#undef ooSTRUCT

/* End of file SpeechSynthesizer_def.h */
