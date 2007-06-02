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
 * pb 2007/06/02
 */

#include "praat.h"

#include "FunctionEditor_Sound.h"
#include "FunctionEditor_SoundAnalysis.h"
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

FORM (LongSound_extractPart, "LongSound: Extract part", 0)
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "1.0")
	BOOLEAN ("Preserve times", 1)
	OK
DO
	EVERY_TO (LongSound_extractPart (OBJECT, GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Preserve times")))
END

FORM (LongSound_getIndexFromTime, "LongSound: Get sample index from time", "Sound: Get index from time...")
	REAL ("Time (s)", "0.5")
	OK
DO
	Melder_informationReal (Sampled_xToIndex (ONLY (classLongSound), GET_REAL ("Time")), NULL);
END

DIRECT (LongSound_getSamplePeriod)
	LongSound me = ONLY (classLongSound);
	Melder_informationReal (my dx, "seconds");
END

DIRECT (LongSound_getSampleRate)
	LongSound me = ONLY (classLongSound);
	Melder_informationReal (1 / my dx, "Hertz");
END

FORM (LongSound_getTimeFromIndex, "LongSound: Get time from sample index", "Sound: Get time from index...")
	INTEGER ("Sample index", "100")
	OK
DO
	Melder_informationReal (Sampled_indexToX (ONLY (classLongSound), GET_INTEGER ("Sample index")), "seconds");
END

DIRECT (LongSound_getNumberOfSamples)
	LongSound me = ONLY (classLongSound);
	Melder_information2 (Melder_integer (my nx), " samples");
END

DIRECT (LongSound_help) Melder_help ("LongSound"); END

FORM_READ (LongSound_open, "Open long sound file", 0)
	if (! praat_new (LongSound_open (file), MelderFile_name (file))) return 0;
END

FORM (LongSound_playPart, "LongSound: Play part", 0)
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "10.0")
	OK
DO
	int n = 0;
	EVERY (n ++)
	if (n == 1 || Melder_getMaximumAsynchronicity () < Melder_ASYNCHRONOUS) {
		EVERY (LongSound_playPart (OBJECT, GET_REAL ("left Time range"), GET_REAL ("right Time range"), NULL, NULL))
	} else {
		Melder_setMaximumAsynchronicity (Melder_INTERRUPTABLE);
		EVERY (LongSound_playPart (OBJECT, GET_REAL ("left Time range"), GET_REAL ("right Time range"), NULL, NULL))
		Melder_setMaximumAsynchronicity (Melder_ASYNCHRONOUS);
	}
END

FORM (LongSound_writePartToAudioFile, "LongSound: Write part to audio file", 0)
	LABEL ("", "Audio file:")
	TEXTFIELD ("Audio file", "")
	RADIO ("Type", 3)
	{ int i; for (i = 1; i <= Melder_NUMBER_OF_AUDIO_FILE_TYPES; i ++) {
		RADIOBUTTON (Melder_audioFileTypeString (i))
	}}
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "10.0")
	OK
DO
	structMelderFile file = { { 0 } };
	if (! Melder_relativePathToFile (GET_STRING ("Audio file"), & file)) return 0;
	if (! LongSound_writePartToAudioFile16 (ONLY (classLongSound), GET_INTEGER ("Type"),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), & file)) return 0;
END
	
FORM (LongSound_to_TextGrid, "LongSound: To TextGrid...", "LongSound: To TextGrid...")
	SENTENCE ("Tier names", "Mary John bell")
	SENTENCE ("Point tiers", "bell")
	OK
DO
	EVERY_TO (TextGrid_create (((LongSound) OBJECT) -> xmin, ((Pitch) OBJECT) -> xmax,
		GET_STRING ("Tier names"), GET_STRING ("Point tiers")))
END

DIRECT (LongSound_view)
	if (theCurrentPraat -> batch)
		return Melder_error ("Cannot view a LongSound from batch.");
	else
		WHERE (SELECTED)
			if (! praat_installEditor (SoundEditor_create (theCurrentPraat -> topShell, FULL_NAME, OBJECT), IOBJECT))
				return 0;
END

FORM_WRITE (LongSound_writeToAifcFile, "Write to AIFC file", 0, "aifc")
	if (! pr_LongSound_concatenate (file, Melder_AIFC)) return 0;
END

FORM_WRITE (LongSound_writeToAiffFile, "Write to AIFF file", 0, "aiff")
	if (! pr_LongSound_concatenate (file, Melder_AIFF)) return 0;
END

FORM_WRITE (LongSound_writeToNextSunFile, "Write to NeXT/Sun file", 0, "au")
	if (! pr_LongSound_concatenate (file, Melder_NEXT_SUN)) return 0;
END

FORM_WRITE (LongSound_writeToNistFile, "Write to NIST file", 0, "nist")
	if (! pr_LongSound_concatenate (file, Melder_NIST)) return 0;
END

FORM_WRITE (LongSound_writeToFlacFile, "Write to FLAC file", 0, "flac")
	if (! pr_LongSound_concatenate (file, Melder_FLAC)) return 0;
END

FORM_WRITE (LongSound_writeToWavFile, "Write to WAV file", 0, "wav")
	if (! pr_LongSound_concatenate (file, Melder_WAV)) return 0;
END

FORM_WRITE (LongSound_writeLeftChannelToAifcFile, "Write left channel to AIFC file", 0, "aifc")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_AIFC, 0, file)) return 0;
END

FORM_WRITE (LongSound_writeLeftChannelToAiffFile, "Write left channel to AIFF file", 0, "aiff")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_AIFF, 0, file)) return 0;
END

FORM_WRITE (LongSound_writeLeftChannelToNextSunFile, "Write left channel to NeXT/Sun file", 0, "au")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_NEXT_SUN, 0, file)) return 0;
END

FORM_WRITE (LongSound_writeLeftChannelToNistFile, "Write left channel to NIST file", 0, "nist")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_NIST, 0, file)) return 0;
END

FORM_WRITE (LongSound_writeLeftChannelToFlacFile, "Write left channel to FLAC file", 0, "flac")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_FLAC, 0, file)) return 0;
END

FORM_WRITE (LongSound_writeLeftChannelToWavFile, "Write left channel to WAV file", 0, "wav")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_WAV, 0, file)) return 0;
END

FORM_WRITE (LongSound_writeRightChannelToAifcFile, "Write right channel to AIFC file", 0, "aifc")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_AIFC, 1, file)) return 0;
END

FORM_WRITE (LongSound_writeRightChannelToAiffFile, "Write right channel to AIFF file", 0, "aiff")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_AIFF, 1, file)) return 0;
END

FORM_WRITE (LongSound_writeRightChannelToNextSunFile, "Write right channel to NeXT/Sun file", 0, "au")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_NEXT_SUN, 1, file)) return 0;
END

FORM_WRITE (LongSound_writeRightChannelToNistFile, "Write right channel to NIST file", 0, "nist")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_NIST, 1, file)) return 0;
END

FORM_WRITE (LongSound_writeRightChannelToFlacFile, "Write right channel to FLAC file", 0, "flac")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_FLAC, 1, file)) return 0;
END

FORM_WRITE (LongSound_writeRightChannelToWavFile, "Write right channel to WAV file", 0, "wav")
	if (! LongSound_writeChannelToAudioFile16 (ONLY_OBJECT, Melder_WAV, 1, file)) return 0;
END

FORM (LongSoundPrefs, "LongSound preferences", "LongSound")
	LABEL ("", "This setting determines the maximum number of seconds")
	LABEL ("", "for viewing the waveform and playing a sound in the LongSound window.")
	LABEL ("", "The LongSound window can become very slow if you set it too high.")
	NATURAL ("Maximum viewable part (seconds)", "60")
	LABEL ("", "Note: this setting works for the next long sound file that you open,")
	LABEL ("", "not for currently existing LongSound objects.")
	OK
SET_INTEGER ("Maximum viewable part", LongSound_getBufferSizePref_seconds ())
DO
	LongSound_setBufferSizePref_seconds (GET_INTEGER ("Maximum viewable part"));
END

/********** LONGSOUND & SOUND **********/

FORM_WRITE (LongSound_Sound_writeToAifcFile, "Write to AIFC file", 0, "aifc")
	if (! pr_LongSound_concatenate (file, Melder_AIFC)) return 0;
END

FORM_WRITE (LongSound_Sound_writeToAiffFile, "Write to AIFF file", 0, "aiff")
	if (! pr_LongSound_concatenate (file, Melder_AIFF)) return 0;
END

FORM_WRITE (LongSound_Sound_writeToNextSunFile, "Write to NeXT/Sun file", 0, "au")
	if (! pr_LongSound_concatenate (file, Melder_NEXT_SUN)) return 0;
END

FORM_WRITE (LongSound_Sound_writeToNistFile, "Write to NIST file", 0, "nist")
	if (! pr_LongSound_concatenate (file, Melder_NIST)) return 0;
END

FORM_WRITE (LongSound_Sound_writeToFlacFile, "Write to FLAC file", 0, "flac")
	if (! pr_LongSound_concatenate (file, Melder_FLAC)) return 0;
END

FORM_WRITE (LongSound_Sound_writeToWavFile, "Write to WAV file", 0, "wav")
	if (! pr_LongSound_concatenate (file, Melder_WAV)) return 0;
END

/********** SOUND **********/

FORM (Sound_add, "Sound: Add", 0)
	LABEL ("", "The following number will be added to the amplitudes of all samples of the sound.")
	REAL ("Number", "0.1")
	OK
DO
	WHERE (SELECTED) {
		Vector_addScalar (OBJECT, GET_REAL ("Number"));
		praat_dataChanged (OBJECT);
	}
END

DIRECT (Sounds_combineToStereo)
	Sound sound1 = NULL, sound2 = NULL;
	int i1 = 0, i2 = 0;
	char name [200];
	WHERE (SELECTED) { if (sound1) { sound2 = OBJECT; i2 = IOBJECT; } else { sound1 = OBJECT; i1 = IOBJECT; } }
	Melder_assert (sound1 && sound2 && i1 && i2);
	sprintf (name, "%s_%s", strchr (theCurrentPraat -> list [i1]. name, ' ') + 1, strchr (theCurrentPraat -> list [i2]. name, ' ') + 1);
	if (! praat_new (Sounds_combineToStereo (sound1, sound2), name)) return 0;
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
			return Melder_error ("To concatenate sounds, their numbers of channels (mono, stereo) must be equal.");
		}
		if (dx == 0.0) {
			dx = my dx;
		} else if (my dx != dx) {
			(void) Melder_error ("To concatenate sounds, their sampling frequencies must be equal.\n");
			return Melder_error ("You could resample one or more of the sounds before concatenating.");
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
	if (! praat_new (thee, "chain")) return 0;
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
			return Melder_error ("To concatenate sounds, their numbers of channels (mono, stereo) must be equal.");
		}
		if (dx == 0.0) {
			dx = my dx;
		} else if (my dx != dx) {
			(void) Melder_error ("To concatenate sounds, their sampling frequencies must be equal.\n");
			return Melder_error ("You could resample one or more of the sounds before concatenating.");
		}
		nx += my nx;
	}
	thee = Sound_create (numberOfChannels, 0.0, nx * dx, nx, dx, 0.5 * dx); cherror
	him = TextGrid_create (0.0, nx * dx, "labels", ""); cherror
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
	praat_new (thee, "chain"); cherror
	praat_new (him, "chain"); cherror
end:
	iferror { forget (thee); forget (him); return 0; }
END

DIRECT (Sound_convertToMono)
	WHERE (SELECTED) {
		Sound me = OBJECT;
		if (! praat_new (Sound_convertToMono (me), "%s_mono", my name)) return 0;
	}
END

DIRECT (Sound_convertToStereo)
	WHERE (SELECTED) {
		Sound me = OBJECT;
		if (! praat_new (Sound_convertToStereo (me), "%s_stereo", my name)) return 0;
	}
END

DIRECT (Sounds_convolve)
	Sound sound1 = NULL, sound2 = NULL;
	int i1 = 0, i2 = 0;
	char name [200];
	WHERE (SELECTED) { if (sound1) { sound2 = OBJECT; i2 = IOBJECT; } else { sound1 = OBJECT; i1 = IOBJECT; } }
	Melder_assert (sound1 && sound2 && i1 && i2);
	sprintf (name, "%s_%s", strchr (theCurrentPraat -> list [i1]. name, ' ') + 1, strchr (theCurrentPraat -> list [i2]. name, ' ') + 1);
	if (! praat_new (Sounds_convolve (sound1, sound2), name)) return 0;
END

static int common_Sound_create (void *dia, bool allowStereo) {
	Sound sound = NULL;
	long channels = allowStereo ? GET_INTEGER ("Channels") : 1;
	double startTime = GET_REAL ("Start time");
	double endTime = GET_REAL ("End time");
	double samplingFrequency = GET_REAL ("Sampling frequency");
	double numberOfSamples_real = floor ((endTime - startTime) * samplingFrequency + 0.5);
	long numberOfSamples;
	if (endTime <= startTime) {
		if (endTime == startTime)
			Melder_error ("A Sound cannot have a duration of zero.");
		else
			Melder_error ("A Sound cannot have a duration less than zero.");
		if (startTime == 0.0)
			return Melder_error ("Please set the end time to something greater than 0 seconds.");
		else
			return Melder_error ("Please lower the start time or raise the end time.");
	}
	if (samplingFrequency <= 0.0) {
		Melder_error ("A Sound cannot have a negative sampling frequency.");
		return Melder_error ("Please set the sampling frequency to something greater than zero, e.g. 44100 Hz.");
	}
	if (numberOfSamples_real < 1.0) {
		Melder_error ("A Sound cannot have zero samples.");
		if (startTime == 0.0)
			return Melder_error ("Please raise the end time.");
		else
			return Melder_error ("Please lower the start time or raise the end time.");
	}
	if (numberOfSamples_real > LONG_MAX) {
		Melder_error ("A Sound cannot have %s samples; the maximum is %s samples "
			"(or less, depending on your computer's memory).", Melder_bigInteger (numberOfSamples_real), Melder_bigInteger (LONG_MAX));
		if (startTime == 0.0)
			return Melder_error ("Please lower the end time or the sampling frequency.");
		else
			return Melder_error ("Please raise the start time, lower the end time, or lower the sampling frequency.");
	}
	numberOfSamples = (long) numberOfSamples_real;
	sound = Sound_create (channels, startTime, endTime, numberOfSamples, 1.0 / samplingFrequency,
		startTime + 0.5 * (endTime - startTime - (numberOfSamples - 1) / samplingFrequency));
	if (sound == NULL) {
		if (strstr (Melder_getError (), "memory")) {
			Melder_clearError ();
			Melder_error ("There is not enough memory to create a Sound that contains %s samples.", Melder_bigInteger (numberOfSamples_real));
			if (startTime == 0.0)
				return Melder_error ("You could lower the end time or the sampling frequency and try again.");
			else
				return Melder_error ("You could raise the start time or lower the end time or the sampling frequency, and try again.");
		} else {
			return 0;   /* Unexpected error. Wait for generic message. */
		}
	}
	Matrix_formula ((Matrix) sound, GET_STRING ("formula"), NULL);
	iferror {
		forget (sound);
		return Melder_error ("Please correct the formula.");
	}
	if (! praat_new (sound, GET_STRING ("Name"))) return 0;
	//praat_updateSelection ();
	return 1;
}

FORM (Sound_create, "Create mono Sound", "Create Sound from formula...")
	WORD ("Name", "sineWithNoise")
	REAL ("Start time (s)", "0.0")
	REAL ("End time (s)", "1.0")
	REAL ("Sampling frequency (Hz)", "44100")
	LABEL ("", "Formula:")
	TEXTFIELD ("formula", "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)")
	OK
DO
	if (! common_Sound_create (dia, false)) return 0;
END

FORM (Sound_createFromFormula, "Create Sound from formula", "Create Sound from formula...")
	WORD ("Name", "sineWithNoise")
	OPTIONMENU ("Channels", 1)
		OPTION ("Mono")
		OPTION ("Stereo")
	REAL ("Start time (s)", "0.0")
	REAL ("End time (s)", "1.0")
	REAL ("Sampling frequency (Hz)", "44100")
	LABEL ("", "Formula:")
	TEXTFIELD ("formula", "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)")
	OK
DO
	if (! common_Sound_create (dia, true)) return 0;
END

FORM (Sound_createFromToneComplex, "Create Sound from tone complex", "Create Sound from tone complex...")
	WORD ("Name", "toneComplex")
	REAL ("Start time (s)", "0.0")
	REAL ("End time (s)", "1.0")
	POSITIVE ("Sampling frequency (Hz)", "44100")
	RADIO ("Phase", 2)
		RADIOBUTTON ("Sine")
		RADIOBUTTON ("Cosine")
	POSITIVE ("Frequency step (Hz)", "100")
	REAL ("First frequency (Hz)", "0 (= frequency step)")
	REAL ("Ceiling (Hz)", "0 (= Nyquist)")
	INTEGER ("Number of components", "0 (= maximum)")
	OK
DO
	if (! praat_new (Sound_createFromToneComplex (GET_REAL ("Start time"), GET_REAL ("End time"),
		GET_REAL ("Sampling frequency"), GET_INTEGER ("Phase") - 1, GET_REAL ("Frequency step"),
		GET_REAL ("First frequency"), GET_REAL ("Ceiling"), GET_INTEGER ("Number of components")),
		GET_STRING ("Name"))) return 0;
END

FORM (Sounds_crossCorrelate, "Cross-correlate", 0)
	REAL ("From lag (s)", "-0.1")
	REAL ("To lag (s)", "0.1")
	BOOLEAN ("Normalize", 1)
	OK
DO
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	if (! praat_new (Sounds_crossCorrelate (s1, s2, GET_REAL ("From lag"), GET_REAL ("To lag"),
		GET_INTEGER ("Normalize")), "cc_%s_%s", s1 -> name, s2 -> name)) return 0;
END

FORM (Sound_deemphasizeInline, "Sound: De-emphasize (in-line)", "Sound: De-emphasize (in-line)...")
	REAL ("From frequency (Hz)", "50.0")
	OK
DO
	WHERE (SELECTED) {
		Sound_deEmphasis (OBJECT, GET_REAL ("From frequency"));
		Vector_scale (OBJECT, 0.99);
		praat_dataChanged (OBJECT);
	}
END

FORM (Sound_deepenBandModulation, "Deepen band modulation", "Sound: Deepen band modulation...")
	POSITIVE ("Enhancement (dB)", "20")
	POSITIVE ("From frequency (Hz)", "300")
	POSITIVE ("To frequency (Hz)", "8000")
	POSITIVE ("Slow modulation (Hz)", "3")
	POSITIVE ("Fast modulation (Hz)", "30")
	POSITIVE ("Band smoothing (Hz)", "100")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new (Sound_deepenBandModulation (OBJECT, GET_REAL ("Enhancement"),
			GET_REAL ("From frequency"), GET_REAL ("To frequency"),
			GET_REAL ("Slow modulation"), GET_REAL ("Fast modulation"), GET_REAL ("Band smoothing")),
			"%s_%.0f", NAME, GET_REAL ("Enhancement"))) return 0;
END

FORM (old_Sound_draw, "Sound: Draw", 0)
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range", "0.0 (= all)")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0 (= auto)")
	BOOLEAN ("Garnish", 1)
	OK
DO
	EVERY_DRAW (Sound_draw (OBJECT, GRAPHICS, GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"), GET_INTEGER ("Garnish"), "curve"))
END

FORM (Sound_draw, "Sound: Draw", 0)
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range", "0.0 (= all)")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0 (= auto)")
	BOOLEAN ("Garnish", 1)
	LABEL ("", "")
	OPTIONMENU ("Drawing method", 1)
		OPTION ("Curve")
		OPTION ("Bars")
		OPTION ("Poles")
		OPTION ("Speckles")
	OK
DO_ALTERNATIVE (old_Sound_draw)
	EVERY_DRAW (Sound_draw (OBJECT, GRAPHICS, GET_REAL ("left Time range"), GET_REAL ("right Time range"),
		GET_REAL ("left Vertical range"), GET_REAL ("right Vertical range"), GET_INTEGER ("Garnish"), GET_STRING ("Drawing method")))
END

static void cb_SoundEditor_publish (Any editor, void *closure, Any publish) {
	(void) editor;
	(void) closure;
	if (! praat_new (publish, NULL)) { Melder_flushError (NULL); return; }
	praat_updateSelection ();
	if (Thing_member (publish, classSpectrum)) {
		int IOBJECT;
		WHERE (SELECTED) {
			SpectrumEditor editor2 = SpectrumEditor_create (theCurrentPraat -> topShell, FULL_NAME, OBJECT);
			if (! editor2) return;
			if (! praat_installEditor (editor2, IOBJECT)) Melder_flushError (NULL);
		}
	}
}
DIRECT (Sound_edit)
	if (theCurrentPraat -> batch) {
		return Melder_error ("Cannot edit a Sound from batch.");
	} else {
		WHERE (SELECTED) {
			SoundEditor editor = SoundEditor_create (theCurrentPraat -> topShell, FULL_NAME, OBJECT);
			if (! editor) return 0;
			if (! praat_installEditor (editor, IOBJECT)) return 0;
			Editor_setPublishCallback (editor, cb_SoundEditor_publish, NULL);
		}
	}
END

DIRECT (Sound_extractLeftChannel)
	WHERE (SELECTED) {
		Sound me = OBJECT;
		if (! praat_new (Sound_extractLeftChannel (me), "%s_left", my name)) return 0;
	}
END

FORM (Sound_extractPart, "Sound: Extract part", 0)
	REAL ("left Time range (s)", "0")
	REAL ("right Time range (s)", "0.1")
	ENUM ("Window", Sound_WINDOW, enumi (Sound_WINDOW, Hanning))
	POSITIVE ("Relative width", "1.0")
	BOOLEAN ("Preserve times", 0)
	OK
DO
	WHERE (SELECTED) {
		Sound me = OBJECT;
		if (! praat_new (Sound_extractPart (me,
			GET_REAL ("left Time range"), GET_REAL ("right Time range"),
			GET_INTEGER ("Window"), GET_REAL ("Relative width"),
			GET_INTEGER ("Preserve times")),
			"%s_part", my name)) return 0;
	}
END

DIRECT (Sound_extractRightChannel)
	WHERE (SELECTED) {
		Sound me = OBJECT;
		if (! praat_new (Sound_extractRightChannel (me), "%s_right", my name)) return 0;
	}
END

FORM (Sound_filter_deemphasis, "Sound: Filter (de-emphasis)", "Sound: Filter (de-emphasis)...")
	REAL ("From frequency (Hz)", "50.0")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (Sound_filter_deemphasis (OBJECT, GET_REAL ("From frequency")),
			"%s_deemp", NAME)) return 0;
	}
END

FORM (Sound_filter_formula, "Sound: Filter (formula)...", "Formula...")
	LABEL ("", "Frequency-domain filtering with a formula (uses Sound-to-Spectrum and Spectrum-to-Sound): x is frequency in Hertz")
	TEXTFIELD ("formula", "if x<500 or x>1000 then 0 else self fi; rectangular band filter")
	OK
DO
	WHERE (SELECTED)
		if (! praat_new (Sound_filter_formula (OBJECT, GET_STRING ("formula")),
			"%s_filt", NAME)) return 0;
END

FORM (Sound_filter_oneFormant, "Sound: Filter (one formant)", "Sound: Filter (one formant)...")
	REAL ("Frequency (Hz)", "1000")
	POSITIVE ("Bandwidth (Hz)", "100")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (Sound_filter_oneFormant (OBJECT, GET_REAL ("Frequency"), GET_REAL ("Bandwidth")),
			"%s_filt", NAME)) return 0;
	}
END

FORM (Sound_filterWithOneFormantInline, "Sound: Filter with one formant (in-line)", "Sound: Filter with one formant (in-line)...")
	REAL ("Frequency (Hz)", "1000")
	POSITIVE ("Bandwidth (Hz)", "100")
	OK
DO
	WHERE (SELECTED) {
		Sound_filterWithOneFormantInline (OBJECT, GET_REAL ("Frequency"), GET_REAL ("Bandwidth"));
		praat_dataChanged (OBJECT);
	}
END

FORM (Sound_filter_passHannBand, "Sound: Filter (pass Hann band)", "Sound: Filter (pass Hann band)...")
	REAL ("From frequency (Hz)", "500")
	REAL ("To frequency (s)", "1000")
	POSITIVE ("Smoothing (Hz)", "100")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (Sound_filter_passHannBand (OBJECT,
			GET_REAL ("From frequency"), GET_REAL ("To frequency"), GET_REAL ("Smoothing")),
			"%s_band", NAME)) return 0;
	}
END

FORM (Sound_filter_preemphasis, "Sound: Filter (pre-emphasis)", "Sound: Filter (pre-emphasis)...")
	REAL ("From frequency (Hz)", "50.0")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (Sound_filter_preemphasis (OBJECT, GET_REAL ("From frequency")),
			"%s_preemp", NAME)) return 0;
	}
END

FORM (Sound_filter_stopHannBand, "Sound: Filter (stop Hann band)", "Sound: Filter (stop Hann band)...")
	REAL ("From frequency (Hz)", "500")
	REAL ("To frequency (s)", "1000")
	POSITIVE ("Smoothing (Hz)", "100")
	OK
DO
	WHERE (SELECTED) {
		if (! praat_new (Sound_filter_stopHannBand (OBJECT,
			GET_REAL ("From frequency"), GET_REAL ("To frequency"), GET_REAL ("Smoothing")),
			"%s_band", NAME)) return 0;
	}
END

FORM (Sound_formula, "Sound: Formula", "Sound: Formula...")
	LABEL ("label1", "! `x' is the time in seconds, `col' is the sample number.")
	LABEL ("label2", "x = x1   ! time associated with first sample")
	LABEL ("label3", "for col from 1 to ncol")
	LABEL ("label4", "   self [col] = ...")
	TEXTFIELD ("formula", "self")
	LABEL ("label5", "   x = x + dx")
	LABEL ("label6", "endfor")
	OK
DO
	if (! praat_Fon_formula (dia)) return 0;
END

FORM (Sound_getAbsoluteExtremum, "Sound: Get absolute extremum", "Sound: Get absolute extremum...")
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0 (= all)")
	RADIO ("Interpolation", 4)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	RADIOBUTTON ("Cubic")
	RADIOBUTTON ("Sinc70")
	RADIOBUTTON ("Sinc700")
	OK
DO
	Melder_informationReal (Vector_getAbsoluteExtremum (ONLY (classSound),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Interpolation") - 1), "Pascal");
END

FORM (Sound_getEnergy, "Sound: Get energy", "Sound: Get energy...")
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0 (= all)")
	OK
DO
	Melder_informationReal (Sound_getEnergy (ONLY (classSound), GET_REAL ("left Time range"), GET_REAL ("right Time range")), "Pa2 sec");
END

DIRECT (Sound_getEnergyInAir)
	Melder_informationReal (Sound_getEnergyInAir (ONLY (classSound)), "Joule/m2");
END

FORM (Sound_getIndexFromTime, "Get sample number from time", "Get sample number from time...")
	REAL ("Time (s)", "0.5")
	OK
DO
	Melder_informationReal (Sampled_xToIndex (ONLY (classSound), GET_REAL ("Time")), NULL);
END

DIRECT (Sound_getIntensity_dB)
	Melder_informationReal (Sound_getIntensity_dB (ONLY (classSound)), "dB");
END

FORM (Sound_getMaximum, "Sound: Get maximum", "Sound: Get maximum...")
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0 (= all)")
	RADIO ("Interpolation", 4)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	RADIOBUTTON ("Cubic")
	RADIOBUTTON ("Sinc70")
	RADIOBUTTON ("Sinc700")
	OK
DO
	Melder_informationReal (Vector_getMaximum (ONLY (classSound),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Interpolation") - 1), "Pascal");
END

FORM (old_Sound_getMean, "Sound: Get mean", "Sound: Get mean...")
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0 (= all)")
	OK
DO
	Melder_informationReal (Vector_getMean (ONLY (classSound),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), Vector_CHANNEL_AVERAGE), "Pascal");
END

FORM (Sound_getMean, "Sound: Get mean", "Sound: Get mean...")
	OPTIONMENU ("Channel", 1)
		OPTION ("All")
		OPTION ("Left")
		OPTION ("Right")
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0 (= all)")
	OK
DO_ALTERNATIVE (old_Sound_getMean)
	Sound me = ONLY (classSound);
	long channel = GET_INTEGER ("Channel") - 1;
	if (channel > my ny) channel = 1;
	Melder_informationReal (Vector_getMean (me, GET_REAL ("left Time range"), GET_REAL ("right Time range"), channel), "Pascal");
END

FORM (Sound_getMinimum, "Sound: Get minimum", "Sound: Get minimum...")
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0 (= all)")
	RADIO ("Interpolation", 4)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	RADIOBUTTON ("Cubic")
	RADIOBUTTON ("Sinc70")
	RADIOBUTTON ("Sinc700")
	OK
DO
	Melder_informationReal (Vector_getMinimum (ONLY (classSound),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Interpolation") - 1), "Pascal");
END

FORM (old_Sound_getNearestZeroCrossing, "Sound: Get nearest zero crossing", "Sound: Get nearest zero crossing...")
	REAL ("Time (s)", "0.5")
	OK
DO
	Sound me = ONLY (classSound);
	if (my ny > 1) return Melder_error ("Cannot determine a zero crossing for a stereo sound.");
END

FORM (Sound_getNearestZeroCrossing, "Sound: Get nearest zero crossing", "Sound: Get nearest zero crossing...")
	OPTIONMENU ("Channel", 1)
		OPTION ("Left")
		OPTION ("Right")
	REAL ("Time (s)", "0.5")
	OK
DO_ALTERNATIVE (Sound_getNearestZeroCrossing)
	Sound me = ONLY (classSound);
	long channel = GET_INTEGER ("Channel");
	if (channel > my ny) channel = 1;
	Melder_informationReal (Sound_getNearestZeroCrossing (me, GET_REAL ("Time"), channel), "seconds");
END

DIRECT (Sound_getNumberOfChannels)
	Sound me = ONLY (classSound);
	Melder_information2 (Melder_integer (my ny), my ny == 1 ? " channel (mono)" : my ny == 2 ? " channels (stereo)" : "channels");
END

DIRECT (Sound_getNumberOfSamples)
	Sound me = ONLY (classSound);
	Melder_information2 (Melder_integer (my nx), " samples");
END

FORM (Sound_getPower, "Sound: Get power", "Sound: Get power...")
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0 (= all)")
	OK
DO
	Melder_informationReal (Sound_getPower (ONLY (classSound), GET_REAL ("left Time range"), GET_REAL ("right Time range")), "Pa2");
END

DIRECT (Sound_getPowerInAir)
	Melder_informationReal (Sound_getPowerInAir (ONLY (classSound)), "Watt/m2");
END

FORM (Sound_getRootMeanSquare, "Sound: Get root-mean-square", "Sound: Get root-mean-square...")
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0 (= all)")
	OK
DO
	Melder_informationReal (Sound_getRootMeanSquare (ONLY (classSound), GET_REAL ("left Time range"), GET_REAL ("right Time range")), "Pascal");
END

DIRECT (Sound_getSamplePeriod)
	Sound me = ONLY (classSound);
	Melder_informationReal (my dx, "seconds");
END

DIRECT (Sound_getSampleRate)
	Sound me = ONLY (classSound);
	Melder_informationReal (1 / my dx, "Hertz");
END

FORM (old_Sound_getStandardDeviation, "Sound: Get standard deviation", "Sound: Get standard deviation...")
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0 (= all)")
	OK
DO
	Melder_informationReal (Vector_getStandardDeviation (ONLY (classSound),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), Vector_CHANNEL_AVERAGE), "Pascal");
END

FORM (Sound_getStandardDeviation, "Sound: Get standard deviation", "Sound: Get standard deviation...")
	OPTIONMENU ("Channel", 1)
		OPTION ("Average")
		OPTION ("Left")
		OPTION ("Right")
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0 (= all)")
	OK
DO_ALTERNATIVE (old_Sound_getStandardDeviation)
	Sound me = ONLY (classSound);
	long channel = GET_INTEGER ("Channel") - 1;
	if (channel > my ny) channel = 1;
	Melder_informationReal (Vector_getStandardDeviation (me,
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), channel), "Pascal");
END

FORM (Sound_getTimeFromIndex, "Get time from sample number", "Get time from sample number...")
	INTEGER ("Sample number", "100")
	OK
DO
	Melder_informationReal (Sampled_indexToX (ONLY (classSound), GET_INTEGER ("Sample number")), "seconds");
END

FORM (Sound_getTimeOfMaximum, "Sound: Get time of maximum", "Sound: Get time of maximum...")
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0 (= all)")
	RADIO ("Interpolation", 4)
		RADIOBUTTON ("None")
		RADIOBUTTON ("Parabolic")
		RADIOBUTTON ("Cubic")
		RADIOBUTTON ("Sinc70")
		RADIOBUTTON ("Sinc700")
	OK
DO
	Melder_informationReal (Vector_getXOfMaximum (ONLY (classSound),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Interpolation") - 1), "seconds");
END

FORM (Sound_getTimeOfMinimum, "Sound: Get time of minimum", "Sound: Get time of minimum...")
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0 (= all)")
	RADIO ("Interpolation", 4)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	RADIOBUTTON ("Cubic")
	RADIOBUTTON ("Sinc70")
	RADIOBUTTON ("Sinc700")
	OK
DO
	Melder_informationReal (Vector_getXOfMinimum (ONLY (classSound),
		GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Interpolation") - 1), "seconds");
END

FORM (old_Sound_getValueAtIndex, "Sound: Get value at sample number", "Sound: Get value at sample number...")
	INTEGER ("Sample number", "100")
	OK
DO
	Sound me = ONLY (classSound);
	long sampleIndex = GET_INTEGER ("Sample number");
	Melder_informationReal (sampleIndex < 1 || sampleIndex > my nx ? NUMundefined :
		my ny == 1 ? my z [1] [sampleIndex] : 0.5 * (my z [1] [sampleIndex] + my z [2] [sampleIndex]), "Pascal");
END

FORM (Sound_getValueAtIndex, "Sound: Get value at sample number", "Sound: Get value at sample number...")
	OPTIONMENU ("Channel", 1)
		OPTION ("Average")
		OPTION ("Left")
		OPTION ("Right")
	INTEGER ("Sample number", "100")
	OK
DO_ALTERNATIVE (old_Sound_getValueAtIndex)
	Sound me = ONLY (classSound);
	long sampleIndex = GET_INTEGER ("Sample number");
	long channel = GET_INTEGER ("Channel") - 1;
	if (channel > my ny) channel = 1;
	Melder_informationReal (sampleIndex < 1 || sampleIndex > my nx ? NUMundefined :
		Sampled_getValueAtSample (me, sampleIndex, channel, 0), "Pascal");
END

FORM (old_Sound_getValueAtTime, "Sound: Get value at time", "Sound: Get value at time...")
	REAL ("Time (s)", "0.5")
	RADIO ("Interpolation", 4)
		RADIOBUTTON ("Nearest")
		RADIOBUTTON ("Linear")
		RADIOBUTTON ("Cubic")
		RADIOBUTTON ("Sinc70")
		RADIOBUTTON ("Sinc700")
	OK
DO
	Melder_informationReal (Vector_getValueAtX (ONLY (classSound), GET_REAL ("Time"),
		Vector_CHANNEL_AVERAGE, GET_INTEGER ("Interpolation") - 1), "Pascal");
END

FORM (Sound_getValueAtTime, "Sound: Get value at time", "Sound: Get value at time...")
	OPTIONMENU ("Channel", 1)
		OPTION ("Average")
		OPTION ("Left")
		OPTION ("Right")
	REAL ("Time (s)", "0.5")
	RADIO ("Interpolation", 4)
		RADIOBUTTON ("Nearest")
		RADIOBUTTON ("Linear")
		RADIOBUTTON ("Cubic")
		RADIOBUTTON ("Sinc70")
		RADIOBUTTON ("Sinc700")
	OK
DO_ALTERNATIVE (old_Sound_getValueAtTime)
	Sound me = ONLY (classSound);
	long channel = GET_INTEGER ("Channel") - 1;
	if (channel > my ny) channel = 1;
	Melder_informationReal (Vector_getValueAtX (ONLY (classSound), GET_REAL ("Time"),
		channel, GET_INTEGER ("Interpolation") - 1), "Pascal");
END

DIRECT (Sound_help) Melder_help ("Sound"); END

FORM (Sound_lengthen_psola, "Sound: Lengthen (PSOLA)", "Sound: Lengthen (PSOLA)...")
	POSITIVE ("Minimum pitch (Hz)", "75")
	POSITIVE ("Maximum pitch (Hz)", "600")
	POSITIVE ("Factor", "1.5")
	OK
DO
	double minimumPitch = GET_REAL ("Minimum pitch"), maximumPitch = GET_REAL ("Maximum pitch");
	double factor = GET_REAL ("Factor");
	REQUIRE (minimumPitch < maximumPitch, "Maximum pitch should be greater than minimum pitch.")
	WHERE (SELECTED)
		if (! praat_new (Sound_lengthen_psola (OBJECT, minimumPitch, maximumPitch, factor),
			"%s_%.2f", NAME, factor)) return 0;
END

FORM (Sound_multiply, "Sound: Multiply", 0)
	REAL ("Multiplication factor", "1.5")
	OK
DO
	WHERE (SELECTED) {
		Vector_multiplyByScalar (OBJECT, GET_REAL ("Multiplication factor"));
		praat_dataChanged (OBJECT);
	}
END

FORM (Sound_multiplyByWindow, "Sound: Multiply by window", 0)
	ENUM ("Window shape", Sound_WINDOW, enumi (Sound_WINDOW, Hanning))
	OK
DO
	WHERE (SELECTED) {
		Sound_multiplyByWindow (OBJECT, GET_INTEGER ("Window shape"));
		praat_dataChanged (OBJECT);
	}
END

FORM (Sound_overrideSamplingFrequency, "Sound: Override sampling frequency", 0)
	POSITIVE ("New sampling frequency (Hz)", "16000.0")
	OK
DO
	WHERE (SELECTED) {
		Sound_overrideSamplingFrequency (OBJECT, GET_REAL ("New sampling frequency"));
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

FORM (Sound_preemphasizeInline, "Sound: Pre-emphasize (in-line)", "Sound: Pre-emphasize (in-line)...")
	REAL ("From frequency (Hz)", "50.0")
	OK
DO
	WHERE (SELECTED) {
		Sound_preEmphasis (OBJECT, GET_REAL ("From frequency"));
		Vector_scale (OBJECT, 0.99);
		praat_dataChanged (OBJECT);
	}
END

FORM_READ (Sound_read2FromStereoFile, "Read two Sounds from stereo file", 0)
	Sound left, right;
	if (! Sound_read2FromSoundFile (file, & left, & right)) return 0;
	if (! praat_new (left, "left")) return 0;
	if (right) { if (! praat_new (right, "right")) return 0; }
END

FORM_READ (Sound_readFromRawAlawFile, "Read Sound from raw Alaw file", 0)
	if (! praat_new (Sound_readFromRawAlawFile (file), MelderFile_name (file))) return 0;
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
	if (! praat_new (publish, NULL)) Melder_flushError (NULL);
	praat_updateSelection ();
}
DIRECT (Sound_record_mono)
	if (theCurrentPraat -> batch) return Melder_error ("Cannot record a Sound from batch.");
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
	if (! praat_new (publish1, "left") || ! praat_new (publish2, "right")) Melder_flushError (NULL);
	praat_updateSelection ();
}
DIRECT (Sound_record_stereo)
	if (theCurrentPraat -> batch) return Melder_error ("Cannot record a Sound from batch.");
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

FORM (Sound_recordFixedTime, "Record Sound", 0)
	RADIO ("Input source", 1)
		RADIOBUTTON ("Microphone")
		RADIOBUTTON ("Line")
	#if defined (sgi)
		RADIOBUTTON ("Digital")
		REAL ("Gain (0-1)", "0.5")
	#else
		REAL ("Gain (0-1)", "0.1")
	#endif
	REAL ("Balance (0-1)", "0.5")
	RADIO ("Sampling frequency", 1)
		#if defined (hpux)
			RADIOBUTTON ("5512")
		#endif
		#ifdef UNIX
		RADIOBUTTON ("8000")
		#endif
		#ifdef sgi
		RADIOBUTTON ("9800")
		#endif
		#ifndef macintosh
		RADIOBUTTON ("11025")
		#endif
		#ifdef UNIX
		RADIOBUTTON ("16000")
		#endif
		#ifndef macintosh
		RADIOBUTTON ("22050")
		#endif
		#ifdef UNIX
		RADIOBUTTON ("32000")
		#endif
		RADIOBUTTON ("44100")
		RADIOBUTTON ("48000")
		RADIOBUTTON ("96000")
	POSITIVE ("Duration (seconds)", "1.0")
	OK
DO
	NEW (Sound_recordFixedTime (GET_INTEGER ("Input source"),
		GET_REAL ("Gain"), GET_REAL ("Balance"),
		atof (GET_STRING ("Sampling frequency")), GET_REAL ("Duration")));
END

FORM (Sound_resample, "Sound: Resample", "Sound: Resample...")
	POSITIVE ("New sampling frequency (Hz)", "10000")
	NATURAL ("Precision (samples)", "50")
	OK
DO
	double samplingFrequency = GET_REAL ("New sampling frequency");
	WHERE (SELECTED)
		if (! praat_new (Sound_resample (OBJECT, samplingFrequency, GET_INTEGER ("Precision")),
			"%s %ld", NAME, (long) floor (samplingFrequency + 0.5))) return 0;
END

DIRECT (Sound_reverse)
	WHERE (SELECTED) {
		Sound_reverse (OBJECT, 0, 0);
		praat_dataChanged (OBJECT);
	}
END

FORM (Sound_scalePeak, "Sound: Scale peak", 0)
	POSITIVE ("New maximum amplitude", "0.99")
	OK
DO
	WHERE (SELECTED) {
		Vector_scale (OBJECT, GET_REAL ("New maximum amplitude"));
		praat_dataChanged (OBJECT);
	}
END

FORM (Sound_scaleIntensity, "Sound: Scale intensity", 0)
	POSITIVE ("New average intensity (dB)", "70.0")
	OK
DO
	WHERE (SELECTED) {
		Sound_scaleIntensity (OBJECT, GET_REAL ("New average intensity"));
		praat_dataChanged (OBJECT);
	}
END

FORM (old_Sound_setValueAtIndex, "Sound: Set value at sample number", "Sound: Set value at sample number...")
	NATURAL ("Sample number", "100")
	REAL ("New value", "0")
	OK
DO
	WHERE (SELECTED) {
		Sound me = OBJECT;
		long index = GET_INTEGER ("Sample number");
		if (index > my nx)
			return Melder_error ("The sample number should not exceed the number of samples, which is %ld.", my nx);
		for (long channel = 1; channel <= my ny; channel ++)
			my z [channel] [index] = GET_REAL ("New value");
		praat_dataChanged (me);
	}
END

FORM (Sound_setValueAtIndex, "Sound: Set value at sample number", "Sound: Set value at sample number...")
	OPTIONMENU ("Channel", 2)
		OPTION ("Both")
		OPTION ("Left")
		OPTION ("Right")
	NATURAL ("Sample number", "100")
	REAL ("New value", "0")
	OK
DO_ALTERNATIVE (old_Sound_setValueAtIndex)
	WHERE (SELECTED) {
		Sound me = OBJECT;
		long index = GET_INTEGER ("Sample number");
		if (index > my nx)
			return Melder_error ("The sample number should not exceed the number of samples, which is %ld.", my nx);
		long channel = GET_INTEGER ("Channel") - 1;
		if (channel > my ny) channel = 1;
		if (channel > 0) {
			my z [channel] [index] = GET_REAL ("New value");
		} else {
			for (channel = 1; channel <= my ny; channel ++) {
				my z [channel] [index] = GET_REAL ("New value");
			}
		}
		praat_dataChanged (me);
	}
END

FORM (Sound_setPartToZero, "Sound: Set part to zero", 0)
	REAL ("left Time range (s)", "0.0")
	REAL ("right Time range (s)", "0.0 (= all)")
	RADIO ("Cut", 2)
		OPTION ("at exactly these times")
		OPTION ("at nearest zero crossing")
	OK
DO
	WHERE (SELECTED) {
		Sound_setZero (OBJECT, GET_REAL ("left Time range"), GET_REAL ("right Time range"), GET_INTEGER ("Cut") - 1);
		praat_dataChanged (OBJECT);
	}
END

DIRECT (Sound_subtractMean)
	WHERE (SELECTED) {
		Vector_subtractMean (OBJECT);
		praat_dataChanged (OBJECT);
	}
END

FORM (Sound_to_Manipulation, "Sound: To Manipulation", "Manipulation")
	POSITIVE ("Time step (s)", "0.01")
	POSITIVE ("Minimum pitch (Hz)", "75")
	POSITIVE ("Maximum pitch (Hz)", "600")
	OK
DO
	double fmin = GET_REAL ("Minimum pitch"), fmax = GET_REAL ("Maximum pitch");
	REQUIRE (fmax > fmin, "Maximum pitch must be greater than minimum pitch.");
	EVERY_TO (Sound_to_Manipulation (OBJECT, GET_REAL ("Time step"), fmin, fmax))
END

FORM (Sound_to_Cochleagram, "Sound: To Cochleagram", 0)
	POSITIVE ("Time step (s)", "0.01")
	POSITIVE ("Frequency resolution (Bark)", "0.1")
	POSITIVE ("Window length (s)", "0.03")
	REAL ("Forward-masking time (s)", "0.03")
	OK
DO
	EVERY_TO (Sound_to_Cochleagram (OBJECT, GET_REAL ("Time step"),
		GET_REAL ("Frequency resolution"), GET_REAL ("Window length"), GET_REAL ("Forward-masking time")))
END

FORM (Sound_to_Cochleagram_edb, "Sound: To Cochleagram (De Boer, Meddis & Hewitt)", 0)
	POSITIVE ("Time step (s)", "0.01")
	POSITIVE ("Frequency resolution (Bark)", "0.1")
	BOOLEAN ("Has synapse", 1)
	LABEL ("", "Meddis synapse properties")
	POSITIVE ("   replenishment rate (/sec)", "5.05")
	POSITIVE ("   loss rate (/sec)", "2500")
	POSITIVE ("   return rate (/sec)", "6580")
	POSITIVE ("   reprocessing rate (/sec)", "66.31")
	OK
DO
	EVERY_TO (Sound_to_Cochleagram_edb (OBJECT, GET_REAL ("Time step"),
		GET_REAL ("Frequency resolution"), GET_INTEGER ("Has synapse"),
		GET_REAL ("   replenishment rate"), GET_REAL ("   loss rate"),
		GET_REAL ("   return rate"), GET_REAL ("   reprocessing rate")))
END

FORM (Sound_to_Formant_burg, "Sound: To Formant (Burg method)", "Sound: To Formant (burg)...")
	REAL ("Time step (s)", "0.0 (= auto)")
	POSITIVE ("Max. number of formants", "5")
	REAL ("Maximum formant (Hz)", "5500 (= adult female)")
	POSITIVE ("Window length (s)", "0.025")
	POSITIVE ("Pre-emphasis from (Hz)", "50")
	OK
DO
	EVERY_TO (Sound_to_Formant_burg (OBJECT, GET_REAL ("Time step"),
		GET_REAL ("Max. number of formants"), GET_REAL ("Maximum formant"),
		GET_REAL ("Window length"), GET_REAL ("Pre-emphasis from")))
END

FORM (Sound_to_Formant_keepAll, "Sound: To Formant (keep all)", "Sound: To Formant (keep all)...")
	REAL ("Time step (s)", "0.0 (= auto)")
	POSITIVE ("Max. number of formants", "5")
	REAL ("Maximum formant (Hz)", "5500 (= adult female)")
	POSITIVE ("Window length (s)", "0.025")
	POSITIVE ("Pre-emphasis from (Hz)", "50")
	OK
DO
	EVERY_TO (Sound_to_Formant_keepAll (OBJECT, GET_REAL ("Time step"),
		GET_REAL ("Max. number of formants"), GET_REAL ("Maximum formant"),
		GET_REAL ("Window length"), GET_REAL ("Pre-emphasis from")))
END

FORM (Sound_to_Formant_willems, "Sound: To Formant (split Levinson (Willems))", "Sound: To Formant (sl)...")
	REAL ("Time step (s)", "0.0 (= auto)")
	POSITIVE ("Number of formants", "5")
	REAL ("Maximum formant (Hz)", "5500 (= adult female)")
	POSITIVE ("Window length (s)", "0.025")
	POSITIVE ("Pre-emphasis from (Hz)", "50")
	OK
DO
	EVERY_TO (Sound_to_Formant_willems (OBJECT, GET_REAL ("Time step"),
		GET_REAL ("Number of formants"), GET_REAL ("Maximum formant"),
		GET_REAL ("Window length"), GET_REAL ("Pre-emphasis from")))
END

FORM (Sound_to_Harmonicity_ac, "Sound: To Harmonicity (ac)", "Sound: To Harmonicity (ac)...")
	POSITIVE ("Time step (s)", "0.01")
	POSITIVE ("Minimum pitch (Hz)", "75")
	REAL ("Silence threshold", "0.1")
	POSITIVE ("Periods per window", "4.5")
	OK
DO
	double periodsPerWindow = GET_REAL ("Periods per window");
	REQUIRE (periodsPerWindow >= 3.0, "Number of periods per window must be >= 3.")
	EVERY_TO (Sound_to_Harmonicity_ac (OBJECT, GET_REAL ("Time step"),
		GET_REAL ("Minimum pitch"), GET_REAL ("Silence threshold"), periodsPerWindow))
END

FORM (Sound_to_Harmonicity_cc, "Sound: To Harmonicity (cc)", "Sound: To Harmonicity (cc)...")
	POSITIVE ("Time step (s)", "0.01")
	POSITIVE ("Minimum pitch (Hz)", "75")
	REAL ("Silence threshold", "0.1")
	POSITIVE ("Periods per window", "1.0")
	OK
DO
	EVERY_TO (Sound_to_Harmonicity_cc (OBJECT, GET_REAL ("Time step"),
		GET_REAL ("Minimum pitch"), GET_REAL ("Silence threshold"),
		GET_REAL ("Periods per window")))
END

FORM (Sound_to_Harmonicity_gne, "Sound: To Harmonicity (gne)", 0)
	POSITIVE ("Minimum frequency (Hz)", "500")
	POSITIVE ("Maximum frequency (Hz)", "4500")
	POSITIVE ("Bandwidth (Hz)", "1000")
	POSITIVE ("Step (Hz)", "80")
	OK
DO
	EVERY_TO (Sound_to_Harmonicity_GNE (OBJECT, GET_REAL ("Minimum frequency"),
		GET_REAL ("Maximum frequency"), GET_REAL ("Bandwidth"),
		GET_REAL ("Step")))
END

FORM (old_Sound_to_Intensity, "Sound: To Intensity", "Sound: To Intensity...")
	POSITIVE ("Minimum pitch (Hz)", "100")
	REAL ("Time step (s)", "0.0 (= auto)")
	OK
DO
	EVERY_TO (Sound_to_Intensity ((Sound) OBJECT,
		GET_REAL ("Minimum pitch"), GET_REAL ("Time step"), FALSE))
END

FORM (Sound_to_Intensity, "Sound: To Intensity", "Sound: To Intensity...")
	POSITIVE ("Minimum pitch (Hz)", "100")
	REAL ("Time step (s)", "0.0 (= auto)")
	BOOLEAN ("Subtract mean", 1)
	OK
DO_ALTERNATIVE (old_Sound_to_Intensity)
	EVERY_TO (Sound_to_Intensity ((Sound) OBJECT,
		GET_REAL ("Minimum pitch"), GET_REAL ("Time step"), GET_INTEGER ("Subtract mean")))
END

DIRECT (Sound_to_IntervalTier)
	EVERY_TO (IntervalTier_create (((Sound) OBJECT) -> xmin, ((Sound) OBJECT) -> xmax))
END

FORM (Sound_to_Ltas, "Sound: To long-term average spectrum", 0)
	POSITIVE ("Bandwidth (Hz)", "100")
	OK
DO
	EVERY_TO (Sound_to_Ltas (OBJECT, GET_REAL ("Bandwidth")))
END

FORM (Sound_to_Ltas_pitchCorrected, "Sound: To Ltas (pitch-corrected)", "Sound: To Ltas (pitch-corrected)...")
	POSITIVE ("Minimum pitch (Hz)", "75")
	POSITIVE ("Maximum pitch (Hz)", "600")
	POSITIVE ("Maximum frequency (Hz)", "5000")
	POSITIVE ("Bandwidth (Hz)", "100")
	REAL ("Shortest period (s)", "0.0001")
	REAL ("Longest period (s)", "0.02")
	POSITIVE ("Maximum period factor", "1.3")
	OK
DO
	double fmin = GET_REAL ("Minimum pitch"), fmax = GET_REAL ("Maximum pitch");
	REQUIRE (fmax > fmin, "Maximum pitch must be greater than minimum pitch.");
	EVERY_TO (Sound_to_Ltas_pitchCorrected (OBJECT, fmin, fmax,
		GET_REAL ("Maximum frequency"), GET_REAL ("Bandwidth"),
		GET_REAL ("Shortest period"), GET_REAL ("Longest period"), GET_REAL ("Maximum period factor")))
END

DIRECT (Sound_to_Matrix)
	EVERY_TO (Sound_to_Matrix (OBJECT))
END

DIRECT (Sounds_to_ParamCurve)
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	if (! praat_new (ParamCurve_create (s1, s2), "%s_%s", s1 -> name, s2 -> name)) return 0;
END

FORM (Sound_to_Pitch, "Sound: To Pitch", "Sound: To Pitch...")
	REAL ("Time step (s)", "0.0 (= auto)")
	POSITIVE ("Pitch floor (Hz)", "75.0")
	POSITIVE ("Pitch ceiling (Hz)", "600.0")
	OK
DO
	EVERY_TO (Sound_to_Pitch (OBJECT, GET_REAL ("Time step"),
		GET_REAL ("Pitch floor"), GET_REAL ("Pitch ceiling")))
END

FORM (Sound_to_Pitch_ac, "Sound: To Pitch (ac)", "Sound: To Pitch (ac)...")
	LABEL ("", "Finding the candidates")
	REAL ("Time step (s)", "0.0 (= auto)")
	POSITIVE ("Pitch floor (Hz)", "75.0")
	NATURAL ("Max. number of candidates", "15")
	BOOLEAN ("Very accurate", 0)
	LABEL ("", "Finding a path")
	REAL ("Silence threshold", "0.03")
	REAL ("Voicing threshold", "0.45")
	REAL ("Octave cost", "0.01")
	REAL ("Octave-jump cost", "0.35")
	REAL ("Voiced / unvoiced cost", "0.14")
	POSITIVE ("Pitch ceiling (Hz)", "600.0")
	OK
DO
	long maxnCandidates = GET_INTEGER ("Max. number of candidates");
	REQUIRE (maxnCandidates >= 2, "Maximum number of candidates must be greater than 1.")
	EVERY_TO (Sound_to_Pitch_ac (OBJECT, GET_REAL ("Time step"),
		GET_REAL ("Pitch floor"), 3.0, maxnCandidates, GET_INTEGER ("Very accurate"),
		GET_REAL ("Silence threshold"), GET_REAL ("Voicing threshold"),
		GET_REAL ("Octave cost"), GET_REAL ("Octave-jump cost"),
		GET_REAL ("Voiced / unvoiced cost"), GET_REAL ("Pitch ceiling")))
END

FORM (Sound_to_Pitch_cc, "Sound: To Pitch (cc)", "Sound: To Pitch (cc)...")
	LABEL ("", "Finding the candidates")
	REAL ("Time step (s)", "0.0 (= auto)")
	POSITIVE ("Pitch floor (Hz)", "75")
	NATURAL ("Max. number of candidates", "15")
	BOOLEAN ("Very accurate", 0)
	LABEL ("", "Finding a path")
	REAL ("Silence threshold", "0.03")
	REAL ("Voicing threshold", "0.45")
	REAL ("Octave cost", "0.01")
	REAL ("Octave-jump cost", "0.35")
	REAL ("Voiced / unvoiced cost", "0.14")
	POSITIVE ("Pitch ceiling (Hz)", "600")
	OK
DO
	long maxnCandidates = GET_INTEGER ("Max. number of candidates");
	REQUIRE (maxnCandidates >= 2, "Maximum number of candidates must be greater than 1.")
	EVERY_TO (Sound_to_Pitch_cc (OBJECT, GET_REAL ("Time step"),
		GET_REAL ("Pitch floor"), 1.0, maxnCandidates, GET_INTEGER ("Very accurate"),
		GET_REAL ("Silence threshold"), GET_REAL ("Voicing threshold"),
		GET_REAL ("Octave cost"), GET_REAL ("Octave-jump cost"),
		GET_REAL ("Voiced / unvoiced cost"), GET_REAL ("Pitch ceiling")))
END

FORM (Sound_to_PointProcess_extrema, "Sound: To PointProcess (extrema)", 0)
	OPTIONMENU ("Channel", 1)
		OPTION ("Left")
		OPTION ("Right")
	BOOLEAN ("Include maxima", 1)
	BOOLEAN ("Include minima", 0)
	RADIO ("Interpolation", 4)
	RADIOBUTTON ("None")
	RADIOBUTTON ("Parabolic")
	RADIOBUTTON ("Cubic")
	RADIOBUTTON ("Sinc70")
	RADIOBUTTON ("Sinc700")
	OK
DO
	long channel = GET_INTEGER ("Channel");
	EVERY_TO (Sound_to_PointProcess_extrema (OBJECT, channel > ((Sound) OBJECT) -> ny ? 1 : channel, GET_INTEGER ("Interpolation") - 1,
		GET_INTEGER ("Include maxima"), GET_INTEGER ("Include minima")))
END

FORM (Sound_to_PointProcess_periodic_cc, "Sound: To PointProcess (periodic, cc)", "Sound: To PointProcess (periodic, cc)...")
	POSITIVE ("Minimum pitch (Hz)", "75")
	POSITIVE ("Maximum pitch (Hz)", "600")
	OK
DO
	double fmin = GET_REAL ("Minimum pitch"), fmax = GET_REAL ("Maximum pitch");
	REQUIRE (fmax > fmin, "Maximum pitch must be greater than minimum pitch.");
	EVERY_TO (Sound_to_PointProcess_periodic_cc (OBJECT, fmin, fmax))
END

FORM (Sound_to_PointProcess_periodic_peaks, "Sound: To PointProcess (periodic, peaks)", "Sound: To PointProcess (periodic, peaks)...")
	POSITIVE ("Minimum pitch (Hz)", "75")
	POSITIVE ("Maximum pitch (Hz)", "600")
	BOOLEAN ("Include maxima", 1)
	BOOLEAN ("Include minima", 0)
	OK
DO
	double fmin = GET_REAL ("Minimum pitch"), fmax = GET_REAL ("Maximum pitch");
	REQUIRE (fmax > fmin, "Maximum pitch must be greater than minimum pitch.");
	EVERY_TO (Sound_to_PointProcess_periodic_peaks (OBJECT, fmin, fmax, GET_INTEGER ("Include maxima"), GET_INTEGER ("Include minima")))
END

FORM (Sound_to_PointProcess_zeroes, "Get zeroes", 0)
	OPTIONMENU ("Channel", 1)
		OPTION ("Left")
		OPTION ("Right")
	BOOLEAN ("Include raisers", 1)
	BOOLEAN ("Include fallers", 0)
	OK
DO
	long channel = GET_INTEGER ("Channel");
	EVERY_TO (Sound_to_PointProcess_zeroes (OBJECT, channel > ((Sound) OBJECT) -> ny ? 1 : channel,
		GET_INTEGER ("Include raisers"), GET_INTEGER ("Include fallers")))
END

FORM (Sound_to_Spectrogram, "Sound: To Spectrogram", "Sound: To Spectrogram...")
	POSITIVE ("Window length (s)", "0.005")
	POSITIVE ("Maximum frequency (Hz)", "5000")
	POSITIVE ("Time step (s)", "0.002")
	POSITIVE ("Frequency step (Hz)", "20")
	RADIO ("Window shape", 6)
	{
		int i; for (i = 0; i < 6; i ++) {
			RADIOBUTTON (Sound_to_Spectrogram_windowShapeText (i))
		}
	}
	OK
DO
	EVERY_TO (Sound_to_Spectrogram (OBJECT, GET_REAL ("Window length"),
		GET_REAL ("Maximum frequency"), GET_REAL ("Time step"),
		GET_REAL ("Frequency step"), GET_INTEGER ("Window shape") - 1, 8.0, 8.0))
END

FORM (Sound_to_Spectrum, "Sound: To Spectrum", "Sound: To Spectrum...")
	BOOLEAN ("Fast", 1)
	OK
DO
	EVERY_TO (Sound_to_Spectrum (OBJECT, GET_INTEGER ("Fast")))
END

DIRECT (Sound_to_Spectrum_dft)
	EVERY_TO (Sound_to_Spectrum (OBJECT, FALSE))
END

DIRECT (Sound_to_Spectrum_fft)
	EVERY_TO (Sound_to_Spectrum (OBJECT, TRUE))
END

FORM (Sound_to_TextGrid, "Sound: To TextGrid", "Sound: To TextGrid...")
	SENTENCE ("All tier names", "Mary John bell")
	SENTENCE ("Which of these are point tiers?", "bell")
	OK
DO
	EVERY_TO (TextGrid_create (((Sound) OBJECT) -> xmin, ((Sound) OBJECT) -> xmax,
		GET_STRING ("All tier names"), GET_STRING ("Which of these are point tiers?")))
END

DIRECT (Sound_to_TextTier)
	EVERY_TO (TextTier_create (((Sound) OBJECT) -> xmin, ((Sound) OBJECT) -> xmax))
END

FORM (SoundInputPrefs, "Sound input preferences", "SoundRecorder")
	NATURAL ("Buffer size (MB)", "20")
	OK
SET_INTEGER ("Buffer size", SoundRecorder_getBufferSizePref_MB ())
DO
	long size = GET_INTEGER ("Buffer size");
	REQUIRE (size <= 1000, "Buffer size cannot exceed 1000 megabytes.")
	SoundRecorder_setBufferSizePref_MB (size);
END

FORM (SoundOutputPrefs, "Sound output preferences", 0)
	#if defined (sun) || defined (HPUX)
		RADIO ("Internal speaker", 1)
		RADIOBUTTON ("On")
		RADIOBUTTON ("Off")
	#endif
	#if defined (pietjepuk)
		REAL ("Output gain (0..1)", "0.3")
	#endif
	LABEL ("", "The following determines how sounds are played.")
	LABEL ("", "Between parentheses, you find what you can do simultaneously.")
	LABEL ("", "Decrease asynchronicity if sound plays with discontinuities.")
	OPTIONMENU ("Maximum asynchronicity", 4)
	OPTION ("Synchronous (nothing)")
	OPTION ("Calling back (view running cursor)")
	OPTION ("Interruptable (Escape key stops playing)")
	OPTION ("Asynchronous (anything)")
	REAL ("Silence before and after (s)", "0.0")
	OK
#if defined (sun) || defined (HPUX)
	SET_INTEGER ("Internal speaker", 2 - Melder_getUseInternalSpeaker ())
#endif
#if defined (pietjepuk)
	SET_REAL ("Output gain", Melder_getOutputGain ())
#endif
SET_INTEGER ("Maximum asynchronicity", Melder_getMaximumAsynchronicity () + 1);
SET_REAL ("Silence before and after", Melder_getZeroPadding ());
DO
	#if defined (sun) || defined (HPUX)
		Melder_setUseInternalSpeaker (2 - GET_INTEGER ("Internal speaker"));
	#endif
	#if defined (pietjepuk)
		Melder_setOutputGain (GET_REAL ("Gain"));
	#endif
	Melder_setMaximumAsynchronicity (GET_INTEGER ("Maximum asynchronicity") - 1);
	Melder_setZeroPadding (GET_REAL ("Silence before and after"));
END

FORM_WRITE (Sound_writeToAifcFile, "Write to AIFC file", 0, "aifc")
	if (! pr_LongSound_concatenate (file, Melder_AIFC)) return 0;
END

FORM_WRITE (Sound_writeToAiffFile, "Write to AIFF file", 0, "aiff")
	if (! pr_LongSound_concatenate (file, Melder_AIFF)) return 0;
END

FORM_WRITE (Sound_writeToRaw8bitUnsignedFile, "Write to raw 8-bit unsigned sound file", 0, "8uns")
	if (! Sound_writeToRaw8bitUnsignedFile (ONLY_OBJECT, file)) return 0;
END

FORM_WRITE (Sound_writeToRaw8bitSignedFile, "Write to raw 8-bit signed sound file", 0, "8sig")
	if (! Sound_writeToRaw8bitSignedFile (ONLY_OBJECT, file)) return 0;
END

FORM (Sound_writeToRawSoundFile, "Write to raw sound file", 0)
	LABEL ("", "Raw binary file:")
	TEXTFIELD ("Raw binary file", "")
	RADIO ("Encoding", 3)
		RADIOBUTTON ("Linear 8-bit signed")
		RADIOBUTTON ("Linear 8-bit unsigned")
		RADIOBUTTON ("Linear 16-bit big-endian")
		RADIOBUTTON ("Linear 16-bit little-endian")
	OK
DO
	structMelderFile file = { { 0 } };
	Melder_relativePathToFile (GET_STRING ("Raw binary file"), & file);
	if (! Sound_writeToRawSoundFile (ONLY_OBJECT, & file, GET_INTEGER ("Encoding"))) return 0;
END

FORM_WRITE (Sound_writeToKayFile, "Write to Kay sound file", 0, "kay")
	if (! Sound_writeToKayFile (ONLY_OBJECT, file)) return 0;
END

#ifdef macintosh
FORM_WRITE (Sound_writeToMacSoundFile, "Write to Macintosh sound file", 0, "macsound")
	if (! Sound_writeToMacSoundFile (ONLY_OBJECT, file)) return 0;
END
#endif

FORM_WRITE (Sound_writeToNextSunFile, "Write to NeXT/Sun file", 0, "au")
	if (! pr_LongSound_concatenate (file, Melder_NEXT_SUN)) return 0;
END

FORM_WRITE (Sound_writeToNistFile, "Write to NIST file", 0, "nist")
	if (! pr_LongSound_concatenate (file, Melder_NIST)) return 0;
END

FORM_WRITE (Sound_writeToFlacFile, "Write to FLAC file", 0, "flac")
	if (! pr_LongSound_concatenate (file, Melder_FLAC)) return 0;
END

FORM_WRITE (Sound_writeToSesamFile, "Write to Sesam file", 0, "sdf")
	if (! Sound_writeToSesamFile (ONLY_OBJECT, file)) return 0;
END

FORM_WRITE (Sound_writeToStereoAifcFile, "Write to stereo AIFC file", 0, "aifc")
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	Melder_assert (s1 && s2);
	Sound stereo = Sounds_combineToStereo (s1, s2); if (stereo == NULL) return 0;
	if (! Sound_writeToAudioFile16 (stereo, file, Melder_AIFC)) { forget (stereo); return 0; }
	forget (stereo);
END

FORM_WRITE (Sound_writeToStereoAiffFile, "Write to stereo AIFF file", 0, "aiff")
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	Melder_assert (s1 && s2);
	Sound stereo = Sounds_combineToStereo (s1, s2); if (stereo == NULL) return 0;
	if (! Sound_writeToAudioFile16 (stereo, file, Melder_AIFF)) { forget (stereo); return 0; }
	forget (stereo);
END

FORM_WRITE (Sound_writeToStereoNextSunFile, "Write to stereo NeXT/Sun file", 0, "au")
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	Melder_assert (s1 && s2);
	Sound stereo = Sounds_combineToStereo (s1, s2); if (stereo == NULL) return 0;
	if (! Sound_writeToAudioFile16 (stereo, file, Melder_NEXT_SUN)) { forget (stereo); return 0; }
	forget (stereo);
END

FORM_WRITE (Sound_writeToStereoNistFile, "Write to stereo NIST file", 0, "nist")
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	Melder_assert (s1 && s2);
	Sound stereo = Sounds_combineToStereo (s1, s2); if (stereo == NULL) return 0;
	if (! Sound_writeToAudioFile16 (stereo, file, Melder_NIST)) { forget (stereo); return 0; }
	forget (stereo);
END

FORM_WRITE (Sound_writeToStereoFlacFile, "Write to stereo FLAC file", 0, "flac")
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	Melder_assert (s1 && s2);
	Sound stereo = Sounds_combineToStereo (s1, s2); if (stereo == NULL) return 0;
	if (! Sound_writeToAudioFile16 (stereo, file, Melder_FLAC)) { forget (stereo); return 0; }
	forget (stereo);
END

FORM_WRITE (Sound_writeToStereoWavFile, "Write to stereo WAV file", 0, "wav")
	Sound s1 = NULL, s2 = NULL;
	WHERE (SELECTED) { if (s1) s2 = OBJECT; else s1 = OBJECT; }
	Melder_assert (s1 && s2);
	Sound stereo = Sounds_combineToStereo (s1, s2); if (stereo == NULL) return 0;
	if (! Sound_writeToAudioFile16 (stereo, file, Melder_WAV)) { forget (stereo); return 0; }
	forget (stereo);
END

FORM_WRITE (Sound_writeToSunAudioFile, "Write to NeXT/Sun file", 0, "au")
	if (! pr_LongSound_concatenate (file, Melder_NEXT_SUN)) return 0;
END

FORM_WRITE (Sound_writeToWavFile, "Write to WAV file", 0, "wav")
	if (! pr_LongSound_concatenate (file, Melder_WAV)) return 0;
END

/***** STOP *****/

DIRECT (stopPlayingSound)
	Melder_stopPlaying (Melder_IMPLICIT);
END

/***** Help menus *****/

DIRECT (AnnotationTutorial) Melder_help ("Intro 7. Annotation"); END
DIRECT (FilteringTutorial) Melder_help ("Filtering"); END

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
	char *fileName = MelderFile_name (file);
	(void) header;
	/*Melder_error ("%d %d %d %d %d %d %d %d %d %d", header [0],
		header [1], header [2], header [3],
		header [4], header [5], header [6],
		header [7], header [8], header [9]);*/
	if (nread < 512 || (! strstr (fileName, ".mov") && ! strstr (fileName, ".MOV") &&
	    ! strstr (fileName, ".avi") && ! strstr (fileName, ".AVI"))) return NULL;
	return Sound_readFromMovieFile (file);
}

static Any sesamFileRecognizer (int nread, const char *header, MelderFile file) {
	char *fileName = MelderFile_name (file);
	(void) header;
	if (nread < 512 || (! strstr (fileName, ".sdf") && ! strstr (fileName, ".SDF"))) return NULL;
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

	SoundEditor_prefs ();
	SoundRecorder_prefs ();
	FunctionEditor_prefs ();
	FunctionEditor_Sound_prefs ();
	FunctionEditor_SoundAnalysis_prefs ();
	LongSound_prefs ();

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
	praat_addMenuCommand ("Objects", "Preferences", "Sound input prefs...", 0, 0, DO_SoundInputPrefs);
	praat_addMenuCommand ("Objects", "Preferences", "Sound output prefs...", 0, 0, DO_SoundOutputPrefs);
	praat_addMenuCommand ("Objects", "Preferences", "LongSound prefs...", 0, 0, DO_LongSoundPrefs);

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
		praat_addAction1 (classSound, 0, "Lengthen (PSOLA)...", 0, 1, DO_Sound_lengthen_psola);
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
