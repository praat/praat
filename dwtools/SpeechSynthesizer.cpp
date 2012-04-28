/* SpeechSynthesizer.cpp
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

#include "SpeechSynthesizer.h"
#include "speech.h"
#include "speak_lib.h"
#include "phoneme.h"
#include "synthesize.h"
#include "voice.h"
#include "translate.h"

#define espeak_SAMPLINGFREQUENCY 22050

extern structMelderDir praatDir;
extern int option_phoneme_events;

Thing_implement (SpeechSynthesizer, Data, 0);

void structSpeechSynthesizer :: v_destroy () {
	Melder_free (d_voiceName);
	Melder_free (d_voiceVariantName);
	NUMvector_free<short>  (d_wav, 1);
	Melder_free (d_punctuations);
	SpeechSynthesizer_Parent :: v_destroy ();
}

void structSpeechSynthesizer :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine2 (L"Voice name: ", d_voiceName);
	MelderInfo_writeLine2 (L"Voice variant: ", d_voiceVariantName);
	MelderInfo_writeLine2 (L"Input text format: ", (d_inputTextFormat == SpeechSynthesizer_INPUT_TEXTONLY ? L"text only" :
		d_inputTextFormat == SpeechSynthesizer_INPUT_PHONEMESONLY ? L"phonemes only" : L"tagged text"));
	MelderInfo_writeLine2 (L"Input phoneme coding: ", (d_inputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM ? L"Kirshenbaum" : L"???"));
	MelderInfo_writeLine3 (L"Sampling frequency: ", Melder_double (d_samplingFrequency), L" Hz");
	MelderInfo_writeLine3 (L"Word gap: ", Melder_double (d_wordgap), L" s");
	MelderInfo_writeLine3 (L"Pitch adjustment value: ", Melder_integer (d_pitchAdjustment), L" (0-100)");
	MelderInfo_writeLine4 (L"Speeking rate: ", Melder_integer (d_wordsPerMinute), L" words per minute", (d_estimateWordsPerMinute ? L" (but estimated from data if possible)" : L" (fixed)"));

	MelderInfo_writeLine2 (L"Output phoneme coding: ", (d_inputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM ? L"Kirshenbaum" : d_inputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_IPA ? L"IPA" : L"???"));
}

static void NUMvector_extendNumberOfElements (long elementSize, void **v, long lo, long *hi, long extraDemand)
{
	try {
		char *result;
		if (*v == NULL) {
			long newhi = lo + extraDemand - 1;
			result = reinterpret_cast <char *> (NUMvector (elementSize, lo, newhi));
			*hi = newhi;
		} else {
			long offset = lo * elementSize;
			for (;;) {   // not very infinite: 99.999 % of the time once, 0.001 % twice
				result = reinterpret_cast <char *> (Melder_realloc ((char *) *v + offset, (*hi - lo + 1 + extraDemand) * elementSize));
				if ((result -= offset) != NULL) break;   // this will normally succeed at the first try
				(void) Melder_realloc_f (result + offset, 1);   // ??make "sure" that the second try will succeed
			}
			(*hi) += extraDemand;
			memset (result + *hi * elementSize, 0, elementSize);   // initialize the new elements to zeroes
		}
		*v = result;
	} catch (MelderError) {
		Melder_throw ("Vector: size not extended.");
	}
}

static void NUMvector_supplyStorage (long elementSize, void **v, long lo, long *hi, long nfilled, long extraDemand)
{
	long old_capacity = *hi - lo + 1, new_capacity = nfilled + extraDemand;
	if (new_capacity < old_capacity) return;
	new_capacity = new_capacity > 2 * old_capacity ? new_capacity : 2 * old_capacity;
	NUMvector_extendNumberOfElements (elementSize, v, lo, hi, new_capacity);
}

template <class T>
void NUMvector_supplyStorage (T** v, long lo, long *hi, long nfilled, long extraDemand) {
	NUMvector_supplyStorage (sizeof (T), (void**) v, lo, hi, nfilled, extraDemand);
}

static int synthCallback (short *wav, int numsamples, espeak_EVENT *events)
{
	char phoneme_name[9];
	if (wav == 0) return 1;
	// It is essential that the SpeechSynthesizer is identified here by the user_data,
	// because the espeakEVENT_LIST_TERMINATED event may still be accompanied by
	// a piece of audio data!!
	SpeechSynthesizer me = (SpeechSynthesizer) (events -> user_data);
	while(events -> type != espeakEVENT_LIST_TERMINATED) {
		if (events -> type == espeakEVENT_SAMPLERATE) {
			my d_internalSamplingFrequency = events -> id.number;
		} else {
			//my events = Table "time type type-t t-pos length a-pos sample id uniq";
			//                    1    2     3      4     5     6     7      8   9
			Table_appendRow (my d_events);
			long irow = my d_events -> rows -> size;
			double time = events -> audio_position * 0.001;
			Table_setNumericValue (my d_events, irow, 1, time);
			Table_setNumericValue (my d_events, irow, 2, events -> type);
			// Column 3 will be filled afterwards
			Table_setNumericValue (my d_events, irow, 4, events -> text_position);
			Table_setNumericValue (my d_events, irow, 5, events -> length);
			Table_setNumericValue (my d_events, irow, 6, events -> audio_position);
			Table_setNumericValue (my d_events, irow, 7, events -> sample);
			if (events -> type == espeakEVENT_MARK || events -> type == espeakEVENT_PLAY) {
				Table_setStringValue (my d_events, irow, 8, Melder_peekUtf8ToWcs (events -> id.name));
			} else {
				// Ugly hack because id.string in not 0-terminated if 8 chars long!
				memcpy (phoneme_name, events -> id.string, 8);
				phoneme_name[8] = 0;
				Table_setStringValue (my d_events, irow, 8, Melder_peekUtf8ToWcs (phoneme_name));
			}
			Table_setNumericValue (my d_events, irow, 9, events -> unique_identifier);
		}
		events++;
	}
	if (me != 0) {
		NUMvector_supplyStorage<short> (&my d_wav, 1, &my d_wavCapacity, my d_numberOfSamples, numsamples);
		for (long i = 1; i <= numsamples; i++) {
			my d_wav[my d_numberOfSamples + i] = wav[i - 1];
		}
		my d_numberOfSamples += numsamples;
	}
	return 0;
}

static void SpeechSynthesizer_setDefaults (SpeechSynthesizer me)
{
	SpeechSynthesizer_setTextInputSettings (me, SpeechSynthesizer_INPUT_TEXTONLY, SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM);
	SpeechSynthesizer_setSpeechOutputSettings (me, 44100, 0.01, 50, 50, 175, true, SpeechSynthesizer_PHONEMECODINGS_IPA);
}

SpeechSynthesizer SpeechSynthesizer_create (long voiceIndex, long voiceVariantIndex) {
	try {
		autoSpeechSynthesizer me = Thing_new (SpeechSynthesizer);
		my d_voice = voiceIndex; my d_voiceVariant = voiceVariantIndex;
		FileInMemory fim = (FileInMemory) espeakdata_voices -> item[voiceIndex];
		my d_voiceName = Melder_wcsdup (fim -> d_id);
		if (voiceVariantIndex != 1) { // 1 is default, i.e. no variant
            voiceVariantIndex--; // !!!
			FileInMemory vfim = (FileInMemory) espeakdata_variants -> item[voiceVariantIndex];
			my d_voiceVariantName = Melder_wcsdup (vfim -> d_id);
		} else {
			my d_voiceVariantName = Melder_wcsdup (L"default"); // TODO what is the default?
		}
		my d_wavCapacity = 2 * 22050; // 2 seconds
		my d_wav = NUMvector<short> (1, my d_wavCapacity);
		SpeechSynthesizer_setDefaults (me.peek());
		int fsamp = espeak_Initialize (AUDIO_OUTPUT_SYNCHRONOUS, 0, NULL, espeakINITIALIZE_PHONEME_EVENTS); // 4000 ms
		if (fsamp == -1) Melder_throw ("Internal espeak error.");
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("SpeechSynthesizer not created.");
	}
}

void SpeechSynthesizer_setTextInputSettings (SpeechSynthesizer me, int inputTextFormat, int inputPhonemeCoding) {
	my d_inputTextFormat = inputTextFormat;
	my d_inputPhonemeCoding = inputPhonemeCoding;
}

void SpeechSynthesizer_setSpeechOutputSettings (SpeechSynthesizer me, double samplingFrequency, double wordgap, long pitchAdjustment, long pitchRange, long wordsPerMinute, bool estimateWordsPerMinute, int outputPhonemeCoding) {
	my d_samplingFrequency = samplingFrequency;
	my d_wordgap = wordgap;
	my d_pitchAdjustment = pitchAdjustment;
	my d_pitchRange = pitchRange;

	if (wordsPerMinute <= 0) wordsPerMinute = 175;
	if (wordsPerMinute > 450) wordsPerMinute = 450;
	if (wordsPerMinute < 80) wordsPerMinute = 80;
	my d_wordsPerMinute = wordsPerMinute;
	my d_estimateWordsPerMinute = estimateWordsPerMinute;
	my d_outputPhonemeCoding = outputPhonemeCoding;
}

void SpeechSynthesizer_playText (SpeechSynthesizer me, const wchar_t *text) {
	autoSound thee= SpeechSynthesizer_to_Sound (me, text, 0, 0);
	Sound_playPart (thee.peek(), thy xmin, thy xmax, 0, 0);
}

static Sound buffer_to_Sound (short *wav, long numberOfSamples, double samplingFrequency)
{
	try {
		double dx = 1.0 / samplingFrequency;
		double xmax = numberOfSamples * dx;
		autoSound thee = Sound_create (1, 0, xmax, numberOfSamples, dx, dx / 2);
		for (long i = 1; i <= numberOfSamples; i++) {
			thy z[1][i] = wav[i] / 32768.0;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("Sound not created from synthesizer data.");
	}
}

static void IntervalTier_addBoundaryUnsorted (IntervalTier me, long iinterval, double time, const wchar_t *newLabel, bool isNewleftLabel) {
	if (time <= my xmin || time >= my xmax) {
		Melder_throw ("Time is outside interval.");
	}

	// Find interval to split
	if (iinterval <= 0) {
		iinterval = IntervalTier_timeToLowIndex (me, time);
	}

	// Modify end time of left label
	TextInterval ti = (TextInterval) my intervals -> item[iinterval];
	ti -> xmax = time;
	if (isNewleftLabel) TextInterval_setText (ti, newLabel);

	autoTextInterval ti_new = TextInterval_create (time, my xmax, (! isNewleftLabel ? newLabel : L""));
	Sorted_addItem_unsorted (my intervals, ti_new.transfer());
}

void MelderString_removeSpecialCharactersFromEnd (MelderString *me) {
	wchar_t kar;
	while (my length > 0 && (kar = my string[my length - 1]) == ' ') { my length --; }
	my string[my length] = '\0';
}

static void Table_setEventTypeString (Table me) {
	try {
		for (long i = 1; i <= my rows -> size; i++) {
			int type = Table_getNumericValue_Assert (me, i, 2);
			const wchar_t *label = L"0";
			if (type == espeakEVENT_WORD) {
				label = L"word";
			} else if (type == espeakEVENT_SENTENCE) {
				label = L"sent";
			} else if (type == espeakEVENT_MARK) {
				label = L"mark";
			} else if (type == espeakEVENT_PLAY) {
				label = L"play";
			} else if (type == espeakEVENT_END) {
				label = L"s-end";
			} else if (type == espeakEVENT_MSG_TERMINATED) {
				label = L"msg_term";
			} else if (type == espeakEVENT_PHONEME) {
				label = L"phoneme";
			}
			Table_setStringValue (me, i, 3, label);
		}
	} catch (MelderError) {
		Melder_throw ("Event types not set.");
	}
}

static TextGrid Table_to_TextGrid (Table me, const wchar_t *text, double xmin, double xmax) {
	//Table_createWithColumnNames (0, L"time type type-t t-pos length a-pos sample id uniq");
	try {
		long length, textLength = wcslen (text);
		long numberOfRows = my rows -> size;
		long timeColumnIndex = Table_getColumnIndexFromColumnLabel (me, L"time");
		long typeColumnIndex = Table_getColumnIndexFromColumnLabel (me, L"type");
		long tposColumnIndex = Table_getColumnIndexFromColumnLabel (me, L"t-pos");
		long   idColumnIndex = Table_getColumnIndexFromColumnLabel (me, L"id");
		autoTextGrid thee = TextGrid_create (xmin, xmax, L"sentence clause word phoneme", L"");

		TextGrid_setIntervalText (thee.peek(), 1, 1, text);

		long p1c = 1, p1w = 1;
		double t1p = xmin;
		bool wordEnd = false;
		autoMelderString mark;

		IntervalTier itc = (IntervalTier) thy tiers -> item[2];
		IntervalTier itw = (IntervalTier) thy tiers -> item[3];
		IntervalTier itp = (IntervalTier) thy tiers -> item[4];

		for (long i = 1; i <= numberOfRows; i++) {
			double time = Table_getNumericValue_Assert (me, i, timeColumnIndex);
			int type = Table_getNumericValue_Assert (me, i, typeColumnIndex);
			long pos = Table_getNumericValue_Assert (me, i, tposColumnIndex);
			if (type == espeakEVENT_SENTENCE) {
				// Only insert a new boundary, no text
				// text will be inserted at end sentence event
				if (time > xmin and time < xmax) {
					IntervalTier_addBoundaryUnsorted (itc, itc -> intervals -> size, time, L"", true);
				}
				p1c = pos;
			} else if (type == espeakEVENT_END) {
				// End of clause: insert new boundary, and fill left interval with text
				length = pos - p1c + 1;
				MelderString_ncopy (&mark, text + p1c - 1, length);
				if (time > xmin and time < xmax) {
					IntervalTier_addBoundaryUnsorted (itc, itc -> intervals -> size, time, mark.string, true);
				} else {
					TextGrid_setIntervalText (thee.peek(), 2, itc -> intervals -> size, mark.string);
				}
				MelderString_empty (&mark);
				p1c = pos;

				// End of clause always signals "end of a word"

				if (pos <= textLength) {
					length = pos - p1w + 1;
					MelderString_ncopy (&mark, text + p1w - 1, length);
					if (time > xmin and time < xmax) {
						IntervalTier_addBoundaryUnsorted (itw, itw -> intervals -> size, time, mark.string, true);
					} else {
						TextGrid_setIntervalText (thee.peek(), 3, itw -> intervals -> size, mark.string);
					}
					MelderString_empty (&mark);
					// now the next word event should not trigger setting the left interval text
					wordEnd = false;
				}
			} else if (type == espeakEVENT_WORD) {
				if (pos < p1w) continue;
				if (time > xmin and time < xmax) {
					length = pos - p1w;
					if (pos == textLength) length++;
					MelderString_ncopy (&mark, text + p1w - 1, length);
					IntervalTier_addBoundaryUnsorted (itw, itw -> intervals -> size, time, (wordEnd ? mark.string : L""), true);
					MelderString_empty (&mark);
				}
				wordEnd = true;
				p1w = pos;
			} else if (type == espeakEVENT_PHONEME) {
				const wchar_t *id = Table_getStringValue_Assert (me, i, idColumnIndex);
				if (time > t1p) {
					// Insert new boudary and label interval with the id
					// TODO: Translate the id to the correct notation
					TextInterval ti = (TextInterval) itp -> intervals -> item[itp -> intervals -> size];
					if (time > ti -> xmin and time < ti -> xmax) {
						IntervalTier_addBoundaryUnsorted (itp, itp -> intervals -> size, time, id, false);
					}
				} else {
					// Just in case the phoneme starts at xmin we only need to set interval text
					TextGrid_setIntervalText (thee.peek(), 4, itp -> intervals -> size, id);
				}
				t1p = time;
			}
		}
		Sorted_sort (itc -> intervals);
		Sorted_sort (itw -> intervals);
		Sorted_sort (itp -> intervals);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("TextGrid not created from Table with events.");
	}
}

static void espeakdata_SetVoiceByName (const char *name, const char *variantName)
{
	espeak_VOICE voice_selector;
//	autoMelderString voice;
//	MelderString_append (&voice, Melder_peekUtf8ToWcs (name), L"+", Melder_peekUtf8ToWcs (variantName));

	memset(&voice_selector, 0, sizeof(voice_selector));
	MelderString npv = { 0 };
	MelderString_append (&npv, Melder_peekUtf8ToWcs (name), L"+", Melder_peekUtf8ToWcs (variantName));

	voice_selector.name = (char *) Melder_peekWcsToUtf8 (npv.string);  // include variant name in voice stack ??
//	voice_selector.name = (char *) name;  // include variant name in voice stack ??

	if (LoadVoice (name,1) != NULL)
	{
		LoadVoice(variantName, 2);
		DoVoiceChange(voice);
		SetVoiceStack (&voice_selector, variantName);
	}
}

Sound SpeechSynthesizer_to_Sound (SpeechSynthesizer me, const wchar_t *text, TextGrid *tg, Table *events) {
	try {
		int fsamp = espeak_Initialize (AUDIO_OUTPUT_SYNCHRONOUS, 0, NULL, // 5000ms
			espeakINITIALIZE_PHONEME_EVENTS|espeakINITIALIZE_PHONEME_IPA);
		if (fsamp == -1) Melder_throw ("Internal espeak error.");
		int synth_flags = espeakCHARS_WCHAR;
		if (my d_inputTextFormat == SpeechSynthesizer_INPUT_TAGGEDTEXT) synth_flags |= espeakSSML;
		if (my d_inputTextFormat != SpeechSynthesizer_INPUT_TEXTONLY) synth_flags |= espeakPHONEMES;
		option_phoneme_events = espeakINITIALIZE_PHONEME_EVENTS; // extern int option_phoneme_events;
		if (my d_outputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_IPA) {
			option_phoneme_events |= espeakINITIALIZE_PHONEME_IPA;
		}

		espeak_SetSynthCallback (synthCallback);

		espeak_SetParameter (espeakRATE, my d_wordsPerMinute, 0);
		espeak_SetParameter (espeakPITCH, my d_pitchAdjustment, 0);
		espeak_SetParameter (espeakRANGE, my d_pitchRange, 0);
		espeakdata_SetVoiceByName ((const char *) Melder_peekWcsToUtf8 (my d_voiceName),
			(const char *) Melder_peekWcsToUtf8 (my d_voiceVariantName));

		espeak_SetParameter (espeakWORDGAP, my d_wordgap * 100, 0); // espeak wordgap is in units of 10 ms
		espeak_SetParameter (espeakCAPITALS, 0, 0);
		espeak_SetParameter (espeakPUNCTUATION, espeakPUNCT_NONE, 0);

		my d_events = Table_createWithColumnNames (0, L"time type type-t t-pos length a-pos sample id uniq");

		long textLength = wcslen (text);

		espeak_Synth (text, textLength + 1, 0, POS_CHARACTER, 0, synth_flags, NULL, me);
		espeak_Terminate ();
		autoSound thee = buffer_to_Sound (my d_wav, my d_numberOfSamples, my d_internalSamplingFrequency);

		if (my d_samplingFrequency != my d_internalSamplingFrequency) {
			thee.reset (Sound_resample (thee.peek(), my d_samplingFrequency, 50));
		}
		my d_numberOfSamples = 0; // re-use the wav-buffer
		if (tg != NULL) {
			double xmin = Table_getNumericValue_Assert (my d_events, 1, 1);
			if (xmin > thy xmin) xmin = thy xmin;
			double xmax = Table_getNumericValue_Assert (my d_events, my d_events -> rows -> size, 1);
			if (xmax < thy xmax) xmax = thy xmax;
			autoTextGrid atg1 = Table_to_TextGrid (my d_events, text, xmin, xmax);
			autoTextGrid atg2 =  TextGrid_extractPart (atg1.peek(), thy xmin, thy xmax, 0);
			*tg = atg2.transfer();
		}
		if (events != NULL) {
			Table_setEventTypeString (my d_events);
			*events = my d_events;
			my d_events = 0;
		}
		forget (my d_events);
		return thee.transfer();
	} catch (MelderError) {
		espeak_Terminate ();
		Melder_throw ("Text not played.");
	}
}

/* End of file SpeechSynthesizer.cpp */