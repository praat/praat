/* RealTierArea.cpp
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

#include "RealTierArea.h"
#include "RealTierEditor.h"

Thing_implement (RealTierArea, FunctionArea, 0);

void RealTierArea_addPointAt (RealTierArea me, RealTier tier, double time, double desiredY) {
	if (isdefined (my v_minimumLegalY ()) && desiredY < my v_minimumLegalY ())
		Melder_throw (U"Cannot add a point below ", my v_minimumLegalY (), my v_rightTickUnits (), U".");
	if (isdefined (my v_maximumLegalY ()) && desiredY > my v_maximumLegalY ())
		Melder_throw (U"Cannot add a point above ", my v_maximumLegalY (), my v_rightTickUnits (), U".");
	RealTier_addPoint (tier, time, my v_yToValue (desiredY));
}

void RealTierArea_removePoints (RealTierArea me, RealTier tier) {
	if (my startSelection() == my endSelection())
		AnyTier_removePointNear (tier->asAnyTier(), my startSelection());
	else
		AnyTier_removePointsBetween (tier->asAnyTier(), my startSelection(), my endSelection());
}

void RealTierArea_addPointAtCursor (RealTierArea me, RealTier tier) {
	const double cursorTime = 0.5 * (my startSelection() + my endSelection());
	RealTierArea_addPointAt (me, tier, cursorTime, my v_yToValue (my ycursor));
}

void RealTierArea_updateScaling (RealTierArea me, RealTier tier) {
	if (tier -> points.size == 0) {
		my ymin = my v_defaultYmin ();
		my ymax = my v_defaultYmax ();
	} else {
		double ymin = my v_valueToY (RealTier_getMinimumValue (tier));
		double ymax = my v_valueToY (RealTier_getMaximumValue (tier));
		const double yrange = ymax - ymin;
		if (yrange == 0.0) {
			ymin -= 1.0;
			ymax += 1.0;
		} else {
			ymin -= 0.2 * yrange;
			ymax += 0.2 * yrange;
		}
		Melder_clip (my v_minimumLegalY(), & ymin, my v_maximumLegalY());
		Melder_clip (my v_minimumLegalY(), & ymax, my v_maximumLegalY());
		if (ymin >= ymax) {
			if (isdefined (my v_minimumLegalY ()) && isdefined (my v_maximumLegalY ())) {
				ymin = my v_minimumLegalY ();
				ymax = my v_maximumLegalY ();
			} else if (isdefined (my v_minimumLegalY ())) {
				ymin = my v_minimumLegalY ();
				ymax = ymin + 1.0;
			} else {
				Melder_assert (isdefined (my v_maximumLegalY ()));
				ymax = my v_maximumLegalY ();
				ymin = ymax - 1.0;
			}
		}
		if (ymin < my ymin)
			my ymin = ymin;
		if (ymax > my ymax)
			my ymax = ymax;
		if (my ycursor <= my ymin || my ycursor >= my ymax)
			my ycursor = 0.382 * my ymin + 0.618 * my ymax;
	}
}

void RealTierArea_draw (RealTierArea me, RealTier tier) {
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
		const double yleft = my v_valueToY (RealTier_getValueAtTime (tier, my startWindow()));
		const double yright = my v_valueToY (RealTier_getValueAtTime (tier, my endWindow()));
		Graphics_line (my graphics(), my startWindow(), yleft, my endWindow(), yright);
	} else {
		Graphics_setColour (my graphics(), Melder_BLUE);
		for (integer ipoint = imin; ipoint <= imax; ipoint ++) {
			RealPoint point = tier -> points.at [ipoint];
			const double t = point -> number, y = my v_valueToY (point -> value);
			if (ipoint == 1)
				Graphics_line (my graphics(), my startWindow(), y, t, y);
			else if (ipoint == imin)
				Graphics_line (my graphics(), t, y, my startWindow(), my v_valueToY (RealTier_getValueAtTime (tier, my startWindow())));
			if (ipoint == tier -> points.size)
				Graphics_line (my graphics(), t, y, my endWindow(), y);
			else if (ipoint == imax)
				Graphics_line (my graphics(), t, y, my endWindow(), my v_valueToY (RealTier_getValueAtTime (tier, my endWindow())));
			else {
				RealPoint pointRight = tier -> points.at [ipoint + 1];
				Graphics_line (my graphics(), t, y, pointRight -> number, my v_valueToY (pointRight -> value));
			}
		}
		for (integer ipoint = imin; ipoint <= imax; ipoint ++) {
			RealPoint point = tier -> points.at [ipoint];
			const double t = point -> number, y = my v_valueToY (point -> value);
			const bool pointIsSelected = ( ipoint >= ifirstSelected && ipoint <= ilastSelected );
			Graphics_setColour (my graphics(), pointIsSelected ? Melder_RED : Melder_BLUE);
			Graphics_fillCircle_mm (my graphics(), t, y, 3.0);
		}
	}
	Graphics_setLineWidth (my graphics(), 1.0);
	Graphics_setColour (my graphics(), Melder_BLACK);
}

void RealTierArea_drawWhileDragging (RealTierArea me, RealTier tier) {
	Graphics_xorOn (my graphics(), Melder_MAROON);
	/*
		Draw all selected points as empty circles, if inside the window.
	*/
	for (integer ipoint = my firstSelected; ipoint <= my lastSelected; ipoint ++) {
		const RealPoint point = tier -> points.at [ipoint];
		const double t = point -> number + my dt, y = my v_valueToY (point -> value) + my dy;
		if (t >= my startWindow() && t <= my endWindow())
			Graphics_circle_mm (my graphics(), t, y, 3);
	}

	if (my lastSelected == my firstSelected) {
		/*
			Draw a crosshair with time and y.
		*/
		const RealPoint point = tier -> points.at [my firstSelected];
		const double t = point -> number + my dt, y = my v_valueToY (point -> value) + my dy;
		Graphics_line (my graphics(), t, my ymin, t, my ymax - Graphics_dyMMtoWC (my graphics(), 4.0));
		Graphics_setTextAlignment (my graphics(), kGraphics_horizontalAlignment::CENTRE, Graphics_TOP);
		Graphics_text (my graphics(), t, my ymax, Melder_fixed (t, 6));
		Graphics_line (my graphics(), my startWindow(), y, my endWindow(), y);
		Graphics_setTextAlignment (my graphics(), Graphics_LEFT, Graphics_BOTTOM);
		Graphics_text (my graphics(), my startWindow(), y, Melder_fixed (y, 6));
	}
	Graphics_xorOff (my graphics());
}

bool RealTierArea_mouse (RealTierArea me, RealTier tier, GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	static bool anchorIsInFreePart, anchorIsNearPoint;
	if (event -> isClick()) {
		anchorIsInFreePart = false;
		anchorIsNearPoint = false;
	}
	const double y_fraction_withinRealTierArea = my y_fraction_globalToLocal (y_fraction);
	const double y_world = (1.0 - y_fraction_withinRealTierArea) * my ymin + y_fraction_withinRealTierArea * my ymax;
	my viewRealTierAsWorldByWorld ();
	if (event -> isClick()) {
		if (isdefined (my anchorTime))
			return false;
		RealPoint clickedPoint = nullptr;
		integer inearestPoint = AnyTier_timeToNearestIndexInTimeWindow (tier->asAnyTier(), x_world, my startWindow(), my endWindow());
		if (inearestPoint != 0) {
			RealPoint nearestPoint = tier -> points.at [inearestPoint];
			if (Graphics_distanceWCtoMM (my graphics(), x_world, y_world, nearestPoint -> number, nearestPoint -> value) < 1.5)
				clickedPoint = nearestPoint;
		}
		if (! clickedPoint) {
			anchorIsInFreePart = true;
			my ycursor = y_world;
			my editor -> viewDataAsWorldByFraction ();
			return my editor -> structFunctionEditor :: v_mouseInWideDataView (event, x_world, y_fraction) || true;
		}
		anchorIsNearPoint = true;
		my draggingSelection = event -> shiftKeyPressed &&
			clickedPoint -> number >= my startSelection() && clickedPoint -> number <= my endSelection();
		if (my draggingSelection) {
			AnyTier_getWindowPoints (tier->asAnyTier(), my startSelection(), my endSelection(), & my firstSelected, & my lastSelected);
			Editor_save (my editor, U"Drag points");   // TODO: title can be more specific
		} else {
			my firstSelected = my lastSelected = inearestPoint;
			Editor_save (my editor, U"Drag point");   // TODO: title can be more specific
		}
		my anchorTime = x_world;
		my anchorY = y_world;
		my dt = 0.0;
		my dy = 0.0;
		return FunctionEditor_UPDATE_NEEDED;
	} else if (event -> isDrag() || event -> isDrop()) {
		if (anchorIsInFreePart) {
			my ycursor = y_world;
			my editor -> viewDataAsWorldByFraction ();
			return my editor -> structFunctionEditor :: v_mouseInWideDataView (event, x_world, y_fraction) || true;
		}
		Melder_assert (anchorIsNearPoint);
		my dt = x_world - my anchorTime;
		my dy = y_world - my anchorY;

		if (event -> isDrop()) {
			my anchorTime = undefined;
			const double leftNewTime = tier -> points.at [my firstSelected] -> number + my dt;
			const double rightNewTime = tier -> points.at [my lastSelected] -> number + my dt;
			const bool offLeft = ( leftNewTime < my editor -> tmin );
			const bool offRight = ( rightNewTime > my editor -> tmax );
			const bool draggedPastLeftNeighbour = ( my firstSelected > 1 && leftNewTime <= tier -> points.at [my firstSelected - 1] -> number );
			const bool draggedPastRightNeighbour = ( my lastSelected < tier -> points.size && rightNewTime >= tier -> points.at [my lastSelected + 1] -> number );
			if (offLeft || offRight || draggedPastLeftNeighbour || draggedPastRightNeighbour) {
				Melder_beep ();
				return FunctionEditor_UPDATE_NEEDED;
			}

			for (integer i = my firstSelected; i <= my lastSelected; i ++) {
				RealPoint point = tier -> points.at [i];
				point -> number += my dt;
				double pointY = my v_valueToY (point -> value);
				pointY += my dy;
				Melder_clip (my v_minimumLegalY (), & pointY, my v_maximumLegalY ());
				point -> value = my v_yToValue (pointY);
			}

			/*
				Make sure that the same points are still selected (a problem with Undo...).
			*/

			if (my draggingSelection) {
				my editor -> startSelection += my dt;
				my editor -> endSelection += my dt;
			}
			if (my firstSelected == my lastSelected) {
				/*
					Move crosshair to only selected point.
				*/
				RealPoint point = tier -> points.at [my firstSelected];
				my editor -> startSelection = my editor -> endSelection = point -> number;
				my ycursor = point -> value;
			} else {
				/*
					Move crosshair to mouse location.
				*/
				my ycursor += my dy;
				Melder_clip (my v_minimumLegalY (), & my ycursor, my v_maximumLegalY ());   // NaN-safe
			}

			Editor_broadcastDataChanged (my editor);
			RealTierArea_updateScaling (me, tier);
		}
	}
	return FunctionEditor_UPDATE_NEEDED;
}

/* End of file RealTierArea.cpp */
