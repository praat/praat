/* praat_Sound_init.cpp
 *
 * Copyright (C) 1992-2020 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

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
#include "SoundSet.h"
#include "SpectrumEditor.h"
#include "TextGrid_Sound.h"
#include "mp3.h"

#include "praat_Sound.h"

#undef iam
#define iam iam_LOOP

/***** LONGSOUND *****/

DIRECT (INFO_LongSound_concatenate) {
	INFO_NONE
		Melder_information (U"To concatenate LongSound objects, "
			"select them in the list\nand choose \"Save as WAV file...\" or a similar command.\n"
			"The result will be a sound file that contains\nthe concatenation of the selected sounds."
		);
	INFO_NONE_END
}

FORM (NEW_LongSound_extractPart, U"LongSound: Extract part", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"1.0")
	BOOLEAN (preserveTimes, U"Preserve times", true)
	OK
DO
	CONVERT_EACH (LongSound)
		autoSound result = LongSound_extractPart (me, fromTime, toTime, preserveTimes);
	CONVERT_EACH_END (my name.get())
}

FORM (REAL_LongSound_getIndexFromTime, U"LongSound: Get sample index from time", U"Sound: Get index from time...") {
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (LongSound)
		const double result = Sampled_xToIndex (me, time);
	NUMBER_ONE_END (U" (index at ", time, U" seconds)")
}

DIRECT (REAL_LongSound_getSamplePeriod) {
	NUMBER_ONE (LongSound)
		const double result = my dx;
	NUMBER_ONE_END (U" seconds");
}

DIRECT (REAL_LongSound_getSampleRate) {
	NUMBER_ONE (LongSound)
		const double result = 1.0 / my dx;
	NUMBER_ONE_END (U" Hz")
}

FORM (REAL_LongSound_getTimeFromIndex, U"LongSound: Get time from sample index", U"Sound: Get time from index...") {
	INTEGER (sampleIndex, U"Sample index", U"100")
	OK
DO
	NUMBER_ONE (LongSound)
		const double result = Sampled_indexToX (me, sampleIndex);
	NUMBER_ONE_END (U" seconds")
}

DIRECT (INTEGER_LongSound_getNumberOfSamples) {
	NUMBER_ONE (LongSound)
		const integer result = my nx;
	NUMBER_ONE_END (U" samples")
}

DIRECT (HELP_LongSound_help) {
	HELP (U"LongSound")
}

FORM_READ (READ1_LongSound_open, U"Open long sound file", nullptr, true) {
	READ_ONE
		autoLongSound result = LongSound_open (file);
	READ_ONE_END
}

FORM (PLAY_LongSound_playPart, U"LongSound: Play part", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"10.0")
	OK
DO
	int n = 0;
	LOOP n ++;
	if (n == 1 || MelderAudio_getOutputMaximumAsynchronicity () < kMelder_asynchronicityLevel::ASYNCHRONOUS) {
		LOOP {
			iam (LongSound);
			LongSound_playPart (me, fromTime, toTime, nullptr, nullptr);
		}
	} else {
		MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel::INTERRUPTABLE);
		LOOP {
			iam (LongSound);
			LongSound_playPart (me, fromTime, toTime, nullptr, nullptr);
		}
		MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel::ASYNCHRONOUS);
	}
END }

FORM (SAVE_LongSound_savePartAsAudioFile, U"LongSound: Save part as audio file", nullptr) {
	TEXTFIELD (audioFile, U"Audio file:", U"")
	RADIO (type, U"Type", 3)
	{ int i; for (i = 1; i <= Melder_NUMBER_OF_AUDIO_FILE_TYPES; i ++) {
		RADIOBUTTON (Melder_audioFileTypeString (i))
	}}
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"10.0")
	OK
DO
	SAVE_ONE (LongSound)
		structMelderFile file { };
		Melder_relativePathToFile (audioFile, & file);
		LongSound_savePartAsAudioFile (me, type, fromTime, toTime, & file, 16);
	SAVE_ONE_END
}

FORM (NEW_LongSound_to_TextGrid, U"LongSound: To TextGrid...", U"LongSound: To TextGrid...") {
	SENTENCE (tierNames, U"Tier names", U"Mary John bell")
	SENTENCE (pointTiers, U"Point tiers", U"bell")
	OK
DO
	CONVERT_EACH (LongSound)
		autoTextGrid result = TextGrid_create (my xmin, my xmax, tierNames, pointTiers);
	CONVERT_EACH_END (my name.get())
}

DIRECT (WINDOW_LongSound_view) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a LongSound from batch.");
	FIND_ONE_WITH_IOBJECT (LongSound)
		autoSoundEditor editor = SoundEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

FORM_SAVE (SAVE_LongSound_saveAsAifcFile, U"Save as AIFC file", nullptr, U"aifc") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_AIFC, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_LongSound_saveAsAiffFile, U"Save as AIFF file", nullptr, U"aiff") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_AIFF, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_LongSound_saveAsNextSunFile, U"Save as NeXT/Sun file", nullptr, U"au") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_NEXT_SUN, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_LongSound_saveAsNistFile, U"Save as NIST file", nullptr, U"nist") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_NIST, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_LongSound_saveAsFlacFile, U"Save as FLAC file", nullptr, U"flac") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_FLAC, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_LongSound_saveAsWavFile, U"Save as WAV file", nullptr, U"wav") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_WAV, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_LongSound_saveLeftChannelAsAifcFile, U"Save left channel as AIFC file", nullptr, U"aifc") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_AIFC, 0, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_LongSound_saveLeftChannelAsAiffFile, U"Save left channel as AIFF file", nullptr, U"aiff") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_AIFF, 0, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_LongSound_saveLeftChannelAsNextSunFile, U"Save left channel as NeXT/Sun file", nullptr, U"au") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_NEXT_SUN, 0, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_LongSound_saveLeftChannelAsNistFile, U"Save left channel as NIST file", nullptr, U"nist") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_NIST, 0, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_LongSound_saveLeftChannelAsFlacFile, U"Save left channel as FLAC file", nullptr, U"flac") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_FLAC, 0, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_LongSound_saveLeftChannelAsWavFile, U"Save left channel as WAV file", nullptr, U"wav") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_WAV, 0, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_LongSound_saveRightChannelAsAifcFile, U"Save right channel as AIFC file", nullptr, U"aifc") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_AIFC, 1, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_LongSound_saveRightChannelAsAiffFile, U"Save right channel as AIFF file", nullptr, U"aiff") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_AIFF, 1, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_LongSound_saveRightChannelAsNextSunFile, U"Save right channel as NeXT/Sun file", nullptr, U"au") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_NEXT_SUN, 1, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_LongSound_saveRightChannelAsNistFile, U"Save right channel as NIST file", nullptr, U"nist") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_NIST, 1, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_LongSound_saveRightChannelAsFlacFile, U"Save right channel as FLAC file", 0, U"flac") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_FLAC, 1, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_LongSound_saveRightChannelAsWavFile, U"Save right channel as WAV file", 0, U"wav") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_WAV, 1, file);
	SAVE_ONE_END
}

FORM (PREFS_LongSoundPrefs, U"LongSound preferences", U"LongSound") {
	LABEL (U"This setting determines the maximum number of seconds")
	LABEL (U"for viewing the waveform and playing a sound in the LongSound window.")
	LABEL (U"The LongSound window can become very slow if you set it too high.")
	NATURAL (maximumViewablePart, U"Maximum viewable part (seconds)", U"60")
	LABEL (U"Note: this setting works for the next long sound file that you open,")
	LABEL (U"not for currently existing LongSound objects.")
OK
	SET_INTEGER (maximumViewablePart, LongSound_getBufferSizePref_seconds ())
DO
	LongSound_setBufferSizePref_seconds (maximumViewablePart);
END }

/********** LONGSOUND & SOUND **********/

FORM_SAVE (SAVE_LongSound_Sound_saveAsAifcFile, U"Save as AIFC file", nullptr, U"aifc") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_AIFC, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_LongSound_Sound_saveAsAiffFile, U"Save as AIFF file", nullptr, U"aiff") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_AIFF, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_LongSound_Sound_saveAsNextSunFile, U"Save as NeXT/Sun file", nullptr, U"au") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_NEXT_SUN, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_LongSound_Sound_saveAsNistFile, U"Save as NIST file", nullptr, U"nist") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_NIST, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_LongSound_Sound_saveAsFlacFile, U"Save as FLAC file", nullptr, U"flac") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_FLAC, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_LongSound_Sound_saveAsWavFile, U"Save as WAV file", nullptr, U"wav") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_WAV, 16);
	SAVE_TYPED_LIST_END
}

/********** SOUND **********/

FORM (MODIFY_Sound_add, U"Sound: Add", nullptr) {
	LABEL (U"The following number will be added to the amplitudes of ")
	LABEL (U"all samples of the sound.")
	REAL (number, U"Number", U"0.1")
	OK
DO
	MODIFY_EACH (Sound)
		Vector_addScalar (me, number);
	MODIFY_EACH_END
}

FORM (NEW_Sound_autoCorrelate, U"Sound: autocorrelate", U"Sound: Autocorrelate...") {
	RADIO_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	RADIO_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
 	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_autoCorrelate (me, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_EACH_END (U"ac_", my name.get())
}

DIRECT (NEW1_Sounds_combineToStereo) {
	CONVERT_LIST (Sound)
		autoSound result = Sounds_combineToStereo (& list);
		const integer numberOfChannels = result -> ny;   // dereference before transferring
	CONVERT_LIST_END (U"combined_", numberOfChannels)
}

DIRECT (NEW1_Sounds_combineIntoSoundList) {
	CONVERT_LIST (Sound)
		autoSoundList result = SoundList_create ();
		for (integer iobject = 1; iobject <= list.size; iobject ++)
			result -> addItem_move (Data_copy (list.at [iobject]));
	CONVERT_LIST_END (U"list")
}

DIRECT (NEW1_Sounds_combineIntoSoundSet) {
	CONVERT_LIST (Sound)
		autoSoundSet result = SoundSet_create ();
		for (integer iobject = 1; iobject <= list.size; iobject ++)
			result -> addItem_move (Data_copy (list.at [iobject]));
	CONVERT_LIST_END (U"ensemble")
}

DIRECT (NEW1_Sounds_concatenate) {
	CONVERT_LIST (Sound)
		autoSound result = Sounds_concatenate (list, 0.0);
	CONVERT_LIST_END (U"chain")
}

FORM (NEW1_Sounds_concatenateWithOverlap, U"Sounds: Concatenate with overlap", U"Sounds: Concatenate with overlap...") {
	POSITIVE (overlap, U"Overlap (s)", U"0.01")
	OK
DO
	CONVERT_LIST (Sound)
		autoSound result = Sounds_concatenate (list, overlap);
	CONVERT_LIST_END (U"chain")
}

DIRECT (NEW2_Sounds_concatenateRecoverably) {
	integer numberOfChannels = 0, nx = 0, iinterval = 0;
	double dx = 0.0, tmin = 0.0;
	LOOP {
		iam (Sound);
		if (numberOfChannels == 0) {
			numberOfChannels = my ny;
		} else if (my ny != numberOfChannels) {
			Melder_throw (U"To concatenate sounds, their numbers of channels (mono, stereo) should be equal.");
		}
		if (dx == 0.0) {
			dx = my dx;
		} else if (my dx != dx) {
			Melder_throw (U"To concatenate sounds, their sampling frequencies should be equal.\n"
					"You could resample one or more of the sounds before concatenating.");
		}
		nx += my nx;
	}
	autoSound thee = Sound_create (numberOfChannels, 0.0, nx * dx, nx, dx, 0.5 * dx);
	autoTextGrid him = TextGrid_create (0.0, nx * dx, U"labels", U"");
	nx = 0;
	LOOP {
		iam (Sound);
		const double tmax = tmin + my nx * dx;
		thy z.verticalBand (nx + 1, nx + my nx) <<= my z.all();
		iinterval ++;
		if (iinterval > 1)
			TextGrid_insertBoundary (him.get(), 1, tmin);
		TextGrid_setIntervalText (him.get(), 1, iinterval, my name.get());
		nx += my nx;
		tmin = tmax;
	}
	praat_new (thee.move(), U"chain");
	praat_new (him.move(), U"chain");
	END
}

DIRECT (NEW_Sound_convertToMono) {
	CONVERT_EACH (Sound)
		autoSound result = Sound_convertToMono (me);
	CONVERT_EACH_END (my name.get(), U"_mono")
}

DIRECT (NEW_Sound_convertToStereo) {
	CONVERT_EACH (Sound)
		autoSound result = Sound_convertToStereo (me);
	CONVERT_EACH_END (my name.get(), U"_stereo")
}

DIRECT (NEW1_Sounds_convolve_old) {
	CONVERT_COUPLE (Sound)
		autoSound result = Sounds_convolve (me, you,
			kSounds_convolve_scaling::SUM, kSounds_convolve_signalOutsideTimeDomain::ZERO);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

FORM (NEW1_Sounds_convolve, U"Sounds: Convolve", U"Sounds: Convolve...") {
	RADIO_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	RADIO_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO
	CONVERT_COUPLE (Sound)
		autoSound result = Sounds_convolve (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_COUPLE_END (my name.get(), U"_", your name.get())
}

static void common_Sound_create (conststring32 name, integer numberOfChannels, double startTime, double endTime,
	double samplingFrequency, conststring32 formula, Interpreter interpreter)
{
	const double numberOfSamples_real = round ((endTime - startTime) * samplingFrequency);
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
	const integer numberOfSamples = (int64) numberOfSamples_real;
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
	Matrix_formula (sound.get(), formula, interpreter, nullptr);
	praat_new (sound.move(), name);
	//praat_updateSelection ();
}

FORM (NEW1_Sound_create, U"Create mono Sound", U"Create Sound from formula...") {
	WORD (name, U"Name", U"sineWithNoise")
	REAL (startTime, U"Start time (s)", U"0.0")
	REAL (endTime, U"End time (s)", U"1.0")
	REAL (samplingFrequency, U"Sampling frequency (Hz)", U"44100")
	TEXTFIELD (formula, U"Formula:", U"1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)")
	OK
DO
	common_Sound_create (name, 1, startTime, endTime, samplingFrequency, formula, interpreter);
END }

FORM (NEW1_Sound_createFromFormula, U"Create Sound from formula", U"Create Sound from formula...") {
	WORD (name, U"Name", U"sineWithNoise")
	CHANNEL (numberOfChannels, U"Number of channels", U"1 (= mono)")
	REAL (startTime, U"Start time (s)", U"0.0")
	REAL (endTime, U"End time (s)", U"1.0")
	REAL (samplingFrequency, U"Sampling frequency (Hz)", U"44100")
	TEXTFIELD (formula, U"Formula:", U"1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)")
	OK
DO
	common_Sound_create (name, numberOfChannels, startTime, endTime, samplingFrequency, formula, interpreter);
END }

FORM (NEW1_Sound_createAsPureTone, U"Create Sound as pure tone", U"Create Sound as pure tone...") {
	WORD (name, U"Name", U"tone")
	CHANNEL (numberOfChannels, U"Number of channels", U"1 (= mono)")
	REAL (startTime, U"Start time (s)", U"0.0")
	REAL (endTime, U"End time (s)", U"0.4")
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	POSITIVE (toneFrequency, U"Tone frequency (Hz)", U"440.0")
	POSITIVE (amplitude, U"Amplitude (Pa)", U"0.2")
	POSITIVE (fadeInDuration, U"Fade-in duration (s)", U"0.01")
	POSITIVE (fadeOutDuration, U"Fade-out duration (s)", U"0.01")
	OK
DO
	CREATE_ONE
		autoSound result = Sound_createAsPureTone (numberOfChannels, startTime, endTime,
				samplingFrequency, toneFrequency, amplitude, fadeInDuration, fadeOutDuration);
	CREATE_ONE_END (name)
}

FORM (NEW1_Sound_createAsToneComplex, U"Create Sound as tone complex", U"Create Sound as tone complex...") {
	WORD (name, U"Name", U"toneComplex")
	REAL (startTime, U"Start time (s)", U"0.0")
	REAL (endTime, U"End time (s)", U"1.0")
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	RADIOx (phase, U"Phase", 2, Sound_TONE_COMPLEX_SINE)
		RADIOBUTTON (U"sine")
		RADIOBUTTON (U"cosine")
	POSITIVE (frequencyStep, U"Frequency step (Hz)", U"100.0")
	REAL (firstFrequency, U"First frequency (Hz)", U"0.0 (= frequency step)")
	REAL (ceiling, U"Ceiling (Hz)", U"0.0 (= Nyquist)")
	INTEGER (numberOfComponents, U"Number of components", U"0 (= maximum)")
	OK
DO
	CREATE_ONE
		autoSound result = Sound_createAsToneComplex (startTime, endTime,
				samplingFrequency, phase, frequencyStep, firstFrequency, ceiling, numberOfComponents);
	CREATE_ONE_END (name)
}

FORM (NEW1_old_Sounds_crossCorrelate, U"Cross-correlate (short)", nullptr) {
	REAL (fromLag, U"From lag (s)", U"-0.1")
	REAL (toLag, U"To lag (s)", U"0.1")
	BOOLEAN (normalize, U"Normalize", true)
	OK
DO
	CONVERT_COUPLE (Sound)
		autoSound result = Sounds_crossCorrelate_short (me, you, fromLag, toLag, normalize);
	CONVERT_COUPLE_END (U"cc_", my name.get(), U"_", your name.get())
}

FORM (NEW1_Sounds_crossCorrelate, U"Sounds: Cross-correlate", U"Sounds: Cross-correlate...") {
	RADIO_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	RADIO_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO_ALTERNATIVE (NEW1_old_Sounds_crossCorrelate)
	CONVERT_COUPLE (Sound)
		autoSound result = Sounds_crossCorrelate (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_COUPLE_END (U"cc_", my name.get(), U"_", your name.get())
}

FORM (MODIFY_Sound_deemphasizeInplace, U"Sound: De-emphasize (in-place)", U"Sound: De-emphasize (in-place)...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"50.0")
	OK
DO
	MODIFY_EACH (Sound)
		Sound_deEmphasis (me, fromFrequency);
		Vector_scale (me, 0.99);
	MODIFY_EACH_END
}

FORM (NEW_Sound_deepenBandModulation, U"Deepen band modulation", U"Sound: Deepen band modulation...") {
	POSITIVE (enhancement, U"Enhancement (dB)", U"20.0")
	POSITIVE (fromFrequency, U"From frequency (Hz)", U"300.0")
	POSITIVE (toFrequency, U"To frequency (Hz)", U"8000.0")
	POSITIVE (slowModulation, U"Slow modulation (Hz)", U"3.0")
	POSITIVE (fastModulation, U"Fast modulation (Hz)", U"30.0")
	POSITIVE (bandSmoothing, U"Band smoothing (Hz)", U"100.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_deepenBandModulation (me, enhancement, fromFrequency, toFrequency,
				slowModulation, fastModulation, bandSmoothing);
	CONVERT_EACH_END (my name.get(), U"_", Melder_roundTowardsZero (enhancement))
}

FORM (GRAPHICS_old_Sound_draw, U"Sound: Draw", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range", U"0.0 (= all)")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0 (= auto)")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Sound)
		Sound_draw (me, GRAPHICS, fromTime, toTime,
				fromAmplitude, toAmplitude, garnish, U"curve");
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Sound_draw, U"Sound: Draw", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range", U"0.0 (= all)")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0 (= auto)")
	BOOLEAN (garnish, U"Garnish", true)
	LABEL (U"")
	OPTIONMENUSTR (drawingMethod, U"Drawing method", 1)
		OPTION (U"curve")
		OPTION (U"bars")
		OPTION (U"poles")
		OPTION (U"speckles")
	OK
DO_ALTERNATIVE (GRAPHICS_old_Sound_draw)
	GRAPHICS_EACH (Sound)
		Sound_draw (me, GRAPHICS, fromTime, toTime,
				fromAmplitude, toAmplitude, garnish, drawingMethod);
	GRAPHICS_EACH_END
}

static void cb_SoundEditor_publication (Editor /* me */, autoDaata publication) {
	/*
	 * Keep the gate for error handling.
	 */
	try {
		const bool isaSpectrum = Thing_isa (publication.get(), classSpectrum);
		praat_new (publication.move(), U"");
		praat_updateSelection ();
		if (isaSpectrum) {
			int IOBJECT;
			FIND_ONE_WITH_IOBJECT (Spectrum)
			autoSpectrumEditor editor2 = SpectrumEditor_create (ID_AND_FULL_NAME, me);
			praat_installEditor (editor2.get(), IOBJECT);
			editor2.releaseToUser();
		}
	} catch (MelderError) {
		Melder_flushError ();
	}
}
DIRECT (WINDOW_Sound_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit a Sound from batch.");
	FIND_ONE_WITH_IOBJECT (Sound)
		autoSoundEditor editor = SoundEditor_create (ID_AND_FULL_NAME, me);
		Editor_setPublicationCallback (editor.get(), cb_SoundEditor_publication);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

DIRECT (NEWMANY_Sound_extractAllChannels) {
	LOOP {
		iam (Sound);
		for (integer channel = 1; channel <= my ny; channel ++) {
			autoSound thee = Sound_extractChannel (me, channel);
			praat_new (thee.move(), my name.get(), U"_ch", channel);
		}
	}
	END
}

FORM (NEW_Sound_extractChannel, U"Sound: Extract channel", nullptr) {
	CHANNEL (channel, U"Channel (number, Left, or Right)", U"1")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_extractChannel (me, channel);
	CONVERT_EACH_END (my name.get(), U"_ch", channel)
}

FORM (NEW_Sound_extractChannels, U"Sound: Extract channels", nullptr) {
	NUMVEC (channels, U"Channel numbers:", U"to# (64)")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_extractChannels (me, channels);
	CONVERT_EACH_END (my name.get(), U"_ch")
}

DIRECT (NEW_Sound_extractLeftChannel) {
	CONVERT_EACH (Sound)
		autoSound result = Sound_extractChannel (me, 1);
	CONVERT_EACH_END (my name.get(), U"_left")
}

FORM (NEW_Sound_extractPart, U"Sound: Extract part", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.1")
	OPTIONMENU_ENUM (kSound_windowShape, windowShape, U"Window shape", kSound_windowShape::DEFAULT)
	POSITIVE (relativeWidth, U"Relative width", U"1.0")
	BOOLEAN (preserveTimes, U"Preserve times", false)
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_extractPart (me, fromTime, toTime,
				windowShape, relativeWidth, preserveTimes);
	CONVERT_EACH_END (my name.get(), U"_part")
}

FORM (NEW_Sound_extractPartForOverlap, U"Sound: Extract part for overlap", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.1")
	POSITIVE (overlap, U"Overlap (s)", U"0.01")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_extractPartForOverlap (me, fromTime, toTime, overlap);
	CONVERT_EACH_END (my name.get(), U"_part")
}

DIRECT (NEW_Sound_extractRightChannel) {
	CONVERT_EACH (Sound)
		autoSound result = Sound_extractChannel (me, 2);
	CONVERT_EACH_END (my name.get(), U"_right")
}

FORM (NEW_Sound_filter_deemphasis, U"Sound: Filter (de-emphasis)", U"Sound: Filter (de-emphasis)...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_filter_deemphasis (me, fromFrequency);
	CONVERT_EACH_END (my name.get(), U"_deemp")
}

FORM (NEW_Sound_filter_formula, U"Sound: Filter (formula)...", U"Formula...") {
	LABEL (U"Frequency-domain filtering with a formula (uses Sound-to-Spectrum and Spectrum-to-Sound): x is frequency in hertz")
	TEXTFIELD (formula, U"Formula:", U"if x<500 or x>1000 then 0 else self fi; rectangular band filter")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_filter_formula (me, formula, interpreter);
	CONVERT_EACH_END (my name.get(), U"_filt")
}

FORM (NEW_Sound_filter_oneFormant, U"Sound: Filter (one formant)", U"Sound: Filter (one formant)...") {
	REAL (frequency, U"Frequency (Hz)", U"1000.0")
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"100.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_filter_oneFormant (me, frequency, bandwidth);
	CONVERT_EACH_END (my name.get(), U"_filt")
}

FORM (MODIFY_Sound_filterWithOneFormantInplace, U"Sound: Filter with one formant (in-place)", U"Sound: Filter with one formant (in-place)...") {
	REAL (frequency, U"Frequency (Hz)", U"1000.0")
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"100.0")
	OK
DO
	MODIFY_EACH (Sound)
		Sound_filterWithOneFormantInplace (me, frequency, bandwidth);
	MODIFY_EACH_END
}

FORM (NEW_Sound_filter_passHannBand, U"Sound: Filter (pass Hann band)", U"Sound: Filter (pass Hann band)...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"500.0")
	REAL (toFrequency, U"To frequency (Hz)", U"1000.0")
	POSITIVE (smoothing, U"Smoothing (Hz)", U"100.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_filter_passHannBand (me, fromFrequency, toFrequency, smoothing);
	CONVERT_EACH_END (my name.get(), U"_band")
}

FORM (NEW_Sound_filter_preemphasis, U"Sound: Filter (pre-emphasis)", U"Sound: Filter (pre-emphasis)...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_filter_preemphasis (me, fromFrequency);
	CONVERT_EACH_END (my name.get(), U"_preemp")
}

FORM (NEW_Sound_filter_stopHannBand, U"Sound: Filter (stop Hann band)", U"Sound: Filter (stop Hann band)...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"500.0")
	REAL (toFrequency, U"To frequency (Hz)", U"1000.0")
	POSITIVE (smoothing, U"Smoothing (Hz)", U"100.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_filter_stopHannBand (me, fromFrequency, toFrequency, smoothing);
	CONVERT_EACH_END (my name.get(), U"_band")
}

FORM (MODIFY_Sound_formula, U"Sound: Formula", U"Sound: Formula...") {
	LABEL (U"# `x` is the time in seconds, `col` is the sample number.")
	LABEL (U"x = x1   ! time associated with first sample")
	LABEL (U"for col from 1 to ncol")
	LABEL (U"   self [col] = ...")
	TEXTFIELD (formula, nullptr, U"self")
	LABEL (U"   x = x + dx")
	LABEL (U"endfor")
	OK
DO
	MODIFY_EACH_WEAK (Sound)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_Sound_formula_part, U"Sound: Formula (part)", U"Sound: Formula...") {
	REAL (fromTime, U"From time", U"0.0")
	REAL (toTime, U"To time", U"0.0 (= all)")
	NATURAL (fromChannel, U"From channel", U"1")
	NATURAL (toChannel, U"To channel", U"2")
	TEXTFIELD (formula, U"Formula:", U"2 * self")
	OK
DO
	MODIFY_EACH_WEAK (Sound)
		Matrix_formula_part (me, fromTime, toTime, fromChannel - 0.5, toChannel + 0.5,
				formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (REAL_Sound_getAbsoluteExtremum, U"Sound: Get absolute extremum", U"Sound: Get absolute extremum...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	RADIO_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation::SINC70)
	OK
DO
	NUMBER_ONE (Sound)
		const double result = Vector_getAbsoluteExtremum (me, fromTime, toTime, peakInterpolationType);
	NUMBER_ONE_END (U" Pascal")
}

FORM (REAL_Sound_getEnergy, U"Sound: Get energy", U"Sound: Get energy...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO
	NUMBER_ONE (Sound)
		const double result = Sound_getEnergy (me, fromTime, toTime);
	NUMBER_ONE_END (U" Pa2 sec")
}

DIRECT (REAL_Sound_getEnergyInAir) {
	NUMBER_ONE (Sound)
		const double result = Sound_getEnergyInAir (me);
	NUMBER_ONE_END (U" Joule/m2")
}

FORM (REAL_Sound_getIndexFromTime, U"Get sample number from time", U"Get sample number from time...") {
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (Sound)
		const double result = Sampled_xToIndex (me, time);
	NUMBER_ONE_END (U" (index at time ", time, U" seconds)")
}

DIRECT (REAL_Sound_getIntensity_dB) {
	NUMBER_ONE (Sound)
		const double result = Sound_getIntensity_dB (me);
	NUMBER_ONE_END (U" dB")
}

FORM (REAL_Sound_getMaximum, U"Sound: Get maximum", U"Sound: Get maximum...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	RADIO_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation::SINC70)
	OK
DO
	NUMBER_ONE (Sound)
		const double result = Vector_getMaximum (me, fromTime, toTime, peakInterpolationType);
	NUMBER_ONE_END (U" Pascal")
}

FORM (REAL_old_Sound_getMean, U"Sound: Get mean", U"Sound: Get mean...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO
	NUMBER_ONE (Sound)
		const double result = Vector_getMean (me, fromTime, toTime, Vector_CHANNEL_AVERAGE);
	NUMBER_ONE_END (U" Pascal")
}

FORM (REAL_Sound_getMean, U"Sound: Get mean", U"Sound: Get mean...") {
	CHANNEL (channel, U"Channel", U"0 (= all)")
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO_ALTERNATIVE (REAL_old_Sound_getMean)
	NUMBER_ONE (Sound)
		if (channel > my ny) channel = 1;
		const double result = Vector_getMean (me, fromTime, toTime, channel);
	NUMBER_ONE_END (U" Pascal")
}

FORM (REAL_Sound_getMinimum, U"Sound: Get minimum", U"Sound: Get minimum...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	RADIO_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation::SINC70)
	OK
DO
	NUMBER_ONE (Sound)
		const double result = Vector_getMinimum (me, fromTime, toTime, peakInterpolationType);
	NUMBER_ONE_END (U" Pascal")
}

FORM (REAL_old_Sound_getNearestZeroCrossing, U"Sound: Get nearest zero crossing", U"Sound: Get nearest zero crossing...") {
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	NUMBER_ONE (Sound)
		if (my ny > 1) Melder_throw (U"Cannot determine a zero crossing for a stereo sound.");
		const double result = Sound_getNearestZeroCrossing (me, time, 1);
	NUMBER_ONE_END (U" seconds")
}

FORM (REAL_Sound_getNearestZeroCrossing, U"Sound: Get nearest zero crossing", U"Sound: Get nearest zero crossing...") {
	CHANNEL (channel, U"Channel (number, Left, or Right)", U"1")
	REAL (time, U"Time (s)", U"0.5")
	OK
DO_ALTERNATIVE (REAL_old_Sound_getNearestZeroCrossing)
	NUMBER_ONE (Sound)
		if (channel > my ny) channel = 1;
		const double result = Sound_getNearestZeroCrossing (me, time, channel);
	NUMBER_ONE_END (U" seconds")
}

DIRECT (INTEGER_Sound_getNumberOfChannels) {
	NUMBER_ONE (Sound)
		const integer result = my ny;
	NUMBER_ONE_END (result == 1 ? U" channel (mono)" : result == 2 ? U" channels (stereo)" : U" channels")
}

DIRECT (INTEGER_Sound_getNumberOfSamples) {
	NUMBER_ONE (Sound)
		const integer result = my nx;
	NUMBER_ONE_END (U" samples")
}

FORM (REAL_Sound_getPower, U"Sound: Get power", U"Sound: Get power...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO
	NUMBER_ONE (Sound)
		const double result = Sound_getPower (me, fromTime, toTime);
	NUMBER_ONE_END (U" Pa2")
}

DIRECT (REAL_Sound_getPowerInAir) {
	NUMBER_ONE (Sound)
		const double result = Sound_getPowerInAir (me);
	NUMBER_ONE_END (U" Watt/m2")
}

FORM (REAL_Sound_getRootMeanSquare, U"Sound: Get root-mean-square", U"Sound: Get root-mean-square...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO
	NUMBER_ONE (Sound)
		const double result = Sound_getRootMeanSquare (me, fromTime, toTime);
	NUMBER_ONE_END (U" Pascal")
}

DIRECT (REAL_Sound_getSamplePeriod) {
	NUMBER_ONE (Sound)
		const double result = my dx;
	NUMBER_ONE_END (U" seconds")
}

DIRECT (REAL_Sound_getSampleRate) {
	NUMBER_ONE (Sound)
		const double result = 1.0 / my dx;
	NUMBER_ONE_END (U" Hz")
}

FORM (REAL_old_Sound_getStandardDeviation, U"Sound: Get standard deviation", U"Sound: Get standard deviation...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO
	NUMBER_ONE (Sound)
		const double result = Vector_getStandardDeviation (me, fromTime, toTime, Vector_CHANNEL_AVERAGE);
	NUMBER_ONE_END (U" Pascal")
}

FORM (REAL_Sound_getStandardDeviation, U"Sound: Get standard deviation", U"Sound: Get standard deviation...") {
	CHANNEL (channel, U"Channel", U"0 (= average)")
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO_ALTERNATIVE (REAL_old_Sound_getStandardDeviation)
	NUMBER_ONE (Sound)
		if (channel > my ny) channel = 1;
		const double result = Vector_getStandardDeviation (me, fromTime, toTime, channel);
	NUMBER_ONE_END (U" Pascal")
}

FORM (REAL_Sound_getTimeFromIndex, U"Get time from sample number", U"Get time from sample number...") {
	INTEGER (sampleNumber, U"Sample number", U"100")
	OK
DO
	NUMBER_ONE (Sound)
		const double result = Sampled_indexToX (me, sampleNumber);
	NUMBER_ONE_END (U" seconds")
}

DIRECT (NUMVEC_Sound_listAllSampleTimes) {
	NUMVEC_ONE (Sound)
		autoVEC result = Sampled_listAllXValues (me);
	NUMVEC_ONE_END
}

FORM (REAL_Sound_getTimeOfMaximum, U"Sound: Get time of maximum", U"Sound: Get time of maximum...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	RADIO_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation::SINC70)
	OK
DO
	NUMBER_ONE (Sound)
		const double result = Vector_getXOfMaximum (me, fromTime, toTime, peakInterpolationType);
	NUMBER_ONE_END (U" seconds")
}

FORM (REAL_Sound_getTimeOfMinimum, U"Sound: Get time of minimum", U"Sound: Get time of minimum...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	RADIO_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation::SINC70)
	OK
DO
	NUMBER_ONE (Sound)
		const double result = Vector_getXOfMinimum (me, fromTime, toTime, peakInterpolationType);
	NUMBER_ONE_END (U" seconds")
}

FORM (REAL_old_Sound_getValueAtIndex, U"Sound: Get value at sample number", U"Sound: Get value at sample number...") {
	INTEGER (sampleNumber, U"Sample number", U"100")
	OK
DO
	NUMBER_ONE (Sound)
		const double result = sampleNumber < 1 || sampleNumber > my nx ? undefined :
				my ny == 1 ? my z [1] [sampleNumber] : 0.5 * (my z [1] [sampleNumber] + my z [2] [sampleNumber]);
	NUMBER_ONE_END (U" Pascal")
}

FORM (REAL_Sound_getValueAtIndex, U"Sound: Get value at sample number", U"Sound: Get value at sample number...") {
	CHANNEL (channel, U"Channel", U"0 (= average)")
	INTEGER (sampleNumber, U"Sample number", U"100")
	OK
DO_ALTERNATIVE (REAL_old_Sound_getValueAtIndex)
	NUMBER_ONE (Sound)
		if (channel > my ny) channel = 1;
		const double result = sampleNumber < 1 || sampleNumber > my nx ? undefined :
				Sampled_getValueAtSample (me, sampleNumber, channel, 0);
	NUMBER_ONE_END (U" Pascal")
}

FORM (REAL_old_Sound_getValueAtTime, U"Sound: Get value at time", U"Sound: Get value at time...") {
	REAL (time, U"Time (s)", U"0.5")
	RADIO_ENUM (kVector_valueInterpolation, valueInterpolationType,
			U"Interpolation", kVector_valueInterpolation::SINC70)
	OK
DO
	NUMBER_ONE (Sound)
		const double result = Vector_getValueAtX (me, time, Vector_CHANNEL_AVERAGE, valueInterpolationType);
	NUMBER_ONE_END (U" Pascal")
}

FORM (REAL_Sound_getValueAtTime, U"Sound: Get value at time", U"Sound: Get value at time...") {
	CHANNEL (channel, U"Channel", U"0 (= average)")
	REAL (time, U"Time (s)", U"0.5")
	RADIO_ENUM (kVector_valueInterpolation, valueInterpolationType,
			U"Interpolation", kVector_valueInterpolation::SINC70)
	OK
DO_ALTERNATIVE (REAL_old_Sound_getValueAtTime)
	NUMBER_ONE (Sound)
		if (channel > my ny) channel = 1;
		const double result = Vector_getValueAtX (me, time, channel, valueInterpolationType);
	NUMBER_ONE_END (U" Pascal")
}

DIRECT (HELP_Sound_help) {
	HELP (U"Sound")
}

FORM (NEW_Sound_lengthen_overlapAdd, U"Sound: Lengthen (overlap-add)", U"Sound: Lengthen (overlap-add)...") {
	POSITIVE (minimumPitch, U"Minimum pitch (Hz)", U"75.0")
	POSITIVE (maximumPitch, U"Maximum pitch (Hz)", U"600.0")
	POSITIVE (factor, U"Factor", U"1.5")
	OK
DO
	if (minimumPitch >= maximumPitch) Melder_throw (U"Maximum pitch should be greater than minimum pitch.");
	CONVERT_EACH (Sound)
		autoSound result = Sound_lengthen_overlapAdd (me, minimumPitch, maximumPitch, factor);
	CONVERT_EACH_END (my name.get(), U"_", Melder_fixed (factor, 2));
}

FORM (MODIFY_Sound_multiply, U"Sound: Multiply", nullptr) {
	REAL (multiplicationFactor, U"Multiplication factor", U"1.5")
	OK
DO
	MODIFY_EACH (Sound)
		Vector_multiplyByScalar (me, multiplicationFactor);
	MODIFY_EACH_END
}

FORM (MODIFY_Sound_multiplyByWindow, U"Sound: Multiply by window", nullptr) {
	OPTIONMENU_ENUM (kSound_windowShape, windowShape, U"Window shape", kSound_windowShape::HANNING)
	OK
DO
	MODIFY_EACH (Sound)
		Sound_multiplyByWindow (me, windowShape);
	MODIFY_EACH_END
}

FORM (MODIFY_Sound_overrideSamplingFrequency, U"Sound: Override sampling frequency", nullptr) {
	POSITIVE (newSamplingFrequency, U"New sampling frequency (Hz)", U"16000.0")
	OK
DO
	MODIFY_EACH (Sound)
		Sound_overrideSamplingFrequency (me, newSamplingFrequency);
	MODIFY_EACH_END
}

DIRECT (PLAY_Sound_play) {
	int n = 0;
	LOOP {
		n ++;
	}
	if (n == 1 || MelderAudio_getOutputMaximumAsynchronicity () < kMelder_asynchronicityLevel::ASYNCHRONOUS) {
		LOOP {
			iam (Sound);
			Sound_play (me, nullptr, nullptr);
		}
	} else {
		MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel::INTERRUPTABLE);
		LOOP {
			iam (Sound);
			Sound_play (me, nullptr, nullptr);   // BUG: exception-safe?
		}
		MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel::ASYNCHRONOUS);
	}
END }

FORM (MODIFY_Sound_preemphasizeInplace, U"Sound: Pre-emphasize (in-place)", U"Sound: Pre-emphasize (in-place)...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"50.0")
	OK
DO
	MODIFY_EACH (Sound)
		Sound_preEmphasis (me, fromFrequency);
		Vector_scale (me, 0.99);
	MODIFY_EACH_END
}

FORM_READ (READMANY_Sound_readSeparateChannelsFromSoundFile, U"Read separate channels from sound file", nullptr, true) {
	autoSound sound = Sound_readFromSoundFile (file);
	char32 name [300];
	Melder_sprint (name,300, MelderFile_name (file));
	char32 *lastPeriod = str32rchr (name, U'.');
	if (lastPeriod) {
		*lastPeriod = U'\0';
	}
	for (integer ichan = 1; ichan <= sound -> ny; ichan ++) {
		autoSound thee = Sound_extractChannel (sound.get(), ichan);
		praat_new (thee.move(), name, U"_ch", ichan);
	}
	END
}

FORM_READ (READ1_Sound_readFromRawAlawFile, U"Read Sound from raw Alaw file", nullptr, true) {
	READ_ONE
		autoSound result = Sound_readFromRawAlawFile (file);
	READ_ONE_END
}

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
DIRECT (WINDOW_Sound_recordMono) {
	do_Sound_record (1);
END }
DIRECT (WINDOW_Sound_recordStereo) {
	do_Sound_record (2);
END }

FORM (RECORD1_Sound_record_fixedTime, U"Record Sound", nullptr) {
	LABEL (U"This menu command is usually hidden,")
	LABEL (U"   because its behaviour is platform-dependent.")
	LABEL (U"The combination of “microphone” and “44100 Hz” is likely")
	LABEL (U"   to work on all computers.")
	LABEL (U"The “Gain” and “Balance” settings tend to be obsolete")
	LABEL (U"   and may not work at all on your computer.")
	RADIO (inputSource, U"Input source", 1)
		OPTION (U"microphone")
		OPTION (U"line")
	REAL (gain, U"Gain (0-1)", U"1.0")
	REAL (balance, U"Balance (0-1)", U"0.5")
	OPTIONMENUSTR (samplingFrequency, U"Sampling frequency (Hz)", 6)
		OPTION (U"8000")
		OPTION (U"11025")
		OPTION (U"16000")
		OPTION (U"22050")
		OPTION (U"32000")
		OPTION (U"44100")
		OPTION (U"48000")
		OPTION (U"96000")
	POSITIVE (duration, U"Duration (seconds)", U"1.0")
	OK
DO
	CREATE_ONE
		autoSound result = Sound_record_fixedTime (inputSource,
				gain, balance, Melder_atof (samplingFrequency), duration);
	CREATE_ONE_END (U"untitled")
}

extern "C" void* Praat_Sound_resample (void* sound, double newSamplingFrequency, int precision);
#if 0
void* Praat_Sound_resample (void* sound, double newSamplingFrequency, int precision) {
	try {
		if (newSamplingFrequency <= 0.0) Melder_throw (U"`newSamplingFrequency` should be positive.");
		if (precision <= 0) Melder_throw (U"`precision` should be greater than 0.");
		autoSound thee = Sound_resample ((Sound) sound, newSamplingFrequency, precision);
		return (void*) thee.releaseToAmbiguousOwner();
	} catch (MelderError) {
		Melder_flushError (U"Praat: Sound_resample: not performed.");
		return NULL;
	}
}
#elif 0
typedef struct {
	int type;
	union { void* _object; double _double; int _int; };
} PraatLibArg;
#define PraatLibArg_OBJECT  0
#define PraatLibArg_DOUBLE  1
#define PraatLibArg_INT  2
extern "C" void* praatlib_do (const char *commandTitle, PraatLibArg args [], int narg);
void* Praat_Sound_resample (void* sound, double newSamplingFrequency, int precision) {
	PraatLibArg args [3];
	args [0]. type = PraatLibArg_OBJECT;
	args [0]. _object = sound;
	args [1]. type = PraatLibArg_DOUBLE;
	args [1]. _double = newSamplingFrequency;
	args [2]. type = PraatLibArg_INT;
	args [2]. _int = precision;
	return praatlib_do ("Sound: Resample...", args, 3);
}
#endif

FORM (NEW_Sound_resample, U"Sound: Resample", U"Sound: Resample...") {
	POSITIVE (newSamplingFrequency, U"New sampling frequency (Hz)", U"10000.0")
	NATURAL (precision, U"Precision (samples)", U"50")
	OK
DO
	CONVERT_EACH (Sound)
		autoSound result = Sound_resample (me, newSamplingFrequency, precision);
	CONVERT_EACH_END (my name.get(), U"_", Melder_iround (newSamplingFrequency));
}

DIRECT (MODIFY_Sound_reverse) {
	MODIFY_EACH (Sound)
		Sound_reverse (me, 0.0, 0.0);
	MODIFY_EACH_END
}

FORM_SAVE (SAVE_Sound_saveAs24BitWavFile, U"Save as 24-bit WAV file", nullptr, U"wav") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_WAV, 24);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_Sound_saveAs32BitWavFile, U"Save as 32-bit WAV file", nullptr, U"wav") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_WAV, 32);
	SAVE_TYPED_LIST_END
}

FORM (MODIFY_Sound_scalePeak, U"Sound: Scale peak", U"Sound: Scale peak...") {
	POSITIVE (newAbsolutePeak, U"New absolute peak", U"0.99")
	OK
DO
	MODIFY_EACH (Sound)
		Vector_scale (me, newAbsolutePeak);
	MODIFY_EACH_END
}

FORM (MODIFY_Sound_scaleIntensity, U"Sound: Scale intensity", U"Sound: Scale intensity...") {
	POSITIVE (newAverageIntensity, U"New average intensity (dB SPL)", U"70.0")
	OK
DO
	MODIFY_EACH (Sound)
		Sound_scaleIntensity (me, newAverageIntensity);
	MODIFY_EACH_END
}

FORM (MODIFY_old_Sound_setValueAtIndex, U"Sound: Set value at sample number", U"Sound: Set value at sample number...") {
	NATURAL (sampleNumber, U"Sample number", U"100")
	REAL (newValue, U"New value", U"0.0")
	OK
DO
	MODIFY_EACH (Sound)
		if (sampleNumber > my nx)
			Melder_throw (U"The sample number should not exceed the number of samples, which is ", my nx, U".");
		for (integer channel = 1; channel <= my ny; channel ++)
			my z [channel] [sampleNumber] = newValue;
	MODIFY_EACH_END
}

FORM (MODIFY_Sound_setValueAtIndex, U"Sound: Set value at sample number", U"Sound: Set value at sample number...") {
	CHANNEL (channel, U"Channel", U"0 (= all)")
	NATURAL (sampleNumber, U"Sample number", U"100")
	REAL (newValue, U"New value", U"0.0")
	OK
DO_ALTERNATIVE (MODIFY_old_Sound_setValueAtIndex)
	MODIFY_EACH (Sound)
		if (sampleNumber > my nx)
			Melder_throw (U"The sample number should not exceed the number of samples, which is ", my nx, U".");
		if (channel > my ny) channel = 1;
		if (channel > 0) {
			my z [channel] [sampleNumber] = newValue;
		} else {
			for (channel = 1; channel <= my ny; channel ++)
				my z [channel] [sampleNumber] = newValue;
		}
	MODIFY_EACH_END
}

FORM (MODIFY_Sound_setPartToZero, U"Sound: Set part to zero", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	RADIOx (cut, U"Cut", 2, 0)
		OPTION (U"at exactly these times")
		OPTION (U"at nearest zero crossing")
	OK
DO
	MODIFY_EACH (Sound)
		Sound_setZero (me, fromTime, toTime, cut);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Sound_subtractMean) {
	MODIFY_EACH (Sound)
		Vector_subtractMean (me);
	MODIFY_EACH_END
}

FORM (NEW_Sound_to_Manipulation, U"Sound: To Manipulation", U"Manipulation") {
	POSITIVE (timeStep, U"Time step (s)", U"0.01")
	POSITIVE (minimumPitch, U"Minimum pitch (Hz)", U"75.0")
	POSITIVE (maximumPitch, U"Maximum pitch (Hz)", U"600.0")
	OK
DO
	if (maximumPitch <= minimumPitch)
		Melder_throw (U"The maximum pitch should be greater than the minimum pitch.");
	CONVERT_EACH (Sound)
		autoManipulation result = Sound_to_Manipulation (me, timeStep, minimumPitch, maximumPitch);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Cochleagram, U"Sound: To Cochleagram", nullptr) {
	POSITIVE (timeStep, U"Time step (s)", U"0.01")
	POSITIVE (frequencyResolution, U"Frequency resolution (Bark)", U"0.1")
	POSITIVE (windowLength, U"Window length (s)", U"0.03")
	REAL (forwardMaskingTime, U"Forward-masking time (s)", U"0.03")
	OK
DO
	CONVERT_EACH (Sound)
		autoCochleagram result = Sound_to_Cochleagram (me, timeStep,
			frequencyResolution, windowLength, forwardMaskingTime);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Cochleagram_edb, U"Sound: To Cochleagram (De Boer, Meddis & Hewitt)", nullptr) {
	POSITIVE (timeStep, U"Time step (s)", U"0.01")
	POSITIVE (frequencyResolution, U"Frequency resolution (Bark)", U"0.1")
	BOOLEAN (hasSynapse, U"Has synapse", true)
	LABEL (U"Meddis synapse properties")
	POSITIVE (replenishmentRate, U"   replenishment rate (/sec)", U"5.05")
	POSITIVE (lossRate, U"   loss rate (/sec)", U"2500.0")
	POSITIVE (returnRate, U"   return rate (/sec)", U"6580.0")
	POSITIVE (reprocessingRate, U"   reprocessing rate (/sec)", U"66.31")
	OK
DO
	CONVERT_EACH (Sound)
		autoCochleagram result = Sound_to_Cochleagram_edb (me, timeStep, frequencyResolution, hasSynapse,
				replenishmentRate, lossRate, returnRate, reprocessingRate);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Formant_burg, U"Sound: To Formant (Burg method)", U"Sound: To Formant (burg)...") {
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (maximumNumberOfFormants, U"Max. number of formants", U"5.0")
	REAL (formantCeiling, U"Formant ceiling (Hz)", U"5500.0 (= adult female)")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrom, U"Pre-emphasis from (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoFormant result = Sound_to_Formant_burg (me, timeStep,
				maximumNumberOfFormants, formantCeiling, windowLength, preEmphasisFrom);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Formant_keepAll, U"Sound: To Formant (keep all)", U"Sound: To Formant (keep all)...") {
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (maximumNumberOfFormants, U"Max. number of formants", U"5.0")
	REAL (formantCeiling, U"Formant ceiling (Hz)", U"5500.0 (= adult female)")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrom, U"Pre-emphasis from (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoFormant result = Sound_to_Formant_keepAll (me, timeStep,
				maximumNumberOfFormants, formantCeiling, windowLength, preEmphasisFrom);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Formant_willems, U"Sound: To Formant (split Levinson (Willems))", U"Sound: To Formant (sl)...") {
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (numberOfFormants, U"Number of formants", U"5.0")
	REAL (formantCeiling, U"Formant ceiling (Hz)", U"5500.0 (= adult female)")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrom, U"Pre-emphasis from (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoFormant result = Sound_to_Formant_willems (me, timeStep,
				numberOfFormants, formantCeiling, windowLength, preEmphasisFrom);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Harmonicity_ac, U"Sound: To Harmonicity (ac)", U"Sound: To Harmonicity (ac)...") {
	POSITIVE (timeStep, U"Time step (s)", U"0.01")
	POSITIVE (minimumPitch, U"Minimum pitch (Hz)", U"75.0")
	REAL (silenceThreshold, U"Silence threshold", U"0.1")
	POSITIVE (periodsPerWindow, U"Periods per window", U"4.5")
	OK
DO
	if (periodsPerWindow < 3.0) Melder_throw (U"Number of periods per window must be at least 3.0.");
	CONVERT_EACH (Sound)
		autoHarmonicity result = Sound_to_Harmonicity_ac (me, timeStep,
				minimumPitch, silenceThreshold, periodsPerWindow);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Harmonicity_cc, U"Sound: To Harmonicity (cc)", U"Sound: To Harmonicity (cc)...") {
	POSITIVE (timeStep, U"Time step (s)", U"0.01")
	POSITIVE (minimumPitch, U"Minimum pitch (Hz)", U"75.0")
	REAL (silenceThreshold, U"Silence threshold", U"0.1")
	POSITIVE (periodsPerWindow, U"Periods per window", U"1.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoHarmonicity result = Sound_to_Harmonicity_cc (me, timeStep,
				minimumPitch, silenceThreshold, periodsPerWindow);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Harmonicity_gne, U"Sound: To Harmonicity (gne)", nullptr) {
	POSITIVE (minimumFrequency, U"Minimum frequency (Hz)", U"500.0")
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"4500.0")
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"1000.0")
	POSITIVE (step, U"Step (Hz)", U"80.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoMatrix result = Sound_to_Harmonicity_GNE (me, minimumFrequency,
				maximumFrequency, bandwidth, step);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_old_Sound_to_Intensity, U"Sound: To Intensity", U"Sound: To Intensity...") {
	POSITIVE (minimumPitch, U"Minimum pitch (Hz)", U"100.0")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	OK
DO
	CONVERT_EACH (Sound)
		autoIntensity result = Sound_to_Intensity (me,
				minimumPitch, timeStep, false);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Intensity, U"Sound: To Intensity", U"Sound: To Intensity...") {
	POSITIVE (minimumPitch, U"Minimum pitch (Hz)", U"100.0")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	BOOLEAN (subtractMean, U"Subtract mean", true)
	OK
DO_ALTERNATIVE (NEW_old_Sound_to_Intensity)
	CONVERT_EACH (Sound)
		autoIntensity result = Sound_to_Intensity (me,
				minimumPitch, timeStep, subtractMean);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_IntensityTier, U"Sound: To IntensityTier", nullptr) {
	POSITIVE (minimumPitch, U"Minimum pitch (Hz)", U"100.0")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	BOOLEAN (subtractMean, U"Subtract mean", true)
	OK
DO
	CONVERT_EACH (Sound)
		autoIntensityTier result = Sound_to_IntensityTier (me,
				minimumPitch, timeStep, subtractMean);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_Sound_to_IntervalTier) {
	CONVERT_EACH (Sound)
		autoIntervalTier result = IntervalTier_create (my xmin, my xmax);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Ltas, U"Sound: To long-term average spectrum", nullptr) {
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"100")
	OK
DO
	CONVERT_EACH (Sound)
		autoLtas result = Sound_to_Ltas (me, bandwidth);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Ltas_pitchCorrected, U"Sound: To Ltas (pitch-corrected)", U"Sound: To Ltas (pitch-corrected)...") {
	POSITIVE (minimumPitch, U"Minimum pitch (Hz)", U"75.0")
	POSITIVE (maximumPitch, U"Maximum pitch (Hz)", U"600.0")
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"100.0")
	REAL (shortestPeriod, U"Shortest period (s)", U"0.0001")
	REAL (longestPeriod, U"Longest period (s)", U"0.02")
	POSITIVE (maximumPeriodFactor, U"Maximum period factor", U"1.3")
	OK
DO
	if (maximumPitch <= minimumPitch) Melder_throw (U"Your maximum pitch should be greater than your minimum pitch.");
	CONVERT_EACH (Sound)
		autoLtas result = Sound_to_Ltas_pitchCorrected (me, minimumPitch, maximumPitch,
				maximumFrequency, bandwidth, shortestPeriod, longestPeriod, maximumPeriodFactor);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_Sound_downto_Matrix) {
	CONVERT_EACH (Sound)
		autoMatrix result = Sound_to_Matrix (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW1_Sounds_to_ParamCurve) {
	Sound s1 = nullptr, s2 = nullptr;
	LOOP {
		iam (Sound);
		( s1 ? s2 : s1 ) = me;
	}
	autoParamCurve thee = ParamCurve_create (s1, s2);
	praat_new (thee.move(), s1 -> name.get(), U"_", s2 -> name.get());
END }

FORM (NEW_Sound_to_Pitch, U"Sound: To Pitch", U"Sound: To Pitch...") {
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	OK
DO
	CONVERT_EACH (Sound)
		autoPitch result = Sound_to_Pitch (me, timeStep, pitchFloor, pitchCeiling);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Pitch_ac, U"Sound: To Pitch (ac)", U"Sound: To Pitch (ac)...") {
	LABEL (U"Finding the candidates")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	NATURAL (maximumNumberOfCandidates, U"Max. number of candidates", U"15")
	BOOLEAN (veryAccurate, U"Very accurate", false)
	LABEL (U"Finding a path")
	REAL (silenceThreshold, U"Silence threshold", U"0.03")
	REAL (voicingThreshold, U"Voicing threshold", U"0.45")
	REAL (octaveCost, U"Octave cost", U"0.01")
	REAL (octaveJumpCost, U"Octave-jump cost", U"0.35")
	REAL (voicedUnvoicedCost, U"Voiced / unvoiced cost", U"0.14")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	OK
DO
	if (maximumNumberOfCandidates <= 1)
		Melder_throw (U"Your maximum number of candidates should be greater than 1.");
	CONVERT_EACH (Sound)
		autoPitch result = Sound_to_Pitch_ac (me, timeStep,
			pitchFloor, 3.0, maximumNumberOfCandidates, veryAccurate,
			silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost, voicedUnvoicedCost, pitchCeiling
		);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Pitch_cc, U"Sound: To Pitch (cc)", U"Sound: To Pitch (cc)...") {
	LABEL (U"Finding the candidates")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	NATURAL (maximumNumberOfCandidates, U"Max. number of candidates", U"15")
	BOOLEAN (veryAccurate, U"Very accurate", false)
	LABEL (U"Finding a path")
	REAL (silenceThreshold, U"Silence threshold", U"0.03")
	REAL (voicingThreshold, U"Voicing threshold", U"0.45")
	REAL (octaveCost, U"Octave cost", U"0.01")
	REAL (octaveJumpCost, U"Octave-jump cost", U"0.35")
	REAL (voicedUnvoicedCost, U"Voiced / unvoiced cost", U"0.14")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	OK
DO
	if (maximumNumberOfCandidates <= 1) Melder_throw (U"Youraximum number of candidates should be greater than 1.");
	CONVERT_EACH (Sound)
		autoPitch result = Sound_to_Pitch_cc (me, timeStep,
			pitchFloor, 1.0, maximumNumberOfCandidates, veryAccurate,
			silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost, voicedUnvoicedCost, pitchCeiling
		);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_PointProcess_extrema, U"Sound: To PointProcess (extrema)", nullptr) {
	CHANNEL (channel, U"Channel (number, Left, or Right)", U"1")
	BOOLEAN (includeMaxima, U"Include maxima", true)
	BOOLEAN (includeMinima, U"Include minima", false)
	RADIO_ENUM (kVector_peakInterpolation, peakInterpolationType,
		U"Interpolation", kVector_peakInterpolation::SINC70)
	OK
DO
	CONVERT_EACH (Sound)
		autoPointProcess result = Sound_to_PointProcess_extrema (me, channel > my ny ? 1 : channel,
				peakInterpolationType, includeMaxima, includeMinima);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_PointProcess_periodic_cc, U"Sound: To PointProcess (periodic, cc)", U"Sound: To PointProcess (periodic, cc)...") {
	POSITIVE (minimumPitch, U"Minimum pitch (Hz)", U"75.0")
	POSITIVE (maximumPitch, U"Maximum pitch (Hz)", U"600.0")
	OK
DO
	if (maximumPitch <= minimumPitch)
		Melder_throw (U"Your maximum pitch should be greater than your minimum pitch.");
	CONVERT_EACH (Sound)
		autoPointProcess result = Sound_to_PointProcess_periodic_cc (me, minimumPitch, maximumPitch);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_PointProcess_periodic_peaks, U"Sound: To PointProcess (periodic, peaks)", U"Sound: To PointProcess (periodic, peaks)...") {
	POSITIVE (minimumPitch, U"Minimum pitch (Hz)", U"75.0")
	POSITIVE (maximumPitch, U"Maximum pitch (Hz)", U"600.0")
	BOOLEAN (includeMaxima, U"Include maxima", true)
	BOOLEAN (includeMinima, U"Include minima", false)
	OK
DO
	if (maximumPitch <= minimumPitch)
		Melder_throw (U"Your maximum pitch should be greater than your minimum pitch.");
	CONVERT_EACH (Sound)
		autoPointProcess result = Sound_to_PointProcess_periodic_peaks (me,
				minimumPitch, maximumPitch, includeMaxima, includeMinima);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_PointProcess_zeroes, U"Get zeroes", nullptr) {
	CHANNEL (channel, U"Channel (number, Left, or Right)", U"1")
	BOOLEAN (includeRaisers, U"Include raisers", true)
	BOOLEAN (includeFallers, U"Include fallers", false)
	OK
DO
	CONVERT_EACH (Sound)
		autoPointProcess result = Sound_to_PointProcess_zeroes (me, channel > my ny ? 1 : channel,
				includeRaisers, includeFallers);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Spectrogram, U"Sound: To Spectrogram", U"Sound: To Spectrogram...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.005")
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	POSITIVE (timeStep, U"Time step (s)", U"0.002")
	POSITIVE (frequencyStep, U"Frequency step (Hz)", U"20.0")
	RADIO_ENUM (kSound_to_Spectrogram_windowShape, windowShape,
			U"Window shape", kSound_to_Spectrogram_windowShape::DEFAULT)
	OK
DO
	CONVERT_EACH (Sound)
		autoSpectrogram result = Sound_to_Spectrogram (me, windowLength,
				maximumFrequency, timeStep, frequencyStep, windowShape, 8.0, 8.0);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_Spectrum, U"Sound: To Spectrum", U"Sound: To Spectrum...") {
	BOOLEAN (fast, U"Fast", true)
	OK
DO
	CONVERT_EACH (Sound)
		autoSpectrum result = Sound_to_Spectrum (me, fast);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_Sound_to_Spectrum_dft) {
	CONVERT_EACH (Sound)
		autoSpectrum result = Sound_to_Spectrum (me, false);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_Sound_to_Spectrum_fft) {
	CONVERT_EACH (Sound)
		autoSpectrum result = Sound_to_Spectrum (me, true);
	CONVERT_EACH_END (my name.get())
}

FORM (NEW_Sound_to_TextGrid, U"Sound: To TextGrid", U"Sound: To TextGrid...") {
	SENTENCE (allTierNames, U"All tier names", U"Mary John bell")
	SENTENCE (whichOfTheseArePointTiers, U"Which of these are point tiers?", U"bell")
	OK
DO
	CONVERT_EACH (Sound)
		autoTextGrid result = TextGrid_create (my xmin, my xmax, allTierNames, whichOfTheseArePointTiers);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_Sound_to_TextTier) {
	CONVERT_EACH (Sound)
		autoTextTier result = TextTier_create (my xmin, my xmax);
	CONVERT_EACH_END (my name.get())
}

FORM (PREFS_SoundInputPrefs, U"Sound recording preferences", U"SoundRecorder") {
	NATURAL (bufferSize, U"Buffer size (MB)", U"60")
	OPTIONMENU_ENUM (kMelder_inputSoundSystem, inputSoundSystem,
			U"Input sound system", kMelder_inputSoundSystem::DEFAULT)
OK
	SET_INTEGER (bufferSize, SoundRecorder_getBufferSizePref_MB ())
	SET_ENUM (inputSoundSystem, kMelder_inputSoundSystem, MelderAudio_getInputSoundSystem())
DO
	if (bufferSize > 1000)
		Melder_throw (U"Buffer size cannot exceed 1000 megabytes.");
	SoundRecorder_setBufferSizePref_MB (bufferSize);
	MelderAudio_setInputSoundSystem (inputSoundSystem);
END }

FORM (PREFS_SoundOutputPrefs, U"Sound playing preferences", nullptr) {
	LABEL (U"The following determines how sounds are played.")
	LABEL (U"Between parentheses, you find what you can do simultaneously.")
	LABEL (U"Decrease asynchronicity if sound plays with discontinuities.")
	OPTIONMENU_ENUM (kMelder_asynchronicityLevel, maximumAsynchronicity,
			U"Maximum asynchronicity", kMelder_asynchronicityLevel::DEFAULT)
	#define xstr(s) str(s)
	#define str(s) #s
	REAL (silenceBefore, U"Silence before (s)", U"" xstr (kMelderAudio_outputSilenceBefore_DEFAULT))
	REAL (silenceAfter, U"Silence after (s)", U"" xstr (kMelderAudio_outputSilenceAfter_DEFAULT))
	OPTIONMENU_ENUM (kMelder_outputSoundSystem, outputSoundSystem,
			U"Output sound system", kMelder_outputSoundSystem::DEFAULT)
OK
	SET_ENUM (maximumAsynchronicity, kMelder_asynchronicityLevel, MelderAudio_getOutputMaximumAsynchronicity ())
	SET_REAL (silenceBefore, MelderAudio_getOutputSilenceBefore ())
	SET_REAL (silenceAfter, MelderAudio_getOutputSilenceAfter ())
	SET_ENUM (outputSoundSystem, kMelder_outputSoundSystem, MelderAudio_getOutputSoundSystem())
DO
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	MelderAudio_setOutputMaximumAsynchronicity (maximumAsynchronicity);
	MelderAudio_setOutputSilenceBefore (silenceBefore);
	MelderAudio_setOutputSilenceAfter (silenceAfter);
	MelderAudio_setOutputSoundSystem (outputSoundSystem);
	END
}

#ifdef HAVE_PULSEAUDIO
void pulseAudio_serverReport ();
DIRECT (INFO_Praat_reportSoundServerProperties) {
	pulseAudio_serverReport ();
END }
#endif

FORM_SAVE (SAVE_Sound_saveAsAifcFile, U"Save as AIFC file", nullptr, U"aifc") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_AIFC, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_Sound_saveAsAiffFile, U"Save as AIFF file", nullptr, U"aiff") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_AIFF, 16);
END }

FORM_SAVE (SAVE_Sound_saveAsFlacFile, U"Save as FLAC file", nullptr, U"flac") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_FLAC, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_Sound_saveAsKayFile, U"Save as Kay sound file", nullptr, U"kay") {
	SAVE_ONE (Sound)
		Sound_saveAsKayFile (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Sound_saveAsNextSunFile, U"Save as NeXT/Sun file", nullptr, U"au") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_NEXT_SUN, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_Sound_saveAsNistFile, U"Save as NIST file", nullptr, U"nist") {
	SAVE_TYPED_LIST (Sampled, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_NIST, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_Sound_saveAsRaw8bitSignedFile, U"Save as raw 8-bit signed sound file", nullptr, U"8sig") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_8_SIGNED);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Sound_saveAsRaw8bitUnsignedFile, U"Save as raw 8-bit unsigned sound file", nullptr, U"8uns") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_8_UNSIGNED);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Sound_saveAsRaw16bitBigEndianFile, U"Save as raw 16-bit big-endian sound file", nullptr, U"16be") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_16_BIG_ENDIAN);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Sound_saveAsRaw16bitLittleEndianFile, U"Save as raw 16-bit little-endian sound file", nullptr, U"16le") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_16_LITTLE_ENDIAN);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Sound_saveAsRaw24bitBigEndianFile, U"Save as raw 24-bit big-endian sound file", nullptr, U"24be") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_24_BIG_ENDIAN);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Sound_saveAsRaw24bitLittleEndianFile, U"Save as raw 24-bit little-endian sound file", nullptr, U"24le") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_24_LITTLE_ENDIAN);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Sound_saveAsRaw32bitBigEndianFile, U"Save as raw 32-bit big-endian sound file", nullptr, U"32be") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_32_BIG_ENDIAN);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Sound_saveAsRaw32bitLittleEndianFile, U"Save as raw 32-bit little-endian sound file", nullptr, U"32le") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_32_LITTLE_ENDIAN);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Sound_saveAsSesamFile, U"Save as Sesam file", nullptr, U"sdf") {
	SAVE_ONE (Sound)
		Sound_saveAsSesamFile (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Sound_saveAsStereoAifcFile, U"Save as stereo AIFC file", nullptr, U"aifc") {
	SAVE_LIST (Sound)
		autoSound stereo = Sounds_combineToStereo (& list);
		Sound_saveAsAudioFile (stereo.get(), file, Melder_AIFC, 16);
	SAVE_LIST_END
}

FORM_SAVE (SAVE_Sound_saveAsStereoAiffFile, U"Save as stereo AIFF file", nullptr, U"aiff") {
	SAVE_LIST (Sound)
		autoSound stereo = Sounds_combineToStereo (& list);
		Sound_saveAsAudioFile (stereo.get(), file, Melder_AIFF, 16);
	SAVE_LIST_END
}

FORM_SAVE (SAVE_Sound_saveAsStereoNextSunFile, U"Save as stereo NeXT/Sun file", nullptr, U"au") {
	SAVE_LIST (Sound)
		autoSound stereo = Sounds_combineToStereo (& list);
		Sound_saveAsAudioFile (stereo.get(), file, Melder_NEXT_SUN, 16);
	SAVE_LIST_END
}

FORM_SAVE (SAVE_Sound_saveAsStereoNistFile, U"Save as stereo NIST file", nullptr, U"nist") {
	SAVE_LIST (Sound)
		autoSound stereo = Sounds_combineToStereo (& list);
		Sound_saveAsAudioFile (stereo.get(), file, Melder_NIST, 16);
	SAVE_LIST_END
}

FORM_SAVE (SAVE_Sound_saveAsStereoFlacFile, U"Save as stereo FLAC file", nullptr, U"flac") {
	SAVE_LIST (Sound)
		autoSound stereo = Sounds_combineToStereo (& list);
		Sound_saveAsAudioFile (stereo.get(), file, Melder_FLAC, 16);
	SAVE_LIST_END
}

FORM_SAVE (SAVE_Sound_saveAsStereoWavFile, U"Save as stereo WAV file", nullptr, U"wav") {
	SAVE_LIST (Sound)
		autoSound stereo = Sounds_combineToStereo (& list);
		Sound_saveAsAudioFile (stereo.get(), file, Melder_WAV, 16);
	SAVE_LIST_END
}

FORM_SAVE (SAVE_Sound_saveAsSunAudioFile, U"Save as NeXT/Sun file", nullptr, U"au") {
	SAVE_TYPED_LIST (Sound, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_NEXT_SUN, 16);
	SAVE_TYPED_LIST_END
}

FORM_SAVE (SAVE_Sound_saveAsWavFile, U"Save as WAV file", nullptr, U"wav") {
	SAVE_TYPED_LIST (Sound, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_WAV, 16);
	SAVE_TYPED_LIST_END
}

/***** SOUNDLIST *****/

DIRECT (NEWMANY_SoundList_extractAllSounds) {
	CONVERT_EACH (SoundList)
		autoSoundList result = Data_copy (me);
		result -> classInfo = classCollection;   // YUCK, in order to force automatic unpacking
	CONVERT_EACH_END (U"dummy")
}

/***** SOUNDSET *****/

DIRECT (NEWMANY_SoundSet_extractAllSounds) {
	CONVERT_EACH (SoundSet)
		autoSoundSet result = Data_copy (me);
		result -> classInfo = classCollection;   // YUCK, in order to force automatic unpacking
	CONVERT_EACH_END (U"dummy")
}

FORM (NEW2_SoundSet_Table_getRandomizedPatterns, U"SoundSet & Table: Get randomized patterns", nullptr) {
	SENTENCE (columnName, U"Column name", U"")
	NATURAL (numberOfPatterns, U"Number of patterns", U"1000")
	NATURAL (inputSize, U"Input size (number of samples)", U"8000")
	NATURAL (outputSize, U"Output size (number of classes)", U"5")
	OK
DO
	FIND_TWO (SoundSet, Table)
		autoPatternList inputs, outputs;
		SoundSet_Table_getRandomizedPatterns (me, you, columnName, numberOfPatterns, inputSize, outputSize,
				& inputs, & outputs);
		praat_new (inputs.move(), U"inputs");
		praat_new (outputs.move(), U"outputs");
	END
}

/***** STOP *****/

DIRECT (PLAY_stopPlayingSound) {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
END }

/***** Help menus *****/

DIRECT (HELP_AnnotationTutorial) {
	HELP (U"Intro 7. Annotation")
}

DIRECT (HELP_FilteringTutorial) {
	HELP (U"Filtering")
}

/***** file recognizers *****/

static autoDaata macSoundOrEmptyFileRecognizer (integer nread, const char * /* header */, MelderFile file) {
	/***** No data in file? This may be a Macintosh sound file with only a resource fork. *****/
	if (nread > 0) return autoDaata ();
	Melder_throw (U"File ", file, U" contains no audio data.");   // !!!
}

static autoDaata soundFileRecognizer (integer nread, const char *header, MelderFile file) {
	if (nread < 16)
		return autoDaata ();
	if (strnequ (header, "FORM", 4) && strnequ (header + 8, "AIF", 3))
		return Sound_readFromSoundFile (file);
	if (strnequ (header, "RIFF", 4) && (strnequ (header + 8, "WAVE", 4) || strnequ (header + 8, "CDDA", 4)))
		return Sound_readFromSoundFile (file);
	if (strnequ (header, ".snd", 4))
		return Sound_readFromSoundFile (file);
	if (strnequ (header, "NIST_1A", 7))
		return Sound_readFromSoundFile (file);
	if (strnequ (header, "fLaC", 4))
		return Sound_readFromSoundFile (file);   // Erez Volk, March 2007
	if ((Melder_stringMatchesCriterion (MelderFile_name (file), kMelder_string::ENDS_WITH, U".mp3", false))
			&& mp3_recognize (nread, header))
		return Sound_readFromSoundFile (file);   // Erez Volk, May 2007
	return autoDaata ();
}

static autoDaata movieFileRecognizer (integer nread, const char * /* header */, MelderFile file) {
	conststring32 fileName = MelderFile_name (file);
	/*Melder_casual ("%d %d %d %d %d %d %d %d %d %d", header [0],
		header [1], header [2], header [3],
		header [4], header [5], header [6],
		header [7], header [8], header [9]);*/
	if (nread < 512 || (! Melder_stringMatchesCriterion (fileName, kMelder_string::ENDS_WITH, U".mov", false) &&
	                    ! Melder_stringMatchesCriterion (fileName, kMelder_string::ENDS_WITH, U".avi", false)))
		return autoDaata ();
	Melder_throw (U"This Praat version cannot open movie files.");
	return autoDaata ();
}

static autoDaata sesamFileRecognizer (integer nread, const char * /* header */, MelderFile file) {
	conststring32 fileName = MelderFile_name (file);
	if (nread < 512 || (! Melder_stringMatchesCriterion (fileName, kMelder_string::ENDS_WITH, U".sdf", false)))
		return autoDaata ();
	return Sound_readFromSesamFile (file);
}

static autoDaata bellLabsFileRecognizer (integer nread, const char *header, MelderFile file) {
	if (nread < 16 || ! strnequ (& header [0], "SIG\n", 4))
		return autoDaata ();
	return Sound_readFromBellLabsFile (file);
}

static autoDaata kayFileRecognizer (integer nread, const char *header, MelderFile file) {
	if (nread <= 12 || ! strnequ (& header [0], "FORMDS16", 8))
		return autoDaata ();
	return Sound_readFromKayFile (file);
}

/***** override play and record buttons in manuals *****/

static autoSound melderSound, melderSoundFromFile;
static Sound last;
static int recordProc (double duration) {
	if (last == melderSound.get())
		last = nullptr;
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	melderSound = Sound_record_fixedTime (1, 1.0, 0.5, 44100, duration);
	if (! melderSound)
		return 0;
	last = melderSound.get();
	return 1;
}
static int recordFromFileProc (MelderFile file) {
	if (last == melderSoundFromFile.get())
		last = nullptr;
	Melder_warningOff ();   // like "missing samples"
	melderSoundFromFile = Data_readFromFile (file). static_cast_move<structSound>();
	Melder_warningOn ();
	if (! melderSoundFromFile)
		return 0;
	if (! Thing_isa (melderSoundFromFile.get(), classSound)) {
		melderSoundFromFile.reset();
		return 0;
	}
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
	if (! last)
		return 0;
	autoSound sound = Data_copy (last);
	return Data_publish (sound.move());
}

/***** buttons *****/

void praat_Sound_init () {
	Thing_recognizeClassesByName (classSound, classLongSound, classSoundList, classSoundSet, nullptr);

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

	praat_addMenuCommand (U"Objects", U"New", U"Record mono Sound...", nullptr, praat_ATTRACTIVE | 'R' | praat_NO_API, WINDOW_Sound_recordMono);
	praat_addMenuCommand (U"Objects", U"New", U"Record stereo Sound...", nullptr, praat_NO_API, WINDOW_Sound_recordStereo);
	praat_addMenuCommand (U"Objects", U"New", U"Record Sound (fixed time)...", nullptr, praat_HIDDEN | praat_FORCE_API, RECORD1_Sound_record_fixedTime);
	praat_addMenuCommand (U"Objects", U"New", U"Sound", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create Sound as pure tone...", nullptr, 1, NEW1_Sound_createAsPureTone);
		praat_addMenuCommand (U"Objects", U"New", U"Create Sound from formula...", nullptr, 1, NEW1_Sound_createFromFormula);
		praat_addMenuCommand (U"Objects", U"New",   U"Create Sound...", U"*Create Sound from formula...", praat_DEPTH_1 | praat_DEPRECATED_2007, NEW1_Sound_create);
		praat_addMenuCommand (U"Objects", U"New", U"-- create sound advanced --", nullptr, 1, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create Sound as tone complex...", nullptr, 1, NEW1_Sound_createAsToneComplex);
		praat_addMenuCommand (U"Objects", U"New",   U"Create Sound from tone complex...", U"*Create Sound as tone complex...", praat_DEPTH_1 | praat_DEPRECATED_2013, NEW1_Sound_createAsToneComplex);

	praat_addMenuCommand (U"Objects", U"Open", U"-- read sound --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Open", U"Open long sound file...", nullptr, 'L', READ1_LongSound_open);
	praat_addMenuCommand (U"Objects", U"Open", U"Read two Sounds from stereo file...", nullptr, praat_DEPRECATED_2010, READMANY_Sound_readSeparateChannelsFromSoundFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read separate channels from sound file...", nullptr, 0, READMANY_Sound_readSeparateChannelsFromSoundFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read from special sound file", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"Open", U"Read Sound from raw Alaw file...", nullptr, praat_DEPTH_1, READ1_Sound_readFromRawAlawFile);

	praat_addMenuCommand (U"Objects", U"Goodies", U"Stop playing sound", nullptr, GuiMenu_ESCAPE, PLAY_stopPlayingSound);
	praat_addMenuCommand (U"Objects", U"Preferences", U"-- sound prefs --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Preferences", U"Sound recording preferences...", nullptr, 0, PREFS_SoundInputPrefs);
	praat_addMenuCommand (U"Objects", U"Preferences", U"Sound playing preferences...", nullptr, 0, PREFS_SoundOutputPrefs);
	praat_addMenuCommand (U"Objects", U"Preferences", U"LongSound preferences...", nullptr, 0, PREFS_LongSoundPrefs);
#ifdef HAVE_PULSEAUDIO
	praat_addMenuCommand (U"Objects", U"Technical", U"Report sound server properties", U"Report system properties", 0, INFO_Praat_reportSoundServerProperties);
#endif

	praat_addAction1 (classLongSound, 0, U"LongSound help", nullptr, 0, HELP_LongSound_help);
	praat_addAction1 (classLongSound, 1, U"View", nullptr, praat_ATTRACTIVE, WINDOW_LongSound_view);
	praat_addAction1 (classLongSound, 1,   U"Open", U"*View", praat_DEPRECATED_2011, WINDOW_LongSound_view);
	praat_addAction1 (classLongSound, 0, U"Play part...", nullptr, 0, PLAY_LongSound_playPart);
	praat_addAction1 (classLongSound, 1, U"Query -", nullptr, 0, nullptr);
		praat_TimeFunction_query_init (classLongSound);
		praat_addAction1 (classLongSound, 1, U"Sampling", nullptr, 1, nullptr);
		praat_addAction1 (classLongSound, 1, U"Get number of samples", nullptr, 2, INTEGER_LongSound_getNumberOfSamples);
		praat_addAction1 (classLongSound, 1, U"Get sampling period", nullptr, 2, REAL_LongSound_getSamplePeriod);
		praat_addAction1 (classLongSound, 1,   U"Get sample duration", U"*Get sampling period", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_LongSound_getSamplePeriod);
		praat_addAction1 (classLongSound, 1,   U"Get sample period", U"*Get sampling period", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_LongSound_getSamplePeriod);
		praat_addAction1 (classLongSound, 1, U"Get sampling frequency", nullptr, 2, REAL_LongSound_getSampleRate);
		praat_addAction1 (classLongSound, 1,   U"Get sample rate", U"*Get sampling frequency", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_LongSound_getSampleRate);
		praat_addAction1 (classLongSound, 1, U"-- get time discretization --", nullptr, 2, nullptr);
		praat_addAction1 (classLongSound, 1, U"Get time from sample number...", nullptr, 2, REAL_LongSound_getTimeFromIndex);
		praat_addAction1 (classLongSound, 1,   U"Get time from index...", U"*Get time from sample number...", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_LongSound_getTimeFromIndex);
		praat_addAction1 (classLongSound, 1, U"Get sample number from time...", nullptr, 2, REAL_LongSound_getIndexFromTime);
		praat_addAction1 (classLongSound, 1,   U"Get index from time...", U"*Get sample number from time...", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_LongSound_getIndexFromTime);
	praat_addAction1 (classLongSound, 0, U"Annotate -", nullptr, 0, nullptr);
		praat_addAction1 (classLongSound, 0, U"Annotation tutorial", nullptr, 1, HELP_AnnotationTutorial);
		praat_addAction1 (classLongSound, 0, U"-- to text grid --", nullptr, 1, nullptr);
		praat_addAction1 (classLongSound, 0, U"To TextGrid...", nullptr, 1, NEW_LongSound_to_TextGrid);
	praat_addAction1 (classLongSound, 0, U"Convert to Sound", nullptr, 0, nullptr);
	praat_addAction1 (classLongSound, 0, U"Extract part...", nullptr, 0, NEW_LongSound_extractPart);
	praat_addAction1 (classLongSound, 0, U"Concatenate?", nullptr, 0, INFO_LongSound_concatenate);
	praat_addAction1 (classLongSound, 0, U"Save as WAV file...", nullptr, 0, SAVE_LongSound_saveAsWavFile);
	praat_addAction1 (classLongSound, 0,   U"Write to WAV file...", U"*Save as WAV file...", praat_DEPRECATED_2011, SAVE_LongSound_saveAsWavFile);
	praat_addAction1 (classLongSound, 0, U"Save as AIFF file...", nullptr, 0, SAVE_LongSound_saveAsAiffFile);
	praat_addAction1 (classLongSound, 0,   U"Write to AIFF file...", U"*Save as AIFF file...", praat_DEPRECATED_2011, SAVE_LongSound_saveAsAiffFile);
	praat_addAction1 (classLongSound, 0, U"Save as AIFC file...", nullptr, 0, SAVE_LongSound_saveAsAifcFile);
	praat_addAction1 (classLongSound, 0,   U"Write to AIFC file...", U"*Save as AIFC file...", praat_DEPRECATED_2011, SAVE_LongSound_saveAsAifcFile);
	praat_addAction1 (classLongSound, 0, U"Save as Next/Sun file...", nullptr, 0, SAVE_LongSound_saveAsNextSunFile);
	praat_addAction1 (classLongSound, 0,   U"Write to Next/Sun file...", U"*Save as Next/Sun file...", praat_DEPRECATED_2011, SAVE_LongSound_saveAsNextSunFile);
	praat_addAction1 (classLongSound, 0, U"Save as NIST file...", nullptr, 0, SAVE_LongSound_saveAsNistFile);
	praat_addAction1 (classLongSound, 0,   U"Write to NIST file...", U"*Save as NIST file...", praat_DEPRECATED_2011, SAVE_LongSound_saveAsNistFile);
	praat_addAction1 (classLongSound, 0, U"Save as FLAC file...", nullptr, 0, SAVE_LongSound_saveAsFlacFile);
	praat_addAction1 (classLongSound, 0,   U"Write to FLAC file...", U"*Save as FLAC file...", praat_DEPRECATED_2011, SAVE_LongSound_saveAsFlacFile);
	praat_addAction1 (classLongSound, 0, U"Save left channel as WAV file...", nullptr, 0, SAVE_LongSound_saveLeftChannelAsWavFile);
	praat_addAction1 (classLongSound, 0,   U"Write left channel to WAV file...", U"*Save left channel as WAV file...", praat_DEPRECATED_2011, SAVE_LongSound_saveLeftChannelAsWavFile);
	praat_addAction1 (classLongSound, 0, U"Save left channel as AIFF file...", nullptr, 0, SAVE_LongSound_saveLeftChannelAsAiffFile);
	praat_addAction1 (classLongSound, 0,   U"Write left channel to AIFF file...", U"*Save left channel as AIFF file...", praat_DEPRECATED_2011, SAVE_LongSound_saveLeftChannelAsAiffFile);
	praat_addAction1 (classLongSound, 0, U"Save left channel as AIFC file...", nullptr, 0, SAVE_LongSound_saveLeftChannelAsAifcFile);
	praat_addAction1 (classLongSound, 0,   U"Write left channel to AIFC file...", U"*Save left channel as AIFC file...", praat_DEPRECATED_2011, SAVE_LongSound_saveLeftChannelAsAifcFile);
	praat_addAction1 (classLongSound, 0, U"Save left channel as Next/Sun file...", nullptr, 0, SAVE_LongSound_saveLeftChannelAsNextSunFile);
	praat_addAction1 (classLongSound, 0,   U"Write left channel to Next/Sun file...", U"*Save left channel as Next/Sun file...", praat_DEPRECATED_2011, SAVE_LongSound_saveLeftChannelAsNextSunFile);
	praat_addAction1 (classLongSound, 0, U"Save left channel as NIST file...", nullptr, 0, SAVE_LongSound_saveLeftChannelAsNistFile);
	praat_addAction1 (classLongSound, 0,   U"Write left channel to NIST file...", U"*Save left channel as NIST file...", praat_DEPRECATED_2011, SAVE_LongSound_saveLeftChannelAsNistFile);
	praat_addAction1 (classLongSound, 0, U"Save left channel as FLAC file...", nullptr, 0, SAVE_LongSound_saveLeftChannelAsFlacFile);
	praat_addAction1 (classLongSound, 0,   U"Write left channel to FLAC file...", U"*Save left channel as FLAC file...", praat_DEPRECATED_2011, SAVE_LongSound_saveLeftChannelAsFlacFile);
	praat_addAction1 (classLongSound, 0, U"Save right channel as WAV file...", nullptr, 0, SAVE_LongSound_saveRightChannelAsWavFile);
	praat_addAction1 (classLongSound, 0,   U"Write right channel to WAV file...", U"*Save right channel as WAV file...", praat_DEPRECATED_2011, SAVE_LongSound_saveRightChannelAsWavFile);
	praat_addAction1 (classLongSound, 0, U"Save right channel as AIFF file...", nullptr, 0, SAVE_LongSound_saveRightChannelAsAiffFile);
	praat_addAction1 (classLongSound, 0,   U"Write right channel to AIFF file...", U"*Save right channel as AIFF file...", praat_DEPRECATED_2011, SAVE_LongSound_saveRightChannelAsAiffFile);
	praat_addAction1 (classLongSound, 0, U"Save right channel as AIFC file...", nullptr, 0, SAVE_LongSound_saveRightChannelAsAifcFile);
	praat_addAction1 (classLongSound, 0,   U"Write right channel to AIFC file...", U"*Save right channel as AIFC file...", praat_DEPRECATED_2011, SAVE_LongSound_saveRightChannelAsAifcFile);
	praat_addAction1 (classLongSound, 0, U"Save right channel as Next/Sun file...", nullptr, 0, SAVE_LongSound_saveRightChannelAsNextSunFile);
	praat_addAction1 (classLongSound, 0,   U"Write right channel to Next/Sun file...", U"*Save right channel as Next/Sun file...", praat_DEPRECATED_2011, SAVE_LongSound_saveRightChannelAsNextSunFile);
	praat_addAction1 (classLongSound, 0, U"Save right channel as NIST file...", nullptr, 0, SAVE_LongSound_saveRightChannelAsNistFile);
	praat_addAction1 (classLongSound, 0,   U"Write right channel to NIST file...", U"*Save right channel as NIST file...", praat_DEPRECATED_2011, SAVE_LongSound_saveRightChannelAsNistFile);
	praat_addAction1 (classLongSound, 0, U"Save right channel as FLAC file...", nullptr, 0, SAVE_LongSound_saveRightChannelAsFlacFile);
	praat_addAction1 (classLongSound, 0,   U"Write right channel to FLAC file...", U"*Save right channel as FLAC file...", praat_DEPRECATED_2011, SAVE_LongSound_saveRightChannelAsFlacFile);
	praat_addAction1 (classLongSound, 0, U"Save part as audio file...", nullptr, 0, SAVE_LongSound_savePartAsAudioFile);
	praat_addAction1 (classLongSound, 0,   U"Write part to audio file...", U"*Save part as audio file...", praat_DEPRECATED_2011, SAVE_LongSound_savePartAsAudioFile);

	praat_addAction1 (classSound, 0, U"Save as WAV file...", nullptr, 0, SAVE_Sound_saveAsWavFile);
	praat_addAction1 (classSound, 0,   U"Write to WAV file...", U"*Save as WAV file...", praat_DEPRECATED_2011, SAVE_Sound_saveAsWavFile);
	praat_addAction1 (classSound, 0, U"Save as AIFF file...", nullptr, 0, SAVE_Sound_saveAsAiffFile);
	praat_addAction1 (classSound, 0,   U"Write to AIFF file...", U"*Save as AIFF file...", praat_DEPRECATED_2011, SAVE_Sound_saveAsAiffFile);
	praat_addAction1 (classSound, 0, U"Save as AIFC file...", nullptr, 0, SAVE_Sound_saveAsAifcFile);
	praat_addAction1 (classSound, 0,   U"Write to AIFC file...", U"*Save as AIFC file...", praat_DEPRECATED_2011, SAVE_Sound_saveAsAifcFile);
	praat_addAction1 (classSound, 0, U"Save as Next/Sun file...", nullptr, 0, SAVE_Sound_saveAsNextSunFile);
	praat_addAction1 (classSound, 0,   U"Write to Next/Sun file...", U"*Save as Next/Sun file...", praat_DEPRECATED_2011, SAVE_Sound_saveAsNextSunFile);
	praat_addAction1 (classSound, 0, U"Save as Sun audio file...", nullptr, praat_HIDDEN, SAVE_Sound_saveAsSunAudioFile);
	praat_addAction1 (classSound, 0,   U"Write to Sun audio file...", U"*Save as Sun audio file...", praat_DEPRECATED_2011, SAVE_Sound_saveAsSunAudioFile);
	praat_addAction1 (classSound, 0, U"Save as NIST file...", nullptr, 0, SAVE_Sound_saveAsNistFile);
	praat_addAction1 (classSound, 0,   U"Write to NIST file...", U"*Save as NIST file...", praat_DEPRECATED_2011, SAVE_Sound_saveAsNistFile);
	praat_addAction1 (classSound, 0, U"Save as FLAC file...", nullptr, 0, SAVE_Sound_saveAsFlacFile);
	praat_addAction1 (classSound, 0,   U"Write to FLAC file...", U"*Save as FLAC file...", praat_DEPRECATED_2011, SAVE_Sound_saveAsFlacFile);
	praat_addAction1 (classSound, 1, U"Save as Kay sound file...", nullptr, 0, SAVE_Sound_saveAsKayFile);
	praat_addAction1 (classSound, 1,   U"Write to Kay sound file...", U"*Save as Kay sound file...", praat_DEPRECATED_2011, SAVE_Sound_saveAsKayFile);
	praat_addAction1 (classSound, 1, U"Save as Sesam file...", nullptr, praat_HIDDEN, SAVE_Sound_saveAsSesamFile);
	praat_addAction1 (classSound, 1,   U"Write to Sesam file...", U"*Save as Sesam file...", praat_DEPRECATED_2011, SAVE_Sound_saveAsSesamFile);
	praat_addAction1 (classSound, 0, U"Save as 24-bit WAV file...", nullptr, 0, SAVE_Sound_saveAs24BitWavFile);
	praat_addAction1 (classSound, 0, U"Save as 32-bit WAV file...", nullptr, 0, SAVE_Sound_saveAs32BitWavFile);
	praat_addAction1 (classSound, 2,   U"Write to stereo WAV file...", U"* \"Combine to stereo\" and \"Save to WAV file...\"", praat_DEPRECATED_2007, SAVE_Sound_saveAsStereoWavFile);
	praat_addAction1 (classSound, 2,   U"Write to stereo AIFF file...", U"* \"Combine to stereo\" and \"Save to AIFF file...\"", praat_DEPRECATED_2007, SAVE_Sound_saveAsStereoAiffFile);
	praat_addAction1 (classSound, 2,   U"Write to stereo AIFC file...", U"* \"Combine to stereo\" and \"Save to AIFC file...\"", praat_DEPRECATED_2007, SAVE_Sound_saveAsStereoAifcFile);
	praat_addAction1 (classSound, 2,   U"Write to stereo Next/Sun file...", U"* \"Combine to stereo\" and \"Save to Next/Sun file...\"", praat_DEPRECATED_2007, SAVE_Sound_saveAsStereoNextSunFile);
	praat_addAction1 (classSound, 2,   U"Write to stereo NIST file...", U"* \"Combine to stereo\" and \"Save to NIST file...\"", praat_DEPRECATED_2007, SAVE_Sound_saveAsStereoNistFile);
	praat_addAction1 (classSound, 2,   U"Write to stereo FLAC file...", U"* \"Combine to stereo\" and \"Save to FLAC file...\"", praat_DEPRECATED_2007, SAVE_Sound_saveAsStereoFlacFile);
	//praat_addAction1 (classSound, 1, U"Save as raw sound file", nullptr, 0, nullptr);
	praat_addAction1 (classSound, 1, U"Save as raw 8-bit signed file...", nullptr, 0, SAVE_Sound_saveAsRaw8bitSignedFile);
	praat_addAction1 (classSound, 1,   U"Write to raw 8-bit signed file...", U"*Save as raw 8-bit signed file...", praat_DEPRECATED_2011, SAVE_Sound_saveAsRaw8bitSignedFile);
	praat_addAction1 (classSound, 1, U"Save as raw 8-bit unsigned file...", nullptr, 0, SAVE_Sound_saveAsRaw8bitUnsignedFile);
	praat_addAction1 (classSound, 1,   U"Write to raw 8-bit unsigned file...", U"*Save as raw 8-bit unsigned file...", praat_DEPRECATED_2011, SAVE_Sound_saveAsRaw8bitUnsignedFile);
	praat_addAction1 (classSound, 1, U"Save as raw 16-bit big-endian file...", nullptr, 0, SAVE_Sound_saveAsRaw16bitBigEndianFile);
	praat_addAction1 (classSound, 1, U"Save as raw 16-bit little-endian file...", nullptr, 0, SAVE_Sound_saveAsRaw16bitLittleEndianFile);
	praat_addAction1 (classSound, 1, U"Save as raw 24-bit big-endian file...", nullptr, 0, SAVE_Sound_saveAsRaw24bitBigEndianFile);
	praat_addAction1 (classSound, 1, U"Save as raw 24-bit little-endian file...", nullptr, 0, SAVE_Sound_saveAsRaw24bitLittleEndianFile);
	praat_addAction1 (classSound, 1, U"Save as raw 32-bit big-endian file...", nullptr, 0, SAVE_Sound_saveAsRaw32bitBigEndianFile);
	praat_addAction1 (classSound, 1, U"Save as raw 32-bit little-endian file...", nullptr, 0, SAVE_Sound_saveAsRaw32bitLittleEndianFile);
	praat_addAction1 (classSound, 0, U"Sound help", nullptr, 0, HELP_Sound_help);
	praat_addAction1 (classSound, 1, U"View & Edit", 0, praat_ATTRACTIVE | praat_NO_API, WINDOW_Sound_viewAndEdit);
	praat_addAction1 (classSound, 1,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011 | praat_NO_API, WINDOW_Sound_viewAndEdit);
	praat_addAction1 (classSound, 1,   U"Open", U"*View & Edit", praat_DEPRECATED_2011 | praat_NO_API, WINDOW_Sound_viewAndEdit);
	praat_addAction1 (classSound, 0, U"Play", nullptr, 0, PLAY_Sound_play);
	praat_addAction1 (classSound, 1, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"Draw...", nullptr, 1, GRAPHICS_Sound_draw);
	praat_addAction1 (classSound, 1, U"Query -", nullptr, 0, nullptr);
		praat_TimeFunction_query_init (classSound);
		praat_addAction1 (classSound, 1, U"Get number of channels", nullptr, 1, INTEGER_Sound_getNumberOfChannels);
		praat_addAction1 (classSound, 1, U"Query time sampling", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get number of samples", nullptr, 2, INTEGER_Sound_getNumberOfSamples);
		praat_addAction1 (classSound, 1, U"Get sampling period", nullptr, 2, REAL_Sound_getSamplePeriod);
		praat_addAction1 (classSound, 1,   U"Get sample duration", U"*Get sampling period", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_Sound_getSamplePeriod);
		praat_addAction1 (classSound, 1,   U"Get sample period", U"*Get sampling period", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_Sound_getSamplePeriod);
		praat_addAction1 (classSound, 1, U"Get sampling frequency", nullptr, 2, REAL_Sound_getSampleRate);
		praat_addAction1 (classSound, 1,   U"Get sample rate", U"*Get sampling frequency", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_Sound_getSampleRate);
		praat_addAction1 (classSound, 1, U"-- get time discretization --", nullptr, 2, nullptr);
		praat_addAction1 (classSound, 1, U"Get time from sample number...", nullptr, 2, REAL_Sound_getTimeFromIndex);
		praat_addAction1 (classSound, 1,   U"Get time from index...", U"*Get time from sample number...", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_Sound_getTimeFromIndex);
		praat_addAction1 (classSound, 1, U"List all sample times", nullptr, 2, NUMVEC_Sound_listAllSampleTimes);
		praat_addAction1 (classSound, 1,   U"Get sample times", U"*List all sample times", praat_DEPTH_2 | praat_DEPRECATED_2019, NUMVEC_Sound_listAllSampleTimes);
		praat_addAction1 (classSound, 1, U"Get sample number from time...", nullptr, 2, REAL_Sound_getIndexFromTime);
		praat_addAction1 (classSound, 1,   U"Get index from time...", U"*Get sample number from time...", praat_DEPTH_2 | praat_DEPRECATED_2004, REAL_Sound_getIndexFromTime);
		praat_addAction1 (classSound, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get value at time...", nullptr, 1, REAL_Sound_getValueAtTime);
		praat_addAction1 (classSound, 1, U"Get value at sample number...", nullptr, 1, REAL_Sound_getValueAtIndex);
		praat_addAction1 (classSound, 1,   U"Get value at index...", U"*Get value at sample number...", praat_DEPTH_1 | praat_DEPRECATED_2004, REAL_Sound_getValueAtIndex);
		praat_addAction1 (classSound, 1, U"-- get shape --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get minimum...", nullptr, 1, REAL_Sound_getMinimum);
		praat_addAction1 (classSound, 1, U"Get time of minimum...", nullptr, 1, REAL_Sound_getTimeOfMinimum);
		praat_addAction1 (classSound, 1, U"Get maximum...", nullptr, 1, REAL_Sound_getMaximum);
		praat_addAction1 (classSound, 1, U"Get time of maximum...", nullptr, 1, REAL_Sound_getTimeOfMaximum);
		praat_addAction1 (classSound, 1, U"Get absolute extremum...", nullptr, 1, REAL_Sound_getAbsoluteExtremum);
		praat_addAction1 (classSound, 1, U"Get nearest zero crossing...", nullptr, 1, REAL_Sound_getNearestZeroCrossing);
		praat_addAction1 (classSound, 1, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get mean...", nullptr, 1, REAL_Sound_getMean);
		praat_addAction1 (classSound, 1, U"Get root-mean-square...", nullptr, 1, REAL_Sound_getRootMeanSquare);
		praat_addAction1 (classSound, 1, U"Get standard deviation...", nullptr, 1, REAL_Sound_getStandardDeviation);
		praat_addAction1 (classSound, 1, U"-- get energy --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get energy...", nullptr, 1, REAL_Sound_getEnergy);
		praat_addAction1 (classSound, 1, U"Get power...", nullptr, 1, REAL_Sound_getPower);
		praat_addAction1 (classSound, 1, U"-- get energy in air --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get energy in air", nullptr, 1, REAL_Sound_getEnergyInAir);
		praat_addAction1 (classSound, 1, U"Get power in air", nullptr, 1, REAL_Sound_getPowerInAir);
		praat_addAction1 (classSound, 1, U"Get intensity (dB)", nullptr, 1, REAL_Sound_getIntensity_dB);
	praat_addAction1 (classSound, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classSound);
		praat_addAction1 (classSound, 0, U"-- modify generic --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Reverse", nullptr, 1, MODIFY_Sound_reverse);
		praat_addAction1 (classSound, 0, U"Formula...", nullptr, 1, MODIFY_Sound_formula);
		praat_addAction1 (classSound, 0, U"Formula (part)...", nullptr, 1, MODIFY_Sound_formula_part);
		praat_addAction1 (classSound, 0, U"-- add & mul --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Add...", nullptr, 1, MODIFY_Sound_add);
		praat_addAction1 (classSound, 0, U"Subtract mean", nullptr, 1, MODIFY_Sound_subtractMean);
		praat_addAction1 (classSound, 0, U"Multiply...", nullptr, 1, MODIFY_Sound_multiply);
		praat_addAction1 (classSound, 0, U"Multiply by window...", nullptr, 1, MODIFY_Sound_multiplyByWindow);
		praat_addAction1 (classSound, 0, U"Scale peak...", nullptr, 1, MODIFY_Sound_scalePeak);
		praat_addAction1 (classSound, 0,   U"Scale...", nullptr, praat_DEPTH_1 | praat_DEPRECATED_2004, MODIFY_Sound_scalePeak);
		praat_addAction1 (classSound, 0, U"Scale intensity...", nullptr, 1, MODIFY_Sound_scaleIntensity);
		praat_addAction1 (classSound, 0, U"-- set --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Set value at sample number...", nullptr, 1, MODIFY_Sound_setValueAtIndex);
		praat_addAction1 (classSound, 0,   U"Set value at index...", U"*Set value at sample number...", praat_DEPTH_1 | praat_DEPRECATED_2004, MODIFY_Sound_setValueAtIndex);
		praat_addAction1 (classSound, 0, U"Set part to zero...", nullptr, 1, MODIFY_Sound_setPartToZero);
		praat_addAction1 (classSound, 0, U"-- modify hack --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Override sampling frequency...", nullptr, 1, MODIFY_Sound_overrideSamplingFrequency);
		praat_addAction1 (classSound, 0,   U"Override sample rate...", U"*Override sampling frequency...", praat_DEPTH_1 | praat_DEPRECATED_2004, MODIFY_Sound_overrideSamplingFrequency);
		praat_addAction1 (classSound, 0, U"-- in-place filters --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"In-place filters", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Filter with one formant (in-place)...", nullptr, 2, MODIFY_Sound_filterWithOneFormantInplace);
		praat_addAction1 (classSound, 0, U"Filter with one formant (in-line)...", nullptr, praat_DEPTH_2 | praat_DEPRECATED_2017, MODIFY_Sound_filterWithOneFormantInplace);
		praat_addAction1 (classSound, 0, U"Pre-emphasize (in-place)...", nullptr, 2, MODIFY_Sound_preemphasizeInplace);
		praat_addAction1 (classSound, 0, U"Pre-emphasize (in-line)...", nullptr, praat_DEPTH_2 | praat_DEPRECATED_2017, MODIFY_Sound_preemphasizeInplace);
		praat_addAction1 (classSound, 0, U"De-emphasize (in-place)...", nullptr, 2, MODIFY_Sound_deemphasizeInplace);
		praat_addAction1 (classSound, 0, U"De-emphasize (in-line)...", nullptr, praat_DEPTH_2 | praat_DEPRECATED_2017, MODIFY_Sound_deemphasizeInplace);
	praat_addAction1 (classSound, 0, U"Annotate -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"Annotation tutorial", nullptr, 1, HELP_AnnotationTutorial);
		praat_addAction1 (classSound, 0, U"-- to text grid --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To TextGrid...", nullptr, 1, NEW_Sound_to_TextGrid);
		praat_addAction1 (classSound, 0, U"To TextTier", nullptr, praat_DEPTH_1 | praat_HIDDEN, NEW_Sound_to_TextTier);
		praat_addAction1 (classSound, 0, U"To IntervalTier", nullptr, praat_DEPTH_1 | praat_HIDDEN, NEW_Sound_to_IntervalTier);
	praat_addAction1 (classSound, 0, U"Analyse periodicity -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"To Pitch...", nullptr, 1, NEW_Sound_to_Pitch);
		praat_addAction1 (classSound, 0, U"To Pitch (ac)...", nullptr, 1, NEW_Sound_to_Pitch_ac);
		praat_addAction1 (classSound, 0, U"To Pitch (cc)...", nullptr, 1, NEW_Sound_to_Pitch_cc);
		praat_addAction1 (classSound, 0, U"To PointProcess (periodic, cc)...", nullptr, 1, NEW_Sound_to_PointProcess_periodic_cc);
		praat_addAction1 (classSound, 0, U"To PointProcess (periodic, peaks)...", nullptr, 1, NEW_Sound_to_PointProcess_periodic_peaks);
		praat_addAction1 (classSound, 0, U"-- points --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To PointProcess (extrema)...", nullptr, 1, NEW_Sound_to_PointProcess_extrema);
		praat_addAction1 (classSound, 0, U"To PointProcess (zeroes)...", nullptr, 1, NEW_Sound_to_PointProcess_zeroes);
		praat_addAction1 (classSound, 0, U"-- hnr --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To Harmonicity (cc)...", nullptr, 1, NEW_Sound_to_Harmonicity_cc);
		praat_addAction1 (classSound, 0, U"To Harmonicity (ac)...", nullptr, 1, NEW_Sound_to_Harmonicity_ac);
		praat_addAction1 (classSound, 0, U"To Harmonicity (gne)...", nullptr, 1, NEW_Sound_to_Harmonicity_gne);
		praat_addAction1 (classSound, 0, U"-- autocorrelation --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Autocorrelate...", nullptr, 1, NEW_Sound_autoCorrelate);
	praat_addAction1 (classSound, 0, U"Analyse spectrum -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"To Spectrum...", nullptr, 1, NEW_Sound_to_Spectrum);
		praat_addAction1 (classSound, 0,   U"To Spectrum (fft)", U"*To Spectrum...", praat_DEPTH_1 | praat_DEPRECATED_2004, NEW_Sound_to_Spectrum_fft);
		praat_addAction1 (classSound, 0,   U"To Spectrum", U"*To Spectrum...", praat_DEPTH_1 | praat_DEPRECATED_2004, NEW_Sound_to_Spectrum_fft);
		praat_addAction1 (classSound, 0,   U"To Spectrum (dft)", U"*To Spectrum...", praat_DEPTH_1 | praat_DEPRECATED_2004, NEW_Sound_to_Spectrum_dft);
		praat_addAction1 (classSound, 0, U"To Ltas...", nullptr, 1, NEW_Sound_to_Ltas);
		praat_addAction1 (classSound, 0, U"To Ltas (pitch-corrected)...", nullptr, 1, NEW_Sound_to_Ltas_pitchCorrected);
		praat_addAction1 (classSound, 0, U"-- spectrotemporal --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To Spectrogram...", nullptr, 1, NEW_Sound_to_Spectrogram);
		praat_addAction1 (classSound, 0, U"To Cochleagram...", nullptr, 1, NEW_Sound_to_Cochleagram);
		praat_addAction1 (classSound, 0, U"To Cochleagram (edb)...", nullptr, praat_DEPTH_1 | praat_HIDDEN, NEW_Sound_to_Cochleagram_edb);
		praat_addAction1 (classSound, 0, U"-- formants --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To Formant (burg)...", nullptr, 1, NEW_Sound_to_Formant_burg);
		praat_addAction1 (classSound, 0, U"To Formant (hack)", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To Formant (keep all)...", nullptr, 2, NEW_Sound_to_Formant_keepAll);
		praat_addAction1 (classSound, 0, U"To Formant (sl)...", nullptr, 2, NEW_Sound_to_Formant_willems);
	praat_addAction1 (classSound, 0, U"To Intensity...", nullptr, 0, NEW_Sound_to_Intensity);
	praat_addAction1 (classSound, 0, U"To IntensityTier...", nullptr, praat_HIDDEN, NEW_Sound_to_IntensityTier);
	praat_addAction1 (classSound, 0, U"Manipulate -", nullptr, 0, nullptr);
	praat_addAction1 (classSound, 0, U"To Manipulation...", nullptr, 1, NEW_Sound_to_Manipulation);
	praat_addAction1 (classSound, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"Convert to mono", nullptr, 1, NEW_Sound_convertToMono);
		praat_addAction1 (classSound, 0, U"Convert to stereo", nullptr, 1, NEW_Sound_convertToStereo);
		praat_addAction1 (classSound, 0, U"Extract all channels", nullptr, 1, NEWMANY_Sound_extractAllChannels);
		praat_addAction1 (classSound, 0, U"Extract one channel...", nullptr, 1, NEW_Sound_extractChannel);
		praat_addAction1 (classSound, 0,   U"Extract left channel", U"*Extract one channel...", praat_DEPTH_1 | praat_DEPRECATED_2010, NEW_Sound_extractLeftChannel);
		praat_addAction1 (classSound, 0,   U"Extract right channel", U"*Extract one channel...", praat_DEPTH_1 | praat_DEPRECATED_2010, NEW_Sound_extractRightChannel);
		praat_addAction1 (classSound, 0, U"Extract channels...", nullptr, 1, NEW_Sound_extractChannels);
		praat_addAction1 (classSound, 0, U"Extract part...", nullptr, 1, NEW_Sound_extractPart);
		praat_addAction1 (classSound, 0, U"Extract part for overlap...", nullptr, 1, NEW_Sound_extractPartForOverlap);
		praat_addAction1 (classSound, 0, U"Resample...", nullptr, 1, NEW_Sound_resample);
		praat_addAction1 (classSound, 0, U"-- enhance --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Lengthen (overlap-add)...", nullptr, 1, NEW_Sound_lengthen_overlapAdd);
		praat_addAction1 (classSound, 0,   U"Lengthen (PSOLA)...", U"*Lengthen (overlap-add)...", praat_DEPTH_1 | praat_DEPRECATED_2007, NEW_Sound_lengthen_overlapAdd);
		praat_addAction1 (classSound, 0, U"Deepen band modulation...", nullptr, 1, NEW_Sound_deepenBandModulation);
		praat_addAction1 (classSound, 0, U"-- cast --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Down to Matrix", nullptr, 1, NEW_Sound_downto_Matrix);
	praat_addAction1 (classSound, 0, U"Filter -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"Filtering tutorial", nullptr, 1, HELP_FilteringTutorial);
		praat_addAction1 (classSound, 0, U"-- frequency-domain filter --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Filter (pass Hann band)...", nullptr, 1, NEW_Sound_filter_passHannBand);
		praat_addAction1 (classSound, 0, U"Filter (stop Hann band)...", nullptr, 1, NEW_Sound_filter_stopHannBand);
		praat_addAction1 (classSound, 0, U"Filter (formula)...", nullptr, 1, NEW_Sound_filter_formula);
		praat_addAction1 (classSound, 0, U"-- time-domain filter --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Filter (one formant)...", nullptr, 1, NEW_Sound_filter_oneFormant);
		praat_addAction1 (classSound, 0, U"Filter (pre-emphasis)...", nullptr, 1, NEW_Sound_filter_preemphasis);
		praat_addAction1 (classSound, 0, U"Filter (de-emphasis)...", nullptr, 1, NEW_Sound_filter_deemphasis);
	praat_addAction1 (classSound, 0, U"Combine -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"Combine to stereo", nullptr, 1, NEW1_Sounds_combineToStereo);
		praat_addAction1 (classSound, 0, U"Combine into SoundList", nullptr, 1, NEW1_Sounds_combineIntoSoundList);
		praat_addAction1 (classSound, 0, U"Combine into SoundSet", nullptr, 1, NEW1_Sounds_combineIntoSoundSet);
		praat_addAction1 (classSound, 0, U"Concatenate", nullptr, 1, NEW1_Sounds_concatenate);
		praat_addAction1 (classSound, 0, U"Concatenate recoverably", nullptr, 1, NEW2_Sounds_concatenateRecoverably);
		praat_addAction1 (classSound, 0, U"Concatenate with overlap...", nullptr, 1, NEW1_Sounds_concatenateWithOverlap);
		praat_addAction1 (classSound, 2, U"Convolve...", nullptr, 1, NEW1_Sounds_convolve);
		praat_addAction1 (classSound, 2,   U"Convolve", U"*Convolve...", praat_DEPTH_1 | praat_DEPRECATED_2010, NEW1_Sounds_convolve_old);
		praat_addAction1 (classSound, 2, U"Cross-correlate...", nullptr, 1, NEW1_Sounds_crossCorrelate);
		praat_addAction1 (classSound, 2, U"To ParamCurve", nullptr, 1, NEW1_Sounds_to_ParamCurve);

	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as WAV file...", nullptr, 0, SAVE_LongSound_Sound_saveAsWavFile);
	praat_addAction2 (classLongSound, 0, classSound, 0,   U"Write to WAV file...", U"*Save as WAV file...", praat_DEPRECATED_2011, SAVE_LongSound_Sound_saveAsWavFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as AIFF file...", nullptr, 0, SAVE_LongSound_Sound_saveAsAiffFile);
	praat_addAction2 (classLongSound, 0, classSound, 0,   U"Write to AIFF file...", U"*Save as AIFF file...", praat_DEPRECATED_2011, SAVE_LongSound_Sound_saveAsAiffFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as AIFC file...", nullptr, 0, SAVE_LongSound_Sound_saveAsAifcFile);
	praat_addAction2 (classLongSound, 0, classSound, 0,   U"Write to AIFC file...", U"*Save as AIFC file...", praat_DEPRECATED_2011, SAVE_LongSound_Sound_saveAsAifcFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as NeXT/Sun file...", nullptr, 0, SAVE_LongSound_Sound_saveAsNextSunFile);
	praat_addAction2 (classLongSound, 0, classSound, 0,   U"Write to NeXT/Sun file...", U"*Save as NeXT/Sun file...", praat_DEPRECATED_2011, SAVE_LongSound_Sound_saveAsNextSunFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as NIST file...", nullptr, 0, SAVE_LongSound_Sound_saveAsNistFile);
	praat_addAction2 (classLongSound, 0, classSound, 0,   U"Write to NIST file...", U"*Save as NIST file...", praat_DEPRECATED_2011, SAVE_LongSound_Sound_saveAsNistFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as FLAC file...", nullptr, 0, SAVE_LongSound_Sound_saveAsFlacFile);
	praat_addAction2 (classLongSound, 0, classSound, 0,   U"Write to FLAC file...", U"*Save as FLAC file...", praat_DEPRECATED_2011, SAVE_LongSound_Sound_saveAsFlacFile);

	praat_addAction1 (classSoundList, 1, U"Extract all Sounds", nullptr, 0, NEWMANY_SoundList_extractAllSounds);

	praat_addAction1 (classSoundSet, 1, U"Extract all Sounds", nullptr, 0, NEWMANY_SoundSet_extractAllSounds);
	praat_addAction2 (classSoundSet, 1, classTable, 1, U"Get randomized patterns...", nullptr, 0, NEW2_SoundSet_Table_getRandomizedPatterns);
}

/* End of file praat_Sound.cpp */
