/* AmplitudeTierEditor.cpp
 *
 * Copyright (C) 2003-2012,2014-2016,2018,2020-2022 Paul Boersma
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

#include "AmplitudeTierEditor.h"
#include "EditorM.h"

Thing_implement (AmplitudeTierEditor, RealTierEditor, 0);

static void HELP_AmplitudeTierHelp (AmplitudeTierEditor /* me */, EDITOR_ARGS) {
	HELP (U"AmplitudeTier")
}

void structAmplitudeTierEditor :: v_createMenuItems_help (EditorMenu menu) {
	AmplitudeTierEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"AmplitudeTier help", 0, HELP_AmplitudeTierHelp);
}

void structAmplitudeTierEditor :: v_play (double startTime, double endTime) {
	if (our soundArea()) {
		Sound_playPart (our soundArea() -> sound(), startTime, endTime, theFunctionEditor_playCallback, this);
	} else {
		//AmplitudeTier_playPart (data, startTime, endTime, false);
	}
}

autoAmplitudeTierEditor AmplitudeTierEditor_create (conststring32 title, AmplitudeTier amplitudeTier, Sound soundToCopy) {
	try {
		autoAmplitudeTierEditor me = Thing_new (AmplitudeTierEditor);
		my realTierArea() = AmplitudeTierArea_create (true, nullptr, me.get());
		if (soundToCopy)
			my soundArea() = SoundArea_create (false, soundToCopy, me.get());
		FunctionEditor_init (me.get(), title, amplitudeTier);
		return me;
	} catch (MelderError) {
		Melder_throw (U"AmplitudeTier window not created.");
	}
}

/* End of file AmplitudeTierEditor.cpp */
