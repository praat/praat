/* SpectrumEditor.c
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 1999/10/25
 * pb 2002/07/16 GPL
 */

#include "SpectrumEditor.h"
#include "Sound_and_Spectrum.h"
#include "Resources.h"
#include "EditorM.h"

#define SpectrumEditor_members FunctionEditor_members \
	double minimum, maximum, cursorHeight; \
	double bandSmoothing; \
	Widget publishBandButton, publishSoundButton;
#define SpectrumEditor_methods FunctionEditor_methods
class_create_opaque (SpectrumEditor, FunctionEditor)

static struct {
	double bandSmoothing;
}
	preferences = {
		100   /* band smoothing */
	};

void SpectrumEditor_prefs (void) {
	Resources_addDouble ("SpectrumEditor.bandSmoothing", & preferences.bandSmoothing);
}

static void updateRange (SpectrumEditor me) {
	if (Spectrum_getPowerDensityRange (my data, & my minimum, & my maximum))
		my minimum = my maximum - 60;
	else
		my minimum = -1000, my maximum = 1000;
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
	FunctionEditor_drawRangeMark (me, "%.1f dB", my maximum, Graphics_TOP);
	FunctionEditor_drawRangeMark (me, "%.1f dB", my minimum, Graphics_BOTTOM);
	if (my cursorHeight > my minimum && my cursorHeight < my maximum)
		FunctionEditor_drawHorizontalHair (me, "%.1f dB", my cursorHeight);
	Graphics_setColour (my graphics, Graphics_BLACK);

	/* Update buttons. */

	selectedSamples = Sampled_getWindowSamples (spectrum, my startSelection, my endSelection, & first, & last);
	XtSetSensitive (my publishBandButton, selectedSamples != 0);
	XtSetSensitive (my publishSoundButton, selectedSamples != 0);
}

static int click (I, double xWC, double yWC, int shiftKeyPressed) {
	iam (SpectrumEditor);
	my cursorHeight = my minimum + yWC * (my maximum - my minimum);
	return inherited (SpectrumEditor) click (me, xWC, yWC, shiftKeyPressed);   /* Move cursor or drag selection. */
}

static Spectrum Spectrum_band (Spectrum me, double fmin, double fmax) {
	long i, imin, imax;
	float *re, *im;
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

FORM (SpectrumEditor, cb_passBand, "Filter (pass Hann band)", "Spectrum: Filter (pass Hann band)...");
	REAL ("Band smoothing (Hz)", "100.0")
	OK
SET_REAL ("Band smoothing", my bandSmoothing)
DO
	preferences.bandSmoothing = my bandSmoothing = GET_REAL ("Band smoothing");
	if (my endSelection <= my startSelection) return Melder_error ("To apply band-pass filter, first make a selection.");
	Editor_save (me, "Pass band");
	Spectrum_passHannBand (my data, my startSelection, my endSelection, my bandSmoothing);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (SpectrumEditor, cb_stopBand, "Filter (stop Hann band)", 0)
	REAL ("Band smoothing (Hz)", "100.0")
	OK
SET_REAL ("Band smoothing", my bandSmoothing)
DO
	preferences.bandSmoothing = my bandSmoothing = GET_REAL ("Band smoothing");
	if (my endSelection <= my startSelection) return Melder_error ("To apply band-stop filter, first make a selection.");
	Editor_save (me, "Stop band");
	Spectrum_stopHannBand (my data, my startSelection, my endSelection, my bandSmoothing);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (SpectrumEditor, cb_help_SpectrumEditor) Melder_help ("SpectrumEditor"); END
DIRECT (SpectrumEditor, cb_help_Spectrum) Melder_help ("Spectrum"); END

static void createMenus (I) {
	iam (SpectrumEditor);
	inherited (SpectrumEditor) createMenus (me);
	my publishBandButton = Editor_addCommand (me, "File", "Publish band", 0, cb_publishBand);
	my publishSoundButton = Editor_addCommand (me, "File", "Publish band-filtered sound", 0, cb_publishSound);
	Editor_addCommand (me, "File", "-- close --", 0, NULL);
	Editor_addCommand (me, "Edit", "-- edit band --", 0, NULL);
	Editor_addCommand (me, "Edit", "Pass band...", 0, cb_passBand);
	Editor_addCommand (me, "Edit", "Stop band...", 0, cb_stopBand);
	Editor_addCommand (me, "Help", "SpectrumEditor help", '?', cb_help_SpectrumEditor);
	Editor_addCommand (me, "Help", "Spectrum help", 0, cb_help_Spectrum);
}

class_methods (SpectrumEditor, FunctionEditor)
	class_method (createMenus)
	class_method (dataChanged)
	class_method (draw)
	us -> format_domain = "Frequency domain:";
	us -> format_short = "%.0f";
	us -> format_long = "%.2f";
	us -> format_units = "Hertz";
	us -> format_totalDuration = "Total bandwidth %.2f Hertz";
	us -> format_window = "Window %.2f Hertz";
	us -> format_selection = "%.2f Hz";
	class_method (click)
	class_method (play)
class_methods_end

SpectrumEditor SpectrumEditor_create (Widget parent, char *title, Any data) {
	SpectrumEditor me = new (SpectrumEditor);
	if (! me || ! FunctionEditor_init (me, parent, title, data)) return NULL;
	updateRange (me);
	my cursorHeight = -1000;
	my bandSmoothing = preferences.bandSmoothing;
	return me;
}

/* End of file SpectrumEditor.c */
