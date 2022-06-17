/* PitchTierEditor.cpp
 *
 * Copyright (C) 1992-2012,2015,2016,2018,2020-2022 Paul Boersma
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

static void menu_cb_PitchTierEditorHelp (PitchTierEditor, EDITOR_ARGS_DIRECT) {
	HELP (U"PitchTierEditor")
}

static void menu_cb_PitchTierHelp (PitchTierEditor, EDITOR_ARGS_DIRECT) {
	HELP (U"PitchTier")
}

void structPitchTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	PitchTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"PitchTierEditor help", 0, menu_cb_PitchTierEditorHelp);
	EditorMenu_addCommand (menu, U"PitchTier help", 0, menu_cb_PitchTierHelp);
}

void structPitchTierEditor :: v_play (double startTime, double endTime) {
	if (our sound())
		Sound_playPart (our sound(), startTime, endTime, theFunctionEditor_playCallback, this);
	else
		PitchTier_playPart (our pitchTierArea() -> pitchTier(), startTime, endTime, false);
}

autoPitchTierEditor PitchTierEditor_create (conststring32 title, PitchTier pitchTier, Sound sound) {
	try {
		autoPitchTierEditor me = Thing_new (PitchTierEditor);
		my data = pitchTier;

		my realTierArea = Thing_new (PitchTierArea);
		RealTierArea_init (my realTierArea.get(), me.get(), pitchTier, false, true);

		if (sound)
			my soundArea = SoundArea_create (me.get(), sound, true, false);

		FunctionEditor_init (me.get(), title);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PitchTier window not created.");
	}
}

/* End of file PitchTierEditor.cpp */
