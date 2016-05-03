/* IntensityTierEditor.cpp
 *
 * Copyright (C) 1992-2011,2012,2014,2015,2016 Paul Boersma
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

#include "IntensityTierEditor.h"
#include "EditorM.h"

Thing_implement (IntensityTierEditor, RealTierEditor, 0);

static void menu_cb_IntensityTierHelp (IntensityTierEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"IntensityTier"); }

void structIntensityTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	IntensityTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"IntensityTier help", 0, menu_cb_IntensityTierHelp);
}

void structIntensityTierEditor :: v_play (double a_tmin, double a_tmax) {
	if (our d_sound.data) {
		Sound_playPart (our d_sound.data, a_tmin, a_tmax, theFunctionEditor_playCallback, this);
	} else {
		//IntensityTier_playPart (our data, a_tmin, a_tmax, false);
	}
}

autoIntensityTierEditor IntensityTierEditor_create (const char32 *title, IntensityTier intensity, Sound sound, bool ownSound) {
	try {
		autoIntensityTierEditor me = Thing_new (IntensityTierEditor);
		RealTierEditor_init (me.get(), title, (RealTier) intensity, sound, ownSound);
		return me;
	} catch (MelderError) {
		Melder_throw (U"IntensityTier window not created.");
	}
}

/* End of file IntensityTierEditor.cpp */
