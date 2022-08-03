/* RealTierArea.cpp
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

#include "RealTierArea.h"
#include "EditorM.h"

Thing_implement (RealTierArea, FunctionArea, 0);

#include "Prefs_define.h"
#include "RealTierArea_prefs.h"
#include "Prefs_install.h"
#include "RealTierArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "RealTierArea_prefs.h"

void RealTierArea_addPointAt (RealTierArea me, double time, double desiredY) {
	if (isdefined (my v_minimumLegalY()) && desiredY < my v_minimumLegalY())
		Melder_throw (U"Cannot add a point below ", my v_minimumLegalY(), my v_rightTickUnits(), U".");
	if (isdefined (my v_maximumLegalY()) && desiredY > my v_maximumLegalY())
		Melder_throw (U"Cannot add a point above ", my v_maximumLegalY(), my v_rightTickUnits(), U".");
	RealTier_addPoint (my realTier(), time, desiredY);
}

void RealTierArea_removePoints (RealTierArea me) {
	if (my startSelection() == my endSelection())
		AnyTier_removePointNear (my realTier()->asAnyTier(), my startSelection());
	else
		AnyTier_removePointsBetween (my realTier()->asAnyTier(), my startSelection(), my endSelection());
}

void RealTierArea_addPointAtCursor (RealTierArea me) {
	const double cursorTime = 0.5 * (my startSelection() + my endSelection());
	RealTierArea_addPointAt (me, cursorTime, my ycursor);
}

static void RealTierArea_drawWhileDragging (RealTierArea me) {
	Graphics_xorOn (my graphics(), Melder_MAROON);
	/*
		Draw all selected points as empty circles, if inside the window.
	*/
	for (integer ipoint = my firstSelected; ipoint <= my lastSelected; ipoint ++) {
		const RealPoint point = my realTier() -> points.at [ipoint];
		const double t = point -> number + my dt, y = point -> value + my dy;
		if (t >= my startWindow() && t <= my endWindow())
			Graphics_circle_mm (my graphics(), t, y, 3);
	}

	if (my lastSelected == my firstSelected) {
		/*
			Draw a crosshair with time and y.
		*/
		const RealPoint point = my realTier() -> points.at [my firstSelected];
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

void RealTierArea_draw (RealTierArea me) {
	Graphics_setWindow (my graphics(), my startWindow(), my endWindow(), my ymin, my ymax);
	Graphics_setColour (my graphics(), Melder_RED);
	Graphics_line (my graphics(), my startWindow(), my ycursor, my endWindow(), my ycursor);
	Graphics_setTextAlignment (my graphics(), Graphics_RIGHT, Graphics_HALF);
	Graphics_text (my graphics(), my startWindow(), my ycursor,
			Melder_float (Melder_half (my ycursor)), my v_rightTickUnits());
	Graphics_setColour (my graphics(), Melder_BLUE);
	Graphics_setTextAlignment (my graphics(), Graphics_LEFT, Graphics_HALF);
	Graphics_text (my graphics(), my endWindow(), my ymax,
			Melder_float (Melder_half (my ymax)), my v_rightTickUnits());
	Graphics_setTextAlignment (my graphics(), Graphics_LEFT, Graphics_HALF);
	Graphics_text (my graphics(), my endWindow(), my ymin,
			Melder_float (Melder_half (my ymin)), my v_rightTickUnits());

	const integer ifirstSelected = AnyTier_timeToHighIndex (my realTier()->asAnyTier(), my startSelection());
	const integer ilastSelected = AnyTier_timeToLowIndex (my realTier()->asAnyTier(), my endSelection());
	const integer imin = AnyTier_timeToHighIndex (my realTier()->asAnyTier(), my startWindow());
	const integer imax = AnyTier_timeToLowIndex (my realTier()->asAnyTier(), my endWindow());
	Graphics_setLineWidth (my graphics(), 2.0);
	if (my realTier() -> points.size == 0) {
		Graphics_setTextAlignment (my graphics(), Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics(), 0.5 * (my startWindow() + my endWindow()), 0.5 * (my ymin + my ymax),
				U"(no points)");
	} else if (imax < imin) {
		const double yleft = RealTier_getValueAtTime (my realTier(), my startWindow());
		const double yright = RealTier_getValueAtTime (my realTier(), my endWindow());
		Graphics_line (my graphics(), my startWindow(), yleft, my endWindow(), yright);
	} else {
		Graphics_setColour (my graphics(), Melder_BLUE);
		for (integer ipoint = imin; ipoint <= imax; ipoint ++) {
			RealPoint point = my realTier() -> points.at [ipoint];
			const double t = point -> number, y = point -> value;
			if (ipoint == 1)
				Graphics_line (my graphics(), my startWindow(), y, t, y);
			else if (ipoint == imin)
				Graphics_line (my graphics(), t, y, my startWindow(), RealTier_getValueAtTime (my realTier(), my startWindow()));
			if (ipoint == my realTier() -> points.size)
				Graphics_line (my graphics(), t, y, my endWindow(), y);
			else if (ipoint == imax)
				Graphics_line (my graphics(), t, y, my endWindow(), RealTier_getValueAtTime (my realTier(), my endWindow()));
			else {
				RealPoint pointRight = my realTier() -> points.at [ipoint + 1];
				Graphics_line (my graphics(), t, y, pointRight -> number, pointRight -> value);
			}
		}
		for (integer ipoint = imin; ipoint <= imax; ipoint ++) {
			RealPoint point = my realTier() -> points.at [ipoint];
			const double t = point -> number, y = point -> value;
			const bool pointIsSelected = ( ipoint >= ifirstSelected && ipoint <= ilastSelected );
			Graphics_setColour (my graphics(), pointIsSelected ? Melder_RED : Melder_BLUE);
			Graphics_fillCircle_mm (my graphics(), t, y, 3.0);
		}
	}
	Graphics_setLineWidth (my graphics(), 1.0);
	Graphics_setColour (my graphics(), Melder_BLACK);
	if (isdefined (my anchorTime))
		RealTierArea_drawWhileDragging (me);
}

void structRealTierArea :: v_drawInside () {
	RealTierArea_draw (this);
}

bool structRealTierArea :: v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double localY_fraction) {
	if (event -> isClick()) {
		our anchorIsInFreePart = false;
		our anchorIsNearPoint = false;
	}
	const double y_world = (1.0 - localY_fraction) * our ymin + localY_fraction * our ymax;
	our viewRealTierAsWorldByWorld ();
	if (event -> isClick()) {
		if (isdefined (our anchorTime))
			return false;
		RealPoint clickedPoint = nullptr;
		const integer inearestPoint = AnyTier_timeToNearestIndexInTimeWindow (our realTier()->asAnyTier(), x_world, our startWindow(), our endWindow());
		if (inearestPoint != 0) {
			RealPoint nearestPoint = our realTier() -> points.at [inearestPoint];
			if (Graphics_distanceWCtoMM (our graphics(), x_world, y_world, nearestPoint -> number, nearestPoint -> value) < 1.5)
				clickedPoint = nearestPoint;
		}
		if (! clickedPoint) {
			our anchorIsInFreePart = true;
			our ycursor = y_world;
			return FunctionEditor_defaultMouseInWideDataView (our functionEditor(), event, x_world) || true;
		}
		our anchorIsNearPoint = true;
		our draggingSelection = event -> shiftKeyPressed &&
				clickedPoint -> number >= our startSelection() && clickedPoint -> number <= our endSelection();
		if (our draggingSelection)
			AnyTier_getWindowPoints (our realTier()->asAnyTier(), our startSelection(), our endSelection(), & our firstSelected, & our lastSelected);
		else
			our firstSelected = our lastSelected = inearestPoint;
		our anchorTime = x_world;
		our anchorY = y_world;
		our dt = 0.0;
		our dy = 0.0;
		return FunctionEditor_UPDATE_NEEDED;
	} else if (event -> isDrag() || event -> isDrop()) {
		if (our anchorIsInFreePart) {
			our ycursor = y_world;
			return FunctionEditor_defaultMouseInWideDataView (our functionEditor(), event, x_world) || true;
		}
		Melder_assert (our anchorIsNearPoint);
		our dt = x_world - our anchorTime;
		our dy = y_world - our anchorY;

		if (event -> isDrop()) {
			our anchorTime = undefined;

			const bool somethingHasMoved = ( our dt != 0.0 || our dy != 0.0 );
			if (somethingHasMoved) {
				if (our draggingSelection)
					FunctionArea_save (this, U"Drag points");   // TODO: title can be more specific
				else
					FunctionArea_save (this, U"Drag point");   // TODO: title can be more specific
				const double leftNewTime = our realTier() -> points.at [our firstSelected] -> number + our dt;
				const double rightNewTime = our realTier() -> points.at [our lastSelected] -> number + our dt;
				const bool offLeft = ( leftNewTime < our tmin() );
				const bool offRight = ( rightNewTime > our tmax() );
				const bool draggedPastLeftNeighbour = ( our firstSelected > 1 && leftNewTime <= our realTier() -> points.at [our firstSelected - 1] -> number );
				const bool draggedPastRightNeighbour = ( our lastSelected < our realTier() -> points.size && rightNewTime >= our realTier() -> points.at [our lastSelected + 1] -> number );
				if (offLeft || offRight || draggedPastLeftNeighbour || draggedPastRightNeighbour) {
					Melder_beep ();
					return FunctionEditor_UPDATE_NEEDED;
				}

				for (integer i = our firstSelected; i <= our lastSelected; i ++) {
					RealPoint point = our realTier() -> points.at [i];
					point -> number += our dt;
					double pointY = point -> value;
					pointY += our dy;
					Melder_clip (our v_minimumLegalY(), & pointY, our v_maximumLegalY());
					point -> value = pointY;
				}

				/*
					Make sure that the same points are still selected (a problem with Undo...).
				*/
				if (our draggingSelection)
					our setSelection (our startSelection() + our dt, our endSelection() + our dt);
			}
			if (our firstSelected == our lastSelected) {
				/*
					Move crosshair to only selected point.
				*/
				RealPoint point = our realTier() -> points.at [our firstSelected];
				our setSelection (point -> number, point -> number);
				our ycursor = point -> value;
			} else {
				/*
					Move crosshair to mouse location.
				*/
				our ycursor += our dy;
				Melder_clip (our v_minimumLegalY(), & our ycursor, our v_maximumLegalY());   // NaN-safe
			}
			FunctionArea_broadcastDataChanged (this);
		}
	}
	return FunctionEditor_UPDATE_NEEDED;
}

bool RealTierArea_mouse (RealTierArea me, GuiDrawingArea_MouseEvent event, double x_world, double globalY_fraction) {
	const double localY_fraction = my y_fraction_globalToLocal (globalY_fraction);
	return my v_mouse (event, x_world, localY_fraction);
}

static void menu_cb_removePoints (RealTierArea me, EDITOR_ARGS_DIRECT) {
	FunctionArea_save (me, U"Remove point(s)");
	RealTierArea_removePoints (me);
	FunctionArea_broadcastDataChanged (me);
}

static void menu_cb_addPointAtCursor (RealTierArea me, EDITOR_ARGS_DIRECT) {
	FunctionArea_save (me, U"Add point");
	RealTierArea_addPointAtCursor (me);
	FunctionArea_broadcastDataChanged (me);
}

static void menu_cb_addPointAt (RealTierArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add point", nullptr)
		REAL (time, U"Time (s)", U"0.0")
		REAL (desiredY, my v_quantityText (), U"0.0")
	EDITOR_OK
		SET_REAL (time, 0.5 * (my startSelection() + my endSelection()))
		SET_REAL (desiredY, my ycursor)
	EDITOR_DO
		FunctionArea_save (me, U"Add point");
		RealTierArea_addPointAt (me, time, desiredY);
		FunctionArea_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_setRange (RealTierArea me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (my v_setRangeTitle (), nullptr)
		REAL (ymin, my v_minimumLabelText (), my default_dataFreeMinimum())
		REAL (ymax, my v_maximumLabelText (), my default_dataFreeMaximum())
	EDITOR_OK
		SET_REAL (ymin, my instancePref_dataFreeMinimum())
		SET_REAL (ymax, my instancePref_dataFreeMaximum())
	EDITOR_DO
		my setInstancePref_dataFreeMinimum (ymin);
		my setInstancePref_dataFreeMaximum (ymax);
		my v_updateScaling ();   // BUG: should be viewChanged()
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

void structRealTierArea :: v0_createMenuItems_view_vertical (EditorMenu menu) {
	FunctionAreaMenu_addCommand (menu, U"-- view/realtier --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, our v_setRangeTitle (), 0,
			menu_cb_setRange, this);
}

void structRealTierArea :: v_createMenus () {
	RealTierArea_Parent :: v_createMenus ();
	EditorMenu menu = Editor_addMenu (our functionEditor(), U"Point", 0);
	FunctionAreaMenu_addCommand (menu, U"Add point at cursor", 'T',
			menu_cb_addPointAtCursor, this);
	FunctionAreaMenu_addCommand (menu, U"Add point at...", 0,
			menu_cb_addPointAt, this);
	FunctionAreaMenu_addCommand (menu, U"-- remove point --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Remove point(s)", GuiMenu_OPTION | 'T',
			menu_cb_removePoints, this);
}

/* End of file RealTierArea.cpp */
