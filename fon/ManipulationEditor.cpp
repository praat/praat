/* ManipulationEditor.cpp
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

#include "ManipulationEditor.h"
#include "PitchTier_to_PointProcess.h"
#include "Sound_to_PointProcess.h"
#include "Sound_to_Pitch.h"
#include "Pitch_to_PitchTier.h"
#include "Pitch_to_PointProcess.h"
#include "EditorM.h"

#include "enums_getText.h"
#include "ManipulationEditor_enums.h"
#include "enums_getValue.h"
#include "ManipulationEditor_enums.h"

Thing_implement (ManipulationEditor, FunctionEditor, 0);

#include "prefs_define.h"
#include "ManipulationEditor_prefs.h"
#include "prefs_install.h"
#include "ManipulationEditor_prefs.h"
#include "prefs_copyToInstance.h"
#include "ManipulationEditor_prefs.h"

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

static void updateMenus (ManipulationEditor me) {
	Melder_assert (my synthPulsesButton);
	GuiMenuItem_check (my synthPulsesButton, my synthesisMethod == Manipulation_PULSES);
	Melder_assert (my synthPulsesHumButton);
	GuiMenuItem_check (my synthPulsesHumButton, my synthesisMethod == Manipulation_PULSES_HUM);
	Melder_assert (my synthPulsesLpcButton);
	GuiMenuItem_check (my synthPulsesLpcButton, my synthesisMethod == Manipulation_PULSES_LPC);
	Melder_assert (my synthPitchButton);
	GuiMenuItem_check (my synthPitchButton, my synthesisMethod == Manipulation_PITCH);
	Melder_assert (my synthPitchHumButton);
	GuiMenuItem_check (my synthPitchHumButton, my synthesisMethod == Manipulation_PITCH_HUM);
	Melder_assert (my synthPulsesPitchButton);
	GuiMenuItem_check (my synthPulsesPitchButton, my synthesisMethod == Manipulation_PULSES_PITCH);
	Melder_assert (my synthPulsesPitchHumButton);
	GuiMenuItem_check (my synthPulsesPitchHumButton, my synthesisMethod == Manipulation_PULSES_PITCH_HUM);
	Melder_assert (my synthOverlapAddButton);
	GuiMenuItem_check (my synthOverlapAddButton, my synthesisMethod == Manipulation_OVERLAPADD);
	Melder_assert (my synthPitchLpcButton);
	GuiMenuItem_check (my synthPitchLpcButton, my synthesisMethod == Manipulation_PITCH_LPC);
}

/*
	The "sound area" contains the original sound and the pulses.
 */
static bool getSoundArea (ManipulationEditor me, double *ymin, double *ymax) {
	*ymin = 0.67;
	*ymax = 1.00;
	return my sound() || my pulses();
}

/********** MENU COMMANDS **********/

/***** FILE MENU *****/

static void menu_cb_extractOriginalSound (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	if (! my sound())
		return;
	autoSound publish = Data_copy (my sound().get());
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_extractPulses (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	if (! my pulses())
		return;
	autoPointProcess publish = Data_copy (my pulses().get());
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_extractPitchTier (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	if (! my pitch())
		return;
	autoPitchTier publish = Data_copy (my pitch().get());
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_extractDurationTier (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	if (! my duration())
		return;
	autoDurationTier publish = Data_copy (my duration().get());
	Editor_broadcastPublication (me, publish.move());
}

static void menu_cb_extractManipulatedSound (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	autoSound publish = Manipulation_to_Sound (my manipulation(), my synthesisMethod);
	Editor_broadcastPublication (me, publish.move());
}

/***** EDIT MENU *****/

void structManipulationEditor :: v_saveData () {
	our previousPulses   = Data_copy (our pulses().get());     // could be null
	our previousPitch    = Data_copy (our pitch().get());      // could be null
	our previousDuration = Data_copy (our duration().get());   // could be null
}

void structManipulationEditor :: v_restoreData () {
	std::swap (our pulses(),   our previousPulses);     // could be null
	std::swap (our pitch(),    our previousPitch);      // could be null
	std::swap (our duration(), our previousDuration);   // could be null
}

/***** PULSES MENU *****/

static void menu_cb_removePulses (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	if (! my pulses())
		return;
	Editor_save (me, U"Remove pulse(s)");
	if (my startSelection == my endSelection)
		PointProcess_removePointNear (my pulses().get(), my startSelection);
	else
		PointProcess_removePointsBetween (my pulses().get(), my startSelection, my endSelection);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPulseAtCursor (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	if (! my pulses()) return;
	Editor_save (me, U"Add pulse");
	PointProcess_addPoint (my pulses().get(), 0.5 * (my startSelection + my endSelection));
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPulseAt (ManipulationEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add pulse", nullptr)
		REAL (position, U"Position (s)", U"0.0")
	EDITOR_OK
		SET_REAL (position, 0.5 * (my startSelection + my endSelection))
	EDITOR_DO
		if (! my pulses())
			return;
		Editor_save (me, U"Add pulse");
		PointProcess_addPoint (my pulses().get(), position);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

/***** PITCH MENU *****/

static void menu_cb_removePitchPoints (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	if (! my pitch())
		return;
	Editor_save (me, U"Remove pitch point(s)");
	if (my startSelection == my endSelection)
		AnyTier_removePointNear (my pitch()->asAnyTier(), my startSelection);
	else
		AnyTier_removePointsBetween (my pitch()->asAnyTier(), my startSelection, my endSelection);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPitchPointAtCursor (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	if (! my pitch())
		return;
	Editor_save (me, U"Add pitch point");
	RealTier_addPoint (my pitch().get(), 0.5 * (my startSelection + my endSelection),
			my pitchTierArea -> v_yToValue (my pitchTierArea -> ycursor));
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addPitchPointAtSlice (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	if (! my pulses())
		Melder_throw (U"There are no pulses.");
	if (! my pitch())
		return;
	const integer ileft = PointProcess_getLowIndex (my pulses().get(), 0.5 * (my startSelection + my endSelection));
	const integer iright = ileft + 1, nt = my pulses() -> nt;
	constVEC t = my pulses() -> t.get();
	double desiredY = my pitchTierArea -> ycursor;   // default
	Editor_save (me, U"Add pitch point");
	if (nt <= 1) {
		/* Ignore. */
	} else if (ileft <= 0) {
		double tright = t [2] - t [1];
		if (tright > 0.0 && tright <= 0.02)
			desiredY = my pitchTierArea -> v_valueToY (1.0 / tright);
	} else if (iright > nt) {
		double tleft = t [nt] - t [nt - 1];
		if (tleft > 0.0 && tleft <= 0.02)
			desiredY = my pitchTierArea -> v_valueToY (1.0 / tleft);
	} else {   /* Three-period median. */
		double tmid = t [iright] - t [ileft], tleft = 0.0, tright = 0.0;
		if (ileft > 1)
			tleft = t [ileft] - t [ileft - 1];
		if (iright < nt)
			tright = t [iright + 1] - t [iright];
		if (tleft > 0.02)
			tleft = 0;
		if (tmid > 0.02)
			tmid = 0;
		if (tright > 0.02)
			tright = 0;
		/* Bubble-sort. */
		if (tmid < tleft)
			std::swap (tmid, tleft);
		if (tright < tleft)
			std::swap (tright, tleft);
		if (tright < tmid)
			std::swap (tright, tmid);
		if (tleft != 0.0)
			desiredY = my pitchTierArea -> v_valueToY (1 / tmid);   // median of 3
		else if (tmid != 0.0)
			desiredY = my pitchTierArea -> v_valueToY (2 / (tmid + tright));   // median of 2
		else if (tright != 0.0)
			desiredY = my pitchTierArea -> v_valueToY (1 / tright);   // median of 1
	}
	RealTierArea_addPointAt (my pitchTierArea.get(), my pitch().get(), 0.5 * (my startSelection + my endSelection), desiredY);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}	

static void menu_cb_addPitchPointAt (ManipulationEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add pitch point", nullptr)
		REAL (time, U"Time (s)", U"0.0")
		REAL (frequency, U"Frequency (Hz or st)", U"100.0")
	EDITOR_OK
		SET_REAL (time, 0.5 * (my startSelection + my endSelection))
		SET_REAL (frequency, my pitchTierArea -> ycursor)
	EDITOR_DO
		if (! my pitch())
			return;
		Editor_save (me, U"Add pitch point");
		RealTierArea_addPointAt (my pitchTierArea.get(), my pitch().get(), time, frequency);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_stylizePitch (ManipulationEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Stylize pitch", U"PitchTier: Stylize...")
		REAL (frequencyResolution, U"Frequency resolution", my default_pitch_stylize_frequencyResolution ())
		RADIO (units, U"Units", my default_pitch_stylize_useSemitones () + 1)
			RADIOBUTTON (U"Hertz")
			RADIOBUTTON (U"semitones")
	EDITOR_OK
		SET_REAL   (frequencyResolution, my p_pitch_stylize_frequencyResolution)
		SET_OPTION (units,               my p_pitch_stylize_useSemitones + 1)
	EDITOR_DO
		if (! my pitch())
			return;
		Editor_save (me, U"Stylize pitch");
		PitchTier_stylize (my pitch().get(),
			my pref_pitch_stylize_frequencyResolution () = my p_pitch_stylize_frequencyResolution = frequencyResolution,
			my pref_pitch_stylize_useSemitones        () = my p_pitch_stylize_useSemitones        = units - 1);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_stylizePitch_2st (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	if (! my pitch())
		return;
	Editor_save (me, U"Stylize pitch");
	PitchTier_stylize (my pitch().get(), 2.0, true);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_interpolateQuadratically (ManipulationEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Interpolate quadratically", nullptr)
		NATURAL (numberOfPointsPerParabola, U"Number of points per parabola", my default_pitch_interpolateQuadratically_numberOfPointsPerParabola ())
	EDITOR_OK
		SET_INTEGER (numberOfPointsPerParabola, my p_pitch_interpolateQuadratically_numberOfPointsPerParabola)
	EDITOR_DO
		if (! my pitch())
			return;
		Editor_save (me, U"Interpolate quadratically");
		RealTier_interpolateQuadratically (my pitch().get(),
			my pref_pitch_interpolateQuadratically_numberOfPointsPerParabola () = my p_pitch_interpolateQuadratically_numberOfPointsPerParabola = numberOfPointsPerParabola,
			my pitchTierArea -> p_units == kPitchTierArea_units::SEMITONES);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_interpolateQuadratically_4pts (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	if (! my pitch())
		return;
	Editor_save (me, U"Interpolate quadratically");
	RealTier_interpolateQuadratically (my pitch().get(), 4, my pitchTierArea -> p_units == kPitchTierArea_units::SEMITONES);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_shiftPitchFrequencies (ManipulationEditor me, EDITOR_ARGS_FORM) {
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
		Editor_save (me, U"Shift pitch frequencies");
		try {
			PitchTier_shiftFrequencies (my pitch().get(), my startSelection, my endSelection, frequencyShift, unit);
			FunctionEditor_redraw (me);
			Editor_broadcastDataChanged (me);
		} catch (MelderError) {
			// the PitchTier may have partially changed
			FunctionEditor_redraw (me);
			Editor_broadcastDataChanged (me);
			throw;
		}
	EDITOR_END
}

static void menu_cb_multiplyPitchFrequencies (ManipulationEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Multiply pitch frequencies", nullptr)
		POSITIVE (factor, U"Factor", U"1.2")
		LABEL (U"The multiplication is always done in hertz.")
	EDITOR_OK
	EDITOR_DO
		if (! my pitch())
			return;
		Editor_save (me, U"Multiply pitch frequencies");
		PitchTier_multiplyFrequencies (my pitch().get(), my startSelection, my endSelection, factor);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_setPitchRange (ManipulationEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Set pitch range", nullptr)
		/* BUG: should include Minimum */
		REAL (maximum, U"Maximum (Hz or st)", my pitchTierArea -> default_maximum ())
	EDITOR_OK
		SET_REAL (maximum, my pitchTierArea -> p_maximum)
	EDITOR_DO
		if (maximum <= my pitchTier.minPeriodic)
			Melder_throw (U"Maximum pitch should be greater than ",
				Melder_half (my pitchTier.minPeriodic), U" ", units_strings [(int) my pitchTierArea -> p_units], U".");
		my pitchTierArea -> ymax = my pitchTierArea -> pref_maximum () = my pitchTierArea -> p_maximum = maximum;
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_setPitchUnits (ManipulationEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Set pitch units", nullptr)
		RADIO_ENUM (kPitchTierArea_units, pitchUnits,
				U"Pitch units", my pitchTierArea -> default_units ())
	EDITOR_OK
		SET_ENUM (pitchUnits, kPitchTierArea_units, my pitchTierArea -> p_units)
	EDITOR_DO
		enum kPitchTierArea_units oldPitchUnits = my pitchTierArea -> p_units;
		my pitchTierArea -> pref_units () = my pitchTierArea -> p_units = pitchUnits;
		if (my pitchTierArea -> p_units == oldPitchUnits) return;
		if (my pitchTierArea -> p_units == kPitchTierArea_units::HERTZ) {
			my pitchTierArea -> p_minimum = 25.0;
			my pitchTier.minPeriodic = 50.0;
			my pitchTierArea -> ymax = my pitchTierArea -> pref_maximum () = my pitchTierArea -> p_maximum = NUMsemitonesToHertz (my pitchTierArea -> p_maximum);
			my pitchTierArea -> ycursor = NUMsemitonesToHertz (my pitchTierArea -> ycursor);
		} else {
			my pitchTierArea -> p_minimum = -24.0;
			my pitchTier.minPeriodic = -12.0;
			my pitchTierArea -> ymax = my pitchTierArea -> pref_maximum () = my pitchTierArea -> p_maximum = NUMhertzToSemitones (my pitchTierArea -> p_maximum);
			my pitchTierArea -> ycursor = NUMhertzToSemitones (my pitchTierArea -> ycursor);
		}
		FunctionEditor_redraw (me);
	EDITOR_END
}

/***** DURATION MENU *****/

static void menu_cb_setDurationRange (ManipulationEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Set duration range", nullptr)
		REAL (minimum, U"Minimum", my durationTierArea -> default_minimum ())
		REAL (maximum, U"Maximum", my durationTierArea -> default_maximum ())
	EDITOR_OK
		SET_REAL (minimum, my durationTierArea -> p_minimum)
		SET_REAL (maximum, my durationTierArea -> p_maximum)
	EDITOR_DO
		double minimumValue = ( my duration() ? RealTier_getMinimumValue (my duration().get()) : undefined );
		double maximumValue = ( my duration() ? RealTier_getMaximumValue (my duration().get()) : undefined );
		if (minimum > 1.0)
			Melder_throw (U"Minimum relative duration should not be greater than 1.");
		if (maximum < 1.0)
			Melder_throw (U"Maximum relative duration should not be less than 1.");
		if (minimum >= maximum)
			Melder_throw (U"Maximum relative duration should be greater than minimum.");
		if (isdefined (minimumValue) && minimum > minimumValue)
			Melder_throw (U"Minimum relative duration should not be greater than the minimum value present, "
				U"which is ", Melder_half (minimumValue), U".");
		if (isdefined (maximumValue) && maximum < maximumValue)
			Melder_throw (U"Maximum relative duration should not be less than the maximum value present, "
				U"which is ", Melder_half (maximumValue), U".");
		my durationTierArea -> ymin = my durationTierArea -> pref_minimum () = my durationTierArea -> p_minimum = minimum;
		my durationTierArea -> ymax = my durationTierArea -> pref_maximum () = my durationTierArea -> p_maximum = maximum;
		FunctionEditor_redraw (me);
	EDITOR_END
}

static void menu_cb_setDraggingStrategy (ManipulationEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Set dragging strategy", U"ManipulationEditor")
		RADIO_ENUM (kManipulationEditor_draggingStrategy, draggingStrategy,
				U"Dragging strategy", my default_pitch_draggingStrategy ())
	EDITOR_OK
		SET_ENUM (draggingStrategy, kManipulationEditor_draggingStrategy, my p_pitch_draggingStrategy)
	EDITOR_DO
		my pref_pitch_draggingStrategy () = my p_pitch_draggingStrategy = draggingStrategy;
	EDITOR_END
}

static void menu_cb_removeDurationPoints (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	if (! my duration())
		return;
	Editor_save (me, U"Remove duration point(s)");
	if (my startSelection == my endSelection)
		AnyTier_removePointNear (my duration()->asAnyTier(), 0.5 * (my startSelection + my endSelection));
	else
		AnyTier_removePointsBetween (my duration()->asAnyTier(), my startSelection, my endSelection);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addDurationPointAtCursor (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	if (! my duration())
		return;
	Editor_save (me, U"Add duration point");
	RealTier_addPoint (my duration().get(), 0.5 * (my startSelection + my endSelection), my durationTierArea -> ycursor);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_addDurationPointAt (ManipulationEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Add duration point", nullptr)
		REAL (time, U"Time (s)", U"0.0");
		REAL (relativeDuration, U"Relative duration", U"1.0");
	EDITOR_OK
		SET_REAL (time, 0.5 * (my startSelection + my endSelection))
	EDITOR_DO
		if (! my duration())
			return;
		Editor_save (me, U"Add duration point");
		RealTier_addPoint (my duration().get(), time, relativeDuration);
		FunctionEditor_redraw (me);
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_newDuration (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"New duration");
	my duration() = DurationTier_create (my manipulation() -> xmin, my manipulation() -> xmax);
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}

static void menu_cb_forgetDuration (ManipulationEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Forget duration");
	my duration() = autoDurationTier();
	FunctionEditor_redraw (me);
	Editor_broadcastDataChanged (me);
}
	
static void menu_cb_ManipulationEditorHelp (ManipulationEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"ManipulationEditor"); }
static void menu_cb_ManipulationHelp (ManipulationEditor, EDITOR_ARGS_DIRECT) { Melder_help (U"Manipulation"); }

#define menu_cb_Synth_common(menu_cb,meth) \
static void menu_cb (ManipulationEditor me, EDITOR_ARGS_DIRECT) { \
	prefs_synthesisMethod = my synthesisMethod = meth; \
	updateMenus (me); \
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

	Editor_addCommand (this, U"File", U"Extract original sound", 0, menu_cb_extractOriginalSound);
	Editor_addCommand (this, U"File", U"Extract pulses", 0, menu_cb_extractPulses);
	Editor_addCommand (this, U"File", U"Extract pitch tier", 0, menu_cb_extractPitchTier);
	Editor_addCommand (this, U"File", U"Extract duration tier", 0, menu_cb_extractDurationTier);
	Editor_addCommand (this, U"File", U"Publish resynthesis", 0, menu_cb_extractManipulatedSound);
	Editor_addCommand (this, U"File", U"-- close --", 0, nullptr);

	Editor_addMenu (this, U"Pulse", 0);
	Editor_addCommand (this, U"Pulse", U"Add pulse at cursor", 'P', menu_cb_addPulseAtCursor);
	Editor_addCommand (this, U"Pulse", U"Add pulse at...", 0, menu_cb_addPulseAt);
	Editor_addCommand (this, U"Pulse", U"-- remove pulses --", 0, nullptr);
	Editor_addCommand (this, U"Pulse", U"Remove pulse(s)", GuiMenu_OPTION | 'P', menu_cb_removePulses);

	Editor_addMenu (this, U"Pitch", 0);
	Editor_addCommand (this, U"Pitch", U"Add pitch point at cursor", 'T', menu_cb_addPitchPointAtCursor);
	Editor_addCommand (this, U"Pitch", U"Add pitch point at time slice", 0, menu_cb_addPitchPointAtSlice);
	Editor_addCommand (this, U"Pitch", U"Add pitch point at...", 0, menu_cb_addPitchPointAt);
	Editor_addCommand (this, U"Pitch", U"-- remove pitch --", 0, nullptr);
	Editor_addCommand (this, U"Pitch", U"Remove pitch point(s)", GuiMenu_OPTION | 'T', menu_cb_removePitchPoints);
	Editor_addCommand (this, U"Pitch", U"-- pitch prefs --", 0, nullptr);
	Editor_addCommand (this, U"Pitch", U"Set pitch range...", 0, menu_cb_setPitchRange);
	Editor_addCommand (this, U"Pitch", U"Set pitch units...", 0, menu_cb_setPitchUnits);
	Editor_addCommand (this, U"Pitch", U"Set pitch dragging strategy...", 0, menu_cb_setDraggingStrategy);
	Editor_addCommand (this, U"Pitch", U"-- modify pitch --", 0, nullptr);
	Editor_addCommand (this, U"Pitch", U"Shift pitch frequencies...", 0, menu_cb_shiftPitchFrequencies);
	Editor_addCommand (this, U"Pitch", U"Multiply pitch frequencies...", 0, menu_cb_multiplyPitchFrequencies);
	Editor_addCommand (this, U"Pitch", U"All:", GuiMenu_INSENSITIVE, menu_cb_stylizePitch);
	Editor_addCommand (this, U"Pitch", U"Stylize pitch...", 0, menu_cb_stylizePitch);
	Editor_addCommand (this, U"Pitch", U"Stylize pitch (2 st)", '2', menu_cb_stylizePitch_2st);
	Editor_addCommand (this, U"Pitch", U"Interpolate quadratically...", 0, menu_cb_interpolateQuadratically);
	Editor_addCommand (this, U"Pitch", U"Interpolate quadratically (4 pts)", '4', menu_cb_interpolateQuadratically_4pts);

	Editor_addMenu (this, U"Dur", 0);
	Editor_addCommand (this, U"Dur", U"Add duration point at cursor", 'D', menu_cb_addDurationPointAtCursor);
	Editor_addCommand (this, U"Dur", U"Add duration point at...", 0, menu_cb_addDurationPointAt);
	Editor_addCommand (this, U"Dur", U"-- remove duration --", 0, nullptr);
	Editor_addCommand (this, U"Dur", U"Remove duration point(s)", GuiMenu_OPTION | 'D', menu_cb_removeDurationPoints);
	Editor_addCommand (this, U"Dur", U"-- duration prefs --", 0, nullptr);
	Editor_addCommand (this, U"Dur", U"Set duration range...", 0, menu_cb_setDurationRange);
	Editor_addCommand (this, U"Dur", U"-- refresh duration --", 0, nullptr);
	Editor_addCommand (this, U"Dur", U"New duration", 0, menu_cb_newDuration);
	Editor_addCommand (this, U"Dur", U"Forget duration", 0, menu_cb_forgetDuration);

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

void structManipulationEditor :: v_createHelpMenuItems (EditorMenu menu) {
	ManipulationEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"ManipulationEditor help", '?', menu_cb_ManipulationEditorHelp);
	EditorMenu_addCommand (menu, U"Manipulation help", 0, menu_cb_ManipulationHelp);
}

/********** DRAWING AREA **********/

static void drawSoundArea (ManipulationEditor me, double ymin, double ymax) {
	Graphics_Viewport viewport = Graphics_insetViewport (my graphics.get(), 0.0, 1.0, ymin, ymax);
	Graphics_setWindow (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics.get(), Melder_WHITE);
	Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics.get(), Melder_BLACK);
	Graphics_rectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_TOP);
	Graphics_setFont (my graphics.get(), kGraphics_font::TIMES);
	Graphics_text (my graphics.get(), 1.0, 1.0, U"%%Sound");
	Graphics_setColour (my graphics.get(), Melder_BLUE);
	Graphics_text (my graphics.get(), 1.0, 1.0 - Graphics_dyMMtoWC (my graphics.get(), 3), U"%%Pulses");
	Graphics_setFont (my graphics.get(), kGraphics_font::HELVETICA);

	/*
		Draw blue pulses.
	*/
	if (my pulses()) {
		Graphics_setWindow (my graphics.get(), my startWindow, my endWindow, 0.0, 1.0);
		Graphics_setColour (my graphics.get(), Melder_BLUE);
		for (integer i = 1; i <= my pulses() -> nt; i ++) {
			double t = my pulses() -> t [i];
			if (t >= my startWindow && t <= my endWindow)
				Graphics_line (my graphics.get(), t, 0.05, t, 0.95);
		}
	}

	/*
		Draw sound.
	*/
	integer first, last;
	if (my sound() && Sampled_getWindowSamples (my sound().get(), my startWindow, my endWindow, & first, & last) > 1) {
		double minimum, maximum, scaleMin, scaleMax;
		Matrix_getWindowExtrema (my sound().get(), first, last, 1, 1, & minimum, & maximum);
		if (minimum == maximum) {
			minimum = -0.5;
			maximum = +0.5;
		}
		/*
			Scaling.
		*/
		scaleMin = 0.83 * minimum + 0.17 * my soundmin;
		scaleMax = 0.83 * maximum + 0.17 * my soundmax;
		Graphics_setWindow (my graphics.get(), my startWindow, my endWindow, scaleMin, scaleMax);
		FunctionEditor_drawRangeMark (me, scaleMin, Melder_float (Melder_half (scaleMin)), U"", Graphics_BOTTOM);
		FunctionEditor_drawRangeMark (me, scaleMax, Melder_float (Melder_half (scaleMax)), U"", Graphics_TOP);
		/*
			Draw dotted zero line.
		*/
		if (minimum < 0.0 && maximum > 0.0) {
			Graphics_setColour (my graphics.get(), Melder_CYAN);
			Graphics_setLineType (my graphics.get(), Graphics_DOTTED);
			Graphics_line (my graphics.get(), my startWindow, 0.0, my endWindow, 0.0);
			Graphics_setLineType (my graphics.get(), Graphics_DRAWN);
		}
		/*
			Draw samples.
		*/
		Graphics_setColour (my graphics.get(), Melder_BLACK);
		Graphics_function (my graphics.get(), & my sound() -> z [1] [0], first, last,
				Sampled_indexToX (my sound().get(), first), Sampled_indexToX (my sound().get(), last));
	}

	Graphics_resetViewport (my graphics.get(), viewport);
}

static void drawPitchArea (ManipulationEditor me) {
	const integer n = ( my pitch() ? my pitch() -> points.size : 0 );
	const bool cursorVisible = ( my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow );
	const double minimumFrequency = my pitchTierArea -> v_valueToY (50.0);
	const int rangePrecisions [] = { 0, 1, 2 };
	static const conststring32 rangeUnits [] = { U"", U" Hz", U" st" };

	my pitchTierArea -> setViewport();

	Graphics_setWindow (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics.get(), Melder_WHITE);
	Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics.get(), Melder_BLACK);
	Graphics_rectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);

	Graphics_setColour (my graphics.get(), Melder_BLUE);
	Graphics_setFont (my graphics.get(), kGraphics_font::TIMES);
	Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_TOP);
	Graphics_text (my graphics.get(), 1.0, 1.0, U"%%Pitch manip");
	Graphics_setGrey (my graphics.get(), 0.7);
	Graphics_text (my graphics.get(), 1.0, 1.0 - Graphics_dyMMtoWC (my graphics.get(), 3), U"%%Pitch from pulses");
	Graphics_setFont (my graphics.get(), kGraphics_font::HELVETICA);

	Graphics_setWindow (my graphics.get(), my startWindow, my endWindow, my pitchTierArea -> p_minimum, my pitchTierArea -> p_maximum);

	/*
		Draw pitch contour based on pulses.
	*/
	Graphics_setGrey (my graphics.get(), 0.7);
	if (my pulses()) for (integer i = 1; i < my pulses() -> nt; i ++) {
		const double tleft = my pulses() -> t [i], tright = my pulses() -> t [i + 1], t = 0.5 * (tleft + tright);
		if (t >= my startWindow && t <= my endWindow) {
			if (tleft != tright) {
				const double f = my pitchTierArea -> v_valueToY (1 / (tright - tleft));
				if (f >= my pitchTier.minPeriodic && f <= my pitchTierArea -> p_maximum)
					Graphics_fillCircle_mm (my graphics.get(), t, f, 1);
			}
		}
	}
	Graphics_setGrey (my graphics.get(), 0.0);

	FunctionEditor_drawGridLine (me, minimumFrequency);
	if (cursorVisible && n > 0) {
		const double y = my pitchTierArea -> v_valueToY (RealTier_getValueAtTime (my pitch().get(), my startSelection));
		FunctionEditor_insertCursorFunctionValue (me, y,
			Melder_fixed (y, rangePrecisions [(int) my pitchTierArea -> p_units]), rangeUnits [(int) my pitchTierArea -> p_units],
			my pitchTierArea -> p_minimum, my pitchTierArea -> p_maximum);
	}
	RealTierArea_draw (my pitchTierArea.get(), my pitch().get());
	if (isdefined (my pitchTierArea -> anchorTime))
		RealTierArea_drawWhileDragging (my pitchTierArea.get(), my pitch().get());

	Graphics_setColour (my graphics.get(), Melder_BLACK);
}

static void drawDurationArea (ManipulationEditor me) {
	DurationTier duration = my manipulation() -> duration.get();
	const bool cursorVisible = ( my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow );

	my durationTierArea -> setViewport();

	Graphics_setWindow (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics.get(), Melder_WHITE);
	Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics.get(), Melder_BLACK);
	Graphics_rectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);

	Graphics_setColour (my graphics.get(), Melder_BLUE);
	Graphics_setFont (my graphics.get(), kGraphics_font::TIMES);
	Graphics_setTextAlignment (my graphics.get(), Graphics_RIGHT, Graphics_TOP);
	Graphics_text (my graphics.get(), 1.0, 1.0, U"%%Duration manip");
	Graphics_setFont (my graphics.get(), kGraphics_font::HELVETICA);

	Graphics_setWindow (my graphics.get(), my startWindow, my endWindow, my durationTierArea -> p_minimum, my durationTierArea -> p_maximum);
	FunctionEditor_drawGridLine (me, 1.0);
	//FunctionEditor_drawRangeMark (me, my durationTierArea -> p_maximum, Melder_fixed (my durationTierArea -> p_maximum, 3), U"", Graphics_HALF);
	//FunctionEditor_drawRangeMark (me, my durationTierArea -> p_minimum, Melder_fixed (my durationTierArea -> p_minimum, 3), U"", Graphics_HALF);
	//if (my startSelection == my endSelection && my durationTierArea -> ycursor >= my durationTierArea -> p_minimum && my durationTierArea -> ycursor <= my durationTierArea -> p_maximum)
	//	FunctionEditor_drawHorizontalHair (me, my durationTierArea -> ycursor, Melder_fixed (my durationTierArea -> ycursor, 3), U"");
	if (cursorVisible && duration -> points.size > 0) {
		const double y = RealTier_getValueAtTime (duration, my startSelection);
		FunctionEditor_insertCursorFunctionValue (me, y, Melder_fixed (y, 3), U"", my durationTierArea -> p_minimum, my durationTierArea -> p_maximum);
	}

	Graphics_setWindow (my graphics.get(), my startWindow, my endWindow, my durationTierArea -> p_minimum, my durationTierArea -> p_maximum);
	RealTierArea_draw (my durationTierArea.get(), duration);
	if (isdefined (my durationTierArea -> anchorTime))
		RealTierArea_drawWhileDragging (my durationTierArea.get(), duration);

	Graphics_setLineWidth (my graphics.get(), 1.0);
	Graphics_setColour (my graphics.get(), Melder_BLACK);
}

void structManipulationEditor :: v_draw () {
	double ysoundmin, ysoundmax;
	(void) getSoundArea (this, & ysoundmin, & ysoundmax);
	if (our sound())
		drawSoundArea (this, ysoundmin, ysoundmax);
	if (our pitch())
		drawPitchArea (this);
	if (our duration())
		drawDurationArea (this);
	updateMenus (this);
}

bool structManipulationEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double globalY_fraction) {
	static bool clickedInWidePitchArea = false;
	static bool clickedInWideDurationArea = false;
	if (event -> isClick()) {
		clickedInWidePitchArea = our pitchTierArea -> y_fraction_globalIsInside (globalY_fraction);
		clickedInWideDurationArea = our durationTierArea -> y_fraction_globalIsInside (globalY_fraction);
	}
	bool result = false;
	if (clickedInWidePitchArea) {
		our pitchTierArea -> setViewport ();
		result = RealTierArea_mouse (our pitchTierArea.get(), our manipulation() -> pitch.get(), event, x_world, globalY_fraction);
		our pitchTierArea -> p_minimum = our pitchTierArea -> ymin;
		our pitchTierArea -> p_maximum = our pitchTierArea -> ymax;
	} else if (clickedInWideDurationArea) {
		our durationTierArea -> setViewport ();
		result = RealTierArea_mouse (our durationTierArea.get(), our manipulation() -> duration.get(), event, x_world, globalY_fraction);
		our durationTierArea -> p_minimum = our durationTierArea -> ymin;
		our durationTierArea -> p_maximum = our durationTierArea -> ymax;
	} else {
		result = our ManipulationEditor_Parent :: v_mouseInWideDataView (event, x_world, globalY_fraction);
	}
	if (event -> isDrop()) {
		clickedInWidePitchArea = false;
		clickedInWideDurationArea = false;
	}
	return result;
}

void structManipulationEditor :: v_play (double startTime, double endTime) {
	if (our clickWasModifiedByShiftKey) {
		if (our manipulation() -> sound)
			Sound_playPart (our manipulation() -> sound.get(), startTime, endTime, theFunctionEditor_playCallback, this);
	} else {
		Manipulation_playPart (our manipulation(), startTime, endTime, our synthesisMethod);
	}
}

autoManipulationEditor ManipulationEditor_create (conststring32 title, Manipulation manipulation) {
	try {
		autoManipulationEditor me = Thing_new (ManipulationEditor);
		FunctionEditor_init (me.get(), title, manipulation);
		my pitchTierArea = PitchTierArea_create (me.get(), ( manipulation -> duration ? 0.17 : 0.0 ), 0.67);
		if (manipulation -> duration) {
			my durationTierArea = DurationTierArea_create (me.get(), 0.0, 0.17);
		}

		const double maximumPitchValue = RealTier_getMaximumValue (manipulation -> pitch.get());
		if (my pitchTierArea -> p_units == kPitchTierArea_units::HERTZ) {
			my pitchTierArea -> ymin = my pitchTierArea -> p_minimum = 25.0;
			my pitchTier.minPeriodic = 50.0;
			my pitchTierArea -> p_maximum = maximumPitchValue;
			my pitchTierArea -> ycursor = my pitchTierArea -> p_maximum * 0.8;
			my pitchTierArea -> ymax = my pitchTierArea -> p_maximum *= 1.2;
		} else if (my pitchTierArea -> p_units == kPitchTierArea_units::SEMITONES) {
			my pitchTierArea -> ymin = my pitchTierArea -> p_minimum = -24.0;
			my pitchTier.minPeriodic = -12.0;
			my pitchTierArea -> p_maximum = ( isdefined (maximumPitchValue) ? NUMhertzToSemitones (maximumPitchValue) : undefined );
			my pitchTierArea -> ycursor = my pitchTierArea -> p_maximum - 4.0;
			my pitchTierArea -> ymax = my pitchTierArea -> p_maximum *= 3.0;
		} else
			Melder_fatal (U"ManipulationEditor_create: Unknown pitch units: ", (int) my pitchTierArea -> p_units);
		if (isundef (my pitchTierArea -> p_maximum) || my pitchTierArea -> p_maximum < my pitchTierArea -> pref_maximum())
			my pitchTierArea -> ymax = my pitchTierArea -> p_maximum = my pitchTierArea -> pref_maximum();

		/*
			If needed, fix preferences to sane values.
		*/
		if (my durationTierArea -> pref_minimum() > 1.0)
			my durationTierArea -> pref_minimum() = Melder_atof (my durationTierArea -> default_minimum());   // sanity
		if (my durationTierArea -> pref_maximum() < 1.0)
			my durationTierArea -> pref_maximum() = Melder_atof (my durationTierArea -> default_maximum());
		Melder_assert (my durationTierArea -> pref_minimum() < my durationTierArea -> pref_maximum());
		/*
			Honour preferences.
		*/
		my durationTierArea -> ymin = my durationTierArea -> p_minimum = my durationTierArea -> pref_minimum();
		my durationTierArea -> ymax = my durationTierArea -> p_maximum = my durationTierArea -> pref_maximum();
		/*
			If needed, widen on the basis of the data.
		*/
		const double minimumDurationValue = ( manipulation -> duration ? RealTier_getMinimumValue (manipulation -> duration.get()) : undefined );
		const double maximumDurationValue = ( manipulation -> duration ? RealTier_getMaximumValue (manipulation -> duration.get()) : undefined );
		if (minimumDurationValue < my durationTierArea -> p_minimum)   // NaN-safe
			my durationTierArea -> ymin = my durationTierArea -> p_minimum = minimumDurationValue / 1.25;
		if (maximumDurationValue > my durationTierArea -> p_maximum)   // NaN-safe
			my durationTierArea -> ymax = my durationTierArea -> p_maximum = minimumDurationValue * 1.25;

		my durationTierArea -> ycursor = 1.0;

		my synthesisMethod = prefs_synthesisMethod;
		if (manipulation -> sound)
			Matrix_getWindowExtrema (manipulation -> sound.get(), 0, 0, 0, 0, & my soundmin, & my soundmax);
		if (my soundmin == my soundmax) {
			my soundmin = -1.0;
			my soundmax = +1.0;
		}
		RealTierArea_updateScaling (my pitchTierArea.get(), manipulation -> pitch.get());
		if (manipulation -> duration) {
			RealTierArea_updateScaling (my durationTierArea.get(), manipulation -> duration.get());
			my durationTierArea -> p_minimum = my durationTierArea -> ymin;
			my durationTierArea -> p_maximum = my durationTierArea -> ymax;
		}
		updateMenus (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Manipulation window not created.");
	}
}

/* End of file ManipulationEditor.cpp */
