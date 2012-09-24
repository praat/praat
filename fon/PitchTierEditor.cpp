/* PitchTierEditor.cpp
 *
 * Copyright (C) 1992-2011,2012 Paul Boersma
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

#include "PitchTierEditor.h"
#include "PitchTier_to_Sound.h"
#include "EditorM.h"

Thing_implement (PitchTierEditor, RealTierEditor, 0);

static void menu_cb_PitchTierEditorHelp (EDITOR_ARGS) { EDITOR_IAM (PitchTierEditor); Melder_help (L"PitchTierEditor"); }
static void menu_cb_PitchTierHelp (EDITOR_ARGS) { EDITOR_IAM (PitchTierEditor); Melder_help (L"PitchTier"); }

void structPitchTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	PitchTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"PitchTierEditor help", 0, menu_cb_PitchTierEditorHelp);
	EditorMenu_addCommand (menu, L"PitchTier help", 0, menu_cb_PitchTierHelp);
}

void structPitchTierEditor :: v_play (double a_tmin, double a_tmax) {
	if (d_sound.data) {
		Sound_playPart (d_sound.data, a_tmin, a_tmax, theFunctionEditor_playCallback, this);
	} else {
		PitchTier_playPart ((PitchTier) data, a_tmin, a_tmax, FALSE);
	}
}

PitchTierEditor PitchTierEditor_create (const wchar_t *title, PitchTier pitch, Sound sound, bool ownSound) {
	try {
		autoPitchTierEditor me = Thing_new (PitchTierEditor);
		RealTierEditor_init (me.peek(), title, (RealTier) pitch, sound, ownSound);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("PitchTier window not created.");
	}
}

/* End of file PitchTierEditor.cpp */
