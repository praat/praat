/* PitchTierEditor.cpp
 *
 * Copyright (C) 1992-2012,2015,2016,2018,2020 Paul Boersma
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

#include "PitchTierEditor.h"
#include "PitchTier_to_Sound.h"
#include "EditorM.h"

Thing_implement (PitchTierEditor, RealTierEditor, 0);

static void menu_cb_PitchTierEditorHelp (PitchTierEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"PitchTierEditor"); }
static void menu_cb_PitchTierHelp (PitchTierEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"PitchTier"); }

void structPitchTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	PitchTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"PitchTierEditor help", 0, menu_cb_PitchTierEditorHelp);
	EditorMenu_addCommand (menu, U"PitchTier help", 0, menu_cb_PitchTierHelp);
}

void structPitchTierEditor :: v_play (double startTime, double endTime) {
	if (our d_sound.data) {
		Sound_playPart (our d_sound.data, startTime, endTime, theFunctionEditor_playCallback, this);
	} else {
		PitchTier_playPart (our pitchTier(), startTime, endTime, false);
	}
}

autoPitchTierEditor PitchTierEditor_create (conststring32 title, PitchTier pitch, Sound sound, bool ownSound) {
	try {
		autoPitchTierEditor me = Thing_new (PitchTierEditor);
		autoPitchTierArea area = PitchTierArea_create (me.get(), 0.0, ( sound ? 1.0 - structRealTierEditor::SOUND_HEIGHT : 1.0 ));
		RealTierEditor_init (me.get(), area.move(), title, pitch, sound, ownSound);
		my pitchTierArea() -> p_units = kPitchTierArea_units::HERTZ;   // override preferences
		return me;
	} catch (MelderError) {
		Melder_throw (U"PitchTier window not created.");
	}
}

/* End of file PitchTierEditor.cpp */
