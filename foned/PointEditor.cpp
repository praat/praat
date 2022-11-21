/* PointEditor.cpp
 *
 * Copyright (C) 1992-2022 Paul Boersma
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

#include "PointEditor.h"
#include "EditorM.h"
#include "VoiceAnalysis.h"

Thing_implement (PointEditor, FunctionEditor, 0);

static void HELP__PointEditorHelp (PointEditor, EDITOR_ARGS) {
	HELP (U"PointEditor")
}

void structPointEditor :: v_createMenuItems_help (EditorMenu menu) {
	PointEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"PointEditor help", '?',
			HELP__PointEditorHelp);
}

autoPointEditor PointEditor_create (conststring32 title, PointProcess pointProcess, Sound optionalSoundToCopy) {
	try {
		autoPointEditor me = Thing_new (PointEditor);
		my pointArea() = PointArea_create (true, nullptr, me.get());
		if (optionalSoundToCopy) {
			autoSound monoSound = Sound_convertToMono (optionalSoundToCopy);
			my soundArea() = SoundArea_create (false, monoSound.get(), me.get());   // BUG: double copy
			my pointArea() -> borrowedSoundArea = my soundArea().get();
		}
		FunctionEditor_init (me.get(), title, pointProcess);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PointProcess window not created.");
	}
}

/* End of file PointEditor.cpp */
