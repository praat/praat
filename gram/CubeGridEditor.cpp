/* CubeGridEditor.cpp
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

#include "CubeGridEditor.h"
#include "EditorM.h"

Thing_implement (CubeGridEditor, FunctionEditor, 0);

#include "Prefs_define.h"
#include "CubeGridEditor_prefs.h"
#include "Prefs_install.h"
#include "CubeGridEditor_prefs.h"
#include "Prefs_copyToInstance.h"
#include "CubeGridEditor_prefs.h"

static void drawSelectionOrWindow (CubeGridEditor me, double tmin, double tmax, conststring32 header) {
	for (integer itier = 1; itier <= my cubeGrid() -> tiers.size; itier ++) {
		if (itier == 1) {
			Graphics_setColour (my graphics.get(), Melder_BLACK);
			Graphics_setTextAlignment (my graphics.get(), kGraphics_horizontalAlignment::CENTRE, Graphics_BOTTOM);
			Graphics_text (my graphics.get(), 0.0, 1.0, header);
		}
		autoCubePoint average = CubeGrid_average (my cubeGrid(), itier, tmin, tmax);
		Graphics_setColour (my graphics.get(), MelderColour (average -> red, average -> green, average -> blue));
		Graphics_fillRectangle (my graphics.get(), 0.15, 0.85, 0.3, 1.0);
		Graphics_setColour (my graphics.get(), Melder_BLACK);
		Graphics_setTextAlignment (my graphics.get(), kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
		Graphics_text (my graphics.get(), 0.5, 0.25,
				Melder_cat (my cubeGrid() -> redName.get(),   U": ", Melder_fixed (average -> red,   6)));
		Graphics_text (my graphics.get(), 0.5, 0.15,
				Melder_cat (my cubeGrid() -> greenName.get(), U": ", Melder_fixed (average -> green, 6)));
		Graphics_text (my graphics.get(), 0.5, 0.05,
				Melder_cat (my cubeGrid() -> blueName.get(),  U": ", Melder_fixed (average -> blue,  6)));
	}
}

void structCubeGridEditor :: v_drawSelectionViewer () {
	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), DataGuiColour_WINDOW_BACKGROUND);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	drawSelectionOrWindow (this, our startSelection, our endSelection, U"");
}

void structCubeGridEditor :: v_drawRealTimeSelectionViewer (double time) {
	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	drawSelectionOrWindow (this, time - 1.0, time + 1.0, U"");
}

autoCubeGridEditor CubeGridEditor_create (conststring32 title, CubeGrid cubeGrid, Sound optionalSoundToCopy) {
	try {
		autoCubeGridEditor me = Thing_new (CubeGridEditor);
		my cubeGridArea() = CubeGridArea_create (true, nullptr, me.get());
		if (optionalSoundToCopy)
			my soundArea() = SoundArea_create (false, optionalSoundToCopy, me.get());
		FunctionEditor_init (me.get(), title, cubeGrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"CubeGrid window not created.");
	}
}

/* End of file CubeGridEditor.cpp */

