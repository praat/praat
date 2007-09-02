/* FunctionEditor_Sound.c
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
 * pb 2002/11/19 pulses
 * pb 2007/01/27 accept stereo Sounds
 * pb 2007/06/10 wchar_t
 * pb 2007/08/12 wchar_t
 */

#include "FunctionEditor_Sound.h"
#include "FunctionEditor_SoundAnalysis.h"
#include "Preferences.h"
#include "EditorM.h"

static struct {
	int autoscaling;
}
	preferences = { TRUE };

void FunctionEditor_Sound_prefs (void) {
	Resources_addInt (L"FunctionEditor.sound.autoscaling", & preferences.autoscaling);
}

/***** VIEW MENU *****/

DIRECT (FunctionEditor, cb_autoscaling)
	preferences.autoscaling = my sound.autoscaling = ! my sound.autoscaling;
	FunctionEditor_redraw (me);
END

void FunctionEditor_Sound_draw (I, double globalMinimum, double globalMaximum) {
	iam (FunctionEditor);
	Sound sound = my sound.data;
	LongSound longSound = my longSound.data;
	long first, last;
	int fits = sound ? TRUE : LongSound_haveWindow (longSound, my startWindow, my endWindow);
	int nchan = sound ? sound -> ny : longSound -> numberOfChannels, ichan;
	int cursorVisible = my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow;
	double cursorFunctionValue = longSound ? 0.0 :
		Vector_getValueAtX (sound, 0.5 * (my startSelection + my endSelection), Vector_CHANNEL_AVERAGE, 70);
	double tfirst, tlast;
	Graphics_setColour (my graphics, Graphics_BLACK);
	iferror {
		int outOfMemory = wcsstr (Melder_getError (), L"memory") != NULL;
		if (Melder_debug == 9) Melder_flushError (NULL); else Melder_clearError ();
		Graphics_setWindow (my graphics, 0, 1, 0, 1);
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics, 0.5, 0.5, outOfMemory ? L"(out of memory)" : L"(cannot read sound file)");
		return;
	}
	if (! fits) {
		Graphics_setWindow (my graphics, 0, 1, 0, 1);
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics, 0.5, 0.5, L"(window too large; zoom in to see the data)");
		return;
	}
	if (Sampled_getWindowSamples (sound ? (Sampled) sound : (Sampled) longSound, my startWindow, my endWindow, & first, & last) <= 1) {
		Graphics_setWindow (my graphics, 0, 1, 0, 1);
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics, 0.5, 0.5, L"(zoom out to see the data)");
		return;
	}
	if (longSound) tfirst = Sampled_indexToX (longSound, first), tlast = Sampled_indexToX (longSound, last);
	for (ichan = 1; ichan <= nchan; ichan ++) {
		/*
		 * BUG: this will only work for mono or stereo, until Graphics_function16 handles quadro.
		 */
		double ymin = (double) (nchan - ichan) / nchan;
		double ymax = (double) (nchan + 1 - ichan) / nchan;
		Graphics_Viewport vp = Graphics_insetViewport (my graphics, 0, 1, ymin, ymax);
		long horizontal = 0;
		double minimum = sound ? globalMinimum : -1.0, maximum = sound ? globalMaximum : 1.0, value;
		if (my sound.autoscaling) {
			if (longSound)
				LongSound_getWindowExtrema (longSound, my startWindow, my endWindow, ichan, & minimum, & maximum);
			else
				Matrix_getWindowExtrema (sound, first, last, ichan, ichan, & minimum, & maximum);
		}
		if (minimum == maximum) { horizontal = 1; value = minimum; minimum -= 1; maximum += 1;}
		Graphics_setWindow (my graphics, my startWindow, my endWindow, minimum, maximum);
		if (horizontal) {
			Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
			Graphics_text1 (my graphics, my startWindow, value, Melder_half (value));
		} else {
			if (! cursorVisible || Graphics_dyWCtoMM (my graphics, cursorFunctionValue - minimum) > 5.0) {
				Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_BOTTOM);
				Graphics_text1 (my graphics, my startWindow, minimum, Melder_half (minimum));
			}
			if (! cursorVisible || Graphics_dyWCtoMM (my graphics, maximum - cursorFunctionValue) > 5.0) {
				Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_TOP);
				Graphics_text1 (my graphics, my startWindow, maximum, Melder_half (maximum));
			}
		}
		if (minimum < 0 && maximum > 0 && ! horizontal) {
			Graphics_setWindow (my graphics, 0, 1, minimum, maximum);
			if (! cursorVisible || fabs (Graphics_dyWCtoMM (my graphics, cursorFunctionValue - 0.0)) > 3.0) {
				Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
				Graphics_text (my graphics, 0, 0, L"0");
			}
			Graphics_setColour (my graphics, Graphics_CYAN);
			Graphics_setLineType (my graphics, Graphics_DOTTED);
			Graphics_line (my graphics, 0, 0, 1, 0);
			Graphics_setLineType (my graphics, Graphics_DRAWN);
		}
		/*
		 * Garnish the drawing area of each channel.
		 */
		Graphics_setWindow (my graphics, 0, 1, 0, 1);
		Graphics_setColour (my graphics, Graphics_CYAN);
		Graphics_innerRectangle (my graphics, 0, 1, 0, 1);
		Graphics_setColour (my graphics, Graphics_BLACK);
		/*
		 * Draw a very thin separator line underneath.
		 */
		if (ichan < nchan) {
			/*Graphics_setColour (my graphics, Graphics_BLACK);*/
			Graphics_line (my graphics, 0, 0, 1, 0);
		}
		/*
		 * Draw the samples.
		 */
		/*if (ichan == 1) FunctionEditor_SoundAnalysis_drawPulses (me);*/
		if (sound) {
			Graphics_setWindow (my graphics, my startWindow, my endWindow, minimum, maximum);
			if (cursorVisible)
				FunctionEditor_drawCursorFunctionValue (me, L"%.4g", cursorFunctionValue);
			Graphics_setColour (my graphics, Graphics_BLACK);
			Graphics_function (my graphics, sound -> z [ichan], first, last,
				Sampled_indexToX (sound, first), Sampled_indexToX (sound, last));
		} else {
			Graphics_setWindow (my graphics, my startWindow, my endWindow, minimum * 32768, maximum * 32768);
			Graphics_function16 (my graphics,
				longSound -> buffer - longSound -> imin * nchan + (ichan - 1), nchan - 1, first, last, tfirst, tlast);
		}
		Graphics_resetViewport (my graphics, vp);
	}
	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_rectangle (my graphics, 0, 1, 0, 1);
}

void FunctionEditor_Sound_createMenus (I) {
	iam (FunctionEditor);
	Editor_addCommand (me, L"View", L"Sound autoscaling", motif_CHECKABLE | (preferences.autoscaling ? motif_CHECKED : 0), cb_autoscaling);
	Editor_addCommand (me, L"View", L"-- sound view --", 0, 0);
}

void FunctionEditor_Sound_init (I) {
	iam (FunctionEditor);
	my sound.autoscaling = preferences.autoscaling;
}

/* End of file FunctionEditor_Sound.c */
