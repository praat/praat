/* FormantGridEditor.cpp
 *
 * Copyright (C) 2008-2011,2012,2013,2014,2015 Paul Boersma & David Weenink
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

static void menu_cb_removePoints (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	Editor_save (me, U"Remove point(s)");
	FormantGrid grid = (FormantGrid) my data;
	Ordered tiers = my editingBandwidths ? grid -> bandwidths : grid -> formants;
	RealTier tier = (RealTier) tiers -> item [my selectedFormant];
	if (my d_startSelection == my d_endSelection)
		AnyTier_removePointNear (tier, my d_startSelection);
	else
		AnyTier_removePointsBetween (tier, my d_startSelection, my d_endSelection);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPointAtCursor (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	Editor_save (me, U"Add point");
	FormantGrid grid = (FormantGrid) my data;
	Ordered tiers = my editingBandwidths ? grid -> bandwidths : grid -> formants;
	RealTier tier = (RealTier) tiers -> item [my selectedFormant];
	RealTier_addPoint (tier, 0.5 * (my d_startSelection + my d_endSelection), my ycursor);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPointAt (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	EDITOR_FORM (U"Add point", 0)
		REAL (U"Time (s)", U"0.0")
		POSITIVE (U"Frequency (Hz)", U"200.0")
	EDITOR_OK
		SET_REAL (U"Time", 0.5 * (my d_startSelection + my d_endSelection))
		SET_REAL (U"Frequency", my ycursor)
	EDITOR_DO
		Editor_save (me, U"Add point");
		FormantGrid grid = (FormantGrid) my data;
		Ordered tiers = my editingBandwidths ? grid -> bandwidths : grid -> formants;
		RealTier tier = (RealTier) tiers -> item [my selectedFormant];
		RealTier_addPoint (tier, GET_REAL (U"Time"), GET_REAL (U"Frequency"));
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_setFormantRange (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	EDITOR_FORM (U"Set formant range", 0)
		REAL (U"Minimum formant (Hz)", my default_formantFloor   ())
		REAL (U"Maximum formant (Hz)", my default_formantCeiling ())
	EDITOR_OK
		SET_REAL (U"Minimum formant", my p_formantFloor)
		SET_REAL (U"Maximum formant", my p_formantCeiling)
	EDITOR_DO
		my pref_formantFloor   () = my p_formantFloor   = GET_REAL (U"Minimum formant");
		my pref_formantCeiling () = my p_formantCeiling = GET_REAL (U"Maximum formant");
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_setBandwidthRange (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	EDITOR_FORM (U"Set bandwidth range", 0)
		REAL (U"Minimum bandwidth (Hz)", my default_bandwidthFloor   ())
		REAL (U"Maximum bandwidth (Hz)", my default_bandwidthCeiling ())
	EDITOR_OK
		SET_REAL (U"Minimum bandwidth", my p_bandwidthFloor)
		SET_REAL (U"Maximum bandwidth", my p_bandwidthCeiling)
	EDITOR_DO
		my pref_bandwidthFloor   () = my p_bandwidthFloor   = GET_REAL (U"Minimum bandwidth");
		my pref_bandwidthCeiling () = my p_bandwidthCeiling = GET_REAL (U"Maximum bandwidth");
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_showBandwidths (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	my editingBandwidths = ! my editingBandwidths;
	GuiMenuItem_check (my d_bandwidthsToggle, my editingBandwidths);
	FunctionEditor_redraw (me);
}

static void selectFormantOrBandwidth (FormantGridEditor me, long iformant) {
	FormantGrid grid = (FormantGrid) my data;
	long numberOfFormants = grid -> formants -> size;
	if (iformant > numberOfFormants)
		Melder_throw (U"Cannot select formant ", iformant, U", because the FormantGrid has only ", numberOfFormants, U" formants.");
	my selectedFormant = iformant;
	FunctionEditor_redraw (me);
}

static void menu_cb_selectFirst   (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 1); }
static void menu_cb_selectSecond  (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 2); }
static void menu_cb_selectThird   (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 3); }
static void menu_cb_selectFourth  (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 4); }
static void menu_cb_selectFifth   (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 5); }
static void menu_cb_selectSixth   (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 6); }
static void menu_cb_selectSeventh (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 7); }
static void menu_cb_selectEighth  (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 8); }
static void menu_cb_selectNinth   (EDITOR_ARGS) { EDITOR_IAM (FormantGridEditor); selectFormantOrBandwidth (me, 9); }
static void menu_cb_selectFormantOrBandwidth (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	EDITOR_FORM (U"Select formant or bandwidth", 0)
		NATURAL (U"Formant number", U"1")
	EDITOR_OK
		SET_INTEGER (U"Formant number", my selectedFormant)
	EDITOR_DO
		selectFormantOrBandwidth (me, GET_INTEGER (U"Formant number"));
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_pitchSettings (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	EDITOR_FORM (U"Source pitch settings", 0)
		LABEL (U"", U"These settings apply to the pitch curve")
		LABEL (U"", U"that you hear when playing the FormantGrid.")
		REAL     (U"Starting time",       my default_source_pitch_tStart  ())
		POSITIVE (U"Starting pitch (Hz)", my default_source_pitch_f0Start ())
		REAL     (U"Mid time",            my default_source_pitch_tMid    ())
		POSITIVE (U"Mid pitch (Hz)",      my default_source_pitch_f0Mid   ())
		REAL     (U"End time",            my default_source_pitch_tEnd    ())
		POSITIVE (U"End pitch (Hz)",      my default_source_pitch_f0End   ())
	EDITOR_OK
		SET_REAL (U"Starting time",  my p_source_pitch_tStart)
		SET_REAL (U"Starting pitch", my p_source_pitch_f0Start)
		SET_REAL (U"Mid time",       my p_source_pitch_tMid)
		SET_REAL (U"Mid pitch",      my p_source_pitch_f0Mid)
		SET_REAL (U"End time",       my p_source_pitch_tEnd)
		SET_REAL (U"End pitch",      my p_source_pitch_f0End)
	EDITOR_DO
		my pref_source_pitch_tStart  () = my p_source_pitch_tStart  = GET_REAL (U"Starting time");
		my pref_source_pitch_f0Start () = my p_source_pitch_f0Start = GET_REAL (U"Starting pitch");
		my pref_source_pitch_tMid    () = my p_source_pitch_tMid    = GET_REAL (U"Mid time");
		my pref_source_pitch_f0Mid   () = my p_source_pitch_f0Mid   = GET_REAL (U"Mid pitch");
		my pref_source_pitch_tEnd    () = my p_source_pitch_tEnd    = GET_REAL (U"End time");
		my pref_source_pitch_f0End   () = my p_source_pitch_f0End   = GET_REAL (U"End pitch");
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
	EditorMenu_addCommand (menu, U"Remove point(s)", GuiMenu_OPTION + 'T', menu_cb_removePoints);
	if (our v_hasSourceMenu ()) {
		menu = Editor_addMenu (this, U"Source", 0);
		EditorMenu_addCommand (menu, U"Pitch settings...", 0, menu_cb_pitchSettings);
		//EditorMenu_addCommand (menu, U"Phonation settings...", 0, menu_cb_phonationSettings);
	}
}

/********** DRAWING AREA **********/

void structFormantGridEditor :: v_draw () {
	FormantGrid grid = (FormantGrid) our data;
	Ordered tiers = our editingBandwidths ? grid -> bandwidths : grid -> formants;
	RealTier selectedTier = (RealTier) tiers -> item [selectedFormant];
	double ymin = our editingBandwidths ? our p_bandwidthFloor   : our p_formantFloor;
	double ymax = our editingBandwidths ? our p_bandwidthCeiling : our p_formantCeiling;
	Graphics_setColour (our d_graphics, Graphics_WHITE);
	Graphics_setWindow (our d_graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (our d_graphics, 0, 1, 0, 1);
	Graphics_setWindow (our d_graphics, our d_startWindow, our d_endWindow, ymin, ymax);
	Graphics_setColour (our d_graphics, Graphics_RED);
	Graphics_line (our d_graphics, our d_startWindow, our ycursor, our d_endWindow, our ycursor);
	Graphics_setTextAlignment (our d_graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_text (our d_graphics, our d_startWindow, our ycursor, Melder_float (Melder_half (our ycursor)));
	Graphics_setColour (our d_graphics, Graphics_BLUE);
	Graphics_setTextAlignment (our d_graphics, Graphics_LEFT, Graphics_TOP);
	Graphics_text (our d_graphics, our d_endWindow, ymax, Melder_float (Melder_half (ymax)), U" Hz");
	Graphics_setTextAlignment (our d_graphics, Graphics_LEFT, Graphics_HALF);
	Graphics_text (our d_graphics, our d_endWindow, ymin, Melder_float (Melder_half (ymin)), U" Hz");
	Graphics_setLineWidth (our d_graphics, 1);
	Graphics_setColour (our d_graphics, Graphics_GREY);
	for (long iformant = 1; iformant <= grid -> formants -> size; iformant ++) if (iformant != our selectedFormant) {
		RealTier tier = (RealTier) tiers -> item [iformant];
		long imin = AnyTier_timeToHighIndex (tier, our d_startWindow);
		long imax = AnyTier_timeToLowIndex (tier, our d_endWindow);
		long n = tier -> points -> size;
		if (n == 0) {
		} else if (imax < imin) {
			double yleft = RealTier_getValueAtTime (tier, our d_startWindow);
			double yright = RealTier_getValueAtTime (tier, our d_endWindow);
			Graphics_line (our d_graphics, our d_startWindow, yleft, our d_endWindow, yright);
		} else for (long i = imin; i <= imax; i ++) {
			RealPoint point = (RealPoint) tier -> points -> item [i];
			double t = point -> number, y = point -> value;
			Graphics_fillCircle_mm (our d_graphics, t, y, 2);
			if (i == 1)
				Graphics_line (our d_graphics, our d_startWindow, y, t, y);
			else if (i == imin)
				Graphics_line (our d_graphics, t, y, our d_startWindow, RealTier_getValueAtTime (tier, our d_startWindow));
			if (i == n)
				Graphics_line (our d_graphics, t, y, our d_endWindow, y);
			else if (i == imax)
				Graphics_line (our d_graphics, t, y, our d_endWindow, RealTier_getValueAtTime (tier, our d_endWindow));
			else {
				RealPoint pointRight = (RealPoint) tier -> points -> item [i + 1];
				Graphics_line (our d_graphics, t, y, pointRight -> number, pointRight -> value);
			}
		}
	}
	Graphics_setColour (our d_graphics, Graphics_BLUE);
	long ifirstSelected = AnyTier_timeToHighIndex (selectedTier, our d_startSelection);
	long ilastSelected = AnyTier_timeToLowIndex (selectedTier, our d_endSelection);
	long n = selectedTier -> points -> size;
	long imin = AnyTier_timeToHighIndex (selectedTier, our d_startWindow);
	long imax = AnyTier_timeToLowIndex (selectedTier, our d_endWindow);
	Graphics_setLineWidth (our d_graphics, 2);
	if (n == 0) {
		Graphics_setTextAlignment (our d_graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (our d_graphics, 0.5 * (our d_startWindow + our d_endWindow),
			0.5 * (ymin + ymax), U"(no points in selected formant tier)");
	} else if (imax < imin) {
		double yleft = RealTier_getValueAtTime (selectedTier, our d_startWindow);
		double yright = RealTier_getValueAtTime (selectedTier, our d_endWindow);
		Graphics_line (our d_graphics, our d_startWindow, yleft, our d_endWindow, yright);
	} else for (long i = imin; i <= imax; i ++) {
		RealPoint point = (RealPoint) selectedTier -> points -> item [i];
		double t = point -> number, y = point -> value;
		if (i >= ifirstSelected && i <= ilastSelected)
			Graphics_setColour (our d_graphics, Graphics_RED);
		Graphics_fillCircle_mm (our d_graphics, t, y, 3);
		Graphics_setColour (our d_graphics, Graphics_BLUE);
		if (i == 1)
			Graphics_line (our d_graphics, our d_startWindow, y, t, y);
		else if (i == imin)
			Graphics_line (our d_graphics, t, y, our d_startWindow, RealTier_getValueAtTime (selectedTier, our d_startWindow));
		if (i == n)
			Graphics_line (our d_graphics, t, y, our d_endWindow, y);
		else if (i == imax)
			Graphics_line (our d_graphics, t, y, our d_endWindow, RealTier_getValueAtTime (selectedTier, our d_endWindow));
		else {
			RealPoint pointRight = (RealPoint) selectedTier -> points -> item [i + 1];
			Graphics_line (our d_graphics, t, y, pointRight -> number, pointRight -> value);
		}
	}
	Graphics_setLineWidth (our d_graphics, 1);
	Graphics_setColour (our d_graphics, Graphics_BLACK);
}

static void drawWhileDragging (FormantGridEditor me, double xWC, double yWC, long first, long last, double dt, double dy) {
	FormantGrid grid = (FormantGrid) my data;
	Ordered tiers = my editingBandwidths ? grid -> bandwidths : grid -> formants;
	RealTier tier = (RealTier) tiers -> item [my selectedFormant];
	double ymin = my editingBandwidths ? my p_bandwidthFloor   : my p_formantFloor;
	double ymax = my editingBandwidths ? my p_bandwidthCeiling : my p_formantCeiling;
	(void) xWC;
	(void) yWC;

	/*
	 * Draw all selected points as magenta empty circles, if inside the window.
	 */
	for (long i = first; i <= last; i ++) {
		RealPoint point = (RealPoint) tier -> points -> item [i];
		double t = point -> number + dt, y = point -> value + dy;
		if (t >= my d_startWindow && t <= my d_endWindow)
			Graphics_circle_mm (my d_graphics, t, y, 3);
	}

	if (last == first) {
		/*
		 * Draw a crosshair with time and y.
		 */
		RealPoint point = (RealPoint) tier -> points -> item [first];
		double t = point -> number + dt, y = point -> value + dy;
		Graphics_line (my d_graphics, t, ymin, t, ymax - Graphics_dyMMtoWC (my d_graphics, 4.0));
		Graphics_setTextAlignment (my d_graphics, kGraphics_horizontalAlignment_CENTRE, Graphics_TOP);
		Graphics_text (my d_graphics, t, ymax, Melder_fixed (t, 6));
		Graphics_line (my d_graphics, my d_startWindow, y, my d_endWindow, y);
		Graphics_setTextAlignment (my d_graphics, Graphics_LEFT, Graphics_BOTTOM);
		Graphics_text (my d_graphics, my d_startWindow, y, Melder_fixed (y, 6));
	}
}

bool structFormantGridEditor :: v_click (double xWC, double yWC, bool shiftKeyPressed) {
	FormantGrid grid = (FormantGrid) our data;
	Ordered tiers = editingBandwidths ? grid -> bandwidths : grid -> formants;
	RealTier tier = (RealTier) tiers -> item [selectedFormant];
	double ymin = our editingBandwidths ? our p_bandwidthFloor   : our p_formantFloor;
	double ymax = our editingBandwidths ? our p_bandwidthCeiling : our p_formantCeiling;
	long inearestPoint, ifirstSelected, ilastSelected;
	RealPoint nearestPoint;
	double dt = 0, df = 0;
	bool draggingSelection;

	/*
	 * Perform the default action: move cursor.
	 */
	//d_startSelection = d_endSelection = xWC;
	our ycursor = (1.0 - yWC) * ymin + yWC * ymax;
	Graphics_setWindow (our d_graphics, our d_startWindow, our d_endWindow, ymin, ymax);
	yWC = our ycursor;

	/*
	 * Clicked on a point?
	 */
	inearestPoint = AnyTier_timeToNearestIndex (tier, xWC);
	if (inearestPoint == 0) {
		return FormantGridEditor_Parent :: v_click (xWC, yWC, shiftKeyPressed);
	}
	nearestPoint = (RealPoint) tier -> points -> item [inearestPoint];
	if (Graphics_distanceWCtoMM (our d_graphics, xWC, yWC, nearestPoint -> number, nearestPoint -> value) > 1.5) {
		return our FormantGridEditor_Parent :: v_click (xWC, yWC, shiftKeyPressed);
	}

	/*
	 * Clicked on a selected point?
	 */
	draggingSelection = shiftKeyPressed &&
		nearestPoint -> number > our d_startSelection && nearestPoint -> number < our d_endSelection;
	if (draggingSelection) {
		ifirstSelected = AnyTier_timeToHighIndex (tier, our d_startSelection);
		ilastSelected = AnyTier_timeToLowIndex (tier, our d_endSelection);
		Editor_save (this, U"Drag points");
	} else {
		ifirstSelected = ilastSelected = inearestPoint;
		Editor_save (this, U"Drag point");
	}

	/*
	 * Drag.
	 */
	Graphics_xorOn (our d_graphics, Graphics_MAROON);
	drawWhileDragging (this, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
	while (Graphics_mouseStillDown (our d_graphics)) {
		double xWC_new, yWC_new;
		Graphics_getMouseLocation (our d_graphics, & xWC_new, & yWC_new);
		if (xWC_new != xWC || yWC_new != yWC) {
			drawWhileDragging (this, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
			dt += xWC_new - xWC, df += yWC_new - yWC;
			xWC = xWC_new, yWC = yWC_new;
			drawWhileDragging (this, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
		}
	}
	Graphics_xorOff (our d_graphics);

	/*
	 * Dragged inside window?
	 */
	if (xWC < d_startWindow || xWC > d_endWindow) return 1;

	/*
	 * Points not dragged past neighbours?
	 */
	RealPoint *points = (RealPoint *) tier -> points -> item;
	double newTime = points [ifirstSelected] -> number + dt;
	if (newTime < our tmin) return 1;   // outside domain
	if (ifirstSelected > 1 && newTime <= points [ifirstSelected - 1] -> number)
		return 1;   // past left neighbour
	newTime = points [ilastSelected] -> number + dt;
	if (newTime > our tmax) return 1;   // outside domain
	if (ilastSelected < tier -> points -> size && newTime >= points [ilastSelected + 1] -> number)
		return FunctionEditor_UPDATE_NEEDED;   // past right neighbour

	/*
	 * Drop.
	 */
	for (long i = ifirstSelected; i <= ilastSelected; i ++) {
		RealPoint point = (RealPoint) tier -> points -> item [i];
		point -> number += dt;
		point -> value += df;
	}

	/*
	 * Make sure that the same points are still selected (a problem with Undo...).
	 */

	if (draggingSelection) our d_startSelection += dt, our d_endSelection += dt;
	if (ifirstSelected == ilastSelected) {
		/*
		 * Move crosshair to only selected formant point.
		 */
		RealPoint point = (RealPoint) tier -> points -> item [ifirstSelected];
		our d_startSelection = our d_endSelection = point -> number;
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
	Melder_assert (data != nullptr);
	Melder_assert (Thing_isa (data, classFormantGrid));
	FunctionEditor_init (me, title, data);
	my ycursor = 0.382 * my p_formantFloor + 0.618 * my p_formantCeiling;
	my selectedFormant = 1;
}

autoFormantGridEditor FormantGridEditor_create (const char32 *title, FormantGrid data) {
	try {
		autoFormantGridEditor me = Thing_new (FormantGridEditor);
		FormantGridEditor_init (me.peek(), title, data);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FormantGrid window not created.");
	}
}

/* End of file FormantGridEditor.cpp */
