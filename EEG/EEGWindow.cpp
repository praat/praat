/* EEGWindow.cpp
 *
 * Copyright (C) 2011-2012 Paul Boersma
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

#include "EEGWindow.h"
#include "EditorM.h"

Thing_implement (EEGWindow, TextGridEditor, 0);

bool                             structEEGWindow :: s_showSelectionViewer;     // overridden
kTimeSoundEditor_scalingStrategy structEEGWindow :: s_sound_scalingStrategy;   // overridden
double                           structEEGWindow :: s_sound_scaling_height;    // overridden
double                           structEEGWindow :: s_sound_scaling_minimum;   // overridden
double                           structEEGWindow :: s_sound_scaling_maximum;   // overridden

void structEEGWindow :: f_preferences (void) {
	Preferences_addBool   (L"EEGWindow.showSelectionViewer",   & s_showSelectionViewer,                      false);   // overridden
	Preferences_addEnum   (L"EEGWindow.sound.scalingStrategy", & s_sound_scalingStrategy, kTimeSoundEditor_scalingStrategy, DEFAULT);   // overridden
	Preferences_addDouble (L"EEGWindow.sound.scaling.height",  & s_sound_scaling_height,                     20e-6);   // overridden
	Preferences_addDouble (L"EEGWindow.sound.scaling.minimum", & s_sound_scaling_minimum,                   -10e-6);   // overridden
	Preferences_addDouble (L"EEGWindow.sound.scaling.maximum", & s_sound_scaling_maximum,                   +10e-6);   // overridden
}

static void menu_cb_EEGWindowHelp (EDITOR_ARGS) { EDITOR_IAM (EEGWindow); Melder_help (L"EEG window"); }

void structEEGWindow :: v_createMenus () {
	EEGWindow_Parent :: v_createMenus ();
}

void structEEGWindow :: v_createHelpMenuItems (EditorMenu menu) {
	TextGridEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"EEGWindow help", '?', menu_cb_EEGWindowHelp);
}

const wchar_t * structEEGWindow :: v_getChannelName (long channelNumber) {
	Melder_assert (d_eeg != NULL);
	return d_eeg -> d_channelNames [channelNumber];
}

void structEEGWindow :: f_init (const wchar_t *title, EEG eeg) {
	d_eeg = eeg;   // before initing, because initing will already draw!
	structTextGridEditor :: f_init (title, eeg -> d_textgrid, eeg -> d_sound, false, NULL);
}

static void menu_cb_ExtractSelectedEEG_preserveTimes (EDITOR_ARGS) {
	EDITOR_IAM (EEGWindow);
	if (my d_endSelection <= my d_startSelection) Melder_throw ("No selection.");
	autoEEG extract = my d_eeg -> f_extractPart (my d_startSelection, my d_endSelection, true);
	my broadcastPublication (extract.transfer());
}

static void menu_cb_ExtractSelectedEEG_timeFromZero (EDITOR_ARGS) {
	EDITOR_IAM (EEGWindow);
	if (my d_endSelection <= my d_startSelection) Melder_throw ("No selection.");
	autoEEG extract = my d_eeg -> f_extractPart (my d_startSelection, my d_endSelection, false);
	my broadcastPublication (extract.transfer());
}

void structEEGWindow :: v_createMenuItems_file_extract (EditorMenu menu) {
	EEGWindow_Parent :: v_createMenuItems_file_extract (menu);
	d_extractSelectedEEGPreserveTimesButton =
		EditorMenu_addCommand (menu, L"Extract selected EEG (preserve times)", 0, menu_cb_ExtractSelectedEEG_preserveTimes);
	d_extractSelectedEEGTimeFromZeroButton =
		EditorMenu_addCommand (menu, L"Extract selected EEG (time from zero)", 0, menu_cb_ExtractSelectedEEG_timeFromZero);
}

void structEEGWindow :: v_updateMenuItems_file () {
	EEGWindow_Parent :: v_updateMenuItems_file ();
	d_extractSelectedEEGPreserveTimesButton -> f_setSensitive (d_endSelection > d_startSelection);
	d_extractSelectedEEGTimeFromZeroButton -> f_setSensitive (d_endSelection > d_startSelection);
}

EEGWindow EEGWindow_create (const wchar_t *title, EEG eeg) {
	try {
		autoEEGWindow me = Thing_new (EEGWindow);
		my f_init (title, eeg);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("EEG window not created.");
	}
}

/* End of file EEGWindow.cpp */
