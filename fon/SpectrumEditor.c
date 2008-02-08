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
 */

#include "SpectrumEditor.h"
#include "Sound_and_Spectrum.h"
#include "Preferences.h"
#include "EditorM.h"

#define SpectrumEditor_members FunctionEditor_members \
	double minimum, maximum, cursorHeight; \
	double bandSmoothing, dynamicRange; \
	Widget publishBandButton, publishSoundButton;
#define SpectrumEditor_methods FunctionEditor_methods
class_create_opaque (SpectrumEditor, FunctionEditor);

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

static void dataChanged (I) {
	iam (SpectrumEditor);
	updateRange (me);
	inherited (SpectrumEditor) dataChanged (me);
}

static void draw (I) {
	iam (SpectrumEditor);
	Spectrum spectrum = my data;
	long first, last, selectedSamples;

	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_rectangle (my graphics, 0, 1, 0, 1);
	Spectrum_drawInside (spectrum, my graphics, my startWindow, my endWindow, my minimum, my maximum);
	FunctionEditor_drawRangeMark (me, my maximum, Melder_fixed (my maximum, 1), L" dB", Graphics_TOP);
	FunctionEditor_drawRangeMark (me, my minimum, Melder_fixed (my minimum, 1), L" dB", Graphics_BOTTOM);
	if (my cursorHeight > my minimum && my cursorHeight < my maximum)
		FunctionEditor_drawHorizontalHair (me, my cursorHeight, Melder_fixed (my cursorHeight, 1), L" dB");
	Graphics_setColour (my graphics, Graphics_BLACK);

	/* Update buttons. */

	selectedSamples = Sampled_getWindowSamples (spectrum, my startSelection, my endSelection, & first, & last);
	GuiObject_setSensitive (my publishBandButton, selectedSamples != 0);
	GuiObject_setSensitive (my publishSoundButton, selectedSamples != 0);
}

static int click (I, double xWC, double yWC, int shiftKeyPressed) {
	iam (SpectrumEditor);
	my cursorHeight = my minimum + yWC * (my maximum - my minimum);
	return inherited (SpectrumEditor) click (me, xWC, yWC, shiftKeyPressed);   /* Move cursor or drag selection. */
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

static void play (I, double fmin, double fmax) {
	iam (SpectrumEditor);
	Sound sound = Spectrum_to_Sound_part (my data, fmin, fmax);
	if (! sound) { Melder_flushError (NULL); return; }
	Sound_play (sound, NULL, NULL);
	forget (sound);
}

DIRECT (SpectrumEditor, cb_publishBand)
	Spectrum publish = Spectrum_band (my data, my startSelection, my endSelection);
	if (! publish) return 0;
	if (my publishCallback)
		my publishCallback (me, my publishClosure, publish);
END

DIRECT (SpectrumEditor, cb_publishSound)
	Sound publish = Spectrum_to_Sound_part (my data, my startSelection, my endSelection);
	if (! publish) return 0;
	if (my publishCallback)
		my publishCallback (me, my publishClosure, publish);
END

FORM (SpectrumEditor, cb_passBand, L"Filter (pass Hann band)", L"Spectrum: Filter (pass Hann band)...");
	REAL (L"Band smoothing (Hz)", L"100.0")
	OK
SET_REAL (L"Band smoothing", my bandSmoothing)
DO
	preferences.bandSmoothing = my bandSmoothing = GET_REAL (L"Band smoothing");
	if (my endSelection <= my startSelection) return Melder_error1 (L"To apply band-pass filter, first make a selection.");
	Editor_save (me, L"Pass band");
	Spectrum_passHannBand (my data, my startSelection, my endSelection, my bandSmoothing);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (SpectrumEditor, cb_stopBand, L"Filter (stop Hann band)", 0)
	REAL (L"Band smoothing (Hz)", L"100.0")
	OK
SET_REAL (L"Band smoothing", my bandSmoothing)
DO
	preferences.bandSmoothing = my bandSmoothing = GET_REAL (L"Band smoothing");
	if (my endSelection <= my startSelection) return Melder_error1 (L"To apply band-stop filter, first make a selection.");
	Editor_save (me, L"Stop band");
	Spectrum_stopHannBand (my data, my startSelection, my endSelection, my bandSmoothing);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (SpectrumEditor, cb_setDynamicRange, L"Set dynamic range", 0)
	POSITIVE (L"Dynamic range (dB)", L"60.0")
	OK
SET_REAL (L"Dynamic range", my dynamicRange)
DO
	preferences.dynamicRange = my dynamicRange = GET_REAL (L"Dynamic range");
	updateRange (me);
	FunctionEditor_redraw (me);
END

DIRECT (SpectrumEditor, cb_help_SpectrumEditor) Melder_help (L"SpectrumEditor"); END
DIRECT (SpectrumEditor, cb_help_Spectrum) Melder_help (L"Spectrum"); END

static void createMenus (I) {
	iam (SpectrumEditor);
	inherited (SpectrumEditor) createMenus (me);
	my publishBandButton = Editor_addCommand (me, L"File", L"Publish band", 0, cb_publishBand);
	my publishSoundButton = Editor_addCommand (me, L"File", L"Publish band-filtered sound", 0, cb_publishSound);
	Editor_addCommand (me, L"File", L"-- close --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"-- edit band --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Pass band...", 0, cb_passBand);
	Editor_addCommand (me, L"Edit", L"Stop band...", 0, cb_stopBand);
	Editor_addCommand (me, L"Help", L"SpectrumEditor help", '?', cb_help_SpectrumEditor);
	Editor_addCommand (me, L"Help", L"Spectrum help", 0, cb_help_Spectrum);
}

static void createMenuItems_view (I, EditorMenu menu) {
	iam (SpectrumEditor);
	(void) me;
	EditorMenu_addCommand (menu, L"Set dynamic range...", 0, cb_setDynamicRange);
	EditorMenu_addCommand (menu, L"-- view settings --", 0, 0);
	inherited (SpectrumEditor) createMenuItems_view (me, menu);
}

class_methods (SpectrumEditor, FunctionEditor)
	class_method (createMenus)
	class_method (createMenuItems_view)
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

SpectrumEditor SpectrumEditor_create (Widget parent, const wchar_t *title, Any data) {
	SpectrumEditor me = new (SpectrumEditor);
	if (! me || ! FunctionEditor_init (me, parent, title, data)) return NULL;
	my cursorHeight = -1000;
	my bandSmoothing = preferences.bandSmoothing;
	my dynamicRange = preferences.dynamicRange;
	updateRange (me);
	return me;
}

/* End of file SpectrumEditor.c */
