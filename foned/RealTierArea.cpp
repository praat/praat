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

void RealTierArea_updateScaling (RealTierArea me) {
	Melder_assert (me);
	Melder_assert (Thing_isa (me, classRealTierArea));
	Melder_assert (isdefined (my instancePref_dataFreeMinimum()));
	Melder_assert (isdefined (my instancePref_dataFreeMaximum()));
	my ymin = my instancePref_dataFreeMinimum();
	my ymax = my instancePref_dataFreeMaximum();
	Melder_assert (my realTier());
	if (my realTier() -> points.size > 0) {
		Melder_assert (! (my v_maximumLegalY() < my v_minimumLegalY()));   // NaN-safe
		const double minimumValue = Melder_clipped (my v_minimumLegalY(), RealTier_getMinimumValue (my realTier()), my v_maximumLegalY());
		const double maximumValue = Melder_clipped (my v_minimumLegalY(), RealTier_getMaximumValue (my realTier()), my v_maximumLegalY());
		Melder_clipRight (& my ymin, minimumValue);
		Melder_clipLeft (maximumValue, & my ymax);
	}
	if (my ycursor <= my ymin || my ycursor >= my ymax)
		my ycursor = 0.382 * my ymin + 0.618 * my ymax;
}

void RealTierArea_draw (RealTierArea me) {
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
}

void RealTierArea_drawWhileDragging (RealTierArea me) {
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

bool RealTierArea_mouse (RealTierArea me, GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	if (event -> isClick()) {
		my anchorIsInFreePart = false;
		my anchorIsNearPoint = false;
	}
	const double y_fraction_withinRealTierArea = my y_fraction_globalToLocal (y_fraction);
	const double y_world = (1.0 - y_fraction_withinRealTierArea) * my ymin + y_fraction_withinRealTierArea * my ymax;
	my viewRealTierAsWorldByWorld ();
	if (event -> isClick()) {
		if (isdefined (my anchorTime))
			return false;
		RealPoint clickedPoint = nullptr;
		const integer inearestPoint = AnyTier_timeToNearestIndexInTimeWindow (my realTier()->asAnyTier(), x_world, my startWindow(), my endWindow());
		if (inearestPoint != 0) {
			RealPoint nearestPoint = my realTier() -> points.at [inearestPoint];
			if (Graphics_distanceWCtoMM (my graphics(), x_world, y_world, nearestPoint -> number, nearestPoint -> value) < 1.5)
				clickedPoint = nearestPoint;
		}
		if (! clickedPoint) {
			my anchorIsInFreePart = true;
			my ycursor = y_world;
			return my defaultMouseInWideDataView (event, x_world, y_fraction) || true;
		}
		my anchorIsNearPoint = true;
		my draggingSelection = event -> shiftKeyPressed &&
				clickedPoint -> number >= my startSelection() && clickedPoint -> number <= my endSelection();
		if (my draggingSelection) {
			AnyTier_getWindowPoints (my realTier()->asAnyTier(), my startSelection(), my endSelection(), & my firstSelected, & my lastSelected);
			my save (U"Drag points");   // TODO: title can be more specific
		} else {
			my firstSelected = my lastSelected = inearestPoint;
			my save (U"Drag point");   // TODO: title can be more specific
		}
		my anchorTime = x_world;
		my anchorY = y_world;
		my dt = 0.0;
		my dy = 0.0;
		return FunctionEditor_UPDATE_NEEDED;
	} else if (event -> isDrag() || event -> isDrop()) {
		if (my anchorIsInFreePart) {
			my ycursor = y_world;
			return my defaultMouseInWideDataView (event, x_world, y_fraction) || true;
		}
		Melder_assert (my anchorIsNearPoint);
		my dt = x_world - my anchorTime;
		my dy = y_world - my anchorY;

		if (event -> isDrop()) {
			my anchorTime = undefined;
			const double leftNewTime = my realTier() -> points.at [my firstSelected] -> number + my dt;
			const double rightNewTime = my realTier() -> points.at [my lastSelected] -> number + my dt;
			const bool offLeft = ( leftNewTime < my tmin() );
			const bool offRight = ( rightNewTime > my tmax() );
			const bool draggedPastLeftNeighbour = ( my firstSelected > 1 && leftNewTime <= my realTier() -> points.at [my firstSelected - 1] -> number );
			const bool draggedPastRightNeighbour = ( my lastSelected < my realTier() -> points.size && rightNewTime >= my realTier() -> points.at [my lastSelected + 1] -> number );
			if (offLeft || offRight || draggedPastLeftNeighbour || draggedPastRightNeighbour) {
				Melder_beep ();
				return FunctionEditor_UPDATE_NEEDED;
			}

			for (integer i = my firstSelected; i <= my lastSelected; i ++) {
				RealPoint point = my realTier() -> points.at [i];
				point -> number += my dt;
				double pointY = point -> value;
				pointY += my dy;
				Melder_clip (my v_minimumLegalY(), & pointY, my v_maximumLegalY());
				point -> value = pointY;
			}

			/*
				Make sure that the same points are still selected (a problem with Undo...).
			*/

			if (my draggingSelection)
				my setSelection (my startSelection() + my dt, my endSelection() + my dt);
			if (my firstSelected == my lastSelected) {
				/*
					Move crosshair to only selected point.
				*/
				RealPoint point = my realTier() -> points.at [my firstSelected];
				my setSelection (point -> number, point -> number);
				my ycursor = point -> value;
			} else {
				/*
					Move crosshair to mouse location.
				*/
				my ycursor += my dy;
				Melder_clip (my v_minimumLegalY(), & my ycursor, my v_maximumLegalY());   // NaN-safe
			}

			my broadcastDataChanged ();
			//RealTierArea_updateScaling (me);
		}
	}
	return FunctionEditor_UPDATE_NEEDED;
}

/* End of file RealTierArea.cpp */
