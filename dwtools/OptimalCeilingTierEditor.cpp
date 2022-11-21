/* OptimalCeilingTierEditor.cpp
 *
 * Copyright (C) 2015-2019 David Weenink, 2022 Paul Boersma
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

Thing_implement (OptimalCeilingTierEditor, RealTierEditor, 0);

static void menu_cb_OptimalCeilingTierHelp (OptimalCeilingTierEditor, EDITOR_ARGS) { Melder_help (U"OptimalCeilingTier"); }

void structOptimalCeilingTierEditor :: v_createMenuItems_help (EditorMenu menu) {
	OptimalCeilingTierEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"OptimalCeilingTier help", 0, menu_cb_OptimalCeilingTierHelp);
}

void structOptimalCeilingTierEditor :: v_play (double startTime, double endTime) {
	if (our soundArea())
		Sound_playPart (our soundArea() -> sound(), startTime, endTime, theFunctionEditor_playCallback, this);
	//else
	//	OptimalCeilingTier_playPart (data, startTime, endTime, false);
}

autoOptimalCeilingTierEditor OptimalCeilingTierEditor_create (conststring32 title,
	OptimalCeilingTier optimalCeilingTier, Sound soundToCopy)
{
	try {
		autoOptimalCeilingTierEditor me = Thing_new (OptimalCeilingTierEditor);
		my realTierArea() = OptimalCeilingTierArea_create (true, nullptr, me.get());
		if (soundToCopy)
			my soundArea() = SoundArea_create (false, soundToCopy, me.get());
		FunctionEditor_init (me.get(), title, optimalCeilingTier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"OptimalCeilingTier window not created.");
	}
}

/* End of file OptimalCeilingTierEditor.cpp */
