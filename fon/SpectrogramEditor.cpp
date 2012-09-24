/* SpectrogramEditor.cpp
 *
 * Copyright (C) 1992-2011,2012 Paul Boersma
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
	Spectrogram spectrogram = (Spectrogram) data;

	Graphics_setWindow (d_graphics, 0, 1, 0, 1);
	Graphics_setColour (d_graphics, Graphics_WHITE);
	Graphics_fillRectangle (d_graphics, 0, 1, 0, 1);
	Graphics_setColour (d_graphics, Graphics_BLACK);
	Graphics_rectangle (d_graphics, 0, 1, 0, 1);

	long itmin, itmax;
	Sampled_getWindowSamples (spectrogram, d_startWindow, d_endWindow, & itmin, & itmax);

	/*
	 * Autoscale frequency axis.
	 */
	maximum = spectrogram -> ymax;

	Graphics_setWindow (d_graphics, d_startWindow, d_endWindow, 0.0, maximum);
	Spectrogram_paintInside (spectrogram, d_graphics, d_startWindow, d_endWindow, 0, 0, 0.0, TRUE,
		 60, 6.0, 0);

	/*
	 * Horizontal scaling lines.
	 */
	Graphics_setWindow (d_graphics, 0.0, 1.0, 0.0, maximum);
	Graphics_setTextAlignment (d_graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_setColour (d_graphics, Graphics_RED);
	long df = 1000;
	for (long f = df; f <= maximum; f += df) {
		Graphics_line (d_graphics, 0.0, f, 1.0, f);
		Graphics_text2 (d_graphics, -0.01, f, Melder_integer (f), L" Hz");
	}

	/*
	 * Vertical cursor lines.
	 */
	Graphics_setWindow (d_graphics, d_startWindow, d_endWindow, 0.0, maximum);
	if (d_startSelection > d_startWindow && d_startSelection < d_endWindow)
		Graphics_line (d_graphics, d_startSelection, 0, d_startSelection, maximum);
	if (d_endSelection > d_startWindow && d_endSelection < d_endWindow)
		Graphics_line (d_graphics, d_endSelection, 0, d_endSelection, maximum);
	Graphics_setColour (d_graphics, Graphics_BLACK);
}

int structSpectrogramEditor :: v_click (double xWC, double yWC, bool shiftKeyPressed) {
	Spectrogram spectrogram = (Spectrogram) data;
	/*double frequency = yWC * maximum;*/
	long bestFrame;
	bestFrame = Sampled_xToNearestIndex (spectrogram, xWC);
	if (bestFrame < 1)
		bestFrame = 1;
	else if (bestFrame > spectrogram -> nx)
		bestFrame = spectrogram -> nx;
	return SpectrogramEditor_Parent :: v_click (xWC, yWC, shiftKeyPressed);
}

SpectrogramEditor SpectrogramEditor_create (const wchar_t *title, Spectrogram data) {
	try {
		autoSpectrogramEditor me = Thing_new (SpectrogramEditor);
		FunctionEditor_init (me.peek(), title, data);
		my maximum = 10000.0;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Spectrogram window not created.");
	}
}

/* End of file SpectrogramEditor.cpp */
