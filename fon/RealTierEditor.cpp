/* RealTierEditor.cpp
 *
 * Copyright (C) 1992-2020 Paul Boersma
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

#include "RealTierEditor.h"
#include "EditorM.h"

Thing_implement (RealTierEditor, TimeSoundEditor, 0);

/* MARK: - MENU COMMANDS */

static void menu_cb_removePoints (RealTierEditor me, EDITOR_ARGS_DIRECT) {
	RealTierArea_removePoints (my realTierArea.get(), my realTier());
	Editor_save (me, U"Remove point(s)");
	RealTierEditor_updateScaling (me);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPointAtCursor (RealTierEditor me, EDITOR_ARGS_DIRECT) {
	RealTierArea_addPointAtCursor (my realTierArea.get(), my realTier());
	Editor_save (me, U"Add point");
	RealTierEditor_updateScaling (me);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPointAt (RealTierEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add point", nullptr)
		REAL (time, U"Time (s)", U"0.0")
		REAL (desiredY, my v_quantityText (), U"0.0")
	EDITOR_OK
		SET_REAL (time, 0.5 * (my startSelection + my endSelection))
		SET_REAL (desiredY, my realTierArea -> ycursor)
	EDITOR_DO
		RealTierArea_addPointAt (my realTierArea.get(), my realTier(), time, desiredY);
		Editor_save (me, U"Add point");
		RealTierEditor_updateScaling (me);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_setRange (RealTierEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (my v_setRangeTitle (), nullptr)
		REAL (ymin, my v_yminText (), my v_defaultYminText ())
		REAL (ymax, my v_ymaxText (), my v_defaultYmaxText ())
	EDITOR_OK
		SET_REAL (ymin, my realTierArea -> ymin)
		SET_REAL (ymax, my realTierArea -> ymax)
	EDITOR_DO
		my realTierArea -> ymin = ymin;
		my realTierArea -> ymax = ymax;
		if (my realTierArea -> ymax <= my realTierArea -> ymin)
			RealTierEditor_updateScaling (me);
		FunctionEditor_redraw (me);
	EDITOR_END
}

void structRealTierEditor :: v_createMenuItems_view (EditorMenu menu) {
	RealTierEditor_Parent :: v_createMenuItems_view (menu);
	EditorMenu_addCommand (menu, U"-- view/realtier --", 0, 0);
	EditorMenu_addCommand (menu, v_setRangeTitle (), 0, menu_cb_setRange);
}

void structRealTierEditor :: v_createMenus () {
	RealTierEditor_Parent :: v_createMenus ();
	EditorMenu menu = Editor_addMenu (this, U"Point", 0);
	EditorMenu_addCommand (menu, U"Add point at cursor", 'T', menu_cb_addPointAtCursor);
	EditorMenu_addCommand (menu, U"Add point at...", 0, menu_cb_addPointAt);
	EditorMenu_addCommand (menu, U"-- remove point --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Remove point(s)", GuiMenu_OPTION | 'T', menu_cb_removePoints);
}

void RealTierEditor_updateScaling (RealTierEditor me) {
	RealTierArea_updateScaling (my realTierArea.get(), my realTier());
}

void structRealTierEditor :: v_dataChanged () {
	RealTierEditor_updateScaling (this);
	RealTierEditor_Parent :: v_dataChanged ();
}

/* MARK: - DRAWING AREA */

void structRealTierEditor :: v_draw () {
	if (our d_sound.data) {
		Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 1.0 - SOUND_HEIGHT, 1.0);
		Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (our graphics.get(), Melder_WHITE);
		Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		TimeSoundEditor_drawSound (this, -1.0, 1.0);
	}
	our realTierArea -> setViewport();

	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), Melder_BLACK);
	Graphics_rectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);

	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, our realTierArea -> ymin, our realTierArea -> ymax);
	RealTierArea_draw (our realTierArea.get(), our realTier());
	if (isdefined (our realTierArea -> anchorTime))
		RealTierArea_drawWhileDragging (our realTierArea.get(), our realTier());
	our v_updateMenuItems_file ();   // TODO: this is not about drawing; improve logic? 2020-07-23
}

bool structRealTierEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double globalY_fraction) {
	static bool clickedInWideRealTierArea = false;
	if (event -> isClick ())
		clickedInWideRealTierArea = our realTierArea -> y_fraction_globalIsInside (globalY_fraction);
	bool result = false;
	if (clickedInWideRealTierArea) {
		our realTierArea -> setViewport ();
		result = RealTierArea_mouse (our realTierArea.get(), our realTier(), event, x_world, globalY_fraction);
	} else {
		result = our RealTierEditor_Parent :: v_mouseInWideDataView (event, x_world, globalY_fraction);
	}
	if (event -> isDrop())
		clickedInWideRealTierArea = false;
	return result;
}

void structRealTierEditor :: v_play (double startTime, double endTime) {
	if (our d_sound.data)
		Sound_playPart (our d_sound.data, startTime, endTime, theFunctionEditor_playCallback, this);
}

void RealTierEditor_init (RealTierEditor me, autoRealTierArea realTierArea, conststring32 title, RealTier data, Sound sound, bool ownSound) {
	Melder_assert (data);
	Melder_assert (Thing_isa (data, classRealTier));
	TimeSoundEditor_init (me, title, data, sound, ownSound);
	my realTierArea = realTierArea.move();
	RealTierEditor_updateScaling (me);
	my realTierArea -> ycursor = 0.382 * my realTierArea -> ymin + 0.618 * my realTierArea -> ymax;
}


void RealTierEditor_init (RealTierEditor me, ClassInfo realTierAreaClass, conststring32 title, RealTier data, Sound sound, bool ownSound) {
	Melder_assert (data);
	Melder_assert (Thing_isa (data, classRealTier));
	TimeSoundEditor_init (me, title, data, sound, ownSound);
	my realTierArea = Thing_newFromClass (realTierAreaClass). static_cast_move <structRealTierArea>();
	FunctionArea_init (my realTierArea.get(), me, 0.0, sound ? 1.0 - my SOUND_HEIGHT : 1.0);
	RealTierEditor_updateScaling (me);
	my realTierArea -> ycursor = 0.382 * my realTierArea -> ymin + 0.618 * my realTierArea -> ymax;
}

/* End of file RealTierEditor.cpp */
