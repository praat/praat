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

#define SOUND_HEIGHT  0.382

/********** MENU COMMANDS **********/

static void menu_cb_removePoints (RealTierEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Remove point(s)");
	RealTier tier = (RealTier) my data;
	if (my startSelection == my endSelection)
		AnyTier_removePointNear (tier->asAnyTier(), my startSelection);
	else
		AnyTier_removePointsBetween (tier->asAnyTier(), my startSelection, my endSelection);
	RealTierEditor_updateScaling (me);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPointAtCursor (RealTierEditor me, EDITOR_ARGS_DIRECT) {
	if (isdefined (my v_minimumLegalValue ()) && my ycursor < my v_minimumLegalValue ())
		Melder_throw (U"Cannot add a point below ", my v_minimumLegalValue (), my v_rightTickUnits (), U".");
	if (isdefined (my v_maximumLegalValue ()) && my ycursor > my v_maximumLegalValue ())
		Melder_throw (U"Cannot add a point above ", my v_maximumLegalValue (), my v_rightTickUnits (), U".");
	Editor_save (me, U"Add point");
	RealTier_addPoint ((RealTier) my data, 0.5 * (my startSelection + my endSelection), my ycursor);
	RealTierEditor_updateScaling (me);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPointAt (RealTierEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add point", nullptr)
		REAL (time, U"Time (s)", U"0.0")
		REAL (desiredValue, my v_quantityText (), U"0.0")
	EDITOR_OK
		SET_REAL (time, 0.5 * (my startSelection + my endSelection))
		SET_REAL (desiredValue, my ycursor)
	EDITOR_DO
		if (isdefined (my v_minimumLegalValue ()) && desiredValue < my v_minimumLegalValue ())
			Melder_throw (U"Cannot add a point below ", my v_minimumLegalValue (), my v_rightTickUnits (), U".");
		if (isdefined (my v_maximumLegalValue ()) && desiredValue > my v_maximumLegalValue ())
			Melder_throw (U"Cannot add a point above ", my v_maximumLegalValue (), my v_rightTickUnits (), U".");
		Editor_save (me, U"Add point");
		RealTier_addPoint ((RealTier) my data, time, desiredValue);
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
		SET_REAL (ymin, my ymin)
		SET_REAL (ymax, my ymax)
	EDITOR_DO
		my ymin = ymin;
		my ymax = ymax;
		if (my ymax <= my ymin) RealTierEditor_updateScaling (me);
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
	RealTier data = (RealTier) my data;
	if (data -> points.size == 0) {
		my ymin = my v_defaultYmin ();
		my ymax = my v_defaultYmax ();
	} else {
		double ymin = RealTier_getMinimumValue (data);
		double ymax = RealTier_getMaximumValue (data);
		double range = ymax - ymin;
		if (range == 0.0) {
			ymin -= 1.0;
			ymax += 1.0;
		} else {
			ymin -= 0.2 * range;
			ymax += 0.2 * range;
		}
		if (isdefined (my v_minimumLegalValue()) && ymin < my v_minimumLegalValue ())
			ymin = my v_minimumLegalValue ();
		if (isdefined (my v_maximumLegalValue ()) && ymin > my v_maximumLegalValue ())
			ymin = my v_maximumLegalValue ();
		if (isdefined (my v_minimumLegalValue ()) && ymax < my v_minimumLegalValue ())
			ymax = my v_minimumLegalValue ();
		if (isdefined (my v_maximumLegalValue ()) && ymax > my v_maximumLegalValue ())
			ymax = my v_maximumLegalValue ();
		if (ymin >= ymax) {
			if (isdefined (my v_minimumLegalValue ()) && isdefined (my v_maximumLegalValue ())) {
				ymin = my v_minimumLegalValue ();
				ymax = my v_maximumLegalValue ();
			} else if (isdefined (my v_minimumLegalValue ())) {
				ymin = my v_minimumLegalValue ();
				ymax = ymin + 1.0;
			} else {
				Melder_assert (isdefined (my v_maximumLegalValue ()));
				ymax = my v_maximumLegalValue ();
				ymin = ymax - 1.0;
			}
		}
		if (ymin < my ymin || my ymin < 0.0)
			my ymin = ymin;
		if (ymax > my ymax)
			my ymax = ymax;
		if (my ycursor <= my ymin || my ycursor >= my ymax)
			my ycursor = 0.382 * my ymin + 0.618 * my ymax;
	}
}

void structRealTierEditor :: v_dataChanged () {
	RealTierEditor_updateScaling (this);
	RealTierEditor_Parent :: v_dataChanged ();
}

/********** DRAWING AREA **********/

static void FunctionEditor_drawRealTier (FunctionEditor me, RealTier tier,
	double ymin, double ycursor, double ymax, conststring32 rightTickUnits
) {
	Graphics_setWindow (my graphics.get(), my startWindow, my endWindow, ymin, ymax);
	Graphics_setColour (my graphics.get(), Melder_RED);
	Graphics_line (my graphics.get(), my startWindow, ycursor, my endWindow, ycursor);
	Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_HALF);
	Graphics_text (my graphics.get(), my startWindow, ycursor,
			Melder_float (Melder_half (ycursor)));
	Graphics_setColour (my graphics.get(), Melder_BLUE);
	Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_TOP);
	Graphics_text (my graphics.get(), my endWindow, ymax,
			Melder_float (Melder_half (ymax)), rightTickUnits);
	Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_HALF);
	Graphics_text (my graphics.get(), my endWindow, ymin,
			Melder_float (Melder_half (ymin)), rightTickUnits);
	const integer ifirstSelected = AnyTier_timeToHighIndex (tier->asAnyTier(), my startSelection);
	const integer ilastSelected = AnyTier_timeToLowIndex (tier->asAnyTier(), my endSelection);
	const integer imin = AnyTier_timeToHighIndex (tier->asAnyTier(), my startWindow);
	const integer imax = AnyTier_timeToLowIndex (tier->asAnyTier(), my endWindow);
	Graphics_setLineWidth (my graphics.get(), 2.0);
	if (tier -> points.size == 0) {
		Graphics_setTextAlignment (my graphics.get(), Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics.get(), 0.5 * (my startWindow + my endWindow), 0.5 * (ymin + ymax),
				U"(no points)");
	} else if (imax < imin) {
		const double yleft = RealTier_getValueAtTime (tier, my startWindow);
		const double yright = RealTier_getValueAtTime (tier, my endWindow);
		Graphics_line (my graphics.get(), my startWindow, yleft, my endWindow, yright);
	} else {
		for (integer ipoint = imin; ipoint <= imax; ipoint ++) {
			RealPoint point = tier -> points.at [ipoint];
			double t = point -> number, y = point -> value;
			if (ipoint >= ifirstSelected && ipoint <= ilastSelected)
				Graphics_setColour (my graphics.get(), Melder_RED);
			Graphics_fillCircle_mm (my graphics.get(), t, y, 3.0);
			Graphics_setColour (my graphics.get(), Melder_BLUE);
			if (ipoint == 1)
				Graphics_line (my graphics.get(), my startWindow, y, t, y);
			else if (ipoint == imin)
				Graphics_line (my graphics.get(), t, y, my startWindow, RealTier_getValueAtTime (tier, my startWindow));
			if (ipoint == tier -> points.size)
				Graphics_line (my graphics.get(), t, y, my endWindow, y);
			else if (ipoint == imax)
				Graphics_line (my graphics.get(), t, y, my endWindow, RealTier_getValueAtTime (tier, my endWindow));
			else {
				RealPoint pointRight = tier -> points.at [ipoint + 1];
				Graphics_line (my graphics.get(), t, y, pointRight -> number, pointRight -> value);
			}
		}
	}
	Graphics_setLineWidth (my graphics.get(), 1.0);
	Graphics_setColour (my graphics.get(), Melder_BLACK);
}

static void FunctionEditor_drawRealTierWhileDragging (FunctionEditor me, RealTier tier,
	double ymin, double ymax, double dt, double dy
) {
	Graphics_xorOn (my graphics.get(), Melder_MAROON);
	/*
		Draw all selected points as empty circles, if inside the window.
	*/
	const integer firstSelectedPointNumber = AnyTier_timeToHighIndex (tier->asAnyTier(), my startSelection);
	const integer lastSelectedPointNumber = AnyTier_timeToLowIndex (tier->asAnyTier(), my endSelection);
	for (integer ipoint = firstSelectedPointNumber; ipoint <= lastSelectedPointNumber; ipoint ++) {
		RealPoint point = tier -> points.at [ipoint];
		double t = point -> number + dt, y = point -> value + dy;
		if (t >= my startWindow && t <= my endWindow)
			Graphics_circle_mm (my graphics.get(), t, y, 3);
	}

	if (lastSelectedPointNumber == firstSelectedPointNumber) {
		/*
			Draw a crosshair with time and y.
		*/
		RealPoint point = tier -> points.at [firstSelectedPointNumber];
		double t = point -> number + dt, y = point -> value + dy;
		Graphics_line (my graphics.get(), t, ymin, t, ymax - Graphics_dyMMtoWC (my graphics.get(), 4.0));
		Graphics_setTextAlignment (my graphics.get(), kGraphics_horizontalAlignment::CENTRE, Graphics_TOP);
		Graphics_text (my graphics.get(), t, ymax, Melder_fixed (t, 6));
		Graphics_line (my graphics.get(), my startWindow, y, my endWindow, y);
		Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_BOTTOM);
		Graphics_text (my graphics.get(), my startWindow, y, Melder_fixed (y, 6));
	}
	Graphics_xorOff (my graphics.get());
}

void structRealTierEditor :: v_draw () {
	RealTier tier = (RealTier) our data;
	Graphics_Viewport viewport;
	if (our d_sound.data) {
		viewport = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 1.0 - SOUND_HEIGHT, 1.0);
		Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setColour (our graphics.get(), Melder_WHITE);
		Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
		TimeSoundEditor_drawSound (this, -1.0, 1.0);
		Graphics_resetViewport (our graphics.get(), viewport);
		Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 0.0, 1.0 - SOUND_HEIGHT);
	}
	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	FunctionEditor_drawRealTier (this, tier, our ymin, our ycursor, our ymax, v_rightTickUnits());
	if (isdefined (our anchorTime))
		FunctionEditor_drawRealTierWhileDragging (this, tier, our ymin, our ymax, our dt, our dy);
	our v_updateMenuItems_file ();   // TODO: this is not about drawing; improve logic? 2020-07-23
}

bool structRealTierEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	RealTier pitch = (RealTier) our data;
	Graphics_Viewport viewport;
	if (event -> isClick()) {
		Melder_assert (isundef (our anchorTime));

		/*
			Perform the default action: move the cursor.
		*/
		//our startSelection = our endSelection = x_world;
		if (our d_sound.data) {
			if (y_fraction < 1.0 - SOUND_HEIGHT) {   // clicked in tier area?
				y_fraction /= 1.0 - SOUND_HEIGHT;
				our ycursor = (1.0 - y_fraction) * our ymin + y_fraction * our ymax;
				viewport = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 0.0, 1.0 - SOUND_HEIGHT);
			} else {
				return our RealTierEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);
			}
		} else {
			our ycursor = (1.0 - y_fraction) * our ymin + y_fraction * our ymax;
		}
		Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, our ymin, our ymax);
		const double y_world = our ycursor;

		/*
			Clicked on a point?
		*/
		integer inearestPoint = AnyTier_timeToNearestIndex (pitch->asAnyTier(), x_world);
		if (inearestPoint == 0)
			return our RealTierEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);
		RealPoint nearestPoint = pitch -> points.at [inearestPoint];
		if (Graphics_distanceWCtoMM (our graphics.get(), x_world, y_world, nearestPoint -> number, nearestPoint -> value) > 1.5) {
			if (our d_sound.data)
				Graphics_resetViewport (our graphics.get(), viewport);
			return our RealTierEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);
		}

		/*
			Clicked on a selected point?
		*/
		our draggingSelection = event -> shiftKeyPressed &&
			nearestPoint -> number > our startSelection && nearestPoint -> number < our endSelection;
		if (our draggingSelection) {
			our firstSelected = AnyTier_timeToHighIndex (pitch->asAnyTier(), our startSelection);
			our lastSelected = AnyTier_timeToLowIndex (pitch->asAnyTier(), our endSelection);
			Editor_save (this, U"Drag points");
		} else {
			our firstSelected = our lastSelected = inearestPoint;
			Editor_save (this, U"Drag point");
		}
		our anchorTime = x_world;
		our anchorY = y_world;
		our dt = 0.0;
		our dy = 0.0;
		return FunctionEditor_UPDATE_NEEDED;
	} else if (event -> isDrag() || event -> isDrop()) {
		if (isundef (our anchorTime))
			return our RealTierEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);
		if (our d_sound.data) {
			y_fraction /= 1.0 - SOUND_HEIGHT;
			our ycursor = (1.0 - y_fraction) * our ymin + y_fraction * our ymax;
			viewport = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 0.0, 1.0 - SOUND_HEIGHT);
		} else {
			our ycursor = (1.0 - y_fraction) * our ymin + y_fraction * our ymax;
		}
		Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, our ymin, our ymax);
		const double y_world = our ycursor;
		our dt = x_world - our anchorTime;
		our dy = y_world - our anchorY;

		/*
			Dragged inside window?
		*/
		if (x_world < our startWindow || x_world > our endWindow)
			return FunctionEditor_UPDATE_NEEDED;

		/*
			Points not dragged past neighbours?
		*/
		{
			double newTime = pitch -> points.at [our firstSelected] -> number + dt;
			if (newTime < our tmin)
				return FunctionEditor_UPDATE_NEEDED;   // outside domain
			if (our firstSelected > 1 && newTime <= pitch -> points.at [our firstSelected - 1] -> number)
				return FunctionEditor_UPDATE_NEEDED;   // past left neighbour
			newTime = pitch -> points.at [our lastSelected] -> number + dt;
			if (newTime > our tmax)
				return FunctionEditor_UPDATE_NEEDED;   // outside domain
			if (our lastSelected < pitch -> points.size && newTime >= pitch -> points.at [our lastSelected + 1] -> number)
				return FunctionEditor_UPDATE_NEEDED;   // past right neighbour
		}

		if (event -> isDrop()) {
			our anchorTime = undefined;

			for (integer i = our firstSelected; i <= our lastSelected; i ++) {
				RealPoint point = pitch -> points.at [i];
				point -> number += dt;
				point -> value += dy;
				if (isdefined (v_minimumLegalValue ()) && point -> value < v_minimumLegalValue ())
					point -> value = v_minimumLegalValue ();
				if (isdefined (v_maximumLegalValue ()) && point -> value > v_maximumLegalValue ())
					point -> value = v_maximumLegalValue ();
			}

			/*
				Make sure that the same points are still selected (a problem with Undo...).
			*/

			if (draggingSelection) {
				our startSelection += dt;
				our endSelection += dt;
			}
			if (our firstSelected == our lastSelected) {
				/*
					Move crosshair to only selected pitch point.
				*/
				RealPoint point = pitch -> points.at [our firstSelected];
				our startSelection = our endSelection = point -> number;
				our ycursor = point -> value;
			} else {
				/*
					Move crosshair to mouse location.
				*/
				/*our cursor += dt;*/
				our ycursor += dy;
				if (isdefined (v_minimumLegalValue ()) && our ycursor < v_minimumLegalValue ())
					our ycursor = v_minimumLegalValue ();
				if (isdefined (v_maximumLegalValue ()) && our ycursor > v_maximumLegalValue ())
					our ycursor = v_maximumLegalValue ();
			}

			Editor_broadcastDataChanged (this);
			RealTierEditor_updateScaling (this);
		}
	}
	return FunctionEditor_UPDATE_NEEDED;
}

void structRealTierEditor :: v_play (double a_tmin, double a_tmax) {
	if (our d_sound.data)
		Sound_playPart (our d_sound.data, a_tmin, a_tmax, theFunctionEditor_playCallback, this);
}

void RealTierEditor_init (RealTierEditor me, conststring32 title, RealTier data, Sound sound, bool ownSound) {
	Melder_assert (data);
	Melder_assert (Thing_isa (data, classRealTier));
	TimeSoundEditor_init (me, title, data, sound, ownSound);
	my ymin = -1.0;
	RealTierEditor_updateScaling (me);
	my ycursor = 0.382 * my ymin + 0.618 * my ymax;
}

/* End of file RealTierEditor.cpp */
