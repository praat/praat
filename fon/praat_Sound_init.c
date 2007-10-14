/* praat_Sound_init.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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

/*
 * pb 2007/10/10
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
#include "TextGrid.h"
#include "mp3.h"

#ifndef LONG_MAX
	#define LONG_MAX  2147483647
#endif

int praat_Fon_formula (Any dia);
void praat_TimeFunction_query_init (void *klas);

static int pr_LongSound_concatenate (MelderFile file, int audioFileType) {
	int IOBJECT;
	Ordered me = Ordered_create ();
	if (! me) return 0;
	WHERE (SELECTED)
		if (! Collection_addItem (me, OBJECT)) { my size = 0; forget (me); return 0; }
	if (! LongSound_concatenate (me, file, audioFileType)) {
		my size = 0; forget (me); return 0;
	}
	my size = 0; forget (me);
	return 1;
}

/***** LONGSOUND *****/

FORMW (LongSound_extractPart, L"LongSound: Extract part", 0)
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"1.0")
	BOOLEANW (L"Preserve times", 1)
	OK
DO
	EVERY_TO (LongSound_extractPart (OBJECT, GET_REALW (L"left Time range"), GET_REALW (L"right Time range"), GET_INTEGERW (L"Preserve times")))
END

FORMW (LongSound_getIndexFromTime, L"LongSound: Get sample index from time", L"Sound: Get index from time...")
	REALW (L"Time (s)", L"0.5")
	OK
DO
	Melder_informationReal (Sampled_xToIndex (ONLY (classLongSound), GET_REALW (L"Time")), NULL);
END

DIRECT (LongSound_getSamplePeriod)
	LongSound me = ONLY (classLongSound);
	Melder_informationReal (my dx, L"seconds");
END

DIRECT (LongSound_getSampleRate)
	LongSound me = ONLY (classLongSound);
	Melder_informationReal (1 / my dx, L"Hertz");
END

FORMW (LongSound_getTimeFromIndex, L"LongSound: Get time from sample index", L"Sound: Get time from index...")
	INTEGERW (L"Sample index", L"100")
	OK
DO
	Melder_informationReal (Sampled_indexToX (ONLY (classLongSound), GET_INTEGERW (L"Sample index")), L"seconds");
END

DIRECT (LongSound_getNumberOfSamples)
	LongSound me = ONLY (classLongSound);
	Melder_information2 (Melder_integer (my nx), L" samples");
END

DIRECT (LongSound_help) Melder_help (L"LongSound"); END

FORM_READW (LongSound_open, L"Open long sound file", 0)
	if (! praat_new1 (LongSound_open (file), MelderFile_name (file))) return 0;
END

FORMW (LongSound_playPart, L"LongSound: Play part", 0)
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"10.0")
	OK
DO
	int n = 0;
	EVERY (n ++)
	if (n == 1 || Melder_getMaximumAsynchronicity () < Melder_ASYNCHRONOUS) {
		EVERY (LongSound_playPart (OBJECT, GET_REALW (L"left Time range"), GET_REALW (L"right Time range"), NULL, NULL))
	} else {
		Melder_setMaximumAsynchronicity (Melder_INTERRUPTABLE);
		EVERY (LongSound_playPart (OBJECT, GET_REALW (L"left Time range"), GET_REALW (L"right Time range"), NULL, NULL))
		Melder_setMaximumAsynchronicity (Melder_ASYNCHRONOUS);
	}
END

FORMW (LongSound_writePartToAudioFile, L"LongSound: Write part to audio file", 0)
	LABELW (L"", L"Audio file:")
	TEXTFIELDW (L"Audio file", L"")
	RADIOW (L"Type", 3)
	{ int i; for (i = 1; i <= Melder_NUMBER_OF_AUDIO_FILE_TYPES; i ++) {
		RADIOBUTTONW (Melder_audioFileTypeString (i))
	}}
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"10.0")
	OK
DO
	structMelderFile file = { 0 };
	if (! Melder_relativePathToFile (GET_STRINGW (L"Audio file"), & file)) return 0;
	if (! LongSound_writePartToAudioFile16 (ONLY (classLongSound), GET_INTEGERW (L"Type"),
		GET_REALW (L"left Time range"), GET_REALW (L"right Time range"), & file)) return 0;
END
	
FORMW (LongSound_to_TextGrid, L"LongSound: To TextGrid...", L"LongSound: To TextGrid...")
	SENTENCEW (L"Tier names", L"Mary John bell")
	SENTENCEW (L"Point tiers", L"bell")
	OK
DO
	EVERY_TO (TextGrid_create (((LongSound) OBJECT) -> xmin, ((Pitch) OBJECT) -> xmax,
		GET_STRINGW (L"Tier names"), GET_STRINGW (L"Point tiers")))
END

DIRECT (LongSound_view)
	if (theCurrentPraat -> batch)
		return Melder_error1 (L"Cannot view a LongSound from batch.");
	else
		WHERE (SELECTED)
			if (! praat_installEditor (SoundEditor_create (theCurrentPraat -> topShell, FULL_NAMEW, OBJECT), IOBJECT))
				return 0;
END

FORM_WRITEW (LongSound_writeToAifcFile, L"Write to AIFC file", 0, L"aifc")
	if (! pr_LongSound_concatenate (file, Melder_AIFC)) return 0;
END

FORM_WRITEW (LongSound_writeToAiffFile, L"Write to AIFF file", 0, L"aiff")
	if (! pr_LongSound_concatenate (file, Melder_AIFF)) return 0;
END

FORM_WRITEW (LongSound_writeToNextSunFile, L"Write to NeXT/Sun file", 0, L"au")
	if (! pr_LongSound_concatenate (file, Melder_NEXT_SUN)) return 0;
END

FORM_WRITEW (LongSound_writeToNistFile, L"Write to NIST file", 0, L"nist")
	if (! pr_LongSound_concatenate (file, Melder_NIST)) return 0;
END

FORM_WRITEW (LongSound_writeToFlacFile, L"Write to FLAC file", 0, L"flac")
	if (! pr_LongSound_concatenate (file, Melder_FLAC)) return 0;
END

FORM_WRITEW (LongSound_writeToWavFile, L"Write to WAV file", 0, L"wav")
	if (! pr_LongSound_concatenate (file, Melder_WAV)) return 0;
END

FORM_WRITEW (LongSound_writeLeftChannelToAifcFile, L"Write left channel to AIFC file", 0, L"aifc")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_AIFC, 0, file)) return 0;
END

FORM_WRITEW (LongSound_writeLeftChannelToAiffFile, L"Write left channel to AIFF file", 0, L"aiff")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_AIFF, 0, file)) return 0;
END

FORM_WRITEW (LongSound_writeLeftChannelToNextSunFile, L"Write left channel to NeXT/Sun file", 0, L"au")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_NEXT_SUN, 0, file)) return 0;
END

FORM_WRITEW (LongSound_writeLeftChannelToNistFile, L"Write left channel to NIST file", 0, L"nist")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_NIST, 0, file)) return 0;
END

FORM_WRITEW (LongSound_writeLeftChannelToFlacFile, L"Write left channel to FLAC file", 0, L"flac")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_FLAC, 0, file)) return 0;
END

FORM_WRITEW (LongSound_writeLeftChannelToWavFile, L"Write left channel to WAV file", 0, L"wav")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_WAV, 0, file)) return 0;
END

FORM_WRITEW (LongSound_writeRightChannelToAifcFile, L"Write right channel to AIFC file", 0, L"aifc")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_AIFC, 1, file)) return 0;
END

FORM_WRITEW (LongSound_writeRightChannelToAiffFile, L"Write right channel to AIFF file", 0, L"aiff")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_AIFF, 1, file)) return 0;
END

FORM_WRITEW (LongSound_writeRightChannelToNextSunFile, L"Write right channel to NeXT/Sun file", 0, L"au")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_NEXT_SUN, 1, file)) return 0;
END

FORM_WRITEW (LongSound_writeRightChannelToNistFile, L"Write right channel to NIST file", 0, L"nist")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_NIST, 1, file)) return 0;
END

FORM_WRITEW (LongSound_writeRightChannelToFlacFile, L"Write right channel to FLAC file", 0, L"flac")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_FLAC, 1, file)) return 0;
END

FORM_WRITEW (LongSound_writeRightChannelToWavFile, L"Write right channel to WAV file", 0, L"wav")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_WAV, 1, file)) return 0;
END

FORMW (LongSoundPrefs, L"LongSound preferences", L"LongSound")
	LABELW (L"", L"This setting determines the maximum number of seconds")
	LABELW (L"", L"for viewing the waveform and playing a sound in the LongSound window.")
	LABELW (L"", L"The LongSound window can become very slow if you set it too high.")
	NATURALW (L"Maximum viewable part (seconds)", L"60")
	LABELW (L"", L"Note: this setting works for the next long sound file that you open,")
	LABELW (L"", L"not for currently existing LongSound objects.")
	OK
SET_INTEGERW (L"Maximum viewable part", LongSound_getBufferSizePref_seconds ())
DO
	LongSound_setBufferSizePref_seconds (GET_INTEGERW (L"Maximum viewable part"));
END

/********** LONGSOUND & SOUND **********/

FORM_WRITEW (LongSound_Sound_writeToAifcFile, L"Write to AIFC file", 0, L"aifc")
	if (! pr_LongSound_concatenate (file, Melder_AIFC)) return 0;
END

FORM_WRITEW (LongSound_Sound_writeToAiffFile, L"Write to AIFF file", 0, L"aiff")
	if (! pr_LongSound_concatenate (file, Melder_AIFF)) return 0;
END

FORM_WRITEW (LongSound_Sound_writeToNextSunFile, L"Write to NeXT/Sun file", 0, L"au")
	if (! pr_LongSound_concatenate (file, Melder_NEXT_SUN)) return 0;
END

FORM_WRITEW (LongSound_Sound_writeToNistFile, L"Write to NIST file", 0, L"nist")
	if (! pr_LongSound_concatenate (file, Melder_NIST)) return 0;
END

FORM_WRITEW (LongSound_Sound_writeToFlacFile, L"Write to FLAC file", 0, L"flac")
	if (! pr_LongSound_concatenate (file, Melder_FLAC)) return 0;
END

FORM_WRITEW (LongSound_Sound_writeToWavFile, L"Write to WAV file", 0, L"wav")
	if (! pr_LongSound_concatenate (file, Melder_WAV)) return 0;
END

/********** SOUND **********/

FORMW (Sound_add, L"Sound: Add", 0)
	LABELW (L"", L"The following number will be added to the amplitudes of all samples of the sound.")
	REALW (L"Number", L"0.1")
	OK
DO
	WHERE (SELECTED) {
		Vector_addScalar (OBJECT, GET_REALW (L"Number"));
		praat_dataChanged (OBJECT);
	}
END

DIRECT (Sounds_combineToStereo)
	Sound sound1 = NULL, sound2 = NULL;
	int i1 = 0, i2 = 0;
	WHERE (SELECTED) { if (sound1) { sound2 = OBJECT; i2 = IOBJECT; } else { sound1 = OBJECT; i1 = IOBJECT; } }
	Melder_assert (sound1 && sound2 && i1 && i2);
	if (! praat_new3 (Sounds_combineToStereo (sound1, sound2),
		wcschr (theCurrentPraat -> list [i1]. name, ' ') + 1, L"_", wcschr (theCurrentPraat -> list [i2]. name, ' ') + 1)) return 0;
END

DIRECT (Sounds_concatenate)
	long numberOfChannels = 0, nx = 0;
	double dx = 0.0;
	Sound thee;
	WHERE (SELECTED) {
		Sound me = OBJECT;
		if (numberOfChannels == 0) {
			numberOfChannels = my ny;
		} else if (my ny != numberOfChannels) {
			return Melder_error1 (L"To concatenate sounds, their numbers of channels (mono, stereo) must be equal.");
		}
		if (dx == 0.0) {
			dx = my dx;
		} else if (my dx != dx) {
			(void) Melder_error1 (L"To concatenate sounds, their sampling frequencies must be equal.\n");
			return Melder_error1 (L"You could resample one or more of the sounds before concatenating.");
		}
		nx += my nx;
	}
	thee = Sound_create (numberOfChannels, 0.0, nx * dx, nx, dx, 0.5 * dx);
	if (! thee) return 0;
	nx = 0;
	WHERE (SELECTED) {
		Sound me = OBJECT;
		for (long channel = 1; channel <= numberOfChannels; channel ++) {
			NUMfvector_copyElements (my z [channel], thy z [channel] + nx, 1, my nx);
		}
		nx += my nx;
	}
	if (! praat_new1 (thee, L"chain")) return 0;
END

DIRECT (Sounds_concatenateRecoverably)
	long numberOfChannels = 0, nx = 0, iinterval = 0;
	double dx = 0.0, tmin = 0.0;
	Sound thee = NULL;
	TextGrid him = NULL;
	WHERE (SELECTED) {
		Sound me = OBJECT;
		if (numberOfChannels == 0) {
			numberOfChannels = my ny;
		} else if (my ny != numberOfChannels) {
			return Melder_error1 (L"To concatenate sounds, their numbers of channels (mono, stereo) must be equal.");
		}
		if (dx == 0.0) {
			dx = my dx;
		} else if (my dx != dx) {
			(void) Melder_error1 (L"To concatenate sounds, their sampling frequencies must be equal.\n");
			return Melder_error1 (L"You could resample one or more of the sounds before concatenating.");
		}
		nx += my nx;
	}
	thee = Sound_create (numberOfChannels, 0.0, nx * dx, nx, dx, 0.5 * dx); cherror
	him = TextGrid_create (0.0, nx * dx, L"labels", L""); cherror
	nx = 0;
	WHERE (SELECTED) {
		Sound me = OBJECT;
		double tmax = tmin + my nx * dx;
		for (long channel = 1; channel <= numberOfChannels; channel ++) {
			NUMfvector_copyElements (my z [channel], thy z [channel] + nx, 1, my nx);
		}
		iinterval ++;
		if (iinterval > 1) { TextGrid_insertBoundary (him, 1, tmin); cherror }
		TextGrid_setIntervalText (him, 1, iinterval, my name); cherror
		nx += my nx;
		tmin = tmax;
	}
	praat_new1 (thee, L"chain"); cherror
	praat_new1 (him, L"chain"); cherror
end:
	iferror { forget (thee); forget (him); return 0; }
END

DIRECT (Sound_convertToMono)
	WHERE (SELECTED) {
		Sound me = OBJECT;
		if (! praat_new2 (Sound_convertToMono (me), my name, L"_mono")) return 0;
	}
END

DIRECT (Sound_convertToStereo)
	WHERE (SELECTED) {
		Sound me = OBJECT;
		if (! praat_new2 (Sound_convertToStereo (me), my name, L"_stereo")) return 0;
	}
END

DIRECT (Sounds_convolve)
	Sound sound1 = NULL, sound2 = NULL;
	int i1 = 0, i2 = 0;
	WHERE (SELECTED) { if (sound1) { sound2 = OBJECT; i2 = IOBJECT; } else { sound1 = OBJECT; i1 = IOBJECT; } }
	Melder_assert (sound1 && sound2 && i1 && i2);
	if (! praat_new3 (Sounds_convolve (sound1, sound2),
		wcschr (theCurrentPraat -> list [i1]. name, ' ') + 1, L"_", wcschr (theCurrentPraat -> list [i2]. name, ' ') + 1)) return 0;
END

static int common_Sound_create (void *dia, bool allowStereo) {
	Sound sound = NULL;
	long channels = allowStereo ? GET_INTEGERW (L"Channels") : 1;
	double startTime = GET_REALW (L"Start time");
	double endTime = GET_REALW (L"End time");
	double samplingFrequency = GET_REALW (L"Sampling frequency");
	double numberOfSamples_real = floor ((endTime - startTime) * samplingFrequency + 0.5);
	long numberOfSamples;
	if (endTime <= startTime) {
		if (endTime == startTime)
			Melder_error1 (L"A Sound cannot have a duration of zero.");
		else
			Melder_error1 (L"A Sound cannot have a duration less than zero.");
		if (startTime == 0.0)
			return Melder_error1 (L"Please set the end time to something greater than 0 seconds.");
		else
			return Melder_error1 (L"Please lower the start time or raise the end time.");
	}
	if (samplingFrequency <= 0.0) {
		Melder_error1 (L"A Sound cannot have a negative sampling frequency.");
		return Melder_error1 (L"Please set the sampling frequency to something greater than zero, e.g. 44100 Hz.");
	}
	if (numberOfSamples_real < 1.0) {
		Melder_error1 (L"A Sound cannot have zero samples.");
		if (startTime == 0.0)
			return Melder_error1 (L"Please raise the end time.");
		else
			return Melder_error1 (L"Please lower the start time or raise the end time.");
	}
	if (numberOfSamples_real > LONG_MAX) {
		Melder_error5 (L"A Sound cannot have ", Melder_bigInteger (numberOfSamples_real), L" samples; the maximum is ",
			Melder_bigInteger (LONG_MAX), L" samples (or less, depending on your computer's memory).");
		if (startTime == 0.0)
			return Melder_error1 (L"Please lower the end time or the sampling frequency.");
		else
			return Melder_error1 (L"Please raise the start time, lower the end time, or lower the sampling frequency.");
	}
	numberOfSamples = (long) numberOfSamples_real;
	sound = Sound_create (channels, startTime, endTime, numberOfSamples, 1.0 / samplingFrequency,
		startTime + 0.5 * (endTime - startTime - (numberOfSamples - 1) / samplingFrequency));
	if (sound == NULL) {
		if (wcsstr (Melder_getError (), L"memory")) {
			Melder_clearError ();
			Melder_error3 (L"There is not enough memory to create a Sound that contains ", Melder_bigInteger (numberOfSamples_real), L" samples.");
			if (startTime == 0.0)
				return Melder_error1 (L"You could lower the end time or the sampling frequency and try again.");
			else
				return Melder_error1 (L"You could raise the start time or lower the end time or the sampling frequency, and try again.");
		} else {
			return 0;   /* Unexpected error. Wait for generic message. */
		}
	}
	Matrix_formula ((Matrix) sound, GET_STRINGW (L"formula"), NULL);
	iferror {
		forget (sound);
		return Melder_error1 (L"Please correct the formula.");
	}
	if (! praat_new1 (sound, GET_STRINGW (L"Name"))) return 0;
	//praat_updateSelection ();
	return 1;
}

FORMW (Sound_create, L"Create mono Sound", L"Create Sound from formula...")
	WORDW (L"Name", L"sineWithNoise")
	REALW (L"Start time (s)", L"0.0")
	REALW (L"End time (s)", L"1.0")
	REALW (L"Sampling frequency (Hz)", L"44100")
	LABELW (L"", L"Formula:")
	TEXTFIELDW (L"formula", L"1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)")
	OK
DO
	if (! common_Sound_create (dia, false)) return 0;
END

FORMW (Sound_createFromFormula, L"Create Sound from formula", L"Create Sound from formula...")
	WORDW (L"Name", L"sineWithNoise")
	OPTIONMENUW (L"Channels", 1)
		OPTIONW (L"Mono")
		OPTIONW (L"Stereo")
	REALW (L"Start time (s)", L"0.0")
	REALW (L"End time (s)", L"1.0")
	REALW (L"Sampling frequency (Hz)", L"44100")
	LABELW (L"", L"Formula:")
	TEXTFIELDW (L"formula", L"1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)")
	OK
DO
	if (! common_Sound_create (dia, true)) return 0;
END

FORMW (Sound_createFromToneComplex, L"Create Sound from tone complex", L"Create Sound from tone complex...")
	WORDW (L"Name", L"toneComplex")
	REALW (L"Start time (s)", L"0.0")
	REALW (L"End time (s)", L"1.0")
	POSITIVEW (L"Sampling frequency (Hz)", L"44100")
	RADIOW (L"Phase", 2)
		RADIOBUTTONW (L"Sine")
		RADIOBUTTONW (L"Cosine")
	POSITIVEW (L"Frequency step (Hz)", L"100")
	REALW (L"First frequency (Hz)", L"0 (= frequency step)")
	REALW (L"Ceiling (Hz)", L"0 (= Nyquist)")
	INTEGERW (L"Number of components", L"0 (= maximum)")
	OK
DO
	if (! praat_new1 (Sound_createFromToneComplex (GET_REALW (L"Start time"), GET_REALW (L"End time"),
		GET_REALW (L"Sampling frequency"), GET_INTEGERW (L"Phase") - 1, GET_REALW (L"Frequency step"),
		GET_REALW (L"First frequency"), GET_REALW (L"Ceiling"), GET_INTEGERW (L"Number of components")),
		GET_STRINGW (L"Name"))) return 0;
END

FORMW (Sounds_crossCorrelate, L"Cross-correlate", 0)
	REALW (L"From lag (s)", L"-0.1")
	REALW (L"To lag (s)", L"0.1")
	BOOLEANW (L"Normalize", 1)
	OK
DO
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	if (! praat_new4 (Sounds_crossCorrelate (s1, s2, GET_REALW (L"From lag"), GET_REALW (L"To lag"),
		GET_INTEGERW (L"Normalize")), L"cc_", s1 -> name, L"_", s2 -> name)) return 0;
END

FORMW (Sound_deemphasizeInline, L"Sound: De-emphasize (in-line)", L"Sound: De-emphasize (in-line)...")
	REALW (L"From frequency (Hz)", L"50.0")
	OK
DO
	WHERE (SELECTED) {
		Sound_deEmphasis (OBJECT, GET_REALW (L"From frequency"));
		Vector_scale (OBJECT, 0.99);
		praat_dataChanged (OBJECT);
	}
END

FORMW (Sound_deepenBandModulation, L"Deepen band modulation", L"Sound: Deepen band modulation...")
	POSITIVEW (L"Enhancement (dB)", L"20")
	POSITIVEW (L"From frequency (Hz)", L"300")
	POSITIVEW (L"To frequency (Hz)", L"8000")
	POSITIVEW (L"Slow modulation (Hz)", L"3")
	POSITIVEW (L"Fast modulation (Hz)", L"30")
	POSITIVEW (L"Band smoothing (Hz)", L"100")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new3 (Sound_deepenBandModulation (OBJECT, GET_REALW (L"Enhancement"),
			GET_REALW (L"From frequency"), GET_REALW (L"To frequency"),
			GET_REALW (L"Slow modulation"), GET_REALW (L"Fast modulation"), GET_REALW (L"Band smoothing")),
			NAMEW, L"_", Melder_integer ((long) GET_REALW (L"Enhancement")))) return 0;
END

FORMW (old_Sound_draw, L"Sound: Draw", 0)
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range", L"0.0 (= all)")
	REALW (L"left Vertical range", L"0.0")
	REALW (L"right Vertical range", L"0.0 (= auto)")
	BOOLEANW (L"Garnish", 1)
	OK
DO
	EVERY_DRAW (Sound_draw (OBJECT, GRAPHICS, GET_REALW (L"left Time range"), GET_REALW (L"right Time range"),
		GET_REALW (L"left Vertical range"), GET_REALW (L"right Vertical range"), GET_INTEGERW (L"Garnish"), L"curve"))
END

FORMW (Sound_draw, L"Sound: Draw", 0)
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range", L"0.0 (= all)")
	REALW (L"left Vertical range", L"0.0")
	REALW (L"right Vertical range", L"0.0 (= auto)")
	BOOLEANW (L"Garnish", 1)
	LABELW (L"", L"")
	OPTIONMENUW (L"Drawing method", 1)
		OPTIONW (L"Curve")
		OPTIONW (L"Bars")
		OPTIONW (L"Poles")
		OPTIONW (L"Speckles")
	OK
DO_ALTERNATIVE (old_Sound_draw)
	EVERY_DRAW (Sound_draw (OBJECT, GRAPHICS, GET_REALW (L"left Time range"), GET_REALW (L"right Time range"),
		GET_REALW (L"left Vertical range"), GET_REALW (L"right Vertical range"), GET_INTEGERW (L"Garnish"), GET_STRINGW (L"Drawing method")))
END

static void cb_SoundEditor_publish (Any editor, void *closure, Any publish) {
	(void) editor;
	(void) closure;
	if (! praat_new1 (publish, NULL)) { Melder_flushError (NULL); return; }
	praat_updateSelection ();
	if (Thing_member (publish, classSpectrum)) {
		int IOBJECT;
		WHERE (SELECTED) {
			SpectrumEditor editor2 = SpectrumEditor_create (theCurrentPraat -> topShell, FULL_NAMEW, OBJECT);
			if (! editor2) return;
			if (! praat_installEditor (editor2, IOBJECT)) Melder_flushError (NULL);
		}
	}
}
DIRECT (Sound_edit)
	if (theCurrentPraat -> batch) {
		return Melder_error1 (L"Cannot edit a Sound from batch.");
	} else {
		WHERE (SELECTED) {
			SoundEditor editor = SoundEditor_create (theCurrentPraat -> topShell, FULL_NAMEW, OBJECT);
			if (! editor) return 0;
			if (! praat_installEditor (editor, IOBJECT)) return 0;
			Editor_setPublishCallback (editor, cb_SoundEditor_publish, NULL);
		}
	}
END

DIRECT (Sound_extractLeftChannel)
	WHERE (SELECTED) {
		Sound me = OBJECT;
		if (! praat_new2 (Sound_extractLeftChannel (me), my name, L"_left")) return 0;
	}
END

FORMW (Sound_extractPart, L"Sound: Extract part", 0)
	REALW (L"left Time range (s)", L"0")
	REALW (L"right Time range (s)", L"0.1")
	ENUM ("Window", Sound_WINDOW, enumi (Sound_WINDOW, Hanning))
	POSITIVEW (L"Relative width", L"1.0")
	BOOLEANW (L"Preserve times", 0)
	OK
DO
	WHERE (SELECTED) {
		Sound me = OBJECT;
		if (! praat_new2 (Sound_extractPart (me,
			GET_REALW (L"left Time range"), GET_REALW (L"right Time range"),
			GET_INTEGERW (L"Window"), GET_REALW (L"Relative width"),
			GET_INTEGERW (L"Preserve times")),
			my name, L"_part")) return 0;
	}
END

DIRECT (Sound_extractRightChannel)
	WHERE (SELECTED) {
		Sound me = OBJECT;
		if (! praat_new2 (Sound_extractRightChannel (me), my name, L"_right")) return 0;
	}
END

FORMW (Sound_filter_deemphasis, L"Sound: Filter (de-emphasis)", L"Sound: Filter (de-emphasis)...")
	REALW (L"From frequency (Hz)", L"50.0")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new2 (Sound_filter_deemphasis (OBJECT, GET_REALW (L"From frequency")),
			NAMEW, L"_deemp")) return 0;
	}
END

FORMW (Sound_filter_formula, L"Sound: Filter (formula)...", L"Formula...")
	LABELW (L"", L"Frequency-domain filtering with a formula (uses Sound-to-Spectrum and Spectrum-to-Sound): x is frequency in Hertz")
	TEXTFIELDW (L"formula", L"if x<500 or x>1000 then 0 else self fi; rectangular band filter")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new2 (Sound_filter_formula (OBJECT, GET_STRINGW (L"formula")),
			NAMEW, L"_filt")) return 0;
END

FORMW (Sound_filter_oneFormant, L"Sound: Filter (one formant)", L"Sound: Filter (one formant)...")
	REALW (L"Frequency (Hz)", L"1000")
	POSITIVEW (L"Bandwidth (Hz)", L"100")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new2 (Sound_filter_oneFormant (OBJECT, GET_REALW (L"Frequency"), GET_REALW (L"Bandwidth")),
			NAMEW, L"_filt")) return 0;
	}
END

FORMW (Sound_filterWithOneFormantInline, L"Sound: Filter with one formant (in-line)", L"Sound: Filter with one formant (in-line)...")
	REALW (L"Frequency (Hz)", L"1000")
	POSITIVEW (L"Bandwidth (Hz)", L"100")
	OK
DO
	WHERE (SELECTED) {
		Sound_filterWithOneFormantInline (OBJECT, GET_REALW (L"Frequency"), GET_REALW (L"Bandwidth"));
		praat_dataChanged (OBJECT);
	}
END

FORMW (Sound_filter_passHannBand, L"Sound: Filter (pass Hann band)", L"Sound: Filter (pass Hann band)...")
	REALW (L"From frequency (Hz)", L"500")
	REALW (L"To frequency (s)", L"1000")
	POSITIVEW (L"Smoothing (Hz)", L"100")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new2 (Sound_filter_passHannBand (OBJECT,
			GET_REALW (L"From frequency"), GET_REALW (L"To frequency"), GET_REALW (L"Smoothing")),
			NAMEW, L"_band")) return 0;
	}
END

FORMW (Sound_filter_preemphasis, L"Sound: Filter (pre-emphasis)", L"Sound: Filter (pre-emphasis)...")
	REALW (L"From frequency (Hz)", L"50.0")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new2 (Sound_filter_preemphasis (OBJECT, GET_REALW (L"From frequency")),
			NAMEW, L"_preemp")) return 0;
	}
END

FORMW (Sound_filter_stopHannBand, L"Sound: Filter (stop Hann band)", L"Sound: Filter (stop Hann band)...")
	REALW (L"From frequency (Hz)", L"500")
	REALW (L"To frequency (s)", L"1000")
	POSITIVEW (L"Smoothing (Hz)", L"100")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new2 (Sound_filter_stopHannBand (OBJECT,
			GET_REALW (L"From frequency"), GET_REALW (L"To frequency"), GET_REALW (L"Smoothing")),
			NAMEW, L"_band")) return 0;
	}
END

FORMW (Sound_formula, L"Sound: Formula", L"Sound: Formula...")
	LABELW (L"label1", L"! `x' is the time in seconds, `col' is the sample number.")
	LABELW (L"label2", L"x = x1   ! time associated with first sample")
	LABELW (L"label3", L"for col from 1 to ncol")
	LABELW (L"label4", L"   self [col] = ...")
	TEXTFIELDW (L"formula", L"self")
	LABELW (L"label5", L"   x = x + dx")
	LABELW (L"label6", L"endfor")
	OK
DO
	if (! praat_Fon_formula (dia)) return 0;
END

FORMW (Sound_getAbsoluteExtremum, L"Sound: Get absolute extremum", L"Sound: Get absolute extremum...")
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"0.0 (= all)")
	RADIOW (L"Interpolation", 4)
	RADIOBUTTONW (L"None")
	RADIOBUTTONW (L"Parabolic")
	RADIOBUTTONW (L"Cubic")
	RADIOBUTTONW (L"Sinc70")
	RADIOBUTTONW (L"Sinc700")
	OK
DO
	Melder_informationReal (Vector_getAbsoluteExtremum (ONLY (classSound),
		GET_REALW (L"left Time range"), GET_REALW (L"right Time range"), GET_INTEGERW (L"Interpolation") - 1), L"Pascal");
END

FORMW (Sound_getEnergy, L"Sound: Get energy", L"Sound: Get energy...")
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"0.0 (= all)")
	OK
DO
	Melder_informationReal (Sound_getEnergy (ONLY (classSound), GET_REALW (L"left Time range"), GET_REALW (L"right Time range")), L"Pa2 sec");
END

DIRECT (Sound_getEnergyInAir)
	Melder_informationReal (Sound_getEnergyInAir (ONLY (classSound)), L"Joule/m2");
END

FORMW (Sound_getIndexFromTime, L"Get sample number from time", L"Get sample number from time...")
	REALW (L"Time (s)", L"0.5")
	OK
DO
	Melder_informationReal (Sampled_xToIndex (ONLY (classSound), GET_REALW (L"Time")), NULL);
END

DIRECT (Sound_getIntensity_dB)
	Melder_informationReal (Sound_getIntensity_dB (ONLY (classSound)), L"dB");
END

FORMW (Sound_getMaximum, L"Sound: Get maximum", L"Sound: Get maximum...")
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"0.0 (= all)")
	RADIOW (L"Interpolation", 4)
	RADIOBUTTONW (L"None")
	RADIOBUTTONW (L"Parabolic")
	RADIOBUTTONW (L"Cubic")
	RADIOBUTTONW (L"Sinc70")
	RADIOBUTTONW (L"Sinc700")
	OK
DO
	Melder_informationReal (Vector_getMaximum (ONLY (classSound),
		GET_REALW (L"left Time range"), GET_REALW (L"right Time range"), GET_INTEGERW (L"Interpolation") - 1), L"Pascal");
END

FORMW (old_Sound_getMean, L"Sound: Get mean", L"Sound: Get mean...")
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"0.0 (= all)")
	OK
DO
	Melder_informationReal (Vector_getMean (ONLY (classSound),
		GET_REALW (L"left Time range"), GET_REALW (L"right Time range"), Vector_CHANNEL_AVERAGE), L"Pascal");
END

FORMW (Sound_getMean, L"Sound: Get mean", L"Sound: Get mean...")
	OPTIONMENUW (L"Channel", 1)
		OPTIONW (L"All")
		OPTIONW (L"Left")
		OPTIONW (L"Right")
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"0.0 (= all)")
	OK
DO_ALTERNATIVE (old_Sound_getMean)
	Sound me = ONLY (classSound);
	long channel = GET_INTEGERW (L"Channel") - 1;
	if (channel > my ny) channel = 1;
	Melder_informationReal (Vector_getMean (me, GET_REALW (L"left Time range"), GET_REALW (L"right Time range"), channel), L"Pascal");
END

FORMW (Sound_getMinimum, L"Sound: Get minimum", L"Sound: Get minimum...")
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"0.0 (= all)")
	RADIOW (L"Interpolation", 4)
	RADIOBUTTONW (L"None")
	RADIOBUTTONW (L"Parabolic")
	RADIOBUTTONW (L"Cubic")
	RADIOBUTTONW (L"Sinc70")
	RADIOBUTTONW (L"Sinc700")
	OK
DO
	Melder_informationReal (Vector_getMinimum (ONLY (classSound),
		GET_REALW (L"left Time range"), GET_REALW (L"right Time range"), GET_INTEGERW (L"Interpolation") - 1), L"Pascal");
END

FORMW (old_Sound_getNearestZeroCrossing, L"Sound: Get nearest zero crossing", L"Sound: Get nearest zero crossing...")
	REALW (L"Time (s)", L"0.5")
	OK
DO
	Sound me = ONLY (classSound);
	if (my ny > 1) return Melder_error1 (L"Cannot determine a zero crossing for a stereo sound.");
	Melder_informationReal (Sound_getNearestZeroCrossing (me, GET_REALW (L"Time"), 1), L"seconds");
END

FORMW (Sound_getNearestZeroCrossing, L"Sound: Get nearest zero crossing", L"Sound: Get nearest zero crossing...")
	OPTIONMENUW (L"Channel", 1)
		OPTIONW (L"Left")
		OPTIONW (L"Right")
	REALW (L"Time (s)", L"0.5")
	OK
DO_ALTERNATIVE (old_Sound_getNearestZeroCrossing)
	Sound me = ONLY (classSound);
	long channel = GET_INTEGERW (L"Channel");
	if (channel > my ny) channel = 1;
	Melder_informationReal (Sound_getNearestZeroCrossing (me, GET_REALW (L"Time"), channel), L"seconds");
END

DIRECT (Sound_getNumberOfChannels)
	Sound me = ONLY (classSound);
	Melder_information2 (Melder_integer (my ny), my ny == 1 ? L" channel (mono)" : my ny == 2 ? L" channels (stereo)" : L"channels");
END

DIRECT (Sound_getNumberOfSamples)
	Sound me = ONLY (classSound);
	Melder_information2 (Melder_integer (my nx), L" samples");
END

FORMW (Sound_getPower, L"Sound: Get power", L"Sound: Get power...")
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"0.0 (= all)")
	OK
DO
	Melder_informationReal (Sound_getPower (ONLY (classSound), GET_REALW (L"left Time range"), GET_REALW (L"right Time range")), L"Pa2");
END

DIRECT (Sound_getPowerInAir)
	Melder_informationReal (Sound_getPowerInAir (ONLY (classSound)), L"Watt/m2");
END

FORMW (Sound_getRootMeanSquare, L"Sound: Get root-mean-square", L"Sound: Get root-mean-square...")
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"0.0 (= all)")
	OK
DO
	Melder_informationReal (Sound_getRootMeanSquare (ONLY (classSound), GET_REALW (L"left Time range"), GET_REALW (L"right Time range")), L"Pascal");
END

DIRECT (Sound_getSamplePeriod)
	Sound me = ONLY (classSound);
	Melder_informationReal (my dx, L"seconds");
END

DIRECT (Sound_getSampleRate)
	Sound me = ONLY (classSound);
	Melder_informationReal (1 / my dx, L"Hertz");
END

FORMW (old_Sound_getStandardDeviation, L"Sound: Get standard deviation", L"Sound: Get standard deviation...")
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"0.0 (= all)")
	OK
DO
	Melder_informationReal (Vector_getStandardDeviation (ONLY (classSound),
		GET_REALW (L"left Time range"), GET_REALW (L"right Time range"), Vector_CHANNEL_AVERAGE), L"Pascal");
END

FORMW (Sound_getStandardDeviation, L"Sound: Get standard deviation", L"Sound: Get standard deviation...")
	OPTIONMENUW (L"Channel", 1)
		OPTIONW (L"Average")
		OPTIONW (L"Left")
		OPTIONW (L"Right")
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"0.0 (= all)")
	OK
DO_ALTERNATIVE (old_Sound_getStandardDeviation)
	Sound me = ONLY (classSound);
	long channel = GET_INTEGERW (L"Channel") - 1;
	if (channel > my ny) channel = 1;
	Melder_informationReal (Vector_getStandardDeviation (me,
		GET_REALW (L"left Time range"), GET_REALW (L"right Time range"), channel), L"Pascal");
END

FORMW (Sound_getTimeFromIndex, L"Get time from sample number", L"Get time from sample number...")
	INTEGERW (L"Sample number", L"100")
	OK
DO
	Melder_informationReal (Sampled_indexToX (ONLY (classSound), GET_INTEGERW (L"Sample number")), L"seconds");
END

FORMW (Sound_getTimeOfMaximum, L"Sound: Get time of maximum", L"Sound: Get time of maximum...")
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"0.0 (= all)")
	RADIOW (L"Interpolation", 4)
		RADIOBUTTONW (L"None")
		RADIOBUTTONW (L"Parabolic")
		RADIOBUTTONW (L"Cubic")
		RADIOBUTTONW (L"Sinc70")
		RADIOBUTTONW (L"Sinc700")
	OK
DO
	Melder_informationReal (Vector_getXOfMaximum (ONLY (classSound),
		GET_REALW (L"left Time range"), GET_REALW (L"right Time range"), GET_INTEGERW (L"Interpolation") - 1), L"seconds");
END

FORMW (Sound_getTimeOfMinimum, L"Sound: Get time of minimum", L"Sound: Get time of minimum...")
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"0.0 (= all)")
	RADIOW (L"Interpolation", 4)
	RADIOBUTTONW (L"None")
	RADIOBUTTONW (L"Parabolic")
	RADIOBUTTONW (L"Cubic")
	RADIOBUTTONW (L"Sinc70")
	RADIOBUTTONW (L"Sinc700")
	OK
DO
	Melder_informationReal (Vector_getXOfMinimum (ONLY (classSound),
		GET_REALW (L"left Time range"), GET_REALW (L"right Time range"), GET_INTEGERW (L"Interpolation") - 1), L"seconds");
END

FORMW (old_Sound_getValueAtIndex, L"Sound: Get value at sample number", L"Sound: Get value at sample number...")
	INTEGERW (L"Sample number", L"100")
	OK
DO
	Sound me = ONLY (classSound);
	long sampleIndex = GET_INTEGERW (L"Sample number");
	Melder_informationReal (sampleIndex < 1 || sampleIndex > my nx ? NUMundefined :
		my ny == 1 ? my z [1] [sampleIndex] : 0.5 * (my z [1] [sampleIndex] + my z [2] [sampleIndex]), L"Pascal");
END

FORMW (Sound_getValueAtIndex, L"Sound: Get value at sample number", L"Sound: Get value at sample number...")
	OPTIONMENUW (L"Channel", 1)
		OPTIONW (L"Average")
		OPTIONW (L"Left")
		OPTIONW (L"Right")
	INTEGERW (L"Sample number", L"100")
	OK
DO_ALTERNATIVE (old_Sound_getValueAtIndex)
	Sound me = ONLY (classSound);
	long sampleIndex = GET_INTEGERW (L"Sample number");
	long channel = GET_INTEGERW (L"Channel") - 1;
	if (channel > my ny) channel = 1;
	Melder_informationReal (sampleIndex < 1 || sampleIndex > my nx ? NUMundefined :
		Sampled_getValueAtSample (me, sampleIndex, channel, 0), L"Pascal");
END

FORMW (old_Sound_getValueAtTime, L"Sound: Get value at time", L"Sound: Get value at time...")
	REALW (L"Time (s)", L"0.5")
	RADIOW (L"Interpolation", 4)
		RADIOBUTTONW (L"Nearest")
		RADIOBUTTONW (L"Linear")
		RADIOBUTTONW (L"Cubic")
		RADIOBUTTONW (L"Sinc70")
		RADIOBUTTONW (L"Sinc700")
	OK
DO
	Melder_informationReal (Vector_getValueAtX (ONLY (classSound), GET_REALW (L"Time"),
		Vector_CHANNEL_AVERAGE, GET_INTEGERW (L"Interpolation") - 1), L"Pascal");
END

FORMW (Sound_getValueAtTime, L"Sound: Get value at time", L"Sound: Get value at time...")
	OPTIONMENUW (L"Channel", 1)
		OPTIONW (L"Average")
		OPTIONW (L"Left")
		OPTIONW (L"Right")
	REALW (L"Time (s)", L"0.5")
	RADIOW (L"Interpolation", 4)
		RADIOBUTTONW (L"Nearest")
		RADIOBUTTONW (L"Linear")
		RADIOBUTTONW (L"Cubic")
		RADIOBUTTONW (L"Sinc70")
		RADIOBUTTONW (L"Sinc700")
	OK
DO_ALTERNATIVE (old_Sound_getValueAtTime)
	Sound me = ONLY (classSound);
	long channel = GET_INTEGERW (L"Channel") - 1;
	if (channel > my ny) channel = 1;
	Melder_informationReal (Vector_getValueAtX (ONLY (classSound), GET_REALW (L"Time"),
		channel, GET_INTEGERW (L"Interpolation") - 1), L"Pascal");
END

DIRECT (Sound_help) Melder_help (L"Sound"); END

FORMW (Sound_lengthen_overlapAdd, L"Sound: Lengthen (overlap-add)", L"Sound: Lengthen (overlap-add)...")
	POSITIVEW (L"Minimum pitch (Hz)", L"75")
	POSITIVEW (L"Maximum pitch (Hz)", L"600")
	POSITIVEW (L"Factor", L"1.5")
	OK
DO
	double minimumPitch = GET_REALW (L"Minimum pitch"), maximumPitch = GET_REALW (L"Maximum pitch");
	double factor = GET_REALW (L"Factor");
	REQUIRE (minimumPitch < maximumPitch, L"Maximum pitch should be greater than minimum pitch.")
	WHERE (SELECTED)
		if (! praat_new3 (Sound_lengthen_overlapAdd (OBJECT, minimumPitch, maximumPitch, factor),
			NAMEW, L"_", Melder_fixed (factor, 2))) return 0;
END

FORMW (Sound_multiply, L"Sound: Multiply", 0)
	REALW (L"Multiplication factor", L"1.5")
	OK
DO
	WHERE (SELECTED) {
		Vector_multiplyByScalar (OBJECT, GET_REALW (L"Multiplication factor"));
		praat_dataChanged (OBJECT);
	}
END

FORMW (Sound_multiplyByWindow, L"Sound: Multiply by window", 0)
	ENUM ("Window shape", Sound_WINDOW, enumi (Sound_WINDOW, Hanning))
	OK
DO
	WHERE (SELECTED) {
		Sound_multiplyByWindow (OBJECT, GET_INTEGERW (L"Window shape"));
		praat_dataChanged (OBJECT);
	}
END

FORMW (Sound_overrideSamplingFrequency, L"Sound: Override sampling frequency", 0)
	POSITIVEW (L"New sampling frequency (Hz)", L"16000.0")
	OK
DO
	WHERE (SELECTED) {
		Sound_overrideSamplingFrequency (OBJECT, GET_REALW (L"New sampling frequency"));
		praat_dataChanged (OBJECT);
	}
END

DIRECT (Sound_play)
	int n = 0;
	EVERY (n ++)
	if (n == 1 || Melder_getMaximumAsynchronicity () < Melder_ASYNCHRONOUS) {
		EVERY (Sound_play (OBJECT, NULL, NULL))
	} else {
		Melder_setMaximumAsynchronicity (Melder_INTERRUPTABLE);
		EVERY (Sound_play (OBJECT, NULL, NULL))
		Melder_setMaximumAsynchronicity (Melder_ASYNCHRONOUS);
	}
END

FORMW (Sound_preemphasizeInline, L"Sound: Pre-emphasize (in-line)", L"Sound: Pre-emphasize (in-line)...")
	REALW (L"From frequency (Hz)", L"50.0")
	OK
DO
	WHERE (SELECTED) {
		Sound_preEmphasis (OBJECT, GET_REALW (L"From frequency"));
		Vector_scale (OBJECT, 0.99);
		praat_dataChanged (OBJECT);
	}
END

FORM_READW (Sound_read2FromStereoFile, L"Read two Sounds from stereo file", 0)
	Sound left, right;
	if (! Sound_read2FromSoundFile (file, & left, & right)) return 0;
	if (! praat_new1 (left, L"left")) return 0;
	if (right) { if (! praat_new1 (right, L"right")) return 0; }
END

FORM_READW (Sound_readFromRawAlawFile, L"Read Sound from raw Alaw file", 0)
	if (! praat_new1 (Sound_readFromRawAlawFile (file), MelderFile_name (file))) return 0;
END

static SoundRecorder soundRecorder;   /* Only one at a time. */
static void cb_SoundRecorder_destroy (Any editor, void *closure) {
	(void) editor;
	(void) closure;
	soundRecorder = NULL;
}
static int previousNumberOfChannels;
static void cb_SoundRecorder_publish (Any editor, void *closure, Any publish) {
	(void) editor;
	(void) closure;
	if (! praat_new1 (publish, NULL)) Melder_flushError (NULL);
	praat_updateSelection ();
}
DIRECT (Sound_record_mono)
	if (theCurrentPraat -> batch) return Melder_error1 (L"Cannot record a Sound from batch.");
	if (soundRecorder) {
		if (previousNumberOfChannels == 1) {
			Editor_raise (soundRecorder);
		} else {
			forget (soundRecorder);
		}
	}
	if (! soundRecorder) {
		soundRecorder = SoundRecorder_create (theCurrentPraat -> topShell, 1, theCurrentPraat -> context);
		if (soundRecorder == NULL) return 0;
		Editor_setDestroyCallback (soundRecorder, cb_SoundRecorder_destroy, NULL);
		Editor_setPublishCallback (soundRecorder, cb_SoundRecorder_publish, NULL);
	}
	previousNumberOfChannels = 1;
END
static void cb_SoundRecorder_publish2 (Any editor, Any closure, Any publish1, Any publish2) {
	(void) editor;
	(void) closure;
	if (! praat_new1 (publish1, L"left") || ! praat_new1 (publish2, L"right")) Melder_flushError (NULL);
	praat_updateSelection ();
}
DIRECT (Sound_record_stereo)
	if (theCurrentPraat -> batch) return Melder_error1 (L"Cannot record a Sound from batch.");
	if (soundRecorder) {
		if (previousNumberOfChannels == 2) {
			Editor_raise (soundRecorder);
		} else {
			forget (soundRecorder);
		}
	}
	if (! soundRecorder) {
		soundRecorder = SoundRecorder_create (theCurrentPraat -> topShell, 2, theCurrentPraat -> context);
		if (soundRecorder == NULL) return 0;
		Editor_setDestroyCallback (soundRecorder, cb_SoundRecorder_destroy, NULL);
		Editor_setPublishCallback (soundRecorder, cb_SoundRecorder_publish, NULL);
		Editor_setPublish2Callback (soundRecorder, cb_SoundRecorder_publish2, NULL);
	}
	previousNumberOfChannels = 2;
END

FORMW (Sound_recordFixedTime, L"Record Sound", 0)
	RADIOW (L"Input source", 1)
		RADIOBUTTONW (L"Microphone")
		RADIOBUTTONW (L"Line")
	#if defined (sgi)
		RADIOBUTTONW (L"Digital")
		REALW (L"Gain (0-1)", L"0.5")
	#else
		REALW (L"Gain (0-1)", L"0.1")
	#endif
	REALW (L"Balance (0-1)", L"0.5")
	RADIOW (L"Sampling frequency", 1)
		#if defined (hpux)
			RADIOBUTTONW (L"5512")
		#endif
		#ifdef UNIX
		RADIOBUTTONW (L"8000")
		#endif
		#ifdef sgi
		RADIOBUTTONW (L"9800")
		#endif
		#ifndef macintosh
		RADIOBUTTONW (L"11025")
		#endif
		#ifdef UNIX
		RADIOBUTTONW (L"16000")
		#endif
		#ifndef macintosh
		RADIOBUTTONW (L"22050")
		#endif
		#ifdef UNIX
		RADIOBUTTONW (L"32000")
		#endif
		RADIOBUTTONW (L"44100")
		RADIOBUTTONW (L"48000")
		RADIOBUTTONW (L"96000")
	POSITIVEW (L"Duration (seconds)", L"1.0")
	OK
DO
	NEW (Sound_recordFixedTime (GET_INTEGERW (L"Input source"),
		GET_REALW (L"Gain"), GET_REALW (L"Balance"),
		wcstod (GET_STRINGW (L"Sampling frequency"), NULL), GET_REALW (L"Duration")));
END

FORMW (Sound_resample, L"Sound: Resample", L"Sound: Resample...")
	POSITIVEW (L"New sampling frequency (Hz)", L"10000")
	NATURALW (L"Precision (samples)", L"50")
	OK
DO
	double samplingFrequency = GET_REALW (L"New sampling frequency");
	WHERE (SELECTED)
		if (! praat_new3 (Sound_resample (OBJECT, samplingFrequency, GET_INTEGERW (L"Precision")),
			NAMEW, L"_", Melder_integer ((long) floor (samplingFrequency + 0.5)))) return 0;
END

DIRECT (Sound_reverse)
	WHERE (SELECTED) {
		Sound_reverse (OBJECT, 0, 0);
		praat_dataChanged (OBJECT);
	}
END

FORMW (Sound_scalePeak, L"Sound: Scale peak", 0)
	POSITIVEW (L"New maximum amplitude", L"0.99")
	OK
DO
	WHERE (SELECTED) {
		Vector_scale (OBJECT, GET_REALW (L"New maximum amplitude"));
		praat_dataChanged (OBJECT);
	}
END

FORMW (Sound_scaleIntensity, L"Sound: Scale intensity", 0)
	POSITIVEW (L"New average intensity (dB)", L"70.0")
	OK
DO
	WHERE (SELECTED) {
		Sound_scaleIntensity (OBJECT, GET_REALW (L"New average intensity"));
		praat_dataChanged (OBJECT);
	}
END

FORMW (old_Sound_setValueAtIndex, L"Sound: Set value at sample number", L"Sound: Set value at sample number...")
	NATURALW (L"Sample number", L"100")
	REALW (L"New value", L"0")
	OK
DO
	WHERE (SELECTED) {
		Sound me = OBJECT;
		long index = GET_INTEGERW (L"Sample number");
		if (index > my nx)
			return Melder_error3 (L"The sample number should not exceed the number of samples, which is ", Melder_integer (my nx), L".");
		for (long channel = 1; channel <= my ny; channel ++)
			my z [channel] [index] = GET_REALW (L"New value");
		praat_dataChanged (me);
	}
END

FORMW (Sound_setValueAtIndex, L"Sound: Set value at sample number", L"Sound: Set value at sample number...")
	OPTIONMENUW (L"Channel", 2)
		OPTIONW (L"Both")
		OPTIONW (L"Left")
		OPTIONW (L"Right")
	NATURALW (L"Sample number", L"100")
	REALW (L"New value", L"0")
	OK
DO_ALTERNATIVE (old_Sound_setValueAtIndex)
	WHERE (SELECTED) {
		Sound me = OBJECT;
		long index = GET_INTEGERW (L"Sample number");
		if (index > my nx)
			return Melder_error3 (L"The sample number should not exceed the number of samples, which is ", Melder_integer (my nx), L".");
		long channel = GET_INTEGERW (L"Channel") - 1;
		if (channel > my ny) channel = 1;
		if (channel > 0) {
			my z [channel] [index] = GET_REALW (L"New value");
		} else {
			for (channel = 1; channel <= my ny; channel ++) {
				my z [channel] [index] = GET_REALW (L"New value");
			}
		}
		praat_dataChanged (me);
	}
END

FORMW (Sound_setPartToZero, L"Sound: Set part to zero", 0)
	REALW (L"left Time range (s)", L"0.0")
	REALW (L"right Time range (s)", L"0.0 (= all)")
	RADIOW (L"Cut", 2)
		OPTIONW (L"at exactly these times")
		OPTIONW (L"at nearest zero crossing")
	OK
DO
	WHERE (SELECTED) {
		Sound_setZero (OBJECT, GET_REALW (L"left Time range"), GET_REALW (L"right Time range"), GET_INTEGERW (L"Cut") - 1);
		praat_dataChanged (OBJECT);
	}
END

DIRECT (Sound_subtractMean)
	WHERE (SELECTED) {
		Vector_subtractMean (OBJECT);
		praat_dataChanged (OBJECT);
	}
END

FORMW (Sound_to_Manipulation, L"Sound: To Manipulation", L"Manipulation")
	POSITIVEW (L"Time step (s)", L"0.01")
	POSITIVEW (L"Minimum pitch (Hz)", L"75")
	POSITIVEW (L"Maximum pitch (Hz)", L"600")
	OK
DO
	double fmin = GET_REALW (L"Minimum pitch"), fmax = GET_REALW (L"Maximum pitch");
	REQUIRE (fmax > fmin, L"Maximum pitch must be greater than minimum pitch.");
	EVERY_TO (Sound_to_Manipulation (OBJECT, GET_REALW (L"Time step"), fmin, fmax))
END

FORMW (Sound_to_Cochleagram, L"Sound: To Cochleagram", 0)
	POSITIVEW (L"Time step (s)", L"0.01")
	POSITIVEW (L"Frequency resolution (Bark)", L"0.1")
	POSITIVEW (L"Window length (s)", L"0.03")
	REALW (L"Forward-masking time (s)", L"0.03")
	OK
DO
	EVERY_TO (Sound_to_Cochleagram (OBJECT, GET_REALW (L"Time step"),
		GET_REALW (L"Frequency resolution"), GET_REALW (L"Window length"), GET_REALW (L"Forward-masking time")))
END

FORMW (Sound_to_Cochleagram_edb, L"Sound: To Cochleagram (De Boer, Meddis & Hewitt)", 0)
	POSITIVEW (L"Time step (s)", L"0.01")
	POSITIVEW (L"Frequency resolution (Bark)", L"0.1")
	BOOLEANW (L"Has synapse", 1)
	LABELW (L"", L"Meddis synapse properties")
	POSITIVEW (L"   replenishment rate (/sec)", L"5.05")
	POSITIVEW (L"   loss rate (/sec)", L"2500")
	POSITIVEW (L"   return rate (/sec)", L"6580")
	POSITIVEW (L"   reprocessing rate (/sec)", L"66.31")
	OK
DO
	EVERY_TO (Sound_to_Cochleagram_edb (OBJECT, GET_REALW (L"Time step"),
		GET_REALW (L"Frequency resolution"), GET_INTEGERW (L"Has synapse"),
		GET_REALW (L"   replenishment rate"), GET_REALW (L"   loss rate"),
		GET_REALW (L"   return rate"), GET_REALW (L"   reprocessing rate")))
END

FORMW (Sound_to_Formant_burg, L"Sound: To Formant (Burg method)", L"Sound: To Formant (burg)...")
	REALW (L"Time step (s)", L"0.0 (= auto)")
	POSITIVEW (L"Max. number of formants", L"5")
	REALW (L"Maximum formant (Hz)", L"5500 (= adult female)")
	POSITIVEW (L"Window length (s)", L"0.025")
	POSITIVEW (L"Pre-emphasis from (Hz)", L"50")
	OK
DO
	EVERY_TO (Sound_to_Formant_burg (OBJECT, GET_REALW (L"Time step"),
		GET_REALW (L"Max. number of formants"), GET_REALW (L"Maximum formant"),
		GET_REALW (L"Window length"), GET_REALW (L"Pre-emphasis from")))
END

FORMW (Sound_to_Formant_keepAll, L"Sound: To Formant (keep all)", L"Sound: To Formant (keep all)...")
	REALW (L"Time step (s)", L"0.0 (= auto)")
	POSITIVEW (L"Max. number of formants", L"5")
	REALW (L"Maximum formant (Hz)", L"5500 (= adult female)")
	POSITIVEW (L"Window length (s)", L"0.025")
	POSITIVEW (L"Pre-emphasis from (Hz)", L"50")
	OK
DO
	EVERY_TO (Sound_to_Formant_keepAll (OBJECT, GET_REALW (L"Time step"),
		GET_REALW (L"Max. number of formants"), GET_REALW (L"Maximum formant"),
		GET_REALW (L"Window length"), GET_REALW (L"Pre-emphasis from")))
END

FORMW (Sound_to_Formant_willems, L"Sound: To Formant (split Levinson (Willems))", L"Sound: To Formant (sl)...")
	REALW (L"Time step (s)", L"0.0 (= auto)")
	POSITIVEW (L"Number of formants", L"5")
	REALW (L"Maximum formant (Hz)", L"5500 (= adult female)")
	POSITIVEW (L"Window length (s)", L"0.025")
	POSITIVEW (L"Pre-emphasis from (Hz)", L"50")
	OK
DO
	EVERY_TO (Sound_to_Formant_willems (OBJECT, GET_REALW (L"Time step"),
		GET_REALW (L"Number of formants"), GET_REALW (L"Maximum formant"),
		GET_REALW (L"Window length"), GET_REALW (L"Pre-emphasis from")))
END

FORMW (Sound_to_Harmonicity_ac, L"Sound: To Harmonicity (ac)", L"Sound: To Harmonicity (ac)...")
	POSITIVEW (L"Time step (s)", L"0.01")
	POSITIVEW (L"Minimum pitch (Hz)", L"75")
	REALW (L"Silence threshold", L"0.1")
	POSITIVEW (L"Periods per window", L"4.5")
	OK
DO
	double periodsPerWindow = GET_REALW (L"Periods per window");
	REQUIRE (periodsPerWindow >= 3.0, L"Number of periods per window must be >= 3.")
	EVERY_TO (Sound_to_Harmonicity_ac (OBJECT, GET_REALW (L"Time step"),
		GET_REALW (L"Minimum pitch"), GET_REALW (L"Silence threshold"), periodsPerWindow))
END

FORMW (Sound_to_Harmonicity_cc, L"Sound: To Harmonicity (cc)", L"Sound: To Harmonicity (cc)...")
	POSITIVEW (L"Time step (s)", L"0.01")
	POSITIVEW (L"Minimum pitch (Hz)", L"75")
	REALW (L"Silence threshold", L"0.1")
	POSITIVEW (L"Periods per window", L"1.0")
	OK
DO
	EVERY_TO (Sound_to_Harmonicity_cc (OBJECT, GET_REALW (L"Time step"),
		GET_REALW (L"Minimum pitch"), GET_REALW (L"Silence threshold"),
		GET_REALW (L"Periods per window")))
END

FORMW (Sound_to_Harmonicity_gne, L"Sound: To Harmonicity (gne)", 0)
	POSITIVEW (L"Minimum frequency (Hz)", L"500")
	POSITIVEW (L"Maximum frequency (Hz)", L"4500")
	POSITIVEW (L"Bandwidth (Hz)", L"1000")
	POSITIVEW (L"Step (Hz)", L"80")
	OK
DO
	EVERY_TO (Sound_to_Harmonicity_GNE (OBJECT, GET_REALW (L"Minimum frequency"),
		GET_REALW (L"Maximum frequency"), GET_REALW (L"Bandwidth"),
		GET_REALW (L"Step")))
END

FORMW (old_Sound_to_Intensity, L"Sound: To Intensity", L"Sound: To Intensity...")
	POSITIVEW (L"Minimum pitch (Hz)", L"100")
	REALW (L"Time step (s)", L"0.0 (= auto)")
	OK
DO
	EVERY_TO (Sound_to_Intensity ((Sound) OBJECT,
		GET_REALW (L"Minimum pitch"), GET_REALW (L"Time step"), FALSE))
END

FORMW (Sound_to_Intensity, L"Sound: To Intensity", L"Sound: To Intensity...")
	POSITIVEW (L"Minimum pitch (Hz)", L"100")
	REALW (L"Time step (s)", L"0.0 (= auto)")
	BOOLEANW (L"Subtract mean", 1)
	OK
DO_ALTERNATIVE (old_Sound_to_Intensity)
	EVERY_TO (Sound_to_Intensity ((Sound) OBJECT,
		GET_REALW (L"Minimum pitch"), GET_REALW (L"Time step"), GET_INTEGERW (L"Subtract mean")))
END

DIRECT (Sound_to_IntervalTier)
	EVERY_TO (IntervalTier_create (((Sound) OBJECT) -> xmin, ((Sound) OBJECT) -> xmax))
END

FORMW (Sound_to_Ltas, L"Sound: To long-term average spectrum", 0)
	POSITIVEW (L"Bandwidth (Hz)", L"100")
	OK
DO
	EVERY_TO (Sound_to_Ltas (OBJECT, GET_REALW (L"Bandwidth")))
END

FORMW (Sound_to_Ltas_pitchCorrected, L"Sound: To Ltas (pitch-corrected)", L"Sound: To Ltas (pitch-corrected)...")
	POSITIVEW (L"Minimum pitch (Hz)", L"75")
	POSITIVEW (L"Maximum pitch (Hz)", L"600")
	POSITIVEW (L"Maximum frequency (Hz)", L"5000")
	POSITIVEW (L"Bandwidth (Hz)", L"100")
	REALW (L"Shortest period (s)", L"0.0001")
	REALW (L"Longest period (s)", L"0.02")
	POSITIVEW (L"Maximum period factor", L"1.3")
	OK
DO
	double fmin = GET_REALW (L"Minimum pitch"), fmax = GET_REALW (L"Maximum pitch");
	REQUIRE (fmax > fmin, L"Maximum pitch must be greater than minimum pitch.");
	EVERY_TO (Sound_to_Ltas_pitchCorrected (OBJECT, fmin, fmax,
		GET_REALW (L"Maximum frequency"), GET_REALW (L"Bandwidth"),
		GET_REALW (L"Shortest period"), GET_REALW (L"Longest period"), GET_REALW (L"Maximum period factor")))
END

DIRECT (Sound_to_Matrix)
	EVERY_TO (Sound_to_Matrix (OBJECT))
END

DIRECT (Sounds_to_ParamCurve)
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	if (! praat_new3 (ParamCurve_create (s1, s2), s1 -> name, L"_", s2 -> name)) return 0;
END

FORMW (Sound_to_Pitch, L"Sound: To Pitch", L"Sound: To Pitch...")
	REALW (L"Time step (s)", L"0.0 (= auto)")
	POSITIVEW (L"Pitch floor (Hz)", L"75.0")
	POSITIVEW (L"Pitch ceiling (Hz)", L"600.0")
	OK
DO
	EVERY_TO (Sound_to_Pitch (OBJECT, GET_REALW (L"Time step"),
		GET_REALW (L"Pitch floor"), GET_REALW (L"Pitch ceiling")))
END

FORMW (Sound_to_Pitch_ac, L"Sound: To Pitch (ac)", L"Sound: To Pitch (ac)...")
	LABELW (L"", L"Finding the candidates")
	REALW (L"Time step (s)", L"0.0 (= auto)")
	POSITIVEW (L"Pitch floor (Hz)", L"75.0")
	NATURALW (L"Max. number of candidates", L"15")
	BOOLEANW (L"Very accurate", 0)
	LABELW (L"", L"Finding a path")
	REALW (L"Silence threshold", L"0.03")
	REALW (L"Voicing threshold", L"0.45")
	REALW (L"Octave cost", L"0.01")
	REALW (L"Octave-jump cost", L"0.35")
	REALW (L"Voiced / unvoiced cost", L"0.14")
	POSITIVEW (L"Pitch ceiling (Hz)", L"600.0")
	OK
DO
	long maxnCandidates = GET_INTEGERW (L"Max. number of candidates");
	REQUIRE (maxnCandidates >= 2, L"Maximum number of candidates must be greater than 1.")
	EVERY_TO (Sound_to_Pitch_ac (OBJECT, GET_REALW (L"Time step"),
		GET_REALW (L"Pitch floor"), 3.0, maxnCandidates, GET_INTEGERW (L"Very accurate"),
		GET_REALW (L"Silence threshold"), GET_REALW (L"Voicing threshold"),
		GET_REALW (L"Octave cost"), GET_REALW (L"Octave-jump cost"),
		GET_REALW (L"Voiced / unvoiced cost"), GET_REALW (L"Pitch ceiling")))
END

FORMW (Sound_to_Pitch_cc, L"Sound: To Pitch (cc)", L"Sound: To Pitch (cc)...")
	LABELW (L"", L"Finding the candidates")
	REALW (L"Time step (s)", L"0.0 (= auto)")
	POSITIVEW (L"Pitch floor (Hz)", L"75")
	NATURALW (L"Max. number of candidates", L"15")
	BOOLEANW (L"Very accurate", 0)
	LABELW (L"", L"Finding a path")
	REALW (L"Silence threshold", L"0.03")
	REALW (L"Voicing threshold", L"0.45")
	REALW (L"Octave cost", L"0.01")
	REALW (L"Octave-jump cost", L"0.35")
	REALW (L"Voiced / unvoiced cost", L"0.14")
	POSITIVEW (L"Pitch ceiling (Hz)", L"600")
	OK
DO
	long maxnCandidates = GET_INTEGERW (L"Max. number of candidates");
	REQUIRE (maxnCandidates >= 2, L"Maximum number of candidates must be greater than 1.")
	EVERY_TO (Sound_to_Pitch_cc (OBJECT, GET_REALW (L"Time step"),
		GET_REALW (L"Pitch floor"), 1.0, maxnCandidates, GET_INTEGERW (L"Very accurate"),
		GET_REALW (L"Silence threshold"), GET_REALW (L"Voicing threshold"),
		GET_REALW (L"Octave cost"), GET_REALW (L"Octave-jump cost"),
		GET_REALW (L"Voiced / unvoiced cost"), GET_REALW (L"Pitch ceiling")))
END

FORMW (Sound_to_PointProcess_extrema, L"Sound: To PointProcess (extrema)", 0)
	OPTIONMENUW (L"Channel", 1)
		OPTIONW (L"Left")
		OPTIONW (L"Right")
	BOOLEANW (L"Include maxima", 1)
	BOOLEANW (L"Include minima", 0)
	RADIOW (L"Interpolation", 4)
	RADIOBUTTONW (L"None")
	RADIOBUTTONW (L"Parabolic")
	RADIOBUTTONW (L"Cubic")
	RADIOBUTTONW (L"Sinc70")
	RADIOBUTTONW (L"Sinc700")
	OK
DO
	long channel = GET_INTEGERW (L"Channel");
	EVERY_TO (Sound_to_PointProcess_extrema (OBJECT, channel > ((Sound) OBJECT) -> ny ? 1 : channel, GET_INTEGERW (L"Interpolation") - 1,
		GET_INTEGERW (L"Include maxima"), GET_INTEGERW (L"Include minima")))
END

FORMW (Sound_to_PointProcess_periodic_cc, L"Sound: To PointProcess (periodic, cc)", L"Sound: To PointProcess (periodic, cc)...")
	POSITIVEW (L"Minimum pitch (Hz)", L"75")
	POSITIVEW (L"Maximum pitch (Hz)", L"600")
	OK
DO
	double fmin = GET_REALW (L"Minimum pitch"), fmax = GET_REALW (L"Maximum pitch");
	REQUIRE (fmax > fmin, L"Maximum pitch must be greater than minimum pitch.");
	EVERY_TO (Sound_to_PointProcess_periodic_cc (OBJECT, fmin, fmax))
END

FORMW (Sound_to_PointProcess_periodic_peaks, L"Sound: To PointProcess (periodic, peaks)", L"Sound: To PointProcess (periodic, peaks)...")
	POSITIVEW (L"Minimum pitch (Hz)", L"75")
	POSITIVEW (L"Maximum pitch (Hz)", L"600")
	BOOLEANW (L"Include maxima", 1)
	BOOLEANW (L"Include minima", 0)
	OK
DO
	double fmin = GET_REALW (L"Minimum pitch"), fmax = GET_REALW (L"Maximum pitch");
	REQUIRE (fmax > fmin, L"Maximum pitch must be greater than minimum pitch.");
	EVERY_TO (Sound_to_PointProcess_periodic_peaks (OBJECT, fmin, fmax, GET_INTEGERW (L"Include maxima"), GET_INTEGERW (L"Include minima")))
END

FORMW (Sound_to_PointProcess_zeroes, L"Get zeroes", 0)
	OPTIONMENUW (L"Channel", 1)
		OPTIONW (L"Left")
		OPTIONW (L"Right")
	BOOLEANW (L"Include raisers", 1)
	BOOLEANW (L"Include fallers", 0)
	OK
DO
	long channel = GET_INTEGERW (L"Channel");
	EVERY_TO (Sound_to_PointProcess_zeroes (OBJECT, channel > ((Sound) OBJECT) -> ny ? 1 : channel,
		GET_INTEGERW (L"Include raisers"), GET_INTEGERW (L"Include fallers")))
END

FORMW (Sound_to_Spectrogram, L"Sound: To Spectrogram", L"Sound: To Spectrogram...")
	POSITIVEW (L"Window length (s)", L"0.005")
	POSITIVEW (L"Maximum frequency (Hz)", L"5000")
	POSITIVEW (L"Time step (s)", L"0.002")
	POSITIVEW (L"Frequency step (Hz)", L"20")
	RADIOW (L"Window shape", 6)
	for (int i = 0; i < 6; i ++) {
		RADIOBUTTONW (Sound_to_Spectrogram_windowShapeText (i))
	}
	OK
DO
	EVERY_TO (Sound_to_Spectrogram (OBJECT, GET_REALW (L"Window length"),
		GET_REALW (L"Maximum frequency"), GET_REALW (L"Time step"),
		GET_REALW (L"Frequency step"), GET_INTEGERW (L"Window shape") - 1, 8.0, 8.0))
END

FORMW (Sound_to_Spectrum, L"Sound: To Spectrum", L"Sound: To Spectrum...")
	BOOLEANW (L"Fast", 1)
	OK
DO
	EVERY_TO (Sound_to_Spectrum (OBJECT, GET_INTEGERW (L"Fast")))
END

DIRECT (Sound_to_Spectrum_dft)
	EVERY_TO (Sound_to_Spectrum (OBJECT, FALSE))
END

DIRECT (Sound_to_Spectrum_fft)
	EVERY_TO (Sound_to_Spectrum (OBJECT, TRUE))
END

FORMW (Sound_to_TextGrid, L"Sound: To TextGrid", L"Sound: To TextGrid...")
	SENTENCEW (L"All tier names", L"Mary John bell")
	SENTENCEW (L"Which of these are point tiers?", L"bell")
	OK
DO
	EVERY_TO (TextGrid_create (((Sound) OBJECT) -> xmin, ((Sound) OBJECT) -> xmax,
		GET_STRINGW (L"All tier names"), GET_STRINGW (L"Which of these are point tiers?")))
END

DIRECT (Sound_to_TextTier)
	EVERY_TO (TextTier_create (((Sound) OBJECT) -> xmin, ((Sound) OBJECT) -> xmax))
END

FORMW (SoundInputPrefs, L"Sound recording preferences", L"SoundRecorder")
	NATURALW (L"Buffer size (MB)", L"20")
	OK
SET_INTEGERW (L"Buffer size", SoundRecorder_getBufferSizePref_MB ())
DO
	long size = GET_INTEGERW (L"Buffer size");
	REQUIRE (size <= 1000, L"Buffer size cannot exceed 1000 megabytes.")
	SoundRecorder_setBufferSizePref_MB (size);
END

FORMW (SoundOutputPrefs, L"Sound playing preferences", 0)
	#if defined (sun) || defined (HPUX)
		RADIOW (L"Internal speaker", 1)
		RADIOBUTTONW (L"On")
		RADIOBUTTONW (L"Off")
	#endif
	#if defined (pietjepuk)
		REALW (L"Output gain (0..1)", L"0.3")
	#endif
	LABELW (L"", L"The following determines how sounds are played.")
	LABELW (L"", L"Between parentheses, you find what you can do simultaneously.")
	LABELW (L"", L"Decrease asynchronicity if sound plays with discontinuities.")
	OPTIONMENUW (L"Maximum asynchronicity", 4)
	OPTIONW (L"Synchronous (nothing)")
	OPTIONW (L"Calling back (view running cursor)")
	OPTIONW (L"Interruptable (Escape key stops playing)")
	OPTIONW (L"Asynchronous (anything)")
	REALW (L"Silence before and after (s)", L"0.0")
	OK
#if defined (sun) || defined (HPUX)
	SET_INTEGERW (L"Internal speaker", 2 - Melder_getUseInternalSpeaker ())
#endif
#if defined (pietjepuk)
	SET_REAL ("Output gain", Melder_getOutputGain ())
#endif
SET_INTEGERW (L"Maximum asynchronicity", Melder_getMaximumAsynchronicity () + 1);
SET_REAL ("Silence before and after", Melder_getZeroPadding ());
DO
	#if defined (sun) || defined (HPUX)
		Melder_setUseInternalSpeaker (2 - GET_INTEGERW (L"Internal speaker"));
	#endif
	#if defined (pietjepuk)
		Melder_setOutputGain (GET_REALW (L"Gain"));
	#endif
	Melder_setMaximumAsynchronicity (GET_INTEGERW (L"Maximum asynchronicity") - 1);
	Melder_setZeroPadding (GET_REALW (L"Silence before and after"));
END

FORM_WRITEW (Sound_writeToAifcFile, L"Write to AIFC file", 0, L"aifc")
	if (! pr_LongSound_concatenate (file, Melder_AIFC)) return 0;
END

FORM_WRITEW (Sound_writeToAiffFile, L"Write to AIFF file", 0, L"aiff")
	if (! pr_LongSound_concatenate (file, Melder_AIFF)) return 0;
END

FORM_WRITEW (Sound_writeToRaw8bitUnsignedFile, L"Write to raw 8-bit unsigned sound file", 0, L"8uns")
	if (! Sound_writeToRaw8bitUnsignedFile (ONLY_OBJECT, file)) return 0;
END

FORM_WRITEW (Sound_writeToRaw8bitSignedFile, L"Write to raw 8-bit signed sound file", 0, L"8sig")
	if (! Sound_writeToRaw8bitSignedFile (ONLY_OBJECT, file)) return 0;
END

FORMW (Sound_writeToRawSoundFile, L"Write to raw sound file", 0)
	LABELW (L"", L"Raw binary file:")
	TEXTFIELDW (L"Raw binary file", L"")
	RADIOW (L"Encoding", 3)
		RADIOBUTTONW (L"Linear 8-bit signed")
		RADIOBUTTONW (L"Linear 8-bit unsigned")
		RADIOBUTTONW (L"Linear 16-bit big-endian")
		RADIOBUTTONW (L"Linear 16-bit little-endian")
	OK
DO
	structMelderFile file = { 0 };
	Melder_relativePathToFile (GET_STRINGW (L"Raw binary file"), & file);
	if (! Sound_writeToRawSoundFile (ONLY_OBJECT, & file, GET_INTEGERW (L"Encoding"))) return 0;
END

FORM_WRITEW (Sound_writeToKayFile, L"Write to Kay sound file", 0, L"kay")
	if (! Sound_writeToKayFile (ONLY_OBJECT, file)) return 0;
END

#ifdef macintosh
FORM_WRITEW (Sound_writeToMacSoundFile, L"Write to Macintosh sound file", 0, L"macsound")
	if (! Sound_writeToMacSoundFile (ONLY_OBJECT, file)) return 0;
END
#endif

FORM_WRITEW (Sound_writeToNextSunFile, L"Write to NeXT/Sun file", 0, L"au")
	if (! pr_LongSound_concatenate (file, Melder_NEXT_SUN)) return 0;
END

FORM_WRITEW (Sound_writeToNistFile, L"Write to NIST file", 0, L"nist")
	if (! pr_LongSound_concatenate (file, Melder_NIST)) return 0;
END

FORM_WRITEW (Sound_writeToFlacFile, L"Write to FLAC file", 0, L"flac")
	if (! pr_LongSound_concatenate (file, Melder_FLAC)) return 0;
END

FORM_WRITEW (Sound_writeToSesamFile, L"Write to Sesam file", 0, L"sdf")
	if (! Sound_writeToSesamFile (ONLY_OBJECT, file)) return 0;
END

FORM_WRITEW (Sound_writeToStereoAifcFile, L"Write to stereo AIFC file", 0, L"aifc")
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	Melder_assert (s1 && s2);
	Sound stereo = Sounds_combineToStereo (s1, s2); if (stereo == NULL) return 0;
	if (! Sound_writeToAudioFile16 (stereo, file, Melder_AIFC)) { forget (stereo); return 0; }
	forget (stereo);
END

FORM_WRITEW (Sound_writeToStereoAiffFile, L"Write to stereo AIFF file", 0, L"aiff")
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	Melder_assert (s1 && s2);
	Sound stereo = Sounds_combineToStereo (s1, s2); if (stereo == NULL) return 0;
	if (! Sound_writeToAudioFile16 (stereo, file, Melder_AIFF)) { forget (stereo); return 0; }
	forget (stereo);
END

FORM_WRITEW (Sound_writeToStereoNextSunFile, L"Write to stereo NeXT/Sun file", 0, L"au")
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	Melder_assert (s1 && s2);
	Sound stereo = Sounds_combineToStereo (s1, s2); if (stereo == NULL) return 0;
	if (! Sound_writeToAudioFile16 (stereo, file, Melder_NEXT_SUN)) { forget (stereo); return 0; }
	forget (stereo);
END

FORM_WRITEW (Sound_writeToStereoNistFile, L"Write to stereo NIST file", 0, L"nist")
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	Melder_assert (s1 && s2);
	Sound stereo = Sounds_combineToStereo (s1, s2); if (stereo == NULL) return 0;
	if (! Sound_writeToAudioFile16 (stereo, file, Melder_NIST)) { forget (stereo); return 0; }
	forget (stereo);
END

FORM_WRITEW (Sound_writeToStereoFlacFile, L"Write to stereo FLAC file", 0, L"flac")
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	Melder_assert (s1 && s2);
	Sound stereo = Sounds_combineToStereo (s1, s2); if (stereo == NULL) return 0;
	if (! Sound_writeToAudioFile16 (stereo, file, Melder_FLAC)) { forget (stereo); return 0; }
	forget (stereo);
END

FORM_WRITEW (Sound_writeToStereoWavFile, L"Write to stereo WAV file", 0, L"wav")
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	Melder_assert (s1 && s2);
	Sound stereo = Sounds_combineToStereo (s1, s2); if (stereo == NULL) return 0;
	if (! Sound_writeToAudioFile16 (stereo, file, Melder_WAV)) { forget (stereo); return 0; }
	forget (stereo);
END

FORM_WRITEW (Sound_writeToSunAudioFile, L"Write to NeXT/Sun file", 0, L"au")
	if (! pr_LongSound_concatenate (file, Melder_NEXT_SUN)) return 0;
END

FORM_WRITEW (Sound_writeToWavFile, L"Write to WAV file", 0, L"wav")
	if (! pr_LongSound_concatenate (file, Melder_WAV)) return 0;
END

/***** STOP *****/

DIRECT (stopPlayingSound)
	Melder_stopPlaying (Melder_IMPLICIT);
END

/***** Help menus *****/

DIRECT (AnnotationTutorial) Melder_help (L"Intro 7. Annotation"); END
DIRECT (FilteringTutorial) Melder_help (L"Filtering"); END

/***** file recognizers *****/

static Any macSoundOrEmptyFileRecognizer (int nread, const char *header, MelderFile file) {
	/***** No data in file? This may be a Macintosh sound file with only a resource fork. *****/
	(void) header;
	if (nread > 0) return NULL;
	#ifdef macintosh
		return Sound_readFromMacSoundFile (file);
	#else
		return Melder_errorp ("File %.200s is empty", MelderFile_messageName (file));   /* !!! */
	#endif
}

static Any soundFileRecognizer (int nread, const char *header, MelderFile file) {
	if (nread < 16) return NULL;
	if ((strnequ (header, "FORM", 4) && strnequ (header + 8, "AIF", 3)) ||
	    (strnequ (header, "RIFF", 4) && (strnequ (header + 8, "WAVE", 4) || strnequ (header + 8, "CDDA", 4))) ||
	    strnequ (header, ".snd", 4) ||
	    strnequ (header, "NIST_1A", 7) ||
	    strnequ (header, "fLaC", 4) || /* Erez Volk, March 2007 */
	    mp3_recognize (nread, header) /* Erez Volk, May 2007 */
	    #ifdef macintosh
	    	|| MelderFile_getMacType (file) == 'Sd2f'
	    #endif
	    )
		return Sound_readFromSoundFile (file);
	return NULL;
}

static Any movieFileRecognizer (int nread, const char *header, MelderFile file) {
	wchar_t *fileName = MelderFile_name (file);
	(void) header;
	/*Melder_error ("%d %d %d %d %d %d %d %d %d %d", header [0],
		header [1], header [2], header [3],
		header [4], header [5], header [6],
		header [7], header [8], header [9]);*/
	if (nread < 512 || (! wcsstr (fileName, L".mov") && ! wcsstr (fileName, L".MOV") &&
	    ! wcsstr (fileName, L".avi") && ! wcsstr (fileName, L".AVI"))) return NULL;
	return Sound_readFromMovieFile (file);
}

static Any sesamFileRecognizer (int nread, const char *header, MelderFile file) {
	wchar_t *fileName = MelderFile_name (file);
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
	Melder_stopPlaying (Melder_IMPLICIT);
	melderSound = Sound_recordFixedTime (1, 1.0, 0.5, 44100, duration);
	if (! melderSound) return 0;
	last = melderSound;
	return 1;
}
static int recordFromFileProc (MelderFile file) {
	if (last == melderSoundFromFile) last = NULL;
	forget (melderSoundFromFile);
	Melder_warningOff ();   /* Like "misssing samples". */
	melderSoundFromFile = Data_readFromFile (file);
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

	SoundRecorder_prefs ();
	FunctionEditor_prefs ();
	LongSound_prefs ();
	TimeSoundEditor_prefs ();
	TimeSoundAnalysisEditor_prefs ();

	Melder_setRecordProc (recordProc);
	Melder_setRecordFromFileProc (recordFromFileProc);
	Melder_setPlayProc (playProc);
	Melder_setPlayReverseProc (playReverseProc);
	Melder_setPublishPlayedProc (publishPlayedProc);

	praat_addMenuCommand ("Objects", "New", "Record mono Sound...", 0, 0, DO_Sound_record_mono);
	praat_addMenuCommand ("Objects", "New", "Record stereo Sound...", 0, 0, DO_Sound_record_stereo);
	praat_addMenuCommand ("Objects", "New", "Record Sound (fixed time)...", 0, praat_HIDDEN, DO_Sound_recordFixedTime);
	praat_addMenuCommand ("Objects", "New", "Sound", 0, 0, 0);
		praat_addMenuCommand ("Objects", "New", "Create Sound...", 0, praat_HIDDEN + praat_DEPTH_1, DO_Sound_create);
		praat_addMenuCommand ("Objects", "New", "Create Sound from formula...", 0, 1, DO_Sound_createFromFormula);
		praat_addMenuCommand ("Objects", "New", "Create Sound from tone complex...", 0, 1, DO_Sound_createFromToneComplex);

	praat_addMenuCommand ("Objects", "Read", "-- read sound --", 0, 0, 0);
	praat_addMenuCommand ("Objects", "Read", "Open long sound file...", 0, 'L', DO_LongSound_open);
	praat_addMenuCommand ("Objects", "Read", "Read two Sounds from stereo file...", 0, 0, DO_Sound_read2FromStereoFile);
	praat_addMenuCommand ("Objects", "Read", "Read from special sound file", 0, 0, 0);
		praat_addMenuCommand ("Objects", "Read", "Read Sound from raw Alaw file...", 0, 1, DO_Sound_readFromRawAlawFile);

	praat_addMenuCommand ("Objects", "Goodies", "Stop playing sound", 0, motif_ESCAPE, DO_stopPlayingSound);
	praat_addMenuCommand ("Objects", "Preferences", "-- sound prefs --", 0, 0, 0);
	praat_addMenuCommand ("Objects", "Preferences", "Sound recording preferences...", 0, 0, DO_SoundInputPrefs);
	praat_addMenuCommand ("Objects", "Preferences", "Sound playing preferences...", 0, 0, DO_SoundOutputPrefs);
	praat_addMenuCommand ("Objects", "Preferences", "LongSound preferences...", 0, 0, DO_LongSoundPrefs);

	praat_addAction1 (classLongSound, 0, "LongSound help", 0, 0, DO_LongSound_help);
	praat_addAction1 (classLongSound, 1, "View", 0, 0, DO_LongSound_view);
	praat_addAction1 (classLongSound, 0, "Play part...", 0, 0, DO_LongSound_playPart);
	praat_addAction1 (classLongSound, 1, "Query -          ", 0, 0, 0);
		praat_TimeFunction_query_init (classLongSound);
		praat_addAction1 (classLongSound, 1, "Sampling", 0, 1, 0);
		praat_addAction1 (classLongSound, 1, "Get number of samples", 0, 2, DO_LongSound_getNumberOfSamples);
		praat_addAction1 (classLongSound, 1, "Get sampling period", 0, 2, DO_LongSound_getSamplePeriod);
							praat_addAction1 (classLongSound, 1, "Get sample duration", 0, praat_HIDDEN + praat_DEPTH_2, DO_LongSound_getSamplePeriod);
							praat_addAction1 (classLongSound, 1, "Get sample period", 0, praat_HIDDEN + praat_DEPTH_2, DO_LongSound_getSamplePeriod);
		praat_addAction1 (classLongSound, 1, "Get sampling frequency", 0, 2, DO_LongSound_getSampleRate);
							praat_addAction1 (classLongSound, 1, "Get sample rate", 0, praat_HIDDEN + praat_DEPTH_2, DO_LongSound_getSampleRate);   /* grandfathered 2004 */
		praat_addAction1 (classLongSound, 1, "-- get time discretization --", 0, 2, 0);
		praat_addAction1 (classLongSound, 1, "Get time from sample number...", 0, 2, DO_LongSound_getTimeFromIndex);
							praat_addAction1 (classLongSound, 1, "Get time from index...", 0, praat_HIDDEN + praat_DEPTH_2, DO_LongSound_getTimeFromIndex);
		praat_addAction1 (classLongSound, 1, "Get sample number from time...", 0, 2, DO_LongSound_getIndexFromTime);
							praat_addAction1 (classLongSound, 1, "Get index from time...", 0, praat_HIDDEN + praat_DEPTH_2, DO_LongSound_getIndexFromTime);
	praat_addAction1 (classLongSound, 0, "Annotate -", 0, 0, 0);
		praat_addAction1 (classLongSound, 0, "Annotation tutorial", 0, 1, DO_AnnotationTutorial);
		praat_addAction1 (classLongSound, 0, "-- to text grid --", 0, 1, 0);
		praat_addAction1 (classLongSound, 0, "To TextGrid...", 0, 1, DO_LongSound_to_TextGrid);
	praat_addAction1 (classLongSound, 0, "Convert to Sound", 0, 0, 0);
	praat_addAction1 (classLongSound, 0, "Extract part...", 0, 0, DO_LongSound_extractPart);
	praat_addAction1 (classLongSound, 0, "Write to WAV file...", 0, 0, DO_LongSound_writeToWavFile);
	praat_addAction1 (classLongSound, 0, "Write to AIFF file...", 0, 0, DO_LongSound_writeToAiffFile);
	praat_addAction1 (classLongSound, 0, "Write to AIFC file...", 0, 0, DO_LongSound_writeToAifcFile);
	praat_addAction1 (classLongSound, 0, "Write to Next/Sun file...", 0, 0, DO_LongSound_writeToNextSunFile);
	praat_addAction1 (classLongSound, 0, "Write to NIST file...", 0, 0, DO_LongSound_writeToNistFile);
	praat_addAction1 (classLongSound, 0, "Write to FLAC file...", 0, 0, DO_LongSound_writeToFlacFile);
	praat_addAction1 (classLongSound, 0, "Write left channel to WAV file...", 0, 0, DO_LongSound_writeLeftChannelToWavFile);
	praat_addAction1 (classLongSound, 0, "Write left channel to AIFF file...", 0, 0, DO_LongSound_writeLeftChannelToAiffFile);
	praat_addAction1 (classLongSound, 0, "Write left channel to AIFC file...", 0, 0, DO_LongSound_writeLeftChannelToAifcFile);
	praat_addAction1 (classLongSound, 0, "Write left channel to Next/Sun file...", 0, 0, DO_LongSound_writeLeftChannelToNextSunFile);
	praat_addAction1 (classLongSound, 0, "Write left channel to NIST file...", 0, 0, DO_LongSound_writeLeftChannelToNistFile);
	praat_addAction1 (classLongSound, 0, "Write left channel to FLAC file...", 0, 0, DO_LongSound_writeLeftChannelToFlacFile);
	praat_addAction1 (classLongSound, 0, "Write right channel to WAV file...", 0, 0, DO_LongSound_writeRightChannelToWavFile);
	praat_addAction1 (classLongSound, 0, "Write right channel to AIFF file...", 0, 0, DO_LongSound_writeRightChannelToAiffFile);
	praat_addAction1 (classLongSound, 0, "Write right channel to AIFC file...", 0, 0, DO_LongSound_writeRightChannelToAifcFile);
	praat_addAction1 (classLongSound, 0, "Write right channel to Next/Sun file...", 0, 0, DO_LongSound_writeRightChannelToNextSunFile);
	praat_addAction1 (classLongSound, 0, "Write right channel to NIST file...", 0, 0, DO_LongSound_writeRightChannelToNistFile);
	praat_addAction1 (classLongSound, 0, "Write right channel to FLAC file...", 0, 0, DO_LongSound_writeRightChannelToFlacFile);
	praat_addAction1 (classLongSound, 0, "Write part to audio file...", 0, 0, DO_LongSound_writePartToAudioFile);

	praat_addAction1 (classSound, 0, "Write to WAV file...", 0, 0, DO_Sound_writeToWavFile);
	praat_addAction1 (classSound, 0, "Write to AIFF file...", 0, 0, DO_Sound_writeToAiffFile);
	praat_addAction1 (classSound, 0, "Write to AIFC file...", 0, 0, DO_Sound_writeToAifcFile);
	praat_addAction1 (classSound, 0, "Write to Next/Sun file...", 0, 0, DO_Sound_writeToNextSunFile);
	praat_addAction1 (classSound, 0, "Write to Sun audio file...", 0, praat_HIDDEN, DO_Sound_writeToSunAudioFile);
	praat_addAction1 (classSound, 0, "Write to NIST file...", 0, 0, DO_Sound_writeToNistFile);
	praat_addAction1 (classSound, 0, "Write to FLAC file...", 0, 0, DO_Sound_writeToFlacFile);
	#ifdef macintosh
	praat_addAction1 (classSound, 1, "Write to Mac sound file...", 0, praat_HIDDEN, DO_Sound_writeToMacSoundFile);
	#endif
	praat_addAction1 (classSound, 1, "Write to Kay sound file...", 0, 0, DO_Sound_writeToKayFile);
	praat_addAction1 (classSound, 1, "Write to Sesam file...", 0, praat_HIDDEN, DO_Sound_writeToSesamFile);
	#ifndef _WIN32
	praat_addAction1 (classSound, 1, "Write to raw sound file...", 0, 0, DO_Sound_writeToRawSoundFile);
	#endif
	praat_addAction1 (classSound, 1, "Write to raw 8-bit signed file...", 0, praat_HIDDEN, DO_Sound_writeToRaw8bitSignedFile);
	praat_addAction1 (classSound, 1, "Write to raw 8-bit unsigned file...", 0, praat_HIDDEN, DO_Sound_writeToRaw8bitUnsignedFile);
	praat_addAction1 (classSound, 2, "Write to stereo WAV file...", 0, praat_HIDDEN, DO_Sound_writeToStereoWavFile);   // grandfathered 2007
	praat_addAction1 (classSound, 2, "Write to stereo AIFF file...", 0, praat_HIDDEN, DO_Sound_writeToStereoAiffFile);   // grandfathered 2007
	praat_addAction1 (classSound, 2, "Write to stereo AIFC file...", 0, praat_HIDDEN, DO_Sound_writeToStereoAifcFile);   // grandfathered 2007
	praat_addAction1 (classSound, 2, "Write to stereo Next/Sun file...", 0, praat_HIDDEN, DO_Sound_writeToStereoNextSunFile);   // grandfathered 2007
	praat_addAction1 (classSound, 2, "Write to stereo NIST file...", 0, praat_HIDDEN, DO_Sound_writeToStereoNistFile);   // grandfathered 2007
	praat_addAction1 (classSound, 2, "Write to stereo FLAC file...", 0, praat_HIDDEN, DO_Sound_writeToStereoFlacFile);
	praat_addAction1 (classSound, 0, "Sound help", 0, 0, DO_Sound_help);
	praat_addAction1 (classSound, 1, "Edit", 0, 0, DO_Sound_edit);
	praat_addAction1 (classSound, 0, "Play", 0, 0, DO_Sound_play);
	praat_addAction1 (classSound, 0, "Draw...", 0, 0, DO_Sound_draw);
	praat_addAction1 (classSound, 1, "Query -          ", 0, 0, 0);
		praat_TimeFunction_query_init (classSound);
		praat_addAction1 (classSound, 1, "Get number of channels", 0, 1, DO_Sound_getNumberOfChannels);
		praat_addAction1 (classSound, 1, "Time sampling", 0, 1, 0);
		praat_addAction1 (classSound, 1, "Get number of samples", 0, 2, DO_Sound_getNumberOfSamples);
		praat_addAction1 (classSound, 1, "Get sampling period", 0, 2, DO_Sound_getSamplePeriod);
							praat_addAction1 (classSound, 1, "Get sample duration", 0, praat_HIDDEN + praat_DEPTH_2, DO_Sound_getSamplePeriod);
							praat_addAction1 (classSound, 1, "Get sample period", 0, praat_HIDDEN + praat_DEPTH_2, DO_Sound_getSamplePeriod);
		praat_addAction1 (classSound, 1, "Get sampling frequency", 0, 2, DO_Sound_getSampleRate);
							praat_addAction1 (classSound, 1, "Get sample rate", 0, praat_HIDDEN + praat_DEPTH_2, DO_Sound_getSampleRate);   /* grandfathered 2004 */
		praat_addAction1 (classSound, 1, "-- get time discretization --", 0, 2, 0);
		praat_addAction1 (classSound, 1, "Get time from sample number...", 0, 2, DO_Sound_getTimeFromIndex);
							praat_addAction1 (classSound, 1, "Get time from index...", 0, praat_HIDDEN + praat_DEPTH_2, DO_Sound_getTimeFromIndex);
		praat_addAction1 (classSound, 1, "Get sample number from time...", 0, 2, DO_Sound_getIndexFromTime);
							praat_addAction1 (classSound, 1, "Get index from time...", 0, praat_HIDDEN + praat_DEPTH_2, DO_Sound_getIndexFromTime);
		praat_addAction1 (classSound, 1, "-- get content --", 0, 1, 0);
		praat_addAction1 (classSound, 1, "Get value at time...", 0, 1, DO_Sound_getValueAtTime);
		praat_addAction1 (classSound, 1, "Get value at sample number...", 0, 1, DO_Sound_getValueAtIndex);
							praat_addAction1 (classSound, 1, "Get value at index...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_getValueAtIndex);
		praat_addAction1 (classSound, 1, "-- get shape --", 0, 1, 0);
		praat_addAction1 (classSound, 1, "Get minimum...", 0, 1, DO_Sound_getMinimum);
		praat_addAction1 (classSound, 1, "Get time of minimum...", 0, 1, DO_Sound_getTimeOfMinimum);
		praat_addAction1 (classSound, 1, "Get maximum...", 0, 1, DO_Sound_getMaximum);
		praat_addAction1 (classSound, 1, "Get time of maximum...", 0, 1, DO_Sound_getTimeOfMaximum);
		praat_addAction1 (classSound, 1, "Get absolute extremum...", 0, 1, DO_Sound_getAbsoluteExtremum);
		praat_addAction1 (classSound, 1, "Get nearest zero crossing...", 0, 1, DO_Sound_getNearestZeroCrossing);
		praat_addAction1 (classSound, 1, "-- get statistics --", 0, 1, 0);
		praat_addAction1 (classSound, 1, "Get mean...", 0, 1, DO_Sound_getMean);
		praat_addAction1 (classSound, 1, "Get root-mean-square...", 0, 1, DO_Sound_getRootMeanSquare);
		praat_addAction1 (classSound, 1, "Get standard deviation...", 0, 1, DO_Sound_getStandardDeviation);
		praat_addAction1 (classSound, 1, "-- get energy --", 0, 1, 0);
		praat_addAction1 (classSound, 1, "Get energy...", 0, 1, DO_Sound_getEnergy);
		praat_addAction1 (classSound, 1, "Get power...", 0, 1, DO_Sound_getPower);
		praat_addAction1 (classSound, 1, "-- get energy in air --", 0, 1, 0);
		praat_addAction1 (classSound, 1, "Get energy in air", 0, 1, DO_Sound_getEnergyInAir);
		praat_addAction1 (classSound, 1, "Get power in air", 0, 1, DO_Sound_getPowerInAir);
		praat_addAction1 (classSound, 1, "Get intensity (dB)", 0, 1, DO_Sound_getIntensity_dB);
	praat_addAction1 (classSound, 0, "Modify -          ", 0, 0, 0);
		praat_addAction1 (classSound, 0, "Reverse", 0, 1, DO_Sound_reverse);
		praat_addAction1 (classSound, 0, "Formula...", 0, 1, DO_Sound_formula);
		praat_addAction1 (classSound, 0, "-- add & mul --", 0, 1, 0);
		praat_addAction1 (classSound, 0, "Add...", 0, 1, DO_Sound_add);
		praat_addAction1 (classSound, 0, "Subtract mean", 0, 1, DO_Sound_subtractMean);
		praat_addAction1 (classSound, 0, "Multiply...", 0, 1, DO_Sound_multiply);
		praat_addAction1 (classSound, 0, "Multiply by window...", 0, 1, DO_Sound_multiplyByWindow);
		praat_addAction1 (classSound, 0, "Scale peak...", 0, 1, DO_Sound_scalePeak);
		praat_addAction1 (classSound, 0, "Scale...", 0, praat_HIDDEN + praat_DEPTH_1, DO_Sound_scalePeak);
		praat_addAction1 (classSound, 0, "Scale intensity...", 0, 1, DO_Sound_scaleIntensity);
		praat_addAction1 (classSound, 0, "-- set --", 0, 1, 0);
		praat_addAction1 (classSound, 0, "Set value at sample number...", 0, 1, DO_Sound_setValueAtIndex);
							praat_addAction1 (classSound, 0, "Set value at index...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_setValueAtIndex);
		praat_addAction1 (classSound, 0, "Set part to zero...", 0, 1, DO_Sound_setPartToZero);
		praat_addAction1 (classSound, 0, "-- modify hack --", 0, 1, 0);
		praat_addAction1 (classSound, 0, "Override sampling frequency...", 0, 1, DO_Sound_overrideSamplingFrequency);
							praat_addAction1 (classSound, 0, "Override sample rate...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_overrideSamplingFrequency);
		praat_addAction1 (classSound, 0, "-- in-line filters --", 0, 1, 0);
		praat_addAction1 (classSound, 0, "In-line filters", 0, 1, 0);
		praat_addAction1 (classSound, 0, "Filter with one formant (in-line)...", 0, 2, DO_Sound_filterWithOneFormantInline);
		praat_addAction1 (classSound, 0, "Pre-emphasize (in-line)...", 0, 2, DO_Sound_preemphasizeInline);
		praat_addAction1 (classSound, 0, "De-emphasize (in-line)...", 0, 2, DO_Sound_deemphasizeInline);
	praat_addAction1 (classSound, 0, "Annotate -   ", 0, 0, 0);
		praat_addAction1 (classSound, 0, "Annotation tutorial", 0, 1, DO_AnnotationTutorial);
		praat_addAction1 (classSound, 0, "-- to text grid --", 0, 1, 0);
		praat_addAction1 (classSound, 0, "To TextGrid...", 0, 1, DO_Sound_to_TextGrid);
		praat_addAction1 (classSound, 0, "-- to single tier --", 0, 1, 0);
		praat_addAction1 (classSound, 0, "To TextTier", 0, 1, DO_Sound_to_TextTier);
		praat_addAction1 (classSound, 0, "To IntervalTier", 0, 1, DO_Sound_to_IntervalTier);
	praat_addAction1 (classSound, 0, "Analyse", 0, 0, 0);
	praat_addAction1 (classSound, 0, "Periodicity -        ", 0, 0, 0);
		praat_addAction1 (classSound, 0, "To Pitch...", 0, 1, DO_Sound_to_Pitch);
		praat_addAction1 (classSound, 0, "To Pitch (ac)...", 0, 1, DO_Sound_to_Pitch_ac);
		praat_addAction1 (classSound, 0, "To Pitch (cc)...", 0, 1, DO_Sound_to_Pitch_cc);
		praat_addAction1 (classSound, 0, "To PointProcess (periodic, cc)...", 0, 1, DO_Sound_to_PointProcess_periodic_cc);
		praat_addAction1 (classSound, 0, "To PointProcess (periodic, peaks)...", 0, 1, DO_Sound_to_PointProcess_periodic_peaks);
		praat_addAction1 (classSound, 0, "-- hnr --", 0, 1, 0);
		praat_addAction1 (classSound, 0, "To Harmonicity (cc)...", 0, 1, DO_Sound_to_Harmonicity_cc);
		praat_addAction1 (classSound, 0, "To Harmonicity (ac)...", 0, 1, DO_Sound_to_Harmonicity_ac);
		praat_addAction1 (classSound, 0, "To Harmonicity (gne)...", 0, 1, DO_Sound_to_Harmonicity_gne);
	praat_addAction1 (classSound, 0, "Spectrum -", 0, 0, 0);
		praat_addAction1 (classSound, 0, "To Spectrum...", 0, 1, DO_Sound_to_Spectrum);
							praat_addAction1 (classSound, 0, "To Spectrum (fft)", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_to_Spectrum_fft);
							praat_addAction1 (classSound, 0, "To Spectrum", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_to_Spectrum_fft);
							praat_addAction1 (classSound, 0, "To Spectrum (dft)", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_to_Spectrum_dft);
		praat_addAction1 (classSound, 0, "To Ltas...", 0, 1, DO_Sound_to_Ltas);
		praat_addAction1 (classSound, 0, "To Ltas (pitch-corrected)...", 0, 1, DO_Sound_to_Ltas_pitchCorrected);
		praat_addAction1 (classSound, 0, "-- spectrotemporal --", 0, 1, 0);
		praat_addAction1 (classSound, 0, "To Spectrogram...", 0, 1, DO_Sound_to_Spectrogram);
		praat_addAction1 (classSound, 0, "To Cochleagram...", 0, 1, DO_Sound_to_Cochleagram);
		praat_addAction1 (classSound, 0, "To Cochleagram (edb)...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_to_Cochleagram_edb);
	praat_addAction1 (classSound, 0, "Formants & LPC -", 0, 0, 0);
		praat_addAction1 (classSound, 0, "To Formant (burg)...", 0, 1, DO_Sound_to_Formant_burg);
		praat_addAction1 (classSound, 0, "To Formant (hack)", 0, 1, 0);
		praat_addAction1 (classSound, 0, "To Formant (keep all)...", 0, 2, DO_Sound_to_Formant_keepAll);
		praat_addAction1 (classSound, 0, "To Formant (sl)...", 0, 2, DO_Sound_to_Formant_willems);
	praat_addAction1 (classSound, 0, "Points -          ", 0, 0, 0);
		praat_addAction1 (classSound, 0, "To PointProcess (extrema)...", 0, 1, DO_Sound_to_PointProcess_extrema);
		praat_addAction1 (classSound, 0, "To PointProcess (zeroes)...", 0, 1, DO_Sound_to_PointProcess_zeroes);
	praat_addAction1 (classSound, 0, "To Intensity...", 0, 0, DO_Sound_to_Intensity);
	praat_addAction1 (classSound, 0, "Manipulate", 0, 0, 0);
	praat_addAction1 (classSound, 0, "To Manipulation...", 0, 0, DO_Sound_to_Manipulation);
	praat_addAction1 (classSound, 0, "Synthesize", 0, 0, 0);
	praat_addAction1 (classSound, 0, "Convert -       ", 0, 0, 0);
		praat_addAction1 (classSound, 0, "Convert to mono", 0, 1, DO_Sound_convertToMono);
		praat_addAction1 (classSound, 0, "Convert to stereo", 0, 1, DO_Sound_convertToStereo);
		praat_addAction1 (classSound, 0, "Extract left channel", 0, 1, DO_Sound_extractLeftChannel);
		praat_addAction1 (classSound, 0, "Extract right channel", 0, 1, DO_Sound_extractRightChannel);
		praat_addAction1 (classSound, 0, "Extract part...", 0, 1, DO_Sound_extractPart);
		praat_addAction1 (classSound, 0, "Resample...", 0, 1, DO_Sound_resample);
		praat_addAction1 (classSound, 0, "-- enhance --", 0, 1, 0);
		praat_addAction1 (classSound, 0, "Lengthen (overlap-add)...", 0, 1, DO_Sound_lengthen_overlapAdd);
		praat_addAction1 (classSound, 0, "Lengthen (PSOLA)...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_Sound_lengthen_overlapAdd);
		praat_addAction1 (classSound, 0, "Deepen band modulation...", 0, 1, DO_Sound_deepenBandModulation);
		praat_addAction1 (classSound, 0, "-- cast --", 0, 1, 0);
		praat_addAction1 (classSound, 0, "Down to Matrix", 0, 1, DO_Sound_to_Matrix);
	praat_addAction1 (classSound, 0, "Filter -       ", 0, 0, 0);
		praat_addAction1 (classSound, 0, "Filtering tutorial", 0, 1, DO_FilteringTutorial);
		praat_addAction1 (classSound, 0, "-- frequency-domain filter --", 0, 1, 0);
		praat_addAction1 (classSound, 0, "Filter (pass Hann band)...", 0, 1, DO_Sound_filter_passHannBand);
		praat_addAction1 (classSound, 0, "Filter (stop Hann band)...", 0, 1, DO_Sound_filter_stopHannBand);
		praat_addAction1 (classSound, 0, "Filter (formula)...", 0, 1, DO_Sound_filter_formula);
		praat_addAction1 (classSound, 0, "-- time-domain filter --", 0, 1, 0);
		praat_addAction1 (classSound, 0, "Filter (one formant)...", 0, 1, DO_Sound_filter_oneFormant);
		praat_addAction1 (classSound, 0, "Filter (pre-emphasis)...", 0, 1, DO_Sound_filter_preemphasis);
		praat_addAction1 (classSound, 0, "Filter (de-emphasis)...", 0, 1, DO_Sound_filter_deemphasis);
	praat_addAction1 (classSound, 0, "Combine sounds -", 0, 0, 0);
		praat_addAction1 (classSound, 2, "Combine to stereo", 0, 1, DO_Sounds_combineToStereo);
		praat_addAction1 (classSound, 2, "Convolve", 0, 1, DO_Sounds_convolve);
		praat_addAction1 (classSound, 0, "Concatenate", 0, 1, DO_Sounds_concatenate);
		praat_addAction1 (classSound, 0, "Concatenate recoverably", 0, 1, DO_Sounds_concatenateRecoverably);
		praat_addAction1 (classSound, 2, "To ParamCurve", 0, 1, DO_Sounds_to_ParamCurve);
		praat_addAction1 (classSound, 2, "Cross-correlate...", 0, 1, DO_Sounds_crossCorrelate);

	praat_addAction2 (classLongSound, 0, classSound, 0, "Write to WAV file...", 0, 0, DO_LongSound_Sound_writeToWavFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, "Write to AIFF file...", 0, 0, DO_LongSound_Sound_writeToAiffFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, "Write to AIFC file...", 0, 0, DO_LongSound_Sound_writeToAifcFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, "Write to NeXT/Sun file...", 0, 0, DO_LongSound_Sound_writeToNextSunFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, "Write to NIST file...", 0, 0, DO_LongSound_Sound_writeToNistFile);
	praat_addAction2 (classLongSound, 0, classSound, 0, "Write to FLAC file...", 0, 0, DO_LongSound_Sound_writeToFlacFile);
}

/* End of file praat_Sound.c */
