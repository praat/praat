/* RealTierEditor.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 2007/06/10 wchar_t
 * pb 2007/09/04 new FunctionEditor API
 * pb 2007/09/08 inherit from TimeSoundEditor
 * pb 2007/11/30 erased Graphics_printf
 * pb 2008/03/21 new Editor API
 * pb 2009/01/23 minimum and maximum legal values
 * pb 2011/03/23 C++
 * pb 2011/07/01 C++
 */

#include "RealTierEditor.h"
#include "Preferences.h"
#include "EditorM.h"

#undef our
#define our ((RealTierEditor_Table) my methods) ->

#define SOUND_HEIGHT  0.382

/********** MENU COMMANDS **********/

static int menu_cb_removePoints (EDITOR_ARGS) {
	EDITOR_IAM (RealTierEditor);
	Editor_save (me, L"Remove point(s)");
	if (my startSelection == my endSelection)
		AnyTier_removePointNear (my data, my startSelection);
	else
		AnyTier_removePointsBetween (my data, my startSelection, my endSelection);
	RealTierEditor_updateScaling (me);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
	return 1;
}

static int menu_cb_addPointAtCursor (EDITOR_ARGS) {
	EDITOR_IAM (RealTierEditor);
	if (NUMdefined (our minimumLegalValue) && my ycursor < our minimumLegalValue)
		return Melder_error4 (L"Cannot add a point below ", Melder_double (our minimumLegalValue), our rightTickUnits, L".");
	if (NUMdefined (our maximumLegalValue) && my ycursor > our maximumLegalValue)
		return Melder_error4 (L"Cannot add a point above ", Melder_double (our maximumLegalValue), our rightTickUnits, L".");
	Editor_save (me, L"Add point");
	RealTier_addPoint (my data, 0.5 * (my startSelection + my endSelection), my ycursor);
	RealTierEditor_updateScaling (me);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
	return 1;
}

static int menu_cb_addPointAt (EDITOR_ARGS) {
	EDITOR_IAM (RealTierEditor);
	EDITOR_FORM (L"Add point", 0)
		REAL (L"Time (s)", L"0.0")
		REAL (our quantityText, L"0.0")
	EDITOR_OK
		SET_REAL (L"Time", 0.5 * (my startSelection + my endSelection))
		SET_REAL (our quantityKey, my ycursor)
	EDITOR_DO
		double desiredValue = GET_REAL (our quantityKey);
		if (NUMdefined (our minimumLegalValue) && desiredValue < our minimumLegalValue)
			return Melder_error4 (L"Cannot add a point below ", Melder_double (our minimumLegalValue), our rightTickUnits, L".");
		if (NUMdefined (our maximumLegalValue) && desiredValue > our maximumLegalValue)
			return Melder_error4 (L"Cannot add a point above ", Melder_double (our maximumLegalValue), our rightTickUnits, L".");
		Editor_save (me, L"Add point");
		RealTier_addPoint (my data, GET_REAL (L"Time"), desiredValue);
		RealTierEditor_updateScaling (me);
		FunctionEditor_redraw (me);
		Editor_broadcastChange (me);
	EDITOR_END
}

static int menu_cb_setRange (EDITOR_ARGS) {
	EDITOR_IAM (RealTierEditor);
	EDITOR_FORM (our setRangeTitle, 0)
		REAL (our yminText, our defaultYminText)
		REAL (our ymaxText, our defaultYmaxText)
	EDITOR_OK
		SET_REAL (our yminKey, my ymin)
		SET_REAL (our ymaxKey, my ymax)
	EDITOR_DO
		my ymin = GET_REAL (our yminKey);
		my ymax = GET_REAL (our ymaxKey);
		if (my ymax <= my ymin) RealTierEditor_updateScaling (me);
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void createMenuItems_view (RealTierEditor me, EditorMenu menu) {
	inherited (RealTierEditor) createMenuItems_view (me, menu);
	EditorMenu_addCommand (menu, L"-- view/realtier --", 0, 0);
	EditorMenu_addCommand (menu, our setRangeTitle, 0, menu_cb_setRange);
}

void structRealTierEditor :: v_createMenus () {
	RealTierEditor_Parent :: v_createMenus ();
	EditorMenu menu = Editor_addMenu (this, L"Point", 0);
	EditorMenu_addCommand (menu, L"Add point at cursor", 'T', menu_cb_addPointAtCursor);
	EditorMenu_addCommand (menu, L"Add point at...", 0, menu_cb_addPointAt);
	EditorMenu_addCommand (menu, L"-- remove point --", 0, NULL);
	EditorMenu_addCommand (menu, L"Remove point(s)", GuiMenu_OPTION + 'T', menu_cb_removePoints);
}

void RealTierEditor_updateScaling (RealTierEditor me) {
	RealTier data = (RealTier) my data;
	if (data -> points -> size == 0) {
		my ymin = our defaultYmin;
		my ymax = our defaultYmax;
	} else {
		double ymin = RealTier_getMinimumValue (my data);
		double ymax = RealTier_getMaximumValue (my data);
		double range = ymax - ymin;
		if (range == 0.0) ymin -= 1.0, ymax += 1.0;
		else ymin -= 0.2 * range, ymax += 0.2 * range;
		if (NUMdefined (our minimumLegalValue) && ymin < our minimumLegalValue)
			ymin = our minimumLegalValue;
		if (NUMdefined (our maximumLegalValue) && ymin > our maximumLegalValue)
			ymin = our maximumLegalValue;
		if (NUMdefined (our minimumLegalValue) && ymax < our minimumLegalValue)
			ymax = our minimumLegalValue;
		if (NUMdefined (our maximumLegalValue) && ymax > our maximumLegalValue)
			ymax = our maximumLegalValue;
		if (ymin >= ymax) {
			if (NUMdefined (our minimumLegalValue) && NUMdefined (our maximumLegalValue)) {
				ymin = our minimumLegalValue;
				ymax = our maximumLegalValue;
			} else if (NUMdefined (our minimumLegalValue)) {
				ymin = our minimumLegalValue;
				ymax = ymin + 1.0;
			} else {
				Melder_assert (NUMdefined (our maximumLegalValue));
				ymax = our maximumLegalValue;
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

static void draw (RealTierEditor me) {
	RealTier data = (RealTier) my data;
	long ifirstSelected, ilastSelected, n = data -> points -> size, imin, imax, i;
	Graphics_Viewport viewport;
	if (my sound.data) {
		viewport = Graphics_insetViewport (my graphics, 0, 1, 1 - SOUND_HEIGHT, 1.0);
		Graphics_setColour (my graphics, Graphics_WHITE);
		Graphics_setWindow (my graphics, 0, 1, 0, 1);
		Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
		TimeSoundEditor_draw_sound (me, -1.0, 1.0);
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
	Graphics_text1 (my graphics, my startWindow, my ycursor, Melder_float (Melder_half (my ycursor)));
	Graphics_setColour (my graphics, Graphics_BLUE);
	Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_TOP);
	Graphics_text2 (my graphics, my endWindow, my ymax, Melder_float (Melder_half (my ymax)), our rightTickUnits);
	Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_HALF);
	Graphics_text2 (my graphics, my endWindow, my ymin, Melder_float (Melder_half (my ymin)), our rightTickUnits);
	ifirstSelected = AnyTier_timeToHighIndex (data, my startSelection);
	ilastSelected = AnyTier_timeToLowIndex (data, my endSelection);
	imin = AnyTier_timeToHighIndex (data, my startWindow);
	imax = AnyTier_timeToLowIndex (data, my endWindow);
	Graphics_setLineWidth (my graphics, 2);
	if (n == 0) {
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics, 0.5 * (my startWindow + my endWindow),
			0.5 * (my ymin + my ymax), L"(no points)");
	} else if (imax < imin) {
		double yleft = RealTier_getValueAtTime (data, my startWindow);
		double yright = RealTier_getValueAtTime (data, my endWindow);
		Graphics_line (my graphics, my startWindow, yleft, my endWindow, yright);
	} else for (i = imin; i <= imax; i ++) {
		RealPoint point = (RealPoint) data -> points -> item [i];
		double t = point -> number, y = point -> value;
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
			RealPoint pointRight = (RealPoint) data -> points -> item [i + 1];
			Graphics_line (my graphics, t, y, pointRight -> number, pointRight -> value);
		}
	}
	Graphics_setLineWidth (my graphics, 1);
	Graphics_setColour (my graphics, Graphics_BLACK);
	our updateMenuItems_file (me);
}

static void drawWhileDragging (RealTierEditor me, double xWC, double yWC, long first, long last, double dt, double dy) {
	RealTier data = (RealTier) my data;
	(void) xWC;
	(void) yWC;

	/*
	 * Draw all selected points as magenta empty circles, if inside the window.
	 */
	for (long i = first; i <= last; i ++) {
		RealPoint point = (RealPoint) data -> points -> item [i];
		double t = point -> number + dt, y = point -> value + dy;
		if (t >= my startWindow && t <= my endWindow)
			Graphics_circle_mm (my graphics, t, y, 3);
	}

	if (last == first) {
		/*
		 * Draw a crosshair with time and y.
		 */
		RealPoint point = (RealPoint) data -> points -> item [first];
		double t = point -> number + dt, y = point -> value + dy;
		Graphics_line (my graphics, t, my ymin, t, my ymax - Graphics_dyMMtoWC (my graphics, 4.0));
		Graphics_setTextAlignment (my graphics, kGraphics_horizontalAlignment_CENTRE, Graphics_TOP);
		Graphics_text1 (my graphics, t, my ymax, Melder_fixed (t, 6));
		Graphics_line (my graphics, my startWindow, y, my endWindow, y);
		Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_BOTTOM);
		Graphics_text1 (my graphics, my startWindow, y, Melder_fixed (y, 6));
	}
}

static int click (RealTierEditor me, double xWC, double yWC, int shiftKeyPressed) {
	RealTier pitch = (RealTier) my data;
	long inearestPoint, ifirstSelected, ilastSelected, i;
	RealPoint nearestPoint;
	double dt = 0, df = 0;
	int draggingSelection;
	Graphics_Viewport viewport;

	/*
	 * Perform the default action: move cursor.
	 */
	//my startSelection = my endSelection = xWC;
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
	nearestPoint = (RealPoint) pitch -> points -> item [inearestPoint];
	if (Graphics_distanceWCtoMM (my graphics, xWC, yWC, nearestPoint -> number, nearestPoint -> value) > 1.5) {
		if (my sound.data) Graphics_resetViewport (my graphics, viewport);
		return inherited (RealTierEditor) click (me, xWC, yWC, shiftKeyPressed);
	}

	/*
	 * Clicked on a selected point?
	 */
	draggingSelection = shiftKeyPressed &&
		nearestPoint -> number > my startSelection && nearestPoint -> number < my endSelection;
	if (draggingSelection) {
		ifirstSelected = AnyTier_timeToHighIndex (pitch, my startSelection);
		ilastSelected = AnyTier_timeToLowIndex (pitch, my endSelection);
		Editor_save (me, L"Drag points");
	} else {
		ifirstSelected = ilastSelected = inearestPoint;
		Editor_save (me, L"Drag point");
	}

	/*
	 * Drag.
	 */
	Graphics_xorOn (my graphics, Graphics_MAROON);
	drawWhileDragging (me, xWC, yWC, ifirstSelected, ilastSelected, dt, df);   // draw at old position
	while (Graphics_mouseStillDown (my graphics)) {
		double xWC_new, yWC_new;
		Graphics_getMouseLocation (my graphics, & xWC_new, & yWC_new);
		if (xWC_new != xWC || yWC_new != yWC) {
			drawWhileDragging (me, xWC, yWC, ifirstSelected, ilastSelected, dt, df);   // undraw at old position
			dt += xWC_new - xWC, df += yWC_new - yWC;
			xWC = xWC_new, yWC = yWC_new;
			drawWhileDragging (me, xWC, yWC, ifirstSelected, ilastSelected, dt, df);   // draw at new position
		}
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
		double newTime = points [ifirstSelected] -> number + dt;
		if (newTime < my tmin) return 1;   // outside domain
		if (ifirstSelected > 1 && newTime <= points [ifirstSelected - 1] -> number)
			return 1;   // past left neighbour
		newTime = points [ilastSelected] -> number + dt;
		if (newTime > my tmax) return 1;   // outside domain
		if (ilastSelected < pitch -> points -> size && newTime >= points [ilastSelected + 1] -> number)
			return 1;   // past right neighbour
	}

	/*
	 * Drop.
	 */
	for (i = ifirstSelected; i <= ilastSelected; i ++) {
		RealPoint point = (RealPoint) pitch -> points -> item [i];
		point -> number += dt;
		point -> value += df;
		if (NUMdefined (our minimumLegalValue) && point -> value < our minimumLegalValue)
			point -> value = our minimumLegalValue;
		if (NUMdefined (our maximumLegalValue) && point -> value > our maximumLegalValue)
			point -> value = our maximumLegalValue;
	}

	/*
	 * Make sure that the same points are still selected (a problem with Undo...).
	 */

	if (draggingSelection) my startSelection += dt, my endSelection += dt;
	if (ifirstSelected == ilastSelected) {
		/*
		 * Move crosshair to only selected pitch point.
		 */
		RealPoint point = (RealPoint) pitch -> points -> item [ifirstSelected];
		my startSelection = my endSelection = point -> number;
		my ycursor = point -> value;
	} else {
		/*
		 * Move crosshair to mouse location.
		 */
		/*my cursor += dt;*/
		my ycursor += df;
		if (NUMdefined (our minimumLegalValue) && my ycursor < our minimumLegalValue)
			my ycursor = our minimumLegalValue;
		if (NUMdefined (our maximumLegalValue) && my ycursor > our maximumLegalValue)
			my ycursor = our maximumLegalValue;
	}

	Editor_broadcastChange (me);
	RealTierEditor_updateScaling (me);
	return 1;   // update needed
}

static void play (RealTierEditor me, double tmin, double tmax) {
	if (my sound.data)
		Sound_playPart (my sound.data, tmin, tmax, our playCallback, me);
}

class_methods (RealTierEditor, TimeSoundEditor) {
	class_method (draw)
	class_method (click)
	class_method (play)
	us -> minimumLegalValue = NUMundefined;
	us -> maximumLegalValue = NUMundefined;
	us -> quantityText = L"Y";   /* Normally includes units. */
	us -> quantityKey = L"Y";   /* Without units. */
	us -> rightTickUnits = L"";
	us -> defaultYmin = 0.0;
	us -> defaultYmax = 1.0;
	us -> setRangeTitle = L"Set range...";
	us -> defaultYminText = L"0.0";
	us -> defaultYmaxText = L"1.0";
	us -> yminText = L"Minimum";   /* Normally includes units. */
	us -> ymaxText = L"Maximum";   /* Normally includes units. */
	us -> yminKey = L"Minimum";   /* Without units. */
	us -> ymaxKey = L"Maximum";   /* Without units. */
	class_method (createMenuItems_view)
	class_methods_end
}

void RealTierEditor_init (RealTierEditor me, GuiObject parent, const wchar_t *title, RealTier data, Sound sound, bool ownSound) {
	Melder_assert (data != NULL);
	Melder_assert (Thing_member (data, classRealTier));
	TimeSoundEditor_init (me, parent, title, data, sound, ownSound);
	my ymin = -1.0;
	RealTierEditor_updateScaling (me);
	my ycursor = 0.382 * my ymin + 0.618 * my ymax;
}

/* End of file RealTierEditor.cpp */
