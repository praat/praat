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
#include "EditorM.h"

Thing_implement (SoundEditor, TimeSoundAnalysisEditor, 0);

/********** METHODS **********/

/***** HELP MENU *****/

static void menu_cb_SoundEditorHelp (SoundEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"SoundEditor"); }
static void menu_cb_LongSoundEditorHelp (SoundEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"LongSoundEditor"); }

void structSoundEditor :: v_createMenus () {
	SoundEditor_Parent :: v_createMenus ();
	v_createMenus_analysis ();
}

void structSoundEditor :: v_createHelpMenuItems (EditorMenu menu) {
	SoundEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"SoundEditor help", '?', menu_cb_SoundEditorHelp);
	EditorMenu_addCommand (menu, U"LongSoundEditor help", 0, menu_cb_LongSoundEditorHelp);
}

/********** UPDATE **********/

void structSoundEditor :: v_distributeAreas () {
	const bool showAnalysis = our instancePref_spectrogram_show() || our instancePref_pitch_show() ||
			our instancePref_intensity_show() || our instancePref_formant_show();
	if (showAnalysis) {
		our soundArea -> setGlobalYRange_fraction (0.5, 1.0);
		our soundAnalysisArea -> setGlobalYRange_fraction (0.0, 0.5);
	} else {
		our soundArea -> setGlobalYRange_fraction (0.0, 1.0);
		our soundAnalysisArea -> setGlobalYRange_fraction (0.0, 0.0);
	}
}

void structSoundEditor :: v_draw () {
	const bool showAnalysis = our instancePref_spectrogram_show() || our instancePref_pitch_show() ||
			our instancePref_intensity_show() || our instancePref_formant_show();
	Melder_assert (our soundOrLongSound());

	/*
		We check beforehand whether the window fits the LongSound buffer. BUG: should be in LongSoundArea
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
	FunctionArea_prepareCanvas (our soundArea.get());
	if (our instancePref_pulses_show())
		our v_draw_analysis_pulses ();
	FunctionArea_drawInside (our soundArea.get());
	if (showAnalysis) {
		our soundAnalysisArea -> setGlobalYRange_fraction (0.0, 0.5);
		FunctionArea_prepareCanvas (our soundAnalysisArea.get());
		our v_draw_analysis ();
	}

	/*
		Update buttons. BUG: move to right place
	*/
	integer first, last;
	const integer selectedSamples = Sampled_getWindowSamples (our soundOrLongSound(),
			our startSelection, our endSelection, & first, & last);
	our v_updateMenuItems ();
}

void structSoundEditor :: v_play (double startTime, double endTime) {
	if (our soundArea)
		SoundArea_play (our soundArea.get(), startTime, endTime);
}

bool structSoundEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction_global) {
	if (our soundAnalysisArea -> y_fraction_globalIsInside (y_fraction_global) &&
		x_world > our startWindow && x_world < our endWindow
	) {
		const double y_fraction_insideAnalysesArea = our soundAnalysisArea -> y_fraction_globalToLocal (y_fraction_global);
		our d_spectrogram_cursor = y_fraction_insideAnalysesArea * our instancePref_spectrogram_viewTo()
				+ (1.0 - y_fraction_insideAnalysesArea) * our instancePref_spectrogram_viewFrom();
	}
	return SoundEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction_global);
}

autoSoundEditor SoundEditor_create (conststring32 title, SampledXY soundOrLongSound) {
	Melder_assert (soundOrLongSound);
	try {
		autoSoundEditor me = Thing_new (SoundEditor);
		if (Thing_isa (soundOrLongSound, classSound))
			my soundArea = SoundArea_create (true, nullptr, me.get());
		else
			my soundArea = LongSoundArea_create (false, nullptr, me.get());
		my soundAnalysisArea = SoundAnalysisArea_create (false, nullptr, me.get());
		FunctionEditor_init (me.get(), title, soundOrLongSound);

		Melder_assert (my soundOrLongSound());
		if (my longSound() && my endWindow - my startWindow > 30.0) {   // BUG: should be in dataChanged?
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
