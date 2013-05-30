/* praat_Sound_init.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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

#ifndef LONG_MAX
	#define LONG_MAX  2147483647
#endif

#undef iam
#define iam iam_LOOP

void praat_TimeFunction_query_init (ClassInfo klas);
void praat_TimeFunction_modify_init (ClassInfo klas);

/***** LONGSOUND *****/

DIRECT (LongSound_concatenate)
	Melder_information (L"To concatenate LongSound objects, select them in the list\nand choose \"Save as WAV file...\" or a similar command.\n"
		"The result will be a sound file that contains\nthe concatenation of the selected sounds.");
END

FORM (LongSound_extractPart, L"LongSound: Extract part", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"1.0")
	BOOLEAN (L"Preserve times", 1)
	OK
DO
	LOOP {
		iam (LongSound);
		autoSound thee = LongSound_extractPart (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Preserve times"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (LongSound_getIndexFromTime, L"LongSound: Get sample index from time", L"Sound: Get index from time...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (LongSound);
		double index = Sampled_xToIndex (me, GET_REAL (L"Time"));
		Melder_informationReal (index, NULL);
	}
END

DIRECT (LongSound_getSamplePeriod)
	LOOP {
		iam (LongSound);
		Melder_informationReal (my dx, L"seconds");
	}
END

DIRECT (LongSound_getSampleRate)
	LOOP {
		iam (LongSound);
		Melder_informationReal (1.0 / my dx, L"Hz");
	}
END

FORM (LongSound_getTimeFromIndex, L"LongSound: Get time from sample index", L"Sound: Get time from index...")
	INTEGER (L"Sample index", L"100")
	OK
DO
	LOOP {
		iam (LongSound);
		Melder_informationReal (Sampled_indexToX (me, GET_INTEGER (L"Sample index")), L"seconds");
	}
END

DIRECT (LongSound_getNumberOfSamples)
	LOOP {
		iam (LongSound);
		Melder_information (Melder_integer (my nx), L" samples");
	}
END

DIRECT (LongSound_help) Melder_help (L"LongSound"); END

FORM_READ (LongSound_open, L"Open long sound file", 0, true)
	autoLongSound me = LongSound_open (file);
	praat_new (me.transfer(), MelderFile_name (file));
END

FORM (LongSound_playPart, L"LongSound: Play part", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"10.0")
	OK
DO
	int n = 0;
	LOOP n ++;
	if (n == 1 || MelderAudio_getOutputMaximumAsynchronicity () < kMelder_asynchronicityLevel_ASYNCHRONOUS) {
		LOOP {
			iam (LongSound);
			LongSound_playPart (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), NULL, NULL);
		}
	} else {
		MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel_INTERRUPTABLE);
		LOOP {
			iam (LongSound);
			LongSound_playPart (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), NULL, NULL);
		}
		MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel_ASYNCHRONOUS);
	}
END

FORM (LongSound_writePartToAudioFile, L"LongSound: Save part as audio file", 0)
	LABEL (L"", L"Audio file:")
	TEXTFIELD (L"Audio file", L"")
	RADIO (L"Type", 3)
	{ int i; for (i = 1; i <= Melder_NUMBER_OF_AUDIO_FILE_TYPES; i ++) {
		RADIOBUTTON (Melder_audioFileTypeString (i))
	}}
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"10.0")
	OK
DO
	LOOP {
		iam (LongSound);
		structMelderFile file = { 0 };
		Melder_relativePathToFile (GET_STRING (L"Audio file"), & file);
		LongSound_writePartToAudioFile (me, GET_INTEGER (L"Type"),
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), & file);
	}
END
	
FORM (LongSound_to_TextGrid, L"LongSound: To TextGrid...", L"LongSound: To TextGrid...")
	SENTENCE (L"Tier names", L"Mary John bell")
	SENTENCE (L"Point tiers", L"bell")
	OK
DO
	LOOP {
		iam (LongSound);
		autoTextGrid thee = TextGrid_create (my xmin, my xmax, GET_STRING (L"Tier names"), GET_STRING (L"Point tiers"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (LongSound_view)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit a LongSound from batch.");
	LOOP {
		iam (LongSound);
		autoSoundEditor editor = SoundEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

FORM_WRITE (LongSound_writeToAifcFile, L"Save as AIFC file", 0, L"aifc")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_AIFC, 16);
END

FORM_WRITE (LongSound_writeToAiffFile, L"Save as AIFF file", 0, L"aiff")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_AIFF, 16);
END

FORM_WRITE (LongSound_writeToNextSunFile, L"Save as NeXT/Sun file", 0, L"au")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_NEXT_SUN, 16);
END

FORM_WRITE (LongSound_writeToNistFile, L"Save as NIST file", 0, L"nist")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_NIST, 16);
END

FORM_WRITE (LongSound_writeToFlacFile, L"Save as FLAC file", 0, L"flac")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_FLAC, 16);
END

FORM_WRITE (LongSound_writeToWavFile, L"Save as WAV file", 0, L"wav")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_WAV, 16);
END

FORM_WRITE (LongSound_writeLeftChannelToAifcFile, L"Save left channel as AIFC file", 0, L"aifc")
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_AIFC, 0, file);
	}
END

FORM_WRITE (LongSound_writeLeftChannelToAiffFile, L"Save left channel as AIFF file", 0, L"aiff")
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_AIFF, 0, file);
	}
END

FORM_WRITE (LongSound_writeLeftChannelToNextSunFile, L"Save left channel as NeXT/Sun file", 0, L"au")
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_NEXT_SUN, 0, file);
	}
END

FORM_WRITE (LongSound_writeLeftChannelToNistFile, L"Save left channel as NIST file", 0, L"nist")
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_NIST, 0, file);
	}
END

FORM_WRITE (LongSound_writeLeftChannelToFlacFile, L"Save left channel as FLAC file", 0, L"flac")
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_FLAC, 0, file);
	}
END

FORM_WRITE (LongSound_writeLeftChannelToWavFile, L"Save left channel as WAV file", 0, L"wav")
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_WAV, 0, file);
	}
END

FORM_WRITE (LongSound_writeRightChannelToAifcFile, L"Save right channel as AIFC file", 0, L"aifc")
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_AIFC, 1, file);
	}
END

FORM_WRITE (LongSound_writeRightChannelToAiffFile, L"Save right channel as AIFF file", 0, L"aiff")
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_AIFF, 1, file);
	}
END

FORM_WRITE (LongSound_writeRightChannelToNextSunFile, L"Save right channel as NeXT/Sun file", 0, L"au")
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_NEXT_SUN, 1, file);
	}
END

FORM_WRITE (LongSound_writeRightChannelToNistFile, L"Save right channel as NIST file", 0, L"nist")
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_NIST, 1, file);
	}
END

FORM_WRITE (LongSound_writeRightChannelToFlacFile, L"Save right channel as FLAC file", 0, L"flac")
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_FLAC, 1, file);
	}
END

FORM_WRITE (LongSound_writeRightChannelToWavFile, L"Save right channel as WAV file", 0, L"wav")
	LOOP {
		iam (LongSound);
		LongSound_writeChannelToAudioFile (me, Melder_WAV, 1, file);
	}
END

FORM (LongSoundPrefs, L"LongSound preferences", L"LongSound")
	LABEL (L"", L"This setting determines the maximum number of seconds")
	LABEL (L"", L"for viewing the waveform and playing a sound in the LongSound window.")
	LABEL (L"", L"The LongSound window can become very slow if you set it too high.")
	NATURAL (L"Maximum viewable part (seconds)", L"60")
	LABEL (L"", L"Note: this setting works for the next long sound file that you open,")
	LABEL (L"", L"not for currently existing LongSound objects.")
	OK
SET_INTEGER (L"Maximum viewable part", LongSound_getBufferSizePref_seconds ())
DO
	LongSound_setBufferSizePref_seconds (GET_INTEGER (L"Maximum viewable part"));
END

/********** LONGSOUND & SOUND **********/

FORM_WRITE (LongSound_Sound_writeToAifcFile, L"Save as AIFC file", 0, L"aifc")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_AIFC, 16);
END

FORM_WRITE (LongSound_Sound_writeToAiffFile, L"Save as AIFF file", 0, L"aiff")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_AIFF, 16);
END

FORM_WRITE (LongSound_Sound_writeToNextSunFile, L"Save as NeXT/Sun file", 0, L"au")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_NEXT_SUN, 16);
END

FORM_WRITE (LongSound_Sound_writeToNistFile, L"Save as NIST file", 0, L"nist")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_NIST, 16);
END

FORM_WRITE (LongSound_Sound_writeToFlacFile, L"Save as FLAC file", 0, L"flac")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_FLAC, 16);
END

FORM_WRITE (LongSound_Sound_writeToWavFile, L"Save as WAV file", 0, L"wav")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_WAV, 16);
END

/********** SOUND **********/

FORM (Sound_add, L"Sound: Add", 0)
	LABEL (L"", L"The following number will be added to the amplitudes of all samples of the sound.")
	REAL (L"Number", L"0.1")
	OK
DO
	LOOP {
		iam (Sound);
		Vector_addScalar (me, GET_REAL (L"Number"));
		praat_dataChanged (me);
	}
END

FORM (Sound_autoCorrelate, L"Sound: autocorrelate", L"Sound: Autocorrelate...")
	RADIO_ENUM (L"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (L"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
 	OK
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_autoCorrelate (me,
			GET_ENUM (kSounds_convolve_scaling, L"Amplitude scaling"),
			GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, L"Signal outside time domain is..."));
		praat_new (thee.transfer(), L"ac_", my name);
	}
END

DIRECT (Sounds_combineToStereo)
	autoCollection set = praat_getSelectedObjects ();
	autoSound result = Sounds_combineToStereo (set.peek());
	long numberOfChannels = result -> ny;   // dereference before transferring
	praat_new (result.transfer(), L"combined_", Melder_integer (numberOfChannels));
END

DIRECT (Sounds_concatenate)
	autoCollection set = praat_getSelectedObjects ();
	autoSound result = Sounds_concatenate_e (set.peek(), 0.0);
	praat_new (result.transfer(), L"chain");
END

FORM (Sounds_concatenateWithOverlap, L"Sounds: Concatenate with overlap", L"Sounds: Concatenate with overlap...")
	POSITIVE (L"Overlap (s)", L"0.01")
	OK
DO
	autoCollection set = praat_getSelectedObjects ();
	autoSound result = Sounds_concatenate_e (set.peek(), GET_REAL (L"Overlap"));
	praat_new (result.transfer(), L"chain");
END

DIRECT (Sounds_concatenateRecoverably)
	long numberOfChannels = 0, nx = 0, iinterval = 0;
	double dx = 0.0, tmin = 0.0;
	LOOP {
		iam (Sound);
		if (numberOfChannels == 0) {
			numberOfChannels = my ny;
		} else if (my ny != numberOfChannels) {
			Melder_throw ("To concatenate sounds, their numbers of channels (mono, stereo) must be equal.");
		}
		if (dx == 0.0) {
			dx = my dx;
		} else if (my dx != dx) {
			Melder_throw ("To concatenate sounds, their sampling frequencies must be equal.\n"
				"You could resample one or more of the sounds before concatenating.");
		}
		nx += my nx;
	}
	autoSound thee = Sound_create (numberOfChannels, 0.0, nx * dx, nx, dx, 0.5 * dx);
	autoTextGrid him = TextGrid_create (0.0, nx * dx, L"labels", L"");
	nx = 0;
	LOOP {
		iam (Sound);
		double tmax = tmin + my nx * dx;
		for (long channel = 1; channel <= numberOfChannels; channel ++) {
			NUMvector_copyElements (my z [channel], thy z [channel] + nx, 1, my nx);
		}
		iinterval ++;
		if (iinterval > 1) {
			TextGrid_insertBoundary (him.peek(), 1, tmin);
		}
		TextGrid_setIntervalText (him.peek(), 1, iinterval, my name);
		nx += my nx;
		tmin = tmax;
	}
	praat_new (thee.transfer(), L"chain");
	praat_new (him.transfer(), L"chain");
END

DIRECT (Sound_convertToMono)
	LOOP {
		iam (Sound);
		autoSound thee = Sound_convertToMono (me);
		praat_new (thee.transfer(), my name, L"_mono");
	}
END

DIRECT (Sound_convertToStereo)
	LOOP {
		iam (Sound);
		autoSound thee = Sound_convertToStereo (me);
		praat_new (thee.transfer(), my name, L"_stereo");
	}
END

DIRECT (Sounds_convolve_old)
	Sound s1 = NULL, s2 = NULL;
	LOOP {
		iam (Sound);
		( s1 ? s2 : s1 ) = me;
	}
	Melder_assert (s1 != NULL && s2 != NULL);
	autoSound thee = Sounds_convolve (s1, s2, kSounds_convolve_scaling_SUM, kSounds_convolve_signalOutsideTimeDomain_ZERO);
	praat_new (thee.transfer(), s1 -> name, L"_", s2 -> name);
END

FORM (Sounds_convolve, L"Sounds: Convolve", L"Sounds: Convolve...")
	RADIO_ENUM (L"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (L"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO
	Sound s1 = NULL, s2 = NULL;
	LOOP {
		iam (Sound);
		( s1 ? s2 : s1 ) = me;
	}
	Melder_assert (s1 != NULL && s2 != NULL);
	autoSound thee = Sounds_convolve (s1, s2,
		GET_ENUM (kSounds_convolve_scaling, L"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, L"Signal outside time domain is..."));
	praat_new (thee.transfer(), s1 -> name, L"_", s2 -> name);
END

static void common_Sound_create (void *dia, Interpreter interpreter, bool allowMultipleChannels) {
	long numberOfChannels = allowMultipleChannels ? GET_INTEGER (L"Number of channels") : 1;
	double startTime = GET_REAL (L"Start time");
	double endTime = GET_REAL (L"End time");
	double samplingFrequency = GET_REAL (L"Sampling frequency");
	double numberOfSamples_real = floor ((endTime - startTime) * samplingFrequency + 0.5);
	long numberOfSamples;
	if (endTime <= startTime) {
		if (endTime == startTime)
			Melder_error_ ("A Sound cannot have a duration of zero.");
		else
			Melder_error_ ("A Sound cannot have a duration less than zero.");
		if (startTime == 0.0)
			Melder_throw ("Please set the end time to something greater than 0 seconds.");
		else
			Melder_throw ("Please lower the start time or raise the end time.");
	}
	if (samplingFrequency <= 0.0) {
		Melder_error_ ("A Sound cannot have a negative sampling frequency.");
		Melder_throw ("Please set the sampling frequency to something greater than zero, e.g. 44100 Hz.");
	}
	if (numberOfChannels < 1)
		Melder_throw ("A Sound cannot have zero channels.");
	if (numberOfSamples_real < 1.0) {
		Melder_error_ ("A Sound cannot have zero samples.");
		if (startTime == 0.0)
			Melder_throw ("Please raise the end time.");
		else
			Melder_throw ("Please lower the start time or raise the end time.");
	}
	if (numberOfSamples_real > LONG_MAX) {
		Melder_error_ ("A Sound cannot have ", Melder_bigInteger (numberOfSamples_real), " samples; the maximum is ",
			Melder_bigInteger (LONG_MAX), " samples (or less, depending on your computer's memory).");
		if (startTime == 0.0)
			Melder_throw ("Please lower the end time or the sampling frequency.");
		else
			Melder_throw ("Please raise the start time, lower the end time, or lower the sampling frequency.");
	}
	numberOfSamples = (long) numberOfSamples_real;
	autoSound sound;
	try {
		sound.reset (Sound_create (numberOfChannels, startTime, endTime, numberOfSamples, 1.0 / samplingFrequency,
			startTime + 0.5 * (endTime - startTime - (numberOfSamples - 1) / samplingFrequency)));
	} catch (MelderError) {
		if (wcsstr (Melder_getError (), L"memory")) {
			Melder_clearError ();
			Melder_error_ ("There is not enough memory to create a Sound that contains ", Melder_bigInteger (numberOfSamples_real), " samples.");
			if (startTime == 0.0)
				Melder_throw ("You could lower the end time or the sampling frequency and try again.");
			else
				Melder_throw ("You could raise the start time or lower the end time or the sampling frequency, and try again.");
		} else {
			throw;   // unexpected error; wait for generic message
		}
	}
	Matrix_formula ((Matrix) sound.peek(), GET_STRING (L"formula"), interpreter, NULL);
	praat_new (sound.transfer(), GET_STRING (L"Name"));
	//praat_updateSelection ();
}

FORM (Sound_create, L"Create mono Sound", L"Create Sound from formula...")
	WORD (L"Name", L"sineWithNoise")
	REAL (L"Start time (s)", L"0.0")
	REAL (L"End time (s)", L"1.0")
	REAL (L"Sampling frequency (Hz)", L"44100")
	LABEL (L"", L"Formula:")
	TEXTFIELD (L"formula", L"1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)")
	OK
DO
	common_Sound_create (dia, interpreter, false);
END

FORM (Sound_createFromFormula, L"Create Sound from formula", L"Create Sound from formula...")
	WORD (L"Name", L"sineWithNoise")
	CHANNEL (L"Number of channels", L"1 (= mono)")
	REAL (L"Start time (s)", L"0.0")
	REAL (L"End time (s)", L"1.0")
	REAL (L"Sampling frequency (Hz)", L"44100")
	LABEL (L"", L"Formula:")
	TEXTFIELD (L"formula", L"1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)")
	OK
DO
	common_Sound_create (dia, interpreter, true);
END

FORM (Sound_createAsPureTone, L"Create Sound as pure tone", L"Create Sound as pure tone...")
	WORD (L"Name", L"tone")
	CHANNEL (L"Number of channels", L"1 (= mono)")
	REAL (L"Start time (s)", L"0.0")
	REAL (L"End time (s)", L"0.4")
	POSITIVE (L"Sampling frequency (Hz)", L"44100")
	POSITIVE (L"Tone frequency (Hz)", L"440.0")
	POSITIVE (L"Amplitude (Pa)", L"0.2")
	POSITIVE (L"Fade-in duration (s)", L"0.01")
	POSITIVE (L"Fade-out duration (s)", L"0.01")
	OK
DO
	autoSound me = Sound_createAsPureTone (GET_INTEGER (L"Number of channels"), GET_REAL (L"Start time"), GET_REAL (L"End time"),
		GET_REAL (L"Sampling frequency"), GET_REAL (L"Tone frequency"), GET_REAL (L"Amplitude"),
		GET_REAL (L"Fade-in duration"), GET_REAL (L"Fade-out duration"));
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

FORM (Sound_createFromToneComplex, L"Create Sound from tone complex", L"Create Sound from tone complex...")
	WORD (L"Name", L"toneComplex")
	REAL (L"Start time (s)", L"0.0")
	REAL (L"End time (s)", L"1.0")
	POSITIVE (L"Sampling frequency (Hz)", L"44100")
	RADIO (L"Phase", 2)
		RADIOBUTTON (L"Sine")
		RADIOBUTTON (L"Cosine")
	POSITIVE (L"Frequency step (Hz)", L"100")
	REAL (L"First frequency (Hz)", L"0 (= frequency step)")
	REAL (L"Ceiling (Hz)", L"0 (= Nyquist)")
	INTEGER (L"Number of components", L"0 (= maximum)")
	OK
DO
	autoSound me = Sound_createFromToneComplex (GET_REAL (L"Start time"), GET_REAL (L"End time"),
		GET_REAL (L"Sampling frequency"), GET_INTEGER (L"Phase") - 1, GET_REAL (L"Frequency step"),
		GET_REAL (L"First frequency"), GET_REAL (L"Ceiling"), GET_INTEGER (L"Number of components"));
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

FORM (old_Sounds_crossCorrelate, L"Cross-correlate (short)", 0)
	REAL (L"From lag (s)", L"-0.1")
	REAL (L"To lag (s)", L"0.1")
	BOOLEAN (L"Normalize", 1)
	OK
DO
	Sound s1 = NULL, s2 = NULL;
	LOOP {
		iam (Sound);
		( s1 ? s2 : s1 ) = me;
	}
	autoSound thee = Sounds_crossCorrelate_short (s1, s2, GET_REAL (L"From lag"), GET_REAL (L"To lag"), GET_INTEGER (L"Normalize"));
	praat_new (thee.transfer(), L"cc_", s1 -> name, L"_", s2 -> name);
END

FORM (Sounds_crossCorrelate, L"Sounds: Cross-correlate", L"Sounds: Cross-correlate...")
	RADIO_ENUM (L"Amplitude scaling", kSounds_convolve_scaling, DEFAULT)
	RADIO_ENUM (L"Signal outside time domain is...", kSounds_convolve_signalOutsideTimeDomain, DEFAULT)
	OK
DO_ALTERNATIVE (old_Sounds_crossCorrelate)
	Sound s1 = NULL, s2 = NULL;
	LOOP {
		iam (Sound);
		( s1 ? s2 : s1 ) = me;
	}
	Melder_assert (s1 != NULL && s2 != NULL);
	autoSound thee = Sounds_crossCorrelate (s1, s2,
		GET_ENUM (kSounds_convolve_scaling, L"Amplitude scaling"),
		GET_ENUM (kSounds_convolve_signalOutsideTimeDomain, L"Signal outside time domain is..."));
	praat_new (thee.transfer(), s1 -> name, L"_", s2 -> name);
END

FORM (Sound_deemphasizeInline, L"Sound: De-emphasize (in-line)", L"Sound: De-emphasize (in-line)...")
	REAL (L"From frequency (Hz)", L"50.0")
	OK
DO
	LOOP {
		iam (Sound);
		Sound_deEmphasis (me, GET_REAL (L"From frequency"));
		Vector_scale (me, 0.99);
		praat_dataChanged (me);
	}
END

FORM (Sound_deepenBandModulation, L"Deepen band modulation", L"Sound: Deepen band modulation...")
	POSITIVE (L"Enhancement (dB)", L"20")
	POSITIVE (L"From frequency (Hz)", L"300")
	POSITIVE (L"To frequency (Hz)", L"8000")
	POSITIVE (L"Slow modulation (Hz)", L"3")
	POSITIVE (L"Fast modulation (Hz)", L"30")
	POSITIVE (L"Band smoothing (Hz)", L"100")
	OK
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_deepenBandModulation (me, GET_REAL (L"Enhancement"),
			GET_REAL (L"From frequency"), GET_REAL (L"To frequency"),
			GET_REAL (L"Slow modulation"), GET_REAL (L"Fast modulation"), GET_REAL (L"Band smoothing"));
		praat_new (thee.transfer(), my name, L"_", Melder_integer ((long) GET_REAL (L"Enhancement")));
	}
END

FORM (old_Sound_draw, L"Sound: Draw", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range", L"0.0 (= all)")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0 (= auto)")
	BOOLEAN (L"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (Sound);
		Sound_draw (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish"), L"curve");
	}
END

FORM (Sound_draw, L"Sound: Draw", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range", L"0.0 (= all)")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0 (= auto)")
	BOOLEAN (L"Garnish", 1)
	LABEL (L"", L"")
	OPTIONMENU (L"Drawing method", 1)
		OPTION (L"Curve")
		OPTION (L"Bars")
		OPTION (L"Poles")
		OPTION (L"Speckles")
	OK
DO_ALTERNATIVE (old_Sound_draw)
	autoPraatPicture picture;
	LOOP {
		iam (Sound);
		Sound_draw (me, GRAPHICS, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"left Vertical range"), GET_REAL (L"right Vertical range"), GET_INTEGER (L"Garnish"), GET_STRING (L"Drawing method"));
	}
END

static void cb_SoundEditor_publication (Editor editor, void *closure, Data publication) {
	(void) editor;
	(void) closure;
	/*
	 * Keep the gate for error handling.
	 */
	try {
		praat_new (publication, NULL);
		praat_updateSelection ();
		if (Thing_member (publication, classSpectrum)) {
			int IOBJECT;
			LOOP {
				iam (Spectrum);
				autoSpectrumEditor editor2 = SpectrumEditor_create (ID_AND_FULL_NAME, me);
				praat_installEditor (editor2.transfer(), IOBJECT);
			}
		}
	} catch (MelderError) {
		Melder_flushError (NULL);
	}
}
DIRECT (Sound_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot view or edit a Sound from batch.");
	LOOP {
		iam (Sound);
		autoSoundEditor editor = SoundEditor_create (ID_AND_FULL_NAME, me);
		editor -> setPublicationCallback (cb_SoundEditor_publication, NULL);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

DIRECT (Sound_extractAllChannels)
	LOOP {
		iam (Sound);
		for (long channel = 1; channel <= my ny; channel ++) {
			autoSound thee = Sound_extractChannel (me, channel);
			praat_new (thee.transfer(), my name, L"_ch", Melder_integer (channel));
		}
	}
END

FORM (Sound_extractChannel, L"Sound: Extract channel", 0)
	CHANNEL (L"Channel (number, Left, or Right)", L"1")
	OK
DO
	long channel = GET_INTEGER (L"Channel");
	LOOP {
		iam (Sound);
		autoSound thee = Sound_extractChannel (me, channel);
		praat_new (thee.transfer(), my name, L"_ch", Melder_integer (channel));
	}
END

DIRECT (Sound_extractLeftChannel)
	LOOP {
		iam (Sound);
		autoSound thee = Sound_extractChannel (me, 1);
		praat_new (thee.transfer(), my name, L"_left");
	}
END

FORM (Sound_extractPart, L"Sound: Extract part", 0)
	REAL (L"left Time range (s)", L"0")
	REAL (L"right Time range (s)", L"0.1")
	OPTIONMENU_ENUM (L"Window shape", kSound_windowShape, DEFAULT)
	POSITIVE (L"Relative width", L"1.0")
	BOOLEAN (L"Preserve times", 0)
	OK
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_extractPart (me,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_ENUM (kSound_windowShape, L"Window shape"), GET_REAL (L"Relative width"),
			GET_INTEGER (L"Preserve times"));
		praat_new (thee.transfer(), my name, L"_part");
	}
END

FORM (Sound_extractPartForOverlap, L"Sound: Extract part for overlap", 0)
	REAL (L"left Time range (s)", L"0")
	REAL (L"right Time range (s)", L"0.1")
	POSITIVE (L"Overlap (s)", L"0.01")
	OK
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_extractPartForOverlap (me,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"),
			GET_REAL (L"Overlap"));
		praat_new (thee.transfer(), my name, L"_part");
	}
END

DIRECT (Sound_extractRightChannel)
	LOOP {
		iam (Sound);
		autoSound thee = Sound_extractChannel (me, 2);
		praat_new (thee.transfer(), my name, L"_right");
	}
END

FORM (Sound_filter_deemphasis, L"Sound: Filter (de-emphasis)", L"Sound: Filter (de-emphasis)...")
	REAL (L"From frequency (Hz)", L"50.0")
	OK
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_filter_deemphasis (me, GET_REAL (L"From frequency"));
		praat_new (thee.transfer(), my name, L"_deemp");
	}
END

FORM (Sound_filter_formula, L"Sound: Filter (formula)...", L"Formula...")
	LABEL (L"", L"Frequency-domain filtering with a formula (uses Sound-to-Spectrum and Spectrum-to-Sound): x is frequency in hertz")
	TEXTFIELD (L"formula", L"if x<500 or x>1000 then 0 else self fi; rectangular band filter")
	OK
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_filter_formula (me, GET_STRING (L"formula"), interpreter);
		praat_new (thee.transfer(), my name, L"_filt");
	}
END

FORM (Sound_filter_oneFormant, L"Sound: Filter (one formant)", L"Sound: Filter (one formant)...")
	REAL (L"Frequency (Hz)", L"1000")
	POSITIVE (L"Bandwidth (Hz)", L"100")
	OK
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_filter_oneFormant (me, GET_REAL (L"Frequency"), GET_REAL (L"Bandwidth"));
		praat_new (thee.transfer(), my name, L"_filt");
	}
END

FORM (Sound_filterWithOneFormantInline, L"Sound: Filter with one formant (in-line)", L"Sound: Filter with one formant (in-line)...")
	REAL (L"Frequency (Hz)", L"1000")
	POSITIVE (L"Bandwidth (Hz)", L"100")
	OK
DO
	LOOP {
		iam (Sound);
		Sound_filterWithOneFormantInline (me, GET_REAL (L"Frequency"), GET_REAL (L"Bandwidth"));
		praat_dataChanged (me);
	}
END

FORM (Sound_filter_passHannBand, L"Sound: Filter (pass Hann band)", L"Sound: Filter (pass Hann band)...")
	REAL (L"From frequency (Hz)", L"500")
	REAL (L"To frequency (Hz)", L"1000")
	POSITIVE (L"Smoothing (Hz)", L"100")
	OK
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_filter_passHannBand (me,
			GET_REAL (L"From frequency"), GET_REAL (L"To frequency"), GET_REAL (L"Smoothing"));
		praat_new (thee.transfer(), my name, L"_band");
	}
END

FORM (Sound_filter_preemphasis, L"Sound: Filter (pre-emphasis)", L"Sound: Filter (pre-emphasis)...")
	REAL (L"From frequency (Hz)", L"50.0")
	OK
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_filter_preemphasis (me, GET_REAL (L"From frequency"));
		praat_new (thee.transfer(), my name, L"_preemp");
	}
END

FORM (Sound_filter_stopHannBand, L"Sound: Filter (stop Hann band)", L"Sound: Filter (stop Hann band)...")
	REAL (L"From frequency (Hz)", L"500")
	REAL (L"To frequency (Hz)", L"1000")
	POSITIVE (L"Smoothing (Hz)", L"100")
	OK
DO
	LOOP {
		iam (Sound);
		autoSound thee = Sound_filter_stopHannBand (me, GET_REAL (L"From frequency"), GET_REAL (L"To frequency"), GET_REAL (L"Smoothing"));
		praat_new (thee.transfer(), my name, L"_band");
	}
END

FORM (Sound_formula, L"Sound: Formula", L"Sound: Formula...")
	LABEL (L"label1", L"! `x' is the time in seconds, `col' is the sample number.")
	LABEL (L"label2", L"x = x1   ! time associated with first sample")
	LABEL (L"label3", L"for col from 1 to ncol")
	LABEL (L"label4", L"   self [col] = ...")
	TEXTFIELD (L"formula", L"self")
	LABEL (L"label5", L"   x = x + dx")
	LABEL (L"label6", L"endfor")
	OK
DO
	LOOP {
		iam (Sound);
		try {
			Matrix_formula ((Matrix) me, GET_STRING (L"formula"), interpreter, NULL);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Sound may have partially changed
			throw;
		}
	}
END

FORM (Sound_formula_part, L"Sound: Formula (part)", L"Sound: Formula...")
	REAL (L"From time", L"0.0")
	REAL (L"To time", L"0.0 (= all)")
	NATURAL (L"From channel", L"1")
	NATURAL (L"To channel", L"2")
	TEXTFIELD (L"formula", L"2 * self")
	OK
DO
	LOOP {
		iam (Sound);
		try {
			Matrix_formula_part ((Matrix) me,
				GET_REAL (L"From time"), GET_REAL (L"To time"),
				GET_INTEGER (L"From channel") - 0.5, GET_INTEGER (L"To channel") + 0.5,
				GET_STRING (L"formula"), interpreter, NULL);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // in case of error, the Sound may have partially changed
			throw;
		}
	}
END

FORM (Sound_getAbsoluteExtremum, L"Sound: Get absolute extremum", L"Sound: Get absolute extremum...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	RADIO (L"Interpolation", 4)
		RADIOBUTTON (L"None")
		RADIOBUTTON (L"Parabolic")
		RADIOBUTTON (L"Cubic")
		RADIOBUTTON (L"Sinc70")
		RADIOBUTTON (L"Sinc700")
	OK
DO
	LOOP {
		iam (Sound);
		double absoluteExtremum = Vector_getAbsoluteExtremum (me,
			GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (absoluteExtremum, L"Pascal");
	}
END

FORM (Sound_getEnergy, L"Sound: Get energy", L"Sound: Get energy...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	OK
DO
	LOOP {
		iam (Sound);
		double energy = Sound_getEnergy (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"));
		Melder_informationReal (energy, L"Pa2 sec");
	}
END

DIRECT (Sound_getEnergyInAir)
	LOOP {
		iam (Sound);
		double energyInAir = Sound_getEnergyInAir (me);
		Melder_informationReal (energyInAir, L"Joule/m2");
	}
END

FORM (Sound_getIndexFromTime, L"Get sample number from time", L"Get sample number from time...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (Sound);
		double realIndex = Sampled_xToIndex (me, GET_REAL (L"Time"));
		Melder_informationReal (realIndex, NULL);
	}
END

DIRECT (Sound_getIntensity_dB)
	LOOP {
		iam (Sound);
		double intensity = Sound_getIntensity_dB (me);
		Melder_informationReal (intensity, L"dB");
	}
END

FORM (Sound_getMaximum, L"Sound: Get maximum", L"Sound: Get maximum...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	RADIO (L"Interpolation", 4)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	RADIOBUTTON (L"Cubic")
	RADIOBUTTON (L"Sinc70")
	RADIOBUTTON (L"Sinc700")
	OK
DO
	LOOP {
		iam (Sound);
		double maximum = Vector_getMaximum (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (maximum, L"Pascal");
	}
END

FORM (old_Sound_getMean, L"Sound: Get mean", L"Sound: Get mean...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	OK
DO
	LOOP {
		iam (Sound);
		double mean = Vector_getMean (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), Vector_CHANNEL_AVERAGE);
		Melder_informationReal (mean, L"Pascal");
	}
END

FORM (Sound_getMean, L"Sound: Get mean", L"Sound: Get mean...")
	CHANNEL (L"Channel", L"0 (= all)")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	OK
DO_ALTERNATIVE (old_Sound_getMean)
	LOOP {
		iam (Sound);
		long channel = GET_INTEGER (L"Channel");
		if (channel > my ny) channel = 1;
		double mean = Vector_getMean (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), channel);
		Melder_informationReal (mean, L"Pascal");
	}
END

FORM (Sound_getMinimum, L"Sound: Get minimum", L"Sound: Get minimum...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	RADIO (L"Interpolation", 4)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	RADIOBUTTON (L"Cubic")
	RADIOBUTTON (L"Sinc70")
	RADIOBUTTON (L"Sinc700")
	OK
DO
	LOOP {
		iam (Sound);
		double minimum = Vector_getMinimum (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (minimum, L"Pascal");
	}
END

FORM (old_Sound_getNearestZeroCrossing, L"Sound: Get nearest zero crossing", L"Sound: Get nearest zero crossing...")
	REAL (L"Time (s)", L"0.5")
	OK
DO
	LOOP {
		iam (Sound);
		if (my ny > 1) Melder_throw ("Cannot determine a zero crossing for a stereo sound.");
		double zeroCrossing = Sound_getNearestZeroCrossing (me, GET_REAL (L"Time"), 1);
		Melder_informationReal (zeroCrossing, L"seconds");
	}
END

FORM (Sound_getNearestZeroCrossing, L"Sound: Get nearest zero crossing", L"Sound: Get nearest zero crossing...")
	CHANNEL (L"Channel (number, Left, or Right)", L"1")
	REAL (L"Time (s)", L"0.5")
	OK
DO_ALTERNATIVE (old_Sound_getNearestZeroCrossing)
	LOOP {
		iam (Sound);
		long channel = GET_INTEGER (L"Channel");
		if (channel > my ny) channel = 1;
		double zeroCrossing = Sound_getNearestZeroCrossing (me, GET_REAL (L"Time"), channel);
		Melder_informationReal (zeroCrossing, L"seconds");
	}
END

DIRECT (Sound_getNumberOfChannels)
	LOOP {
		iam (Sound);
		long numberOfChannels = my ny;
		Melder_information (Melder_integer (numberOfChannels), numberOfChannels == 1 ? L" channel (mono)" : numberOfChannels == 2 ? L" channels (stereo)" : L"channels");
	}
END

DIRECT (Sound_getNumberOfSamples)
	LOOP {
		iam (Sound);
		long numberOfSamples = my nx;
		Melder_information (Melder_integer (numberOfSamples), L" samples");
	}
END

FORM (Sound_getPower, L"Sound: Get power", L"Sound: Get power...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	OK
DO
	LOOP {
		iam (Sound);
		double power = Sound_getPower (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"));
		Melder_informationReal (power, L"Pa2");
	}
END

DIRECT (Sound_getPowerInAir)
	LOOP {
		iam (Sound);
		double powerInAir = Sound_getPowerInAir (me);
		Melder_informationReal (powerInAir, L"Watt/m2");
	}
END

FORM (Sound_getRootMeanSquare, L"Sound: Get root-mean-square", L"Sound: Get root-mean-square...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	OK
DO
	LOOP {
		iam (Sound);
		double rootMeanSquare = Sound_getRootMeanSquare (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"));
		Melder_informationReal (rootMeanSquare, L"Pascal");
	}
END

DIRECT (Sound_getSamplePeriod)
	LOOP {
		iam (Sound);
		double samplePeriod = my dx;
		Melder_informationReal (samplePeriod, L"seconds");
	}
END

DIRECT (Sound_getSampleRate)
	LOOP {
		iam (Sound);
		double samplingFrequency = 1.0 / my dx;
		Melder_informationReal (samplingFrequency, L"Hz");
	}
END

FORM (old_Sound_getStandardDeviation, L"Sound: Get standard deviation", L"Sound: Get standard deviation...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	OK
DO
	LOOP {
		iam (Sound);
		double stdev = Vector_getStandardDeviation (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), Vector_CHANNEL_AVERAGE);
		Melder_informationReal (stdev, L"Pascal");
	}
END

FORM (Sound_getStandardDeviation, L"Sound: Get standard deviation", L"Sound: Get standard deviation...")
	CHANNEL (L"Channel", L"0 (= average)")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	OK
DO_ALTERNATIVE (old_Sound_getStandardDeviation)
	LOOP {
		iam (Sound);
		long channel = GET_INTEGER (L"Channel");
		if (channel > my ny) channel = 1;
		double stdev = Vector_getStandardDeviation (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), channel);
		Melder_informationReal (stdev, L"Pascal");
	}
END

FORM (Sound_getTimeFromIndex, L"Get time from sample number", L"Get time from sample number...")
	INTEGER (L"Sample number", L"100")
	OK
DO
	LOOP {
		iam (Sound);
		double time = Sampled_indexToX (me, GET_INTEGER (L"Sample number"));
		Melder_informationReal (time, L"seconds");
	}
END

FORM (Sound_getTimeOfMaximum, L"Sound: Get time of maximum", L"Sound: Get time of maximum...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	RADIO (L"Interpolation", 4)
		RADIOBUTTON (L"None")
		RADIOBUTTON (L"Parabolic")
		RADIOBUTTON (L"Cubic")
		RADIOBUTTON (L"Sinc70")
		RADIOBUTTON (L"Sinc700")
	OK
DO
	LOOP {
		iam (Sound);
		double time = Vector_getXOfMaximum (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (time, L"seconds");
	}
END

FORM (Sound_getTimeOfMinimum, L"Sound: Get time of minimum", L"Sound: Get time of minimum...")
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	RADIO (L"Interpolation", 4)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	RADIOBUTTON (L"Cubic")
	RADIOBUTTON (L"Sinc70")
	RADIOBUTTON (L"Sinc700")
	OK
DO
	LOOP {
		iam (Sound);
		double time = Vector_getXOfMinimum (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (time, L"seconds");
	}
END

FORM (old_Sound_getValueAtIndex, L"Sound: Get value at sample number", L"Sound: Get value at sample number...")
	INTEGER (L"Sample number", L"100")
	OK
DO
	LOOP {
		iam (Sound);
		long sampleIndex = GET_INTEGER (L"Sample number");
		Melder_informationReal (sampleIndex < 1 || sampleIndex > my nx ? NUMundefined :
			my ny == 1 ? my z [1] [sampleIndex] : 0.5 * (my z [1] [sampleIndex] + my z [2] [sampleIndex]), L"Pascal");
	}
END

FORM (Sound_getValueAtIndex, L"Sound: Get value at sample number", L"Sound: Get value at sample number...")
	CHANNEL (L"Channel", L"0 (= average)")
	INTEGER (L"Sample number", L"100")
	OK
DO_ALTERNATIVE (old_Sound_getValueAtIndex)
	LOOP {
		iam (Sound);
		long sampleIndex = GET_INTEGER (L"Sample number");
		long channel = GET_INTEGER (L"Channel");
		if (channel > my ny) channel = 1;
		Melder_informationReal (sampleIndex < 1 || sampleIndex > my nx ? NUMundefined :
			Sampled_getValueAtSample (me, sampleIndex, channel, 0), L"Pascal");
	}
END

FORM (old_Sound_getValueAtTime, L"Sound: Get value at time", L"Sound: Get value at time...")
	REAL (L"Time (s)", L"0.5")
	RADIO (L"Interpolation", 4)
		RADIOBUTTON (L"Nearest")
		RADIOBUTTON (L"Linear")
		RADIOBUTTON (L"Cubic")
		RADIOBUTTON (L"Sinc70")
		RADIOBUTTON (L"Sinc700")
	OK
DO
	LOOP {
		iam (Sound);
		double value = Vector_getValueAtX (me, GET_REAL (L"Time"), Vector_CHANNEL_AVERAGE, GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (value, L"Pascal");
	}
END

FORM (Sound_getValueAtTime, L"Sound: Get value at time", L"Sound: Get value at time...")
	CHANNEL (L"Channel", L"0 (= average)")
	REAL (L"Time (s)", L"0.5")
	RADIO (L"Interpolation", 4)
		RADIOBUTTON (L"Nearest")
		RADIOBUTTON (L"Linear")
		RADIOBUTTON (L"Cubic")
		RADIOBUTTON (L"Sinc70")
		RADIOBUTTON (L"Sinc700")
	OK
DO_ALTERNATIVE (old_Sound_getValueAtTime)
	LOOP {
		iam (Sound);
		long channel = GET_INTEGER (L"Channel");
		if (channel > my ny) channel = 1;
		double value = Vector_getValueAtX (me, GET_REAL (L"Time"), channel, GET_INTEGER (L"Interpolation") - 1);
		Melder_informationReal (value, L"Pascal");
	}
END

DIRECT (Sound_help) Melder_help (L"Sound"); END

FORM (Sound_lengthen_overlapAdd, L"Sound: Lengthen (overlap-add)", L"Sound: Lengthen (overlap-add)...")
	POSITIVE (L"Minimum pitch (Hz)", L"75")
	POSITIVE (L"Maximum pitch (Hz)", L"600")
	POSITIVE (L"Factor", L"1.5")
	OK
DO
	double minimumPitch = GET_REAL (L"Minimum pitch"), maximumPitch = GET_REAL (L"Maximum pitch");
	double factor = GET_REAL (L"Factor");
	if (minimumPitch >= maximumPitch) Melder_throw ("Maximum pitch should be greater than minimum pitch.");
	LOOP {
		iam (Sound);
		autoSound thee = Sound_lengthen_overlapAdd (me, minimumPitch, maximumPitch, factor);
		praat_new (thee.transfer(), my name, L"_", Melder_fixed (factor, 2));
	}
END

FORM (Sound_multiply, L"Sound: Multiply", 0)
	REAL (L"Multiplication factor", L"1.5")
	OK
DO
	LOOP {
		iam (Sound);
		Vector_multiplyByScalar (me, GET_REAL (L"Multiplication factor"));
		praat_dataChanged (me);
	}
END

FORM (Sound_multiplyByWindow, L"Sound: Multiply by window", 0)
	OPTIONMENU_ENUM (L"Window shape", kSound_windowShape, HANNING)
	OK
DO
	LOOP {
		iam (Sound);
		Sound_multiplyByWindow (me, GET_ENUM (kSound_windowShape, L"Window shape"));
		praat_dataChanged (me);
	}
END

FORM (Sound_overrideSamplingFrequency, L"Sound: Override sampling frequency", 0)
	POSITIVE (L"New sampling frequency (Hz)", L"16000.0")
	OK
DO
	LOOP {
		iam (Sound);
		Sound_overrideSamplingFrequency (me, GET_REAL (L"New sampling frequency"));
		praat_dataChanged (me);
	}
END

DIRECT (Sound_play)
	int n = 0;
	LOOP {
		n ++;
	}
	if (n == 1 || MelderAudio_getOutputMaximumAsynchronicity () < kMelder_asynchronicityLevel_ASYNCHRONOUS) {
		LOOP {
			iam (Sound);
			Sound_play (me, NULL, NULL);
		}
	} else {
		MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel_INTERRUPTABLE);
		LOOP {
			iam (Sound);
			Sound_play (me, NULL, NULL);
		}
		MelderAudio_setOutputMaximumAsynchronicity (kMelder_asynchronicityLevel_ASYNCHRONOUS);
	}
END

FORM (Sound_preemphasizeInline, L"Sound: Pre-emphasize (in-line)", L"Sound: Pre-emphasize (in-line)...")
	REAL (L"From frequency (Hz)", L"50.0")
	OK
DO
	LOOP {
		iam (Sound);
		Sound_preEmphasis (me, GET_REAL (L"From frequency"));
		Vector_scale (me, 0.99);
		praat_dataChanged (me);
	}
END

FORM_READ (Sound_readSeparateChannelsFromSoundFile, L"Read separate channels from sound file", 0, true)
	autoSound sound = Sound_readFromSoundFile (file);
	wchar_t name [300];
	wcscpy (name, MelderFile_name (file));
	wchar_t *lastPeriod = wcsrchr (name, '.');
	if (lastPeriod != NULL) {
		*lastPeriod = '\0';
	}
	for (long ichan = 1; ichan <= sound -> ny; ichan ++) {
		autoSound thee = Sound_extractChannel (sound.peek(), ichan);
		praat_new (thee.transfer(), name, L"_ch", Melder_integer (ichan));
	}
END

FORM_READ (Sound_readFromRawAlawFile, L"Read Sound from raw Alaw file", 0, true)
	autoSound me = Sound_readFromRawAlawFile (file);
	praat_new (me.transfer(), MelderFile_name (file));
END

static SoundRecorder theSoundRecorder;   // only one at a time can exist
static int thePreviousNumberOfChannels;
static void cb_SoundRecorder_destruction (Editor editor, void *closure) {
	(void) editor;
	(void) closure;
	theSoundRecorder = NULL;
}
static void cb_SoundRecorder_publication (Editor editor, void *closure, Data publication) {
	(void) editor;
	(void) closure;
	try {
		praat_new (publication, NULL);
	} catch (MelderError) {
		Melder_flushError (NULL);
	}
	praat_updateSelection ();
}
static void do_Sound_record (int numberOfChannels) {
	if (theCurrentPraatApplication -> batch)
		Melder_throw ("Cannot record a Sound from batch.");
	if (theSoundRecorder) {
		if (numberOfChannels == thePreviousNumberOfChannels) {
			theSoundRecorder -> raise ();
		} else {
			forget (theSoundRecorder);
		}
	}
	if (! theSoundRecorder) {
		theSoundRecorder = SoundRecorder_create (numberOfChannels);
		theSoundRecorder -> setDestructionCallback (cb_SoundRecorder_destruction, NULL);
		theSoundRecorder -> setPublicationCallback (cb_SoundRecorder_publication, NULL);
	}
	thePreviousNumberOfChannels = numberOfChannels;
}
DIRECT (Sound_record_mono)
	do_Sound_record (1);
END
DIRECT (Sound_record_stereo)
	do_Sound_record (2);
END

FORM (Sound_recordFixedTime, L"Record Sound", 0)
	RADIO (L"Input source", 1)
		RADIOBUTTON (L"Microphone")
		RADIOBUTTON (L"Line")
		REAL (L"Gain (0-1)", L"0.1")
	REAL (L"Balance (0-1)", L"0.5")
	RADIO (L"Sampling frequency", 1)
		#ifdef UNIX
		RADIOBUTTON (L"8000")
		#endif
		#ifndef macintosh
		RADIOBUTTON (L"11025")
		#endif
		#ifdef UNIX
		RADIOBUTTON (L"16000")
		#endif
		#ifndef macintosh
		RADIOBUTTON (L"22050")
		#endif
		#ifdef UNIX
		RADIOBUTTON (L"32000")
		#endif
		RADIOBUTTON (L"44100")
		RADIOBUTTON (L"48000")
		RADIOBUTTON (L"96000")
	POSITIVE (L"Duration (seconds)", L"1.0")
	OK
DO
	autoSound me = Sound_recordFixedTime (GET_INTEGER (L"Input source"),
		GET_REAL (L"Gain"), GET_REAL (L"Balance"),
		wcstod (GET_STRING (L"Sampling frequency"), NULL), GET_REAL (L"Duration"));
	praat_new (me.transfer(), L"untitled");
END

FORM (Sound_resample, L"Sound: Resample", L"Sound: Resample...")
	POSITIVE (L"New sampling frequency (Hz)", L"10000")
	NATURAL (L"Precision (samples)", L"50")
	OK
DO
	double samplingFrequency = GET_REAL (L"New sampling frequency");
	LOOP {
		iam (Sound);
		autoSound thee = Sound_resample (me, samplingFrequency, GET_INTEGER (L"Precision"));
		praat_new (thee.transfer(), my name, L"_", Melder_integer ((long) round (samplingFrequency)));
	}
END

DIRECT (Sound_reverse)
	LOOP {
		iam (Sound);
		Sound_reverse (me, 0, 0);
		praat_dataChanged (me);
	}
END

FORM_WRITE (Sound_saveAs24BitWavFile, L"Save as 24-bit WAV file", 0, L"wav")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_WAV, 24);
END

FORM_WRITE (Sound_saveAs32BitWavFile, L"Save as 32-bit WAV file", 0, L"wav")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_WAV, 32);
END

FORM (Sound_scalePeak, L"Sound: Scale peak", L"Sound: Scale peak...")
	POSITIVE (L"New absolute peak", L"0.99")
	OK
DO
	LOOP {
		iam (Sound);
		Vector_scale (me, GET_REAL (L"New absolute peak"));
		praat_dataChanged (me);
	}
END

FORM (Sound_scaleIntensity, L"Sound: Scale intensity", L"Sound: Scale intensity")
	POSITIVE (L"New average intensity (dB SPL)", L"70.0")
	OK
DO
	LOOP {
		iam (Sound);
		Sound_scaleIntensity (me, GET_REAL (L"New average intensity"));
		praat_dataChanged (me);
	}
END

FORM (old_Sound_setValueAtIndex, L"Sound: Set value at sample number", L"Sound: Set value at sample number...")
	NATURAL (L"Sample number", L"100")
	REAL (L"New value", L"0")
	OK
DO
	LOOP {
		iam (Sound);
		long index = GET_INTEGER (L"Sample number");
		if (index > my nx)
			Melder_throw ("The sample number should not exceed the number of samples, which is ", my nx, ".");
		for (long channel = 1; channel <= my ny; channel ++)
			my z [channel] [index] = GET_REAL (L"New value");
		praat_dataChanged (me);
	}
END

FORM (Sound_setValueAtIndex, L"Sound: Set value at sample number", L"Sound: Set value at sample number...")
	CHANNEL (L"Channel", L"0 (= all)")
	NATURAL (L"Sample number", L"100")
	REAL (L"New value", L"0")
	OK
DO_ALTERNATIVE (old_Sound_setValueAtIndex)
	LOOP {
		iam (Sound);
		long index = GET_INTEGER (L"Sample number");
		if (index > my nx)
			Melder_throw ("The sample number should not exceed the number of samples, which is ", my nx, ".");
		long channel = GET_INTEGER (L"Channel");
		if (channel > my ny) channel = 1;
		if (channel > 0) {
			my z [channel] [index] = GET_REAL (L"New value");
		} else {
			for (channel = 1; channel <= my ny; channel ++) {
				my z [channel] [index] = GET_REAL (L"New value");
			}
		}
		praat_dataChanged (me);
	}
END

FORM (Sound_setPartToZero, L"Sound: Set part to zero", 0)
	REAL (L"left Time range (s)", L"0.0")
	REAL (L"right Time range (s)", L"0.0 (= all)")
	RADIO (L"Cut", 2)
		OPTION (L"at exactly these times")
		OPTION (L"at nearest zero crossing")
	OK
DO
	LOOP {
		iam (Sound);
		Sound_setZero (me, GET_REAL (L"left Time range"), GET_REAL (L"right Time range"), GET_INTEGER (L"Cut") - 1);
		praat_dataChanged (me);
	}
END

DIRECT (Sound_subtractMean)
	LOOP {
		iam (Sound);
		Vector_subtractMean (me);
		praat_dataChanged (me);
	}
END

FORM (Sound_to_Manipulation, L"Sound: To Manipulation", L"Manipulation")
	POSITIVE (L"Time step (s)", L"0.01")
	POSITIVE (L"Minimum pitch (Hz)", L"75")
	POSITIVE (L"Maximum pitch (Hz)", L"600")
	OK
DO
	double fmin = GET_REAL (L"Minimum pitch"), fmax = GET_REAL (L"Maximum pitch");
	if (fmax <= fmin) Melder_throw ("Maximum pitch must be greater than minimum pitch.");
	LOOP {
		iam (Sound);
		autoManipulation thee = Sound_to_Manipulation (me, GET_REAL (L"Time step"), fmin, fmax);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_Cochleagram, L"Sound: To Cochleagram", 0)
	POSITIVE (L"Time step (s)", L"0.01")
	POSITIVE (L"Frequency resolution (Bark)", L"0.1")
	POSITIVE (L"Window length (s)", L"0.03")
	REAL (L"Forward-masking time (s)", L"0.03")
	OK
DO
	LOOP {
		iam (Sound);
		autoCochleagram thee = Sound_to_Cochleagram (me, GET_REAL (L"Time step"),
			GET_REAL (L"Frequency resolution"), GET_REAL (L"Window length"), GET_REAL (L"Forward-masking time"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_Cochleagram_edb, L"Sound: To Cochleagram (De Boer, Meddis & Hewitt)", 0)
	POSITIVE (L"Time step (s)", L"0.01")
	POSITIVE (L"Frequency resolution (Bark)", L"0.1")
	BOOLEAN (L"Has synapse", 1)
	LABEL (L"", L"Meddis synapse properties")
	POSITIVE (L"   replenishment rate (/sec)", L"5.05")
	POSITIVE (L"   loss rate (/sec)", L"2500")
	POSITIVE (L"   return rate (/sec)", L"6580")
	POSITIVE (L"   reprocessing rate (/sec)", L"66.31")
	OK
DO
	LOOP {
		iam (Sound);
		autoCochleagram thee = Sound_to_Cochleagram_edb (me, GET_REAL (L"Time step"),
			GET_REAL (L"Frequency resolution"), GET_INTEGER (L"Has synapse"),
			GET_REAL (L"   replenishment rate"), GET_REAL (L"   loss rate"),
			GET_REAL (L"   return rate"), GET_REAL (L"   reprocessing rate"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_Formant_burg, L"Sound: To Formant (Burg method)", L"Sound: To Formant (burg)...")
	REAL (L"Time step (s)", L"0.0 (= auto)")
	POSITIVE (L"Max. number of formants", L"5")
	REAL (L"Maximum formant (Hz)", L"5500 (= adult female)")
	POSITIVE (L"Window length (s)", L"0.025")
	POSITIVE (L"Pre-emphasis from (Hz)", L"50")
	OK
DO
	LOOP {
		iam (Sound);
		autoFormant thee = Sound_to_Formant_burg (me, GET_REAL (L"Time step"),
			GET_REAL (L"Max. number of formants"), GET_REAL (L"Maximum formant"),
			GET_REAL (L"Window length"), GET_REAL (L"Pre-emphasis from"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_Formant_keepAll, L"Sound: To Formant (keep all)", L"Sound: To Formant (keep all)...")
	REAL (L"Time step (s)", L"0.0 (= auto)")
	POSITIVE (L"Max. number of formants", L"5")
	REAL (L"Maximum formant (Hz)", L"5500 (= adult female)")
	POSITIVE (L"Window length (s)", L"0.025")
	POSITIVE (L"Pre-emphasis from (Hz)", L"50")
	OK
DO
	LOOP {
		iam (Sound);
		autoFormant thee = Sound_to_Formant_keepAll (me, GET_REAL (L"Time step"),
			GET_REAL (L"Max. number of formants"), GET_REAL (L"Maximum formant"),
			GET_REAL (L"Window length"), GET_REAL (L"Pre-emphasis from"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_Formant_willems, L"Sound: To Formant (split Levinson (Willems))", L"Sound: To Formant (sl)...")
	REAL (L"Time step (s)", L"0.0 (= auto)")
	POSITIVE (L"Number of formants", L"5")
	REAL (L"Maximum formant (Hz)", L"5500 (= adult female)")
	POSITIVE (L"Window length (s)", L"0.025")
	POSITIVE (L"Pre-emphasis from (Hz)", L"50")
	OK
DO
	LOOP {
		iam (Sound);
		autoFormant thee = Sound_to_Formant_willems (me, GET_REAL (L"Time step"),
			GET_REAL (L"Number of formants"), GET_REAL (L"Maximum formant"),
			GET_REAL (L"Window length"), GET_REAL (L"Pre-emphasis from"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_Harmonicity_ac, L"Sound: To Harmonicity (ac)", L"Sound: To Harmonicity (ac)...")
	POSITIVE (L"Time step (s)", L"0.01")
	POSITIVE (L"Minimum pitch (Hz)", L"75")
	REAL (L"Silence threshold", L"0.1")
	POSITIVE (L"Periods per window", L"4.5")
	OK
DO
	double periodsPerWindow = GET_REAL (L"Periods per window");
	if (periodsPerWindow < 3.0) Melder_throw ("Number of periods per window must be at least 3.0.");
	LOOP {
		iam (Sound);
		autoHarmonicity thee = Sound_to_Harmonicity_ac (me, GET_REAL (L"Time step"),
			GET_REAL (L"Minimum pitch"), GET_REAL (L"Silence threshold"), periodsPerWindow);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_Harmonicity_cc, L"Sound: To Harmonicity (cc)", L"Sound: To Harmonicity (cc)...")
	POSITIVE (L"Time step (s)", L"0.01")
	POSITIVE (L"Minimum pitch (Hz)", L"75")
	REAL (L"Silence threshold", L"0.1")
	POSITIVE (L"Periods per window", L"1.0")
	OK
DO
	LOOP {
		iam (Sound);
		autoHarmonicity thee = Sound_to_Harmonicity_cc (me, GET_REAL (L"Time step"),
			GET_REAL (L"Minimum pitch"), GET_REAL (L"Silence threshold"),
			GET_REAL (L"Periods per window"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_Harmonicity_gne, L"Sound: To Harmonicity (gne)", 0)
	POSITIVE (L"Minimum frequency (Hz)", L"500")
	POSITIVE (L"Maximum frequency (Hz)", L"4500")
	POSITIVE (L"Bandwidth (Hz)", L"1000")
	POSITIVE (L"Step (Hz)", L"80")
	OK
DO
	LOOP {
		iam (Sound);
		autoMatrix thee = Sound_to_Harmonicity_GNE (me, GET_REAL (L"Minimum frequency"),
			GET_REAL (L"Maximum frequency"), GET_REAL (L"Bandwidth"),
			GET_REAL (L"Step"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (old_Sound_to_Intensity, L"Sound: To Intensity", L"Sound: To Intensity...")
	POSITIVE (L"Minimum pitch (Hz)", L"100")
	REAL (L"Time step (s)", L"0.0 (= auto)")
	OK
DO
	LOOP {
		iam (Sound);
		autoIntensity thee = Sound_to_Intensity (me,
			GET_REAL (L"Minimum pitch"), GET_REAL (L"Time step"), FALSE);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_Intensity, L"Sound: To Intensity", L"Sound: To Intensity...")
	POSITIVE (L"Minimum pitch (Hz)", L"100")
	REAL (L"Time step (s)", L"0.0 (= auto)")
	BOOLEAN (L"Subtract mean", 1)
	OK
DO_ALTERNATIVE (old_Sound_to_Intensity)
	LOOP {
		iam (Sound);
		autoIntensity thee = Sound_to_Intensity (me,
			GET_REAL (L"Minimum pitch"), GET_REAL (L"Time step"), GET_INTEGER (L"Subtract mean"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_IntensityTier, L"Sound: To IntensityTier", NULL)
	POSITIVE (L"Minimum pitch (Hz)", L"100")
	REAL (L"Time step (s)", L"0.0 (= auto)")
	BOOLEAN (L"Subtract mean", 1)
	OK
DO
	LOOP {
		iam (Sound);
		autoIntensityTier thee = Sound_to_IntensityTier (me,
			GET_REAL (L"Minimum pitch"), GET_REAL (L"Time step"), GET_INTEGER (L"Subtract mean"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Sound_to_IntervalTier)
	LOOP {
		iam (Sound);
		autoIntervalTier thee = IntervalTier_create (my xmin, my xmax);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_Ltas, L"Sound: To long-term average spectrum", 0)
	POSITIVE (L"Bandwidth (Hz)", L"100")
	OK
DO
	LOOP {
		iam (Sound);
		autoLtas thee = Sound_to_Ltas (me, GET_REAL (L"Bandwidth"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_Ltas_pitchCorrected, L"Sound: To Ltas (pitch-corrected)", L"Sound: To Ltas (pitch-corrected)...")
	POSITIVE (L"Minimum pitch (Hz)", L"75")
	POSITIVE (L"Maximum pitch (Hz)", L"600")
	POSITIVE (L"Maximum frequency (Hz)", L"5000")
	POSITIVE (L"Bandwidth (Hz)", L"100")
	REAL (L"Shortest period (s)", L"0.0001")
	REAL (L"Longest period (s)", L"0.02")
	POSITIVE (L"Maximum period factor", L"1.3")
	OK
DO
	double fmin = GET_REAL (L"Minimum pitch"), fmax = GET_REAL (L"Maximum pitch");
	if (fmax <= fmin) Melder_throw ("Maximum pitch must be greater than minimum pitch.");
	LOOP {
		iam (Sound);
		autoLtas thee = Sound_to_Ltas_pitchCorrected (me, fmin, fmax,
			GET_REAL (L"Maximum frequency"), GET_REAL (L"Bandwidth"),
			GET_REAL (L"Shortest period"), GET_REAL (L"Longest period"), GET_REAL (L"Maximum period factor"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Sound_to_Matrix)
	LOOP {
		iam (Sound);
		autoMatrix thee = Sound_to_Matrix (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Sounds_to_ParamCurve)
	Sound s1 = NULL, s2 = NULL;
	LOOP {
		iam (Sound);
		( s1 ? s2 : s1 ) = me;
	}
	autoParamCurve thee = ParamCurve_create (s1, s2);
	praat_new (thee.transfer(), s1 -> name, L"_", s2 -> name);
END

FORM (Sound_to_Pitch, L"Sound: To Pitch", L"Sound: To Pitch...")
	REAL (L"Time step (s)", L"0.0 (= auto)")
	POSITIVE (L"Pitch floor (Hz)", L"75.0")
	POSITIVE (L"Pitch ceiling (Hz)", L"600.0")
	OK
DO
	LOOP {
		iam (Sound);
		autoPitch thee = Sound_to_Pitch (me, GET_REAL (L"Time step"), GET_REAL (L"Pitch floor"), GET_REAL (L"Pitch ceiling"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_Pitch_ac, L"Sound: To Pitch (ac)", L"Sound: To Pitch (ac)...")
	LABEL (L"", L"Finding the candidates")
	REAL (L"Time step (s)", L"0.0 (= auto)")
	POSITIVE (L"Pitch floor (Hz)", L"75.0")
	NATURAL (L"Max. number of candidates", L"15")
	BOOLEAN (L"Very accurate", 0)
	LABEL (L"", L"Finding a path")
	REAL (L"Silence threshold", L"0.03")
	REAL (L"Voicing threshold", L"0.45")
	REAL (L"Octave cost", L"0.01")
	REAL (L"Octave-jump cost", L"0.35")
	REAL (L"Voiced / unvoiced cost", L"0.14")
	POSITIVE (L"Pitch ceiling (Hz)", L"600.0")
	OK
DO
	long maxnCandidates = GET_INTEGER (L"Max. number of candidates");
	if (maxnCandidates <= 1) Melder_throw ("Maximum number of candidates must be greater than 1.");
	LOOP {
		iam (Sound);
		autoPitch thee = Sound_to_Pitch_ac (me, GET_REAL (L"Time step"),
			GET_REAL (L"Pitch floor"), 3.0, maxnCandidates, GET_INTEGER (L"Very accurate"),
			GET_REAL (L"Silence threshold"), GET_REAL (L"Voicing threshold"),
			GET_REAL (L"Octave cost"), GET_REAL (L"Octave-jump cost"),
			GET_REAL (L"Voiced / unvoiced cost"), GET_REAL (L"Pitch ceiling"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_Pitch_cc, L"Sound: To Pitch (cc)", L"Sound: To Pitch (cc)...")
	LABEL (L"", L"Finding the candidates")
	REAL (L"Time step (s)", L"0.0 (= auto)")
	POSITIVE (L"Pitch floor (Hz)", L"75")
	NATURAL (L"Max. number of candidates", L"15")
	BOOLEAN (L"Very accurate", 0)
	LABEL (L"", L"Finding a path")
	REAL (L"Silence threshold", L"0.03")
	REAL (L"Voicing threshold", L"0.45")
	REAL (L"Octave cost", L"0.01")
	REAL (L"Octave-jump cost", L"0.35")
	REAL (L"Voiced / unvoiced cost", L"0.14")
	POSITIVE (L"Pitch ceiling (Hz)", L"600")
	OK
DO
	long maxnCandidates = GET_INTEGER (L"Max. number of candidates");
	if (maxnCandidates <= 1) Melder_throw ("Maximum number of candidates must be greater than 1.");
	LOOP {
		iam (Sound);
		autoPitch thee = Sound_to_Pitch_cc (me, GET_REAL (L"Time step"),
			GET_REAL (L"Pitch floor"), 1.0, maxnCandidates, GET_INTEGER (L"Very accurate"),
			GET_REAL (L"Silence threshold"), GET_REAL (L"Voicing threshold"),
			GET_REAL (L"Octave cost"), GET_REAL (L"Octave-jump cost"),
			GET_REAL (L"Voiced / unvoiced cost"), GET_REAL (L"Pitch ceiling"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_PointProcess_extrema, L"Sound: To PointProcess (extrema)", 0)
	CHANNEL (L"Channel (number, Left, or Right)", L"1")
	BOOLEAN (L"Include maxima", 1)
	BOOLEAN (L"Include minima", 0)
	RADIO (L"Interpolation", 4)
	RADIOBUTTON (L"None")
	RADIOBUTTON (L"Parabolic")
	RADIOBUTTON (L"Cubic")
	RADIOBUTTON (L"Sinc70")
	RADIOBUTTON (L"Sinc700")
	OK
DO
	long channel = GET_INTEGER (L"Channel");
	LOOP {
		iam (Sound);
		autoPointProcess thee = Sound_to_PointProcess_extrema (me, channel > my ny ? 1 : channel, GET_INTEGER (L"Interpolation") - 1,
			GET_INTEGER (L"Include maxima"), GET_INTEGER (L"Include minima"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_PointProcess_periodic_cc, L"Sound: To PointProcess (periodic, cc)", L"Sound: To PointProcess (periodic, cc)...")
	POSITIVE (L"Minimum pitch (Hz)", L"75")
	POSITIVE (L"Maximum pitch (Hz)", L"600")
	OK
DO
	double fmin = GET_REAL (L"Minimum pitch"), fmax = GET_REAL (L"Maximum pitch");
	if (fmax <= fmin) Melder_throw ("Maximum pitch must be greater than minimum pitch.");
	LOOP {
		iam (Sound);
		autoPointProcess thee = Sound_to_PointProcess_periodic_cc (me, fmin, fmax);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_PointProcess_periodic_peaks, L"Sound: To PointProcess (periodic, peaks)", L"Sound: To PointProcess (periodic, peaks)...")
	POSITIVE (L"Minimum pitch (Hz)", L"75")
	POSITIVE (L"Maximum pitch (Hz)", L"600")
	BOOLEAN (L"Include maxima", 1)
	BOOLEAN (L"Include minima", 0)
	OK
DO
	double fmin = GET_REAL (L"Minimum pitch"), fmax = GET_REAL (L"Maximum pitch");
	if (fmax <= fmin) Melder_throw ("Maximum pitch must be greater than minimum pitch.");
	LOOP {
		iam (Sound);
		autoPointProcess thee = Sound_to_PointProcess_periodic_peaks (me, fmin, fmax, GET_INTEGER (L"Include maxima"), GET_INTEGER (L"Include minima"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_PointProcess_zeroes, L"Get zeroes", 0)
	CHANNEL (L"Channel (number, Left, or Right)", L"1")
	BOOLEAN (L"Include raisers", 1)
	BOOLEAN (L"Include fallers", 0)
	OK
DO
	long channel = GET_INTEGER (L"Channel");
	LOOP {
		iam (Sound);
		autoPointProcess thee = Sound_to_PointProcess_zeroes (me, channel > my ny ? 1 : channel, GET_INTEGER (L"Include raisers"), GET_INTEGER (L"Include fallers"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_Spectrogram, L"Sound: To Spectrogram", L"Sound: To Spectrogram...")
	POSITIVE (L"Window length (s)", L"0.005")
	POSITIVE (L"Maximum frequency (Hz)", L"5000")
	POSITIVE (L"Time step (s)", L"0.002")
	POSITIVE (L"Frequency step (Hz)", L"20")
	RADIO_ENUM (L"Window shape", kSound_to_Spectrogram_windowShape, DEFAULT)
	OK
DO
	LOOP {
		iam (Sound);
		autoSpectrogram thee = Sound_to_Spectrogram (me, GET_REAL (L"Window length"),
			GET_REAL (L"Maximum frequency"), GET_REAL (L"Time step"),
			GET_REAL (L"Frequency step"), GET_ENUM (kSound_to_Spectrogram_windowShape, L"Window shape"), 8.0, 8.0);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_Spectrum, L"Sound: To Spectrum", L"Sound: To Spectrum...")
	BOOLEAN (L"Fast", 1)
	OK
DO
	LOOP {
		iam (Sound);
		autoSpectrum thee = Sound_to_Spectrum (me, GET_INTEGER (L"Fast"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Sound_to_Spectrum_dft)
	LOOP {
		iam (Sound);
		autoSpectrum thee = Sound_to_Spectrum (me, FALSE);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Sound_to_Spectrum_fft)
	LOOP {
		iam (Sound);
		autoSpectrum thee = Sound_to_Spectrum (me, TRUE);
		praat_new (thee.transfer(), my name);
	}
END

FORM (Sound_to_TextGrid, L"Sound: To TextGrid", L"Sound: To TextGrid...")
	SENTENCE (L"All tier names", L"Mary John bell")
	SENTENCE (L"Which of these are point tiers?", L"bell")
	OK
DO
	LOOP {
		iam (Sound);
		autoTextGrid thee = TextGrid_create (my xmin, my xmax, GET_STRING (L"All tier names"), GET_STRING (L"Which of these are point tiers?"));
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (Sound_to_TextTier)
	LOOP {
		iam (Sound);
		autoTextTier thee = TextTier_create (my xmin, my xmax);
		praat_new (thee.transfer(), my name);
	}
END

FORM (SoundInputPrefs, L"Sound recording preferences", L"SoundRecorder")
	NATURAL (L"Buffer size (MB)", L"20")
	OK
SET_INTEGER (L"Buffer size", SoundRecorder_getBufferSizePref_MB ())
DO
	long size = GET_INTEGER (L"Buffer size");
	if (size > 1000) Melder_throw ("Buffer size cannot exceed 1000 megabytes.");
	SoundRecorder_setBufferSizePref_MB (size);
END

FORM (SoundOutputPrefs, L"Sound playing preferences", 0)
	LABEL (L"", L"The following determines how sounds are played.")
	LABEL (L"", L"Between parentheses, you find what you can do simultaneously.")
	LABEL (L"", L"Decrease asynchronicity if sound plays with discontinuities.")
	OPTIONMENU_ENUM (L"Maximum asynchronicity", kMelder_asynchronicityLevel, DEFAULT)
	#define xstr(s) str(s)
	#define str(s) #s
	REAL (L"Silence before (s)", L"" xstr (kMelderAudio_outputSilenceBefore_DEFAULT))
	REAL (L"Silence after (s)", L"" xstr (kMelderAudio_outputSilenceAfter_DEFAULT))
	OK
SET_ENUM (L"Maximum asynchronicity", kMelder_asynchronicityLevel, MelderAudio_getOutputMaximumAsynchronicity ())
SET_REAL (L"Silence before", MelderAudio_getOutputSilenceBefore ())
SET_REAL (L"Silence after", MelderAudio_getOutputSilenceAfter ())
DO
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	MelderAudio_setOutputMaximumAsynchronicity (GET_ENUM (kMelder_asynchronicityLevel, L"Maximum asynchronicity"));
	MelderAudio_setOutputSilenceBefore (GET_REAL (L"Silence before"));
	MelderAudio_setOutputSilenceAfter (GET_REAL (L"Silence after"));
END

FORM_WRITE (Sound_writeToAifcFile, L"Save as AIFC file", 0, L"aifc")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_AIFC, 16);
END

FORM_WRITE (Sound_writeToAiffFile, L"Save as AIFF file", 0, L"aiff")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_AIFF, 16);
END

FORM_WRITE (Sound_writeToFlacFile, L"Save as FLAC file", 0, L"flac")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_FLAC, 16);
END

FORM_WRITE (Sound_writeToKayFile, L"Save as Kay sound file", 0, L"kay")
	LOOP {
		iam (Sound);
		Sound_writeToKayFile (me, file);
	}
END

FORM_WRITE (Sound_writeToNextSunFile, L"Save as NeXT/Sun file", 0, L"au")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_NEXT_SUN, 16);
END

FORM_WRITE (Sound_writeToNistFile, L"Save as NIST file", 0, L"nist")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_NIST, 16);
END

FORM_WRITE (Sound_saveAsRaw8bitSignedFile, L"Save as raw 8-bit signed sound file", 0, L"8sig")
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_8_SIGNED);
	}
END

FORM_WRITE (Sound_saveAsRaw8bitUnsignedFile, L"Save as raw 8-bit unsigned sound file", 0, L"8uns")
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_8_UNSIGNED);
	}
END

FORM_WRITE (Sound_saveAsRaw16bitBigEndianFile, L"Save as raw 16-bit big-endian sound file", 0, L"16be")
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_16_BIG_ENDIAN);
	}
END

FORM_WRITE (Sound_saveAsRaw16bitLittleEndianFile, L"Save as raw 16-bit little-endian sound file", 0, L"16le")
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_16_LITTLE_ENDIAN);
	}
END

FORM_WRITE (Sound_saveAsRaw24bitBigEndianFile, L"Save as raw 24-bit big-endian sound file", 0, L"24be")
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_24_BIG_ENDIAN);
	}
END

FORM_WRITE (Sound_saveAsRaw24bitLittleEndianFile, L"Save as raw 24-bit little-endian sound file", 0, L"24le")
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_24_LITTLE_ENDIAN);
	}
END

FORM_WRITE (Sound_saveAsRaw32bitBigEndianFile, L"Save as raw 32-bit big-endian sound file", 0, L"32be")
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_32_BIG_ENDIAN);
	}
END

FORM_WRITE (Sound_saveAsRaw32bitLittleEndianFile, L"Save as raw 32-bit little-endian sound file", 0, L"32le")
	LOOP {
		iam (Sound);
		Sound_writeToRawSoundFile (me, file, Melder_LINEAR_32_LITTLE_ENDIAN);
	}
END

FORM_WRITE (Sound_writeToSesamFile, L"Save as Sesam file", 0, L"sdf")
	LOOP {
		iam (Sound);
		Sound_writeToSesamFile (me, file);
	}
END

FORM_WRITE (Sound_writeToStereoAifcFile, L"Save as stereo AIFC file", 0, L"aifc")
	autoCollection set = praat_getSelectedObjects ();
	autoSound stereo = Sounds_combineToStereo (set.peek());
	Sound_writeToAudioFile (stereo.peek(), file, Melder_AIFC, 16);
END

FORM_WRITE (Sound_writeToStereoAiffFile, L"Save as stereo AIFF file", 0, L"aiff")
	autoCollection set = praat_getSelectedObjects ();
	autoSound stereo = Sounds_combineToStereo (set.peek());
	Sound_writeToAudioFile (stereo.peek(), file, Melder_AIFF, 16);
END

FORM_WRITE (Sound_writeToStereoNextSunFile, L"Save as stereo NeXT/Sun file", 0, L"au")
	autoCollection set = praat_getSelectedObjects ();
	autoSound stereo = Sounds_combineToStereo (set.peek());
	Sound_writeToAudioFile (stereo.peek(), file, Melder_NEXT_SUN, 16);
END

FORM_WRITE (Sound_writeToStereoNistFile, L"Save as stereo NIST file", 0, L"nist")
	autoCollection set = praat_getSelectedObjects ();
	autoSound stereo = Sounds_combineToStereo (set.peek());
	Sound_writeToAudioFile (stereo.peek(), file, Melder_NIST, 16);
END

FORM_WRITE (Sound_writeToStereoFlacFile, L"Save as stereo FLAC file", 0, L"flac")
	autoCollection set = praat_getSelectedObjects ();
	autoSound stereo = Sounds_combineToStereo (set.peek());
	Sound_writeToAudioFile (stereo.peek(), file, Melder_FLAC, 16);
END

FORM_WRITE (Sound_writeToStereoWavFile, L"Save as stereo WAV file", 0, L"wav")
	autoCollection set = praat_getSelectedObjects ();
	autoSound stereo = Sounds_combineToStereo (set.peek());
	Sound_writeToAudioFile (stereo.peek(), file, Melder_WAV, 16);
END

FORM_WRITE (Sound_writeToSunAudioFile, L"Save as NeXT/Sun file", 0, L"au")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_NEXT_SUN, 16);
END

FORM_WRITE (Sound_writeToWavFile, L"Save as WAV file", 0, L"wav")
	autoCollection set = praat_getSelectedObjects ();
	LongSound_concatenate (set.peek(), file, Melder_WAV, 16);
END

/***** STOP *****/

DIRECT (stopPlayingSound)
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
END

/***** Help menus *****/

DIRECT (AnnotationTutorial) Melder_help (L"Intro 7. Annotation"); END
DIRECT (FilteringTutorial) Melder_help (L"Filtering"); END

/***** file recognizers *****/

static Any macSoundOrEmptyFileRecognizer (int nread, const char *header, MelderFile file) {
	/***** No data in file? This may be a Macintosh sound file with only a resource fork. *****/
	(void) header;
	if (nread > 0) return NULL;
	Melder_throw ("File ", file, " contains no audio data.");   // !!!
}

static Any soundFileRecognizer (int nread, const char *header, MelderFile file) {
	if (nread < 16) return NULL;
	if (strnequ (header, "FORM", 4) && strnequ (header + 8, "AIF", 3)) return Sound_readFromSoundFile (file);
	if (strnequ (header, "RIFF", 4) && (strnequ (header + 8, "WAVE", 4) || strnequ (header + 8, "CDDA", 4))) return Sound_readFromSoundFile (file);
	if (strnequ (header, ".snd", 4)) return Sound_readFromSoundFile (file);
	if (strnequ (header, "NIST_1A", 7)) return Sound_readFromSoundFile (file);
	if (strnequ (header, "fLaC", 4)) return Sound_readFromSoundFile (file);   // Erez Volk, March 2007
	if ((wcsstr (MelderFile_name (file), L".mp3") || wcsstr (MelderFile_name (file), L".MP3")) && mp3_recognize (nread, header)) return Sound_readFromSoundFile (file);   // Erez Volk, May 2007
	return NULL;
}

static Any movieFileRecognizer (int nread, const char *header, MelderFile file) {
	const wchar_t *fileName = MelderFile_name (file);
	(void) header;
	/*Melder_casual ("%d %d %d %d %d %d %d %d %d %d", header [0],
		header [1], header [2], header [3],
		header [4], header [5], header [6],
		header [7], header [8], header [9]);*/
	if (nread < 512 || (! wcsstr (fileName, L".mov") && ! wcsstr (fileName, L".MOV") &&
	    ! wcsstr (fileName, L".avi") && ! wcsstr (fileName, L".AVI"))) return NULL;
	Melder_throw ("This Praat version cannot open movie files.");
	return NULL;
}

static Any sesamFileRecognizer (int nread, const char *header, MelderFile file) {
	const wchar_t *fileName = MelderFile_name (file);
	(void) header;
	if (nread < 512 || (! wcsstr (fileName, L".sdf") && ! wcsstr (fileName, L".SDF"))) return NULL;
	return Sound_readFromSesamFile (file);
}

static Any bellLabsFileRecognizer (int nread, const char *header, MelderFile file) {
	if (nread < 16 || ! strnequ (& header [0], "SIG\n", 4)) return NULL;
	return Sound_readFromBellLabsFile (file);
}

static Any kayFileRecognizer (int nread, const char *header, MelderFile file) {
	if (nread <= 12 || ! strnequ (& header [0], "FORMDS16", 8)) return NULL;
	return Sound_readFromKayFile (file);
}

/***** override play and record buttons in manuals *****/

static Sound melderSound, melderSoundFromFile, last;
static int recordProc (double duration) {
	if (last == melderSound) last = NULL;
	forget (melderSound);
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	melderSound = Sound_recordFixedTime (1, 1.0, 0.5, 44100, duration);
	if (! melderSound) return 0;
	last = melderSound;
	return 1;
}
static int recordFromFileProc (MelderFile file) {
	if (last == melderSoundFromFile) last = NULL;
	forget (melderSoundFromFile);
	Melder_warningOff ();   /* Like "misssing samples". */
	melderSoundFromFile = (Sound) Data_readFromFile (file);
	Melder_warningOn ();
	if (! melderSoundFromFile) return 0;
	if (! Thing_member (melderSoundFromFile, classSound)) { forget (melderSoundFromFile); return 0; }
	last = melderSoundFromFile;
	Sound_play (melderSoundFromFile, NULL, NULL);
	return 1;
}
static void playProc (void) {
	if (melderSound) {
		Sound_play (melderSound, NULL, NULL);
		last = melderSound;
	}
}
static void playReverseProc (void) {
	/*if (melderSound) Sound_playReverse (melderSound);*/
}
static int publishPlayedProc (void) {
	if (! last) return 0;
	return Melder_publish (Data_copy (last));
}

/***** buttons *****/

void praat_uvafon_Sound_init (void);
void praat_uvafon_Sound_init (void) {

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

	praat_addMenuCommand (L"Objects", L"New", L"Record mono Sound...", 0, praat_ATTRACTIVE + 'R', DO_Sound_record_mono);
	praat_addMenuCommand (L"Objects", L"New", L"Record stereo Sound...", 0, 0, DO_Sound_record_stereo);
	praat_addMenuCommand (L"Objects", L"New", L"Record Sound (fixed time)...", 0, praat_HIDDEN, DO_Sound_recordFixedTime);
	praat_addMenuCommand (L"Objects", L"New", L"Sound", 0, 0, 0);
		praat_addMenuCommand (L"Objects", L"New", L"Create Sound...", 0, praat_HIDDEN + praat_DEPTH_1, DO_Sound_create);
		praat_addMenuCommand (L"Objects", L"New", L"Create Sound as pure tone...", 0, 1, DO_Sound_createAsPureTone);
		praat_addMenuCommand (L"Objects", L"New", L"Create Sound from formula...", 0, 1, DO_Sound_createFromFormula);
		praat_addMenuCommand (L"Objects", L"New", L"-- create sound advanced --", 0, 1, 0);
		praat_addMenuCommand (L"Objects", L"New", L"Create Sound as tone complex...", 0, 1, DO_Sound_createFromToneComplex);
		praat_addMenuCommand (L"Objects", L"New", L"Create Sound from tone complex...", 0, praat_HIDDEN + praat_DEPTH_1, DO_Sound_createFromToneComplex);

	praat_addMenuCommand (L"Objects", L"Open", L"-- read sound --", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"Open", L"Open long sound file...", 0, 'L', DO_LongSound_open);
	praat_addMenuCommand (L"Objects", L"Open", L"Read two Sounds from stereo file...", 0, praat_HIDDEN, DO_Sound_readSeparateChannelsFromSoundFile);   // deprecated 2010
	praat_addMenuCommand (L"Objects", L"Open", L"Read separate channels from sound file...", 0, 0, DO_Sound_readSeparateChannelsFromSoundFile);
	praat_addMenuCommand (L"Objects", L"Open", L"Read from special sound file", 0, 0, 0);
		praat_addMenuCommand (L"Objects", L"Open", L"Read Sound from raw Alaw file...", 0, 1, DO_Sound_readFromRawAlawFile);

	praat_addMenuCommand (L"Objects", L"Goodies", L"Stop playing sound", 0, GuiMenu_ESCAPE, DO_stopPlayingSound);
	praat_addMenuCommand (L"Objects", L"Preferences", L"-- sound prefs --", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"Preferences", L"Sound recording preferences...", 0, 0, DO_SoundInputPrefs);
	praat_addMenuCommand (L"Objects", L"Preferences", L"Sound playing preferences...", 0, 0, DO_SoundOutputPrefs);
	praat_addMenuCommand (L"Objects", L"Preferences", L"LongSound preferences...", 0, 0, DO_LongSoundPrefs);

	praat_addAction1 (classLongSound, 0, L"LongSound help", 0, 0, DO_LongSound_help);
	praat_addAction1 (classLongSound, 1, L"View", 0, praat_ATTRACTIVE, DO_LongSound_view);
	praat_addAction1 (classLongSound, 1, L"Open", 0, praat_HIDDEN, DO_LongSound_view);   // deprecated 2011
	praat_addAction1 (classLongSound, 0, L"Play part...", 0, 0, DO_LongSound_playPart);
	praat_addAction1 (classLongSound, 1, L"Query -", 0, 0, 0);
		praat_TimeFunction_query_init (classLongSound);
		praat_addAction1 (classLongSound, 1, L"Sampling", 0, 1, 0);
		praat_addAction1 (classLongSound, 1, L"Get number of samples", 0, 2, DO_LongSound_getNumberOfSamples);
		praat_addAction1 (classLongSound, 1, L"Get sampling period", 0, 2, DO_LongSound_getSamplePeriod);
							praat_addAction1 (classLongSound, 1, L"Get sample duration", 0, praat_HIDDEN + praat_DEPTH_2, DO_LongSound_getSamplePeriod);
							praat_addAction1 (classLongSound, 1, L"Get sample period", 0, praat_HIDDEN + praat_DEPTH_2, DO_LongSound_getSamplePeriod);
		praat_addAction1 (classLongSound, 1, L"Get sampling frequency", 0, 2, DO_LongSound_getSampleRate);
							praat_addAction1 (classLongSound, 1, L"Get sample rate", 0, praat_HIDDEN + praat_DEPTH_2, DO_LongSound_getSampleRate);   // deprecated 2004
		praat_addAction1 (classLongSound, 1, L"-- get time discretization --", 0, 2, 0);
		praat_addAction1 (classLongSound, 1, L"Get time from sample number...", 0, 2, DO_LongSound_getTimeFromIndex);
							praat_addAction1 (classLongSound, 1, L"Get time from index...", 0, praat_HIDDEN + praat_DEPTH_2, DO_LongSound_getTimeFromIndex);
		praat_addAction1 (classLongSound, 1, L"Get sample number from time...", 0, 2, DO_LongSound_getIndexFromTime);
							praat_addAction1 (classLongSound, 1, L"Get index from time...", 0, praat_HIDDEN + praat_DEPTH_2, DO_LongSound_getIndexFromTime);
	praat_addAction1 (classLongSound, 0, L"Annotate -", 0, 0, 0);
		praat_addAction1 (classLongSound, 0, L"Annotation tutorial", 0, 1, DO_AnnotationTutorial);
		praat_addAction1 (classLongSound, 0, L"-- to text grid --", 0, 1, 0);
		praat_addAction1 (classLongSound, 0, L"To TextGrid...", 0, 1, DO_LongSound_to_TextGrid);
	praat_addAction1 (classLongSound, 0, L"Convert to Sound", 0, 0, 0);
	praat_addAction1 (classLongSound, 0, L"Extract part...", 0, 0, DO_LongSound_extractPart);
	praat_addAction1 (classLongSound, 0, L"Concatenate?", 0, 0, DO_LongSound_concatenate);
	praat_addAction1 (classLongSound, 0, L"Save as WAV file...", 0, 0, DO_LongSound_writeToWavFile);
	praat_addAction1 (classLongSound, 0, L"Write to WAV file...", 0, praat_HIDDEN, DO_LongSound_writeToWavFile);
	praat_addAction1 (classLongSound, 0, L"Save as AIFF file...", 0, 0, DO_LongSound_writeToAiffFile);
	praat_addAction1 (classLongSound, 0, L"Write to AIFF file...", 0, praat_HIDDEN, DO_LongSound_writeToAiffFile);
	praat_addAction1 (classLongSound, 0, L"Save as AIFC file...", 0, 0, DO_LongSound_writeToAifcFile);
	praat_addAction1 (classLongSound, 0, L"Write to AIFC file...", 0, praat_HIDDEN, DO_LongSound_writeToAifcFile);
	praat_addAction1 (classLongSound, 0, L"Save as Next/Sun file...", 0, 0, DO_LongSound_writeToNextSunFile);
	praat_addAction1 (classLongSound, 0, L"Write to Next/Sun file...", 0, praat_HIDDEN, DO_LongSound_writeToNextSunFile);
	praat_addAction1 (classLongSound, 0, L"Save as NIST file...", 0, 0, DO_LongSound_writeToNistFile);
	praat_addAction1 (classLongSound, 0, L"Write to NIST file...", 0, praat_HIDDEN, DO_LongSound_writeToNistFile);
	praat_addAction1 (classLongSound, 0, L"Save as FLAC file...", 0, 0, DO_LongSound_writeToFlacFile);
	praat_addAction1 (classLongSound, 0, L"Write to FLAC file...", 0, praat_HIDDEN, DO_LongSound_writeToFlacFile);
	praat_addAction1 (classLongSound, 0, L"Save left channel as WAV file...", 0, 0, DO_LongSound_writeLeftChannelToWavFile);
	praat_addAction1 (classLongSound, 0, L"Write left channel to WAV file...", 0, praat_HIDDEN, DO_LongSound_writeLeftChannelToWavFile);
	praat_addAction1 (classLongSound, 0, L"Save left channel as AIFF file...", 0, 0, DO_LongSound_writeLeftChannelToAiffFile);
	praat_addAction1 (classLongSound, 0, L"Write left channel to AIFF file...", 0, praat_HIDDEN, DO_LongSound_writeLeftChannelToAiffFile);
	praat_addAction1 (classLongSound, 0, L"Save left channel as AIFC file...", 0, 0, DO_LongSound_writeLeftChannelToAifcFile);
	praat_addAction1 (classLongSound, 0, L"Write left channel to AIFC file...", 0, praat_HIDDEN, DO_LongSound_writeLeftChannelToAifcFile);
	praat_addAction1 (classLongSound, 0, L"Save left channel as Next/Sun file...", 0, 0, DO_LongSound_writeLeftChannelToNextSunFile);
	praat_addAction1 (classLongSound, 0, L"Write left channel to Next/Sun file...", 0, praat_HIDDEN, DO_LongSound_writeLeftChannelToNextSunFile);
	praat_addAction1 (classLongSound, 0, L"Save left channel as NIST file...", 0, 0, DO_LongSound_writeLeftChannelToNistFile);
	praat_addAction1 (classLongSound, 0, L"Write left channel to NIST file...", 0, praat_HIDDEN, DO_LongSound_writeLeftChannelToNistFile);
	praat_addAction1 (classLongSound, 0, L"Save left channel as FLAC file...", 0, 0, DO_LongSound_writeLeftChannelToFlacFile);
	praat_addAction1 (classLongSound, 0, L"Write left channel to FLAC file...", 0, praat_HIDDEN, DO_LongSound_writeLeftChannelToFlacFile);
	praat_addAction1 (classLongSound, 0, L"Save right channel as WAV file...", 0, 0, DO_LongSound_writeRightChannelToWavFile);
	praat_addAction1 (classLongSound, 0, L"Write right channel to WAV file...", 0, praat_HIDDEN, DO_LongSound_writeRightChannelToWavFile);
	praat_addAction1 (classLongSound, 0, L"Save right channel as AIFF file...", 0, 0, DO_LongSound_writeRightChannelToAiffFile);
	praat_addAction1 (classLongSound, 0, L"Write right channel to AIFF file...", 0, praat_HIDDEN, DO_LongSound_writeRightChannelToAiffFile);
	praat_addAction1 (classLongSound, 0, L"Save right channel as AIFC file...", 0, 0, DO_LongSound_writeRightChannelToAifcFile);
	praat_addAction1 (classLongSound, 0, L"Write right channel to AIFC file...", 0, praat_HIDDEN, DO_LongSound_writeRightChannelToAifcFile);
	praat_addAction1 (classLongSound, 0, L"Save right channel as Next/Sun file...", 0, 0, DO_LongSound_writeRightChannelToNextSunFile);
	praat_addAction1 (classLongSound, 0, L"Write right channel to Next/Sun file...", 0, praat_HIDDEN, DO_LongSound_writeRightChannelToNextSunFile);
	praat_addAction1 (classLongSound, 0, L"Save right channel as NIST file...", 0, 0, DO_LongSound_writeRightChannelToNistFile);
	praat_addAction1 (classLongSound, 0, L"Write right channel to NIST file...", 0, praat_HIDDEN, DO_LongSound_writeRightChannelToNistFile);
	praat_addAction1 (classLongSound, 0, L"Save right channel as FLAC file...", 0, 0, DO_LongSound_writeRightChannelToFlacFile);
	praat_addAction1 (classLongSound, 0, L"Write right channel to FLAC file...", 0, praat_HIDDEN, DO_LongSound_writeRightChannelToFlacFile);
	praat_addAction1 (classLongSound, 0, L"Save part as audio file...", 0, 0, DO_LongSound_writePartToAudioFile);
	praat_addAction1 (classLongSound, 0, L"Write part to audio file...", 0, praat_HIDDEN, DO_LongSound_writePartToAudioFile);

	praat_addAction1 (classSound, 0, L"Save as WAV file...", 0, 0, DO_Sound_writeToWavFile);
	praat_addAction1 (classSound, 0, L"Write to WAV file...", 0, praat_HIDDEN, DO_Sound_writeToWavFile);   // hidden 2011
	praat_addAction1 (classSound, 0, L"Save as AIFF file...", 0, 0, DO_Sound_writeToAiffFile);
	praat_addAction1 (classSound, 0, L"Write to AIFF file...", 0, praat_HIDDEN, DO_Sound_writeToAiffFile);
	praat_addAction1 (classSound, 0, L"Save as AIFC file...", 0, 0, DO_Sound_writeToAifcFile);
	praat_addAction1 (classSound, 0, L"Write to AIFC file...", 0, praat_HIDDEN, DO_Sound_writeToAifcFile);
	praat_addAction1 (classSound, 0, L"Save as Next/Sun file...", 0, 0, DO_Sound_writeToNextSunFile);
	praat_addAction1 (classSound, 0, L"Write to Next/Sun file...", 0, praat_HIDDEN, DO_Sound_writeToNextSunFile);
	praat_addAction1 (classSound, 0, L"Save as Sun audio file...", 0, praat_HIDDEN, DO_Sound_writeToSunAudioFile);
	praat_addAction1 (classSound, 0, L"Write to Sun audio file...", 0, praat_HIDDEN, DO_Sound_writeToSunAudioFile);
	praat_addAction1 (classSound, 0, L"Save as NIST file...", 0, 0, DO_Sound_writeToNistFile);
	praat_addAction1 (classSound, 0, L"Write to NIST file...", 0, praat_HIDDEN, DO_Sound_writeToNistFile);
	praat_addAction1 (classSound, 0, L"Save as FLAC file...", 0, 0, DO_Sound_writeToFlacFile);
	praat_addAction1 (classSound, 0, L"Write to FLAC file...", 0, praat_HIDDEN, DO_Sound_writeToFlacFile);
	praat_addAction1 (classSound, 1, L"Save as Kay sound file...", 0, 0, DO_Sound_writeToKayFile);
	praat_addAction1 (classSound, 1, L"Write to Kay sound file...", 0, praat_HIDDEN, DO_Sound_writeToKayFile);
	praat_addAction1 (classSound, 1, L"Save as Sesam file...", 0, praat_HIDDEN, DO_Sound_writeToSesamFile);
	praat_addAction1 (classSound, 1, L"Write to Sesam file...", 0, praat_HIDDEN, DO_Sound_writeToSesamFile);
	praat_addAction1 (classSound, 0, L"Save as 24-bit WAV file...", 0, 0, DO_Sound_saveAs24BitWavFile);
	praat_addAction1 (classSound, 0, L"Save as 32-bit WAV file...", 0, 0, DO_Sound_saveAs32BitWavFile);
	praat_addAction1 (classSound, 2, L"Write to stereo WAV file...", 0, praat_HIDDEN, DO_Sound_writeToStereoWavFile);   // deprecated 2007
	praat_addAction1 (classSound, 2, L"Write to stereo AIFF file...", 0, praat_HIDDEN, DO_Sound_writeToStereoAiffFile);   // deprecated 2007
	praat_addAction1 (classSound, 2, L"Write to stereo AIFC file...", 0, praat_HIDDEN, DO_Sound_writeToStereoAifcFile);   // deprecated 2007
	praat_addAction1 (classSound, 2, L"Write to stereo Next/Sun file...", 0, praat_HIDDEN, DO_Sound_writeToStereoNextSunFile);   // deprecated 2007
	praat_addAction1 (classSound, 2, L"Write to stereo NIST file...", 0, praat_HIDDEN, DO_Sound_writeToStereoNistFile);   // deprecated 2007
	praat_addAction1 (classSound, 2, L"Write to stereo FLAC file...", 0, praat_HIDDEN, DO_Sound_writeToStereoFlacFile);
	//praat_addAction1 (classSound, 1, L"Save as raw sound file", 0, 0, 0);
	praat_addAction1 (classSound, 1, L"Save as raw 8-bit signed file...", 0, 0, DO_Sound_saveAsRaw8bitSignedFile);
	praat_addAction1 (classSound, 1, L"Write to raw 8-bit signed file...", 0, praat_HIDDEN, DO_Sound_saveAsRaw8bitSignedFile);
	praat_addAction1 (classSound, 1, L"Save as raw 8-bit unsigned file...", 0, 0, DO_Sound_saveAsRaw8bitUnsignedFile);
	praat_addAction1 (classSound, 1, L"Write to raw 8-bit unsigned file...", 0, praat_HIDDEN, DO_Sound_saveAsRaw8bitUnsignedFile);
	praat_addAction1 (classSound, 1, L"Save as raw 16-bit big-endian file...", 0, 0, DO_Sound_saveAsRaw16bitBigEndianFile);
	praat_addAction1 (classSound, 1, L"Save as raw 16-bit little-endian file...", 0, 0, DO_Sound_saveAsRaw16bitLittleEndianFile);
	praat_addAction1 (classSound, 1, L"Save as raw 24-bit big-endian file...", 0, 0, DO_Sound_saveAsRaw24bitBigEndianFile);
	praat_addAction1 (classSound, 1, L"Save as raw 24-bit little-endian file...", 0, 0, DO_Sound_saveAsRaw24bitLittleEndianFile);
	praat_addAction1 (classSound, 1, L"Save as raw 32-bit big-endian file...", 0, 0, DO_Sound_saveAsRaw32bitBigEndianFile);
	praat_addAction1 (classSound, 1, L"Save as raw 32-bit little-endian file...", 0, 0, DO_Sound_saveAsRaw32bitLittleEndianFile);
	praat_addAction1 (classSound, 0, L"Sound help", 0, 0, DO_Sound_help);
	praat_addAction1 (classSound, 1, L"Edit", 0, praat_HIDDEN, DO_Sound_edit);   // deprecated 2011
	praat_addAction1 (classSound, 1, L"Open", 0, praat_HIDDEN, DO_Sound_edit);   // deprecated 2011
	praat_addAction1 (classSound, 1, L"View & Edit", 0, praat_ATTRACTIVE, DO_Sound_edit);
	praat_addAction1 (classSound, 0, L"Play", 0, 0, DO_Sound_play);
	praat_addAction1 (classSound, 1, L"Draw -", 0, 0, 0);
		praat_addAction1 (classSound, 0, L"Draw...", 0, 1, DO_Sound_draw);
	praat_addAction1 (classSound, 1, L"Query -", 0, 0, 0);
		praat_TimeFunction_query_init (classSound);
		praat_addAction1 (classSound, 1, L"Get number of channels", 0, 1, DO_Sound_getNumberOfChannels);
		praat_addAction1 (classSound, 1, L"Query time sampling", 0, 1, 0);
		praat_addAction1 (classSound, 1, L"Get number of samples", 0, 2, DO_Sound_getNumberOfSamples);
		praat_addAction1 (classSound, 1, L"Get sampling period", 0, 2, DO_Sound_getSamplePeriod);
							praat_addAction1 (classSound, 1, L"Get sample duration", 0, praat_HIDDEN + praat_DEPTH_2, DO_Sound_getSamplePeriod);
							praat_addAction1 (classSound, 1, L"Get sample period", 0, praat_HIDDEN + praat_DEPTH_2, DO_Sound_getSamplePeriod);
		praat_addAction1 (classSound, 1, L"Get sampling frequency", 0, 2, DO_Sound_getSampleRate);
							praat_addAction1 (classSound, 1, L"Get sample rate", 0, praat_HIDDEN + praat_DEPTH_2, DO_Sound_getSampleRate);   // deprecated 2004
		praat_addAction1 (classSound, 1, L"-- get time discretization --", 0, 2, 0);
		praat_addAction1 (classSound, 1, L"Get time from sample number...", 0, 2, DO_Sound_getTimeFromIndex);
							praat_addAction1 (classSound, 1, L"Get time from index...", 0, praat_HIDDEN + praat_DEPTH_2, DO_Sound_getTimeFromIndex);
		praat_addAction1 (classSound, 1, L"Get sample number from time...", 0, 2, DO_Sound_getIndexFromTime);
							praat_addAction1 (classSound, 1, L"Get index from time...", 0, praat_HIDDEN + praat_DEPTH_2, DO_Sound_getIndexFromTime);
		praat_addAction1 (classSound, 1, L"-- get content --", 0, 1, 0);
		praat_addAction1 (classSound, 1, L"Get value at time...", 0, 1, DO_Sound_getValueAtTime);
		praat_addAction1 (classSound, 1, L"Get value at sample number...", 0, 1, DO_Sound_getValueAtIndex);
							praat_addAction1 (classSound, 1, L"Get value at index...", 0, praat_HIDDEN + praat_DEPTH_1, DO_Sound_getValueAtIndex);
		praat_addAction1 (classSound, 1, L"-- get shape --", 0, 1, 0);
		praat_addAction1 (classSound, 1, L"Get minimum...", 0, 1, DO_Sound_getMinimum);
		praat_addAction1 (classSound, 1, L"Get time of minimum...", 0, 1, DO_Sound_getTimeOfMinimum);
		praat_addAction1 (classSound, 1, L"Get maximum...", 0, 1, DO_Sound_getMaximum);
		praat_addAction1 (classSound, 1, L"Get time of maximum...", 0, 1, DO_Sound_getTimeOfMaximum);
		praat_addAction1 (classSound, 1, L"Get absolute extremum...", 0, 1, DO_Sound_getAbsoluteExtremum);
		praat_addAction1 (classSound, 1, L"Get nearest zero crossing...", 0, 1, DO_Sound_getNearestZeroCrossing);
		praat_addAction1 (classSound, 1, L"-- get statistics --", 0, 1, 0);
		praat_addAction1 (classSound, 1, L"Get mean...", 0, 1, DO_Sound_getMean);
		praat_addAction1 (classSound, 1, L"Get root-mean-square...", 0, 1, DO_Sound_getRootMeanSquare);
		praat_addAction1 (classSound, 1, L"Get standard deviation...", 0, 1, DO_Sound_getStandardDeviation);
		praat_addAction1 (classSound, 1, L"-- get energy --", 0, 1, 0);
		praat_addAction1 (classSound, 1, L"Get energy...", 0, 1, DO_Sound_getEnergy);
		praat_addAction1 (classSound, 1, L"Get power...", 0, 1, DO_Sound_getPower);
		praat_addAction1 (classSound, 1, L"-- get energy in air --", 0, 1, 0);
		praat_addAction1 (classSound, 1, L"Get energy in air", 0, 1, DO_Sound_getEnergyInAir);
		praat_addAction1 (classSound, 1, L"Get power in air", 0, 1, DO_Sound_getPowerInAir);
		praat_addAction1 (classSound, 1, L"Get intensity (dB)", 0, 1, DO_Sound_getIntensity_dB);
	praat_addAction1 (classSound, 0, L"Modify -", 0, 0, 0);
		praat_TimeFunction_modify_init (classSound);
		praat_addAction1 (classSound, 0, L"-- modify generic --", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"Reverse", 0, 1, DO_Sound_reverse);
		praat_addAction1 (classSound, 0, L"Formula...", 0, 1, DO_Sound_formula);
		praat_addAction1 (classSound, 0, L"Formula (part)...", 0, 1, DO_Sound_formula_part);
		praat_addAction1 (classSound, 0, L"-- add & mul --", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"Add...", 0, 1, DO_Sound_add);
		praat_addAction1 (classSound, 0, L"Subtract mean", 0, 1, DO_Sound_subtractMean);
		praat_addAction1 (classSound, 0, L"Multiply...", 0, 1, DO_Sound_multiply);
		praat_addAction1 (classSound, 0, L"Multiply by window...", 0, 1, DO_Sound_multiplyByWindow);
		praat_addAction1 (classSound, 0, L"Scale peak...", 0, 1, DO_Sound_scalePeak);
		praat_addAction1 (classSound, 0, L"Scale...", 0, praat_HIDDEN + praat_DEPTH_1, DO_Sound_scalePeak);
		praat_addAction1 (classSound, 0, L"Scale intensity...", 0, 1, DO_Sound_scaleIntensity);
		praat_addAction1 (classSound, 0, L"-- set --", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"Set value at sample number...", 0, 1, DO_Sound_setValueAtIndex);
							praat_addAction1 (classSound, 0, L"Set value at index...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_setValueAtIndex);
		praat_addAction1 (classSound, 0, L"Set part to zero...", 0, 1, DO_Sound_setPartToZero);
		praat_addAction1 (classSound, 0, L"-- modify hack --", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"Override sampling frequency...", 0, 1, DO_Sound_overrideSamplingFrequency);
							praat_addAction1 (classSound, 0, L"Override sample rate...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_overrideSamplingFrequency);
		praat_addAction1 (classSound, 0, L"-- in-line filters --", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"In-line filters", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"Filter with one formant (in-line)...", 0, 2, DO_Sound_filterWithOneFormantInline);
		praat_addAction1 (classSound, 0, L"Pre-emphasize (in-line)...", 0, 2, DO_Sound_preemphasizeInline);
		praat_addAction1 (classSound, 0, L"De-emphasize (in-line)...", 0, 2, DO_Sound_deemphasizeInline);
	praat_addAction1 (classSound, 0, L"Annotate -", 0, 0, 0);
		praat_addAction1 (classSound, 0, L"Annotation tutorial", 0, 1, DO_AnnotationTutorial);
		praat_addAction1 (classSound, 0, L"-- to text grid --", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"To TextGrid...", 0, 1, DO_Sound_to_TextGrid);
		praat_addAction1 (classSound, 0, L"To TextTier", 0, praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_TextTier);
		praat_addAction1 (classSound, 0, L"To IntervalTier", 0, praat_HIDDEN + praat_DEPTH_1, DO_Sound_to_IntervalTier);
	praat_addAction1 (classSound, 0, L"Analyse periodicity -", 0, 0, 0);
		praat_addAction1 (classSound, 0, L"To Pitch...", 0, 1, DO_Sound_to_Pitch);
		praat_addAction1 (classSound, 0, L"To Pitch (ac)...", 0, 1, DO_Sound_to_Pitch_ac);
		praat_addAction1 (classSound, 0, L"To Pitch (cc)...", 0, 1, DO_Sound_to_Pitch_cc);
		praat_addAction1 (classSound, 0, L"To PointProcess (periodic, cc)...", 0, 1, DO_Sound_to_PointProcess_periodic_cc);
		praat_addAction1 (classSound, 0, L"To PointProcess (periodic, peaks)...", 0, 1, DO_Sound_to_PointProcess_periodic_peaks);
		praat_addAction1 (classSound, 0, L"-- points --", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"To PointProcess (extrema)...", 0, 1, DO_Sound_to_PointProcess_extrema);
		praat_addAction1 (classSound, 0, L"To PointProcess (zeroes)...", 0, 1, DO_Sound_to_PointProcess_zeroes);
		praat_addAction1 (classSound, 0, L"-- hnr --", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"To Harmonicity (cc)...", 0, 1, DO_Sound_to_Harmonicity_cc);
		praat_addAction1 (classSound, 0, L"To Harmonicity (ac)...", 0, 1, DO_Sound_to_Harmonicity_ac);
		praat_addAction1 (classSound, 0, L"To Harmonicity (gne)...", 0, 1, DO_Sound_to_Harmonicity_gne);
		praat_addAction1 (classSound, 0, L"-- autocorrelation --", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"Autocorrelate...", 0, 1, DO_Sound_autoCorrelate);
	praat_addAction1 (classSound, 0, L"Analyse spectrum -", 0, 0, 0);
		praat_addAction1 (classSound, 0, L"To Spectrum...", 0, 1, DO_Sound_to_Spectrum);
							praat_addAction1 (classSound, 0, L"To Spectrum (fft)", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_to_Spectrum_fft);
							praat_addAction1 (classSound, 0, L"To Spectrum", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_to_Spectrum_fft);
							praat_addAction1 (classSound, 0, L"To Spectrum (dft)", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_to_Spectrum_dft);
		praat_addAction1 (classSound, 0, L"To Ltas...", 0, 1, DO_Sound_to_Ltas);
		praat_addAction1 (classSound, 0, L"To Ltas (pitch-corrected)...", 0, 1, DO_Sound_to_Ltas_pitchCorrected);
		praat_addAction1 (classSound, 0, L"-- spectrotemporal --", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"To Spectrogram...", 0, 1, DO_Sound_to_Spectrogram);
		praat_addAction1 (classSound, 0, L"To Cochleagram...", 0, 1, DO_Sound_to_Cochleagram);
		praat_addAction1 (classSound, 0, L"To Cochleagram (edb)...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_to_Cochleagram_edb);
		praat_addAction1 (classSound, 0, L"-- formants --", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"To Formant (burg)...", 0, 1, DO_Sound_to_Formant_burg);
		praat_addAction1 (classSound, 0, L"To Formant (hack)", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"To Formant (keep all)...", 0, 2, DO_Sound_to_Formant_keepAll);
		praat_addAction1 (classSound, 0, L"To Formant (sl)...", 0, 2, DO_Sound_to_Formant_willems);
	praat_addAction1 (classSound, 0, L"To Intensity...", 0, 0, DO_Sound_to_Intensity);
	praat_addAction1 (classSound, 0, L"To IntensityTier...", 0, praat_HIDDEN, DO_Sound_to_IntensityTier);
	praat_addAction1 (classSound, 0, L"Manipulate -", 0, 0, 0);
	praat_addAction1 (classSound, 0, L"To Manipulation...", 0, 1, DO_Sound_to_Manipulation);
	praat_addAction1 (classSound, 0, L"Convert -", 0, 0, 0);
		praat_addAction1 (classSound, 0, L"Convert to mono", 0, 1, DO_Sound_convertToMono);
		praat_addAction1 (classSound, 0, L"Convert to stereo", 0, 1, DO_Sound_convertToStereo);
		praat_addAction1 (classSound, 0, L"Extract all channels", 0, 1, DO_Sound_extractAllChannels);
		praat_addAction1 (classSound, 0, L"Extract left channel", 0, praat_HIDDEN + praat_DEPTH_1, DO_Sound_extractLeftChannel);   // deprecated 2010
		praat_addAction1 (classSound, 0, L"Extract right channel", 0, praat_HIDDEN + praat_DEPTH_1, DO_Sound_extractRightChannel);   // deprecated 2010
		praat_addAction1 (classSound, 0, L"Extract one channel...", 0, 1, DO_Sound_extractChannel);
		praat_addAction1 (classSound, 0, L"Extract part...", 0, 1, DO_Sound_extractPart);
		praat_addAction1 (classSound, 0, L"Extract part for overlap...", 0, 1, DO_Sound_extractPartForOverlap);
		praat_addAction1 (classSound, 0, L"Resample...", 0, 1, DO_Sound_resample);
		praat_addAction1 (classSound, 0, L"-- enhance --", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"Lengthen (overlap-add)...", 0, 1, DO_Sound_lengthen_overlapAdd);
		praat_addAction1 (classSound, 0, L"Lengthen (PSOLA)...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_lengthen_overlapAdd);
		praat_addAction1 (classSound, 0, L"Deepen band modulation...", 0, 1, DO_Sound_deepenBandModulation);
		praat_addAction1 (classSound, 0, L"-- cast --", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"Down to Matrix", 0, 1, DO_Sound_to_Matrix);
	praat_addAction1 (classSound, 0, L"Filter -", 0, 0, 0);
		praat_addAction1 (classSound, 0, L"Filtering tutorial", 0, 1, DO_FilteringTutorial);
		praat_addAction1 (classSound, 0, L"-- frequency-domain filter --", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"Filter (pass Hann band)...", 0, 1, DO_Sound_filter_passHannBand);
		praat_addAction1 (classSound, 0, L"Filter (stop Hann band)...", 0, 1, DO_Sound_filter_stopHannBand);
		praat_addAction1 (classSound, 0, L"Filter (formula)...", 0, 1, DO_Sound_filter_formula);
		praat_addAction1 (classSound, 0, L"-- time-domain filter --", 0, 1, 0);
		praat_addAction1 (classSound, 0, L"Filter (one formant)...", 0, 1, DO_Sound_filter_oneFormant);
		praat_addAction1 (classSound, 0, L"Filter (pre-emphasis)...", 0, 1, DO_Sound_filter_preemphasis);
		praat_addAction1 (classSound, 0, L"Filter (de-emphasis)...", 0, 1, DO_Sound_filter_deemphasis);
	praat_addAction1 (classSound, 0, L"Combine -", 0, 0, 0);
		praat_addAction1 (classSound, 0, L"Combine to stereo", 0, 1, DO_Sounds_combineToStereo);
		praat_addAction1 (classSound, 0, L"Concatenate", 0, 1, DO_Sounds_concatenate);
		praat_addAction1 (classSound, 0, L"Concatenate recoverably", 0, 1, DO_Sounds_concatenateRecoverably);
		praat_addAction1 (classSound, 0, L"Concatenate with overlap...", 0, 1, DO_Sounds_concatenateWithOverlap);
		praat_addAction1 (classSound, 2, L"Convolve", 0, praat_HIDDEN + praat_DEPTH_1, DO_Sounds_convolve_old);
		praat_addAction1 (classSound, 2, L"Convolve...", 0, 1, DO_Sounds_convolve);
		praat_addAction1 (classSound, 2, L"Cross-correlate...", 0, 1, DO_Sounds_crossCorrelate);
		praat_addAction1 (classSound, 2, L"To ParamCurve", 0, 1, DO_Sounds_to_ParamCurve);

	praat_addAction2 (classLongSound, 0, classSound, 0, L"Save as WAV file...", 0, 0, DO_LongSound_Sound_writeToWavFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, L"Write to WAV file...", 0, praat_HIDDEN, DO_LongSound_Sound_writeToWavFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, L"Save as AIFF file...", 0, 0, DO_LongSound_Sound_writeToAiffFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, L"Write to AIFF file...", 0, praat_HIDDEN, DO_LongSound_Sound_writeToAiffFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, L"Save as AIFC file...", 0, 0, DO_LongSound_Sound_writeToAifcFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, L"Write to AIFC file...", 0, praat_HIDDEN, DO_LongSound_Sound_writeToAifcFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, L"Save as NeXT/Sun file...", 0, 0, DO_LongSound_Sound_writeToNextSunFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, L"Write to NeXT/Sun file...", 0, praat_HIDDEN, DO_LongSound_Sound_writeToNextSunFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, L"Save as NIST file...", 0, 0, DO_LongSound_Sound_writeToNistFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, L"Write to NIST file...", 0, praat_HIDDEN, DO_LongSound_Sound_writeToNistFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, L"Save as FLAC file...", 0, 0, DO_LongSound_Sound_writeToFlacFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, L"Write to FLAC file...", 0, praat_HIDDEN, DO_LongSound_Sound_writeToFlacFile);
}

/* End of file praat_Sound.cpp */
