/* PitchEditor.cpp
 *
 * Copyright (C) 1992-2012,2014-2022 Paul Boersma
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

#include "PitchEditor.h"
#include "EditorM.h"

Thing_implement (PitchEditor, FunctionEditor, 0);

static void menu_cb_PitchEditorHelp (PitchEditor, EDITOR_ARGS) { Melder_help (U"PitchEditor"); }
static void menu_cb_PitchHelp (PitchEditor, EDITOR_ARGS) { Melder_help (U"Pitch"); }

void structPitchEditor :: v_createMenuItems_help (EditorMenu menu) {
	PitchEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"PitchEditor help", U'?', menu_cb_PitchEditorHelp);
	EditorMenu_addCommand (menu, U"Pitch help", 0, menu_cb_PitchHelp);
}

autoPitchEditor PitchEditor_create (conststring32 title, Pitch pitch) {
	try {
		autoPitchEditor me = Thing_new (PitchEditor);
		my pitchArea() = PitchArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, pitch);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Pitch window not created.");
	}
}

/* End of file PitchEditor.cpp */
