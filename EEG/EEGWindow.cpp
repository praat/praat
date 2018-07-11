/* EEGWindow.cpp
 *
 * Copyright (C) 2011-2012,2013,2014,2015,2016,2017 Paul Boersma
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

#include "EEGWindow.h"
#include "EditorM.h"

Thing_implement (EEGWindow, TextGridEditor, 0);

#include "prefs_define.h"
#include "EEGWindow_prefs.h"
#include "prefs_install.h"
#include "EEGWindow_prefs.h"
#include "prefs_copyToInstance.h"
#include "EEGWindow_prefs.h"

static void menu_cb_EEGWindowHelp (EEGWindow, EDITOR_ARGS_DIRECT) { Melder_help (U"EEG window"); }

void structEEGWindow :: v_createMenus () {
	EEGWindow_Parent :: v_createMenus ();
}

void structEEGWindow :: v_createHelpMenuItems (EditorMenu menu) {
	TextGridEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"EEGWindow help", '?', menu_cb_EEGWindowHelp);
}

conststring32 structEEGWindow :: v_getChannelName (integer channelNumber) {
	Melder_assert (our eeg != nullptr);
	return our eeg -> channelNames [channelNumber].get();
}

static void menu_cb_ExtractSelectedEEG_preserveTimes (EEGWindow me, EDITOR_ARGS_DIRECT) {
	if (my endSelection <= my startSelection) Melder_throw (U"No selection.");
	autoEEG extract = EEG_extractPart (my eeg, my startSelection, my endSelection, true);
	Editor_broadcastPublication (me, extract.move());
}

static void menu_cb_ExtractSelectedEEG_timeFromZero (EEGWindow me, EDITOR_ARGS_DIRECT) {
	if (my endSelection <= my startSelection) Melder_throw (U"No selection.");
	autoEEG extract = EEG_extractPart (my eeg, my startSelection, my endSelection, false);
	Editor_broadcastPublication (me, extract.move());
}

void structEEGWindow :: v_createMenuItems_file_extract (EditorMenu menu) {
	EEGWindow_Parent :: v_createMenuItems_file_extract (menu);
	our extractSelectedEEGPreserveTimesButton =
		EditorMenu_addCommand (menu, U"Extract selected EEG (preserve times)", 0, menu_cb_ExtractSelectedEEG_preserveTimes);
	our extractSelectedEEGTimeFromZeroButton =
		EditorMenu_addCommand (menu, U"Extract selected EEG (time from zero)", 0, menu_cb_ExtractSelectedEEG_timeFromZero);
}

void structEEGWindow :: v_updateMenuItems_file () {
	EEGWindow_Parent :: v_updateMenuItems_file ();
	GuiThing_setSensitive (our extractSelectedEEGPreserveTimesButton, our endSelection > our startSelection);
	GuiThing_setSensitive (our extractSelectedEEGTimeFromZeroButton,  our endSelection > our startSelection);
}

void EEGWindow_init (EEGWindow me, conststring32 title, EEG eeg) {
	my eeg = eeg;   // before initing, because initing will already draw!
	TextGridEditor_init (me, title, eeg -> textgrid.get(), eeg -> sound.get(), false, nullptr, nullptr);
}

autoEEGWindow EEGWindow_create (conststring32 title, EEG eeg) {
	try {
		autoEEGWindow me = Thing_new (EEGWindow);
		EEGWindow_init (me.get(), title, eeg);
		return me;
	} catch (MelderError) {
		Melder_throw (U"EEG window not created.");
	}
}

/* End of file EEGWindow.cpp */
