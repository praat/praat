/* SpeechSynthesizer.cpp
 *
//  * Copyright (C) 2011-2017 David Weenink
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

#include "espeakdata_FileInMemory.h"
	djmw 20111214
*/
#include "espeak_ng_version.h"
#include "espeak_ng.h"
#include "espeakdata_FileInMemory.h"


#include "SpeechSynthesizer.h"
#include "Strings_extensions.h"
#include "speak_lib.h"
#include "encoding.h"
#include "string.h"
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

Thing_implement (EspeakVoice, Daata, 0);

autoEspeakVoice EspeakVoice_create () {
	try {
		autoEspeakVoice me = Thing_new (EspeakVoice);
		my numberOfFormants = 9; // equals N_PEAKS 
		my numberOfKlattParameters = 8;
		my klattv = NUMvector<int32> (1, my numberOfKlattParameters);
		my freq = NUMvector<int32> (1, my numberOfFormants);
		my height = NUMvector<int32> (1, my numberOfFormants);	// 100% = 256
		my width = NUMvector<int32> (0, my numberOfFormants);		// 100% = 256
		my freqadd = NUMvector<int32> (0, my numberOfFormants);	// Hz

		// copies without temporary adjustments from embedded commands
		my freq2 = NUMvector<int32> (0, my numberOfFormants);		// 100% = 256
		my height2 = NUMvector<int32> (0, my numberOfFormants);	// 100% = 256
		my width2 = NUMvector<int32> (0, my numberOfFormants);	// 100% = 256

		my breath = NUMvector<int32> (0, my numberOfFormants);	// amount of breath for each formant. breath[0] indicates whether any are set.
		my breathw = NUMvector<int32> (0, my numberOfFormants);	// width of each breath formant
		my numberOfToneAdjusts = 1000; // equals N_TONE_ADJUST in voice.h
		my tone_adjust = NUMvector<unsigned char> (1, my numberOfToneAdjusts);
		EspeakVoice_setDefaults (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"EspeakVoice not created.");
	}
}

void EspeakVoice_setDefaults (EspeakVoice me) {
	(void) me;
}

void EspeakVoice_initFromEspeakVoice (EspeakVoice me, voice_t *voice) {
	my v_name = Melder_dup (Melder_peek8to32 (voice -> v_name));

	my phoneme_tab_ix = voice -> phoneme_tab_ix;
	my pitch_base = voice -> pitch_base;
	my pitch_range = voice -> pitch_range;

	my speedf1 = voice -> speedf1;
	my speedf2 = voice -> speedf2;
	my speedf3 = voice -> speedf3;

	my speed_percent = voice -> speed_percent;
	my flutter = voice -> flutter;
	my roughness = voice -> roughness;
	my echo_delay = voice -> echo_delay;
	my echo_amp = voice -> echo_amp;
	my n_harmonic_peaks = voice -> n_harmonic_peaks;
	my peak_shape = voice -> peak_shape;
	my voicing = voice -> voicing;
	my formant_factor = voice -> formant_factor;
	my consonant_amp = voice -> consonant_amp;
	my consonant_ampv = voice -> consonant_ampv;
	my samplerate = voice -> samplerate;
	my numberOfKlattParameters = 8;
	for (integer i = 1; i <= my numberOfKlattParameters; i ++) {
		my klattv [i] = voice -> klattv [i - 1];
	}
	for (integer i = 1; i <= my numberOfFormants; i ++) {
		my freq [i] = voice -> freq [i - 1];
		my height [i] = voice -> height [i - 1];
		my width [i] = voice -> width [i - 1];
		my freqadd [i] = voice -> freqadd [i - 1];
		my freq2 [i] = voice -> freq2 [i - 1];
		my height2 [i] = voice -> height2 [i - 1];
		my width2 [i] = voice -> width2 [i - 1];
		my breath [i] = voice -> breath [i - 1];
		my breathw [i] = voice -> breathw [i - 1];
	}
	my numberOfToneAdjusts = 1000;
	for (integer i = 1; i <= my numberOfToneAdjusts; i ++) {
		my tone_adjust [i] = voice -> tone_adjust [i - 1];
	}
}

void EspeakVoice_into_voice (EspeakVoice me, voice_t *voice) {

	if (my v_name) {
		strncpy (voice -> v_name, Melder_peek32to8 (my v_name), 40);
	}
	if (my language_name) {
		strncpy (voice -> language_name, Melder_peek32to8 (my language_name), 20);
	}
	voice -> phoneme_tab_ix = my phoneme_tab_ix;
	voice -> pitch_base = my pitch_base;
	voice -> pitch_range = my pitch_range;

	voice -> speedf1 = my speedf1;
	voice -> speedf2 = my speedf2;
	voice -> speedf3 = my speedf3;

	voice -> speed_percent = my speed_percent;
	voice -> flutter = my flutter;
	voice -> roughness = my roughness;
	voice -> echo_delay = my echo_delay;
	voice -> echo_amp = my echo_amp;
	voice -> n_harmonic_peaks = my n_harmonic_peaks;
	voice -> peak_shape = my peak_shape;
	voice -> voicing = my voicing;
	voice -> formant_factor = my formant_factor;
	voice -> consonant_amp = my consonant_amp;
	voice -> consonant_ampv = my consonant_ampv;
	voice -> samplerate = my samplerate;
	for (integer i = 1; i <= my numberOfKlattParameters; i ++) {
		voice -> klattv [i - 1] = my klattv [i];
	}
	for (integer i = 1; i <= my numberOfFormants; i ++) {
		voice -> freq [i - 1] = my freq [i];
		voice -> height [i - 1] = my height [i];
		voice -> width [i - 1] = my width [i];
		voice -> freqadd [i - 1] = my freqadd [i];
		voice -> freq2 [i - 1] = my freq2 [i];
		voice -> height2 [i - 1] = my height2 [i];
		voice -> width2 [i - 1] = my width2 [i];
		voice -> breath [i - 1] = my breath [i];
		voice -> breathw [i - 1] = my breathw [i];
	}
	for (integer i = 1; i <= my numberOfToneAdjusts; i ++) {
		voice -> tone_adjust [i - 1] = voice -> tone_adjust [i];
	}
}

Thing_implement (SpeechSynthesizer, Daata, 1);

void structSpeechSynthesizer :: v_info () {
	SpeechSynthesizer_Parent :: v_info ();
	MelderInfo_writeLine (U"Synthesizer version: espeak-ng ", d_synthesizerVersion);
	MelderInfo_writeLine (U"Language: ", d_languageName);
	MelderInfo_writeLine (U"Voice: ", d_voiceName);
	MelderInfo_writeLine (U"Phoneme set: ", d_phonemeSet);
	MelderInfo_writeLine (U"Input text format: ", (d_inputTextFormat == SpeechSynthesizer_INPUT_TEXTONLY ? U"text only" :
		d_inputTextFormat == SpeechSynthesizer_INPUT_PHONEMESONLY ? U"phonemes only" : U"tagged text"));
	MelderInfo_writeLine (U"Input phoneme coding: ", (d_inputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM ? U"Kirshenbaum" : U"???"));
	MelderInfo_writeLine (U"Sampling frequency: ", d_samplingFrequency, U" Hz");
	MelderInfo_writeLine (U"Word gap: ", d_wordgap, U" s");
	MelderInfo_writeLine (U"Pitch multiplier: ", d_pitchAdjustment, U" (0.5-2.0)");
	MelderInfo_writeLine (U"Pitch range multiplier: ", d_pitchRange, U" (0.0-2.0)");
	MelderInfo_writeLine (U"Speaking rate: ", d_wordsPerMinute, U" words per minute", (d_estimateSpeechRate ? U" (but estimated from speech if possible)" : U" (fixed)"));

	MelderInfo_writeLine (U"Output phoneme coding: ", (d_inputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM ? U"Kirshenbaum" : d_inputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_IPA ? U"IPA" : U"???"));
}

static void NUMvector_extendNumberOfElements (integer elementSize, void **v, integer lo, integer *hi, integer extraDemand)
{
	try {
		byte *result;
		if (! *v) {
			integer newhi = lo + extraDemand - 1;
			result = NUMvector_generic (elementSize, lo, newhi, true);
			*hi = newhi;
		} else {
			integer offset = lo * elementSize;
			for (;;) {   // not very infinite: 99.999 % of the time once, 0.001 % twice
				result = reinterpret_cast <byte *> (Melder_realloc ((char *) *v + offset, (*hi - lo + 1 + extraDemand) * elementSize));
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

static void NUMvector_supplyStorage (integer elementSize, void **v, integer lo, integer *hi, integer nfilled, integer extraDemand)
{
	integer old_capacity = *hi - lo + 1, new_capacity = nfilled + extraDemand;
	if (new_capacity < old_capacity) return;
	new_capacity = new_capacity > 2 * old_capacity ? new_capacity : 2 * old_capacity;
	NUMvector_extendNumberOfElements (elementSize, v, lo, hi, new_capacity);
}

template <class T>
void NUMvector_supplyStorage (T** v, integer lo, integer *hi, integer nfilled, integer extraDemand) {
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
			integer irow = my d_events -> rows.size;
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
		NUMvector_supplyStorage<int> (& my d_wav, 1, & my d_wavCapacity, my d_numberOfSamples, numsamples);
		for (integer i = 1; i <= numsamples; i++) {
			my d_wav [my d_numberOfSamples + i] = wav [i - 1];
		}
		my d_numberOfSamples += numsamples;
	}
	return 0;
}

const char32 *SpeechSynthesizer_getLanguageCode (SpeechSynthesizer me) {
	try {
		integer irow = Table_searchColumn (espeakdata_languages_propertiesTable.get(), 2, my d_languageName);
		if (irow == 0) {
			Melder_throw (U"Cannot find language \"", my d_languageName, U"\".");
		}
		return Table_getStringValue_Assert (espeakdata_languages_propertiesTable.get(), irow, 1);
	} catch (MelderError) {
		Melder_throw (me, U": Cannot find language code.");
	}
}

const char32 *SpeechSynthesizer_getPhonemeCode (SpeechSynthesizer me) {
	try {
		integer irow = Table_searchColumn (espeakdata_languages_propertiesTable.get(), 2, my d_phonemeSet);
		if (irow == 0) {
			Melder_throw (U"Cannot find phoneme set \"", my d_phonemeSet, U"\".");
		}
		return Table_getStringValue_Assert (espeakdata_languages_propertiesTable.get(), irow, 1);
	} catch (MelderError) {
		Melder_throw (me, U": Cannot find phoneme code.");
	}
}

const char32 *SpeechSynthesizer_getVoiceCode (SpeechSynthesizer me) {
	try {
		integer irow = Table_searchColumn (espeakdata_voices_propertiesTable.get(), 2, my d_voiceName);
		if (irow == 0) {
			Melder_throw (U": Cannot find voice variant \"", my d_voiceName, U"\".");
		}
		return Table_getStringValue_Assert (espeakdata_voices_propertiesTable.get(), irow, 1);
	} catch (MelderError) {
		Melder_throw (me, U": Cannot find voice code.");
	}
}

autoSpeechSynthesizer SpeechSynthesizer_create (const char32 *languageName, const char32 *voiceName) {
	try {
		autoSpeechSynthesizer me = Thing_new (SpeechSynthesizer);
		my d_synthesizerVersion = Melder_dup(ESPEAK_NG_VERSION);
		my d_languageName = Melder_dup (languageName);
		(void) SpeechSynthesizer_getLanguageCode (me.get());  // existence check
		my d_voiceName = Melder_dup (voiceName);
		(void) SpeechSynthesizer_getVoiceCode (me.get());  // existence check
		my d_phonemeSet = Melder_dup (languageName);
		SpeechSynthesizer_setTextInputSettings (me.get(), SpeechSynthesizer_INPUT_TEXTONLY, SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM);
		SpeechSynthesizer_setSpeechOutputSettings (me.get(), 44100.0, 0.01, 1.0, 1.0, 175, SpeechSynthesizer_PHONEMECODINGS_IPA);
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

void SpeechSynthesizer_setSpeechOutputSettings (SpeechSynthesizer me, double samplingFrequency, double wordgap, double pitchAdjustment, double pitchRange, double wordsPerMinute, int outputPhonemeCoding) {
	my d_samplingFrequency = samplingFrequency;
	my d_wordgap = wordgap;
	pitchAdjustment = pitchAdjustment < 0.5 ? 0.5 : (pitchAdjustment > 2.0 ? 2.0 : pitchAdjustment);
	my d_pitchAdjustment = pitchAdjustment;
	pitchRange = pitchRange < 0.0 ? 0.0 : (pitchRange > 2.0 ? 2.0 : pitchRange);
	my d_pitchRange = pitchRange;

	if (wordsPerMinute <= 0.0) wordsPerMinute = 175.0;
	if (wordsPerMinute > 450.0) wordsPerMinute = 450.0;
	if (wordsPerMinute < 80.0) wordsPerMinute = 80.0;
	my d_wordsPerMinute = wordsPerMinute;
	my d_outputPhonemeCoding = outputPhonemeCoding;
}

void SpeechSynthesizer_playText (SpeechSynthesizer me, const char32 *text) {
	autoSound thee = SpeechSynthesizer_to_Sound (me, text, nullptr, nullptr);
	Sound_play (thee.get(), nullptr, nullptr);
}

static autoSound buffer_to_Sound (int *wav, integer numberOfSamples, double samplingFrequency)
{
	try {
		double dx = 1.0 / samplingFrequency;
		double xmax = numberOfSamples * dx;
		autoSound thee = Sound_create (1, 0.0, xmax, numberOfSamples, dx, dx / 2.0);
		for (integer i = 1; i <= numberOfSamples; i++) {
			thy z[1][i] = wav[i] / 32768.0;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from synthesizer data.");
	}
}

static void IntervalTier_addBoundaryUnsorted (IntervalTier me, integer iinterval, double time, const char32 *newLabel, bool isNewleftLabel) {
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
		for (integer i = 1; i <= my rows.size; i ++) {
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
	integer intervalIndex = my intervals.size;
	TextInterval right = my intervals.at [intervalIndex];
	integer labelLength_right = TextInterval_labelLength (right);
	bool isEmptyInterval_right = labelLength_right == 0 || (labelLength_right == 1 && Melder_equ (right -> text, U"\001"));
	while (intervalIndex > 1) {
		TextInterval left = my intervals.at [intervalIndex - 1];
		integer labelLength_left = TextInterval_labelLength (left);
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
	
	integer intervalNumber = IntervalTier_timeToLowIndex (me, t);
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
	for (integer iint = 1; iint <= from -> intervals.size; iint ++) {
		TextInterval tifrom = from -> intervals.at [iint];
		if (TextInterval_labelLength (tifrom) == 0) {   // found empty interval
			double t_left = tifrom -> xmin, t_right = tifrom -> xmax;
			integer intervalIndex_to = IntervalTier_timeToLowIndex (to, t_left);
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
	integer iint = 1;
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
		integer length, textLength = str32len (text);
		integer numberOfRows = my rows.size;
		integer timeColumnIndex = Table_getColumnIndexFromColumnLabel (me, U"time");
		integer typeColumnIndex = Table_getColumnIndexFromColumnLabel (me, U"type");
		integer tposColumnIndex = Table_getColumnIndexFromColumnLabel (me, U"t-pos");
		integer   idColumnIndex = Table_getColumnIndexFromColumnLabel (me, U"id");
		autoTextGrid thee = TextGrid_create (xmin, xmax, U"sentence clause word phoneme", U"");

		TextGrid_setIntervalText (thee.get(), 1, 1, text);

		integer p1c = 1, p1w = 1;
		double time_phon_p = xmin;
		bool wordEnd = false;
		autoMelderString mark;

		IntervalTier clauses = (IntervalTier) thy tiers->at [2];
		IntervalTier words = (IntervalTier) thy tiers->at [3];
		IntervalTier phonemes = (IntervalTier) thy tiers->at [4];

		for (integer i = 1; i <= numberOfRows; i++) {
			double time = Table_getNumericValue_Assert (me, i, timeColumnIndex);
			int type = Table_getNumericValue_Assert (me, i, typeColumnIndex);
			integer pos = Table_getNumericValue_Assert (me, i, tposColumnIndex);
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

static void espeakdata_SetVoiceByName (const char32 *languageName, const char32 *voiceName)
{
	espeak_VOICE voice_selector;

	memset (& voice_selector, 0, sizeof voice_selector);
	voice_selector.name = Melder_peek32to8 (Melder_cat (languageName, U"+", voiceName));  // include variant name in voice stack ??

	if (LoadVoice (Melder_peek32to8 (languageName), 1)) {
		LoadVoice (Melder_peek32to8 (voiceName), 2);
		DoVoiceChange (voice);
		SetVoiceStack (& voice_selector, Melder_peek32to8 (voiceName));
	}
}

autoSound SpeechSynthesizer_to_Sound (SpeechSynthesizer me, const char32 *text, autoTextGrid *tg, autoTable *events) {
	try {
		espeak_ng_InitializePath (nullptr); // PATH_ESPEAK_DATA
		espeak_ng_ERROR_CONTEXT context = { 0 };
		espeak_ng_STATUS status = espeak_ng_Initialize (& context);
		if (status != ENS_OK) {
			Melder_throw (U"Internal espeak error.", status);
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

		espeak_ng_SetParameter (espeakRATE, my d_wordsPerMinute, 0);
		/*
			pitchAdjustment_0_99 = a * log10 (my d_pitchAdjustment) + b,
			where 0.5 <= my d_pitchAdjustment <= 2
			pitchRange_0_99 = my d_pitchRange * 49.5,
			where 0 <= my d_pitchRange <= 2
		*/
		int pitchAdjustment_0_99 = (int) ((49.5 / log10(2.0)) * log10 (my d_pitchAdjustment) + 49.5);
		espeak_ng_SetParameter (espeakPITCH, pitchAdjustment_0_99, 0);
		int pitchRange_0_99 = (int) (my d_pitchRange * 49.5);
		espeak_ng_SetParameter (espeakRANGE, pitchRange_0_99, 0);
		const char32 *languageCode = SpeechSynthesizer_getLanguageCode (me);
		const char32 *voiceCode = SpeechSynthesizer_getVoiceCode (me);
		
		espeak_ng_SetVoiceByName(Melder_peek32to8 (Melder_cat (languageCode, U"+", voiceCode)));
		int wordgap_10ms = my d_wordgap * 100; // espeak wordgap is in units of 10 ms
		espeak_ng_SetParameter (espeakWORDGAP, wordgap_10ms, 0);
		espeak_ng_SetParameter (espeakCAPITALS, 0, 0);
		espeak_ng_SetParameter (espeakPUNCTUATION, espeakPUNCT_NONE, 0);
		
		status =  espeak_ng_InitializeOutput (ENOUTPUT_MODE_SYNCHRONOUS, 2048, nullptr);
		espeak_SetSynthCallback (synthCallback);
		if (! Melder_equ (my d_phonemeSet, my d_languageName)) {
			const char32 *phonemeCode = SpeechSynthesizer_getPhonemeCode (me);
			int index_phon_table_list = LookupPhonemeTable (Melder_32to8 (phonemeCode));
			if (index_phon_table_list > 0) {
				voice -> phoneme_tab_ix = index_phon_table_list;
				DoVoiceChange(voice);
			}
		}

		my d_events = Table_createWithColumnNames (0, U"time type type-t t-pos length a-pos sample id uniq");

		#ifdef _WIN32
                wchar_t *textW = Melder_peek32toW (text);
                espeak_ng_Synthesize (textW, wcslen (textW) + 1, 0, POS_CHARACTER, 0, synth_flags, nullptr, me);
		#else
                espeak_ng_Synthesize (text, str32len (text) + 1, 0, POS_CHARACTER, 0, synth_flags, nullptr, me);
		#endif
				
		espeak_ng_Terminate ();
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
