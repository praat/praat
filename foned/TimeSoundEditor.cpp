/* TimeSoundEditor.cpp
 *
 * Copyright (C) 1992-2022 Paul Boersma
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

#include "NUM2.h"
#include "TimeSoundEditor.h"
#include "EditorM.h"

Thing_implement_pureVirtual (TimeSoundEditor, FunctionEditor, 0);

#include "Prefs_define.h"
#include "TimeSoundEditor_prefs.h"
#include "Prefs_install.h"
#include "TimeSoundEditor_prefs.h"
#include "Prefs_copyToInstance.h"
#include "TimeSoundEditor_prefs.h"

/********** Thing methods **********/

void structTimeSoundEditor :: v1_info () {
	TimeSoundEditor_Parent :: v1_info ();
	if (our soundArea)
		MelderInfo_writeLine (U"Sound scaling strategy: ",
				kSoundArea_scalingStrategy_getText (our soundArea -> instancePref_scalingStrategy()));
}

/***** FILE MENU *****/

static autoSound do_ExtractSelectedSound (TimeSoundEditor me, bool preserveTimes) {
	if (my endSelection <= my startSelection)
		Melder_throw (U"No selection.");
	if (my longSound())
		return LongSound_extractPart (my longSound(), my startSelection, my endSelection, preserveTimes);
	else if (my sound())
		return Sound_extractPart (my sound(), my startSelection, my endSelection, kSound_windowShape::RECTANGULAR, 1.0, preserveTimes);
	Melder_fatal (U"No Sound or LongSound available.");
	return autoSound();   // never reached
}

static void CONVERT_DATA_TO_ONE__ExtractSelectedSound_timeFromZero (TimeSoundEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		autoSound result = do_ExtractSelectedSound (me, false);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void CONVERT_DATA_TO_ONE__ExtractSelectedSound_preserveTimes (TimeSoundEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	CONVERT_DATA_TO_ONE
		autoSound result = do_ExtractSelectedSound (me, true);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void CONVERT_DATA_TO_ONE__ExtractSelectedSound_windowed (TimeSoundEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Extract selected sound (windowed)", nullptr)
		WORD (name, U"Name", U"slice")
		OPTIONMENU_ENUM (kSound_windowShape, windowShape, U"Window shape", my default_extract_windowShape())
		POSITIVE (relativeWidth, U"Relative width", my default_extract_relativeWidth())
		BOOLEAN (preserveTimes, U"Preserve times", my default_extract_preserveTimes())
	EDITOR_OK
		SET_ENUM (windowShape, kSound_windowShape, my classPref_extract_windowShape())
		SET_REAL (relativeWidth, my classPref_extract_relativeWidth())
		SET_BOOLEAN (preserveTimes, my classPref_extract_preserveTimes())
	EDITOR_DO
		Melder_assert (my sound());   // no LongSound
		CONVERT_DATA_TO_ONE
			my setClassPref_extract_windowShape (windowShape);
			my setClassPref_extract_relativeWidth (relativeWidth);
			my setClassPref_extract_preserveTimes (preserveTimes);
			autoSound result = Sound_extractPart (my sound(), my startSelection, my endSelection, windowShape, relativeWidth, preserveTimes);
		CONVERT_DATA_TO_ONE_END (name)
	EDITOR_END
}

static void CONVERT_DATA_TO_ONE__ExtractSelectedSoundForOverlap (TimeSoundEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Extract selected sound for overlap)", nullptr)
		WORD (name, U"Name", U"slice")
		POSITIVE (overlap, U"Overlap (s)", my default_extract_overlap ())
	EDITOR_OK
		SET_REAL (overlap, my classPref_extract_overlap())
	EDITOR_DO
		Melder_assert (my sound());   // no LongSound
		CONVERT_DATA_TO_ONE
			my setClassPref_extract_overlap (overlap);
			autoSound result = Sound_extractPartForOverlap (my sound(), my startSelection, my endSelection, overlap);
		CONVERT_DATA_TO_ONE_END (name)
	EDITOR_END
}

static void do_write (TimeSoundEditor me, MelderFile file, int format, int numberOfBitsPerSamplePoint) {
	if (my startSelection >= my endSelection)
		Melder_throw (U"No samples selected.");
	if (my longSound()) {
		LongSound_savePartAsAudioFile (my longSound(), format, my startSelection, my endSelection, file, numberOfBitsPerSamplePoint);
	} else if (my sound()) {
		double margin = 0.0;
		integer nmargin = Melder_ifloor (margin / my sound() -> dx);
		integer first, last;
		const integer numberOfSamples = Sampled_getWindowSamples (my sound(),
				my startSelection, my endSelection, & first, & last) + nmargin * 2;
		first -= nmargin;
		last += nmargin;
		if (numberOfSamples) {
			autoSound save = Sound_create (my sound() -> ny, 0.0,
					numberOfSamples * my sound() -> dx, numberOfSamples, my sound() -> dx, 0.5 * my sound() -> dx);
			integer offset = first - 1;
			if (first < 1)
				first = 1;   // TODO: check logic, i.e. whether `offset` outdates or not
			if (last > my sound() -> nx)
				last = my sound() -> nx;
			for (integer channel = 1; channel <= my sound() -> ny; channel ++) {
				for (integer i = first; i <= last; i ++)
					save -> z [channel] [i - offset] = my sound() -> z [channel] [i];
			}
			Sound_saveAsAudioFile (save.get(), file, format, numberOfBitsPerSamplePoint);
		}
	}
}

static void menu_cb_WriteWav (TimeSoundEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as WAV file", nullptr)
		Melder_sprint (defaultName,300, my soundOrLongSound() -> name.get(), U".wav");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_WAV, 16);
	EDITOR_END
}

static void menu_cb_SaveAs24BitWav (TimeSoundEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as 24-bit WAV file", nullptr)
		Melder_assert (! my longSound() && my sound());
		Melder_sprint (defaultName,300, my sound() -> name.get(), U".wav");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_WAV, 24);
	EDITOR_END
}

static void menu_cb_SaveAs32BitWav (TimeSoundEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as 32-bit WAV file", nullptr)
		Melder_assert (! my longSound() && my sound());
		Melder_sprint (defaultName,300, my sound() -> name.get(), U".wav");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_WAV, 32);
	EDITOR_END
}

static void menu_cb_WriteAiff (TimeSoundEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as AIFF file", nullptr)
		Melder_sprint (defaultName,300, my soundOrLongSound() -> name.get(), U".aiff");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_AIFF, 16);
	EDITOR_END
}

static void menu_cb_WriteAifc (TimeSoundEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as AIFC file", nullptr)
		Melder_sprint (defaultName,300, my soundOrLongSound() -> name.get(), U".aifc");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_AIFC, 16);
	EDITOR_END
}

static void menu_cb_WriteNextSun (TimeSoundEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as NeXT/Sun file", nullptr)
		Melder_sprint (defaultName,300, my soundOrLongSound() -> name.get(), U".au");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_NEXT_SUN, 16);
	EDITOR_END
}

static void menu_cb_WriteNist (TimeSoundEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as NIST file", nullptr)
		Melder_sprint (defaultName,300, my soundOrLongSound() -> name.get(), U".nist");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_NIST, 16);
	EDITOR_END
}

static void menu_cb_WriteFlac (TimeSoundEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM_SAVE (U"Save selected sound as FLAC file", nullptr)
		Melder_sprint (defaultName,300, my soundOrLongSound() -> name.get(), U".flac");
	EDITOR_DO_SAVE
		do_write (me, file, Melder_FLAC, 16);
	EDITOR_END
}

void structTimeSoundEditor :: v_createMenuItems_file_draw (EditorMenu menu) {
}

void structTimeSoundEditor :: v_createMenuItems_file_extract (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"Extract to objects window:", GuiMenu_INSENSITIVE,
			CONVERT_DATA_TO_ONE__ExtractSelectedSound_preserveTimes /* dummy */);
	if (our soundArea) {
		our publishPreserveButton = EditorMenu_addCommand (menu, U"Extract selected sound (preserve times)", 0,
				CONVERT_DATA_TO_ONE__ExtractSelectedSound_preserveTimes);
			EditorMenu_addCommand (menu, U"Extract sound selection (preserve times)", Editor_HIDDEN,
					CONVERT_DATA_TO_ONE__ExtractSelectedSound_preserveTimes);
			EditorMenu_addCommand (menu, U"Extract selection (preserve times)", Editor_HIDDEN,
					CONVERT_DATA_TO_ONE__ExtractSelectedSound_preserveTimes);
		our publishButton = EditorMenu_addCommand (menu, U"Extract selected sound (time from 0)", 0,
				CONVERT_DATA_TO_ONE__ExtractSelectedSound_timeFromZero);
			EditorMenu_addCommand (menu, U"Extract sound selection (time from 0)", Editor_HIDDEN,
					CONVERT_DATA_TO_ONE__ExtractSelectedSound_timeFromZero);
			EditorMenu_addCommand (menu, U"Extract selection (time from 0)", Editor_HIDDEN,
					CONVERT_DATA_TO_ONE__ExtractSelectedSound_timeFromZero);
			EditorMenu_addCommand (menu, U"Extract selection", Editor_HIDDEN,
					CONVERT_DATA_TO_ONE__ExtractSelectedSound_timeFromZero);
		if (our soundArea && ! Thing_isa (our soundArea.get(), classLongSoundArea)) {
			our publishWindowButton = EditorMenu_addCommand (menu, U"Extract selected sound (windowed)...", 0,
					CONVERT_DATA_TO_ONE__ExtractSelectedSound_windowed);
				EditorMenu_addCommand (menu, U"Extract windowed sound selection...", Editor_HIDDEN,
						CONVERT_DATA_TO_ONE__ExtractSelectedSound_windowed);
				EditorMenu_addCommand (menu, U"Extract windowed selection...", Editor_HIDDEN,
						CONVERT_DATA_TO_ONE__ExtractSelectedSound_windowed);
			our publishOverlapButton = EditorMenu_addCommand (menu, U"Extract selected sound for overlap...", 0,
					CONVERT_DATA_TO_ONE__ExtractSelectedSoundForOverlap);
		}
	}
}

void structTimeSoundEditor :: v_createMenuItems_file_write (EditorMenu menu) {
	EditorMenu_addCommand (menu, U"Save to disk:", GuiMenu_INSENSITIVE, menu_cb_WriteWav /* dummy */);
	if (our soundArea) {
		our writeWavButton = EditorMenu_addCommand (menu, U"Save selected sound as WAV file...", 0, menu_cb_WriteWav);
			EditorMenu_addCommand (menu, U"Write selected sound to WAV file...", Editor_HIDDEN, menu_cb_WriteWav);
			EditorMenu_addCommand (menu, U"Write sound selection to WAV file...", Editor_HIDDEN, menu_cb_WriteWav);
			EditorMenu_addCommand (menu, U"Write selection to WAV file...", Editor_HIDDEN, menu_cb_WriteWav);
		if (our soundArea && ! Thing_isa (our soundArea.get(), classLongSoundArea)) {   // BUG: why not for LongSound?
			our saveAs24BitWavButton = EditorMenu_addCommand (menu, U"Save selected sound as 24-bit WAV file...", 0, menu_cb_SaveAs24BitWav);
			our saveAs32BitWavButton = EditorMenu_addCommand (menu, U"Save selected sound as 32-bit WAV file...", 0, menu_cb_SaveAs32BitWav);
		}
		our writeAiffButton = EditorMenu_addCommand (menu, U"Save selected sound as AIFF file...", 0, menu_cb_WriteAiff);
			EditorMenu_addCommand (menu, U"Write selected sound to AIFF file...", Editor_HIDDEN, menu_cb_WriteAiff);
			EditorMenu_addCommand (menu, U"Write sound selection to AIFF file...", Editor_HIDDEN, menu_cb_WriteAiff);
			EditorMenu_addCommand (menu, U"Write selection to AIFF file...", Editor_HIDDEN, menu_cb_WriteAiff);
		our writeAifcButton = EditorMenu_addCommand (menu, U"Save selected sound as AIFC file...", 0, menu_cb_WriteAifc);
			EditorMenu_addCommand (menu, U"Write selected sound to AIFC file...", Editor_HIDDEN, menu_cb_WriteAifc);
			EditorMenu_addCommand (menu, U"Write sound selection to AIFC file...", Editor_HIDDEN, menu_cb_WriteAifc);
			EditorMenu_addCommand (menu, U"Write selection to AIFC file...", Editor_HIDDEN, menu_cb_WriteAifc);
		our writeNextSunButton = EditorMenu_addCommand (menu, U"Save selected sound as NeXT/Sun file...", 0, menu_cb_WriteNextSun);
			EditorMenu_addCommand (menu, U"Write selected sound to NeXT/Sun file...", Editor_HIDDEN, menu_cb_WriteNextSun);
			EditorMenu_addCommand (menu, U"Write sound selection to NeXT/Sun file...", Editor_HIDDEN, menu_cb_WriteNextSun);
			EditorMenu_addCommand (menu, U"Write selection to NeXT/Sun file...", Editor_HIDDEN, menu_cb_WriteNextSun);
		our writeNistButton = EditorMenu_addCommand (menu, U"Save selected sound as NIST file...", 0, menu_cb_WriteNist);
			EditorMenu_addCommand (menu, U"Write selected sound to NIST file...", Editor_HIDDEN, menu_cb_WriteNist);
			EditorMenu_addCommand (menu, U"Write sound selection to NIST file...", Editor_HIDDEN, menu_cb_WriteNist);
			EditorMenu_addCommand (menu, U"Write selection to NIST file...", Editor_HIDDEN, menu_cb_WriteNist);
		our writeFlacButton = EditorMenu_addCommand (menu, U"Save selected sound as FLAC file...", 0, menu_cb_WriteFlac);
			EditorMenu_addCommand (menu, U"Write selected sound to FLAC file...", Editor_HIDDEN, menu_cb_WriteFlac);
			EditorMenu_addCommand (menu, U"Write sound selection to FLAC file...", Editor_HIDDEN, menu_cb_WriteFlac);
	}
}

void structTimeSoundEditor :: v_createMenuItems_file (EditorMenu menu) {
	our TimeSoundEditor_Parent :: v_createMenuItems_file (menu);
	our v_createMenuItems_file_draw (menu);
	EditorMenu_addCommand (menu, U"-- after file draw --", 0, nullptr);
	our v_createMenuItems_file_extract (menu);
	EditorMenu_addCommand (menu, U"-- after file extract --", 0, nullptr);
	our v_createMenuItems_file_write (menu);
	EditorMenu_addCommand (menu, U"-- after file write --", 0, nullptr);
}

/********** QUERY MENU **********/

static void INFO_DATA__SoundInfo (TimeSoundEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	INFO_DATA
		Thing_info (my sound());
	INFO_DATA_END
}

static void INFO_DATA__LongSoundInfo (TimeSoundEditor me, EDITOR_ARGS_DIRECT_WITH_OUTPUT) {
	INFO_DATA
		Thing_info (my longSound());
	INFO_DATA_END
}

void structTimeSoundEditor :: v_createMenuItems_query_info (EditorMenu menu) {
	TimeSoundEditor_Parent :: v_createMenuItems_query_info (menu);
	/*
		If the Sound or LongSound has been copied, then it's not the main data,
		so we should add an entry for its info, as Editor will not do that.
	*/
	if (our soundArea && our soundArea -> functionHasBeenCopied()) {
		if (Thing_isa (our soundArea.get(), classLongSoundArea))
			EditorMenu_addCommand (menu, U"LongSound info", 0, INFO_DATA__LongSoundInfo);
		else
			EditorMenu_addCommand (menu, U"Sound info", 0, INFO_DATA__SoundInfo);
	}
}


void structTimeSoundEditor:: v_createMenus () {
	TimeSoundEditor_Parent :: v_createMenus ();
	if (our soundArea)
		our soundArea -> v_createMenus ();
}

void structTimeSoundEditor :: v_updateMenuItems_file () {
	if (! our soundOrLongSound())
		return;
	integer first, last;
	const integer selectedSamples = Sampled_getWindowSamples (our soundOrLongSound(), our startSelection, our endSelection, & first, & last);
	if (our soundArea && our soundArea -> drawButton) {
		GuiThing_setSensitive (our soundArea -> drawButton, selectedSamples != 0);
		GuiThing_setSensitive (our publishButton, selectedSamples != 0);
		GuiThing_setSensitive (our publishPreserveButton, selectedSamples != 0);
		if (our publishWindowButton)
			GuiThing_setSensitive (our publishWindowButton, selectedSamples != 0);
		if (our publishOverlapButton)
			GuiThing_setSensitive (our publishOverlapButton, selectedSamples != 0);
	}
	GuiThing_setSensitive (our writeWavButton, selectedSamples != 0);
	if (our saveAs24BitWavButton)
		GuiThing_setSensitive (our saveAs24BitWavButton, selectedSamples != 0);
	if (our saveAs32BitWavButton)
		GuiThing_setSensitive (our saveAs32BitWavButton, selectedSamples != 0);
	GuiThing_setSensitive (our writeAiffButton, selectedSamples != 0);
	GuiThing_setSensitive (our writeAifcButton, selectedSamples != 0);
	GuiThing_setSensitive (our writeNextSunButton, selectedSamples != 0);
	GuiThing_setSensitive (our writeNistButton, selectedSamples != 0);
	GuiThing_setSensitive (our writeFlacButton, selectedSamples != 0);
}

bool structTimeSoundEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	if (event -> isClick()) {
		if (our soundOrLongSound()) {
			y_fraction = (y_fraction - v_getBottomOfSoundArea ()) / (1.0 - v_getBottomOfSoundArea ());
			const integer numberOfChannels = our soundOrLongSound() -> ny;
			if (event -> commandKeyPressed) {
				if (numberOfChannels > 1) {
					const integer numberOfVisibleChannels = Melder_clippedRight (numberOfChannels, 8_integer);
					Melder_assert (numberOfVisibleChannels >= 1);   // for Melder_clipped
					const integer clickedChannel = our soundArea -> channelOffset +
							Melder_clipped (1_integer, Melder_ifloor ((1.0 - y_fraction) * numberOfVisibleChannels + 1), numberOfVisibleChannels);
					const integer firstVisibleChannel = our soundArea -> channelOffset + 1;
					const integer lastVisibleChannel = Melder_clippedRight (our soundArea -> channelOffset + numberOfVisibleChannels, numberOfChannels);
					if (clickedChannel >= firstVisibleChannel && clickedChannel <= lastVisibleChannel) {
						our soundArea -> muteChannels [clickedChannel] = ! our soundArea -> muteChannels [clickedChannel];
						return FunctionEditor_UPDATE_NEEDED;
					}
				}
			} else {
				if (numberOfChannels > 8) {
					if (x_world >= our endWindow && y_fraction > 0.875 && y_fraction <= 1.000 && our soundArea -> channelOffset > 0) {
						our soundArea -> channelOffset -= 8;
						return FunctionEditor_UPDATE_NEEDED;
					}
					if (x_world >= our endWindow && y_fraction > 0.000 && y_fraction <= 0.125 && our soundArea -> channelOffset < numberOfChannels - 8) {
						our soundArea -> channelOffset += 8;
						return FunctionEditor_UPDATE_NEEDED;
					}
				}
			}
		}
	}
	return TimeSoundEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);   // BUG: use FunctionEditor_defaultMouseInWideDataView()
}

/* End of file TimeSoundEditor.cpp */
