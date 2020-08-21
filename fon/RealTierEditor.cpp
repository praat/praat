/** RealTierEditor.cpp
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

/* MARK: - FUNCTIONVIEW */

Thing_implement (FunctionView, Thing, 0);

/* MARK: - REALTIERVIEW */

Thing_implement (RealTierView, FunctionView, 0);

static void RealTierView_addPointAt (RealTierView me, RealTier tier, double time, double desiredValue) {
	if (isdefined (my v_minimumLegalValue ()) && desiredValue < my v_minimumLegalValue ())
		Melder_throw (U"Cannot add a point below ", my v_minimumLegalValue (), my v_rightTickUnits (), U".");
	if (isdefined (my v_maximumLegalValue ()) && desiredValue > my v_maximumLegalValue ())
		Melder_throw (U"Cannot add a point above ", my v_maximumLegalValue (), my v_rightTickUnits (), U".");
	RealTier_addPoint (tier, time, desiredValue);
}

static void RealTierView_removePoints (RealTierView me, RealTier tier) {
	if (my startSelection() == my endSelection())
		AnyTier_removePointNear (tier->asAnyTier(), my startSelection());
	else
		AnyTier_removePointsBetween (tier->asAnyTier(), my startSelection(), my endSelection());
}

static void RealTierView_addPointAtCursor (RealTierView me, RealTier tier) {
	const double cursorTime = 0.5 * (my startSelection() + my endSelection());
	RealTierView_addPointAt (me, tier, cursorTime, my ycursor);
}

static void RealTierView_updateScaling (RealTierView me, RealTier tier) {
	if (tier -> points.size == 0) {
		my ymin = my v_defaultYmin ();
		my ymax = my v_defaultYmax ();
	} else {
		double ymin = RealTier_getMinimumValue (tier);
		double ymax = RealTier_getMaximumValue (tier);
		const double range = ymax - ymin;
		if (range == 0.0) {
			ymin -= 1.0;
			ymax += 1.0;
		} else {
			ymin -= 0.2 * range;
			ymax += 0.2 * range;
		}
		Melder_clip (my v_minimumLegalValue(), & ymin, my v_maximumLegalValue());
		Melder_clip (my v_minimumLegalValue(), & ymax, my v_maximumLegalValue());
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
		if (ymin < my ymin || my ymin < 0.0)   // what?
			my ymin = ymin;
		if (ymax > my ymax)
			my ymax = ymax;
		if (my ycursor <= my ymin || my ycursor >= my ymax)
			my ycursor = 0.382 * my ymin + 0.618 * my ymax;
	}
}

static void RealTierView_draw (RealTierView me, RealTier tier) {
	Graphics_setColour (my graphics(), Melder_RED);
	Graphics_line (my graphics(), my startWindow(), my ycursor, my endWindow(), my ycursor);
	Graphics_setTextAlignment (my graphics(), Graphics_RIGHT, Graphics_HALF);
	Graphics_text (my graphics(), my startWindow(), my ycursor,
			Melder_float (Melder_half (my ycursor)));
	Graphics_setColour (my graphics(), Melder_BLUE);
	Graphics_setTextAlignment (my graphics(), Graphics_LEFT, Graphics_TOP);
	Graphics_text (my graphics(), my endWindow(), my ymax,
			Melder_float (Melder_half (my ymax)), my v_rightTickUnits());
	Graphics_setTextAlignment (my graphics(), Graphics_LEFT, Graphics_HALF);
	Graphics_text (my graphics(), my endWindow(), my ymin,
			Melder_float (Melder_half (my ymin)), my v_rightTickUnits());
	const integer ifirstSelected = AnyTier_timeToHighIndex (tier->asAnyTier(), my startSelection());
	const integer ilastSelected = AnyTier_timeToLowIndex (tier->asAnyTier(), my endSelection());
	const integer imin = AnyTier_timeToHighIndex (tier->asAnyTier(), my startWindow());
	const integer imax = AnyTier_timeToLowIndex (tier->asAnyTier(), my endWindow());
	Graphics_setLineWidth (my graphics(), 2.0);
	if (tier -> points.size == 0) {
		Graphics_setTextAlignment (my graphics(), Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics(), 0.5 * (my startWindow() + my endWindow()), 0.5 * (my ymin + my ymax),
				U"(no points)");
	} else if (imax < imin) {
		const double yleft = RealTier_getValueAtTime (tier, my startWindow());
		const double yright = RealTier_getValueAtTime (tier, my endWindow());
		Graphics_line (my graphics(), my startWindow(), yleft, my endWindow(), yright);
	} else {
		for (integer ipoint = imin; ipoint <= imax; ipoint ++) {
			RealPoint point = tier -> points.at [ipoint];
			double t = point -> number, y = point -> value;
			if (ipoint >= ifirstSelected && ipoint <= ilastSelected)
				Graphics_setColour (my graphics(), Melder_RED);
			Graphics_fillCircle_mm (my graphics(), t, y, 3.0);
			Graphics_setColour (my graphics(), Melder_BLUE);
			if (ipoint == 1)
				Graphics_line (my graphics(), my startWindow(), y, t, y);
			else if (ipoint == imin)
				Graphics_line (my graphics(), t, y, my startWindow(), RealTier_getValueAtTime (tier, my startWindow()));
			if (ipoint == tier -> points.size)
				Graphics_line (my graphics(), t, y, my endWindow(), y);
			else if (ipoint == imax)
				Graphics_line (my graphics(), t, y, my endWindow(), RealTier_getValueAtTime (tier, my endWindow()));
			else {
				RealPoint pointRight = tier -> points.at [ipoint + 1];
				Graphics_line (my graphics(), t, y, pointRight -> number, pointRight -> value);
			}
		}
	}
	Graphics_setLineWidth (my graphics(), 1.0);
	Graphics_setColour (my graphics(), Melder_BLACK);
}

static void RealTierView_drawWhileDragging (RealTierView me, RealTier tier) {
	Graphics_xorOn (my graphics(), Melder_MAROON);
	/*
		Draw all selected points as empty circles, if inside the window.
	*/
	for (integer ipoint = my firstSelected; ipoint <= my lastSelected; ipoint ++) {
		const RealPoint point = tier -> points.at [ipoint];
		const double t = point -> number + my dt, y = point -> value + my dy;
		if (t >= my startWindow() && t <= my endWindow())
			Graphics_circle_mm (my graphics(), t, y, 3);
	}

	if (my lastSelected == my firstSelected) {
		/*
			Draw a crosshair with time and y.
		*/
		const RealPoint point = tier -> points.at [my firstSelected];
		const double t = point -> number + my dt, y = point -> value + my dy;
		Graphics_line (my graphics(), t, my ymin, t, my ymax - Graphics_dyMMtoWC (my graphics(), 4.0));
		Graphics_setTextAlignment (my graphics(), kGraphics_horizontalAlignment::CENTRE, Graphics_TOP);
		Graphics_text (my graphics(), t, my ymax, Melder_fixed (t, 6));
		Graphics_line (my graphics(), my startWindow(), y, my endWindow(), y);
		Graphics_setTextAlignment (my graphics(), Graphics_LEFT, Graphics_BOTTOM);
		Graphics_text (my graphics(), my startWindow(), y, Melder_fixed (y, 6));
	}
	Graphics_xorOff (my graphics());
}

/* MARK: - REALTIEREDITOR */

Thing_implement (RealTierEditor, TimeSoundEditor, 0);

/* MARK: - MENU COMMANDS */

static void menu_cb_removePoints (RealTierEditor me, EDITOR_ARGS_DIRECT) {
	RealTierView_removePoints (my view.get(), static_cast <RealTier> (my data));
	Editor_save (me, U"Remove point(s)");
	RealTierEditor_updateScaling (me);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPointAtCursor (RealTierEditor me, EDITOR_ARGS_DIRECT) {
	RealTierView_addPointAtCursor (my view.get(), static_cast <RealTier> (my data));
	Editor_save (me, U"Add point");
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
		SET_REAL (desiredValue, my view -> ycursor)
	EDITOR_DO
		RealTierView_addPointAt (my view.get(), static_cast <RealTier> (my data), time, desiredValue);
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
		SET_REAL (ymin, my view -> ymin)
		SET_REAL (ymax, my view -> ymax)
	EDITOR_DO
		my view -> ymin = ymin;
		my view -> ymax = ymax;
		if (my view -> ymax <= my view -> ymin)
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
	RealTierView_updateScaling (my view.get(), (RealTier) my data);
}

void structRealTierEditor :: v_dataChanged () {
	RealTierEditor_updateScaling (this);
	RealTierEditor_Parent :: v_dataChanged ();
}

/* MARK: - DRAWING AREA */

void structRealTierEditor :: v_draw () {
	RealTier tier = (RealTier) our data;
	if (our d_sound.data) {
		Graphics_Viewport viewport = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 1.0 - SOUND_HEIGHT, 1.0);
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
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, our view -> ymin, our view -> ymax);
	RealTierView_draw (our view.get(), tier);
	if (isdefined (our view -> anchorTime))
		RealTierView_drawWhileDragging (our view.get(), tier);
	our v_updateMenuItems_file ();   // TODO: this is not about drawing; improve logic? 2020-07-23
}

bool structRealTierEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	RealTier tier = (RealTier) our data;
	static bool anchorIsInSoundPart, anchorIsInRealTierPart, anchorIsInFreePart, anchorIsNearPoint;
	if (event -> isClick()) {
		anchorIsInSoundPart = ( y_fraction >= our view -> ymax_fraction );
		anchorIsInRealTierPart = false;
		anchorIsInFreePart = false;
		anchorIsNearPoint = false;
	}
	if (anchorIsInSoundPart)
		return our RealTierEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);
	anchorIsInRealTierPart = true;
	const double y_fraction_withinRealTierArea = y_fraction / our view -> ymax_fraction;
	const double y_world = (1.0 - y_fraction_withinRealTierArea) * our view -> ymin + y_fraction_withinRealTierArea * our view -> ymax;
	our view -> viewRealTierAsWorldByWorld ();
	if (event -> isClick()) {
		Melder_assert (isundef (our view -> anchorTime));
		RealPoint clickedPoint = nullptr;
		integer inearestPoint = AnyTier_timeToNearestIndexInTimeWindow (tier->asAnyTier(), x_world, our startWindow, our endWindow);
		if (inearestPoint != 0) {
			RealPoint nearestPoint = tier -> points.at [inearestPoint];
			if (Graphics_distanceWCtoMM (our graphics.get(), x_world, y_world, nearestPoint -> number, nearestPoint -> value) < 1.5)
				clickedPoint = nearestPoint;
		}
		if (! clickedPoint) {
			anchorIsInFreePart = true;
			our view -> ycursor = y_world;
			our viewDataAsWorldByFraction ();
			return our RealTierEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);
		}
		anchorIsNearPoint = true;
		our view -> draggingSelection = event -> shiftKeyPressed &&
			clickedPoint -> number >= our startSelection && clickedPoint -> number <= our endSelection;
		if (our view -> draggingSelection) {
			AnyTier_getWindowPoints (tier->asAnyTier(), our startSelection, our endSelection, & our view -> firstSelected, & our view -> lastSelected);
			Editor_save (this, U"Drag points");
		} else {
			our view -> firstSelected = our view -> lastSelected = inearestPoint;
			Editor_save (this, U"Drag point");
		}
		our view -> anchorTime = x_world;
		our view -> anchorY = y_world;
		our view -> dt = 0.0;
		our view -> dy = 0.0;
		return FunctionEditor_UPDATE_NEEDED;
	} else if (event -> isDrag() || event -> isDrop()) {
		Melder_assert (anchorIsInRealTierPart);
		if (anchorIsInFreePart) {
			our view -> ycursor = y_world;
			our viewDataAsWorldByFraction ();
			return our RealTierEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);
		}
		Melder_assert (anchorIsNearPoint);
		our view -> dt = x_world - our view -> anchorTime;
		our view -> dy = y_world - our view -> anchorY;

		if (event -> isDrop()) {
			our view -> anchorTime = undefined;
			const double leftNewTime = tier -> points.at [our view -> firstSelected] -> number + our view -> dt;
			const double rightNewTime = tier -> points.at [our view -> lastSelected] -> number + our view -> dt;
			const bool offLeft = ( leftNewTime < our tmin );
			const bool offRight = ( rightNewTime > our tmax );
			const bool draggedPastLeftNeighbour = ( our view -> firstSelected > 1 && leftNewTime <= tier -> points.at [our view -> firstSelected - 1] -> number );
			const bool draggedPastRightNeighbour = ( our view -> lastSelected < tier -> points.size && rightNewTime >= tier -> points.at [our view -> lastSelected + 1] -> number );
			if (offLeft || offRight || draggedPastLeftNeighbour || draggedPastRightNeighbour) {
				Melder_beep ();
				return FunctionEditor_UPDATE_NEEDED;
			}

			for (integer i = our view -> firstSelected; i <= our view -> lastSelected; i ++) {
				RealPoint point = tier -> points.at [i];
				point -> number += our view -> dt;
				point -> value += our view -> dy;
				Melder_clipLeft (our view -> v_minimumLegalValue (), & point -> value);
				Melder_clipRight (& point -> value, our view -> v_maximumLegalValue ());
			}

			/*
				Make sure that the same points are still selected (a problem with Undo...).
			*/

			if (our view -> draggingSelection) {
				our startSelection += our view -> dt;
				our endSelection += our view -> dt;
			}
			if (our view -> firstSelected == our view -> lastSelected) {
				/*
					Move crosshair to only selected point.
				*/
				RealPoint point = tier -> points.at [our view -> firstSelected];
				our startSelection = our endSelection = point -> number;
				our view -> ycursor = point -> value;
			} else {
				/*
					Move crosshair to mouse location.
				*/
				our view -> ycursor += our view -> dy;
				Melder_clipLeft (our view -> v_minimumLegalValue (), & our view -> ycursor);   // NaN-safe
				Melder_clipRight (& our view -> ycursor, our view -> v_maximumLegalValue ());   // NaN-safe
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

void RealTierEditor_init (RealTierEditor me, ClassInfo viewClass, conststring32 title, RealTier data, Sound sound, bool ownSound) {
	Melder_assert (data);
	Melder_assert (Thing_isa (data, classRealTier));
	TimeSoundEditor_init (me, title, data, sound, ownSound);
	my view = Thing_newFromClass (viewClass). static_cast_move <structRealTierView>();
	FunctionView_init (my view.get(), me, 0.0, sound ? 1.0 - my SOUND_HEIGHT : 1.0);
	my view -> ymin = -1.0;   // what?
	RealTierEditor_updateScaling (me);
	my view -> ycursor = 0.382 * my view -> ymin + 0.618 * my view -> ymax;
}

/* End of file RealTierEditor.cpp */
