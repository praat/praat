/* praat_Sound.cpp
 *
 * Copyright (C) 1992-2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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

/***** LONGSOUND *****/

DIRECT (INFO_NONE__LongSound_concatenate) {
	INFO_NONE
		Melder_information (U"To concatenate LongSound objects, "
			"select them in the list\nand choose \"Save as WAV file...\" or a similar command.\n"
			"The result will be a sound file that contains\nthe concatenation of the selected sounds."
		);
	INFO_NONE_END
}

FORM (NEW_LongSound_extractPart, U"LongSound: Extract part", U"LongSound: Extract part...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"1.0")
	BOOLEAN (preserveTimes, U"Preserve times", true)
	OK
DO
	CONVERT_EACH_TO_ONE (LongSound)
		autoSound result = LongSound_extractPart (me, fromTime, toTime, preserveTimes);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (REAL_LongSound_getIndexFromTime, U"LongSound: Get sample index from time", U"Sound: Get index from time...") {
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	QUERY_ONE_FOR_REAL (LongSound)
		const double result = Sampled_xToIndex (me, time);
	QUERY_ONE_FOR_REAL_END (U" (index at ", time, U" seconds)")
}

DIRECT (REAL_LongSound_getSamplePeriod) {
	QUERY_ONE_FOR_REAL (LongSound)
		const double result = my dx;
	QUERY_ONE_FOR_REAL_END (U" seconds");
}

DIRECT (REAL_LongSound_getSampleRate) {
	QUERY_ONE_FOR_REAL (LongSound)
		const double result = 1.0 / my dx;
	QUERY_ONE_FOR_REAL_END (U" Hz")
}

FORM (REAL_LongSound_getTimeFromIndex, U"LongSound: Get time from sample index", U"Sound: Get time from index...") {
	INTEGER (sampleIndex, U"Sample index", U"100")
	OK
DO
	QUERY_ONE_FOR_REAL (LongSound)
		const double result = Sampled_indexToX (me, sampleIndex);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

DIRECT (INTEGER_LongSound_getNumberOfSamples) {
	QUERY_ONE_FOR_REAL (LongSound)
		const integer result = my nx;
	QUERY_ONE_FOR_REAL_END (U" samples")
}

DIRECT (HELP__LongSound_help) {
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
	PLAY_EACH (LongSound)
		LongSound_playPart (me, fromTime, toTime, nullptr, nullptr);
	PLAY_EACH_END
}

FORM (SAVE_ONE__LongSound_savePartAsAudioFile, U"LongSound: Save part as audio file", nullptr) {
	OUTFILE (audioFile, U"Audio file", U"")
	CHOICE (type, U"Type", 3)
	{ int i; for (i = 1; i <= Melder_NUMBER_OF_AUDIO_FILE_TYPES; i ++) {
		OPTION (Melder_audioFileTypeString (i))
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
	CONVERT_EACH_TO_ONE (LongSound)
		autoTextGrid result = TextGrid_create (my xmin, my xmax, tierNames, pointTiers);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (EDITOR_ONE__LongSound_view) {
	EDITOR_ONE (a,LongSound)
		autoSoundEditor editor = SoundEditor_create (ID_AND_FULL_NAME, me);
	EDITOR_ONE_END
}

FORM_SAVE (SAVE_ALL__LongSound_saveAsAifcFile, U"Save as AIFC file", nullptr, U"aifc") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_AIFC, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ALL__LongSound_saveAsAiffFile, U"Save as AIFF file", nullptr, U"aiff") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_AIFF, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ALL__LongSound_saveAsNextSunFile, U"Save as NeXT/Sun file", nullptr, U"au") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_NEXT_SUN, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ALL__LongSound_saveAsNistFile, U"Save as NIST file", nullptr, U"nist") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_NIST, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ALL__LongSound_saveAsFlacFile, U"Save as FLAC file", nullptr, U"flac") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_FLAC, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ALL__LongSound_saveAsWavFile, U"Save as WAV file", nullptr, U"wav") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_WAV, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ONE__LongSound_saveLeftChannelAsAifcFile, U"Save left channel as AIFC file", nullptr, U"aifc") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_AIFC, 0, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__LongSound_saveLeftChannelAsAiffFile, U"Save left channel as AIFF file", nullptr, U"aiff") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_AIFF, 0, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__LongSound_saveLeftChannelAsNextSunFile, U"Save left channel as NeXT/Sun file", nullptr, U"au") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_NEXT_SUN, 0, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__LongSound_saveLeftChannelAsNistFile, U"Save left channel as NIST file", nullptr, U"nist") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_NIST, 0, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__LongSound_saveLeftChannelAsFlacFile, U"Save left channel as FLAC file", nullptr, U"flac") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_FLAC, 0, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__LongSound_saveLeftChannelAsWavFile, U"Save left channel as WAV file", nullptr, U"wav") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_WAV, 0, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__LongSound_saveRightChannelAsAifcFile, U"Save right channel as AIFC file", nullptr, U"aifc") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_AIFC, 1, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__LongSound_saveRightChannelAsAiffFile, U"Save right channel as AIFF file", nullptr, U"aiff") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_AIFF, 1, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__LongSound_saveRightChannelAsNextSunFile, U"Save right channel as NeXT/Sun file", nullptr, U"au") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_NEXT_SUN, 1, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__LongSound_saveRightChannelAsNistFile, U"Save right channel as NIST file", nullptr, U"nist") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_NIST, 1, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__LongSound_saveRightChannelAsFlacFile, U"Save right channel as FLAC file", 0, U"flac") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_FLAC, 1, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__LongSound_saveRightChannelAsWavFile, U"Save right channel as WAV file", 0, U"wav") {
	SAVE_ONE (LongSound)
		LongSound_saveChannelAsAudioFile (me, Melder_WAV, 1, file);
	SAVE_ONE_END
}

FORM (SETTINGS__LongSoundSettings, U"LongSound settings", U"LongSound") {
	COMMENT (U"This setting determines the maximum number of seconds")
	COMMENT (U"for viewing the waveform and playing a sound in the LongSound window.")
	COMMENT (U"The LongSound window can become very slow if you set it too high.")
	NATURAL (maximumViewablePart, U"Maximum viewable part (seconds)", U"60")
	COMMENT (U"Note: this setting works for the next long sound file that you open,")
	COMMENT (U"not for currently existing LongSound objects.")
OK
	SET_INTEGER (maximumViewablePart, LongSound_getBufferSizePref_seconds ())
DO
	PREFS
		LongSound_setBufferSizePref_seconds (maximumViewablePart);
	PREFS_END
}

/********** LONGSOUND & SOUND **********/

FORM_SAVE (SAVE_ALL__LongSound_Sound_saveAsAifcFile, U"Save as AIFC file", nullptr, U"aifc") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_AIFC, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ALL__LongSound_Sound_saveAsAiffFile, U"Save as AIFF file", nullptr, U"aiff") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_AIFF, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ALL__LongSound_Sound_saveAsNextSunFile, U"Save as NeXT/Sun file", nullptr, U"au") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_NEXT_SUN, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ALL__LongSound_Sound_saveAsNistFile, U"Save as NIST file", nullptr, U"nist") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_NIST, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ALL__LongSound_Sound_saveAsFlacFile, U"Save as FLAC file", nullptr, U"flac") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_FLAC, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ALL__LongSound_Sound_saveAsWavFile, U"Save as WAV file", nullptr, U"wav") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_WAV, 16);
	SAVE_ALL_LISTED_END
}

/********** SOUND **********/

FORM (MODIFY_Sound_add, U"Sound: Add", nullptr) {
	COMMENT (U"The following number will be added to the amplitudes of ")
	COMMENT (U"all samples of the sound.")
	REAL (number, U"Number", U"0.1")
	OK
DO
	MODIFY_EACH (Sound)
		Vector_addScalar (me, number);
	MODIFY_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__Sound_autoCorrelate, U"Sound: autocorrelate", U"Sound: Autocorrelate...") {
	CHOICE_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	CHOICE_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
 	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_autoCorrelate (me, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_EACH_TO_ONE_END (U"ac_", my name.get())
}

DIRECT (COMBINE_ALL_TO_ONE__Sounds_combineToStereo) {
	COMBINE_ALL_TO_ONE (Sound)
		autoSound result = Sounds_combineToStereo (& list);
		const integer numberOfChannels = result -> ny;   // dereference before transferring
	COMBINE_ALL_TO_ONE_END (U"combined_", numberOfChannels)
}

DIRECT (COMBINE_ALL_TO_ONE__Sounds_combineIntoSoundList) {
	COMBINE_ALL_TO_ONE (Sound)
		autoSoundList result = SoundList_create ();
		for (integer iobject = 1; iobject <= list.size; iobject ++)
			result -> addItem_move (Data_copy (list.at [iobject]));
	COMBINE_ALL_TO_ONE_END (U"list")
}

DIRECT (COMBINE_ALL_TO_ONE__Sounds_combineIntoSoundSet) {
	COMBINE_ALL_TO_ONE (Sound)
		autoSoundSet result = SoundSet_create ();
		for (integer iobject = 1; iobject <= list.size; iobject ++)
			result -> addItem_move (Data_copy (list.at [iobject]));
	COMBINE_ALL_TO_ONE_END (U"ensemble")
}

DIRECT (COMBINE_ALL_TO_ONE__Sounds_concatenate) {
	COMBINE_ALL_LISTED_TO_ONE (Sound, SoundList)
		autoSound result = Sounds_concatenate (list.get(), 0.0);
	COMBINE_ALL_LISTED_TO_ONE_END (U"chain")
}

FORM (COMBINE_ALL_TO_ONE__Sounds_concatenateWithOverlap, U"Sounds: Concatenate with overlap", U"Sounds: Concatenate with overlap...") {
	POSITIVE (overlap, U"Overlap (s)", U"0.01")
	OK
DO
	COMBINE_ALL_LISTED_TO_ONE (Sound, SoundList)
		autoSound result = Sounds_concatenate (list.get(), overlap);
	COMBINE_ALL_LISTED_TO_ONE_END (U"chain")
}

DIRECT (CONVERT_ALL_TO_MULTIPLE__Sounds_concatenateRecoverably) {
	CONVERT_ALL_LISTED_TO_MULTIPLE (Sound, SoundList)
		integer numberOfChannels = 0, nx = 0, iinterval = 0;
		double dx = 0.0, tmin = 0.0;
		for (integer isound = 1; isound <= list->size; isound ++) {
			Sound me = list->at [isound];
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
		for (integer isound = 1; isound <= list->size; isound ++) {
			Sound me = list->at [isound];
			const double tmax = tmin + my nx * dx;
			thy z.verticalBand (nx + 1, nx + my nx)  <<=  my z.all();
			iinterval ++;
			if (iinterval > 1)
				TextGrid_insertBoundary (him.get(), 1, tmin);
			TextGrid_setIntervalText (him.get(), 1, iinterval, my name.get());
			nx += my nx;
			tmin = tmax;
		}
		praat_new (thee.move(), U"chain");
		praat_new (him.move(), U"chain");
	CONVERT_ALL_LISTED_TO_MULTIPLE_END
}

DIRECT (CONVERT_EACH_TO_ONE__Sound_convertToMono) {
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_convertToMono (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_mono")
}

DIRECT (CONVERT_EACH_TO_ONE__Sound_convertToStereo) {
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_convertToStereo (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_stereo")
}

DIRECT (CONVERT_TWO_TO_ONE__Sounds_convolve_old) {
	CONVERT_TWO_TO_ONE (Sound)
		autoSound result = Sounds_convolve (me, you,
			kSounds_convolve_scaling::SUM, kSounds_convolve_signalOutsideTimeDomain::ZERO);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

FORM (CONVERT_TWO_TO_ONE__Sounds_convolve, U"Sounds: Convolve", U"Sounds: Convolve...") {
	CHOICE_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	CHOICE_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO
	CONVERT_TWO_TO_ONE (Sound)
		autoSound result = Sounds_convolve (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

static autoSound common_Sound_create (integer numberOfChannels, double startTime, double endTime,
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
	return sound;
}

FORM (CREATE_ONE__Sound_create, U"Create mono Sound", U"Create Sound from formula...") {
	WORD (name, U"Name", U"sineWithNoise")
	REAL (startTime, U"Start time (s)", U"0.0")
	REAL (endTime, U"End time (s)", U"1.0")
	REAL (samplingFrequency, U"Sampling frequency (Hz)", U"44100")
	FORMULA (formula, U"Formula", U"1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)")
	OK
DO
	CREATE_ONE
		autoSound result = common_Sound_create (1, startTime, endTime, samplingFrequency, formula, interpreter);
	CREATE_ONE_END (name)
}

FORM (CREATE_ONE__Sound_createFromFormula, U"Create Sound from formula", U"Create Sound from formula...") {
	WORD (name, U"Name", U"sineWithNoise")
	CHANNEL (numberOfChannels, U"Number of channels", U"1 (= mono)")
	REAL (startTime, U"Start time (s)", U"0.0")
	REAL (endTime, U"End time (s)", U"1.0")
	REAL (samplingFrequency, U"Sampling frequency (Hz)", U"44100")
	FORMULA (formula, U"Formula", U"1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)")
	OK
DO
	CREATE_ONE
		autoSound result = common_Sound_create (numberOfChannels, startTime, endTime, samplingFrequency, formula, interpreter);
	CREATE_ONE_END (name)
}

FORM (CREATE_ONE__Sound_createAsPureTone, U"Create Sound as pure tone", U"Create Sound as pure tone...") {
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

FORM (CREATE_ONE__Sound_createAsToneComplex, U"Create Sound as tone complex", U"Create Sound as tone complex...") {
	WORD (name, U"Name", U"toneComplex")
	REAL (startTime, U"Start time (s)", U"0.0")
	REAL (endTime, U"End time (s)", U"1.0")
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	CHOICEx (phase, U"Phase", 2, Sound_TONE_COMPLEX_SINE)
		OPTION (U"sine")
		OPTION (U"cosine")
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

FORM (CONVERT_TWO_TO_ONE__old_Sounds_crossCorrelate, U"Cross-correlate (short)", nullptr) {
	REAL (fromLag, U"From lag (s)", U"-0.1")
	REAL (toLag, U"To lag (s)", U"0.1")
	BOOLEAN (normalize, U"Normalize", true)
	OK
DO
	CONVERT_TWO_TO_ONE (Sound)
		autoSound result = Sounds_crossCorrelate_short (me, you, fromLag, toLag, normalize);
	CONVERT_TWO_TO_ONE_END (U"cc_", my name.get(), U"_", your name.get())
}

FORM (CONVERT_TWO_TO_ONE__Sounds_crossCorrelate, U"Sounds: Cross-correlate", U"Sounds: Cross-correlate...") {
	CHOICE_ENUM (kSounds_convolve_scaling, amplitudeScaling,
			U"Amplitude scaling", kSounds_convolve_scaling::DEFAULT)
	CHOICE_ENUM (kSounds_convolve_signalOutsideTimeDomain, signalOutsideTimeDomainIs,
			U"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain::DEFAULT)
	OK
DO_ALTERNATIVE (CONVERT_TWO_TO_ONE__old_Sounds_crossCorrelate)
	CONVERT_TWO_TO_ONE (Sound)
		autoSound result = Sounds_crossCorrelate (me, you, amplitudeScaling, signalOutsideTimeDomainIs);
	CONVERT_TWO_TO_ONE_END (U"cc_", my name.get(), U"_", your name.get())
}

FORM (MODIFY_Sound_deemphasizeInplace, U"Sound: De-emphasize (in-place)", U"Sound: De-emphasize (in-place)...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"50.0")
	OK
DO
	MODIFY_EACH (mutableSound)
		Sound_deEmphasize_inplace (me, fromFrequency);
		Vector_scale (me, 0.99);
	MODIFY_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__Sound_deepenBandModulation, U"Deepen band modulation", U"Sound: Deepen band modulation...") {
	POSITIVE (enhancement, U"Enhancement (dB)", U"20.0")
	POSITIVE (fromFrequency, U"From frequency (Hz)", U"300.0")
	POSITIVE (toFrequency, U"To frequency (Hz)", U"8000.0")
	POSITIVE (slowModulation, U"Slow modulation (Hz)", U"3.0")
	POSITIVE (fastModulation, U"Fast modulation (Hz)", U"30.0")
	POSITIVE (bandSmoothing, U"Band smoothing (Hz)", U"100.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_deepenBandModulation (me, enhancement, fromFrequency, toFrequency,
				slowModulation, fastModulation, bandSmoothing);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", Melder_roundTowardsZero (enhancement))
}

FORM (GRAPHICS_EACH__old_Sound_draw, U"Sound: Draw", nullptr) {
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

FORM (GRAPHICS_EACH__Sound_draw, U"Sound: Draw", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range", U"0.0 (= all)")
	REAL (fromAmplitude, U"left Amplitude range", U"0.0")
	REAL (toAmplitude, U"right Amplitude range", U"0.0 (= auto)")
	BOOLEAN (garnish, U"Garnish", true)
	COMMENT (U"")
	OPTIONMENUSTR (drawingMethod, U"Drawing method", 1)
		OPTION (U"curve")
		OPTION (U"bars")
		OPTION (U"poles")
		OPTION (U"speckles")
	OK
DO_ALTERNATIVE (GRAPHICS_EACH__old_Sound_draw)
	GRAPHICS_EACH (Sound)
		Sound_draw (me, GRAPHICS, fromTime, toTime,
				fromAmplitude, toAmplitude, garnish, drawingMethod);
	GRAPHICS_EACH_END
}

static void cb_SoundEditor_publication (Editor /* me */, autoDaata publication) {
	/*
		Keep the gate for error handling.
	*/
	try {
		const bool isaSpectrum = Thing_isa (publication.get(), classSpectrum);
		praat_new (publication.move(), U"");
		praat_updateSelection ();
		if (isaSpectrum) {
			integer IOBJECT;
			FIND_ONE_WITH_IOBJECT (Spectrum)
			autoSpectrumEditor editor2 = SpectrumEditor_create (ID_AND_FULL_NAME, me);
			praat_installEditor (editor2.get(), IOBJECT);
			editor2.releaseToUser();
		}
	} catch (MelderError) {
		Melder_flushError ();
	}
}
DIRECT (EDITOR_ONE__Sound_viewAndEdit) {
	EDITOR_ONE (a,Sound)
		autoSoundEditor editor = SoundEditor_create (ID_AND_FULL_NAME, me);
		Editor_setPublicationCallback (editor.get(), cb_SoundEditor_publication);
	EDITOR_ONE_END
}

DIRECT (CONVERT_ONE_TO_MULTIPLE__Sound_extractAllChannels) {
	CONVERT_ONE_TO_MULTIPLE (Sound)
		for (integer channel = 1; channel <= my ny; channel ++) {
			autoSound thee = Sound_extractChannel (me, channel);
			praat_new (thee.move(), my name.get(), U"_ch", channel);
		}
	CONVERT_ONE_TO_MULTIPLE_END
}

FORM (CONVERT_EACH_TO_ONE__Sound_extractChannel, U"Sound: Extract channel", nullptr) {
	CHANNEL (channel, U"Channel (number, Left, or Right)", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_extractChannel (me, channel);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_ch", channel)
}

FORM (CONVERT_EACH_TO_ONE__Sound_extractChannels, U"Sound: Extract channels", nullptr) {
	NATURALVECTOR (channels, U"Channel numbers", RANGES_, U"1:64")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_extractChannels (me, channels);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_ch")
}

DIRECT (CONVERT_EACH_TO_ONE__Sound_extractLeftChannel) {
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_extractChannel (me, 1);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_left")
}

FORM (CONVERT_EACH_TO_ONE__Sound_extractPart, U"Sound: Extract part", U"Sound: Extract part...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.1")
	OPTIONMENU_ENUM (kSound_windowShape, windowShape, U"Window shape", kSound_windowShape::DEFAULT)
	POSITIVE (relativeWidth, U"Relative width", U"1.0")
	BOOLEAN (preserveTimes, U"Preserve times", false)
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_extractPart (me, fromTime, toTime,
				windowShape, relativeWidth, preserveTimes);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_part")
}

FORM (CONVERT_EACH_TO_ONE__Sound_extractPartForOverlap, U"Sound: Extract part for overlap", nullptr) {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.1")
	POSITIVE (overlap, U"Overlap (s)", U"0.01")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_extractPartForOverlap (me, fromTime, toTime, overlap);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_part")
}

DIRECT (CONVERT_EACH_TO_ONE__Sound_extractRightChannel) {
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_extractChannel (me, 2);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_right")
}

FORM (CONVERT_EACH_TO_ONE__Sound_filter_deemphasis, U"Sound: Filter (de-emphasis)", U"Sound: Filter (de-emphasis)...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_filter_deemphasis (me, fromFrequency);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_deemp")
}

FORM (CONVERT_EACH_TO_ONE__Sound_filter_formula, U"Sound: Filter (formula)...", U"Formula...") {
	COMMENT (U"Frequency-domain filtering with a formula")
	COMMENT (U"(uses Sound-to-Spectrum and Spectrum-to-Sound): x is frequency in hertz")
	FORMULA (formula, U"Formula", U"if x<500 or x>1000 then 0 else self fi; rectangular band filter")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_filter_formula (me, formula, interpreter);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_filt")
}

FORM (CONVERT_EACH_TO_ONE__Sound_filter_oneFormant, U"Sound: Filter (one formant)", U"Sound: Filter (one formant)...") {
	REAL (frequency, U"Frequency (Hz)", U"1000.0")
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"100.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_filter_oneFormant (me, frequency, bandwidth);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_filt")
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

FORM (CONVERT_EACH_TO_ONE__Sound_filter_passHannBand, U"Sound: Filter (pass Hann band)", U"Sound: Filter (pass Hann band)...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"500.0")
	REAL (toFrequency, U"To frequency (Hz)", U"1000.0")
	POSITIVE (smoothing, U"Smoothing (Hz)", U"100.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_filter_passHannBand (me, fromFrequency, toFrequency, smoothing);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_band")
}

FORM (CONVERT_EACH_TO_ONE__Sound_filter_preemphasis, U"Sound: Filter (pre-emphasis)", U"Sound: Filter (pre-emphasis)...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_filter_preemphasis (me, fromFrequency);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_preemp")
}

FORM (CONVERT_EACH_TO_ONE__Sound_filter_stopHannBand, U"Sound: Filter (stop Hann band)", U"Sound: Filter (stop Hann band)...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"500.0")
	REAL (toFrequency, U"To frequency (Hz)", U"1000.0")
	POSITIVE (smoothing, U"Smoothing (Hz)", U"100.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_filter_stopHannBand (me, fromFrequency, toFrequency, smoothing);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_band")
}

FORM (MODIFY_Sound_formula, U"Sound: Formula", U"Sound: Formula...") {
	COMMENT (U"# `x` is the time in seconds, `col` is the sample number.")
	COMMENT (U"x = x1   ! time associated with first sample")
	COMMENT (U"for col from 1 to ncol")
	COMMENT (U"   self [col] = ...")
	FORMULA (formula, U"Formula", U"self")
	COMMENT (U"   x = x + dx")
	COMMENT (U"endfor")
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
	FORMULA (formula, U"Formula", U"2 * self")
	OK
DO
	MODIFY_EACH_WEAK (Sound)
		Matrix_formula_part (me, fromTime, toTime, fromChannel - 0.5, toChannel + 0.5,
				formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (QUERY_ONE_FOR_REAL__Sound_getAbsoluteExtremum, U"Sound: Get absolute extremum", U"Sound: Get absolute extremum...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation::SINC70)
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Vector_getAbsoluteExtremum (me, fromTime, toTime, peakInterpolationType);
	QUERY_ONE_FOR_REAL_END (U" Pascal")
}

FORM (QUERY_ONE_FOR_REAL__Sound_getEnergy, U"Sound: Get energy", U"Sound: Get energy...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Sound_getEnergy (me, fromTime, toTime);
	QUERY_ONE_FOR_REAL_END (U" Pa2 sec")
}

DIRECT (QUERY_ONE_FOR_REAL__Sound_getEnergyInAir) {
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Sound_getEnergyInAir (me);
	QUERY_ONE_FOR_REAL_END (U" Joule/m2")
}

FORM (QUERY_ONE_FOR_REAL__Sound_getIndexFromTime, U"Get sample number from time", U"Get sample number from time...") {
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Sampled_xToIndex (me, time);
	QUERY_ONE_FOR_REAL_END (U" (index at time ", time, U" seconds)")
}

DIRECT (QUERY_ONE_FOR_REAL__Sound_getIntensity_dB) {
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Sound_getIntensity_dB (me);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (QUERY_ONE_FOR_REAL__Sound_getMaximum, U"Sound: Get maximum", U"Sound: Get maximum...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation::SINC70)
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Vector_getMaximum (me, fromTime, toTime, peakInterpolationType);
	QUERY_ONE_FOR_REAL_END (U" Pascal")
}

FORM (QUERY_ONE_FOR_REAL__old_Sound_getMean, U"Sound: Get mean", U"Sound: Get mean...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Vector_getMean (me, fromTime, toTime, Vector_CHANNEL_AVERAGE);
	QUERY_ONE_FOR_REAL_END (U" Pascal")
}

FORM (QUERY_ONE_FOR_REAL__Sound_getMean, U"Sound: Get mean", U"Sound: Get mean...") {
	CHANNEL (channel, U"Channel", U"0 (= all)")
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO_ALTERNATIVE (QUERY_ONE_FOR_REAL__old_Sound_getMean)
	QUERY_ONE_FOR_REAL (Sound)
		if (channel > my ny) channel = 1;
		const double result = Vector_getMean (me, fromTime, toTime, channel);
	QUERY_ONE_FOR_REAL_END (U" Pascal")
}

FORM (QUERY_ONE_FOR_REAL__Sound_getMinimum, U"Sound: Get minimum", U"Sound: Get minimum...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation::SINC70)
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Vector_getMinimum (me, fromTime, toTime, peakInterpolationType);
	QUERY_ONE_FOR_REAL_END (U" Pascal")
}

FORM (QUERY_ONE_FOR_REAL__old_Sound_getNearestZeroCrossing, U"Sound: Get nearest zero crossing", U"Sound: Get nearest zero crossing...") {
	REAL (time, U"Time (s)", U"0.5")
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		if (my ny > 1)
			Melder_throw (U"Cannot determine a zero crossing for a stereo sound.");
		const double result = Sound_getNearestZeroCrossing (me, time, 1);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

FORM (QUERY_ONE_FOR_REAL__Sound_getNearestZeroCrossing, U"Sound: Get nearest zero crossing", U"Sound: Get nearest zero crossing...") {
	CHANNEL (channel, U"Channel (number, Left, or Right)", U"1")
	REAL (time, U"Time (s)", U"0.5")
	OK
DO_ALTERNATIVE (QUERY_ONE_FOR_REAL__old_Sound_getNearestZeroCrossing)
	QUERY_ONE_FOR_REAL (Sound)
		Melder_require (channel >= 1,
			U"The channel number should be at least 1.");
		Melder_require (channel <= my ny,
			U"The channel number should be at most the number of channels (", my ny, U").");
		const double result = Sound_getNearestZeroCrossing (me, time, channel);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

DIRECT (QUERY_ONE_FOR_INTEGER__Sound_getNumberOfChannels) {
	QUERY_ONE_FOR_INTEGER (Sound)
		const integer result = my ny;
	QUERY_ONE_FOR_INTEGER_END (result == 1 ? U" channel (mono)" : result == 2 ? U" channels (stereo)" : U" channels")
}

DIRECT (QUERY_ONE_FOR_INTEGER__Sound_getNumberOfSamples) {
	QUERY_ONE_FOR_INTEGER (Sound)
		const integer result = my nx;
	QUERY_ONE_FOR_INTEGER_END (U" samples")
}

FORM (QUERY_ONE_FOR_REAL__Sound_getPower, U"Sound: Get power", U"Sound: Get power...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Sound_getPower (me, fromTime, toTime);
	QUERY_ONE_FOR_REAL_END (U" Pa2")
}

DIRECT (QUERY_ONE_FOR_REAL__Sound_getPowerInAir) {
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Sound_getPowerInAir (me);
	QUERY_ONE_FOR_REAL_END (U" Watt/m2")
}

FORM (QUERY_ONE_FOR_REAL__Sound_getRootMeanSquare, U"Sound: Get root-mean-square", U"Sound: Get root-mean-square...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Sound_getRootMeanSquare (me, fromTime, toTime);
	QUERY_ONE_FOR_REAL_END (U" Pascal")
}

DIRECT (QUERY_ONE_FOR_REAL__Sound_getSamplePeriod) {
	QUERY_ONE_FOR_REAL (Sound)
		const double result = my dx;
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

DIRECT (QUERY_ONE_FOR_REAL__Sound_getSampleRate) {
	QUERY_ONE_FOR_REAL (Sound)
		const double result = 1.0 / my dx;
	QUERY_ONE_FOR_REAL_END (U" Hz")
}

FORM (QUERY_ONE_FOR_REAL__old_Sound_getStandardDeviation, U"Sound: Get standard deviation", U"Sound: Get standard deviation...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Vector_getStandardDeviation (me, fromTime, toTime, Vector_CHANNEL_AVERAGE);
	QUERY_ONE_FOR_REAL_END (U" Pascal")
}

FORM (QUERY_ONE_FOR_REAL__Sound_getStandardDeviation, U"Sound: Get standard deviation", U"Sound: Get standard deviation...") {
	CHANNEL (channel, U"Channel", U"0 (= average)")
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO_ALTERNATIVE (QUERY_ONE_FOR_REAL__old_Sound_getStandardDeviation)
	QUERY_ONE_FOR_REAL (Sound)
		if (channel > my ny) channel = 1;
		const double result = Vector_getStandardDeviation (me, fromTime, toTime, channel);
	QUERY_ONE_FOR_REAL_END (U" Pascal")
}

FORM (QUERY_ONE_FOR_REAL__Sound_getTimeFromIndex, U"Get time from sample number", U"Get time from sample number...") {
	INTEGER (sampleNumber, U"Sample number", U"100")
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Sampled_indexToX (me, sampleNumber);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

DIRECT (NUMVEC_Sound_listAllSampleTimes) {
	QUERY_ONE_FOR_REAL_VECTOR (Sound)
		autoVEC result = Sampled_listAllXValues (me);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (QUERY_ONE_FOR_REAL__Sound_getTimeOfMaximum, U"Sound: Get time of maximum", U"Sound: Get time of maximum...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation::SINC70)
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Vector_getXOfMaximum (me, fromTime, toTime, peakInterpolationType);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

FORM (QUERY_ONE_FOR_REAL__Sound_getTimeOfMinimum, U"Sound: Get time of minimum", U"Sound: Get time of minimum...") {
	REAL (fromTime, U"left Time range (s)", U"0.0")
	REAL (toTime, U"right Time range (s)", U"0.0 (= all)")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation::SINC70)
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Vector_getXOfMinimum (me, fromTime, toTime, peakInterpolationType);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

FORM (QUERY_ONE_FOR_REAL__old_Sound_getValueAtIndex, U"Sound: Get value at sample number", U"Sound: Get value at sample number...") {
	INTEGER (sampleNumber, U"Sample number", U"100")
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		const double result = sampleNumber < 1 || sampleNumber > my nx ? undefined :
				my ny == 1 ? my z [1] [sampleNumber] : 0.5 * (my z [1] [sampleNumber] + my z [2] [sampleNumber]);
	QUERY_ONE_FOR_REAL_END (U" Pascal")
}

FORM (QUERY_ONE_FOR_REAL__Sound_getValueAtIndex, U"Sound: Get value at sample number", U"Sound: Get value at sample number...") {
	CHANNEL (channel, U"Channel", U"0 (= average)")
	INTEGER (sampleNumber, U"Sample number", U"100")
	OK
DO_ALTERNATIVE (QUERY_ONE_FOR_REAL__old_Sound_getValueAtIndex)
	QUERY_ONE_FOR_REAL (Sound)
		if (channel > my ny) channel = 1;
		const double result = sampleNumber < 1 || sampleNumber > my nx ? undefined :
				Sampled_getValueAtSample (me, sampleNumber, channel, 0);
	QUERY_ONE_FOR_REAL_END (U" Pascal")
}

FORM (QUERY_ONE_FOR_REAL__old_Sound_getValueAtTime, U"Sound: Get value at time", U"Sound: Get value at time...") {
	REAL (time, U"Time (s)", U"0.5")
	CHOICE_ENUM (kVector_valueInterpolation, valueInterpolationType,
			U"Interpolation", kVector_valueInterpolation::SINC70)
	OK
DO
	QUERY_ONE_FOR_REAL (Sound)
		const double result = Vector_getValueAtX (me, time, Vector_CHANNEL_AVERAGE, valueInterpolationType);
	QUERY_ONE_FOR_REAL_END (U" Pascal")
}

FORM (QUERY_ONE_FOR_REAL__Sound_getValueAtTime, U"Sound: Get value at time", U"Sound: Get value at time...") {
	CHANNEL (channel, U"Channel", U"0 (= average)")
	REAL (time, U"Time (s)", U"0.5")
	CHOICE_ENUM (kVector_valueInterpolation, valueInterpolationType,
			U"Interpolation", kVector_valueInterpolation::SINC70)
	OK
DO_ALTERNATIVE (QUERY_ONE_FOR_REAL__old_Sound_getValueAtTime)
	QUERY_ONE_FOR_REAL (Sound)
		if (channel > my ny) channel = 1;
		const double result = Vector_getValueAtX (me, time, channel, valueInterpolationType);
	QUERY_ONE_FOR_REAL_END (U" Pascal")
}

DIRECT (HELP__Sound_help) {
	HELP (U"Sound")
}

FORM (CONVERT_EACH_TO_ONE__Sound_lengthen_overlapAdd, U"Sound: Lengthen (overlap-add)", U"Sound: Lengthen (overlap-add)...") {
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	POSITIVE (factor, U"Factor", U"1.5")
	OK
DO
	Melder_require (pitchFloor < pitchCeiling,
		U"The pitch ceiling should be greater than the pitch floor.");
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_lengthen_overlapAdd (me, pitchFloor, pitchCeiling, factor);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", Melder_fixed (factor, 2));
}

FORM (MODIFY_Sound_multiply, U"Sound: Multiply", nullptr) {
	REAL (multiplicationFactor, U"Multiplication factor", U"1.5")
	OK
DO
	MODIFY_EACH (Sound)
		Vector_multiplyByScalar (me, multiplicationFactor);
	MODIFY_EACH_END
}

FORM (MODIFY_Sound_multiplyByWindow, U"Sound: Multiply by window", U"Sound: Multiply by window...") {
	OPTIONMENU_ENUM (kSound_windowShape, windowShape, U"Window shape", kSound_windowShape::HANNING)
	OK
DO
	MODIFY_EACH (mutableSound)
		Sound_multiplyByWindow (me, windowShape);
	MODIFY_EACH_END
}

FORM (MODIFY_Sound_overrideSamplingFrequency, U"Sound: Override sampling frequency", nullptr) {
	POSITIVE (newSamplingFrequency, U"New sampling frequency (Hz)", U"16000.0")
	OK
DO
	MODIFY_EACH (mutableSound)
		Sound_overrideSamplingFrequency (me, newSamplingFrequency);
	MODIFY_EACH_END
}

DIRECT (PLAY_EACH__Sound_play) {
	FIND_ALL_LISTED (Sound, SoundList)
		SoundList_play (list.get(), nullptr, nullptr);
	END_NO_NEW_DATA
}

FORM (MODIFY_Sound_preemphasizeInplace, U"Sound: Pre-emphasize (in-place)", U"Sound: Pre-emphasize (in-place)...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"50.0")
	OK
DO
	MODIFY_EACH (Sound)
		Sound_preEmphasize_inplace (me, fromFrequency);
		Vector_scale (me, 0.99);
	MODIFY_EACH_END
}

FORM_READ (READ_MULTIPLE__Sound_readSeparateChannelsFromSoundFile, U"Read separate channels from sound file", nullptr, true) {
	READ_MULTIPLE
		autoSound sound = Sound_readFromSoundFile (file);
		char32 name [300];
		Melder_sprint (name,300, MelderFile_name (file));
		char32 *lastPeriod = str32rchr (name, U'.');
		if (lastPeriod)
			*lastPeriod = U'\0';
		for (integer ichan = 1; ichan <= sound -> ny; ichan ++) {
			autoSound thee = Sound_extractChannel (sound.get(), ichan);
			praat_new (thee.move(), name, U"_ch", ichan);
		}
	READ_MULTIPLE_END
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
DIRECT (SINGLETON_CREATION_WINDOW__Sound_recordMono) {
	SINGLETON_CREATION_WINDOW (a,Sound)
		do_Sound_record (1);
	SINGLETON_CREATION_WINDOW_END
}
DIRECT (SINGLETON_CREATION_WINDOW__Sound_recordStereo) {
	SINGLETON_CREATION_WINDOW (a,Sound)
		do_Sound_record (2);
	SINGLETON_CREATION_WINDOW_END
}

FORM (RECORD_ONE__Sound_record_fixedTime, U"Record Sound", nullptr) {
	COMMENT (U"This menu command is usually hidden,")
	COMMENT (U"   because its behaviour is platform-dependent.")
	COMMENT (U"The combination of “microphone” and “48000 Hz” is likely")
	COMMENT (U"   to work on all computers.")
	COMMENT (U"The “Gain” and “Balance” settings tend to be obsolete")
	COMMENT (U"   and may not work at all on your computer.")
	CHOICE (inputSource, U"Input source", 1)
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
	RECORD_ONE
		autoSound result = Sound_record_fixedTime (inputSource,
				gain, balance, Melder_atof (samplingFrequency), duration);
	RECORD_ONE_END (U"untitled")
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

FORM (CONVERT_EACH_TO_ONE__Sound_resample, U"Sound: Resample", U"Sound: Resample...") {
	POSITIVE (newSamplingFrequency, U"New sampling frequency (Hz)", U"10000.0")
	NATURAL (precision, U"Precision (samples)", U"50")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound result = Sound_resample (me, newSamplingFrequency, precision);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", Melder_iround (newSamplingFrequency));
}

DIRECT (MODIFY_Sound_reverse) {
	MODIFY_EACH (Sound)
		Sound_reverse (me, 0.0, 0.0);
	MODIFY_EACH_END
}

FORM_SAVE (SAVE_ALL__Sound_saveAs24BitWavFile, U"Save as 24-bit WAV file", nullptr, U"wav") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_WAV, 24);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ALL__Sound_saveAs32BitWavFile, U"Save as 32-bit WAV file", nullptr, U"wav") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_WAV, 32);
	SAVE_ALL_LISTED_END
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
	CHOICEx (cut, U"Cut", 2, 0)
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

DIRECT (MODIFY_Sound_shiftTimesToBetweenZeroAndPhysicalDuration) {
	MODIFY_EACH (Sound)
		Sound_shiftTimesToBetweenZeroAndPhysicalDuration (me);
	MODIFY_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Manipulation, U"Sound: To Manipulation", U"Manipulation") {
	POSITIVE (timeStep, U"Time step (s)", U"0.01")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	OK
DO
	Melder_require (pitchCeiling > pitchFloor,
		U"The pitch ceiling should be greater than the pitch floor.");
	CONVERT_EACH_TO_ONE (Sound)
		autoManipulation result = Sound_to_Manipulation (me, timeStep, pitchFloor, pitchCeiling);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Cochleagram, U"Sound: To Cochleagram", nullptr) {
	POSITIVE (timeStep, U"Time step (s)", U"0.01")
	POSITIVE (frequencyResolution, U"Frequency resolution (Bark)", U"0.1")
	POSITIVE (windowLength, U"Window length (s)", U"0.03")
	REAL (forwardMaskingTime, U"Forward-masking time (s)", U"0.03")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoCochleagram result = Sound_to_Cochleagram (me, timeStep,
			frequencyResolution, windowLength, forwardMaskingTime);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Cochleagram_edb, U"Sound: To Cochleagram (De Boer, Meddis & Hewitt)", nullptr) {
	POSITIVE (timeStep, U"Time step (s)", U"0.01")
	POSITIVE (frequencyResolution, U"Frequency resolution (Bark)", U"0.1")
	BOOLEAN (hasSynapse, U"Has synapse", true)
	COMMENT (U"Meddis synapse properties")
	POSITIVE (replenishmentRate, U"   replenishment rate (/sec)", U"5.05")
	POSITIVE (lossRate, U"   loss rate (/sec)", U"2500.0")
	POSITIVE (returnRate, U"   return rate (/sec)", U"6580.0")
	POSITIVE (reprocessingRate, U"   reprocessing rate (/sec)", U"66.31")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoCochleagram result = Sound_to_Cochleagram_edb (me, timeStep, frequencyResolution, hasSynapse,
				replenishmentRate, lossRate, returnRate, reprocessingRate);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Formant_burg, U"Sound: To Formant (Burg method)", U"Sound: To Formant (burg)...") {
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (maximumNumberOfFormants, U"Max. number of formants", U"5.0")
	REAL (formantCeiling, U"Formant ceiling (Hz)", U"5500.0 (= adult female)")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrom, U"Pre-emphasis from (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoFormant result = Sound_to_Formant_burg (me, timeStep,
				maximumNumberOfFormants, formantCeiling, windowLength, preEmphasisFrom);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Formant_keepAll, U"Sound: To Formant (keep all)", U"Sound: To Formant (keep all)...") {
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (maximumNumberOfFormants, U"Max. number of formants", U"5.0")
	REAL (formantCeiling, U"Formant ceiling (Hz)", U"5500.0 (= adult female)")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrom, U"Pre-emphasis from (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoFormant result = Sound_to_Formant_keepAll (me, timeStep,
				maximumNumberOfFormants, formantCeiling, windowLength, preEmphasisFrom);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Formant_willems, U"Sound: To Formant (split Levinson (Willems))", U"Sound: To Formant (sl)...") {
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (numberOfFormants, U"Number of formants", U"5.0")
	REAL (formantCeiling, U"Formant ceiling (Hz)", U"5500.0 (= adult female)")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrom, U"Pre-emphasis from (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoFormant result = Sound_to_Formant_willems (me, timeStep,
				numberOfFormants, formantCeiling, windowLength, preEmphasisFrom);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Harmonicity_ac, U"Sound: To Harmonicity (ac)", U"Sound: To Harmonicity (ac)...") {
	POSITIVE (timeStep, U"Time step (s)", U"0.01")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	REAL (silenceThreshold, U"Silence threshold", U"0.1")
	POSITIVE (periodsPerWindow, U"Periods per window", U"4.5")
	OK
DO
	Melder_require (periodsPerWindow >= 3.0,
		U"The number of periods per window must be at least 3.0.");
	CONVERT_EACH_TO_ONE (Sound)
		autoHarmonicity result = Sound_to_Harmonicity_ac (me, timeStep,
				pitchFloor, silenceThreshold, periodsPerWindow);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Harmonicity_cc, U"Sound: To Harmonicity (cc)", U"Sound: To Harmonicity (cc)...") {
	POSITIVE (timeStep, U"Time step (s)", U"0.01")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	REAL (silenceThreshold, U"Silence threshold", U"0.1")
	POSITIVE (periodsPerWindow, U"Periods per window", U"1.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoHarmonicity result = Sound_to_Harmonicity_cc (me, timeStep,
				pitchFloor, silenceThreshold, periodsPerWindow);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Harmonicity_gne, U"Sound: To Harmonicity (gne)", nullptr) {
	POSITIVE (minimumFrequency, U"Minimum frequency (Hz)", U"500.0")
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"4500.0")
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"1000.0")
	POSITIVE (step, U"Step (Hz)", U"80.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoMatrix result = Sound_to_Harmonicity_GNE (me, minimumFrequency,
				maximumFrequency, bandwidth, step);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__old_Sound_to_Intensity, U"Sound: To Intensity", U"Sound: To Intensity...") {
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"100.0")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoIntensity result = Sound_to_Intensity (me,
				pitchFloor, timeStep, false);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Intensity, U"Sound: To Intensity", U"Sound: To Intensity...") {
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"100.0")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	BOOLEAN (subtractMean, U"Subtract mean", true)
	OK
DO_ALTERNATIVE (CONVERT_EACH_TO_ONE__old_Sound_to_Intensity)
	CONVERT_EACH_TO_ONE (Sound)
		autoIntensity result = Sound_to_Intensity (me,
				pitchFloor, timeStep, subtractMean);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_IntensityTier, U"Sound: To IntensityTier", nullptr) {
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"100.0")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	BOOLEAN (subtractMean, U"Subtract mean", true)
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoIntensityTier result = Sound_to_IntensityTier (me,
				pitchFloor, timeStep, subtractMean);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Sound_to_IntervalTier) {
	CONVERT_EACH_TO_ONE (Sound)
		autoIntervalTier result = IntervalTier_create (my xmin, my xmax);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Ltas, U"Sound: To long-term average spectrum", nullptr) {
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"100")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoLtas result = Sound_to_Ltas (me, bandwidth);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Ltas_pitchCorrected, U"Sound: To Ltas (pitch-corrected)", U"Sound: To Ltas (pitch-corrected)...") {
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"100.0")
	REAL (shortestPeriod, U"Shortest period (s)", U"0.0001")
	REAL (longestPeriod, U"Longest period (s)", U"0.02")
	POSITIVE (maximumPeriodFactor, U"Maximum period factor", U"1.3")
	OK
DO
	Melder_require (pitchCeiling > pitchFloor,
		U"Your pitch ceiling should be greater than your pitch floor.");
	CONVERT_EACH_TO_ONE (Sound)
		autoLtas result = Sound_to_Ltas_pitchCorrected (me, pitchFloor, pitchCeiling,
				maximumFrequency, bandwidth, shortestPeriod, longestPeriod, maximumPeriodFactor);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Sound_downto_Matrix) {
	CONVERT_EACH_TO_ONE (Sound)
		autoMatrix result = Sound_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_TWO_TO_ONE__Sounds_to_ParamCurve) {
	CONVERT_TWO_TO_ONE (Sound)
		autoParamCurve result = ParamCurve_create (me, you);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

DIRECT (HELP__How_to_choose_a_pitch_analysis_method) {
	HELP (U"how to choose a pitch analysis method")
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Pitch, U"Sound: To Pitch", U"Sound: To Pitch...") {
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoPitch result = Sound_to_Pitch (me, timeStep, pitchFloor, pitchCeiling);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Pitch_ac, U"Sound: To Pitch (ac)", U"Sound: To Pitch (ac)...") {
	COMMENT (U"Finding the candidates...")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	NATURAL (maximumNumberOfCandidates, U"Max. number of candidates", U"15")
	BOOLEAN (veryAccurate, U"Very accurate", false)
	COMMENT (U"Finding a path...")
	REAL (silenceThreshold, U"Silence threshold", U"0.03")
	REAL (voicingThreshold, U"Voicing threshold", U"0.45")
	REAL (octaveCost, U"Octave cost", U"0.01")
	REAL (octaveJumpCost, U"Octave-jump cost", U"0.35")
	REAL (voicedUnvoicedCost, U"Voiced / unvoiced cost", U"0.14")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	OK
DO
	Melder_require (maximumNumberOfCandidates > 1,
		U"Your maximum number of candidates should be greater than 1.");
	CONVERT_EACH_TO_ONE (Sound)
		autoPitch result = Sound_to_Pitch_rawAc (me,
			timeStep, pitchFloor, pitchCeiling,
			maximumNumberOfCandidates, veryAccurate,
			silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost, voicedUnvoicedCost
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}
FORM (CONVERT_EACH_TO_ONE__Sound_to_Pitch_cc, U"Sound: To Pitch (cc)", U"Sound: To Pitch (cc)...") {
	COMMENT (U"Finding the candidates...")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	NATURAL (maximumNumberOfCandidates, U"Max. number of candidates", U"15")
	BOOLEAN (veryAccurate, U"Very accurate", false)
	COMMENT (U"Finding a path...")
	REAL (silenceThreshold, U"Silence threshold", U"0.03")
	REAL (voicingThreshold, U"Voicing threshold", U"0.45")
	REAL (octaveCost, U"Octave cost", U"0.01")
	REAL (octaveJumpCost, U"Octave-jump cost", U"0.35")
	REAL (voicedUnvoicedCost, U"Voiced / unvoiced cost", U"0.14")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	OK
DO
	Melder_require (maximumNumberOfCandidates > 1,
		U"Your maximum number of candidates should be greater than 1.");
	CONVERT_EACH_TO_ONE (Sound)
		autoPitch result = Sound_to_Pitch_rawCc (me,
			timeStep, pitchFloor, pitchCeiling,
			maximumNumberOfCandidates, veryAccurate,
			silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost, voicedUnvoicedCost
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Pitch_rawAutocorrelation, U"Sound: To Pitch (raw autocorrelation)", U"Sound: To Pitch (raw autocorrelation)...") {
	HEADING (U"Where to search...")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (pitchFloor, U"left Pitch floor and ceiling (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"right Pitch floor and ceiling (Hz)", U"600.0")
	HEADING (U"How to find the candidates...")
	NATURAL (maximumNumberOfCandidates, U"Max. number of candidates", U"15")
	BOOLEAN (veryAccurate, U"Very accurate", false)
	HEADING (U"How to find a path through the candidates...")
	REAL (silenceThreshold, U"Silence threshold", U"0.03")
	REAL (voicingThreshold, U"Voicing threshold", U"0.45")
	REAL (octaveCost, U"Octave cost", U"0.01")
	REAL (octaveJumpCost, U"Octave-jump cost", U"0.35")
	REAL (voicedUnvoicedCost, U"Voiced / unvoiced cost", U"0.14")
	OK
DO
	Melder_require (maximumNumberOfCandidates > 1,
		U"Your maximum number of candidates should be greater than 1.");
	CONVERT_EACH_TO_ONE (Sound)
		autoPitch result = Sound_to_Pitch_rawAc (me,
			timeStep, pitchFloor, pitchCeiling,
			maximumNumberOfCandidates, veryAccurate,
			silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost, voicedUnvoicedCost
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Pitch_rawCrossCorrelation, U"Sound: To Pitch (raw cross-correlation)", U"Sound: To Pitch (raw cross-correlation)...") {
	HEADING (U"Where to search...")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (pitchFloor, U"left Pitch floor and ceiling (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"right Pitch floor and ceiling (Hz)", U"600.0")
	HEADING (U"How to find the candidates...")
	NATURAL (maximumNumberOfCandidates, U"Max. number of candidates", U"15")
	BOOLEAN (veryAccurate, U"Very accurate", false)
	HEADING (U"How to find a path through the candidates...")
	REAL (silenceThreshold, U"Silence threshold", U"0.03")
	REAL (voicingThreshold, U"Voicing threshold", U"0.45")
	REAL (octaveCost, U"Octave cost", U"0.01")
	REAL (octaveJumpCost, U"Octave-jump cost", U"0.35")
	REAL (voicedUnvoicedCost, U"Voiced / unvoiced cost", U"0.14")
	OK
DO
	Melder_require (maximumNumberOfCandidates > 1,
		U"Your maximum number of candidates should be greater than 1.");
	CONVERT_EACH_TO_ONE (Sound)
		autoPitch result = Sound_to_Pitch_rawCc (me,
			timeStep, pitchFloor, pitchCeiling,
			maximumNumberOfCandidates, veryAccurate,
			silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost, voicedUnvoicedCost
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Pitch_filteredAutocorrelation, U"Sound: To Pitch (filtered autocorrelation)", U"Sound: To Pitch (filtered autocorrelation)...") {
	HEADING (U"Where to search...")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (pitchFloor, U"left Pitch floor and top (Hz)", U"50.0")
	POSITIVE (pitchTop, U"right Pitch floor and top (Hz)", U"800.0")
	HEADING (U"How to find the candidates...")
	NATURAL (maximumNumberOfCandidates, U"Max. number of candidates", U"15")
	BOOLEAN (veryAccurate, U"Very accurate", false)
	HEADING (U"How to preprocess the sound...")
	POSITIVE (attenuationAtTop, U"Attenuation at top", U"0.03")
	HEADING (U"How to find a path through the candidates...")
	REAL (silenceThreshold, U"Silence threshold", U"0.09")
	REAL (voicingThreshold, U"Voicing threshold", U"0.50")
	REAL (octaveCost, U"Octave cost", U"0.055")
	REAL (octaveJumpCost, U"Octave-jump cost", U"0.35")
	REAL (voicedUnvoicedCost, U"Voiced / unvoiced cost", U"0.14")
	OK
DO
	Melder_require (maximumNumberOfCandidates > 1,
		U"Your maximum number of candidates should be greater than 1.");
	CONVERT_EACH_TO_ONE (Sound)
		autoPitch result = Sound_to_Pitch_filteredAc (me,
			timeStep, pitchFloor, pitchTop,
			maximumNumberOfCandidates, veryAccurate, attenuationAtTop,
			silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost, voicedUnvoicedCost
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Pitch_filteredCrossCorrelation, U"Sound: To Pitch (filtered cross-correlation)", U"Sound: To Pitch (filtered cross-correlation)...") {
	HEADING (U"Where to search...")
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (pitchFloor, U"left Pitch floor and top (Hz)", U"50.0")
	POSITIVE (pitchTop, U"right Pitch floor and top (Hz)", U"800.0")
	HEADING (U"How to find the candidates...")
	NATURAL (maximumNumberOfCandidates, U"Max. number of candidates", U"15")
	BOOLEAN (veryAccurate, U"Very accurate", false)
	HEADING (U"How to preprocess the sound...")
	POSITIVE (attenuationAtTop, U"Attenuation at top", U"0.03")
	HEADING (U"How to find a path through the candidates...")
	REAL (silenceThreshold, U"Silence threshold", U"0.09")
	REAL (voicingThreshold, U"Voicing threshold", U"0.50")
	REAL (octaveCost, U"Octave cost", U"0.055")
	REAL (octaveJumpCost, U"Octave-jump cost", U"0.35")
	REAL (voicedUnvoicedCost, U"Voiced / unvoiced cost", U"0.14")
	OK
DO
	Melder_require (maximumNumberOfCandidates > 1,
		U"Your maximum number of candidates should be greater than 1.");
	CONVERT_EACH_TO_ONE (Sound)
		autoPitch result = Sound_to_Pitch_filteredCc (me,
			timeStep, pitchFloor, pitchTop,
			maximumNumberOfCandidates, veryAccurate, attenuationAtTop,
			silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost, voicedUnvoicedCost
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_PointProcess_extrema, U"Sound: To PointProcess (extrema)", nullptr) {
	CHANNEL (channel, U"Channel (number, Left, or Right)", U"1")
	BOOLEAN (includeMaxima, U"Include maxima", true)
	BOOLEAN (includeMinima, U"Include minima", false)
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
		U"Interpolation", kVector_peakInterpolation::SINC70)
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoPointProcess result = Sound_to_PointProcess_extrema (me, channel > my ny ? 1 : channel,
				peakInterpolationType, includeMaxima, includeMinima);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_PointProcess_periodic_cc, U"Sound: To PointProcess (periodic, cc)", U"Sound: To PointProcess (periodic, cc)...") {
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	OK
DO
	Melder_require (pitchCeiling > pitchFloor,
		U"Your pitch ceiling should be greater than your pitch floor.");
	CONVERT_EACH_TO_ONE (Sound)
		autoPointProcess result = Sound_to_PointProcess_periodic_cc (me, pitchFloor, pitchCeiling);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_PointProcess_periodic_peaks, U"Sound: To PointProcess (periodic, peaks)", U"Sound: To PointProcess (periodic, peaks)...") {
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"75.0")
	POSITIVE (pitchCeiling, U"Pitch ceiling (Hz)", U"600.0")
	BOOLEAN (includeMaxima, U"Include maxima", true)
	BOOLEAN (includeMinima, U"Include minima", false)
	OK
DO
	Melder_require (pitchCeiling > pitchFloor,
		U"Your pitch ceiling should be greater than your pitch floor.");
	CONVERT_EACH_TO_ONE (Sound)
		autoPointProcess result = Sound_to_PointProcess_periodic_peaks (me,
				pitchFloor, pitchCeiling, includeMaxima, includeMinima);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_PointProcess_zeroes, U"Get zeroes", nullptr) {
	CHANNEL (channel, U"Channel (number, Left, or Right)", U"1")
	BOOLEAN (includeRaisers, U"Include raisers", true)
	BOOLEAN (includeFallers, U"Include fallers", false)
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoPointProcess result = Sound_to_PointProcess_zeroes (me, channel > my ny ? 1 : channel,
				includeRaisers, includeFallers);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Spectrogram, U"Sound: To Spectrogram", U"Sound: To Spectrogram...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.005")
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	POSITIVE (timeStep, U"Time step (s)", U"0.002")
	POSITIVE (frequencyStep, U"Frequency step (Hz)", U"20.0")
	CHOICE_ENUM (kSound_to_Spectrogram_windowShape, windowShape,
			U"Window shape", kSound_to_Spectrogram_windowShape::DEFAULT)
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSpectrogram result = Sound_to_Spectrogram_e (me, windowLength,
				maximumFrequency, timeStep, frequencyStep, windowShape, 8.0, 8.0);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_Spectrum, U"Sound: To Spectrum", U"Sound: To Spectrum...") {
	BOOLEAN (fast, U"Fast", true)
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSpectrum result = Sound_to_Spectrum (me, fast);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Sound_to_Spectrum_dft) {
	CONVERT_EACH_TO_ONE (Sound)
		autoSpectrum result = Sound_to_Spectrum (me, false);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Sound_to_Spectrum_fft) {
	CONVERT_EACH_TO_ONE (Sound)
		autoSpectrum result = Sound_to_Spectrum (me, true);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_TextGrid, U"Sound: To TextGrid", U"Sound: To TextGrid...") {
	SENTENCE (allTierNames, U"All tier names", U"Mary John bell")
	SENTENCE (whichOfTheseArePointTiers, U"Which of these are point tiers?", U"bell")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoTextGrid result = TextGrid_create (my xmin, my xmax, allTierNames, whichOfTheseArePointTiers);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Sound_to_TextTier) {
	CONVERT_EACH_TO_ONE (Sound)
		autoTextTier result = TextTier_create (my xmin, my xmax);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (SETTINGS__SoundRecordingSettings, U"Sound recording settings", U"SoundRecorder") {
	NATURAL (bufferSize, U"Buffer size (MB)", U"60")
	OPTIONMENU_ENUM (kMelder_inputSoundSystem, inputSoundSystem,
			U"Input sound system", kMelder_inputSoundSystem::DEFAULT)
OK
	SET_INTEGER (bufferSize, SoundRecorder_getBufferSizePref_MB ())
	SET_ENUM (inputSoundSystem, kMelder_inputSoundSystem, MelderAudio_getInputSoundSystem())
DO
	PREFS
		if (bufferSize > 1000)
			Melder_throw (U"Buffer size cannot exceed 1000 megabytes.");
		SoundRecorder_setBufferSizePref_MB (bufferSize);
		MelderAudio_setInputSoundSystem (inputSoundSystem);
	PREFS_END
}

FORM (SETTINGS__SoundPlayingSettings, U"Sound playing settings", nullptr) {
	COMMENT (U"The following determines how sounds are played.")
	COMMENT (U"Between parentheses, you find what you can do simultaneously.")
	COMMENT (U"Decrease asynchronicity if sound plays with discontinuities.")
	OPTIONMENU_ENUM (kMelder_asynchronicityLevel, maximumAsynchronicity,
			U"Maximum asynchronicity", kMelder_asynchronicityLevel::DEFAULT)
	REAL (silenceBefore, U"Silence before (s)", U"" stringize(kMelderAudio_outputSilenceBefore_DEFAULT))
	REAL (silenceAfter, U"Silence after (s)", U"" stringize(kMelderAudio_outputSilenceAfter_DEFAULT))
	OPTIONMENU_ENUM (kMelder_outputSoundSystem, outputSoundSystem,
			U"Output sound system", kMelder_outputSoundSystem::DEFAULT)
OK
	SET_ENUM (maximumAsynchronicity, kMelder_asynchronicityLevel, MelderAudio_getOutputMaximumAsynchronicity ())
	SET_REAL (silenceBefore, MelderAudio_getOutputSilenceBefore ())
	SET_REAL (silenceAfter, MelderAudio_getOutputSilenceAfter ())
	SET_ENUM (outputSoundSystem, kMelder_outputSoundSystem, MelderAudio_getOutputSoundSystem())
DO
	PREFS
		MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
		MelderAudio_setOutputMaximumAsynchronicity (maximumAsynchronicity);
		MelderAudio_setOutputSilenceBefore (silenceBefore);
		MelderAudio_setOutputSilenceAfter (silenceAfter);
		MelderAudio_setOutputSoundSystem (outputSoundSystem);
	PREFS_END
}

#ifdef HAVE_PULSEAUDIO
void pulseAudio_serverReport ();
DIRECT (INFO_NONE__Praat_reportSoundServerProperties) {
	INFO_NONE
		pulseAudio_serverReport ();
	INFO_NONE_END
}
#endif

FORM_SAVE (SAVE_ALL__Sound_saveAsAifcFile, U"Save as AIFC file", nullptr, U"aifc") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_AIFC, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ALL__Sound_saveAsAiffFile, U"Save as AIFF file", nullptr, U"aiff") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_AIFF, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ALL__Sound_saveAsFlacFile, U"Save as FLAC file", nullptr, U"flac") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_FLAC, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ONE__Sound_saveAsKayFile, U"Save as Kay sound file", nullptr, U"kay") {
	SAVE_ONE (Sound)
		Sound_saveAsKayFile (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ALL__Sound_saveAsNextSunFile, U"Save as NeXT/Sun file", nullptr, U"au") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_NEXT_SUN, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ALL__Sound_saveAsNistFile, U"Save as NIST file", nullptr, U"nist") {
	SAVE_ALL_LISTED (SampledXY, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_NIST, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ONE__Sound_saveAsRaw8bitSignedFile, U"Save as raw 8-bit signed sound file", nullptr, U"8sig") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_8_SIGNED);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__Sound_saveAsRaw8bitUnsignedFile, U"Save as raw 8-bit unsigned sound file", nullptr, U"8uns") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_8_UNSIGNED);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__Sound_saveAsRaw16bitBigEndianFile, U"Save as raw 16-bit big-endian sound file", nullptr, U"16be") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_16_BIG_ENDIAN);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__Sound_saveAsRaw16bitLittleEndianFile, U"Save as raw 16-bit little-endian sound file", nullptr, U"16le") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_16_LITTLE_ENDIAN);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__Sound_saveAsRaw24bitBigEndianFile, U"Save as raw 24-bit big-endian sound file", nullptr, U"24be") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_24_BIG_ENDIAN);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__Sound_saveAsRaw24bitLittleEndianFile, U"Save as raw 24-bit little-endian sound file", nullptr, U"24le") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_24_LITTLE_ENDIAN);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__Sound_saveAsRaw32bitBigEndianFile, U"Save as raw 32-bit big-endian sound file", nullptr, U"32be") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_32_BIG_ENDIAN);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__Sound_saveAsRaw32bitLittleEndianFile, U"Save as raw 32-bit little-endian sound file", nullptr, U"32le") {
	SAVE_ONE (Sound)
		Sound_saveAsRawSoundFile (me, file, Melder_LINEAR_32_LITTLE_ENDIAN);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ONE__Sound_saveAsSesamFile, U"Save as Sesam file", nullptr, U"sdf") {
	SAVE_ONE (Sound)
		Sound_saveAsSesamFile (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_ALL__Sound_saveAsStereoAifcFile, U"Save as stereo AIFC file", nullptr, U"aifc") {
	SAVE_ALL (Sound)
		autoSound stereo = Sounds_combineToStereo (& list);
		Sound_saveAsAudioFile (stereo.get(), file, Melder_AIFC, 16);
	SAVE_ALL_END
}

FORM_SAVE (SAVE_ALL__Sound_saveAsStereoAiffFile, U"Save as stereo AIFF file", nullptr, U"aiff") {
	SAVE_ALL (Sound)
		autoSound stereo = Sounds_combineToStereo (& list);
		Sound_saveAsAudioFile (stereo.get(), file, Melder_AIFF, 16);
	SAVE_ALL_END
}

FORM_SAVE (SAVE_ALL__Sound_saveAsStereoNextSunFile, U"Save as stereo NeXT/Sun file", nullptr, U"au") {
	SAVE_ALL (Sound)
		autoSound stereo = Sounds_combineToStereo (& list);
		Sound_saveAsAudioFile (stereo.get(), file, Melder_NEXT_SUN, 16);
	SAVE_ALL_END
}

FORM_SAVE (SAVE_ALL__Sound_saveAsStereoNistFile, U"Save as stereo NIST file", nullptr, U"nist") {
	SAVE_ALL (Sound)
		autoSound stereo = Sounds_combineToStereo (& list);
		Sound_saveAsAudioFile (stereo.get(), file, Melder_NIST, 16);
	SAVE_ALL_END
}

FORM_SAVE (SAVE_ALL__Sound_saveAsStereoFlacFile, U"Save as stereo FLAC file", nullptr, U"flac") {
	SAVE_ALL (Sound)
		autoSound stereo = Sounds_combineToStereo (& list);
		Sound_saveAsAudioFile (stereo.get(), file, Melder_FLAC, 16);
	SAVE_ALL_END
}

FORM_SAVE (SAVE_ALL__Sound_saveAsStereoWavFile, U"Save as stereo WAV file", nullptr, U"wav") {
	SAVE_ALL (Sound)
		autoSound stereo = Sounds_combineToStereo (& list);
		Sound_saveAsAudioFile (stereo.get(), file, Melder_WAV, 16);
	SAVE_ALL_END
}

FORM_SAVE (SAVE_ALL__Sound_saveAsSunAudioFile, U"Save as NeXT/Sun file", nullptr, U"au") {
	SAVE_ALL_LISTED (Sound, SoundAndLongSoundList)
		LongSound_concatenate (list.get(), file, Melder_NEXT_SUN, 16);
	SAVE_ALL_LISTED_END
}

FORM_SAVE (SAVE_ALL__Sound_saveAsWavFile, U"Save as WAV file", nullptr, U"wav") {
	SAVE_ALL_LISTED (Sound, SoundAndLongSoundList)
		//Melder_checkTrust (_interpreter_, U"save the selected Sound object(s) to the WAV file\n", file);
		LongSound_concatenate (list.get(), file, Melder_WAV, 16);
	SAVE_ALL_LISTED_END
}

/***** SOUNDLIST *****/

DIRECT (CONVERT_EACH_TO_MULTIPLE__SoundList_extractAllSounds) {
	CONVERT_EACH_TO_MULTIPLE (SoundList)
		autoSoundList result = Data_copy (me);
		result -> classInfo = classCollection;   // YUCK, in order to force automatic unpacking
		praat_new (result.move(), U"dummy");
	CONVERT_EACH_TO_MULTIPLE_END
}

/***** SOUNDSET *****/

DIRECT (CONVERT_EACH_TO_MULTIPLE__SoundSet_extractAllSounds) {
	CONVERT_EACH_TO_MULTIPLE (SoundSet)
		autoSoundSet result = Data_copy (me);
		result -> classInfo = classCollection;   // YUCK, in order to force automatic unpacking
		praat_new (result.move(), U"dummy");
	CONVERT_EACH_TO_MULTIPLE_END
}

FORM (CONVERT_ONE_AND_ONE_TO_MULTIPLE__SoundSet_Table_getRandomizedPatterns, U"SoundSet & Table: Get randomized patterns", nullptr) {
	SENTENCE (columnName, U"Column name", U"")
	NATURAL (numberOfPatterns, U"Number of patterns", U"1000")
	NATURAL (inputSize, U"Input size (number of samples)", U"8000")
	NATURAL (outputSize, U"Output size (number of classes)", U"5")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_MULTIPLE (SoundSet, Table)
		autoPatternList inputs, outputs;
		SoundSet_Table_getRandomizedPatterns (me, you, columnName, numberOfPatterns, inputSize, outputSize,
				& inputs, & outputs);
		praat_new (inputs.move(), U"inputs");
		praat_new (outputs.move(), U"outputs");
	CONVERT_ONE_AND_ONE_TO_MULTIPLE_END
}

/***** STOP *****/

DIRECT (PLAY__stopPlayingSound) {
	PLAY_NONE
		MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	PLAY_NONE_END
}

/***** Help menus *****/

DIRECT (HELP__AnnotationTutorial) {
	HELP (U"Intro 7. Annotation")
}

DIRECT (HELP__FilteringTutorial) {
	HELP (U"Filtering")
}

/***** file recognizers *****/

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
	if ((Melder_endsWith_caseAware (MelderFile_name (file), U".mp3"))
			&& mp3_recognize (nread, header))
		return Sound_readFromSoundFile (file);   // Erez Volk, May 2007
	return autoDaata ();
}

static autoDaata movieFileRecognizer (integer nread, const char * /* header */, MelderFile file) {
	conststring32 fileName = MelderFile_name (file);
	if (nread < 512 || (! Melder_endsWith_caseAware (fileName, U".mov") &&
	                    ! Melder_endsWith_caseAware (fileName, U".avi")))
		return autoDaata ();
	Melder_throw (U"This Praat version cannot open movie files.");
	return autoDaata ();
}

static autoDaata sesamFileRecognizer (integer nread, const char * /* header */, MelderFile file) {
	conststring32 fileName = MelderFile_name (file);
	if (nread < 512 || (! Melder_endsWith_caseAware (fileName, U".sdf")))
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
	return Sound_readFromAnyKayFile (file);
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
	melderSoundFromFile = Data_readFromFile (file).static_cast_move <structSound>();
	Melder_warningOn ();
	if (! melderSoundFromFile)
		return 0;
	if (! Thing_isa (melderSoundFromFile.get(), classSound)) {
		melderSoundFromFile. reset();
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

	Data_recognizeFileType (soundFileRecognizer);
	Data_recognizeFileType (movieFileRecognizer);
	Data_recognizeFileType (sesamFileRecognizer);
	Data_recognizeFileType (bellLabsFileRecognizer);
	Data_recognizeFileType (kayFileRecognizer);

	structSoundArea               :: f_preferences ();
	structSoundAnalysisArea       :: f_preferences ();

	SoundRecorder_preferences ();
	structSoundRecorder           :: f_preferences ();
	structFunctionEditor          :: f_preferences ();
	LongSound_preferences ();

	Melder_setRecordProc (recordProc);
	Melder_setRecordFromFileProc (recordFromFileProc);
	Melder_setPlayProc (playProc);
	Melder_setPlayReverseProc (playReverseProc);
	Melder_setPublishPlayedProc (publishPlayedProc);

	praat_addMenuCommand (U"Objects", U"New", U"Record mono Sound...", nullptr, GuiMenu_ATTRACTIVE | 'R' | GuiMenu_NO_API,
			SINGLETON_CREATION_WINDOW__Sound_recordMono);
	praat_addMenuCommand (U"Objects", U"New", U"Record stereo Sound...", nullptr, GuiMenu_NO_API,
			SINGLETON_CREATION_WINDOW__Sound_recordStereo);
	praat_addMenuCommand (U"Objects", U"New", U"Record Sound (fixed time)...", nullptr, GuiMenu_HIDDEN | GuiMenu_FORCE_API,
			RECORD_ONE__Sound_record_fixedTime);
	praat_addMenuCommand (U"Objects", U"New", U"Sound", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create Sound as pure tone...", nullptr, 1,
				CREATE_ONE__Sound_createAsPureTone);
		praat_addMenuCommand (U"Objects", U"New", U"Create Sound from formula...", nullptr, 1,
				CREATE_ONE__Sound_createFromFormula);
		praat_addMenuCommand (U"Objects", U"New",   U"Create Sound...", nullptr, GuiMenu_DEPTH_1 | GuiMenu_DEPRECATED_2007,
				CREATE_ONE__Sound_create);   // replace with Create Sound from formula: arg1, 1, arg2, arg3, arg4, arg5
		praat_addMenuCommand (U"Objects", U"New", U"-- create sound advanced --", nullptr, 1, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create Sound as tone complex... || Create Sound from tone complex...", nullptr, 1,
				CREATE_ONE__Sound_createAsToneComplex);   // alternative COMPATIBILITY <= 2013

	praat_addMenuCommand (U"Objects", U"Open", U"-- read sound --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Open", U"Open long sound file...", nullptr, 'L', READ1_LongSound_open);
	praat_addMenuCommand (U"Objects", U"Open", U"Read separate channels from sound file... || Read two Sounds from stereo file...", nullptr, 0,
			READ_MULTIPLE__Sound_readSeparateChannelsFromSoundFile);   // alternative COMPATIBILITY <= 2010
	praat_addMenuCommand (U"Objects", U"Open", U"Read from special sound file", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"Open", U"Read Sound from raw Alaw file...", nullptr, GuiMenu_DEPTH_1, READ1_Sound_readFromRawAlawFile);

	praat_addMenuCommand (U"Objects", U"Goodies", U"-- sound goodies --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Goodies", U"Stop playing sound", nullptr, GuiMenu_ESCAPE,
			PLAY__stopPlayingSound);
	praat_addMenuCommand (U"Objects", U"Settings", U"-- sound settings --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Settings", U"Sound recording settings... || Sound recording preferences...", nullptr, 0,
			SETTINGS__SoundRecordingSettings);   // alternative GuiMenu_DEPRECATED_2023
	praat_addMenuCommand (U"Objects", U"Settings", U"Sound playing settings... || Sound playing preferences...", nullptr, 0,
			SETTINGS__SoundPlayingSettings);   // alternative GuiMenu_DEPRECATED_2023
	praat_addMenuCommand (U"Objects", U"Settings", U"LongSound settings... || LongSound preferences...", nullptr, 0,
			SETTINGS__LongSoundSettings);   // alternative GuiMenu_DEPRECATED_2023
#ifdef HAVE_PULSEAUDIO
	praat_addMenuCommand (U"Objects", U"Technical", U"Report sound server properties", U"Report system properties", 0,
			INFO_NONE__Praat_reportSoundServerProperties);
#endif

	praat_addAction1 (classLongSound, 0, U"LongSound help", nullptr, 0,
			HELP__LongSound_help);
	praat_addAction1 (classLongSound, 1, U"View || Open", nullptr, GuiMenu_ATTRACTIVE,
			EDITOR_ONE__LongSound_view);   // alternative COMPATIBILITY <= 2011
	praat_addAction1 (classLongSound, 0, U"Play part...", nullptr, 0, PLAY_LongSound_playPart);
	praat_addAction1 (classLongSound, 1, U"Query -", nullptr, 0, nullptr);
		praat_TimeFunction_query_init (classLongSound);
		praat_addAction1 (classLongSound, 1, U"Sampling", nullptr, 1, nullptr);
		praat_addAction1 (classLongSound, 1, U"Get number of samples", nullptr, 2, INTEGER_LongSound_getNumberOfSamples);
		praat_addAction1 (classLongSound, 1, U"Get sampling period || Get sample duration || Get sample period",
				nullptr, 2, REAL_LongSound_getSamplePeriod);   // alternatives COMPATIBILITY <= 2004
		praat_addAction1 (classLongSound, 1, U"Get sampling frequency || Get sample rate",
				nullptr, 2, REAL_LongSound_getSampleRate);   // alternative COMPATIBILITY <= 2004
		praat_addAction1 (classLongSound, 1, U"-- get time discretization --", nullptr, 2, nullptr);
		praat_addAction1 (classLongSound, 1, U"Get time from sample number... || Get time from index...",
				nullptr, 2, REAL_LongSound_getTimeFromIndex);   // alternative COMPATIBILITY <= 2004
		praat_addAction1 (classLongSound, 1, U"Get sample number from time... || Get index from time...",
				nullptr, 2, REAL_LongSound_getIndexFromTime);   // alternative COMPATIBILITY <= 2004
	praat_addAction1 (classLongSound, 0, U"Annotate -", nullptr, 0, nullptr);
		praat_addAction1 (classLongSound, 0, U"Annotation tutorial", nullptr, 1,
				HELP__AnnotationTutorial);
		praat_addAction1 (classLongSound, 0, U"-- to text grid --", nullptr, 1, nullptr);
		praat_addAction1 (classLongSound, 0, U"To TextGrid...", nullptr, 1, NEW_LongSound_to_TextGrid);
	praat_addAction1 (classLongSound, 0, U"Convert to Sound", nullptr, 0, nullptr);
	praat_addAction1 (classLongSound, 0, U"Extract part...", nullptr, 0, NEW_LongSound_extractPart);
	praat_addAction1 (classLongSound, 0, U"Concatenate?", nullptr, 0,
			INFO_NONE__LongSound_concatenate);
	praat_addAction1 (classLongSound, 0, U"Save as WAV file... || Write to WAV file...",
			nullptr, 0, SAVE_ALL__LongSound_saveAsWavFile);   // alternative COMPATIBILITY <= 2011
	praat_addAction1 (classLongSound, 0, U"Save as AIFF file... || Write to AIFF file...",
			nullptr, 0, SAVE_ALL__LongSound_saveAsAiffFile);
	praat_addAction1 (classLongSound, 0, U"Save as AIFC file... || Write to AIFC file...",
			nullptr, 0, SAVE_ALL__LongSound_saveAsAifcFile);
	praat_addAction1 (classLongSound, 0, U"Save as NeXT/Sun file... || Write to NeXT/Sun file...",
			nullptr, 0, SAVE_ALL__LongSound_saveAsNextSunFile);
	praat_addAction1 (classLongSound, 0, U"Save as NIST file... || Write to NIST file...",
			nullptr, 0, SAVE_ALL__LongSound_saveAsNistFile);
	praat_addAction1 (classLongSound, 0, U"Save as FLAC file... || Write to FLAC file...",
			nullptr, 0, SAVE_ALL__LongSound_saveAsFlacFile);
	praat_addAction1 (classLongSound, 1, U"Save left channel as WAV file... || Write left channel to WAV file...",
			nullptr, 0, SAVE_ONE__LongSound_saveLeftChannelAsWavFile);
	praat_addAction1 (classLongSound, 1, U"Save left channel as AIFF file... || Write left channel to AIFF file...",
			nullptr, 0, SAVE_ONE__LongSound_saveLeftChannelAsAiffFile);
	praat_addAction1 (classLongSound, 1, U"Save left channel as AIFC file... || Write left channel to AIFC file...",
			nullptr, 0, SAVE_ONE__LongSound_saveLeftChannelAsAifcFile);
	praat_addAction1 (classLongSound, 1, U"Save left channel as NeXT/Sun file... || Write left channel to NeXT/Sun file...",
			nullptr, 0, SAVE_ONE__LongSound_saveLeftChannelAsNextSunFile);
	praat_addAction1 (classLongSound, 1, U"Save left channel as NIST file... || Write left channel to NIST file...",
			nullptr, 0, SAVE_ONE__LongSound_saveLeftChannelAsNistFile);
	praat_addAction1 (classLongSound, 1, U"Save left channel as FLAC file... || Write left channel to FLAC file...",
			nullptr, 0, SAVE_ONE__LongSound_saveLeftChannelAsFlacFile);
	praat_addAction1 (classLongSound, 1, U"Save right channel as WAV file... || Write right channel to WAV file...",
			nullptr, 0, SAVE_ONE__LongSound_saveRightChannelAsWavFile);
	praat_addAction1 (classLongSound, 1, U"Save right channel as AIFF file... || Write right channel to AIFF file...",
			nullptr, 0, SAVE_ONE__LongSound_saveRightChannelAsAiffFile);
	praat_addAction1 (classLongSound, 1, U"Save right channel as AIFC file... || Write right channel to AIFC file...",
			nullptr, 0, SAVE_ONE__LongSound_saveRightChannelAsAifcFile);
	praat_addAction1 (classLongSound, 1, U"Save right channel as NeXT/Sun file... || Write right channel to NeXT/Sun file...",
			nullptr, 0, SAVE_ONE__LongSound_saveRightChannelAsNextSunFile);
	praat_addAction1 (classLongSound, 1, U"Save right channel as NIST file... || Write right channel to NIST file...",
			nullptr, 0, SAVE_ONE__LongSound_saveRightChannelAsNistFile);
	praat_addAction1 (classLongSound, 1, U"Save right channel as FLAC file... || Write right channel to FLAC file...",
			nullptr, 0, SAVE_ONE__LongSound_saveRightChannelAsFlacFile);
	praat_addAction1 (classLongSound, 1, U"Save part as audio file... || Write part to audio file...",
			nullptr, 0, SAVE_ONE__LongSound_savePartAsAudioFile);

	praat_addAction1 (classSound, 0, U"Save as WAV file... || Write to WAV file...",
			nullptr, 0, SAVE_ALL__Sound_saveAsWavFile);   // alternative COMPATIBILITY <= 2011
	praat_addAction1 (classSound, 0, U"Save as AIFF file... || Write to AIFF file...",
			nullptr, 0, SAVE_ALL__Sound_saveAsAiffFile);
	praat_addAction1 (classSound, 0, U"Save as AIFC file... || Write to AIFC file...",
			nullptr, 0, SAVE_ALL__Sound_saveAsAifcFile);
	praat_addAction1 (classSound, 0, U"Save as NeXT/Sun file... || Write to NeXT/Sun file...",
			nullptr, 0, SAVE_ALL__Sound_saveAsNextSunFile);
	praat_addAction1 (classSound, 0, U"Save as Sun audio file... || Write to Sun audio file...",
			nullptr, GuiMenu_HIDDEN, SAVE_ALL__Sound_saveAsSunAudioFile);
	praat_addAction1 (classSound, 0, U"Save as NIST file... || Write to NIST file...",
			nullptr, 0, SAVE_ALL__Sound_saveAsNistFile);
	praat_addAction1 (classSound, 0, U"Save as FLAC file... || Write to FLAC file...",
			nullptr, 0, SAVE_ALL__Sound_saveAsFlacFile);
	praat_addAction1 (classSound, 1, U"Save as Kay sound file... || Write to Kay sound file...",
			nullptr, 0, SAVE_ONE__Sound_saveAsKayFile);
	praat_addAction1 (classSound, 1, U"Save as Sesam file... || Write to Sesam file...",
			nullptr, GuiMenu_HIDDEN, SAVE_ONE__Sound_saveAsSesamFile);
	praat_addAction1 (classSound, 0, U"Save as 24-bit WAV file...", nullptr, 0,
			SAVE_ALL__Sound_saveAs24BitWavFile);
	praat_addAction1 (classSound, 0, U"Save as 32-bit WAV file...", nullptr, 0,
			SAVE_ALL__Sound_saveAs32BitWavFile);
	praat_addAction1 (classSound, 2,   U"Write to stereo WAV file...", nullptr, GuiMenu_DEPRECATED_2007,
			SAVE_ALL__Sound_saveAsStereoWavFile);   // replace with "Combine to stereo" and "Save to WAV file..."
	praat_addAction1 (classSound, 2,   U"Write to stereo AIFF file...", nullptr, GuiMenu_DEPRECATED_2007,
			SAVE_ALL__Sound_saveAsStereoAiffFile);   // replace with "Combine to stereo" and "Save to AIFF file..."
	praat_addAction1 (classSound, 2,   U"Write to stereo AIFC file...", nullptr, GuiMenu_DEPRECATED_2007,
			SAVE_ALL__Sound_saveAsStereoAifcFile);   // replace with "Combine to stereo" and "Save to AIFC file..."
	praat_addAction1 (classSound, 2,   U"Write to stereo NeXT/Sun file...", nullptr, GuiMenu_DEPRECATED_2007,
			SAVE_ALL__Sound_saveAsStereoNextSunFile);   // replace with "Combine to stereo" and "Save to NeXT/Sun file..."
	praat_addAction1 (classSound, 2,   U"Write to stereo NIST file...", nullptr, GuiMenu_DEPRECATED_2007,
			SAVE_ALL__Sound_saveAsStereoNistFile);   // replace with "Combine to stereo" and "Save to NIST file..."
	praat_addAction1 (classSound, 2,   U"Write to stereo FLAC file...", nullptr, GuiMenu_DEPRECATED_2007,
			SAVE_ALL__Sound_saveAsStereoFlacFile);   // replace with "Combine to stereo" and "Save to FLAC file..."
	//praat_addAction1 (classSound, 1, U"Save as raw sound file", nullptr, 0, nullptr);
	praat_addAction1 (classSound, 1, U"Save as raw 8-bit signed file... || Write to raw 8-bit signed file...",
			nullptr, 0, SAVE_ONE__Sound_saveAsRaw8bitSignedFile);
	praat_addAction1 (classSound, 1, U"Save as raw 8-bit unsigned file... || Write to raw 8-bit unsigned file...",
			nullptr, 0, SAVE_ONE__Sound_saveAsRaw8bitUnsignedFile);
	praat_addAction1 (classSound, 1, U"Save as raw 16-bit big-endian file...", nullptr, 0,
			SAVE_ONE__Sound_saveAsRaw16bitBigEndianFile);
	praat_addAction1 (classSound, 1, U"Save as raw 16-bit little-endian file...", nullptr, 0,
			SAVE_ONE__Sound_saveAsRaw16bitLittleEndianFile);
	praat_addAction1 (classSound, 1, U"Save as raw 24-bit big-endian file...", nullptr, 0,
			SAVE_ONE__Sound_saveAsRaw24bitBigEndianFile);
	praat_addAction1 (classSound, 1, U"Save as raw 24-bit little-endian file...", nullptr, 0,
			SAVE_ONE__Sound_saveAsRaw24bitLittleEndianFile);
	praat_addAction1 (classSound, 1, U"Save as raw 32-bit big-endian file...", nullptr, 0,
			SAVE_ONE__Sound_saveAsRaw32bitBigEndianFile);
	praat_addAction1 (classSound, 1, U"Save as raw 32-bit little-endian file...", nullptr, 0,
			SAVE_ONE__Sound_saveAsRaw32bitLittleEndianFile);
	praat_addAction1 (classSound, 0, U"Sound help", nullptr, 0,
			HELP__Sound_help);
	praat_addAction1 (classSound, 1, U"View & Edit || Edit || Open",
			nullptr, GuiMenu_ATTRACTIVE | GuiMenu_NO_API, EDITOR_ONE__Sound_viewAndEdit);
	praat_addAction1 (classSound, 0, U"Play", nullptr, 0,
			PLAY_EACH__Sound_play);
	praat_addAction1 (classSound, 1, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"Draw...", nullptr, 1,
				GRAPHICS_EACH__Sound_draw);
	praat_addAction1 (classSound, 1, U"Query -", nullptr, 0, nullptr);
		praat_TimeFunction_query_init (classSound);
		praat_addAction1 (classSound, 1, U"Get number of channels", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__Sound_getNumberOfChannels);
		praat_addAction1 (classSound, 1, U"Query time sampling", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get number of samples", nullptr, 2,
				QUERY_ONE_FOR_INTEGER__Sound_getNumberOfSamples);
		praat_addAction1 (classSound, 1, U"Get sampling period || Get sample duration || Get sample period",
				nullptr, 2, QUERY_ONE_FOR_REAL__Sound_getSamplePeriod);   // alternatives COMPATIBILITY <= 2004
		praat_addAction1 (classSound, 1, U"Get sampling frequency || Get sample rate",
				nullptr, 2, QUERY_ONE_FOR_REAL__Sound_getSampleRate);   // alternative COMPATIBILITY <= 2004
		praat_addAction1 (classSound, 1, U"-- get time discretization --", nullptr, 2, nullptr);
		praat_addAction1 (classSound, 1, U"Get time from sample number... || Get time from index...",
				nullptr, 2, QUERY_ONE_FOR_REAL__Sound_getTimeFromIndex);   // alternative COMPATIBILITY <= 2004
		praat_addAction1 (classSound, 1, U"List all sample times || Get sample times",
				nullptr, 2, NUMVEC_Sound_listAllSampleTimes);   // alternative GuiMenu_DEPRECATED_2004
		praat_addAction1 (classSound, 1, U"Get sample number from time... || Get index from time...",
				nullptr, 2, QUERY_ONE_FOR_REAL__Sound_getIndexFromTime);   // alternative COMPATIBILITY <= 2004
		praat_addAction1 (classSound, 1, U"-- get content --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get value at time...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Sound_getValueAtTime);
		praat_addAction1 (classSound, 1, U"Get value at sample number... || Get value at index...",
				nullptr, 1, QUERY_ONE_FOR_REAL__Sound_getValueAtIndex);   // alternative COMPATIBILITY <= 2004
		praat_addAction1 (classSound, 1, U"-- get shape --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get minimum...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Sound_getMinimum);
		praat_addAction1 (classSound, 1, U"Get time of minimum...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Sound_getTimeOfMinimum);
		praat_addAction1 (classSound, 1, U"Get maximum...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Sound_getMaximum);
		praat_addAction1 (classSound, 1, U"Get time of maximum...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Sound_getTimeOfMaximum);
		praat_addAction1 (classSound, 1, U"Get absolute extremum...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Sound_getAbsoluteExtremum);
		praat_addAction1 (classSound, 1, U"Get nearest zero crossing...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Sound_getNearestZeroCrossing);
		praat_addAction1 (classSound, 1, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get mean...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Sound_getMean);
		praat_addAction1 (classSound, 1, U"Get root-mean-square...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Sound_getRootMeanSquare);
		praat_addAction1 (classSound, 1, U"Get standard deviation...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Sound_getStandardDeviation);
		praat_addAction1 (classSound, 1, U"-- get energy --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get energy...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Sound_getEnergy);
		praat_addAction1 (classSound, 1, U"Get power...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Sound_getPower);
		praat_addAction1 (classSound, 1, U"-- get energy in air --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 1, U"Get energy in air", nullptr, 1,
				QUERY_ONE_FOR_REAL__Sound_getEnergyInAir);
		praat_addAction1 (classSound, 1, U"Get power in air", nullptr, 1,
				QUERY_ONE_FOR_REAL__Sound_getPowerInAir);
		praat_addAction1 (classSound, 1, U"Get intensity (dB)", nullptr, 1,
				QUERY_ONE_FOR_REAL__Sound_getIntensity_dB);
	praat_addAction1 (classSound, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classSound);
		praat_addAction1 (classSound, 0, U"-- modify generic --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Reverse", nullptr, 1, MODIFY_Sound_reverse);
		praat_addAction1 (classSound, 0, U"Formula...", nullptr, 1, MODIFY_Sound_formula);
		praat_addAction1 (classSound, 0, U"Formula (part)...", nullptr, 1, MODIFY_Sound_formula_part);
		praat_addAction1 (classSound, 0, U"-- add & mul --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Add...", nullptr, 1, MODIFY_Sound_add);
		praat_addAction1 (classSound, 0, U"Shift times to between zero and physical duration", U"Shift times to...", 3, MODIFY_Sound_shiftTimesToBetweenZeroAndPhysicalDuration);
		praat_addAction1 (classSound, 0, U"Subtract mean", nullptr, 1, MODIFY_Sound_subtractMean);
		praat_addAction1 (classSound, 0, U"Multiply...", nullptr, 1, MODIFY_Sound_multiply);
		praat_addAction1 (classSound, 0, U"Multiply by window...", nullptr, 1, MODIFY_Sound_multiplyByWindow);
		praat_addAction1 (classSound, 0, U"Scale peak... || Scale...",
				nullptr, 1, MODIFY_Sound_scalePeak);   // alternative COMPATIBILITY <= 2004
		praat_addAction1 (classSound, 0, U"Scale intensity...", nullptr, 1, MODIFY_Sound_scaleIntensity);
		praat_addAction1 (classSound, 0, U"-- set --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Set value at sample number... || Set value at index...",
				nullptr, 1, MODIFY_Sound_setValueAtIndex);   // alternative COMPATIBILITY <= 2004
		praat_addAction1 (classSound, 0, U"Set part to zero...", nullptr, 1, MODIFY_Sound_setPartToZero);
		praat_addAction1 (classSound, 0, U"-- modify hack --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Override sampling frequency... || Override sample rate...",
				nullptr, 1, MODIFY_Sound_overrideSamplingFrequency);   // alternative COMPATIBILITY <= 2004
		praat_addAction1 (classSound, 0, U"-- in-place filters --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"In-place filters", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Filter with one formant (in-place)... || Filter with one formant (in-line)...",
				nullptr, 2, MODIFY_Sound_filterWithOneFormantInplace);   // alternative COMPATIBILITY <= 2017
		praat_addAction1 (classSound, 0, U"Pre-emphasize (in-place)... || Pre-emphasize (in-line)...",
				nullptr, 2, MODIFY_Sound_preemphasizeInplace);
		praat_addAction1 (classSound, 0, U"De-emphasize (in-place)... || De-emphasize (in-line)...",
				nullptr, 2, MODIFY_Sound_deemphasizeInplace);
	praat_addAction1 (classSound, 0, U"Annotate -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"Annotation tutorial", nullptr, 1,
				HELP__AnnotationTutorial);
		praat_addAction1 (classSound, 0, U"-- to text grid --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To TextGrid...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_TextGrid);
		praat_addAction1 (classSound, 0, U"To TextTier", nullptr, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
				CONVERT_EACH_TO_ONE__Sound_to_TextTier);
		praat_addAction1 (classSound, 0, U"To IntervalTier", nullptr, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
				CONVERT_EACH_TO_ONE__Sound_to_IntervalTier);
	praat_addAction1 (classSound, 0, U"Analyse periodicity -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"How to choose a pitch analysis method", nullptr, 1,
				HELP__How_to_choose_a_pitch_analysis_method);
		praat_addAction1 (classSound, 0, U"-- pitch --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To Pitch...", nullptr, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
				CONVERT_EACH_TO_ONE__Sound_to_Pitch);
		praat_addAction1 (classSound, 0, U"To Pitch (filtered autocorrelation)... || To Pitch (filtered ac)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_Pitch_filteredAutocorrelation);
		praat_addAction1 (classSound, 0, U"To Pitch (raw cross-correlation)... || To Pitch (raw cc)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_Pitch_rawCrossCorrelation);
		praat_addAction1 (classSound, 0, U"To Pitch (raw autocorrelation)... || To Pitch (raw ac)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_Pitch_rawAutocorrelation);
		praat_addAction1 (classSound, 0, U"To Pitch (filtered cross-correlation)... || To Pitch (filtered cc)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_Pitch_filteredCrossCorrelation);
		praat_addAction1 (classSound, 0, U"To Pitch (ac)...", nullptr, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
				CONVERT_EACH_TO_ONE__Sound_to_Pitch_ac);
		praat_addAction1 (classSound, 0, U"To Pitch (cc)...", nullptr, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
				CONVERT_EACH_TO_ONE__Sound_to_Pitch_cc);

		praat_addAction1 (classSound, 0, U"To PointProcess (periodic, cc)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_PointProcess_periodic_cc);
		praat_addAction1 (classSound, 0, U"To PointProcess (periodic, peaks)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_PointProcess_periodic_peaks);
		praat_addAction1 (classSound, 0, U"-- points --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To PointProcess (extrema)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_PointProcess_extrema);
		praat_addAction1 (classSound, 0, U"To PointProcess (zeroes)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_PointProcess_zeroes);
		praat_addAction1 (classSound, 0, U"-- hnr --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To Harmonicity (cc)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_Harmonicity_cc);
		praat_addAction1 (classSound, 0, U"To Harmonicity (ac)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_Harmonicity_ac);
		praat_addAction1 (classSound, 0, U"To Harmonicity (gne)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_Harmonicity_gne);
		praat_addAction1 (classSound, 0, U"-- autocorrelation --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Autocorrelate...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_autoCorrelate);
	praat_addAction1 (classSound, 0, U"Analyse spectrum -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"To Spectrum...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_Spectrum);
		praat_addAction1 (classSound, 0,   U"To Spectrum (fft)", nullptr, GuiMenu_DEPTH_1 | GuiMenu_DEPRECATED_2004,
				CONVERT_EACH_TO_ONE__Sound_to_Spectrum_fft);   // replace with To Spectrum: 1
		praat_addAction1 (classSound, 0,   U"To Spectrum", nullptr, GuiMenu_DEPTH_1 | GuiMenu_DEPRECATED_2004,
				CONVERT_EACH_TO_ONE__Sound_to_Spectrum_fft);   // replace with To Spectrum: 1
		praat_addAction1 (classSound, 0,   U"To Spectrum (dft)", nullptr, GuiMenu_DEPTH_1 | GuiMenu_DEPRECATED_2004,
				CONVERT_EACH_TO_ONE__Sound_to_Spectrum_dft);   // replace with To Spectrum: 0
		praat_addAction1 (classSound, 0, U"To Ltas...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_Ltas);
		praat_addAction1 (classSound, 0, U"To Ltas (pitch-corrected)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_Ltas_pitchCorrected);
		praat_addAction1 (classSound, 0, U"-- spectrotemporal --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To Spectrogram...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_Spectrogram);
		praat_addAction1 (classSound, 0, U"To Cochleagram...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_Cochleagram);
		praat_addAction1 (classSound, 0, U"To Cochleagram (edb)...", nullptr, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
				CONVERT_EACH_TO_ONE__Sound_to_Cochleagram_edb);
		praat_addAction1 (classSound, 0, U"-- formants --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To Formant (burg)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_Formant_burg);
		praat_addAction1 (classSound, 0, U"To Formant (hack)", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"To Formant (keep all)...", nullptr, 2,
				CONVERT_EACH_TO_ONE__Sound_to_Formant_keepAll);
		praat_addAction1 (classSound, 0, U"To Formant (sl)...", nullptr, 2,
				CONVERT_EACH_TO_ONE__Sound_to_Formant_willems);
	praat_addAction1 (classSound, 0, U"To Intensity...", nullptr, 0,
			CONVERT_EACH_TO_ONE__Sound_to_Intensity);
	praat_addAction1 (classSound, 0, U"To IntensityTier...", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Sound_to_IntensityTier);
	praat_addAction1 (classSound, 0, U"Manipulate -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"To Manipulation...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_to_Manipulation);
	praat_addAction1 (classSound, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"Convert to mono", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_convertToMono);
		praat_addAction1 (classSound, 0, U"Convert to stereo", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_convertToStereo);
		praat_addAction1 (classSound, 0, U"Extract all channels", nullptr, 1,
				CONVERT_ONE_TO_MULTIPLE__Sound_extractAllChannels);
		praat_addAction1 (classSound, 0, U"Extract one channel...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_extractChannel);
		praat_addAction1 (classSound, 0,   U"Extract left channel", nullptr, GuiMenu_DEPTH_1 | GuiMenu_DEPRECATED_2010,
				CONVERT_EACH_TO_ONE__Sound_extractLeftChannel);   // replace with Extract one channel: 1
		praat_addAction1 (classSound, 0,   U"Extract right channel", nullptr, GuiMenu_DEPTH_1 | GuiMenu_DEPRECATED_2010,
				CONVERT_EACH_TO_ONE__Sound_extractRightChannel);   // replace with Extract one channel: 2
		praat_addAction1 (classSound, 0, U"Extract channels...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_extractChannels);
		praat_addAction1 (classSound, 0, U"Extract part...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_extractPart);
		praat_addAction1 (classSound, 0, U"Extract part for overlap...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_extractPartForOverlap);
		praat_addAction1 (classSound, 0, U"Resample...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_resample);
		praat_addAction1 (classSound, 0, U"-- enhance --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Lengthen (overlap-add)... || Lengthen (PSOLA)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_lengthen_overlapAdd);   // alternative COMPATIBILITY <= 2007
		praat_addAction1 (classSound, 0, U"Deepen band modulation...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_deepenBandModulation);
		praat_addAction1 (classSound, 0, U"-- cast --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Down to Matrix", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_downto_Matrix);
	praat_addAction1 (classSound, 0, U"Filter -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"Filtering tutorial", nullptr, 1,
				HELP__FilteringTutorial);
		praat_addAction1 (classSound, 0, U"-- frequency-domain filter --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Filter (pass Hann band)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_filter_passHannBand);
		praat_addAction1 (classSound, 0, U"Filter (stop Hann band)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_filter_stopHannBand);
		praat_addAction1 (classSound, 0, U"Filter (formula)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_filter_formula);
		praat_addAction1 (classSound, 0, U"-- time-domain filter --", nullptr, 1, nullptr);
		praat_addAction1 (classSound, 0, U"Filter (one formant)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_filter_oneFormant);
		praat_addAction1 (classSound, 0, U"Filter (pre-emphasis)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_filter_preemphasis);
		praat_addAction1 (classSound, 0, U"Filter (de-emphasis)...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Sound_filter_deemphasis);
	praat_addAction1 (classSound, 0, U"Combine -", nullptr, 0, nullptr);
		praat_addAction1 (classSound, 0, U"Combine to stereo", nullptr, 1,
				COMBINE_ALL_TO_ONE__Sounds_combineToStereo);
		praat_addAction1 (classSound, 0, U"Combine into SoundList", nullptr, 1,
				COMBINE_ALL_TO_ONE__Sounds_combineIntoSoundList);
		praat_addAction1 (classSound, 0, U"Combine into SoundSet", nullptr, 1,
				COMBINE_ALL_TO_ONE__Sounds_combineIntoSoundSet);
		praat_addAction1 (classSound, 0, U"Concatenate", nullptr, 1,
				COMBINE_ALL_TO_ONE__Sounds_concatenate);
		praat_addAction1 (classSound, 0, U"Concatenate recoverably", nullptr, 1,
				CONVERT_ALL_TO_MULTIPLE__Sounds_concatenateRecoverably);
		praat_addAction1 (classSound, 0, U"Concatenate with overlap...", nullptr, 1,
				COMBINE_ALL_TO_ONE__Sounds_concatenateWithOverlap);
		praat_addAction1 (classSound, 2, U"Convolve...", nullptr, 1,
				CONVERT_TWO_TO_ONE__Sounds_convolve);
		praat_addAction1 (classSound, 2,   U"Convolve", nullptr, GuiMenu_DEPTH_1 | GuiMenu_DEPRECATED_2010,
				CONVERT_TWO_TO_ONE__Sounds_convolve_old);   // replace with Convolve: "sum", "zero"
		praat_addAction1 (classSound, 2, U"Cross-correlate...", nullptr, 1,
				CONVERT_TWO_TO_ONE__Sounds_crossCorrelate);
		praat_addAction1 (classSound, 2, U"To ParamCurve", nullptr, 1,
				CONVERT_TWO_TO_ONE__Sounds_to_ParamCurve);

	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as WAV file... || Write to WAV file...",
			nullptr, 0, SAVE_ALL__LongSound_Sound_saveAsWavFile);   // alternative COMPATIBILITY <= 2011
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as AIFF file... || Write to AIFF file...",
			nullptr, 0, SAVE_ALL__LongSound_Sound_saveAsAiffFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as AIFC file... || Write to AIFC file...",
			nullptr, 0, SAVE_ALL__LongSound_Sound_saveAsAifcFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as NeXT/Sun file... || Write to NeXT/Sun file...",
			nullptr, 0, SAVE_ALL__LongSound_Sound_saveAsNextSunFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as NIST file... || Write to NIST file...",
			nullptr, 0, SAVE_ALL__LongSound_Sound_saveAsNistFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, U"Save as FLAC file... || Write to FLAC file...",
			nullptr, 0, SAVE_ALL__LongSound_Sound_saveAsFlacFile);

	praat_addAction1 (classSoundList, 1, U"Extract all Sounds", nullptr, 0,
			CONVERT_EACH_TO_MULTIPLE__SoundList_extractAllSounds);

	praat_addAction1 (classSoundSet, 1, U"Extract all Sounds", nullptr, 0,
			CONVERT_EACH_TO_MULTIPLE__SoundSet_extractAllSounds);
	praat_addAction2 (classSoundSet, 1, classTable, 1, U"Get randomized patterns...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_MULTIPLE__SoundSet_Table_getRandomizedPatterns);
}

/* End of file praat_Sound.cpp */
