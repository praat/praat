/* TimeSoundEditor.cpp
 *
 * Copyright (C) 1992-2012,2013 Paul Boersma
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

#include "TimeSoundEditor.h"
#include "EditorM.h"
#include "UnicodeData.h"

#include "enums_getText.h"
#include "TimeSoundEditor_enums.h"
#include "enums_getValue.h"
#include "TimeSoundEditor_enums.h"

Thing_implement (TimeSoundEditor, FunctionEditor, 0);

#include "prefs_define.h"
#include "TimeSoundEditor_prefs.h"
#include "prefs_install.h"
#include "TimeSoundEditor_prefs.h"
#include "prefs_copyToInstance.h"
#include "TimeSoundEditor_prefs.h"

/********** Thing methods **********/

void structTimeSoundEditor :: v_destroy () {
	if (d_ownSound)
		forget (d_sound.data);
	TimeSoundEditor_Parent :: v_destroy ();
}

void structTimeSoundEditor :: v_info () {
	TimeSoundEditor_Parent :: v_info ();
	/* Sound flags: */
	MelderInfo_writeLine (L"Sound scaling strategy: ", kTimeSoundEditor_scalingStrategy_getText (p_sound_scalingStrategy));
}

/***** FILE MENU *****/

static void menu_cb_DrawVisibleSound (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM (L"Draw visible sound", 0)
		my v_form_pictureWindow (cmd);
		LABEL (L"", L"Sound:")
		BOOLEAN (L"Preserve times", my default_picture_preserveTimes ());
		REAL (L"left Vertical range", my default_picture_bottom ())
		REAL (L"right Vertical range", my default_picture_top ())
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (L"Garnish", my default_picture_garnish ());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		SET_INTEGER (L"Preserve times", my pref_picture_preserveTimes ());
		SET_REAL (L"left Vertical range", my pref_picture_bottom ());
		SET_REAL (L"right Vertical range", my pref_picture_top ());
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_INTEGER (L"Garnish", my pref_picture_garnish ());
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my pref_picture_preserveTimes () = GET_INTEGER (L"Preserve times");
		my pref_picture_bottom () = GET_REAL (L"left Vertical range");
		my pref_picture_top () = GET_REAL (L"right Vertical range");
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my pref_picture_garnish () = GET_INTEGER (L"Garnish");
		if (my d_longSound.data == NULL && my d_sound.data == NULL)
			Melder_throw ("There is no sound to draw.");
		autoSound publish = my d_longSound.data ?
			LongSound_extractPart (my d_longSound.data, my d_startWindow, my d_endWindow, my pref_picture_preserveTimes ()) :
			Sound_extractPart (my d_sound.data, my d_startWindow, my d_endWindow, kSound_windowShape_RECTANGULAR, 1.0, my pref_picture_preserveTimes ());
		Editor_openPraatPicture (me);
		Sound_draw (publish.peek(), my pictureGraphics, 0.0, 0.0, my pref_picture_bottom (), my pref_picture_top (),
			my pref_picture_garnish (), L"Curve");
		FunctionEditor_garnish (me);
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void menu_cb_DrawSelectedSound (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM (L"Draw selected sound", 0)
		my v_form_pictureWindow (cmd);
		LABEL (L"", L"Sound:")
		BOOLEAN (L"Preserve times", my default_picture_preserveTimes ());
		REAL (L"left Vertical range", my default_picture_bottom ());
		REAL (L"right Vertical range", my default_picture_top ());
		my v_form_pictureMargins (cmd);
		BOOLEAN (L"Garnish", my default_picture_garnish ());
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		SET_INTEGER (L"Preserve times", my pref_picture_preserveTimes ());
		SET_REAL (L"left Vertical range", my pref_picture_bottom ());
		SET_REAL (L"right Vertical range", my pref_picture_top ());
		my v_ok_pictureMargins (cmd);
		SET_INTEGER (L"Garnish", my pref_picture_garnish ());
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my pref_picture_preserveTimes () = GET_INTEGER (L"Preserve times");
		my pref_picture_bottom () = GET_REAL (L"left Vertical range");
		my pref_picture_top () = GET_REAL (L"right Vertical range");
		my v_do_pictureMargins (cmd);
		my pref_picture_garnish () = GET_INTEGER (L"Garnish");
		if (my d_longSound.data == NULL && my d_sound.data == NULL)
			Melder_throw ("There is no sound to draw.");
		autoSound publish = my d_longSound.data ?
			LongSound_extractPart (my d_longSound.data, my d_startSelection, my d_endSelection, my pref_picture_preserveTimes ()) :
			Sound_extractPart (my d_sound.data, my d_startSelection, my d_endSelection, kSound_windowShape_RECTANGULAR, 1.0, my pref_picture_preserveTimes ());
		Editor_openPraatPicture (me);
		Sound_draw (publish.peek(), my pictureGraphics, 0.0, 0.0, my pref_picture_bottom (), my pref_picture_top (),
			my pref_picture_garnish (), L"Curve");
		Editor_closePraatPicture (me);
	EDITOR_END
}

static void do_ExtractSelectedSound (TimeSoundEditor me, bool preserveTimes) {
	autoSound extract = NULL;
	if (my d_endSelection <= my d_startSelection)
		Melder_throw ("No selection.");
	if (my d_longSound.data) {
		extract.reset (LongSound_extractPart (my d_longSound.data, my d_startSelection, my d_endSelection, preserveTimes));
	} else if (my d_sound.data) {
		extract.reset (Sound_extractPart (my d_sound.data, my d_startSelection, my d_endSelection, kSound_windowShape_RECTANGULAR, 1.0, preserveTimes));
	}
	my broadcastPublication (extract.transfer());
}

static void menu_cb_ExtractSelectedSound_timeFromZero (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	do_ExtractSelectedSound (me, FALSE);
}

static void menu_cb_ExtractSelectedSound_preserveTimes (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	do_ExtractSelectedSound (me, TRUE);
}

static void menu_cb_ExtractSelectedSound_windowed (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM (L"Extract selected sound (windowed)", 0)
		WORD (L"Name", L"slice")
		OPTIONMENU_ENUM (L"Window shape", kSound_windowShape, my default_extract_windowShape ())
		POSITIVE (L"Relative width", my default_extract_relativeWidth ())
		BOOLEAN (L"Preserve times", my default_extract_preserveTimes ())
	EDITOR_OK
		SET_ENUM (L"Window shape", kSound_windowShape, my pref_extract_windowShape ())
		SET_REAL (L"Relative width", my pref_extract_relativeWidth ())
		SET_INTEGER (L"Preserve times", my pref_extract_preserveTimes ())
	EDITOR_DO
		Sound sound = my d_sound.data;
		Melder_assert (sound != NULL);
		my pref_extract_windowShape () = GET_ENUM (kSound_windowShape, L"Window shape");
		my pref_extract_relativeWidth () = GET_REAL (L"Relative width");
		my pref_extract_preserveTimes () = GET_INTEGER (L"Preserve times");
		autoSound extract = Sound_extractPart (sound, my d_startSelection, my d_endSelection, my pref_extract_windowShape (),
			my pref_extract_relativeWidth (), my pref_extract_preserveTimes ());
		Thing_setName (extract.peek(), GET_STRING (L"Name"));
		my broadcastPublication (extract.transfer());
	EDITOR_END
}

static void menu_cb_ExtractSelectedSoundForOverlap (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM (L"Extract selected sound for overlap)", 0)
		WORD (L"Name", L"slice")
		POSITIVE (L"Overlap (s)", my default_extract_overlap ())
	EDITOR_OK
		SET_REAL (L"Overlap", my pref_extract_overlap ())
	EDITOR_DO
		Sound sound = my d_sound.data;
		Melder_assert (sound != NULL);
		my pref_extract_overlap () = GET_REAL (L"Overlap");
		autoSound extract = Sound_extractPartForOverlap (sound, my d_startSelection, my d_endSelection,
			my pref_extract_overlap ());
		Thing_setName (extract.peek(), GET_STRING (L"Name"));
		my broadcastPublication (extract.transfer());
	EDITOR_END
}

static void do_write (TimeSoundEditor me, MelderFile file, int format, int numberOfBitsPersamplePoint) {
	if (my d_startSelection >= my d_endSelection)
		Melder_throw ("No samples selected.");
	if (my d_longSound.data) {
		LongSound_writePartToAudioFile (my d_longSound.data, format, my d_startSelection, my d_endSelection, file);
	} else if (my d_sound.data) {
		Sound sound = my d_sound.data;
		double margin = 0.0;
		long nmargin = margin / sound -> dx;
		long first, last, numberOfSamples = Sampled_getWindowSamples (sound,
			my d_startSelection, my d_endSelection, & first, & last) + nmargin * 2;
		first -= nmargin;
		last += nmargin;
		if (numberOfSamples) {
			autoSound save = Sound_create (sound -> ny, 0.0, numberOfSamples * sound -> dx, numberOfSamples, sound -> dx, 0.5 * sound -> dx);
			long offset = first - 1;
			if (first < 1) first = 1;
			if (last > sound -> nx) last = sound -> nx;
			for (long channel = 1; channel <= sound -> ny; channel ++) {
				for (long i = first; i <= last; i ++) {
					save -> z [channel] [i - offset] = sound -> z [channel] [i];
				}
			}
			Sound_writeToAudioFile (save.peek(), file, format, numberOfBitsPersamplePoint);
		}
	}
}

static void menu_cb_WriteWav (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as WAV file", 0)
		swprintf (defaultName, 300, L"%ls.wav", my d_longSound.data ? my d_longSound.data -> name : my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_WAV, 16);
	EDITOR_END
}

static void menu_cb_SaveAs24BitWav (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as 24-bit WAV file", 0)
		Melder_assert (my d_longSound.data == NULL && my d_sound.data != NULL);
		swprintf (defaultName, 300, L"%ls.wav", my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_WAV, 24);
	EDITOR_END
}

static void menu_cb_SaveAs32BitWav (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as 32-bit WAV file", 0)
		Melder_assert (my d_longSound.data == NULL && my d_sound.data != NULL);
		swprintf (defaultName, 300, L"%ls.wav", my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_WAV, 32);
	EDITOR_END
}

static void menu_cb_WriteAiff (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as AIFF file", 0)
		swprintf (defaultName, 300, L"%ls.aiff", my d_longSound.data ? my d_longSound.data -> name : my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_AIFF, 16);
	EDITOR_END
}

static void menu_cb_WriteAifc (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as AIFC file", 0)
		swprintf (defaultName, 300, L"%ls.aifc", my d_longSound.data ? my d_longSound.data -> name : my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_AIFC, 16);
	EDITOR_END
}

static void menu_cb_WriteNextSun (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as NeXT/Sun file", 0)
		swprintf (defaultName, 300, L"%ls.au", my d_longSound.data ? my d_longSound.data -> name : my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_NEXT_SUN, 16);
	EDITOR_END
}

static void menu_cb_WriteNist (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as NIST file", 0)
		swprintf (defaultName, 300, L"%ls.nist", my d_longSound.data ? my d_longSound.data -> name : my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_NIST, 16);
	EDITOR_END
}

static void menu_cb_WriteFlac (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM_WRITE (L"Save selected sound as FLAC file", 0)
		swprintf (defaultName, 300, L"%ls.flac", my d_longSound.data ? my d_longSound.data -> name : my d_sound.data -> name);
	EDITOR_DO_WRITE
		do_write (me, file, Melder_FLAC, 16);
	EDITOR_END
}

void structTimeSoundEditor :: v_createMenuItems_file_draw (EditorMenu menu) {
	EditorMenu_addCommand (menu, L"Draw to picture window:", GuiMenu_INSENSITIVE, menu_cb_DrawVisibleSound /* dummy */);
	if (d_sound.data || d_longSound.data) {
		EditorMenu_addCommand (menu, L"Draw visible sound...", 0, menu_cb_DrawVisibleSound);
		drawButton = EditorMenu_addCommand (menu, L"Draw selected sound...", 0, menu_cb_DrawSelectedSound);
	}
}

void structTimeSoundEditor :: v_createMenuItems_file_extract (EditorMenu menu) {
	EditorMenu_addCommand (menu, L"Extract to objects window:", GuiMenu_INSENSITIVE, menu_cb_ExtractSelectedSound_preserveTimes /* dummy */);
	if (d_sound.data || d_longSound.data) {
		publishPreserveButton = EditorMenu_addCommand (menu, L"Extract selected sound (preserve times)", 0, menu_cb_ExtractSelectedSound_preserveTimes);
			EditorMenu_addCommand (menu, L"Extract sound selection (preserve times)", Editor_HIDDEN, menu_cb_ExtractSelectedSound_preserveTimes);
			EditorMenu_addCommand (menu, L"Extract selection (preserve times)", Editor_HIDDEN, menu_cb_ExtractSelectedSound_preserveTimes);
		publishButton = EditorMenu_addCommand (menu, L"Extract selected sound (time from 0)", 0, menu_cb_ExtractSelectedSound_timeFromZero);
			EditorMenu_addCommand (menu, L"Extract sound selection (time from 0)", Editor_HIDDEN, menu_cb_ExtractSelectedSound_timeFromZero);
			EditorMenu_addCommand (menu, L"Extract selection (time from 0)", Editor_HIDDEN, menu_cb_ExtractSelectedSound_timeFromZero);
			EditorMenu_addCommand (menu, L"Extract selection", Editor_HIDDEN, menu_cb_ExtractSelectedSound_timeFromZero);
		if (d_sound.data) {
			publishWindowButton = EditorMenu_addCommand (menu, L"Extract selected sound (windowed)...", 0, menu_cb_ExtractSelectedSound_windowed);
				EditorMenu_addCommand (menu, L"Extract windowed sound selection...", Editor_HIDDEN, menu_cb_ExtractSelectedSound_windowed);
				EditorMenu_addCommand (menu, L"Extract windowed selection...", Editor_HIDDEN, menu_cb_ExtractSelectedSound_windowed);
			publishOverlapButton = EditorMenu_addCommand (menu, L"Extract selected sound for overlap...", 0, menu_cb_ExtractSelectedSoundForOverlap);
		}
	}
}

void structTimeSoundEditor :: v_createMenuItems_file_write (EditorMenu menu) {
	EditorMenu_addCommand (menu, L"Save to disk:", GuiMenu_INSENSITIVE, menu_cb_WriteWav /* dummy */);
	if (d_sound.data || d_longSound.data) {
		writeWavButton = EditorMenu_addCommand (menu, L"Save selected sound as WAV file...", 0, menu_cb_WriteWav);
			EditorMenu_addCommand (menu, L"Write selected sound to WAV file...", Editor_HIDDEN, menu_cb_WriteWav);
			EditorMenu_addCommand (menu, L"Write sound selection to WAV file...", Editor_HIDDEN, menu_cb_WriteWav);
			EditorMenu_addCommand (menu, L"Write selection to WAV file...", Editor_HIDDEN, menu_cb_WriteWav);
		if (d_sound.data) {
			d_saveAs24BitWavButton = EditorMenu_addCommand (menu, L"Save selected sound as 24-bit WAV file...", 0, menu_cb_SaveAs24BitWav);
			d_saveAs32BitWavButton = EditorMenu_addCommand (menu, L"Save selected sound as 32-bit WAV file...", 0, menu_cb_SaveAs32BitWav);
		}
		writeAiffButton = EditorMenu_addCommand (menu, L"Save selected sound as AIFF file...", 0, menu_cb_WriteAiff);
			EditorMenu_addCommand (menu, L"Write selected sound to AIFF file...", Editor_HIDDEN, menu_cb_WriteAiff);
			EditorMenu_addCommand (menu, L"Write sound selection to AIFF file...", Editor_HIDDEN, menu_cb_WriteAiff);
			EditorMenu_addCommand (menu, L"Write selection to AIFF file...", Editor_HIDDEN, menu_cb_WriteAiff);
		writeAifcButton = EditorMenu_addCommand (menu, L"Save selected sound as AIFC file...", 0, menu_cb_WriteAifc);
			EditorMenu_addCommand (menu, L"Write selected sound to AIFC file...", Editor_HIDDEN, menu_cb_WriteAifc);
			EditorMenu_addCommand (menu, L"Write sound selection to AIFC file...", Editor_HIDDEN, menu_cb_WriteAifc);
			EditorMenu_addCommand (menu, L"Write selection to AIFC file...", Editor_HIDDEN, menu_cb_WriteAifc);
		writeNextSunButton = EditorMenu_addCommand (menu, L"Save selected sound as Next/Sun file...", 0, menu_cb_WriteNextSun);
			EditorMenu_addCommand (menu, L"Write selected sound to Next/Sun file...", Editor_HIDDEN, menu_cb_WriteNextSun);
			EditorMenu_addCommand (menu, L"Write sound selection to Next/Sun file...", Editor_HIDDEN, menu_cb_WriteNextSun);
			EditorMenu_addCommand (menu, L"Write selection to Next/Sun file...", Editor_HIDDEN, menu_cb_WriteNextSun);
		writeNistButton = EditorMenu_addCommand (menu, L"Save selected sound as NIST file...", 0, menu_cb_WriteNist);
			EditorMenu_addCommand (menu, L"Write selected sound to NIST file...", Editor_HIDDEN, menu_cb_WriteNist);
			EditorMenu_addCommand (menu, L"Write sound selection to NIST file...", Editor_HIDDEN, menu_cb_WriteNist);
			EditorMenu_addCommand (menu, L"Write selection to NIST file...", Editor_HIDDEN, menu_cb_WriteNist);
		writeFlacButton = EditorMenu_addCommand (menu, L"Save selected sound as FLAC file...", 0, menu_cb_WriteFlac);
			EditorMenu_addCommand (menu, L"Write selected sound to FLAC file...", Editor_HIDDEN, menu_cb_WriteFlac);
			EditorMenu_addCommand (menu, L"Write sound selection to FLAC file...", Editor_HIDDEN, menu_cb_WriteFlac);
	}
}

void structTimeSoundEditor :: v_createMenuItems_file (EditorMenu menu) {
	TimeSoundEditor_Parent :: v_createMenuItems_file (menu);
	v_createMenuItems_file_draw (menu);
	EditorMenu_addCommand (menu, L"-- after file draw --", 0, NULL);
	v_createMenuItems_file_extract (menu);
	EditorMenu_addCommand (menu, L"-- after file extract --", 0, NULL);
	v_createMenuItems_file_write (menu);
	EditorMenu_addCommand (menu, L"-- after file write --", 0, NULL);
}

/********** QUERY MENU **********/

static void menu_cb_SoundInfo (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	Thing_info (my d_sound.data);
}

static void menu_cb_LongSoundInfo (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	Thing_info (my d_longSound.data);
}

void structTimeSoundEditor :: v_createMenuItems_query_info (EditorMenu menu) {
	TimeSoundEditor_Parent :: v_createMenuItems_query_info (menu);
	if (d_sound.data != NULL && d_sound.data != data) {
		EditorMenu_addCommand (menu, L"Sound info", 0, menu_cb_SoundInfo);
	} else if (d_longSound.data != NULL && d_longSound.data != data) {
		EditorMenu_addCommand (menu, L"LongSound info", 0, menu_cb_LongSoundInfo);
	}
}

/********** VIEW MENU **********/

static void menu_cb_soundScaling (EDITOR_ARGS) {
	EDITOR_IAM (TimeSoundEditor);
	EDITOR_FORM (L"Sound scaling", 0)
		OPTIONMENU_ENUM (L"Scaling strategy", kTimeSoundEditor_scalingStrategy, my default_sound_scalingStrategy ())
		LABEL (L"", L"For \"fixed height\":");
		POSITIVE (L"Height", my default_sound_scaling_height ())
		LABEL (L"", L"For \"fixed range\":");
		REAL (L"Minimum", my default_sound_scaling_minimum ())
		REAL (L"Maximum", my default_sound_scaling_maximum ())
	EDITOR_OK
		SET_ENUM (L"Scaling strategy", kTimeSoundEditor_scalingStrategy, my p_sound_scalingStrategy)
		SET_REAL (L"Height", my p_sound_scaling_height)
		SET_REAL (L"Minimum", my p_sound_scaling_minimum)
		SET_REAL (L"Maximum", my p_sound_scaling_maximum)
	EDITOR_DO
		my pref_sound_scalingStrategy () = my p_sound_scalingStrategy = GET_ENUM (kTimeSoundEditor_scalingStrategy, L"Scaling strategy");
		my pref_sound_scaling_height  () = my p_sound_scaling_height  = GET_REAL (L"Height");
		my pref_sound_scaling_minimum () = my p_sound_scaling_minimum = GET_REAL (L"Minimum");
		my pref_sound_scaling_maximum () = my p_sound_scaling_maximum = GET_REAL (L"Maximum");
		FunctionEditor_redraw (me);
	EDITOR_END
}

void structTimeSoundEditor :: v_createMenuItems_view (EditorMenu menu) {
	if (d_sound.data || d_longSound.data)
		v_createMenuItems_view_sound (menu);
	TimeSoundEditor_Parent :: v_createMenuItems_view (menu);
}

void structTimeSoundEditor :: v_createMenuItems_view_sound (EditorMenu menu) {
	EditorMenu_addCommand (menu, L"Sound scaling...", 0, menu_cb_soundScaling);
	EditorMenu_addCommand (menu, L"-- sound view --", 0, 0);
}

void structTimeSoundEditor :: v_updateMenuItems_file () {
	Sampled sound = d_sound.data != NULL ? (Sampled) d_sound.data : (Sampled) d_longSound.data;
	if (sound == NULL) return;
	long first, last, selectedSamples = Sampled_getWindowSamples (sound, d_startSelection, d_endSelection, & first, & last);
	if (drawButton) {
		drawButton -> f_setSensitive (selectedSamples != 0);
		publishButton -> f_setSensitive (selectedSamples != 0);
		publishPreserveButton -> f_setSensitive (selectedSamples != 0);
		if (publishWindowButton) publishWindowButton -> f_setSensitive (selectedSamples != 0);
		if (publishOverlapButton) publishOverlapButton -> f_setSensitive (selectedSamples != 0);
	}
	writeWavButton -> f_setSensitive (selectedSamples != 0);
	if (d_saveAs24BitWavButton)
		d_saveAs24BitWavButton -> f_setSensitive (selectedSamples != 0);
	if (d_saveAs32BitWavButton)
		d_saveAs32BitWavButton -> f_setSensitive (selectedSamples != 0);
	writeAiffButton -> f_setSensitive (selectedSamples != 0);
	writeAifcButton -> f_setSensitive (selectedSamples != 0);
	writeNextSunButton -> f_setSensitive (selectedSamples != 0);
	writeNistButton -> f_setSensitive (selectedSamples != 0);
	writeFlacButton -> f_setSensitive (selectedSamples != 0);
}

void structTimeSoundEditor :: f_drawSound (double globalMinimum, double globalMaximum) {
	Sound sound = d_sound.data;
	LongSound longSound = d_longSound.data;
	Melder_assert ((sound == NULL) != (longSound == NULL));
	int nchan = sound ? sound -> ny : longSound -> numberOfChannels;
	bool cursorVisible = d_startSelection == d_endSelection and d_startSelection >= d_startWindow and d_startSelection <= d_endWindow;
	Graphics_setColour (d_graphics, Graphics_BLACK);
	bool fits;
	try {
		fits = sound ? true : LongSound_haveWindow (longSound, d_startWindow, d_endWindow);
	} catch (MelderError) {
		int outOfMemory = wcsstr (Melder_getError (), L"memory") != NULL;
		if (Melder_debug == 9) Melder_flushError (NULL); else Melder_clearError ();
		Graphics_setWindow (d_graphics, 0, 1, 0, 1);
		Graphics_setTextAlignment (d_graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (d_graphics, 0.5, 0.5, outOfMemory ? L"(out of memory)" : L"(cannot read sound file)");
		return;
	}
	if (! fits) {
		Graphics_setWindow (d_graphics, 0, 1, 0, 1);
		Graphics_setTextAlignment (d_graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (d_graphics, 0.5, 0.5, L"(window too large; zoom in to see the data)");
		return;
	}
	long first, last;
	if (Sampled_getWindowSamples (sound ? (Sampled) sound : (Sampled) longSound, d_startWindow, d_endWindow, & first, & last) <= 1) {
		Graphics_setWindow (d_graphics, 0, 1, 0, 1);
		Graphics_setTextAlignment (d_graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (d_graphics, 0.5, 0.5, L"(zoom out to see the data)");
		return;
	}
	const int numberOfVisibleChannels = nchan > 8 ? 8 : nchan;
	const int firstVisibleChannel = d_sound.channelOffset + 1;
	int lastVisibleChannel = d_sound.channelOffset + numberOfVisibleChannels;
	if (lastVisibleChannel > nchan) lastVisibleChannel = nchan;
	double maximumExtent = 0.0, visibleMinimum = 0.0, visibleMaximum = 0.0;
	if (p_sound_scalingStrategy == kTimeSoundEditor_scalingStrategy_BY_WINDOW) {
		if (longSound)
			LongSound_getWindowExtrema (longSound, d_startWindow, d_endWindow, firstVisibleChannel, & visibleMinimum, & visibleMaximum);
		else
			Matrix_getWindowExtrema (sound, first, last, firstVisibleChannel, firstVisibleChannel, & visibleMinimum, & visibleMaximum);
		for (int ichan = firstVisibleChannel + 1; ichan <= lastVisibleChannel; ichan ++) {
			double visibleChannelMinimum, visibleChannelMaximum;
			if (longSound)
				LongSound_getWindowExtrema (longSound, d_startWindow, d_endWindow, ichan, & visibleChannelMinimum, & visibleChannelMaximum);
			else
				Matrix_getWindowExtrema (sound, first, last, ichan, ichan, & visibleChannelMinimum, & visibleChannelMaximum);
			if (visibleChannelMinimum < visibleMinimum)
				visibleMinimum = visibleChannelMinimum;
			if (visibleChannelMaximum > visibleMaximum)
				visibleMaximum = visibleChannelMaximum;
		}
		maximumExtent = visibleMaximum - visibleMinimum;
	}
	for (int ichan = firstVisibleChannel; ichan <= lastVisibleChannel; ichan ++) {
		double cursorFunctionValue = longSound ? 0.0 :
			Vector_getValueAtX (sound, 0.5 * (d_startSelection + d_endSelection), ichan, 70);
		/*
		 * BUG: this will only work for mono or stereo, until Graphics_function16 handles quadro.
		 */
		double ymin = (double) (numberOfVisibleChannels - ichan + d_sound.channelOffset) / numberOfVisibleChannels;
		double ymax = (double) (numberOfVisibleChannels + 1 - ichan + d_sound.channelOffset) / numberOfVisibleChannels;
		Graphics_Viewport vp = Graphics_insetViewport (d_graphics, 0, 1, ymin, ymax);
		bool horizontal = false;
		double minimum = sound ? globalMinimum : -1.0, maximum = sound ? globalMaximum : 1.0;
		if (p_sound_scalingStrategy == kTimeSoundEditor_scalingStrategy_BY_WINDOW) {
			if (nchan > 2) {
				if (longSound) {
					LongSound_getWindowExtrema (longSound, d_startWindow, d_endWindow, ichan, & minimum, & maximum);
				} else {
					Matrix_getWindowExtrema (sound, first, last, ichan, ichan, & minimum, & maximum);
				}
				if (maximumExtent > 0.0) {
					double middle = 0.5 * (minimum + maximum);
					minimum = middle - 0.5 * maximumExtent;
					maximum = middle + 0.5 * maximumExtent;
				}
			} else {
				minimum = visibleMinimum;
				maximum = visibleMaximum;
			}
		} else if (p_sound_scalingStrategy == kTimeSoundEditor_scalingStrategy_BY_WINDOW_AND_CHANNEL) {
			if (longSound) {
				LongSound_getWindowExtrema (longSound, d_startWindow, d_endWindow, ichan, & minimum, & maximum);
			} else {
				Matrix_getWindowExtrema (sound, first, last, ichan, ichan, & minimum, & maximum);
			}
		} else if (p_sound_scalingStrategy == kTimeSoundEditor_scalingStrategy_FIXED_HEIGHT) {
			if (longSound) {
				LongSound_getWindowExtrema (longSound, d_startWindow, d_endWindow, ichan, & minimum, & maximum);
			} else {
				Matrix_getWindowExtrema (sound, first, last, ichan, ichan, & minimum, & maximum);
			}
			double channelExtent = p_sound_scaling_height;
			double middle = 0.5 * (minimum + maximum);
			minimum = middle - 0.5 * channelExtent;
			maximum = middle + 0.5 * channelExtent;
		} else if (p_sound_scalingStrategy == kTimeSoundEditor_scalingStrategy_FIXED_RANGE) {
			minimum = p_sound_scaling_minimum;
			maximum = p_sound_scaling_maximum;
		}
		if (minimum == maximum) { horizontal = true; minimum -= 1; maximum += 1;}
		Graphics_setWindow (d_graphics, d_startWindow, d_endWindow, minimum, maximum);
		if (horizontal) {
			Graphics_setTextAlignment (d_graphics, Graphics_RIGHT, Graphics_HALF);
			double mid = 0.5 * (minimum + maximum);
			Graphics_text1 (d_graphics, d_startWindow, mid, Melder_float (Melder_half (mid)));
		} else {
			if (not cursorVisible or Graphics_dyWCtoMM (d_graphics, cursorFunctionValue - minimum) > 5.0) {
				Graphics_setTextAlignment (d_graphics, Graphics_RIGHT, Graphics_BOTTOM);
				Graphics_text1 (d_graphics, d_startWindow, minimum, Melder_float (Melder_half (minimum)));
			}
			if (not cursorVisible or Graphics_dyWCtoMM (d_graphics, maximum - cursorFunctionValue) > 5.0) {
				Graphics_setTextAlignment (d_graphics, Graphics_RIGHT, Graphics_TOP);
				Graphics_text1 (d_graphics, d_startWindow, maximum, Melder_float (Melder_half (maximum)));
			}
		}
		if (minimum < 0 && maximum > 0 && ! horizontal) {
			Graphics_setWindow (d_graphics, 0, 1, minimum, maximum);
			if (not cursorVisible or fabs (Graphics_dyWCtoMM (d_graphics, cursorFunctionValue - 0.0)) > 3.0) {
				Graphics_setTextAlignment (d_graphics, Graphics_RIGHT, Graphics_HALF);
				Graphics_text (d_graphics, 0, 0, L"0");
			}
			Graphics_setColour (d_graphics, Graphics_CYAN);
			Graphics_setLineType (d_graphics, Graphics_DOTTED);
			Graphics_line (d_graphics, 0, 0, 1, 0);
			Graphics_setLineType (d_graphics, Graphics_DRAWN);
		}
		/*
		 * Garnish the drawing area of each channel.
		 */
		Graphics_setWindow (d_graphics, 0, 1, 0, 1);
		Graphics_setColour (d_graphics, Graphics_CYAN);
		Graphics_innerRectangle (d_graphics, 0, 1, 0, 1);
		Graphics_setColour (d_graphics, Graphics_BLACK);
		if (nchan > 1) {
			Graphics_setTextAlignment (d_graphics, Graphics_LEFT, Graphics_HALF);
			static MelderString channelLabel;
			const wchar_t *channelName = v_getChannelName (ichan);
			MelderString_copy (& channelLabel, channelName ? L"ch" : L"Channel ");
			MelderString_append (& channelLabel, Melder_integer (ichan));
			if (channelName)
				MelderString_append (& channelLabel, L": ", channelName);
			if (ichan > 8 && ichan - d_sound.channelOffset == 1) {
				MelderString_append (& channelLabel, L" " UNITEXT_UPWARDS_ARROW);
			} else if (ichan >= 8 && ichan - d_sound.channelOffset == 8 && ichan < nchan) {
				MelderString_append (& channelLabel, L" " UNITEXT_DOWNWARDS_ARROW);
			}
			Graphics_text1 (d_graphics, 1, 0.5, channelLabel.string);
		}
		/*
		 * Draw a very thin separator line underneath.
		 */
		if (ichan < nchan) {
			/*Graphics_setColour (d_graphics, Graphics_BLACK);*/
			Graphics_line (d_graphics, 0, 0, 1, 0);
		}
		/*
		 * Draw the samples.
		 */
		/*if (ichan == 1) FunctionEditor_SoundAnalysis_drawPulses (this);*/
		if (sound) {
			Graphics_setWindow (d_graphics, d_startWindow, d_endWindow, minimum, maximum);
			if (cursorVisible)
				FunctionEditor_drawCursorFunctionValue (this, cursorFunctionValue, Melder_float (Melder_half (cursorFunctionValue)), L"");
			Graphics_setColour (d_graphics, Graphics_BLACK);
			Graphics_function (d_graphics, sound -> z [ichan], first, last,
				Sampled_indexToX (sound, first), Sampled_indexToX (sound, last));
		} else {
			Graphics_setWindow (d_graphics, d_startWindow, d_endWindow, minimum * 32768, maximum * 32768);
			Graphics_function16 (d_graphics,
				longSound -> buffer - longSound -> imin * nchan + (ichan - 1), nchan - 1, first, last,
				Sampled_indexToX (longSound, first), Sampled_indexToX (longSound, last));
		}
		Graphics_resetViewport (d_graphics, vp);
	}
	Graphics_setWindow (d_graphics, 0, 1, 0, 1);
	Graphics_rectangle (d_graphics, 0, 1, 0, 1);
}

int structTimeSoundEditor :: v_click (double xbegin, double ybegin, bool shiftKeyPressed) {
	Sound sound = d_sound.data;
	LongSound longSound = d_longSound.data;
	if ((sound == NULL) != (longSound == NULL)) {
		ybegin = (ybegin - v_getBottomOfSoundArea ()) / (1.0 - v_getBottomOfSoundArea ());
		int nchan = sound ? sound -> ny : longSound -> numberOfChannels;
		if (nchan > 8) {
			//Melder_casual ("%f %f %d %d", xbegin, ybegin, (int) nchan, (int) this -> sound.channelOffset);
			if (xbegin >= d_endWindow && ybegin > 0.875 && ybegin <= 1.000 && d_sound.channelOffset > 0) {
				d_sound.channelOffset -= 8;
				return 1;
			}
			if (xbegin >= d_endWindow && ybegin > 0.000 && ybegin <= 0.125 && d_sound.channelOffset < nchan - 8) {
				d_sound.channelOffset += 8;
				return 1;
			}
		}
	}
	return TimeSoundEditor_Parent :: v_click (xbegin, ybegin, shiftKeyPressed);
}

void structTimeSoundEditor :: f_init (const wchar_t *title, Function data, Sampled sound, bool ownSound) {
	d_ownSound = ownSound;
	if (sound != NULL) {
		if (ownSound) {
			Melder_assert (Thing_member (sound, classSound));
			d_sound.data = Data_copy ((Sound) sound);   // deep copy; ownership transferred
			Matrix_getWindowExtrema (sound, 1, d_sound.data -> nx, 1, d_sound.data -> ny, & d_sound.minimum, & d_sound.maximum);
		} else if (Thing_member (sound, classSound)) {
			d_sound.data = (Sound) sound;   // reference copy; ownership not transferred
			Matrix_getWindowExtrema (sound, 1, d_sound.data -> nx, 1, d_sound.data -> ny, & d_sound.minimum, & d_sound.maximum);
		} else if (Thing_member (sound, classLongSound)) {
			d_longSound.data = (LongSound) sound;
			d_sound.minimum = -1.0, d_sound.maximum = 1.0;
		} else {
			Melder_fatal ("Invalid sound class in TimeSoundEditor::init.");
		}
	}
	FunctionEditor_init (this, title, data);
}

/* End of file TimeSoundEditor.cpp */
