/* SpectrogramEditor.cpp
 *
 * Copyright (C) 1992-2005,2007-2012,2014-2020 Paul Boersma
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

#include "SpectrogramEditor.h"

Thing_implement (SpectrogramEditor, FunctionEditor, 0);

void structSpectrogramEditor :: v_draw () {
	Spectrogram spectrogram = (Spectrogram) our data;

	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	Graphics_rectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);

	integer itmin, itmax;
	Sampled_getWindowSamples (spectrogram, our startWindow, our endWindow, & itmin, & itmax);

	/*
		Autoscale frequency axis.
	*/
	our maximum = spectrogram -> ymax;

	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, our maximum);
	Spectrogram_paintInside (spectrogram, our graphics.get(), our startWindow, our endWindow, 0, 0, 0.0, true,
		 60, 6.0, 0);

	/*
		Horizontal scaling lines.
	*/
	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, our maximum);
	Graphics_setTextAlignment (our graphics.get(), Graphics_RIGHT, Graphics_HALF);
	Graphics_setColour (our graphics.get(), Melder_RED);
	integer df = 1000;
	for (integer f = df; f <= our maximum; f += df) {
		Graphics_line (our graphics.get(), 0.0, f, 1.0, f);
		Graphics_text (our graphics.get(), -0.01, f,   f, U" Hz");
	}

	/*
		Vertical cursor lines.
	*/
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, our maximum);
	if (our startSelection > our startWindow && our startSelection < our endWindow)
		Graphics_line (our graphics.get(), our startSelection, 0, our startSelection, our maximum);
	if (our endSelection > our startWindow && our endSelection < our endWindow)
		Graphics_line (our graphics.get(), our endSelection, 0, our endSelection, our maximum);
	Graphics_setColour (our graphics.get(), Melder_BLACK);
}

bool structSpectrogramEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	if (event -> isClick()) {
		Spectrogram spectrogram = (Spectrogram) our data;
		double clickedFrequency = y_fraction * our maximum;
		const integer clickedFrame = Melder_clipped (1_integer, Sampled_xToNearestIndex (spectrogram, x_world), spectrogram -> nx);
		// TODO
	}
	return our SpectrogramEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);
}

autoSpectrogramEditor SpectrogramEditor_create (conststring32 title, Spectrogram data) {
	try {
		autoSpectrogramEditor me = Thing_new (SpectrogramEditor);
		FunctionEditor_init (me.get(), title, data);
		my maximum = 10000.0;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Spectrogram window not created.");
	}
}

/* End of file SpectrogramEditor.cpp */
