/* SpectrumArea.cpp
 *
 * Copyright (C) 1992-2022 Paul Boersma
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

#pragma mark - SpectrumArea info

void structSpectrumArea :: v1_info () {
	MelderInfo_writeLine (U"Dynamic range: ", our instancePref_dynamicRange(), U" dB");
	MelderInfo_writeLine (U"Cursor height: ", our cursorHeight, U" dB");
}

void structSpectrumArea :: v_drawInside () {
	Spectrum_drawInside (our spectrum(), our graphics(), our startWindow(), our endWindow(), our minimum, our maximum);
	FunctionEditor_drawRangeMark (our functionEditor(), our maximum, Melder_fixed (maximum, 1), U" dB", Graphics_TOP);
	FunctionEditor_drawRangeMark (our functionEditor(), our minimum, Melder_fixed (minimum, 1), U" dB", Graphics_BOTTOM);
	if (our cursorHeight > our minimum && our cursorHeight < our maximum)
		FunctionEditor_drawHorizontalHair (our functionEditor(), our cursorHeight, Melder_fixed (our cursorHeight, 1), U" dB");
}

bool structSpectrumArea :: v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	our cursorHeight = our minimum + y_fraction * (our maximum - our minimum);
	return FunctionEditor_defaultMouseInWideDataView (our functionEditor(), event, x_world) || true;   // BUG:
}

static void CONVERT_DATA_TO_ONE__PublishBand (SpectrumArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		autoSpectrum result = Spectrum_band (my spectrum(), my startSelection(), my endSelection());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void CONVERT_DATA_TO_ONE__PublishSound (SpectrumArea me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		autoSound result = Spectrum_to_Sound_part (my spectrum(), my startSelection(), my endSelection());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void menu_cb_passBand (SpectrumArea me, EDITOR_ARGS_FORM) {
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

		Editor_broadcastDataChanged (my functionEditor());
	EDITOR_END
}

static void menu_cb_stopBand (SpectrumArea me, EDITOR_ARGS_FORM) {
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

		Editor_broadcastDataChanged (my functionEditor());
	EDITOR_END
}

static void menu_cb_moveCursorToPeak (SpectrumArea me, EDITOR_ARGS_DIRECT) {
	MelderPoint peak = Spectrum_getNearestMaximum (my spectrum(), 0.5 * (my startSelection() + my endSelection()));
	my setSelection (peak. x, peak. x);
	my cursorHeight = peak. y;
	FunctionEditor_marksChanged (my functionEditor(), true);
}

static void menu_cb_setDynamicRange (SpectrumArea me, EDITOR_ARGS_FORM) {
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

void structSpectrumArea :: v_createMenus () {
	FunctionAreaMenu_addCommand (our functionEditor() -> extractMenu, U"-- spectrum extract --", 0, nullptr, this);
	our publishBandButton = FunctionAreaMenu_addCommand (our functionEditor() -> extractMenu, U"Publish band", 0,
			CONVERT_DATA_TO_ONE__PublishBand, this);
	our publishSoundButton = FunctionAreaMenu_addCommand (our functionEditor() -> extractMenu, U"Publish band-filtered sound", 0,
			CONVERT_DATA_TO_ONE__PublishSound, this);
	FunctionAreaMenu_addCommand (our functionEditor() -> editMenu, U"-- spectrum modify --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (our functionEditor() -> editMenu, U"Modify spectrum:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (our functionEditor() -> editMenu, U"Pass band...", 0,
			menu_cb_passBand, this);
	FunctionAreaMenu_addCommand (our functionEditor() -> editMenu, U"Stop band...", 0,
			menu_cb_stopBand, this);
	FunctionAreaMenu_addCommand (our functionEditor() -> selectMenu, U"-- move to peak --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (our functionEditor() -> selectMenu, U"Select by spectrum:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (our functionEditor() -> selectMenu, U"Move cursor to nearest peak", 'K',
			menu_cb_moveCursorToPeak, this);
}

void structSpectrumArea :: v_createMenuItems_view (EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"-- spectrum view --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Set dynamic range...", 0,
			menu_cb_setDynamicRange, this);
}

/* End of file SpectrumArea.cpp */
