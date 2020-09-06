/* FormantGridEditor.cpp
 *
 * Copyright (C) 2008-2020 Paul Boersma & David Weenink
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

Thing_implement (FormantGridArea, RealTierArea, 0);

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
	RealTierArea_removePoints (my formantGridArea.get(), tier);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPointAtCursor (FormantGridEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Add point");
	FormantGrid grid = (FormantGrid) my data;
	OrderedOf<structRealTier>* tiers = ( my editingBandwidths ? & grid -> bandwidths : & grid -> formants );
	RealTier tier = tiers->at [my selectedFormant];
	RealTierArea_addPointAtCursor (my formantGridArea.get(), tier);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPointAt (FormantGridEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add point", nullptr)
		REAL (time, U"Time (s)", U"0.0")
		POSITIVE (frequency, U"Frequency (Hz)", U"200.0")
	EDITOR_OK
		SET_REAL (time, 0.5 * (my startSelection + my endSelection))
		SET_REAL (frequency, my formantGridArea -> ycursor)
	EDITOR_DO
		Editor_save (me, U"Add point");
		FormantGrid grid = (FormantGrid) my data;
		OrderedOf<structRealTier>* tiers = ( my editingBandwidths ? & grid -> bandwidths : & grid -> formants );
		RealTier tier = tiers->at [my selectedFormant];
		RealTierArea_addPointAt (my formantGridArea.get(), tier, time, frequency);
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
	our formantGridArea -> ymin = ( our editingBandwidths ? our p_bandwidthFloor   : our p_formantFloor );
	our formantGridArea -> ymax = ( our editingBandwidths ? our p_bandwidthCeiling : our p_formantCeiling );

	our formantGridArea -> setViewport();

	Graphics_setColour (our graphics.get(), Melder_WHITE);
	Graphics_setWindow (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_fillRectangle (our graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setWindow (our graphics.get(), our startWindow, our endWindow, our formantGridArea -> ymin, our formantGridArea -> ymax);
	Graphics_setColour (our graphics.get(), Melder_RED);
	//Graphics_line (our graphics.get(), our startWindow, our formantGridArea -> ycursor, our endWindow, our formantGridArea -> ycursor);
	Graphics_setTextAlignment (our graphics.get(), Graphics_RIGHT, Graphics_HALF);
	//Graphics_text (our graphics.get(), our startWindow, our formantGridArea -> ycursor,
	//		Melder_float (Melder_half (our formantGridArea -> ycursor)));
	Graphics_setLineWidth (our graphics.get(), 1.0);
	Graphics_setColour (our graphics.get(), Melder_GREY);
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
		} else {
			for (integer i = imin; i <= imax; i ++) {
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
	}
	RealTierArea_draw (our formantGridArea.get(), selectedTier);
	if (isdefined (our formantGridArea -> anchorTime))
		RealTierArea_drawWhileDragging (our formantGridArea.get(), selectedTier);
}

bool structFormantGridEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double globalY_fraction) {
	FormantGrid grid = (FormantGrid) our data;
	OrderedOf<structRealTier>* tiers = our editingBandwidths ? & grid -> bandwidths : & grid -> formants;
	RealTier tier = tiers->at [selectedFormant];
	our formantGridArea -> ymin = ( our editingBandwidths ? our p_bandwidthFloor   : our p_formantFloor );
	our formantGridArea -> ymax = ( our editingBandwidths ? our p_bandwidthCeiling : our p_formantCeiling );

	static bool clickedInWideRealTierArea = false;
	if (event -> isClick ())
		clickedInWideRealTierArea = our formantGridArea -> y_fraction_globalIsInside (globalY_fraction);
	bool result = false;
	if (clickedInWideRealTierArea) {
		our formantGridArea -> setViewport ();
		result = RealTierArea_mouse (our formantGridArea.get(), tier, event, x_world, globalY_fraction);
	} else {
		result = our FormantGridEditor_Parent :: v_mouseInWideDataView (event, x_world, globalY_fraction);
	}
	if (event -> isDrop())
		clickedInWideRealTierArea = false;
	return result;
}

void structFormantGridEditor :: v_play (double startTime, double endTime) {
	FormantGrid_playPart ((FormantGrid) our data, startTime, endTime, our p_play_samplingFrequency,
		our p_source_pitch_tStart, our p_source_pitch_f0Start,
		our p_source_pitch_tMid,   our p_source_pitch_f0Mid,
		our p_source_pitch_tEnd,   our p_source_pitch_f0End,
		our p_source_phonation_adaptFactor, our p_source_phonation_maximumPeriod,
		our p_source_phonation_openPhase,   our p_source_phonation_collisionPhase,
		our p_source_phonation_power1,      our p_source_phonation_power2,
		theFunctionEditor_playCallback, this);
}

void FormantGridEditor_init (FormantGridEditor me, conststring32 title, FormantGrid data) {
	Melder_assert (data);
	Melder_assert (Thing_isa (data, classFormantGrid));
	FunctionEditor_init (me, title, data);
	my selectedFormant = 1;
	my formantGridArea = Thing_new (FormantGridArea);
	FunctionArea_init (my formantGridArea.get(), me, 0.0, 1.0);
	my formantGridArea -> ycursor = 0.382 * my p_formantFloor + 0.618 * my p_formantCeiling;
}

autoFormantGridEditor FormantGridEditor_create (conststring32 title, FormantGrid data) {
	try {
		autoFormantGridEditor me = Thing_new (FormantGridEditor);
		FormantGridEditor_init (me.get(), title, data);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FormantGrid window not created.");
	}
}

/* End of file FormantGridEditor.cpp */
