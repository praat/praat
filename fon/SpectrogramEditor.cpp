/* SpectrogramEditor.cpp
 *
 * Copyright (C) 1992-2011,2012,2014,2015 Paul Boersma
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

#include "SpectrogramEditor.h"

Thing_implement (SpectrogramEditor, FunctionEditor, 0);

void structSpectrogramEditor :: v_draw () {
	Spectrogram spectrogram = (Spectrogram) our data;

	Graphics_setWindow (our d_graphics, 0, 1, 0, 1);
	Graphics_setColour (our d_graphics, Graphics_WHITE);
	Graphics_fillRectangle (our d_graphics, 0, 1, 0, 1);
	Graphics_setColour (our d_graphics, Graphics_BLACK);
	Graphics_rectangle (our d_graphics, 0, 1, 0, 1);

	long itmin, itmax;
	Sampled_getWindowSamples (spectrogram, our d_startWindow, our d_endWindow, & itmin, & itmax);

	/*
	 * Autoscale frequency axis.
	 */
	our maximum = spectrogram -> ymax;

	Graphics_setWindow (our d_graphics, our d_startWindow, our d_endWindow, 0.0, our maximum);
	Spectrogram_paintInside (spectrogram, our d_graphics, our d_startWindow, our d_endWindow, 0, 0, 0.0, true,
		 60, 6.0, 0);

	/*
	 * Horizontal scaling lines.
	 */
	Graphics_setWindow (our d_graphics, 0.0, 1.0, 0.0, our maximum);
	Graphics_setTextAlignment (our d_graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_setColour (our d_graphics, Graphics_RED);
	long df = 1000;
	for (long f = df; f <= our maximum; f += df) {
		Graphics_line (our d_graphics, 0.0, f, 1.0, f);
		Graphics_text (our d_graphics, -0.01, f,   f, U" Hz");
	}

	/*
	 * Vertical cursor lines.
	 */
	Graphics_setWindow (our d_graphics, our d_startWindow, our d_endWindow, 0.0, our maximum);
	if (our d_startSelection > our d_startWindow && our d_startSelection < our d_endWindow)
		Graphics_line (our d_graphics, our d_startSelection, 0, our d_startSelection, our maximum);
	if (our d_endSelection > our d_startWindow && d_endSelection < d_endWindow)
		Graphics_line (our d_graphics, our d_endSelection, 0, our d_endSelection, our maximum);
	Graphics_setColour (our d_graphics, Graphics_BLACK);
}

int structSpectrogramEditor :: v_click (double xWC, double yWC, bool shiftKeyPressed) {
	Spectrogram spectrogram = (Spectrogram) our data;
	/*double frequency = yWC * our maximum;*/
	long bestFrame;
	bestFrame = Sampled_xToNearestIndex (spectrogram, xWC);
	if (bestFrame < 1)
		bestFrame = 1;
	else if (bestFrame > spectrogram -> nx)
		bestFrame = spectrogram -> nx;
	return our SpectrogramEditor_Parent :: v_click (xWC, yWC, shiftKeyPressed);
}

autoSpectrogramEditor SpectrogramEditor_create (const char32 *title, Spectrogram data) {
	try {
		autoSpectrogramEditor me = Thing_new (SpectrogramEditor);
		FunctionEditor_init (me.peek(), title, data);
		my maximum = 10000.0;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Spectrogram window not created.");
	}
}

/* End of file SpectrogramEditor.cpp */
