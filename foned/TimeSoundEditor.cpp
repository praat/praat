/* TimeSoundEditor.cpp
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

#include "NUM2.h"
#include "TimeSoundEditor.h"
#include "EditorM.h"

Thing_implement_pureVirtual (TimeSoundEditor, FunctionEditor, 0);

/*
	TimeSoundEditor is designed to be removed.
	Most of its remaining functionality is to just delegate everything to SoundArea.
	At some point, editors will just have to include a SoundArea instead of deriving from TimeSoundEditor.
*/

void structTimeSoundEditor :: v1_info () {
	TimeSoundEditor_Parent :: v1_info ();
	if (our soundArea)
		our soundArea -> v1_info ();
}

void structTimeSoundEditor:: v_createMenus () {
	TimeSoundEditor_Parent :: v_createMenus ();
	if (our soundArea)
		our soundArea -> v_createMenus ();
}
void structTimeSoundEditor :: v_createMenuItems_file (EditorMenu menu) {
	our TimeSoundEditor_Parent :: v_createMenuItems_file (menu);
	our v_createMenuItems_file_write (menu);
	if (our soundArea)
		our soundArea -> v_createMenuItems_file (menu);
	EditorMenu_addCommand (menu, U"-- after file write --", 0, nullptr);
}
void structTimeSoundEditor :: v_createMenuItems_edit (EditorMenu menu) {
	TimeSoundEditor_Parent :: v_createMenuItems_edit (menu);
	if (our soundArea)
		our soundArea -> v_createMenuItems_edit (menu);
}
void structTimeSoundEditor :: v_updateMenuItems () {
	if (! our soundOrLongSound())
		return;
	if (our soundArea)
		our soundArea -> v_updateMenuItems ();
}

bool structTimeSoundEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double globalY_fraction) {
	if (event -> isClick ())
		our clickedInWideSoundArea = our soundArea -> y_fraction_globalIsInside (globalY_fraction);
	bool result = false;
	if (our clickedInWideSoundArea) {
		result = SoundArea_mouse (our soundArea.get(), event, x_world, globalY_fraction);
	} else {
		result = our TimeSoundEditor_Parent :: v_mouseInWideDataView (event, x_world, globalY_fraction);
	}
	if (event -> isDrop())
		our clickedInWideSoundArea = false;
	return result;
}

/* End of file TimeSoundEditor.cpp */
