/* SpeechSynthesizer.cpp
 *
//  * Copyright (C) 2011-2013, 2015-2016 David Weenink
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

#include "SpeechSynthesizer.h"
#include "Strings_extensions.h"
#include "translate.h"

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


#define espeak_SAMPLINGFREQUENCY 22050

extern structMelderDir praatDir;
extern int option_phoneme_events;

Thing_implement (SpeechSynthesizerVoice, Daata, 0);

autoSpeechSynthesizerVoice SpeechSynthesizerVoice_create (long numberOfFormants) {
	try {
		autoSpeechSynthesizerVoice me = Thing_new (SpeechSynthesizerVoice);
		my d_numberOfFormants = numberOfFormants;
		my d_freq = NUMvector<int> (0, numberOfFormants);
		my d_height = NUMvector<int> (0, my d_numberOfFormants);	// 100% = 256
		my d_width = NUMvector<int> (0, my d_numberOfFormants);		// 100% = 256
		my d_freqadd = NUMvector<int> (0, my d_numberOfFormants);	// Hz

		// copies without temporary adjustments from embedded commands
		my d_freq2 = NUMvector<int> (0, my d_numberOfFormants);		// 100% = 256
		my d_height2 = NUMvector<int> (0, my d_numberOfFormants);	// 100% = 256
		my d_width2 = NUMvector<int> (0, my d_numberOfFormants);	// 100% = 256

		my d_breath = NUMvector<int> (0, my d_numberOfFormants);	// amount of breath for each formant. breath[0] indicates whether any are set.
		my d_breathw = NUMvector<int> (0, my d_numberOfFormants);	// width of each breath formant
		SpeechSynthesizerVoice_setDefaults (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"SpeechSynthesizerVoice not created.");
	}
}

void SpeechSynthesizerVoice_setDefaults (SpeechSynthesizerVoice me) {
	(void) me;
}

void SpeechSynthesizerVoice_initFromEspeakVoice (SpeechSynthesizerVoice me, voice_t *evoice) {
	my d_v_name = Melder_dup (Melder_peek8to32 (evoice -> v_name));

	my d_phoneme_tab_ix = evoice -> phoneme_tab_ix;
	my d_pitch_base = evoice -> pitch_base;
	my d_pitch_range = evoice -> pitch_range;

	my d_speedf1 = evoice -> speedf1;
	my d_speedf2 = evoice -> speedf2;
	my d_speedf3 = evoice -> speedf3;

	my d_speed_percent = evoice -> speed_percent;
	my d_flutter = evoice -> flutter;
	my d_roughness = evoice -> roughness;
	my d_echo_delay = evoice -> echo_delay;
	my d_echo_amp = evoice -> echo_amp;
	my d_n_harmonic_peaks = evoice -> n_harmonic_peaks;
	my d_peak_shape = evoice -> peak_shape;
	my d_voicing = evoice -> voicing;
	my d_formant_factor = evoice -> formant_factor;
	my d_consonant_amp = evoice -> consonant_amp;
	my d_consonant_ampv = evoice -> consonant_ampv;
	my d_samplingFrequency = evoice -> samplerate;
	for (long i = 0; i < 7; i++) {
		my d_klattv[i] = evoice -> klattv[i];
	}
	for (long i = 0; i <= my d_numberOfFormants; i++) {
		my d_freq[i] = evoice -> freq[i];
		my d_height[i] = evoice -> height[i];
		my d_width[i] = evoice -> width[i];
		my d_freqadd[i] = evoice -> freqadd[i];
		my d_freq2[i] = evoice -> freq2[i];
		my d_height2[i] = evoice -> height2[i];
		my d_width2[i] = evoice -> width2[i];
		my d_breath[i] = evoice -> breath[i];
		my d_breathw[i] = evoice -> breathw[i];
	}
}

Thing_implement (SpeechSynthesizer, Daata, 0);

void structSpeechSynthesizer :: v_info () {
	SpeechSynthesizer_Parent :: v_info ();
	MelderInfo_writeLine (U"Voice language: ", d_voiceLanguageName);
	MelderInfo_writeLine (U"Voice variant: ", d_voiceVariantName);
	MelderInfo_writeLine (U"Input text format: ", (d_inputTextFormat == SpeechSynthesizer_INPUT_TEXTONLY ? U"text only" :
		d_inputTextFormat == SpeechSynthesizer_INPUT_PHONEMESONLY ? U"phonemes only" : U"tagged text"));
	MelderInfo_writeLine (U"Input phoneme coding: ", (d_inputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM ? U"Kirshenbaum" : U"???"));
	MelderInfo_writeLine (U"Sampling frequency: ", d_samplingFrequency, U" Hz");
	MelderInfo_writeLine (U"Word gap: ", d_wordgap, U" s");
	MelderInfo_writeLine (U"Pitch adjustment value: ", d_pitchAdjustment, U" (0-100)");
	MelderInfo_writeLine (U"Speeking rate: ", d_wordsPerMinute, U" words per minute", (d_estimateWordsPerMinute ? U" (but estimated from data if possible)" : U" (fixed)"));

	MelderInfo_writeLine (U"Output phoneme coding: ", (d_inputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM ? U"Kirshenbaum" : d_inputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_IPA ? U"IPA" : U"???"));
	MelderInfo_writeLine (U"Text to speech synthesis with eSpeak version 1.48.04");
}

static void NUMvector_extendNumberOfElements (long elementSize, void **v, long lo, long *hi, long extraDemand)
{
	try {
		char *result;
		if (! *v) {
			long newhi = lo + extraDemand - 1;
			result = reinterpret_cast <char *> (NUMvector (elementSize, lo, newhi, true));
			*hi = newhi;
		} else {
			long offset = lo * elementSize;
			for (;;) {   // not very infinite: 99.999 % of the time once, 0.001 % twice
				result = reinterpret_cast <char *> (Melder_realloc ((char *) *v + offset, (*hi - lo + 1 + extraDemand) * elementSize));
				if ((result -= offset)) break;   // this will normally succeed at the first try
				(void) Melder_realloc_f (result + offset, 1);   // ??make "sure" that the second try will succeed
			}
			(*hi) += extraDemand;
			memset (result + *hi * elementSize, 0, elementSize);   // initialize the new elements to zeroes
		}
		*v = result;
	} catch (MelderError) {
		Melder_throw (U"Vector: size not extended.");
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
	while (events -> type != espeakEVENT_LIST_TERMINATED) {
		if (events -> type == espeakEVENT_SAMPLERATE) {
			my d_internalSamplingFrequency = events -> id.number;
		} else {
			//my events = Table "time type type-t t-pos length a-pos sample id uniq";
			//                    1    2     3      4     5     6     7      8   9
			Table_appendRow (my d_events.get());
			long irow = my d_events -> rows.size;
			double time = events -> audio_position * 0.001;
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
				phoneme_name[8] = 0;
				Table_setStringValue (my d_events.get(), irow, 8, Melder_peek8to32 (phoneme_name));
			}
			Table_setNumericValue (my d_events.get(), irow, 9, events -> unique_identifier);
		}
		events++;
	}
	if (me) {
		NUMvector_supplyStorage<int> (&my d_wav, 1, &my d_wavCapacity, my d_numberOfSamples, numsamples);
		for (long i = 1; i <= numsamples; i++) {
			my d_wav [my d_numberOfSamples + i] = wav [i - 1];
		}
		my d_numberOfSamples += numsamples;
	}
	return 0;
}

const char32 *SpeechSynthesizer_getVoiceLanguageCodeFromName (SpeechSynthesizer /* me */, const char32 *voiceLanguageName) {
	try {
		long voiceLanguageNameIndex = Strings_findString (espeakdata_voices_names.get(), voiceLanguageName);
		if (voiceLanguageNameIndex == 0) {
			Melder_throw (U"Cannot find language \"", voiceLanguageName, U"\".");
		}
		FileInMemory fim = espeakdata_voices->at [voiceLanguageNameIndex];
		return fim -> d_id;
	} catch (MelderError) {
		Melder_throw (U"Cannot find language code.");
	}
}

const char32 *SpeechSynthesizer_getVoiceVariantCodeFromName (SpeechSynthesizer /* me */, const char32 *voiceVariantName) {
	try {
		static const char32 * defaultVariantCode = U"default";
		// Strings espeakdata_variants_names is one longer than the actual list of variants
		long voiceVariantIndex = Strings_findString (espeakdata_variants_names.get(), voiceVariantName);
		if (voiceVariantIndex == 0) {
			Melder_throw (U"Cannot find voice variant \"", voiceVariantName, U"\".");
		}
		// ... we have to decrease the index
		if (voiceVariantIndex != 1) { // 1 is default, i.e. no variant
			voiceVariantIndex --; // !!!
			FileInMemory vfim = espeakdata_variants->at [voiceVariantIndex];
			return vfim -> d_id;
		} else {
			return defaultVariantCode; // TODO what is the default?
		}
	} catch (MelderError) {
		Melder_throw (U"Cannot find voice variant code.");
	}
}

void SpeechSynthesizer_initEspeak () {
	int fsamp = espeak_Initialize (AUDIO_OUTPUT_SYNCHRONOUS, 0, nullptr, espeakINITIALIZE_PHONEME_EVENTS); // 4000 ms
	if (fsamp == -1) {
		Melder_throw (U"Internal espeak error.");
	}
}

autoSpeechSynthesizer SpeechSynthesizer_create (const char32 *voiceLanguageName, const char32 *voiceVariantName) {
	try {
		autoSpeechSynthesizer me = Thing_new (SpeechSynthesizer);
		(void) SpeechSynthesizer_getVoiceLanguageCodeFromName (me.get(), voiceLanguageName);
		(void) SpeechSynthesizer_getVoiceVariantCodeFromName (me.get(), voiceVariantName);
		my d_voiceLanguageName = Melder_dup (voiceLanguageName);
		my d_voiceVariantName = Melder_dup (voiceVariantName);
		SpeechSynthesizer_setTextInputSettings (me.get(), SpeechSynthesizer_INPUT_TEXTONLY, SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM);
		SpeechSynthesizer_setSpeechOutputSettings (me.get(), 44100, 0.01, 50, 50, 175, true, SpeechSynthesizer_PHONEMECODINGS_IPA);
		SpeechSynthesizer_initEspeak ();
		return me;
	} catch (MelderError) {
		Melder_throw (U"SpeechSynthesizer not created.");
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

void SpeechSynthesizer_playText (SpeechSynthesizer me, const char32 *text) {
	autoSound thee = SpeechSynthesizer_to_Sound (me, text, nullptr, nullptr);
	Sound_playPart (thee.get(), thy xmin, thy xmax, nullptr, nullptr);
}

static autoSound buffer_to_Sound (int *wav, long numberOfSamples, double samplingFrequency)
{
	try {
		double dx = 1.0 / samplingFrequency;
		double xmax = numberOfSamples * dx;
		autoSound thee = Sound_create (1, 0.0, xmax, numberOfSamples, dx, dx / 2.0);
		for (long i = 1; i <= numberOfSamples; i++) {
			thy z[1][i] = wav[i] / 32768.0;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from synthesizer data.");
	}
}

static void IntervalTier_addBoundaryUnsorted (IntervalTier me, long iinterval, double time, const char32 *newLabel, bool isNewleftLabel) {
	if (time <= my xmin || time >= my xmax) {
		Melder_throw (U"Time is outside interval domains.");
	}

	// Find interval to split
	if (iinterval <= 0) {
		iinterval = IntervalTier_timeToLowIndex (me, time);
	}

	// Modify end time of left label
	TextInterval ti = my intervals.at [iinterval];
	ti -> xmax = time;
	if (isNewleftLabel) TextInterval_setText (ti, newLabel);

	autoTextInterval ti_new = TextInterval_create (time, my xmax, (! isNewleftLabel ? newLabel : U"" ));
	my intervals. addItem_unsorted_move (ti_new.move());
}

static void Table_setEventTypeString (Table me) {
	try {
		for (long i = 1; i <= my rows.size; i ++) {
			int type = Table_getNumericValue_Assert (me, i, 2);
			const char32 *label = U"0";
			if (type == espeakEVENT_WORD) {
				label = U"word";
			} else if (type == espeakEVENT_SENTENCE) {
				label = U"sent";
			} else if (type == espeakEVENT_MARK) {
				label = U"mark";
			} else if (type == espeakEVENT_PLAY) {
				label = U"play";
			} else if (type == espeakEVENT_END) {
				label = U"s-end";
			} else if (type == espeakEVENT_MSG_TERMINATED) {
				label = U"msg_term";
			} else if (type == espeakEVENT_PHONEME) {
				label = U"phoneme";
			}
			Table_setStringValue (me, i, 3, label);
		}
	} catch (MelderError) {
		Melder_throw (U"Event types not set.");
	}
}

static void MelderString_trimWhiteSpaceAtEnd (MelderString *me) {
	while (my length > 1 && (my string [my length - 1] == U' ' || my string [my length - 1] == U'\t'
		|| my string [my length - 1] == U'\r' || my string [my length - 1] == U'\n'))
	{
		my string [my length - 1] = U'\0';
		my length--;
	}
}

static void IntervalTier_mergeSpecialIntervals (IntervalTier me) {
	long intervalIndex = my intervals.size;
	TextInterval right = my intervals.at [intervalIndex];
	long labelLength_right = TextInterval_labelLength (right);
	bool isEmptyInterval_right = labelLength_right == 0 || (labelLength_right == 1 && Melder_equ (right -> text, U"\001"));
	while (intervalIndex > 1) {
		TextInterval left = my intervals.at [intervalIndex - 1];
		long labelLength_left = TextInterval_labelLength (left);
		bool isEmptyInterval_left = labelLength_left == 0 || (labelLength_left == 1 && Melder_equ (left -> text, U"\001"));
		if (isEmptyInterval_right && isEmptyInterval_left) {
			// remove right interval and empty left interval
			left -> xmax = right -> xmax;
			TextInterval_setText (left, U""); 
			my intervals. removeItem (intervalIndex);
		}
		right = left; 
		isEmptyInterval_right = isEmptyInterval_left;
		intervalIndex --;
	}
}

#if 0
/* insert boundary at time t and merge/delete intervals after this time */
static void IntervalTier_insertBoundaryAndMergeIntervalsAfter (IntervalTier me, double t) {
	if (t <= my xmin || t >= my xmax) {
		return;
	}
	
	long intervalNumber = IntervalTier_timeToLowIndex (me, t);
	while (my intervals.size > intervalNumber + 1) {
		my intervals. removeItem (my intervals.size);
	}
	// there can be maximally one interval left to the right of intervalNumber
	TextInterval ti = my intervals.at [intervalNumber];
	if (ti -> xmin == t) {   // if t happens to be on a boundary: remove the next interval if it exists
		if (my intervals.size > intervalNumber) {
			my intervals. removeItem (my intervals .size);
		}
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
	for (long iint = 1; iint <= from -> intervals.size; iint ++) {
		TextInterval tifrom = from -> intervals.at [iint];
		if (TextInterval_labelLength (tifrom) == 0) {   // found empty interval
			double t_left = tifrom -> xmin, t_right = tifrom -> xmax;
			long intervalIndex_to = IntervalTier_timeToLowIndex (to, t_left);
			if (intervalIndex_to > 0) {   // insert to the right of intervalIndex_to
				TextInterval tito = to -> intervals.at [intervalIndex_to];
				if (! almost_equal (tito -> xmin, t_left)) {   // not on the start boundary of the interval, it cannot be at xmax
					autoTextInterval newInterval = TextInterval_create (t_left, tito -> xmax, U"");
					tito -> xmax = t_left;
					to -> intervals. addItem_move (newInterval.move());
				}
			}
			intervalIndex_to = IntervalTier_timeToHighIndex (to, t_right);
			TextInterval tito = to -> intervals.at [intervalIndex_to];
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
	long iint = 1;
	while (iint <= my intervals.size) {
		TextInterval ti = my intervals.at [iint];
		if (almost_equal (ti -> xmin, ti -> xmax)) {
			my intervals.removeItem (iint);
		} else {
			iint ++;
		}
	}
}

static autoTextGrid Table_to_TextGrid (Table me, const char32 *text, double xmin, double xmax) {
	//Table_createWithColumnNames (0, L"time type type-t t-pos length a-pos sample id uniq");
	try {
		long length, textLength = str32len (text);
		long numberOfRows = my rows.size;
		long timeColumnIndex = Table_getColumnIndexFromColumnLabel (me, U"time");
		long typeColumnIndex = Table_getColumnIndexFromColumnLabel (me, U"type");
		long tposColumnIndex = Table_getColumnIndexFromColumnLabel (me, U"t-pos");
		long   idColumnIndex = Table_getColumnIndexFromColumnLabel (me, U"id");
		autoTextGrid thee = TextGrid_create (xmin, xmax, U"sentence clause word phoneme", U"");

		TextGrid_setIntervalText (thee.get(), 1, 1, text);

		long p1c = 1, p1w = 1;
		double time_phon_p = xmin;
		bool wordEnd = false;
		autoMelderString mark;

		IntervalTier clauses = (IntervalTier) thy tiers->at [2];
		IntervalTier words = (IntervalTier) thy tiers->at [3];
		IntervalTier phonemes = (IntervalTier) thy tiers->at [4];

		for (long i = 1; i <= numberOfRows; i++) {
			double time = Table_getNumericValue_Assert (me, i, timeColumnIndex);
			int type = Table_getNumericValue_Assert (me, i, typeColumnIndex);
			long pos = Table_getNumericValue_Assert (me, i, tposColumnIndex);
			if (type == espeakEVENT_SENTENCE) {
				// Only insert a new boundary, no text
				// text will be inserted at end sentence event
				if (time > xmin && time < xmax) {
					IntervalTier_addBoundaryUnsorted (clauses, clauses -> intervals.size, time, U"", true);
				}
				p1c = pos;
			} else if (type == espeakEVENT_END) {
				// End of clause: insert new boundary, and fill left interval with text
				length = pos - p1c + 1;
				MelderString_ncopy (&mark, text + p1c - 1, length);
				MelderString_trimWhiteSpaceAtEnd (& mark);
				if (time > xmin && time < xmax) {
					IntervalTier_addBoundaryUnsorted (clauses, clauses -> intervals.size, time, mark.string, true);
				} else {
					TextGrid_setIntervalText (thee.get(), 2, clauses -> intervals.size, mark.string);
				}
				p1c = pos;

				// End of clause always signals "end of a word"

				if (pos <= textLength) {
					length = pos - p1w + 1;
					MelderString_ncopy (&mark, text + p1w - 1, length);
					MelderString_trimWhiteSpaceAtEnd (& mark);
					if (time > xmin && time < xmax) {
						IntervalTier_addBoundaryUnsorted (words, words -> intervals.size, time, mark.string, true);
					} else {
						TextGrid_setIntervalText (thee.get(), 3, words -> intervals.size, mark.string);
					}
					// now the next word event should not trigger setting the left interval text
					wordEnd = false;
				}
			} else if (type == espeakEVENT_WORD) {
				if (pos < p1w) {
					continue;
				}
				if (time > xmin && time < xmax) {
					length = pos - p1w;
					if (pos == textLength) {
						length++;
					}
					MelderString_ncopy (&mark, text + p1w - 1, length);
					MelderString_trimWhiteSpaceAtEnd (& mark);
					IntervalTier_addBoundaryUnsorted (words, words -> intervals.size, time, ( wordEnd ? mark.string : U"" ), true);
				}
				wordEnd = true;
				p1w = pos;
			} else if (type == espeakEVENT_PHONEME) {
				const char32 *id = Table_getStringValue_Assert (me, i, idColumnIndex);
				if (time > time_phon_p) {
					// Insert new boudary and label interval with the id
					// TODO: Translate the id to the correct notation
					TextInterval ti = phonemes -> intervals.at [phonemes -> intervals.size];
					if (time > ti -> xmin && time < ti -> xmax) {
						IntervalTier_addBoundaryUnsorted (phonemes, phonemes -> intervals.size, time, id, false);
					}
				} else {
					// Just in case the phoneme starts at xmin we only need to set interval text
					TextGrid_setIntervalText (thee.get(), 4, phonemes -> intervals.size, id);
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
		
		/* Use empty intervals in phoneme tier for more precision in the word tier */
		
		IntervalTier_insertEmptyIntervalsFromOtherTier (words, phonemes);
		IntervalTier_mergeSpecialIntervals (words); // Merge neighbouring empty U"" and U"\001" intervals

		return thee;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not created from Table with events.");
	}
}

static void espeakdata_SetVoiceByName (const char *name, const char *variantName)
{
	espeak_VOICE voice_selector;

	memset (& voice_selector, 0, sizeof voice_selector);
	voice_selector.name = Melder_peek32to8 (Melder_cat (Melder_peek8to32 (name), U"+", Melder_peek8to32 (variantName)));  // include variant name in voice stack ??

	if (LoadVoice (name, 1)) {
		LoadVoice (variantName, 2);
		DoVoiceChange (voice);
		SetVoiceStack (& voice_selector, variantName);
	}
}

void SpeechSynthesizer_changeLanguageNameToCurrent (SpeechSynthesizer me) {
	try {
		struct espeakLanguagestruct { const char32 *oldName, *currentName; } names[] = {
			{ U"Akan-test", nullptr}, 
			{ U"Bulgarian-test", U"Bulgarian"}, 
			{ U"Dari-test", nullptr}, 
			{ U"Divehi-test", nullptr}, { U"Dutch-test", U"Dutch"}, 
			{ U"French (Belgium)", U"French-Belgium"},
			{ U"Georgian-test", U"Georgian"}, 
			{ U"Haitian", nullptr},
			{ U"Icelandic-test", U"Icelandic"}, { U"Indonesian-test", U"Indonesian"},
			{ U"Irish-test", U"Irish-gaeilge"}, 
			{ U"Kazakh", nullptr}, { U"Kinyarwanda-test", nullptr}, { U"Korean", U"Korean-test"}, 
			{ U"Lancashire", nullptr},
			{ U"Macedonian-test", U"Macedonian"}, { U"Maltese-test", nullptr},
			{ U"Nahuatl - classical", U"Nahuatl-classical"}, { U"Nepali-test", U"Nepali"}, { U"Northern-sotho", nullptr},
			{ U"Punjabi-test", U"Punjabi"},
			{ U"Russian_test", U"Russian"}, // yes, underscore
			{ U"Setswana-test", nullptr}, { U"Sinhala", U"Sinhala-test"},
			{ U"Spanish-latin-american", U"Spanish-latin-am"}, { U"Tatar-test", nullptr},
			{ U"Telugu", U"Telugu-test"}, 
			{ U"Welsh-test", U"Welsh"}, { U"Wolof-test", nullptr},
			{nullptr,nullptr}};
		long index = 0;
		while (const char32 *oldName = names [index]. oldName) {
			if (Melder_equ (oldName, my d_voiceLanguageName)) {
				if (names [index]. currentName) {
					autostring32 newLabel = Melder_dup (names [index]. currentName);
					Melder_free (my d_voiceLanguageName);
					my d_voiceLanguageName = newLabel.transfer();
					break;
				} else {
					Melder_throw (U"Language ", oldName, U" is not available any longer.");
				}
			}
			++ index;
		}
	} catch (MelderError) {
		Melder_throw (U"Cannot change language name.");
	}
}

autoSound SpeechSynthesizer_to_Sound (SpeechSynthesizer me, const char32 *text, autoTextGrid *tg, autoTable *events) {
	try {
		int fsamp = espeak_Initialize (AUDIO_OUTPUT_SYNCHRONOUS, 0, nullptr, // 5000ms
			espeakINITIALIZE_PHONEME_EVENTS|espeakINITIALIZE_PHONEME_IPA);
		if (fsamp == -1) {
			Melder_throw (U"Internal espeak error.");
		}
		int synth_flags = espeakCHARS_WCHAR;
		if (my d_inputTextFormat == SpeechSynthesizer_INPUT_TAGGEDTEXT) {
			synth_flags |= espeakSSML;
		}
		if (my d_inputTextFormat != SpeechSynthesizer_INPUT_TEXTONLY) {
			synth_flags |= espeakPHONEMES;
		}
		option_phoneme_events = espeakINITIALIZE_PHONEME_EVENTS; // extern int option_phoneme_events;
		if (my d_outputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_IPA) {
			option_phoneme_events |= espeakINITIALIZE_PHONEME_IPA;
		}

		espeak_SetParameter (espeakRATE, my d_wordsPerMinute, 0);
		espeak_SetParameter (espeakPITCH, my d_pitchAdjustment, 0);
		espeak_SetParameter (espeakRANGE, my d_pitchRange, 0);
		const char32 *voiceLanguageCode = SpeechSynthesizer_getVoiceLanguageCodeFromName (me, my d_voiceLanguageName);
		const char32 *voiceVariantCode = SpeechSynthesizer_getVoiceVariantCodeFromName (me, my d_voiceVariantName);
		espeakdata_SetVoiceByName ((const char *) Melder_peek32to8 (voiceLanguageCode), 
			(const char *) Melder_peek32to8 (voiceVariantCode));

		espeak_SetParameter (espeakWORDGAP, my d_wordgap * 100, 0); // espeak wordgap is in units of 10 ms
		espeak_SetParameter (espeakCAPITALS, 0, 0);
		espeak_SetParameter (espeakPUNCTUATION, espeakPUNCT_NONE, 0);

		espeak_SetSynthCallback (synthCallback);

		my d_events = Table_createWithColumnNames (0, U"time type type-t t-pos length a-pos sample id uniq");

		#ifdef _WIN32
                wchar_t *textW = Melder_peek32toW (text);
                espeak_Synth (textW, wcslen (textW) + 1, 0, POS_CHARACTER, 0, synth_flags, nullptr, me);
		#else
                espeak_Synth (text, str32len (text) + 1, 0, POS_CHARACTER, 0, synth_flags, nullptr, me);
		#endif
				
		espeak_Terminate ();
		autoSound thee = buffer_to_Sound (my d_wav, my d_numberOfSamples, my d_internalSamplingFrequency);

		if (my d_samplingFrequency != my d_internalSamplingFrequency) {
			thee = Sound_resample (thee.get(), my d_samplingFrequency, 50);
		}
		my d_numberOfSamples = 0; // re-use the wav-buffer
		if (tg) {
			double xmin = Table_getNumericValue_Assert (my d_events.get(), 1, 1);
			if (xmin > thy xmin) {
				xmin = thy xmin;
			}
			double xmax = Table_getNumericValue_Assert (my d_events.get(), my d_events -> rows.size, 1);
			if (xmax < thy xmax) {
				xmax = thy xmax;
			}
			autoTextGrid tg1 = Table_to_TextGrid (my d_events.get(), text, xmin, xmax);
			*tg = TextGrid_extractPart (tg1.get(), thy xmin, thy xmax, 0);
		}
		if (events) {
			Table_setEventTypeString (my d_events.get());
			*events = my d_events.move();
		}
		my d_events.reset();
		return thee;
	} catch (MelderError) {
		espeak_Terminate ();
		Melder_throw (U"Text not played.");
	}
}

/* End of file SpeechSynthesizer.cpp */
