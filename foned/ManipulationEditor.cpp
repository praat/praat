/* ManipulationEditor.cpp
 *
 * Copyright (C) 1992-2024 Paul Boersma
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

#include "ManipulationEditor.h"
#include "PitchTier_to_PointProcess.h"
#include "Sound_to_PointProcess.h"
#include "Sound_to_Pitch.h"
#include "Pitch_to_PitchTier.h"
#include "Pitch_to_PointProcess.h"
#include "EditorM.h"


#pragma mark - ManipPulsesArea

Thing_implement (ManipulationPulsesArea, PointArea, 0);


#pragma mark - ManipPulsesArea Modify

static void menu_cb_removePulses (ManipulationPulsesArea me, EDITOR_ARGS) {
	MODIFY_DATA (U"Remove pulse(s)")
		if (my startSelection() == my endSelection())
			PointProcess_removePointNear (my pulses(), my startSelection());
		else
			PointProcess_removePointsBetween (my pulses(), my startSelection(), my endSelection());
	MODIFY_DATA_END
}
static void menu_cb_addPulseAtCursor (ManipulationPulsesArea me, EDITOR_ARGS) {
	MODIFY_DATA (U"Add pulse")
		PointProcess_addPoint (my pulses(), 0.5 * (my startSelection() + my endSelection()));
	MODIFY_DATA_END
}
static void menu_cb_addPulseAt (ManipulationPulsesArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Add pulse", nullptr)
		REAL (position, U"Position (s)", U"0.0")
	EDITOR_OK
		SET_REAL (position, 0.5 * (my startSelection() + my endSelection()))
	EDITOR_DO
		MODIFY_DATA (U"Add pulse")
			PointProcess_addPoint (my pulses(), position);
		MODIFY_DATA_END
	EDITOR_END
}


#pragma mark - ManipPulsesArea all menus

void structManipulationPulsesArea :: v_createMenus () {
	EditorMenu menu = Editor_addMenu (our functionEditor(), U"Pulse", 0);
	FunctionAreaMenu_addCommand (menu, U"Add pulse at cursor", 'P',
			menu_cb_addPulseAtCursor, this);
	FunctionAreaMenu_addCommand (menu, U"Add pulse at...", 0,
			menu_cb_addPulseAt, this);
	FunctionAreaMenu_addCommand (menu, U"-- remove pulses --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Remove pulse(s)", GuiMenu_OPTION | 'P',
			menu_cb_removePulses, this);
}


#pragma mark - ManipulationSoundArea

Thing_implement (ManipulationSoundArea, SoundArea, 0);


#pragma mark - ManipPitchTierArea

Thing_implement (ManipulationPitchTierArea, PitchTierArea, 0);

#include "enums_getText.h"
#include "ManipulationEditor_enums.h"
#include "enums_getValue.h"
#include "ManipulationEditor_enums.h"

#include "Prefs_define.h"
#include "ManipulationEditor_prefs.h"
#include "Prefs_install.h"
#include "ManipulationEditor_prefs.h"
#include "Prefs_copyToInstance.h"
#include "ManipulationEditor_prefs.h"


#pragma mark - ManipPitchTierArea Modify

static void menu_cb_removePitchPoints (ManipulationPitchTierArea me, EDITOR_ARGS) {
	if (! my pitch())
		return;
	MODIFY_DATA (U"Remove pitch point(s)")
		if (my startSelection() == my endSelection())
			AnyTier_removePointNear (my pitch()->asAnyTier(), my startSelection());
		else
			AnyTier_removePointsBetween (my pitch()->asAnyTier(), my startSelection(), my endSelection());
	MODIFY_DATA_END
}
static void menu_cb_addPitchPointAtCursor (ManipulationPitchTierArea me, EDITOR_ARGS) {
	if (! my pitch())
		return;
	MODIFY_DATA (U"Add pitch point")
		RealTier_addPoint (my pitch(), 0.5 * (my startSelection() + my endSelection()), my ycursor);
	MODIFY_DATA_END
}
static void menu_cb_addPitchPointAtSlice (ManipulationPitchTierArea me, EDITOR_ARGS) {
	if (! my borrowedPulsesArea || ! my borrowedPulsesArea -> pulses())
		Melder_throw (U"There are no pulses.");
	if (! my pitch())
		return;
	const integer ileft = PointProcess_getLowIndex (my borrowedPulsesArea -> pulses(), 0.5 * (my startSelection() + my endSelection()));
	const integer iright = ileft + 1, nt = my borrowedPulsesArea -> pulses() -> nt;
	constVEC t = my borrowedPulsesArea -> pulses() -> t.get();
	double desiredY = my ycursor;   // default
	MODIFY_DATA (U"Add pitch point")
		if (nt <= 1) {
			/* Ignore. */
		} else if (ileft <= 0) {
			double tright = t [2] - t [1];
			if (tright > 0.0 && tright <= 0.02)
				desiredY = 1.0 / tright;
		} else if (iright > nt) {
			double tleft = t [nt] - t [nt - 1];
			if (tleft > 0.0 && tleft <= 0.02)
				desiredY = 1.0 / tleft;
		} else {   // three-period median
			double tmid = t [iright] - t [ileft], tleft = 0.0, tright = 0.0;
			if (ileft > 1)
				tleft = t [ileft] - t [ileft - 1];
			if (iright < nt)
				tright = t [iright + 1] - t [iright];
			if (tleft > 0.02)
				tleft = 0.0;
			if (tmid > 0.02)
				tmid = 0.0;
			if (tright > 0.02)
				tright = 0.0;
			/* Bubble-sort. */
			Melder_sort (& tleft, & tmid);
			Melder_sort (& tleft, & tright);
			Melder_sort (& tmid, & tright);
			if (tleft != 0.0)
				desiredY = 1.0 / tmid;   // median of 3
			else if (tmid != 0.0)
				desiredY = 2.0 / (tmid + tright);   // median of 2
			else if (tright != 0.0)
				desiredY = 1.0 / tright;   // median of 1
		}
		RealTierArea_addPointAt (me, 0.5 * (my startSelection() + my endSelection()), desiredY);
	MODIFY_DATA_END
}
static void menu_cb_addPitchPointAt (ManipulationPitchTierArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Add pitch point", nullptr)
		REAL (time, U"Time (s)", U"0.0")
		REAL (frequency, U"Frequency (Hz or st)", U"100.0")
	EDITOR_OK
		SET_REAL (time, 0.5 * (my startSelection() + my endSelection()))
		SET_REAL (frequency, my ycursor)
	EDITOR_DO
		if (! my pitch())
			return;
		MODIFY_DATA (U"Add pitch point")
			RealTierArea_addPointAt (me, time, frequency);
		MODIFY_DATA_END
	EDITOR_END
}
static void menu_cb_stylizePitch (ManipulationPitchTierArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Stylize pitch", U"PitchTier: Stylize...")
		REAL (frequencyResolution, U"Frequency resolution", my default_pitch_stylize_frequencyResolution ())
		CHOICE (units, U"Units", my default_pitch_stylize_useSemitones () + 1)
			OPTION (U"Hertz")
			OPTION (U"semitones")
	EDITOR_OK
		SET_REAL   (frequencyResolution, my instancePref_pitch_stylize_frequencyResolution())
		SET_OPTION (units,               my instancePref_pitch_stylize_useSemitones() + 1)
	EDITOR_DO
		if (! my pitch())
			return;
		MODIFY_DATA (U"Stylize pitch")
			my setInstancePref_pitch_stylize_frequencyResolution (frequencyResolution);
			my setInstancePref_pitch_stylize_useSemitones (units - 1);
			PitchTier_stylize (my pitch(), frequencyResolution, my instancePref_pitch_stylize_useSemitones());
		MODIFY_DATA_END
	EDITOR_END
}
static void menu_cb_stylizePitch_2st (ManipulationPitchTierArea me, EDITOR_ARGS) {
	if (! my pitch())
		return;
	MODIFY_DATA (U"Stylize pitch")
		PitchTier_stylize (my pitch(), 2.0, true);
	MODIFY_DATA_END
}
static void menu_cb_interpolateQuadratically (ManipulationPitchTierArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Interpolate quadratically", nullptr)
		NATURAL (numberOfPointsPerParabola, U"Number of points per parabola", my default_pitch_interpolateQuadratically_numberOfPointsPerParabola ())
		CHOICE (units, U"Units", my default_pitch_stylize_useSemitones () + 1)
			OPTION (U"Hertz")
			OPTION (U"semitones")
	EDITOR_OK
		SET_INTEGER (numberOfPointsPerParabola, my instancePref_pitch_interpolateQuadratically_numberOfPointsPerParabola())
		SET_OPTION  (units,                     my instancePref_pitch_stylize_useSemitones() + 1)
	EDITOR_DO
		if (! my pitch())
			return;
		MODIFY_DATA (U"Interpolate quadratically")
			my setInstancePref_pitch_interpolateQuadratically_numberOfPointsPerParabola (numberOfPointsPerParabola);
			my setInstancePref_pitch_stylize_useSemitones (units - 1);
			RealTier_interpolateQuadratically (my pitch(), numberOfPointsPerParabola, my instancePref_pitch_stylize_useSemitones());
		MODIFY_DATA_END
	EDITOR_END
}
static void menu_cb_shiftPitchFrequencies (ManipulationPitchTierArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Shift pitch frequencies", nullptr)
		REAL (frequencyShift, U"Frequency shift", U"-20.0")
		OPTIONMENU (unit_i, U"Unit", 1)
			OPTION (U"Hertz")
			OPTION (U"mel")
			OPTION (U"logHertz")
			OPTION (U"semitones")
			OPTION (U"ERB")
	EDITOR_OK
	EDITOR_DO
		kPitch_unit unit =
			unit_i == 1 ? kPitch_unit::HERTZ :
			unit_i == 2 ? kPitch_unit::MEL :
			unit_i == 3 ? kPitch_unit::LOG_HERTZ :
			unit_i == 4 ? kPitch_unit::SEMITONES_1 :
			kPitch_unit::ERB;
		if (! my pitch())
			return;
		MODIFY_DATA (U"Shift pitch frequencies")
		try {
			PitchTier_shiftFrequencies (my pitch(), my startSelection(), my endSelection(), frequencyShift, unit);
			MODIFY_DATA_END
		} catch (MelderError) {
			// the PitchTier may have partially changed
			MODIFY_DATA_END
			throw;
		}
	EDITOR_END
}
static void menu_cb_multiplyPitchFrequencies (ManipulationPitchTierArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Multiply pitch frequencies", nullptr)
		POSITIVE (factor, U"Factor", U"1.2")
		COMMENT (U"The multiplication is always done in hertz.")
	EDITOR_OK
	EDITOR_DO
		if (! my pitch())
			return;
		MODIFY_DATA (U"Multiply pitch frequencies")
			PitchTier_multiplyFrequencies (my pitch(), my startSelection(), my endSelection(), factor);
		MODIFY_DATA_END
	EDITOR_END
}
static void menu_cb_setPitchRange (ManipulationPitchTierArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Set pitch range", nullptr)
		REAL (dataFreeMinimum, U"Data-free minimum (Hz)", my default_dataFreeMinimum())
		REAL (dataFreeMaximum, U"Data-free maximum (Hz)", my default_dataFreeMaximum())
	EDITOR_OK
		SET_REAL (dataFreeMinimum, my instancePref_dataFreeMinimum())
		SET_REAL (dataFreeMaximum, my instancePref_dataFreeMaximum())
	EDITOR_DO
		my setInstancePref_dataFreeMinimum (dataFreeMinimum);
		my setInstancePref_dataFreeMaximum (dataFreeMaximum);
		my v_updateScaling ();   // BUG: should be viewChanged()
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}
void structManipulationPitchTierArea :: v_createMenus () {
	EditorMenu menu = Editor_addMenu (our functionEditor(), U"Pitch", 0);

	FunctionAreaMenu_addCommand (menu, U"Set pitch range...", 0,
			menu_cb_setPitchRange, this);

	FunctionAreaMenu_addCommand (menu, U"- Edit pitch:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Add pitch point at cursor", 'T' | GuiMenu_DEPTH_1,
			menu_cb_addPitchPointAtCursor, this);
	FunctionAreaMenu_addCommand (menu, U"Add pitch point at time slice", 1,
			menu_cb_addPitchPointAtSlice, this);
	FunctionAreaMenu_addCommand (menu, U"Add pitch point at...", 1,
			menu_cb_addPitchPointAt, this);
	FunctionAreaMenu_addCommand (menu, U"Remove pitch point(s)", GuiMenu_OPTION | 'T' | GuiMenu_DEPTH_1,
			menu_cb_removePitchPoints, this);

	FunctionAreaMenu_addCommand (menu, U"- Modify selected pitch:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Shift pitch frequencies...", 1,
			menu_cb_shiftPitchFrequencies, this);
	FunctionAreaMenu_addCommand (menu, U"Multiply pitch frequencies...", 1,
			menu_cb_multiplyPitchFrequencies, this);

	FunctionAreaMenu_addCommand (menu, U"- Modify whole pitch:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Stylize pitch...", 1,
			menu_cb_stylizePitch, this);
	FunctionAreaMenu_addCommand (menu, U"Stylize pitch (2 st)", '2' | GuiMenu_DEPTH_1,
			menu_cb_stylizePitch_2st, this);
	FunctionAreaMenu_addCommand (menu, U"Interpolate quadratically...", 1,
			menu_cb_interpolateQuadratically, this);
}


#pragma mark - ManipDurationTierArea

Thing_implement (ManipulationDurationTierArea, DurationTierArea, 0);

static void menu_cb_setDurationRange (ManipulationDurationTierArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Set duration range", nullptr)
		REAL (dataFreeMinimum, U"Data-free minimum", my default_dataFreeMinimum())
		REAL (dataFreeMaximum, U"Data-free maximum", my default_dataFreeMaximum())
	EDITOR_OK
		SET_REAL (dataFreeMinimum, my instancePref_dataFreeMinimum())
		SET_REAL (dataFreeMaximum, my instancePref_dataFreeMaximum())
	EDITOR_DO
		if (dataFreeMinimum > 1.0)
			Melder_throw (U"Minimum relative duration should not be greater than 1.");
		if (dataFreeMaximum < 1.0)
			Melder_throw (U"Maximum relative duration should not be less than 1.");
		if (dataFreeMinimum >= dataFreeMaximum)
			Melder_throw (U"Maximum relative duration should be greater than minimum.");
		my setInstancePref_dataFreeMinimum (dataFreeMinimum);
		my setInstancePref_dataFreeMaximum (dataFreeMaximum);
		my v_updateScaling ();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}
static void menu_cb_removeDurationPoints (ManipulationDurationTierArea me, EDITOR_ARGS) {
	if (! my duration())
		return;
	MODIFY_DATA (U"Remove duration point(s)")
		if (my startSelection() == my endSelection())
			AnyTier_removePointNear (my duration()->asAnyTier(), 0.5 * (my startSelection() + my endSelection()));
		else
			AnyTier_removePointsBetween (my duration()->asAnyTier(), my startSelection(), my endSelection());
	MODIFY_DATA_END
}
static void menu_cb_addDurationPointAtCursor (ManipulationDurationTierArea me, EDITOR_ARGS) {
	if (! my duration())
		return;
	MODIFY_DATA (U"Add duration point")
		RealTier_addPoint (my duration(), 0.5 * (my startSelection() + my endSelection()), my ycursor);
	MODIFY_DATA_END
}
static void menu_cb_addDurationPointAt (ManipulationDurationTierArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Add duration point", nullptr)
		REAL (time, U"Time (s)", U"0.0");
		REAL (relativeDuration, U"Relative duration", U"1.0");
	EDITOR_OK
		SET_REAL (time, 0.5 * (my startSelection() + my endSelection()))
	EDITOR_DO
		if (! my duration())
			return;
		MODIFY_DATA (U"Add duration point")
			RealTierArea_addPointAt (me, time, relativeDuration);
		MODIFY_DATA_END
	EDITOR_END
}
void structManipulationDurationTierArea :: v_createMenus () {
	EditorMenu menu = Editor_addMenu (our functionEditor(), U"Dur", 0);

	FunctionAreaMenu_addCommand (menu, U"Set duration range...", 0,
			menu_cb_setDurationRange, this);

	FunctionAreaMenu_addCommand (menu, U"- Edit duration:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Add duration point at cursor", 'D' | GuiMenu_DEPTH_1,
			menu_cb_addDurationPointAtCursor, this);
	FunctionAreaMenu_addCommand (menu, U"Add duration point at...", 1,
			menu_cb_addDurationPointAt, this);
	FunctionAreaMenu_addCommand (menu, U"Remove duration point(s)", GuiMenu_OPTION | 'D' | GuiMenu_DEPTH_1,
			menu_cb_removeDurationPoints, this);
}

#pragma mark - ManipulationEditor

Thing_implement (ManipulationEditor, FunctionEditor, 0);

/*
 * How to add a synthesis method (in an interruptable order):
 * 1. add an Manipulation_ #define in Manipulation.h;
 * 2. add a synthesize_ routine in Manipulation.cpp, and a reference to it in Manipulation_to_Sound;
 * 3. add a button in ManipulationEditor.h;
 * 4. add a cb_Synth_ callback.
 * 5. create the button in createMenus and update updateMenus;
 */

static const conststring32 units_strings [] = { 0, U"Hz", U"st" };

static int prefs_synthesisMethod = Manipulation_OVERLAPADD;   /* Remembered across editor creations, not across Praat sessions. */

void structManipulationEditor :: v_updateMenuItems () {
	ManipulationEditor_Parent :: v_updateMenuItems ();
	Melder_assert (our synthPulsesButton);
	GuiMenuItem_check (our synthPulsesButton, our synthesisMethod == Manipulation_PULSES);
	Melder_assert (our synthPulsesHumButton);
	GuiMenuItem_check (our synthPulsesHumButton, our synthesisMethod == Manipulation_PULSES_HUM);
	Melder_assert (our synthPulsesLpcButton);
	GuiMenuItem_check (our synthPulsesLpcButton, our synthesisMethod == Manipulation_PULSES_LPC);
	Melder_assert (our synthPitchButton);
	GuiMenuItem_check (our synthPitchButton, our synthesisMethod == Manipulation_PITCH);
	Melder_assert (our synthPitchHumButton);
	GuiMenuItem_check (our synthPitchHumButton, our synthesisMethod == Manipulation_PITCH_HUM);
	Melder_assert (our synthPulsesPitchButton);
	GuiMenuItem_check (our synthPulsesPitchButton, our synthesisMethod == Manipulation_PULSES_PITCH);
	Melder_assert (our synthPulsesPitchHumButton);
	GuiMenuItem_check (our synthPulsesPitchHumButton, our synthesisMethod == Manipulation_PULSES_PITCH_HUM);
	Melder_assert (our synthOverlapAddButton);
	GuiMenuItem_check (our synthOverlapAddButton, our synthesisMethod == Manipulation_OVERLAPADD);
	Melder_assert (our synthPitchLpcButton);
	GuiMenuItem_check (our synthPitchLpcButton, our synthesisMethod == Manipulation_PITCH_LPC);
}

/***** FILE MENU *****/

static void CONVERT_DATA_TO_ONE__ExtractOriginalSound (ManipulationEditor me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		autoSound result = Data_copy (my soundArea() -> sound());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void CONVERT_DATA_TO_ONE__ExtractPulses (ManipulationEditor me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		autoPointProcess result = Data_copy (my pulsesArea() -> pulses());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void CONVERT_DATA_TO_ONE__ExtractPitchTier (ManipulationEditor me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		autoPitchTier result = Data_copy (my pitchTierArea() -> pitch());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void CONVERT_DATA_TO_ONE__ExtractDurationTier (ManipulationEditor me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		autoDurationTier result = Data_copy (my durationTierArea() -> duration());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void CONVERT_DATA_TO_ONE__ExtractManipulatedSound (ManipulationEditor me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		autoSound result = Manipulation_to_Sound (my manipulation(), my synthesisMethod);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

/***** EDIT MENU *****/

//void structManipulationEditor :: v_saveData () {
//	our previousPulses   = Data_copy (our manipulation() -> pulses.get());     // could be null
//	our previousPitch    = Data_copy (our manipulation() -> pitch.get());      // could be null
//	our previousDuration = Data_copy (our manipulation() -> duration.get());   // could be null
//}

//void structManipulationEditor :: v_restoreData () {
//	std::swap (our manipulation() -> pulses,   our previousPulses);     // could be null
//	std::swap (our manipulation() -> pitch,    our previousPitch);      // could be null
//	std::swap (our manipulation() -> duration, our previousDuration);   // could be null
//}


static void menu_cb_ManipulationEditorHelp (ManipulationEditor, EDITOR_ARGS) { Melder_help (U"ManipulationEditor"); }
static void menu_cb_ManipulationHelp (ManipulationEditor, EDITOR_ARGS) { Melder_help (U"Manipulation"); }

#define menu_cb_Synth_common(menu_cb,meth) \
static void menu_cb (ManipulationEditor me, EDITOR_ARGS) { \
	prefs_synthesisMethod = my synthesisMethod = meth; \
	my v_updateMenuItems (); /* once this change should be reflected graphically, use FunctionEditor_redraw() instead */ \
}
menu_cb_Synth_common (menu_cb_Synth_Pulses, Manipulation_PULSES)
menu_cb_Synth_common (menu_cb_Synth_Pulses_hum, Manipulation_PULSES_HUM)
menu_cb_Synth_common (menu_cb_Synth_Pulses_Lpc, Manipulation_PULSES_LPC)
menu_cb_Synth_common (menu_cb_Synth_Pitch, Manipulation_PITCH)
menu_cb_Synth_common (menu_cb_Synth_Pitch_hum, Manipulation_PITCH_HUM)
menu_cb_Synth_common (menu_cb_Synth_Pulses_Pitch, Manipulation_PULSES_PITCH)
menu_cb_Synth_common (menu_cb_Synth_Pulses_Pitch_hum, Manipulation_PULSES_PITCH_HUM)
menu_cb_Synth_common (menu_cb_Synth_OverlapAdd_nodur, Manipulation_OVERLAPADD_NODUR)
menu_cb_Synth_common (menu_cb_Synth_OverlapAdd, Manipulation_OVERLAPADD)
menu_cb_Synth_common (menu_cb_Synth_Pitch_Lpc, Manipulation_PITCH_LPC)

void structManipulationEditor :: v_createMenus () {
	ManipulationEditor_Parent :: v_createMenus ();

	Editor_addCommand (this, U"File", U"Extract original sound", 0,
			CONVERT_DATA_TO_ONE__ExtractOriginalSound);
	Editor_addCommand (this, U"File", U"Extract pulses", 0,
			CONVERT_DATA_TO_ONE__ExtractPulses);
	Editor_addCommand (this, U"File", U"Extract pitch tier", 0,
			CONVERT_DATA_TO_ONE__ExtractPitchTier);
	Editor_addCommand (this, U"File", U"Extract duration tier", 0,
			CONVERT_DATA_TO_ONE__ExtractDurationTier);
	Editor_addCommand (this, U"File", U"Publish resynthesis", 0,
			CONVERT_DATA_TO_ONE__ExtractManipulatedSound);
	Editor_addCommand (this, U"File", U"-- close --", 0, nullptr);

	Editor_addMenu (this, U"Synth", 0);
	our synthPulsesButton = Editor_addCommand (this, U"Synth", U"Pulses --", GuiMenu_RADIO_FIRST, menu_cb_Synth_Pulses);
	our synthPulsesHumButton = Editor_addCommand (this, U"Synth", U"Pulses (hum) --", GuiMenu_RADIO_NEXT, menu_cb_Synth_Pulses_hum);

	our synthPulsesLpcButton = Editor_addCommand (this, U"Synth", U"Pulses & LPC -- (\"LPC resynthesis\")", GuiMenu_RADIO_NEXT, menu_cb_Synth_Pulses_Lpc);
	Editor_addCommand (this, U"Synth", U"-- pitch resynth --", 0, nullptr);
	our synthPitchButton = Editor_addCommand (this, U"Synth", U" -- Pitch", GuiMenu_RADIO_NEXT, menu_cb_Synth_Pitch);
	our synthPitchHumButton = Editor_addCommand (this, U"Synth", U" -- Pitch (hum)", GuiMenu_RADIO_NEXT, menu_cb_Synth_Pitch_hum);
	our synthPulsesPitchButton = Editor_addCommand (this, U"Synth", U"Pulses -- Pitch", GuiMenu_RADIO_NEXT, menu_cb_Synth_Pulses_Pitch);
	our synthPulsesPitchHumButton = Editor_addCommand (this, U"Synth", U"Pulses -- Pitch (hum)", GuiMenu_RADIO_NEXT, menu_cb_Synth_Pulses_Pitch_hum);
	Editor_addCommand (this, U"Synth", U"-- full resynth --", 0, nullptr);
	our synthOverlapAddButton = Editor_addCommand (this, U"Synth", U"Sound & Pulses -- Pitch & Duration  (\"Overlap-add manipulation\")", GuiMenu_RADIO_NEXT | GuiMenu_TOGGLE_ON, menu_cb_Synth_OverlapAdd);
	our synthPitchLpcButton = Editor_addCommand (this, U"Synth", U"LPC -- Pitch  (\"LPC pitch manipulation\")", GuiMenu_RADIO_NEXT, menu_cb_Synth_Pitch_Lpc);
}

void structManipulationEditor :: v_createMenuItems_help (EditorMenu menu) {
	ManipulationEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"ManipulationEditor help", '?', menu_cb_ManipulationEditorHelp);
	EditorMenu_addCommand (menu, U"Manipulation help", 0, menu_cb_ManipulationHelp);
}

/********** DRAWING AREA **********/

void structManipulationPitchTierArea :: v_drawInside () {
	const bool cursorVisible = (
		our startSelection() == our endSelection() &&
		our startSelection() >= our startWindow() &&
		our startSelection() <= our endWindow()
	);
	Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), our ymin, our ymax);

	/*
		Draw pitch contour based on pulses.
	*/
	Graphics_setGrey (graphics(), 0.7);
	/* BUG: should include grey dots */
	constPointProcess pulses = ((ManipulationEditor) functionEditor()) -> manipulation() -> pulses.get();
	if (pulses) for (integer i = 1; i < pulses -> nt; i ++) {
		const double tleft = pulses -> t [i], tright = pulses -> t [i + 1], t = 0.5 * (tleft + tright);
		if (t >= startWindow() && t <= endWindow()) {
			if (tleft != tright) {
				const double y = 1.0 / (tright - tleft);
				if (y >= our ymin && y <= our ymax)
					Graphics_fillCircle_mm (our graphics(), t, y, 1.0);
			}
		}
	}
	Graphics_setGrey (graphics(), 0.0);

	constexpr double minimumFrequency = 50.0;
	FunctionEditor_drawGridLine (our functionEditor(), minimumFrequency);

	const integer n = ( our pitchTier() ? our pitchTier() -> points.size : 0 );
	if (cursorVisible && n > 0) {
		const double y = RealTier_getValueAtTime (our pitchTier(), our startSelection());
		FunctionEditor_insertCursorFunctionValue (our functionEditor(), y,
			Melder_fixed (y, 1), U" Hz",
			our ymin, our ymax);
	}
	our ManipulationPitchTierArea_Parent :: v_drawInside ();
}

void structManipulationDurationTierArea :: v_drawInside () {
	const bool cursorVisible = (
		our startSelection() == our endSelection() &&
		our startSelection() >= our startWindow() &&
		our startSelection() <= our endWindow()
	);
	Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), our ymin, our ymax);
	FunctionEditor_drawGridLine (our functionEditor(), 1.0);   // BUG: should move to FunctionArea
	if (cursorVisible && our durationTier() -> points.size > 0) {
		const double y = RealTier_getValueAtTime (our durationTier(), our startSelection());
		FunctionEditor_insertCursorFunctionValue (our functionEditor(), y,   // BUG: should move to FunctionArea
				Melder_fixed (y, 3), U"", our ymin, our ymax);
	}
	our ManipulationDurationTierArea_Parent :: v_drawInside ();
}

void structManipulationEditor :: v_play (double startTime, double endTime) {
	if (our clickWasModifiedByShiftKey) {
		Sound_playPart (our soundArea() -> sound(), startTime, endTime, theFunctionEditor_playCallback, this);
	} else {
		Manipulation_playPart (our manipulation(), startTime, endTime, our synthesisMethod);
	}
}

autoManipulationEditor ManipulationEditor_create (conststring32 title, Manipulation manipulation) {
	try {
		autoManipulationEditor me = Thing_new (ManipulationEditor);
		my soundArea() = ManipulationSoundArea_create (false, nullptr, me.get());
		my pulsesArea() = ManipulationPulsesArea_create (true, nullptr, me.get());
		my pitchTierArea() = ManipulationPitchTierArea_create (true, nullptr, me.get());
		my pitchTierArea() -> borrowedPulsesArea = my pulsesArea().get();
		my durationTierArea() = ManipulationDurationTierArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, manipulation);

		my synthesisMethod = prefs_synthesisMethod; // BUG: should be in v1_dataChanged()
		//my v_updateMenuItems ();  // BUG: should not be necessary
		return me;
	} catch (MelderError) {
		Melder_throw (U"Manipulation window not created.");
	}
}

/* End of file ManipulationEditor.cpp */
