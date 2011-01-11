/* TimeSoundEditor.c
 *
 * Copyright (C) 1992-2010 Paul Boersma
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
 * pb 2004/10/16 C++ compatible struct tags
 * pb 2007/06/10 wchar_t
 * pb 2007/08/12 wchar_t
 * pb 2007/09/19 info
 * pb 2007/09/22 Draw visible sound
 * pb 2010/12/08 
 */

#include "TimeSoundEditor.h"
#include "Preferences.h"
#include "EditorM.h"

/********** PREFERENCES **********/

static struct {
	struct TimeSoundEditor_sound sound;
	struct {
		bool preserveTimes;
		double bottom, top;
		bool garnish;
	} picture;
	struct {
		int windowShape;
		double relativeWidth;
		bool preserveTimes;
	} extract;
}
	preferences;

void TimeSoundEditor_prefs (void) {
	Preferences_addBool (L"TimeSoundEditor.sound.autoscaling", & preferences.sound.autoscaling, true);
	Preferences_addBool (L"TimeSoundEditor.picture.preserveTimes", & preferences.picture.preserveTimes, true);
	Preferences_addDouble (L"TimeSoundEditor.picture.bottom", & preferences.picture.bottom, 0.0);
	Preferences_addDouble (L"TimeSoundEditor.picture.top", & preferences.picture.top, 0.0);
	Preferences_addBool (L"TimeSoundEditor.picture.garnish", & preferences.picture.garnish, true);
	Preferences_addEnum (L"TimeSoundEditor.extract.windowShape", & preferences.extract.windowShape, kSound_windowShape, DEFAULT);
	Preferences_addDouble (L"TimeSoundEditor.extract.relativeWidth", & preferences.extract.relativeWidth, 1.0);
	Preferences_addBool (L"TimeSoundEditor.extract.preserveTimes", & preferences.extract.preserveTimes, true);
}

/********** Thing methods **********/

static void destroy (I) {
	iam (TimeSoundEditor);
	if (my ownSound) forget (my sound.data);
	inherited (TimeSoundEditor) destroy (me);
}

static void info (I) {
	iam (TimeSoundEditor);
	inherited (TimeSoundEditor) info (me);
	/* Sound flag: */
	MelderInfo_writeLine2 (L"Sound autoscaling: ", Melder_boolean (my sound.autoscaling));
}

/***** FILE MENU *****/

static int menu_cb_DrawVisibleSound (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM (L"Draw visible sound", 0)
		our form_pictureWindow (me, cmd);
		LABEL (L"", L"Sound:")
		BOOLEAN (L"Preserve times", 1);
		REAL (L"left Vertical range", L"0.0")
		REAL (L"right Vertical range", L"0.0 (= auto)")
		our form_pictureMargins (me, cmd);
		our form_pictureSelection (me, cmd);
		BOOLEAN (L"Garnish", 1);
	EDITOR_OK
		our ok_pictureWindow (me, cmd);
		SET_INTEGER (L"Preserve times", preferences.picture.preserveTimes);
		SET_REAL (L"left Vertical range", preferences.picture.bottom);
		SET_REAL (L"right Vertical range", preferences.picture.top);
		our ok_pictureMargins (me, cmd);
		our ok_pictureSelection (me, cmd);
		SET_INTEGER (L"Garnish", preferences.picture.garnish);
	EDITOR_DO
		our do_pictureWindow (me, cmd);
		preferences.picture.preserveTimes = GET_INTEGER (L"Preserve times");
		preferences.picture.bottom = GET_REAL (L"left Vertical range");
		preferences.picture.top = GET_REAL (L"right Vertical range");
		our do_pictureMargins (me, cmd);
		our do_pictureSelection (me, cmd);
		preferences.picture.garnish = GET_INTEGER (L"Garnish");
		if (my longSound.data == NULL && my sound.data == NULL)
			return Melder_error1 (L"There is no sound to draw.");
		Sound publish = my longSound.data ?
			LongSound_extractPart (my longSound.data, my startWindow, my endWindow, preferences.picture.preserveTimes) :
			Sound_extractPart (my sound.data, my startWindow, my endWindow, kSound_windowShape_RECTANGULAR, 1.0, preferences.picture.preserveTimes);
		if (! publish) return 0;
		Editor_openPraatPicture (TimeSoundEditor_as_Editor (me));
		Sound_draw (publish, my pictureGraphics, 0.0, 0.0, preferences.picture.bottom, preferences.picture.top,
			preferences.picture.garnish, L"Curve");
		forget (publish);
		FunctionEditor_garnish (TimeSoundEditor_as_FunctionEditor (me));
		Editor_closePraatPicture (TimeSoundEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_DrawSelectedSound (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM (L"Draw selected sound", 0)
		our form_pictureWindow (me, cmd);
		LABEL (L"", L"Sound:")
		BOOLEAN (L"Preserve times", 1);
		REAL (L"left Vertical range", L"0.0")
		REAL (L"right Vertical range", L"0.0 (= auto)")
		our form_pictureMargins (me, cmd);
		BOOLEAN (L"Garnish", 1);
	EDITOR_OK
		our ok_pictureWindow (me, cmd);
		SET_INTEGER (L"Preserve times", preferences.picture.preserveTimes);
		SET_REAL (L"left Vertical range", preferences.picture.bottom);
		SET_REAL (L"right Vertical range", preferences.picture.top);
		our ok_pictureMargins (me, cmd);
		SET_INTEGER (L"Garnish", preferences.picture.garnish);
	EDITOR_DO
		our do_pictureWindow (me, cmd);
		preferences.picture.preserveTimes = GET_INTEGER (L"Preserve times");
		preferences.picture.bottom = GET_REAL (L"left Vertical range");
		preferences.picture.top = GET_REAL (L"right Vertical range");
		our do_pictureMargins (me, cmd);
		preferences.picture.garnish = GET_INTEGER (L"Garnish");
		if (my longSound.data == NULL && my sound.data == NULL)
			return Melder_error1 (L"There is no sound to draw.");
		Sound publish = my longSound.data ?
			LongSound_extractPart (my longSound.data, my startSelection, my endSelection, preferences.picture.preserveTimes) :
			Sound_extractPart (my sound.data, my startSelection, my endSelection, kSound_windowShape_RECTANGULAR, 1.0, preferences.picture.preserveTimes);
		if (! publish) return 0;
		Editor_openPraatPicture (TimeSoundEditor_as_Editor (me));
		Sound_draw (publish, my pictureGraphics, 0.0, 0.0, preferences.picture.bottom, preferences.picture.top,
			preferences.picture.garnish, L"Curve");
		forget (publish);
		Editor_closePraatPicture (TimeSoundEditor_as_Editor (me));
	EDITOR_END
}

static int do_ExtractSelectedSound (TimeSoundEditor me, bool preserveTimes) {
	Sound extract = NULL;
	if (my endSelection <= my startSelection) return Melder_error1 (L"No selection.");
	if (my longSound.data) {
		extract = LongSound_extractPart (my longSound.data, my startSelection, my endSelection, preserveTimes);
		iferror return 0;
	} else if (my sound.data) {
		extract = Sound_extractPart (my sound.data, my startSelection, my endSelection,
			kSound_windowShape_RECTANGULAR, 1.0, preserveTimes);
		iferror return 0;
	}
	Melder_assert (extract != NULL);
	if (my publishCallback)
		my publishCallback (me, my publishClosure, extract);
	return 1;
}

static int menu_cb_ExtractSelectedSound_timeFromZero (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	return do_ExtractSelectedSound (me, FALSE);
}

static int menu_cb_ExtractSelectedSound_preserveTimes (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	return do_ExtractSelectedSound (me, TRUE);
}

static int menu_cb_ExtractSelectedSound_windowed (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM (L"Extract selected sound (windowed)", 0)
		WORD (L"Name", L"slice")
		OPTIONMENU_ENUM (L"Window shape", kSound_windowShape, HANNING)
		POSITIVE (L"Relative width", L"1.0")
		BOOLEAN (L"Preserve times", 1)
	EDITOR_OK
		SET_ENUM (L"Window shape", kSound_windowShape, preferences.extract.windowShape)
		SET_REAL (L"Relative width", preferences.extract.relativeWidth)
		SET_INTEGER (L"Preserve times", preferences.extract.preserveTimes)
	EDITOR_DO
		Sound sound = my sound.data;
		Melder_assert (sound != NULL);
		preferences.extract.windowShape = GET_ENUM (kSound_windowShape, L"Window shape");
		preferences.extract.relativeWidth = GET_REAL (L"Relative width");
		preferences.extract.preserveTimes = GET_INTEGER (L"Preserve times");
		Sound extract = Sound_extractPart (sound, my startSelection, my endSelection, preferences.extract.windowShape,
			preferences.extract.relativeWidth, preferences.extract.preserveTimes);
		if (! extract) return 0;
		Thing_setName (extract, GET_STRING (L"Name"));
		if (my publishCallback)
			my publishCallback (me, my publishClosure, extract);
	EDITOR_END
}

static int do_write (TimeSoundEditor me, MelderFile file, int format) {
	if (my startSelection >= my endSelection)
		return Melder_error1 (L"No samples selected.");
	if (my longSound.data) {
		return LongSound_writePartToAudioFile16 (my longSound.data, format, my startSelection, my endSelection, file);
	} else if (my sound.data) {
		Sound sound = my sound.data;
		double margin = 0.0;
		long nmargin = margin / sound -> dx;
		long first, last, numberOfSamples = Sampled_getWindowSamples (sound,
			my startSelection, my endSelection, & first, & last) + nmargin * 2;
		first -= nmargin;
		last += nmargin;
		if (numberOfSamples) {
			Sound save = Sound_create (sound -> ny, 0.0, numberOfSamples * sound -> dx,
							numberOfSamples, sound -> dx, 0.5 * sound -> dx);
			if (! save) return 0;
			long offset = first - 1;
			if (first < 1) first = 1;
			if (last > sound -> nx) last = sound -> nx;
			for (long channel = 1; channel <= sound -> ny; channel ++) {
				for (long i = first; i <= last; i ++) {
					save -> z [channel] [i - offset] = sound -> z [channel] [i];
				}
			}
			int result = Sound_writeToAudioFile16 (save, file, format);
			forget (save);
			return result;
		}
	}
	return 0;
}

static int menu_cb_WriteWav (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Write selected sound to WAV file", 0)
		swprintf (defaultName, 300, L"%ls.wav", my longSound.data ? my longSound.data -> name : my sound.data -> name);
	EDITOR_DO_WRITE
		if (! do_write (me, file, Melder_WAV)) return 0;
	EDITOR_END
}

static int menu_cb_WriteAiff (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Write selected sound to AIFF file", 0)
		swprintf (defaultName, 300, L"%ls.aiff", my longSound.data ? my longSound.data -> name : my sound.data -> name);
	EDITOR_DO_WRITE
		if (! do_write (me, file, Melder_AIFF)) return 0;
	EDITOR_END
}

static int menu_cb_WriteAifc (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Write selected sound to AIFC file", 0)
		swprintf (defaultName, 300, L"%ls.aifc", my longSound.data ? my longSound.data -> name : my sound.data -> name);
	EDITOR_DO_WRITE
		if (! do_write (me, file, Melder_AIFC)) return 0;
	EDITOR_END
}

static int menu_cb_WriteNextSun (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Write selected sound to NeXT/Sun file", 0)
		swprintf (defaultName, 300, L"%ls.au", my longSound.data ? my longSound.data -> name : my sound.data -> name);
	EDITOR_DO_WRITE
		if (! do_write (me, file, Melder_NEXT_SUN)) return 0;
	EDITOR_END
}

static int menu_cb_WriteNist (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Write selected sound to NIST file", 0)
		swprintf (defaultName, 300, L"%ls.nist", my longSound.data ? my longSound.data -> name : my sound.data -> name);
	EDITOR_DO_WRITE
		if (! do_write (me, file, Melder_NIST)) return 0;
	EDITOR_END
}

static int menu_cb_WriteFlac (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Write selected sound to FLAC file", 0)
		swprintf (defaultName, 300, L"%ls.flac", my longSound.data ? my longSound.data -> name : my sound.data -> name);
	EDITOR_DO_WRITE
		if (! do_write (me, file, Melder_FLAC)) return 0;
	EDITOR_END
}

static void createMenuItems_file_draw (TimeSoundEditor me, EditorMenu menu) {
	EditorMenu_addCommand (menu, L"Draw to picture window:", GuiMenu_INSENSITIVE, menu_cb_DrawVisibleSound /* dummy */);
	if (my sound.data || my longSound.data) {
		EditorMenu_addCommand (menu, L"Draw visible sound...", 0, menu_cb_DrawVisibleSound);
		my drawButton = EditorMenu_addCommand (menu, L"Draw selected sound...", 0, menu_cb_DrawSelectedSound);
	}
}

static void createMenuItems_file_extract (TimeSoundEditor me, EditorMenu menu) {
	EditorMenu_addCommand (menu, L"Extract to objects window:", GuiMenu_INSENSITIVE, menu_cb_ExtractSelectedSound_preserveTimes /* dummy */);
	if (my sound.data || my longSound.data) {
		my publishPreserveButton = EditorMenu_addCommand (menu, L"Extract selected sound (preserve times)", 0, menu_cb_ExtractSelectedSound_preserveTimes);
			EditorMenu_addCommand (menu, L"Extract sound selection (preserve times)", Editor_HIDDEN, menu_cb_ExtractSelectedSound_preserveTimes);
			EditorMenu_addCommand (menu, L"Extract selection (preserve times)", Editor_HIDDEN, menu_cb_ExtractSelectedSound_preserveTimes);
		my publishButton = EditorMenu_addCommand (menu, L"Extract selected sound (time from 0)", 0, menu_cb_ExtractSelectedSound_timeFromZero);
			EditorMenu_addCommand (menu, L"Extract sound selection (time from 0)", Editor_HIDDEN, menu_cb_ExtractSelectedSound_timeFromZero);
			EditorMenu_addCommand (menu, L"Extract selection (time from 0)", Editor_HIDDEN, menu_cb_ExtractSelectedSound_timeFromZero);
			EditorMenu_addCommand (menu, L"Extract selection", Editor_HIDDEN, menu_cb_ExtractSelectedSound_timeFromZero);
		if (my sound.data) {
			my publishWindowButton = EditorMenu_addCommand (menu, L"Extract selected sound (windowed)...", 0, menu_cb_ExtractSelectedSound_windowed);
				EditorMenu_addCommand (menu, L"Extract windowed sound selection...", Editor_HIDDEN, menu_cb_ExtractSelectedSound_windowed);
				EditorMenu_addCommand (menu, L"Extract windowed selection...", Editor_HIDDEN, menu_cb_ExtractSelectedSound_windowed);
		}
	}
}

static void createMenuItems_file_write (TimeSoundEditor me, EditorMenu menu) {
	EditorMenu_addCommand (menu, L"Save to disk:", GuiMenu_INSENSITIVE, menu_cb_WriteWav /* dummy */);
	if (my sound.data || my longSound.data) {
		my writeWavButton = EditorMenu_addCommand (menu, L"Write selected sound to WAV file...", 0, menu_cb_WriteWav);
			EditorMenu_addCommand (menu, L"Write sound selection to WAV file...", Editor_HIDDEN, menu_cb_WriteWav);
			EditorMenu_addCommand (menu, L"Write selection to WAV file...", Editor_HIDDEN, menu_cb_WriteWav);
		my writeAiffButton = EditorMenu_addCommand (menu, L"Write selected sound to AIFF file...", 0, menu_cb_WriteAiff);
			EditorMenu_addCommand (menu, L"Write sound selection to AIFF file...", Editor_HIDDEN, menu_cb_WriteAiff);
			EditorMenu_addCommand (menu, L"Write selection to AIFF file...", Editor_HIDDEN, menu_cb_WriteAiff);
		my writeAifcButton = EditorMenu_addCommand (menu, L"Write selected sound to AIFC file...", 0, menu_cb_WriteAifc);
			EditorMenu_addCommand (menu, L"Write sound selection to AIFC file...", Editor_HIDDEN, menu_cb_WriteAifc);
			EditorMenu_addCommand (menu, L"Write selection to AIFC file...", Editor_HIDDEN, menu_cb_WriteAifc);
		my writeNextSunButton = EditorMenu_addCommand (menu, L"Write selected sound to Next/Sun file...", 0, menu_cb_WriteNextSun);
			EditorMenu_addCommand (menu, L"Write sound selection to Next/Sun file...", Editor_HIDDEN, menu_cb_WriteNextSun);
			EditorMenu_addCommand (menu, L"Write selection to Next/Sun file...", Editor_HIDDEN, menu_cb_WriteNextSun);
		my writeNistButton = EditorMenu_addCommand (menu, L"Write selected sound to NIST file...", 0, menu_cb_WriteNist);
			EditorMenu_addCommand (menu, L"Write sound selection to NIST file...", Editor_HIDDEN, menu_cb_WriteNist);
			EditorMenu_addCommand (menu, L"Write selection to NIST file...", Editor_HIDDEN, menu_cb_WriteNist);
		my writeFlacButton = EditorMenu_addCommand (menu, L"Write selected sound to FLAC file...", 0, menu_cb_WriteFlac);
			EditorMenu_addCommand (menu, L"Write sound selection to FLAC file...", Editor_HIDDEN, menu_cb_WriteFlac);
	}
}

static void createMenuItems_file (TimeSoundEditor me, EditorMenu menu) {
	inherited (TimeSoundEditor) createMenuItems_file (TimeSoundEditor_as_parent (me), menu);
	our createMenuItems_file_draw (me, menu);
	EditorMenu_addCommand (menu, L"-- after file draw --", 0, NULL);
	our createMenuItems_file_extract (me, menu);
	EditorMenu_addCommand (menu, L"-- after file extract --", 0, NULL);
	our createMenuItems_file_write (me, menu);
	EditorMenu_addCommand (menu, L"-- after file write --", 0, NULL);
}

/********** QUERY MENU **********/

static int menu_cb_SoundInfo (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	Thing_info (my sound.data);
	return 1;
}

static int menu_cb_LongSoundInfo (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	Thing_info (my longSound.data);
	return 1;
}

static void createMenuItems_query_info (TimeSoundEditor me, EditorMenu menu) {
	inherited (TimeSoundEditor) createMenuItems_query_info (TimeSoundEditor_as_parent (me), menu);
	if (my sound.data != NULL && my sound.data != my data) {
		EditorMenu_addCommand (menu, L"Sound info", 0, menu_cb_SoundInfo);
	} else if (my longSound.data != NULL && my longSound.data != my data) {
		EditorMenu_addCommand (menu, L"LongSound info", 0, menu_cb_LongSoundInfo);
	}
}

/********** VIEW MENU **********/

static int menu_cb_autoscaling (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	preferences.sound.autoscaling = my sound.autoscaling = ! my sound.autoscaling;
	FunctionEditor_redraw (TimeSoundEditor_as_FunctionEditor (me));
	return 1;
}

static void createMenuItems_view (TimeSoundEditor me, EditorMenu menu) {
	if (my sound.data || my longSound.data) our createMenuItems_view_sound (me, menu);
	inherited (TimeSoundEditor) createMenuItems_view (TimeSoundEditor_as_parent (me), menu);
}

static void createMenuItems_view_sound (TimeSoundEditor me, EditorMenu menu) {
	(void) me;
	EditorMenu_addCommand (menu, L"Sound autoscaling", GuiMenu_CHECKBUTTON | (preferences.sound.autoscaling ? GuiMenu_TOGGLE_ON : 0), menu_cb_autoscaling);
	EditorMenu_addCommand (menu, L"-- sound view --", 0, 0);
}

static void updateMenuItems_file (TimeSoundEditor me) {
	Any sound = my sound.data != NULL ? (Sampled) my sound.data : (Sampled) my longSound.data;
	if (sound == NULL) return;
	long first, last, selectedSamples = Sampled_getWindowSamples (sound, my startSelection, my endSelection, & first, & last);
	if (my drawButton) {
		GuiObject_setSensitive (my drawButton, selectedSamples != 0);
		GuiObject_setSensitive (my publishButton, selectedSamples != 0);
		GuiObject_setSensitive (my publishPreserveButton, selectedSamples != 0);
		if (my publishWindowButton) GuiObject_setSensitive (my publishWindowButton, selectedSamples != 0);
	}
	GuiObject_setSensitive (my writeWavButton, selectedSamples != 0);
	GuiObject_setSensitive (my writeAiffButton, selectedSamples != 0);
	GuiObject_setSensitive (my writeAifcButton, selectedSamples != 0);
	GuiObject_setSensitive (my writeNextSunButton, selectedSamples != 0);
	GuiObject_setSensitive (my writeNistButton, selectedSamples != 0);
	GuiObject_setSensitive (my writeFlacButton, selectedSamples != 0);
}

void TimeSoundEditor_draw_sound (TimeSoundEditor me, double globalMinimum, double globalMaximum) {
	Sound sound = my sound.data;
	LongSound longSound = my longSound.data;
	Melder_assert ((sound == NULL) != (longSound == NULL));
	int fits = sound ? TRUE : LongSound_haveWindow (longSound, my startWindow, my endWindow);
	int nchan = sound ? sound -> ny : longSound -> numberOfChannels;
	int cursorVisible = my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow;
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
	long first, last;
	if (Sampled_getWindowSamples (sound ? (Sampled) sound : (Sampled) longSound, my startWindow, my endWindow, & first, & last) <= 1) {
		Graphics_setWindow (my graphics, 0, 1, 0, 1);
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics, 0.5, 0.5, L"(zoom out to see the data)");
		return;
	}
	for (int ichan = 1; ichan <= nchan; ichan ++) {
		double cursorFunctionValue = longSound ? 0.0 :
			Vector_getValueAtX (sound, 0.5 * (my startSelection + my endSelection), ichan, 70);
		/*
		 * BUG: this will only work for mono or stereo, until Graphics_function16 handles quadro.
		 */
		double ymin = (double) (nchan - ichan) / nchan;
		double ymax = (double) (nchan + 1 - ichan) / nchan;
		Graphics_Viewport vp = Graphics_insetViewport (my graphics, 0, 1, ymin, ymax);
		bool horizontal = false;
		double minimum = sound ? globalMinimum : -1.0, maximum = sound ? globalMaximum : 1.0;
		if (my sound.autoscaling) {
			if (longSound)
				LongSound_getWindowExtrema (longSound, my startWindow, my endWindow, ichan, & minimum, & maximum);
			else
				Matrix_getWindowExtrema (sound, first, last, ichan, ichan, & minimum, & maximum);
		}
		if (minimum == maximum) { horizontal = true; minimum -= 1; maximum += 1;}
		Graphics_setWindow (my graphics, my startWindow, my endWindow, minimum, maximum);
		if (horizontal) {
			Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
			double mid = 0.5 * (minimum + maximum);
			Graphics_text1 (my graphics, my startWindow, mid, Melder_half (mid));
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
				FunctionEditor_drawCursorFunctionValue (TimeSoundEditor_as_FunctionEditor (me), cursorFunctionValue, Melder_float (Melder_half (cursorFunctionValue)), L"");
			Graphics_setColour (my graphics, Graphics_BLACK);
			Graphics_function (my graphics, sound -> z [ichan], first, last,
				Sampled_indexToX (sound, first), Sampled_indexToX (sound, last));
		} else {
			Graphics_setWindow (my graphics, my startWindow, my endWindow, minimum * 32768, maximum * 32768);
			Graphics_function16 (my graphics,
				longSound -> buffer - longSound -> imin * nchan + (ichan - 1), nchan - 1, first, last,
				Sampled_indexToX (longSound, first), Sampled_indexToX (longSound, last));
		}
		Graphics_resetViewport (my graphics, vp);
	}
	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_rectangle (my graphics, 0, 1, 0, 1);
}

class_methods (TimeSoundEditor, FunctionEditor) {
	class_method (destroy)
	class_method (info)
	class_method (createMenuItems_file)
	class_method (createMenuItems_file_draw)
	class_method (createMenuItems_file_extract)
	class_method (createMenuItems_file_write)
	class_method (createMenuItems_query_info)
	class_method (createMenuItems_view)
	class_method (createMenuItems_view_sound)
	class_method (updateMenuItems_file)
	class_methods_end
}

int TimeSoundEditor_init (TimeSoundEditor me, GuiObject parent, const wchar_t *title, Any data, Any sound, bool ownSound) {
	my ownSound = ownSound;
	if (sound != NULL) {
		if (ownSound) {
			Melder_assert (Thing_member (sound, classSound));
			my sound.data = Data_copy (sound); cherror   // Deep copy; ownership transferred.
			Matrix_getWindowExtrema (sound, 1, my sound.data -> nx, 1, my sound.data -> ny, & my sound.minimum, & my sound.maximum);
		} else if (Thing_member (sound, classSound)) {
			my sound.data = sound;   // Reference copy; ownership not transferred.
			Matrix_getWindowExtrema (sound, 1, my sound.data -> nx, 1, my sound.data -> ny, & my sound.minimum, & my sound.maximum);
		} else if (Thing_member (sound, classLongSound)) {
			my longSound.data = sound;
			my sound.minimum = -1.0, my sound.maximum = 1.0;
		} else {
			Melder_fatal ("Invalid sound class in TimeSoundEditor_init.");
		}
	}
	FunctionEditor_init (TimeSoundEditor_as_parent (me), parent, title, data); cherror
	my sound.autoscaling = preferences.sound.autoscaling;
end:
	iferror return 0;
	return 1;
}

/* End of file FunctionEditor.c */
