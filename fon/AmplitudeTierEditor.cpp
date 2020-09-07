/* AmplitudeTierEditor.cpp
 *
 * Copyright (C) 2003-2012,2014-2016,2018,2020 Paul Boersma
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

#include "AmplitudeTierEditor.h"
#include "EditorM.h"

Thing_implement (AmplitudeTierArea, RealTierArea, 0);

Thing_implement (AmplitudeTierEditor, RealTierEditor, 0);

static void menu_cb_AmplitudeTierHelp (AmplitudeTierEditor /* me */, EDITOR_ARGS_DIRECT) {
	Melder_help (U"AmplitudeTier");
}

void structAmplitudeTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	AmplitudeTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"AmplitudeTier help", 0, menu_cb_AmplitudeTierHelp);
}

void structAmplitudeTierEditor :: v_play (double startTime, double endTime) {
	if (our d_sound.data) {
		Sound_playPart (our d_sound.data, startTime, endTime, theFunctionEditor_playCallback, this);
	} else {
		//AmplitudeTier_playPart (data, startTime, endTime, false);
	}
}

autoAmplitudeTierEditor AmplitudeTierEditor_create (conststring32 title, AmplitudeTier amplitude, Sound sound, bool ownSound) {
	try {
		autoAmplitudeTierEditor me = Thing_new (AmplitudeTierEditor);
		RealTierEditor_init (me.get(), classAmplitudeTierArea, title, amplitude, sound, ownSound);
		return me;
	} catch (MelderError) {
		Melder_throw (U"AmplitudeTier window not created.");
	}
}

/* End of file AmplitudeTierEditor.cpp */
