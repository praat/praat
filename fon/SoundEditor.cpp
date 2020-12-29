/* SoundEditor.cpp
 *
 * Copyright (C) 1992-2020 Paul Boersma, 2007 Erez Volk (FLAC support)
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

#include "SoundEditor.h"
#include "Sound_and_Spectrogram.h"
#include "Pitch.h"
#include "Sound_and_MixingMatrix.h"
#include "EditorM.h"

Thing_implement (SoundEditor, TimeSoundAnalysisEditor, 0);

/********** METHODS **********/

void structSoundEditor :: v_dataChanged () {
	Sound sound = (Sound) data;
	Melder_assert (sound);
	if (sound -> classInfo == classSound)   // LongSound editors can get spurious v_dataChanged messages (e.g. in a TextGrid editor)
		Matrix_getWindowExtrema (sound, 1, sound -> nx, 1, sound -> ny, & d_sound.minimum, & d_sound.maximum);   // BUG unreadable
	v_reset_analysis ();
	SoundEditor_Parent :: v_dataChanged ();
}

/***** EDIT MENU *****/

static void menu_cb_Copy (SoundEditor me, EDITOR_ARGS_DIRECT) {
	try {
		Sound_clipboard = my d_longSound.data ?
				LongSound_extractPart ((LongSound) my data, my startSelection, my endSelection, false) :
				Sound_extractPart ((Sound) my data, my startSelection, my endSelection, kSound_windowShape::RECTANGULAR, 1.0, false);
	} catch (MelderError) {
		Melder_throw (U"Sound selection not copied to clipboard.");
	}
}

static void menu_cb_Cut (SoundEditor me, EDITOR_ARGS_DIRECT) {
	try {
		Sound sound = (Sound) my data;
		integer first, last, selectionNumberOfSamples = Sampled_getWindowSamples (sound,
				my startSelection, my endSelection, & first, & last);
		integer oldNumberOfSamples = sound -> nx;
		integer newNumberOfSamples = oldNumberOfSamples - selectionNumberOfSamples;
		if (newNumberOfSamples < 1)
			Melder_throw (U"You cannot cut all of the signal away,\n"
				U"because you cannot create a Sound with 0 samples.\n"
				U"You could consider using Copy instead.");
		if (selectionNumberOfSamples) {
			/*
				Create without change.
			*/
			autoSound publish = Sound_create (sound -> ny, 0.0, selectionNumberOfSamples * sound -> dx,
							selectionNumberOfSamples, sound -> dx, 0.5 * sound -> dx);
			for (integer channel = 1; channel <= sound -> ny; channel ++) {
				integer j = 0;
				for (integer i = first; i <= last; i ++)
					publish -> z [channel] [++ j] = sound -> z [channel] [i];
			}
			autoMAT newData = raw_MAT (sound -> ny, newNumberOfSamples);
			for (integer channel = 1; channel <= sound -> ny; channel ++) {
				integer j = 0;
				for (integer i = 1; i < first; i ++)
					newData [channel] [++ j] = sound -> z [channel] [i];
				for (integer i = last + 1; i <= oldNumberOfSamples; i ++)
					newData [channel] [++ j] = sound -> z [channel] [i];
				Melder_assert (j == newData.ncol);
			}
			Editor_save (me, U"Cut");
			/*
				Change without error.
			*/
			sound -> xmin = 0.0;
			sound -> xmax = newNumberOfSamples * sound -> dx;
			sound -> nx = newNumberOfSamples;
			sound -> x1 = 0.5 * sound -> dx;
			sound -> z = newData.move();
			Sound_clipboard = publish.move();

			/*
				Start updating the markers of the FunctionEditor, respecting the invariants.
			*/
			my tmin = sound -> xmin;
			my tmax = sound -> xmax;

			/*
				Collapse the selection,
				so that the Cut operation can immediately be undone by a Paste.
				The exact position will be half-way in between two samples.
			*/
			my startSelection = my endSelection = sound -> xmin + (first - 1) * sound -> dx;

			/*
				Update the window.
			*/
			{
				double t1 = (first - 1) * sound -> dx;
				double t2 = last * sound -> dx;
				double windowLength = my endWindow - my startWindow;   // > 0
				if (t1 > my startWindow)
					if (t2 < my endWindow)
						my startWindow -= 0.5 * (t2 - t1);
					else
						(void) 0;
				else if (t2 < my endWindow)
					my startWindow -= t2 - t1;
				else   /* Cut overlaps entire window: centre. */
					my startWindow = my startSelection - 0.5 * windowLength;
				my endWindow = my startWindow + windowLength;   // first try
				if (my endWindow > my tmax) {
					my startWindow -= my endWindow - my tmax;   // second try
					if (my startWindow < my tmin)
						my startWindow = my tmin;   // third try
					my endWindow = my tmax;   // second try
				} else if (my startWindow < my tmin) {
					my endWindow -= my startWindow - my tmin;   // second try
					if (my endWindow > my tmax)
						my endWindow = my tmax;   // third try
					my startWindow = my tmin;   // second try
				}
			}

			/*
				Force FunctionEditor to show changes.
			*/
			Matrix_getWindowExtrema (sound, 1, sound -> nx, 1, sound -> ny, & my d_sound.minimum, & my d_sound.maximum);
			my v_reset_analysis ();
			FunctionEditor_ungroup (me);
			FunctionEditor_marksChanged (me, false);
			Editor_broadcastDataChanged (me);
		} else {
			Melder_warning (U"No samples selected.");
		}
	} catch (MelderError) {
		Melder_throw (U"Sound selection not cut to clipboard.");
	}
}

static void menu_cb_Paste (SoundEditor me, EDITOR_ARGS_DIRECT) {
	Sound sound = (Sound) my data;
	integer leftSample = Sampled_xToLowIndex (sound, my endSelection);
	integer oldNumberOfSamples = sound -> nx, newNumberOfSamples;
	if (! Sound_clipboard) {
		Melder_warning (U"Clipboard is empty; nothing pasted.");
		return;
	}
	Melder_require (Sound_clipboard -> ny == sound -> ny,
		U"Cannot paste, because\n"
		U"the number of channels of the clipboard is not equal to\n"
		U"the number of channels of the edited sound."
	);
	Melder_require (Sound_clipboard -> dx == sound -> dx,
		U"Cannot paste, because\n"
		U"the sampling frequency of the clipboard is not equal to\n"
		U"the sampling frequency of the edited sound."
	);
	Melder_clip (0_integer, & leftSample, oldNumberOfSamples);
	newNumberOfSamples = oldNumberOfSamples + Sound_clipboard -> nx;
	/*
		Check without change.
	*/
	autoMAT newData = raw_MAT (sound -> ny, newNumberOfSamples);
	for (integer channel = 1; channel <= sound -> ny; channel ++) {
		integer j = 0;
		for (integer i = 1; i <= leftSample; i ++)
			newData [channel] [++ j] = sound -> z [channel] [i];
		for (integer i = 1; i <= Sound_clipboard -> nx; i ++)
			newData [channel] [++ j] = Sound_clipboard -> z [channel] [i];
		for (integer i = leftSample + 1; i <= oldNumberOfSamples; i ++)
			newData [channel] [++ j] = sound -> z [channel] [i];
		Melder_assert (j == newData.ncol);
	}
	Editor_save (me, U"Paste");
	/*
		Change without error.
	*/
	sound -> xmin = 0.0;
	sound -> xmax = newNumberOfSamples * sound -> dx;
	sound -> nx = newNumberOfSamples;
	sound -> x1 = 0.5 * sound -> dx;
	sound -> z = newData.move();

	/*
		Start updating the markers of the FunctionEditor, respecting the invariants.
	*/
	my tmin = sound -> xmin;
	my tmax = sound -> xmax;
	Melder_clipLeft (my tmin, & my startWindow);
	Melder_clipRight (& my endWindow, my tmax);
	my startSelection = leftSample * sound -> dx;
	my endSelection = (leftSample + Sound_clipboard -> nx) * sound -> dx;

	/*
		Force FunctionEditor to show changes.
	*/
	Matrix_getWindowExtrema (sound, 1, sound -> nx, 1, sound -> ny, & my d_sound.minimum, & my d_sound.maximum);
	my v_reset_analysis ();
	FunctionEditor_ungroup (me);
	FunctionEditor_marksChanged (me, false);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_SetSelectionToZero (SoundEditor me, EDITOR_ARGS_DIRECT) {
	Sound sound = (Sound) my data;
	integer first, last;
	Sampled_getWindowSamples (sound, my startSelection, my endSelection, & first, & last);
	Editor_save (me, U"Set to zero");
	sound -> z.verticalBand	(first, last)  <<=  0.0;
	my v_reset_analysis ();
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_ReverseSelection (SoundEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Reverse selection");
	Sound_reverse ((Sound) my data, my startSelection, my endSelection);
	my v_reset_analysis ();
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

/***** SELECT MENU *****/

static void menu_cb_MoveCursorToZero (SoundEditor me, EDITOR_ARGS_DIRECT) {
	const double zero = Sound_getNearestZeroCrossing ((Sound) my data, 0.5 * (my startSelection + my endSelection), 1);   // STEREO BUG
	if (isdefined (zero)) {
		my startSelection = my endSelection = zero;
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_MoveBtoZero (SoundEditor me, EDITOR_ARGS_DIRECT) {
	const double zero = Sound_getNearestZeroCrossing ((Sound) my data, my startSelection, 1);   // STEREO BUG
	if (isdefined (zero)) {
		my startSelection = zero;
		Melder_sort (& my startSelection, & my endSelection);
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_MoveEtoZero (SoundEditor me, EDITOR_ARGS_DIRECT) {
	double zero = Sound_getNearestZeroCrossing ((Sound) my data, my endSelection, 1);   // STEREO BUG
	if (isdefined (zero)) {
		my endSelection = zero;
		Melder_sort (& my startSelection, & my endSelection);
		FunctionEditor_marksChanged (me, true);
	}
}

/***** HELP MENU *****/

static void menu_cb_SoundEditorHelp (SoundEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"SoundEditor"); }
static void menu_cb_LongSoundEditorHelp (SoundEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"LongSoundEditor"); }

void structSoundEditor :: v_createMenus () {
	SoundEditor_Parent :: v_createMenus ();
	Melder_assert (data);
	Melder_assert (d_sound.data || d_longSound.data);

	Editor_addCommand (this, U"Edit", U"-- cut copy paste --", 0, nullptr);
	if (d_sound.data)
		cutButton = Editor_addCommand (this, U"Edit", U"Cut", 'X', menu_cb_Cut);
	copyButton = Editor_addCommand (this, U"Edit", U"Copy selection to Sound clipboard", 'C', menu_cb_Copy);
	if (d_sound.data)
		pasteButton = Editor_addCommand (this, U"Edit", U"Paste after selection", 'V', menu_cb_Paste);
	if (d_sound.data) {
		Editor_addCommand (this, U"Edit", U"-- zero --", 0, nullptr);
		zeroButton = Editor_addCommand (this, U"Edit", U"Set selection to zero", 0, menu_cb_SetSelectionToZero);
		reverseButton = Editor_addCommand (this, U"Edit", U"Reverse selection", 'R', menu_cb_ReverseSelection);
	}
	if (d_sound.data) {
		Editor_addCommand (this, U"Select", U"-- move to zero --", 0, 0);
		Editor_addCommand (this, U"Select", U"Move start of selection to nearest zero crossing", ',', menu_cb_MoveBtoZero);
		Editor_addCommand (this, U"Select", U"Move begin of selection to nearest zero crossing", Editor_HIDDEN, menu_cb_MoveBtoZero);
		Editor_addCommand (this, U"Select", U"Move cursor to nearest zero crossing", '0', menu_cb_MoveCursorToZero);
		Editor_addCommand (this, U"Select", U"Move end of selection to nearest zero crossing", '.', menu_cb_MoveEtoZero);
	}
	v_createMenus_analysis ();
}

void structSoundEditor :: v_createHelpMenuItems (EditorMenu menu) {
	SoundEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"SoundEditor help", '?', menu_cb_SoundEditorHelp);
	EditorMenu_addCommand (menu, U"LongSoundEditor help", 0, menu_cb_LongSoundEditorHelp);
}

/********** UPDATE **********/

void structSoundEditor :: v_prepareDraw () {
	if (d_longSound.data) {
		try {
			LongSound_haveWindow (our d_longSound.data, our startWindow, our endWindow);
		} catch (MelderError) {
			Melder_clearError ();
		}
	}
}

void structSoundEditor :: v_draw () {
	Sampled data = (Sampled) our data;
	Graphics_Viewport viewport;
	bool showAnalysis = our p_spectrogram_show || our p_pitch_show || our p_intensity_show || our p_formant_show;
	Melder_assert (data);
	Melder_assert (our d_sound.data || our d_longSound.data);

	/*
		We check beforehand whether the window fits the LongSound buffer.
	*/
	if (our d_longSound.data && our endWindow - our startWindow > our d_longSound.data -> bufferLength) {
		Graphics_setColour (our graphics.get(), Melder_WHITE);
		Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (our graphics.get(), Melder_BLACK);
		Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (our graphics.get(), 0.5, 0.5,   U"(window longer than ", Melder_float (Melder_single (our d_longSound.data -> bufferLength)), U" seconds)");
		Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_TOP);
		Graphics_text (our graphics.get(), 0.5, 0.5, U"(zoom in to see the samples)");
		return;
	}

	/*
		Draw data.
	*/
	if (showAnalysis)
		viewport = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 0.5, 1.0);
	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	if (p_pulses_show)
		v_draw_analysis_pulses ();
	TimeSoundEditor_drawSound (this, our d_sound.minimum, our d_sound.maximum);
	if (showAnalysis) {
		Graphics_resetViewport (our graphics.get(), viewport);
		viewport = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 0.0, 0.5);
		v_draw_analysis ();
		Graphics_resetViewport (our graphics.get(), viewport);
	}

	/*
		Update buttons.
	*/
	integer first, last;
	integer selectedSamples = Sampled_getWindowSamples (data, our startSelection, our endSelection, & first, & last);
	v_updateMenuItems_file ();
	if (our d_sound.data) {
		GuiThing_setSensitive (cutButton     , selectedSamples != 0 && selectedSamples < our d_sound.data -> nx);
		GuiThing_setSensitive (copyButton    , selectedSamples != 0);
		GuiThing_setSensitive (zeroButton    , selectedSamples != 0);
		GuiThing_setSensitive (reverseButton , selectedSamples != 0);
	}
}

void structSoundEditor :: v_play (double startTime, double endTime) {
	integer numberOfChannels = ( our d_longSound.data ? our d_longSound.data -> numberOfChannels : our d_sound.data -> ny );
	integer numberOfMuteChannels = 0;
	Melder_assert (our d_sound.muteChannels.size == numberOfChannels);
	for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
		if (our d_sound.muteChannels [ichan])
			numberOfMuteChannels ++;
	integer numberOfChannelsToPlay = numberOfChannels - numberOfMuteChannels;
	Melder_require (numberOfChannelsToPlay > 0,
		U"Please select at least one channel to play.");
	if (our d_longSound.data) {
		if (numberOfMuteChannels > 0) {
			autoSound part = LongSound_extractPart (our d_longSound.data, startTime, endTime, 1);
			autoMixingMatrix thee = MixingMatrix_create (numberOfChannelsToPlay, numberOfChannels);
			MixingMatrix_muteAndActivateChannels (thee.get(), our d_sound.muteChannels.get());
			Sound_MixingMatrix_playPart (part.get(), thee.get(), startTime, endTime, theFunctionEditor_playCallback, this);
		} else {
			LongSound_playPart (our d_longSound.data, startTime, endTime, theFunctionEditor_playCallback, this);
		}
	} else {
		if (numberOfMuteChannels > 0) {
			autoMixingMatrix thee = MixingMatrix_create (numberOfChannelsToPlay, numberOfChannels);
			MixingMatrix_muteAndActivateChannels (thee.get(), our d_sound.muteChannels.get());
			Sound_MixingMatrix_playPart (our d_sound.data, thee.get(), startTime, endTime, theFunctionEditor_playCallback, this);
		} else {
			Sound_playPart (our d_sound.data, startTime, endTime, theFunctionEditor_playCallback, this);
		}
	}
}

bool structSoundEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double xWC, double yWC) {
	if ((our p_spectrogram_show || our p_formant_show) && yWC < 0.5 && xWC > our startWindow && xWC < our endWindow)
		our d_spectrogram_cursor = our p_spectrogram_viewFrom +
				2.0 * yWC * (our p_spectrogram_viewTo - our p_spectrogram_viewFrom);
	return SoundEditor_Parent :: v_mouseInWideDataView (event, xWC, yWC);
}

void structSoundEditor :: v_highlightSelection (double left, double right, double bottom, double top) {
	if (our p_spectrogram_show)
		Graphics_highlight (our graphics.get(), left, right, 0.5 * (bottom + top), top);
	else
		Graphics_highlight (our graphics.get(), left, right, bottom, top);
}

void SoundEditor_init (SoundEditor me, conststring32 title, Sampled data) {
	/*
	 * my longSound.data or my sound.data have to be set before we call FunctionEditor_init,
	 * because createMenus expects that one of them is not null.
	 */
	TimeSoundAnalysisEditor_init (me, title, data, data, false);
	if (my d_longSound.data && my endWindow - my startWindow > 30.0) {
		my endWindow = my startWindow + 30.0;
		if (my startWindow == my tmin)
			my startSelection = my endSelection = 0.5 * (my startWindow + my endWindow);
		FunctionEditor_marksChanged (me, false);
	}
}

autoSoundEditor SoundEditor_create (conststring32 title, Sampled data) {
	Melder_assert (data);
	try {
		autoSoundEditor me = Thing_new (SoundEditor);
		SoundEditor_init (me.get(), title, data);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound window not created.");
	}
}

/* End of file SoundEditor.cpp */
