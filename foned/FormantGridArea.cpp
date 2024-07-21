/* FormantGridArea.cpp
 *
 * Copyright (C) 2008-2022,2024 Paul Boersma & David Weenink
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

#include "FormantGridArea.h"
#include "EditorM.h"

Thing_implement (FormantGridArea, RealTierArea, 0);

#include "Prefs_define.h"
#include "FormantGridArea_prefs.h"
#include "Prefs_install.h"
#include "FormantGridArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "FormantGridArea_prefs.h"


#pragma mark - FormantGridArea drawing

void structFormantGridArea :: v_drawInside () {
	Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), our ymin, our ymax);
	Graphics_setColour (our graphics(), Melder_RED);
	//Graphics_line (our graphics(), our startWindow, our formantGridArea -> ycursor, our endWindow, our formantGridArea -> ycursor);
	Graphics_setTextAlignment (our graphics(), Graphics_RIGHT, Graphics_HALF);
	//Graphics_text (our graphics(), our startWindow(), our ycursor,
	//		Melder_float (Melder_half (our ycursor)));
	Graphics_setLineWidth (our graphics(), 1.0);
	Graphics_setColour (our graphics(), Melder_GREY);
	OrderedOf<structRealTier>* tiers = (
		our editingBandwidths ?
		& our formantGrid() -> bandwidths :
		& our formantGrid() -> formants
	);
	for (integer iformant = 1; iformant <= our formantGrid() -> formants.size; iformant ++) if (iformant != our selectedFormant) {
		RealTier tier = tiers->at [iformant];
		Melder_assert (Thing_isa (tier, classRealTier));
		const integer imin = AnyTier_timeToHighIndex (tier->asAnyTier(), our startWindow());
		const integer imax = AnyTier_timeToLowIndex (tier->asAnyTier(), our endWindow());
		const integer n = tier -> points.size;
		if (n == 0) {
		} else if (imax < imin) {
			const double yleft = RealTier_getValueAtTime (tier, our startWindow());
			const double yright = RealTier_getValueAtTime (tier, our endWindow());
			Graphics_line (our graphics(), our startWindow(), yleft, our endWindow(), yright);
		} else {
			for (integer i = imin; i <= imax; i ++) {
				RealPoint point = tier -> points.at [i];
				const double t = point -> number, y = point -> value;
				Graphics_fillCircle_mm (our graphics(), t, y, 2.0);
				if (i == 1)
					Graphics_line (our graphics(), our startWindow(), y, t, y);
				else if (i == imin)
					Graphics_line (our graphics(), t, y, our startWindow(), RealTier_getValueAtTime (tier, our startWindow()));
				if (i == n)
					Graphics_line (our graphics(), t, y, our endWindow(), y);
				else if (i == imax)
					Graphics_line (our graphics(), t, y, our endWindow(), RealTier_getValueAtTime (tier, our endWindow()));
				else {
					RealPoint pointRight = tier -> points.at [i + 1];
					Graphics_line (our graphics(), t, y, pointRight -> number, pointRight -> value);
				}
			}
		}
	}
	FormantGridArea_Parent :: v_drawInside ();
}


#pragma mark - FormantGridArea Modify

static void menu_cb_removePoints (FormantGridArea me, EDITOR_ARGS) {
	FunctionArea_save (me, U"Remove point(s)");
	RealTierArea_removePoints (me);
	FunctionArea_broadcastDataChanged (me);
}
static void menu_cb_addPointAtCursor (FormantGridArea me, EDITOR_ARGS) {
	FunctionArea_save (me, U"Add point");
	RealTierArea_addPointAtCursor (me);
	FunctionArea_broadcastDataChanged (me);
}
static void menu_cb_addPointAt (FormantGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Add point", nullptr)
		REAL (time, U"Time (s)", U"0.0")
		POSITIVE (frequency, U"Frequency (Hz)", U"200.0")
	EDITOR_OK
		SET_REAL (time, 0.5 * (my startSelection() + my endSelection()))
		SET_REAL (frequency, my ycursor)
	EDITOR_DO
		Editor_save (my boss(), U"Add point");
		RealTierArea_addPointAt (me, time, frequency);
		Editor_broadcastDataChanged (my boss());
	EDITOR_END
}
static void menu_cb_setFormantRange (FormantGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Set formant range", nullptr)
		REAL (minimumFormant, U"Minimum formant (Hz)", my default_formantFloor())
		REAL (maximumFormant, U"Maximum formant (Hz)", my default_formantCeiling())
	EDITOR_OK
		SET_REAL (minimumFormant, my instancePref_formantFloor())
		SET_REAL (maximumFormant, my instancePref_formantCeiling())
	EDITOR_DO
		my setInstancePref_formantFloor (minimumFormant);
		my setInstancePref_formantCeiling (maximumFormant);
		my v_updateScaling ();   // BUG: should be viewChanged()
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}
static void menu_cb_setBandwidthRange (FormantGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Set bandwidth range", nullptr)
		REAL (minimumBandwidth, U"Minimum bandwidth (Hz)", my default_bandwidthFloor())
		REAL (maximumBandwidth, U"Maximum bandwidth (Hz)", my default_bandwidthCeiling())
	EDITOR_OK
		SET_REAL (minimumBandwidth, my instancePref_bandwidthFloor())
		SET_REAL (maximumBandwidth, my instancePref_bandwidthCeiling())
	EDITOR_DO
		my setInstancePref_bandwidthFloor (minimumBandwidth);
		my setInstancePref_bandwidthCeiling (maximumBandwidth);
		my v_updateScaling ();   // BUG: should be viewChanged()
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}
static void menu_cb_showBandwidths (FormantGridArea me, EDITOR_ARGS) {
	my editingBandwidths = ! my editingBandwidths;
	GuiMenuItem_check (my d_bandwidthsToggle, my editingBandwidths);
	Editor_broadcastDataChanged (my boss());   // BUG: the data themselves have not changed, but the view on them has
}
static void selectFormantOrBandwidth (FormantGridArea me, integer iformant) {
	const integer numberOfFormants = my formantGrid() -> formants.size;
	if (iformant > numberOfFormants)
		Melder_throw (U"Cannot select formant ", iformant, U", because the FormantGrid has only ", numberOfFormants, U" formants.");
	my selectedFormant = iformant;
	Editor_dataChanged (my boss(), my boss());   // BUG: the data themselves have not changed, but the view on them has
}

static void menu_cb_selectFirst   (FormantGridArea me, EDITOR_ARGS) { selectFormantOrBandwidth (me, 1); }
static void menu_cb_selectSecond  (FormantGridArea me, EDITOR_ARGS) { selectFormantOrBandwidth (me, 2); }
static void menu_cb_selectThird   (FormantGridArea me, EDITOR_ARGS) { selectFormantOrBandwidth (me, 3); }
static void menu_cb_selectFourth  (FormantGridArea me, EDITOR_ARGS) { selectFormantOrBandwidth (me, 4); }
static void menu_cb_selectFifth   (FormantGridArea me, EDITOR_ARGS) { selectFormantOrBandwidth (me, 5); }
static void menu_cb_selectSixth   (FormantGridArea me, EDITOR_ARGS) { selectFormantOrBandwidth (me, 6); }
static void menu_cb_selectSeventh (FormantGridArea me, EDITOR_ARGS) { selectFormantOrBandwidth (me, 7); }
static void menu_cb_selectEighth  (FormantGridArea me, EDITOR_ARGS) { selectFormantOrBandwidth (me, 8); }
static void menu_cb_selectNinth   (FormantGridArea me, EDITOR_ARGS) { selectFormantOrBandwidth (me, 9); }
static void menu_cb_selectFormantOrBandwidth (FormantGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Select formant or bandwidth", nullptr)
		NATURAL (formantNumber, U"Formant number", U"1")
	EDITOR_OK
		SET_INTEGER (formantNumber, my selectedFormant)
	EDITOR_DO
		selectFormantOrBandwidth (me, formantNumber);
		Editor_broadcastDataChanged (my boss());   // BUG: the data themselves have not changed, but the view on them has
	EDITOR_END
}

static void menu_cb_pitchSettings (FormantGridArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Source pitch settings", nullptr)
		COMMENT (U"These settings apply to the pitch curve")
		COMMENT (U"that you hear when playing the FormantGrid.")
		REAL     (startTime,  U"Start time",       my default_source_pitch_tStart  ())
		POSITIVE (startPitch, U"Start pitch (Hz)", my default_source_pitch_f0Start ())
		REAL     (midTime,    U"Mid time",         my default_source_pitch_tMid    ())
		POSITIVE (midPitch,   U"Mid pitch (Hz)",   my default_source_pitch_f0Mid   ())
		REAL     (endTime,    U"End time",         my default_source_pitch_tEnd    ())
		POSITIVE (endPitch,   U"End pitch (Hz)",   my default_source_pitch_f0End   ())
	EDITOR_OK
		SET_REAL (startTime,  my instancePref_source_pitch_tStart())
		SET_REAL (startPitch, my instancePref_source_pitch_f0Start())
		SET_REAL (midTime,    my instancePref_source_pitch_tMid())
		SET_REAL (midPitch,   my instancePref_source_pitch_f0Mid())
		SET_REAL (endTime,    my instancePref_source_pitch_tEnd())
		SET_REAL (endPitch,   my instancePref_source_pitch_f0End())
	EDITOR_DO
		my setInstancePref_source_pitch_tStart  (startTime);
		my setInstancePref_source_pitch_f0Start (startPitch);
		my setInstancePref_source_pitch_tMid    (midTime);
		my setInstancePref_source_pitch_f0Mid   (midPitch);
		my setInstancePref_source_pitch_tEnd    (endTime);
		my setInstancePref_source_pitch_f0End   (endPitch);
	EDITOR_END
}

void structFormantGridArea :: v_createMenus () {
	//FormantGridArea_Parent :: v_createMenus ();
	EditorMenu menu = Editor_addMenu (our functionEditor(), U"Formant", 0);
	our d_bandwidthsToggle = FunctionAreaMenu_addCommand (menu, U"Show bandwidths", GuiMenu_CHECKBUTTON,
			menu_cb_showBandwidths, this);
	FunctionAreaMenu_addCommand (menu, U"Set formant range...", 0,
			menu_cb_setFormantRange, this);
	FunctionAreaMenu_addCommand (menu, U"Set bandwidth range...", 0,
			menu_cb_setBandwidthRange, this);
	FunctionAreaMenu_addCommand (menu, U"-- select formant --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Select first",   '1', menu_cb_selectFirst,   this);
	FunctionAreaMenu_addCommand (menu, U"Select second",  '2', menu_cb_selectSecond,  this);
	FunctionAreaMenu_addCommand (menu, U"Select third",   '3', menu_cb_selectThird,   this);
	FunctionAreaMenu_addCommand (menu, U"Select fourth",  '4', menu_cb_selectFourth,  this);
	FunctionAreaMenu_addCommand (menu, U"Select fifth",   '5', menu_cb_selectFifth,   this);
	FunctionAreaMenu_addCommand (menu, U"Select sixth",   '6', menu_cb_selectSixth,   this);
	FunctionAreaMenu_addCommand (menu, U"Select seventh", '7', menu_cb_selectSeventh, this);
	FunctionAreaMenu_addCommand (menu, U"Select eighth",  '8', menu_cb_selectEighth,  this);
	FunctionAreaMenu_addCommand (menu, U"Select ninth",   '9', menu_cb_selectNinth,   this);
	FunctionAreaMenu_addCommand (menu, U"Select formant or bandwidth...", 0,
			menu_cb_selectFormantOrBandwidth, this);
	menu = Editor_addMenu (our functionEditor(), U"Point", 0);
	FunctionAreaMenu_addCommand (menu, U"Add point at cursor", 'T',
			menu_cb_addPointAtCursor, this);
	FunctionAreaMenu_addCommand (menu, U"Add point at...", 0,
			menu_cb_addPointAt, this);
	FunctionAreaMenu_addCommand (menu, U"-- remove point --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Remove point(s)", GuiMenu_OPTION | 'T',
			menu_cb_removePoints, this);
	if (our v_hasSourceMenu ()) {
		menu = Editor_addMenu (our functionEditor(), U"Source", 0);
		FunctionAreaMenu_addCommand (menu, U"Pitch settings...", 0,
				menu_cb_pitchSettings, this);
		//FunctionAreaMenu_addCommand (menu, U"Phonation settings...", 0,
		//		menu_cb_phonationSettings, this);
	}
}

void FormantGridArea_playPart (FormantGridArea me, double startTime, double endTime, Sound_PlayCallback playCallback, Thing playBoss) {
	FormantGrid_playPart (my formantGrid(), startTime, endTime, my instancePref_play_samplingFrequency(),
		my instancePref_source_pitch_tStart(), my instancePref_source_pitch_f0Start(),
		my instancePref_source_pitch_tMid(),   my instancePref_source_pitch_f0Mid(),
		my instancePref_source_pitch_tEnd(),   my instancePref_source_pitch_f0End(),
		my instancePref_source_phonation_adaptFactor(), my instancePref_source_phonation_maximumPeriod(),
		my instancePref_source_phonation_openPhase(),   my instancePref_source_phonation_collisionPhase(),
		my instancePref_source_phonation_power1(),      my instancePref_source_phonation_power2(),
		playCallback, playBoss);
}

/* End of file FormantGridArea.cpp */
