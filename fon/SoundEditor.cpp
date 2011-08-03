/* SoundEditor.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma, 2007 Erez Volk (FLAC support)
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

#undef our
#define our ((SoundEditor_Table) my methods) ->
#define its ((SoundEditor_Table) methods) ->

/********** METHODS **********/

void structSoundEditor :: v_dataChanged () {
	Sound sound = (Sound) data;
	Melder_assert (sound != NULL);   // LongSound objects should not get v_dataChanged messages
	Matrix_getWindowExtrema (sound, 1, sound -> nx, 1, sound -> ny, & this -> sound.minimum, & this -> sound.maximum);   // BUG unreadable
	destroy_analysis ();
	SoundEditor_Parent :: v_dataChanged ();
}

/***** EDIT MENU *****/

static int menu_cb_Copy (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	Sound publish = my longSound.data ? LongSound_extractPart ((LongSound) my data, my startSelection, my endSelection, FALSE) :
		Sound_extractPart ((Sound) my data, my startSelection, my endSelection, kSound_windowShape_RECTANGULAR, 1.0, FALSE);
	forget (Sound_clipboard);
	Sound_clipboard = publish;
	return 1;
}

static int menu_cb_Cut (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	Sound sound = (Sound) my data;
	long first, last, selectionNumberOfSamples = Sampled_getWindowSamples (sound,
		my startSelection, my endSelection, & first, & last);
	long oldNumberOfSamples = sound -> nx;
	long newNumberOfSamples = oldNumberOfSamples - selectionNumberOfSamples;
	if (newNumberOfSamples < 1)
		Melder_throw ("You cannot cut all of the signal away,\n"
			"because you cannot create a Sound with 0 samples.\n"
			"You could consider using Copy instead.");
	if (selectionNumberOfSamples) {
		double **newData, **oldData = sound -> z;
		forget (Sound_clipboard);
		Sound_clipboard = Sound_create (sound -> ny, 0.0, selectionNumberOfSamples * sound -> dx,
						selectionNumberOfSamples, sound -> dx, 0.5 * sound -> dx);
		if (! Sound_clipboard) return 0;
		for (long channel = 1; channel <= sound -> ny; channel ++) {
			long j = 0;
			for (long i = first; i <= last; i ++) {
				Sound_clipboard -> z [channel] [++ j] = oldData [channel] [i];
			}
		}
		newData = NUMmatrix <double> (1, sound -> ny, 1, newNumberOfSamples);   // BUG memory
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
		NUMmatrix_free <double> (oldData, 1, 1);
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

		Matrix_getWindowExtrema (sound, 1, sound -> nx, 1, sound -> ny, & my sound.minimum, & my sound.maximum);
		my destroy_analysis ();
		FunctionEditor_ungroup (me);
		FunctionEditor_marksChanged (me);
		Editor_broadcastChange (me);
	} else {
		Melder_warning1 (L"No samples selected.");
	}
	return 1;
}

static int menu_cb_Paste (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	Sound sound = (Sound) my data;
	long leftSample = Sampled_xToLowIndex (sound, my endSelection);
	long oldNumberOfSamples = sound -> nx, newNumberOfSamples;
	double **oldData = sound -> z;
	if (! Sound_clipboard) {
		Melder_warning1 (L"(SoundEditor_paste:) Clipboard is empty; nothing pasted.");
		return 1;
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

	my tmin = sound -> xmin;
	my tmax = sound -> xmax;
	my startSelection = leftSample * sound -> dx;
	my endSelection = (leftSample + Sound_clipboard -> nx) * sound -> dx;

	/* Force FunctionEditor to show changes. */

	Matrix_getWindowExtrema (sound, 1, sound -> nx, 1, sound -> ny, & my sound.minimum, & my sound.maximum);
	my destroy_analysis ();
	FunctionEditor_ungroup (me);
	FunctionEditor_marksChanged (me);
	Editor_broadcastChange (me);
	return 1;
}

static int menu_cb_SetSelectionToZero (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	Sound sound = (Sound) my data;
	long first, last;
	Sampled_getWindowSamples (sound, my startSelection, my endSelection, & first, & last);
	Editor_save (me, L"Set to zero");
	for (long channel = 1; channel <= sound -> ny; channel ++) {
		for (long i = first; i <= last; i ++) {
			sound -> z [channel] [i] = 0.0;
		}
	}
	my destroy_analysis ();
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
	return 1;
}

static int menu_cb_ReverseSelection (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	Editor_save (me, L"Reverse selection");
	Sound_reverse ((Sound) my data, my startSelection, my endSelection);
	my destroy_analysis ();
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
	return 1;
}

/***** SELECT MENU *****/

static int menu_cb_MoveCursorToZero (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	double zero = Sound_getNearestZeroCrossing ((Sound) my data, 0.5 * (my startSelection + my endSelection), 1);   // STEREO BUG
	if (NUMdefined (zero)) {
		my startSelection = my endSelection = zero;
		FunctionEditor_marksChanged (me);
	}
	return 1;
}

static int menu_cb_MoveBtoZero (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	double zero = Sound_getNearestZeroCrossing ((Sound) my data, my startSelection, 1);   // STEREO BUG
	if (NUMdefined (zero)) {
		my startSelection = zero;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		FunctionEditor_marksChanged (me);
	}
	return 1;
}

static int menu_cb_MoveEtoZero (EDITOR_ARGS) {
	EDITOR_IAM (SoundEditor);
	double zero = Sound_getNearestZeroCrossing ((Sound) my data, my endSelection, 1);   // STEREO BUG
	if (NUMdefined (zero)) {
		my endSelection = zero;
		if (my startSelection > my endSelection) {
			double dummy = my startSelection;
			my startSelection = my endSelection;
			my endSelection = dummy;
		}
		FunctionEditor_marksChanged (me);
	}
	return 1;
}

/***** HELP MENU *****/

static int menu_cb_SoundEditorHelp (EDITOR_ARGS) { EDITOR_IAM (SoundEditor); Melder_help (L"SoundEditor"); return 1; }
static int menu_cb_LongSoundEditorHelp (EDITOR_ARGS) { EDITOR_IAM (SoundEditor); Melder_help (L"LongSoundEditor"); return 1; }

void structSoundEditor :: v_createMenus () {
	SoundEditor_Parent :: v_createMenus ();
	Melder_assert (data != NULL);
	Melder_assert (sound.data != NULL || longSound.data != NULL);

	Editor_addCommand (this, L"Edit", L"-- cut copy paste --", 0, NULL);
	if (sound.data) cutButton = Editor_addCommand (this, L"Edit", L"Cut", 'X', menu_cb_Cut);
	copyButton = Editor_addCommand (this, L"Edit", L"Copy selection to Sound clipboard", 'C', menu_cb_Copy);
	if (sound.data) pasteButton = Editor_addCommand (this, L"Edit", L"Paste after selection", 'V', menu_cb_Paste);
	if (sound.data) {
		Editor_addCommand (this, L"Edit", L"-- zero --", 0, NULL);
		zeroButton = Editor_addCommand (this, L"Edit", L"Set selection to zero", 0, menu_cb_SetSelectionToZero);
		reverseButton = Editor_addCommand (this, L"Edit", L"Reverse selection", 'R', menu_cb_ReverseSelection);
	}

	if (sound.data) {
		Editor_addCommand (this, L"Select", L"-- move to zero --", 0, 0);
		Editor_addCommand (this, L"Select", L"Move start of selection to nearest zero crossing", ',', menu_cb_MoveBtoZero);
		Editor_addCommand (this, L"Select", L"Move begin of selection to nearest zero crossing", Editor_HIDDEN, menu_cb_MoveBtoZero);
		Editor_addCommand (this, L"Select", L"Move cursor to nearest zero crossing", '0', menu_cb_MoveCursorToZero);
		Editor_addCommand (this, L"Select", L"Move end of selection to nearest zero crossing", '.', menu_cb_MoveEtoZero);
	}

	its createMenus_analysis (this);
}

void structSoundEditor :: v_createHelpMenuItems (EditorMenu menu) {
	SoundEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"SoundEditor help", '?', menu_cb_SoundEditorHelp);
	EditorMenu_addCommand (menu, L"LongSoundEditor help", 0, menu_cb_LongSoundEditorHelp);
}

/********** UPDATE **********/

static void prepareDraw (SoundEditor me) {
	if (my longSound.data) {
		try {
			LongSound_haveWindow (my longSound.data, my startWindow, my endWindow);
		} catch (MelderError) {
			Melder_clearError ();
		}
	}
}

static void draw (SoundEditor me) {
	long first, last, selectedSamples;
	Graphics_Viewport viewport;
	int showAnalysis = my spectrogram.show || my pitch.show || my intensity.show || my formant.show;
	Melder_assert (my data != NULL);
	Melder_assert (my sound.data != NULL || my longSound.data != NULL);

	/*
	 * We check beforehand whether the window fits the LongSound buffer.
	 */
	if (my longSound.data && my endWindow - my startWindow > my longSound.data -> bufferLength) {
		Graphics_setColour (my graphics, Graphics_WHITE);
		Graphics_setWindow (my graphics, 0, 1, 0, 1);
		Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
		Graphics_setColour (my graphics, Graphics_BLACK);
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text3 (my graphics, 0.5, 0.5, L"(window longer than ", Melder_float (Melder_single (my longSound.data -> bufferLength)), L" seconds)");
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_TOP);
		Graphics_text1 (my graphics, 0.5, 0.5, L"(zoom in to see the samples)");
		return;
	}

	/* Draw sound. */

	if (showAnalysis)
		viewport = Graphics_insetViewport (my graphics, 0, 1, 0.5, 1);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	TimeSoundEditor_draw_sound (me, my sound.minimum, my sound.maximum);
	Graphics_flushWs (my graphics);
	if (showAnalysis)
		Graphics_resetViewport (my graphics, viewport);

	/* Draw analyses. */

	if (showAnalysis) {
		/* Draw spectrogram, pitch, formants. */
		viewport = Graphics_insetViewport (my graphics, 0, 1, 0, 0.5);
		our draw_analysis (me);
		Graphics_flushWs (my graphics);
		Graphics_resetViewport (my graphics, viewport);
	}

	/* Draw pulses. */

	if (my pulses.show) {
		if (showAnalysis)
			viewport = Graphics_insetViewport (my graphics, 0, 1, 0.5, 1);
		our draw_analysis_pulses (me);
		TimeSoundEditor_draw_sound (me, my sound.minimum, my sound.maximum);   // second time, partially across the pulses
		Graphics_flushWs (my graphics);
		if (showAnalysis)
			Graphics_resetViewport (my graphics, viewport);
	}

	/* Update buttons. */

	selectedSamples = Sampled_getWindowSamples (my data, my startSelection, my endSelection, & first, & last);
	our updateMenuItems_file (me);
	if (my sound.data) {
		GuiObject_setSensitive (my cutButton, selectedSamples != 0 && selectedSamples < my sound.data -> nx);
		GuiObject_setSensitive (my copyButton, selectedSamples != 0);
		GuiObject_setSensitive (my zeroButton, selectedSamples != 0);
		GuiObject_setSensitive (my reverseButton, selectedSamples != 0);
	}
}

static void play (SoundEditor me, double tmin, double tmax) {
	if (my longSound.data)
		LongSound_playPart ((LongSound) my data, tmin, tmax, our playCallback, me);
	else
		Sound_playPart ((Sound) my data, tmin, tmax, our playCallback, me);
}

static int click (SoundEditor me, double xWC, double yWC, int shiftKeyPressed) {
	if ((my spectrogram.show || my formant.show) && yWC < 0.5 && xWC > my startWindow && xWC < my endWindow) {
		my spectrogram.cursor = my spectrogram.viewFrom +
			2 * yWC * (my spectrogram.viewTo - my spectrogram.viewFrom);
	}
	return inherited (SoundEditor) click (me, xWC, yWC, shiftKeyPressed);   /* Drag & update. */
}

static void highlightSelection (SoundEditor me, double left, double right, double bottom, double top) {
	if (my spectrogram.show)
		Graphics_highlight (my graphics, left, right, 0.5 * (bottom + top), top);
	else
		Graphics_highlight (my graphics, left, right, bottom, top);
}

static void unhighlightSelection (SoundEditor me, double left, double right, double bottom, double top) {
	if (my spectrogram.show)
		Graphics_unhighlight (my graphics, left, right, 0.5 * (bottom + top), top);
	else
		Graphics_unhighlight (my graphics, left, right, bottom, top);
}

class_methods (SoundEditor, TimeSoundAnalysisEditor) {
	class_method (prepareDraw)
	class_method (draw)
	class_method (play)
	class_method (click)
	class_method (highlightSelection)
	class_method (unhighlightSelection)
	class_methods_end
}

SoundEditor SoundEditor_create (GuiObject parent, const wchar *title, Function data) {
	Melder_assert (data != NULL);
	try {
		autoSoundEditor me = Thing_new (SoundEditor);
		/*
		 * my longSound.data or my sound.data have to be set before we call FunctionEditor_init,
		 * because createMenus expect that one of them is not NULL.
		 */
		TimeSoundAnalysisEditor_init (me.peek(), parent, title, data, data, false); therror
		if (my longSound.data && my endWindow - my startWindow > 30.0) {
			my endWindow = my startWindow + 30.0;
			if (my startWindow == my tmin)
				my startSelection = my endSelection = 0.5 * (my startWindow + my endWindow);
			FunctionEditor_marksChanged (me.peek());
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Sound window not created.");
	}
}

/* End of file SoundEditor.cpp */
