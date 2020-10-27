/* NoulliGridEditor.cpp
 *
 * Copyright (C) 2018-2020 Paul Boersma
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

Thing_implement (NoulliGridEditor, TimeSoundEditor, 0);

#include "prefs_define.h"
#include "NoulliGridEditor_prefs.h"
#include "prefs_install.h"
#include "NoulliGridEditor_prefs.h"
#include "prefs_copyToInstance.h"
#include "NoulliGridEditor_prefs.h"

#define SOUND_HEIGHT  0.2

/********** DRAWING AREA **********/

void structNoulliGridEditor :: v_draw () {
	NoulliGrid data = (NoulliGrid) our data;
	if (our d_sound.data) {
		Graphics_Viewport viewport = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 1.0 - SOUND_HEIGHT, 1.0);
		Graphics_setColour (our graphics.get(), Melder_WHITE);
		Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		TimeSoundEditor_drawSound (this, -1.0, 1.0);
		Graphics_resetViewport (our graphics.get(), viewport);
		Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 0.0, 1.0 - SOUND_HEIGHT);
	}
	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	if (data -> numberOfCategories == 2) {
		Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, 1.0);
		Graphics_setLineWidth (our graphics.get(), 3.0);
		for (integer itier = 1; itier <= data -> tiers.size; itier ++) {
			NoulliTier tier = data -> tiers.at [itier];
			for (integer ipoint = 1; ipoint < tier -> points.size; ipoint ++) {
				NoulliPoint point = tier -> points.at [ipoint], nextPoint = tier -> points.at [ipoint + 1];
				const double time = 0.5 * (point -> xmin + point -> xmax);
				const double nextTime = 0.5 * (nextPoint -> xmin + nextPoint -> xmax);
				if (time > our startWindow && nextTime < our endWindow) {
					const double prob = point -> probabilities [1], nextProb = nextPoint -> probabilities [1];
					Graphics_setColour (our graphics.get(), Melder_cyclingBackgroundColour (itier));
					Graphics_line (our graphics.get(), time, prob, nextTime, nextProb);
				}
			}
			Graphics_setColour (our graphics.get(), Melder_BLACK);
		}
	} else {
		Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, 0.0, data -> tiers.size);
		for (integer itier = 1; itier <= data -> tiers.size; itier ++) {
			NoulliTier tier = data -> tiers.at [itier];
			const double ymin = data -> tiers.size - itier, ymax = ymin + 1;
			for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
				NoulliPoint point = tier -> points.at [ipoint];
				if (point -> xmax > our startWindow && point -> xmin < our endWindow) {
					const double xmin = Melder_clippedLeft (our startWindow, point -> xmin);
					const double xmax = Melder_clippedRight (point -> xmax, our endWindow);
					double prob1 = 1.0;
					for (integer icategory = 1; icategory <= point -> numberOfCategories; icategory ++) {
						const double prob2 = prob1;
						prob1 -= point -> probabilities [icategory];
						Graphics_setColour (our graphics.get(), Melder_cyclingBackgroundColour (icategory));
						Graphics_fillRectangle (our graphics.get(), xmin, xmax,
								ymin + prob1 * (ymax - ymin), ymin + prob2 * (ymax - ymin));
					}
				}
			}
			Graphics_setColour (our graphics.get(), Melder_BLACK);
			if (itier > 1) {
				Graphics_setLineWidth (our graphics.get(), 1.0);
				Graphics_line (our graphics.get(), our startWindow, ymax, our endWindow, ymax);
			}
		}
	}
	Graphics_setLineWidth (our graphics.get(), 1.0);
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	our v_updateMenuItems_file ();
}

void structNoulliGridEditor :: v_play (double startTime, double endTime) {
	if (our d_sound.data)
		Sound_playPart (our d_sound.data, startTime, endTime, theFunctionEditor_playCallback, this);
}

static void drawSelectionOrWindow (NoulliGridEditor me, double xmin, double xmax, double tmin, double tmax, conststring32 header) {
	NoulliGrid grid = (NoulliGrid) my data;
	for (integer itier = 1; itier <= grid -> tiers.size; itier ++) {
		if (itier == 1) {
			Graphics_setColour (my graphics.get(), Melder_BLACK);
			Graphics_setTextAlignment (my graphics.get(), kGraphics_horizontalAlignment::CENTRE, Graphics_BOTTOM);
			Graphics_text (my graphics.get(), 0.0, 1.0, header);
		}
		autoNoulliPoint average = NoulliGrid_average (grid, itier, tmin, tmax);
		integer winningCategory = NoulliPoint_getWinningCategory (average.get());
		conststring32 winningCategoryName = grid -> categoryNames [winningCategory].get();
		if (winningCategory != 0 && average -> probabilities [winningCategory] > 1.0/3.0) {
			const bool shouldDrawPicture =
				(my p_showCategoryInSelectionViewerAs == kNoulliGridEditor_showCategoryInSelectionViewerAs::PICTURE ||
				 my p_showCategoryInSelectionViewerAs == kNoulliGridEditor_showCategoryInSelectionViewerAs::PICTURE_AND_TEXT)
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
			if (my p_showCategoryInSelectionViewerAs == kNoulliGridEditor_showCategoryInSelectionViewerAs::COLOUR_AND_TEXT ||
				my p_showCategoryInSelectionViewerAs == kNoulliGridEditor_showCategoryInSelectionViewerAs::PICTURE_AND_TEXT ||
				my p_showCategoryInSelectionViewerAs == kNoulliGridEditor_showCategoryInSelectionViewerAs::PICTURE && ! shouldDrawPicture)
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
	Graphics_setColour (our graphics.get(), Melder_WINDOW_BACKGROUND_COLOUR);
	Graphics_fillRectangle (our graphics.get(), -1.0, +1.0, -1.0, +1.0);
	drawSelectionOrWindow (this, 0.0, 1.0, our startSelection, our endSelection, U"");
	//drawSelectionOrWindow (this, 0.0, 0.5, our startSelection, our endSelection,
	//	our tmin == our tmax ? U"Cursor" : U"Selection");
	//drawSelectionOrWindow (this, 0.5, 1.0, our startWindow, our endWindow, U"Window");
}

void structNoulliGridEditor :: v_drawRealTimeSelectionViewer (double time) {
	Graphics_setWindow (our graphics.get(), -1.0, +1.0, -1.0, +1.0);
	drawSelectionOrWindow (this, 0.0, 1.0, time - 1.0, time + 1.0, U"");
}

OPTIONMENU_ENUM_VARIABLE (kNoulliGridEditor_showCategoryInSelectionViewerAs, v_prefs_addFields_showCategoryInSelectionViewerAs)
void structNoulliGridEditor :: v_prefs_addFields (EditorCommand cmd) {
	OPTIONMENU_ENUM_FIELD (kNoulliGridEditor_showCategoryInSelectionViewerAs, v_prefs_addFields_showCategoryInSelectionViewerAs,
			U"Show category in selection viewer as", kNoulliGridEditor_showCategoryInSelectionViewerAs::DEFAULT)
}
void structNoulliGridEditor :: v_prefs_setValues (EditorCommand cmd) {
	SET_ENUM (v_prefs_addFields_showCategoryInSelectionViewerAs, kNoulliGridEditor_showCategoryInSelectionViewerAs, our p_showCategoryInSelectionViewerAs)
}
void structNoulliGridEditor :: v_prefs_getValues (EditorCommand /* cmd */) {
	our pref_showCategoryInSelectionViewerAs () = our p_showCategoryInSelectionViewerAs = v_prefs_addFields_showCategoryInSelectionViewerAs;
	FunctionEditor_redraw (this);
}

void NoulliGridEditor_init (NoulliGridEditor me, conststring32 title, NoulliGrid data, Sound sound, bool ownSound) {
	Melder_assert (data);
	Melder_assert (Thing_isa (data, classNoulliGrid));
	TimeSoundEditor_init (me, title, data, sound, ownSound);
}

autoNoulliGridEditor NoulliGridEditor_create (conststring32 title, NoulliGrid grid, Sound sound, bool ownSound) {
	try {
		autoNoulliGridEditor me = Thing_new (NoulliGridEditor);
		NoulliGridEditor_init (me.get(), title, grid, sound, ownSound);
		return me;
	} catch (MelderError) {
		Melder_throw (U"NoulliGrid window not created.");
	}
}

/* End of file NoulliGridEditor.cpp */

