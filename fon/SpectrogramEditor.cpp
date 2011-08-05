/* SpectrogramEditor.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

	Graphics_setWindow (graphics, 0, 1, 0, 1);
	Graphics_setColour (graphics, Graphics_WHITE);
	Graphics_fillRectangle (graphics, 0, 1, 0, 1);
	Graphics_setColour (graphics, Graphics_BLACK);
	Graphics_rectangle (graphics, 0, 1, 0, 1);

	long itmin, itmax;
	Sampled_getWindowSamples (spectrogram, startWindow, endWindow, & itmin, & itmax);

	/*
	 * Autoscale frequency axis.
	 */
	maximum = spectrogram -> ymax;

	Graphics_setWindow (graphics, startWindow, endWindow, 0.0, maximum);
	Spectrogram_paintInside (spectrogram, graphics, startWindow, endWindow, 0, 0, 0.0, TRUE,
		 60, 6.0, 0);

	/*
	 * Horizontal scaling lines.
	 */
	Graphics_setWindow (graphics, 0.0, 1.0, 0.0, maximum);
	Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_setColour (graphics, Graphics_RED);
	long df = 1000;
	for (long f = df; f <= maximum; f += df) {
		Graphics_line (graphics, 0.0, f, 1.0, f);
		Graphics_text2 (graphics, -0.01, f, Melder_integer (f), L" Hz");
	}

	/*
	 * Vertical cursor lines.
	 */
	Graphics_setWindow (graphics, startWindow, endWindow, 0.0, maximum);
	if (startSelection > startWindow && startSelection < endWindow)
		Graphics_line (graphics, startSelection, 0, startSelection, maximum);
	if (endSelection > startWindow && endSelection < endWindow)
		Graphics_line (graphics, endSelection, 0, endSelection, maximum);
	Graphics_setColour (graphics, Graphics_BLACK);
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

SpectrogramEditor SpectrogramEditor_create (GuiObject parent, const wchar *title, Spectrogram data) {
	try {
		autoSpectrogramEditor me = Thing_new (SpectrogramEditor);
		FunctionEditor_init (me.peek(), parent, title, data);
		my maximum = 10000.0;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Spectrogram window not created.");
	}
}

/* End of file SpectrogramEditor.cpp */
