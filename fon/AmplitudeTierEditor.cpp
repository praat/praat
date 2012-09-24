/* AmplitudeTierEditor.cpp
 *
 * Copyright (C) 2003-2011,2012 Paul Boersma
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

#include "AmplitudeTierEditor.h"
#include "EditorM.h"

Thing_implement (AmplitudeTierEditor, RealTierEditor, 0);

static void menu_cb_AmplitudeTierHelp (EDITOR_ARGS) { EDITOR_IAM (AmplitudeTierEditor); Melder_help (L"AmplitudeTier"); }

void structAmplitudeTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	AmplitudeTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"AmplitudeTier help", 0, menu_cb_AmplitudeTierHelp);
}

void structAmplitudeTierEditor :: v_play (double tmin, double tmax) {
	if (d_sound.data) {
		Sound_playPart (d_sound.data, tmin, tmax, theFunctionEditor_playCallback, this);
	} else {
		//AmplitudeTier_playPart (data, tmin, tmax, FALSE);
	}
}

AmplitudeTierEditor AmplitudeTierEditor_create (const wchar_t *title, AmplitudeTier amplitude, Sound sound, bool ownSound) {
	try {
		autoAmplitudeTierEditor me = Thing_new (AmplitudeTierEditor);
		RealTierEditor_init (me.peek(), title, (RealTier) amplitude, sound, ownSound);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("AmplitudeTier window not created.");
	}
}

/* End of file AmplitudeTierEditor.cpp */
