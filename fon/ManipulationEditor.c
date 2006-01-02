/* ManipulationEditor.c
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
 * pb 2002/10/06 improved visibility of dragging
 * pb 2003/11/20 PitchTier: Interpolate quadratically...
 * pb 2004/04/13 less flashing
 * pb 2006/01/02 removed bug in Shift Frequencies: wrong option list
 */

#include "ManipulationEditor.h"
#include "Resources.h"
#include "PitchTier_to_PointProcess.h"
#include "Sound_to_PointProcess.h"
#include "Sound_to_Pitch.h"
#include "Pitch_to_PitchTier.h"
#include "Pitch_to_PointProcess.h"
#include "EditorM.h"

/*
 * How to add a synthesis method (in an interruptable order):
 * 1. add an Manipulation_ #define in Manipulation.h;
 * 2. add a synthesize_ routine in Manipulation.c, and a reference to it in Manipulation_to_Sound;
 * 3. add a button in ManipulationEditor.h;
 * 4. add a cb_Synth_ callback.
 * 5. create the button in createMenus and update updateMenus;
 */

#define ManipulationEditor_members FunctionEditor_members \
	PointProcess previousPulses; \
	PitchTier previousPitch; \
	DurationTier previousDuration; \
	double soundmin, soundmax; \
	int synthesisMethod; \
	Widget synthPulsesButton, synthPulsesHumButton; \
	Widget synthPulsesLpcButton; \
	Widget synthPitchButton, synthPitchHumButton; \
	Widget synthPulsesPitchButton, synthPulsesPitchHumButton; \
	Widget synthPsolaNodurButton, synthPsolaButton; \
	Widget synthPitchLpcButton; \
	struct { int units, draggingStrategy; double minimum, minPeriodic, maximum, cursor; } pitchTier; \
	struct { double minimum, maximum, cursor;  } duration; \
	Graphics_Viewport inset;
#define ManipulationEditor_methods FunctionEditor_methods
class_create_opaque (ManipulationEditor, FunctionEditor)

#define SCALING_FIXED  1
#define SCALING_GLOBAL  2
#define SCALING_WINDOW  3

#define DRAG_ALL  1
#define DRAG_HORIZONTAL  2
#define DRAG_VERTICAL  3
#define DRAG_HYBRID  4
#define DRAG_MAX  DRAG_HYBRID

#define UNITS_HERTZ  1
#define UNITS_SEMITONES  2
#define UNITS_MAX  UNITS_SEMITONES
static const char *units_strings [] = { 0, "Hz", "st" };

static struct {
	struct {
		int scaling; double minimum, maximum;
		int units, draggingStrategy;
		struct { double frequencyResolution; int useSemitones; } stylize;
		struct { long numberOfPointsPerParabola; } interpolateQuadratically;
	} pitchTier;
	struct { double minimum, maximum; } duration;
} prefs = {
	{
		SCALING_GLOBAL, 50.0, 300.0,
		UNITS_HERTZ, DRAG_ALL,
		{ 2.0, 1 },
		{ 4 }
	},
	{ 0.25, 3.0 }
};

static int prefs_synthesisMethod = Manipulation_PSOLA;   /* Remembered across editor creations, not across Praat sessions. */

/* BUG: 25 should be fmin */
#define YLIN(freq)  (my pitchTier.units == UNITS_HERTZ ? ((freq) < 25 ? 25 : (freq)) : NUMhertzToSemitones ((freq) < 25 ? 25 : (freq)))
#define YLININV(freq)  (my pitchTier.units == UNITS_HERTZ ? (freq) : NUMsemitonesToHertz (freq))

void ManipulationEditor_prefs (void) {
	Resources_addInt ("ManipulationEditor.pitch.scaling", & prefs.pitchTier.scaling);
	Resources_addDouble ("ManipulationEditor.pitch.minimum", & prefs.pitchTier.minimum);
	Resources_addDouble ("ManipulationEditor.pitch.maximum", & prefs.pitchTier.maximum);
	Resources_addInt ("ManipulationEditor.pitch.units", & prefs.pitchTier.units);
	Resources_addInt ("ManipulationEditor.pitch.draggingStrategy", & prefs.pitchTier.draggingStrategy);
	Resources_addDouble ("ManipulationEditor.pitch.stylize.frequencyResolution", & prefs.pitchTier.stylize.frequencyResolution);
	Resources_addInt ("ManipulationEditor.pitch.stylize.useSemitones", & prefs.pitchTier.stylize.useSemitones);
	Resources_addLong ("ManipulationEditor.pitch.interpolateQuadratically.numberOfPointsPerParabola", & prefs.pitchTier.interpolateQuadratically.numberOfPointsPerParabola);
	Resources_addDouble ("ManipulationEditor.duration.minimum", & prefs.duration.minimum);
	Resources_addDouble ("ManipulationEditor.duration.maximum", & prefs.duration.maximum);
	/*Resources_addInt ("ManipulationEditor.synthesis.method.1", & prefs.synthesis.method);*/
}

static void updateMenus (ManipulationEditor me) {
	XmToggleButtonSetState (my synthPulsesButton, my synthesisMethod == Manipulation_PULSES, False);
	XmToggleButtonSetState (my synthPulsesHumButton, my synthesisMethod == Manipulation_PULSES_HUM, False);
	XmToggleButtonSetState (my synthPulsesLpcButton, my synthesisMethod == Manipulation_PULSES_LPC, False);
	XmToggleButtonSetState (my synthPitchButton, my synthesisMethod == Manipulation_PITCH, False);
	XmToggleButtonSetState (my synthPitchHumButton, my synthesisMethod == Manipulation_PITCH_HUM, False);
	XmToggleButtonSetState (my synthPulsesPitchButton, my synthesisMethod == Manipulation_PULSES_PITCH, False);
	XmToggleButtonSetState (my synthPulsesPitchHumButton, my synthesisMethod == Manipulation_PULSES_PITCH_HUM, False);
	XmToggleButtonSetState (my synthPsolaButton, my synthesisMethod == Manipulation_PSOLA, False);
	XmToggleButtonSetState (my synthPitchLpcButton, my synthesisMethod == Manipulation_PITCH_LPC, False);
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

#define cb_extract_common(cb,obj) \
DIRECT (ManipulationEditor, cb) \
	Manipulation ana = my data; \
	if (! ana -> obj) return 0; \
	if (my publishCallback) { \
		Data publish = Data_copy (ana -> obj); \
		if (! publish) return 0; \
		my publishCallback (me, my publishClosure, publish); \
	} \
END
cb_extract_common (cb_extractOriginalSound, sound)
cb_extract_common (cb_extractPulses, pulses)
cb_extract_common (cb_extractPitchTier, pitch)
cb_extract_common (cb_extractDurationTier, duration)
DIRECT (ManipulationEditor, cb_extractManipulatedSound)
	Manipulation ana = my data;
	if (my publishCallback) {
		Sound publish = Manipulation_to_Sound (ana, my synthesisMethod);
		if (! publish) return 0;
		my publishCallback (me, my publishClosure, publish);
	}
END

/***** EDIT MENU *****/

static void save (I) {
	iam (ManipulationEditor);
	Manipulation ana = my data;
	forget (my previousPulses);
	forget (my previousPitch);
	forget (my previousDuration);
	if (ana -> pulses) my previousPulses = Data_copy (ana -> pulses);
	if (ana -> pitch) my previousPitch = Data_copy (ana -> pitch);
	if (ana -> duration) my previousDuration = Data_copy (ana -> duration);
}

static void restore (I) {
	iam (ManipulationEditor);
	Manipulation ana = my data;
	Any dummy;
	dummy = ana -> pulses; ana -> pulses = my previousPulses; my previousPulses = dummy;
	dummy = ana -> pitch; ana -> pitch = my previousPitch; my previousPitch = dummy;
	dummy = ana -> duration; ana -> duration = my previousDuration; my previousDuration = dummy;
}

/***** PULSES MENU *****/

DIRECT (ManipulationEditor, cb_removePulses)
	Manipulation ana = my data;
	if (! ana -> pulses) return 0;
	Editor_save (me, "Remove pulse(s)");
	if (my startSelection == my endSelection)
		PointProcess_removePointNear (ana -> pulses, my startSelection);
	else
		PointProcess_removePointsBetween (ana -> pulses, my startSelection, my endSelection);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (ManipulationEditor, cb_addPulseAtCursor)
	Manipulation ana = my data;
	if (! ana -> pulses) return 0;
	Editor_save (me, "Add pulse");
	PointProcess_addPoint (ana -> pulses, 0.5 * (my startSelection + my endSelection));
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (ManipulationEditor, cb_addPulseAt, "Add pulse", 0)
	REAL ("Position (s)", "0.0")
	OK
SET_REAL ("Position", 0.5 * (my startSelection + my endSelection))
DO
	Manipulation ana = my data;
	if (! ana -> pulses) return 0;
	Editor_save (me, "Add pulse");
	PointProcess_addPoint (ana -> pulses, GET_REAL ("Position"));
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

/***** PITCH MENU *****/

DIRECT (ManipulationEditor, cb_removePitchPoints)
	Manipulation ana = my data;
	if (! ana -> pitch) return 0;
	Editor_save (me, "Remove pitch point(s)");
	if (my startSelection == my endSelection)
		AnyTier_removePointNear (ana -> pitch, my startSelection);
	else
		AnyTier_removePointsBetween (ana -> pitch, my startSelection, my endSelection);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (ManipulationEditor, cb_addPitchPointAtCursor)
	Manipulation ana = my data;
	if (! ana -> pitch) return 0;
	Editor_save (me, "Add pitch point");
	RealTier_addPoint (ana -> pitch, 0.5 * (my startSelection + my endSelection), YLININV (my pitchTier.cursor));
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (ManipulationEditor, cb_addPitchPointAtSlice)
	Manipulation ana = my data;
	PointProcess pulses = ana -> pulses;
	long ileft, iright, nt;
	double *t, f;
	if (! pulses) return Melder_error ("There are no pulses.");
	if (! ana -> pitch) return 0;
	ileft = PointProcess_getLowIndex (pulses, 0.5 * (my startSelection + my endSelection)), iright = ileft + 1, nt = pulses -> nt;
	t = pulses -> t, f = my pitchTier.cursor;   /* Default. */
	Editor_save (me, "Add pitch point");
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
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (ManipulationEditor, cb_addPitchPointAt, "Add pitch point", 0);
	REAL ("Time (s)", "0.0")
	REAL ("Frequency (Hz or st)", "100.0")
	OK
SET_REAL ("Time", 0.5 * (my startSelection + my endSelection))
SET_REAL ("Frequency", my pitchTier.cursor)
DO
	Manipulation ana = my data;
	if (! ana -> pitch) return 0;
	Editor_save (me, "Add pitch point");
	RealTier_addPoint (ana -> pitch, GET_REAL ("Time"), YLININV (GET_REAL ("Frequency")));
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (ManipulationEditor, cb_stylizePitch, "Stylize pitch", "PitchTier: Stylize...");
	REAL ("Frequency resolution", "2.0")
	RADIO ("Units", 2)
		RADIOBUTTON ("Hz")
		RADIOBUTTON ("Semitones")
	OK
SET_REAL ("Frequency resolution", prefs.pitchTier.stylize.frequencyResolution)   /* Once. */
SET_INTEGER ("Units", prefs.pitchTier.stylize.useSemitones + 1)   /* Once. */
DO
	Manipulation ana = my data;
	if (! ana -> pitch) return 0;
	Editor_save (me, "Stylize pitch");
	PitchTier_stylize (ana -> pitch, prefs.pitchTier.stylize.frequencyResolution = GET_REAL ("Frequency resolution"),
		prefs.pitchTier.stylize.useSemitones = GET_INTEGER ("Units") - 1);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (ManipulationEditor, cb_stylizePitch_2st)
	Manipulation ana = my data;
	if (! ana -> pitch) return 0;
	Editor_save (me, "Stylize pitch");
	PitchTier_stylize (ana -> pitch, 2.0, TRUE);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (ManipulationEditor, cb_interpolateQuadratically, "Interpolate quadratically", 0);
	NATURAL ("Number of points per parabola", "4")
	OK
SET_INTEGER ("Number of points per parabola", prefs.pitchTier.interpolateQuadratically.numberOfPointsPerParabola)   /* Once. */
DO
	Manipulation ana = my data;
	if (! ana -> pitch) return 0;
	Editor_save (me, "Interpolate quadratically");
	RealTier_interpolateQuadratically (ana -> pitch,
		prefs.pitchTier.interpolateQuadratically.numberOfPointsPerParabola = GET_INTEGER ("Number of points per parabola"),
		my pitchTier.units == UNITS_SEMITONES);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (ManipulationEditor, cb_interpolateQuadratically_4pts)
	Manipulation ana = my data;
	if (! ana -> pitch) return 0;
	Editor_save (me, "Interpolate quadratically");
	RealTier_interpolateQuadratically (ana -> pitch, 4, my pitchTier.units == UNITS_SEMITONES);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (ManipulationEditor, cb_shiftPitchFrequencies, "Shift pitch frequencies", 0);
	REAL ("Frequency shift", "-20.0")
	OPTIONMENU ("Unit", 1)
		OPTION ("Hertz")
		OPTION ("mel")
		OPTION ("logHertz")
		OPTION ("semitones")
		OPTION ("ERB")
	OK
DO
	Manipulation ana = my data;
	int unit = GET_INTEGER ("Unit");
	unit =
		unit == 1 ? Pitch_UNIT_HERTZ :
		unit == 2 ? Pitch_UNIT_MEL :
		unit == 3 ? Pitch_UNIT_LOG_HERTZ :
		unit == 4 ? Pitch_UNIT_SEMITONES_1 :
		Pitch_UNIT_ERB;
	if (! ana -> pitch) return 0;
	Editor_save (me, "Shift pitch frequencies");
	PitchTier_shiftFrequencies (ana -> pitch, my startSelection, my endSelection, GET_REAL ("Frequency shift"), unit);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
	iferror return 0;
END

FORM (ManipulationEditor, cb_multiplyPitchFrequencies, "Multiply pitch frequencies", 0);
	POSITIVE ("Factor", "1.2")
	LABEL ("", "The multiplication is always done in Hertz.")
	OK
DO
	Manipulation ana = my data;
	if (! ana -> pitch) return 0;
	Editor_save (me, "Multiply pitch frequencies");
	PitchTier_multiplyFrequencies (ana -> pitch, my startSelection, my endSelection, GET_REAL ("Factor"));
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (ManipulationEditor, cb_setPitchRange, "Set pitch range", 0);
	/* BUG: should include Minimum */
	REAL ("Maximum (Hz or st)", "300.0")
	OK
SET_REAL ("Maximum", my pitchTier.maximum)
DO
	double maximum = GET_REAL ("Maximum");
	if (maximum <= my pitchTier.minPeriodic)
		return Melder_error ("Maximum pitch must be greater than %.4g %s.", my pitchTier.minPeriodic, units_strings [my pitchTier.units]);
	prefs.pitchTier.maximum = my pitchTier.maximum = maximum;
	FunctionEditor_redraw (me);
END

FORM (ManipulationEditor, cb_setPitchUnits, "Set pitch units", 0)
	RADIO ("Pitch units", 1)
		RADIOBUTTON ("Hertz")
		RADIOBUTTON ("Semitones re 100 Hz")
	OK
SET_INTEGER ("Pitch units", my pitchTier.units)
DO
	int newPitchUnits = GET_INTEGER ("Pitch units");
	if (my pitchTier.units == newPitchUnits) return 1;
	prefs.pitchTier.units = my pitchTier.units = newPitchUnits;
	if (my pitchTier.units == UNITS_HERTZ) {
		my pitchTier.minimum = 25.0;
		my pitchTier.minPeriodic = 50.0;
		prefs.pitchTier.maximum = my pitchTier.maximum = NUMsemitonesToHertz (my pitchTier.maximum);
		my pitchTier.cursor = NUMsemitonesToHertz (my pitchTier.cursor);
	} else {
		my pitchTier.minimum = -24.0;
		my pitchTier.minPeriodic = -12.0;
		prefs.pitchTier.maximum = my pitchTier.maximum = NUMhertzToSemitones (my pitchTier.maximum);
		my pitchTier.cursor = NUMhertzToSemitones (my pitchTier.cursor);
	}
	FunctionEditor_redraw (me);
END

/***** DURATION MENU *****/

FORM (ManipulationEditor, cb_setDurationRange, "Set duration range", 0);
	REAL ("Minimum", "0.25")
	REAL ("Maximum", "3.0")
	OK
SET_REAL ("Minimum", my duration.minimum)
SET_REAL ("Maximum", my duration.maximum)
DO
	Manipulation ana = my data;
	double minimum = GET_REAL ("Minimum"), maximum = GET_REAL ("Maximum");
	double minimumValue = ana -> duration ? RealTier_getMinimumValue (ana -> duration) : +1e300;
	double maximumValue = ana -> duration ? RealTier_getMaximumValue (ana -> duration) : -1e300;
	if (minimum > 1) return Melder_error ("Minimum relative duration must not be greater than 1.");
	if (maximum < 1) return Melder_error ("Maximum relative duration must not be less than 1.");
	if (minimum >= maximum) return Melder_error ("Maximum relative duration must be greater than minimum.");
	if (minimum > minimumValue)
		return Melder_error ("Minimum relative duration must not be greater than the minimum value present, "
			"which is %.4g.", minimumValue);
	if (maximum < maximumValue)
		return Melder_error ("Maximum relative duration must not be less than the maximum value present, "
			"which is %.4g.", maximumValue);
	prefs.duration.minimum = my duration.minimum = minimum;
	prefs.duration.maximum = my duration.maximum = maximum;
	FunctionEditor_redraw (me);
END

FORM (ManipulationEditor, cb_setDraggingStrategy, "Set dragging strategy", "ManipulationEditor")
	RADIO ("Dragging strategy", 1)
		RADIOBUTTON ("All")
		RADIOBUTTON ("Only horizontal")
		RADIOBUTTON ("Only vertical")
		RADIOBUTTON ("Single all, multiple only vertical")
	OK
SET_INTEGER ("Dragging strategy", my pitchTier.draggingStrategy)
DO
	prefs.pitchTier.draggingStrategy = my pitchTier.draggingStrategy = GET_INTEGER ("Dragging strategy");
END

DIRECT (ManipulationEditor, cb_removeDurationPoints)
	Manipulation ana = my data;
	if (! ana -> duration) return 0;
	Editor_save (me, "Remove duration point(s)");
	if (my startSelection == my endSelection)
		AnyTier_removePointNear (ana -> duration, 0.5 * (my startSelection + my endSelection));
	else
		AnyTier_removePointsBetween (ana -> duration, my startSelection, my endSelection);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (ManipulationEditor, cb_addDurationPointAtCursor)
	Manipulation ana = my data;
	if (! ana -> duration) return 0;
	Editor_save (me, "Add duration point");
	RealTier_addPoint (ana -> duration, 0.5 * (my startSelection + my endSelection), my duration.cursor);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

FORM (ManipulationEditor, cb_addDurationPointAt, "Add duration point", 0)
	REAL ("Time (s)", "0.0");
	REAL ("Relative duration", "1.0");
	OK
SET_REAL ("Time", 0.5 * (my startSelection + my endSelection))
DO
	Manipulation ana = my data;
	if (! ana -> duration) return 0;
	Editor_save (me, "Add duration point");
	RealTier_addPoint (ana -> duration, GET_REAL ("Time"), GET_REAL ("Relative duration"));
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (ManipulationEditor, cb_newDuration)
	Manipulation ana = my data;
	Editor_save (me, "New duration");
	forget (ana -> duration);
	ana -> duration = DurationTier_create (ana -> xmin, ana -> xmax);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END

DIRECT (ManipulationEditor, cb_forgetDuration)
	Manipulation ana = my data;
	forget (ana -> duration);
	FunctionEditor_redraw (me);
	Editor_broadcastChange (me);
END
	
DIRECT (ManipulationEditor, cb_ManipulationEditorHelp) Melder_help ("ManipulationEditor"); END

DIRECT (ManipulationEditor, cb_ManipulationHelp) Melder_help ("Manipulation"); END

#define cb_Synth_common(cb,meth) \
DIRECT (ManipulationEditor, cb) \
	prefs_synthesisMethod = my synthesisMethod = meth; \
	updateMenus (me); \
END
cb_Synth_common (cb_Synth_Pulses, Manipulation_PULSES)
cb_Synth_common (cb_Synth_Pulses_hum, Manipulation_PULSES_HUM)
cb_Synth_common (cb_Synth_Pulses_Lpc, Manipulation_PULSES_LPC)
cb_Synth_common (cb_Synth_Pitch, Manipulation_PITCH)
cb_Synth_common (cb_Synth_Pitch_hum, Manipulation_PITCH_HUM)
cb_Synth_common (cb_Synth_Pulses_Pitch, Manipulation_PULSES_PITCH)
cb_Synth_common (cb_Synth_Pulses_Pitch_hum, Manipulation_PULSES_PITCH_HUM)
cb_Synth_common (cb_Synth_Psola_nodur, Manipulation_PSOLA_NODUR)
cb_Synth_common (cb_Synth_Psola, Manipulation_PSOLA)
cb_Synth_common (cb_Synth_Pitch_Lpc, Manipulation_PITCH_LPC)

static void createMenus (I) {
	iam (ManipulationEditor);
	inherited (ManipulationEditor) createMenus (me);

	Editor_addCommand (me, "File", "Extract original sound", 0, cb_extractOriginalSound);
	Editor_addCommand (me, "File", "Extract pulses", 0, cb_extractPulses);
	Editor_addCommand (me, "File", "Extract pitch tier", 0, cb_extractPitchTier);
	Editor_addCommand (me, "File", "Extract duration tier", 0, cb_extractDurationTier);
	Editor_addCommand (me, "File", "Publish resynthesis", 0, cb_extractManipulatedSound);
	Editor_addCommand (me, "File", "-- close --", 0, NULL);

	Editor_addMenu (me, "Pulse", 0);
	Editor_addCommand (me, "Pulse", "Add pulse at cursor", 'P', cb_addPulseAtCursor);
	Editor_addCommand (me, "Pulse", "Add pulse at...", 0, cb_addPulseAt);
	Editor_addCommand (me, "Pulse", "-- remove pulses --", 0, NULL);
	Editor_addCommand (me, "Pulse", "Remove pulse(s)", motif_OPTION + 'P', cb_removePulses);

	Editor_addMenu (me, "Pitch", 0);
	Editor_addCommand (me, "Pitch", "Add pitch point at cursor", 'T', cb_addPitchPointAtCursor);
	Editor_addCommand (me, "Pitch", "Add pitch point at time slice", 0, cb_addPitchPointAtSlice);
	Editor_addCommand (me, "Pitch", "Add pitch point at...", 0, cb_addPitchPointAt);
	Editor_addCommand (me, "Pitch", "-- remove pitch --", 0, NULL);
	Editor_addCommand (me, "Pitch", "Remove pitch point(s)", motif_OPTION + 'T', cb_removePitchPoints);
	Editor_addCommand (me, "Pitch", "-- pitch prefs --", 0, NULL);
	Editor_addCommand (me, "Pitch", "Set pitch range...", 0, cb_setPitchRange);
	Editor_addCommand (me, "Pitch", "Set pitch units...", 0, cb_setPitchUnits);
	Editor_addCommand (me, "Pitch", "Set pitch dragging strategy...", 0, cb_setDraggingStrategy);
	Editor_addCommand (me, "Pitch", "-- modify pitch --", 0, NULL);
	Editor_addCommand (me, "Pitch", "Shift pitch frequencies...", 0, cb_shiftPitchFrequencies);
	Editor_addCommand (me, "Pitch", "Multiply pitch frequencies...", 0, cb_multiplyPitchFrequencies);
	Editor_addCommand (me, "Pitch", "All:", motif_INSENSITIVE, cb_stylizePitch);
	Editor_addCommand (me, "Pitch", "Stylize pitch...", 0, cb_stylizePitch);
	Editor_addCommand (me, "Pitch", "Stylize pitch (2 st)", '2', cb_stylizePitch_2st);
	Editor_addCommand (me, "Pitch", "Interpolate quadratically...", 0, cb_interpolateQuadratically);
	Editor_addCommand (me, "Pitch", "Interpolate quadratically (4 pts)", '4', cb_interpolateQuadratically_4pts);

	Editor_addMenu (me, "Dur", 0);
	Editor_addCommand (me, "Dur", "Add duration point at cursor", 'D', cb_addDurationPointAtCursor);
	Editor_addCommand (me, "Dur", "Add duration point at...", 0, cb_addDurationPointAt);
	Editor_addCommand (me, "Dur", "-- remove duration --", 0, NULL);
	Editor_addCommand (me, "Dur", "Remove duration point(s)", motif_OPTION + 'D', cb_removeDurationPoints);
	Editor_addCommand (me, "Dur", "-- duration prefs --", 0, NULL);
	Editor_addCommand (me, "Dur", "Set duration range...", 0, cb_setDurationRange);
	Editor_addCommand (me, "Dur", "-- refresh duration --", 0, NULL);
	Editor_addCommand (me, "Dur", "New duration", 0, cb_newDuration);
	Editor_addCommand (me, "Dur", "Forget duration", 0, cb_forgetDuration);

	Editor_addMenu (me, "Synth", 0);
	my synthPulsesButton = Editor_addCommand (me, "Synth", "Pulses --", motif_CHECKABLE, cb_Synth_Pulses);
	my synthPulsesHumButton = Editor_addCommand (me, "Synth", "Pulses (hum) --", motif_CHECKABLE, cb_Synth_Pulses_hum);

	my synthPulsesLpcButton = Editor_addCommand (me, "Synth", "Pulses & LPC -- (\"LPC resynthesis\")", motif_CHECKABLE, cb_Synth_Pulses_Lpc);
	Editor_addCommand (me, "Synth", "-- pitch resynth --", 0, NULL);
	my synthPitchButton = Editor_addCommand (me, "Synth", " -- Pitch", motif_CHECKABLE, cb_Synth_Pitch);
	my synthPitchHumButton = Editor_addCommand (me, "Synth", " -- Pitch (hum)", motif_CHECKABLE, cb_Synth_Pitch_hum);
	my synthPulsesPitchButton = Editor_addCommand (me, "Synth", "Pulses -- Pitch", motif_CHECKABLE, cb_Synth_Pulses_Pitch);
	my synthPulsesPitchHumButton = Editor_addCommand (me, "Synth", "Pulses -- Pitch (hum)", motif_CHECKABLE, cb_Synth_Pulses_Pitch_hum);
	Editor_addCommand (me, "Synth", "-- full resynth --", 0, NULL);
	my synthPsolaButton = Editor_addCommand (me, "Synth", "Sound & Pulses -- Pitch & Duration  (\"PSOLA manipulation\")", motif_CHECKED, cb_Synth_Psola);
	my synthPitchLpcButton = Editor_addCommand (me, "Synth", "LPC -- Pitch  (\"LPC pitch manipulation\")", motif_CHECKABLE, cb_Synth_Pitch_Lpc);

	Editor_addCommand (me, "Help", "ManipulationEditor help", '?', cb_ManipulationEditorHelp);
	Editor_addCommand (me, "Help", "Manipulation help", 0, cb_ManipulationHelp);
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
	Graphics_setFont (my graphics, Graphics_TIMES);
	Graphics_text (my graphics, 1, 1, "%%Sound");
	Graphics_setColour (my graphics, Graphics_BLUE);
	Graphics_text (my graphics, 1, 1 - Graphics_dyMMtoWC (my graphics, 3), "%%Pulses");
	Graphics_setFont (my graphics, Graphics_HELVETICA);

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
		FunctionEditor_drawRangeMark (me, "%.4g", scaleMin, Graphics_BOTTOM);
		FunctionEditor_drawRangeMark (me, "%.4g", scaleMax, Graphics_TOP);

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
	char *rangeFormats [] = { "", "%.1f Hz", "%.2f st" };

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
	Graphics_setFont (my graphics, Graphics_TIMES);
	Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_TOP);
	Graphics_text (my graphics, 1, 1, "%%Pitch manip");
	Graphics_setGrey (my graphics, 0.7);
	Graphics_text (my graphics, 1, 1 - Graphics_dyMMtoWC (my graphics, 3), "%%Pitch from pulses");
	Graphics_setFont (my graphics, Graphics_HELVETICA);

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

	FunctionEditor_drawGridLine (me, minimumFrequency);
	FunctionEditor_drawRangeMark (me, rangeFormats [my pitchTier.units], my pitchTier.maximum, Graphics_TOP);
	FunctionEditor_drawRangeMark (me, rangeFormats [my pitchTier.units], my pitchTier.minimum, Graphics_BOTTOM);
	if (my startSelection == my endSelection && my pitchTier.cursor >= my pitchTier.minimum && my pitchTier.cursor <= my pitchTier.maximum)
		FunctionEditor_drawHorizontalHair (me, rangeFormats [my pitchTier.units], my pitchTier.cursor);
	if (cursorVisible && n > 0)
		FunctionEditor_insertCursorFunctionValue (me, rangeFormats [my pitchTier.units],
			YLIN (RealTier_getValueAtTime (pitch, my startSelection)), my pitchTier.minimum, my pitchTier.maximum);

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
		Graphics_text (my graphics, 0.5 * (my startWindow + my endWindow), 0.5 * (my pitchTier.minimum + my pitchTier.maximum), "(no pitch points)");
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
	Graphics_setFont (my graphics, Graphics_TIMES);
	Graphics_setTextAlignment (my graphics, Graphics_RIGHT, Graphics_TOP);
	Graphics_text (my graphics, 1, 1, "%%Duration manip");
	Graphics_setFont (my graphics, Graphics_HELVETICA);

	Graphics_setWindow (my graphics, my startWindow, my endWindow, my duration.minimum, my duration.maximum);
	FunctionEditor_drawGridLine (me, 1.0);
	FunctionEditor_drawRangeMark (me, "%.3f", my duration.maximum, Graphics_TOP);
	FunctionEditor_drawRangeMark (me, "%.3f", my duration.minimum, Graphics_BOTTOM);
	if (my startSelection == my endSelection && my duration.cursor >= my duration.minimum && my duration.cursor <= my duration.maximum)
		FunctionEditor_drawHorizontalHair (me, "%.3f", my duration.cursor);
	if (cursorVisible && n > 0)
		FunctionEditor_insertCursorFunctionValue (me, "%.3f",
			RealTier_getValueAtTime (duration, my startSelection), my duration.minimum, my duration.maximum);

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
			0.5 * (my duration.minimum + my duration.maximum), "(no duration points)");
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

static void draw (I) {
	iam (ManipulationEditor);
	double ysoundmin, ysoundmax;
	double ypitchmin, ypitchmax, ydurationmin, ydurationmax;
	int hasSoundArea = getSoundArea (me, & ysoundmin, & ysoundmax);
	int hasPitchArea = getPitchArea (me, & ypitchmin, & ypitchmax);
	int hasDurationArea = getDurationArea (me, & ydurationmin, & ydurationmax);

	if (hasSoundArea) drawSoundArea (me, ysoundmin, ysoundmax);
	if (hasPitchArea) drawPitchArea (me, ypitchmin, ypitchmax);
	if (hasDurationArea) drawDurationArea (me, ydurationmin, ydurationmax);

	Graphics_setWindow (my graphics, 0.0, 1.0, 0.0, 1.0);
	Graphics_setGrey (my graphics, 0.75);
	Graphics_fillRectangle (my graphics, -0.001, 1.001, ypitchmax, ysoundmin);
	Graphics_setGrey (my graphics, 0.00);
	Graphics_line (my graphics, 0, ysoundmin, 1, ysoundmin);
	Graphics_line (my graphics, 0, ypitchmax, 1, ypitchmax);
	if (hasDurationArea) {
		Graphics_setGrey (my graphics, 0.75);
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
		Graphics_setTextAlignment (my graphics, Graphics_CENTER, Graphics_TOP);
		Graphics_printf (my graphics, t, my pitchTier.maximum, "%f", t);
		Graphics_line (my graphics, my startWindow, fWC, my endWindow, fWC);
		Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_BOTTOM);
		Graphics_printf (my graphics, my startWindow, fWC, "%.5f", fWC);
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
		return inherited (ManipulationEditor) click (me, xWC, yWC, shiftKeyPressed);
	}
	Graphics_setWindow (my graphics, my startWindow, my endWindow, my pitchTier.minimum, my pitchTier.maximum);
	yWC = my pitchTier.cursor;

	/*
	 * Clicked on a pitch point?
	 */
	inearestPoint = AnyTier_timeToNearestIndex (pitch, xWC);
	if (inearestPoint == 0) {
		Graphics_resetViewport (my graphics, my inset);
		return inherited (ManipulationEditor) click (me, xWC, yWC, shiftKeyPressed);
	}
	nearestPoint = pitch -> points -> item [inearestPoint];
	if (Graphics_distanceWCtoMM (my graphics, xWC, yWC, nearestPoint -> time, YLIN (nearestPoint -> value)) > 1.5) {
		Graphics_resetViewport (my graphics, my inset);
		return inherited (ManipulationEditor) click (me, xWC, yWC, shiftKeyPressed);
	}

	/*
	 * Clicked on a selected pitch point?
	 */
	draggingSelection = shiftKeyPressed &&
		nearestPoint -> time > my startSelection && nearestPoint -> time < my endSelection;
	if (draggingSelection) {
		ifirstSelected = AnyTier_timeToHighIndex (pitch, my startSelection);
		ilastSelected = AnyTier_timeToLowIndex (pitch, my endSelection);
		Editor_save (me, "Drag pitch points");
	} else {
		ifirstSelected = ilastSelected = inearestPoint;
		Editor_save (me, "Drag pitch point");
	}

	/*
	 * Drag.
	 */
	 /*
	  * Draw at the old location once.
	  * Since some systems do double buffering,
	  * the undrawing at the old position and redrawing at the new have to be bracketed by Graphics_mouseStillDown ().
	  */
	Graphics_xorOn (my graphics, Graphics_MAGENTA);
	drawWhileDragging (me, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
	dragHorizontal = my pitchTier.draggingStrategy != DRAG_VERTICAL &&
		(! shiftKeyPressed || my pitchTier.draggingStrategy != DRAG_HYBRID);
	dragVertical = my pitchTier.draggingStrategy != DRAG_HORIZONTAL;
	while (Graphics_mouseStillDown (my graphics)) {
		double xWC_new, yWC_new;
		drawWhileDragging (me, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
		Graphics_getMouseLocation (my graphics, & xWC_new, & yWC_new);
		if (dragHorizontal) dt += xWC_new - xWC, xWC = xWC_new;
		if (dragVertical) df += yWC_new - yWC, yWC = yWC_new;
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

	Editor_broadcastChange (me);
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
		Graphics_setTextAlignment (my graphics, Graphics_CENTER, Graphics_TOP);
		Graphics_printf (my graphics, t, my duration.maximum, "%f", t);
		Graphics_line (my graphics, my startWindow, durWC, my endWindow, durWC);
		Graphics_setTextAlignment (my graphics, Graphics_LEFT, Graphics_BOTTOM);
		Graphics_printf (my graphics, my startWindow, durWC, "%.2f", durWC);
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
		return inherited (ManipulationEditor) click (me, xWC, yWC, shiftKeyPressed);
	}
	Graphics_setWindow (my graphics, my startWindow, my endWindow, my duration.minimum, my duration.maximum);

	/*
	 * Clicked on a duration point?
	 */
	inearestPoint = AnyTier_timeToNearestIndex (duration, xWC);
	if (inearestPoint == 0) {
		Graphics_resetViewport (my graphics, my inset);
		return inherited (ManipulationEditor) click (me, xWC, yWC, shiftKeyPressed);
	}
	nearestPoint = duration -> points -> item [inearestPoint];
	if (Graphics_distanceWCtoMM (my graphics, xWC, yWC, nearestPoint -> time, nearestPoint -> value) > 1.5) {
		Graphics_resetViewport (my graphics, my inset);
		return inherited (ManipulationEditor) click (me, xWC, yWC, shiftKeyPressed);
	}

	/*
	 * Clicked on a selected duration point?
	 */
	draggingSelection = shiftKeyPressed &&
		nearestPoint -> time > my startSelection && nearestPoint -> time < my endSelection;
	if (draggingSelection) {
		ifirstSelected = AnyTier_timeToHighIndex (duration, my startSelection);
		ilastSelected = AnyTier_timeToLowIndex (duration, my endSelection);
		Editor_save (me, "Drag duration points");
	} else {
		ifirstSelected = ilastSelected = inearestPoint;
		Editor_save (me, "Drag duration point");
	}

	/*
	 * Drag.
	 */
	Graphics_xorOn (my graphics, Graphics_MAGENTA);
	drawDurationWhileDragging (me, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
	while (Graphics_mouseStillDown (my graphics)) {
		double xWC_new, yWC_new;
		drawDurationWhileDragging (me, xWC, yWC, ifirstSelected, ilastSelected, dt, df);
		Graphics_getMouseLocation (my graphics, & xWC_new, & yWC_new);
		dt += xWC_new - xWC, xWC = xWC_new;
		df += yWC_new - yWC, yWC = yWC_new;
		drawDurationWhileDragging (me, xWC_new, yWC_new, ifirstSelected, ilastSelected, dt, df);
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

	Editor_broadcastChange (me);
	return 1;   /* Update needed. */
}

static int click (I, double xWC, double yWC, int shiftKeyPressed) {
	iam (ManipulationEditor);
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
	return inherited (ManipulationEditor) click (me, xWC, yWC, shiftKeyPressed);
}

static void play (I, double tmin, double tmax) {
	iam (ManipulationEditor);
	Manipulation ana = my data;
	if (my shiftKeyPressed) {
		if (ana -> sound) Sound_playPart (ana -> sound, tmin, tmax, our playCallback, me);
	} else {
		if (! Manipulation_playPart (ana, tmin, tmax, my synthesisMethod))
			Melder_flushError (NULL);
	}
}

class_methods (ManipulationEditor, FunctionEditor)
	class_method (destroy)
	class_method (createMenus)
	class_method (save)
	class_method (restore)
	class_method (draw)
	class_method (click)
	class_method (play)
class_methods_end

ManipulationEditor ManipulationEditor_create (Widget parent, const char *title, Manipulation ana) {
	ManipulationEditor me = new (ManipulationEditor);
	if (! me || ! FunctionEditor_init (me, parent, title, ana)) return NULL;

	if (prefs.pitchTier.draggingStrategy < 1 || prefs.pitchTier.draggingStrategy > DRAG_MAX) prefs.pitchTier.draggingStrategy = 1;
	if (prefs.pitchTier.units < 1 || prefs.pitchTier.units > UNITS_MAX) prefs.pitchTier.units = 1;
	my pitchTier.draggingStrategy = prefs.pitchTier.draggingStrategy;
	my pitchTier.units = prefs.pitchTier.units;
	if (my pitchTier.units == UNITS_HERTZ) {
		my pitchTier.minimum = 25.0;
		my pitchTier.minPeriodic = 50.0;
		my pitchTier.maximum = RealTier_getMaximumValue (ana -> pitch);
		my pitchTier.cursor = my pitchTier.maximum * 0.8;
		my pitchTier.maximum *= 1.2;
	} else {
		my pitchTier.minimum = -24.0;
		my pitchTier.minPeriodic = -12.0;
		my pitchTier.maximum = NUMhertzToSemitones (RealTier_getMaximumValue (ana -> pitch));
		my pitchTier.cursor = my pitchTier.maximum - 4.0;
		my pitchTier.maximum += 3.0;
	}
	if (my pitchTier.maximum < prefs.pitchTier.maximum) my pitchTier.maximum = prefs.pitchTier.maximum;

	my duration.minimum = ana -> duration ? RealTier_getMinimumValue (ana -> duration) : 1.0;
	if (prefs.duration.minimum > 1) prefs.duration.minimum = 0.25;
	if (my duration.minimum > prefs.duration.minimum) my duration.minimum = prefs.duration.minimum;
	my duration.maximum = ana -> duration ? RealTier_getMaximumValue (ana -> duration) : 1.0;
	if (prefs.duration.maximum < 1) prefs.duration.maximum = 3.0;
	if (prefs.duration.maximum <= prefs.duration.minimum) prefs.duration.minimum = 0.25, prefs.duration.maximum = 3.0;
	if (my duration.maximum < prefs.duration.maximum) my duration.maximum = prefs.duration.maximum;
	my duration.cursor = 1.0;

	my synthesisMethod = prefs_synthesisMethod;
	if (ana -> sound)
		Matrix_getWindowExtrema (ana -> sound, 0, 0, 0, 0, & my soundmin, & my soundmax);
	if (my soundmin == my soundmax) my soundmin = -1.0, my soundmax = +1.0;
	updateMenus (me);
	return me;
}

/* End of file ManipulationEditor.c */
