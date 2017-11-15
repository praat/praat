/* FormantGridEditor.cpp
 *
 * Copyright (C) 2008-2011,2012,2013,2014,2015,2016,2017 Paul Boersma & David Weenink
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

#include "FormantGridEditor.h"
#include "EditorM.h"
#include "PointProcess_and_Sound.h"

Thing_implement (FormantGridEditor, FunctionEditor, 0);

#include "prefs_define.h"
#include "FormantGridEditor_prefs.h"
#include "prefs_install.h"
#include "FormantGridEditor_prefs.h"
#include "prefs_copyToInstance.h"
#include "FormantGridEditor_prefs.h"

/********** MENU COMMANDS **********/

static void menu_cb_removePoints (FormantGridEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Remove point(s)");
	FormantGrid grid = (FormantGrid) my data;
	OrderedOf<structRealTier>* tiers = ( my editingBandwidths ? & grid -> bandwidths : & grid -> formants );
	RealTier tier = tiers->at [my selectedFormant];
	if (my startSelection == my endSelection)
		AnyTier_removePointNear (tier->asAnyTier(), my startSelection);
	else
		AnyTier_removePointsBetween (tier->asAnyTier(), my startSelection, my endSelection);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPointAtCursor (FormantGridEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Add point");
	FormantGrid grid = (FormantGrid) my data;
	OrderedOf<structRealTier>* tiers = ( my editingBandwidths ? & grid -> bandwidths : & grid -> formants );
	RealTier tier = tiers->at [my selectedFormant];
	RealTier_addPoint (tier, 0.5 * (my startSelection + my endSelection), my ycursor);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPointAt (FormantGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add point", nullptr)
		REAL (time, U"Time (s)", U"0.0")
		POSITIVE (frequency, U"Frequency (Hz)", U"200.0")
	EDITOR_OK
		SET_REAL (time, 0.5 * (my startSelection + my endSelection))
		SET_REAL (frequency, my ycursor)
	EDITOR_DO
		Editor_save (me, U"Add point");
		FormantGrid grid = (FormantGrid) my data;
		OrderedOf<structRealTier>* tiers = ( my editingBandwidths ? & grid -> bandwidths : & grid -> formants );
		RealTier tier = tiers->at [my selectedFormant];
		RealTier_addPoint (tier, time, frequency);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_setFormantRange (FormantGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Set formant range", nullptr)
		REAL (minimumFormant, U"Minimum formant (Hz)", my default_formantFloor   ())
		REAL (maximumFormant, U"Maximum formant (Hz)", my default_formantCeiling ())
	EDITOR_OK
		SET_REAL (minimumFormant, my p_formantFloor)
		SET_REAL (maximumFormant, my p_formantCeiling)
	EDITOR_DO
		my pref_formantFloor   () = my p_formantFloor   = minimumFormant;
		my pref_formantCeiling () = my p_formantCeiling = maximumFormant;
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_setBandwidthRange (FormantGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Set bandwidth range", nullptr)
		REAL (minimumBandwidth, U"Minimum bandwidth (Hz)", my default_bandwidthFloor   ())
		REAL (maximumBandwidth, U"Maximum bandwidth (Hz)", my default_bandwidthCeiling ())
	EDITOR_OK
		SET_REAL (minimumBandwidth, my p_bandwidthFloor)
		SET_REAL (maximumBandwidth, my p_bandwidthCeiling)
	EDITOR_DO
		my pref_bandwidthFloor   () = my p_bandwidthFloor   = minimumBandwidth;
		my pref_bandwidthCeiling () = my p_bandwidthCeiling = maximumBandwidth;
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_showBandwidths (FormantGridEditor me, EDITOR_ARGS_DIRECT) {
	my editingBandwidths = ! my editingBandwidths;
	GuiMenuItem_check (my d_bandwidthsToggle, my editingBandwidths);
	FunctionEditor_redraw (me);
}

static void selectFormantOrBandwidth (FormantGridEditor me, integer iformant) {
	FormantGrid grid = (FormantGrid) my data;
	integer numberOfFormants = grid -> formants.size;
	if (iformant > numberOfFormants)
		Melder_throw (U"Cannot select formant ", iformant, U", because the FormantGrid has only ", numberOfFormants, U" formants.");
	my selectedFormant = iformant;
	FunctionEditor_redraw (me);
}

static void menu_cb_selectFirst   (FormantGridEditor me, EDITOR_ARGS_DIRECT) { selectFormantOrBandwidth (me, 1); }
static void menu_cb_selectSecond  (FormantGridEditor me, EDITOR_ARGS_DIRECT) { selectFormantOrBandwidth (me, 2); }
static void menu_cb_selectThird   (FormantGridEditor me, EDITOR_ARGS_DIRECT) { selectFormantOrBandwidth (me, 3); }
static void menu_cb_selectFourth  (FormantGridEditor me, EDITOR_ARGS_DIRECT) { selectFormantOrBandwidth (me, 4); }
static void menu_cb_selectFifth   (FormantGridEditor me, EDITOR_ARGS_DIRECT) { selectFormantOrBandwidth (me, 5); }
static void menu_cb_selectSixth   (FormantGridEditor me, EDITOR_ARGS_DIRECT) { selectFormantOrBandwidth (me, 6); }
static void menu_cb_selectSeventh (FormantGridEditor me, EDITOR_ARGS_DIRECT) { selectFormantOrBandwidth (me, 7); }
static void menu_cb_selectEighth  (FormantGridEditor me, EDITOR_ARGS_DIRECT) { selectFormantOrBandwidth (me, 8); }
static void menu_cb_selectNinth   (FormantGridEditor me, EDITOR_ARGS_DIRECT) { selectFormantOrBandwidth (me, 9); }
static void menu_cb_selectFormantOrBandwidth (FormantGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Select formant or bandwidth", nullptr)
		NATURAL (formantNumber, U"Formant number", U"1")
	EDITOR_OK
		SET_INTEGER (formantNumber, my selectedFormant)
	EDITOR_DO
		selectFormantOrBandwidth (me, formantNumber);
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_pitchSettings (FormantGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Source pitch settings", nullptr)
		LABEL (U"These settings apply to the pitch curve")
		LABEL (U"that you hear when playing the FormantGrid.")
		REAL     (startTime,  U"Start time",       my default_source_pitch_tStart  ())
		POSITIVE (startPitch, U"Start pitch (Hz)", my default_source_pitch_f0Start ())
		REAL     (midTime,    U"Mid time",         my default_source_pitch_tMid    ())
		POSITIVE (midPitch,   U"Mid pitch (Hz)",   my default_source_pitch_f0Mid   ())
		REAL     (endTime,    U"End time",         my default_source_pitch_tEnd    ())
		POSITIVE (endPitch,   U"End pitch (Hz)",   my default_source_pitch_f0End   ())
	EDITOR_OK
		SET_REAL (startTime,  my p_source_pitch_tStart)
		SET_REAL (startPitch, my p_source_pitch_f0Start)
		SET_REAL (midTime,    my p_source_pitch_tMid)
		SET_REAL (midPitch,   my p_source_pitch_f0Mid)
		SET_REAL (endTime,    my p_source_pitch_tEnd)
		SET_REAL (endPitch,   my p_source_pitch_f0End)
	EDITOR_DO
		my pref_source_pitch_tStart  () = my p_source_pitch_tStart  = startTime;
		my pref_source_pitch_f0Start () = my p_source_pitch_f0Start = startPitch;
		my pref_source_pitch_tMid    () = my p_source_pitch_tMid    = midTime;
		my pref_source_pitch_f0Mid   () = my p_source_pitch_f0Mid   = midPitch;
		my pref_source_pitch_tEnd    () = my p_source_pitch_tEnd    = endTime;
		my pref_source_pitch_f0End   () = my p_source_pitch_f0End   = endPitch;
	EDITOR_END
}

void structFormantGridEditor :: v_createMenus () {
	FormantGridEditor_Parent :: v_createMenus ();
	EditorMenu menu = Editor_addMenu (this, U"Formant", 0);
	our d_bandwidthsToggle = EditorMenu_addCommand (menu, U"Show bandwidths", GuiMenu_CHECKBUTTON, menu_cb_showBandwidths);
	EditorMenu_addCommand (menu, U"Set formant range...", 0, menu_cb_setFormantRange);
	EditorMenu_addCommand (menu, U"Set bandwidth range...", 0, menu_cb_setBandwidthRange);
	EditorMenu_addCommand (menu, U"-- select formant --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Select first", '1', menu_cb_selectFirst);
	EditorMenu_addCommand (menu, U"Select second", '2', menu_cb_selectSecond);
	EditorMenu_addCommand (menu, U"Select third", '3', menu_cb_selectThird);
	EditorMenu_addCommand (menu, U"Select fourth", '4', menu_cb_selectFourth);
	EditorMenu_addCommand (menu, U"Select fifth", '5', menu_cb_selectFifth);
	EditorMenu_addCommand (menu, U"Select sixth", '6', menu_cb_selectSixth);
	EditorMenu_addCommand (menu, U"Select seventh", '7', menu_cb_selectSeventh);
	EditorMenu_addCommand (menu, U"Select eighth", '8', menu_cb_selectEighth);
	EditorMenu_addCommand (menu, U"Select ninth", '9', menu_cb_selectNinth);
	EditorMenu_addCommand (menu, U"Select formant or bandwidth...", 0, menu_cb_selectFormantOrBandwidth);
	menu = Editor_addMenu (this, U"Point", 0);
	EditorMenu_addCommand (menu, U"Add point at cursor", 'T', menu_cb_addPointAtCursor);
	EditorMenu_addCommand (menu, U"Add point at...", 0, menu_cb_addPointAt);
	EditorMenu_addCommand (menu, U"-- remove point --", 0, nullptr);
	EditorMenu_addCommand (menu, U"Remove point(s)", GuiMenu_OPTION | 'T', menu_cb_removePoints);
	if (our v_hasSourceMenu ()) {
		menu = Editor_addMenu (this, U"Source", 0);
		EditorMenu_addCommand (menu, U"Pitch settings...", 0, menu_cb_pitchSettings);
		//EditorMenu_addCommand (menu, U"Phonation settings...", 0, menu_cb_phonationSettings);
	}
}

/********** DRAWING AREA **********/

void structFormantGridEditor :: v_draw () {
	FormantGrid grid = (FormantGrid) our data;
	OrderedOf<structRealTier>* tiers = ( our editingBandwidths ? & grid -> bandwidths : & grid -> formants );
	RealTier selectedTier = tiers->at [our selectedFormant];
	double ymin = our editingBandwidths ? our p_bandwidthFloor   : our p_formantFloor;
	double ymax = our editingBandwidths ? our p_bandwidthCeiling : our p_formantCeiling;
	Graphics_setColour (our graphics.get(), Graphics_WHITE);
	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, ymin, ymax);
	Graphics_setColour (our graphics.get(), Graphics_RED);
	Graphics_line (our graphics.get(), our startWindow, our ycursor, our endWindow, our ycursor);
	Graphics_setTextAlignment (our graphics.get(), Graphics_RIGHT, Graphics_HALF);
	Graphics_text (our graphics.get(), our startWindow, our ycursor, Melder_float (Melder_half (our ycursor)));
	Graphics_setColour (our graphics.get(), Graphics_BLUE);
	Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT, Graphics_TOP);
	Graphics_text (our graphics.get(), our endWindow, ymax, Melder_float (Melder_half (ymax)), U" Hz");
	Graphics_setTextAlignment (our graphics.get(), Graphics_LEFT, Graphics_HALF);
	Graphics_text (our graphics.get(), our endWindow, ymin, Melder_float (Melder_half (ymin)), U" Hz");
	Graphics_setLineWidth (our graphics.get(), 1.0);
	Graphics_setColour (our graphics.get(), Graphics_GREY);
	for (integer iformant = 1; iformant <= grid -> formants.size; iformant ++) if (iformant != our selectedFormant) {
		RealTier tier = tiers->at [iformant];
		integer imin = AnyTier_timeToHighIndex (tier->asAnyTier(), our startWindow);
		integer imax = AnyTier_timeToLowIndex (tier->asAnyTier(), our endWindow);
		integer n = tier -> points.size;
		if (n == 0) {
		} else if (imax < imin) {
			double yleft = RealTier_getValueAtTime (tier, our startWindow);
			double yright = RealTier_getValueAtTime (tier, our endWindow);
			Graphics_line (our graphics.get(), our startWindow, yleft, our endWindow, yright);
		} else for (integer i = imin; i <= imax; i ++) {
			RealPoint point = tier -> points.at [i];
			double t = point -> number, y = point -> value;
			Graphics_fillCircle_mm (our graphics.get(), t, y, 2.0);
			if (i == 1)
				Graphics_line (our graphics.get(), our startWindow, y, t, y);
			else if (i == imin)
				Graphics_line (our graphics.get(), t, y, our startWindow, RealTier_getValueAtTime (tier, our startWindow));
			if (i == n)
				Graphics_line (our graphics.get(), t, y, our endWindow, y);
			else if (i == imax)
				Graphics_line (our graphics.get(), t, y, our endWindow, RealTier_getValueAtTime (tier, our endWindow));
			else {
				RealPoint pointRight = tier -> points.at [i + 1];
				Graphics_line (our graphics.get(), t, y, pointRight -> number, pointRight -> value);
			}
		}
	}
	Graphics_setColour (our graphics.get(), Graphics_BLUE);
	integer ifirstSelected = AnyTier_timeToHighIndex (selectedTier->asAnyTier(), our startSelection);
	integer ilastSelected = AnyTier_timeToLowIndex (selectedTier->asAnyTier(), our endSelection);
	integer n = selectedTier -> points.size;
	integer imin = AnyTier_timeToHighIndex (selectedTier->asAnyTier(), our startWindow);
	integer imax = AnyTier_timeToLowIndex (selectedTier->asAnyTier(), our endWindow);
	Graphics_setLineWidth (our graphics.get(), 2.0);
	if (n == 0) {
		Graphics_setTextAlignment (our graphics.get(), Graphics_CENTRE, Graphics_HALF);
		Graphics_text (our graphics.get(), 0.5 * (our startWindow + our endWindow),
			0.5 * (ymin + ymax), U"(no points in selected formant tier)");
	} else if (imax < imin) {
		double yleft = RealTier_getValueAtTime (selectedTier, our startWindow);
		double yright = RealTier_getValueAtTime (selectedTier, our endWindow);
		Graphics_line (our graphics.get(), our startWindow, yleft, our endWindow, yright);
	} else for (integer i = imin; i <= imax; i ++) {
		RealPoint point = selectedTier -> points.at [i];
		double t = point -> number, y = point -> value;
		if (i >= ifirstSelected && i <= ilastSelected)
			Graphics_setColour (our graphics.get(), Graphics_RED);
		Graphics_fillCircle_mm (our graphics.get(), t, y, 3);
		Graphics_setColour (our graphics.get(), Graphics_BLUE);
		if (i == 1)
			Graphics_line (our graphics.get(), our startWindow, y, t, y);
		else if (i == imin)
			Graphics_line (our graphics.get(), t, y, our startWindow, RealTier_getValueAtTime (selectedTier, our startWindow));
		if (i == n)
			Graphics_line (our graphics.get(), t, y, our endWindow, y);
		else if (i == imax)
			Graphics_line (our graphics.get(), t, y, our endWindow, RealTier_getValueAtTime (selectedTier, our endWindow));
		else {
			RealPoint pointRight = selectedTier -> points.at [i + 1];
			Graphics_line (our graphics.get(), t, y, pointRight -> number, pointRight -> value);
		}
	}
	Graphics_setLineWidth (our graphics.get(), 1.0);
	Graphics_setColour (our graphics.get(), Graphics_BLACK);
}

static void drawWhileDragging (FormantGridEditor me, double /* xWC */, double /* yWC */, integer first, integer last, double dt, double dy) {
	FormantGrid grid = (FormantGrid) my data;
	OrderedOf<structRealTier>* tiers = my editingBandwidths ? & grid -> bandwidths : & grid -> formants;
	RealTier tier = tiers->at [my selectedFormant];
	double ymin = my editingBandwidths ? my p_bandwidthFloor   : my p_formantFloor;
	double ymax = my editingBandwidths ? my p_bandwidthCeiling : my p_formantCeiling;

	/*
	 * Draw all selected points as magenta empty circles, if inside the window.
	 */
	for (integer i = first; i <= last; i ++) {
		RealPoint point = tier -> points.at [i];
		double t = point -> number + dt, y = point -> value + dy;
		if (t >= my startWindow && t <= my endWindow)
			Graphics_circle_mm (my graphics.get(), t, y, 3.0);
	}

	if (last == first) {
		/*
		 * Draw a crosshair with time and y.
		 */
		RealPoint point = tier -> points.at [first];
		double t = point -> number + dt, y = point -> value + dy;
		Graphics_line (my graphics.get(), t, ymin, t, ymax - Graphics_dyMMtoWC (my graphics.get(), 4.0));
		Graphics_setTextAlignment (my graphics.get(), kGraphics_horizontalAlignment::CENTRE, Graphics_TOP);
		Graphics_text (my graphics.get(), t, ymax, Melder_fixed (t, 6));
		Graphics_line (my graphics.get(), my startWindow, y, my endWindow, y);
		Graphics_setTextAlignment (my graphics.get(), Graphics_LEFT, Graphics_BOTTOM);
		Graphics_text (my graphics.get(), my startWindow, y, Melder_fixed (y, 6));
	}
}

bool structFormantGridEditor :: v_click (double xWC, double yWC, bool shiftKeyPressed) {
	FormantGrid grid = (FormantGrid) our data;
	OrderedOf<structRealTier>* tiers = our editingBandwidths ? & grid -> bandwidths : & grid -> formants;
	RealTier tier = tiers->at [selectedFormant];
	double ymin = our editingBandwidths ? our p_bandwidthFloor   : our p_formantFloor;
	double ymax = our editingBandwidths ? our p_bandwidthCeiling : our p_formantCeiling;
	integer inearestPoint, ifirstSelected, ilastSelected;
	RealPoint nearestPoint;
	double dt = 0, df = 0;
	bool draggingSelection;

	/*
	 * Perform the default action: move cursor.
	 */
	//our startSelection = our endSelection = xWC;
	our ycursor = (1.0 - yWC) * ymin + yWC * ymax;
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, ymin, ymax);
	yWC = our ycursor;

	/*
	 * Clicked on a point?
	 */
	inearestPoint = AnyTier_timeToNearestIndex (tier->asAnyTier(), xWC);
	if (inearestPoint == 0) {
		return FormantGridEditor_Parent :: v_click (xWC, yWC, shiftKeyPressed);
	}
	nearestPoint = tier -> points.at [inearestPoint];
	if (Graphics_distanceWCtoMM (our graphics.get(), xWC, yWC, nearestPoint -> number, nearestPoint -> value) > 1.5) {
		return our FormantGridEditor_Parent :: v_click (xWC, yWC, shiftKeyPressed);
	}

	/*
	 * Clicked on a selected point?
	 */
	draggingSelection = shiftKeyPressed &&
		nearestPoint -> number > our startSelection && nearestPoint -> number < our endSelection;
	if (draggingSelection) {
		ifirstSelected = AnyTier_timeToHighIndex (tier->asAnyTier(), our startSelection);
		ilastSelected = AnyTier_timeToLowIndex (tier->asAnyTier(), our endSelection);
		Editor_save (this, U"Drag points");
	} else {
		ifirstSelected = ilastSelected = inearestPoint;
		Editor_save (this, U"Drag point");
	}

	/*
	 * Drag.
	 */
	Graphics_xorOn (our graphics.get(), Graphics_MAROON);
	drawWhileDragging (this, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
	while (Graphics_mouseStillDown (our graphics.get())) {
		double xWC_new, yWC_new;
		Graphics_getMouseLocation (our graphics.get(), & xWC_new, & yWC_new);
		if (xWC_new != xWC || yWC_new != yWC) {
			drawWhileDragging (this, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
			dt += xWC_new - xWC, df += yWC_new - yWC;
			xWC = xWC_new, yWC = yWC_new;
			drawWhileDragging (this, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
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
	double newTime = tier -> points.at [ifirstSelected] -> number + dt;
	if (newTime < our tmin) return 1;   // outside domain
	if (ifirstSelected > 1 && newTime <= tier -> points.at [ifirstSelected - 1] -> number)
		return 1;   // past left neighbour
	newTime = tier -> points.at [ilastSelected] -> number + dt;
	if (newTime > our tmax) return 1;   // outside domain
	if (ilastSelected < tier -> points.size && newTime >= tier -> points.at [ilastSelected + 1] -> number)
		return FunctionEditor_UPDATE_NEEDED;   // past right neighbour

	/*
	 * Drop.
	 */
	for (integer i = ifirstSelected; i <= ilastSelected; i ++) {
		RealPoint point = tier -> points.at [i];
		point -> number += dt;
		point -> value += df;
	}

	/*
	 * Make sure that the same points are still selected (a problem with Undo...).
	 */

	if (draggingSelection) our startSelection += dt, our endSelection += dt;
	if (ifirstSelected == ilastSelected) {
		/*
		 * Move crosshair to only selected formant point.
		 */
		RealPoint point = tier -> points.at [ifirstSelected];
		our startSelection = our endSelection = point -> number;
		our ycursor = point -> value;
	} else {
		/*
		 * Move crosshair to mouse location.
		 */
		/*our cursor += dt;*/
		our ycursor += df;
	}

	Editor_broadcastDataChanged (this);
	return FunctionEditor_UPDATE_NEEDED;
}

void structFormantGridEditor :: v_play (double tmin, double tmax) {
	FormantGrid_playPart ((FormantGrid) our data, tmin, tmax, our p_play_samplingFrequency,
		our p_source_pitch_tStart, our p_source_pitch_f0Start,
		our p_source_pitch_tMid,   our p_source_pitch_f0Mid,
		our p_source_pitch_tEnd,   our p_source_pitch_f0End,
		our p_source_phonation_adaptFactor, our p_source_phonation_maximumPeriod,
		our p_source_phonation_openPhase,   our p_source_phonation_collisionPhase,
		our p_source_phonation_power1,      our p_source_phonation_power2,
		theFunctionEditor_playCallback, this);
}

void FormantGridEditor_init (FormantGridEditor me, const char32 *title, FormantGrid data) {
	Melder_assert (data);
	Melder_assert (Thing_isa (data, classFormantGrid));
	FunctionEditor_init (me, title, data);
	my ycursor = 0.382 * my p_formantFloor + 0.618 * my p_formantCeiling;
	my selectedFormant = 1;
}

autoFormantGridEditor FormantGridEditor_create (const char32 *title, FormantGrid data) {
	try {
		autoFormantGridEditor me = Thing_new (FormantGridEditor);
		FormantGridEditor_init (me.get(), title, data);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FormantGrid window not created.");
	}
}

/* End of file FormantGridEditor.cpp */
