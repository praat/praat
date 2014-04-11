/* RealTierEditor.c
 *
 * Copyright (C) 1992-2006 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/07/16 GPL
 * pb 2002/10/06 improved visilibity of dragging
 * pb 2004/04/13 less flashing
 * pb 2006/12/08 keyboard shortcuts
 */

#include "RealTierEditor.h"
#include "FunctionEditor_Sound.h"
#include "Preferences.h"
#include "EditorM.h"

#define SOUND_HEIGHT  0.382

/********** DESTRUCTION **********/

static void destroy (I) {
	iam (RealTierEditor);
	if (my ownSound) forget (my sound.data);
	inherited (RealTierEditor) destroy (me);
}

/********** MENU COMMANDS **********/

DIRECT (RealTierEditor, cb_removePoints)
	Editor_save (me, "Remove point(s)");
	if (my startSelection == my endSelection)
		AnyTier_removePointNear (my data, my startSelection);
	else
		AnyTier_removePointsBetween (my data, my startSelection, my endSelection);
	RealTierEditor_updateScaling (me);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (RealTierEditor, cb_addPointAtCursor)
	Editor_save (me, "Add point");
	RealTier_addPoint (my data, 0.5 * (my startSelection + my endSelection), my ycursor);
	RealTierEditor_updateScaling (me);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (RealTierEditor, cb_addPointAt, "Add point", 0);
	REAL ("Time (s)", "0.0")
	REAL (our quantityText, "0.0")
	OK
SET_REAL ("Time", 0.5 * (my startSelection + my endSelection))
SET_REAL (our quantityKey, my ycursor)
DO
	Editor_save (me, "Add point");
	RealTier_addPoint (my data, GET_REAL ("Time"), GET_REAL (our quantityKey));
	RealTierEditor_updateScaling (me);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (RealTierEditor, cb_setRange, our setRangeTitle, 0);
	REAL (our yminText, our defaultYminText)
	REAL (our ymaxText, our defaultYmaxText)
	OK
SET_REAL (our yminKey, my ymin)
SET_REAL (our ymaxKey, my ymax)
DO
	my ymin = GET_REAL (our yminKey);
	my ymax = GET_REAL (our ymaxKey);
	if (my ymax <= my ymin) RealTierEditor_updateScaling (me);
	FunctionEditor_redraw (me);
END

static void createMenus (I) {
	iam (RealTierEditor);
	inherited (RealTierEditor) createMenus (me);

	if (my sound.data) FunctionEditor_Sound_createMenus (me);

	Editor_addCommand (me, "View", "-- view/realtier --", 0, 0);
	Editor_addCommand (me, "View", our setRangeTitle, 0, cb_setRange);

	Editor_addMenu (me, "Point", 0);
	Editor_addCommand (me, "Point", "Remove point(s)", motif_OPTION + 'T', cb_removePoints);
	Editor_addCommand (me, "Point", "Add point at cursor", 'T', cb_addPointAtCursor);
	Editor_addCommand (me, "Point", "Add point at...", 0, cb_addPointAt);
}

void RealTierEditor_updateScaling (I) {
	iam (RealTierEditor);
	RealTier data = my data;
	if (data -> points -> size == 0) {
		my ymin = our defaultYmin;
		my ymax = our defaultYmax;
	} else {
		double ymin = RealTier_getMinimumValue (my data);
		double ymax = RealTier_getMaximumValue (my data);
		double range = ymax - ymin;
		if (range == 0.0) ymin -= 1.0, ymax += 1.0;
		else ymin -= 0.2 * range, ymax += 0.2 * range;
		if (ymin < my ymin || my ymin < 0.0) my ymin = ymin;
		if (ymax > my ymax) my ymax = ymax;
		if (my ycursor <= my ymin || my ycursor >= my ymax)
			my ycursor = 0.382 * my ymin + 0.618 * my ymax;
	}
}

static void dataChanged (I) {
	iam (RealTierEditor);
	RealTierEditor_updateScaling (me);
	inherited (RealTierEditor) dataChanged (me);
}

/********** DRAWING AREA **********/

static void draw (I) {
	iam (RealTierEditor);
	RealTier data = my data;
	long ifirstSelected, ilastSelected, n = data -> points -> size, imin, imax, i;
	Graphics_Viewport viewport;
	if (my sound.data) {
		viewport = Graphics_insetViewport (my graphics, 0, 1, 1 - SOUND_HEIGHT, 1.0);
		Graphics_setColour (my graphics, Graphics_WHITE);
		Graphics_setWindow (my graphics, 0, 1, 0, 1);
		Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
		FunctionEditor_Sound_draw (me, -1.0, 1.0);
		Graphics_resetViewport (my graphics, viewport);
		Graphics_insetViewport (my graphics, 0, 1, 0.0, 1 - SOUND_HEIGHT);
	}
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	Graphics_setWindow (my graphics, my startWindow, my endWindow, my ymin, my ymax);
	Graphics_setColour (my graphics, Graphics_RED);
	Graphics_line (my graphics, my startWindow, my ycursor, my endWindow, my ycursor);
	Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_printf (my graphics, my startWindow, my ycursor, our leftTickFormat, my ycursor);
	Graphics_setColour (my graphics, Graphics_BLUE);
	Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_TOP);
	Graphics_printf (my graphics, my endWindow, my ymax, our rightTickFormat, my ymax);
	Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_HALF);
	Graphics_printf (my graphics, my endWindow, my ymin, our rightTickFormat, my ymin);
	ifirstSelected = AnyTier_timeToHighIndex (data, my startSelection);
	ilastSelected = AnyTier_timeToLowIndex (data, my endSelection);
	imin = AnyTier_timeToHighIndex (data, my startWindow);
	imax = AnyTier_timeToLowIndex (data, my endWindow);
	Graphics_setLineWidth (my graphics, 2);
	if (n == 0) {
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics, 0.5 * (my startWindow + my endWindow),
			0.5 * (my ymin + my ymax), "(no points)");
	} else if (imax < imin) {
		double yleft = RealTier_getValueAtTime (data, my startWindow);
		double yright = RealTier_getValueAtTime (data, my endWindow);
		Graphics_line (my graphics, my startWindow, yleft, my endWindow, yright);
	} else for (i = imin; i <= imax; i ++) {
		RealPoint point = data -> points -> item [i];
		double t = point -> time, y = point -> value;
		if (i >= ifirstSelected && i <= ilastSelected)
			Graphics_setColour (my graphics, Graphics_RED);	
		Graphics_fillCircle_mm (my graphics, t, y, 3);
		Graphics_setColour (my graphics, Graphics_BLUE);
		if (i == 1)
			Graphics_line (my graphics, my startWindow, y, t, y);
		else if (i == imin)
			Graphics_line (my graphics, t, y, my startWindow, RealTier_getValueAtTime (data, my startWindow));
		if (i == n)
			Graphics_line (my graphics, t, y, my endWindow, y);
		else if (i == imax)
			Graphics_line (my graphics, t, y, my endWindow, RealTier_getValueAtTime (data, my endWindow));
		else {
			RealPoint pointRight = data -> points -> item [i + 1];
			Graphics_line (my graphics, t, y, pointRight -> time, pointRight -> value);
		}
	}
	Graphics_setLineWidth (my graphics, 1);
	Graphics_setColour (my graphics, Graphics_BLACK);
}

static void drawWhileDragging (RealTierEditor me, double xWC, double yWC, long first, long last, double dt, double dy) {
	RealTier data = my data;
	long i;
	(void) xWC;
	(void) yWC;

	/*
	 * Draw all selected points as magenta empty circles, if inside the window.
	 */
	for (i = first; i <= last; i ++) {
		RealPoint point = data -> points -> item [i];
		double t = point -> time + dt, y = point -> value + dy;
		if (t >= my startWindow && t <= my endWindow)
			Graphics_circle_mm (my graphics, t, y, 3);
	}

	if (last == first) {
		/*
		 * Draw a crosshair with time and y.
		 */
		RealPoint point = data -> points -> item [first];
		double t = point -> time + dt, y = point -> value + dy;
		Graphics_line (my graphics, t, my ymin, t, my ymax - Graphics_dyMMtoWC (my graphics, 4.0));
		Graphics_setTextAlignment (my graphics, Graphics_CENTER, Graphics_TOP);
		Graphics_printf (my graphics, t, my ymax, "%f", t);
		Graphics_line (my graphics, my startWindow, y, my endWindow, y);
		Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_BOTTOM);
		Graphics_printf (my graphics, my startWindow, y, "%f", y);
	}
}

static int click (I, double xWC, double yWC, int shiftKeyPressed) {
	iam (RealTierEditor);
	RealTier pitch = my data;
	long inearestPoint, ifirstSelected, ilastSelected, i;
	RealPoint nearestPoint;
	double dt = 0, df = 0;
	int draggingSelection;
	Graphics_Viewport viewport;

	/*
	 * Perform the default action: move cursor.
	 */
	my startSelection = my endSelection = xWC;
	if (my sound.data) {
		if (yWC < 1 - SOUND_HEIGHT) {   /* Clicked in tier area? */
			yWC /= 1 - SOUND_HEIGHT;
			my ycursor = (1.0 - yWC) * my ymin + yWC * my ymax;
			viewport = Graphics_insetViewport (my graphics, 0, 1, 0, 1 - SOUND_HEIGHT);
		} else {
			return inherited (RealTierEditor) click (me, xWC, yWC, shiftKeyPressed);
		}
	} else {
		my ycursor = (1.0 - yWC) * my ymin + yWC * my ymax;
	}
	Graphics_setWindow (my graphics, my startWindow, my endWindow, my ymin, my ymax);
	yWC = my ycursor;

	/*
	 * Clicked on a point?
	 */
	inearestPoint = AnyTier_timeToNearestIndex (pitch, xWC);
	if (inearestPoint == 0) return inherited (RealTierEditor) click (me, xWC, yWC, shiftKeyPressed);
	nearestPoint = pitch -> points -> item [inearestPoint];
	if (Graphics_distanceWCtoMM (my graphics, xWC, yWC, nearestPoint -> time, nearestPoint -> value) > 1.5) {
		if (my sound.data) Graphics_resetViewport (my graphics, viewport);
		return inherited (RealTierEditor) click (me, xWC, yWC, shiftKeyPressed);
	}

	/*
	 * Clicked on a selected point?
	 */
	draggingSelection = shiftKeyPressed &&
		nearestPoint -> time > my startSelection && nearestPoint -> time < my endSelection;
	if (draggingSelection) {
		ifirstSelected = AnyTier_timeToHighIndex (pitch, my startSelection);
		ilastSelected = AnyTier_timeToLowIndex (pitch, my endSelection);
		Editor_save (me, "Drag points");
	} else {
		ifirstSelected = ilastSelected = inearestPoint;
		Editor_save (me, "Drag point");
	}

	/*
	 * Drag.
	 */
	Graphics_xorOn (my graphics, Graphics_MAGENTA);
	drawWhileDragging (me, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
	while (Graphics_mouseStillDown (my graphics)) {
		double xWC_new, yWC_new;
		drawWhileDragging (me, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
		Graphics_getMouseLocation (my graphics, & xWC_new, & yWC_new);
		dt += xWC_new - xWC, df += yWC_new - yWC;
		xWC = xWC_new, yWC = yWC_new;
		drawWhileDragging (me, xWC_new, yWC_new, ifirstSelected, ilastSelected, dt, df);
	}
	Graphics_xorOff (my graphics);

	/*
	 * Dragged inside window?
	 */
	if (xWC < my startWindow || xWC > my endWindow) return 1;

	/*
	 * Points not dragged past neighbours?
	 */
	{
		RealPoint *points = (RealPoint *) pitch -> points -> item;
		double newTime = points [ifirstSelected] -> time + dt;
		if (newTime < my tmin) return 1;   /* Outside domain. */
		if (ifirstSelected > 1 && newTime <= points [ifirstSelected - 1] -> time)
			return 1;   /* Past left neighbour. */
		newTime = points [ilastSelected] -> time + dt;
		if (newTime > my tmax) return 1;   /* Outside domain. */
		if (ilastSelected < pitch -> points -> size && newTime >= points [ilastSelected + 1] -> time)
			return 1;   /* Past right neighbour. */
	}

	/*
	 * Drop.
	 */
	for (i = ifirstSelected; i <= ilastSelected; i ++) {
		RealPoint point = pitch -> points -> item [i];
		point -> time += dt;
		point -> value += df;
	}

	/*
	 * Make sure that the same points are still selected (a problem with Undo...).
	 */

	if (draggingSelection) my startSelection += dt, my endSelection += dt;
	if (ifirstSelected == ilastSelected) {
		/*
		 * Move crosshair to only selected pitch point.
		 */
		RealPoint point = pitch -> points -> item [ifirstSelected];
		my startSelection = my endSelection = point -> time;
		my ycursor = point -> value;
	} else {
		/*
		 * Move crosshair to mouse location.
		 */
		/*my cursor += dt;*/
		my ycursor += df;
	}

	Editor_broadcastChange (me);
	RealTierEditor_updateScaling (me);
	return 1;   /* Update needed. */
}

static void play (I, double tmin, double tmax) {
	iam (RealTierEditor);
	if (my sound.data)
		Sound_playPart (my sound.data, tmin, tmax, our playCallback, me);
}

class_methods (RealTierEditor, FunctionEditor)
	class_method (destroy)
	class_method (dataChanged)
	class_method (createMenus)
	class_method (draw)
	class_method (click)
	class_method (play)
	us -> zeroIsMinimum = FALSE;
	us -> quantityText = "Y";   /* Normally includes units. */
	us -> quantityKey = "Y";   /* Without units. */
	us -> leftTickFormat = "%5g";   /* Often without units (so that number does not run off screen). */
	us -> rightTickFormat = "%5g";   /* Often with units (units may run off screen). */
	us -> defaultYmin = 0.0;
	us -> defaultYmax = 1.0;
	us -> setRangeTitle = "Set range...";
	us -> defaultYminText = "0.0";
	us -> defaultYmaxText = "1.0";
	us -> yminText = "Minimum";   /* Normally includes units. */
	us -> ymaxText = "Maximum";   /* Normally includes units. */
	us -> yminKey = "Minimum";   /* Without units. */
	us -> ymaxKey = "Maximum";   /* Without units. */
class_methods_end

int RealTierEditor_init (I, Widget parent, const char *title, RealTier data, Sound sound, int ownSound) {
	iam (RealTierEditor);
	Melder_assert (data != NULL);
	Melder_assert (Thing_member (data, classRealTier));
	Melder_assert (sound == NULL || Thing_member (sound, classSound));
	my ownSound = ownSound;
	if (sound && ownSound) {
		if (! (my sound.data = Data_copy (sound))) return 0;   /* Deep copy; ownership transferred. */
	} else {
		my sound.data = sound;   /* Reference copy; ownership not transferred. */
	}
	if (! FunctionEditor_init (me, parent, title, data)) return 0;
	FunctionEditor_Sound_init (me);
	my ymin = -1.0;
	RealTierEditor_updateScaling (me);
	my ycursor = 0.382 * my ymin + 0.618 * my ymax;
	return 1;
}

/* End of file RealTierEditor.c */
