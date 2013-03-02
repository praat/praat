/* FormantGridEditor.cpp
 *
 * Copyright (C) 2008-2011,2012,2013 Paul Boersma & David Weenink
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
	Editor_save (me, L"Remove point(s)");
	FormantGrid grid = (FormantGrid) my data;
	Ordered tiers = my editingBandwidths ? grid -> bandwidths : grid -> formants;
	RealTier tier = (RealTier) tiers -> item [my selectedFormant];
	if (my d_startSelection == my d_endSelection)
		AnyTier_removePointNear (tier, my d_startSelection);
	else
		AnyTier_removePointsBetween (tier, my d_startSelection, my d_endSelection);
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_addPointAtCursor (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	Editor_save (me, L"Add point");
	FormantGrid grid = (FormantGrid) my data;
	Ordered tiers = my editingBandwidths ? grid -> bandwidths : grid -> formants;
	RealTier tier = (RealTier) tiers -> item [my selectedFormant];
	RealTier_addPoint (tier, 0.5 * (my d_startSelection + my d_endSelection), my ycursor);
	FunctionEditor_redraw (me);
	my broadcastDataChanged ();
}

static void menu_cb_addPointAt (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	EDITOR_FORM (L"Add point", 0)
		REAL (L"Time (s)", L"0.0")
		POSITIVE (L"Frequency (Hz)", L"200.0")
	EDITOR_OK
		SET_REAL (L"Time", 0.5 * (my d_startSelection + my d_endSelection))
		SET_REAL (L"Frequency", my ycursor)
	EDITOR_DO
		Editor_save (me, L"Add point");
		FormantGrid grid = (FormantGrid) my data;
		Ordered tiers = my editingBandwidths ? grid -> bandwidths : grid -> formants;
		RealTier tier = (RealTier) tiers -> item [my selectedFormant];
		RealTier_addPoint (tier, GET_REAL (L"Time"), GET_REAL (L"Frequency"));
		FunctionEditor_redraw (me);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_setFormantRange (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	EDITOR_FORM (L"Set formant range", 0)
		REAL (L"Minimum formant (Hz)", my default_formantFloor   ())
		REAL (L"Maximum formant (Hz)", my default_formantCeiling ())
	EDITOR_OK
		SET_REAL (L"Minimum formant", my p_formantFloor)
		SET_REAL (L"Maximum formant", my p_formantCeiling)
	EDITOR_DO
		my pref_formantFloor   () = my p_formantFloor   = GET_REAL (L"Minimum formant");
		my pref_formantCeiling () = my p_formantCeiling = GET_REAL (L"Maximum formant");
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_setBandwidthRange (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	EDITOR_FORM (L"Set bandwidth range", 0)
		REAL (L"Minimum bandwidth (Hz)", my default_bandwidthFloor   ())
		REAL (L"Maximum bandwidth (Hz)", my default_bandwidthCeiling ())
	EDITOR_OK
		SET_REAL (L"Minimum bandwidth", my p_bandwidthFloor)
		SET_REAL (L"Maximum bandwidth", my p_bandwidthCeiling)
	EDITOR_DO
		my pref_bandwidthFloor   () = my p_bandwidthFloor   = GET_REAL (L"Minimum bandwidth");
		my pref_bandwidthCeiling () = my p_bandwidthCeiling = GET_REAL (L"Maximum bandwidth");
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_showBandwidths (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	my editingBandwidths = ! my editingBandwidths;
	FunctionEditor_redraw (me);
}

static void selectFormantOrBandwidth (FormantGridEditor me, long iformant) {
	FormantGrid grid = (FormantGrid) my data;
	long numberOfFormants = grid -> formants -> size;
	if (iformant > numberOfFormants)
		Melder_throw ("Cannot select formant ", iformant, ", because the FormantGrid has only ", numberOfFormants, " formants.");
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
	EDITOR_FORM (L"Select formant or bandwidth", 0)
		NATURAL (L"Formant number", L"1")
	EDITOR_OK
		SET_INTEGER (L"Formant number", my selectedFormant)
	EDITOR_DO
		selectFormantOrBandwidth (me, GET_INTEGER (L"Formant number"));
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_pitchSettings (EDITOR_ARGS) {
	EDITOR_IAM (FormantGridEditor);
	EDITOR_FORM (L"Source pitch settings", 0)
		LABEL (L"", L"These settings apply to the pitch curve")
		LABEL (L"", L"that you hear when playing the FormantGrid.")
		REAL     (L"Starting time",       my default_source_pitch_tStart  ())
		POSITIVE (L"Starting pitch (Hz)", my default_source_pitch_f0Start ())
		REAL     (L"Mid time",            my default_source_pitch_tMid    ())
		POSITIVE (L"Mid pitch (Hz)",      my default_source_pitch_f0Mid   ())
		REAL     (L"End time",            my default_source_pitch_tEnd    ())
		POSITIVE (L"End pitch (Hz)",      my default_source_pitch_f0End   ())
	EDITOR_OK
		SET_REAL (L"Starting time",  my p_source_pitch_tStart)
		SET_REAL (L"Starting pitch", my p_source_pitch_f0Start)
		SET_REAL (L"Mid time",       my p_source_pitch_tMid)
		SET_REAL (L"Mid pitch",      my p_source_pitch_f0Mid)
		SET_REAL (L"End time",       my p_source_pitch_tEnd)
		SET_REAL (L"End pitch",      my p_source_pitch_f0End)
	EDITOR_DO
		my pref_source_pitch_tStart  () = my p_source_pitch_tStart  = GET_REAL (L"Starting time");
		my pref_source_pitch_f0Start () = my p_source_pitch_f0Start = GET_REAL (L"Starting pitch");
		my pref_source_pitch_tMid    () = my p_source_pitch_tMid    = GET_REAL (L"Mid time");
		my pref_source_pitch_f0Mid   () = my p_source_pitch_f0Mid   = GET_REAL (L"Mid pitch");
		my pref_source_pitch_tEnd    () = my p_source_pitch_tEnd    = GET_REAL (L"End time");
		my pref_source_pitch_f0End   () = my p_source_pitch_f0End   = GET_REAL (L"End pitch");
	EDITOR_END
}

void structFormantGridEditor :: v_createMenus () {
	FormantGridEditor_Parent :: v_createMenus ();
	EditorMenu menu = Editor_addMenu (this, L"Formant", 0);
	EditorMenu_addCommand (menu, L"Show bandwidths", GuiMenu_CHECKBUTTON + 'B', menu_cb_showBandwidths);
	EditorMenu_addCommand (menu, L"Set formant range...", 0, menu_cb_setFormantRange);
	EditorMenu_addCommand (menu, L"Set bandwidth range...", 0, menu_cb_setBandwidthRange);
	EditorMenu_addCommand (menu, L"-- select formant --", 0, NULL);
	EditorMenu_addCommand (menu, L"Select first", '1', menu_cb_selectFirst);
	EditorMenu_addCommand (menu, L"Select second", '2', menu_cb_selectSecond);
	EditorMenu_addCommand (menu, L"Select third", '3', menu_cb_selectThird);
	EditorMenu_addCommand (menu, L"Select fourth", '4', menu_cb_selectFourth);
	EditorMenu_addCommand (menu, L"Select fifth", '5', menu_cb_selectFifth);
	EditorMenu_addCommand (menu, L"Select sixth", '6', menu_cb_selectSixth);
	EditorMenu_addCommand (menu, L"Select seventh", '7', menu_cb_selectSeventh);
	EditorMenu_addCommand (menu, L"Select eighth", '8', menu_cb_selectEighth);
	EditorMenu_addCommand (menu, L"Select ninth", '9', menu_cb_selectNinth);
	EditorMenu_addCommand (menu, L"Select formant or bandwidth...", 0, menu_cb_selectFormantOrBandwidth);
	menu = Editor_addMenu (this, L"Point", 0);
	EditorMenu_addCommand (menu, L"Add point at cursor", 'T', menu_cb_addPointAtCursor);
	EditorMenu_addCommand (menu, L"Add point at...", 0, menu_cb_addPointAt);
	EditorMenu_addCommand (menu, L"-- remove point --", 0, NULL);
	EditorMenu_addCommand (menu, L"Remove point(s)", GuiMenu_OPTION + 'T', menu_cb_removePoints);
	if (v_hasSourceMenu ()) {
		menu = Editor_addMenu (this, L"Source", 0);
		EditorMenu_addCommand (menu, L"Pitch settings...", 0, menu_cb_pitchSettings);
		//EditorMenu_addCommand (menu, L"Phonation settings...", 0, menu_cb_phonationSettings);
	}
}

/********** DRAWING AREA **********/

void structFormantGridEditor :: v_draw () {
	FormantGrid grid = (FormantGrid) data;
	Ordered tiers = editingBandwidths ? grid -> bandwidths : grid -> formants;
	RealTier selectedTier = (RealTier) tiers -> item [selectedFormant];
	double ymin = editingBandwidths ? p_bandwidthFloor   : p_formantFloor;
	double ymax = editingBandwidths ? p_bandwidthCeiling : p_formantCeiling;
	Graphics_setColour (d_graphics, Graphics_WHITE);
	Graphics_setWindow (d_graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (d_graphics, 0, 1, 0, 1);
	Graphics_setWindow (d_graphics, d_startWindow, d_endWindow, ymin, ymax);
	Graphics_setColour (d_graphics, Graphics_RED);
	Graphics_line (d_graphics, d_startWindow, ycursor, d_endWindow, ycursor);
	Graphics_setTextAlignment (d_graphics, Graphics_RIGHT, Graphics_HALF);
	Graphics_text1 (d_graphics, d_startWindow, ycursor, Melder_float (Melder_half (ycursor)));
	Graphics_setColour (d_graphics, Graphics_BLUE);
	Graphics_setTextAlignment (d_graphics, Graphics_LEFT, Graphics_TOP);
	Graphics_text2 (d_graphics, d_endWindow, ymax, Melder_float (Melder_half (ymax)), L" Hz");
	Graphics_setTextAlignment (d_graphics, Graphics_LEFT, Graphics_HALF);
	Graphics_text2 (d_graphics, d_endWindow, ymin, Melder_float (Melder_half (ymin)), L" Hz");
	Graphics_setLineWidth (d_graphics, 1);
	Graphics_setColour (d_graphics, Graphics_GREY);
	for (long iformant = 1; iformant <= grid -> formants -> size; iformant ++) if (iformant != selectedFormant) {
		RealTier tier = (RealTier) tiers -> item [iformant];
		long imin = AnyTier_timeToHighIndex (tier, d_startWindow);
		long imax = AnyTier_timeToLowIndex (tier, d_endWindow);
		long n = tier -> points -> size;
		if (n == 0) {
		} else if (imax < imin) {
			double yleft = RealTier_getValueAtTime (tier, d_startWindow);
			double yright = RealTier_getValueAtTime (tier, d_endWindow);
			Graphics_line (d_graphics, d_startWindow, yleft, d_endWindow, yright);
		} else for (long i = imin; i <= imax; i ++) {
			RealPoint point = (RealPoint) tier -> points -> item [i];
			double t = point -> number, y = point -> value;
			Graphics_fillCircle_mm (d_graphics, t, y, 2);
			if (i == 1)
				Graphics_line (d_graphics, d_startWindow, y, t, y);
			else if (i == imin)
				Graphics_line (d_graphics, t, y, d_startWindow, RealTier_getValueAtTime (tier, d_startWindow));
			if (i == n)
				Graphics_line (d_graphics, t, y, d_endWindow, y);
			else if (i == imax)
				Graphics_line (d_graphics, t, y, d_endWindow, RealTier_getValueAtTime (tier, d_endWindow));
			else {
				RealPoint pointRight = (RealPoint) tier -> points -> item [i + 1];
				Graphics_line (d_graphics, t, y, pointRight -> number, pointRight -> value);
			}
		}
	}
	Graphics_setColour (d_graphics, Graphics_BLUE);
	long ifirstSelected = AnyTier_timeToHighIndex (selectedTier, d_startSelection);
	long ilastSelected = AnyTier_timeToLowIndex (selectedTier, d_endSelection);
	long n = selectedTier -> points -> size;
	long imin = AnyTier_timeToHighIndex (selectedTier, d_startWindow);
	long imax = AnyTier_timeToLowIndex (selectedTier, d_endWindow);
	Graphics_setLineWidth (d_graphics, 2);
	if (n == 0) {
		Graphics_setTextAlignment (d_graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (d_graphics, 0.5 * (d_startWindow + d_endWindow),
			0.5 * (ymin + ymax), L"(no points in selected formant tier)");
	} else if (imax < imin) {
		double yleft = RealTier_getValueAtTime (selectedTier, d_startWindow);
		double yright = RealTier_getValueAtTime (selectedTier, d_endWindow);
		Graphics_line (d_graphics, d_startWindow, yleft, d_endWindow, yright);
	} else for (long i = imin; i <= imax; i ++) {
		RealPoint point = (RealPoint) selectedTier -> points -> item [i];
		double t = point -> number, y = point -> value;
		if (i >= ifirstSelected && i <= ilastSelected)
			Graphics_setColour (d_graphics, Graphics_RED);	
		Graphics_fillCircle_mm (d_graphics, t, y, 3);
		Graphics_setColour (d_graphics, Graphics_BLUE);
		if (i == 1)
			Graphics_line (d_graphics, d_startWindow, y, t, y);
		else if (i == imin)
			Graphics_line (d_graphics, t, y, d_startWindow, RealTier_getValueAtTime (selectedTier, d_startWindow));
		if (i == n)
			Graphics_line (d_graphics, t, y, d_endWindow, y);
		else if (i == imax)
			Graphics_line (d_graphics, t, y, d_endWindow, RealTier_getValueAtTime (selectedTier, d_endWindow));
		else {
			RealPoint pointRight = (RealPoint) selectedTier -> points -> item [i + 1];
			Graphics_line (d_graphics, t, y, pointRight -> number, pointRight -> value);
		}
	}
	Graphics_setLineWidth (d_graphics, 1);
	Graphics_setColour (d_graphics, Graphics_BLACK);
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
		Graphics_text1 (my d_graphics, t, ymax, Melder_fixed (t, 6));
		Graphics_line (my d_graphics, my d_startWindow, y, my d_endWindow, y);
		Graphics_setTextAlignment (my d_graphics, Graphics_LEFT, Graphics_BOTTOM);
		Graphics_text1 (my d_graphics, my d_startWindow, y, Melder_fixed (y, 6));
	}
}

int structFormantGridEditor :: v_click (double xWC, double yWC, bool shiftKeyPressed) {
	FormantGrid grid = (FormantGrid) data;
	Ordered tiers = editingBandwidths ? grid -> bandwidths : grid -> formants;
	RealTier tier = (RealTier) tiers -> item [selectedFormant];
	double ymin = editingBandwidths ? p_bandwidthFloor   : p_formantFloor;
	double ymax = editingBandwidths ? p_bandwidthCeiling : p_formantCeiling;
	long inearestPoint, ifirstSelected, ilastSelected;
	RealPoint nearestPoint;
	double dt = 0, df = 0;
	int draggingSelection;

	/*
	 * Perform the default action: move cursor.
	 */
	d_startSelection = d_endSelection = xWC;
	ycursor = (1.0 - yWC) * ymin + yWC * ymax;
	Graphics_setWindow (d_graphics, d_startWindow, d_endWindow, ymin, ymax);
	yWC = ycursor;

	/*
	 * Clicked on a point?
	 */
	inearestPoint = AnyTier_timeToNearestIndex (tier, xWC);
	if (inearestPoint == 0) {
		return FormantGridEditor_Parent :: v_click (xWC, yWC, shiftKeyPressed);
	}
	nearestPoint = (RealPoint) tier -> points -> item [inearestPoint];
	if (Graphics_distanceWCtoMM (d_graphics, xWC, yWC, nearestPoint -> number, nearestPoint -> value) > 1.5) {
		return FormantGridEditor_Parent :: v_click (xWC, yWC, shiftKeyPressed);
	}

	/*
	 * Clicked on a selected point?
	 */
	draggingSelection = shiftKeyPressed &&
		nearestPoint -> number > d_startSelection && nearestPoint -> number < d_endSelection;
	if (draggingSelection) {
		ifirstSelected = AnyTier_timeToHighIndex (tier, d_startSelection);
		ilastSelected = AnyTier_timeToLowIndex (tier, d_endSelection);
		Editor_save (this, L"Drag points");
	} else {
		ifirstSelected = ilastSelected = inearestPoint;
		Editor_save (this, L"Drag point");
	}

	/*
	 * Drag.
	 */
	Graphics_xorOn (d_graphics, Graphics_MAROON);
	drawWhileDragging (this, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
	while (Graphics_mouseStillDown (d_graphics)) {
		double xWC_new, yWC_new;
		Graphics_getMouseLocation (d_graphics, & xWC_new, & yWC_new);
		if (xWC_new != xWC || yWC_new != yWC) {
			drawWhileDragging (this, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
			dt += xWC_new - xWC, df += yWC_new - yWC;
			xWC = xWC_new, yWC = yWC_new;
			drawWhileDragging (this, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
		}
	}
	Graphics_xorOff (d_graphics);

	/*
	 * Dragged inside window?
	 */
	if (xWC < d_startWindow || xWC > d_endWindow) return 1;

	/*
	 * Points not dragged past neighbours?
	 */
	RealPoint *points = (RealPoint *) tier -> points -> item;
	double newTime = points [ifirstSelected] -> number + dt;
	if (newTime < d_tmin) return 1;   // outside domain
	if (ifirstSelected > 1 && newTime <= points [ifirstSelected - 1] -> number)
		return 1;   // past left neighbour
	newTime = points [ilastSelected] -> number + dt;
	if (newTime > d_tmax) return 1;   // outside domain
	if (ilastSelected < tier -> points -> size && newTime >= points [ilastSelected + 1] -> number)
		return 1;   // past right neighbour

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

	if (draggingSelection) d_startSelection += dt, d_endSelection += dt;
	if (ifirstSelected == ilastSelected) {
		/*
		 * Move crosshair to only selected formant point.
		 */
		RealPoint point = (RealPoint) tier -> points -> item [ifirstSelected];
		d_startSelection = d_endSelection = point -> number;
		ycursor = point -> value;
	} else {
		/*
		 * Move crosshair to mouse location.
		 */
		/*cursor += dt;*/
		ycursor += df;
	}

	broadcastDataChanged ();
	return 1;   // update needed
}

void structFormantGridEditor :: v_play (double tmin, double tmax) {
	FormantGrid_playPart ((FormantGrid) data, tmin, tmax, p_play_samplingFrequency,
		p_source_pitch_tStart, p_source_pitch_f0Start,
		p_source_pitch_tMid,   p_source_pitch_f0Mid,
		p_source_pitch_tEnd,   p_source_pitch_f0End,
		p_source_phonation_adaptFactor, p_source_phonation_maximumPeriod,
		p_source_phonation_openPhase,   p_source_phonation_collisionPhase,
		p_source_phonation_power1,      p_source_phonation_power2,
		theFunctionEditor_playCallback, this);
}

void FormantGridEditor_init (FormantGridEditor me, const wchar_t *title, FormantGrid data) {
	Melder_assert (data != NULL);
	Melder_assert (Thing_member (data, classFormantGrid));
	FunctionEditor_init (me, title, data);
	my ycursor = 0.382 * my p_formantFloor + 0.618 * my p_formantCeiling;
	my selectedFormant = 1;
}

FormantGridEditor FormantGridEditor_create (const wchar_t *title, FormantGrid data) {
	try {
		autoFormantGridEditor me = Thing_new (FormantGridEditor);
		FormantGridEditor_init (me.peek(), title, data);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("FormantGrid window not created.");
	}
}

/* End of file FormantGridEditor.cpp */
