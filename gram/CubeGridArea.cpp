/* CubeGridArea.cpp
 *
 * Copyright (C) 2023 Paul Boersma
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

#include "CubeGridArea.h"
#include "EditorM.h"

Thing_implement (CubeGridArea, FunctionArea, 0);

void structCubeGridArea :: v_drawInside () {
	CubeGrid_paintInside (our cubeGrid(), our graphics(), our startWindow(), our endWindow());
}

#pragma mark - CubeGridArea Query selection

static void INFO_selection (CubeGridArea me, EDITOR_ARGS) {
	INFO_EDITOR
		MelderInfo_open ();
		for (integer itier = 1; itier <= my cubeGrid() -> tiers.size; itier ++) {
			MelderInfo_writeLine (U"Tier number: ", itier);
			autoCubePoint average = CubeGrid_average (my cubeGrid(), itier, my startSelection(), my endSelection());
			MelderInfo_writeLine (
				U"   red   = ", Melder_fixed (average -> red,   6), U"   ; ", my cubeGrid() -> redName.get(),
				U"   green = ", Melder_fixed (average -> green, 6), U"   ; ", my cubeGrid() -> greenName.get(),
				U"   blue  = ", Melder_fixed (average -> blue,  6), U"   ; ", my cubeGrid() -> blueName.get()
			);
		}
		MelderInfo_close ();
	INFO_EDITOR_END
}

void structCubeGridArea :: v_createMenus () {
	CubeGridArea_Parent :: v_createMenus ();   // does nothing (last checked 2023-02-06)

	EditorMenu menu = Editor_addMenu (our functionEditor(), U"CubeGrid", 0);

	FunctionAreaMenu_addCommand (menu, U"- Query selected part of CubeGrid:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Info on selection", 'P' + GuiMenu_DEPTH_1, INFO_selection, this);
}

/* End of file CubeGridArea.cpp */
