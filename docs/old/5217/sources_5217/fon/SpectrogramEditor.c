/* SpectrogramEditor.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2004/04/13 less flashing
 * pb 2007/06/10 wchar_t
 * pb 2007/11/30 erased Graphics_printf
 */

#include "SpectrogramEditor.h"

static void draw (SpectrogramEditor me) {
	Spectrogram spectrogram = my data;
	long itmin, itmax;

	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_rectangle (my graphics, 0, 1, 0, 1);

	Sampled_getWindowSamples (spectrogram,
		my startWindow, my endWindow, & itmin, & itmax);

	/* Autoscale frequency axis. */
	my maximum = spectrogram -> ymax;

	Graphics_setWindow (my graphics, my startWindow, my endWindow, 0, my maximum);
	Spectrogram_paintInside (spectrogram, my graphics, my startWindow, my endWindow, 0, 0, 0.0, TRUE,
		 60, 6.0, 0);

	/* Horizontal scaling lines. */
	{
		long f, df = 1000;
		Graphics_setWindow (my graphics, 0, 1, 0, my maximum);
		Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
		Graphics_setColour (my graphics, Graphics_RED);
		for (f = df; f <= my maximum; f += df) {
			Graphics_line (my graphics, 0, f, 1, f);
			Graphics_text2 (my graphics, -0.01, f, Melder_integer (f), L" Hz");
		}
	}
	/* Vertical cursor lines. */
	Graphics_setWindow (my graphics, my startWindow, my endWindow, 0, my maximum);
	if (my startSelection > my startWindow && my startSelection < my endWindow)
		Graphics_line (my graphics, my startSelection, 0, my startSelection, my maximum);
	if (my endSelection > my startWindow && my endSelection < my endWindow)
		Graphics_line (my graphics, my endSelection, 0, my endSelection, my maximum);
	Graphics_setColour (my graphics, Graphics_BLACK);
}

static int click (SpectrogramEditor me, double xWC, double yWC, int shiftKeyPressed) {
	Spectrogram spectrogram = my data;
	/*double frequency = yWC * my maximum;*/
	long bestFrame;
	bestFrame = Sampled_xToNearestIndex (spectrogram, xWC);
	if (bestFrame < 1)
		bestFrame = 1;
	else if (bestFrame > spectrogram -> nx)
		bestFrame = spectrogram -> nx;
	return inherited (SpectrogramEditor) click (SpectrogramEditor_as_parent (me), xWC, yWC, shiftKeyPressed);
}

class_methods (SpectrogramEditor, FunctionEditor) {
	class_method (draw)
	class_method (click)
	class_methods_end
}

SpectrogramEditor SpectrogramEditor_create (GuiObject parent, const wchar_t *title, Any data) {
	SpectrogramEditor me = new (SpectrogramEditor); cherror
	FunctionEditor_init (SpectrogramEditor_as_parent (me), parent, title, data); cherror
	my maximum = 10000;
end:
	iferror forget (me);
	return me;
}

/* End of file SpectrogramEditor.c */
