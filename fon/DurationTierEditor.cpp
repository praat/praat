/* DurationTierEditor.cpp
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

#include "DurationTierEditor.h"
#include "EditorM.h"

Thing_implement (DurationTierEditor, RealTierEditor, 0);

static void menu_cb_DurationTierHelp (EDITOR_ARGS) { EDITOR_IAM (DurationTierEditor); Melder_help (L"DurationTier"); }

void structDurationTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	DurationTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"DurationTier help", 0, menu_cb_DurationTierHelp);
}

void structDurationTierEditor :: v_play (double a_tmin, double a_tmax) {
	if (d_sound.data) {
		Sound_playPart (d_sound.data, a_tmin, a_tmax, NULL, NULL);
	} else {
		//DurationTier_playPart (data, a_tmin, a_tmax, FALSE);
	}
}

DurationTierEditor DurationTierEditor_create (const wchar_t *title, DurationTier duration, Sound sound, bool ownSound) {
	try {
		autoDurationTierEditor me = Thing_new (DurationTierEditor);
		RealTierEditor_init (me.peek(), title, (RealTier) duration, sound, ownSound);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("DurationTier window not created.");
	}
}

/* End of file DurationTierEditor.cpp */
