/* SoundEditor.cpp
 *
 * Copyright (C) 1992-2022 Paul Boersma, 2007 Erez Volk (FLAC support)
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
	Melder_assert (our soundOrLongSound());
	if (our sound())   // LongSound editors can get spurious v_dataChanged messages (e.g. in a TextGrid editor)
		Matrix_getWindowExtrema (our sound(), 1, our sound() -> nx, 1, our sound() -> ny,
				& our soundArea -> cache. globalMinimum, & our soundArea -> cache. globalMaximum);   // BUG unreadable
	SoundEditor_Parent :: v_dataChanged ();
}

/***** EDIT MENU *****/

static void menu_cb_Copy (SoundEditor me, EDITOR_ARGS_DIRECT) {
	try {
		Sound_clipboard = ( my longSound()
			? LongSound_extractPart (my longSound(), my startSelection, my endSelection, false)
			: Sound_extractPart (my sound(), my startSelection, my endSelection, kSound_windowShape::RECTANGULAR, 1.0, false)
		);
	} catch (MelderError) {
		Melder_throw (U"Sound selection not copied to clipboard.");
	}
}

static void menu_cb_Cut (SoundEditor me, EDITOR_ARGS_DIRECT) {
	Melder_assert (my sound());
	try {
		integer first, last;
		const integer selectionNumberOfSamples = Sampled_getWindowSamples (my sound(),
				my startSelection, my endSelection, & first, & last);
		const integer oldNumberOfSamples = my sound() -> nx;
		const integer newNumberOfSamples = oldNumberOfSamples - selectionNumberOfSamples;
		if (newNumberOfSamples < 1)
			Melder_throw (U"You cannot cut all of the signal away,\n"
				U"because you cannot create a Sound with 0 samples.\n"
				U"You could consider using Copy instead."
			);

		if (selectionNumberOfSamples > 0) {
			/*
				Create without change.
			*/
			autoSound publish = Sound_create (my sound() -> ny,
				0.0, selectionNumberOfSamples * my sound() -> dx,
				selectionNumberOfSamples, my sound() -> dx, 0.5 * my sound() -> dx
			);
			for (integer channel = 1; channel <= my sound() -> ny; channel ++) {
				integer j = 0;
				for (integer i = first; i <= last; i ++)
					publish -> z [channel] [++ j] = my sound() -> z [channel] [i];
			}
			autoMAT newData = raw_MAT (my sound() -> ny, newNumberOfSamples);
			for (integer channel = 1; channel <= my sound() -> ny; channel ++) {
				integer j = 0;
				for (integer i = 1; i < first; i ++)
					newData [channel] [++ j] = my sound() -> z [channel] [i];
				for (integer i = last + 1; i <= oldNumberOfSamples; i ++)
					newData [channel] [++ j] = my sound() -> z [channel] [i];
				Melder_assert (j == newData.ncol);
			}
			Editor_save (me, U"Cut");
			/*
				Change without error.
			*/
			my sound() -> xmin = 0.0;
			my sound() -> xmax = newNumberOfSamples * my sound() -> dx;
			my sound() -> nx = newNumberOfSamples;
			my sound() -> x1 = 0.5 * my sound() -> dx;
			my sound() -> z = newData.move();
			Sound_clipboard = publish.move();

			/*
				Start updating the markers of the FunctionEditor, respecting the invariants.
			*/
			my tmin = my sound() -> xmin;
			my tmax = my sound() -> xmax;

			/*
				Collapse the selection,
				so that the Cut operation can immediately be undone by a Paste.
				The exact position will be half-way in between two samples.
			*/
			my startSelection = my endSelection = my sound() -> xmin + (first - 1) * my sound() -> dx;

			/*
				Update the window.
			*/
			{
				const double t1 = (first - 1) * my sound() -> dx;
				const double t2 = last * my sound() -> dx;
				const double windowLength = my endWindow - my startWindow;   // > 0
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
					Melder_clipLeft (my tmin, & my startWindow);   // third try
					my endWindow = my tmax;   // second try
				} else if (my startWindow < my tmin) {
					my endWindow -= my startWindow - my tmin;   // second try
					Melder_clipRight (& my endWindow, my tmax);   // third try
					my startWindow = my tmin;   // second try
				}
			}

			/*
				Force FunctionEditor to show changes.
			*/
			Matrix_getWindowExtrema (my sound(), 1, my sound() -> nx, 1, my sound() -> ny,
					& my soundArea -> cache. globalMinimum, & my soundArea -> cache. globalMaximum);
			my v_reset_analysis ();
			FunctionEditor_ungroup (my sound());
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
	Melder_assert (my sound());
	integer leftSample = Sampled_xToLowIndex (my sound(), my endSelection);
	const integer oldNumberOfSamples = my sound() -> nx;
	if (! Sound_clipboard) {
		Melder_warning (U"Clipboard is empty; nothing pasted.");
		return;
	}
	Melder_require (Sound_clipboard -> ny == my sound() -> ny,
		U"Cannot paste, because\n"
		U"the number of channels of the clipboard is not equal to\n"
		U"the number of channels of the edited sound."
	);
	Melder_require (Sound_clipboard -> dx == my sound() -> dx,
		U"Cannot paste, because\n"
		U"the sampling frequency of the clipboard is not equal to\n"
		U"the sampling frequency of the edited sound."
	);
	Melder_clip (0_integer, & leftSample, oldNumberOfSamples);
	const integer newNumberOfSamples = oldNumberOfSamples + Sound_clipboard -> nx;
	/*
		Check without change.
	*/
	autoMAT newData = raw_MAT (my sound() -> ny, newNumberOfSamples);
	for (integer channel = 1; channel <= my sound() -> ny; channel ++) {
		integer j = 0;
		for (integer i = 1; i <= leftSample; i ++)
			newData [channel] [++ j] = my sound() -> z [channel] [i];
		for (integer i = 1; i <= Sound_clipboard -> nx; i ++)
			newData [channel] [++ j] = Sound_clipboard -> z [channel] [i];
		for (integer i = leftSample + 1; i <= oldNumberOfSamples; i ++)
			newData [channel] [++ j] = my sound() -> z [channel] [i];
		Melder_assert (j == newData.ncol);
	}
	Editor_save (me, U"Paste");
	/*
		Change without error.
	*/
	my sound() -> xmin = 0.0;
	my sound() -> xmax = newNumberOfSamples * my sound() -> dx;
	my sound() -> nx = newNumberOfSamples;
	my sound() -> x1 = 0.5 * my sound() -> dx;
	my sound() -> z = newData.move();

	/*
		Start updating the markers of the FunctionEditor, respecting the invariants.
	*/
	my tmin = my sound() -> xmin;
	my tmax = my sound() -> xmax;
	Melder_clipLeft (my tmin, & my startWindow);
	Melder_clipRight (& my endWindow, my tmax);
	my startSelection = leftSample * my sound() -> dx;
	my endSelection = (leftSample + Sound_clipboard -> nx) * my sound() -> dx;

	/*
		Force FunctionEditor to show changes.
	*/
	Matrix_getWindowExtrema (my sound(), 1, my sound() -> nx, 1, my sound() -> ny,
			& my soundArea -> cache. globalMinimum, & my soundArea -> cache. globalMaximum);
	my v_reset_analysis ();
	FunctionEditor_ungroup (my sound());
	FunctionEditor_marksChanged (me, false);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_SetSelectionToZero (SoundEditor me, EDITOR_ARGS_DIRECT) {
	Melder_assert (my sound());
	integer first, last;
	Sampled_getWindowSamples (my sound(), my startSelection, my endSelection, & first, & last);
	Editor_save (me, U"Set to zero");
	my sound() -> z.verticalBand	(first, last)  <<=  0.0;
	my v_reset_analysis ();
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_ReverseSelection (SoundEditor me, EDITOR_ARGS_DIRECT) {
	Melder_assert (my sound());
	Editor_save (me, U"Reverse selection");
	Sound_reverse (my sound(), my startSelection, my endSelection);
	my v_reset_analysis ();
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

/***** SELECT MENU *****/

static void menu_cb_MoveCursorToZero (SoundEditor me, EDITOR_ARGS_DIRECT) {
	Melder_assert (my sound());
	const double zero = Sound_getNearestZeroCrossing (my sound(), 0.5 * (my startSelection + my endSelection), 1);   // STEREO BUG
	if (isdefined (zero)) {
		my startSelection = my endSelection = zero;
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_MoveBtoZero (SoundEditor me, EDITOR_ARGS_DIRECT) {
	Melder_assert (my sound());
	const double zero = Sound_getNearestZeroCrossing (my sound(), my startSelection, 1);   // STEREO BUG
	if (isdefined (zero)) {
		my startSelection = zero;
		Melder_sort (& my startSelection, & my endSelection);
		FunctionEditor_marksChanged (me, true);
	}
}

static void menu_cb_MoveEtoZero (SoundEditor me, EDITOR_ARGS_DIRECT) {
	Melder_assert (my sound());
	const double zero = Sound_getNearestZeroCrossing (my sound(), my endSelection, 1);   // STEREO BUG
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
	Melder_assert (our soundOrLongSound());

	Editor_addCommand (this, U"Edit", U"-- cut copy paste --", 0, nullptr);
	if (our sound())
		cutButton = Editor_addCommand (this, U"Edit", U"Cut", 'X', menu_cb_Cut);
	copyButton = Editor_addCommand (this, U"Edit", U"Copy selection to Sound clipboard", 'C', menu_cb_Copy);
	if (our sound())
		pasteButton = Editor_addCommand (this, U"Edit", U"Paste after selection", 'V', menu_cb_Paste);
	if (our sound()) {
		Editor_addCommand (this, U"Edit", U"-- zero --", 0, nullptr);
		zeroButton = Editor_addCommand (this, U"Edit", U"Set selection to zero", 0, menu_cb_SetSelectionToZero);
		reverseButton = Editor_addCommand (this, U"Edit", U"Reverse selection", 'R', menu_cb_ReverseSelection);
	}
	if (our sound()) {
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

void structSoundEditor :: v_distributeAreas () {
}

void structSoundEditor :: v_prepareDraw () {
	if (our longSound()) {
		try {
			LongSound_haveWindow (our longSound(), our startWindow, our endWindow);
		} catch (MelderError) {
			Melder_clearError ();
		}
	}
}

void structSoundEditor :: v_draw () {
	Graphics_Viewport viewport;
	const bool showAnalysis = our instancePref_spectrogram_show() || our instancePref_pitch_show() ||
			our instancePref_intensity_show() || our instancePref_formant_show();
	Melder_assert (our soundOrLongSound());

	/*
		We check beforehand whether the window fits the LongSound buffer.
	*/
	if (our longSound() && our endWindow - our startWindow > our longSound() -> bufferLength) {
		Graphics_setColour (our graphics.get(), Melder_WHITE);
		Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (our graphics.get(), Melder_BLACK);
		Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_BOTTOM);
		Graphics_text (our graphics.get(), 0.5, 0.5,   U"(window longer than ", Melder_float (Melder_single (our longSound() -> bufferLength)), U" seconds)");
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
	if (our instancePref_pulses_show())
		our v_draw_analysis_pulses ();
	SoundArea_draw (our soundArea.get(), our soundArea -> cache. globalMinimum, our soundArea -> cache. globalMaximum);
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
	const integer selectedSamples = Sampled_getWindowSamples (our soundOrLongSound(),
			our startSelection, our endSelection, & first, & last);
	v_updateMenuItems_file ();
	if (our sound()) {
		GuiThing_setSensitive (cutButton     , selectedSamples != 0 && selectedSamples < our sound() -> nx);
		GuiThing_setSensitive (copyButton    , selectedSamples != 0);
		GuiThing_setSensitive (zeroButton    , selectedSamples != 0);
		GuiThing_setSensitive (reverseButton , selectedSamples != 0);
	}
}

void structSoundEditor :: v_play (double startTime, double endTime) {
	const integer numberOfChannels = our soundOrLongSound() -> ny;
	integer numberOfMuteChannels = 0;
	Melder_assert (our soundArea -> muteChannels.size == numberOfChannels);
	for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
		if (our soundArea -> muteChannels [ichan])
			numberOfMuteChannels ++;
	const integer numberOfChannelsToPlay = numberOfChannels - numberOfMuteChannels;
	Melder_require (numberOfChannelsToPlay > 0,
		U"Please select at least one channel to play.");
	if (our longSound()) {
		if (numberOfMuteChannels > 0) {
			autoSound part = LongSound_extractPart (our longSound(), startTime, endTime, 1);
			autoMixingMatrix thee = MixingMatrix_create (numberOfChannelsToPlay, numberOfChannels);
			MixingMatrix_muteAndActivateChannels (thee.get(), our soundArea -> muteChannels.get());
			Sound_MixingMatrix_playPart (part.get(), thee.get(), startTime, endTime, theFunctionEditor_playCallback, this);
		} else {
			LongSound_playPart (our longSound(), startTime, endTime, theFunctionEditor_playCallback, this);
		}
	} else {
		if (numberOfMuteChannels > 0) {
			autoMixingMatrix thee = MixingMatrix_create (numberOfChannelsToPlay, numberOfChannels);
			MixingMatrix_muteAndActivateChannels (thee.get(), our soundArea -> muteChannels.get());
			Sound_MixingMatrix_playPart (our sound(), thee.get(), startTime, endTime, theFunctionEditor_playCallback, this);
		} else {
			Sound_playPart (our sound(), startTime, endTime, theFunctionEditor_playCallback, this);
		}
	}
}

bool structSoundEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	constexpr double ANALYSIS_HEIGHT = 0.5;
	if ((our instancePref_spectrogram_show() || our instancePref_formant_show()) && y_fraction < ANALYSIS_HEIGHT && x_world > our startWindow && x_world < our endWindow)
		our d_spectrogram_cursor = our instancePref_spectrogram_viewFrom() +
				2.0 * y_fraction * (our instancePref_spectrogram_viewTo() - our instancePref_spectrogram_viewFrom());
	return SoundEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);
}

void structSoundEditor :: v_highlightSelection (double left, double right, double bottom, double top) {
	if (our instancePref_spectrogram_show())
		Graphics_highlight (our graphics.get(), left, right, 0.5 * (bottom + top), top);
	else
		Graphics_highlight (our graphics.get(), left, right, bottom, top);
}

autoSoundEditor SoundEditor_create (conststring32 title, SampledXY soundOrLongSound) {
	Melder_assert (soundOrLongSound);
	try {
		autoSoundEditor me = Thing_new (SoundEditor);
		autoSoundArea soundArea = SoundArea_create (me.get(), soundOrLongSound);
		TimeSoundAnalysisEditor_init (me.get(), soundArea.move(), title, soundOrLongSound, false);
		if (my longSound() && my endWindow - my startWindow > 30.0) {
			my endWindow = my startWindow + 30.0;
			if (my startWindow == my tmin)
				my startSelection = my endSelection = 0.5 * (my startWindow + my endWindow);
			FunctionEditor_marksChanged (me.get(), false);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound window not created.");
	}
}

/* End of file SoundEditor.cpp */
