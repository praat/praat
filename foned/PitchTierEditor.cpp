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
#include "EditorM.h"

Thing_implement (PitchTierEditor, FunctionEditor, 0);

static void menu_cb_PitchTierEditorHelp (PitchTierEditor, EDITOR_ARGS) {
	HELP (U"PitchTierEditor")
}

static void menu_cb_PitchTierHelp (PitchTierEditor, EDITOR_ARGS) {
	HELP (U"PitchTier")
}

void structPitchTierEditor :: v_createMenuItems_help (EditorMenu menu) {
	PitchTierEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"PitchTierEditor help", 0, menu_cb_PitchTierEditorHelp);
	EditorMenu_addCommand (menu, U"PitchTier help", 0, menu_cb_PitchTierHelp);
}

autoPitchTierEditor PitchTierEditor_create (conststring32 title, PitchTier pitchTier, Sound optionalSoundToCopy) {
	try {
		autoPitchTierEditor me = Thing_new (PitchTierEditor);
		my pitchTierArea() = PitchTierArea_create (true, nullptr, me.get());
		if (optionalSoundToCopy)
			my soundArea() = SoundArea_create (false, optionalSoundToCopy, me.get());
		FunctionEditor_init (me.get(), title, pitchTier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PitchTier window not created.");
	}
}

/* End of file PitchTierEditor.cpp */
