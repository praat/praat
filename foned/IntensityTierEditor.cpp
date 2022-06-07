/* IntensityTierEditor.cpp
 *
 * Copyright (C) 1992-2012,2014-2016,2018,2020-2022 Paul Boersma
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

static void HELP_IntensityTierHelp (IntensityTierEditor, EDITOR_ARGS_DIRECT) {
	HELP (U"IntensityTier")
}

void structIntensityTierEditor :: v_createHelpMenuItems (EditorMenu menu) {
	IntensityTierEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"IntensityTier help", 0, HELP_IntensityTierHelp);
}

void structIntensityTierEditor :: v_play (double startTime, double endTime) {
	if (our sound()) {
		Sound_playPart (our sound(), startTime, endTime, theFunctionEditor_playCallback, this);
	} else {
		//IntensityTier_playPart (our data, startTime, endTime, false);
	}
}

autoIntensityTierEditor IntensityTierEditor_create (conststring32 title, IntensityTier intensityTier, Sound sound) {
	try {
		autoIntensityTierEditor me = Thing_new (IntensityTierEditor);
		autoIntensityTierArea mainArea = IntensityTierArea_create (me.get(), intensityTier);
		autoSoundArea soundArea = ( sound ? SoundArea_create (me.get(), sound) : autoSoundArea() );
		RealTierEditor_init (me.get(), mainArea.move(), soundArea.move(), title, true);
		return me;
	} catch (MelderError) {
		Melder_throw (U"IntensityTier window not created.");
	}
}

/* End of file IntensityTierEditor.cpp */
