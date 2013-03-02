/* SoundEditor.cpp
 *
 * Copyright (C) 1992-2012,2013 Paul Boersma, 2007 Erez Volk (FLAC support)
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

#include "SoundEditor.h"
#include "Sound_and_Spectrogram.h"
#include "Pitch.h"
#include "Preferences.h"
#include "EditorM.h"

Thing_implement (SoundEditor, TimeSoundAnalysisEditor, 0);

/********** METHODS **********/

void structSoundEditor :: v_dataChanged () {
	Sound sound = (Sound) data;
	Melder_assert (sound != NULL);   // LongSound objects should not get v_dataChanged messages
	Matrix_getWindowExtrema (sound, 1, sound -> nx, 1, sound -> ny, & d_sound.minimum, & d_sound.maximum);   // BUG unreadable
	v_destroy_analysis ();
	SoundEditor_Parent :: v_dataChanged ();
}

/***** EDIT MENU *****/

static void menu_cb_Copy (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	try {
		/*
		 * Create without change.
		 */
		autoSound publish = my d_longSound.data ? LongSound_extractPart ((LongSound) my data, my d_startSelection, my d_endSelection, FALSE) :
			Sound_extractPart ((Sound) my data, my d_startSelection, my d_endSelection, kSound_windowShape_RECTANGULAR, 1.0, FALSE);
		/*
		 * Change without error.
		 */
		forget (Sound_clipboard);
		Sound_clipboard = publish.transfer();
	} catch (MelderError) {
		Melder_throw ("Sound selection not copied to clipboard.");
	}
}

static void menu_cb_Cut (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	try {
		Sound sound = (Sound) my data;
		long first, last, selectionNumberOfSamples = Sampled_getWindowSamples (sound,
			my d_startSelection, my d_endSelection, & first, & last);
		long oldNumberOfSamples = sound -> nx;
		long newNumberOfSamples = oldNumberOfSamples - selectionNumberOfSamples;
		if (newNumberOfSamples < 1)
			Melder_throw ("You cannot cut all of the signal away,\n"
				"because you cannot create a Sound with 0 samples.\n"
				"You could consider using Copy instead.");
		if (selectionNumberOfSamples) {
			double **oldData = sound -> z;
			/*
			 * Create without change.
			 */
			autoSound publish = Sound_create (sound -> ny, 0.0, selectionNumberOfSamples * sound -> dx,
							selectionNumberOfSamples, sound -> dx, 0.5 * sound -> dx);
			for (long channel = 1; channel <= sound -> ny; channel ++) {
				long j = 0;
				for (long i = first; i <= last; i ++) {
					publish -> z [channel] [++ j] = oldData [channel] [i];
				}
			}
			autoNUMmatrix <double> newData (1, sound -> ny, 1, newNumberOfSamples);
			for (long channel = 1; channel <= sound -> ny; channel ++) {
				long j = 0;
				for (long i = 1; i < first; i ++) {
					newData [channel] [++ j] = oldData [channel] [i];
				}
				for (long i = last + 1; i <= oldNumberOfSamples; i ++) {
					newData [channel] [++ j] = oldData [channel] [i];
				}
			}
			Editor_save (me, L"Cut");
			/*
			 * Change without error.
			 */
			NUMmatrix_free <double> (oldData, 1, 1);
			sound -> xmin = 0.0;
			sound -> xmax = newNumberOfSamples * sound -> dx;
			sound -> nx = newNumberOfSamples;
			sound -> x1 = 0.5 * sound -> dx;
			sound -> z = newData.transfer();
			forget (Sound_clipboard);
			Sound_clipboard = publish.transfer();

			/* Start updating the markers of the FunctionEditor, respecting the invariants. */

			my d_tmin = sound -> xmin;
			my d_tmax = sound -> xmax;

			/* Collapse the selection, */
			/* so that the Cut operation can immediately be undone by a Paste. */
			/* The exact position will be half-way in between two samples. */

			my d_startSelection = my d_endSelection = sound -> xmin + (first - 1) * sound -> dx;

			/* Update the window. */
			{
				double t1 = (first - 1) * sound -> dx;
				double t2 = last * sound -> dx;
				double windowLength = my d_endWindow - my d_startWindow;   // > 0
				if (t1 > my d_startWindow)
					if (t2 < my d_endWindow)
						my d_startWindow -= 0.5 * (t2 - t1);
					else
						(void) 0;
				else if (t2 < my d_endWindow)
					my d_startWindow -= t2 - t1;
				else   /* Cut overlaps entire window: centre. */
					my d_startWindow = my d_startSelection - 0.5 * windowLength;
				my d_endWindow = my d_startWindow + windowLength;   // first try
				if (my d_endWindow > my d_tmax) {
					my d_startWindow -= my d_endWindow - my d_tmax;   // second try
					if (my d_startWindow < my d_tmin)
						my d_startWindow = my d_tmin;   // third try
					my d_endWindow = my d_tmax;   // second try
				} else if (my d_startWindow < my d_tmin) {
					my d_endWindow -= my d_startWindow - my d_tmin;   // second try
					if (my d_endWindow > my d_tmax)
						my d_endWindow = my d_tmax;   // third try
					my d_startWindow = my d_tmin;   // second try
				}
			}

			/* Force FunctionEditor to show changes. */

			Matrix_getWindowExtrema (sound, 1, sound -> nx, 1, sound -> ny, & my d_sound.minimum, & my d_sound.maximum);
			my v_destroy_analysis ();
			FunctionEditor_ungroup (me);
			FunctionEditor_marksChanged (me, false);
			my broadcastDataChanged ();
		} else {
			Melder_warning (L"No samples selected.");
		}
	} catch (MelderError) {
		Melder_throw ("Sound selection not cut to clipboard.");
	}
}

static void menu_cb_Paste (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	Sound sound = (Sound) my data;
	long leftSample = Sampled_xToLowIndex (sound, my d_endSelection);
	long oldNumberOfSamples = sound -> nx, newNumberOfSamples;
	double **oldData = sound -> z;
	if (! Sound_clipboard) {
		Melder_warning (L"Clipboard is empty; nothing pasted.");
		return;
	}
	if (Sound_clipboard -> ny != sound -> ny)
		Melder_throw ("Cannot paste, because\n"
			"the number of channels of the clipboard is not equal to\n"
			"the number of channels of the edited sound.");
	if (Sound_clipboard -> dx != sound -> dx)
		Melder_throw ("Cannot paste, because\n"
			"the sampling frequency of the clipboard is not equal to\n"
			"the sampling frequency of the edited sound.");
	if (leftSample < 0) leftSample = 0;
	if (leftSample > oldNumberOfSamples) leftSample = oldNumberOfSamples;
	newNumberOfSamples = oldNumberOfSamples + Sound_clipboard -> nx;
	/*
	 * Check without change.
	 */
	autoNUMmatrix <double> newData (1, sound -> ny, 1, newNumberOfSamples);
	for (long channel = 1; channel <= sound -> ny; channel ++) {
		long j = 0;
		for (long i = 1; i <= leftSample; i ++) {
			newData [channel] [++ j] = oldData [channel] [i];
		}
		for (long i = 1; i <= Sound_clipboard -> nx; i ++) {
			newData [channel] [++ j] = Sound_clipboard -> z [channel] [i];
		}
		for (long i = leftSample + 1; i <= oldNumberOfSamples; i ++) {
			newData [channel] [++ j] = oldData [channel] [i];
		}
	}
	Editor_save (me, L"Paste");
	/*
	 * Change without error.
	 */
	NUMmatrix_free <double> (oldData, 1, 1);
	sound -> xmin = 0.0;
	sound -> xmax = newNumberOfSamples * sound -> dx;
	sound -> nx = newNumberOfSamples;
	sound -> x1 = 0.5 * sound -> dx;
	sound -> z = newData.transfer();

	/* Start updating the markers of the FunctionEditor, respecting the invariants. */

	my d_tmin = sound -> xmin;
	my d_tmax = sound -> xmax;
	my d_startSelection = leftSample * sound -> dx;
	my d_endSelection = (leftSample + Sound_clipboard -> nx) * sound -> dx;

	/* Force FunctionEditor to show changes. */

	Matrix_getWindowExtrema (sound, 1, sound -> nx, 1, sound -> ny, & my d_sound.minimum, & my d_sound.maximum);
	my v_destroy_analysis ();
	FunctionEditor_ungroup (me);
	FunctionEditor_marksChanged (me, false);
	my broadcastDataChanged ();
}

static void menu_cb_SetSelectionToZero (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	Sound sound = (Sound) my data;
	long first, last;
	Sampled_getWindowSamples (sound, my d_startSelection, my d_endSelection, & first, & last);
	Editor_save (me, L"Set to zero");
	for (long channel = 1; channel <= sound -> ny; channel ++) {
		for (long i = first; i <= last; i ++) {
			sound -> z [channel] [i] = 0.0;
		}
	}
	my v_destroy_analysis ();
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_ReverseSelection (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	Editor_save (me, L"Reverse selection");
	Sound_reverse ((Sound) my data, my d_startSelection, my d_endSelection);
	my v_destroy_analysis ();
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

/***** SELECT MENU *****/

static void menu_cb_MoveCursorToZero (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	double zero = Sound_getNearestZeroCrossing ((Sound) my data, 0.5 * (my d_startSelection + my d_endSelection), 1);   // STEREO BUG
	if (NUMdefined (zero)) {
		my d_startSelection = my d_endSelection = zero;
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_MoveBtoZero (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	double zero = Sound_getNearestZeroCrossing ((Sound) my data, my d_startSelection, 1);   // STEREO BUG
	if (NUMdefined (zero)) {
		my d_startSelection = zero;
		if (my d_startSelection > my d_endSelection) {
			double dummy = my d_startSelection;
			my d_startSelection = my d_endSelection;
			my d_endSelection = dummy;
		}
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_MoveEtoZero (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	double zero = Sound_getNearestZeroCrossing ((Sound) my data, my d_endSelection, 1);   // STEREO BUG
	if (NUMdefined (zero)) {
		my d_endSelection = zero;
		if (my d_startSelection > my d_endSelection) {
			double dummy = my d_startSelection;
			my d_startSelection = my d_endSelection;
			my d_endSelection = dummy;
		}
		FunctionEditor_marksChanged (me, true);
	}
}

/***** HELP MENU *****/

static void menu_cb_SoundEditorHelp (EDITOR_ARGS) { EDITOR_IAM (SoundEditor); Melder_help (L"SoundEditor"); }
static void menu_cb_LongSoundEditorHelp (EDITOR_ARGS) { EDITOR_IAM (SoundEditor); Melder_help (L"LongSoundEditor"); }

void structSoundEditor :: v_createMenus () {
	SoundEditor_Parent :: v_createMenus ();
	Melder_assert (data != NULL);
	Melder_assert (d_sound.data != NULL || d_longSound.data != NULL);

	Editor_addCommand (this, L"Edit", L"-- cut copy paste --", 0, NULL);
	if (d_sound.data) cutButton = Editor_addCommand (this, L"Edit", L"Cut", 'X', menu_cb_Cut);
	copyButton = Editor_addCommand (this, L"Edit", L"Copy selection to Sound clipboard", 'C', menu_cb_Copy);
	if (d_sound.data) pasteButton = Editor_addCommand (this, L"Edit", L"Paste after selection", 'V', menu_cb_Paste);
	if (d_sound.data) {
		Editor_addCommand (this, L"Edit", L"-- zero --", 0, NULL);
		zeroButton = Editor_addCommand (this, L"Edit", L"Set selection to zero", 0, menu_cb_SetSelectionToZero);
		reverseButton = Editor_addCommand (this, L"Edit", L"Reverse selection", 'R', menu_cb_ReverseSelection);
	}

	if (d_sound.data) {
		Editor_addCommand (this, L"Select", L"-- move to zero --", 0, 0);
		Editor_addCommand (this, L"Select", L"Move start of selection to nearest zero crossing", ',', menu_cb_MoveBtoZero);
		Editor_addCommand (this, L"Select", L"Move begin of selection to nearest zero crossing", Editor_HIDDEN, menu_cb_MoveBtoZero);
		Editor_addCommand (this, L"Select", L"Move cursor to nearest zero crossing", '0', menu_cb_MoveCursorToZero);
		Editor_addCommand (this, L"Select", L"Move end of selection to nearest zero crossing", '.', menu_cb_MoveEtoZero);
	}

	v_createMenus_analysis ();
}

void structSoundEditor :: v_createHelpMenuItems (EditorMenu menu) {
	SoundEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"SoundEditor help", '?', menu_cb_SoundEditorHelp);
	EditorMenu_addCommand (menu, L"LongSoundEditor help", 0, menu_cb_LongSoundEditorHelp);
}

/********** UPDATE **********/

void structSoundEditor :: v_prepareDraw () {
	if (d_longSound.data) {
		try {
			LongSound_haveWindow (d_longSound.data, d_startWindow, d_endWindow);
		} catch (MelderError) {
			Melder_clearError ();
		}
	}
}

void structSoundEditor :: v_draw () {
	Sampled data = (Sampled) this -> data;
	Graphics_Viewport viewport;
	bool showAnalysis = p_spectrogram_show || p_pitch_show || p_intensity_show || p_formant_show;
	Melder_assert (data != NULL);
	Melder_assert (d_sound.data != NULL || d_longSound.data != NULL);

	/*
	 * We check beforehand whether the window fits the LongSound buffer.
	 */
	if (d_longSound.data && d_endWindow - d_startWindow > d_longSound.data -> bufferLength) {
		Graphics_setColour (d_graphics, Graphics_WHITE);
		Graphics_setWindow (d_graphics, 0, 1, 0, 1);
		Graphics_fillRectangle (d_graphics, 0, 1, 0, 1);
		Graphics_setColour (d_graphics, Graphics_BLACK);
		Graphics_setTextAlignment (d_graphics, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text3 (d_graphics, 0.5, 0.5, L"(window longer than ", Melder_float (Melder_single (d_longSound.data -> bufferLength)), L" seconds)");
		Graphics_setTextAlignment (d_graphics, Graphics_CENTRE, Graphics_TOP);
		Graphics_text1 (d_graphics, 0.5, 0.5, L"(zoom in to see the samples)");
		return;
	}

	/* Draw sound. */

	if (showAnalysis)
		viewport = Graphics_insetViewport (d_graphics, 0, 1, 0.5, 1);
	Graphics_setColour (d_graphics, Graphics_WHITE);
	Graphics_setWindow (d_graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (d_graphics, 0, 1, 0, 1);
	f_drawSound (d_sound.minimum, d_sound.maximum);
	Graphics_flushWs (d_graphics);
	if (showAnalysis)
		Graphics_resetViewport (d_graphics, viewport);

	/* Draw analyses. */

	if (showAnalysis) {
		/* Draw spectrogram, pitch, formants. */
		viewport = Graphics_insetViewport (d_graphics, 0, 1, 0, 0.5);
		v_draw_analysis ();
		Graphics_flushWs (d_graphics);
		Graphics_resetViewport (d_graphics, viewport);
	}

	/* Draw pulses. */

	if (p_pulses_show) {
		if (showAnalysis)
			viewport = Graphics_insetViewport (d_graphics, 0, 1, 0.5, 1);
		v_draw_analysis_pulses ();
		f_drawSound (d_sound.minimum, d_sound.maximum);   // second time, partially across the pulses
		Graphics_flushWs (d_graphics);
		if (showAnalysis)
			Graphics_resetViewport (d_graphics, viewport);
	}

	/* Update buttons. */

	long first, last;
	long selectedSamples = Sampled_getWindowSamples (data, d_startSelection, d_endSelection, & first, & last);
	v_updateMenuItems_file ();
	if (d_sound.data) {
		cutButton     -> f_setSensitive (selectedSamples != 0 && selectedSamples < d_sound.data -> nx);
		copyButton    -> f_setSensitive (selectedSamples != 0);
		zeroButton    -> f_setSensitive (selectedSamples != 0);
		reverseButton -> f_setSensitive (selectedSamples != 0);
	}
}

void structSoundEditor :: v_play (double a_tmin, double a_tmax) {
	if (d_longSound.data)
		LongSound_playPart ((LongSound) data, a_tmin, a_tmax, theFunctionEditor_playCallback, this);
	else
		Sound_playPart ((Sound) data, a_tmin, a_tmax, theFunctionEditor_playCallback, this);
}

int structSoundEditor :: v_click (double xWC, double yWC, bool shiftKeyPressed) {
	if ((p_spectrogram_show || p_formant_show) && yWC < 0.5 && xWC > d_startWindow && xWC < d_endWindow) {
		d_spectrogram_cursor = p_spectrogram_viewFrom +
			2 * yWC * (p_spectrogram_viewTo - p_spectrogram_viewFrom);
	}
	return SoundEditor_Parent :: v_click (xWC, yWC, shiftKeyPressed);   // drag & update
}

void structSoundEditor :: v_highlightSelection (double left, double right, double bottom, double top) {
	if (p_spectrogram_show)
		Graphics_highlight (d_graphics, left, right, 0.5 * (bottom + top), top);
	else
		Graphics_highlight (d_graphics, left, right, bottom, top);
}

void structSoundEditor :: v_unhighlightSelection (double left, double right, double bottom, double top) {
	if (p_spectrogram_show)
		Graphics_unhighlight (d_graphics, left, right, 0.5 * (bottom + top), top);
	else
		Graphics_unhighlight (d_graphics, left, right, bottom, top);
}

void structSoundEditor :: f_init (const wchar_t *title, Sampled data) {
	/*
	 * my longSound.data or my sound.data have to be set before we call FunctionEditor_init,
	 * because createMenus expect that one of them is not NULL.
	 */
	structTimeSoundAnalysisEditor :: f_init (title, data, data, false);
	if (d_longSound.data && d_endWindow - d_startWindow > 30.0) {
		d_endWindow = d_startWindow + 30.0;
		if (d_startWindow == d_tmin)
			d_startSelection = d_endSelection = 0.5 * (d_startWindow + d_endWindow);
		FunctionEditor_marksChanged (this, false);
	}
}

SoundEditor SoundEditor_create (const wchar_t *title, Sampled data) {
	Melder_assert (data != NULL);
	try {
		autoSoundEditor me = Thing_new (SoundEditor);
		me -> f_init (title, data);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Sound window not created.");
	}
}

/* End of file SoundEditor.cpp */
