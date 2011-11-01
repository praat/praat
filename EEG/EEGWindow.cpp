/* EEGWindow.cpp
 *
 * Copyright (C) 2011 Paul Boersma
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

void EEGWindow_preferences (void) {
}

static void menu_cb_EEGWindowHelp (EDITOR_ARGS) { EDITOR_IAM (EEGWindow); Melder_help (L"EEG window"); }

void structEEGWindow :: v_createMenus () {
	EEGWindow_Parent :: v_createMenus ();
}

void structEEGWindow :: v_createHelpMenuItems (EditorMenu menu) {
	TextGridEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"EEGWindow help", '?', menu_cb_EEGWindowHelp);
}

const wchar * structEEGWindow :: v_getChannelName (long channelNumber) {
	Melder_assert (d_eeg != NULL);
	return d_eeg -> d_channelNames [channelNumber];
}

void structEEGWindow :: f_init (GuiObject parent, const wchar *title, EEG eeg) {
	d_eeg = eeg;   // before initing, because initing will already draw!
	structTextGridEditor :: f_init (parent, title, eeg -> d_textgrid, eeg -> d_sound, false, NULL);
}

EEGWindow EEGWindow_create (GuiObject parent, const wchar *title, EEG eeg) {
	try {
		autoEEGWindow me = Thing_new (EEGWindow);
		my f_init (parent, title, eeg);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("EEG window not created.");
	}
}

/* End of file EEGWindow.cpp */
