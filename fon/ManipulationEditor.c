/* ManipulationEditor.c
 *
 * Copyright (C) 1992-2008 Paul Boersma
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
 * pb 2002/10/06 improved visibility of dragging
 * pb 2003/11/20 PitchTier: Interpolate quadratically...
 * pb 2004/04/13 less flashing
 * pb 2006/01/02 removed bug in Shift Frequencies: wrong option list
 * pb 2006/12/08 better NUMundefined pitch and duration range checking
 * pb 2007/06/10 wchar_t
 * pb 2007/08/12 wchar_t
 * pb 2007/11/30 erased Graphics_printf
 * pb 2008/03/20 split off Help menu
 * pb 2008/03/21 new Editor API
 */

#include "ManipulationEditor.h"
#include "Preferences.h"
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

/*
 * How to add a synthesis method (in an interruptable order):
 * 1. add an Manipulation_ #define in Manipulation.h;
 * 2. add a synthesize_ routine in Manipulation.c, and a reference to it in Manipulation_to_Sound;
 * 3. add a button in ManipulationEditor.h;
 * 4. add a cb_Synth_ callback.
 * 5. create the button in createMenus and update updateMenus;
 */

static const wchar_t *units_strings [] = { 0, L"Hz", L"st" };

static struct {
	struct {
		double minimum, maximum;
		enum kManipulationEditor_pitchUnits units;
		enum kManipulationEditor_draggingStrategy draggingStrategy;
		struct { double frequencyResolution; bool useSemitones; } stylize;
		struct { long numberOfPointsPerParabola; } interpolateQuadratically;
	} pitchTier;
	struct { double minimum, maximum; } duration;
} preferences;

static int prefs_synthesisMethod = Manipulation_OVERLAPADD;   /* Remembered across editor creations, not across Praat sessions. */

/* BUG: 25 should be fmin */
#define YLIN(freq)  (my pitchTier.units == kManipulationEditor_pitchUnits_HERTZ ? ((freq) < 25 ? 25 : (freq)) : NUMhertzToSemitones ((freq) < 25 ? 25 : (freq)))
#define YLININV(freq)  (my pitchTier.units == kManipulationEditor_pitchUnits_HERTZ ? (freq) : NUMsemitonesToHertz (freq))

void ManipulationEditor_prefs (void) {
	Preferences_addDouble (L"ManipulationEditor.pitch.minimum", & preferences.pitchTier.minimum, 50.0);
	Preferences_addDouble (L"ManipulationEditor.pitch.maximum", & preferences.pitchTier.maximum, 300.0);
	Preferences_addEnum (L"ManipulationEditor.pitch.units", & preferences.pitchTier.units, kManipulationEditor_pitchUnits, DEFAULT);
	Preferences_addEnum (L"ManipulationEditor.pitch.draggingStrategy", & preferences.pitchTier.draggingStrategy, kManipulationEditor_draggingStrategy, DEFAULT);
	Preferences_addDouble (L"ManipulationEditor.pitch.stylize.frequencyResolution", & preferences.pitchTier.stylize.frequencyResolution, 2.0);
	Preferences_addBool (L"ManipulationEditor.pitch.stylize.useSemitones", & preferences.pitchTier.stylize.useSemitones, true);
	Preferences_addLong (L"ManipulationEditor.pitch.interpolateQuadratically.numberOfPointsPerParabola", & preferences.pitchTier.interpolateQuadratically.numberOfPointsPerParabola, 4);
	Preferences_addDouble (L"ManipulationEditor.duration.minimum", & preferences.duration.minimum, 0.25);
	Preferences_addDouble (L"ManipulationEditor.duration.maximum", & preferences.duration.maximum, 3.0);
	/*Preferences_addInt (L"ManipulationEditor.synthesis.method.1", & prefs_synthesisMethod, Manipulation_OVERLAPADD);*/
}

static void updateMenus (ManipulationEditor me) {
	Melder_assert (my synthPulsesButton != NULL);
	GuiMenuItem_check (my synthPulsesButton, my synthesisMethod == Manipulation_PULSES);
	Melder_assert (my synthPulsesHumButton != NULL);
	GuiMenuItem_check (my synthPulsesHumButton, my synthesisMethod == Manipulation_PULSES_HUM);
	Melder_assert (my synthPulsesLpcButton != NULL);
	GuiMenuItem_check (my synthPulsesLpcButton, my synthesisMethod == Manipulation_PULSES_LPC);
	Melder_assert (my synthPitchButton != NULL);
	GuiMenuItem_check (my synthPitchButton, my synthesisMethod == Manipulation_PITCH);
	Melder_assert (my synthPitchHumButton != NULL);
	GuiMenuItem_check (my synthPitchHumButton, my synthesisMethod == Manipulation_PITCH_HUM);
	Melder_assert (my synthPulsesPitchButton != NULL);
	GuiMenuItem_check (my synthPulsesPitchButton, my synthesisMethod == Manipulation_PULSES_PITCH);
	Melder_assert (my synthPulsesPitchHumButton != NULL);
	GuiMenuItem_check (my synthPulsesPitchHumButton, my synthesisMethod == Manipulation_PULSES_PITCH_HUM);
	Melder_assert (my synthOverlapAddButton != NULL);
	GuiMenuItem_check (my synthOverlapAddButton, my synthesisMethod == Manipulation_OVERLAPADD);
	Melder_assert (my synthPitchLpcButton != NULL);
	GuiMenuItem_check (my synthPitchLpcButton, my synthesisMethod == Manipulation_PITCH_LPC);
}

/*
 * The "sound area" contains the original sound and the pulses.
 */
static int getSoundArea (ManipulationEditor me, double *ymin, double *ymax) {
	Manipulation ana = my data;
	*ymin = 0.66;
	*ymax = 1.00;
	return ana -> sound != NULL || ana -> pulses != NULL;
}
/*
 * The "pitch area" contains the grey pitch analysis based on the pulses, and the blue pitch tier.
 */
static int getPitchArea (ManipulationEditor me, double *ymin, double *ymax) {
	Manipulation ana = my data;
	*ymin = ana -> duration ? 0.16 : 0.00;
	*ymax = 0.65;
	return ana -> pulses != NULL || ana -> pitch != NULL;
}
static int getDurationArea (ManipulationEditor me, double *ymin, double *ymax) {
	Manipulation ana = my data;
	if (! ana -> duration) return FALSE;
	*ymin = 0.00;
	*ymax = 0.15;
	return TRUE;
}

/********** DESTRUCTION **********/

static void destroy (I) {
	iam (ManipulationEditor);
	forget (my previousPulses);
	forget (my previousPitch);
	forget (my previousDuration);
	inherited (ManipulationEditor) destroy (me);
}

/********** MENU COMMANDS **********/

/***** FILE MENU *****/

#define menu_cb_extract_common(menu_cb,obj) \
static int menu_cb (EDITOR_ARGS) { \
	EDITOR_IAM (ManipulationEditor); \
	Manipulation ana = my data; \
	if (! ana -> obj) return 0; \
	if (my publishCallback) { \
		Data publish = Data_copy (ana -> obj); \
		if (! publish) return 0; \
		my publishCallback (me, my publishClosure, publish); \
	} \
	return 1; \
}
menu_cb_extract_common (menu_cb_extractOriginalSound, sound)
menu_cb_extract_common (menu_cb_extractPulses, pulses)
menu_cb_extract_common (menu_cb_extractPitchTier, pitch)
menu_cb_extract_common (menu_cb_extractDurationTier, duration)

static int menu_cb_extractManipulatedSound (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	Manipulation ana = my data;
	if (my publishCallback) {
		Sound publish = Manipulation_to_Sound (ana, my synthesisMethod);
		if (! publish) return 0;
		my publishCallback (me, my publishClosure, publish);
	}
	return 1;
}

/***** EDIT MENU *****/

static void save (ManipulationEditor me) {
	Manipulation ana = my data;
	forget (my previousPulses);
	forget (my previousPitch);
	forget (my previousDuration);
	if (ana -> pulses) my previousPulses = Data_copy (ana -> pulses);
	if (ana -> pitch) my previousPitch = Data_copy (ana -> pitch);
	if (ana -> duration) my previousDuration = Data_copy (ana -> duration);
}

static void restore (ManipulationEditor me) {
	Manipulation ana = my data;
	Any dummy;
	dummy = ana -> pulses; ana -> pulses = my previousPulses; my previousPulses = dummy;
	dummy = ana -> pitch; ana -> pitch = my previousPitch; my previousPitch = dummy;
	dummy = ana -> duration; ana -> duration = my previousDuration; my previousDuration = dummy;
}

/***** PULSES MENU *****/

static int menu_cb_removePulses (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);	
	Manipulation ana = my data;
	if (! ana -> pulses) return 0;
	Editor_save (ManipulationEditor_as_Editor (me), L"Remove pulse(s)");
	if (my startSelection == my endSelection)
		PointProcess_removePointNear (ana -> pulses, my startSelection);
	else
		PointProcess_removePointsBetween (ana -> pulses, my startSelection, my endSelection);
	FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
	Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	return 1;
}

static int menu_cb_addPulseAtCursor (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	Manipulation ana = my data;
	if (! ana -> pulses) return 0;
	Editor_save (ManipulationEditor_as_Editor (me), L"Add pulse");
	PointProcess_addPoint (ana -> pulses, 0.5 * (my startSelection + my endSelection));
	FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
	Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	return 1;
}

static int menu_cb_addPulseAt (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	EDITOR_FORM (L"Add pulse", 0)
		REAL (L"Position (s)", L"0.0")
	EDITOR_OK
		SET_REAL (L"Position", 0.5 * (my startSelection + my endSelection))
	EDITOR_DO
		Manipulation ana = my data;
		if (! ana -> pulses) return 0;
		Editor_save (ManipulationEditor_as_Editor (me), L"Add pulse");
		PointProcess_addPoint (ana -> pulses, GET_REAL (L"Position"));
		FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
		Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	EDITOR_END
}

/***** PITCH MENU *****/

static int menu_cb_removePitchPoints (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	Manipulation ana = my data;
	if (! ana -> pitch) return 0;
	Editor_save (ManipulationEditor_as_Editor (me), L"Remove pitch point(s)");
	if (my startSelection == my endSelection)
		AnyTier_removePointNear (ana -> pitch, my startSelection);
	else
		AnyTier_removePointsBetween (ana -> pitch, my startSelection, my endSelection);
	FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
	Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	return 1;
}

static int menu_cb_addPitchPointAtCursor (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	Manipulation ana = my data;
	if (! ana -> pitch) return 0;
	Editor_save (ManipulationEditor_as_Editor (me), L"Add pitch point");
	RealTier_addPoint (ana -> pitch, 0.5 * (my startSelection + my endSelection), YLININV (my pitchTier.cursor));
	FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
	Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	return 1;
}

static int menu_cb_addPitchPointAtSlice (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	Manipulation ana = my data;
	PointProcess pulses = ana -> pulses;
	long ileft, iright, nt;
	double *t, f;
	if (! pulses) return Melder_error1 (L"There are no pulses.");
	if (! ana -> pitch) return 0;
	ileft = PointProcess_getLowIndex (pulses, 0.5 * (my startSelection + my endSelection)), iright = ileft + 1, nt = pulses -> nt;
	t = pulses -> t, f = my pitchTier.cursor;   /* Default. */
	Editor_save (ManipulationEditor_as_Editor (me), L"Add pitch point");
	if (nt <= 1) {
		/* Ignore. */
	} else if (ileft <= 0) {
		double tright = t [2] - t [1];
		if (tright > 0.0 && tright <= 0.02) f = YLIN (1.0 / tright);
	} else if (iright > nt) {
		double tleft = t [nt] - t [nt - 1];
		if (tleft > 0.0 && tleft <= 0.02) f = YLIN (1.0 / tleft);
	} else {   /* Three-period median. */
		double tmid = t [iright] - t [ileft], tleft = 0.0, tright = 0.0;
		if (ileft > 1) tleft = t [ileft] - t [ileft - 1];
		if (iright < nt) tright = t [iright + 1] - t [iright];
		if (tleft > 0.02) tleft = 0;
		if (tmid > 0.02) tmid = 0;
		if (tright > 0.02) tright = 0;
		/* Bubble-sort. */
		if (tmid < tleft) { double dum = tmid; tmid = tleft; tleft = dum; }
		if (tright < tleft)  { double dum = tright; tright = tleft; tleft = dum; }
		if (tright < tmid)  { double dum = tright; tright = tmid; tmid = dum; }
		if (tleft != 0.0) f = YLIN (1 / tmid);   /* Median of 3. */
		else if (tmid != 0.0) f = YLIN (2 / (tmid + tright));   /* Median of 2. */
		else if (tright != 0.0) f = YLIN (1 / tright);   /* Median of 1. */
	}
	RealTier_addPoint (ana -> pitch, 0.5 * (my startSelection + my endSelection), YLININV (f));
	FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
	Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	return 1;
}	

static int menu_cb_addPitchPointAt (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	EDITOR_FORM (L"Add pitch point", 0)
		REAL (L"Time (s)", L"0.0")
		REAL (L"Frequency (Hz or st)", L"100.0")
	EDITOR_OK
		SET_REAL (L"Time", 0.5 * (my startSelection + my endSelection))
		SET_REAL (L"Frequency", my pitchTier.cursor)
	EDITOR_DO
		Manipulation ana = my data;
		if (! ana -> pitch) return 0;
		Editor_save (ManipulationEditor_as_Editor (me), L"Add pitch point");
		RealTier_addPoint (ana -> pitch, GET_REAL (L"Time"), YLININV (GET_REAL (L"Frequency")));
		FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
		Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_stylizePitch (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	EDITOR_FORM (L"Stylize pitch", L"PitchTier: Stylize...")
		REAL (L"Frequency resolution", L"2.0")
		RADIO (L"Units", 2)
			RADIOBUTTON (L"Hertz")
			RADIOBUTTON (L"semitones")
	EDITOR_OK
		SET_REAL (L"Frequency resolution", preferences.pitchTier.stylize.frequencyResolution)   /* Once. */
		SET_INTEGER (L"Units", preferences.pitchTier.stylize.useSemitones + 1)   /* Once. */
	EDITOR_DO
		Manipulation ana = my data;
		if (! ana -> pitch) return 0;
		Editor_save (ManipulationEditor_as_Editor (me), L"Stylize pitch");
		PitchTier_stylize (ana -> pitch, preferences.pitchTier.stylize.frequencyResolution = GET_REAL (L"Frequency resolution"),
			preferences.pitchTier.stylize.useSemitones = GET_INTEGER (L"Units") - 1);
		FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
		Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_stylizePitch_2st (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	Manipulation ana = my data;
	if (! ana -> pitch) return 0;
	Editor_save (ManipulationEditor_as_Editor (me), L"Stylize pitch");
	PitchTier_stylize (ana -> pitch, 2.0, TRUE);
	FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
	Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	return 1;
}

static int menu_cb_interpolateQuadratically (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	EDITOR_FORM (L"Interpolate quadratically", 0)
		NATURAL (L"Number of points per parabola", L"4")
	EDITOR_OK
		SET_INTEGER (L"Number of points per parabola", preferences.pitchTier.interpolateQuadratically.numberOfPointsPerParabola)   /* Once. */
	EDITOR_DO
		Manipulation ana = my data;
		if (! ana -> pitch) return 0;
		Editor_save (ManipulationEditor_as_Editor (me), L"Interpolate quadratically");
		RealTier_interpolateQuadratically (ana -> pitch,
			preferences.pitchTier.interpolateQuadratically.numberOfPointsPerParabola = GET_INTEGER (L"Number of points per parabola"),
			my pitchTier.units == kManipulationEditor_pitchUnits_SEMITONES);
		FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
		Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_interpolateQuadratically_4pts (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	Manipulation ana = my data;
	if (! ana -> pitch) return 0;
	Editor_save (ManipulationEditor_as_Editor (me), L"Interpolate quadratically");
	RealTier_interpolateQuadratically (ana -> pitch, 4, my pitchTier.units == kManipulationEditor_pitchUnits_SEMITONES);
	FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
	Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	return 1;
}

static int menu_cb_shiftPitchFrequencies (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	EDITOR_FORM (L"Shift pitch frequencies", 0)
		REAL (L"Frequency shift", L"-20.0")
		OPTIONMENU (L"Unit", 1)
			OPTION (L"Hertz")
			OPTION (L"mel")
			OPTION (L"logHertz")
			OPTION (L"semitones")
			OPTION (L"ERB")
	EDITOR_OK
	EDITOR_DO
		Manipulation ana = my data;
		int unit = GET_INTEGER (L"Unit");
		unit =
			unit == 1 ? kPitch_unit_HERTZ :
			unit == 2 ? kPitch_unit_MEL :
			unit == 3 ? kPitch_unit_LOG_HERTZ :
			unit == 4 ? kPitch_unit_SEMITONES_1 :
			kPitch_unit_ERB;
		if (! ana -> pitch) return 0;
		Editor_save (ManipulationEditor_as_Editor (me), L"Shift pitch frequencies");
		PitchTier_shiftFrequencies (ana -> pitch, my startSelection, my endSelection, GET_REAL (L"Frequency shift"), unit);
		FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
		Editor_broadcastChange (ManipulationEditor_as_Editor (me));
		iferror return 0;
	EDITOR_END
}

static int menu_cb_multiplyPitchFrequencies (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	EDITOR_FORM (L"Multiply pitch frequencies", 0)
		POSITIVE (L"Factor", L"1.2")
		LABEL (L"", L"The multiplication is always done in Hertz.")
	EDITOR_OK
	EDITOR_DO
		Manipulation ana = my data;
		if (! ana -> pitch) return 0;
		Editor_save (ManipulationEditor_as_Editor (me), L"Multiply pitch frequencies");
		PitchTier_multiplyFrequencies (ana -> pitch, my startSelection, my endSelection, GET_REAL (L"Factor"));
		FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
		Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_setPitchRange (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	EDITOR_FORM (L"Set pitch range", 0)
		/* BUG: should include Minimum */
		REAL (L"Maximum (Hz or st)", L"300.0")
	EDITOR_OK
		SET_REAL (L"Maximum", my pitchTier.maximum)
	EDITOR_DO
		double maximum = GET_REAL (L"Maximum");
		if (maximum <= my pitchTier.minPeriodic)
			return Melder_error5 (L"Maximum pitch must be greater than ", Melder_half (my pitchTier.minPeriodic), L" ", units_strings [my pitchTier.units], L".");
		preferences.pitchTier.maximum = my pitchTier.maximum = maximum;
		FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
	EDITOR_END
}

static int menu_cb_setPitchUnits (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	EDITOR_FORM (L"Set pitch units", 0)
		RADIO_ENUM (L"Pitch units", kManipulationEditor_pitchUnits, DEFAULT)
	EDITOR_OK
		SET_ENUM (L"Pitch units", kManipulationEditor_pitchUnits, my pitchTier.units)
	EDITOR_DO
		enum kManipulationEditor_pitchUnits newPitchUnits = GET_ENUM (kManipulationEditor_pitchUnits, L"Pitch units");
		if (my pitchTier.units == newPitchUnits) return 1;
		preferences.pitchTier.units = my pitchTier.units = newPitchUnits;
		if (my pitchTier.units == kManipulationEditor_pitchUnits_HERTZ) {
			my pitchTier.minimum = 25.0;
			my pitchTier.minPeriodic = 50.0;
			preferences.pitchTier.maximum = my pitchTier.maximum = NUMsemitonesToHertz (my pitchTier.maximum);
			my pitchTier.cursor = NUMsemitonesToHertz (my pitchTier.cursor);
		} else {
			my pitchTier.minimum = -24.0;
			my pitchTier.minPeriodic = -12.0;
			preferences.pitchTier.maximum = my pitchTier.maximum = NUMhertzToSemitones (my pitchTier.maximum);
			my pitchTier.cursor = NUMhertzToSemitones (my pitchTier.cursor);
		}
		FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
	EDITOR_END
}

/***** DURATION MENU *****/

static int menu_cb_setDurationRange (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	EDITOR_FORM (L"Set duration range", 0)
		REAL (L"Minimum", L"0.25")
		REAL (L"Maximum", L"3.0")
	EDITOR_OK
		SET_REAL (L"Minimum", my duration.minimum)
		SET_REAL (L"Maximum", my duration.maximum)
	EDITOR_DO
		Manipulation ana = my data;
		double minimum = GET_REAL (L"Minimum"), maximum = GET_REAL (L"Maximum");
		double minimumValue = ana -> duration ? RealTier_getMinimumValue (ana -> duration) : NUMundefined;
		double maximumValue = ana -> duration ? RealTier_getMaximumValue (ana -> duration) : NUMundefined;
		if (minimum > 1) return Melder_error1 (L"Minimum relative duration must not be greater than 1.");
		if (maximum < 1) return Melder_error1 (L"Maximum relative duration must not be less than 1.");
		if (minimum >= maximum) return Melder_error1 (L"Maximum relative duration must be greater than minimum.");
		if (NUMdefined (minimumValue) && minimum > minimumValue)
			return Melder_error3 (L"Minimum relative duration must not be greater than the minimum value present, "
				"which is ", Melder_half (minimumValue), L".");
		if (NUMdefined (maximumValue) && maximum < maximumValue)
			return Melder_error3 (L"Maximum relative duration must not be less than the maximum value present, "
				"which is ", Melder_half (maximumValue), L".");
		preferences.duration.minimum = my duration.minimum = minimum;
		preferences.duration.maximum = my duration.maximum = maximum;
		FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
	EDITOR_END
}

static int menu_cb_setDraggingStrategy (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	EDITOR_FORM (L"Set dragging strategy", L"ManipulationEditor")
		RADIO_ENUM (L"Dragging strategy", kManipulationEditor_draggingStrategy, DEFAULT)
	EDITOR_OK
		SET_INTEGER (L"Dragging strategy", my pitchTier.draggingStrategy)
	EDITOR_DO
		preferences.pitchTier.draggingStrategy = my pitchTier.draggingStrategy = GET_ENUM (kManipulationEditor_draggingStrategy, L"Dragging strategy");
	EDITOR_END
}

static int menu_cb_removeDurationPoints (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	Manipulation ana = my data;
	if (! ana -> duration) return 0;
	Editor_save (ManipulationEditor_as_Editor (me), L"Remove duration point(s)");
	if (my startSelection == my endSelection)
		AnyTier_removePointNear (ana -> duration, 0.5 * (my startSelection + my endSelection));
	else
		AnyTier_removePointsBetween (ana -> duration, my startSelection, my endSelection);
	FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
	Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	return 1;
}

static int menu_cb_addDurationPointAtCursor (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	Manipulation ana = my data;
	if (! ana -> duration) return 0;
	Editor_save (ManipulationEditor_as_Editor (me), L"Add duration point");
	RealTier_addPoint (ana -> duration, 0.5 * (my startSelection + my endSelection), my duration.cursor);
	FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
	Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	return 1;
}

static int menu_cb_addDurationPointAt (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	EDITOR_FORM (L"Add duration point", 0)
		REAL (L"Time (s)", L"0.0");
		REAL (L"Relative duration", L"1.0");
	EDITOR_OK
		SET_REAL (L"Time", 0.5 * (my startSelection + my endSelection))
	EDITOR_DO
		Manipulation ana = my data;
		if (! ana -> duration) return 0;
		Editor_save (ManipulationEditor_as_Editor (me), L"Add duration point");
		RealTier_addPoint (ana -> duration, GET_REAL (L"Time"), GET_REAL (L"Relative duration"));
		FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
		Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_newDuration (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	Manipulation ana = my data;
	Editor_save (ManipulationEditor_as_Editor (me), L"New duration");
	forget (ana -> duration);
	ana -> duration = DurationTier_create (ana -> xmin, ana -> xmax);
	FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
	Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	return 1;
}

static int menu_cb_forgetDuration (EDITOR_ARGS) {
	EDITOR_IAM (ManipulationEditor);
	Manipulation ana = my data;
	forget (ana -> duration);
	FunctionEditor_redraw (ManipulationEditor_as_FunctionEditor (me));
	Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	return 1;
}
	
static int menu_cb_ManipulationEditorHelp (EDITOR_ARGS) { EDITOR_IAM (ManipulationEditor); Melder_help (L"ManipulationEditor"); return 1; }
static int menu_cb_ManipulationHelp (EDITOR_ARGS) { EDITOR_IAM (ManipulationEditor); Melder_help (L"Manipulation"); return 1; }

#define menu_cb_Synth_common(menu_cb,meth) \
static int menu_cb (EDITOR_ARGS) { \
	EDITOR_IAM (ManipulationEditor); \
	prefs_synthesisMethod = my synthesisMethod = meth; \
	updateMenus (me); \
	return 1; \
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

static void createMenus (ManipulationEditor me) {
	inherited (ManipulationEditor) createMenus (ManipulationEditor_as_parent (me));

	Editor_addCommand (me, L"File", L"Extract original sound", 0, menu_cb_extractOriginalSound);
	Editor_addCommand (me, L"File", L"Extract pulses", 0, menu_cb_extractPulses);
	Editor_addCommand (me, L"File", L"Extract pitch tier", 0, menu_cb_extractPitchTier);
	Editor_addCommand (me, L"File", L"Extract duration tier", 0, menu_cb_extractDurationTier);
	Editor_addCommand (me, L"File", L"Publish resynthesis", 0, menu_cb_extractManipulatedSound);
	Editor_addCommand (me, L"File", L"-- close --", 0, NULL);

	Editor_addMenu (me, L"Pulse", 0);
	Editor_addCommand (me, L"Pulse", L"Add pulse at cursor", 'P', menu_cb_addPulseAtCursor);
	Editor_addCommand (me, L"Pulse", L"Add pulse at...", 0, menu_cb_addPulseAt);
	Editor_addCommand (me, L"Pulse", L"-- remove pulses --", 0, NULL);
	Editor_addCommand (me, L"Pulse", L"Remove pulse(s)", GuiMenu_OPTION + 'P', menu_cb_removePulses);

	Editor_addMenu (me, L"Pitch", 0);
	Editor_addCommand (me, L"Pitch", L"Add pitch point at cursor", 'T', menu_cb_addPitchPointAtCursor);
	Editor_addCommand (me, L"Pitch", L"Add pitch point at time slice", 0, menu_cb_addPitchPointAtSlice);
	Editor_addCommand (me, L"Pitch", L"Add pitch point at...", 0, menu_cb_addPitchPointAt);
	Editor_addCommand (me, L"Pitch", L"-- remove pitch --", 0, NULL);
	Editor_addCommand (me, L"Pitch", L"Remove pitch point(s)", GuiMenu_OPTION + 'T', menu_cb_removePitchPoints);
	Editor_addCommand (me, L"Pitch", L"-- pitch prefs --", 0, NULL);
	Editor_addCommand (me, L"Pitch", L"Set pitch range...", 0, menu_cb_setPitchRange);
	Editor_addCommand (me, L"Pitch", L"Set pitch units...", 0, menu_cb_setPitchUnits);
	Editor_addCommand (me, L"Pitch", L"Set pitch dragging strategy...", 0, menu_cb_setDraggingStrategy);
	Editor_addCommand (me, L"Pitch", L"-- modify pitch --", 0, NULL);
	Editor_addCommand (me, L"Pitch", L"Shift pitch frequencies...", 0, menu_cb_shiftPitchFrequencies);
	Editor_addCommand (me, L"Pitch", L"Multiply pitch frequencies...", 0, menu_cb_multiplyPitchFrequencies);
	Editor_addCommand (me, L"Pitch", L"All:", GuiMenu_INSENSITIVE, menu_cb_stylizePitch);
	Editor_addCommand (me, L"Pitch", L"Stylize pitch...", 0, menu_cb_stylizePitch);
	Editor_addCommand (me, L"Pitch", L"Stylize pitch (2 st)", '2', menu_cb_stylizePitch_2st);
	Editor_addCommand (me, L"Pitch", L"Interpolate quadratically...", 0, menu_cb_interpolateQuadratically);
	Editor_addCommand (me, L"Pitch", L"Interpolate quadratically (4 pts)", '4', menu_cb_interpolateQuadratically_4pts);

	Editor_addMenu (me, L"Dur", 0);
	Editor_addCommand (me, L"Dur", L"Add duration point at cursor", 'D', menu_cb_addDurationPointAtCursor);
	Editor_addCommand (me, L"Dur", L"Add duration point at...", 0, menu_cb_addDurationPointAt);
	Editor_addCommand (me, L"Dur", L"-- remove duration --", 0, NULL);
	Editor_addCommand (me, L"Dur", L"Remove duration point(s)", GuiMenu_OPTION + 'D', menu_cb_removeDurationPoints);
	Editor_addCommand (me, L"Dur", L"-- duration prefs --", 0, NULL);
	Editor_addCommand (me, L"Dur", L"Set duration range...", 0, menu_cb_setDurationRange);
	Editor_addCommand (me, L"Dur", L"-- refresh duration --", 0, NULL);
	Editor_addCommand (me, L"Dur", L"New duration", 0, menu_cb_newDuration);
	Editor_addCommand (me, L"Dur", L"Forget duration", 0, menu_cb_forgetDuration);

	Editor_addMenu (me, L"Synth", 0);
	my synthPulsesButton = Editor_addCommand (me, L"Synth", L"Pulses --", GuiMenu_RADIO_FIRST, menu_cb_Synth_Pulses);
	my synthPulsesHumButton = Editor_addCommand (me, L"Synth", L"Pulses (hum) --", GuiMenu_RADIO_NEXT, menu_cb_Synth_Pulses_hum);

	my synthPulsesLpcButton = Editor_addCommand (me, L"Synth", L"Pulses & LPC -- (\"LPC resynthesis\")", GuiMenu_RADIO_NEXT, menu_cb_Synth_Pulses_Lpc);
	Editor_addCommand (me, L"Synth", L"-- pitch resynth --", 0, NULL);
	my synthPitchButton = Editor_addCommand (me, L"Synth", L" -- Pitch", GuiMenu_RADIO_NEXT, menu_cb_Synth_Pitch);
	my synthPitchHumButton = Editor_addCommand (me, L"Synth", L" -- Pitch (hum)", GuiMenu_RADIO_NEXT, menu_cb_Synth_Pitch_hum);
	my synthPulsesPitchButton = Editor_addCommand (me, L"Synth", L"Pulses -- Pitch", GuiMenu_RADIO_NEXT, menu_cb_Synth_Pulses_Pitch);
	my synthPulsesPitchHumButton = Editor_addCommand (me, L"Synth", L"Pulses -- Pitch (hum)", GuiMenu_RADIO_NEXT, menu_cb_Synth_Pulses_Pitch_hum);
	Editor_addCommand (me, L"Synth", L"-- full resynth --", 0, NULL);
	my synthOverlapAddButton = Editor_addCommand (me, L"Synth", L"Sound & Pulses -- Pitch & Duration  (\"Overlap-add manipulation\")", GuiMenu_RADIO_NEXT | GuiMenu_TOGGLE_ON, menu_cb_Synth_OverlapAdd);
	my synthPitchLpcButton = Editor_addCommand (me, L"Synth", L"LPC -- Pitch  (\"LPC pitch manipulation\")", GuiMenu_RADIO_NEXT, menu_cb_Synth_Pitch_Lpc);
}

static void createHelpMenuItems (ManipulationEditor me, EditorMenu menu) {
	inherited (ManipulationEditor) createHelpMenuItems (ManipulationEditor_as_parent (me), menu);
	EditorMenu_addCommand (menu, L"ManipulationEditor help", '?', menu_cb_ManipulationEditorHelp);
	EditorMenu_addCommand (menu, L"Manipulation help", 0, menu_cb_ManipulationHelp);
}

/********** DRAWING AREA **********/

static void drawSoundArea (ManipulationEditor me, double ymin, double ymax) {
	Manipulation ana = my data;
	Sound sound = ana -> sound;
	PointProcess pulses = ana -> pulses;
	long first, last, i;
	Graphics_Viewport viewport = Graphics_insetViewport (my graphics, 0, 1, ymin, ymax);
	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_rectangle (my graphics, 0, 1, 0, 1);
	Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_TOP);
	Graphics_setFont (my graphics, kGraphics_font_TIMES);
	Graphics_text (my graphics, 1, 1, L"%%Sound");
	Graphics_setColour (my graphics, Graphics_BLUE);
	Graphics_text (my graphics, 1, 1 - Graphics_dyMMtoWC (my graphics, 3), L"%%Pulses");
	Graphics_setFont (my graphics, kGraphics_font_HELVETICA);

	/*
	 * Draw blue pulses.
	 */
	if (pulses) {
		Graphics_setWindow (my graphics, my startWindow, my endWindow, 0.0, 1.0);
		Graphics_setColour (my graphics, Graphics_BLUE);
		for (i = 1; i <= pulses -> nt; i ++) {
			double t = pulses -> t [i];
			if (t >= my startWindow && t <= my endWindow)
				Graphics_line (my graphics, t, 0.05, t, 0.95);
		}
	}

	/*
	 * Draw sound.
	 */
	if (sound && Sampled_getWindowSamples (sound, my startWindow, my endWindow, & first, & last) > 1) {
		double minimum, maximum, scaleMin, scaleMax;
		Matrix_getWindowExtrema (sound, first, last, 1, 1, & minimum, & maximum);
		if (minimum == maximum) minimum = -0.5, maximum = +0.5;

		/*
		 * Scaling.
		 */
		scaleMin = 0.83 * minimum + 0.17 * my soundmin;
		scaleMax = 0.83 * maximum + 0.17 * my soundmax;
		Graphics_setWindow (my graphics, my startWindow, my endWindow, scaleMin, scaleMax);
		FunctionEditor_drawRangeMark (ManipulationEditor_as_FunctionEditor (me), scaleMin, Melder_float (Melder_half (scaleMin)), L"", Graphics_BOTTOM);
		FunctionEditor_drawRangeMark (ManipulationEditor_as_FunctionEditor (me), scaleMax, Melder_float (Melder_half (scaleMax)), L"", Graphics_TOP);

		/*
		 * Draw dotted zero line.
		 */
		if (minimum < 0.0 && maximum > 0.0) {
			Graphics_setColour (my graphics, Graphics_CYAN);
			Graphics_setLineType (my graphics, Graphics_DOTTED);
			Graphics_line (my graphics, my startWindow, 0.0, my endWindow, 0.0);
			Graphics_setLineType (my graphics, Graphics_DRAWN);
		} 

		/*
		 * Draw samples.
		 */    
		Graphics_setColour (my graphics, Graphics_BLACK);
		Graphics_function (my graphics, sound -> z [1], first, last,
			Sampled_indexToX (sound, first), Sampled_indexToX (sound, last));
	}

	Graphics_resetViewport (my graphics, viewport);
}

static void drawPitchArea (ManipulationEditor me, double ymin, double ymax) {
	Manipulation ana = my data;
	PointProcess pulses = ana -> pulses;
	PitchTier pitch = ana -> pitch;
	long ifirstSelected, ilastSelected, n = pitch ? pitch -> points -> size : 0, imin, imax, i;
	int cursorVisible = my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow;
	double minimumFrequency = YLIN (50);
	int rangePrecisions [] = { 0, 1, 2 };
	wchar_t *rangeUnits [] = { L"", L" Hz", L" st" };

	/*
	 * Pitch contours.
	 */
	Graphics_Viewport viewport = Graphics_insetViewport (my graphics, 0, 1, ymin, ymax);
	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_rectangle (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_GREEN);
	Graphics_setFont (my graphics, kGraphics_font_TIMES);
	Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_TOP);
	Graphics_text (my graphics, 1, 1, L"%%Pitch manip");
	Graphics_setGrey (my graphics, 0.7);
	Graphics_text (my graphics, 1, 1 - Graphics_dyMMtoWC (my graphics, 3), L"%%Pitch from pulses");
	Graphics_setFont (my graphics, kGraphics_font_HELVETICA);

	Graphics_setWindow (my graphics, my startWindow, my endWindow, my pitchTier.minimum, my pitchTier.maximum);

	/*
	 * Draw pitch contour based on pulses.
	 */
	Graphics_setGrey (my graphics, 0.7);
	if (pulses) for (i = 1; i < pulses -> nt; i ++) {
		double tleft = pulses -> t [i], tright = pulses -> t [i + 1], t = 0.5 * (tleft + tright);
		if (t >= my startWindow && t <= my endWindow) {
			if (tleft != tright) {
				double f = YLIN (1 / (tright - tleft));
				if (f >= my pitchTier.minPeriodic && f <= my pitchTier.maximum) {
					Graphics_fillCircle_mm (my graphics, t, f, 1);
				}
			}
		}
	}
	Graphics_setGrey (my graphics, 0.0);

	FunctionEditor_drawGridLine (ManipulationEditor_as_FunctionEditor (me), minimumFrequency);
	FunctionEditor_drawRangeMark (ManipulationEditor_as_FunctionEditor (me), my pitchTier.maximum,
		Melder_fixed (my pitchTier.maximum, rangePrecisions [my pitchTier.units]), rangeUnits [my pitchTier.units], Graphics_TOP);
	FunctionEditor_drawRangeMark (ManipulationEditor_as_FunctionEditor (me), my pitchTier.minimum,
		Melder_fixed (my pitchTier.minimum, rangePrecisions [my pitchTier.units]), rangeUnits [my pitchTier.units], Graphics_BOTTOM);
	if (my startSelection == my endSelection && my pitchTier.cursor >= my pitchTier.minimum && my pitchTier.cursor <= my pitchTier.maximum)
		FunctionEditor_drawHorizontalHair (ManipulationEditor_as_FunctionEditor (me), my pitchTier.cursor,
			Melder_fixed (my pitchTier.cursor, rangePrecisions [my pitchTier.units]), rangeUnits [my pitchTier.units]);
	if (cursorVisible && n > 0) {
		double y = YLIN (RealTier_getValueAtTime (pitch, my startSelection));
		FunctionEditor_insertCursorFunctionValue (ManipulationEditor_as_FunctionEditor (me), y,
			Melder_fixed (y, rangePrecisions [my pitchTier.units]), rangeUnits [my pitchTier.units],
			my pitchTier.minimum, my pitchTier.maximum);
	}
	if (pitch) {
		ifirstSelected = AnyTier_timeToHighIndex (pitch, my startSelection);
		ilastSelected = AnyTier_timeToLowIndex (pitch, my endSelection);
		imin = AnyTier_timeToHighIndex (pitch, my startWindow);
		imax = AnyTier_timeToLowIndex (pitch, my endWindow);
	}
	Graphics_setLineWidth (my graphics, 2);
	if (n == 0) {
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_setColour (my graphics, Graphics_BLACK);
		Graphics_text (my graphics, 0.5 * (my startWindow + my endWindow), 0.5 * (my pitchTier.minimum + my pitchTier.maximum), L"(no pitch points)");
	} else if (imax < imin) {
		double fleft = YLIN (RealTier_getValueAtTime (pitch, my startWindow));
		double fright = YLIN (RealTier_getValueAtTime (pitch, my endWindow));
		Graphics_setColour (my graphics, Graphics_GREEN);
		Graphics_line (my graphics, my startWindow, fleft, my endWindow, fright);
	} else {
		for (i = imin; i <= imax; i ++) {
			RealPoint point = pitch -> points -> item [i];
			double t = point -> time, f = YLIN (point -> value);
			Graphics_setColour (my graphics, Graphics_GREEN);
			if (i == 1)
				Graphics_line (my graphics, my startWindow, f, t, f);
			else if (i == imin)
				Graphics_line (my graphics, t, f, my startWindow, YLIN (RealTier_getValueAtTime (pitch, my startWindow)));
			if (i == n)
				Graphics_line (my graphics, t, f, my endWindow, f);
			else if (i == imax)
				Graphics_line (my graphics, t, f, my endWindow, YLIN (RealTier_getValueAtTime (pitch, my endWindow)));
			else {
				RealPoint pointRight = pitch -> points -> item [i + 1];
				Graphics_line (my graphics, t, f, pointRight -> time, YLIN (pointRight -> value));
			}
		}
		for (i = imin; i <= imax; i ++) {
			RealPoint point = pitch -> points -> item [i];
			double t = point -> time, f = YLIN (point -> value);
			if (i >= ifirstSelected && i <= ilastSelected)
				Graphics_setColour (my graphics, Graphics_RED);	
			else
				Graphics_setColour (my graphics, Graphics_GREEN);
			Graphics_fillCircle_mm (my graphics, t, f, 3);
		}
	}
	Graphics_setLineWidth (my graphics, 1);

	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_resetViewport (my graphics, viewport);
}

static void drawDurationArea (ManipulationEditor me, double ymin, double ymax) {
	Manipulation ana = my data;
	DurationTier duration = ana -> duration;
	long ifirstSelected, ilastSelected, n = duration ? duration -> points -> size : 0, imin, imax, i;
	int cursorVisible = my startSelection == my endSelection && my startSelection >= my startWindow && my startSelection <= my endWindow;

	/*
	 * Duration contours.
	 */
	Graphics_Viewport viewport = Graphics_insetViewport (my graphics, 0, 1, ymin, ymax);
	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_rectangle (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_GREEN);
	Graphics_setFont (my graphics, kGraphics_font_TIMES);
	Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_TOP);
	Graphics_text (my graphics, 1, 1, L"%%Duration manip");
	Graphics_setFont (my graphics, kGraphics_font_HELVETICA);

	Graphics_setWindow (my graphics, my startWindow, my endWindow, my duration.minimum, my duration.maximum);
	FunctionEditor_drawGridLine (ManipulationEditor_as_FunctionEditor (me), 1.0);
	FunctionEditor_drawRangeMark (ManipulationEditor_as_FunctionEditor (me), my duration.maximum, Melder_fixed (my duration.maximum, 3), L"", Graphics_TOP);
	FunctionEditor_drawRangeMark (ManipulationEditor_as_FunctionEditor (me), my duration.minimum, Melder_fixed (my duration.minimum, 3), L"", Graphics_BOTTOM);
	if (my startSelection == my endSelection && my duration.cursor >= my duration.minimum && my duration.cursor <= my duration.maximum)
		FunctionEditor_drawHorizontalHair (ManipulationEditor_as_FunctionEditor (me), my duration.cursor, Melder_fixed (my duration.cursor, 3), L"");
	if (cursorVisible && n > 0) {
		double y = RealTier_getValueAtTime (duration, my startSelection);
		FunctionEditor_insertCursorFunctionValue (ManipulationEditor_as_FunctionEditor (me), y, Melder_fixed (y, 3), L"", my duration.minimum, my duration.maximum);
	}

	/*
	 * Draw duration tier.
	 */
	if (duration) {
		ifirstSelected = AnyTier_timeToHighIndex (duration, my startSelection);
		ilastSelected = AnyTier_timeToLowIndex (duration, my endSelection);
		imin = AnyTier_timeToHighIndex (duration, my startWindow);
		imax = AnyTier_timeToLowIndex (duration, my endWindow);
	}
	Graphics_setLineWidth (my graphics, 2);
	if (n == 0) {
		Graphics_setColour (my graphics, Graphics_BLACK);
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics, 0.5 * (my startWindow + my endWindow),
			0.5 * (my duration.minimum + my duration.maximum), L"(no duration points)");
	} else if (imax < imin) {
		double fleft = RealTier_getValueAtTime (duration, my startWindow);
		double fright = RealTier_getValueAtTime (duration, my endWindow);
		Graphics_setColour (my graphics, Graphics_GREEN);
		Graphics_line (my graphics, my startWindow, fleft, my endWindow, fright);
	} else {
		for (i = imin; i <= imax; i ++) {
			RealPoint point = duration -> points -> item [i];
			double t = point -> time, dur = point -> value;
			Graphics_setColour (my graphics, Graphics_GREEN);
			if (i == 1)
				Graphics_line (my graphics, my startWindow, dur, t, dur);
			else if (i == imin)
				Graphics_line (my graphics, t, dur, my startWindow, RealTier_getValueAtTime (duration, my startWindow));
			if (i == n)
				Graphics_line (my graphics, t, dur, my endWindow, dur);
			else if (i == imax)
				Graphics_line (my graphics, t, dur, my endWindow, RealTier_getValueAtTime (duration, my endWindow));
			else {
				RealPoint pointRight = duration -> points -> item [i + 1];
				Graphics_line (my graphics, t, dur, pointRight -> time, pointRight -> value);
			}
		}
		for (i = imin; i <= imax; i ++) {
			RealPoint point = duration -> points -> item [i];
			double t = point -> time, dur = point -> value;
			if (i >= ifirstSelected && i <= ilastSelected)
				Graphics_setColour (my graphics, Graphics_RED);	
			else
				Graphics_setColour (my graphics, Graphics_GREEN);	
			Graphics_fillCircle_mm (my graphics, t, dur, 3);
		}
	}

	Graphics_setLineWidth (my graphics, 1);
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_resetViewport (my graphics, viewport);
}

static void draw (ManipulationEditor me) {
	double ysoundmin, ysoundmax;
	double ypitchmin, ypitchmax, ydurationmin, ydurationmax;
	int hasSoundArea = getSoundArea (me, & ysoundmin, & ysoundmax);
	int hasPitchArea = getPitchArea (me, & ypitchmin, & ypitchmax);
	int hasDurationArea = getDurationArea (me, & ydurationmin, & ydurationmax);

	if (hasSoundArea) drawSoundArea (me, ysoundmin, ysoundmax);
	if (hasPitchArea) drawPitchArea (me, ypitchmin, ypitchmax);
	if (hasDurationArea) drawDurationArea (me, ydurationmin, ydurationmax);

	Graphics_setWindow (my graphics, 0.0, 1.0, 0.0, 1.0);
	Graphics_setGrey (my graphics, 0.85);
	Graphics_fillRectangle (my graphics, -0.001, 1.001, ypitchmax, ysoundmin);
	Graphics_setGrey (my graphics, 0.00);
	Graphics_line (my graphics, 0, ysoundmin, 1, ysoundmin);
	Graphics_line (my graphics, 0, ypitchmax, 1, ypitchmax);
	if (hasDurationArea) {
		Graphics_setGrey (my graphics, 0.85);
		Graphics_fillRectangle (my graphics, -0.001, 1.001, ydurationmax, ypitchmin);
		Graphics_setGrey (my graphics, 0.00);
		Graphics_line (my graphics, 0, ypitchmin, 1, ypitchmin);
		Graphics_line (my graphics, 0, ydurationmax, 1, ydurationmax);
	}
	updateMenus (me);
}

static void drawWhileDragging (ManipulationEditor me, double xWC, double yWC, long first, long last, double dt, double df) {
	Manipulation ana = my data;
	PitchTier pitch = ana -> pitch;
	long i;
	(void) xWC;
	(void) yWC;

	/*
	 * Draw all selected pitch points as magenta empty circles, if inside the window.
	 */
	for (i = first; i <= last; i ++) {
		RealPoint point = pitch -> points -> item [i];
		double t = point -> time + dt, f = YLIN (point -> value) + df;
		if (t >= my startWindow && t <= my endWindow)
			Graphics_circle_mm (my graphics, t,
				f < my pitchTier.minPeriodic ? my pitchTier.minPeriodic : f > my pitchTier.maximum ? my pitchTier.maximum : f, 3);
	}

	if (last == first) {
		/*
		 * Draw a crosshair with time and frequency.
		 */
		RealPoint point = pitch -> points -> item [first];
		double t = point -> time + dt, fWC = YLIN (point -> value) + df;
		Graphics_line (my graphics, t, my pitchTier.minimum, t, my pitchTier.maximum - Graphics_dyMMtoWC (my graphics, 4.0));
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_TOP);
		Graphics_text1 (my graphics, t, my pitchTier.maximum, Melder_fixed (t, 6));
		Graphics_line (my graphics, my startWindow, fWC, my endWindow, fWC);
		Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_BOTTOM);
		Graphics_text1 (my graphics, my startWindow, fWC, Melder_fixed (fWC, 5));
	}
}

static int clickPitch (ManipulationEditor me, double xWC, double yWC, int shiftKeyPressed) {
	Manipulation ana = my data;
	PitchTier pitch = ana -> pitch;
	long inearestPoint, ifirstSelected, ilastSelected, i;
	RealPoint nearestPoint;
	double dt = 0, df = 0;
	int draggingSelection, dragHorizontal, dragVertical;

	my pitchTier.cursor = my pitchTier.minimum + yWC * (my pitchTier.maximum - my pitchTier.minimum);
	if (! pitch) {
		Graphics_resetViewport (my graphics, my inset);
		return inherited (ManipulationEditor) click (ManipulationEditor_as_parent (me), xWC, yWC, shiftKeyPressed);
	}
	Graphics_setWindow (my graphics, my startWindow, my endWindow, my pitchTier.minimum, my pitchTier.maximum);
	yWC = my pitchTier.cursor;

	/*
	 * Clicked on a pitch point?
	 */
	inearestPoint = AnyTier_timeToNearestIndex (pitch, xWC);
	if (inearestPoint == 0) {
		Graphics_resetViewport (my graphics, my inset);
		return inherited (ManipulationEditor) click (ManipulationEditor_as_parent (me), xWC, yWC, shiftKeyPressed);
	}
	nearestPoint = pitch -> points -> item [inearestPoint];
	if (Graphics_distanceWCtoMM (my graphics, xWC, yWC, nearestPoint -> time, YLIN (nearestPoint -> value)) > 1.5) {
		Graphics_resetViewport (my graphics, my inset);
		return inherited (ManipulationEditor) click (ManipulationEditor_as_parent (me), xWC, yWC, shiftKeyPressed);
	}

	/*
	 * Clicked on a selected pitch point?
	 */
	draggingSelection = shiftKeyPressed &&
		nearestPoint -> time > my startSelection && nearestPoint -> time < my endSelection;
	if (draggingSelection) {
		ifirstSelected = AnyTier_timeToHighIndex (pitch, my startSelection);
		ilastSelected = AnyTier_timeToLowIndex (pitch, my endSelection);
		Editor_save (ManipulationEditor_as_Editor (me), L"Drag pitch points");
	} else {
		ifirstSelected = ilastSelected = inearestPoint;
		Editor_save (ManipulationEditor_as_Editor (me), L"Drag pitch point");
	}

	/*
	 * Drag.
	 */
	 /*
	  * Draw at the old location once.
	  * Since some systems do double buffering,
	  * the undrawing at the old position and redrawing at the new have to be bracketed by Graphics_mouseStillDown ().
	  */
	Graphics_xorOn (my graphics, Graphics_MAROON);
	drawWhileDragging (me, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
	dragHorizontal = my pitchTier.draggingStrategy != kManipulationEditor_draggingStrategy_VERTICAL &&
		(! shiftKeyPressed || my pitchTier.draggingStrategy != kManipulationEditor_draggingStrategy_HYBRID);
	dragVertical = my pitchTier.draggingStrategy != kManipulationEditor_draggingStrategy_HORIZONTAL;
	while (Graphics_mouseStillDown (my graphics)) {
		double xWC_new, yWC_new;
		Graphics_getMouseLocation (my graphics, & xWC_new, & yWC_new);
		if (xWC_new != xWC || yWC_new != yWC) {
			drawWhileDragging (me, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
			if (dragHorizontal) dt += xWC_new - xWC;
			if (dragVertical) df += yWC_new - yWC;
			xWC = xWC_new, yWC = yWC_new;
			drawWhileDragging (me, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
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
		point -> value = YLININV (YLIN (point -> value) + df);
		if (point -> value < 50.0) point -> value = 50.0;
		if (point -> value > YLININV (my pitchTier.maximum)) point -> value = YLININV (my pitchTier.maximum);
	}

	/*
	 * Make sure that the same pitch points are still selected (a problem with Undo...).
	 */

	if (draggingSelection) my startSelection += dt, my endSelection += dt;
	if (my startSelection == my endSelection) {
		RealPoint point = pitch -> points -> item [ifirstSelected];
		my startSelection = my endSelection = point -> time;
		my pitchTier.cursor = YLIN (point -> value);
	}

	Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	return 1;   /* Update needed. */
}

static void drawDurationWhileDragging (ManipulationEditor me, double xWC, double yWC, long first, long last, double dt, double df) {
	Manipulation ana = my data;
	DurationTier duration = ana -> duration;
	long i;
	(void) xWC;
	(void) yWC;

	/*
	 * Draw all selected duration points as magenta empty circles, if inside the window.
	 */
	for (i = first; i <= last; i ++) {
		RealPoint point = duration -> points -> item [i];
		double t = point -> time + dt, dur = point -> value + df;
		if (t >= my startWindow && t <= my endWindow)
			Graphics_circle_mm (my graphics, t, dur < my duration.minimum ? my duration.minimum :
				dur > my duration.maximum ? my duration.maximum : dur, 3);
	}

	if (last == first) {
		/*
		 * Draw a crosshair with time and duration.
		 */
		RealPoint point = duration -> points -> item [first];
		double t = point -> time + dt, durWC = point -> value + df;
		Graphics_line (my graphics, t, my duration.minimum, t, my duration.maximum - Graphics_dyMMtoWC (my graphics, 4.0));
		Graphics_setTextAlignment (my graphics, Graphics_CENTRE, Graphics_TOP);
		Graphics_text1 (my graphics, t, my duration.maximum, Melder_fixed (t, 6));
		Graphics_line (my graphics, my startWindow, durWC, my endWindow, durWC);
		Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_BOTTOM);
		Graphics_text1 (my graphics, my startWindow, durWC, Melder_fixed (durWC, 2));
	}
}

static int clickDuration (ManipulationEditor me, double xWC, double yWC, int shiftKeyPressed) {
	Manipulation ana = my data;
	DurationTier duration = ana -> duration;
	long inearestPoint, ifirstSelected, ilastSelected, i;
	RealPoint nearestPoint;
	double dt = 0, df = 0;
	int draggingSelection;

	/*
	 * Convert from FunctionEditor's [0, 1] coordinates to world coordinates.
	 */
	yWC = my duration.minimum + yWC * (my duration.maximum - my duration.minimum);

	/*
	 * Move horizontal hair to clicked position.
	 */
	my duration.cursor = yWC;

	if (! duration) {
		Graphics_resetViewport (my graphics, my inset);
		return inherited (ManipulationEditor) click (ManipulationEditor_as_parent (me), xWC, yWC, shiftKeyPressed);
	}
	Graphics_setWindow (my graphics, my startWindow, my endWindow, my duration.minimum, my duration.maximum);

	/*
	 * Clicked on a duration point?
	 */
	inearestPoint = AnyTier_timeToNearestIndex (duration, xWC);
	if (inearestPoint == 0) {
		Graphics_resetViewport (my graphics, my inset);
		return inherited (ManipulationEditor) click (ManipulationEditor_as_parent (me), xWC, yWC, shiftKeyPressed);
	}
	nearestPoint = duration -> points -> item [inearestPoint];
	if (Graphics_distanceWCtoMM (my graphics, xWC, yWC, nearestPoint -> time, nearestPoint -> value) > 1.5) {
		Graphics_resetViewport (my graphics, my inset);
		return inherited (ManipulationEditor) click (ManipulationEditor_as_parent (me), xWC, yWC, shiftKeyPressed);
	}

	/*
	 * Clicked on a selected duration point?
	 */
	draggingSelection = shiftKeyPressed &&
		nearestPoint -> time > my startSelection && nearestPoint -> time < my endSelection;
	if (draggingSelection) {
		ifirstSelected = AnyTier_timeToHighIndex (duration, my startSelection);
		ilastSelected = AnyTier_timeToLowIndex (duration, my endSelection);
		Editor_save (ManipulationEditor_as_Editor (me), L"Drag duration points");
	} else {
		ifirstSelected = ilastSelected = inearestPoint;
		Editor_save (ManipulationEditor_as_Editor (me), L"Drag duration point");
	}

	/*
	 * Drag.
	 */
	Graphics_xorOn (my graphics, Graphics_MAROON);
	drawDurationWhileDragging (me, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
	while (Graphics_mouseStillDown (my graphics)) {
		double xWC_new, yWC_new;
		Graphics_getMouseLocation (my graphics, & xWC_new, & yWC_new);
		if (xWC_new != xWC || yWC_new != yWC) {
			drawDurationWhileDragging (me, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
			dt += xWC_new - xWC, xWC = xWC_new;
			df += yWC_new - yWC, yWC = yWC_new;
			drawDurationWhileDragging (me, xWC_new, yWC_new, ifirstSelected, ilastSelected, dt, df);
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
		RealPoint *points = (RealPoint *) duration -> points -> item;
		double newTime = points [ifirstSelected] -> time + dt;
		if (newTime < my tmin) return 1;   /* Outside domain. */
		if (ifirstSelected > 1 && newTime <= points [ifirstSelected - 1] -> time)
			return 1;   /* Past left neighbour. */
		newTime = points [ilastSelected] -> time + dt;
		if (newTime > my tmax) return 1;   /* Outside domain. */
		if (ilastSelected < duration -> points -> size && newTime >= points [ilastSelected + 1] -> time)
			return 1;   /* Past right neighbour. */
	}

	/*
	 * Drop.
	 */
	for (i = ifirstSelected; i <= ilastSelected; i ++) {
		RealPoint point = duration -> points -> item [i];
		point -> time += dt;
		point -> value += df;
		if (point -> value < my duration.minimum) point -> value = my duration.minimum;
		if (point -> value > my duration.maximum) point -> value = my duration.maximum;
	}

	/*
	 * Make sure that the same duration points are still selected (a problem with Undo...).
	 */

	if (draggingSelection) my startSelection += dt, my endSelection += dt;
	if (my startSelection == my endSelection) {
		RealPoint point = duration -> points -> item [ifirstSelected];
		my startSelection = my endSelection = point -> time;
		my duration.cursor = point -> value;
	}

	Editor_broadcastChange (ManipulationEditor_as_Editor (me));
	return 1;   /* Update needed. */
}

static int click (ManipulationEditor me, double xWC, double yWC, int shiftKeyPressed) {
	double ypitchmin, ypitchmax, ydurationmin, ydurationmax;
	int hasPitchArea = getPitchArea (me, & ypitchmin, & ypitchmax);
	int hasDurationArea = getDurationArea (me, & ydurationmin, & ydurationmax);

	/*
	 * Dispatch click to clicked area.
	 */
	if (hasPitchArea && yWC > ypitchmin && yWC < ypitchmax) {   /* Clicked in pitch area? */
		my inset = Graphics_insetViewport (my graphics, 0, 1, ypitchmin, ypitchmax);
		return clickPitch (me, xWC, (yWC - ypitchmin) / (ypitchmax - ypitchmin), shiftKeyPressed);
	} else if (hasDurationArea && yWC > ydurationmin && yWC < ydurationmax) {   /* Clicked in duration area? */
		my inset = Graphics_insetViewport (my graphics, 0, 1, ydurationmin, ydurationmax);
		return clickDuration (me, xWC, (yWC - ydurationmin) / (ydurationmax - ydurationmin), shiftKeyPressed);
	}
	/*
	 * Perform the default action: move cursor or drag selection.
	 */
	return inherited (ManipulationEditor) click (ManipulationEditor_as_parent (me), xWC, yWC, shiftKeyPressed);
}

static void play (ManipulationEditor me, double tmin, double tmax) {
	Manipulation ana = my data;
	if (my shiftKeyPressed) {
		if (ana -> sound) Sound_playPart (ana -> sound, tmin, tmax, our playCallback, me);
	} else {
		if (! Manipulation_playPart (ana, tmin, tmax, my synthesisMethod))
			Melder_flushError (NULL);
	}
}

class_methods (ManipulationEditor, FunctionEditor) {
	class_method (destroy)
	class_method (createMenus)
	class_method (createHelpMenuItems)
	class_method (save)
	class_method (restore)
	class_method (draw)
	class_method (click)
	class_method (play)
	class_methods_end
}

ManipulationEditor ManipulationEditor_create (GuiObject parent, const wchar_t *title, Manipulation ana) {
	ManipulationEditor me = Thing_new (ManipulationEditor);
	if (! me || ! FunctionEditor_init (ManipulationEditor_as_parent (me), parent, title, ana)) return NULL;

	my pitchTier.draggingStrategy = preferences.pitchTier.draggingStrategy;
	my pitchTier.units = preferences.pitchTier.units;
	double maximumPitchValue = RealTier_getMaximumValue (ana -> pitch);
	if (my pitchTier.units == kManipulationEditor_pitchUnits_HERTZ) {
		my pitchTier.minimum = 25.0;
		my pitchTier.minPeriodic = 50.0;
		my pitchTier.maximum = maximumPitchValue;
		my pitchTier.cursor = my pitchTier.maximum * 0.8;
		my pitchTier.maximum *= 1.2;
	} else {
		my pitchTier.minimum = -24.0;
		my pitchTier.minPeriodic = -12.0;
		my pitchTier.maximum = NUMdefined (maximumPitchValue) ? NUMhertzToSemitones (maximumPitchValue) : NUMundefined;
		my pitchTier.cursor = my pitchTier.maximum - 4.0;
		my pitchTier.maximum += 3.0;
	}
	if (my pitchTier.maximum == NUMundefined || my pitchTier.maximum < preferences.pitchTier.maximum) my pitchTier.maximum = preferences.pitchTier.maximum;

	double minimumDurationValue = ana -> duration ? RealTier_getMinimumValue (ana -> duration) : NUMundefined;
	my duration.minimum = NUMdefined (minimumDurationValue) ? minimumDurationValue : 1.0;
	if (preferences.duration.minimum > 1) preferences.duration.minimum = 0.25;
	if (my duration.minimum > preferences.duration.minimum) my duration.minimum = preferences.duration.minimum;
	double maximumDurationValue = ana -> duration ? RealTier_getMaximumValue (ana -> duration) : NUMundefined;
	my duration.maximum = NUMdefined (maximumDurationValue) ? maximumDurationValue : 1.0;
	if (preferences.duration.maximum < 1) preferences.duration.maximum = 3.0;
	if (preferences.duration.maximum <= preferences.duration.minimum) preferences.duration.minimum = 0.25, preferences.duration.maximum = 3.0;
	if (my duration.maximum < preferences.duration.maximum) my duration.maximum = preferences.duration.maximum;
	my duration.cursor = 1.0;

	my synthesisMethod = prefs_synthesisMethod;
	if (ana -> sound)
		Matrix_getWindowExtrema (ana -> sound, 0, 0, 0, 0, & my soundmin, & my soundmax);
	if (my soundmin == my soundmax) my soundmin = -1.0, my soundmax = +1.0;
	updateMenus (me);
	return me;
}

/* End of file ManipulationEditor.c */
