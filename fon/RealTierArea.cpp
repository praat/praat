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
#include "EditorM.h"

/* MARK: - FUNCTIONVIEW */

Thing_implement (FunctionView, Thing, 0);

/* MARK: - REALTIERVIEW */

Thing_implement (RealTierView, FunctionView, 0);

void RealTierArea_addPointAt (RealTierView me, RealTier tier, double time, double desiredY) {
	if (isdefined (my v_minimumLegalY ()) && desiredY < my v_minimumLegalY ())
		Melder_throw (U"Cannot add a point below ", my v_minimumLegalY (), my v_rightTickUnits (), U".");
	if (isdefined (my v_maximumLegalY ()) && desiredY > my v_maximumLegalY ())
		Melder_throw (U"Cannot add a point above ", my v_maximumLegalY (), my v_rightTickUnits (), U".");
	RealTier_addPoint (tier, time, my v_yToValue (desiredY));
}

void RealTierArea_removePoints (RealTierView me, RealTier tier) {
	if (my startSelection() == my endSelection())
		AnyTier_removePointNear (tier->asAnyTier(), my startSelection());
	else
		AnyTier_removePointsBetween (tier->asAnyTier(), my startSelection(), my endSelection());
}

void RealTierArea_addPointAtCursor (RealTierView me, RealTier tier) {
	const double cursorTime = 0.5 * (my startSelection() + my endSelection());
	RealTierArea_addPointAt (me, tier, cursorTime, my v_yToValue (my ycursor));
}

void RealTierArea_updateScaling (RealTierView me, RealTier tier) {
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
		if (ymin < my ymin || my ymin < 0.0)   // what?
			my ymin = ymin;
		if (ymax > my ymax)
			my ymax = ymax;
		if (my ycursor <= my ymin || my ycursor >= my ymax)
			my ycursor = 0.382 * my ymin + 0.618 * my ymax;
	}
}

void RealTierArea_draw (RealTierView me, RealTier tier) {
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

void RealTierArea_drawWhileDragging (RealTierView me, RealTier tier) {
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

/* End of file RealTierArea.cpp */
