/* SoundEditor.c
 *
 * Copyright (C) 1992-2005 Paul Boersma
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
 * pb 2002/07/16 GPL
 * pb 2003/05/21 more complete settings report
 * pb 2004/02/15 highlight selection, but not the spectrogram
 * pb 2005/06/16 units
 * pb 2005/09/21 interface update
 */

#include "SoundEditor.h"
#include "FunctionEditor_Sound.h"
#include "FunctionEditor_SoundAnalysis.h"
#include "Sound_and_Spectrogram.h"
#include "Pitch.h"
#include "Resources.h"
#include "EditorM.h"

#define SoundEditor_members FunctionEditor_members \
	Widget publishButton, publishPreserveButton, publishWindowButton; \
	Widget writeAiffButton, writeAifcButton, writeWavButton, writeNextSunButton, writeNistButton; \
	Widget cutButton, copyButton, pasteButton, zeroButton, reverseButton; \
	double minimum, maximum; \
	struct { int windowType; double relativeWidth; int preserveTimes; } publish; \
	double maxBuffer;
#define SoundEditor_methods FunctionEditor_methods
class_create_opaque (SoundEditor, FunctionEditor)

static struct {
	struct {
		int windowType;
		double relativeWidth;
		int preserveTimes;
	}
		publish;
}
	preferences = {
		enumi (Sound_WINDOW, Hanning), 1.0, TRUE   /* publish */
	};

void SoundEditor_prefs (void) {
	Resources_addInt ("SoundEditor.publish.windowType", & preferences.publish.windowType);
	Resources_addDouble ("SoundEditor.publish.relativeWidth", & preferences.publish.relativeWidth);
	Resources_addInt ("SoundEditor.publish.preserveTimes", & preferences.publish.preserveTimes);
}

/********** DESTRUCTION **********/

static void destroy (I) {
	iam (SoundEditor);
	FunctionEditor_SoundAnalysis_forget (me);
	inherited (SoundEditor) destroy (me);
}

/***** FILE MENU *****/

static int do_publish (SoundEditor me, int preserveTimes) {
	Sound publish = my longSound.data ? LongSound_extractPart (my data, my startSelection, my endSelection, preserveTimes) :
		Sound_extractPart (my data, my startSelection, my endSelection, enumi (Sound_WINDOW, Rectangular), 1.0, preserveTimes);
	if (! publish) return 0;
	if (my publishCallback)
		my publishCallback (me, my publishClosure, publish);
	return 1;
}

DIRECT (SoundEditor, cb_publish)
	return do_publish (me, FALSE);
END

DIRECT (SoundEditor, cb_publishPreserve)
	return do_publish (me, TRUE);
END

FORM (SoundEditor, cb_publishWindow, "Extract windowed selection", 0)
	WORD ("Name", "slice")
	ENUM ("Window", Sound_WINDOW, enumi (Sound_WINDOW, Hanning))
	POSITIVE ("Relative width", "1.0")
	BOOLEAN ("Preserve times", 1)
	OK
SET_INTEGER ("Window", my publish.windowType)
SET_REAL ("Relative width", my publish.relativeWidth)
SET_INTEGER ("Preserve times", my publish.preserveTimes)
DO
	Sound sound = my data, publish;
	preferences.publish.windowType = my publish.windowType = GET_INTEGER ("Window");
	preferences.publish.relativeWidth = my publish.relativeWidth = GET_REAL ("Relative width");
	preferences.publish.preserveTimes = my publish.preserveTimes = GET_INTEGER ("Preserve times");
	publish = Sound_extractPart (sound, my startSelection, my endSelection, my publish.windowType,
		my publish.relativeWidth, my publish.preserveTimes);
	if (! publish) return 0;
	Thing_setName (publish, GET_STRING ("Name"));
	if (my publishCallback)
		my publishCallback (me, my publishClosure, publish);
END

static int do_write (SoundEditor me, MelderFile file, int format) {
	if (my startSelection >= my endSelection)
		return Melder_error ("No samples selected.");
	if (my longSound.data) {
		return LongSound_writePartToAudioFile16 (my data, format, my startSelection, my endSelection, file);
	} else {
		Sound sound = my data;
		double margin = 0.0;
		long nmargin = margin / sound -> dx;
		long first, last, numberOfSamples, i, offset;
		numberOfSamples = Sampled_getWindowSamples (sound,
			my startSelection, my endSelection, & first, & last) + nmargin * 2;
		first -= nmargin;
		last += nmargin;
		if (numberOfSamples) {
			Sound save = Sound_create (0.0, numberOfSamples * sound -> dx,
							numberOfSamples, sound -> dx, 0.5 * sound -> dx);
			if (! save) return 0;
			offset = first - 1;
			if (first < 1) first = 1;
			if (last > sound -> nx) last = sound -> nx;
			for (i = first; i <= last; i ++)
				save -> z [1] [i - offset] = sound -> z [1] [i];
			return Sound_writeToAudioFile16 (save, NULL, file, format);
		}
	}
	return 0;
}

FORM_WRITE (SoundEditor, cb_writeAiff, "Write selection to AIFF file", 0)
	sprintf (defaultName, "%s.aiff", my longSound.data ? my longSound.data -> name : my sound.data -> name);
DO_WRITE
	if (! do_write (me, file, Melder_AIFF)) return 0;
END

FORM_WRITE (SoundEditor, cb_writeAifc, "Write selection to AIFC file", 0)
	sprintf (defaultName, "%s.aifc", my longSound.data ? my longSound.data -> name : my sound.data -> name);
DO_WRITE
	if (! do_write (me, file, Melder_AIFC)) return 0;
END

FORM_WRITE (SoundEditor, cb_writeWav, "Write selection to WAV file", 0)
	sprintf (defaultName, "%s.wav", my longSound.data ? my longSound.data -> name : my sound.data -> name);
DO_WRITE
	if (! do_write (me, file, Melder_WAV)) return 0;
END

FORM_WRITE (SoundEditor, cb_writeNextSun, "Write selection to NeXT/Sun file", 0)
	sprintf (defaultName, "%s.au", my longSound.data ? my longSound.data -> name : my sound.data -> name);
DO_WRITE
	if (! do_write (me, file, Melder_NEXT_SUN)) return 0;
END

FORM_WRITE (SoundEditor, cb_writeNist, "Write selection to NIST file", 0)
	sprintf (defaultName, "%s.nist", my longSound.data ? my longSound.data -> name : my sound.data -> name);
DO_WRITE
	if (! do_write (me, file, Melder_NIST)) return 0;
END

/***** EDIT MENU *****/

DIRECT (SoundEditor, cb_copy)
	Sound publish = my longSound.data ? LongSound_extractPart (my data, my startSelection, my endSelection, FALSE) :
		Sound_extractPart (my data, my startSelection, my endSelection, enumi (Sound_WINDOW, Rectangular), 1.0, FALSE);
	iferror return 0;
	forget (Sound_clipboard);
	Sound_clipboard = publish;
END

DIRECT (SoundEditor, cb_cut)
	Sound sound = my data;
	long i, j, first, last;
	long selectionNumberOfSamples = Sampled_getWindowSamples (sound,
		my startSelection, my endSelection, & first, & last);
	long oldNumberOfSamples = sound -> nx;
	long newNumberOfSamples = oldNumberOfSamples - selectionNumberOfSamples;
	if (newNumberOfSamples < 1)
		return Melder_error ("(SoundEditor_cut:) You cannot cut all of the signal away,\n"
			"because you cannot create a Sound with 0 samples.\n"
			"You could consider using Copy instead.");
	if (selectionNumberOfSamples) {
		float **newData, **oldData = sound -> z, *amp;
		forget (Sound_clipboard);
		Sound_clipboard = Sound_create (0.0, selectionNumberOfSamples * sound -> dx,
						selectionNumberOfSamples, sound -> dx, 0.5 * sound -> dx);
		if (! Sound_clipboard) return 0;
		j = 0;
		amp = Sound_clipboard -> z [1];
		for (i = first; i <= last; i ++)
			amp [++ j] = oldData [1] [i];
		newData = NUMfmatrix (1, 1, 1, newNumberOfSamples);
		j = 0;
		for (i = 1; i < first; i ++)
			newData [1] [++ j] = oldData [1] [i];
		for (i = last + 1; i <= oldNumberOfSamples; i ++)
			newData [1] [++ j] = oldData [1] [i];
		Editor_save (me, "Cut");
		NUMfmatrix_free (oldData, 1, 1);
		sound -> xmin = 0.0;
		sound -> xmax = newNumberOfSamples * sound -> dx;
		sound -> nx = newNumberOfSamples;
		sound -> x1 = 0.5 * sound -> dx;
		sound -> z = newData;

		/* Start updating the markers of the FunctionEditor, respecting the invariants. */

		my tmin = sound -> xmin;
		my tmax = sound -> xmax;

		/* Collapse the selection, */
		/* so that the Cut operation can immediately be undone by a Paste. */
		/* The exact position will be half-way in between two samples. */

		my startSelection = my endSelection = sound -> xmin + (first - 1) * sound -> dx;

		/* Update the window. */
		{
			double t1 = (first - 1) * sound -> dx;
			double t2 = last * sound -> dx;
			double windowLength = my endWindow - my startWindow;   /* > 0 */
			if (t1 > my startWindow)
				if (t2 < my endWindow)
					my startWindow -= 0.5 * (t2 - t1);
				else
					(void) 0;
			else if (t2 < my endWindow)
				my startWindow -= t2 - t1;
			else   /* Cut overlaps entire window: centre. */
				my startWindow = my startSelection - 0.5 * windowLength;
			my endWindow = my startWindow + windowLength;   /* First try. */
			if (my endWindow > my tmax) {
				my startWindow -= my endWindow - my tmax;   /* 2nd try. */
				if (my startWindow < my tmin)
					my startWindow = my tmin;   /* Third try. */
				my endWindow = my tmax;   /* Second try. */
			} else if (my startWindow < my tmin) {
				my endWindow -= my startWindow - my tmin;   /* Second try. */
				if (my endWindow > my tmax)
					my endWindow = my tmax;   /* Third try. */
				my startWindow = my tmin;   /* Second try. */
			}
		}

		/* Force FunctionEditor to show changes. */

		Matrix_getWindowExtrema (sound, 1, sound -> nx, 1, 1, & my minimum, & my maximum);
		FunctionEditor_SoundAnalysis_forget (me);
		FunctionEditor_ungroup (me);
		FunctionEditor_marksChanged (me);
		Editor_broadcastChange (me);
	} else {
		Melder_warning ("No samples selected.");
	}
END

DIRECT (SoundEditor, cb_paste)
	Sound sound = my data;
	long leftSample = Sampled_xToLowIndex (sound, my endSelection);
	long oldNumberOfSamples = sound -> nx, newNumberOfSamples, i, j, n;
	float **newData, **oldData = sound -> z, *amp;
	if (! Sound_clipboard) {
		Melder_warning ("(SoundEditor_paste:) Clipboard is empty; nothing pasted.");
		return 1;
	}
	if (Sound_clipboard -> dx != sound -> dx)
		return Melder_error ("(SoundEditor_paste:) Cannot paste because\n"
 			"sampling frequency of clipboard does not match\n"
			"sampling frequency of edited sound.");
	if (leftSample < 0) leftSample = 0;
	if (leftSample > oldNumberOfSamples) leftSample = oldNumberOfSamples;
	newNumberOfSamples = oldNumberOfSamples + Sound_clipboard -> nx;
	if (! (newData = NUMfmatrix (1, 1, 1, newNumberOfSamples))) return 0;
	j = 0;
	for (i = 1; i <= leftSample; i ++) newData [1] [++ j] = oldData [1] [i];
	n = Sound_clipboard -> nx;
	amp = Sound_clipboard -> z [1];
	for (i = 1; i <= n; i ++)
		newData [1] [++ j] = amp [i];
	for (i = leftSample + 1; i <= oldNumberOfSamples; i ++)
		newData [1] [++ j] = oldData [1] [i];
	Editor_save (me, "Paste");
	NUMfmatrix_free (oldData, 1, 1);
	sound -> xmin = 0.0;
	sound -> xmax = newNumberOfSamples * sound -> dx;
	sound -> nx = newNumberOfSamples;
	sound -> x1 = 0.5 * sound -> dx;
	sound -> z = newData;

	/* Start updating the markers of the FunctionEditor, respecting the invariants. */

	my tmin = sound -> xmin;
 	my tmax = sound -> xmax;
	my startSelection = leftSample * sound -> dx;
	my endSelection = (leftSample + Sound_clipboard -> nx) * sound -> dx;

	/* Force FunctionEditor to show changes. */

	Matrix_getWindowExtrema (sound, 1, sound -> nx, 1, 1, & my minimum, & my maximum);
	FunctionEditor_SoundAnalysis_forget (me);
	FunctionEditor_ungroup (me);
	FunctionEditor_marksChanged (me);
	Editor_broadcastChange (me);
END

DIRECT (SoundEditor, cb_setSelectionToZero)
	Sound sound = my data;
	long first, last, i;
	Sampled_getWindowSamples (sound, my startSelection, my endSelection, & first, & last);
	Editor_save (me, "Set to zero");
	for (i = first; i <= last; i ++)
		sound -> z [1] [i] = 0.0;
	FunctionEditor_SoundAnalysis_forget (me);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (SoundEditor, cb_reverseSelection)
	Editor_save (me, "Reverse selection");
	Sound_reverse (my data, my startSelection, my endSelection);
	FunctionEditor_SoundAnalysis_forget (me);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

/***** QUERY MENU *****/

DIRECT (SoundEditor, cb_settingsReport)
	MelderInfo_open ();
	MelderInfo_writeLine2 ("Data class: ", ((Thing) my data) -> methods -> _className);
	MelderInfo_writeLine2 ("Data name: ", ((Thing) my data) -> name);
	MelderInfo_writeLine3 ("Editor start: ", Melder_double (my tmin), " seconds");
	MelderInfo_writeLine3 ("Editor end: ", Melder_double (my tmax), " seconds");
	MelderInfo_writeLine3 ("Window start: ", Melder_double (my startWindow), " seconds");
	MelderInfo_writeLine3 ("Window end: ", Melder_double (my endWindow), " seconds");
	MelderInfo_writeLine3 ("Selection start: ", Melder_double (my startSelection), " seconds");
	MelderInfo_writeLine3 ("Selection end: ", Melder_double (my endSelection), " seconds");
	/* Sound flag: */
	MelderInfo_writeLine2 ("Sound autoscaling: ", Melder_boolean (my sound.autoscaling));
	/* Spectrogram flag: */
	MelderInfo_writeLine2 ("Spectrogram show: ", Melder_boolean (my spectrogram.show));
	/* Spectrogram settings: */
	MelderInfo_writeLine3 ("Spectrogram view from: ", Melder_double (my spectrogram.viewFrom), " Hertz");
	MelderInfo_writeLine3 ("Spectrogram view to: ", Melder_double (my spectrogram.viewTo), " Hertz");
	MelderInfo_writeLine3 ("Spectrogram window length: ", Melder_double (my spectrogram.windowLength), " seconds");
	MelderInfo_writeLine3 ("Spectrogram dynamic range: ", Melder_double (my spectrogram.dynamicRange), " dB");
	/* Advanced spectrogram settings: */
	MelderInfo_writeLine2 ("Spectrogram number of time steps: ", Melder_integer (my spectrogram.timeSteps));
	MelderInfo_writeLine2 ("Spectrogram number of frequency steps: ", Melder_integer (my spectrogram.frequencySteps));
	MelderInfo_writeLine2 ("Spectrogram method: ", "Fourier");
	MelderInfo_writeLine2 ("Spectrogram window shape: ", Sound_to_Spectrogram_windowShapeText (my spectrogram.windowShape));
	MelderInfo_writeLine2 ("Spectrogram autoscaling: ", Melder_boolean (my spectrogram.autoscaling));
	MelderInfo_writeLine3 ("Spectrogram maximum: ", Melder_double (my spectrogram.maximum), " dB/Hz");
	MelderInfo_writeLine3 ("Spectrogram pre-emphasis: ", Melder_integer (my spectrogram.preemphasis), " dB/octave");
	MelderInfo_writeLine2 ("Spectrogram dynamicCompression: ", Melder_integer (my spectrogram.dynamicCompression));
	/* Dynamic information: */
	MelderInfo_writeLine3 ("Spectrogram cursor frequency: ", Melder_double (my spectrogram.cursor), " Hertz");
	/* Pitch flag: */
	MelderInfo_writeLine2 ("Pitch show: ", Melder_boolean (my pitch.show));
	/* Pitch settings: */
	MelderInfo_writeLine3 ("Pitch floor: ", Melder_double (my pitch.floor), " Hertz");
	MelderInfo_writeLine3 ("Pitch ceiling: ", Melder_double (my pitch.ceiling), " Hertz");
	MelderInfo_writeLine2 ("Pitch unit: ", ClassFunction_getUnitText (classPitch, Pitch_LEVEL_FREQUENCY, my pitch.unit, Function_UNIT_TEXT_MENU));
	/* Advanced pitch settings: */
	MelderInfo_writeLine4 ("Pitch view from: ", Melder_double (my pitch.viewFrom), " ", ClassFunction_getUnitText (classPitch, Pitch_LEVEL_FREQUENCY, my pitch.unit, Function_UNIT_TEXT_MENU));
	MelderInfo_writeLine4 ("Pitch view to: ", Melder_double (my pitch.viewTo), " ", ClassFunction_getUnitText (classPitch, Pitch_LEVEL_FREQUENCY, my pitch.unit, Function_UNIT_TEXT_MENU));
	MelderInfo_writeLine2 ("Pitch method: ", my pitch.method == 1 ? "Autocorrelation" : "Forward cross-correlation");
	MelderInfo_writeLine2 ("Pitch very accurate: ", Melder_boolean (my pitch.veryAccurate));
	MelderInfo_writeLine2 ("Pitch max. number of candidates: ", Melder_integer (my pitch.maximumNumberOfCandidates));
	MelderInfo_writeLine3 ("Pitch silence threshold: ", Melder_double (my pitch.silenceThreshold), " of global peak");
	MelderInfo_writeLine3 ("Pitch voicing threshold: ", Melder_double (my pitch.voicingThreshold), " (periodic power / total power)");
	MelderInfo_writeLine3 ("Pitch octave cost: ", Melder_double (my pitch.octaveCost), " per octave");
	MelderInfo_writeLine3 ("Pitch octave jump cost: ", Melder_double (my pitch.octaveJumpCost), " per octave");
	MelderInfo_writeLine3 ("Pitch voiced/unvoiced cost: ", Melder_double (my pitch.voicedUnvoicedCost), " Hertz");
	/* Intensity flag: */
	MelderInfo_writeLine2 ("Intensity show: ", Melder_boolean (my intensity.show));
	/* Intensity settings: */
	MelderInfo_writeLine3 ("Intensity view from: ", Melder_double (my intensity.viewFrom), " dB");
	MelderInfo_writeLine3 ("Intensity view to: ", Melder_double (my intensity.viewTo), " dB");
	/* Formant flag: */
	MelderInfo_writeLine2 ("Formant show: ", Melder_boolean (my formant.show));
	/* Formant settings: */
	MelderInfo_writeLine3 ("Formant maximum formant: ", Melder_double (my formant.maximumFormant), " Hertz");
	MelderInfo_writeLine2 ("Formant number of poles: ", Melder_integer (my formant.numberOfPoles));
	MelderInfo_writeLine3 ("Formant window length: ", Melder_double (my formant.windowLength), " seconds");
	MelderInfo_writeLine3 ("Formant dynamic range: ", Melder_double (my formant.dynamicRange), " dB");
	MelderInfo_writeLine3 ("Formant dot size: ", Melder_double (my formant.dotSize), " mm");
	/* Advanced formant settings: */
	MelderInfo_writeLine2 ("Formant method: ", "Burg");
	MelderInfo_writeLine3 ("Formant pre-emphasis from: ", Melder_double (my formant.preemphasisFrom), " Hertz");
	/* Pulses flag: */
	MelderInfo_writeLine2 ("Pulses show: ", Melder_boolean (my pulses.show));
	MelderInfo_close ();
END

/***** SELECT MENU *****/

DIRECT (SoundEditor, cb_moveCursorToZero)
	double zero = Sound_getNearestZeroCrossing (my data, 0.5 * (my startSelection + my endSelection));
	if (NUMdefined (zero)) {
		my startSelection = my endSelection = zero;
		FunctionEditor_marksChanged (me);
	}
END
DIRECT (SoundEditor, cb_moveBtoZero)
	double zero = Sound_getNearestZeroCrossing (my data, my startSelection);
	if (NUMdefined (zero)) {
		my startSelection = zero;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		FunctionEditor_marksChanged (me);
	}
END
DIRECT (SoundEditor, cb_moveEtoZero)
	double zero = Sound_getNearestZeroCrossing (my data, my endSelection);
	if (NUMdefined (zero)) {
		my endSelection = zero;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		FunctionEditor_marksChanged (me);
	}
END

/***** HELP MENU *****/

DIRECT (SoundEditor, cb_SoundEditorHelp) Melder_help ("SoundEditor"); END
DIRECT (SoundEditor, cb_LongSoundEditorHelp) Melder_help ("LongSoundEditor"); END

static void createMenus (I) {
	iam (SoundEditor);
	inherited (SoundEditor) createMenus (me);

	Editor_addCommand (me, "File", "Copy to list of objects:", motif_INSENSITIVE, cb_publish /* dummy */);
	my publishPreserveButton = Editor_addCommand (me, "File", "Extract sound selection (preserve times)", 0, cb_publishPreserve);
	Editor_addCommand (me, "File", "Extract selection (preserve times)", Editor_HIDDEN, cb_publishPreserve);
	my publishButton = Editor_addCommand (me, "File", "Extract sound selection (time from 0)", 0, cb_publish);
	Editor_addCommand (me, "File", "Extract selection (time from 0)", Editor_HIDDEN, cb_publish);
	Editor_addCommand (me, "File", "Extract selection", Editor_HIDDEN, cb_publish);
	if (my sound.data) {
		my publishWindowButton = Editor_addCommand (me, "File", "Extract windowed sound selection...", 0, cb_publishWindow);
		Editor_addCommand (me, "File", "Extract windowed selection...", Editor_HIDDEN, cb_publishWindow);
	}
	Editor_addCommand (me, "File", "-- write --", 0, NULL);
	Editor_addCommand (me, "File", "Copy to disk:", motif_INSENSITIVE, cb_publish /* dummy */);
	my writeWavButton = Editor_addCommand (me, "File", "Write sound selection to WAV file...", 0, cb_writeWav);
	Editor_addCommand (me, "File", "Write selection to WAV file...", Editor_HIDDEN, cb_writeWav);
	my writeAiffButton = Editor_addCommand (me, "File", "Write sound selection to AIFF file...", 0, cb_writeAiff);
	Editor_addCommand (me, "File", "Write selection to AIFF file...", Editor_HIDDEN, cb_writeAiff);
	my writeAifcButton = Editor_addCommand (me, "File", "Write sound selection to AIFC file...", 0, cb_writeAifc);
	Editor_addCommand (me, "File", "Write selection to AIFC file...", Editor_HIDDEN, cb_writeAifc);
	my writeNextSunButton = Editor_addCommand (me, "File", "Write sound selection to Next/Sun file...", 0, cb_writeNextSun);
	Editor_addCommand (me, "File", "Write selection to Next/Sun file...", Editor_HIDDEN, cb_writeNextSun);
	my writeNistButton = Editor_addCommand (me, "File", "Write sound selection to NIST file...", 0, cb_writeNist);
	Editor_addCommand (me, "File", "Write selection to NIST file...", Editor_HIDDEN, cb_writeNist);
	Editor_addCommand (me, "File", "-- close --", 0, NULL);

	Editor_addCommand (me, "Edit", "-- cut copy paste --", 0, NULL);
	if (my sound.data) my cutButton = Editor_addCommand (me, "Edit", "Cut", 'X', cb_cut);
	my copyButton = Editor_addCommand (me, "Edit", "Copy selection to Sound clipboard", 'C', cb_copy);
	if (my sound.data) my pasteButton = Editor_addCommand (me, "Edit", "Paste after selection", 'V', cb_paste);
	if (my sound.data) {
		Editor_addCommand (me, "Edit", "-- zero --", 0, NULL);
		my zeroButton = Editor_addCommand (me, "Edit", "Set selection to zero", 0, cb_setSelectionToZero);
		my reverseButton = Editor_addCommand (me, "Edit", "Reverse selection", 'R', cb_reverseSelection);
	}

	FunctionEditor_SoundAnalysis_selectionQueries (me);

	if (my sound.data) {
		Editor_addCommand (me, "Select", "-- move to zero --", 0, 0);
		Editor_addCommand (me, "Select", "Move start of selection to nearest zero crossing", ',', cb_moveBtoZero);
		Editor_addCommand (me, "Select", "Move begin of selection to nearest zero crossing", Editor_HIDDEN, cb_moveBtoZero);
		Editor_addCommand (me, "Select", "Move cursor to nearest zero crossing", '0', cb_moveCursorToZero);
		Editor_addCommand (me, "Select", "Move end of selection to nearest zero crossing", '.', cb_moveEtoZero);
	}

	FunctionEditor_SoundAnalysis_addMenus (me);
	Editor_addCommand (me, "Query", "-- reports --", 0, 0);
	Editor_addCommand (me, "Query", "Settings report", 0, cb_settingsReport);

	Editor_addCommand (me, "Help", "SoundEditor help", '?', cb_SoundEditorHelp);
	Editor_addCommand (me, "Help", "LongSoundEditor help", 0, cb_LongSoundEditorHelp);
}

/********** UPDATE **********/

static void prepareDraw (I) {
	iam (SoundEditor);
	if (my longSound.data) {
		LongSound_haveWindow (my longSound.data, my startWindow, my endWindow);
		Melder_clearError ();
	}
}

static void draw (I) {
	iam (SoundEditor);
	long first, last, selectedSamples;
	Graphics_Viewport viewport;
	int showAnalysis = my spectrogram.show || my pitch.show || my intensity.show || my formant.show;

	/*
	 * We check beforehand whether the window fits the LongSound buffer.
	 */
	if (my longSound.data && my endWindow - my startWindow > my longSound.data -> bufferLength) {
		Graphics_setColour (my graphics, Graphics_WHITE);
		Graphics_setWindow (my graphics, 0, 1, 0, 1);
		Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
		Graphics_setColour (my graphics, Graphics_BLACK);
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_printf (my graphics, 0.5, 0.5, "(window longer than %.7g seconds)", my longSound.data -> bufferLength);
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_TOP);
		Graphics_printf (my graphics, 0.5, 0.5, "(zoom in to see the samples)");
		return;
	}

	/* Draw sound. */

	if (showAnalysis)
		viewport = Graphics_insetViewport (my graphics, 0, 1, 0.5, 1);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	FunctionEditor_Sound_draw (me, my minimum, my maximum);
	Graphics_flushWs (my graphics);
	if (showAnalysis)
		Graphics_resetViewport (my graphics, viewport);

	/* Draw analyses. */

	if (showAnalysis) {
		/* Draw spectrogram, pitch, formants. */
		viewport = Graphics_insetViewport (my graphics, 0, 1, 0, 0.5);
		FunctionEditor_SoundAnalysis_draw (me);
		Graphics_flushWs (my graphics);
		Graphics_resetViewport (my graphics, viewport);
	}

	/* Draw pulses. */

	if (my pulses.show) {
		if (showAnalysis)
			viewport = Graphics_insetViewport (my graphics, 0, 1, 0.5, 1);
		FunctionEditor_SoundAnalysis_drawPulses (me);
		FunctionEditor_Sound_draw (me, my minimum, my maximum);   /* Second time, partially across the pulses. */
		Graphics_flushWs (my graphics);
		if (showAnalysis)
			Graphics_resetViewport (my graphics, viewport);
	}

	/* Update buttons. */

	selectedSamples = Sampled_getWindowSamples (my data, my startSelection, my endSelection, & first, & last);
	XtSetSensitive (my publishButton, selectedSamples != 0);
	XtSetSensitive (my publishPreserveButton, selectedSamples != 0);
	if (my publishWindowButton) XtSetSensitive (my publishWindowButton, selectedSamples != 0);
	XtSetSensitive (my writeAiffButton, selectedSamples != 0);
	XtSetSensitive (my writeAifcButton, selectedSamples != 0);
	XtSetSensitive (my writeWavButton, selectedSamples != 0);
	XtSetSensitive (my writeNextSunButton, selectedSamples != 0);
	XtSetSensitive (my writeNistButton, selectedSamples != 0);
	if (my sound.data) {
		XtSetSensitive (my cutButton, selectedSamples != 0 && selectedSamples < my sound.data -> nx);
		XtSetSensitive (my copyButton, selectedSamples != 0);
		XtSetSensitive (my zeroButton, selectedSamples != 0);
		XtSetSensitive (my reverseButton, selectedSamples != 0);
	}
}

static void play (I, double tmin, double tmax) {
	iam (SoundEditor);
	if (my longSound.data)
		LongSound_playPart (my data, tmin, tmax, our playCallback, me);
	else
		Sound_playPart (my data, tmin, tmax, our playCallback, me);
}

static void dataChanged (I) {
	iam (SoundEditor);
	Sound sound = my data;
	Melder_assert (sound != NULL);   /* LongSound objects should not get dataChanged messages. */
	Matrix_getWindowExtrema (sound, 1, sound -> nx, 1, 1, & my minimum, & my maximum);
	FunctionEditor_SoundAnalysis_forget (me);
	inherited (SoundEditor) dataChanged (me);
}

static int click (I, double xWC, double yWC, int shiftKeyPressed) {
	iam (SoundEditor);
	if ((my spectrogram.show || my formant.show) && yWC < 0.5) {
		my spectrogram.cursor = my spectrogram.viewFrom +
			2 * yWC * (my spectrogram.viewTo - my spectrogram.viewFrom);
	}
	return inherited (SoundEditor) click (me, xWC, yWC, shiftKeyPressed);   /* Drag & update. */
}

static void viewMenuEntries (I) {
	iam (SoundEditor);
	FunctionEditor_Sound_createMenus (me);
	FunctionEditor_SoundAnalysis_viewMenus (me);
}

static void highlightSelection (I, double left, double right, double bottom, double top) {
	iam (SoundEditor);
	if (my spectrogram.show)
		Graphics_highlight (my graphics, left, right, 0.5 * (bottom + top), top);
	else
		Graphics_highlight (my graphics, left, right, bottom, top);
}

static void unhighlightSelection (I, double left, double right, double bottom, double top) {
	iam (SoundEditor);
	if (my spectrogram.show)
		Graphics_unhighlight (my graphics, left, right, 0.5 * (bottom + top), top);
	else
		Graphics_unhighlight (my graphics, left, right, bottom, top);
}

class_methods (SoundEditor, FunctionEditor)
	class_method (destroy)
	class_method (createMenus)
	class_method (dataChanged)
	class_method (prepareDraw)
	class_method (draw)
	class_method (play)
	class_method (click)
	class_method (viewMenuEntries)
	class_method (highlightSelection)
	class_method (unhighlightSelection)
class_methods_end

SoundEditor SoundEditor_create (Widget parent, const char *title, Any data) {
	SoundEditor me = new (SoundEditor);
	if (Thing_member (data, classLongSound))
		my longSound.data = data;
	else if (Thing_member (data, classSound))
		my sound.data = data;
	if (! me || ! FunctionEditor_init (me, parent, title, data))
		return NULL;
	if (my longSound.data)
		my minimum = -1, my maximum = 1;
	else
		Matrix_getWindowExtrema (data, 1, my sound.data -> nx, 1, 1, & my minimum, & my maximum);
	FunctionEditor_Sound_init (me);
	FunctionEditor_SoundAnalysis_init (me);
	my publish.windowType = preferences.publish.windowType;
	my publish.relativeWidth = preferences.publish.relativeWidth;
	my publish.preserveTimes = preferences.publish.preserveTimes;
	if (my longSound.data && my endWindow - my startWindow > 30.0) {
		my endWindow = my startWindow + 30.0;
		FunctionEditor_marksChanged (me);
	}
	return me;
}

/* End of file SoundEditor.c */
