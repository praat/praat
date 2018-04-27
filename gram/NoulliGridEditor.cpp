/* NoulliGridEditor.cpp
 *
 * Copyright (C) 2018 Paul Boersma
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

#include "NoulliGridEditor.h"
#include "EditorM.h"

Thing_implement (NoulliGridEditor, TimeSoundEditor, 0);

#define SOUND_HEIGHT  0.382

/********** DRAWING AREA **********/

void structNoulliGridEditor :: v_draw () {
	NoulliGrid data = (NoulliGrid) our data;
	Graphics_Viewport viewport;
	if (our d_sound.data) {
		viewport = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 1.0 - SOUND_HEIGHT, 1.0);
		Graphics_setColour (our graphics.get(), Graphics_WHITE);
		Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		TimeSoundEditor_drawSound (this, -1.0, 1.0);
		Graphics_resetViewport (our graphics.get(), viewport);
		Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 0.0, 1.0 - SOUND_HEIGHT);
	}
	Graphics_setColour (our graphics.get(), Graphics_WHITE);
	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, data -> tiers.size);
	for (integer itier = 1; itier <= data -> tiers.size; itier ++) {
		NoulliTier tier = data -> tiers.at [itier];
		double ymin = data -> tiers.size - itier, ymax = ymin + 1;
		for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
			NoulliPoint point = tier -> points.at [ipoint];
			if (point -> xmax > our startWindow && point -> xmin < our endWindow) {
				double xmin = point -> xmin > our startWindow ? point -> xmin : our startWindow;
				double xmax = point -> xmax < our endWindow ? point -> xmax : our endWindow;
				double prob1 = 1.0, prob2;
				for (integer icategory = 1; icategory <= point -> numberOfCategories; icategory ++) {
					prob2 = prob1;
					prob1 -= point -> probabilities [icategory];
					Graphics_setColour (our graphics.get(), Graphics_cyclingBackgroundColour (icategory));
					Graphics_fillRectangle (our graphics.get(), xmin, xmax, ymin + prob1 * (ymax - ymin), ymin + prob2 * (ymax - ymin));
				}
			}
		}
		Graphics_setColour (our graphics.get(), Graphics_BLACK);
		if (itier > 1) {
			Graphics_setLineWidth (our graphics.get(), 1.0);
			Graphics_line (our graphics.get(), our startWindow, ymax, our endWindow, ymax);
		}
	}
	Graphics_setLineWidth (our graphics.get(), 1.0);
	Graphics_setColour (our graphics.get(), Graphics_BLACK);
	our v_updateMenuItems_file ();
}

void structNoulliGridEditor :: v_play (double a_tmin, double a_tmax) {
	if (our d_sound.data)
		Sound_playPart (our d_sound.data, a_tmin, a_tmax, theFunctionEditor_playCallback, this);
}

void NoulliGridEditor_init (NoulliGridEditor me, const char32 *title, NoulliGrid data, Sound sound, bool ownSound) {
	Melder_assert (data);
	Melder_assert (Thing_isa (data, classNoulliGrid));
	TimeSoundEditor_init (me, title, data, sound, ownSound);
}

autoNoulliGridEditor NoulliGridEditor_create (const char32 *title, NoulliGrid grid, Sound sound, bool ownSound) {
	try {
		autoNoulliGridEditor me = Thing_new (NoulliGridEditor);
		NoulliGridEditor_init (me.get(), title, grid, sound, ownSound);
		return me;
	} catch (MelderError) {
		Melder_throw (U"NoulliGrid window not created.");
	}
}

/* End of file NoulliGridEditor.cpp */

