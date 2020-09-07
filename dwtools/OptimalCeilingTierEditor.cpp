/* OptimalCeilingTierEditor.cpp
 *
 * Copyright (C) 2015-2019 David Weenink
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

#include "OptimalCeilingTierEditor.h"
#include "EditorM.h"

Thing_implement (OptimalCeilingTierArea, RealTierArea, 0);

Thing_implement (OptimalCeilingTierEditor, RealTierEditor, 0);

static void menu_cb_OptimalCeilingTierHelp (OptimalCeilingTierEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"OptimalCeilingTier"); }

void structOptimalCeilingTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	OptimalCeilingTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"OptimalCeilingTier help", 0, menu_cb_OptimalCeilingTierHelp);
}

void structOptimalCeilingTierEditor :: v_play (double startTime, double endTime) {
	if (our d_sound.data)
		Sound_playPart (our d_sound.data, startTime, endTime, theFunctionEditor_playCallback, this);
	//else
	//	OptimalCeilingTier_playPart (data, startTime, endTime, false);
}

autoOptimalCeilingTierEditor OptimalCeilingTierEditor_create (conststring32 title, OptimalCeilingTier octier, Sound sound, bool ownSound) {
	try {
		autoOptimalCeilingTierEditor me = Thing_new (OptimalCeilingTierEditor);
		RealTierEditor_init (me.get(), classOptimalCeilingTierArea, title, octier, sound, ownSound);
		return me;
	} catch (MelderError) {
		Melder_throw (U"OptimalCeilingTier window not created.");
	}
}

/* End of file OptimalCeilingTierEditor.cpp */
