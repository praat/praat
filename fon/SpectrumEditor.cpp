/* SpectrumEditor.cpp
 *
 * Copyright (C) 1992-2020 Paul Boersma
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

#include "SpectrumEditor.h"
#include "Sound_and_Spectrum.h"
#include "EditorM.h"

Thing_implement (SpectrumEditor, FunctionEditor, 0);

#include "prefs_define.h"
#include "SpectrumEditor_prefs.h"
#include "prefs_install.h"
#include "SpectrumEditor_prefs.h"
#include "prefs_copyToInstance.h"
#include "SpectrumEditor_prefs.h"

static void updateRange (SpectrumEditor me) {
	if (Spectrum_getPowerDensityRange ((Spectrum) my data, & my minimum, & my maximum)) {
		my minimum = my maximum - my p_dynamicRange;
	} else {
		my minimum = -1000.0;
		my maximum = 1000.0;
	}
}

void structSpectrumEditor :: v_dataChanged () {
	updateRange (this);
	SpectrumEditor_Parent :: v_dataChanged ();
}

void structSpectrumEditor :: v_draw () {
	Spectrum spectrum = (Spectrum) our data;

	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	Graphics_rectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Spectrum_drawInside (spectrum, our graphics.get(), our startWindow, our endWindow, our minimum, our maximum);
	FunctionEditor_drawRangeMark (this, our maximum, Melder_fixed (maximum, 1), U" dB", Graphics_TOP);
	FunctionEditor_drawRangeMark (this, our minimum, Melder_fixed (minimum, 1), U" dB", Graphics_BOTTOM);
	if (our cursorHeight > our minimum && our cursorHeight < our maximum)
		FunctionEditor_drawHorizontalHair (this, our cursorHeight, Melder_fixed (our cursorHeight, 1), U" dB");
	Graphics_setColour (our graphics.get(), Melder_BLACK);

	/*
		Update buttons.
		TODO: this is not about drawing, so improve the logic.
	*/
	integer first, last;
	const integer selectedSamples = Sampled_getWindowSamples (spectrum, our startSelection, our endSelection, & first, & last);
	GuiThing_setSensitive (our publishBandButton,  selectedSamples != 0);
	GuiThing_setSensitive (our publishSoundButton, selectedSamples != 0);
}

bool structSpectrumEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	our cursorHeight = our minimum + y_fraction * (our maximum - our minimum);
	return our SpectrumEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction) || true;
}

static autoSpectrum Spectrum_band (Spectrum me, double fmin, double fmax) {
	autoSpectrum band = Data_copy (me);
	double *re = & band -> z [1] [0], *im = & band -> z [2] [0];
	const integer imin = Sampled_xToLowIndex (band.get(), fmin);
	const integer imax = Sampled_xToHighIndex (band.get(), fmax);
	for (integer i = 1; i <= imin; i ++)
		re [i] = 0.0, im [i] = 0.0;
	for (integer i = imax; i <= band -> nx; i ++)
		re [i] = 0.0, im [i] = 0.0;
	return band;
}

static autoSound Spectrum_to_Sound_part (Spectrum me, double fmin, double fmax) {
	autoSpectrum band = Spectrum_band (me, fmin, fmax);
	autoSound sound = Spectrum_to_Sound (band.get());
	return sound;
}

void structSpectrumEditor :: v_play (double fmin, double fmax) {
	autoSound sound = Spectrum_to_Sound_part ((Spectrum) our data, fmin, fmax);
	Sound_play (sound.get(), nullptr, nullptr);
}

static void menu_cb_publishBand (SpectrumEditor me, EDITOR_ARGS_DIRECT) {
	autoSpectrum publish = Spectrum_band ((Spectrum) my data, my startSelection, my endSelection);
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_publishSound (SpectrumEditor me, EDITOR_ARGS_DIRECT) {
	autoSound publish = Spectrum_to_Sound_part ((Spectrum) my data, my startSelection, my endSelection);
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_passBand (SpectrumEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Filter (pass Hann band)", U"Spectrum: Filter (pass Hann band)...");
		REAL (bandSmoothing, U"Band smoothing (Hz)", my default_bandSmoothing ())
	EDITOR_OK
		SET_REAL (bandSmoothing, my p_bandSmoothing)
	EDITOR_DO
		my pref_bandSmoothing() = my p_bandSmoothing = bandSmoothing;
		Melder_require (my endSelection > my startSelection,
			U"To apply a band-pass filter, first make a selection.");
		Editor_save (me, U"Pass band");
		Spectrum_passHannBand ((Spectrum) my data, my startSelection, my endSelection, my p_bandSmoothing);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_stopBand (SpectrumEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Filter (stop Hann band)", nullptr)
		REAL (bandSmoothing, U"Band smoothing (Hz)", my default_bandSmoothing ())
	EDITOR_OK
		SET_REAL (bandSmoothing, my p_bandSmoothing)
	EDITOR_DO
		my pref_bandSmoothing () = my p_bandSmoothing = bandSmoothing;
		Melder_require (my endSelection > my startSelection,
			U"To apply a band-stop filter, first make a selection.");
		Editor_save (me, U"Stop band");
		Spectrum_stopHannBand ((Spectrum) my data, my startSelection, my endSelection, my p_bandSmoothing);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_moveCursorToPeak (SpectrumEditor me, EDITOR_ARGS_DIRECT) {
	MelderPoint peak = Spectrum_getNearestMaximum ((Spectrum) my data, 0.5 * (my startSelection + my endSelection));
	my startSelection = my endSelection = peak. x;
	my cursorHeight = peak. y;
	FunctionEditor_marksChanged (me, true);
}

static void menu_cb_setDynamicRange (SpectrumEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Set dynamic range", nullptr)
		POSITIVE (dynamicRange, U"Dynamic range (dB)", my default_dynamicRange ())
	EDITOR_OK
		SET_REAL (dynamicRange, my p_dynamicRange)
	EDITOR_DO
		my pref_dynamicRange () = my p_dynamicRange = dynamicRange;
		updateRange (me);
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_help_SpectrumEditor (SpectrumEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"SpectrumEditor"); }
static void menu_cb_help_Spectrum (SpectrumEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Spectrum"); }

void structSpectrumEditor :: v_createMenus () {
	SpectrumEditor_Parent :: v_createMenus ();
	our publishBandButton = Editor_addCommand (this, U"File", U"Publish band", 0, menu_cb_publishBand);
	our publishSoundButton = Editor_addCommand (this, U"File", U"Publish band-filtered sound", 0, menu_cb_publishSound);
	Editor_addCommand (this, U"File", U"-- close --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"-- edit band --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Pass band...", 0, menu_cb_passBand);
	Editor_addCommand (this, U"Edit", U"Stop band...", 0, menu_cb_stopBand);
	Editor_addCommand (this, U"Select", U"-- move to peak --", 0, nullptr);
	Editor_addCommand (this, U"Select", U"Move cursor to nearest peak", 'K', menu_cb_moveCursorToPeak);
}

void structSpectrumEditor :: v_createMenuItems_view (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"Set dynamic range...", 0, menu_cb_setDynamicRange);
	EditorMenu_addCommand (menu, U"-- view settings --", 0, 0);
	SpectrumEditor_Parent :: v_createMenuItems_view (menu);
}

void structSpectrumEditor :: v_createHelpMenuItems (EditorMenu menu) {
	SpectrumEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"SpectrumEditor help", '?', menu_cb_help_SpectrumEditor);
	EditorMenu_addCommand (menu, U"Spectrum help", 0, menu_cb_help_Spectrum);
}

autoSpectrumEditor SpectrumEditor_create (conststring32 title, Spectrum data) {
	try {
		autoSpectrumEditor me = Thing_new (SpectrumEditor);
		FunctionEditor_init (me.get(), title, data);
		my cursorHeight = -1000;
		updateRange (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Spectrum window not created.");
	}
}

/* End of file SpectrumEditor.cpp */
