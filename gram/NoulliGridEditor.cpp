/* NoulliGridEditor.cpp
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

#include "NoulliGridEditor.h"
#include "EditorM.h"

#include "enums_getText.h"
#include "NoulliGridEditor_enums.h"
#include "enums_getValue.h"
#include "NoulliGridEditor_enums.h"

Thing_implement (NoulliGridEditor, FunctionEditor, 0);

#include "Prefs_define.h"
#include "NoulliGridEditor_prefs.h"
#include "Prefs_install.h"
#include "NoulliGridEditor_prefs.h"
#include "Prefs_copyToInstance.h"
#include "NoulliGridEditor_prefs.h"

static void drawSelectionOrWindow (NoulliGridEditor me, double tmin, double tmax, conststring32 header) {
	for (integer itier = 1; itier <= my noulliGrid() -> tiers.size; itier ++) {
		if (itier == 1) {
			Graphics_setColour (my graphics.get(), Melder_BLACK);
			Graphics_setTextAlignment (my graphics.get(), kGraphics_horizontalAlignment::CENTRE, Graphics_BOTTOM);
			Graphics_text (my graphics.get(), 0.0, 1.0, header);
		}
		autoNoulliPoint average = NoulliGrid_average (my noulliGrid(), itier, tmin, tmax);
		integer winningCategory = NoulliPoint_getWinningCategory (average.get());
		conststring32 winningCategoryName = my noulliGrid() -> categoryNames [winningCategory].get();
		if (winningCategory != 0 && average -> probabilities [winningCategory] > 1.0/3.0) {
			const bool shouldDrawPicture =
				(my instancePref_showCategoryInSelectionViewerAs() == kNoulliGridEditor_showCategoryInSelectionViewerAs::PICTURE ||
				 my instancePref_showCategoryInSelectionViewerAs() == kNoulliGridEditor_showCategoryInSelectionViewerAs::PICTURE_AND_TEXT)
				&&
				(Melder_equ_firstCharacterCaseInsensitive (winningCategoryName, U"happy") ||
				 Melder_equ_firstCharacterCaseInsensitive (winningCategoryName, U"neutral") ||
				 Melder_equ_firstCharacterCaseInsensitive (winningCategoryName, U"sad") ||
				 Melder_equ_firstCharacterCaseInsensitive (winningCategoryName, U"scared") ||
				 Melder_equ_firstCharacterCaseInsensitive (winningCategoryName, U"angry"));
			if (shouldDrawPicture) {
				Graphics_setColour (my graphics.get(), Melder_cyclingBackgroundColour (winningCategory));
				Graphics_fillEllipse (my graphics.get(), -0.985, +0.985, -0.985, +0.985);
				Graphics_setColour (my graphics.get(), Melder_cyclingTextColour (winningCategory));
				/*
					Draw the eyes.
				*/
				if (Melder_equ_firstCharacterCaseInsensitive (winningCategoryName, U"happy")) {
					Graphics_setLineWidth (my graphics.get(), 15.0);
					const double dx = 0.375, y = 0.10, radius = 0.18, angle = 85.0;
					Graphics_arc (my graphics.get(), - dx, y, radius, 90.0 - angle, 90.0 + angle);   // left eye
					Graphics_arc (my graphics.get(), + dx, y, radius, 90.0 - angle, 90.0 + angle);   // right eye
				} else {
					const double dx = 0.375, y = 0.1875, radius = 0.125;
					Graphics_fillCircle (my graphics.get(), - dx, y, radius);   // left eye
					Graphics_fillCircle (my graphics.get(), + dx, y, radius);   // right eye
				}
				/*
					Draw the mouth.
				*/
				Graphics_setLineWidth (my graphics.get(), 8.0);
				if (Melder_equ_firstCharacterCaseInsensitive (winningCategoryName, U"happy")) {
					const double y = -0.20, radius = 0.35, angle = 55.0;
					Graphics_arc (my graphics.get(), 0.0, y, radius, 270.0 - angle, 270.0 + angle);
				} else if (Melder_equ_firstCharacterCaseInsensitive (winningCategoryName, U"neutral")) {
					Graphics_setLineWidth (my graphics.get(), 13.0);
					const double dx = 0.20, y = -0.50;
					Graphics_line (my graphics.get(), - dx, y, + dx, y);
				} else if (Melder_equ_firstCharacterCaseInsensitive (winningCategoryName, U"sad")) {
					const double y = -0.80, radius = 0.35, angle = 55.0;
					Graphics_arc (my graphics.get(), 0.0, y, radius, 90.0 - angle, 90.0 + angle);
				} else if (Melder_equ_firstCharacterCaseInsensitive (winningCategoryName, U"scared")) {
					const double y = -0.50, dx = 0.10, dy = 0.25;
					Graphics_fillEllipse (my graphics.get(), - dx, + dx, y - dy, y + dy);
				} else if (Melder_equ_firstCharacterCaseInsensitive (winningCategoryName, U"angry")) {
					const double y = -0.80, radius = 0.35, angle = 55.0;
					Graphics_arc (my graphics.get(), 0.0, y, radius, 90.0 - angle, 90.0 + angle);
				}
				/*
					Draw the eyebrows.
				*/
				Graphics_setLineWidth (my graphics.get(), 12.0);
				if (Melder_equ_firstCharacterCaseInsensitive (winningCategoryName, U"sad")) {
					const double dx1 = 0.60, dx2 = 0.30, y1 = 0.40, y2 = 0.60;
					Graphics_line (my graphics.get(), - dx2, y2, - dx1, y1);   // left eyebrow
					Graphics_line (my graphics.get(), + dx1, y1, + dx2, y2);   // right eyebrow
				} else if (Melder_equ_firstCharacterCaseInsensitive (winningCategoryName, U"scared")) {
					const double dx1 = 0.60, dx2 = 0.30, y1 = 0.45, y2 = 0.60;
					Graphics_line (my graphics.get(), - dx2, y2, - dx1, y1);   // left eyebrow
					Graphics_line (my graphics.get(), + dx1, y1, + dx2, y2);   // right eyebrow
				} else if (Melder_equ_firstCharacterCaseInsensitive (winningCategoryName, U"angry")) {
					const double dx1 = 0.25, dx2 = 0.55, y1 = 0.40, y2 = 0.60;
					Graphics_line (my graphics.get(), - dx2, y2, - dx1, y1);   // left eyebrow
					Graphics_line (my graphics.get(), + dx1, y1, + dx2, y2);   // right eyebrow
				}
				Graphics_setLineWidth (my graphics.get(), 1.0);
			} else {
				Graphics_setColour (my graphics.get(), Melder_cyclingBackgroundColour (winningCategory));
				Graphics_fillEllipse (my graphics.get(), -0.985, +0.985, -0.985, +0.985);
			}
			if (my instancePref_showCategoryInSelectionViewerAs() == kNoulliGridEditor_showCategoryInSelectionViewerAs::COLOUR_AND_TEXT ||
				my instancePref_showCategoryInSelectionViewerAs() == kNoulliGridEditor_showCategoryInSelectionViewerAs::PICTURE_AND_TEXT ||
				my instancePref_showCategoryInSelectionViewerAs() == kNoulliGridEditor_showCategoryInSelectionViewerAs::PICTURE && ! shouldDrawPicture)
			{
				Graphics_setColour (my graphics.get(), Melder_cyclingTextColour (winningCategory));
				Graphics_setTextAlignment (my graphics.get(), kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
				Graphics_text (my graphics.get(), 0.0, 0.0, winningCategoryName);
			}
		} else {
			Graphics_setColour (my graphics.get(), Melder_WHITE);
			Graphics_fillEllipse (my graphics.get(), -0.985, +0.985, -0.985, +0.985);
			Graphics_setColour (my graphics.get(), Melder_BLACK);
			Graphics_setTextAlignment (my graphics.get(), kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
			Graphics_text (my graphics.get(), 0.0, 0.0, U"?");
		}
	}
	Graphics_setColour (my graphics.get(), Melder_BLACK);
}

void structNoulliGridEditor :: v_drawSelectionViewer () {
	Graphics_setWindow (our graphics.get(), -1.0, +1.0, -1.0, +1.0);
	Graphics_setColour (our graphics.get(), DataGuiColour_WINDOW_BACKGROUND);
	Graphics_fillRectangle (our graphics.get(), -1.0, +1.0, -1.0, +1.0);
	drawSelectionOrWindow (this, our startSelection, our endSelection, U"");
}

void structNoulliGridEditor :: v_drawRealTimeSelectionViewer (double time) {
	Graphics_setWindow (our graphics.get(), -1.0, +1.0, -1.0, +1.0);
	drawSelectionOrWindow (this, time - 1.0, time + 1.0, U"");
}

OPTIONMENU_ENUM_VARIABLE (kNoulliGridEditor_showCategoryInSelectionViewerAs, v_prefs_addFields__showCategoryInSelectionViewerAs)
void structNoulliGridEditor :: v_prefs_addFields (EditorCommand cmd) {
	OPTIONMENU_ENUM_FIELD (kNoulliGridEditor_showCategoryInSelectionViewerAs, v_prefs_addFields__showCategoryInSelectionViewerAs,
			U"Show category in selection viewer as", kNoulliGridEditor_showCategoryInSelectionViewerAs::DEFAULT)
}
void structNoulliGridEditor :: v_prefs_setValues (EditorCommand cmd) {
	SET_ENUM (v_prefs_addFields__showCategoryInSelectionViewerAs, kNoulliGridEditor_showCategoryInSelectionViewerAs, our instancePref_showCategoryInSelectionViewerAs())
}
void structNoulliGridEditor :: v_prefs_getValues (EditorCommand /* cmd */) {
	our setInstancePref_showCategoryInSelectionViewerAs (v_prefs_addFields__showCategoryInSelectionViewerAs);
}

autoNoulliGridEditor NoulliGridEditor_create (conststring32 title, NoulliGrid noulliGrid, Sound optionalSoundToCopy) {
	try {
		autoNoulliGridEditor me = Thing_new (NoulliGridEditor);
		my noulliGridArea() = NoulliGridArea_create (true, nullptr, me.get());
		if (optionalSoundToCopy)
			my soundArea() = SoundArea_create (false, optionalSoundToCopy, me.get());
		FunctionEditor_init (me.get(), title, noulliGrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"NoulliGrid window not created.");
	}
}

/* End of file NoulliGridEditor.cpp */

