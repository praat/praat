/* IntensityTierEditor.cpp
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

#include "IntensityTierEditor.h"
#include "EditorM.h"

Thing_implement (IntensityTierEditor, RealTierEditor, 0);

static void menu_cb_IntensityTierHelp (EDITOR_ARGS) { EDITOR_IAM (IntensityTierEditor); Melder_help (L"IntensityTier"); }

void structIntensityTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	IntensityTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"IntensityTier help", 0, menu_cb_IntensityTierHelp);
}

void structIntensityTierEditor :: v_play (double a_tmin, double a_tmax) {
	if (d_sound.data) {
		Sound_playPart (d_sound.data, a_tmin, a_tmax, theFunctionEditor_playCallback, this);
	} else {
		//IntensityTier_playPart (data, a_tmin, a_tmax, FALSE);
	}
}

IntensityTierEditor IntensityTierEditor_create (const wchar_t *title, IntensityTier intensity, Sound sound, bool ownSound) {
	try {
		autoIntensityTierEditor me = Thing_new (IntensityTierEditor);
		RealTierEditor_init (me.peek(), title, (RealTier) intensity, sound, ownSound);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("IntensityTier window not created.");
	}
}

/* End of file IntensityTierEditor.cpp */
