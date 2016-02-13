/* praat_Sound_init.cpp
 *
 * Copyright (C) 1992-2012,2014,2015,2016 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "praat.h"

#include "Ltas.h"
#include "LongSound.h"
#include "Manipulation.h"
#include "ParamCurve.h"
#include "Sound_and_Spectrogram.h"
#include "Sound_and_Spectrum.h"
#include "Sound_extensions.h"
#include "Sound_to_Cochleagram.h"
#include "Sound_to_Formant.h"
#include "Sound_to_Harmonicity.h"
#include "Sound_to_Intensity.h"
#include "Sound_to_Pitch.h"
#include "Sound_to_PointProcess.h"
#include "SoundEditor.h"
#include "SoundRecorder.h"
#include "SpectrumEditor.h"
#include "TextGrid_Sound.h"
#include "mp3.h"

#undef iam
#define iam iam_LOOP

void praat_TimeFunction_query_init (ClassInfo klas);
void praat_TimeFunction_modify_init (ClassInfo klas);

/***** LONGSOUND *****/

DIRECT2 (LongSound_concatenate) {
	Melder_information (U"To concatenate LongSound objects, select them in the list\nand choose \"Save as WAV file...\" or a similar command.\n"
		"The result will be a sound file that contains\nthe concatenation of the selected sounds.");
END2 }

FORM (LongSound_extractPart, U"LongSound: Extract part", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"1.0")
	BOOLEAN (U"Preserve times", true)
	OK2
DO
	LOOP {
		iam (LongSound);
		autoSound thee = LongSound_extractPart (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Preserve times"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (LongSound_getIndexFromTime, U"LongSound: Get sample index from time", U"Sound: Get index from time...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (LongSound);
		double index = Sampled_xToIndex (me, GET_REAL (U"Time"));
		Melder_informationReal (index, nullptr);
	}
END2 }

DIRECT2 (LongSound_getSamplePeriod) {
	LOOP {
		iam (LongSound);
		Melder_informationReal (my dx, U"seconds");
	}
END2 }

DIRECT2 (LongSound_getSampleRate) {
	LOOP {
		iam (LongSound);
		Melder_informationReal (1.0 / my dx, U"Hz");
	}
END2 }

FORM (LongSound_getTimeFromIndex, U"LongSound: Get time from sample index", U"Sound: Get time from index...") {
	INTEGER (U"Sample index", U"100")
	OK2
DO
	LOOP {
		iam (LongSound);
		Melder_informationReal (Sampled_indexToX (me, GET_INTEGER (U"Sample index")), U"seconds");
	}
END2 }

DIRECT2 (LongSound_getNumberOfSamples) {
	LOOP {
		iam (LongSound);
		Melder_information (my nx, U" samples");
	}
END2 }

DIRECT2 (LongSound_help) { Melder_help (U"LongSound"); END2 }

FORM_READ2 (LongSound_open, U"Open long sound file", nullptr, true) {
	autoLongSound me = LongSound_open (file);
	praat_new (me.move(), MelderFile_name (file));
END2 }

FORM (LongSound_playPart, U"LongSound: Play part", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"10.0")
	OK2
DO
	int n = 0;
	LOOP n ++;
	if (n == 1 || MelderAudio_getOutputMaximumAsynchronicity () < kMelder_asynchronicityLevel_ASYNCHRONOUS) {
		LOOP {
			iam (LongSound);
			LongSound_playPart (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), nullptr, nullptr);
		}
	} else {
		MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel_INTERRUPTABLE);
		LOOP {
			iam (LongSound);
			LongSound_playPart (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), nullptr, nullptr);
		}
		MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel_ASYNCHRONOUS);
	}
END2 }

FORM (LongSound_writePartToAudioFile, U"LongSound: Save part as audio file", nullptr) {
	LABEL (U"", U"Audio file:")
	TEXTFIELD (U"Audio file", U"")
	RADIO (U"Type", 3)
	{ int i; for (i = 1; i <= Melder_NUMBER_OF_AUDIO_FILE_TYPES; i ++) {
		RADIOBUTTON (Melder_audioFileTypeString (i))
	}}
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"10.0")
	OK2
DO
	LOOP {
		iam (LongSound);
		structMelderFile file = { 0 };
		Melder_relativePathToFile (GET_STRING (U"Audio file"), & file);
		LongSound_writePartToAudioFile (me, GET_INTEGER (U"Type"),
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), & file, 16);
	}
END2 }
	
FORM (LongSound_to_TextGrid, U"LongSound: To TextGrid...", U"LongSound: To TextGrid...") {
	SENTENCE (U"Tier names", U"Mary John bell")
	SENTENCE (U"Point tiers", U"bell")
	OK2
DO
	LOOP {
		iam (LongSound);
		autoTextGrid thee = TextGrid_create (my xmin, my xmax, GET_STRING (U"Tier names"), GET_STRING (U"Point tiers"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (LongSound_view) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a LongSound from batch.");
	LOOP {
		iam (LongSound);
		autoSoundEditor editor = SoundEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

FORM_WRITE2 (LongSound_writeToAifcFile, U"Save as AIFC file", nullptr, U"aifc") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_AIFC, 16);
END2 }

FORM_WRITE2 (LongSound_writeToAiffFile, U"Save as AIFF file", nullptr, U"aiff") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_AIFF, 16);
END2 }

FORM_WRITE2 (LongSound_writeToNextSunFile, U"Save as NeXT/Sun file", nullptr, U"au") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_NEXT_SUN, 16);
END2 }

FORM_WRITE2 (LongSound_writeToNistFile, U"Save as NIST file", nullptr, U"nist") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_NIST, 16);
END2 }

FORM_WRITE2 (LongSound_writeToFlacFile, U"Save as FLAC file", nullptr, U"flac") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_FLAC, 16);
END2 }

FORM_WRITE2 (LongSound_writeToWavFile, U"Save as WAV file", nullptr, U"wav") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_WAV, 16);
END2 }

FORM_WRITE2 (LongSound_writeLeftChannelToAifcFile, U"Save left channel as AIFC file", nullptr, U"aifc") {
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_AIFC, 0, file);
	}
END2 }

FORM_WRITE2 (LongSound_writeLeftChannelToAiffFile, U"Save left channel as AIFF file", nullptr, U"aiff") {
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_AIFF, 0, file);
	}
END2 }

FORM_WRITE2 (LongSound_writeLeftChannelToNextSunFile, U"Save left channel as NeXT/Sun file", nullptr, U"au") {
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_NEXT_SUN, 0, file);
	}
END2 }

FORM_WRITE2 (LongSound_writeLeftChannelToNistFile, U"Save left channel as NIST file", nullptr, U"nist") {
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_NIST, 0, file);
	}
END2 }

FORM_WRITE2 (LongSound_writeLeftChannelToFlacFile, U"Save left channel as FLAC file", nullptr, U"flac") {
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_FLAC, 0, file);
	}
END2 }

FORM_WRITE2 (LongSound_writeLeftChannelToWavFile, U"Save left channel as WAV file", nullptr, U"wav") {
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_WAV, 0, file);
	}
END2 }

FORM_WRITE2 (LongSound_writeRightChannelToAifcFile, U"Save right channel as AIFC file", nullptr, U"aifc") {
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_AIFC, 1, file);
	}
END2 }

FORM_WRITE2 (LongSound_writeRightChannelToAiffFile, U"Save right channel as AIFF file", nullptr, U"aiff") {
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_AIFF, 1, file);
	}
END2 }

FORM_WRITE2 (LongSound_writeRightChannelToNextSunFile, U"Save right channel as NeXT/Sun file", nullptr, U"au") {
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_NEXT_SUN, 1, file);
	}
END2 }

FORM_WRITE2 (LongSound_writeRightChannelToNistFile, U"Save right channel as NIST file", nullptr, U"nist") {
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_NIST, 1, file);
	}
END2 }

FORM_WRITE2 (LongSound_writeRightChannelToFlacFile, U"Save right channel as FLAC file", 0, U"flac") {
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_FLAC, 1, file);
	}
END2 }

FORM_WRITE2 (LongSound_writeRightChannelToWavFile, U"Save right channel as WAV file", 0, U"wav") {
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_WAV, 1, file);
	}
END2 }

FORM (LongSoundPrefs, U"LongSound preferences", U"LongSound") {
	LABEL (U"", U"This setting determines the maximum number of seconds")
	LABEL (U"", U"for viewing the waveform and playing a sound in the LongSound window.")
	LABEL (U"", U"The LongSound window can become very slow if you set it too high.")
	NATURAL (U"Maximum viewable part (seconds)", U"60")
	LABEL (U"", U"Note: this setting works for the next long sound file that you open,")
	LABEL (U"", U"not for currently existing LongSound objects.")
	OK2
SET_INTEGER (U"Maximum viewable part", LongSound_getBufferSizePref_seconds ())
DO
	LongSound_setBufferSizePref_seconds (GET_INTEGER (U"Maximum viewable part"));
END2 }

/********** LONGSOUND & SOUND **********/

FORM_WRITE2 (LongSound_Sound_writeToAifcFile, U"Save as AIFC file", nullptr, U"aifc") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_AIFC, 16);
END2 }

FORM_WRITE2 (LongSound_Sound_writeToAiffFile, U"Save as AIFF file", nullptr, U"aiff") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_AIFF, 16);
END2 }

FORM_WRITE2 (LongSound_Sound_writeToNextSunFile, U"Save as NeXT/Sun file", nullptr, U"au") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_NEXT_SUN, 16);
END2 }

FORM_WRITE2 (LongSound_Sound_writeToNistFile, U"Save as NIST file", nullptr, U"nist") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_NIST, 16);
END2 }

FORM_WRITE2 (LongSound_Sound_writeToFlacFile, U"Save as FLAC file", nullptr, U"flac") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_FLAC, 16);
END2 }

FORM_WRITE2 (LongSound_Sound_writeToWavFile, U"Save as WAV file", nullptr, U"wav") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_WAV, 16);
END2 }

/********** SOUND **********/

FORM (Sound_add, U"Sound: Add", nullptr) {
	LABEL (U"", U"The following number will be added to the amplitudes of ")
	LABEL (U"", U"all samples of the sound.")
	REAL (U"Number", U"0.1")
	OK2
DO
	LOOP {
		iam (Sound);
		Vector_addScalar (me, GET_REAL (U"Number"));
		praat_dataChanged (me);
	}
END2 }

FORM (Sound_autoCorrelate, U"Sound: autocorrelate", U"Sound: Autocorrelate...") {
	RADIO_ENUM (U"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
 	OK2
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_autoCorrelate (me,
			GET_ENUM (kSounds_convolve_scaling, U"Amplitude scaling"),
			GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, U"Signal outside time domain is..."));
		praat_new (thee.move(), U"ac_", my name);
	}
END2 }

DIRECT2 (Sounds_combineToStereo) {
	OrderedOf<structSound> list;
	LOOP {
		iam (Sound);
		list. addItem_ref (me);
	}
	autoSound result = Sounds_combineToStereo (& list);
	long numberOfChannels = result -> ny;   // dereference before transferring
	praat_new (result.move(), U"combined_", numberOfChannels);
END2 }

DIRECT2 (Sounds_concatenate) {
	OrderedOf<structSound> list;
	LOOP {
		iam_LOOP (Sound);
		list.addItem_ref (me);
	}
	autoSound result = Sounds_concatenate (list, 0.0);
	praat_new (result.move(), U"chain");
END2 }

FORM (Sounds_concatenateWithOverlap, U"Sounds: Concatenate with overlap", U"Sounds: Concatenate with overlap...") {
	POSITIVE (U"Overlap (s)", U"0.01")
	OK2
DO
	OrderedOf<structSound> list;
	LOOP {
		iam_LOOP (Sound);
		list.addItem_ref (me);
	}
	autoSound result = Sounds_concatenate (list, GET_REAL (U"Overlap"));
	praat_new (result.move(), U"chain");
END2 }

DIRECT2 (Sounds_concatenateRecoverably) {
	long numberOfChannels = 0, nx = 0, iinterval = 0;
	double dx = 0.0, tmin = 0.0;
	LOOP {
		iam (Sound);
		if (numberOfChannels == 0) {
			numberOfChannels = my ny;
		} else if (my ny != numberOfChannels) {
			Melder_throw (U"To concatenate sounds, their numbers of channels (mono, stereo) must be equal.");
		}
		if (dx == 0.0) {
			dx = my dx;
		} else if (my dx != dx) {
			Melder_throw (U"To concatenate sounds, their sampling frequencies must be equal.\n"
				"You could resample one or more of the sounds before concatenating.");
		}
		nx += my nx;
	}
	autoSound thee = Sound_create (numberOfChannels, 0.0, nx * dx, nx, dx, 0.5 * dx);
	autoTextGrid him = TextGrid_create (0.0, nx * dx, U"labels", U"");
	nx = 0;
	LOOP {
		iam (Sound);
		double tmax = tmin + my nx * dx;
		for (long channel = 1; channel <= numberOfChannels; channel ++) {
			NUMvector_copyElements (my z [channel], thy z [channel] + nx, 1, my nx);
		}
		iinterval ++;
		if (iinterval > 1) {
			TextGrid_insertBoundary (him.get(), 1, tmin);
		}
		TextGrid_setIntervalText (him.get(), 1, iinterval, my name);
		nx += my nx;
		tmin = tmax;
	}
	praat_new (thee.move(), U"chain");
	praat_new (him.move(), U"chain");
END2 }

DIRECT2 (Sound_convertToMono) {
	LOOP {
		iam (Sound);
		autoSound thee = Sound_convertToMono (me);
		praat_new (thee.move(), my name, U"_mono");
	}
END2 }

DIRECT2 (Sound_convertToStereo) {
	LOOP {
		iam (Sound);
		autoSound thee = Sound_convertToStereo (me);
		praat_new (thee.move(), my name, U"_stereo");
	}
END2 }

DIRECT2 (Sounds_convolve_old) {
	Sound s1 = nullptr, s2 = nullptr;
	LOOP {
		iam (Sound);
		( s1 ? s2 : s1 ) = me;
	}
	Melder_assert (s1 && s2);
	autoSound thee = Sounds_convolve (s1, s2, kSounds_convolve_scaling_SUM, kSounds_convolve_signalOutsideTimeDomain_ZERO);
	praat_new (thee.move(), s1 -> name, U"_", s2 -> name);
END2 }

FORM (Sounds_convolve, U"Sounds: Convolve", U"Sounds: Convolve...") {
	RADIO_ENUM (U"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK2
DO
	Sound s1 = nullptr, s2 = nullptr;
	LOOP {
		iam (Sound);
		( s1 ? s2 : s1 ) = me;
	}
	Melder_assert (s1 && s2);
	autoSound thee = Sounds_convolve (s1, s2,
		GET_ENUM (kSounds_convolve_scaling, U"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, U"Signal outside time domain is..."));
	praat_new (thee.move(), s1 -> name, U"_", s2 -> name);
END2 }

static void common_Sound_create (UiForm dia, Interpreter interpreter, bool allowMultipleChannels) {
	long numberOfChannels = allowMultipleChannels ? GET_INTEGER (U"Number of channels") : 1;
	double startTime = GET_REAL (U"Start time");
	double endTime = GET_REAL (U"End time");
	double samplingFrequency = GET_REAL (U"Sampling frequency");
	double numberOfSamples_real = round ((endTime - startTime) * samplingFrequency);
	if (endTime <= startTime) {
		if (endTime == startTime)
			Melder_appendError (U"A Sound cannot have a duration of zero.");
		else
			Melder_appendError (U"A Sound cannot have a duration less than zero.");
		if (startTime == 0.0)
			Melder_throw (U"Please set the end time to something greater than 0 seconds.");
		else
			Melder_throw (U"Please lower the start time or raise the end time.");
	}
	if (samplingFrequency <= 0.0) {
		Melder_appendError (U"A Sound cannot have a negative sampling frequency.");
		Melder_throw (U"Please set the sampling frequency to something greater than zero, e.g. 44100 Hz.");
	}
	if (numberOfChannels < 1)
		Melder_throw (U"A Sound cannot have zero channels.");
	if (numberOfSamples_real < 1.0) {
		Melder_appendError (U"A Sound cannot have zero samples.");
		if (startTime == 0.0)
			Melder_throw (U"Please raise the end time.");
		else
			Melder_throw (U"Please lower the start time or raise the end time.");
	}
	if (numberOfSamples_real > INT54_MAX) {
		Melder_appendError (U"A Sound cannot have ", numberOfSamples_real, U" samples; the maximum is ",
			Melder_bigInteger (INT54_MAX), U" samples (or less, depending on your computer's memory).");
		if (startTime == 0.0)
			Melder_throw (U"Please lower the end time or the sampling frequency.");
		else
			Melder_throw (U"Please raise the start time, lower the end time, or lower the sampling frequency.");
	}
	int64 numberOfSamples = (int64) numberOfSamples_real;
	autoSound sound;
	try {
		sound = Sound_create (numberOfChannels, startTime, endTime, numberOfSamples, 1.0 / samplingFrequency,
			startTime + 0.5 * (endTime - startTime - (numberOfSamples - 1) / samplingFrequency));
	} catch (MelderError) {
		if (str32str (Melder_getError (), U"memory")) {
			Melder_clearError ();
			Melder_appendError (U"There is not enough memory to create a Sound that contains ", Melder_bigInteger (numberOfSamples), U" samples.");
			if (startTime == 0.0)
				Melder_throw (U"You could lower the end time or the sampling frequency and try again.");
			else
				Melder_throw (U"You could raise the start time or lower the end time or the sampling frequency, and try again.");
		} else {
			throw;   // unexpected error; wait for generic message
		}
	}
	Matrix_formula (sound.get(), GET_STRING (U"formula"), interpreter, nullptr);
	praat_new (sound.move(), GET_STRING (U"Name"));
	//praat_updateSelection ();
}

FORM (Sound_create, U"Create mono Sound", U"Create Sound from formula...") {
	WORD (U"Name", U"sineWithNoise")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	REAL (U"Sampling frequency (Hz)", U"44100")
	LABEL (U"", U"Formula:")
	TEXTFIELD (U"formula", U"1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)")
	OK2
DO
	common_Sound_create (dia, interpreter, false);
END2 }

FORM (Sound_createFromFormula, U"Create Sound from formula", U"Create Sound from formula...") {
	WORD (U"Name", U"sineWithNoise")
	CHANNEL (U"Number of channels", U"1 (= mono)")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	REAL (U"Sampling frequency (Hz)", U"44100")
	LABEL (U"", U"Formula:")
	TEXTFIELD (U"formula", U"1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)")
	OK2
DO
	common_Sound_create (dia, interpreter, true);
END2 }

FORM (Sound_createAsPureTone, U"Create Sound as pure tone", U"Create Sound as pure tone...") {
	WORD (U"Name", U"tone")
	CHANNEL (U"Number of channels", U"1 (= mono)")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"0.4")
	POSITIVE (U"Sampling frequency (Hz)", U"44100.0")
	POSITIVE (U"Tone frequency (Hz)", U"440.0")
	POSITIVE (U"Amplitude (Pa)", U"0.2")
	POSITIVE (U"Fade-in duration (s)", U"0.01")
	POSITIVE (U"Fade-out duration (s)", U"0.01")
	OK2
DO
	autoSound me = Sound_createAsPureTone (GET_INTEGER (U"Number of channels"), GET_REAL (U"Start time"), GET_REAL (U"End time"),
		GET_REAL (U"Sampling frequency"), GET_REAL (U"Tone frequency"), GET_REAL (U"Amplitude"),
		GET_REAL (U"Fade-in duration"), GET_REAL (U"Fade-out duration"));
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

FORM (Sound_createFromToneComplex, U"Create Sound from tone complex", U"Create Sound from tone complex...") {
	WORD (U"Name", U"toneComplex")
	REAL (U"Start time (s)", U"0.0")
	REAL (U"End time (s)", U"1.0")
	POSITIVE (U"Sampling frequency (Hz)", U"44100.0")
	RADIO (U"Phase", 2)
		RADIOBUTTON (U"Sine")
		RADIOBUTTON (U"Cosine")
	POSITIVE (U"Frequency step (Hz)", U"100.0")
	REAL (U"First frequency (Hz)", U"0.0 (= frequency step)")
	REAL (U"Ceiling (Hz)", U"0.0 (= Nyquist)")
	INTEGER (U"Number of components", U"0 (= maximum)")
	OK2
DO
	autoSound me = Sound_createFromToneComplex (GET_REAL (U"Start time"), GET_REAL (U"End time"),
		GET_REAL (U"Sampling frequency"), GET_INTEGER (U"Phase") - 1, GET_REAL (U"Frequency step"),
		GET_REAL (U"First frequency"), GET_REAL (U"Ceiling"), GET_INTEGER (U"Number of components"));
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

FORM (old_Sounds_crossCorrelate, U"Cross-correlate (short)", nullptr) {
	REAL (U"From lag (s)", U"-0.1")
	REAL (U"To lag (s)", U"0.1")
	BOOLEAN (U"Normalize", 1)
	OK2
DO
	Sound s1 = nullptr, s2 = nullptr;
	LOOP {
		iam (Sound);
		( s1 ? s2 : s1 ) = me;
	}
	autoSound thee = Sounds_crossCorrelate_short (s1, s2, GET_REAL (U"From lag"), GET_REAL (U"To lag"), GET_INTEGER (U"Normalize"));
	praat_new (thee.move(), U"cc_", s1 -> name, U"_", s2 -> name);
END2 }

FORM (Sounds_crossCorrelate, U"Sounds: Cross-correlate", U"Sounds: Cross-correlate...") {
	RADIO_ENUM (U"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK2
DO_ALTERNATIVE (old_Sounds_crossCorrelate)
	Sound s1 = nullptr, s2 = nullptr;
	LOOP {
		iam (Sound);
		( s1 ? s2 : s1 ) = me;
	}
	Melder_assert (s1 && s2);
	autoSound thee = Sounds_crossCorrelate (s1, s2,
		GET_ENUM (kSounds_convolve_scaling, U"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, U"Signal outside time domain is..."));
	praat_new (thee.move(), s1 -> name, U"_", s2 -> name);
END2 }

FORM (Sound_deemphasizeInline, U"Sound: De-emphasize (in-line)", U"Sound: De-emphasize (in-line)...") {
	REAL (U"From frequency (Hz)", U"50.0")
	OK2
DO
	LOOP {
		iam (Sound);
		Sound_deEmphasis (me, GET_REAL (U"From frequency"));
		Vector_scale (me, 0.99);
		praat_dataChanged (me);
	}
END2 }

FORM (Sound_deepenBandModulation, U"Deepen band modulation", U"Sound: Deepen band modulation...") {
	POSITIVE (U"Enhancement (dB)", U"20")
	POSITIVE (U"From frequency (Hz)", U"300")
	POSITIVE (U"To frequency (Hz)", U"8000")
	POSITIVE (U"Slow modulation (Hz)", U"3")
	POSITIVE (U"Fast modulation (Hz)", U"30")
	POSITIVE (U"Band smoothing (Hz)", U"100")
	OK2
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_deepenBandModulation (me, GET_REAL (U"Enhancement"),
			GET_REAL (U"From frequency"), GET_REAL (U"To frequency"),
			GET_REAL (U"Slow modulation"), GET_REAL (U"Fast modulation"), GET_REAL (U"Band smoothing"));
		praat_new (thee.move(), my name, U"_", (long) (GET_REAL (U"Enhancement")));   // truncate number toward zero for visual effect
	}
END2 }

FORM (old_Sound_draw, U"Sound: Draw", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range", U"0.0 (= all)")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0 (= auto)")
	BOOLEAN (U"Garnish", 1)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (Sound);
		Sound_draw (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"), GET_INTEGER (U"Garnish"), U"curve");
	}
END2 }

FORM (Sound_draw, U"Sound: Draw", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range", U"0.0 (= all)")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0 (= auto)")
	BOOLEAN (U"Garnish", 1)
	LABEL (U"", U"")
	OPTIONMENU (U"Drawing method", 1)
		OPTION (U"Curve")
		OPTION (U"Bars")
		OPTION (U"Poles")
		OPTION (U"Speckles")
	OK2
DO_ALTERNATIVE (old_Sound_draw)
	autoPraatPicture picture;
	LOOP {
		iam (Sound);
		Sound_draw (me, GRAPHICS, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"left Vertical range"), GET_REAL (U"right Vertical range"), GET_INTEGER (U"Garnish"), GET_STRING (U"Drawing method"));
	}
END2 }

static void cb_SoundEditor_publication (Editor /* me */, autoDaata publication) {
	/*
	 * Keep the gate for error handling.
	 */
	try {
		bool isaSpectrum = Thing_isa (publication.get(), classSpectrum);
		praat_new (publication.move(), U"");
		praat_updateSelection ();
		if (isaSpectrum) {
			int IOBJECT;
			LOOP {
				iam (Spectrum);
				autoSpectrumEditor editor2 = SpectrumEditor_create (ID_AND_FULL_NAME, me);
				praat_installEditor (editor2.get(), IOBJECT);
				editor2.releaseToUser();
			}
		}
	} catch (MelderError) {
		Melder_flushError ();
	}
}
DIRECT2 (Sound_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a Sound from batch.");
	LOOP {
		iam (Sound);
		autoSoundEditor editor = SoundEditor_create (ID_AND_FULL_NAME, me);
		Editor_setPublicationCallback (editor.get(), cb_SoundEditor_publication);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

DIRECT2 (Sound_extractAllChannels) {
	LOOP {
		iam (Sound);
		for (long channel = 1; channel <= my ny; channel ++) {
			autoSound thee = Sound_extractChannel (me, channel);
			praat_new (thee.move(), my name, U"_ch", channel);
		}
	}
END2 }

FORM (Sound_extractChannel, U"Sound: Extract channel", nullptr) {
	CHANNEL (U"Channel (number, Left, or Right)", U"1")
	OK2
DO
	long channel = GET_INTEGER (U"Channel");
	LOOP {
		iam (Sound);
		autoSound thee = Sound_extractChannel (me, channel);
		praat_new (thee.move(), my name, U"_ch", channel);
	}
END2 }

DIRECT2 (Sound_extractLeftChannel) {
	LOOP {
		iam (Sound);
		autoSound thee = Sound_extractChannel (me, 1);
		praat_new (thee.move(), my name, U"_left");
	}
END2 }

FORM (Sound_extractPart, U"Sound: Extract part", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.1")
	OPTIONMENU_ENUM (U"Window shape", kSound_windowShape, DEFAULT)
	POSITIVE (U"Relative width", U"1.0")
	BOOLEAN (U"Preserve times", false)
	OK2
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_extractPart (me,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_ENUM (kSound_windowShape, U"Window shape"), GET_REAL (U"Relative width"),
			GET_INTEGER (U"Preserve times"));
		praat_new (thee.move(), my name, U"_part");
	}
END2 }

FORM (Sound_extractPartForOverlap, U"Sound: Extract part for overlap", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.1")
	POSITIVE (U"Overlap (s)", U"0.01")
	OK2
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_extractPartForOverlap (me,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"),
			GET_REAL (U"Overlap"));
		praat_new (thee.move(), my name, U"_part");
	}
END2 }

DIRECT2 (Sound_extractRightChannel) {
	LOOP {
		iam (Sound);
		autoSound thee = Sound_extractChannel (me, 2);
		praat_new (thee.move(), my name, U"_right");
	}
END2 }

FORM (Sound_filter_deemphasis, U"Sound: Filter (de-emphasis)", U"Sound: Filter (de-emphasis)...") {
	REAL (U"From frequency (Hz)", U"50.0")
	OK2
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_filter_deemphasis (me, GET_REAL (U"From frequency"));
		praat_new (thee.move(), my name, U"_deemp");
	}
END2 }

FORM (Sound_filter_formula, U"Sound: Filter (formula)...", U"Formula...") {
	LABEL (U"", U"Frequency-domain filtering with a formula (uses Sound-to-Spectrum and Spectrum-to-Sound): x is frequency in hertz")
	TEXTFIELD (U"formula", U"if x<500 or x>1000 then 0 else self fi; rectangular band filter")
	OK2
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_filter_formula (me, GET_STRING (U"formula"), interpreter);
		praat_new (thee.move(), my name, U"_filt");
	}
END2 }

FORM (Sound_filter_oneFormant, U"Sound: Filter (one formant)", U"Sound: Filter (one formant)...") {
	REAL (U"Frequency (Hz)", U"1000.0")
	POSITIVE (U"Bandwidth (Hz)", U"100.0")
	OK2
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_filter_oneFormant (me, GET_REAL (U"Frequency"), GET_REAL (U"Bandwidth"));
		praat_new (thee.move(), my name, U"_filt");
	}
END2 }

FORM (Sound_filterWithOneFormantInline, U"Sound: Filter with one formant (in-line)", U"Sound: Filter with one formant (in-line)...") {
	REAL (U"Frequency (Hz)", U"1000.0")
	POSITIVE (U"Bandwidth (Hz)", U"100.0")
	OK2
DO
	LOOP {
		iam (Sound);
		Sound_filterWithOneFormantInline (me, GET_REAL (U"Frequency"), GET_REAL (U"Bandwidth"));
		praat_dataChanged (me);
	}
END2 }

FORM (Sound_filter_passHannBand, U"Sound: Filter (pass Hann band)", U"Sound: Filter (pass Hann band)...") {
	REAL (U"From frequency (Hz)", U"500.0")
	REAL (U"To frequency (Hz)", U"1000.0")
	POSITIVE (U"Smoothing (Hz)", U"100.0")
	OK2
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_filter_passHannBand (me,
			GET_REAL (U"From frequency"), GET_REAL (U"To frequency"), GET_REAL (U"Smoothing"));
		praat_new (thee.move(), my name, U"_band");
	}
END2 }

FORM (Sound_filter_preemphasis, U"Sound: Filter (pre-emphasis)", U"Sound: Filter (pre-emphasis)...") {
	REAL (U"From frequency (Hz)", U"50.0")
	OK2
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_filter_preemphasis (me, GET_REAL (U"From frequency"));
		praat_new (thee.move(), my name, U"_preemp");
	}
END2 }

FORM (Sound_filter_stopHannBand, U"Sound: Filter (stop Hann band)", U"Sound: Filter (stop Hann band)...") {
	REAL (U"From frequency (Hz)", U"500.0")
	REAL (U"To frequency (Hz)", U"1000.0")
	POSITIVE (U"Smoothing (Hz)", U"100.0")
	OK2
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_filter_stopHannBand (me, GET_REAL (U"From frequency"), GET_REAL (U"To frequency"), GET_REAL (U"Smoothing"));
		praat_new (thee.move(), my name, U"_band");
	}
END2 }

FORM (Sound_formula, U"Sound: Formula", U"Sound: Formula...") {
	LABEL (U"label1", U"! `x' is the time in seconds, `col' is the sample number.")
	LABEL (U"label2", U"x = x1   ! time associated with first sample")
	LABEL (U"label3", U"for col from 1 to ncol")
	LABEL (U"label4", U"   self [col] = ...")
	TEXTFIELD (U"formula", U"self")
	LABEL (U"label5", U"   x = x + dx")
	LABEL (U"label6", U"endfor")
	OK2
DO
	LOOP {
		iam (Sound);
		try {
			Matrix_formula (me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Sound may have partially changed
			throw;
		}
	}
END2 }

FORM (Sound_formula_part, U"Sound: Formula (part)", U"Sound: Formula...") {
	REAL (U"From time", U"0.0")
	REAL (U"To time", U"0.0 (= all)")
	NATURAL (U"From channel", U"1")
	NATURAL (U"To channel", U"2")
	TEXTFIELD (U"formula", U"2 * self")
	OK2
DO
	LOOP {
		iam (Sound);
		try {
			Matrix_formula_part (me,
				GET_REAL (U"From time"), GET_REAL (U"To time"),
				GET_INTEGER (U"From channel") - 0.5, GET_INTEGER (U"To channel") + 0.5,
				GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Sound may have partially changed
			throw;
		}
	}
END2 }

FORM (Sound_getAbsoluteExtremum, U"Sound: Get absolute extremum", U"Sound: Get absolute extremum...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	LOOP {
		iam (Sound);
		double absoluteExtremum = Vector_getAbsoluteExtremum (me,
			GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (absoluteExtremum, U"Pascal");
	}
END2 }

FORM (Sound_getEnergy, U"Sound: Get energy", U"Sound: Get energy...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	OK2
DO
	LOOP {
		iam (Sound);
		double energy = Sound_getEnergy (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"));
		Melder_informationReal (energy, U"Pa2 sec");
	}
END2 }

DIRECT2 (Sound_getEnergyInAir) {
	LOOP {
		iam (Sound);
		double energyInAir = Sound_getEnergyInAir (me);
		Melder_informationReal (energyInAir, U"Joule/m2");
	}
END2 }

FORM (Sound_getIndexFromTime, U"Get sample number from time", U"Get sample number from time...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (Sound);
		double realIndex = Sampled_xToIndex (me, GET_REAL (U"Time"));
		Melder_informationReal (realIndex, nullptr);
	}
END2 }

DIRECT2 (Sound_getIntensity_dB) {
	LOOP {
		iam (Sound);
		double intensity = Sound_getIntensity_dB (me);
		Melder_informationReal (intensity, U"dB");
	}
END2 }

FORM (Sound_getMaximum, U"Sound: Get maximum", U"Sound: Get maximum...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	LOOP {
		iam (Sound);
		double maximum = Vector_getMaximum (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (maximum, U"Pascal");
	}
END2 }

FORM (old_Sound_getMean, U"Sound: Get mean", U"Sound: Get mean...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	OK2
DO
	LOOP {
		iam (Sound);
		double mean = Vector_getMean (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), Vector_CHANNEL_AVERAGE);
		Melder_informationReal (mean, U"Pascal");
	}
END2 }

FORM (Sound_getMean, U"Sound: Get mean", U"Sound: Get mean...") {
	CHANNEL (U"Channel", U"0 (= all)")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	OK2
DO_ALTERNATIVE (old_Sound_getMean)
	LOOP {
		iam (Sound);
		long channel = GET_INTEGER (U"Channel");
		if (channel > my ny) channel = 1;
		double mean = Vector_getMean (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), channel);
		Melder_informationReal (mean, U"Pascal");
	}
END2 }

FORM (Sound_getMinimum, U"Sound: Get minimum", U"Sound: Get minimum...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	LOOP {
		iam (Sound);
		double minimum = Vector_getMinimum (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (minimum, U"Pascal");
	}
END2 }

FORM (old_Sound_getNearestZeroCrossing, U"Sound: Get nearest zero crossing", U"Sound: Get nearest zero crossing...") {
	REAL (U"Time (s)", U"0.5")
	OK2
DO
	LOOP {
		iam (Sound);
		if (my ny > 1) Melder_throw (U"Cannot determine a zero crossing for a stereo sound.");
		double zeroCrossing = Sound_getNearestZeroCrossing (me, GET_REAL (U"Time"), 1);
		Melder_informationReal (zeroCrossing, U"seconds");
	}
END2 }

FORM (Sound_getNearestZeroCrossing, U"Sound: Get nearest zero crossing", U"Sound: Get nearest zero crossing...") {
	CHANNEL (U"Channel (number, Left, or Right)", U"1")
	REAL (U"Time (s)", U"0.5")
	OK2
DO_ALTERNATIVE (old_Sound_getNearestZeroCrossing)
	LOOP {
		iam (Sound);
		long channel = GET_INTEGER (U"Channel");
		if (channel > my ny) channel = 1;
		double zeroCrossing = Sound_getNearestZeroCrossing (me, GET_REAL (U"Time"), channel);
		Melder_informationReal (zeroCrossing, U"seconds");
	}
END2 }

DIRECT2 (Sound_getNumberOfChannels) {
	LOOP {
		iam (Sound);
		long numberOfChannels = my ny;
		Melder_information (numberOfChannels, numberOfChannels == 1 ? U" channel (mono)" : numberOfChannels == 2 ? U" channels (stereo)" : U"channels");
	}
END2 }

DIRECT2 (Sound_getNumberOfSamples) {
	LOOP {
		iam (Sound);
		long numberOfSamples = my nx;
		Melder_information (numberOfSamples, U" samples");
	}
END2 }

FORM (Sound_getPower, U"Sound: Get power", U"Sound: Get power...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	OK2
DO
	LOOP {
		iam (Sound);
		double power = Sound_getPower (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"));
		Melder_informationReal (power, U"Pa2");
	}
END2 }

DIRECT2 (Sound_getPowerInAir) {
	LOOP {
		iam (Sound);
		double powerInAir = Sound_getPowerInAir (me);
		Melder_informationReal (powerInAir, U"Watt/m2");
	}
END2 }

FORM (Sound_getRootMeanSquare, U"Sound: Get root-mean-square", U"Sound: Get root-mean-square...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	OK2
DO
	LOOP {
		iam (Sound);
		double rootMeanSquare = Sound_getRootMeanSquare (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"));
		Melder_informationReal (rootMeanSquare, U"Pascal");
	}
END2 }

DIRECT2 (Sound_getSamplePeriod) {
	LOOP {
		iam (Sound);
		double samplePeriod = my dx;
		Melder_informationReal (samplePeriod, U"seconds");
	}
END2 }

DIRECT2 (Sound_getSampleRate) {
	LOOP {
		iam (Sound);
		double samplingFrequency = 1.0 / my dx;
		Melder_informationReal (samplingFrequency, U"Hz");
	}
END2 }

FORM (old_Sound_getStandardDeviation, U"Sound: Get standard deviation", U"Sound: Get standard deviation...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	OK2
DO
	LOOP {
		iam (Sound);
		double stdev = Vector_getStandardDeviation (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), Vector_CHANNEL_AVERAGE);
		Melder_informationReal (stdev, U"Pascal");
	}
END2 }

FORM (Sound_getStandardDeviation, U"Sound: Get standard deviation", U"Sound: Get standard deviation...") {
	CHANNEL (U"Channel", U"0 (= average)")
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	OK2
DO_ALTERNATIVE (old_Sound_getStandardDeviation)
	LOOP {
		iam (Sound);
		long channel = GET_INTEGER (U"Channel");
		if (channel > my ny) channel = 1;
		double stdev = Vector_getStandardDeviation (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), channel);
		Melder_informationReal (stdev, U"Pascal");
	}
END2 }

FORM (Sound_getTimeFromIndex, U"Get time from sample number", U"Get time from sample number...") {
	INTEGER (U"Sample number", U"100")
	OK2
DO
	LOOP {
		iam (Sound);
		double time = Sampled_indexToX (me, GET_INTEGER (U"Sample number"));
		Melder_informationReal (time, U"seconds");
	}
END2 }

FORM (Sound_getTimeOfMaximum, U"Sound: Get time of maximum", U"Sound: Get time of maximum...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	LOOP {
		iam (Sound);
		double time = Vector_getXOfMaximum (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (time, U"seconds");
	}
END2 }

FORM (Sound_getTimeOfMinimum, U"Sound: Get time of minimum", U"Sound: Get time of minimum...") {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	LOOP {
		iam (Sound);
		double time = Vector_getXOfMinimum (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (time, U"seconds");
	}
END2 }

FORM (old_Sound_getValueAtIndex, U"Sound: Get value at sample number", U"Sound: Get value at sample number...") {
	INTEGER (U"Sample number", U"100")
	OK2
DO
	LOOP {
		iam (Sound);
		long sampleIndex = GET_INTEGER (U"Sample number");
		Melder_informationReal (sampleIndex < 1 || sampleIndex > my nx ? NUMundefined :
			my ny == 1 ? my z [1] [sampleIndex] : 0.5 * (my z [1] [sampleIndex] + my z [2] [sampleIndex]), U"Pascal");
	}
END2 }

FORM (Sound_getValueAtIndex, U"Sound: Get value at sample number", U"Sound: Get value at sample number...") {
	CHANNEL (U"Channel", U"0 (= average)")
	INTEGER (U"Sample number", U"100")
	OK2
DO_ALTERNATIVE (old_Sound_getValueAtIndex)
	LOOP {
		iam (Sound);
		long sampleIndex = GET_INTEGER (U"Sample number");
		long channel = GET_INTEGER (U"Channel");
		if (channel > my ny) channel = 1;
		Melder_informationReal (sampleIndex < 1 || sampleIndex > my nx ? NUMundefined :
			Sampled_getValueAtSample (me, sampleIndex, channel, 0), U"Pascal");
	}
END2 }

FORM (old_Sound_getValueAtTime, U"Sound: Get value at time", U"Sound: Get value at time...") {
	REAL (U"Time (s)", U"0.5")
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"Nearest")
		RADIOBUTTON (U"Linear")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	LOOP {
		iam (Sound);
		double value = Vector_getValueAtX (me, GET_REAL (U"Time"), Vector_CHANNEL_AVERAGE, GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (value, U"Pascal");
	}
END2 }

FORM (Sound_getValueAtTime, U"Sound: Get value at time", U"Sound: Get value at time...") {
	CHANNEL (U"Channel", U"0 (= average)")
	REAL (U"Time (s)", U"0.5")
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"Nearest")
		RADIOBUTTON (U"Linear")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO_ALTERNATIVE (old_Sound_getValueAtTime)
	LOOP {
		iam (Sound);
		long channel = GET_INTEGER (U"Channel");
		if (channel > my ny) channel = 1;
		double value = Vector_getValueAtX (me, GET_REAL (U"Time"), channel, GET_INTEGER (U"Interpolation") - 1);
		Melder_informationReal (value, U"Pascal");
	}
END2 }

DIRECT2 (Sound_help) {
	Melder_help (U"Sound");
END2 }

FORM (Sound_lengthen_overlapAdd, U"Sound: Lengthen (overlap-add)", U"Sound: Lengthen (overlap-add)...") {
	POSITIVE (U"Minimum pitch (Hz)", U"75")
	POSITIVE (U"Maximum pitch (Hz)", U"600")
	POSITIVE (U"Factor", U"1.5")
	OK2
DO
	double minimumPitch = GET_REAL (U"Minimum pitch"), maximumPitch = GET_REAL (U"Maximum pitch");
	double factor = GET_REAL (U"Factor");
	if (minimumPitch >= maximumPitch) Melder_throw (U"Maximum pitch should be greater than minimum pitch.");
	LOOP {
		iam (Sound);
		autoSound thee = Sound_lengthen_overlapAdd (me, minimumPitch, maximumPitch, factor);
		praat_new (thee.move(), my name, U"_", Melder_fixed (factor, 2));
	}
END2 }

FORM (Sound_multiply, U"Sound: Multiply", nullptr) {
	REAL (U"Multiplication factor", U"1.5")
	OK2
DO
	LOOP {
		iam (Sound);
		Vector_multiplyByScalar (me, GET_REAL (U"Multiplication factor"));
		praat_dataChanged (me);
	}
END2 }

FORM (Sound_multiplyByWindow, U"Sound: Multiply by window", nullptr) {
	OPTIONMENU_ENUM (U"Window shape", kSound_windowShape, HANNING)
	OK2
DO
	LOOP {
		iam (Sound);
		Sound_multiplyByWindow (me, GET_ENUM (kSound_windowShape, U"Window shape"));
		praat_dataChanged (me);
	}
END2 }

FORM (Sound_overrideSamplingFrequency, U"Sound: Override sampling frequency", nullptr) {
	POSITIVE (U"New sampling frequency (Hz)", U"16000.0")
	OK2
DO
	LOOP {
		iam (Sound);
		Sound_overrideSamplingFrequency (me, GET_REAL (U"New sampling frequency"));
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (Sound_play) {
	int n = 0;
	LOOP {
		n ++;
	}
	if (n == 1 || MelderAudio_getOutputMaximumAsynchronicity () < kMelder_asynchronicityLevel_ASYNCHRONOUS) {
		LOOP {
			iam (Sound);
			Sound_play (me, nullptr, nullptr);
		}
	} else {
		MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel_INTERRUPTABLE);
		LOOP {
			iam (Sound);
			Sound_play (me, nullptr, nullptr);   // BUG: exception-safe?
		}
		MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel_ASYNCHRONOUS);
	}
END2 }

FORM (Sound_preemphasizeInline, U"Sound: Pre-emphasize (in-line)", U"Sound: Pre-emphasize (in-line)...") {
	REAL (U"From frequency (Hz)", U"50.0")
	OK2
DO
	LOOP {
		iam (Sound);
		Sound_preEmphasis (me, GET_REAL (U"From frequency"));
		Vector_scale (me, 0.99);
		praat_dataChanged (me);
	}
END2 }

FORM_READ2 (Sound_readSeparateChannelsFromSoundFile, U"Read separate channels from sound file", nullptr, true) {
	autoSound sound = Sound_readFromSoundFile (file);
	char32 name [300];
	Melder_sprint (name,300, MelderFile_name (file));
	char32 *lastPeriod = str32rchr (name, U'.');
	if (lastPeriod) {
		*lastPeriod = U'\0';
	}
	for (long ichan = 1; ichan <= sound -> ny; ichan ++) {
		autoSound thee = Sound_extractChannel (sound.get(), ichan);
		praat_new (thee.move(), name, U"_ch", ichan);
	}
END2 }

FORM_READ2 (Sound_readFromRawAlawFile, U"Read Sound from raw Alaw file", nullptr, true) {
	autoSound me = Sound_readFromRawAlawFile (file);
	praat_new (me.move(), MelderFile_name (file));
END2 }

static SoundRecorder theReferenceToTheOnlySoundRecorder;
static int thePreviousNumberOfChannels;

static void cb_SoundRecorder_destruction (SoundRecorder /* me */) {
	theReferenceToTheOnlySoundRecorder = nullptr;
}
static void cb_SoundRecorder_publication (Editor /* me */, autoDaata publication) {
	try {
		praat_new (publication.move());
	} catch (MelderError) {
		Melder_flushError ();
	}
	praat_updateSelection ();
}
static void do_Sound_record (int numberOfChannels) {
	if (theCurrentPraatApplication -> batch)
		Melder_throw (U"Cannot record a Sound from batch.");
	if (theReferenceToTheOnlySoundRecorder && numberOfChannels == thePreviousNumberOfChannels) {
		Editor_raise (theReferenceToTheOnlySoundRecorder);
	} else {
		forget (theReferenceToTheOnlySoundRecorder);
		autoSoundRecorder recorder = SoundRecorder_create (numberOfChannels);
		Editor_setDestructionCallback (recorder.get(), cb_SoundRecorder_destruction);
		Editor_setPublicationCallback (recorder.get(), cb_SoundRecorder_publication);
		theReferenceToTheOnlySoundRecorder = recorder.get();
		recorder.releaseToUser();
		thePreviousNumberOfChannels = numberOfChannels;
	}
}
DIRECT2 (Sound_record_mono) {
	do_Sound_record (1);
END2 }
DIRECT2 (Sound_record_stereo) {
	do_Sound_record (2);
END2 }

FORM (Sound_recordFixedTime, U"Record Sound", nullptr) {
	RADIO (U"Input source", 1)
		RADIOBUTTON (U"Microphone")
		RADIOBUTTON (U"Line")
	REAL (U"Gain (0-1)", U"0.1")
	REAL (U"Balance (0-1)", U"0.5")
	RADIO (U"Sampling frequency", 1)
		#ifdef UNIX
		RADIOBUTTON (U"8000")
		#endif
		#ifndef macintosh
		RADIOBUTTON (U"11025")
		#endif
		#ifdef UNIX
		RADIOBUTTON (U"16000")
		#endif
		#ifndef macintosh
		RADIOBUTTON (U"22050")
		#endif
		#ifdef UNIX
		RADIOBUTTON (U"32000")
		#endif
		RADIOBUTTON (U"44100")
		RADIOBUTTON (U"48000")
		RADIOBUTTON (U"96000")
	POSITIVE (U"Duration (seconds)", U"1.0")
	OK2
DO
	autoSound me = Sound_recordFixedTime (GET_INTEGER (U"Input source"),
		GET_REAL (U"Gain"), GET_REAL (U"Balance"),
		Melder_atof (GET_STRING (U"Sampling frequency")), GET_REAL (U"Duration"));
	praat_new (me.move(), U"untitled");
END2 }

FORM (Sound_resample, U"Sound: Resample", U"Sound: Resample...") {
	POSITIVE (U"New sampling frequency (Hz)", U"10000.0")
	NATURAL (U"Precision (samples)", U"50")
	OK2
DO
	double samplingFrequency = GET_REAL (U"New sampling frequency");
	LOOP {
		iam (Sound);
		autoSound thee = Sound_resample (me, samplingFrequency, GET_INTEGER (U"Precision"));
		praat_new (thee.move(), my name, U"_", (long) round (samplingFrequency));
	}
END2 }

DIRECT2 (Sound_reverse) {
	LOOP {
		iam (Sound);
		Sound_reverse (me, 0.0, 0.0);
		praat_dataChanged (me);
	}
END2 }

FORM_WRITE2 (Sound_saveAs24BitWavFile, U"Save as 24-bit WAV file", nullptr, U"wav") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_WAV, 24);
END2 }

FORM_WRITE2 (Sound_saveAs32BitWavFile, U"Save as 32-bit WAV file", nullptr, U"wav") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_WAV, 32);
END2 }

FORM (Sound_scalePeak, U"Sound: Scale peak", U"Sound: Scale peak...") {
	POSITIVE (U"New absolute peak", U"0.99")
	OK2
DO
	LOOP {
		iam (Sound);
		Vector_scale (me, GET_REAL (U"New absolute peak"));
		praat_dataChanged (me);
	}
END2 }

FORM (Sound_scaleIntensity, U"Sound: Scale intensity", U"Sound: Scale intensity...") {
	POSITIVE (U"New average intensity (dB SPL)", U"70.0")
	OK2
DO
	LOOP {
		iam (Sound);
		Sound_scaleIntensity (me, GET_REAL (U"New average intensity"));
		praat_dataChanged (me);
	}
END2 }

FORM (old_Sound_setValueAtIndex, U"Sound: Set value at sample number", U"Sound: Set value at sample number...") {
	NATURAL (U"Sample number", U"100")
	REAL (U"New value", U"0")
	OK2
DO
	LOOP {
		iam (Sound);
		long index = GET_INTEGER (U"Sample number");
		if (index > my nx)
			Melder_throw (U"The sample number should not exceed the number of samples, which is ", my nx, U".");
		for (long channel = 1; channel <= my ny; channel ++)
			my z [channel] [index] = GET_REAL (U"New value");
		praat_dataChanged (me);
	}
END2 }

FORM (Sound_setValueAtIndex, U"Sound: Set value at sample number", U"Sound: Set value at sample number...") {
	CHANNEL (U"Channel", U"0 (= all)")
	NATURAL (U"Sample number", U"100")
	REAL (U"New value", U"0")
	OK2
DO_ALTERNATIVE (old_Sound_setValueAtIndex)
	LOOP {
		iam (Sound);
		long index = GET_INTEGER (U"Sample number");
		if (index > my nx)
			Melder_throw (U"The sample number should not exceed the number of samples, which is ", my nx, U".");
		long channel = GET_INTEGER (U"Channel");
		if (channel > my ny) channel = 1;
		if (channel > 0) {
			my z [channel] [index] = GET_REAL (U"New value");
		} else {
			for (channel = 1; channel <= my ny; channel ++) {
				my z [channel] [index] = GET_REAL (U"New value");
			}
		}
		praat_dataChanged (me);
	}
END2 }

FORM (Sound_setPartToZero, U"Sound: Set part to zero", nullptr) {
	REAL (U"left Time range (s)", U"0.0")
	REAL (U"right Time range (s)", U"0.0 (= all)")
	RADIO (U"Cut", 2)
		OPTION (U"at exactly these times")
		OPTION (U"at nearest zero crossing")
	OK2
DO
	LOOP {
		iam (Sound);
		Sound_setZero (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), GET_INTEGER (U"Cut") - 1);
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (Sound_subtractMean) {
	LOOP {
		iam (Sound);
		Vector_subtractMean (me);
		praat_dataChanged (me);
	}
END2 }

FORM (Sound_to_Manipulation, U"Sound: To Manipulation", U"Manipulation") {
	POSITIVE (U"Time step (s)", U"0.01")
	POSITIVE (U"Minimum pitch (Hz)", U"75")
	POSITIVE (U"Maximum pitch (Hz)", U"600")
	OK2
DO
	double fmin = GET_REAL (U"Minimum pitch"), fmax = GET_REAL (U"Maximum pitch");
	if (fmax <= fmin) Melder_throw (U"Maximum pitch must be greater than minimum pitch.");
	LOOP {
		iam (Sound);
		autoManipulation thee = Sound_to_Manipulation (me, GET_REAL (U"Time step"), fmin, fmax);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Cochleagram, U"Sound: To Cochleagram", nullptr) {
	POSITIVE (U"Time step (s)", U"0.01")
	POSITIVE (U"Frequency resolution (Bark)", U"0.1")
	POSITIVE (U"Window length (s)", U"0.03")
	REAL (U"Forward-masking time (s)", U"0.03")
	OK2
DO
	LOOP {
		iam (Sound);
		autoCochleagram thee = Sound_to_Cochleagram (me, GET_REAL (U"Time step"),
			GET_REAL (U"Frequency resolution"), GET_REAL (U"Window length"), GET_REAL (U"Forward-masking time"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Cochleagram_edb, U"Sound: To Cochleagram (De Boer, Meddis & Hewitt)", nullptr) {
	POSITIVE (U"Time step (s)", U"0.01")
	POSITIVE (U"Frequency resolution (Bark)", U"0.1")
	BOOLEAN (U"Has synapse", true)
	LABEL (U"", U"Meddis synapse properties")
	POSITIVE (U"   replenishment rate (/sec)", U"5.05")
	POSITIVE (U"   loss rate (/sec)", U"2500.0")
	POSITIVE (U"   return rate (/sec)", U"6580.0")
	POSITIVE (U"   reprocessing rate (/sec)", U"66.31")
	OK2
DO
	LOOP {
		iam (Sound);
		autoCochleagram thee = Sound_to_Cochleagram_edb (me, GET_REAL (U"Time step"),
			GET_REAL (U"Frequency resolution"), GET_INTEGER (U"Has synapse"),
			GET_REAL (U"   replenishment rate"), GET_REAL (U"   loss rate"),
			GET_REAL (U"   return rate"), GET_REAL (U"   reprocessing rate"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Formant_burg, U"Sound: To Formant (Burg method)", U"Sound: To Formant (burg)...") {
	REAL (U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (U"Max. number of formants", U"5.0")
	REAL (U"Maximum formant (Hz)", U"5500.0 (= adult female)")
	POSITIVE (U"Window length (s)", U"0.025")
	POSITIVE (U"Pre-emphasis from (Hz)", U"50.0")
	OK2
DO
	LOOP {
		iam (Sound);
		autoFormant thee = Sound_to_Formant_burg (me, GET_REAL (U"Time step"),
			GET_REAL (U"Max. number of formants"), GET_REAL (U"Maximum formant"),
			GET_REAL (U"Window length"), GET_REAL (U"Pre-emphasis from"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Formant_keepAll, U"Sound: To Formant (keep all)", U"Sound: To Formant (keep all)...") {
	REAL (U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (U"Max. number of formants", U"5.0")
	REAL (U"Maximum formant (Hz)", U"5500.0 (= adult female)")
	POSITIVE (U"Window length (s)", U"0.025")
	POSITIVE (U"Pre-emphasis from (Hz)", U"50.0")
	OK2
DO
	LOOP {
		iam (Sound);
		autoFormant thee = Sound_to_Formant_keepAll (me, GET_REAL (U"Time step"),
			GET_REAL (U"Max. number of formants"), GET_REAL (U"Maximum formant"),
			GET_REAL (U"Window length"), GET_REAL (U"Pre-emphasis from"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Formant_willems, U"Sound: To Formant (split Levinson (Willems))", U"Sound: To Formant (sl)...") {
	REAL (U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (U"Number of formants", U"5.0")
	REAL (U"Maximum formant (Hz)", U"5500.0 (= adult female)")
	POSITIVE (U"Window length (s)", U"0.025")
	POSITIVE (U"Pre-emphasis from (Hz)", U"50.0")
	OK2
DO
	LOOP {
		iam (Sound);
		autoFormant thee = Sound_to_Formant_willems (me, GET_REAL (U"Time step"),
			GET_REAL (U"Number of formants"), GET_REAL (U"Maximum formant"),
			GET_REAL (U"Window length"), GET_REAL (U"Pre-emphasis from"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Harmonicity_ac, U"Sound: To Harmonicity (ac)", U"Sound: To Harmonicity (ac)...") {
	POSITIVE (U"Time step (s)", U"0.01")
	POSITIVE (U"Minimum pitch (Hz)", U"75.0")
	REAL (U"Silence threshold", U"0.1")
	POSITIVE (U"Periods per window", U"4.5")
	OK2
DO
	double periodsPerWindow = GET_REAL (U"Periods per window");
	if (periodsPerWindow < 3.0) Melder_throw (U"Number of periods per window must be at least 3.0.");
	LOOP {
		iam (Sound);
		autoHarmonicity thee = Sound_to_Harmonicity_ac (me, GET_REAL (U"Time step"),
			GET_REAL (U"Minimum pitch"), GET_REAL (U"Silence threshold"), periodsPerWindow);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Harmonicity_cc, U"Sound: To Harmonicity (cc)", U"Sound: To Harmonicity (cc)...") {
	POSITIVE (U"Time step (s)", U"0.01")
	POSITIVE (U"Minimum pitch (Hz)", U"75.0")
	REAL (U"Silence threshold", U"0.1")
	POSITIVE (U"Periods per window", U"1.0")
	OK2
DO
	LOOP {
		iam (Sound);
		autoHarmonicity thee = Sound_to_Harmonicity_cc (me, GET_REAL (U"Time step"),
			GET_REAL (U"Minimum pitch"), GET_REAL (U"Silence threshold"),
			GET_REAL (U"Periods per window"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Harmonicity_gne, U"Sound: To Harmonicity (gne)", nullptr) {
	POSITIVE (U"Minimum frequency (Hz)", U"500.0")
	POSITIVE (U"Maximum frequency (Hz)", U"4500.0")
	POSITIVE (U"Bandwidth (Hz)", U"1000.0")
	POSITIVE (U"Step (Hz)", U"80.0")
	OK2
DO
	LOOP {
		iam (Sound);
		autoMatrix thee = Sound_to_Harmonicity_GNE (me, GET_REAL (U"Minimum frequency"),
			GET_REAL (U"Maximum frequency"), GET_REAL (U"Bandwidth"),
			GET_REAL (U"Step"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (old_Sound_to_Intensity, U"Sound: To Intensity", U"Sound: To Intensity...") {
	POSITIVE (U"Minimum pitch (Hz)", U"100.0")
	REAL (U"Time step (s)", U"0.0 (= auto)")
	OK2
DO
	LOOP {
		iam (Sound);
		autoIntensity thee = Sound_to_Intensity (me,
			GET_REAL (U"Minimum pitch"), GET_REAL (U"Time step"), false);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Intensity, U"Sound: To Intensity", U"Sound: To Intensity...") {
	POSITIVE (U"Minimum pitch (Hz)", U"100.0")
	REAL (U"Time step (s)", U"0.0 (= auto)")
	BOOLEAN (U"Subtract mean", true)
	OK2
DO_ALTERNATIVE (old_Sound_to_Intensity)
	LOOP {
		iam (Sound);
		autoIntensity thee = Sound_to_Intensity (me,
			GET_REAL (U"Minimum pitch"), GET_REAL (U"Time step"), GET_INTEGER (U"Subtract mean"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_IntensityTier, U"Sound: To IntensityTier", nullptr) {
	POSITIVE (U"Minimum pitch (Hz)", U"100.0")
	REAL (U"Time step (s)", U"0.0 (= auto)")
	BOOLEAN (U"Subtract mean", true)
	OK2
DO
	LOOP {
		iam (Sound);
		autoIntensityTier thee = Sound_to_IntensityTier (me,
			GET_REAL (U"Minimum pitch"), GET_REAL (U"Time step"), GET_INTEGER (U"Subtract mean"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Sound_to_IntervalTier) {
	LOOP {
		iam (Sound);
		autoIntervalTier thee = IntervalTier_create (my xmin, my xmax);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Ltas, U"Sound: To long-term average spectrum", nullptr) {
	POSITIVE (U"Bandwidth (Hz)", U"100")
	OK2
DO
	LOOP {
		iam (Sound);
		autoLtas thee = Sound_to_Ltas (me, GET_REAL (U"Bandwidth"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Ltas_pitchCorrected, U"Sound: To Ltas (pitch-corrected)", U"Sound: To Ltas (pitch-corrected)...") {
	POSITIVE (U"Minimum pitch (Hz)", U"75.0")
	POSITIVE (U"Maximum pitch (Hz)", U"600.0")
	POSITIVE (U"Maximum frequency (Hz)", U"5000.0")
	POSITIVE (U"Bandwidth (Hz)", U"100.0")
	REAL (U"Shortest period (s)", U"0.0001")
	REAL (U"Longest period (s)", U"0.02")
	POSITIVE (U"Maximum period factor", U"1.3")
	OK2
DO
	double fmin = GET_REAL (U"Minimum pitch"), fmax = GET_REAL (U"Maximum pitch");
	if (fmax <= fmin) Melder_throw (U"Maximum pitch must be greater than minimum pitch.");
	LOOP {
		iam (Sound);
		autoLtas thee = Sound_to_Ltas_pitchCorrected (me, fmin, fmax,
			GET_REAL (U"Maximum frequency"), GET_REAL (U"Bandwidth"),
			GET_REAL (U"Shortest period"), GET_REAL (U"Longest period"), GET_REAL (U"Maximum period factor"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Sound_to_Matrix) {
	LOOP {
		iam (Sound);
		autoMatrix thee = Sound_to_Matrix (me);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Sounds_to_ParamCurve) {
	Sound s1 = nullptr, s2 = nullptr;
	LOOP {
		iam (Sound);
		( s1 ? s2 : s1 ) = me;
	}
	autoParamCurve thee = ParamCurve_create (s1, s2);
	praat_new (thee.move(), s1 -> name, U"_", s2 -> name);
END2 }

FORM (Sound_to_Pitch, U"Sound: To Pitch", U"Sound: To Pitch...") {
	REAL (U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (U"Pitch floor (Hz)", U"75.0")
	POSITIVE (U"Pitch ceiling (Hz)", U"600.0")
	OK2
DO
	LOOP {
		iam (Sound);
		autoPitch thee = Sound_to_Pitch (me, GET_REAL (U"Time step"), GET_REAL (U"Pitch floor"), GET_REAL (U"Pitch ceiling"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Pitch_ac, U"Sound: To Pitch (ac)", U"Sound: To Pitch (ac)...") {
	LABEL (U"", U"Finding the candidates")
	REAL (U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (U"Pitch floor (Hz)", U"75.0")
	NATURAL (U"Max. number of candidates", U"15")
	BOOLEAN (U"Very accurate", false)
	LABEL (U"", U"Finding a path")
	REAL (U"Silence threshold", U"0.03")
	REAL (U"Voicing threshold", U"0.45")
	REAL (U"Octave cost", U"0.01")
	REAL (U"Octave-jump cost", U"0.35")
	REAL (U"Voiced / unvoiced cost", U"0.14")
	POSITIVE (U"Pitch ceiling (Hz)", U"600.0")
	OK2
DO
	long maxnCandidates = GET_INTEGER (U"Max. number of candidates");
	if (maxnCandidates <= 1) Melder_throw (U"Maximum number of candidates must be greater than 1.");
	LOOP {
		iam (Sound);
		autoPitch thee = Sound_to_Pitch_ac (me, GET_REAL (U"Time step"),
			GET_REAL (U"Pitch floor"), 3.0, maxnCandidates, GET_INTEGER (U"Very accurate"),
			GET_REAL (U"Silence threshold"), GET_REAL (U"Voicing threshold"),
			GET_REAL (U"Octave cost"), GET_REAL (U"Octave-jump cost"),
			GET_REAL (U"Voiced / unvoiced cost"), GET_REAL (U"Pitch ceiling"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Pitch_cc, U"Sound: To Pitch (cc)", U"Sound: To Pitch (cc)...") {
	LABEL (U"", U"Finding the candidates")
	REAL (U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (U"Pitch floor (Hz)", U"75.0")
	NATURAL (U"Max. number of candidates", U"15")
	BOOLEAN (U"Very accurate", false)
	LABEL (U"", U"Finding a path")
	REAL (U"Silence threshold", U"0.03")
	REAL (U"Voicing threshold", U"0.45")
	REAL (U"Octave cost", U"0.01")
	REAL (U"Octave-jump cost", U"0.35")
	REAL (U"Voiced / unvoiced cost", U"0.14")
	POSITIVE (U"Pitch ceiling (Hz)", U"600")
	OK2
DO
	long maxnCandidates = GET_INTEGER (U"Max. number of candidates");
	if (maxnCandidates <= 1) Melder_throw (U"Maximum number of candidates must be greater than 1.");
	LOOP {
		iam (Sound);
		autoPitch thee = Sound_to_Pitch_cc (me, GET_REAL (U"Time step"),
			GET_REAL (U"Pitch floor"), 1.0, maxnCandidates, GET_INTEGER (U"Very accurate"),
			GET_REAL (U"Silence threshold"), GET_REAL (U"Voicing threshold"),
			GET_REAL (U"Octave cost"), GET_REAL (U"Octave-jump cost"),
			GET_REAL (U"Voiced / unvoiced cost"), GET_REAL (U"Pitch ceiling"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_PointProcess_extrema, U"Sound: To PointProcess (extrema)", nullptr) {
	CHANNEL (U"Channel (number, Left, or Right)", U"1")
	BOOLEAN (U"Include maxima", true)
	BOOLEAN (U"Include minima", false)
	RADIO (U"Interpolation", 4)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK2
DO
	long channel = GET_INTEGER (U"Channel");
	LOOP {
		iam (Sound);
		autoPointProcess thee = Sound_to_PointProcess_extrema (me, channel > my ny ? 1 : channel, GET_INTEGER (U"Interpolation") - 1,
			GET_INTEGER (U"Include maxima"), GET_INTEGER (U"Include minima"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_PointProcess_periodic_cc, U"Sound: To PointProcess (periodic, cc)", U"Sound: To PointProcess (periodic, cc)...") {
	POSITIVE (U"Minimum pitch (Hz)", U"75")
	POSITIVE (U"Maximum pitch (Hz)", U"600")
	OK2
DO
	double fmin = GET_REAL (U"Minimum pitch"), fmax = GET_REAL (U"Maximum pitch");
	if (fmax <= fmin) Melder_throw (U"Maximum pitch must be greater than minimum pitch.");
	LOOP {
		iam (Sound);
		autoPointProcess thee = Sound_to_PointProcess_periodic_cc (me, fmin, fmax);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_PointProcess_periodic_peaks, U"Sound: To PointProcess (periodic, peaks)", U"Sound: To PointProcess (periodic, peaks)...") {
	POSITIVE (U"Minimum pitch (Hz)", U"75")
	POSITIVE (U"Maximum pitch (Hz)", U"600")
	BOOLEAN (U"Include maxima", true)
	BOOLEAN (U"Include minima", false)
	OK2
DO
	double fmin = GET_REAL (U"Minimum pitch"), fmax = GET_REAL (U"Maximum pitch");
	if (fmax <= fmin) Melder_throw (U"Maximum pitch must be greater than minimum pitch.");
	LOOP {
		iam (Sound);
		autoPointProcess thee = Sound_to_PointProcess_periodic_peaks (me, fmin, fmax, GET_INTEGER (U"Include maxima"), GET_INTEGER (U"Include minima"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_PointProcess_zeroes, U"Get zeroes", nullptr) {
	CHANNEL (U"Channel (number, Left, or Right)", U"1")
	BOOLEAN (U"Include raisers", true)
	BOOLEAN (U"Include fallers", false)
	OK2
DO
	long channel = GET_INTEGER (U"Channel");
	LOOP {
		iam (Sound);
		autoPointProcess thee = Sound_to_PointProcess_zeroes (me, channel > my ny ? 1 : channel, GET_INTEGER (U"Include raisers"), GET_INTEGER (U"Include fallers"));
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Spectrogram, U"Sound: To Spectrogram", U"Sound: To Spectrogram...") {
	POSITIVE (U"Window length (s)", U"0.005")
	POSITIVE (U"Maximum frequency (Hz)", U"5000.0")
	POSITIVE (U"Time step (s)", U"0.002")
	POSITIVE (U"Frequency step (Hz)", U"20.0")
	RADIO_ENUM (U"Window shape", kSound_to_Spectrogram_windowShape, DEFAULT)
	OK2
DO
	LOOP {
		iam (Sound);
		autoSpectrogram thee = Sound_to_Spectrogram (me, GET_REAL (U"Window length"),
			GET_REAL (U"Maximum frequency"), GET_REAL (U"Time step"),
			GET_REAL (U"Frequency step"), GET_ENUM (kSound_to_Spectrogram_windowShape, U"Window shape"), 8.0, 8.0);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_Spectrum, U"Sound: To Spectrum", U"Sound: To Spectrum...") {
	BOOLEAN (U"Fast", 1)
	OK2
DO
	LOOP {
		iam (Sound);
		autoSpectrum thee = Sound_to_Spectrum (me, GET_INTEGER (U"Fast"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Sound_to_Spectrum_dft) {
	LOOP {
		iam (Sound);
		autoSpectrum thee = Sound_to_Spectrum (me, false);
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Sound_to_Spectrum_fft) {
	LOOP {
		iam (Sound);
		autoSpectrum thee = Sound_to_Spectrum (me, true);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (Sound_to_TextGrid, U"Sound: To TextGrid", U"Sound: To TextGrid...") {
	SENTENCE (U"All tier names", U"Mary John bell")
	SENTENCE (U"Which of these are point tiers?", U"bell")
	OK2
DO
	LOOP {
		iam (Sound);
		autoTextGrid thee = TextGrid_create (my xmin, my xmax, GET_STRING (U"All tier names"), GET_STRING (U"Which of these are point tiers?"));
		praat_new (thee.move(), my name);
	}
END2 }

DIRECT2 (Sound_to_TextTier) {
	LOOP {
		iam (Sound);
		autoTextTier thee = TextTier_create (my xmin, my xmax);
		praat_new (thee.move(), my name);
	}
END2 }

FORM (SoundInputPrefs, U"Sound recording preferences", U"SoundRecorder") {
	NATURAL (U"Buffer size (MB)", U"60")
	OPTIONMENU_ENUM (U"Input sound system", kMelder_inputSoundSystem, DEFAULT)
	OK2
SET_INTEGER (U"Buffer size", SoundRecorder_getBufferSizePref_MB ())
SET_ENUM (U"Input sound system", kMelder_inputSoundSystem, MelderAudio_getInputSoundSystem())
DO
	long size = GET_INTEGER (U"Buffer size");
	if (size > 1000) Melder_throw (U"Buffer size cannot exceed 1000 megabytes.");
	SoundRecorder_setBufferSizePref_MB (size);
	MelderAudio_setInputSoundSystem (GET_ENUM (kMelder_inputSoundSystem, U"Input sound system"));
END2 }

FORM (SoundOutputPrefs, U"Sound playing preferences", nullptr) {
	LABEL (U"", U"The following determines how sounds are played.")
	LABEL (U"", U"Between parentheses, you find what you can do simultaneously.")
	LABEL (U"", U"Decrease asynchronicity if sound plays with discontinuities.")
	OPTIONMENU_ENUM (U"Maximum asynchronicity", kMelder_asynchronicityLevel, DEFAULT)
	#define xstr(s) str(s)
	#define str(s) #s
	REAL (U"Silence before (s)", U"" xstr (kMelderAudio_outputSilenceBefore_DEFAULT))
	REAL (U"Silence after (s)", U"" xstr (kMelderAudio_outputSilenceAfter_DEFAULT))
	OPTIONMENU_ENUM (U"Output sound system", kMelder_outputSoundSystem, DEFAULT)
	OK2
SET_ENUM (U"Maximum asynchronicity", kMelder_asynchronicityLevel, MelderAudio_getOutputMaximumAsynchronicity ())
SET_REAL (U"Silence before", MelderAudio_getOutputSilenceBefore ())
SET_REAL (U"Silence after", MelderAudio_getOutputSilenceAfter ())
SET_ENUM (U"Output sound system", kMelder_outputSoundSystem, MelderAudio_getOutputSoundSystem())
DO
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	MelderAudio_setOutputMaximumAsynchronicity (GET_ENUM (kMelder_asynchronicityLevel, U"Maximum asynchronicity"));
	MelderAudio_setOutputSilenceBefore (GET_REAL (U"Silence before"));
	MelderAudio_setOutputSilenceAfter (GET_REAL (U"Silence after"));
	MelderAudio_setOutputSoundSystem (GET_ENUM (kMelder_outputSoundSystem, U"Output sound system"));
END2 }

#ifdef HAVE_PULSEAUDIO
void pulseAudio_serverReport ();
DIRECT (Praat_reportSoundServerProperties)
	pulseAudio_serverReport ();
END
#endif

FORM_WRITE2 (Sound_writeToAifcFile, U"Save as AIFC file", nullptr, U"aifc") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_AIFC, 16);
END2 }

FORM_WRITE2 (Sound_writeToAiffFile, U"Save as AIFF file", nullptr, U"aiff") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_AIFF, 16);
END2 }

FORM_WRITE2 (Sound_writeToFlacFile, U"Save as FLAC file", nullptr, U"flac") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_FLAC, 16);
END2 }

FORM_WRITE2 (Sound_writeToKayFile, U"Save as Kay sound file", nullptr, U"kay") {
	LOOP {
		iam (Sound);
		Sound_writeToKayFile (me, file);
	}
END2 }

FORM_WRITE2 (Sound_writeToNextSunFile, U"Save as NeXT/Sun file", nullptr, U"au") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_NEXT_SUN, 16);
END2 }

FORM_WRITE2 (Sound_writeToNistFile, U"Save as NIST file", nullptr, U"nist") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_NIST, 16);
END2 }

FORM_WRITE2 (Sound_saveAsRaw8bitSignedFile, U"Save as raw 8-bit signed sound file", nullptr, U"8sig") {
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_8_SIGNED);
	}
END2 }

FORM_WRITE2 (Sound_saveAsRaw8bitUnsignedFile, U"Save as raw 8-bit unsigned sound file", nullptr, U"8uns") {
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_8_UNSIGNED);
	}
END2 }

FORM_WRITE2 (Sound_saveAsRaw16bitBigEndianFile, U"Save as raw 16-bit big-endian sound file", nullptr, U"16be") {
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_16_BIG_ENDIAN);
	}
END2 }

FORM_WRITE2 (Sound_saveAsRaw16bitLittleEndianFile, U"Save as raw 16-bit little-endian sound file", nullptr, U"16le") {
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_16_LITTLE_ENDIAN);
	}
END2 }

FORM_WRITE2 (Sound_saveAsRaw24bitBigEndianFile, U"Save as raw 24-bit big-endian sound file", nullptr, U"24be") {
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_24_BIG_ENDIAN);
	}
END2 }

FORM_WRITE2 (Sound_saveAsRaw24bitLittleEndianFile, U"Save as raw 24-bit little-endian sound file", nullptr, U"24le") {
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_24_LITTLE_ENDIAN);
	}
END2 }

FORM_WRITE2 (Sound_saveAsRaw32bitBigEndianFile, U"Save as raw 32-bit big-endian sound file", nullptr, U"32be") {
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_32_BIG_ENDIAN);
	}
END2 }

FORM_WRITE2 (Sound_saveAsRaw32bitLittleEndianFile, U"Save as raw 32-bit little-endian sound file", nullptr, U"32le") {
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_32_LITTLE_ENDIAN);
	}
END2 }

FORM_WRITE2 (Sound_writeToSesamFile, U"Save as Sesam file", nullptr, U"sdf") {
	LOOP {
		iam (Sound);
		Sound_writeToSesamFile (me, file);
	}
END2 }

FORM_WRITE2 (Sound_writeToStereoAifcFile, U"Save as stereo AIFC file", nullptr, U"aifc") {
	OrderedOf<structSound> list;
	LOOP {
		iam (Sound);
		list. addItem_ref (me);
	}
	autoSound stereo = Sounds_combineToStereo (& list);
	Sound_writeToAudioFile (stereo.get(), file, Melder_AIFC, 16);
END2 }

FORM_WRITE2 (Sound_writeToStereoAiffFile, U"Save as stereo AIFF file", nullptr, U"aiff") {
	OrderedOf<structSound> list;
	LOOP {
		iam (Sound);
		list. addItem_ref (me);
	}
	autoSound stereo = Sounds_combineToStereo (& list);
	Sound_writeToAudioFile (stereo.get(), file, Melder_AIFF, 16);
END2 }

FORM_WRITE2 (Sound_writeToStereoNextSunFile, U"Save as stereo NeXT/Sun file", nullptr, U"au") {
	OrderedOf<structSound> list;
	LOOP {
		iam (Sound);
		list. addItem_ref (me);
	}
	autoSound stereo = Sounds_combineToStereo (& list);
	Sound_writeToAudioFile (stereo.get(), file, Melder_NEXT_SUN, 16);
END2 }

FORM_WRITE2 (Sound_writeToStereoNistFile, U"Save as stereo NIST file", nullptr, U"nist") {
	OrderedOf<structSound> list;
	LOOP {
		iam (Sound);
		list. addItem_ref (me);
	}
	autoSound stereo = Sounds_combineToStereo (& list);
	Sound_writeToAudioFile (stereo.get(), file, Melder_NIST, 16);
END2 }

FORM_WRITE2 (Sound_writeToStereoFlacFile, U"Save as stereo FLAC file", nullptr, U"flac") {
	OrderedOf<structSound> list;
	LOOP {
		iam (Sound);
		list. addItem_ref (me);
	}
	autoSound stereo = Sounds_combineToStereo (& list);
	Sound_writeToAudioFile (stereo.get(), file, Melder_FLAC, 16);
END2 }

FORM_WRITE2 (Sound_writeToStereoWavFile, U"Save as stereo WAV file", nullptr, U"wav") {
	OrderedOf<structSound> list;
	LOOP {
		iam (Sound);
		list. addItem_ref (me);
	}
	autoSound stereo = Sounds_combineToStereo (& list);
	Sound_writeToAudioFile (stereo.get(), file, Melder_WAV, 16);
END2 }

FORM_WRITE2 (Sound_writeToSunAudioFile, U"Save as NeXT/Sun file", nullptr, U"au") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_NEXT_SUN, 16);
END2 }

FORM_WRITE2 (Sound_writeToWavFile, U"Save as WAV file", nullptr, U"wav") {
	autoSoundAndLongSoundList list = SoundAndLongSoundList_create ();
	LOOP {
		iam (Sampled);
		list -> addItem_ref (me);
	}
	LongSound_concatenate (list.get(), file, Melder_WAV, 16);
END2 }

/***** STOP *****/

DIRECT2 (stopPlayingSound) {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
END2 }

/***** Help menus *****/

DIRECT2 (AnnotationTutorial) {
	Melder_help (U"Intro 7. Annotation");
END2 }

DIRECT2 (FilteringTutorial) {
	Melder_help (U"Filtering");
END2 }

/***** file recognizers *****/

static autoDaata macSoundOrEmptyFileRecognizer (int nread, const char * /* header */, MelderFile file) {
	/***** No data in file? This may be a Macintosh sound file with only a resource fork. *****/
	if (nread > 0) return autoDaata ();
	Melder_throw (U"File ", file, U" contains no audio data.");   // !!!
}

static autoDaata soundFileRecognizer (int nread, const char *header, MelderFile file) {
	if (nread < 16) return autoDaata ();
	if (strnequ (header, "FORM", 4) && strnequ (header + 8, "AIF", 3)) return Sound_readFromSoundFile (file);
	if (strnequ (header, "RIFF", 4) && (strnequ (header + 8, "WAVE", 4) || strnequ (header + 8, "CDDA", 4))) return Sound_readFromSoundFile (file);
	if (strnequ (header, ".snd", 4)) return Sound_readFromSoundFile (file);
	if (strnequ (header, "NIST_1A", 7)) return Sound_readFromSoundFile (file);
	if (strnequ (header, "fLaC", 4)) return Sound_readFromSoundFile (file);   // Erez Volk, March 2007
	if ((Melder_stringMatchesCriterion (MelderFile_name (file), kMelder_string_ENDS_WITH, U".mp3") ||
	     Melder_stringMatchesCriterion (MelderFile_name (file), kMelder_string_ENDS_WITH, U".MP3"))
		&& mp3_recognize (nread, header)) return Sound_readFromSoundFile (file);   // Erez Volk, May 2007
	return autoDaata ();
}

static autoDaata movieFileRecognizer (int nread, const char * /* header */, MelderFile file) {
	const char32 *fileName = MelderFile_name (file);
	/*Melder_casual ("%d %d %d %d %d %d %d %d %d %d", header [0],
		header [1], header [2], header [3],
		header [4], header [5], header [6],
		header [7], header [8], header [9]);*/
	if (nread < 512 || (! Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".mov") &&
	                    ! Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".MOV") &&
	                    ! Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".avi") &&
	                    ! Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".AVI"))) return autoDaata ();
	Melder_throw (U"This Praat version cannot open movie files.");
	return autoDaata ();
}

static autoDaata sesamFileRecognizer (int nread, const char * /* header */, MelderFile file) {
	const char32 *fileName = MelderFile_name (file);
	if (nread < 512 || (! Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".sdf") &&
	                    ! Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".SDF"))) return autoDaata ();
	return Sound_readFromSesamFile (file);
}

static autoDaata bellLabsFileRecognizer (int nread, const char *header, MelderFile file) {
	if (nread < 16 || ! strnequ (& header [0], "SIG\n", 4)) return autoDaata ();
	return Sound_readFromBellLabsFile (file);
}

static autoDaata kayFileRecognizer (int nread, const char *header, MelderFile file) {
	if (nread <= 12 || ! strnequ (& header [0], "FORMDS16", 8)) return autoDaata ();
	return Sound_readFromKayFile (file);
}

/***** override play and record buttons in manuals *****/

static autoSound melderSound, melderSoundFromFile;
static Sound last;
static int recordProc (double duration) {
	if (last == melderSound.get()) last = nullptr;
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	melderSound = Sound_recordFixedTime (1, 1.0, 0.5, 44100, duration);
	if (! melderSound) return 0;
	last = melderSound.get();
	return 1;
}
static int recordFromFileProc (MelderFile file) {
	if (last == melderSoundFromFile.get()) last = nullptr;
	Melder_warningOff ();   // like "misssing samples"
	melderSoundFromFile = Data_readFromFile (file). static_cast_move<structSound>();
	Melder_warningOn ();
	if (! melderSoundFromFile) return 0;
	if (! Thing_isa (melderSoundFromFile.get(), classSound)) { melderSoundFromFile.reset(); return 0; }
	last = melderSoundFromFile.get();
	Sound_play (melderSoundFromFile.get(), nullptr, nullptr);
	return 1;
}
static void playProc () {
	if (melderSound) {
		Sound_play (melderSound.get(), nullptr, nullptr);
		last = melderSound.get();
	}
}
static void playReverseProc () {
	/*if (melderSound) Sound_playReverse (melderSound);*/
}
static int publishPlayedProc () {
	if (! last) return 0;
	autoSound sound = Data_copy (last);
	return Data_publish (sound.move());
}

/***** buttons *****/

void praat_uvafon_Sound_init ();
void praat_uvafon_Sound_init () {

	Data_recognizeFileType (macSoundOrEmptyFileRecognizer);
	Data_recognizeFileType (soundFileRecognizer);
	Data_recognizeFileType (movieFileRecognizer);
	Data_recognizeFileType (sesamFileRecognizer);
	Data_recognizeFileType (bellLabsFileRecognizer);
	Data_recognizeFileType (kayFileRecognizer);

	SoundRecorder_preferences ();
	structSoundRecorder           :: f_preferences ();
	structFunctionEditor          :: f_preferences ();
	LongSound_preferences ();
	structTimeSoundEditor         :: f_preferences ();
	structTimeSoundAnalysisEditor :: f_preferences ();

	Melder_setRecordProc (recordProc);
	Melder_setRecordFromFileProc (recordFromFileProc);
	Melder_setPlayProc (playProc);
	Melder_setPlayReverseProc (playReverseProc);
	Melder_setPublishPlayedProc (publishPlayedProc);

	praat_addMenuCommand (U"Objects", U"New", U"Record mono Sound...", nullptr, praat_ATTRACTIVE + 'R', DO_Sound_record_mono);
	praat_addMenuCommand (U"Objects", U"New", U"Record stereo Sound...", nullptr, 0, DO_Sound_record_stereo);
	praat_addMenuCommand (U"Objects", U"New", U"Record Sound (fixed time)...", nullptr, praat_HIDDEN, DO_Sound_recordFixedTime);
	praat_addMenuCommand (U"Objects", U"New", U"Sound", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create Sound...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_Sound_create);
		praat_addMenuCommand (U"Objects", U"New", U"Create Sound as pure tone...", nullptr, 1, DO_Sound_createAsPureTone);
		praat_addMenuCommand (U"Objects", U"New", U"Create Sound from formula...", nullptr, 1, DO_Sound_createFromFormula);
		praat_addMenuCommand (U"Objects", U"New", U"-- create sound advanced --", nullptr, 1, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create Sound as tone complex...", nullptr, 1, DO_Sound_createFromToneComplex);
		praat_addMenuCommand (U"Objects", U"New", U"Create Sound from tone complex...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_Sound_createFromToneComplex);

	praat_addMenuCommand (U"Objects", U"Open", U"-- read sound --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Open", U"Open long sound file...", nullptr, 'L', DO_LongSound_open);
	praat_addMenuCommand (U"Objects", U"Open", U"Read two Sounds from stereo file...", nullptr, praat_HIDDEN, DO_Sound_readSeparateChannelsFromSoundFile);   // deprecated 2010
	praat_addMenuCommand (U"Objects", U"Open", U"Read separate channels from sound file...", nullptr, 0, DO_Sound_readSeparateChannelsFromSoundFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read from special sound file", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"Open", U"Read Sound from raw Alaw file...", nullptr, 1, DO_Sound_readFromRawAlawFile);

	praat_addMenuCommand (U"Objects", U"Goodies", U"Stop playing sound", nullptr, GuiMenu_ESCAPE, DO_stopPlayingSound);
	praat_addMenuCommand (U"Objects", U"Preferences", U"-- sound prefs --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Preferences", U"Sound recording preferences...", nullptr, 0, DO_SoundInputPrefs);
	praat_addMenuCommand (U"Objects", U"Preferences", U"Sound playing preferences...", nullptr, 0, DO_SoundOutputPrefs);
	praat_addMenuCommand (U"Objects", U"Preferences", U"LongSound preferences...", nullptr, 0, DO_LongSoundPrefs);
#ifdef HAVE_PULSEAUDIO
	praat_addMenuCommand (U"Objects", U"Technical", U"Report sound server properties", U"Report system properties", 0, DO_Praat_reportSoundServerProperties);
#endif

	praat_addAction1 (classLongSound, 0, U"LongSound help", nullptr, 0, DO_LongSound_help);
	praat_addAction1 (classLongSound, 1, U"View", nullptr, praat_ATTRACTIVE, DO_LongSound_view);
	praat_addAction1 (classLongSound, 1, U"Open", nullptr, praat_HIDDEN, DO_LongSound_view);   // deprecated 2011
	praat_addAction1 (classLongSound, 0, U"Play part...", nullptr, 0, DO_LongSound_playPart);
	praat_addAction1 (classLongSound, 1, U"Query -", nullptr, 0, nullptr);
		praat_TimeFunction_query_init (classLongSound);
		praat_addAction1 (classLongSound, 1, U"Sampling", nullptr, 1, nullptr);
		praat_addAction1 (classLongSound, 1, U"Get number of samples", nullptr, 2, DO_LongSound_getNumberOfSamples);
		praat_addAction1 (classLongSound, 1, U"Get sampling period", nullptr, 2, DO_LongSound_getSamplePeriod);
							praat_addAction1 (classLongSound, 1, U"Get sample duration", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_LongSound_getSamplePeriod);
							praat_addAction1 (classLongSound, 1, U"Get sample period", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_LongSound_getSamplePeriod);
		praat_addAction1 (classLongSound, 1, U"Get sampling frequency", nullptr, 2, DO_LongSound_getSampleRate);
							praat_addAction1 (classLongSound, 1, U"Get sample rate", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_LongSound_getSampleRate);   // deprecated 2004
		praat_addAction1 (classLongSound, 1, U"-- get time discretization --", nullptr, 2, nullptr);
		praat_addAction1 (classLongSound, 1, U"Get time from sample number...", nullptr, 2, DO_LongSound_getTimeFromIndex);
							praat_addAction1 (classLongSound, 1, U"Get time from index...", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_LongSound_getTimeFromIndex);
		praat_addAction1 (classLongSound, 1, U"Get sample number from time...", nullptr, 2, DO_LongSound_getIndexFromTime);
							praat_addAction1 (classLongSound, 1, U"Get index from time...", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_LongSound_getIndexFromTime);
	praat_addAction1 (classLongSound, 0, U"Annotate -", nullptr, 0, nullptr);
		praat_addAction1 (classLongSound, 0, U"Annotation tutorial", nullptr, 1, DO_AnnotationTutorial);
		praat_addAction1 (classLongSound, 0, U"-- to text grid --", nullptr, 1, nullptr);
		praat_addAction1 (classLongSound, 0, U"To TextGrid...", nullptr, 1, DO_LongSound_to_TextGrid);
	praat_addAction1 (classLongSound, 0, U"Convert to Sound", nullptr, 0, nullptr);
	praat_addAction1 (classLongSound, 0, U"Extract part...", nullptr, 0, DO_LongSound_extractPart);
	praat_addAction1 (classLongSound, 0, U"Concatenate?", nullptr, 0, DO_LongSound_concatenate);
	praat_addAction1 (classLongSound, 0, U"Save as WAV file...", nullptr, 0, DO_LongSound_writeToWavFile);
	praat_addAction1 (classLongSound, 0, U"Write to WAV file...", nullptr, praat_HIDDEN, DO_LongSound_writeToWavFile);
	praat_addAction1 (classLongSound, 0, U"Save as AIFF file...", nullptr, 0, DO_LongSound_writeToAiffFile);
	praat_addAction1 (classLongSound, 0, U"Write to AIFF file...", nullptr, praat_HIDDEN, DO_LongSound_writeToAiffFile);
	praat_addAction1 (classLongSound, 0, U"Save as AIFC file...", nullptr, 0, DO_LongSound_writeToAifcFile);
	praat_addAction1 (classLongSound, 0, U"Write to AIFC file...", nullptr, praat_HIDDEN, DO_LongSound_writeToAifcFile);
	praat_addAction1 (classLongSound, 0, U"Save as Next/Sun file...", nullptr, 0, DO_LongSound_writeToNextSunFile);
	praat_addAction1 (classLongSound, 0, U"Write to Next/Sun file...", nullptr, praat_HIDDEN, DO_LongSound_writeToNextSunFile);
	praat_addAction1 (classLongSound, 0, U"Save as NIST file...", nullptr, 0, DO_LongSound_writeToNistFile);
	praat_addAction1 (classLongSound, 0, U"Write to NIST file...", nullptr, praat_HIDDEN, DO_LongSound_writeToNistFile);
	praat_addAction1 (classLongSound, 0, U"Save as FLAC file...", nullptr, 0, DO_LongSound_writeToFlacFile);
	praat_addAction1 (classLongSound, 0, U"Write to FLAC file...", nullptr, praat_HIDDEN, DO_LongSound_writeToFlacFile);
	praat_addAction1 (classLongSound, 0, U"Save left channel as WAV file...", nullptr, 0, DO_LongSound_writeLeftChannelToWavFile);
	praat_addAction1 (classLongSound, 0, U"Write left channel to WAV file...", nullptr, praat_HIDDEN, DO_LongSound_writeLeftChannelToWavFile);
	praat_addAction1 (classLongSound, 0, U"Save left channel as AIFF file...", nullptr, 0, DO_LongSound_writeLeftChannelToAiffFile);
	praat_addAction1 (classLongSound, 0, U"Write left channel to AIFF file...", nullptr, praat_HIDDEN, DO_LongSound_writeLeftChannelToAiffFile);
	praat_addAction1 (classLongSound, 0, U"Save left channel as AIFC file...", nullptr, 0, DO_LongSound_writeLeftChannelToAifcFile);
	praat_addAction1 (classLongSound, 0, U"Write left channel to AIFC file...", nullptr, praat_HIDDEN, DO_LongSound_writeLeftChannelToAifcFile);
	praat_addAction1 (classLongSound, 0, U"Save left channel as Next/Sun file...", nullptr, 0, DO_LongSound_writeLeftChannelToNextSunFile);
	praat_addAction1 (classLongSound, 0, U"Write left channel to Next/Sun file...", nullptr, praat_HIDDEN, DO_LongSound_writeLeftChannelToNextSunFile);
	praat_addAction1 (classLongSound, 0, U"Save left channel as NIST file...", nullptr, 0, DO_LongSound_writeLeftChannelToNistFile);
	praat_addAction1 (classLongSound, 0, U"Write left channel to NIST file...", nullptr, praat_HIDDEN, DO_LongSound_writeLeftChannelToNistFile);
	praat_addAction1 (classLongSound, 0, U"Save left channel as FLAC file...", nullptr, 0, DO_LongSound_writeLeftChannelToFlacFile);
	praat_addAction1 (classLongSound, 0, U"Write left channel to FLAC file...", nullptr, praat_HIDDEN, DO_LongSound_writeLeftChannelToFlacFile);
	praat_addAction1 (classLongSound, 0, U"Save right channel as WAV file...", nullptr, 0, DO_LongSound_writeRightChannelToWavFile);
	praat_addAction1 (classLongSound, 0, U"Write right channel to WAV file...", nullptr, praat_HIDDEN, DO_LongSound_writeRightChannelToWavFile);
	praat_addAction1 (classLongSound, 0, U"Save right channel as AIFF file...", nullptr, 0, DO_LongSound_writeRightChannelToAiffFile);
	praat_addAction1 (classLongSound, 0, U"Write right channel to AIFF file...", nullptr, praat_HIDDEN, DO_LongSound_writeRightChannelToAiffFile);
	praat_addAction1 (classLongSound, 0, U"Save right channel as AIFC file...", nullptr, 0, DO_LongSound_writeRightChannelToAifcFile);
	praat_addAction1 (classLongSound, 0, U"Write right channel to AIFC file...", nullptr, praat_HIDDEN, DO_LongSound_writeRightChannelToAifcFile);
	praat_addAction1 (classLongSound, 0, U"Save right channel as Next/Sun file...", nullptr, 0, DO_LongSound_writeRightChannelToNextSunFile);
	praat_addAction1 (classLongSound, 0, U"Write right channel to Next/Sun file...", nullptr, praat_HIDDEN, DO_LongSound_writeRightChannelToNextSunFile);
	praat_addAction1 (classLongSound, 0, U"Save right channel as NIST file...", nullptr, 0, DO_LongSound_writeRightChannelToNistFile);
	praat_addAction1 (classLongSound, 0, U"Write right channel to NIST file...", nullptr, praat_HIDDEN, DO_LongSound_writeRightChannelToNistFile);
	praat_addAction1 (classLongSound, 0, U"Save right channel as FLAC file...", nullptr, 0, DO_LongSound_writeRightChannelToFlacFile);
	praat_addAction1 (classLongSound, 0, U"Write right channel to FLAC file...", nullptr, praat_HIDDEN, DO_LongSound_writeRightChannelToFlacFile);
	praat_addAction1 (classLongSound, 0, U"Save part as audio file...", nullptr, 0, DO_LongSound_writePartToAudioFile);
	praat_addAction1 (classLongSound, 0, U"Write part to audio file...", nullptr, praat_HIDDEN, DO_LongSound_writePartToAudioFile);

	praat_addAction1 (classSound, 0, U"Save as WAV file...", nullptr, 0, DO_Sound_writeToWavFile);
	praat_addAction1 (classSound, 0, U"Write to WAV file...", nullptr, praat_HIDDEN, DO_Sound_writeToWavFile);   // hidden 2011
	praat_addAction1 (classSound, 0, U"Save as AIFF file...", nullptr, 0, DO_Sound_writeToAiffFile);
	praat_addAction1 (classSound, 0, U"Write to AIFF file...", nullptr, praat_HIDDEN, DO_Sound_writeToAiffFile);
	praat_addAction1 (classSound, 0, U"Save as AIFC file...", nullptr, 0, DO_Sound_writeToAifcFile);
	praat_addAction1 (classSound, 0, U"Write to AIFC file...", nullptr, praat_HIDDEN, DO_Sound_writeToAifcFile);
	praat_addAction1 (classSound, 0, U"Save as Next/Sun file...", nullptr, 0, DO_Sound_writeToNextSunFile);
	praat_addAction1 (classSound, 0, U"Write to Next/Sun file...", nullptr, praat_HIDDEN, DO_Sound_writeToNextSunFile);
	praat_addAction1 (classSound, 0, U"Save as Sun audio file...", nullptr, praat_HIDDEN, DO_Sound_writeToSunAudioFile);
	praat_addAction1 (classSound, 0, U"Write to Sun audio file...", nullptr, praat_HIDDEN, DO_Sound_writeToSunAudioFile);
	praat_addAction1 (classSound, 0, U"Save as NIST file...", nullptr, 0, DO_Sound_writeToNistFile);
	praat_addAction1 (classSound, 0, U"Write to NIST file...", nullptr, praat_HIDDEN, DO_Sound_writeToNistFile);
	praat_addAction1 (classSound, 0, U"Save as FLAC file...", nullptr, 0, DO_Sound_writeToFlacFile);
	praat_addAction1 (classSound, 0, U"Write to FLAC file...", nullptr, praat_HIDDEN, DO_Sound_writeToFlacFile);
	praat_addAction1 (classSound, 1, U"Save as Kay sound file...", nullptr, 0, DO_Sound_writeToKayFile);
	praat_addAction1 (classSound, 1, U"Write to Kay sound file...", nullptr, praat_HIDDEN, DO_Sound_writeToKayFile);
	praat_addAction1 (classSound, 1, U"Save as Sesam file...", nullptr, praat_HIDDEN, DO_Sound_writeToSesamFile);
	praat_addAction1 (classSound, 1, U"Write to Sesam file...", nullptr, praat_HIDDEN, DO_Sound_writeToSesamFile);
	praat_addAction1 (classSound, 0, U"Save as 24-bit WAV file...", nullptr, 0, DO_Sound_saveAs24BitWavFile);
	praat_addAction1 (classSound, 0, U"Save as 32-bit WAV file...", nullptr, 0, DO_Sound_saveAs32BitWavFile);
	praat_addAction1 (classSound, 2, U"Write to stereo WAV file...", nullptr, praat_HIDDEN, DO_Sound_writeToStereoWavFile);   // deprecated 2007
	praat_addAction1 (classSound, 2, U"Write to stereo AIFF file...", nullptr, praat_HIDDEN, DO_Sound_writeToStereoAiffFile);   // deprecated 2007
	praat_addAction1 (classSound, 2, U"Write to stereo AIFC file...", nullptr, praat_HIDDEN, DO_Sound_writeToStereoAifcFile);   // deprecated 2007
	praat_addAction1 (classSound, 2, U"Write to stereo Next/Sun file...", nullptr, praat_HIDDEN, DO_Sound_writeToStereoNextSunFile);   // deprecated 2007
	praat_addAction1 (classSound, 2, U"Write to stereo NIST file...", nullptr, praat_HIDDEN, DO_Sound_writeToStereoNistFile);   // deprecated 2007
	praat_addAction1 (classSound, 2, U"Write to stereo FLAC file...", nullptr, praat_HIDDEN, DO_Sound_writeToStereoFlacFile);
	//praat_addAction1 (classSound, 1, U"Save as raw sound file", nullptr, 0, nullptr);
	praat_addAction1 (classSound, 1, U"Save as raw 8-bit signed file...", nullptr, 0, DO_Sound_saveAsRaw8bitSignedFile);
	praat_addAction1 (classSound, 1, U"Write to raw 8-bit signed file...", nullptr, praat_HIDDEN, DO_Sound_saveAsRaw8bitSignedFile);
	praat_addAction1 (classSound, 1, U"Save as raw 8-bit unsigned file...", nullptr, 0, DO_Sound_saveAsRaw8bitUnsignedFile);
	praat_addAction1 (classSound, 1, U"Write to raw 8-bit unsigned file...", nullptr, praat_HIDDEN, DO_Sound_saveAsRaw8bitUnsignedFile);
	praat_addAction1 (classSound, 1, U"Save as raw 16-bit big-endian file...", nullptr, 0, DO_Sound_saveAsRaw16bitBigEndianFile);
	praat_addAction1 (classSound, 1, U"Save as raw 16-bit little-endian file...", nullptr, 0, DO_Sound_saveAsRaw16bitLittleEndianFile);
	praat_addAction1 (classSound, 1, U"Save as raw 24-bit big-endian file...", nullptr, 0, DO_Sound_saveAsRaw24bitBigEndianFile);
	praat_addAction1 (classSound, 1, U"Save as raw 24-bit little-endian file...", nullptr, 0, DO_Sound_saveAsRaw24bitLittleEndianFile);
	praat_addAction1 (classSound, 1, U"Save as raw 32-bit big-endian file...", nullptr, 0, DO_Sound_saveAsRaw32bitBigEndianFile);
	praat_addAction1 (classSound, 1, U"Save as raw 32-bit little-endian file...", nullptr, 0, DO_Sound_saveAsRaw32bitLittleEndianFile);
	praat_addAction1 (classSound, 0, U"Sound help", nullptr, 0, DO_Sound_help);
	praat_addAction1 (classSound, 1, U"Edit", nullptr, praat_HIDDEN, DO_Sound_edit);   // deprecated 2011
	praat_addAction1 (classSound, 1, U"Open", nullptr, praat_HIDDEN, DO_Sound_edit);   // deprecated 2011
	praat_addAction1 (classSound, 1, U"View & Edit", 0, praat_ATTRACTIVE, DO_Sound_edit);
	praat_addAction1 (classSound, 0, U"Play", nullptr, 0, DO_Sound_play);
	praat_addAction1 (classSound, 1, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"Draw...", nullptr, 1, DO_Sound_draw);
	praat_addAction1 (classSound, 1, U"Query -", nullptr, 0, nullptr);
		praat_TimeFunction_query_init (classSound);
		praat_addAction1 (classSound, 1, U"Get number of channels", nullptr, 1, DO_Sound_getNumberOfChannels);
		praat_addAction1 (classSound, 1, U"Query time sampling", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get number of samples", nullptr, 2, DO_Sound_getNumberOfSamples);
		praat_addAction1 (classSound, 1, U"Get sampling period", nullptr, 2, DO_Sound_getSamplePeriod);
							praat_addAction1 (classSound, 1, U"Get sample duration", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_Sound_getSamplePeriod);
							praat_addAction1 (classSound, 1, U"Get sample period", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_Sound_getSamplePeriod);
		praat_addAction1 (classSound, 1, U"Get sampling frequency", nullptr, 2, DO_Sound_getSampleRate);
							praat_addAction1 (classSound, 1, U"Get sample rate", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_Sound_getSampleRate);   // deprecated 2004
		praat_addAction1 (classSound, 1, U"-- get time discretization --", nullptr, 2, nullptr);
		praat_addAction1 (classSound, 1, U"Get time from sample number...", nullptr, 2, DO_Sound_getTimeFromIndex);
							praat_addAction1 (classSound, 1, U"Get time from index...", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_Sound_getTimeFromIndex);
		praat_addAction1 (classSound, 1, U"Get sample number from time...", nullptr, 2, DO_Sound_getIndexFromTime);
							praat_addAction1 (classSound, 1, U"Get index from time...", nullptr, praat_HIDDEN + praat_DEPTH_2, DO_Sound_getIndexFromTime);
		praat_addAction1 (classSound, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get value at time...", nullptr, 1, DO_Sound_getValueAtTime);
		praat_addAction1 (classSound, 1, U"Get value at sample number...", nullptr, 1, DO_Sound_getValueAtIndex);
							praat_addAction1 (classSound, 1, U"Get value at index...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_Sound_getValueAtIndex);
		praat_addAction1 (classSound, 1, U"-- get shape --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get minimum...", nullptr, 1, DO_Sound_getMinimum);
		praat_addAction1 (classSound, 1, U"Get time of minimum...", nullptr, 1, DO_Sound_getTimeOfMinimum);
		praat_addAction1 (classSound, 1, U"Get maximum...", nullptr, 1, DO_Sound_getMaximum);
		praat_addAction1 (classSound, 1, U"Get time of maximum...", nullptr, 1, DO_Sound_getTimeOfMaximum);
		praat_addAction1 (classSound, 1, U"Get absolute extremum...", nullptr, 1, DO_Sound_getAbsoluteExtremum);
		praat_addAction1 (classSound, 1, U"Get nearest zero crossing...", nullptr, 1, DO_Sound_getNearestZeroCrossing);
		praat_addAction1 (classSound, 1, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get mean...", nullptr, 1, DO_Sound_getMean);
		praat_addAction1 (classSound, 1, U"Get root-mean-square...", nullptr, 1, DO_Sound_getRootMeanSquare);
		praat_addAction1 (classSound, 1, U"Get standard deviation...", nullptr, 1, DO_Sound_getStandardDeviation);
		praat_addAction1 (classSound, 1, U"-- get energy --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get energy...", nullptr, 1, DO_Sound_getEnergy);
		praat_addAction1 (classSound, 1, U"Get power...", nullptr, 1, DO_Sound_getPower);
		praat_addAction1 (classSound, 1, U"-- get energy in air --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get energy in air", nullptr, 1, DO_Sound_getEnergyInAir);
		praat_addAction1 (classSound, 1, U"Get power in air", nullptr, 1, DO_Sound_getPowerInAir);
		praat_addAction1 (classSound, 1, U"Get intensity (dB)", nullptr, 1, DO_Sound_getIntensity_dB);
	praat_addAction1 (classSound, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classSound);
		praat_addAction1 (classSound, 0, U"-- modify generic --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Reverse", nullptr, 1, DO_Sound_reverse);
		praat_addAction1 (classSound, 0, U"Formula...", nullptr, 1, DO_Sound_formula);
		praat_addAction1 (classSound, 0, U"Formula (part)...", nullptr, 1, DO_Sound_formula_part);
		praat_addAction1 (classSound, 0, U"-- add & mul --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Add...", nullptr, 1, DO_Sound_add);
		praat_addAction1 (classSound, 0, U"Subtract mean", nullptr, 1, DO_Sound_subtractMean);
		praat_addAction1 (classSound, 0, U"Multiply...", nullptr, 1, DO_Sound_multiply);
		praat_addAction1 (classSound, 0, U"Multiply by window...", nullptr, 1, DO_Sound_multiplyByWindow);
		praat_addAction1 (classSound, 0, U"Scale peak...", nullptr, 1, DO_Sound_scalePeak);
		praat_addAction1 (classSound, 0, U"Scale...", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_Sound_scalePeak);
		praat_addAction1 (classSound, 0, U"Scale intensity...", nullptr, 1, DO_Sound_scaleIntensity);
		praat_addAction1 (classSound, 0, U"-- set --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Set value at sample number...", nullptr, 1, DO_Sound_setValueAtIndex);
							praat_addAction1 (classSound, 0, U"Set value at index...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_setValueAtIndex);
		praat_addAction1 (classSound, 0, U"Set part to zero...", nullptr, 1, DO_Sound_setPartToZero);
		praat_addAction1 (classSound, 0, U"-- modify hack --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Override sampling frequency...", nullptr, 1, DO_Sound_overrideSamplingFrequency);
							praat_addAction1 (classSound, 0, U"Override sample rate...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_overrideSamplingFrequency);
		praat_addAction1 (classSound, 0, U"-- in-line filters --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"In-line filters", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Filter with one formant (in-line)...", nullptr, 2, DO_Sound_filterWithOneFormantInline);
		praat_addAction1 (classSound, 0, U"Pre-emphasize (in-line)...", nullptr, 2, DO_Sound_preemphasizeInline);
		praat_addAction1 (classSound, 0, U"De-emphasize (in-line)...", nullptr, 2, DO_Sound_deemphasizeInline);
	praat_addAction1 (classSound, 0, U"Annotate -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"Annotation tutorial", nullptr, 1, DO_AnnotationTutorial);
		praat_addAction1 (classSound, 0, U"-- to text grid --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To TextGrid...", nullptr, 1, DO_Sound_to_TextGrid);
		praat_addAction1 (classSound, 0, U"To TextTier", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_TextTier);
		praat_addAction1 (classSound, 0, U"To IntervalTier", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_IntervalTier);
	praat_addAction1 (classSound, 0, U"Analyse periodicity -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"To Pitch...", nullptr, 1, DO_Sound_to_Pitch);
		praat_addAction1 (classSound, 0, U"To Pitch (ac)...", nullptr, 1, DO_Sound_to_Pitch_ac);
		praat_addAction1 (classSound, 0, U"To Pitch (cc)...", nullptr, 1, DO_Sound_to_Pitch_cc);
		praat_addAction1 (classSound, 0, U"To PointProcess (periodic, cc)...", nullptr, 1, DO_Sound_to_PointProcess_periodic_cc);
		praat_addAction1 (classSound, 0, U"To PointProcess (periodic, peaks)...", nullptr, 1, DO_Sound_to_PointProcess_periodic_peaks);
		praat_addAction1 (classSound, 0, U"-- points --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To PointProcess (extrema)...", nullptr, 1, DO_Sound_to_PointProcess_extrema);
		praat_addAction1 (classSound, 0, U"To PointProcess (zeroes)...", nullptr, 1, DO_Sound_to_PointProcess_zeroes);
		praat_addAction1 (classSound, 0, U"-- hnr --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To Harmonicity (cc)...", nullptr, 1, DO_Sound_to_Harmonicity_cc);
		praat_addAction1 (classSound, 0, U"To Harmonicity (ac)...", nullptr, 1, DO_Sound_to_Harmonicity_ac);
		praat_addAction1 (classSound, 0, U"To Harmonicity (gne)...", nullptr, 1, DO_Sound_to_Harmonicity_gne);
		praat_addAction1 (classSound, 0, U"-- autocorrelation --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Autocorrelate...", nullptr, 1, DO_Sound_autoCorrelate);
	praat_addAction1 (classSound, 0, U"Analyse spectrum -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"To Spectrum...", nullptr, 1, DO_Sound_to_Spectrum);
							praat_addAction1 (classSound, 0, U"To Spectrum (fft)", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_to_Spectrum_fft);
							praat_addAction1 (classSound, 0, U"To Spectrum", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_to_Spectrum_fft);
							praat_addAction1 (classSound, 0, U"To Spectrum (dft)", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_to_Spectrum_dft);
		praat_addAction1 (classSound, 0, U"To Ltas...", nullptr, 1, DO_Sound_to_Ltas);
		praat_addAction1 (classSound, 0, U"To Ltas (pitch-corrected)...", nullptr, 1, DO_Sound_to_Ltas_pitchCorrected);
		praat_addAction1 (classSound, 0, U"-- spectrotemporal --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To Spectrogram...", nullptr, 1, DO_Sound_to_Spectrogram);
		praat_addAction1 (classSound, 0, U"To Cochleagram...", nullptr, 1, DO_Sound_to_Cochleagram);
		praat_addAction1 (classSound, 0, U"To Cochleagram (edb)...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_to_Cochleagram_edb);
		praat_addAction1 (classSound, 0, U"-- formants --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To Formant (burg)...", nullptr, 1, DO_Sound_to_Formant_burg);
		praat_addAction1 (classSound, 0, U"To Formant (hack)", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To Formant (keep all)...", nullptr, 2, DO_Sound_to_Formant_keepAll);
		praat_addAction1 (classSound, 0, U"To Formant (sl)...", nullptr, 2, DO_Sound_to_Formant_willems);
	praat_addAction1 (classSound, 0, U"To Intensity...", nullptr, 0, DO_Sound_to_Intensity);
	praat_addAction1 (classSound, 0, U"To IntensityTier...", nullptr, praat_HIDDEN, DO_Sound_to_IntensityTier);
	praat_addAction1 (classSound, 0, U"Manipulate -", nullptr, 0, nullptr);
	praat_addAction1 (classSound, 0, U"To Manipulation...", nullptr, 1, DO_Sound_to_Manipulation);
	praat_addAction1 (classSound, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"Convert to mono", nullptr, 1, DO_Sound_convertToMono);
		praat_addAction1 (classSound, 0, U"Convert to stereo", nullptr, 1, DO_Sound_convertToStereo);
		praat_addAction1 (classSound, 0, U"Extract all channels", nullptr, 1, DO_Sound_extractAllChannels);
		praat_addAction1 (classSound, 0, U"Extract left channel", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_Sound_extractLeftChannel);   // deprecated 2010
		praat_addAction1 (classSound, 0, U"Extract right channel", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_Sound_extractRightChannel);   // deprecated 2010
		praat_addAction1 (classSound, 0, U"Extract one channel...", nullptr, 1, DO_Sound_extractChannel);
		praat_addAction1 (classSound, 0, U"Extract part...", nullptr, 1, DO_Sound_extractPart);
		praat_addAction1 (classSound, 0, U"Extract part for overlap...", nullptr, 1, DO_Sound_extractPartForOverlap);
		praat_addAction1 (classSound, 0, U"Resample...", nullptr, 1, DO_Sound_resample);
		praat_addAction1 (classSound, 0, U"-- enhance --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Lengthen (overlap-add)...", nullptr, 1, DO_Sound_lengthen_overlapAdd);
		praat_addAction1 (classSound, 0, U"Lengthen (PSOLA)...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_lengthen_overlapAdd);
		praat_addAction1 (classSound, 0, U"Deepen band modulation...", nullptr, 1, DO_Sound_deepenBandModulation);
		praat_addAction1 (classSound, 0, U"-- cast --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Down to Matrix", nullptr, 1, DO_Sound_to_Matrix);
	praat_addAction1 (classSound, 0, U"Filter -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"Filtering tutorial", nullptr, 1, DO_FilteringTutorial);
		praat_addAction1 (classSound, 0, U"-- frequency-domain filter --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Filter (pass Hann band)...", nullptr, 1, DO_Sound_filter_passHannBand);
		praat_addAction1 (classSound, 0, U"Filter (stop Hann band)...", nullptr, 1, DO_Sound_filter_stopHannBand);
		praat_addAction1 (classSound, 0, U"Filter (formula)...", nullptr, 1, DO_Sound_filter_formula);
		praat_addAction1 (classSound, 0, U"-- time-domain filter --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Filter (one formant)...", nullptr, 1, DO_Sound_filter_oneFormant);
		praat_addAction1 (classSound, 0, U"Filter (pre-emphasis)...", nullptr, 1, DO_Sound_filter_preemphasis);
		praat_addAction1 (classSound, 0, U"Filter (de-emphasis)...", nullptr, 1, DO_Sound_filter_deemphasis);
	praat_addAction1 (classSound, 0, U"Combine -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"Combine to stereo", nullptr, 1, DO_Sounds_combineToStereo);
		praat_addAction1 (classSound, 0, U"Concatenate", nullptr, 1, DO_Sounds_concatenate);
		praat_addAction1 (classSound, 0, U"Concatenate recoverably", nullptr, 1, DO_Sounds_concatenateRecoverably);
		praat_addAction1 (classSound, 0, U"Concatenate with overlap...", nullptr, 1, DO_Sounds_concatenateWithOverlap);
		praat_addAction1 (classSound, 2, U"Convolve", nullptr, praat_HIDDEN + praat_DEPTH_1, DO_Sounds_convolve_old);
		praat_addAction1 (classSound, 2, U"Convolve...", nullptr, 1, DO_Sounds_convolve);
		praat_addAction1 (classSound, 2, U"Cross-correlate...", nullptr, 1, DO_Sounds_crossCorrelate);
		praat_addAction1 (classSound, 2, U"To ParamCurve", nullptr, 1, DO_Sounds_to_ParamCurve);

	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as WAV file...", nullptr, 0, DO_LongSound_Sound_writeToWavFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Write to WAV file...", nullptr, praat_HIDDEN, DO_LongSound_Sound_writeToWavFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as AIFF file...", nullptr, 0, DO_LongSound_Sound_writeToAiffFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Write to AIFF file...", nullptr, praat_HIDDEN, DO_LongSound_Sound_writeToAiffFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as AIFC file...", nullptr, 0, DO_LongSound_Sound_writeToAifcFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Write to AIFC file...", nullptr, praat_HIDDEN, DO_LongSound_Sound_writeToAifcFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as NeXT/Sun file...", nullptr, 0, DO_LongSound_Sound_writeToNextSunFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Write to NeXT/Sun file...", nullptr, praat_HIDDEN, DO_LongSound_Sound_writeToNextSunFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as NIST file...", nullptr, 0, DO_LongSound_Sound_writeToNistFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Write to NIST file...", nullptr, praat_HIDDEN, DO_LongSound_Sound_writeToNistFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as FLAC file...", nullptr, 0, DO_LongSound_Sound_writeToFlacFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Write to FLAC file...", nullptr, praat_HIDDEN, DO_LongSound_Sound_writeToFlacFile);
}

/* End of file praat_Sound.cpp */
