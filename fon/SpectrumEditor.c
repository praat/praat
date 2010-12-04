/* SpectrumEditor.c
 *
 * Copyright (C) 1992-2008 Paul Boersma
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
 * pb 2006/04/01 dynamic range setting
 * pb 2007/06/10 wchar_t
 * pb 2007/08/12 wchar_t
 * pb 2007/09/04 new FunctionEditor API
 * pb 2008/01/19 double
 * pb 2008/03/20 split off Help menu
 * pb 2008/03/21 new Editor API
 */

#include "SpectrumEditor.h"
#include "Sound_and_Spectrum.h"
#include "Preferences.h"
#include "EditorM.h"

static struct {
	double bandSmoothing;
	double dynamicRange;
} preferences;

void SpectrumEditor_prefs (void) {
	Preferences_addDouble (L"SpectrumEditor.bandSmoothing", & preferences.bandSmoothing, 100.0);
	Preferences_addDouble (L"SpectrumEditor.dynamicRange", & preferences.dynamicRange, 60.0);
}

static void updateRange (SpectrumEditor me) {
	if (Spectrum_getPowerDensityRange (my data, & my minimum, & my maximum)) {
		my minimum = my maximum - my dynamicRange;
	} else {
		my minimum = -1000, my maximum = 1000;
	}
}

static void dataChanged (SpectrumEditor me) {
	updateRange (me);
	inherited (SpectrumEditor) dataChanged (SpectrumEditor_as_parent (me));
}

static void draw (SpectrumEditor me) {
	Spectrum spectrum = my data;
	long first, last, selectedSamples;

	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_rectangle (my graphics, 0, 1, 0, 1);
	Spectrum_drawInside (spectrum, my graphics, my startWindow, my endWindow, my minimum, my maximum);
	FunctionEditor_drawRangeMark (SpectrumEditor_as_FunctionEditor (me), my maximum, Melder_fixed (my maximum, 1), L" dB", Graphics_TOP);
	FunctionEditor_drawRangeMark (SpectrumEditor_as_FunctionEditor (me), my minimum, Melder_fixed (my minimum, 1), L" dB", Graphics_BOTTOM);
	if (my cursorHeight > my minimum && my cursorHeight < my maximum)
		FunctionEditor_drawHorizontalHair (SpectrumEditor_as_FunctionEditor (me), my cursorHeight, Melder_fixed (my cursorHeight, 1), L" dB");
	Graphics_setColour (my graphics, Graphics_BLACK);

	/* Update buttons. */

	selectedSamples = Sampled_getWindowSamples (spectrum, my startSelection, my endSelection, & first, & last);
	GuiObject_setSensitive (my publishBandButton, selectedSamples != 0);
	GuiObject_setSensitive (my publishSoundButton, selectedSamples != 0);
}

static int click (SpectrumEditor me, double xWC, double yWC, int shiftKeyPressed) {
	my cursorHeight = my minimum + yWC * (my maximum - my minimum);
	return inherited (SpectrumEditor) click (SpectrumEditor_as_parent (me), xWC, yWC, shiftKeyPressed);   /* Move cursor or drag selection. */
}

static Spectrum Spectrum_band (Spectrum me, double fmin, double fmax) {
	long i, imin, imax;
	double *re, *im;
	Spectrum band = Data_copy (me);
	if (! band) return NULL;
	re = band -> z [1], im = band -> z [2];
	imin = Sampled_xToLowIndex (band, fmin), imax = Sampled_xToHighIndex (band, fmax);
	for (i = 1; i <= imin; i ++) re [i] = 0.0, im [i] = 0.0;
	for (i = imax; i <= band -> nx; i ++) re [i] = 0.0, im [i] = 0.0;
	return band;
}

static Sound Spectrum_to_Sound_part (Spectrum me, double fmin, double fmax) {
	Spectrum band = Spectrum_band (me, fmin, fmax);
	Sound sound;
	if (! band) return NULL;
	sound = Spectrum_to_Sound (band);
	forget (band);
	return sound;
}

static void play (SpectrumEditor me, double fmin, double fmax) {
	Sound sound = Spectrum_to_Sound_part (my data, fmin, fmax);
	if (! sound) { Melder_flushError (NULL); return; }
	Sound_play (sound, NULL, NULL);
	forget (sound);
}

static int menu_cb_publishBand (EDITOR_ARGS) {
	EDITOR_IAM (SpectrumEditor);
	Spectrum publish = Spectrum_band (my data, my startSelection, my endSelection);
	if (! publish) return 0;
	if (my publishCallback)
		my publishCallback (me, my publishClosure, publish);
	return 1;
}

static int menu_cb_publishSound (EDITOR_ARGS) {
	EDITOR_IAM (SpectrumEditor);
	Sound publish = Spectrum_to_Sound_part (my data, my startSelection, my endSelection);
	if (! publish) return 0;
	if (my publishCallback)
		my publishCallback (me, my publishClosure, publish);
	return 1;
}

static int menu_cb_passBand (EDITOR_ARGS) {
	EDITOR_IAM (SpectrumEditor);
	EDITOR_FORM (L"Filter (pass Hann band)", L"Spectrum: Filter (pass Hann band)...");
		REAL (L"Band smoothing (Hz)", L"100.0")
	EDITOR_OK
		SET_REAL (L"Band smoothing", my bandSmoothing)
	EDITOR_DO
		preferences.bandSmoothing = my bandSmoothing = GET_REAL (L"Band smoothing");
		if (my endSelection <= my startSelection) return Melder_error1 (L"To apply band-pass filter, first make a selection.");
		Editor_save (SpectrumEditor_as_Editor (me), L"Pass band");
		Spectrum_passHannBand (my data, my startSelection, my endSelection, my bandSmoothing);
		FunctionEditor_redraw (SpectrumEditor_as_FunctionEditor (me));
		Editor_broadcastChange (SpectrumEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_stopBand (EDITOR_ARGS) {
	EDITOR_IAM (SpectrumEditor);
	EDITOR_FORM (L"Filter (stop Hann band)", 0)
		REAL (L"Band smoothing (Hz)", L"100.0")
	EDITOR_OK
		SET_REAL (L"Band smoothing", my bandSmoothing)
	EDITOR_DO
		preferences.bandSmoothing = my bandSmoothing = GET_REAL (L"Band smoothing");
		if (my endSelection <= my startSelection) return Melder_error1 (L"To apply band-stop filter, first make a selection.");
		Editor_save (SpectrumEditor_as_Editor (me), L"Stop band");
		Spectrum_stopHannBand (my data, my startSelection, my endSelection, my bandSmoothing);
		FunctionEditor_redraw (SpectrumEditor_as_FunctionEditor (me));
		Editor_broadcastChange (SpectrumEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_setDynamicRange (EDITOR_ARGS) {
	EDITOR_IAM (SpectrumEditor);
	EDITOR_FORM (L"Set dynamic range", 0)
		POSITIVE (L"Dynamic range (dB)", L"60.0")
	EDITOR_OK
		SET_REAL (L"Dynamic range", my dynamicRange)
	EDITOR_DO
		preferences.dynamicRange = my dynamicRange = GET_REAL (L"Dynamic range");
		updateRange (me);
		FunctionEditor_redraw (SpectrumEditor_as_FunctionEditor (me));
	EDITOR_END
}

static int menu_cb_help_SpectrumEditor (EDITOR_ARGS) { EDITOR_IAM (SpectrumEditor); Melder_help (L"SpectrumEditor"); return 1; }
static int menu_cb_help_Spectrum (EDITOR_ARGS) { EDITOR_IAM (SpectrumEditor); Melder_help (L"Spectrum"); return 1; }

static void createMenus (SpectrumEditor me) {
	inherited (SpectrumEditor) createMenus (SpectrumEditor_as_parent (me));
	my publishBandButton = Editor_addCommand (me, L"File", L"Publish band", 0, menu_cb_publishBand);
	my publishSoundButton = Editor_addCommand (me, L"File", L"Publish band-filtered sound", 0, menu_cb_publishSound);
	Editor_addCommand (me, L"File", L"-- close --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"-- edit band --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Pass band...", 0, menu_cb_passBand);
	Editor_addCommand (me, L"Edit", L"Stop band...", 0, menu_cb_stopBand);
}

static void createMenuItems_view (SpectrumEditor me, EditorMenu menu) {
	(void) me;
	EditorMenu_addCommand (menu, L"Set dynamic range...", 0, menu_cb_setDynamicRange);
	EditorMenu_addCommand (menu, L"-- view settings --", 0, 0);
	inherited (SpectrumEditor) createMenuItems_view (SpectrumEditor_as_parent (me), menu);
}

static void createHelpMenuItems (SpectrumEditor me, EditorMenu menu) {
	inherited (SpectrumEditor) createHelpMenuItems (SpectrumEditor_as_parent (me), menu);
	EditorMenu_addCommand (menu, L"SpectrumEditor help", '?', menu_cb_help_SpectrumEditor);
	EditorMenu_addCommand (menu, L"Spectrum help", 0, menu_cb_help_Spectrum);
}

class_methods (SpectrumEditor, FunctionEditor) {
	class_method (createMenus)
	class_method (createMenuItems_view)
	class_method (createHelpMenuItems)
	class_method (dataChanged)
	class_method (draw)
	us -> format_domain = L"Frequency domain:";
	us -> format_short = L"%.0f";
	us -> format_long = L"%.2f";
	us -> fixedPrecision_long = 2;
	us -> format_units = L"Hertz";
	us -> format_totalDuration = L"Total bandwidth %.2f Hertz";
	us -> format_window = L"Window %.2f Hertz";
	us -> format_selection = L"%.2f Hz";
	class_method (click)
	class_method (play)
	class_methods_end
}

SpectrumEditor SpectrumEditor_create (GuiObject parent, const wchar_t *title, Any data) {
	SpectrumEditor me = new (SpectrumEditor);
	FunctionEditor_init (SpectrumEditor_as_parent (me), parent, title, data); cherror
	my cursorHeight = -1000;
	my bandSmoothing = preferences.bandSmoothing;
	my dynamicRange = preferences.dynamicRange;
	updateRange (me);
end:
	iferror forget (me);
	return me;
}

/* End of file SpectrumEditor.c */
