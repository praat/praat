/* NoulliGridArea.cpp
 *
 * Copyright (C) 2018-2023 Paul Boersma
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

#include "NoulliGridArea.h"
#include "EditorM.h"

Thing_implement (NoulliGridArea, FunctionArea, 0);

void structNoulliGridArea :: v_drawInside () {
	NoulliGrid_paintInside (our noulliGrid(), our graphics(), our startWindow(), our endWindow());
}

#pragma mark - NoulliGridArea Query selection

static void INFO_selection (NoulliGridArea me, EDITOR_ARGS) {
	INFO_EDITOR
		MelderInfo_open ();
		for (integer itier = 1; itier <= my noulliGrid() -> tiers.size; itier ++) {
			MelderInfo_writeLine (U"Tier number: ", itier);
			autoNoulliPoint average = NoulliGrid_average (my noulliGrid(), itier, my startSelection(), my endSelection());
			for (integer icat = 1; icat <= average -> numberOfCategories; icat ++)
				MelderInfo_writeLine (U"   p(", icat, U") = ", Melder_fixed (average -> probabilities [icat], 6),
						U"   ; ", my noulliGrid() -> categoryNames [icat].get());
			const integer winningCategory = NoulliPoint_getWinningCategory (average.get());
			MelderInfo_writeLine (U"   Winner: ", winningCategory, U"   ; ",
					my noulliGrid() -> categoryNames [winningCategory].get());
		}
		MelderInfo_close ();
	INFO_EDITOR_END
}

void structNoulliGridArea :: v_createMenus () {
	NoulliGridArea_Parent :: v_createMenus ();   // does nothing (last checked 2023-02-06)

	EditorMenu menu = Editor_addMenu (our functionEditor(), U"NoulliGrid", 0);

	FunctionAreaMenu_addCommand (menu, U"- Query selected part of NoulliGrid:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Info on selection", 'P' + GuiMenu_DEPTH_1, INFO_selection, this);
}

/* End of file NoulliGridArea.cpp */
