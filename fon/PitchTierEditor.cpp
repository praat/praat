/* PitchTierEditor.cpp
 *
 * Copyright (C) 1992-2011,2012,20152016 Paul Boersma
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

void structPitchTierEditor :: v_play (double a_tmin, double a_tmax) {
	if (d_sound.data) {
		Sound_playPart (d_sound.data, a_tmin, a_tmax, theFunctionEditor_playCallback, this);
	} else {
		PitchTier_playPart ((PitchTier) data, a_tmin, a_tmax, false);
	}
}

autoPitchTierEditor PitchTierEditor_create (const char32 *title, PitchTier pitch, Sound sound, bool ownSound) {
	try {
		autoPitchTierEditor me = Thing_new (PitchTierEditor);
		RealTierEditor_init (me.get(), title, pitch, sound, ownSound);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PitchTier window not created.");
	}
}

/* End of file PitchTierEditor.cpp */
