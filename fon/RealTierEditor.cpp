/* RealTierEditor.cpp
 *
 * Copyright (C) 1992-2011,2012,2013,2014,2015,2016,2017 Paul Boersma
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
	EDITOR_FORM (U"Add point", 0)
		REAL (U"Time (s)", U"0.0")
		REAL (my v_quantityText (), U"0.0")
	EDITOR_OK
		SET_REAL (U"Time", 0.5 * (my startSelection + my endSelection))
		SET_REAL (my v_quantityKey (), my ycursor)
	EDITOR_DO
		double desiredValue = GET_REAL (my v_quantityKey ());
		if (isdefined (my v_minimumLegalValue ()) && desiredValue < my v_minimumLegalValue ())
			Melder_throw (U"Cannot add a point below ", my v_minimumLegalValue (), my v_rightTickUnits (), U".");
		if (isdefined (my v_maximumLegalValue ()) && desiredValue > my v_maximumLegalValue ())
			Melder_throw (U"Cannot add a point above ", my v_maximumLegalValue (), my v_rightTickUnits (), U".");
		Editor_save (me, U"Add point");
		RealTier_addPoint ((RealTier) my data, GET_REAL (U"Time"), desiredValue);
		RealTierEditor_updateScaling (me);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_setRange (RealTierEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (my v_setRangeTitle (), 0)
		REAL (my v_yminText (), my v_defaultYminText ())
		REAL (my v_ymaxText (), my v_defaultYmaxText ())
	EDITOR_OK
		SET_REAL (my v_yminKey (), my ymin)
		SET_REAL (my v_ymaxKey (), my ymax)
	EDITOR_DO
		my ymin = GET_REAL (my v_yminKey ());
		my ymax = GET_REAL (my v_ymaxKey ());
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
		if (range == 0.0) ymin -= 1.0, ymax += 1.0;
		else ymin -= 0.2 * range, ymax += 0.2 * range;
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
		if (ymin < my ymin || my ymin < 0.0) my ymin = ymin;
		if (ymax > my ymax) my ymax = ymax;
		if (my ycursor <= my ymin || my ycursor >= my ymax)
			my ycursor = 0.382 * my ymin + 0.618 * my ymax;
	}
}

void structRealTierEditor :: v_dataChanged () {
	RealTierEditor_updateScaling (this);
	RealTierEditor_Parent :: v_dataChanged ();
}

/********** DRAWING AREA **********/

void structRealTierEditor :: v_draw () {
	RealTier data = (RealTier) our data;
	long n = data -> points.size;
	trace (U"structRealTierEditor :: v_draw ", n);
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
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, our ymin, our ymax);
	Graphics_setColour (our graphics.get(), Graphics_RED);
	Graphics_line (our graphics.get(), our startWindow, ycursor, our endWindow, our ycursor);
	Graphics_setTextAlignment (our graphics.get(), Graphics_RIGHT, Graphics_HALF);
	Graphics_text (our graphics.get(), our startWindow, our ycursor, Melder_float (Melder_half (our ycursor)));
	Graphics_setColour (our graphics.get(), Graphics_BLUE);
	Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT, Graphics_TOP);
	Graphics_text (our graphics.get(), our endWindow, our ymax,   Melder_float (Melder_half (ymax)), our v_rightTickUnits ());
	Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT, Graphics_HALF);
	Graphics_text (our graphics.get(), our endWindow, our ymin,   Melder_float (Melder_half (our ymin)), our v_rightTickUnits ());
	long ifirstSelected = AnyTier_timeToHighIndex (data->asAnyTier(), our startSelection);
	long ilastSelected = AnyTier_timeToLowIndex (data->asAnyTier(), our endSelection);
	trace (U"structRealTierEditor :: v_draw: selected from ", our startSelection, U" ",
		ifirstSelected, U" to ", our endSelection, U" ", ilastSelected);
	long imin = AnyTier_timeToHighIndex (data->asAnyTier(), our startWindow);
	long imax = AnyTier_timeToLowIndex (data->asAnyTier(), our endWindow);
	Graphics_setLineWidth (our graphics.get(), 2.0);
	if (n == 0) {
		Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_HALF);
		Graphics_text (our graphics.get(), 0.5 * (our startWindow + our endWindow),
			0.5 * (our ymin + our ymax), U"(no points)");
	} else if (imax < imin) {
		double yleft = RealTier_getValueAtTime (data, our startWindow);
		double yright = RealTier_getValueAtTime (data, our endWindow);
		Graphics_line (our graphics.get(), our startWindow, yleft, our endWindow, yright);
	} else for (long i = imin; i <= imax; i ++) {
		RealPoint point = data -> points.at [i];
		double t = point -> number, y = point -> value;
		if (i >= ifirstSelected && i <= ilastSelected)
			Graphics_setColour (our graphics.get(), Graphics_RED);
		Graphics_fillCircle_mm (our graphics.get(), t, y, 3.0);
		Graphics_setColour (our graphics.get(), Graphics_BLUE);
		if (i == 1)
			Graphics_line (our graphics.get(), our startWindow, y, t, y);
		else if (i == imin)
			Graphics_line (our graphics.get(), t, y, our startWindow, RealTier_getValueAtTime (data, our startWindow));
		if (i == n)
			Graphics_line (our graphics.get(), t, y, our endWindow, y);
		else if (i == imax)
			Graphics_line (our graphics.get(), t, y, our endWindow, RealTier_getValueAtTime (data, our endWindow));
		else {
			RealPoint pointRight = data -> points.at [i + 1];
			Graphics_line (our graphics.get(), t, y, pointRight -> number, pointRight -> value);
		}
	}
	Graphics_setLineWidth (our graphics.get(), 1.0);
	Graphics_setColour (our graphics.get(), Graphics_BLACK);
	our v_updateMenuItems_file ();
}

static void drawWhileDragging (RealTierEditor me, double /* xWC */, double /* yWC */, long first, long last, double dt, double dy) {
	RealTier data = (RealTier) my data;

	/*
	 * Draw all selected points as magenta empty circles, if inside the window.
	 */
	for (long i = first; i <= last; i ++) {
		RealPoint point = data -> points.at [i];
		double t = point -> number + dt, y = point -> value + dy;
		if (t >= my startWindow && t <= my endWindow)
			Graphics_circle_mm (my graphics.get(), t, y, 3);
	}

	if (last == first) {
		/*
		 * Draw a crosshair with time and y.
		 */
		RealPoint point = data -> points.at [first];
		double t = point -> number + dt, y = point -> value + dy;
		Graphics_line (my graphics.get(), t, my ymin, t, my ymax - Graphics_dyMMtoWC (my graphics.get(), 4.0));
		Graphics_setTextAlignment (my graphics.get(), kGraphics_horizontalAlignment_CENTRE, Graphics_TOP);
		Graphics_text (my graphics.get(), t, my ymax, Melder_fixed (t, 6));
		Graphics_line (my graphics.get(), my startWindow, y, my endWindow, y);
		Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_BOTTOM);
		Graphics_text (my graphics.get(), my startWindow, y, Melder_fixed (y, 6));
	}
}

bool structRealTierEditor :: v_click (double xWC, double yWC, bool shiftKeyPressed) {
	RealTier pitch = (RealTier) our data;
	double dt = 0.0, df = 0.0;
	Graphics_Viewport viewport;

	/*
	 * Perform the default action: move cursor.
	 */
	//our startSelection = our endSelection = xWC;
	if (our d_sound.data) {
		if (yWC < 1.0 - SOUND_HEIGHT) {   // clicked in tier area?
			yWC /= 1.0 - SOUND_HEIGHT;
			our ycursor = (1.0 - yWC) * our ymin + yWC * our ymax;
			viewport = Graphics_insetViewport (our graphics.get(), 0.0, 1.0, 0.0, 1.0 - SOUND_HEIGHT);
		} else {
			return our RealTierEditor_Parent :: v_click (xWC, yWC, shiftKeyPressed);
		}
	} else {
		our ycursor = (1.0 - yWC) * our ymin + yWC * our ymax;
	}
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, our ymin, our ymax);
	yWC = our ycursor;

	/*
	 * Clicked on a point?
	 */
	long inearestPoint = AnyTier_timeToNearestIndex (pitch->asAnyTier(), xWC);
	if (inearestPoint == 0) return our RealTierEditor_Parent :: v_click (xWC, yWC, shiftKeyPressed);
	RealPoint nearestPoint = pitch -> points.at [inearestPoint];
	if (Graphics_distanceWCtoMM (our graphics.get(), xWC, yWC, nearestPoint -> number, nearestPoint -> value) > 1.5) {
		if (our d_sound.data) Graphics_resetViewport (our graphics.get(), viewport);
		return our RealTierEditor_Parent :: v_click (xWC, yWC, shiftKeyPressed);
	}

	/*
	 * Clicked on a selected point?
	 */
	bool draggingSelection = shiftKeyPressed &&
		nearestPoint -> number > our startSelection && nearestPoint -> number < our endSelection;
	long ifirstSelected, ilastSelected;
	if (draggingSelection) {
		ifirstSelected = AnyTier_timeToHighIndex (pitch->asAnyTier(), our startSelection);
		ilastSelected = AnyTier_timeToLowIndex (pitch->asAnyTier(), our endSelection);
		Editor_save (this, U"Drag points");
	} else {
		ifirstSelected = ilastSelected = inearestPoint;
		Editor_save (this, U"Drag point");
	}

	/*
	 * Drag.
	 */
	Graphics_xorOn (our graphics.get(), Graphics_MAROON);
	drawWhileDragging (this, xWC, yWC, ifirstSelected, ilastSelected, dt, df);   // draw at old position
	while (Graphics_mouseStillDown (our graphics.get())) {
		double xWC_new, yWC_new;
		Graphics_getMouseLocation (our graphics.get(), & xWC_new, & yWC_new);
		if (xWC_new != xWC || yWC_new != yWC) {
			drawWhileDragging (this, xWC, yWC, ifirstSelected, ilastSelected, dt, df);   // undraw at old position
			dt += xWC_new - xWC, df += yWC_new - yWC;
			xWC = xWC_new, yWC = yWC_new;
			drawWhileDragging (this, xWC, yWC, ifirstSelected, ilastSelected, dt, df);   // draw at new position
		}
	}
	Graphics_xorOff (our graphics.get());

	/*
	 * Dragged inside window?
	 */
	if (xWC < our startWindow || xWC > our endWindow) return 1;

	/*
	 * Points not dragged past neighbours?
	 */
	{
		double newTime = pitch -> points.at [ifirstSelected] -> number + dt;
		if (newTime < our tmin) return 1;   // outside domain
		if (ifirstSelected > 1 && newTime <= pitch -> points.at [ifirstSelected - 1] -> number)
			return 1;   // past left neighbour
		newTime = pitch -> points.at [ilastSelected] -> number + dt;
		if (newTime > our tmax) return 1;   // outside domain
		if (ilastSelected < pitch -> points.size && newTime >= pitch -> points.at [ilastSelected + 1] -> number)
			return FunctionEditor_UPDATE_NEEDED;   // past right neighbour
	}

	/*
	 * Drop.
	 */
	for (int i = ifirstSelected; i <= ilastSelected; i ++) {
		RealPoint point = pitch -> points.at [i];
		point -> number += dt;
		point -> value += df;
		if (isdefined (v_minimumLegalValue ()) && point -> value < v_minimumLegalValue ())
			point -> value = v_minimumLegalValue ();
		if (isdefined (v_maximumLegalValue ()) && point -> value > v_maximumLegalValue ())
			point -> value = v_maximumLegalValue ();
	}

	/*
	 * Make sure that the same points are still selected (a problem with Undo...).
	 */

	if (draggingSelection) our startSelection += dt, our endSelection += dt;
	if (ifirstSelected == ilastSelected) {
		/*
		 * Move crosshair to only selected pitch point.
		 */
		RealPoint point = pitch -> points.at [ifirstSelected];
		our startSelection = our endSelection = point -> number;
		our ycursor = point -> value;
	} else {
		/*
		 * Move crosshair to mouse location.
		 */
		/*our cursor += dt;*/
		our ycursor += df;
		if (isdefined (v_minimumLegalValue ()) && our ycursor < v_minimumLegalValue ())
			our ycursor = v_minimumLegalValue ();
		if (isdefined (v_maximumLegalValue ()) && our ycursor > v_maximumLegalValue ())
			our ycursor = v_maximumLegalValue ();
	}

	Editor_broadcastDataChanged (this);
	RealTierEditor_updateScaling (this);
	return FunctionEditor_UPDATE_NEEDED;
}

void structRealTierEditor :: v_play (double a_tmin, double a_tmax) {
	if (our d_sound.data)
		Sound_playPart (our d_sound.data, a_tmin, a_tmax, theFunctionEditor_playCallback, this);
}

void RealTierEditor_init (RealTierEditor me, const char32 *title, RealTier data, Sound sound, bool ownSound) {
	Melder_assert (data);
	Melder_assert (Thing_isa (data, classRealTier));
	TimeSoundEditor_init (me, title, data, sound, ownSound);
	my ymin = -1.0;
	RealTierEditor_updateScaling (me);
	my ycursor = 0.382 * my ymin + 0.618 * my ymax;
}

/* End of file RealTierEditor.cpp */
