/* SpectrumArea.cpp
 *
 * Copyright (C) 1992-2023 Paul Boersma
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

#include "SpectrumArea.h"
#include "EditorM.h"

Thing_implement (SpectrumArea, FunctionArea, 0);

#include "Prefs_define.h"
#include "SpectrumArea_prefs.h"
#include "Prefs_install.h"
#include "SpectrumArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "SpectrumArea_prefs.h"


#pragma mark - SpectrumArea helpers

inline static autoSpectrum Spectrum_band (Spectrum me, double fmin, double fmax) {
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
inline static autoSound Spectrum_to_Sound_part (Spectrum me, double fmin, double fmax) {
	autoSpectrum band = Spectrum_band (me, fmin, fmax);
	autoSound sound = Spectrum_to_Sound (band.get());
	return sound;
}


#pragma mark - SpectrumArea info

void structSpectrumArea :: v1_info () {
	MelderInfo_writeLine (U"Dynamic range: ", our instancePref_dynamicRange(), U" dB");
	MelderInfo_writeLine (U"Cursor height: ", our cursorHeight, U" dB");
	MelderInfo_writeLine (U"Minimum: ", our minimum(), U" dB");
	MelderInfo_writeLine (U"Maximum: ", our maximum(), U" dB");
}


#pragma mark - SpectrumArea drawing

void structSpectrumArea :: v_drawInside () {
	Spectrum_drawInside (our spectrum(), our graphics(), our startWindow(), our endWindow(),
			our minimum(), our maximum());
	Graphics_setColour (our graphics(), Melder_BLUE);
	FunctionArea_drawRightMark (this, our maximum(),
			Melder_fixed (our maximum(), 1), U" dB", Graphics_HALF);
	FunctionArea_drawRightMark (this, our minimum(),
			Melder_fixed (our minimum(), 1), U" dB", Graphics_HALF);
	if (our cursorHeight > our minimum() && our cursorHeight < our maximum())
		FunctionEditor_drawHorizontalHair (our functionEditor(), our cursorHeight,
				Melder_fixed (our cursorHeight, 1), U" dB");
}


#pragma mark - SpectrumArea tracking

bool structSpectrumArea :: v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double localY_fraction) {
	our cursorHeight = our minimum() + localY_fraction * (our maximum() - our minimum());
	return FunctionEditor_defaultMouseInWideDataView (our functionEditor(), event, x_world) || true;   // BUG:
}


#pragma mark - SpectrumArea View

static void menu_cb_setDynamicRange (SpectrumArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Set dynamic range", nullptr)
		POSITIVE (dynamicRange, U"Dynamic range (dB)", my default_dynamicRange())
	EDITOR_OK
		SET_REAL (dynamicRange, my instancePref_dynamicRange())
	EDITOR_DO
		my setInstancePref_dynamicRange (dynamicRange);
		my updateRange ();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}


#pragma mark - SpectrumArea Modify

static void menu_cb_passBand (SpectrumArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Filter (pass Hann band)", U"Spectrum: Filter (pass Hann band)...");
		REAL (bandSmoothing, U"Band smoothing (Hz)", my default_bandSmoothing())
	EDITOR_OK
		SET_REAL (bandSmoothing, my instancePref_bandSmoothing())
	EDITOR_DO
		my setInstancePref_bandSmoothing (bandSmoothing);
		Melder_require (my endSelection() > my startSelection(),
			U"To apply a band-pass filter, first make a selection.");

		FunctionArea_save (me, U"Pass band");

		Spectrum_passHannBand (my spectrum(), my startSelection(), my endSelection(), my instancePref_bandSmoothing());

		FunctionArea_broadcastDataChanged (me);
	EDITOR_END
}
static void menu_cb_stopBand (SpectrumArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Filter (stop Hann band)", nullptr)
		REAL (bandSmoothing, U"Band smoothing (Hz)", my default_bandSmoothing())
	EDITOR_OK
		SET_REAL (bandSmoothing, my instancePref_bandSmoothing())
	EDITOR_DO
		my setInstancePref_bandSmoothing (bandSmoothing);
		Melder_require (my endSelection() > my startSelection(),
			U"To apply a band-stop filter, first make a selection.");

		FunctionArea_save (me, U"Stop band");

		Spectrum_stopHannBand (my spectrum(), my startSelection(), my endSelection(), my instancePref_bandSmoothing());

		FunctionArea_broadcastDataChanged (me);
	EDITOR_END
}


#pragma mark - SpectrumArea Select

static void menu_cb_moveCursorToPeak (SpectrumArea me, EDITOR_ARGS) {
	MelderPoint peak = Spectrum_getNearestMaximum (my spectrum(), 0.5 * (my startSelection() + my endSelection()));
	my setSelection (peak. x, peak. x);
	my cursorHeight = peak. y;
	Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_selectionMarksChanged()
	FunctionEditor_selectionMarksChanged (my functionEditor());
}


#pragma mark - SpectrumArea Extract

static void CONVERT_DATA_TO_ONE__PublishBand (SpectrumArea me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		autoSpectrum result = Spectrum_band (my spectrum(), my startSelection(), my endSelection());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}
static void CONVERT_DATA_TO_ONE__PublishSound (SpectrumArea me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		autoSound result = Spectrum_to_Sound_part (my spectrum(), my startSelection(), my endSelection());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}


#pragma mark - SpectrumArea all menus?

void structSpectrumArea :: v_createMenus () {
	EditorMenu menu = Editor_addMenu (our functionEditor(), U"Spectrum", 0);

	FunctionAreaMenu_addCommand (menu, U"Power density range:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Set dynamic range...", 1,
			menu_cb_setDynamicRange, this);

	FunctionAreaMenu_addCommand (menu, U"- Modify spectrum:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Pass band...", 1,
			menu_cb_passBand, this);
	FunctionAreaMenu_addCommand (menu, U"Stop band...", 1,
			menu_cb_stopBand, this);

	FunctionAreaMenu_addCommand (menu, U"- Select by spectrum:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Move cursor to nearest peak", 'K' | GuiMenu_DEPTH_1,
			menu_cb_moveCursorToPeak, this);

	FunctionAreaMenu_addCommand (menu, U"- Extract spectrum:", 0, nullptr, this);
	our publishBandButton = FunctionAreaMenu_addCommand (menu, U"Publish band", 1,
			CONVERT_DATA_TO_ONE__PublishBand, this);
	our publishSoundButton = FunctionAreaMenu_addCommand (menu, U"Publish band-filtered sound", 1,
			CONVERT_DATA_TO_ONE__PublishSound, this);
}


#pragma mark - SpectrumArea functions

void SpectrumArea_play (SpectrumArea me, double fromFrequency, double toFrequency) {
	autoSound sound = Spectrum_to_Sound_part (my spectrum(), fromFrequency, toFrequency);
	Sound_play (sound.get(), nullptr, nullptr);
}

/* End of file SpectrumArea.cpp */
